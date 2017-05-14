#pragma once

class Network : public ThreadSync<Network>
{
public:
	Network();
	virtual ~Network();

	bool Initialize(WORD port = 0, INT back_log = 0);

	bool CreateSocket();
	
	bool Listen(WORD port, INT back_log);
	bool Accept(SOCKET listen_socket);
	bool Connect(std::string& ip, WORD port);
	
	bool RecvbyIocp(BYTE *data, DWORD &data_length);
	bool RecvbyEventSelect(BYTE *data, DWORD &data_length);
	bool ResetRecvState();

	bool Send(const BYTE *data, DWORD data_length);

	SOCKET GetSocket();
	bool GetIP(std::string& ip);
	WORD GetPort();

protected:
	void ResetSocket();

private:
	OVERLAPPED_EX accept_overlapped_;
	OVERLAPPED_EX read_overlapped_;
	OVERLAPPED_EX write_overlapped_;

	BYTE read_buffer_[MAX_BUFFER_LENGTH];

	SOCKET socket_;
};
