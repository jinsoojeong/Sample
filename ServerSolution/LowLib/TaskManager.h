#pragma once
#include "Singleton.h"

class TaskManager : public Singleton<TaskManager>
{
public:
	TaskManager();
	virtual ~TaskManager();

	bool CreateTask(DWORD task_id, DWORD create_count, ProcessObject* process_object, DWORD stack_size, int priority);
	bool StartTaskJob(DWORD task_id);
	Task* GetTask(DWORD task_id);

private:
	typedef std::map<DWORD, Task*> TASKS;

	void Finalize();
	void EndWorkTasks();
	void CleanUpTasks();

	TASKS tasks_;
	CriticalSection sc_;
};

#define TASK_MANAGER TaskManager::GetInstance()
#define DELETE_TASK_MANAGER TaskManager::Destroy()