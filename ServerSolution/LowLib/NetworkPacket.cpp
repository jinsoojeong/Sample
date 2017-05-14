#include "stdafx.h"
#include "CriticalSection.h"
#include "ThreadSync.h"
#include "Network.h"
#include "NetworkPacket.h"
#include "Msg.h"

NetworkPacket::NetworkPacket()
{
	memset(packet_buffer_, 0, sizeof(packet_buffer_));

	remain_length_ = 0;
	current_packet_number_ = 0;
	last_read_packet_number_ = 0;
}

NetworkPacket::~NetworkPacket()
{
	TS_CRETICAL_SECSION sc;

	write_msg_queue_.clear();
}

bool NetworkPacket::Initialize()
{
	TS_CRETICAL_SECSION sc;

	memset(packet_buffer_, 0, sizeof(packet_buffer_));

	remain_length_ = 0;
	current_packet_number_ = 0;
	last_read_packet_number_ = 0;
	write_msg_queue_.clear();

	if (Network::Initialize() == false)
		return false;

	return true;
}

bool NetworkPacket::RecvbyIocp(DWORD read_length)
{
	TS_CRETICAL_SECSION sc;

	if (!Network::RecvbyIocp(packet_buffer_ + remain_length_, read_length))
		return false;

	remain_length_ += read_length;

	return true;
}

bool NetworkPacket::RecvbyEventSelect()
{
	TS_CRETICAL_SECSION sc;

	DWORD read_length = 0;

	if (!Network::RecvbyEventSelect(packet_buffer_ + remain_length_, read_length))
		return false;

	remain_length_ += read_length;

	return true;
}

bool NetworkPacket::SendMsg(Msg* msg)
{
	TS_CRETICAL_SECSION sc;

	if (msg == nullptr)
		return false;

	const BYTE* buffer = msg->GetBuffer();
	if (buffer == nullptr)
		return false;

	const DWORD protocol = msg->GetID();

	DWORD packet_size = sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + msg->GetLength();

	if (packet_size >= MAX_BUFFER_LENGTH)
		return false;

	current_packet_number_++;

	BYTE packet_buffer[MAX_BUFFER_LENGTH] = { 0, };

	memcpy(packet_buffer, &packet_size, sizeof(DWORD));
	memcpy(packet_buffer + sizeof(DWORD), &current_packet_number_, sizeof(DWORD));
	memcpy(packet_buffer + sizeof(DWORD) + sizeof(DWORD), &protocol, sizeof(DWORD));
	memcpy(packet_buffer + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD), buffer, msg->GetLength());

	write_msg_queue_.push(msg);

	if (Network::Send(packet_buffer, packet_size) == false)
		return false;

	return true;
}

bool NetworkPacket::GetMsg(Msg*& msg)
{
	TS_CRETICAL_SECSION sc;

	if (remain_length_ < sizeof(DWORD))
		return false;

	DWORD packet_size = 0;
	memcpy(&packet_size, packet_buffer_, sizeof(DWORD));

	if (packet_size > MAX_BUFFER_LENGTH || packet_size <= 0) // Invalid Packet
	{
		remain_length_ = 0;
		return false;
	}

	if (packet_size > remain_length_)
		return false;

	DWORD packet_number = 0;
	DWORD protocol = 0;
	DWORD buffer_size = 0;

	// packet_number 추출
	memcpy(&packet_number, packet_buffer_ + sizeof(DWORD), sizeof(DWORD));

	// protocol 추출
	memcpy(&protocol, packet_buffer_ + sizeof(DWORD) + sizeof(DWORD), sizeof(DWORD));

	buffer_size = packet_size - sizeof(DWORD) - sizeof(DWORD) - sizeof(DWORD);

	msg = new Msg(protocol);
	memcpy(const_cast<BYTE*>(msg->GetBuffer()), packet_buffer_ + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD), buffer_size);

	if (remain_length_ - packet_size > 0)
		memmove(packet_buffer_, packet_buffer_ + packet_size, remain_length_ - packet_size);

	remain_length_ -= packet_size;

	if (remain_length_ <= 0)
	{
		remain_length_ = 0;
		memset(packet_buffer_, 0, sizeof(packet_buffer_));
	}

	if (packet_number <= last_read_packet_number_)
		return false;

	last_read_packet_number_ = packet_number;

	return true;
}

bool NetworkPacket::SendComplete()
{
	TS_CRETICAL_SECSION sc;

	Msg* msg = nullptr;
	if (write_msg_queue_.try_pop(msg) == false)
		return false;

	SAFE_DELETE(msg);

	return true;
}

void NetworkPacket::ResetSocket()
{
	TS_CRETICAL_SECSION sc;

	Network::ResetSocket();
}