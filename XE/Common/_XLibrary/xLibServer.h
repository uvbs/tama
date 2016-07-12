
#pragma once

/**
 콘솔툴 개발용 XE 프레임워크
*/
#define _SERVER
#define _VER_DX		// 서버는 무조건 dx버전
#define _XUSE_LUA

#include "XLib.h"
#include "Network/xenDef.h"
#include "XFramework/MFC/CoolDialogBar.h"
#include "Network/XWinNetwork.h"

// 192번 서버는 사양이 너무 낮아서 무조건 low로.
#if _DEV_LEVEL == DLV_DEV_CREW
#define _XLOW_SERVER
#define _XNOT_BREAK
#endif

#ifdef _XUZHU
// #define _DEV
// #ifndef _XBOT			// bot모드에선 low사용하지 않음.
// #ifndef _XLOW_SERVER
// // xuzhu자리에선 평소엔 low로 하고 필요에 따라 넣었다 뺏다 하도록
// #define _XLOW_SERVER		// 개발중엔 시스템이 느려지지 않도록 이것을 사용
// #endif
// #endif // _XBOT
#endif

#if defined(_DEBUG) && !defined(_XBOT)
#ifndef _XLOW_SERVER
// 봇모드도 아닌 디버그 모드에선 항상 low로
#define _XLOW_SERVER
#endif
#endif


