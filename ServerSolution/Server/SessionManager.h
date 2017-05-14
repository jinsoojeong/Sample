#pragma once

class SessionManager : public ThreadSync<SessionManager>, public Singleton<SessionManager>
{
public:
	SessionManager();
	virtual ~SessionManager();

	bool Initialize(SOCKET listen_socket, DWORD max_user);
	void Finalize();

	void BroadCast(Msg* msg, Session* except_session = nullptr);
	std::wstring GetModuleVersion() { return module_version_; }

private:
	typedef std::vector<Session*> Sessions;
	Sessions sessions_;

	std::wstring module_version_;
};

#define SESSION_MANAGER SessionManager::GetInstance()