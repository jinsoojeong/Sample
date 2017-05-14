#include "stdafx.h"
#include "SessionManager.h"

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Initialize(SOCKET listen_socket, DWORD max_user)
{
	TS_CRETICAL_SECSION sc;

	IniReader ini_reader_;
	DWORD buffer_length;
	if (ini_reader_.Initialize(std::wstring(L"./ServerConfig.ini")) == false)
		return false;

	ini_reader_.LoadConfigData(L"SERVER", L"VERSION", module_version_, &buffer_length);

	if (module_version_.empty())
		return false;

	Log::ReportLog(L"VERSION : %s", module_version_.c_str());

	if (!listen_socket)
		return false;

	for (DWORD i = 0;i<max_user;i++)
	{
		Session *connect_session = new Session();
		sessions_.push_back(connect_session);
	}

	for each (Sessions::value_type session in sessions_)
	{
		if (!session->Initialize())
			return false;

		if (session->Accept(listen_socket) == false)
			return false;
	}

	return true;
}

void SessionManager::Finalize()
{
	TS_CRETICAL_SECSION sc;

	for each (Sessions::value_type session in sessions_)
		SAFE_DELETE(session);

	sessions_.clear();
}

void SessionManager::BroadCast(Msg* msg, Session* except_session)
{
	TS_CRETICAL_SECSION sc;

	for each (Sessions::value_type session in sessions_)
	{
		if (session->GetIsConnected() == false)
			continue;

		if (session == except_session)
			continue;

		session->SendMsg(msg);
	}
}