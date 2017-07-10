#pragma once

class Session
{
public:
	Session(DWORD id);
	virtual ~Session();

	void Update(ULONGLONG current_tick);
	bool Initialize(SOCKET listen_socket);
	bool ReAcceptWait(SOCKET listen_socket);

	SOCKET GetSocket();
	bool RecvByIocp(DWORD data_length);

	bool GetIsConnected() { return is_connected_; };
	DWORD GetID() { return id_; };

	bool SendMsg(Msg* msg, bool broad_cast = false);
	bool GetMsg(Msg*& msg);

	bool RecvWait(SOCKET listen_socket);

private:
	friend class SessionManager;
	void SetIsConnected(bool is_connected) { is_connected_ = is_connected; };

	DWORD id_;
	bool is_connected_;
	PacketSystem *packet_system_;
};
