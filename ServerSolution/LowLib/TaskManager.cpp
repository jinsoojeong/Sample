#include "stdafx.h"
#include "Task.h"
#include "ThreadSync.h"
#include "TaskManager.h"
#include "ThreadManager.h"

TaskManager::TaskManager()
{

}

TaskManager::~TaskManager()
{
	Finalize();
}

bool TaskManager::CreateTask(DWORD task_id, DWORD create_count, ProcessObject* process_object, DWORD stack_size, int priority)
{
	SCOPED_SINGLE_LOCK(&sc_);

	Task* new_task = new Task;

	if (new_task->Create(task_id, create_count, process_object, stack_size, priority) == false)
		return false;

	tasks_.insert(TASKS::value_type(task_id, new_task));

	return true;
}

bool TaskManager::StartTaskJob(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASKS::iterator itor = tasks_.find(task_id);

	if (itor == tasks_.end())
		return false;

	return (*itor).second->StartJob();
}

void TaskManager::Finalize()
{
	EndWorkTasks();
	CleanUpTasks();

	for (TASKS::iterator itor = tasks_.begin(); itor != tasks_.end(); itor++)
		SAFE_DELETE((*itor).second);

	DELETE_THREAD_MANAGER;
}

void TaskManager::EndWorkTasks()
{
	SCOPED_SINGLE_LOCK(&sc_);

	for each (const TASKS::value_type& value in tasks_)
		value.second->EndWork();
}

void TaskManager::CleanUpTasks()
{
	SCOPED_SINGLE_LOCK(&sc_);

	for each (const TASKS::value_type& value in tasks_)
		value.second->CleanUp();
}

Task* TaskManager::GetTask(DWORD task_id)
{
	SCOPED_SINGLE_LOCK(&sc_);

	TASKS::iterator itor = tasks_.find(task_id);
	if (itor == tasks_.end())
		return nullptr;

	return (*itor).second;
}