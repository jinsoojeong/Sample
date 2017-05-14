#pragma once

class ServerIocp : public Iocp, public Singleton<ServerIocp>
{
public:
	ServerIocp();
	virtual ~ServerIocp();

	bool Initialize();
	void KeepAliveThreadCallback();

protected:
	void OnIoRead(void *object, DWORD data_length);
	void OnIoWrote(void *object, DWORD data_length);
	void OnIoConnected(void *object);
	void OnIoDisconnected(void *object);

private:
	Network *listen_;

	HANDLE keep_alive_handle_;
	HANDLE keep_alive_handle_destroy_event_;
};
