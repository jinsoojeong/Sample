#pragma once

class User;
class UserManager : public ThreadSync<SessionManager>, public Singleton<UserManager>
{
public:
	UserManager();
	~UserManager();
	
	bool UserCertify(const std::wstring& id, const std::wstring& pw, DWORD& user_id);
	bool RegistUser(const DWORD id, const std::wstring& nick);
	bool UnRegistUser(const DWORD id);
	User* FindUser(DWORD user_id);
	void BuildMsg(Msg* msg, User *user);

private:
	typedef std::map<std::wstring, DWORD> CertUsers;	
	typedef std::map<DWORD, User*> Users;
	
	CertUsers cert_users_;
	Users users_;

	DWORD identity_id_;
};

#define USER_MANAGER UserManager::GetInstance()