#include "stdafx.h"
#include "ThreadSync.h"
#include "IniReader.h"
#include "SockSystem.h"
#include "PacketSystem.h"
#include "Session.h"
#include "SessionManager.h"
#include "Msg.h"

SessionManager::SessionManager() : session_id_(0), listen_socket_(NULL), port_(0)
{
	
}

SessionManager::~SessionManager()
{
	Finalize();
}

void SessionManager::Update(ULONGLONG current_tick)
{
	for each (const Sessions::value_type& itor in worker_sessions_)
		(*itor).Update(current_tick);
}

bool SessionManager::Initialize(SOCKET listen_socket)
{
	SCOPED_SINGLE_LOCK(&cs_);

	if (listen_socket == NULL)
		return false;

	IniReader ini_reader;
	if (ini_reader.Initialize(std::wstring(L"./ServerConfig.ini")) == false)
		return false;

	ini_reader.LoadConfigData(L"SERVER", L"INIT_SESSION_COUNT", &init_session_count_);
	Log::ReportLog(L"INIT_SESSION_COUNT : %d", init_session_count_);

	if (init_session_count_ == 0)
		return false;

	ini_reader.LoadConfigData(L"SERVER", L"MAX_SESSION_COUNT", &max_session_count_);
	Log::ReportLog(L"MAX_SESSION_COUNT : %d", max_session_count_);

	if (max_session_count_ == 0)
		return false;

	listen_socket_ = listen_socket;

	for (DWORD i = 0 ; i < init_session_count_; i++)
	{
		InterlockedIncrement(&session_id_);

		Session *session = new Session(session_id_);
		if (session->Initialize(listen_socket_) == false)
			return false;

		accept_sessions_.push_back(session);
	}

	return true;
}

Session* SessionManager::GetAcceptSession(DWORD id)
{
	SCOPED_SINGLE_LOCK(&cs_);

	Session *session = nullptr;

	for each (const Sessions::value_type& itor in accept_sessions_)
	{
		if ((*itor).GetID() == id)
			session = itor;
	}

	return session;
}

Session* SessionManager::GetSession(DWORD id)
{
	SCOPED_SINGLE_LOCK(&cs_);

	Session *session = nullptr;

	for each (const Sessions::value_type& itor in worker_sessions_)
	{
		if ((*itor).GetID() == id)
			session = itor;
	}

	return session;
}

void SessionManager::BroadCast(Msg *msg, Session *except_session)
{
	SCOPED_SINGLE_LOCK(&cs_);

	for each (const Sessions::value_type& session in worker_sessions_)
	{
		if (session->GetIsConnected() == false)
			continue;

		if (session == except_session)
			continue;

		session->SendMsg(msg, true);
	}

	SAFE_DELETE(msg);
}

bool SessionManager::ConnectSession(Session *session)
{
	SCOPED_SINGLE_LOCK(&cs_);

	if (session == nullptr)
		return false;

	if (session->RecvWait(listen_socket_) == false)
		return false;
	
	session->SetIsConnected(true);

	accept_sessions_.remove(session);
	worker_sessions_.push_back(session);

	return true;
}

void SessionManager::DisConnectSession(Session *session)
{
	SCOPED_SINGLE_LOCK(&cs_);

	if (session)
	{
		session->ReAcceptWait(listen_socket_);
		session->SetIsConnected(false);

		worker_sessions_.remove(session);
		accept_sessions_.push_back(session);
	}
}

void SessionManager::AcceptPoolCheck()
{
	SCOPED_SINGLE_LOCK(&cs_);

	if (accept_sessions_.empty())
	{
		DWORD add_accept_session_count = 10;

		// 0이면 무제한 세션을 증가 시킨다.
		if (max_session_count_ != 0)
		{
			if (max_session_count_ < (session_id_ + add_accept_session_count))
				add_accept_session_count = (max_session_count_ - session_id_);
		}

		for (DWORD i = 0; i < add_accept_session_count; i++)
		{
			InterlockedIncrement(&session_id_);

			Session *session = new Session(session_id_);
			if (session->Initialize(listen_socket_) == false)
				return;

			accept_sessions_.push_back(session);
		}
	}
}

void SessionManager::Finalize()
{
	SCOPED_SINGLE_LOCK(&cs_);

	for (Sessions::iterator itor = accept_sessions_.begin(); itor != accept_sessions_.end(); itor++)
		SAFE_DELETE((*itor));

	accept_sessions_.clear();

	for (Sessions::iterator itor = worker_sessions_.begin(); itor != worker_sessions_.end(); itor++)
		SAFE_DELETE((*itor));

	worker_sessions_.clear();

	IniReader ini_reader;
	ini_reader.SetConfigData(L"SERVER", L"INIT_SESSION_COUNT", session_id_);
}