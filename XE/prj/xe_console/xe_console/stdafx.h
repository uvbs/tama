// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <WinSock2.h>		// "windows.h" 보다 먼저 선언해야함.
#include "windows.h"
#include "targetver.h"
#include "tchar.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "xLibConsole.h"

// afx를 안쓰면 DEBUG_NEW를 못쓰므로 XE를 LIB로 만들어 쓸때는 mfc 포함을 시키는게 좋을듯 하다.
// 아니면 직접 메모리 관리자를 만들어 쓰던가.
#define DEBUG_NEW		new

// #ifdef WIN32
// 	#ifdef _DEBUG
// 	#pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/xzlibd.lib" )
// 	#else
// 	#pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/xzlib.lib" )
// 	#endif
// #endif