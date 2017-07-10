#include "stdafx.h"
#include "ChatServer.h"
#include "MainProcess.h"

ChatServer::ChatServer() : default_user_count_(0)
{
}

ChatServer::~ChatServer()
{
	Finalize();
}

void ChatServer::OnIoConnected(void *obejct)
{
	Session *session = reinterpret_cast<Session*>(obejct);
	
	if (Iocp::RegistSocket(session->GetSocket(), reinterpret_cast<ULONGLONG>(session)) == false)
		return;

	Msg *msg = new Msg(PT_SESSION_CONNECT);
	msg->SetSessionID(session->GetID());

	MAIN_PROCESS.PushMsgQueue(msg);
}

void ChatServer::OnIoDisconnected(void *obejct)
{
	Session *session = reinterpret_cast<Session*>(obejct);
	
	Msg *msg = new Msg(PT_SESSION_DISCONNECT);
	msg->SetSessionID(session->GetID());

	MAIN_PROCESS.PushMsgQueue(msg);
}

void ChatServer::OnIoRead(void *obejct, DWORD data_length)
{
	Session *session = reinterpret_cast<Session*>(obejct);

	if (session->RecvByIocp(data_length))
	{
		Msg* msg = nullptr;
		session->GetMsg(msg);

		if (msg == nullptr)
			return;

		Log::ReportLog(L"Read Msg Session:[%lld], Protocol:[%d]", reinterpret_cast<ULONGLONG>(session), msg->GetID());
		
		msg->SetSessionID(session->GetID());
		MAIN_PROCESS.PushMsgQueue(msg);
	}

	if (session->RecvWait(listener_->GetSocket()) == false)
		Log::ReportLog(L"Session Return RecvWait Failed : [%lld]", reinterpret_cast<ULONGLONG>(session));
}

bool ChatServer::Init()
{
	if (ini_reader_.Initialize(std::wstring(L"./ServerConfig.ini")) == false)
		return false;

	std::wstring ip;
	DWORD buffer_length;
	ini_reader_.LoadConfigData(L"SERVER", L"IP", ip, &buffer_length);
	Log::ReportLog(L"IP : %s", ip.c_str());

	if (ip.empty())
		return false;

	DWORD port = 0;
	ini_reader_.LoadConfigData(L"SERVER", L"PORT", &port);
	Log::ReportLog(L"PORT : %d", port);

	if (port == 0)
		return false;

	listener_ = new PacketSystem();
	if (listener_->Initialize(this, static_cast<WORD>(port)) == false)
		return false;

	if (Iocp::Initialize(listener_->GetSocket(), reinterpret_cast<ULONGLONG>(listener_)) == false)
		return false;

	if (!SESSION_MANAGER.Initialize(listener_->GetSocket()))
		return false;

	return true;
}

void ChatServer::Finalize()
{
	DELETE_SESSION_MANAGER;

	if (listener_)
		SAFE_DELETE(listener_);
}