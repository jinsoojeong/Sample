// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include <iostream>
#include <tchar.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <utility>

#include <string>
#include <algorithm>
#include <sstream>
#include <process.h>
#include <functional>
#include <mutex>

#include <concurrent_queue.h>

#include "DataStruct.h"
#include "Log.h"
#include "Thread.h"
#include "ProcessObject.h"

#define SAFE_DELETE(x) { delete x; x = nullptr; }

enum IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE
};

struct OVERLAPPED_EX
{
	OVERLAPPED overlapped;
	IO_TYPE io_type;
	void *obejct;
};

enum TASK_ID
{
	TASK_IOCP = 100,
	TASK_EVENT_SELECT,
	TASK_MAIN_PROCESS
};