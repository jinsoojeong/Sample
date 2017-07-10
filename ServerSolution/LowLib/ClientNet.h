#pragma once

class ClientNet : public EventSelect
{
public:
	ClientNet();
	virtual ~ClientNet();

	bool Initialize(const std::string& ip, WORD port);

	bool GetIP(std::string& ip);
	WORD GetPort();

	bool SendMsg(Msg* msg);
	Msg* GetMsg();

protected:
	void OnIoRead();
	void OnIoWrote() {};
	void OnIoConnected() {};
	void OnIoDisconnected() {};

	virtual void TestClientLog(wchar_t* words, ...) = 0;
	virtual void MsgHandle() = 0;

private:
	PacketSystem *packet_system_;
	Concurrency::concurrent_queue<Msg*> client_msg_queue_;
};
