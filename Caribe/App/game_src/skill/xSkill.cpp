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
	float invokeRatio = 0;
	if( pEffect->aryInvokeRatio.size() == 0 )
		invokeRatio = 1.f;
	else {
		XBREAK( level == 0 );
		if( pEffect->aryInvokeRatio.size() == 1 )	// 테이블형태로 안넣고 숫자하나만 넣었을경우.
			invokeRatio = pEffect->aryInvokeRatio[ 0 ];
		else
			invokeRatio = pEffect->aryInvokeRatio[ level ];
	}
	if( invokeRatio < 1.0f 
			&& ( pEffect->invokeJuncture != xJC_PERSIST 
					|| pEffect->secInvokeDOT > 0 
					|| pEffect->strInvokeSkill.empty() == false 
					|| pEffect->idInvokeSkill ) )
	{
		DWORD dice = xRandom( 1000 );
		DWORD ratioInvoke = (DWORD)( 1000 * invokeRatio );
		if( dice >= ratioInvoke )
			bSuccess = false;
	}
	return bSuccess;
}

XE_NAMESPACE_END

