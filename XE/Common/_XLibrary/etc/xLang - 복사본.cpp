#include "stdafx.h"
#include "xLang.h"

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
BOOL XLang::LoadINI( LPCTSTR resIni )
{
	CToken token;
	if( token.LoadFile( resIni, XE::TXT_EUCKR ) == xFAIL ) {
		return FALSE;
	}
	LPCTSTR szKey = token.GetToken();
	if( szKey == NULL )
	{
		CONSOLE("%s: lang key read error", resIni );
		return FALSE;
	}
	m_strKey = szKey;
	LPCTSTR szFolder = token.GetToken();
	if( szFolder == NULL )
	{
		CONSOLE("%s: lang folder read error", resIni );
		Init();		// 디폴트 값으로 다시 채움
		return FALSE;
	}
	CONSOLE( "lang key=%s folder=%s", m_strKey.c_str(), szFolder );
	SetszFolder( szFolder );
	return TRUE;
}





