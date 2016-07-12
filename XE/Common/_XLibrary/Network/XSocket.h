#pragma once

// 네트워크 연결 체크
#define _XCHECK_CONNECT( ID_TEXT ) \
	if( IsDisconnected() ) { \
		int _idText = ID_TEXT; \
		if( _idText == 0 )	_idText = 6; \
		XWND_ALERT( "%s", XTEXT(_idText) ); \
		return FALSE; \
	}

#ifdef _VER_IOS
  #ifdef _XBSD_SOCKET
	#include "Network/android/XBSDSocketClient.h"
	#define XSOCKET	XBSDSocketClient
  #else
	#include "Network/iOS/XCFSocketClient.h"
	#define XSOCKET	XCFSocketClient
  #endif 
#endif
#ifdef WIN32
	#include "Network/win32/XWinSocketClient.h"
	#define XSOCKET	XEWinSocketClient
#endif
#ifdef _VER_ANDROID
	#include "Network/android/XBSDSocketClient.h"
	#define XSOCKET	XBSDSocketClient
#endif
