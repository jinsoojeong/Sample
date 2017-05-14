#pragma once

class CriticalSection
{
public:
	CriticalSection()
	{
		InitializeCriticalSection(&sc_);
	}

	~CriticalSection()
	{
		DeleteCriticalSection(&sc_);
	}

	void Lock()
	{
		EnterCriticalSection(&sc_);
	}

	void UnLock()
	{
		LeaveCriticalSection(&sc_);
	}

private:
	CRITICAL_SECTION sc_;
};