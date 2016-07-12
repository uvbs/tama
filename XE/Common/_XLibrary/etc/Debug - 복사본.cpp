#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Debug.h"
#include "xeDef.h"
#include "XLibrary.h"
#ifdef _VER_ANDROID
#include <android/log.h>
#endif
#ifdef _CLIENT
#include "XClientMain.h"
#endif

namespace XSYSTEM
{
	int xMessageBox( LPCTSTR szMsg, XE::xtAlert type )
	{
		int retv = 1;
  #ifdef WIN32
	#ifdef _AFX
		retv = AfxMessageBox( szMsg, (UINT)type );
	#else
		retv = MessageBox( NULL, szMsg, _T("Alert"), (UINT) type );
	#endif
  #endif
  #ifdef _VER_IOS
	#error "do not use this file. use debug.mm"
  #endif
  #ifdef _VER_ANDROID
		XLOG( "%s", szMsg );
  #endif
		return retv;
	}
	//
	void xDebugBreak( void )
	{
#ifdef WIN32
  #ifdef _AFX
		AfxDebugBreak();
  #else
		DebugBreak();
  #endif
#else
		volatile int nBreak = 0;
		nBreak = 1;
#endif
	}
};


/*
#ifndef TRACE
#ifdef WIN32
//#define TRACE __noop()
#else
#define TRACE ((void)0)
#endif
#endif // not TRACE
*/

namespace XE
{
	BOOL bAllowAlert = TRUE;		// 메시지창 허용 여부
};

#ifdef _VER_ANDROID
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"xuzhu",__VA_ARGS__)
#else
#define LOGD(...)   (0)
#endif // android


// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
void xPutsTimeString( FILE *fp )
{
#ifdef WIN32
	static char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, "%d/%2d/%2d   %02d:%02d:%02d   \n", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond );
	fputs( szStr, fp );
#endif
}

void WriteLogToFile( const char *cFile )
{

}
//
int __xLog( int type, LPCTSTR str, ...)
{
    TCHAR szBuff[2048];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    _vstprintf_s(szBuff, str, vl);
    va_end(vl);

#ifdef _DEBUG
#else
	// 릴리즈 모드에선 로그나 에러모두 파일에 써야 한다
	{
		char szChar[8192];
		memset( szChar, 0, sizeof(szChar) );
		WideCharToMultiByte(CP_ACP, 0, szBuff, -1, szChar, 1024, NULL, NULL);
		int len = strlen(szChar);
		szChar[len] = '\n';
		szChar[len+1] = 0;
		FILE *fp;
		char cFilename[ 64 ];
  #ifdef _SERVER
		strcpy_s( cFilename, "error_s.txt" );
  #else
		strcpy_s( cFilename, "error.txt" );
  #endif
  #ifdef _XTOOL
		CString str = XE::GetCwd();
		str += "error.txt";
		fopen_s( &fp, Convert_TCHAR_To_char( str ), "a+" );
  #else
		fopen_s( &fp, cFilename, "a+" );
  #endif
		XBREAK( fp == NULL );
		if( fp )
		{
			xPutsTimeString( fp );
			fputs( szChar, fp );
			int size = ftell( fp );
			fclose(fp);
		}
  #ifdef _GAME	// 게임에서만...
		// 로그파일이 무한정 쌓이지 않도록 한다.
		if( size >= 0xffff )
		{
			fopen_s( &fp, cFilename, "w+" );
			if( fp )
			{
				xPutsTimeString( fp );
				fputs( szChar, fp );
				fclose(fp);
    #ifndef _MASTER	// 마스터본이 아닐때만
//				XALERT( "로그파일 삭제" );  
    #endif // not master
			}
		}
  #endif 
	}
#endif // not debug
	TCHAR szTitle[256] = {0,};
	if( type == XLOGTYPE_ERROR )
	{
		_tcscpy_s( szTitle, _T("Error!") );
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	#ifdef _XTOOL
		XSYSTEM::xDebugBreak();
		XSYSTEM::xMessageBox( szBuff, XE::xMB_OK );
		// 툴에선 XERROR를 불러도 exit()시키지 않는다. 데이터를 저장하지 않은상태이기때문에 어떻게 해서든 살리려고 시도해야한다
	#else
      #ifdef WIN32
		if( XE::bAllowAlert )
        {
			XSYSTEM::xMessageBox( szBuff, XE::xMB_OK );
		#ifdef _CLIENT
				XE::GetMain()->OnAlert();
		#endif
        }
      #else
        LOGD( szBuff );
      #endif
		XSYSTEM::xDebugBreak();
		exit(1);
	#endif
	} else
	if( type == XLOGTYPE_LOG )
	{
		// LOG
		_tcscpy_s( szTitle, _T("Message!") );
		_tcscat_s( szBuff, _T("\n") );
#ifdef _VER_ANDROID
        LOGD( szBuff );
#endif 
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	} else
	if( type == XLOGTYPE_ALERT )
	{
		XSYSTEM::xDebugBreak();
		_tcscpy_s( szTitle, _T("Message!") );
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
#ifdef WIN32
		if( XE::bAllowAlert )
        {
			XSYSTEM::xMessageBox( szBuff, XE::xMB_OK );
#ifdef _CLIENT
            XE::GetMain()->OnAlert();
#endif
        }
		else
		{
			// win32의 경우 콘솔을 호출하면 디폴트로는 vc콘솔창에 출력되어야 하고
			// 커스텀 콘솔창을 쓰고 싶다면 엔진에서 Console함수를 재정의해서 써야 한다.
			XLIB->Console( _T("%s"), szBuff );		// 메시지창이 허용되지 않으면 콘솔에 쓴다.

		}
#else
        LOGD( szBuff );
#endif
	}
#ifdef _XCONSOLE
  #ifdef _DEBUG	// 서버에서 XLOG로 에러로그 기록 남기는데 콘솔에까지 나오면 느려질까봐 뺌
	CONSOLE( "%s", szBuff );		// 툴에서는 콘솔뷰로도 보낸다
  #endif
#endif

	return 1;		// XBREAK()같은데서 쓰이므로 항상 1을 리턴해야함
}

// 비졀스튜디오 콘솔창에만 찍는 버전
void XTrace( LPCTSTR szFormat, ... )
{
#ifdef WIN32
    TCHAR szBuff[ 0xffff ];
    va_list         vl;
	
    va_start(vl, szFormat);
    _vstprintf_s(szBuff, szFormat, vl);
    va_end(vl);

	_tcscat_s( szBuff, _T("\r\n") );
	::OutputDebugString( szBuff );
#endif
}

// 끝에 \n을 붙이지 않는다.
// 함수명같은것도 출력하지 않고 파일에도 쓰지 않는다.
// 주로 호출한 함수명 같은걸 알필요없는 엔진 초기화 진행상황 같은걸 볼때 쓴다.
int __xLogx( LPCTSTR str, ...)
{
    TCHAR szBuff[4096];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    _vstprintf_s(szBuff, str, vl);
    va_end(vl);
    
    // LOG
#ifdef _VER_ANDROID
    LOGD( szBuff );
#endif
#ifdef WIN32
	XLIB->Console( _T("%s"), szBuff );
    ::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
#endif
	return 1;
}
