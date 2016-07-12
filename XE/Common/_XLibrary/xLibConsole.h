#pragma once

/**
 콘솔툴 개발용 XE 프레임워크
*/
#define _CONSOLE

#include "XLib.h"


// #ifdef _DEBUG
// #pragma comment( lib, "../../XE/bin/Debug/xe_console.lib" )
// #else
// #pragma comment( lib, "../../XE/bin/Release/xe_console.lib" )
// #endif

#ifdef WIN32
	#ifdef _DEBUG
	#pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/xzlibd.lib" )
	#else
	#pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/xzlib.lib" )
	#endif
#endif
