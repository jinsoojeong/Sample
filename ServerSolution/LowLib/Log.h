#pragma once

class Log
{
public:
	#define MAX_BUFFER_LENGTH	4096

	static bool	ReportLog(wchar_t* data, ...)
	{
		va_list	va;
		wchar_t	log[MAX_BUFFER_LENGTH] = { 0, };

		va_start(va, data);
		_vstprintf(log, data, va);
		va_end(va);

		SYSTEMTIME	current_time;
		GetLocalTime(&current_time);

		wchar_t	current_date[32] = { 0, };
		_sntprintf(current_date, 32, _T("%d-%d-%d %d:%d:%d"), current_time.wYear, current_time.wMonth,
			current_time.wDay, current_time.wHour, current_time.wMinute, current_time.wSecond);

		wchar_t file_name[MAX_PATH] = { 0, };
		_sntprintf(file_name, MAX_PATH, _T("LOG_%d-%d-%d %d.log"),
			current_time.wYear, current_time.wMonth, current_time.wDay, current_time.wHour);

		FILE *file= nullptr;
		file = _tfopen(file_name, _T("a"));
		if (file == nullptr)
			return false;

		_ftprintf(file, _T("[%s] %s\n"), current_date, log);

		wchar_t debug_log[MAX_BUFFER_LENGTH] = { 0, };
		_sntprintf(debug_log, MAX_BUFFER_LENGTH, _T("[%s] %s\n"), current_date, log);

		fflush(file);

		fclose(file);

		OutputDebugString(debug_log);
		_tprintf(_T("%s"), debug_log);

		return true;
	}
};
