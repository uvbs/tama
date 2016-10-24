#include "stdafx.h"
#include "XFramework/XEOption.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEOption* XEOption::s_pInstance = nullptr;

////////////////////////////////////////////////////////////////
/**
 @brief 절대 패스로 읽는 버전
*/
bool XEOption::LoadAbs( LPCTSTR szFile )
{
	m_strFile = szFile;
	CToken token;
	if( token.LoadAbsolutePath( szFile, XE::TXT_EUCKR) == xFAIL ) {
		return false;
	}
	while( token.GetToken() ) {
		const std::string strcKey = SZ2C( token.m_Token );
		token.GetToken();	// =
		XBREAK( token.m_Token[0] != '=' );
		Parse( strcKey, token );
	}
	return true;
}

bool XEOption::Load( LPCTSTR szFile )
{
	m_strFile = szFile;
	CToken token;
	if( token.LoadFromDoc( szFile, XE::TXT_EUCKR ) == xFAIL ) {
		return false;
	}
	while( token.GetToken() ) {
		const std::string strcKey = SZ2C( token.m_Token );
		token.GetToken();	// =
		XBREAK( token.m_Token[ 0 ] != '=' );
		Parse( strcKey, token );
	}
	return true;
}
/**
 @brief key/value형태로 한꺼번에 읽는버전.
*/
bool XEOption::Load2( LPCTSTR szFile )
{
	m_strFile = szFile;		// 최초 실행시에는 option.ini가 없기때문에 실패 return 이전에 위치해야한다.
	CToken token;
	if( token.LoadAbsolutePath( szFile, XE::TXT_EUCKR ) == xFAIL ) {
		return false;
	}
//	m_strFile = szFile;		// 여기다 놔두면 안됨. 
	while( token.GetToken() ) {
		const std::string strcKey = SZ2C( token.m_Token );
		token.GetToken();	// =
		XBREAK( token.m_Token[0] != '=' );
		token.GetToken();
		const std::string strcVal = SZ2C( token.m_Token );
		m_Param.Set( strcKey, strcVal );
	}
	if( m_pDelegate )
		m_pDelegate->DelegateDidFinishLoad( m_Param );
	DidFinishLoad( m_Param );
	return true;
}

bool XEOption::Save()
{
	FILE *fp = nullptr;
	LPCTSTR _szPath = XE::MakeDocFullPath( _T( "" ), m_strFile.c_str() );
	std::string strPath = SZ2C( _szPath );
	CONSOLE("option save:%s", _szPath );
	fopen_s( &fp, strPath.c_str(), "wt" );
	if( fp ) {
		std::string str;
		Write( &str );
		fputs( str.c_str(), fp );
		fclose( fp );
		XLOGXNA("saved option:%s", str.c_str() );
		return true;
	}
	return false;
}
