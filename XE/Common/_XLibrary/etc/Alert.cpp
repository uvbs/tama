#include "stdafx.h"
#include "Alert.h"
#include "debug.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if defined(_LE) || defined(_XCONSOLE)
	#ifdef _SE
	#include "ConsoleView.h"
	#endif
	#if !defined(_SE) && defined(_XCONSOLE)
	#include "XDlgConsole.h"
	#endif
#endif

#ifdef _CLIENT
#include "XFramework/client/XClientMain.h"
static XClientMain *s_pHandler = NULL;
void SetAlertHandler( XClientMain *pHandler )
{
	s_pHandler = pHandler;
}
#endif

int XAlertString( int type, LPCTSTR szString )
{
	// XALERT_YESNO매크로에서 이함수를 사용하므로 여기다 브레이크포인트 넣으면 안되는데..
	// XAlert계열 함수는 에러상황이 아닌 일반적인 알림창으로 쓰이므로 브레이크 포인트 넣지 말것.
    int retv = 0;
#ifdef WIN32
  #ifdef _DEBUG
	retv = XSYSTEM::xMessageBox( szString, (XE::xtAlert) type );
  #else
	XLOGXN( "XALERT: %s", szString );
// 	int *p = nullptr;
// 	*p = 0;
	if( XE::bAllowAlert )
		retv = XSYSTEM::xMessageBox( szString, (XE::xtAlert) type );
  #endif
#else
    XLOG("%s", szString );
#endif
	 
#ifdef _XCONSOLE
	::XConsole( szString );		// 툴에서는 콘솔뷰로도 보낸다
#endif

#ifdef _DEBUG
	XBREAK(1);
#endif
#ifdef _CLIENT
	XBREAK( s_pHandler == NULL );
	s_pHandler->OnAlert();
#endif 
	return retv;
}
 
int XAlert( int type, LPCTSTR str, ... )
{
    TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl; 
	
    va_start(vl, str); 
    _vstprintf_s(szBuff, str, vl);
    va_end(vl);
	// 
	return XAlertString( type, szBuff );
}

int XAlert( XE::xtAlert type, LPCTSTR str, ... )
{ 
    TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    _vstprintf_s(szBuff, str, vl);
    va_end(vl);
	//
	int nType = 0;
#ifdef WIN32
	switch( type )
	{
	case XE::xMB_OK:	nType = MB_OK;	break;
	case XE::xMB_YESNO:	nType = MB_YESNO;	break;
	}
#endif
	return XAlertString( nType, szBuff );
}
