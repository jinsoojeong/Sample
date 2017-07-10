// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

// lib
#include "../LowLib/stdafx.h"

#include "../LowLib/ThreadSync.h"
#include "../LowLib/MemoryPool.h"
#include "../LowLib/Msg.h"
#include "../LowLib/SockSystem.h"
#include "../LowLib/PacketSystem.h"
#include "../LowLib/Session.h"
#include "../LowLib/SessionManager.h"
#include "../LowLib/Iocp.h"
#include "../LowLib/MiniDump.h"
#include "../LowLib/IniReader.h"
#include "../LowLib/Log.h"
#include "../LowLib/Singleton.h"
#include "../LowLib/StringFunc.h"
#include "../LowLib/TaskManager.h"

#include "../CommonFile/Protocol.h"
#include "../CommonFile/PT_ErrorCode.h"
#include "../CommonFile/PT_Structure.h"
#include "../CommonFile/PT_ReadMsg.h"

#include "ChatServer.h"