#include "stdafx.h"
#include "ServerIocp.h"
#include "UserManager.h"
#include "User.h"

unsigned __stdcall KeepAliveThreadCallback(void *object)
{
	ServerIocp *owner = (ServerIocp*)object;
	owner->KeepAliveThreadCallback();

	return 0;
}

ServerIocp::ServerIocp()
{
}

ServerIocp::~ServerIocp()
{
	if (keep_alive_handle_)
	{
		SetEvent(keep_alive_handle_destroy_event_);

		WaitForSingleObject(keep_alive_handle_, INFINITE);
		CloseHandle(keep_alive_handle_);
		keep_alive_handle_ = NULL;
	}

	if (keep_alive_handle_destroy_event_)
	{
		CloseHandle(keep_alive_handle_destroy_event_);
		keep_alive_handle_destroy_event_ = NULL;
	}

	SESSION_MANAGER.Finalize();

	if (listen_)
		SAFE_DELETE(listen_);
}

void ServerIocp::KeepAliveThreadCallback()
{
	while (true)
	{
		DWORD dwResult = WaitForSingleObject(keep_alive_handle_destroy_event_, 30000);

		if (dwResult == WAIT_OBJECT_0) 
			return;

		Msg* broad_cast = new Msg(PT_KEEP_ALIVE);
		SESSION_MANAGER.BroadCast(broad_cast);
		SAFE_DELETE(broad_cast);

		Log::ReportLog(L"KEEP ALIVE SESSION CHECK");
	}
}

void ServerIocp::OnIoConnected(void *obejct)
{
	Session *session = reinterpret_cast<Session*>(obejct);
	
	if (Iocp::RegistSocket(session->GetSocket(), reinterpret_cast<ULONGLONG>(session)) == false)
		return;

	session->ConnectProc(listen_->GetSocket());

	Log::ReportLog(L"Session Connect : [%lld]", reinterpret_cast<ULONGLONG>(session));
}

void ServerIocp::OnIoDisconnected(void *obejct)
{
	Session *session = reinterpret_cast<Session*>(obejct);

	session->DisConnectProc(listen_->GetSocket());
	
	Log::ReportLog(L"Session DisConnect : [%lld]", reinterpret_cast<ULONGLONG>(session));
}

void ServerIocp::OnIoRead(void *obejct, DWORD data_length)
{
	Session *session = reinterpret_cast<Session*>(obejct);

	if (session->RecvbyIocp(data_length))
	{
		Msg* msg = nullptr;
		session->GetMsg(msg);

		if (msg == nullptr)
			return;

		Log::ReportLog(L"Read Msg Session:[%lld], Protocol:[%d]", reinterpret_cast<ULONGLONG>(session), msg->GetID());

		session->MsgHandle(msg);
	}

	if (session->ResetRecvState() == false)
		session->Restart(listen_->GetSocket());
}

void ServerIocp::OnIoWrote(void *pObject, DWORD dwDataLength)
{

}

bool ServerIocp::Initialize()
{
	IniReader ini_reader_;
	DWORD buffer_length;
	if (ini_reader_.Initialize(std::wstring(L"./ServerConfig.ini")) == false)
		return false;

	std::wstring ip;
	ini_reader_.LoadConfigData(L"SERVER", L"IP", ip, &buffer_length);
	Log::ReportLog(L"IP : %s", ip.c_str());

	if (ip.empty())
		return false;

	DWORD port = 0;
	ini_reader_.LoadConfigData(L"SERVER", L"PORT", &port);
	Log::ReportLog(L"PORT : %d", port);

	if (port == 0)
		return false;

	DWORD max_user = 0;
	ini_reader_.LoadConfigData(L"SERVER", L"MAX_USER", &max_user);
	Log::ReportLog(L"MAX_USER : %d", max_user);

	if (max_user == 0)
		return false;

	listen_ = new Network();
	if (listen_->Initialize(static_cast<WORD>(port), max_user) == false)
		return false;

	if (Iocp::Initialize(listen_->GetSocket(), reinterpret_cast<ULONGLONG>(listen_)) == false)
		return false;

	if (!SESSION_MANAGER.Initialize(listen_->GetSocket(), max_user))
		return false;

	keep_alive_handle_destroy_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!keep_alive_handle_destroy_event_)
		return false; 

	keep_alive_handle_ = (HANDLE)::_beginthreadex(NULL, 0, ::KeepAliveThreadCallback, this, 0, NULL);
	if (!keep_alive_handle_)
		return false; 

	return true;
}