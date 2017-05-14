#include "stdafx.h"
#include "Iocp.h"

unsigned __stdcall WorkerThreadCallback(LPVOID parameter)
{
	Iocp *Owner = (Iocp*)parameter;
	Owner->IOCPCallback();

	return 0;
}

Iocp::Iocp()
{
	startup_event_handle_ = nullptr;
	iocp_handle_ = nullptr;

	worker_thread_count_ = 0;
}

Iocp::~Iocp()
{
	for each (WORKER_THREADS::value_type work_thread in worker_threads_)
	{
		::PostQueuedCompletionStatus(iocp_handle_, 0, 0, nullptr);

		::WaitForSingleObject(work_thread, INFINITE);
		::CloseHandle(work_thread);
	}

	worker_threads_.clear();

	if (iocp_handle_)
		::CloseHandle(iocp_handle_);

	if (startup_event_handle_)
		::CloseHandle(startup_event_handle_);
}

bool Iocp::Initialize(SOCKET socket, ULONGLONG completion_key)
{
	iocp_handle_ = nullptr;

	SYSTEM_INFO system_info;
	::GetSystemInfo(&system_info);

	worker_thread_count_ = system_info.dwNumberOfProcessors * 2;
	iocp_handle_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (iocp_handle_ == nullptr)
		return false;

	startup_event_handle_ = CreateEvent(0, false, false, 0);
	if (startup_event_handle_ == nullptr)
		return false;

	worker_threads_.reserve(worker_thread_count_);

	for (DWORD thread_count = 0; thread_count < worker_thread_count_; thread_count++)
	{
		HANDLE worker_thread = (HANDLE)::_beginthreadex(nullptr, 0, ::WorkerThreadCallback, this, 0, nullptr);
		worker_threads_.push_back(worker_thread);

		::WaitForSingleObject(startup_event_handle_, INFINITE);
	}

	if (RegistSocket(socket, completion_key) == false)
		return false;

	return true;
}

bool Iocp::RegistSocket(SOCKET socket, ULONGLONG completion_key)
{
	if (!socket)
		return false;

	if (!completion_key)
		return false;

	iocp_handle_ = ::CreateIoCompletionPort((HANDLE)socket, iocp_handle_, completion_key, 0);

	if (!iocp_handle_)
		return false;

	return true;
}

void Iocp::IOCPCallback()
{
	BOOL success = false;
	DWORD transfered_byte_count = 0;

	ULONGLONG completion_key = NULL;

	OVERLAPPED_EX *overlapped_ex = nullptr;
	OVERLAPPED *overlapped = nullptr;

	void *object = nullptr;

	while (true)
	{
		SetEvent(startup_event_handle_);

		success = GetQueuedCompletionStatus(iocp_handle_, &transfered_byte_count, &completion_key, &overlapped, INFINITE);

		if (!completion_key)
			return;

		overlapped_ex = (OVERLAPPED_EX*)overlapped;
		object = overlapped_ex->obejct;

		if ((success == false) || (success && transfered_byte_count == 0))
		{
			if (overlapped_ex->io_type == IO_ACCEPT)
				OnIoConnected(object);
			else
				OnIoDisconnected(object);

			continue;
		}

		switch (overlapped_ex->io_type)
		{
		case IO_READ:
			OnIoRead(object, transfered_byte_count);
			break;
		case IO_WRITE:
			OnIoWrote(object, transfered_byte_count);
			break;
		}
	}
}