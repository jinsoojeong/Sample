#include "stdafx.h"
#include "CriticalSection.h"
#include "Network.h"
#include "NetworkPacket.h"
#include "ClientNet.h"
#include "EventSelect.h"
#include "Msg.h"
#include "../CommonFile/Protocol.h"

ClientNet::ClientNet()
{
}

ClientNet::~ClientNet()
{
	SAFE_DELETE(network_packet_);
}

bool ClientNet::Initialize(std::string& ip, WORD port)
{
	if (ip.empty() || port <= 0)
		return false;

	network_packet_ = new NetworkPacket();

	if (!network_packet_->Initialize())
		return false;

	if (!network_packet_->CreateSocket())
		return false;

	if (!EventSelect::Initialize(network_packet_->GetSocket()))
		return false;

	if (!network_packet_->Connect(ip, port))
		return false;

	return true;
}

bool ClientNet::GetIP(std::string& ip)
{
	return network_packet_->GetIP(ip);
}

WORD ClientNet::GetPort()
{
	return network_packet_->GetPort();
}

bool ClientNet::SendMsg(Msg* msg)
{
	if (!network_packet_->SendMsg(msg))
		return false;

	if (!network_packet_->SendComplete())
		return false;

	return true;
}

Msg* ClientNet::GetMsg()
{
	Msg* msg = nullptr;

	if (read_msg_queue_.try_pop(msg) == false)
		return nullptr;

	return msg;
}

void ClientNet::OnIoRead()
{
	Msg* msg = nullptr;

	if (network_packet_->RecvbyEventSelect())
	{
		while (network_packet_->GetMsg(msg))
		{
			if (msg == nullptr)
				return;

			read_msg_queue_.push(msg);	
			MsgHandle();
		}		
	}
}