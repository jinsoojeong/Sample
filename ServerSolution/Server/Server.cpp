// Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "../LowLib/MiniDump.h"

int server_app_start(); 

int _tmain(int argc, _TCHAR* argv[])
{
	RegistUnHandledExceptionFilter();

	__try
	{
		server_app_start();
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
		Log::ReportLog(L"Detected UnHandled Exception - exeption_code:[%d]", GetExceptionCode());
		return -1;
	}

	return 0;
}

int server_app_start()
{
	HRESULT hr = S_FALSE;
	unsigned long error_line = 0;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);
	if (FAILED(hr))
	{
		Log::ReportLog(L"CoInitializeEx failed");
		return 0;
	}

	Log::ReportLog(L"Server Start");

	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	Log::ReportLog(L"Server Iocp Start");

	ServerIocp* server_iocp = new ServerIocp();

	if (server_iocp->Initialize())
		getchar();

	SAFE_DELETE(server_iocp);
	WSACleanup();
	CoUninitialize();

	return 0;
}
