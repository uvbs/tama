#include "stdafx.h"
#include "xLang.h"
#include "xeDef.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace XE 
{
	XLang LANG;
/*
	xtLang		g_Lang;					// 언어 옵션
#ifdef WIN32
	LPCTSTR	g_dirLang[] = { 0,
									_T("ko\\"),
									_T("en\\"),
									_T("jp\\"),
									_T("ch\\"), };
#else
	LPCTSTR	g_dirLang[] = { 0,
		_T("ko/"),
		_T("en/"),
		_T("jp/"),
		_T("ch/"), };
#endif
*/
};

void XLang::Init() 
{
	m_strKey = _T( "english" );
	m_strFolder = _T( "en" );
	TCHAR szBuff[32];
	_tcscpy_s( szBuff, m_strFolder.c_str() );
	_tcscat_s( szBuff, _T( "/" ) );
	XPLATFORM_PATH( szBuff );
	m_strPath = szBuff;
}

// szRes 패스를 국가별 폴더버전으로 만들어서 되돌려준다. ui/ -> ui/ko/
LPCTSTR XLang::ChangeToLangDir( LPCTSTR szRes, TCHAR *pOut, int sizeOut )
{
	_tcscpy_s( pOut, sizeOut, XE::GetFilePath( szRes ) );		// 패스만 따로 떼냄
	_tcscat_s( pOut, sizeOut, GetLangDir() );
	_tcscat_s( pOut, sizeOut, XE::GetFileName( szRes ) );		// 
	return pOut;
}
#ifdef WIN32
const char* XLang::ChangeToLangDir( const char *cPath, char *pOut, int sizeOut )
{
	strcpy_s( pOut, sizeOut, XE::GetFilePath( cPath ) );		// 패스만 따로 떼냄
	strcat_s( pOut, sizeOut, SZ2C( GetLangDir() ) );
	strcat_s( pOut, sizeOut, XE::GetFileName( cPath ) );		// 
	return pOut;
}
#endif

_tstring XLang::MakeFolderPath( LPCTSTR szFolder ) 
{
	XBREAK( XE::IsEmpty(szFolder) );
	_tstring strPath = szFolder;
	strPath += _T(PATH_SLASH);
	return strPath;
}

bool XLang::LoadINI( LPCTSTR resIni )
{
	CToken token;
	if( token.LoadFile( resIni, XE::TXT_EUCKR ) == xFAIL ) {
		return false;
	}
	while( token.GetToken() ) {
		xLang lang;
		lang.m_strcKey = SZ2C( token.m_Token );
		XBREAK( lang.m_strcKey.empty() );
		lang.m_strcDir = SZ2C( token.GetToken() );
		XBREAK( lang.m_strcDir.empty() );
		lang.m_strPath = MakeFolderPath( token.m_Token );
		XBREAK( lang.m_strPath.empty() );
		m_aryLangs.Add( lang );
	}
	return true;
}



// lang.txt읽기
// 게임최초 시작시 option읽어서 언어 선택시키기
// 잘돌아가는지 테스트
// 옵션에서 국가 선택하면 현재 언어 바꾸기
// text_xx.txt 다시 읽기


