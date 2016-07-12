#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Debug.h"
#include "xeDef.h"
#include "XLibrary.h"
#include "XSystem.h"
#include "XFramework/XConsoleMain.h"
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
#ifdef _SERVER
	// 서버의경우 개인개발자용 빌드만 메시지창이 뜬다.
	#if _DEV_LEVEL == DLV_DEV_PERSONAL
		BOOL bAllowAlert = TRUE;		// 메시지창 허용 여부
	#else
		BOOL bAllowAlert = FALSE;		// 메시지창 허용 여부
	#endif
#else
	// 클라에선 디버그일때는 뜨지 않는다.
	#ifdef _DEBUG
		BOOL bAllowAlert = FALSE;		// 메시지창 허용 여부
	#else
		BOOL bAllowAlert = TRUE;		// 메시지창 허용 여부
	#endif
#endif
	// 현재로선 debug.cpp의 모든 함수의 진입과 탈출에 대해서 사용.
	static XLock x_Lock;
	static bool x_bEntered = false;		// __xLog2함수 중복진입 방지 장치
#if defined(_VER_ANDROID)
	std::vector<bool> x_aryAllowLog = {false, true, true};
#elif defined(_VER_IOS)
	std::vector<bool> x_aryAllowLog = {false, true, true};
#elif defined(WIN32)
	std::vector<bool> x_aryAllowLog = {true, true, true};
#else
#error "unknown platform"
#endif 
	void SetbAllowLog( int typeLog, bool bFlag ) {
		if( typeLog < 0 || typeLog >= (int)x_aryAllowLog.size() )
			return;
		x_aryAllowLog[ typeLog ] = bFlag;
	}
	bool GetbAllowLog( int typeLog ) {
		if( typeLog < 0 || typeLog >= (int)x_aryAllowLog.size() )
			return false;
		return x_aryAllowLog[ typeLog ];
	}
	bool GetbAllowLog() {
		return x_aryAllowLog[ 0 ];
	}
	void SetbAllowLog( bool bFlag ) {
		x_aryAllowLog[0] = bFlag;
	}
};


#ifdef _VER_ANDROID
static int sGetLogTypeForAndroid( int typeXE ) {
	switch( typeXE ) {
	case XLOGTYPE_LOG:		return ANDROID_LOG_DEBUG;
	case XLOGTYPE_ALERT:
	case XLOGTYPE_ERROR:		
		return ANDROID_LOG_ERROR;
		break;
	default:
		break;
	}
	return ANDROID_LOG_DEBUG;
}
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"xuzhu",__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "xuzhu",__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"xuzhu",__VA_ARGS__)
#define  LOG_ANDROID( TYPE, ...)  __android_log_print( sGetLogTypeForAndroid(TYPE), "xuzhu",__VA_ARGS__)
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
#ifdef _XBOT
	return;
#endif // _XBOT
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
// 	XAUTO_LOCK2( XE::x_Lock );
	// 여기내부에선 다시 다른락을 걸어선 안됨.
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

/**
 @brief 로그를 파일에 쓴다. 파일명은 자동으로 생성한다
*/
void XE::WriteLogToFile( LPCTSTR szBuff )
{
	char cFullpath[1024];
	XE::GetLogFilename( cFullpath );
#ifndef WIN32
	// 기기에선 파일사이즈가 지나치게 커지면 삭제시킴
	DWORD size = XSYSTEM::GetFileSize( cFullpath );
	if( size > 0xffff )
		XSYSTEM::RemoveFile( cFullpath );
#endif
	XE::WriteLogToFile( cFullpath, szBuff );
}

//
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
/**
 @brief 앞으로 이버전을 쓰도록 한다. lock을 이곳에서만 하기위해 ...파라메터를 빼고 외부에서 ...을 변형해서 가져오게 한다.
*/
//////////////////////////////////////////////////////////////////////////
int __xLog( int type, LPCTSTR _szBuff )
{
 	if( !CONSOLE_MAIN )
 		return 1;
#ifdef _XBOT
	return 1;
#endif // _XBOT
	// 다른스레드에서 동시에 호출된거라면 락에 걸린다.
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
	// 같은스레드에서 재귀호출되면 그냥 리턴한다.
	if( XE::x_bEntered )
		return 1;
	XE::x_bEntered = true;
	_tstring strMsg = _szBuff;
	strMsg += _T("\n");
	//////////////////////////////////////////////////////////////////////////
	// 일단 로그 메시지는 파일에 기록
	if( type != XLOGTYPE_LOG		// XBREAK등으로 생긴건 무조건 로그에 써야함.
		|| (type == XLOGTYPE_LOG && XE::GetbAllowLog( XLOGTYPE_LOG ))  ) {		// 일반 로그는 켜져있을때만...
#if !defined(_DEBUG) || defined(_XNOT_BREAK)	// 디버그가 아니거나 NOT_BREAK가 있으면 파일에 씀.
#pragma message("not debug or _xnot_break")
		// 릴리즈 모드에선 로그나 에러모두 파일에 써야 한다
		XE::WriteLogToFile( strMsg.c_str() );
#else
#pragma message("debug or not _xnot_break")
#endif // not debug
	}
	//////////////////////////////////////////////////////////////////////////
	// 플랫폼별로 통합환경의 콘솔창에 콘솔메시지 출력
#if defined(WIN32)
	::OutputDebugString( strMsg.c_str() );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	_tprintf( _T( "%s" ), strMsg.c_str() );
#elif defined(_VER_IOS)
#error "ios use Debug.mm !!"
#elif defined(_VER_ANDROID)
//		LOG_ANDROID( type, "%s", strMsg.c_str() );
	LOG_ANDROID( type, "%s", strMsg.c_str() );
#endif
	// 필요하다면 메시지창 이나 브레이크
	//////////////////////////////////////////////////////////////////////////
	if( type == XLOGTYPE_ERROR || type == XLOGTYPE_ALERT ) {
		auto retAlert = XE::xAR_NONE;
		if( XE::bAllowAlert ) {
			// 메시지박스를 허용하는 옵션
			retAlert = XSYSTEM::xMessageBox( strMsg.c_str(), XE::xMB_OKCANCEL );
  #ifdef _CLIENT
			XE::GetMain()->OnAlert();		// 클라의경우 핸들러를 호출
  #endif
			// alert창 떴을때 cancel누르면 종료하게 함.
			if( retAlert == XE::xAR_CANCEL ) {
				exit( 1 );
			}
		}
//	throw new std::exception();
	}
	// 마지막으로 앱쪽에 다른곳에 로그를 찍을 기회를 줌. 
	if( XLIB ) {
		XLIB->Console( strMsg.c_str() );
	}
	XE::x_bEntered = false;
	return 1;		// XBREAK()같은데서 쓰이므로 항상 1을 리턴해야함
} // __xLog

/**
 @brief 앞으로 이버전을 쓰도록 한다.__xLog2로 단일화 시킨 버전
*/
int __xLogf( int type, LPCTSTR format, ... )
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
	va_list vl;
	va_start(vl, format);
	_vstprintf_s(szBuff, format, vl);
	va_end(vl);
	__xLog( type, szBuff );
	return 1;
}

/**
 @brief 앞으로 이버전을 쓰도록 한다.__xLog2로 단일화 시킨 버전
 win32에서 const char* 쓰는 버전
*/
int __xLogfA( int type, const char* format, ... )
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	char buff[ 4096 ];
	va_list vl;
	va_start( vl, format );
	vsprintf_s( buff, format, vl );
	va_end( vl );
	//
	const _tstring strt = C2SZ( buff );
	__xLog( type, strt.c_str() );
	return 1;
}

/**
 @brief cTag에 XConsoleMain::m_listTags리스트의 태그가 포함되어있으면 로그를 남긴다.
*/
int __xLogTag( const char* cTag, int type, LPCTSTR szLog )
{
#ifdef _SERVER
	if( CONSOLE_MAIN ) {
		if( CONSOLE_MAIN->IsHaveTag( cTag ) ) {
			return __xLog( type, szLog );
		}
	}
#else
	return __xLog( type, szLog );
#endif // _SERVER
	return 1;
}

int __xLogfTag( const char* cTag, int type, LPCTSTR format, ... )
{
	TCHAR szLog[4096];	// utf8이 길어서 넉넉하게 잡았다.
	va_list vl;
	va_start( vl, format );
	_vstprintf_s( szLog, format, vl );
	va_end( vl );
	return __xLogTag( cTag, type, szLog );
}

int __xLogfTagA( const char* cTag, int type, const char* format, ... )
{
#ifdef _XBOT
	return 1;
#endif // _XBOT
	char cLog[4096];
	va_list vl;
	va_start( vl, format );
	vsprintf_s( cLog, format, vl );
	va_end( vl );
	//
	const _tstring strt = C2SZ( cLog );
	return __xLogTag( cTag, type, strt.c_str() );
}


// 비졀스튜디오 콘솔창에만 찍는 버전
void XTraceWin32( LPCTSTR szFormat, ... )
{
#ifdef _XBOT
	return;
#endif // _XBOT
#ifdef WIN32
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
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
void XTrace( LPCTSTR str, ...)
{
#ifdef _XBOT
	return;
#endif // _XBOT
	TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
	va_list         vl;
	va_start( vl, str );
	_vstprintf_s( szBuff, str, vl );
	va_end( vl );
	// LOG
#ifdef _VER_ANDROID
	LOGD( "%s", szBuff );
#endif // _DEV
#ifdef WIN32
	::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
#endif
	return;
}

// int __xLibConsole( LPCTSTR szFormat, ... )
// {
// #ifdef _XBOT
// 	return 1;
// #endif // _XBOT
// 	XAUTO_LOCK2( XE::x_Lock );
// 	TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
// 	va_list         vl;
// 	va_start(vl, szFormat);
// 	_vstprintf_s(szBuff, szFormat, vl);
// 	va_end(vl);
// 	if( XLIB )
// 		XLIB->Console( _T("%s"), szBuff );
// 	return 1;
// }
// 
// int __xLibConsoleA( const char* cFormat, ... )
// {
// #ifdef _XBOT
// 	return 1;
// #endif // _XBOT
// 	XAUTO_LOCK2( XE::x_Lock );
// 	char buff[ 4096 ];
// 	va_list vl;
// 	va_start( vl, cFormat );
// 	vsprintf_s( buff, cFormat, vl );
// 	va_end( vl );
// 	if( XLIB ) {
// 		const _tstring strt = C2SZ(buff);
// 		XLIB->Console( _T( "%s" ), strt.c_str() );
// 	}
// 	return 1;
// }
// 
