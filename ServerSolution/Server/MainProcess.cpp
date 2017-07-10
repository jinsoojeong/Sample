#include "stdafx.h"
#include "MainProcess.h"
#include "User.h"
#include "UserManager.h"

#define KEEP_ALIVE_INTERVAL 30000

MainProcess::MainProcess() : keep_alive_tick_(0)
{
}

MainProcess::~MainProcess()
{
	DELETE_USER_MANAGER;
}

void MainProcess::DoWork(void* param)
{
	bool *running = (bool*)param;

	keep_alive_tick_ = ::GetTickCount64() + KEEP_ALIVE_INTERVAL;

	while ((*running))
	{
		ULONGLONG current_tick = ::GetTickCount64();

		Update(current_tick);
		USER_MANAGER.Update(current_tick);
		SESSION_MANAGER.Update(current_tick);

		if (current_tick >= keep_alive_tick_)
		{
			Msg *msg = new Msg(PT_KEEP_ALIVE);
			PushMsgQueue(msg);

			keep_alive_tick_ = current_tick + KEEP_ALIVE_INTERVAL;
		}
	}
}

void MainProcess::EndWork()
{
}

void MainProcess::Update(ULONGLONG current_tick)
{
	Msg* msg = nullptr;

	if (PopMsgQueue(msg) == false)
		return;

	MsgHandle(msg);
}

void MainProcess::MsgHandle(Msg *msg)
{
	if (msg->GetSessionID())
	{
		Session* session = SESSION_MANAGER.GetSession(msg->GetSessionID());
		if (session == nullptr)
		{
			if (msg->GetID() == PT_SESSION_CONNECT)
				session = SESSION_MANAGER.GetAcceptSession(msg->GetSessionID());

			if (session == nullptr)
			{
				Log::ReportLog(L"msg handle failed - invalid session id:[%d]", msg->GetSessionID());
				SAFE_DELETE(msg);
				return;
			}
		}

		switch (msg->GetID())
		{
		case PT_SESSION_CONNECT:
		{
			PROC_PT_SESSION_CONNECT(session, msg);
			SAFE_DELETE(msg);
		}
		break;
		case PT_SESSION_DISCONNECT:
		{
			PROC_PT_SESSION_DISCONNECT(session, msg);
			SAFE_DELETE(msg);
		}
		break;
		case PT_CHECK_VERSION_REQ:
		{
			PROC_PT_CHECK_VERSION_REQ(session, msg);
			SAFE_DELETE(msg);
		}
		break;
		case PT_REG_USER_REQ:
		{
			PROC_PT_REG_USER_REQ(session, msg);
			SAFE_DELETE(msg);
		}
		break;
		case PT_UNREG_USER_REQ:
		{
			PROC_PT_UNREG_USER_REQ(session, msg);
			SAFE_DELETE(msg);
		}
		break;
		default:
		{
			User* user = USER_MANAGER.FindUser(session->GetID(), true);
			if (user)
				user->PushMsgQueue(msg);
		}
		break;
		}
	}
	else
	{
		switch (msg->GetID())
		{
		case PT_KEEP_ALIVE:
			PROC_PT_KEEP_ALIVE(nullptr, msg);
			break;
		default:
		{
			Log::ReportLog(L"msg handle failed - invalid msg id:[%d]", msg->GetID());
			SAFE_DELETE(msg);
		}
		break;
		}
	}
}

bool MainProcess::Init()
{
	IniReader ini_reader_;
	DWORD buffer_length;
	if (ini_reader_.Initialize(std::wstring(L"./ServerConfig.ini")) == false)
		return false;

	ini_reader_.LoadConfigData(L"SERVER", L"VERSION", module_version_, &buffer_length);

	if (module_version_.empty())
		return false;

	Log::ReportLog(L"VERSION : %s", module_version_.c_str());

	return true;
}

void MainProcess::PushMsgQueue(Msg* msg)
{
	main_msg_queue_.push(msg);
}

bool MainProcess::PopMsgQueue(Msg*& msg)
{
	if (main_msg_queue_.try_pop(msg) == false)
		return false;

	return true;
 }

void MainProcess::PROC_PT_CHECK_VERSION_REQ(Session* session, Msg* msg)
{
	BYTE error_code = PT_ERROR_NONE;

	CHECK_VERSION_REQ client_info;
	READ_PT_CHECK_VERSION_REQ(msg, client_info);

	if (client_info.version.compare(GetModuleVersion()) != 0)
	{
		Log::ReportLog(L"invalid client version");
		error_code = PT_ERROR_INVALID_VERSION;
		goto FAILED;
	}

	Msg *ack_msg = new Msg(PT_CHECK_VERSION_ACK);
	*ack_msg << error_code;

	session->SendMsg(ack_msg);

	Log::ReportLog(L"Client Check Version Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_CHECK_VERSION_ACK);
	*failed_msg << error_code;

	session->SendMsg(failed_msg);

	Log::ReportLog(L"Client Check Version Failed");
}

void MainProcess::PROC_PT_REG_USER_REQ(Session* session, Msg* msg)
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
	result = USER_MANAGER.RegistUser(session->GetID(), user_id, element[0]);
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

	user->SetSession(session);

	Msg* notify_msg = new Msg(PT_NEW_CONNECT_USER);
	*notify_msg << user->GetNick();
	SESSION_MANAGER.BroadCast(notify_msg, session);

	Msg* ack_msg = new Msg(PT_REG_USER_ACK);
	*ack_msg << error_code;
	*ack_msg << user->GetID();
	*ack_msg << user->GetNick();
	USER_MANAGER.BuildMsg(ack_msg, user);
	session->SendMsg(ack_msg);

	Log::ReportLog(L"Regist User Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_REG_USER_ACK);
	*failed_msg << error_code;
	*failed_msg << 0;
	*failed_msg << std::wstring(L"");

	session->SendMsg(failed_msg);

	Log::ReportLog(L"Regist User Failed");
}

void MainProcess::PROC_PT_UNREG_USER_REQ(Session* session, Msg* msg)
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

	tartget_user_nick = target_user->GetNick();

	// Regist User
	bool result = USER_MANAGER.UnRegistUser(target_user->GetID());
	if (result == false)
	{
		error_code = PT_ERROR_UNREGIST_FAILED;
		goto FAILED;
	}

	Msg* notify_msg = new Msg(PT_DISCONNECT_USER);
	*notify_msg << tartget_user_nick;
	SESSION_MANAGER.BroadCast(notify_msg, session);

	Msg* ack_msg = new Msg(PT_UNREG_USER_ACK);
	*ack_msg << error_code;
	session->SendMsg(ack_msg);

	Log::ReportLog(L"UnRegist User Succeed");

	return;

FAILED:
	Msg* failed_msg = new Msg(PT_UNREG_USER_ACK);
	*failed_msg << error_code;
	session->SendMsg(failed_msg);

	Log::ReportLog(L"UnRegist User Failed");
}

void MainProcess::PROC_PT_KEEP_ALIVE(Session* session, Msg* msg)
{
	SESSION_MANAGER.BroadCast(msg);
	Log::ReportLog(L"KEEP ALIVE SESSION CHECK");
}

void MainProcess::PROC_PT_SESSION_CONNECT(Session* session, Msg* msg)
{
	if (SESSION_MANAGER.ConnectSession(session) == false)
		Log::ReportLog(L"Session Connect Failed : [%lld]", reinterpret_cast<ULONGLONG>(session));
	else
		Log::ReportLog(L"Session Connect : [%lld]", reinterpret_cast<ULONGLONG>(session));

	SESSION_MANAGER.AcceptPoolCheck();
}

void MainProcess::PROC_PT_SESSION_DISCONNECT(Session* session, Msg* msg)
{
	User *user = USER_MANAGER.FindUser(session->GetID(), true);
	if (user)
	{
		Log::ReportLog(L"Session DisConnect User UnRegist : User_ID:[%d]", user->GetID());

		Msg* notify_msg = new Msg(PT_DISCONNECT_USER);
		*notify_msg << user->GetNick();
		SESSION_MANAGER.BroadCast(notify_msg, session);
		USER_MANAGER.UnRegistUser(user->GetID());
	}

	SESSION_MANAGER.DisConnectSession(session);

	Log::ReportLog(L"Session DisConnect : [%lld]", reinterpret_cast<ULONGLONG>(session));
}