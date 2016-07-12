#include "stdafx.h"
#include "XGameUtil.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XE )
/**
 @brief 확률표에따라 주사위를 굴려 테이블의 인덱스를 리턴한다.
 확률표는 확률의 합이 1.0(100%)이 되어야 한다.
*/
int GetDiceChance( const std::vector<float>& aryChance ) 
{
	DWORD maxDice = 1000;
	DWORD dice = random( maxDice );
	double addChance = 0;
	int size = aryChance.size();
	for( int i = 0; i < size; ++i ) {
		addChance += aryChance[ i ];
		if( i == size - 1 && addChance != 1.0f ) {
			if( aryChance[ i ] == 0 )
				return i - 1;
			addChance = 1.f;
		}
		if( addChance > 1.01 )
			XALERT( "warning: Go beyond the sum of the probability of 1.0" );
		if( dice < (DWORD)( maxDice * addChance ) )
			return i;
	}
	// 확률표 합이 100미만일경우 여기까지 오는경우도 있다.
	return -1;
}

XE_NAMESPACE_END;


