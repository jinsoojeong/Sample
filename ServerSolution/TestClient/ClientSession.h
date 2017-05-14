#pragma once

class ClientSession : public ClientNet
{
public:
	ClientSession() {};
	~ClientSession() {};

	BOOL Initialize(std::string ip, WORD port);

	bool SendCheckVersionReq(const std::wstring& client_version);
	bool SendRegUserReq(const std::wstring& ticket);
	bool SendUnRegUserReq(const DWORD target_id);
	bool SendChatReq(const std::wstring& chat);

protected:
	void OnIoConnected();
	void OnIoDisconnected();
	void TestClientLog(wchar_t* words, ...);
	void MsgHandle();

private:
	void SetUserInfo(DWORD user_id, std::wstring nick);
	void NewUserConnect(std::wstring new_user_nick);
	void UserDisConnect(std::wstring user_nick);
	void Chat(std::wstring chat);
};