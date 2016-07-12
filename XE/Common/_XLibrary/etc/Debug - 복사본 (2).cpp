#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Debug.h"
#include "xeDef.h"
#include "XLibrary.h"
#include "XSystem.h"
#ifdef _VER_ANDROID
#include <android/log.h>
#endif
#ifdef _CLIENT
#include "XFramework/client/XClientMain.h"
#endif
#include "XLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/*
#ifndef TRACE
#ifdef WIN32
//#define TRACE __noop()
#else
#define TRACE ((void)0)
#endif
#endif // not TRACE
*/

#if defined(_DEBUG) || defined(DEBUG)
#pragma message("debug compiling....")
#else
#pragma message("release compiling....")
#endif

namespace XE
{
	BOOL bAllowAlert = TRUE;		// 메시지창 허용 여부
};

#ifdef _VER_ANDROID
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"xuzhu",__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"xuzhu",__VA_ARGS__)
#else
#define LOGD(...)   (0)
#endif // android

// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
void xPutsTimeString( FILE *fp )
{
// #ifdef WIN32
	static char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, "%d/%2d/%2d   %02d:%02d:%02d   \n", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond );
	fputs( szStr, fp );
//#endif
}

#ifdef WIN32
void xPutsTimeStringW( FILE *fp )
{
#ifdef WIN32
	static TCHAR szStr[ 8192 ];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	_stprintf_s( szStr, _T("%d/%2d/%2d   %02d:%02d:%02d   \n"), 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond );
	_fputts( szStr, fp );
#endif
}
#endif

/**
 @brief 날짜에 기반한 파일명을 만들어 낸다.
*/
const char* XE::MakeFilenameFromDay( char *pOut, int len, const char *prefix )
{
	if( prefix == NULL )
		prefix = "";
	XBREAK( len == 0 );
	SYSTEMTIME		time;
	GetLocalTime( &time );
#ifdef WIN32
	sprintf_s( pOut, len, "%s%02d%02d%02d.txt", prefix, time.wYear-2000, time.wMonth, time.wDay );
#else
	sprintf( pOut, "%s%02d%02d%02d.txt", prefix, time.wYear-2000, time.wMonth, time.wDay );
#endif
	return pOut;
}
const char* XE::MakeFilenameFromTime( char *pOut, int len, const char *prefix )
{
	if( prefix == NULL )
		prefix = "";
	XBREAK( len == 0 );
	SYSTEMTIME		time;
	GetLocalTime( &time );
//#ifdef WIN32
	sprintf_s( pOut, len, "%s%02d%02d%02d%02d.txt", 
						prefix, time.wDay, time.wHour, time.wMinute, time.wSecond );
//#else
//	sprintf( pOut, "%s%02d%02d%02d.txt", prefix, time.wYear-2000, time.wMonth, time.wDay );
//#endif
	return pOut;
}

// 날짜에 기반한 로그파일명을 만들어 낸다.
const char* MakeLogFilenameFromDay( char *pOut, int len )
{
#ifdef _SERVER
#ifndef WIN32
#error "only WIN32 platform support."
#endif
	// 실행파일의 하위에 실행파일명의 폴더를 만들어 날짜파일명을 저장한다.
	_tstring strPath = XE::GetExecDir();
	strPath += XE::GetExecFileTitle();
	strPath += _T("\\");
	char cFilename[ 256 ];
	XE::MakeFilenameFromDay( cFilename, 256, "_xse_" );	// _xse_130318.txt
	strcpy_s( pOut, len, SZ2C( strPath.c_str() ) );

#else
	// 실행파일이 있는곳에 저장한다.(스마트폰기기는 패스명이 길기때문에 폴더만들때 불리).
#endif
	return pOut;
}
#ifdef WIN32
LPCTSTR MakeLogFilenameFromDay( TCHAR *pOut, int len )
{
	char cBuff[ 1024 ];
	MakeLogFilenameFromDay( cBuff, 1024 );
	_tcscpy_s( pOut, len, C2SZ( cBuff ) );
	return pOut;
}
#endif


void XE::WriteLogToFile( const char *cFile, LPCTSTR szBuff )
{
	// euckr로 변환. 후에 tchar버전으로 바꾸자.
	char szChar[8192];
#ifdef WIN32
	memset( szChar, 0, sizeof(szChar) );
	WideCharToMultiByte(CP_ACP, 0, szBuff, -1, szChar, 1024, NULL, NULL);
//	strcat_s( szChar, "\n" );
#else
	strcpy_s( szChar, szBuff );
//	strcat_s( szChar, "\n" )
#endif
	//
	FILE *fp;
#ifdef _XTOOL
	CString str = XE::GetCwd();
	str += "error.txt";
	fopen_s( &fp, Convert_TCHAR_To_char( str ), "a+" );
#else
//	XSYSTEM::MakeDir( strPath.c_str() );		// 폴더부터 만듬.
	fopen_s( &fp, cFile, "a+" );
#endif
	if( fp ) {
#ifdef _LOGIN_SERVER
		fputs( "LoginServer: ", fp );
#endif
#ifdef _GAME_SERVER
		fputs( "GameServer: ", fp );
#endif
		xPutsTimeString( fp );
		fputs( szChar, fp );
		int size = ftell( fp );
		fclose(fp);
	}
} // writeLogToFile
//
namespace XE
{
//	_XNoRecursiveInstance _nr__xLog;		// 상호호출 방지 인스턴스.
	// <-구체적으로 어떤자원에 대한 락이라는건지 모호함.
	// 현재로선 debug.cpp의 모든 함수의 진입과 탈출에 대해서 사용.
	static XLock x_Lock;		
}

const char* XE::GetLogFilename( char *pOut, int len )
{
	SYSTEMTIME		time;
	GetLocalTime( &time );
	char cFullpath[ 1024 ];
	// 날짜로 파일명생성.
	// 클라에선 xse로 하고 서버에선 각자 실행파일명의 하위폴더에다.
#ifdef WIN32
  #ifdef _SERVER
	sprintf_s( cFullpath, len, "xse_%02d%02d%02d.txt", time.wYear-2000, time.wMonth, time.wDay );
  #else
	sprintf_s( cFullpath, len, "xce_%02d%02d%02d.txt", time.wYear-2000, time.wMonth, time.wDay );
  #endif  
#else
//	sprintf( cFullpath, "xce_%02d%02d%02d.txt", time.wYear-2000, time.wMonth, time.wDay );
	strcpy_s( cFullpath, "xce.txt" );	// win32가 아닐땐 한파일에만 계속 쓰고 커지면 삭제시키자.
	if( XE::IsHave( XE::GetPathWork() ) ) {
		char cFile[ 1024 ];
		strcpy_s( cFile, cFullpath );
		strcpy_s( cFullpath, XE::GetPathWork() );
		strcat_s( cFullpath, cFile );
  #ifdef _VER_ANDROID
		static BOOL s_bFlag = FALSE;
		if( s_bFlag == FALSE ) {
			LOGD( "log path: %s", cFullpath );
			s_bFlag = TRUE;
		}
  #endif
	}
#endif
	strcpy_s( pOut, len, cFullpath );
	return pOut;
}
//////////////////////////////////////////////////////////////////////////
// XLOG류에서 호출되는 로그찍는 함수. 
// 장차 이런함수들은 모두 XLibrary안으로 들어간다.
int __xLog( int type, LPCTSTR str, ...)
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
	//
	TCHAR szBuff[ 4096 ];	// utf8이 길어서 넉넉하게 잡았다.
	va_list         vl;
	va_start( vl, str );
	_vstprintf_s( szBuff, str, vl );
	va_end( vl );
	_tcscat_s( szBuff, _T( "\n" ) );
	//////////////////////////////////////////////////////////////////////////
	// 일단 로그 메시지는 파일에 기록
#if !defined(_DEBUG) || defined(_XNOT_BREAK)	// 디버그가 아니거나 NOT_BREAK가 있으면 파일에 씀.
#pragma message("not debug or _xnot_break")
	// 릴리즈 모드에선 로그나 에러모두 파일에 써야 한다
	{
		char cFullpath[ 1024 ];
		XE::GetLogFilename( cFullpath );
		// 파일사이즈가 지나치게 커지면 삭제시킴
  #ifndef WIN32
		DWORD size = XSYSTEM::GetFileSize( cFullpath );
		if( size > 0xffff )
			XSYSTEM::RemoveFile( cFullpath );
  #endif
		XE::WriteLogToFile( cFullpath, szBuff );
	}
#else
	#pragma message("debug or not _xnot_break")
#endif // not debug
	//////////////////////////////////////////////////////////////////////////
	// 플랫폼별로 통합환경의 콘솔창에 콘솔메시지 출력
#ifdef WIN32
	::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	_tprintf(_T("%s"), szBuff);
#endif
#ifdef _VER_IOS
#error "ios use Debug.mm !!"
#endif
	// win32의 경우 콘솔을 호출하면 디폴트로는 vc콘솔창에 출력되어야 하고
	// 커스텀 콘솔창을 쓰고 싶다면 엔진에서 Console함수를 재정의해서 써야 한다.
	// 이제 여기서 콘솔쓰지 말것. 복잡도 증가로 상호재호출 금지됨.
//	if( XLIB )
//		XLIB->Console( _T("%s"), szBuff );		// 메시지창이 허용되지 않으면 콘솔에 쓴다.
	// 필요하다면 메시지창 이나 브레이크
	if( type == XLOGTYPE_ERROR ) {
#ifdef _VER_ANDROID
		LOGE( "%s", szBuff );
#endif
		auto retAlert = XE::xAR_NONE;
	  #ifdef _XTOOL
		XSYSTEM::xDebugBreak();
		retAlert = XSYSTEM::xMessageBox( szBuff, XE::xMB_OKCANCEL );
		// 툴에선 XERROR를 불러도 exit()시키지 않는다. 데이터를 저장하지 않은상태이기때문에 어떻게 해서든 살리려고 시도해야한다
	  #else
		if( XE::bAllowAlert ) {
			retAlert = XSYSTEM::xMessageBox( szBuff, XE::xMB_OKCANCEL );
			#ifdef _CLIENT
				XE::GetMain()->OnAlert();
			#endif
    }
		XBREAK_POINT();
		#ifdef _SERVER
			// 서버에서 XLOGTYPE_ERROR를 써서 exit()를 했다면 왜 exit() 를 했는지 로그를 자세히 남겨야 한다.
			#pragma message("check this")
		#endif
		if( retAlert == XE::xAR_CANCEL ) {
			//XLOGXN("exit(1)");
			exit(1);
		}
	  #endif // not tool
	} else
	if( type == XLOGTYPE_LOG ) {
#ifdef _VER_ANDROID
		LOGD( "%s", szBuff );
#endif
	} else
	if( type == XLOGTYPE_ALERT )	{
#ifdef _VER_ANDROID
		LOGD( "%s", szBuff );
#endif
		XBREAK_POINT();
//		_tcscpy_s( szTitle, _T("Message!") );
		if( XE::bAllowAlert ) {
			XSYSTEM::xMessageBox( szBuff, XE::xMB_YESNO );
		  #ifdef _CLIENT
            XE::GetMain()->OnAlert();
		  #endif
    }
	}
	if( type == XLOGTYPE_ERROR || type == XLOGTYPE_ALERT ) {
#ifdef _VER_ANDROID
		LOGE( "throw =====================================================" );
#endif // _VER_ANDROID
//		throw new std::exception();
	}
	return 1;		// XBREAK()같은데서 쓰이므로 항상 1을 리턴해야함
}

// 비졀스튜디오 콘솔창에만 찍는 버전
void XTrace( LPCTSTR szFormat, ... )
{
#ifdef _XBOT
	return;
#endif // _XBOT
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
#ifdef WIN32
	TCHAR szBuff[ 0x10000 ];
	va_list         vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	_tcscat_s( szBuff, _T( "\r\n" ) );
	::OutputDebugString( szBuff );
#endif
}

// 비주얼스튜디오나 이클립스의 디버그콘솔에 메시지르 출력한다. 파일에 쓰지는 않는다.
// 끝에 \n을 붙이지 않는다. 그러므로 엔터가 필요하다면 str에 포함되어야 한다.
// 함수명같은것도 출력하지 않고 파일에도 쓰지 않는다.
// 주로 호출한 함수명 같은걸 알필요없는 엔진 초기화 진행상황 같은걸 볼때 쓴다.
int __xLogx( LPCTSTR str, ...)
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
    TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    _vstprintf_s(szBuff, str, vl);
    va_end(vl);
    
    // LOG
#ifdef _VER_ANDROID
    LOGD( "%s", szBuff );
#endif // _DEV
#ifdef WIN32
//	XLIB->Console( _T("%s"), szBuff );
    ::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
#endif
	return 1;
}

int __xLibConsole( LPCTSTR szFormat, ... )
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	XAUTO_LOCK2( XE::x_Lock );
	TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
	va_list         vl;
	va_start(vl, szFormat);
	_vstprintf_s(szBuff, szFormat, vl);
	va_end(vl);
	if( XLIB )
		XLIB->Console( _T("%s"), szBuff );
	return 1;
}

int __xLibConsoleA( const char* cFormat, ... )
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	XAUTO_LOCK2( XE::x_Lock );
	char buff[ 4096 ];
	va_list vl;
	va_start( vl, cFormat );
	vsprintf_s( buff, cFormat, vl );
	va_end( vl );
	if( XLIB ) {
		const _tstring strt = C2SZ(buff);
		XLIB->Console( _T( "%s" ), strt.c_str() );
	}
	return 1;
}

