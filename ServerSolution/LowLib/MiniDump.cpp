#include "stdafx.h"
#include "MiniDump.h"

typedef bool(WINAPI *MINIDUMPWRITEDUMP)(
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UsermsgParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

extern int UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exception_Info)
{
	HMODULE	dll_hangle = NULL;
	dll_hangle = LoadLibrary(_T("DBGHELP.DLL"));

	if (!dll_hangle)
		return EXCEPTION_CONTINUE_SEARCH;

	MINIDUMPWRITEDUMP dump = (MINIDUMPWRITEDUMP)GetProcAddress(dll_hangle, "MiniDumpWriteDump");

	if (dump)
	{
		wchar_t dump_path[MAX_PATH] = { 0, };
		SYSTEMTIME current_time;

		GetLocalTime(&current_time);

		_sntprintf(dump_path, MAX_PATH, _T("%d-%d-%d %d_%d_%d.dmp"), 
			current_time.wYear, current_time.wMonth, current_time.wDay, 
			current_time.wHour, current_time.wMinute, current_time.wSecond);

		HANDLE create_file = CreateFile( dump_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (create_file == INVALID_HANDLE_VALUE)
		{
			CloseHandle(create_file);
			return EXCEPTION_CONTINUE_SEARCH;
		}
			
		_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;

		MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
		MiniDumpExceptionInfo.ExceptionPointers = exception_Info;
		MiniDumpExceptionInfo.ClientPointers = NULL;

		bool result = dump(GetCurrentProcess(), GetCurrentProcessId(), create_file, MiniDumpNormal, &MiniDumpExceptionInfo, NULL, NULL);

		CloseHandle(create_file);

		if (result == false)
			return EXCEPTION_CONTINUE_SEARCH;
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	// 이미 OverFlow 상태에서 덤프를 위한 스택을 사용하면 안될 것 같다.
	// 지금으 에러 처리 해놓지만, 스레드를 새로 생성하여 덤프를 남길 수도 있을 것
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
		return EXCEPTION_EXECUTE_HANDLER;

	return UnHandledExceptionFilter(pExceptionInfo);
}

void RegistUnHandledExceptionFilter()
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	PreviousExceptionFilter = SetUnhandledExceptionFilter(TopLevelExceptionFilter);
}