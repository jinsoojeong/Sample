// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
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
#include <concurrent_queue.h>

#define WIN32_LEAN_AND_MEAN
#define MAX_BUFFER_LENGTH	4096
#define MAX_CONFIG_BUFFER	16
#define SAFE_DELETE(x) { delete x; x = nullptr; }

#include "DataStruct.h"
#include "Log.h"

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