#include "stdafx.h"
#include "UserManager.h"
#include "User.h"

using namespace PT_Format;

void User::MsgHandle(Msg* msg)
{
	switch (msg->GetID())
	{
	case PT_CHAT_REQ:
		PROC_PT_CHAT_REQ(msg);
		break;
	default:
		Log::ReportLog(L"msg handle failed - invalid msg id:[%d], user_id:[%d]", msg->GetID(),GetID());
		break;
	}
}

void User::PROC_PT_CHAT_REQ(Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;

	CHAT chat;
	READ_PT_CHAT(msg, chat);

	Msg* notify_msg = new Msg(PT_CHAT);
	*notify_msg << chat.text;
	USER_MANAGER.BroadCast(notify_msg, GetID());

	Msg* ack_msg = new Msg(PT_CHAT_ACK);
	*ack_msg << chat.text;
	SendMsg(ack_msg);
}