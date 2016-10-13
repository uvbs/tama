#include "stdafx.h"
#include "XFramework/XEBaseProp.h"
#include "XFramework/XConstant.h"
#include "XFramework/client/XClientMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

int XEBaseProp::TokenGetNumber( CToken& token )
{
	token.GetNextToken();		// 포인터 이동없이 다음 토큰 읽어보고
	if( token.m_NextToken[0] == 0 )
		return TOKEN_EOF;
	XBREAK( CONSTANT == NULL );
	int num = CONSTANT->GetValue( token.m_NextToken );	// 상수인지확인
	if( num == XConstant::xERROR )
	{
		// 상수가 아니라서 숫자형으로 다시 읽어본다.
		num = token.GetNumber();
		if( num == TOKEN_ERROR )
		{
			// 그래도 에러라면 잘못읽은것임
			XERROR( "%s: line number %d\r\ninvalid number=%s", 
				XE::GetFileName( token.m_strFileName ),
				token.GetCurrLine(),
				token.m_Token );
			// defineGame.h에 enum이 있는데도 못찾는 경우는 마지막 enum뒤에 ,가 없어서임.
			return TOKEN_ERROR;
		}
	} else
		token.GetToken();
	return num;
}

// szIdentifier읽기 전용 함수
LPCTSTR XEBaseProp::GetTokenIdentifier( CToken& token )
{
	auto pToken = token.GetToken();
	if( pToken == nullptr )
		return _T("");
	_tcscpy_s( m_szBuff, pToken );
	_tcslwr_s( m_szBuff );		// identifer는 모두 소문자로 씀
	return m_szBuff;
}

// 스트링 항목 읽기 전용함수
LPCTSTR XEBaseProp::GetTokenString( CToken& token )
{
	LPCTSTR szStr = token.GetToken();
	if( szStr[0] == '=' )
		return _T("");
	return szStr;
}

ID XEBaseProp::GetTokenID( CToken& token )
{
	int idNum = token.GetNumber();
	if( idNum < 0 )
		idNum = 0;
	return idNum;
}

// 공통 로딩부
BOOL XEBaseProp::Load( LPCTSTR szFilename, DWORD dwParam )
{
	XLOAD_PROFILE1;
	m_strFilename = szFilename;
	CToken token;
	// 국가별폴더부터 읽어보고 없으면
	if( token.LoadFile( XE::MakePathLang( DIR_PROP, szFilename ), XE::TXT_UTF16 ) == xFAIL )
	{
		// 유니버셜 폴더에서 읽음.
		if( token.LoadFile( XE::MakePath( DIR_PROP, szFilename ), XE::TXT_UTF16 ) == xFAIL )
		{
			XERROR( "%s read failed", szFilename );
			return FALSE;
		}
	}
	OnDidBeforeReadProp( token );
	int count = 0;
	while(1)
	{
		if( ReadProp( token,dwParam ) == FALSE )
			break;
		// 에러검출
		++count;
	}
	//
	OnDidFinishReadProp( token );
	// 아이템 총 개수 세어 배열 생성
	int num = GetSize();	// virtual
	CONSOLE( "%s loaded....%d", szFilename, num );
	XLOAD_PROFILE2;
	XLOGP( "%s, %llu", XE::GetFileName( szFilename ), __llPass );
	return TRUE;
}
