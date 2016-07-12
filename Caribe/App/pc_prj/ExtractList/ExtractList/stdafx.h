// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifdef NDEBUG
#ifdef _DEBUG
#undef _DEBUG
#endif
#endif

#include "windows.h"
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


/*
#define _XDEBUG
#define _XPACKAGE
#define _NO_QUEST
#define _XNEW_RES
*/
// 콘솔 툴
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "../../../game_src/VersionCommon.h"
#define _NO_QUEST
#include "xLibConsole.h"

//#include "xph.h"

#define DEBUG_NEW	new
