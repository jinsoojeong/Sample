#include "stdafx.h"
#include "UserManager.h"
#include "User.h"

UserManager::UserManager() : identity_id_(0)
{
}

UserManager::~UserManager()
{
	TS_CRETICAL_SECSION sc;

	for (Users::iterator itor = users_.begin() ; itor != users_.end(); itor++)
		SAFE_DELETE(itor->second);

	users_.clear();
}

bool UserManager::RegistUser(const DWORD id, const std::wstring& nick)
{
	TS_CRETICAL_SECSION sc;

	Users::iterator itor = users_.find(id);
	if (itor != users_.end())
		return false;

	User* user = new User(id, nick);
	if (user == nullptr)
		return false;

	users_.insert(Users::value_type(user->GetID(), user));

	return true;
}

bool UserManager::UnRegistUser(const DWORD id)
{
	TS_CRETICAL_SECSION sc;

	Users::iterator itor = users_.find(id);
	if (itor == users_.end())
		return false;

	users_.erase(itor);

	return true;
}

User* UserManager::FindUser(DWORD user_id)
{
	TS_CRETICAL_SECSION sc;

	Users::iterator itor = users_.find(user_id);
	if (itor == users_.end())
		return nullptr;

	return (*itor).second;
}

bool UserManager::UserCertify(const std::wstring& id, const std::wstring& pw, DWORD& user_id)
{
	TS_CRETICAL_SECSION sc;

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

		for each(Users::value_type itor in users_)
			*msg << itor.second->GetNick();
	}
	break;
	}
}