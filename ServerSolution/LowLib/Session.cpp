#include "stdafx.h"
#include "ThreadSync.h"
#include "SockSystem.h"
#include "PacketSystem.h"
#include "Session.h"
#include "Msg.h"

Session::Session(DWORD id) : id_(id), packet_system_(nullptr), is_connected_(false)
{
}

Session::~Session()
{
	SAFE_DELETE(packet_system_);
}

void Session::Update(ULONGLONG current_tick)
{
}

bool Session::Initialize(SOCKET listen_socket)
{
	if (listen_socket == NULL)
		return false;

	packet_system_ = new PacketSystem;

	if (packet_system_->Initialize(this) == false)
		return false;

	if (packet_system_->Accept(listen_socket) == false)
		return false;

	return true;
}

bool Session::ReAcceptWait(SOCKET listen_socket)
{
	if (listen_socket == NULL)
		return false;

	packet_system_->ResetSocket();

	if (packet_system_->Accept(listen_socket) == false)
		return false;

	return true;
}

SOCKET Session::GetSocket()
{
	return packet_system_->GetSocket();
}

bool Session::RecvByIocp(DWORD data_length)
{
	return packet_system_->RecvByIocp(data_length);
}

bool Session::SendMsg(Msg* msg, bool broad_cast)
{
	bool result = packet_system_->SendMsg(msg);

	if (broad_cast == false)
		SAFE_DELETE(msg);

	return result;
}

bool Session::GetMsg(Msg*& msg)
{
	Msg *new_msg = nullptr;
	if (packet_system_->GetMsg(new_msg) == false)
		return false;

	msg = new_msg;
	return true;
}

bool Session::RecvWait(SOCKET listen_socket)
{
	if (packet_system_->RecvWait() == false)
	{
		ReAcceptWait(listen_socket);
		return false;
	}

	return true;
}
