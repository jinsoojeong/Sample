#pragma once

class ProcessObject;
class Task
{
public:
	Task();
	virtual ~Task();

	bool Create(DWORD task_id, DWORD create_count, ProcessObject* process_object, DWORD stack_size, int priority);
	void CleanUp();

	bool DoWork(void *param);
	void EndWork();
	bool StartJob();

	DWORD GetID() { return task_id_; };
	bool IsRunning() { return is_running_; };

private:
	typedef std::list<DWORD> THREADIDS;

	THREADIDS thread_ids_;
	ProcessObject *object_;
	DWORD task_id_;
	bool is_running_;
};