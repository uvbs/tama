
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����









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
// DelegateConnection()�� �ѱ� �Ķ���� ����
enum xtConnectParam {
	xCP_NONE,
	xCP_LOGIN_UUID,		// UUID�� �α����� ����
	xCP_LOGIN_IDPW,		// ID/PW�� �α����� ����
	xCP_LOGIN_FACEBOOK,	// facebook���� �α���
	xCP_LOGIN_NEW_ACC,	// ������ ���� ��û
	xCP_LOGIN_NEW_ACC_BY_FACEBOOK,	// ������ ���� ��û
	//		xCP_LOGIN_FACEBOOK,
};

// ���� ����
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
