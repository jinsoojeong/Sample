#pragma once

class Iocp
{
public:
	Iocp();
	virtual ~Iocp();

	bool Initialize(SOCKET socket, ULONGLONG completion_key);
	bool RegistSocket(SOCKET socket, ULONGLONG completionKey);
	void IOCPCallback();

protected:
	virtual void OnIoRead(void *object, DWORD dataLength) = 0;
	virtual void OnIoWrote(void *object, DWORD dataLength) = 0;
	virtual void OnIoConnected(void *object) = 0;
	virtual void OnIoDisconnected(void *object) = 0;

private:
	typedef std::vector<HANDLE> WORKER_THREADS;
	WORKER_THREADS worker_threads_;

	HANDLE iocp_handle_;
	HANDLE startup_event_handle_;
	DWORD worker_thread_count_;
};

