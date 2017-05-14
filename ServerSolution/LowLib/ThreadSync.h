#pragma once

template <class T>
class ThreadSync
{
	friend class TS_CRETICAL_SECSION;
public:
	class TS_CRETICAL_SECSION
	{
	public:
		TS_CRETICAL_SECSION()
		{
			T::critical_section.Lock();
		}

		~TS_CRETICAL_SECSION()
		{
			T::critical_section.UnLock();
		}
	};

private:
	static CriticalSection critical_section;
};

template <class T>
CriticalSection ThreadSync<T>::critical_section;