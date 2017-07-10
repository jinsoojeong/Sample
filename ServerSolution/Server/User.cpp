#include "stdafx.h"
#include "User.h"

User::User(DWORD id, std::wstring nick) : id_(id), nick_(nick)
{
}

User::~User()
{
	user_msg_queue_.clear();
}

void User::Update(ULONGLONG current_tick)
{
	Msg* msg = nullptr;

	if (PopMsgQueue(msg))
		MsgHandle(msg);
}

void User::PushMsgQueue(Msg* msg)
{
	user_msg_queue_.push(msg);
}

bool User::PopMsgQueue(Msg*& msg)
{
	if (user_msg_queue_.try_pop(msg) == false)
		return false;

	return true;
}