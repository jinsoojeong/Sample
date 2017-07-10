#pragma once
#include "ThreadSync.h"

template <class T, int ALLOC_BLOCK_SIZE = 50>
class MemoryPool
{
public:
	MemoryPool() {}
	virtual ~MemoryPool() {}

	static void* operator new(std::size_t alloc_length)
	{
		SCOPED_SINGLE_LOCK(&sc);

		assert(sizeof(T) == alloc_length);
		assert(sizeof(T) >= sizeof(BYTE*));

		if (!free_pointer)
			AllocBlock();

		BYTE *return_pointer = free_pointer;
		free_pointer = *reinterpret_cast<BYTE**>(return_pointer);

		return return_pointer;
	}

	static void	operator delete(void* delete_pointer)
	{
		SCOPED_SINGLE_LOCK(&sc);

		*reinterpret_cast<BYTE**>(delete_pointer) = free_pointer;
		free_pointer = static_cast<BYTE*>(delete_pointer);
	}

private:
	static void	AllocBlock()
	{
		SCOPED_SINGLE_LOCK(&sc);

		free_pointer = new BYTE[sizeof(T) * ALLOC_BLOCK_SIZE];

		BYTE **current = reinterpret_cast<BYTE **>(free_pointer);
		BYTE *next = free_pointer;

		for (int i = 0;i<ALLOC_BLOCK_SIZE - 1;++i)
		{
			next += sizeof(T);
			*current = next;
			current = reinterpret_cast<BYTE**>(next);
		}

		*current = 0;
	}

	static BYTE *free_pointer;
	static CriticalSection sc;
};

template <class T, int ALLOC_BLOCK_SIZE>
BYTE* MemoryPool<T, ALLOC_BLOCK_SIZE>::free_pointer;

template <class T, int ALLOC_BLOCK_SIZE>
CriticalSection MemoryPool<T, ALLOC_BLOCK_SIZE>::sc;
