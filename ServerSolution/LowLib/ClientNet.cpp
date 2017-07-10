#include "stdafx.h"
#include "SockSystem.h"
#include "PacketSystem.h"
#include "ClientNet.h"
#include "EventSelect.h"
#include "Msg.h"
#include "../CommonFile/Protocol.h"

ClientNet::ClientNet()
{
}

ClientNet::~ClientNet()
{
	SAFE_DELETE(packet_system_);
}

bool ClientNet::Initialize(const std::string& ip, WORD port)
{
	if (ip.empty() || port <= 0)
		return false;

	packet_system_ = new PacketSystem();

	if (!packet_system_->Initialize(this, 0))
		return false;

	if (!packet_system_->Create())
		return false;

	if (!EventSelect::Initialize(packet_system_->GetSocket()))
		return false;

	if (!packet_system_->Connect(ip, port))
		return false;

	return true;
}

bool ClientNet::GetIP(std::string& ip)
{
	return packet_system_->GetIP(ip);
}

WORD ClientNet::GetPort()
{
	return packet_system_->GetPort();
}

bool ClientNet::SendMsg(Msg* msg)
{
	if (!packet_system_->SendMsg(msg))
		return false;

	if (!packet_system_->SendComplete())
		return false;

	return true;
}

Msg* ClientNet::GetMsg()
{
	Msg* msg = nullptr;

	if (client_msg_queue_.try_pop(msg) == false)
		return nullptr;

	return msg;
}

void ClientNet::OnIoRead()
{
	Msg* msg = nullptr;

	if (packet_system_->RecvByEventSelect())
	{
		while (packet_system_->GetMsg(msg))
		{
			if (msg == nullptr)
				return;

			client_msg_queue_.push(msg);
			MsgHandle();
		}		
	}
}