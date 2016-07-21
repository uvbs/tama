#include "stdafx.h"
#include "xSkill.h"
#include "xLib.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )
void xEffSfx::Serialize( XArchive& ar ) const {
	ar << m_strSpr;
	XBREAK( m_idAct > 0xff );
	ar << (BYTE)m_idAct;
	ar << (char)m_Point;
	ar << (char)m_Loop;
	ar << (char)0;
}
void xEffSfx::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	char c0;
	ar >> m_strSpr;
	ar >> b0;		m_idAct = b0;
	ar >> c0;		m_Point = (xtPoint)c0;
	ar >> c0;		m_Loop = (xtAniLoop)c0;
	ar >> c0;
}

/*	int ExecuteInvokeScript( XLua *pLua, const char *szScript ) {
		// 스트링이 없는건 걍 리턴
		if( szScript == NULL )	return 0;
		if( szScript[0] == 0 )	return 0;
		if( XBREAK( pLua == NULL ) )		return 0;
		// 시작 스크립트 로드해서 실행
	#pragma message( "매번 LoadStr을 할수 없으니 최적화 해야함" )
		// 시작/실행/도트/끝을 하나의 스트링으로 만들면서 각각을 function으로 구분. 게임에선 펑션콜을 하는방식. 효과가 여러개이므로 앞에 효과번호가 붙게 하는것도 방법
		pLua->LoadStr( szScript );
		pLua->PCall();
		return 1;
	} */
	/**
 @brief pEffect가 발동확률이 있을때 주사위를 굴려 발동여부를 판정한다.
*/
bool DoDiceInvokeRatio( const EFFECT *pEffect, int level )
{
	bool bSuccess = true;
	float ratio = 0;
	if( pEffect->aryInvokeRatio.size() == 0 ) {
		ratio = 1.f;
	} else {
		XBREAK( level == 0 );
		if( pEffect->aryInvokeRatio.size() == 1 )	// 테이블형태로 안넣고 숫자하나만 넣었을경우.
			ratio = pEffect->aryInvokeRatio[ 0 ];
		else
			ratio = pEffect->aryInvokeRatio[ level ];
	}
	if( ratio < 1.0f 
			&& ( pEffect->invokeJuncture != xJC_PERSIST 
					|| pEffect->secInvokeDOT > 0 
					|| pEffect->strInvokeSkill.empty() == false 
					|| pEffect->idInvokeSkill ) )
	{
		DWORD dice = xRandom( 1000 );
		DWORD ratioInvoke = (DWORD)( 1000 * ratio );
		if( dice >= ratioInvoke )
			bSuccess = false;
	}
	return bSuccess;
}

/**
 @brief 발동적용확률
*/
bool DoDiceInvokeApplyRatio( const EFFECT *pEffect, int level )
{
	auto& aryRatio = pEffect->m_aryInvokeApplyRatio;
	bool bSuccess = true;
	float ratio = 0;
	if( aryRatio.size() == 0 )
		ratio = 1.f;
	else {
		XBREAK( level == 0 );
		if( aryRatio.size() == 1 )	// 테이블형태로 안넣고 숫자하나만 넣었을경우.
			ratio = aryRatio[0];
		else
			ratio = aryRatio[level];
	}
	if( ratio < 1.0f
		&& ( pEffect->invokeJuncture != xJC_PERSIST
		|| pEffect->secInvokeDOT > 0
		|| pEffect->strInvokeSkill.empty() == false
		|| pEffect->idInvokeSkill ) ) {
		DWORD dice = xRandom( 1000 );
		DWORD ratioInvoke = (DWORD)( 1000 * ratio );
		if( dice >= ratioInvoke )
			bSuccess = false;
	}
	return bSuccess;
}

XE_NAMESPACE_END

