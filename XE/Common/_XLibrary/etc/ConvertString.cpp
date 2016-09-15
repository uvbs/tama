/*
 *  EUCKR2UTF8.mm
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 21.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "ConvertString.h"
#include "debug.h"
#include "xUtil.h"
#include "XFramework/XConsoleMain.h"
#include "XLock.h"

#if defined(_AFX) || defined(_AFXDLL)
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
#endif

static char _buffEuckr[ 8192 ] = {0,};		// 애플 영수증 코드가 길어서 버퍼 길이를 늘임
static WCHAR _buffUTF16[ 8192 ] = {0,};
static char _buffUTF8[ 8192 ] = {0,};
//static XLock CONSOLE_MAIN->GetlockString();


#ifdef _SERVER
#pragma message("_SERVER build ===================================================")

#endif // _SERVER

// UTF-16문자열을 EUCKR로 변환해서 새로 메모리를 할당받아 돌려준다
char* _CreateConvertUTF16ToEUCKR( LPCWSTR utf16 )
{
	XLockObj autoLock(CONSOLE_MAIN->GetlockString(),__TFUNC__);
#ifdef WIN32
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
	XBREAK( utf16 == NULL );		// 이건 널이면 안됨
	if( XE::IsEmpty( utf16 ) ) {		// 버퍼가 널이거나 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
//		XCLEAR_ARRAY( _buffEuckr );
//		return _buffEuckr;
		return "";
	}
	int lenMultiByte = WideCharToMultiByte( CP_ACP, 0, utf16, -1, NULL, 0, NULL, NULL );			// 멀티바이트로 변환될때 필요한 버퍼크기를 얻는다. 널포함해서 얻어진다
	XBREAK( lenMultiByte <= 0 );
	lenMultiByte ++;			// WideCharToMultiByte는 널포함 길이를 알려주긴하던데 반대는 아닌경우가 있어서 안전을 위해서 한바이트 더 받음
	char *szChar = new char[ lenMultiByte ];
	if( WideCharToMultiByte( CP_ACP, 0, utf16, -1, szChar, lenMultiByte, NULL, NULL ) == 0 ) {
		XBREAKF( 1, "%s", _T("WideCharToMultiByte return 0") );
	} else {
		szChar[ lenMultiByte ] = 0;
	}
	return szChar;
#endif // win32
#ifdef _VER_IOS
#error "do not this file. use ConvertString.mm"
#endif
#ifdef _VER_ANDROID
    return NULL;
#endif // android
}

char* _ConvertUTF16ToEUCKR( char *buff, int lenBuff, LPCWSTR utf16 )
{
	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
#ifdef WIN32
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
	if( XE::IsEmpty( utf16 ) ) {		// 버퍼가 널이거나 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
//		memset( buff, 0, lenBuff );
		buff[0] = 0;
		return buff;
	}
	XBREAK( buff == NULL );
	XBREAK( lenBuff <= 1 );		// 버퍼크기가 지나치게 작은 경우.
	int len = WideCharToMultiByte( CP_ACP, 0, utf16, -1, buff, lenBuff, NULL, NULL );
	if( len == 0 ) {
		XBREAKF( 1, "WideCharToMultiByte return 0" );
	} else {
		buff[ len ] = 0;
	}
	return buff;
#endif
#ifdef _VER_ANDROID
    return NULL;
#endif // android
}
char* _ConvertUTF16ToEUCKR( LPCWSTR utf16 )
{
// 	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
	return _ConvertUTF16ToEUCKR( _buffEuckr, utf16 );
}

// EUCKR -> UTF16
WCHAR* _CreateConvertEUCKRToUTF16( const char *euckr )
{
	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
#ifdef WIN32
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
	XBREAK( euckr == NULL );
	if( XE::IsEmpty( euckr ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
// 		XCLEAR_ARRAY( _buffUTF16 );
		return L"";
	}
	int lenEuckr = strlen(euckr);
	int lenUnicode = MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, NULL, 0 );
	XBREAK( lenUnicode <= 0 );
	lenUnicode ++;			// MultiByteToWideChar는 널을 뺀 길이를 돌려주길래 하나더 받음
	WCHAR *szwChar = new WCHAR[ lenUnicode ];
	if( MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, szwChar, lenUnicode ) == 0 ) {
		XBREAKF( 1, "MultiByteToWideChar return 0" );
	} else {
		szwChar[ lenUnicode ] = 0;
	}
	return szwChar;
#endif
#ifdef _VER_ANDROID
    return NULL;
#endif // android
}

WCHAR* _ConvertEUCKRToUTF16( WCHAR *wBuff, int lenBuff, const char *euckr )
{
	if( euckr == NULL )
		return _T("");
#ifdef WIN32
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
//	XBREAK( euckr == NULL );
	XBREAK( wBuff == NULL );
	XBREAK( lenBuff <= 1 );		// 버퍼크기가 지나치게 작은 경우.
	if( XE::IsEmpty( euckr ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
//		memset( wBuff, 0, lenBuff * sizeof(WCHAR) );
		wBuff[0] = 0;
		return wBuff;
	}
	int lenEuckr = strlen(euckr);
//	memset( wBuff, 0, sizeof(WCHAR) * lenBuff );
	auto len = MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, wBuff, lenBuff );
	if( len == 0 ) {
		XBREAKF( 1, "MultiByteToWideChar return 0" );
		wBuff[0] = 0;
	} else {
		wBuff[ len ] = 0;
	}

	return wBuff;
#endif 
#ifdef _VER_ANDROID
    return NULL;
#endif // android
    
}
WCHAR* _ConvertEUCKRToUTF16( const char *euckr )
{
	if( CONSOLE_MAIN ) {
		XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
		_buffUTF16[0] = 0;
		return _ConvertEUCKRToUTF16( _buffUTF16, euckr );
	} else {
		_buffUTF16[0] = 0;
		return _ConvertEUCKRToUTF16( _buffUTF16, euckr );
	}
}

WCHAR* ConvertUTF8ToUTF16( const char *cUTF8 )
{
	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
	_buffUTF16[ 0 ] = 0;
	return ConvertUTF8ToUTF16( _buffUTF16, 1024, cUTF8 );
}

WCHAR* ConvertUTF8ToUTF16( WCHAR *wBuff, int lenBuff, const char *cUTF8 ) 
{
if( cUTF8 == nullptr )
		return L"";
#ifdef WIN32
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
//	CStringW utf16;
	wBuff[0] = 0;
	int len = MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, NULL, 0);   
	if (len>1) {       
		if( XBREAK( len >= lenBuff ) ) {
			wcscpy_s( wBuff, lenBuff, L"err");
			return wBuff;
		}
//		memset( wBuff, 0, sizeof(WCHAR) * lenBuff );
		wchar_t *ptr = wBuff;
		if (ptr) 
			MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, ptr, len);      
		wBuff[ len ] = 0;
	}
	return wBuff;
#endif
#ifdef _VER_ANDROID
    return NULL;
#endif // android
    
}


#if defined(_AFX) || defined(_AFXDLL)
/*
CStringA UTF16toUTF8(const CStringW& utf16)
{   
	CStringA utf8;   
	int len = WideCharToMultiByte(CP_ACP, 0, utf16, -1, NULL, 0, 0, 0);   
	if (len>1)   
	{       
		char *ptr = utf8.GetBuffer(len-1);   
		if (ptr) WideCharToMultiByte(CP_ACP, 0, utf16, -1, ptr, len, 0, 0);      
		utf8.ReleaseBuffer();   
	}   
	return utf8;
} 
*/
#endif // afx

#ifdef WIN32

// utf8을 담을 버퍼를 내부에서 할당받아 되돌려 주는 방식
const char* CreateConvertUTF16toUTF8(LPCWSTR szUtf16)
{   
	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
	if( XBREAK( szUtf16 == NULL ) )
		return NULL;
	int len = WideCharToMultiByte(CP_UTF8, 0, szUtf16, -1, NULL, 0, 0, 0);   
	if (len>1)   
	{       
		char *utf8 = new char[ len+1 ];
		WideCharToMultiByte(CP_UTF8, 0, szUtf16, -1, utf8, len, 0, 0);
		utf8[len] = 0;
		return utf8;
	}   
	return NULL;
} 

// utf8을 담을 버퍼를 외부에서 주는 방식
const char* ConvertUTF16toUTF8( LPCWSTR szUtf16, char *pOutBuff, int sizeOutBuff )
{   
// #if _DEV_LEVEL <= DLV_LOCAL
// 	ID idThread = ::GetCurrentThreadId();
// 	XBREAK( CONSOLE_MAIN->GetidThreadMain() != idThread );	// 메인스레드외에서 호출되었는지
// #endif
	if( XBREAK( szUtf16 == NULL ) )
		return NULL;
	int len = WideCharToMultiByte(CP_UTF8, 0, szUtf16, -1, NULL, 0, 0, 0);   
	if (len>1)   
	{       
		if( XBREAKF( len+1 > sizeOutBuff, "utf8 buff too small" ) )
			return NULL;
		WideCharToMultiByte(CP_UTF8, 0, szUtf16, -1, pOutBuff, len, 0, 0);
		pOutBuff[len] = 0;
		return pOutBuff;
	}   
	return NULL;
} 

const char* ConvertUTF8ToUTF16( LPCWSTR utf16 ) 
{
	XLockObj autoLock( CONSOLE_MAIN->GetlockString(), __TFUNC__ );
	return ConvertUTF16toUTF8( utf16, _buffUTF8 );
}

int Utf8ToUnicode( const char *utf8, WCHAR *utf16Buff )
{
	int p = 0;
	int q = 0;
	WCHAR d;
	while( utf8[ p ] ) {
		if( ( utf8[ p ] & 0xE0 ) == 0xE0 ) {
			d = ( ( utf8[ p ] & 0x0f ) << 12 ) | ( ( utf8[ p + 1 ] & 0x3F ) << 6 ) | ( utf8[ p + 2 ] & 0x3F );
			p += 3;
		}
		else if( ( utf8[ p ] & 0xC0 ) == 0xC0 ) {
			d = ( ( utf8[ p ] & 0x1F ) << 6 ) | ( utf8[ p + 1 ] & 0x3F );
			p += 2;
		}
		else {
			d = utf8[ p ] & 0x7F;
			p++;
		}
		utf16Buff[ q ] = d;
		q++;
	}
	utf16Buff[ q ] = NULL;
	return q;
}

// win32
#else 
// not win32
#endif 
