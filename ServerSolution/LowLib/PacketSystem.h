#pragma once

class Msg;
class Session;
class PacketSystem : public SockSystem
{
public:
	PacketSystem();
	virtual ~PacketSystem();

	bool Initialize(void *object, WORD port = 0, INT back_log = 5);

	// TCP
	bool Create();
	bool Accept(SOCKET listen_socket);
	bool Connect(const std::string& ip, WORD port);

	bool RecvWait();
	bool RecvByIocp(DWORD read_length);
	bool RecvByEventSelect();
	
	bool SendMsg(Msg* msg);
	bool GetMsg(Msg*& msg);

	bool SendComplete();
	void ResetSocket();
	
private:
	BYTE packet_buffer_[MAX_BUFFER_LENGTH * 3];

	DWORD remain_length_;
	DWORD current_packet_number_;
	DWORD last_read_packet_number_;

	CriticalSection sc_;

	Concurrency::concurrent_queue<Msg*> write_msg_queue_;
};
