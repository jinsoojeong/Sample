#pragma once

class User;
class UserManager : public Singleton<UserManager>
{
public:
	UserManager();
	~UserManager();
	
	void Update(ULONGLONG current_tick);
	bool UserCertify(const std::wstring& id, const std::wstring& pw, DWORD& user_id);
	bool RegistUser(const DWORD session_id, const DWORD id, const std::wstring& nick);
	bool UnRegistUser(const DWORD id);
	User* FindUser(DWORD user_id);
	User* FindUser(DWORD session_id, bool);
	void BuildMsg(Msg* msg, User *user);

	void BroadCast(Msg *msg, const DWORD except_user);

private:
	typedef DWORD USER_ID;
	typedef DWORD SESSION_ID;
	typedef std::map<std::wstring, DWORD> CertUsers;	
	typedef std::map<SESSION_ID, USER_ID> SessionUserIDs;
	typedef std::map<USER_ID, User*> Users;

	void Finalize();
	
	CertUsers cert_users_;
	SessionUserIDs session_user_ids_;
	Users users_;
	DWORD identity_id_;
};

#define USER_MANAGER UserManager::GetInstance()
#define DELETE_USER_MANAGER UserManager::Destroy()