#include "stdafx.h"
#include "XPropResearch.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XPropResearch *PROP_RESEARCH = NULL;

XPropResearch::XPropResearch() 
{ 
	Init(); 
}

void XPropResearch::Destroy() 
{
	std::map<_tstring, xPROP*>::iterator itor;
	for( itor = m_mapData.begin(); itor != m_mapData.end(); itor++ )
	{
		xPROP *pProp = itor->second;
		SAFE_DELETE( pProp );
	}
}

BOOL XPropResearch::ReadProp( CToken& token )
{

	ID idProp = TokenGetNumber( token );
	if( GetpProp( idProp ) )
	{
#ifndef _VER_IOS
		XERROR( "%s: duplication Mob ID. %d", GetstrFilename().c_str(), idProp );
#endif
		return FALSE;
	}
	if( idProp == TOKEN_EOF )
		return FALSE;		// false를 리턴해야 루프를 빠져나옴
	
	xPROP *pProp = new xPROP;		
	pProp->idProp = idProp;				// ID
	XBREAK( pProp->idProp > 0xffff );		// 이렇게 큰숫자를 쓰는건 권장하지 않음.
	
	pProp->strIdentifier = GetTokenIdentifier( token );					// 식별자
	int idText			= TokenGetNumber( token );						// 텍스트 ID
	pProp->strName		= XTEXT(idText);								// 이름
	pProp->attack		= token.GetNumberF();							// 공격력
	pProp->defense		= token.GetNumberF();							// 방어력
	pProp->hp			= token.GetNumberF();							// 체력
	pProp->attackSpeed	= token.GetNumberF();							// 공격속도
	pProp->moveSpeed		= token.GetNumberF();							// 이동속도
	
	pProp->strSpr = GetTokenString( token );							// spr 파일
	if( pProp->strSpr.empty() == false )
		pProp->strSpr += _T(".spr");
	else
		CONSOLE( "warning: identifier=%s, szSpr is empty", pProp->strIdentifier.c_str() );
	pProp->strIcon = GetTokenString( token );							// 아이콘 파일
	if( pProp->strIcon.empty() == false )
		pProp->strIcon += _T(".png");

	// 추가
	Add( pProp->strIdentifier, pProp );

	return TRUE;
}

void XPropResearch::OnDidFinishReadProp( void )
{
	int size = m_mapData.size();
	if( size > 0 )
	{
		m_aryProp.Create( size );
		std::map<_tstring, xPROP*>::iterator itor;
		for( itor = m_mapData.begin(); itor != m_mapData.end(); ++itor )
		{
			xPROP *pProp = itor->second;
			XBREAK( pProp == NULL );
			m_aryProp.Add( pProp );
		}
	}
}

void XPropResearch::Add( _tstring& strIdentifer, xPROP *pProp )
{
	m_mapData[ strIdentifer ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
}

XPropResearch::xPROP* XPropResearch::GetpProp( LPCTSTR szIdentifier )
{
	TCHAR szBuff[ 256 ];
	_tcscpy_s( szBuff, szIdentifier );
	_tcslwr_s( szBuff );
	std::map<_tstring, xPROP*>::iterator itor;
	itor = m_mapData.find( szBuff );
	if( itor == m_mapData.end() )
		return NULL;							// 못찾았으면 널 리턴
	xPROP* pProp = (*itor).second;
	XBREAK( pProp == NULL );			// 위에서 find로 검사했기때문에 NULL나와선 안됨.
	return pProp;
}

XPropResearch::xPROP* XPropResearch::GetpProp( ID idProp )
{
	if( idProp == 0 )
		return NULL;
	std::map<ID, xPROP*>::iterator itor;
	itor = m_mapID.find( idProp );
	if( itor == m_mapID.end() ) {
		return NULL;							// 못찾았으면 에러 리턴
	}
	xPROP *pProp = (*itor).second;
	XBREAK( pProp == NULL );			// 위에서 find로 검사했기때문에 NULL나와선 안됨.
	return pProp; 
}


// szName의 이름을 갖는 프로퍼티를 찾아서 돌려준다.
XPropResearch::xPROP* XPropResearch::GetpPropFromName( LPCTSTR szName )
{
    XBREAK( szName == NULL );
    XBREAK( XE::IsEmpty( szName ) );
    int num = m_aryProp.size();
    for( int i = 0; i < num; ++i )
    {
        xPROP *pProp = m_aryProp[i];
        if( XE::IsSame( pProp->strName.c_str(), szName ) )
            return pProp;
    }
    return NULL;
}


