#pragma once

class SockSystem
{
public:
	SockSystem();
	virtual ~SockSystem();

	SOCKET GetSocket();
	bool GetIP(std::string& ip);
	WORD GetPort();

protected:
	bool Initialize(void *object, WORD port, INT back_log);

	bool Create();
	bool Accept(SOCKET listen_socket);
	bool Connect(const std::string& ip, WORD port);

	bool RecvWait();
	bool RecvByIocp(BYTE *data, DWORD &data_length);
	bool RecvByEventSelect(BYTE *data, DWORD &data_length);

	bool Send(const BYTE *data, DWORD data_length);

	void ResetSocket();

private:
	bool Listen(WORD port, INT back_log);

	OVERLAPPED_EX accept_overlapped_;
	OVERLAPPED_EX read_overlapped_;
	OVERLAPPED_EX write_overlapped_;

	BYTE read_buffer_[MAX_BUFFER_LENGTH];

	SOCKET socket_;
};
