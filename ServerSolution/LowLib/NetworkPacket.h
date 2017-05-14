#pragma once

class Msg;
class NetworkPacket : public Network
{
public:
	NetworkPacket();
	virtual ~NetworkPacket();

	bool Initialize();

	// TCP
	bool RecvbyIocp(DWORD read_length);
	bool RecvbyEventSelect();
	bool SendMsg(Msg* msg);
	bool GetMsg(Msg*& msg);

	bool SendComplete();

protected:
	void ResetSocket();

private:
	BYTE packet_buffer_[MAX_BUFFER_LENGTH * 3];

	DWORD remain_length_;
	DWORD current_packet_number_;
	DWORD last_read_packet_number_;

	Concurrency::concurrent_queue<Msg*> write_msg_queue_;
};
