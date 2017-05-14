#include "stdafx.h"
#include "Session.h"
#include "UserManager.h"
#include "User.h"

Session::Session() : is_connected_(false), user_id_(0)
{
}

Session::~Session()
{
}

bool Session::Restart(SOCKET listen_socket)
{
	TS_CRETICAL_SECSION sc;

	NetworkPacket::ResetSocket();

	if (Initialize() && Accept(listen_socket))
		return true;

	return false;
}

void Session::SetIsConnected(bool is_connected) 
{
	TS_CRETICAL_SECSION sc; 

	is_connected_ = is_connected; 
}

bool Session::GetIsConnected() 
{ 
	TS_CRETICAL_SECSION sc;

	return is_connected_; 
}

void Session::SetUserID(DWORD user_id)
{
	TS_CRETICAL_SECSION sc;

	user_id_ = user_id;
}

DWORD Session::GetUserID()
{
	TS_CRETICAL_SECSION sc;

	return user_id_;
}

void Session::ConnectProc(SOCKET listen_socket)
{
	TS_CRETICAL_SECSION sc;

	if (ResetRecvState() == false)
	{
		Restart(listen_socket);
		return;
	}

	SetIsConnected(true);
}

void Session::DisConnectProc(SOCKET listen_socket)
{
	TS_CRETICAL_SECSION sc;

	User* user = USER_MANAGER.FindUser(GetUserID());

	if (user)
	{
		Log::ReportLog(L"Session DisConnect User UnRegist : User_ID:[%d]", user->GetID());

		Msg* notify_msg = new Msg(PT_DISCONNECT_USER);
		*notify_msg << user->GetNick();
		SESSION_MANAGER.BroadCast(notify_msg, this);

		SAFE_DELETE(notify_msg);
	}

	USER_MANAGER.UnRegistUser(GetUserID());
	
	Restart(listen_socket);
	SetIsConnected(false);
	SetUserID(0);
}