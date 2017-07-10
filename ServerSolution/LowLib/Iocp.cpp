#include "stdafx.h"
#include "Iocp.h"

Iocp::Iocp() : iocp_handle_(nullptr), socket_(NULL)
{
}

Iocp::~Iocp()
{
	Finalize();
}

bool Iocp::Initialize(SOCKET socket, ULONGLONG completion_key)
{
	iocp_handle_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, GetAllocThreadCount());

	if (iocp_handle_ == nullptr)
		return false;

	if (RegistSocket(socket, completion_key) == false)
		return false;

	return true;
}

bool Iocp::RegistSocket(SOCKET socket, ULONGLONG completion_key)
{
	if (!socket)
		return false;

	socket_ = socket;

	if (!completion_key)
		return false;

	iocp_handle_ = ::CreateIoCompletionPort((HANDLE)socket_, iocp_handle_, completion_key, 0);

	if (!iocp_handle_)
		return false;

	return true;
}

void Iocp::DoWork(void *param)
{
	bool *running = (bool*)param;
	BOOL success = false;
	DWORD transfered_byte_count = 0;

	ULONGLONG completion_key = NULL;

	OVERLAPPED_EX *overlapped_ex = nullptr;
	OVERLAPPED *overlapped = nullptr;

	void *object = nullptr;

	while ((*running))
	{
		success = GetQueuedCompletionStatus(iocp_handle_, &transfered_byte_count, &completion_key, &overlapped, INFINITE);

		if (!completion_key)
			break;

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

void Iocp::EndWork()
{
	::PostQueuedCompletionStatus(iocp_handle_, 0, 0, nullptr);
}

void Iocp::Finalize()
{
	if (iocp_handle_)
		::CloseHandle(iocp_handle_);
}