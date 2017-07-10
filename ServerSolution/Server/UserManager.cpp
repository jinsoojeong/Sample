#include "stdafx.h"
#include "User.h"
#include "UserManager.h"

UserManager::UserManager() : identity_id_(0)
{
}

UserManager::~UserManager()
{
	Finalize();
}

void UserManager::Update(ULONGLONG current_tick)
{
	for each(const Users::value_type& user in users_)
		(*user.second).Update(current_tick);
}

bool UserManager::RegistUser(const DWORD session_id, const DWORD id, const std::wstring& nick)
{
	Users::iterator itor = users_.find(id);
	if (itor != users_.end())
		return false;

	User* user = new User(id, nick);
	if (user == nullptr)
		return false;

	users_.insert(Users::value_type(user->GetID(), user));
	session_user_ids_.insert(SessionUserIDs::value_type(session_id, user->GetID()));

	return true;
}

bool UserManager::UnRegistUser(const DWORD id)
{
	Users::iterator itor = users_.find(id);
	if (itor == users_.end())
		return false;

	users_.erase(itor);

	return true;
}

User* UserManager::FindUser(DWORD user_id)
{
	Users::iterator itor = users_.find(user_id);
	if (itor == users_.end())
		return nullptr;

	return (*itor).second;
}

User* UserManager::FindUser(DWORD session_id, bool)
{
	SessionUserIDs::iterator itor = session_user_ids_.find(session_id);
	if (itor == session_user_ids_.end())
		return nullptr;

	return FindUser((*itor).second);
}

bool UserManager::UserCertify(const std::wstring& id, const std::wstring& pw, DWORD& user_id)
{
	CertUsers::iterator itor = cert_users_.find(id);
	if (itor != cert_users_.end())
	{
		user_id = (*itor).second;
		return true;
	}

	InterlockedIncrement(&identity_id_);
	user_id = identity_id_;

	cert_users_.insert(CertUsers::value_type(id, identity_id_));

	return true;
}

void UserManager::BuildMsg(Msg* msg, User *user)
{
	switch (msg->GetID())
	{
	case PT_REG_USER_ACK:
	{
		*msg << (DWORD)users_.size();

		for each(const Users::value_type& itor in users_)
			*msg << itor.second->GetNick();
	}
	break;
	}
}

void UserManager::BroadCast(Msg *msg, const DWORD except_user)
{
	for each (const Users::value_type& itor in users_)
	{
		if (except_user == itor.first)
			continue;

		User *user = itor.second;
		if (user == nullptr)
		{
			Log::ReportLog(L"BroadCast Fatal Warnning - find invalid user point");
			continue;
		}

		user->SendMsg(msg, true);
	}

	SAFE_DELETE(msg);
}

void UserManager::Finalize()
{
	for (Users::iterator itor = users_.begin(); itor != users_.end(); itor++)
		SAFE_DELETE(itor->second);
}