#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager() : max_thread_count_(0)
{
	SYSTEM_INFO system_info;
	::GetSystemInfo(&system_info);

	DWORD& max_thread_count = const_cast<DWORD&>(max_thread_count_);

	max_thread_count = system_info.dwNumberOfProcessors * 2;
}

ThreadManager::~ThreadManager()
{
	Finalize();
}

bool ThreadManager::AppendThread(DWORD& create_count, Task* task, DWORD stack_size, int priority, bool immediately_run)
{
	SCOPED_SINGLE_LOCK(&sc_);

	if (create_count == 0 || create_count > max_thread_count_)
		create_count = max_thread_count_;

	for (DWORD i = 0; i < create_count; i++)
	{
		Thread *new_thread = nullptr;

		if (immediately_run)
			new_thread = CreateThread(task, stack_size, priority, false);
		else
			new_thread = CreateThread(task, stack_size, priority);

		if (new_thread == nullptr)
			return false;

		threads_[task->GetID()].push_back(new_thread);
	}

	return true;

}
void ThreadManager::EndWorkThread(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASK_THREADS::iterator itor = threads_.find(task_id);

	if (itor == threads_.end())
		return;

	for each (const THREADS::value_type& thread in (*itor).second)
	{
		if (thread->IsRunning() == true)
		{
			Sleep(0);
			thread->End();
		}
	}
}

void ThreadManager::CleanUpThread(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASK_THREADS::iterator threads = threads_.find(task_id);

	if (threads == threads_.end())
		return;

	for (THREADS::iterator itor = (*threads).second.begin() ; itor != (*threads).second.end() ; itor++)
	{
		WaitForSingleObject((*itor)->GetHandle(), INFINITE);

		if (CloseHandle((*itor)->GetHandle()))
			SAFE_DELETE(*itor);
	}

	(*threads).second.clear();
}

bool ThreadManager::RunThread(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASK_THREADS::iterator itor = threads_.find(task_id);

	if (itor == threads_.end())
		return false;

	for each (const THREADS::value_type& value in (*itor).second)
		value->Run();

	return true;
}

bool ThreadManager::Resume(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASK_THREADS::iterator itor = threads_.find(task_id);

	if (itor == threads_.end())
		return false;

	for each (const THREADS::value_type& value in (*itor).second)
		value->Resume();

	return true;
}

bool ThreadManager::Suspend(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASK_THREADS::iterator itor = threads_.find(task_id);

	if (itor == threads_.end())
		return false;

	for each (const THREADS::value_type& value in (*itor).second)
		value->Suspended();

	return true;
}

Thread* ThreadManager::CreateThread(Task *task, DWORD stack_size, int priority, bool suspended)
{
	SCOPED_SINGLE_LOCK(&sc_);

	Thread *thread = new Thread;

	if (thread->Create(task, stack_size, priority, suspended) == false)
	{
		SAFE_DELETE(thread);
		return nullptr;
	}

	return thread;
}

void ThreadManager::Finalize()
{
	if (threads_.empty())
		return;

	for each (const TASK_THREADS::value_type& task_thread in threads_)
		CleanUpThread(task_thread.first);

	threads_.clear();
}