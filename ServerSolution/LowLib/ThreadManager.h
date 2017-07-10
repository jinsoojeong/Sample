#pragma once
#include "ThreadSync.h"
#include "Singleton.h"

class ThreadManager : public Singleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	bool AppendThread(DWORD& create_count, Task *task, DWORD stack_size, int priority, bool immediately_run = false);
	void EndWorkThread(DWORD task_id);
	void CleanUpThread(DWORD task_id);
	bool RunThread(DWORD task_id);
	bool Resume(DWORD task_id);
	bool Suspend(DWORD task_id);

private:
	typedef std::list<Thread*> THREADS;
	typedef std::map<DWORD, THREADS> TASK_THREADS;

	void Finalize();
	Thread* CreateThread(Task *task, DWORD stack_size, int priority, bool suspended = true);

	TASK_THREADS threads_;
	const DWORD max_thread_count_;
	CriticalSection sc_;
};

#define THREAD_MANAGER ThreadManager::GetInstance()
#define DELETE_THREAD_MANAGER ThreadManager::Destroy()