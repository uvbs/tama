// MakePackage.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <conio.h>
#include "XFramework/XConsoleMain.h"
#include "XResObj.h"
#include "xeDef.h"
#include "XSystem.h"

/**
 플랫폼별로 폴더를 만들어 패키지폴더를 만든다.
*/

class XMain : public XConsoleMain
{
	_tstring m_strResource;
	_tstring m_strPackageRoot;
	_tstring m_strFileCoreVer;
//	_tstring m_strFileThemeVer;		// 기본테마팩 버전파일
	void Init() {}
	void Destroy() {}
public:
	XMain() { Init(); }
	virtual ~XMain() { Destroy(); }
	//
	void SetPath( void );
	void DoPackaging( void );
	void GetPackageFiles( XList4<_tstring> *pOut );
	void AddFileList( LPCTSTR szFile, XList4<_tstring> *pOut );
	void CopyFilesToPackage( XList4<_tstring>& list, LPCTSTR szPlatform );
};


XMain *MAIN = NULL;

int _tmain( int argc, _TCHAR* argv[] )
{
	// 엔진 초기화
	MAIN = new XMain;
	MAIN->Create();
	// 패키징 추출
	MAIN->DoPackaging();
	return 0;
}

void XMain::DoPackaging( void )
{
	SetPath();
	// PatchFile/폴더의 리스트 파일을 참조하여 패키지파일 목록을 작성함.
	XList4<_tstring> listPackage;
	GetPackageFiles( &listPackage );
	// ini를 기반으로 패키지파일을 패키지폴더로 옮긴다. 플랫폼별로 따로 패키징 실행할수 있게 할것.
	CopyFilesToPackage( listPackage, _T( "Package_iOS\\" ) );
	CopyFilesToPackage( listPackage, _T( "Package_Android\\" ) );
	// 
}


void XMain::SetPath( void )
{
	TCHAR szWorkPath[ 1024 ];
	GetCurrentDirectory( MAX_PATH, szWorkPath );		// 현재 작업폴더를 읽는다
	_tcscat_s( szWorkPath, _T("\\") );
//	m_strResource = szExePath;
// 	_tprintf(_T("\nGetCurrentDirectory:%s\n"), szExePath );
//	exe파일은 bin같은데다 공통으로 넣어두고 ..\bin\MakePackage.exe 와같은식으로 실행할수 있도록.
// 	CToken token;
// 	_tstring strPath = szWorkPath;
// 	strPath += _T( "\\MakePackage.ini" );
// 	if( !token.LoadAbsolutePath( strPath.c_str(), XE::TXT_EUCKR ) )
// 	{
// 		_tprintf(_T("not found %s"), strPath.c_str() );
// 		exit(1);
// 	}
// 	m_strResource = token.GetTokenPath();	// 리소스 패스를 읽음.
// 	m_strResource += _T("patchFile\\");
// 	auto charLast = m_strResource.back();
// 	if( charLast != '\\' )
// 		m_strResource += _T( "\\" );
	m_strResource = szWorkPath;
	m_strResource += _T("patchFile\\");

	// ini를 읽어서 패키지 루트 폴더를 지정한다.
/*
	CToken token;
	_tstring strINI = szExePath;
	strINI += _T("MakePackage.ini");
	if( token.LoadAbsolutePath( strINI.c_str(), XE::TXT_EUCKR ) == xFAIL )
	{
		XALERT( "%s not found", strINI.c_str() );
		exit(1);
	}
	TCHAR szPackage[ 1024 ] = { 0, };
	_tcscpy_s( szPackage, token.GetToken() );
*/
// 	TCHAR szPackage[] = _T("Package\\");
/*
	int len = _tcslen( szPackage );
	// 끝에 /로 끝나지 않으면 붙여준다.
	if( szPackage[ len - 1 ] != '\\' )
	{
		szPackage[ len ] = '\\';
		szPackage[ len + 1 ] = 0;
	}
*/
	m_strPackageRoot = _T("Package\\");
	//
	m_strFileCoreVer = szWorkPath;
	m_strFileCoreVer += _T("Update\\core\\LastUpdate.txt");
//	m_strFileThemeVer = szExePath;
//	m_strFileThemeVer += _T("Update\\theme_ne\\LastUpdate.txt");
}

void XMain::GetPackageFiles( XList4<_tstring> *pOut )
{
	_tstring strFile = m_strResource;
	strFile += _T("apk_list.txt");
	AddFileList( strFile.c_str(), pOut );
	//
//	strFile = m_strResource;
//	strFile += _T("theme_ne_list.txt");
//	AddFileList( strFile.c_str(), pOut );
	//
}

void XMain::CopyFilesToPackage( XList4<_tstring>& list, LPCTSTR szPlatform )
{
	for( auto& strFile : list )
	{
		_tstring strSrc = m_strResource;
		strSrc += strFile;
		_tstring strDst = m_strPackageRoot;
		strDst += szPlatform;
		strDst += strFile;
		TCHAR szPath[ 1024 ];
		_tcscpy_s( szPath, XE::GetFilePath( strDst.c_str() ) );
		// 폴더를 먼저 만든다.
		XSYSTEM::MakeRecursiveDir( szPath );
		XSYSTEM::CopyFileX( strSrc.c_str(), strDst.c_str() );
		CONSOLE("copy to.....%s", strDst.c_str() );
		}
	//
	// 패키지에 포함안시키고 파일이 없으면 최초 다운이라고 인식하도록 바꿈.
// 	_tstring strDst = m_strPackageRoot;
// 	strDst += szPlatform;
// 	strDst += _T("update\\core\\LastUpdate.txt");
// 	XSYSTEM::MakeRecursiveDir( XE::GetFilePath( strDst.c_str() ) );
// 	XSYSTEM::CopyFileX( m_strFileCoreVer.c_str(), strDst.c_str() );
	//
}

void strReplace( TCHAR *szBuff, TCHAR szSrc, TCHAR szDst )
{
	int len = _tcslen( szBuff );
	for( int i = 0; i < len; ++i )
	{
		if( szBuff[ i ] == szSrc )
			szBuff[ i ] = szDst;
	}
}
void XMain::AddFileList( LPCTSTR szFile, XList4<_tstring> *pOut )
{
	CToken token;
	if( token.LoadAbsolutePath( szFile, XE::TXT_EUCKR ) == xFAIL )
	{
		XALERT( "%s not found", szFile );
		exit(1);
	}
	while( token.GetToken() )
	{
		strReplace( token.m_Token, '/', '\\' );
		pOut->Add( _tstring( token.m_Token ) );
	}
}

