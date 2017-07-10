#pragma once
#include "Singleton.h"

class Msg;
class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager();
	virtual ~SessionManager();

	void Update(ULONGLONG current_tick);
	bool Initialize(SOCKET listen_socket);
	Session* GetAcceptSession(DWORD session_id);
	Session* GetSession(DWORD session_id);
	void BroadCast(Msg* msg, Session* except_session = nullptr);
	
	bool ConnectSession(Session *session);
	void DisConnectSession(Session *session);

	void AcceptPoolCheck();

private:
	typedef std::list<Session*> Sessions;

	void Finalize();

	DWORD session_id_;
	DWORD init_session_count_;
	DWORD max_session_count_;
	WORD port_;
	
	SOCKET listen_socket_;
	CriticalSection cs_;

	Sessions accept_sessions_;
	Sessions worker_sessions_;
};

#define SESSION_MANAGER SessionManager::GetInstance()
#define DELETE_SESSION_MANAGER SessionManager::Destroy()