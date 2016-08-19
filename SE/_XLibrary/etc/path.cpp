#include "stdafx.h"
#ifdef _VER_IPHONE
//#include "ConvertString.h"
#include "xString.h"
#endif
#include "path.h"
#include "xUtil.h"
#include "debug.h"
#include "ConvertString.h"
#include "xLang.h"
static TCHAR g_szExecDir[ MAX_PATH ];			// 실행파일폴더
static TCHAR g_szTWorkDir[MAX_PATH];		// TCHAR버전
static TCHAR _szTTempBuff[MAX_PATH];
#ifdef WIN32
static char	g_szWorkDir[MAX_PATH];			// char 버전
static char _szTempBuff[MAX_PATH];
#endif

// XE::GetPath는 GetFilePath()로 바뀜
#ifdef WIN32 
// TCHAR sz[100] -> size는 100
// 실행파일의 폴더를 얻음
LPCTSTR XE::GetCwd( TCHAR *szBuff, int nSize )
{
	TCHAR szFull[MAX_PATH];
	GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	_tcscpy_s( szBuff, nSize, GetFilePath( szFull ) );
//	GetPath( szBuff, szFull, nSize );						// 패스부분만 떼냄.
	return szBuff;
}
// 실행파일의 폴더를 얻음
const char *XE::GetCwd( char *cBuff, int nSize )
{
	char cFull[ MAX_PATH ];
	GetModuleFileNameA( NULL, cFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	strcpy_s( cBuff, nSize, GetFilePath( cFull ) );
	return cBuff;
}
// 실행파일의 폴더를 얻음
LPCTSTR XE::GetCwd( void  )
{
	TCHAR szFull[MAX_PATH];
	GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	_tcscpy_s( _szTTempBuff, MAX_PATH, GetFilePath( szFull ) );
	return _szTTempBuff;
}
#endif // WIN32

// 워킹폴더 설정
LPCTSTR _SetWorkDir( TCHAR *szBuff, int maxlen );
LPCTSTR XE::SetWorkDir( LPCTSTR defaultWorkDir )
{
#ifdef _VER_IPHONE
	_SetWorkDir( g_szTWorkDir, MAX_PATH );  // path.mm
#endif
#ifdef WIN32
	if( defaultWorkDir )
		_tcscpy_s( g_szTWorkDir, defaultWorkDir );		// 외부에서 워킹폴더를 줬을땐 그것을 사용하고
	else
		GetCurrentDirectory( MAX_PATH, g_szTWorkDir );		// 현재 작업폴더를 읽는다
	Convert_TCHAR_To_char( g_szWorkDir, MAX_PATH, g_szTWorkDir );
//		XE::GetCwd( g_szTWorkDir, MAX_PATH );			// 아니면 실행파일폴더를 워킹폴더로 한다
	_tcscpy_s( g_szExecDir, XE::GetCwd() );
#endif
	return g_szTWorkDir;
}

LPCTSTR XE::GetExecDir()
{
	return g_szExecDir;
}

LPCTSTR	XE::MakePath( LPCTSTR szPath, LPCTSTR szFileName, XE::xtLang lang )
{
	if( IsEmpty(g_szTWorkDir) )
		XERROR( "SetWorkDir()를 하지 않았다" );
	_tcscpy_s( _szTTempBuff, g_szTWorkDir );
#ifdef _VER_IPHONE
	_tcscat_s( _szTTempBuff, "/" );
	TCHAR szBuff[256];
	_tcscpy_s( szBuff, szPath );
 #ifdef _ALL_FILE_LOWER_CASE
	_tcslwr( szBuff );
 #endif
	_tcscat_s( _szTTempBuff, szBuff );
	if( lang )
		_tcscat_s( _szTTempBuff, XE::GetLangDir( lang ) );		// 랭기지 옵션이 들어오면 랭기지 폴더도 붙임
	_tcscpy_s( szBuff, szFileName );
	// 아이폰에서는 파일명 대/소문자 정책
	// 파일을 모두 소문자로 만들면 프로그램이 편하긴 한데 파일관리할때 소문자로만 된 파일명을 봐야해서 가독성이 떨어진다
	// 그러므로 좀 불편해도 대소문자를 확실히 구분해서 사용하도록 하자
 #ifdef _ALL_FILE_LOWER_CASE		// 옴니헬이나 bnb등은 이 디파인을 붙여줄것
	_tcslwr( szBuff );
 #endif
	_tcscat_s( _szTTempBuff, szBuff );
#else
	_tcscat_s( _szTTempBuff, _T("\\") );
	_tcscat_s( _szTTempBuff, szPath );
 #ifdef _GAME
	if( lang )
		_tcscat_s( _szTTempBuff, XE::GetLangDir( lang ) );		// 랭기지 옵션이 들어오면 랭기지 폴더도 붙임
 #endif
	_tcscat_s( _szTTempBuff, szFileName );
#endif
	
#ifdef _VER_IPHONE
  #ifndef _UNICODE		// 유니코드모드가 아닐때(bnbTest)는 파일명이 멀티바이트로 올수도 있고 utf8로 올수도 있으므로 변환해준다
	// 조합된 풀패스스트링을 UTF-8로 변환한다. 이미UTF-8이라면 그대로 돌려준다.
	char buff[1024];
	_ConvertEUCKRToUTF8(buff, _szTTempBuff, 1024);	
	strcpy( _szTTempBuff, buff );
  #endif 
#endif
//	XLOG( "%s", _szTTempBuff );
#ifdef _VER_IPHONE
//	_NSLog( "%s", _szTTempBuff );
#endif
	return _szTTempBuff;
}
#ifdef WIN32
const char*	XE::MakePath( const char* szPath, const char* szFileName )
{
	if( IsEmpty(g_szWorkDir) )
		XERROR( "SetWorkDir()를 하지 않았다" );
	strcpy_s( _szTempBuff, g_szWorkDir );
	strcat_s( _szTempBuff, "\\" );
	strcat_s( _szTempBuff, szPath );
	strcat_s( _szTempBuff, szFileName );
	
//	XLOG( "%s", _szTempBuff );		// TCHAR형이기 때문에 여기서 못씀
	return _szTempBuff;
}
#endif

// szSrc의 상위 패스를 만든다.
// ex) c:/test/test -> c:/test
// 끝에 /나 스페이스 등이 붙어도 자동으로 잘라준다.
BOOL XE::MakeHighPath( TCHAR *pOut, int lenOut, LPCTSTR _szSrc )
{
#ifdef WIN32
	TCHAR dirMark = _T( '\\' );
#else
	char dirMark = '/';
#endif
	memset( pOut, 0, lenOut * sizeof( TCHAR ) );
	int lenSrc = _tcslen( _szSrc );
	if( lenSrc == 1 && _szSrc[ 0 ] == dirMark )
	{
		pOut[ 0 ] = 0;
		return FALSE;
	}
	TCHAR szSrc[ 1024 ];
	_tcscpy_s( szSrc, _szSrc );
	// szSrc의 맨끝에 \나 스페이스가 있는지 검사해서 잘라냄.
	for( int i = lenSrc - 1; i >= 0; --i )
	{
		TCHAR c = szSrc[ i ];
		if( c != dirMark && c != _T( ' ' ) )
		{
			szSrc[ i + 1 ] = 0;
			break;
		}
	}
	lenSrc = _tcslen( szSrc );	// 다시 계산.
	// szSrc의 뒤에서부터 검사해서 \위치의 포인터를 찾는다.
	LPCTSTR s = _tcsrchr( szSrc, dirMark );
	if( s == NULL )
		return FALSE;
	for( int i = 0; i < lenSrc; ++i )
	{
		pOut[ i ] = szSrc[ i ];
		if( szSrc + i == s || XBREAK( i >= lenOut - 1 ) )
		{
			//			pOut[i] = 0;
			break;
		}
	}
	return TRUE;
}
#define IS_DIR_MARK( C )		( (C) == '\\' || (C) == '/' )
#define IS_NOT_DIR_MARK( C )		( (C) != '\\' && (C) != '/' )
#ifdef WIN32
BOOL XE::MakeHighPath( char *pOut, int lenOut, const char *_cSrc )
{
	//	char dirMark = _T('\\');
	memset( pOut, 0, lenOut * sizeof( char ) );
	int lenSrc = strlen( _cSrc );
	if( lenSrc == 1 && IS_DIR_MARK( _cSrc[ 0 ] ) )
	{
		pOut[ 0 ] = 0;
		return FALSE;
	}
	char cSrc[ 1024 ];
	strcpy_s( cSrc, _cSrc );
	// cSrc의 맨끝에 \나 스페이스가 있는지 검사해서 잘라냄.
	for( int i = lenSrc - 1; i >= 0; --i )
	{
		char c = cSrc[ i ];
		if( IS_NOT_DIR_MARK( c ) && c != ' ' )
		{
			cSrc[ i + 1 ] = 0;
			break;
		}
	}
	lenSrc = strlen( cSrc );	// 다시 계산.
	// cSrc의 뒤에서부터 검사해서 \위치의 포인터를 찾는다.
	const char* s1 = strrchr( cSrc, '\\' );
	const char* s2 = strrchr( cSrc, '/' );
	if( s1 == NULL && s2 == NULL )
		return FALSE;
	const char* s = ( (DWORD)s1 > (DWORD)s2 ) ? s1 : s2;
	for( int i = 0; i < lenSrc; ++i )
	{
		pOut[ i ] = cSrc[ i ];
		if( cSrc + i == s || XBREAK( i >= lenOut - 1 ) )
		{
			//			pOut[i] = 0;
			break;
		}
	}
	return TRUE;
}
#endif // win32


LPCTSTR XE::GetWorkDir()
{
	return g_szTWorkDir;
}

#ifdef WIN32
#include <stdio.h>
#include <io.h>
BOOL XE::IsDirExists( LPCTSTR s ) 
{
  _tfinddatai64_t c_file;
  intptr_t hFile;
  BOOL result = FALSE;

  hFile = _tfindfirsti64(s, &c_file);
  if (c_file.attrib & _A_SUBDIR ) result = TRUE;
  _findclose(hFile);

  return result;
}
BOOL XE::CreateDir( LPCTSTR szPath )
{
	return CreateDirectory( szPath, NULL );
}
#endif
