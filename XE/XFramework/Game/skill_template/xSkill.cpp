#include "stdafx.h"
#include "xSkill.h"
#include "xLib.h"

NAMESPACE_XSKILL_START
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
NAMESPACE_XSKILL_END

