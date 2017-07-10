#include "stdafx.h"
#include "EventSelect.h"

EventSelect::EventSelect() : socket_(NULL), select_event_handle_(NULL), destroy_event_handle_(NULL)
{
}

EventSelect::~EventSelect()
{
	Finalize();
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

	destroy_event_handle_ = ::WSACreateEvent();
	if (destroy_event_handle_ == NULL)
		return false;

	DWORD result = ::WSAEventSelect(socket, select_event_handle_, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (result == SOCKET_ERROR)
		return false;

	return true;
}

void EventSelect::DoWork(void *param)
{
	bool *running = (bool*)param;

	WSANETWORKEVENTS	network_events;
	DWORD				event_id = 0;
	DWORD				result = 0;
	HANDLE				thread_events[2] = { destroy_event_handle_, select_event_handle_ };

	while ((*running))
	{
		event_id = ::WaitForMultipleObjects(2, thread_events, false, INFINITE);
		switch (event_id)
		{
		case WAIT_OBJECT_0:
			return;
		case WAIT_OBJECT_0 + 1:
			event_id = ::WSAEnumNetworkEvents(socket_, select_event_handle_, &network_events);
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

void EventSelect::EndWork()
{
	::WSASetEvent(destroy_event_handle_);
}

void EventSelect::Finalize()
{
	if (socket_)
	{
		closesocket(socket_);
		socket_ = NULL;
	}

	if (select_event_handle_)
		::CloseHandle(select_event_handle_);

	if (destroy_event_handle_)
		::CloseHandle(destroy_event_handle_);
}