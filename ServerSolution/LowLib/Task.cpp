#include "stdafx.h"
#include "ProcessObject.h"
#include "ThreadManager.h"
#include "Task.h"

Task::Task() : object_(nullptr), task_id_(0), is_running_(false)
{
}

Task::~Task()
{
	//CleanUp();
}

bool Task::DoWork(void *param)
{
	object_->DoWork(param);

	return true;
}

bool Task::StartJob()
{
	return THREAD_MANAGER.RunThread(GetID());
}

bool Task::Create(DWORD task_id, DWORD create_count, ProcessObject* process_object, DWORD stack_size, int priority)
{
	object_ = process_object;
	task_id_ = task_id;
	
	if (THREAD_MANAGER.AppendThread(create_count, this, stack_size, priority) == false)
		return false;

	is_running_ = true;

	object_->SetAllocThreadCount(create_count);
	object_->Init();
	
	return true;
}

void Task::EndWork()
{
	THREAD_MANAGER.EndWorkThread(GetID());

	object_->EndWork();
}

void Task::CleanUp()
{
	THREAD_MANAGER.CleanUpThread(GetID());

	object_ = nullptr;
	task_id_ = 0;
}