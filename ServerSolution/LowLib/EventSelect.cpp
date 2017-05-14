#include "stdafx.h"
#include "EventSelect.h"

unsigned __stdcall EventSelectCallback(LPVOID parameter)
{
	EventSelect *Owner = (EventSelect*)parameter;
	Owner->EventSelectCallback();

	return 0;
}

EventSelect::EventSelect()
{
	select_event_handle_ = nullptr;
	destroy_event_handle_ = nullptr;
	select_thread_handle_ = nullptr;
	startup_event_handle_ = nullptr;

	socket_ = NULL;
}

EventSelect::~EventSelect()
{
	if (socket_)
	{
		closesocket(socket_);
		socket_ = NULL;
	}

	if (select_thread_handle_)
	{
		::SetEvent(destroy_event_handle_);

		::WaitForSingleObject(select_thread_handle_, INFINITE);
		::CloseHandle(select_thread_handle_);
	}

	if (select_event_handle_)
		::CloseHandle(select_event_handle_);

	if (destroy_event_handle_)
		::CloseHandle(destroy_event_handle_);

	if (startup_event_handle_)
		::CloseHandle(startup_event_handle_);
}

bool EventSelect::Initialize(SOCKET socket)
{
	if (socket == NULL)
		return false;

	if (socket_)
		return false;

	socket_ = socket;

	select_event_handle_ = ::WSACreateEvent();
	if (select_event_handle_ == WSA_INVALID_EVENT)
		return false;

	destroy_event_handle_ = ::CreateEvent(0, false, false, 0);
	if (destroy_event_handle_ == nullptr)
		return false;

	startup_event_handle_ = ::CreateEvent(0, false, false, 0);
	if (startup_event_handle_ == nullptr)
		return false;

	DWORD result = WSAEventSelect(socket, select_event_handle_, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (result == SOCKET_ERROR)
		return false;

	UINT threadid = 0;
	select_thread_handle_ = (HANDLE)::_beginthreadex(nullptr, 0, ::EventSelectCallback, this, 0, &threadid);

	if (select_thread_handle_ == nullptr)
		return false;

	::WaitForSingleObject(startup_event_handle_, INFINITE);

	return true;
}

void EventSelect::EventSelectCallback()
{
	WSANETWORKEVENTS	network_events;
	DWORD				event_id = 0;
	DWORD				result = 0;
	HANDLE				thread_events[2] = { destroy_event_handle_, select_event_handle_ };

	while (true)
	{
		SetEvent(startup_event_handle_);

		event_id = ::WaitForMultipleObjects(2, thread_events, false, INFINITE);
		switch (event_id)
		{
		case WAIT_OBJECT_0:
			return;
		case WAIT_OBJECT_0 + 1:
			event_id = WSAEnumNetworkEvents(socket_, select_event_handle_, &network_events);
			if (event_id == SOCKET_ERROR)
				return;

			if (network_events.lNetworkEvents & FD_CONNECT)
			{
				OnIoConnected();
				if (network_events.iErrorCode[FD_CONNECT_BIT])
					return;
			}
			else if (network_events.lNetworkEvents & FD_WRITE)
			{
				OnIoWrote();
			}
			else if (network_events.lNetworkEvents & FD_READ)
			{
				OnIoRead();
			}
			else if (network_events.lNetworkEvents & FD_CLOSE)
			{
				OnIoDisconnected();
				return;
			}
			break;
		default:
			return;
		}
	}
}

