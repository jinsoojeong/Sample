#pragma once

class EventSelect : public ProcessObject
{
public:
	EventSelect();
	virtual ~EventSelect();

	bool Initialize(SOCKET socket);

protected:
	virtual void OnIoRead() = 0;
	virtual void OnIoWrote() = 0;
	virtual void OnIoConnected() = 0;
	virtual void OnIoDisconnected() = 0;

	void DoWork(void *param) override;
	void EndWork() override;

private:
	void Finalize();

	HANDLE			destroy_event_handle_;
	HANDLE			select_event_handle_;

	SOCKET			socket_;
};
