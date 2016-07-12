// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

//#include "targetver.h"
#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.

#ifdef NDEBUG
#ifdef _DEBUG
#undef _DEBUG
#endif
#endif

#include "windows.h"
//#include "targetver.h"
#include "tchar.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#define _XBOT2		

// afx를 안쓰면 DEBUG_NEW를 못쓰므로 XE를 LIB로 만들어 쓸때는 mfc 포함을 시키는게 좋을듯 하다.
// 아니면 직접 메모리 관리자를 만들어 쓰던가.
//#define DEBUG_NEW		new
// 라이브러리 디파인이 있으면 xlibGame.h에 넣을것.
// XLibrary
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "xLibGame.h"



