#pragma once

class ProcessObject
{
public:
	ProcessObject() : alloc_thread_count_(0) {};
	virtual ~ProcessObject() {};

	virtual bool Init() = 0;
	virtual void DoWork(void* param) = 0;
	virtual void EndWork() = 0;

	void SetAllocThreadCount(DWORD thread_count) 
	{
		DWORD& alloc_thread_count = const_cast<DWORD&>(alloc_thread_count_);
		alloc_thread_count = thread_count;
	};

	DWORD GetAllocThreadCount() { return alloc_thread_count_; };

private:
	const DWORD alloc_thread_count_;

};