#include "stdafx.h"
#include "ClientSession.h"
#include "TestClientDlg.h"

BOOL ClientSession::Initialize(std::string ip, WORD port)
{
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (ClientNet::Initialize(ip, port) == FALSE)
		return FALSE;

	return TRUE;
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

void ClientSession::OnIoConnected()
{
	return;
}

void ClientSession::OnIoDisconnected()
{
	return;
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
	((CTestClientDlg*)AfxGetApp()->m_pMainWnd)->SetUserInfo(user_id, nick);
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

			for each (std::vector<std::wstring>::value_type itor in reg_user_ack.connect_users)
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