#pragma once

class ChatServer : public Iocp, public Singleton<ChatServer>
{
public:
	ChatServer();
	virtual ~ChatServer();

protected:
	void OnIoRead(void *object, DWORD data_length);
	void OnIoWrote(void *object, DWORD data_length) {};
	void OnIoConnected(void *object);
	void OnIoDisconnected(void *object);
	bool Init() override;

private:
	void Finalize();

	PacketSystem *listener_;
	HANDLE keep_alive_handle_;
	HANDLE keep_alive_handle_destroy_event_;

	DWORD default_user_count_;
	IniReader ini_reader_;
};

#define CHAT_SERVER ChatServer::GetInstance()
#define DELETE_CHAT_SERVER ChatServer::Destroy()