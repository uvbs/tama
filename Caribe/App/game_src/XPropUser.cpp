#include "stdafx.h"
#include "XPropUser.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if defined(_CLIENT) || defined(_GAME_SERVER)

std::shared_ptr<XPropUser> XPropUser::s_spInstance;
std::shared_ptr<XPropUser>& XPropUser::sGet() 
{
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XPropUser>( new XPropUser );
	return s_spInstance;
}

void XPropUser::Destroy() 
{
	for( auto pProp : m_aryProp ) {
		SAFE_DELETE( pProp );
	}
}

BOOL XPropUser::ReadProp( CToken& token, DWORD dwParam )
{
	int lv = token.GetNumber();
	if( lv == TOKEN_EOF )
		return FALSE;		// false를 리턴해야 루프를 빠져나옴
	
	auto pProp = new xPropUser::xPROP;		
	pProp->lvAcc = lv;				// lvAcc
	pProp->goldLoot = token.GetNumber();					// 식별자
	pProp->expMax = token.GetNumber();
	pProp->apPerBattle = (int)(token.GetNumber() /* * 3.5f*/);		// 밸런스 조절중.
	pProp->apMax = token.GetNumber();
	// 추가
	Add( lv, pProp );

	return TRUE;
}

void XPropUser::Add( int lv, xPropUser::xPROP *pProp )
{
	m_aryProp[ lv ] = pProp;
}

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
