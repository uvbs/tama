#include "stdafx.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "xe.h"
#include "path.h"
#include "xUtil.h"
#include "debug.h"
#include "ConvertString.h"
#include "xeDef.h"
#include "XSystem.h"
#ifdef _CLIENT
#include "xLang.h"
#endif
#include "XResMng.h"
#include "XFramework/XConsoleMain.h"
#ifdef _VER_ANDROID
#include "XFramework/android/com_mtricks_xe_Cocos2dxHelper.h"
#include "XFramework/android/JniHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace XE
{
#ifdef WIN32
	static TCHAR _DIR_SPR[ 32 ] = _T("spr\\");
#else
	static TCHAR _DIR_SPR[ 32 ] = _T("spr/");
#endif
	// 이제 패스류는 끝에 /로 끝나야 한다.
static int x_lenWorkDir = 0;
static TCHAR x_szWorkDir[MAX_PATH];		// 워킹폴더
static TCHAR x_szDocDir[MAX_PATH];		// Doc폴더(사용자가 생성환 파일을 보관하는 곳)
static _tstring x_strPackage;      // apk 풀패스
// #ifndef _VER_ANDROID
// 	static TCHAR x_szPackageDir[MAX_PATH];		// 패키지폴더. android에선 x_strPackage만 씀.
// #endif // not android
static TCHAR _szTempBuff[4096];
static TCHAR x_szPatchDir[MAX_PATH];	// 패치로 받는 파일들의 폴더. 원래 work가 패치폴더이지만 윈도우에서는 파일관리의 편의성때문에 별도의 폴더로 설정함.
#ifdef WIN32
	static TCHAR x_szExecDir[ MAX_PATH ];			// 실행파일폴더
	static char	x_cWorkDir[MAX_PATH];			// char 버전
	static char	x_cPackageDir[MAX_PATH];			// char 버전
	static char _cTempBuff[MAX_PATH];
	static char x_cPatchDir[ MAX_PATH ];	// 패치로 받는 파일들의 폴더. 원래 work가 패치폴더이지만 윈도우에서는 파일관리의 편의성때문에 별도의 폴더로 설정함.
#endif
// #ifdef _VER_ANDROID
// 	_tstring x_strPackage;      // apk 풀패스
// 	const _tstring& GetstrPackage() {
// 		return x_strPackage;
// 	}
// #else
// 	const _tstring& GetstrPackage() {
// 		return x_strPackage;
// 	}
// #endif
	const _tstring& GetstrPackage() {
		return x_strPackage;
	}
	void SetDirSpr( LPCTSTR szDir )
	{
		_tcscpy_s( _DIR_SPR, szDir );
	}
	TCHAR* GetDirSpr()
	{
		return _DIR_SPR;
	}
}; // namespace XE

// 워킹폴더 설정
void XE::InitResourceManager( XConsoleMain *pEngineMain, xtLoadType typeLoad )
{
	x_szWorkDir[0] = 0;
	x_szPatchDir[0] = 0;
#ifndef _VER_ANDROID
//	x_szPackageDir[0] = 0;
#endif // not android
    x_szDocDir[0] = 0;
	//
	XBREAK( typeLoad == xLT_NONE );
	SetLoadType( typeLoad );
	// 패키지 패스와 워킹패스를 지정한다.
	// 패스류는 끝에 /로 끝나야 한다.
#ifdef _VER_IOS
    // 도큐먼트 폴더를 미리 받아둔다.
    XIOS::_GetDocumentPath( x_szDocDir );
    _tcscat_s( x_szDocDir, _T("/") );
	// 번들패스를 버퍼에 받는다.
//	XIOS::SetPackageDir( x_szPackageDir, MAX_PATH );	// path.mm
		TCHAR szPackageDir[ MAX_PATH ] = {0,};
	XIOS::GetBundleDir( szPackageDir, MAX_PATH );	// path.mm
	x_strPackage = szPackageDir;
//	_tcscat_s( x_szPackageDir, pEngineMain->GetVirtualPackageDir() );
	// 캐쉬폴더나 도큐먼트 폴더를 버퍼에 받는다.
        XIOS::SetWorkDir( x_szWorkDir, MAX_PATH );  // path.mm
	// 워킹폴더 생성
	// 안드로이드에서 루아의 SetModulePath때문에 Work폴더가 필요해서 일단은 Work폴더를 무조건 만들게 바꿈
	_tmkdir( x_szWorkDir );
    XLOGXN("mkdir:%s", x_szWorkDir);
#endif
#ifdef _VER_ANDROID
	// JNI를 통해 apk 패스를 받는다.
	x_strPackage = getApkPath();
	// /data/data/패키지이름/Resource/ 를 받는다.
	XBREAK( JniHelper::getInternalResourcePath() == nullptr );
	_tcscpy_s( x_szWorkDir, JniHelper::getInternalResourcePath() );
	_tmkdir( x_szWorkDir );
    XLOGXN("mkdir:%s", x_szWorkDir);
    _tcscpy_s( x_szDocDir, x_szWorkDir );
#endif
#ifdef WIN32
	TCHAR szWin32WorkPath[ 1024 ];
	TCHAR szHighPath[ 1024 ];
	GetCurrentDirectory( MAX_PATH, szWin32WorkPath );		// 현재 작업폴더를 읽는다
	MakeHighPath( szHighPath, szWin32WorkPath );	// 상위폴더를 얻는다.
// 	_tcscpy_s( x_szPackageDir, szHighPath );
// 	_tcscat_s( x_szPackageDir, pEngineMain->GetVirtualPackageDir() );
// 	XBREAK( XE::IsEmpty( x_szPackageDir ) == TRUE );
	x_strPackage = szHighPath;
	x_strPackage += pEngineMain->GetVirtualPackageDir();
// 	_tcscpy_s( x_szPackageDir, szHighPath );
// 	_tcscat_s( x_szPackageDir, pEngineMain->GetVirtualPackageDir() );
	XBREAK( x_strPackage.empty() );
	// 워킹폴더 생성
	// 안드로이드에서 루아의 SetModulePath때문에 Work폴더가 필요해서 일단은 Work폴더를 무조건 만들게 바꿈
	_tcscpy_s( x_szWorkDir, szHighPath );
	_tcscat_s( x_szWorkDir, pEngineMain->GetVirtualWorkDir() );
	XBREAK( XE::IsEmpty( pEngineMain->GetVirtualWorkDir() ) == TRUE );
	XPLATFORM_PATH( x_szWorkDir );
	_tmkdir( x_szWorkDir );
	_tcscat_s( x_szDocDir, x_szWorkDir );
	switch( typeLoad )
	{
	case XE::xLT_WORK_FOLDER_ONLY:
	case XE::xLT_WORK_TO_PACKAGE:
	case XE::xLT_WORK_TO_PACKAGE_COPY:
		{
		}
		break;
	default:
//		_tcscpy_s( x_szWorkDir, x_szPackageDir );
		break;
	}
	_tcscpy_s( x_szExecDir, XE::GetCwd() );
	Convert_TCHAR_To_char( x_cWorkDir, MAX_PATH, x_szWorkDir );
#endif // win32
	x_lenWorkDir = _tcslen( x_szWorkDir );
//#ifdef _VER_ANDROID
	XLOGXN( "package path:%s", x_strPackage.c_str() );
// #else
// 	XLOGXN( "package path:%s\n", x_szPackageDir );
// #endif
	XLOGXN( "work path:%s\n", x_szWorkDir );
    XLOGXN( "doc path:%s\n", x_szDocDir );
}

// XE::GetPath는 GetFilePath()로 바뀜
#ifdef WIN32 
void XE::SetPackagePath( LPCTSTR szPath )
{
//	_tcscpy_s( x_szPackageDir, szPath );
	x_strPackage = szPath;
}
void XE::SetWorkPath( LPCTSTR szPath )
{
	_tcscpy_s( x_szWorkDir, szPath );
}
void XE::SetDocPath( LPCTSTR szPath )
{
	_tcscpy_s( x_szDocDir, szPath );
}
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
	_tcscpy_s( _szTempBuff, MAX_PATH, GetFilePath( szFull ) );
	return _szTempBuff;
}
std::string XE::GetstrCwd()
{
	char buff[ 1024 ] = {0,};
	GetCwd( buff, 1024 );
	return std::string( buff );
}
LPCTSTR XE::GetExecDir()
{
    return x_szExecDir;
}
LPCTSTR XE::GetExecFilename()
{
	TCHAR szFull[MAX_PATH];
	GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	_tcscpy_s( _szTempBuff, MAX_PATH, GetFileName( szFull ) );
	return _szTempBuff;
}

LPCTSTR XE::GetExecFileTitle()
{
	TCHAR szFull[MAX_PATH];
	GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	_tcscpy_s( _szTempBuff, MAX_PATH, GetFileTitle( szFull ) );
	return _szTempBuff;
}


#endif // WIN32

//#ifdef _CLIENT
LPCTSTR	XE::MakePathLang( LPCTSTR szLocalPath, LPCTSTR szFile )
{
	_tcscpy_s( _szTempBuff, szLocalPath );
//	_tcscat_s( _szTempBuff, XE::GetLangDir( g_Lang ) );		// 랭기지 옵션이 들어오면 랭기지 폴더도 붙임
	_tcscat_s( _szTempBuff, LANG.GetLangDir() );		// 랭기지 옵션이 들어오면 랭기지 폴더도 붙임
	_tcscat_s( _szTempBuff, szFile );
#ifdef _VER_ANDROID
//	XLOGXN( "%s", _szTempBuff );
#endif // not win32
	return _szTempBuff;
}
//#endif 

// szPath + szFileName을 해 로컬패스를 만든다.
LPCTSTR	XE::MakePath( LPCTSTR szLocalPath, LPCTSTR szFile )
{
	if( szFile == NULL )
		return _T("");
	_tcscpy_s( _szTempBuff, szLocalPath );
	_tcscat_s( _szTempBuff, szFile );
#ifdef _VER_ANDROID
//	XLOGXN( "%s", _szTempBuff );
#endif // not win32

	return _szTempBuff;
}

std::string XE::MakePath2( const char* cLocalPath, const char* cFile )
{
	if( !cFile )
		return "";
#ifdef _VER_ANDROID
	//	XLOGXN( "%s", _szTempBuff );
#endif // not win32

	return std::string( cLocalPath ) + cFile;
}

#ifdef WIN32
_tstring	XE::MakePath2( LPCTSTR szLocalPath, LPCTSTR szFile )
{
	if( !szFile )
		return _T( "" );
#ifdef _VER_ANDROID
	//	XLOGXN( "%s", _szTempBuff );
#endif // not win32

	return _tstring(szLocalPath) + szFile;
}
#endif // IN32

/**
 내부버퍼(szTempBuff)를 사용하지 않는 버전.
*/
LPCTSTR XE::MakePath( _tstring *pstrOut, LPCTSTR szLocalPath, LPCTSTR szFile )
{
	if( szFile == NULL )
		return _T("");
	*pstrOut = szLocalPath;
	*pstrOut += szFile;
#ifdef _VER_ANDROID
	//	XLOGXN( "%s", _szTempBuff );
#endif // not win32

	return pstrOut->c_str();
}


LPCTSTR XE::MakeWorkFullPath( LPCTSTR szLocalPath, LPCTSTR szFile )
{
    _tcscpy_s( _szTempBuff, GetPathWork() );
    _tcscat_s( _szTempBuff, szLocalPath );
    _tcscat_s( _szTempBuff, szFile );
    return _szTempBuff;
}
LPCTSTR XE::MakeDocFullPath( LPCTSTR szLocalPath, LPCTSTR szFile )
{
    _tcscpy_s( _szTempBuff, GetPathDoc() );
    _tcscat_s( _szTempBuff, szLocalPath );
    _tcscat_s( _szTempBuff, szFile );
    return _szTempBuff;
}

/**
 리소스가 있는 폴더의 풀패스를 만들어 낸다. 
 기기에선 쓸일이 없으므로 win32로 제한을 걸어둠
 @warning 원래 win32와 ios에서만 사용할수 있게 ifdef 되어있었음.  
					필요할때가 있어 풀긴했지만 apk는 압축포맷이기때문에 apk상의 패스를 안다고 해도 파일오픈이 다이렉트로 되진 않는다.
					워크에 옮기지 않고 패키지내의 파일을 직접 써야할땐 CopyPackageToWorkWithDstName로 work에 카피를 하되 다른이름으로 카피를 해서 그 파일을 오픈한다.
					CToken::LoadFromPackage참조.
*/
LPCTSTR XE::MakePackageFullPath( LPCTSTR szLocalPath, LPCTSTR szFile )
{
//	_tcscpy_s( _szTempBuff, XE::x_szPackageDir );
	_tcscpy_s( _szTempBuff, XE::x_strPackage.c_str() );
	_tcscat_s( _szTempBuff, _T("/") );
	_tcscat_s( _szTempBuff, szLocalPath );
	_tcscat_s( _szTempBuff, szFile );
	return _szTempBuff;
}

#ifdef WIN32
const char* XE::MakePath( const char* cLocalPath, const char* cFile )
{
	strcpy_s( _cTempBuff, cLocalPath );
	strcat_s( _cTempBuff, cFile );
	return _cTempBuff;
}
const char* XE::MakeDocFullPath( const char* cLocalPath, const char* cFile )
{
	strcpy_s( _cTempBuff, GetPathDocA() );
	strcat_s( _cTempBuff, cLocalPath );
	strcat_s( _cTempBuff, cFile );
	return _cTempBuff;
}
const char* XE::MakeWorkFullPath( const char* cLocalPath, const char* cFile )
{
	strcpy_s( _cTempBuff, GetPathWorkA() );
	strcat_s( _cTempBuff, cLocalPath );
	strcat_s( _cTempBuff, cFile );
	return _cTempBuff;
}
const char* XE::GetPathDocA()
{
	// 일단은 워크패스로 사용.
	return x_cWorkDir;
}
#endif // win32

#ifdef WIN32
/*LPCTSTR XE::MakeDocumentPath( LPCTSTR szFilename )
{
	return XE::MakePath( DIR_SAVE, szFilename );
}
const char* XE::MakeDocumentPath( const char *szFilename )
{
	return XE::MakePath( DIR_SAVEA, szFilename );
}*/
#endif
#ifdef _VER_IOS
LPCSTR XE::MakeDocumentPath( LPCSTR szFilename )
{
    _tcscpy_s( _szTempBuff, x_szDocDir );
//    XIOS::GetDocumentPath( _szTempBuff );
    CHAR szLwr[64];
    strcpy_s( szLwr, szFilename );
    strlwr_s( szLwr );
    strcat_s( _szTempBuff, "/" );
    strcat_s( _szTempBuff, szLwr );
    return _szTempBuff;
}
#endif
#ifdef _VER_ANDROID
LPCTSTR XE::MakeDocumentPath( LPCTSTR szFilename )
{
	return XE::MakePath( "", szFilename );
}
#endif

/**
 스트링pBuff의 idxAt위치에 c를 써넣는다. 
 만약 idxAt위치가 널이었다면 idxAt+1위치에 자동으로 널을 넣어준다.
 */
void XE::StringSetAt( TCHAR *pBuff, int lenBuff, int idxAt, TCHAR c )
{
    // 무조건 버퍼영역 밖에는 쓸수 없다.
    if( XBREAK( idxAt >= lenBuff) )
        return;
    if( XBREAK( idxAt < 0 ) )
        return;
    // 널문자는 버퍼의 가장 끝에 들어갈수 있게 했고 그외의 문자는 널공간을 비워둬야 하므로 -1했다.
    if( XBREAK( c != 0 && idxAt >= lenBuff-1 ) )
        return;
    // 문자가 들어갈곳이 널이었다면 널을 한칸 뒤로 밀어준다.
    if( pBuff[ idxAt ] == 0 )
        pBuff[ idxAt + 1 ] = 0;
    pBuff[ idxAt ] = c;
}
//////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(_VER_IOS)
// 패키지가 있는 루트패스를 얻는다. ex) c:/t2/Resource/
// 안드로이드에선 사용불가
LPCTSTR XE::_GetPathPackageRoot()
{
//	return x_szPackageDir;
	return x_strPackage.c_str();
}
  #ifdef WIN32
const char* XE::GetPathWorkA()
{
	return x_cWorkDir;
}
  #endif // WIN32
#endif // win32 or ios
//////////////////////////////////////////////////////////////////////////
LPCTSTR XE::GetPathWork()
{
    return x_szWorkDir;
}

LPCTSTR XE::GetPathDoc()
{
    return x_szDocDir;
}

// szSrc의 상위 패스를 만든다.
// ex) c:/test/test -> c:/test
// 끝에 /나 스페이스 등이 붙어도 자동으로 잘라준다.
BOOL XE::MakeHighPath( TCHAR *pOut, int lenOut, LPCTSTR _szSrc )
{
#ifdef WIN32
	TCHAR dirMark = _T('\\');
#else
	char dirMark = '/';
#endif
	memset( pOut, 0, lenOut * sizeof(TCHAR) );
	int lenSrc = _tcslen( _szSrc );
	if( lenSrc == 1 && _szSrc[0] == dirMark )
	{
		pOut[0] = 0;
		return FALSE;
	}
	TCHAR szSrc[ 1024 ];
	_tcscpy_s( szSrc, _szSrc );
	// szSrc의 맨끝에 \나 스페이스가 있는지 검사해서 잘라냄.
	for( int i = lenSrc-1; i >= 0; --i )
	{
		TCHAR c = szSrc[i];
		if( c != dirMark && c != _T(' ') )
		{
			szSrc[i+1] = 0;
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
		pOut[i] = szSrc[i];
		if( szSrc + i == s || XBREAK( i >= lenOut-1 ) )
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
	memset( pOut, 0, lenOut * sizeof(char) );
	int lenSrc = strlen( _cSrc );
	if( lenSrc == 1 && IS_DIR_MARK( _cSrc[0] )  )
	{
		pOut[0] = 0;
		return FALSE;
	}
	char cSrc[ 1024 ];
	strcpy_s( cSrc, _cSrc );
	// cSrc의 맨끝에 \나 스페이스가 있는지 검사해서 잘라냄.
	for( int i = lenSrc-1; i >= 0; --i )
	{
		char c = cSrc[i];
		if( IS_NOT_DIR_MARK( c ) && c != ' ' )
		{
			cSrc[i+1] = 0;
			break;
		}
	}
	lenSrc = strlen( cSrc );	// 다시 계산.
	// cSrc의 뒤에서부터 검사해서 \위치의 포인터를 찾는다.
	const char* s1 = strrchr( cSrc, '\\' );
	const char* s2 = strrchr( cSrc, '/' );
	if( s1 == NULL && s2 == NULL )
		return FALSE;
	const char* s = ( (DWORD)s1 > (DWORD)s2 )? s1 : s2;
	for( int i = 0; i < lenSrc; ++i )
	{
		pOut[i] = cSrc[i];
		if( cSrc + i == s || XBREAK( i >= lenOut-1 ) )
		{
//			pOut[i] = 0;
			break;
		}
	}
	return TRUE;
}
#endif // win32
// szPath를 만든다. 만약 상위폴더가 없다면 실패하며 MakeRecursiveDir을 사용해야 한다.
BOOL XE::MakeDirToWork( LPCTSTR szPath )
{
	TCHAR szBuff[ 1024 ];
	_tcscpy_s( szBuff, x_szWorkDir );
	_tcscat_s( szBuff, szPath );
	if( _tmkdir( szBuff ) == 0 )
		return TRUE;
	return FALSE;
}
// szLocalPath는 파일명이 포함되선 안되며 로컬형태로 와야한다.
// ex) spr/ko/
BOOL XE::MakeRecursiveDirToWork( LPCTSTR szLocalPath )
{
	if( szLocalPath[0] == 0 )
		return TRUE;
	if( MakeDirToWork( szLocalPath ) == FALSE )
	{
//		XLOGXN("mkdir failed");
		// szLocalPath의 상위폴더명을 얻는다.
		TCHAR szUpPath[ 1024 ];
		MakeHighPath( szUpPath, 1024, szLocalPath );
//		XLOGXN("high path: %s", szUpPath );
		MakeRecursiveDirToWork( szUpPath );
		return MakeDirToWork( szLocalPath );
	}
	return TRUE;
}


#ifdef WIN32
// 이건 나중에 namespace XSYSTEM으로 보낼것.
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
#endif // win32

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#endif
namespace XSYSTEM
{
#ifdef WIN32
	// szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑는다.
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XVector<XFILE_INFO> *pOutAry, LPCTSTR szSubDir )
	{
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;

// 		TCHAR szFullpath[ 1024 ];
// 		_tcscpy_s( szFullpath, szPath );
// 		_tcscat_s( szFullpath, szWildcard );
		_tstring strFullpath = szPath;
		strFullpath += szWildcard;
		// c:\\test\\*.* 같은형식으로 만듬.
		if( ( hFind = FindFirstFile( strFullpath.c_str(), &FindFileData ) ) != INVALID_HANDLE_VALUE ) {
			do {
				// . 이나 .. 은 걸러냄
				if( XE::IsSame( FindFileData.cFileName, _T( "." ) ) == FALSE &&
					XE::IsSame( FindFileData.cFileName, _T( ".." ) ) == FALSE ) {
					// hidden속성도 걸러냄
					if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) == 0 ) {
						// 디렉토리면 리커시브로 들어감.
						if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
// 							TCHAR szSubPath[256];
// 							_tcscpy_s( szSubPath, szPath );
// 							_tcscat_s( szSubPath, FindFileData.cFileName );
// 							_tcscat_s( szSubPath, _T( "\\" ) );
							_tstring strSubPath = szPath;
							strSubPath += FindFileData.cFileName;
							strSubPath += _T( "\\" );
							_tstring strLocal = szSubDir;
							strLocal += FindFileData.cFileName;
							strLocal += _T( "\\" );
							GetFileList( strSubPath.c_str(), szWildcard, pOutAry, strLocal.c_str() );
						} else {
							XFILE_INFO info;
							info.strFile = szSubDir;
							info.strFile += FindFileData.cFileName;
							XBREAK( FindFileData.nFileSizeHigh != 0 );
							info.size = FindFileData.nFileSizeLow;		// 32비트 크기 파일만 지원
							pOutAry->Add( info );
							XTRACE( "%s", info.strFile.c_str() );
						}
					}
				}
			} while( FindNextFile( hFind, &FindFileData ) );
			FindClose( hFind );
		}
		return pOutAry->size();
	}
	//
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XVector<XFILE_INFO> *pOutAry )
	{
//		XArrayLinearN<XFILE_INFO, 2048> ary;
		//
		GetFileList( szPath, szWildcard, pOutAry, _T( "" ) );
		//
// 		if( pOutAry->GetMaxSize() == 0 )
// 			pOutAry->Create( ary.size() );
// 		XARRAYLINEARN_LOOP( ary, XFILE_INFO, info )
// 		{
// 			pOutAry->Add( info );
// 		} END_LOOP;
		//
		return pOutAry->size();
	}	// szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑는다.

	// szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑는다.
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinearN<XFILE_INFO, 2048> *pOutAry, LPCTSTR szSubDir ) 
	{
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;

		TCHAR szFullpath[ 256 ];
		_tcscpy_s( szFullpath, szPath );
		_tcscat_s( szFullpath, szWildcard );
		// c:\\test\\*.* 같은형식으로 만듬.
		if( (hFind = FindFirstFile( szFullpath, &FindFileData)) != INVALID_HANDLE_VALUE )
		{
			do
			{
				// . 이나 .. 은 걸러냄
				if( XE::IsSame( FindFileData.cFileName, _T(".") ) == FALSE &&
					XE::IsSame( FindFileData.cFileName, _T("..") ) == FALSE )
				{
					// hidden속성도 걸러냄
					if( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
					{
						// 디렉토리면 리커시브로 들어감.
						if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							TCHAR szSubPath[ 256 ];
							_tcscpy_s( szSubPath, szPath );
							_tcscat_s( szSubPath, FindFileData.cFileName );
							_tcscat_s( szSubPath, _T("\\") );
							_tstring strLocal = szSubDir;
							strLocal += FindFileData.cFileName;
							strLocal += _T("\\");
							GetFileList( szSubPath, szWildcard, pOutAry, strLocal.c_str() );
						} else
						{
							XFILE_INFO info;
							info.strFile = szSubDir;
							info.strFile += FindFileData.cFileName;
							XBREAK( FindFileData.nFileSizeHigh != 0 );
							info.size = FindFileData.nFileSizeLow;		// 32비트 크기 파일만 지원
							pOutAry->Add( info );
							XTRACE( "%s", info.strFile.c_str() );
						}
					}
				}
			} while( FindNextFile( hFind, &FindFileData ) );
			FindClose( hFind );
		}
		return pOutAry->size();
	}
	//
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinear<XFILE_INFO> *pOutAry ) 
	{
		XArrayLinearN<XFILE_INFO, 2048> ary;
		//
		GetFileList( szPath, szWildcard, &ary, _T("") );
		//
		if( pOutAry->GetMaxSize() == 0 )
			pOutAry->Create( ary.size() );
		XARRAYLINEARN_LOOP( ary, XFILE_INFO, info )
		{
			pOutAry->Add( info );
		} END_LOOP;
		//
		return pOutAry->size();
	}	// szPath와 그하위에 있는 모든 폴더의 파일들의 목록을 뽑는다.
	//
	int GetFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinearN<_tstring, 2048> *pOutAry, LPCTSTR szSubDir ) 
	{
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;

		TCHAR szFullpath[ 256 ];
		_tcscpy_s( szFullpath, szPath );
		_tcscat_s( szFullpath, szWildcard );
		// c:\\test\\*.* 같은형식으로 만듬.
		if( (hFind = FindFirstFile( szFullpath, &FindFileData)) != INVALID_HANDLE_VALUE )
		{
			do
			{
				// . 이나 .. 은 걸러냄
				if( XE::IsSame( FindFileData.cFileName, _T(".") ) == FALSE &&
					XE::IsSame( FindFileData.cFileName, _T("..") ) == FALSE )
				{
					// hidden속성도 걸러냄
					if( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
					{
						// 디렉토리면 리커시브로 들어감.
						if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							TCHAR szSubPath[ 256 ];
							_tcscpy_s( szSubPath, szPath );
							_tcscat_s( szSubPath, FindFileData.cFileName );
							_tcscat_s( szSubPath, _T("\\") );
							_tstring strLocal = szSubDir;
							strLocal += FindFileData.cFileName;
							strLocal += _T("\\");
							GetFileList( szSubPath, szWildcard, pOutAry, strLocal.c_str() );
						} else
						{
							_tstring str = szSubDir;
							str += FindFileData.cFileName;
							pOutAry->Add( str );
							XTRACE( "%s", str.c_str() );
						}
					}
				}
			} while( FindNextFile( hFind, &FindFileData ) );
			FindClose( hFind );
		}
		return pOutAry->size();
	}
	//
	int CreateFileList( LPCTSTR szPath, LPCTSTR szWildcard, XArrayLinear<_tstring> *pOutAry ) 
	{
		XArrayLinearN<_tstring, 2048> ary;
		//
		GetFileList( szPath, szWildcard, &ary );
		//
		if( pOutAry->GetMaxSize() == 0 )
			pOutAry->Create( ary.size() );
		XARRAYLINEARN_LOOP( ary, _tstring, str )
		{
			pOutAry->Add( str );
		} END_LOOP;
		//
		return pOutAry->size();
	}
#endif // win32
#ifndef WIN32
	int GetFileList_Test()
	{
/*		const char* pathDoc = XE::GetPathPackageRoot();
		//	const char* pathDoc = XE::MakeDocFullPath("", "");
		DIR *dp = opendir( pathDoc );
		if( dp )
		{
			struct dirent* entry = NULL;
			while(1)
			{
				entry = readdir( dp );
				if( entry == NULL )
					break;
				XLOGXN("%s", entry->d_name );
			}
			closedir( dp );
		} */
		return 1;
	}
#endif // not win32
};

// package
