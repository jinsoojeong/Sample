#pragma once

// NetworkPacket을 상속받은 연결 관리 Session
class Session : public NetworkPacket
{
public:
	Session();
	virtual ~Session();

	bool Restart(SOCKET listen_socket);

	bool GetIsConnected();
	
	void ConnectProc(SOCKET listen_socket);
	void DisConnectProc(SOCKET listen_socket);

	void MsgHandle(Msg* msg);

private:
	bool is_connected_;
	DWORD user_id_;

	void SetUserID(DWORD user_id);
	DWORD GetUserID();

	void SetIsConnected(bool is_connected);
	
	void PROC_PT_CHECK_VERSION_REQ(Msg* msg);
	void PROC_PT_REG_USER_REQ(Msg* msg);
	void PROC_PT_UNREG_USER_REQ(Msg* msg);
	void PROC_PT_CHAT_REQ(Msg* msg);
};
