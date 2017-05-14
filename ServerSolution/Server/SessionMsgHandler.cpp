#include "stdafx.h"
#include "SessionManager.h"
#include "Session.h"
#include "UserManager.h"
#include "User.h"

using namespace PT_Format;

void Session::MsgHandle(Msg* msg)
{
	switch (msg->GetID())
	{
	case PT_CHECK_VERSION_REQ:
		PROC_PT_CHECK_VERSION_REQ(msg);
		break;
	case PT_REG_USER_REQ:
		PROC_PT_REG_USER_REQ(msg);
		break;
	case PT_UNREG_USER_REQ:
		PROC_PT_UNREG_USER_REQ(msg);
		break;
	case PT_CHAT_REQ:
		PROC_PT_CHAT_REQ(msg);
		break;
	}

	SAFE_DELETE(msg);
}

void Session::PROC_PT_CHECK_VERSION_REQ(Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;

	CHECK_VERSION_REQ client_info;
	READ_PT_CHECK_VERSION_REQ(msg, client_info);

	if (client_info.version.compare(SESSION_MANAGER.GetModuleVersion()) != 0)
	{
		Log::ReportLog(L"invalid client version");
		error_code = PT_ERROR_INVALID_VERSION;
		goto FAILED;
	}

	Msg* ack_msg = new Msg(PT_CHECK_VERSION_ACK);
	*ack_msg << error_code;

	SendMsg(ack_msg);

	Log::ReportLog(L"Client Check Version Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_CHECK_VERSION_ACK);
	*failed_msg << error_code;

	SendMsg(failed_msg);

	Log::ReportLog(L"Client Check Version Failed");
}

void Session::PROC_PT_REG_USER_REQ(Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;

	REG_USER_REQ user_info;
	READ_PT_REG_USER_REQ(msg, user_info);

	// Cert
	std::vector<std::wstring> element;
	StringSplit(user_info.ticket, '|', element);

	if (element.empty())
	{
		Log::ReportLog(L"invalid ticket string");
		error_code = PT_ERROR_USER_CERT_FAILED;
		goto FAILED;
	}
	
	DWORD user_id;
	bool result = USER_MANAGER.UserCertify(element[0], element[1], user_id);
	if (result == false)
	{
		Log::ReportLog(L"user certify failed - ticket:[%s]", user_info.ticket.c_str());
		error_code = PT_ERROR_USER_CERT_FAILED;
		goto FAILED;
	}

	// Regist User
	result = USER_MANAGER.RegistUser(user_id, element[0]);
	if (result == false)
	{
		error_code = PT_ERROR_USER_EXIST;
		goto FAILED;
	}

	User* user = USER_MANAGER.FindUser(user_id);
	if (user == nullptr)
	{
		error_code = PT_ERROR_USER_NOT_FOUND;
		goto FAILED;
	}

	SetUserID(user_id);

	Msg* notify_msg = new Msg(PT_NEW_CONNECT_USER);
	*notify_msg << user->GetNick();
	SESSION_MANAGER.BroadCast(notify_msg, this);

	SAFE_DELETE(notify_msg);

	Msg* ack_msg = new Msg(PT_REG_USER_ACK);
	*ack_msg << error_code;
	*ack_msg << user->GetID();
	*ack_msg << user->GetNick();
	USER_MANAGER.BuildMsg(ack_msg, user);
	SendMsg(ack_msg);

	SAFE_DELETE(ack_msg);

	Log::ReportLog(L"Regist User Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_REG_USER_ACK);
	*failed_msg << error_code;
	*failed_msg << 0;
	*failed_msg << std::wstring(L"");

	SendMsg(failed_msg);

	SAFE_DELETE(failed_msg);

	Log::ReportLog(L"Regist User Failed");
}

void Session::PROC_PT_UNREG_USER_REQ(Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;
	std::wstring tartget_user_nick;

	UNREG_USER_REQ unreg_user_info;
	READ_PT_UNREG_USER_REQ(msg, unreg_user_info);

	User* target_user = USER_MANAGER.FindUser(unreg_user_info.id);
	if (target_user == nullptr)
	{
		error_code = PT_ERROR_USER_NOT_FOUND;
		goto FAILED;
	}

	// Regist User
	bool result = USER_MANAGER.UnRegistUser(target_user->GetID());
	if (result == false)
	{
		error_code = PT_ERROR_UNREGIST_FAILED;
		goto FAILED;
	}

	tartget_user_nick = target_user->GetNick();

	Msg* notify_msg = new Msg(PT_DISCONNECT_USER);
	*notify_msg << tartget_user_nick;
	SESSION_MANAGER.BroadCast(notify_msg, this);

	SAFE_DELETE(notify_msg);

	Msg* ack_msg = new Msg(PT_UNREG_USER_ACK);
	*ack_msg << error_code;
	SendMsg(ack_msg);

	SAFE_DELETE(ack_msg);

	Log::ReportLog(L"UnRegist User Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_UNREG_USER_ACK);
	*failed_msg << error_code;
	SendMsg(failed_msg);

	SAFE_DELETE(failed_msg);
	
	Log::ReportLog(L"UnRegist User Failed");
}

void Session::PROC_PT_CHAT_REQ(Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;

	CHAT chat;
	READ_PT_CHAT(msg, chat);

	Msg* notify_msg = new Msg(PT_CHAT);
	*notify_msg << chat.text;
	SESSION_MANAGER.BroadCast(notify_msg, this);

	SAFE_DELETE(notify_msg);

	Msg* ack_msg = new Msg(PT_CHAT_ACK);
	*ack_msg << chat.text;
	SendMsg(ack_msg);

	SAFE_DELETE(ack_msg);
}
