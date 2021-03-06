#include "stdafx.h"
#include "ThreadSync.h"
#include "SockSystem.h"

SockSystem::SockSystem() : socket_(NULL)
{
	memset(&accept_overlapped_, 0, sizeof(accept_overlapped_));
	memset(&read_overlapped_, 0, sizeof(read_overlapped_));
	memset(&write_overlapped_, 0, sizeof(write_overlapped_));

	memset(read_buffer_, 0, sizeof(read_buffer_));

	accept_overlapped_.io_type = IO_ACCEPT;
	read_overlapped_.io_type = IO_READ;
	write_overlapped_.io_type = IO_WRITE;

	accept_overlapped_.obejct = nullptr;
	read_overlapped_.obejct = nullptr;
	write_overlapped_.obejct = nullptr;
}

SockSystem::~SockSystem()
{
	if (socket_)
	{
		closesocket(socket_);
		socket_ = NULL;
	}
}

bool SockSystem::Initialize(void *object, WORD port, INT back_log)
{
	if (socket_)
		socket_ = NULL;

	memset(read_buffer_, 0, sizeof(read_buffer_));

	accept_overlapped_.obejct = object;
	read_overlapped_.obejct = object;
	write_overlapped_.obejct = object;

	if (port != 0 && back_log != 0)
	{
		if (Create() == false)
			return false;

		if (Listen(port, back_log) == false)
			return false;
	}

	return true;
}

bool SockSystem::Create()
{
	if (socket_)
		return false;

	socket_ = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket_ == INVALID_SOCKET)
		return false;

	//bool no_delay = TRUE;
	//setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&no_delay, sizeof(no_delay));

	return true;
}

void SockSystem::ResetSocket()
{
	if (socket_)
	{
		closesocket(socket_);
		socket_ = NULL;
	}
}

bool SockSystem::Listen(WORD port, INT back_log)
{
	if (port <= 0 || back_log <= 0)
		return false;

	if (socket == NULL)
		return false;

	SOCKADDR_IN listen_socket_info;

	listen_socket_info.sin_family = AF_INET;
	listen_socket_info.sin_port = htons(port);
	listen_socket_info.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(socket_, (struct sockaddr*) &listen_socket_info, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		return false;

	if (listen(socket_, back_log) == SOCKET_ERROR)
		return false;

	LINGER Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0;

	if (::setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(LINGER)) == SOCKET_ERROR)
		return false;

	return true;
}

bool SockSystem::Accept(SOCKET listen_socket)
{
	if (listen_socket == NULL)
		return false;

	if (socket_)
		return false;

	socket_ = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket_ == INVALID_SOCKET)
		return false;

	//BOOL NoDelay = TRUE;
	//setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

	if (!AcceptEx(listen_socket, socket_, read_buffer_, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_overlapped_.overlapped))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING && ::WSAGetLastError() != WSAEWOULDBLOCK)
			return false;
	}

	return true;
}

bool SockSystem::Connect(const std::string& ip, WORD port)
{
	if (ip.empty() || port <= 0)
		return false;

	if (socket_ == NULL)
		return false;

	SOCKADDR_IN address_info;

	address_info.sin_family = AF_INET;
	address_info.sin_port = htons(port);
	address_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (::WSAConnect(socket_, (LPSOCKADDR)&address_info, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		if (::WSAGetLastError() != WSAEWOULDBLOCK)
			return false;
	}

	return true;
}

bool SockSystem::RecvByIocp(BYTE *data, DWORD &data_length)
{
	if (socket_ == NULL)
		return false;

	if (!data || data_length <= 0)
		return false;

	memcpy(data, read_buffer_, data_length);

	return true;
}

bool SockSystem::RecvByEventSelect(BYTE *data, DWORD &data_length)
{
	if (socket_ == NULL)
		return false;

	if (data == nullptr)
		return false;

	WSABUF wsa_buf;
	DWORD read_bytes = 0;
	DWORD read_flag = 0;

	wsa_buf.buf = (char*)read_buffer_;
	wsa_buf.len = MAX_BUFFER_LENGTH;

	int result = ::WSARecv(socket_, &wsa_buf, 1, &read_bytes, &read_flag, &read_overlapped_.overlapped, NULL);

	if (result == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING && ::WSAGetLastError() != WSAEWOULDBLOCK)
		return false;

	memcpy(data, read_buffer_, read_bytes);
	data_length = read_bytes;

	return true;
}

bool SockSystem::RecvWait()
{
	if (socket_ == NULL)
		return false;

	WSABUF	wsa_buf;
	DWORD	read_bytes = 0;
	DWORD	read_flag = 0;

	wsa_buf.buf = (char*)read_buffer_;
	wsa_buf.len = MAX_BUFFER_LENGTH;

	int result = WSARecv(socket_, &wsa_buf, 1, &read_bytes, &read_flag, &read_overlapped_.overlapped, NULL);

	if (result == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING && ::WSAGetLastError() != WSAEWOULDBLOCK)
		return false;

	return true;
}

bool SockSystem::Send(const BYTE *data, DWORD data_length)
{
	if (socket_ == NULL)
		return false;

	if (!data || data_length <= 0)
		return false;

	WSABUF WsaBuf;
	DWORD WriteBytes = 0;
	DWORD WriteFlag = 0;

	WsaBuf.buf = (char*)data;
	WsaBuf.len = data_length;

	INT result = WSASend(socket_, &WsaBuf, 1, &WriteBytes, WriteFlag, &write_overlapped_.overlapped, NULL);

	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		return false;

	return true;
}

SOCKET SockSystem::GetSocket()
{
	return socket_;
}

bool SockSystem::GetIP(std::string& ip)
{
	if (socket_ == NULL)
		return false;

	char host_name[256] = { 0, };
	gethostname(host_name, sizeof(host_name));

	PHOSTENT host = gethostbyname(host_name);

	ip = inet_ntoa(*(struct in_addr*)*host->h_addr_list);

	return false;
}

WORD SockSystem::GetPort()
{
	if (socket_ == NULL)
		return 0;

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));

	int addr_size = sizeof(addr);
	if (getsockname(socket_, (sockaddr*)&addr, &addr_size) != SOCKET_ERROR)
		return ntohs(addr.sin_port);

	return 0;
}