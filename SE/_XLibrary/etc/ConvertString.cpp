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

#ifdef _AFX
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
#endif

static char _buffEuckr[ 1024 ] = {0,};
static TCHAR _buffUTF16[ 1024 ] = {0,};
// UTF-16문자열을 EUCKR로 변환해서 새로 메모리를 할당받아 돌려준다
char* _CreateConvertUTF16ToEUCKR( LPCWSTR utf16 )
{
//	XBREAK( utf16 == NULL );
	if( XE::IsEmpty( utf16 ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
		XCLEAR_ARRAY( _buffEuckr );
		return _buffEuckr;
	}
	int lenMultiByte = WideCharToMultiByte( CP_ACP, 0, utf16, -1, NULL, 0, NULL, NULL );			// 멀티바이트로 변환될때 필요한 버퍼크기를 얻는다. 널포함해서 얻어진다
	XBREAK( lenMultiByte <= 0 );
	lenMultiByte ++;			// WideCharToMultiByte는 널포함 길이를 알려주긴하던데 반대는 아닌경우가 있어서 안전을 위해서 한바이트 더 받음
	char *szChar = new char[ lenMultiByte ];
	if( WideCharToMultiByte( CP_ACP, 0, utf16, -1, szChar, lenMultiByte, NULL, NULL ) == 0 )
		XBREAKF( 1, "WideCharToMultiByte return 0" );
	return szChar;
}

char* _ConvertUTF16ToEUCKR( char *buff, int lenBuff, LPCWSTR utf16 )
{
//	XBREAK( utf16 == NULL );
	XBREAK( buff == NULL );
	XBREAK( lenBuff <= 1 );		// 버퍼크기가 지나치게 작은 경우.
	if( XE::IsEmpty( utf16 ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
		memset( buff, 0, lenBuff );
		return buff;
	}
	if( WideCharToMultiByte( CP_ACP, 0, utf16, -1, buff, lenBuff, NULL, NULL ) == 0 )
		XBREAKF( 1, "WideCharToMultiByte return 0" );
	return buff;
}
char* _ConvertUTF16ToEUCKR( LPCWSTR utf16 )
{
	return _ConvertUTF16ToEUCKR( _buffEuckr, utf16 );
}

// EUCKR -> UTF16
WCHAR* _CreateConvertEUCKRToUTF16( const char *euckr )
{
// 	XBREAK( euckr == NULL );
	if( XE::IsEmpty( euckr ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
		XCLEAR_ARRAY( _buffUTF16 );
		return _buffUTF16;
	}
	int lenEuckr = strlen(euckr);
	int lenUnicode = MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, NULL, 0 );
	XBREAK( lenUnicode <= 0 );
	lenUnicode ++;			// MultiByteToWideChar는 널을 뺀 길이를 돌려주길래 하나더 받음
	WCHAR *szwChar = new WCHAR[ lenUnicode ];
	if( MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, szwChar, lenUnicode ) == 0 )
		XBREAKF( 1, "MultiByteToWideChar return 0" );
	return szwChar;
}

WCHAR* _ConvertEUCKRToUTF16( WCHAR *wBuff, int lenBuff, const char *euckr )
{
// 	XBREAK( euckr == NULL );
	XBREAK( wBuff == NULL );
	XBREAK( lenBuff <= 1 );		// 버퍼크기가 지나치게 작은 경우.
	if( XE::IsEmpty( euckr ) ) {		// 버퍼가 비어있으면 클리어만 해서 리턴한다. 아래에서 에러를 내지는 않는다
		memset( wBuff, 0, lenBuff * sizeof(WCHAR) );
		return wBuff;
	}
	int lenEuckr = strlen(euckr);
	memset( wBuff, 0, sizeof(WCHAR) * lenBuff );
	if( MultiByteToWideChar( CP_ACP, 0, euckr, lenEuckr, wBuff, lenBuff ) == 0 )
		XBREAKF( 1, "MultiByteToWideChar return 0" );
	return wBuff;
}
WCHAR* _ConvertEUCKRToUTF16( const char *euckr )
{
	return _ConvertEUCKRToUTF16( _buffUTF16, euckr );
}

WCHAR* ConvertUTF8ToUTF16( const char *cUTF8 )
{
	return ConvertUTF8ToUTF16( _buffUTF16, 1024, cUTF8 );
}

WCHAR* ConvertUTF8ToUTF16( WCHAR *wBuff, int lenBuff, const char *cUTF8 ) 
{   
	CStringW utf16;   
	int len = MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, NULL, 0);   
	if (len>1)   
	{       
		memset( wBuff, 0, sizeof(WCHAR) * lenBuff );
		wchar_t *ptr = wBuff;
		if (ptr) MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, ptr, len);      
	}   
	return wBuff;
} 

#ifdef _AFX

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
#endif // afx
