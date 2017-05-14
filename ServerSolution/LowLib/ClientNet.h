#pragma once

class ClientNet : public EventSelect
{
public:
	ClientNet();
	virtual ~ClientNet();

	bool Initialize(std::string& ip, WORD port);

	bool GetIP(std::string& ip);
	WORD GetPort();

	bool SendMsg(Msg* msg);
	Msg* GetMsg();

protected:
	void OnIoRead();
	void OnIoWrote() {};

	virtual void TestClientLog(wchar_t* words, ...) = 0;
	virtual void MsgHandle() = 0;

private:
	NetworkPacket* network_packet_;
	Concurrency::concurrent_queue<Msg*> read_msg_queue_;
};
