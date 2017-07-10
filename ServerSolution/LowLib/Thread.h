#pragma once
#include "Task.h"

struct THREAD_PARAM
{
	Task *task;
	HANDLE event;
	void *is_running;
};

class Thread
{
public:
	Thread();
	~Thread();

	bool Create(Task *task, DWORD stack_size, int priority, bool suspended);

	HANDLE GetHandle() { return thread_handle_; };
	HANDLE GetEventHandle() { return event_; }
	DWORD GetID() { return thread_id_; };

	bool IsRunning() { return is_running_; };
	void SetIsRunning(bool is_running) { is_running_ = is_running; };

	void Run();
	void End();
	bool Resume();
	bool Suspended();

private:
	static unsigned __stdcall WorkerThreadFunction(LPVOID param)
	{
		Task* task = (*((THREAD_PARAM*)param)).task;
		if (task == nullptr)
			return -1;

		if (task->DoWork((*((THREAD_PARAM*)param)).is_running) == false)
			return -2;

		return 0;
	}

	THREAD_PARAM thread_param_;

	HANDLE event_;
	HANDLE thread_handle_;
	DWORD thread_id_;
	bool is_running_;
};