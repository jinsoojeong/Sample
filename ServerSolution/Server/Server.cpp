#include "stdafx.h"
#include "ChatServer.h"
#include "MainProcess.h"

BYTE server_app_start();
bool run_server_iocp();
bool run_main_process();

int _tmain(int argc, _TCHAR* argv[])
{
	RegistUnHandledExceptionFilter();

	__try
	{
		if (server_app_start() == false)
			return -1;
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
		Log::ReportLog(L"Detected UnHandled Exception - exeption_code:[%d]", GetExceptionCode());
		return -2;
	}

	return 0;
}

BYTE server_app_start()
{
	Log::ReportLog(L"Server Start");

	BYTE error_code = 0;
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	Log::ReportLog(L"Server Iocp Start");

	if (run_server_iocp() == false)
	{
		error_code = 1;
		goto FINALIZE;
	}
		
	Log::ReportLog(L"Main Process Start");

	if (run_main_process() == false)
	{
		error_code = 2;
		goto FINALIZE;
	}

	getchar();

	goto FINALIZE;

FINALIZE:
	DELETE_MAIN_PROCESS;
	DELETE_CHAT_SERVER;
	DELETE_TASK_MANAGER;

	WSACleanup();

	return error_code;
}

bool run_server_iocp()
{
	// ProcessObject를 상속 받은 IOCP 태스크 생성, 생성되는 스레드 0 = 물리코어 * 2
	if (TASK_MANAGER.CreateTask(TASK_IOCP, 0, &CHAT_SERVER, 0, 0) == false)
		return false;

	// IOCP 태스트 시작
	if (TASK_MANAGER.StartTaskJob(TASK_IOCP) == false)
		return false;

	return true;
}

bool run_main_process()
{
	// ProcessObject를 상속 받은 MainPorcess 태스크 생성, 생성되는 스레드 1
	if (TASK_MANAGER.CreateTask(TASK_MAIN_PROCESS, 1, &MAIN_PROCESS, 0, 0) == false)
		return false;
	
	// MainProcess 태스트 시작
	if (TASK_MANAGER.StartTaskJob(TASK_MAIN_PROCESS) == false)
		return false;

	return true;
}