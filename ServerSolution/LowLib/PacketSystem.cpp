#include "stdafx.h"
#include "ThreadSync.h"
#include "SockSystem.h"
#include "PacketSystem.h"
#include "Session.h"
#include "Msg.h"

PacketSystem::PacketSystem() : remain_length_(0), current_packet_number_(0), last_read_packet_number_(0)
{
	memset(packet_buffer_, 0, sizeof(packet_buffer_));
}

PacketSystem::~PacketSystem()
{
	write_msg_queue_.clear();
}

bool PacketSystem::Initialize(void *object, WORD port, INT back_log)
{
	SCOPED_SINGLE_LOCK(&sc_);

	memset(packet_buffer_, 0, sizeof(packet_buffer_));

	remain_length_ = 0;
	current_packet_number_ = 0;
	last_read_packet_number_ = 0;
	write_msg_queue_.clear();

	if (SockSystem::Initialize(object, port, back_log) == false)
		return false;

	return true;
}

bool PacketSystem::Create()
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (SockSystem::Create() == false)
		return false;

	return true;
}

bool PacketSystem::Accept(SOCKET listen_socket)
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (SockSystem::Accept(listen_socket) == false)
		return false;

	return true;
}

bool PacketSystem::Connect(const std::string& ip, WORD port)
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (SockSystem::Connect(ip, port) == false)
		return false;

	return true;
}

bool PacketSystem::RecvWait()
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (SockSystem::RecvWait() == false)
		return false;

	return true;
}

bool PacketSystem::RecvByIocp(DWORD read_length)
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (SockSystem::RecvByIocp(packet_buffer_ + remain_length_, read_length) == false)
		return false;

	remain_length_ += read_length;

	return true;
}

bool PacketSystem::RecvByEventSelect()
{
	SCOPED_SINGLE_LOCK(&sc_);

	DWORD read_length = 0;

	if (SockSystem::RecvByEventSelect(packet_buffer_ + remain_length_, read_length) == false)
		return false;

	remain_length_ += read_length;

	return true;
}

bool PacketSystem::SendMsg(Msg* msg)
{
	SCOPED_SINGLE_LOCK(&sc_);

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

	if (SockSystem::Send(packet_buffer, packet_size) == false)
		return false;

	return true;
}

bool PacketSystem::GetMsg(Msg*& msg)
{
	SCOPED_SINGLE_LOCK(&sc_);

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

bool PacketSystem::SendComplete()
{
	SCOPED_SINGLE_LOCK(&sc_);

	Msg* msg = nullptr;
	if (write_msg_queue_.try_pop(msg) == false)
		return false;

	SAFE_DELETE(msg);

	return true;
}

void PacketSystem::ResetSocket()
{
	SCOPED_SINGLE_LOCK(&sc_);

	SockSystem::ResetSocket();
}