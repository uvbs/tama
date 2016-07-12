
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include "xLibConsole.h"

XE_NAMESPACE_START( XGAME )
// DelegateConnection()에 넘길 파라메터 종류
enum xtConnectParam {
	xCP_NONE,
	xCP_LOGIN_UUID,		// UUID로 로그인할 예정
	xCP_LOGIN_IDPW,		// ID/PW로 로그인할 예정
	xCP_LOGIN_FACEBOOK,	// facebook으로 로그인
	xCP_LOGIN_NEW_ACC,	// 새계정 생성 요청
	xCP_LOGIN_NEW_ACC_BY_FACEBOOK,	// 새계정 생성 요청
	//		xCP_LOGIN_FACEBOOK,
};

// 접속 상태
// enum xtState {
// 	xST_NONE,
// 	xST_CONNECTING_L,
// 	xST_LOGINING_L,
// 	xST_MAKE_NEW_ACCOUNT,
// 	xST_CONNECTING_G,
// 	xST_LOGINING_G,
// 	xST_ONLINE,
// 	xST_MAX,
// };
//LPCTSTR GetstrEnumByxtState( xtState state );
//
XE_NAMESPACE_END; // XGAME
