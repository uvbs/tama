#include "stdafx.h"
#include "XFramework/XConstant.h"
#include "etc/Token.h"
#include "XLua.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XConstant *CONSTANT = NULL;

XConstant::~XConstant() 
{
	Destroy();
}

void XConstant::Destroy()
{
	for( auto itor = m_mapData.begin(); itor != m_mapData.end(); itor++ ) {
		xCONSTANT *pConst = itor->second;
		SAFE_DELETE( pConst );
	}
	m_mapData.clear();
}

//
// Constant.h를 읽어 #define name value들을 모두 메모리에 적재
BOOL XConstant::Load( LPCTSTR szFile )
{
	CToken token;
	TCHAR szName[ 256 ];
	
//	AXLOGXN("try constant:%s", szFile);
	if( token.LoadFile( XE::MakePath( DIR_ROOT, szFile), XE::TXT_EUCKR ) == xFAIL ) {
		XALERT( "%s file not found", szFile );
		return FALSE;
	}
	while(1){
		if( token.GetToken() == NULL )	break;
		if( token == _T("define") )	{
			token.GetToken();		
			// ex) #define OBJECT_DAO	1
			_tcscpy_s( szName, token.m_Token );		
			int val = token.GetNumber();
//    AXLOGXN("%s=%d",szName,val);
			if( val != TOKEN_ERROR ) 
				Add( szName, val, NULL );
		} else
		if( token == _T("enum") )	{
			LoadEnum( token );
		}
	}
    XLOGXN("constant loading success.....%s", szFile );
	return xSUCCESS;
}

int XConstant::TokenGetNumber( CToken& token )
{
	token.GetNextToken();		// 포인터 이동없이 다음 토큰 읽어보고
	if( token.m_NextToken[ 0 ] == 0 )
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
	}
	else
		token.GetToken();
	return num;
}


// enum 블럭 읽음.
BOOL XConstant::LoadEnum( CToken& token )
{
	int numBlock = 0;
	int numAuto = 0;		// enum의 자동으로 올라가는 숫자.
	BOOL bFirst = TRUE;
	TCHAR szEnum[ 256 ];
	while(1)
	{
		if( token.GetToken() == NULL )		break;		// eof
		if( token == _T("{") )
			++numBlock;
		else if( token == _T("}") ) {
			--numBlock;
			if( numBlock < 0 )	// } 가 { 보다 더 많은 경우.
			{
				XERROR( "%s: { }블럭의 짝이 맞지 않음.", token.m_strFileName );
				break;
			} else if( numBlock == 0 )
				return TRUE;
		} else	{
			if( numBlock >= 1 )		// 블럭안에 있는것만 읽어야 함.
			{
				_tcscpy_s( szEnum, token.m_Token );		// enum 상수
				// 여기에 ,가 올수도 있고 =0 같은게 올수도 있다.
				token.GetNextToken();		// 이름뒤에 하나 더 읽어봄
				if( token.m_NextToken[0] == _T(',') )		// ,면 상수추가하고 다음으로 넘어감
				{
					token.GetToken();	// ,
					if( bFirst )		// 바로 ,가 나왔는데 첫줄이면 자동으로 0
						numAuto = 0;
					Add( szEnum, numAuto++ );		// 자동증가 상수
				} else
				if( token.m_NextToken[0] == _T('}') )
				{
				} else
				{
					token.GetToken();
					XBREAK( token.m_Token[0] != _T('=') );		// =여야 한다.
					int num = TokenGetNumber( token ); //token.GetNumber();
					// 중간에 값이 나왔을때를 위해 뺌.
//					if( bFirst )			// =n이 나왔는데 그게 첫줄이었다면 초기값으로 첫줄값을 지정
						numAuto = num;
					Add( szEnum, num );
					++numAuto;
					token.GetToken();	// ,
				}
				bFirst = FALSE;			// enum의 첫줄
			}
		}
	}
	// } 블럭이 모자랄경우 eof까지 가버린다. }을 누락한것이다.
	XERROR( "%s: { }블럭의 짝이 맞지 않음.", token.m_strFileName );
	return FALSE;
}

void XConstant::Add( LPCTSTR szName, int value, LPCTSTR szRemark )
{
	auto itor = m_mapData.find( _tstring(szName) );
	if( itor == m_mapData.end() ) {
		xCONSTANT *pConst = new xCONSTANT;
		pConst->strName = szName;
		pConst->value = value;
		if( szRemark )
			pConst->strRemark = szRemark;
		m_mapData[ _tstring( szName ) ] = pConst;
	} else {
#if !defined(_XSINGLE)
		XBREAK( itor != m_mapData.end() );
#endif 
	}
}

void XConstant::xCONSTANT::Serialize( XArchive& ar ) const {
	ar << strName;
	ar << strRemark;
	ar << value;
}
void XConstant::xCONSTANT::DeSerialize( XArchive& ar, int ) {
	ar >> strName >> strRemark >> value;
}

void XConstant::Serialize( XArchive& ar ) const
{
	ar << VER_CONSTANT;
	ar << (int)m_mapData.size();
	for( auto itor : m_mapData ) {
		xCONSTANT* pConst = itor.second;
		ar << (*pConst);
	}
}
void XConstant::DeSerialize( XArchive& ar, int )
{
	AXLOGXN( "%s", __TFUNC__ );
	Destroy();
	int ver, num;
	ar >> ver >> num;
	for( int i = 0; i < num; ++i ) {
		auto pConst = new xCONSTANT();
		pConst->DeSerialize( ar, ver );
		m_mapData[ pConst->strName ] = pConst;
	}
}


#ifdef _XUSE_LUA
void XConstant::RegisterScript( XLua *pLua )
{
	std::map<_tstring, xCONSTANT*>::iterator itor;
	for( itor = m_mapData.begin(); itor != m_mapData.end(); itor++ )
	{
		xCONSTANT *pConst = itor->second;
		const char *cBuff = Convert_TCHAR_To_char( pConst->strName.c_str() );
		pLua->RegisterVar( cBuff, pConst->value );
	}
}
#endif






