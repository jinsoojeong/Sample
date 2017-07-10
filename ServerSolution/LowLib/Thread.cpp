#include "stdafx.h"
#include "Thread.h"
#include "Task.h"

Thread::Thread() : thread_handle_(NULL), thread_id_(0), is_running_(true)
{
	event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
}

Thread::~Thread()
{
}

bool Thread::Create(Task *task, DWORD stack_size, int priority, bool suspended)
{
	DWORD init_flag = 0;

	if (suspended)
	{
		init_flag = (DWORD)CREATE_SUSPENDED;
		is_running_ = false;
	}

	thread_param_.task = task;
	thread_param_.event = event_;
	thread_param_.is_running = &is_running_;

	thread_handle_ = (HANDLE)::_beginthreadex(NULL,
		0,
		&WorkerThreadFunction,
		&thread_param_,
		init_flag,
		(UINT*)&thread_id_
	);

	if (thread_handle_ == nullptr)
		return false;

	::SetThreadPriority(thread_handle_, priority);

	return true;
};

void Thread::Run()
{
	do
	{
	} while (ResumeThread(thread_handle_));

	SetIsRunning(true);
}

void Thread::End()
{
	SetIsRunning(false);
}

bool Thread::Resume()
{
	::ResumeThread(thread_handle_);
	SetIsRunning(true);
	return true;
}

bool Thread::Suspended()
{
	::SuspendThread(thread_handle_);
	SetIsRunning(false);
	return true;
}