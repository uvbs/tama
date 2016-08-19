#include "stdafx.h"
#include <stdio.h>
#include "Debug.h"

/*
#ifndef TRACE
#ifdef WIN32
//#define TRACE __noop()
#else
#define TRACE ((void)0)
#endif
#endif // not TRACE
*/


// str로 들어오는 스트링은 모두 UTF-8로 가정한다.
void xPutsTimeString( FILE *fp )
{
	static char            szStr[8192];
	SYSTEMTIME		time;
	GetLocalTime( &time );
	sprintf_s( szStr, "%d/%2d/%2d   %02d:%02d:%02d   \n", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond );
	fputs( szStr, fp );
}
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/08/11 17:40
*****************************************************************/
class XCheckRecursive
{
public:
	static bool m_bEnter;
	static bool m_bError;
	static bool IsRecursive() {
		return m_bError;
	}
public:
	XCheckRecursive() { 
		if( m_bEnter ) {
			m_bError = true;
			return;
		}
		m_bEnter = true;

	}
	virtual ~XCheckRecursive() { 
		m_bEnter = false;
	}
	// get/setter
	// public member
// private member
private:
// private method
private:
	void Init() {}
	void Destroy() {}
}; // class XCheckResursive
bool XCheckRecursive::m_bEnter = false;
bool XCheckRecursive::m_bError = false;

/**
 @brief 
*/
int __xLog( int type, LPCTSTR str, ...)
{
	XCheckRecursive checkRecursive;
	if( checkRecursive.IsRecursive() )
		return 1;
    TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
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
#ifdef _XTOOL
		CString str = XE::GetCwd();
		str += "error.txt";
		fopen_s( &fp, Convert_TCHAR_To_char( str ), "a+" );
#else
		fopen_s( &fp, "error.txt", "a+" );
#endif
		//XBREAK( fp == NULL );
		if( XASSERT(fp) ) {
			xPutsTimeString( fp );
			fputs( szChar, fp );
			int size = ftell( fp );
			fclose(fp);
		}
#ifdef _GAME	// 게임에서만...
		// 로그파일이 무한정 쌓이지 않도록 한다.
		if( size >= 0xffff ) {
			fopen_s( &fp, "error.txt", "w+" );
			if( fp ) {
				xPutsTimeString( fp );
				fputs( szChar, fp );
				fclose( fp );
#ifndef _MASTER	// 마스터본이 아닐때만
				XALERT( "로그파일 삭제" );
#endif
			}
		}
#endif 
	}
#endif
	TCHAR szTitle[256] = {0,};
	if( type == XLOGTYPE_ERROR )
	{
		_tcscpy_s( szTitle, _T("Error!") );
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	#ifdef _XTOOL
		AfxDebugBreak();
		AfxMessageBox( szBuff, MB_OK );
		// 툴에선 XERROR를 불러도 exit()시키지 않는다. 데이터를 저장하지 않은상태이기때문에 어떻게 해서든 살리려고 시도해야한다
	#else
//		::MessageBox( NULL, szBuff, szTitle, MB_OK );
		AfxMessageBox( szBuff, MB_OK );
		AfxDebugBreak();
		exit(1);
	#endif
	} else
	if( type == XLOGTYPE_LOG ) {
		// LOG
		_tcscpy_s( szTitle, _T("Message!") );
		_tcscat_s( szBuff, _T("\n") );
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	} else
	if( type == XLOGTYPE_ALERT ) {
		AfxDebugBreak();
		_tcscpy_s( szTitle, _T("Message!") );
//		::OutputDebugString( szBuff );		// TRACE로 한글출력이 안되서 이걸로 바꿈
	#ifdef _XTOOL
		AfxMessageBox( szBuff, MB_OK );
	#else
//		int retv = ::MessageBox( NULL, szBuff, szTitle, MB_OK );
		AfxMessageBox( szBuff, MB_OK );
	#endif
	}
#ifdef _XCONSOLE
	CONSOLE( "%s", szBuff );		// 툴에서는 콘솔뷰로도 보낸다
#endif
	return 1;		// XBREAK()같은데서 쓰이므로 항상 1을 리턴해야함
}

// 비졀스튜디오 콘솔창에만 찍는 버전
void XTrace( LPCTSTR szFormat, ... )
{
    TCHAR szBuff[ 0xffff ];	
    va_list         vl;
	
    va_start(vl, szFormat);
    _vstprintf_s(szBuff, szFormat, vl);
    va_end(vl);

	_tcscat_s( szBuff, _T("\n") );
	::OutputDebugString( szBuff );
}

