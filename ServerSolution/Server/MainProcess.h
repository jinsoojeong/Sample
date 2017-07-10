#pragma once

class MainProcess : public ProcessObject, public Singleton<MainProcess>
{
public:
	MainProcess();
	~MainProcess();

	void PushMsgQueue(Msg* msg);
	
protected:
	bool Init() override;
	void DoWork(void* param) override;
	void EndWork() override;

private:
	void PROC_PT_SESSION_CONNECT(Session* session, Msg* msg);
	void PROC_PT_SESSION_DISCONNECT(Session* session, Msg* msg);
	void PROC_PT_KEEP_ALIVE(Session* session, Msg* msg);
	void PROC_PT_CHECK_VERSION_REQ(Session* session, Msg* msg);
	void PROC_PT_REG_USER_REQ(Session* session, Msg* msg);
	void PROC_PT_UNREG_USER_REQ(Session* session, Msg* msg);

	void Update(ULONGLONG current_tick);
	void MsgHandle(Msg *msg);
	bool PopMsgQueue(Msg*& msg);
	std::wstring GetModuleVersion() { return module_version_; }

	ULONGLONG keep_alive_tick_;
	std::wstring module_version_;
	Concurrency::concurrent_queue<Msg*> main_msg_queue_;
};

#define MAIN_PROCESS MainProcess::GetInstance()
#define DELETE_MAIN_PROCESS MainProcess::Destroy()