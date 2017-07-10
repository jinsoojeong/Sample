#pragma once

class User : public MemoryPool<User>
{
public:
	User(DWORD id, std::wstring nick);
	virtual ~User();

	void Update(ULONGLONG current_tick);

	void PushMsgQueue(Msg* msg);
	const DWORD GetID() const { return id_; }
	const std::wstring& GetNick() const { return nick_; }
	void SendMsg(Msg* msg, bool broad_cast = false) { session_->SendMsg(msg, broad_cast);  };
	void SetSession(Session* session) { session_ = session; }
	Session* GetSession() { return session_; }

private:
	void MsgHandle(Msg* msg);
	bool PopMsgQueue(Msg*& msg);

	void PROC_PT_CHAT_REQ(Msg* msg);

	DWORD id_;
	std::wstring nick_;

	Session* session_;
	Concurrency::concurrent_queue<Msg*> user_msg_queue_;
};