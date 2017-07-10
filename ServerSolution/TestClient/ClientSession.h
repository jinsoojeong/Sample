#pragma once

class ClientSession : public ClientNet
{
public:
	ClientSession();
	~ClientSession();

	enum ClientState
	{
		InitState,
		ConnectedState,
		RunningState, // equal logout state
		LoginState
	};
	
	bool VersionCheck();
	bool UserLogin(const std::wstring ticket);
	bool UserLogOut();
	bool SendChatText(const std::wstring text);

protected:
	void TestClientLog(wchar_t* words, ...);
	void MsgHandle();

	bool Init() override;

private:
	bool SendCheckVersionReq(const std::wstring& client_version);
	bool SendRegUserReq(const std::wstring& ticket);
	bool SendUnRegUserReq(const DWORD target_id);
	bool SendChatReq(const std::wstring& chat);

	bool Connect();
	void SetUserInfo(DWORD user_id, std::wstring nick);
	void NewUserConnect(std::wstring new_user_nick);
	void UserDisConnect(std::wstring user_nick);
	void Chat(std::wstring chat);

	void SetClientState(ClientState state) { client_state_ = state; };

	std::wstring ip_;
	DWORD port_;

	DWORD connect_error_;
	std::wstring version_;
		
	std::wstring user_nick_;
	DWORD user_id_;

	ClientState client_state_;
};