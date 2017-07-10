#pragma once

class SyncObject 
{
public:
	SyncObject() {};
	virtual ~SyncObject() {};

	virtual void Lock() {};
	virtual void UnLock() {};
};

class CriticalSection : public SyncObject
{
public:
	CriticalSection() : SyncObject() 
	{ 
		InitializeCriticalSection(&sc_); 
	}
	~CriticalSection() { DeleteCriticalSection(&sc_); }

	void Lock() override { EnterCriticalSection(&sc_); }
	void UnLock() override { LeaveCriticalSection(&sc_); }

private:
	CRITICAL_SECTION sc_;
};

class ScopedLock
{
public:
	ScopedLock(SyncObject* sync_object) : sync_object_(sync_object)
	{ 
		if (sync_object_)
			sync_object_->Lock(); 
	}
	~ScopedLock() 
	{ 
		if (sync_object_)
			sync_object_->UnLock(); 
	}

private:
	SyncObject* sync_object_;
};

#define SCOPED_SINGLE_LOCK(x) ScopedLock scoped_lock(x)