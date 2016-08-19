#include "stdafx.h"
#include "Alert.h"

#if defined(_LE) || defined(_XCONSOLE)
	#ifdef _SE
	#include "ConsoleView.h"
	#endif
	#if !defined(_SE) && defined(_XCONSOLE)
	#include "XDlgConsole.h"
	#endif
#endif

int XAlertString( int type, LPCTSTR szString )
{
	// XALERT_YESNO매크로에서 이함수를 사용하므로 여기다 브레이크포인트 넣으면 안되는데..
	// XAlert계열 함수는 에러상황이 아닌 일반적인 알림창으로 쓰이므로 브레이크 포인트 넣지 말것.
	int retv = AfxMessageBox( szString, type );
#ifdef _XCONSOLE
	::XConsole( szString );		// 툴에서는 콘솔뷰로도 보낸다
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
