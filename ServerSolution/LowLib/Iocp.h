#pragma once

class Iocp : public ProcessObject
{
public:
	Iocp();
	virtual ~Iocp();

	bool Initialize(SOCKET socket, ULONGLONG completion_key);
	bool RegistSocket(SOCKET socket, ULONGLONG completionKey);

protected:
	virtual void OnIoRead(void *object, DWORD dataLength) = 0;
	virtual void OnIoWrote(void *object, DWORD dataLength) = 0;
	virtual void OnIoConnected(void *object) = 0;
	virtual void OnIoDisconnected(void *object) = 0;

	void DoWork(void *param) override;
	void EndWork() override;

private:
	void Finalize();

	HANDLE iocp_handle_;
	SOCKET socket_;
};

