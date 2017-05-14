#pragma once

class EventSelect
{
public:
	EventSelect();
	virtual ~EventSelect();

	bool Initialize(SOCKET socket);
	void EventSelectCallback();

protected:
	virtual void OnIoRead() = 0;
	virtual void OnIoWrote() = 0;
	virtual void OnIoConnected() = 0;
	virtual void OnIoDisconnected() = 0;

private:
	HANDLE			select_event_handle_;
	HANDLE			startup_event_handle_;
	HANDLE			destroy_event_handle_;

	HANDLE			select_thread_handle_;

	SOCKET			socket_;
};
