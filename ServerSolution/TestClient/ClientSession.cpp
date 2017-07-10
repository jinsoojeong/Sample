#include "stdafx.h"
#include "ClientSession.h"
#include "TestClientDlg.h"

ClientSession::ClientSession() : connect_error_(0), user_id_(0), client_state_(InitState)
{
}

ClientSession::~ClientSession()
{
}

bool ClientSession::SendCheckVersionReq(const std::wstring& client_version)
{
	Msg* req_msg = new Msg(PT_CHECK_VERSION_REQ);
	*req_msg << client_version;
	return SendMsg(req_msg);
}

bool ClientSession::SendRegUserReq(const std::wstring& ticket)
{
	Msg* req_msg = new Msg(PT_REG_USER_REQ);
	*req_msg << ticket;
	return SendMsg(req_msg);
}

bool ClientSession::SendUnRegUserReq(const DWORD target_id)
{
	Msg* req_msg = new Msg(PT_UNREG_USER_REQ);
	*req_msg << target_id;
	return SendMsg(req_msg);
}

bool ClientSession::SendChatReq(const std::wstring& chat)
{
	Msg* req_msg = new Msg(PT_CHAT_REQ);
	*req_msg << chat;
	return SendMsg(req_msg);
}

void ClientSession::TestClientLog(wchar_t* words, ...)
{
	TCHAR buff[1024] = { 0, };
	va_list va;
	va_start(va, words);
	if (StringCchVPrintfW(buff, 1024, words, va) == S_OK)
	{
		CString log = buff;
		((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->LogView(log);
	}
}

void ClientSession::SetUserInfo(DWORD user_id, std::wstring nick)
{
	user_id_ = user_id;
	user_nick_ = nick;
}

void ClientSession::NewUserConnect(std::wstring new_user_nick)
{
	((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->UserConnected(new_user_nick.c_str());
	((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->connect_users_.push_back(new_user_nick.c_str());;
}

void ClientSession::UserDisConnect(std::wstring user_nick)
{
	((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->UserDisConnected(user_nick.c_str());
}

void ClientSession::Chat(std::wstring chat)
{
	((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->ChatView(chat.c_str());
}

void ClientSession::MsgHandle()
{
	Msg* msg = ClientNet::GetMsg();

	if (msg == nullptr)
		return;

	switch (msg->GetID())
	{
	case PT_KEEP_ALIVE:
		TestClientLog(L"KEEP ALIVE SESSION CHECK");
		break;
	case PT_CHECK_VERSION_ACK:
	{
		CHECK_VERSION_ACK check_version_ack;
		READ_PT_CHECK_VERSION_ACK(msg, check_version_ack);

		if (check_version_ack.error_code == PT_ERROR_NONE)
			TestClientLog(L"VERSION CEHCK SUCCEED");
		else
			TestClientLog(L"VERSION CHECK FAILED, ERROR_CODE:[%d]", check_version_ack.error_code);
	}
	break;
	case PT_REG_USER_ACK:
	{
		REG_USER_ACK reg_user_ack;
		READ_PT_REG_USER_ACK(msg, reg_user_ack);

		// 읽은 패킷을 출력합니다.
		if (reg_user_ack.error_code == PT_ERROR_NONE)
		{ 
			TestClientLog(L"REG SUCCEED : USER_ID:[%d], USER_NICK:[%s]", reg_user_ack.id, reg_user_ack.nick.c_str());
			SetUserInfo(reg_user_ack.id, reg_user_ack.nick);

			for each (const std::vector<std::wstring>::value_type& itor in reg_user_ack.connect_users)
				NewUserConnect(itor);
		}
		else
			TestClientLog(L"REG FAILED : ERROR_CODE:[%d]", reg_user_ack.error_code);
	}
	break;
	case PT_UNREG_USER_ACK:
	{
		UNREG_USER_ACK unreg_user_ack;
		READ_PT_UNREG_USER_ACK(msg, unreg_user_ack);

		// 읽은 패킷을 출력합니다.
		if (unreg_user_ack.error_code == PT_ERROR_NONE)
		{
			TestClientLog(L"UNREG SUCCEED");
			SetUserInfo(0, L"");
		}
		else
			TestClientLog(L"UNREG FAILED : ERROR_CODE:[%d]", unreg_user_ack.error_code);
	}
	break;
	case PT_NEW_CONNECT_USER:
	{
		NEW_CONNECT_USER new_connect_user;
		READ_PT_NEW_CONNECT_USER(msg, new_connect_user);

		TestClientLog(L"NEW CONNECTED USER : USER_NICK:[%s]", new_connect_user.nick.c_str());
		NewUserConnect(new_connect_user.nick);
	}
	break;
	case PT_DISCONNECT_USER:
	{
		DISCONNECT_USER disconnect_user;
		READ_PT_DISCONNECT_USER(msg, disconnect_user);

		TestClientLog(L"DISCONNECTED USER : USER_NICK:[%s]", disconnect_user.nick.c_str());
		UserDisConnect(disconnect_user.nick);
	}
	break;
	case PT_CHAT:
	case PT_CHAT_ACK:
	{
		CHAT chat;
		READ_PT_CHAT(msg, chat);

		TestClientLog(L"NEW CHAT TEXT : [%s]", chat.text.c_str());

		if (chat.text.compare(L"") != 0)
			Chat(chat.text);
	}
	break;
	}

	SAFE_DELETE(msg);
}

bool ClientSession::Init()
{
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	IniReader ini_reader_;
	DWORD buffer_length;
	if (ini_reader_.Initialize(std::wstring(L"./ClientConfig.ini")) == false)
	{
		TestClientLog(L"---- Client Config Initialize Failed ----");
		connect_error_ = 1;
		return false;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"IP", ip_, &buffer_length);

	if (ip_.empty())
	{
		TestClientLog(L"---- Client Config Initialize Failed - ip load failed ----");
		connect_error_ = 2;
		return false;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"PORT", &port_);

	if (port_ == 0)
	{
		TestClientLog(L"---- Client Config Initialize Failed - port load failed ----");
		connect_error_ = 3;
		return false;
	}

	ini_reader_.LoadConfigData(L"CLIENT", L"VERSION", version_, &buffer_length);

	if (version_.empty())
	{
		TestClientLog(L"---- Client Config Initialize Failed - version load failed ----");
		connect_error_ = 4;
		return false;	
	}

	if (Connect() == false)
		return false;

	return true;
}

bool ClientSession::Connect()
{
	if (client_state_ != InitState)
	{
		TestClientLog(L"connect failed - action denied invalid client state:[%d]", client_state_);
		return false;
	}

	USES_CONVERSION;

	if (ClientNet::Initialize(W2A(ip_.c_str()), static_cast<WORD>(port_)) == false)
	{
		TestClientLog(L"---- Client Session Initialize Failed ----");
		connect_error_ = 5;
		return false;
	}		

	TestClientLog(L"---- Client Session Initialize Succeed ----");
	SetClientState(ConnectedState);

	return true;
}

bool ClientSession::VersionCheck()
{
	if (client_state_ != ConnectedState)
	{
		TestClientLog(L"version check failed - action denied invalid client state:[%d]", client_state_);
		return false;
	}

	if (connect_error_ != 0)
	{
		TestClientLog(L"client connect initialize faild - connect error : [%d]", connect_error_);
		return false;
	}

	// 패킷을 전송합니다.
	std::wstring client_version = L"1.0.0.1";
	if (SendCheckVersionReq(client_version) == false)
	{
		TestClientLog(L"SendCheckVersionReq - connect error");
		connect_error_ = 6;
		return false;
	}

	SetClientState(RunningState);
	return true;
}

bool ClientSession::UserLogin(const std::wstring ticket)
{
	if (client_state_ != RunningState)
	{
		TestClientLog(L"user login failed - action denied invalid client state:[%d]", client_state_);
		return false;
	}

	if (connect_error_ != 0)
	{
		TestClientLog(L"client connect initialize faild - connect error : [%d]", connect_error_);
		return false;
	}

	if (user_id_ != 0 || user_nick_.compare(L"") != 0)
	{
		TestClientLog(L"client already reg user - user_id : [%d], nick : [%s]", user_id_, user_nick_.c_str());
		return false;
	}
	
	if (SendRegUserReq(ticket) == false)
		return false;

	SetClientState(LoginState);
	return true;
}

bool ClientSession::UserLogOut()
{
	if (client_state_ != LoginState)
	{
		TestClientLog(L"user logout failed - action denied invalid client state:[%d]", client_state_);
		return false;
	}

	if (connect_error_ != 0)
	{
		TestClientLog(L"client connect initialize faild - connect error : [%d]", connect_error_);
		return false;
	}

	if (user_id_ == 0 && user_nick_.compare(L"") == 0)
	{
		TestClientLog(L"client already unreg user - user_id : [%d], nick : [%s]", user_id_, user_nick_.c_str());
		return false;
	}

	// 패킷을 전송합니다.

	if (SendUnRegUserReq(user_id_) == false)
		return false;

	SetUserInfo(0, L"");
	SetClientState(RunningState);
	return true;
}

bool ClientSession::SendChatText(const std::wstring text)
{
	if (client_state_ != LoginState)
	{
		TestClientLog(L"send chat text failed - action denied invalid client state:[%d]", client_state_);
		return false;
	}

	if (connect_error_ != 0)
	{
		TestClientLog(L"client connect initialize faild - connect error : [%d]", connect_error_);
		return false;
	}

	if (user_id_ == 0 && user_nick_.compare(L"") == 0)
	{
		TestClientLog(L"client not reg user - user_id : [%d], nick : [%s]", user_id_, user_nick_.c_str());
		return false;
	}

	std::wstring chat = user_nick_ + L" : " + text;

	if (SendChatReq(chat) == false)
		return false;

	return true;
}