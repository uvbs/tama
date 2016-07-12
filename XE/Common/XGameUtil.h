#pragma once

namespace XE
{
//	int GetDiceChance( XArrayLinearN<float, 100>& aryChance );
	// 확률표에따라 주사위를 굴려 등급하나를 뽑아낸다.
	// 확률표는 확률의 합이 1.0(100%)이 되어야 한다.
	template<int N>
	int GetDiceChance( const XArrayLinearN<float, N>& aryChance ) {
//		DWORD maxDice = 1000000000;		// 십억분의 1 정밀도
		DWORD maxDice = 1000;		
		DWORD dice = random( maxDice );
		double addChance = 0;
		int size = aryChance.size();
		for( int i = 0; i < size; ++i )
		{
//			addChance += (double)aryChance[ i ];
			addChance += aryChance[ i ];
			if( i == size-1 && addChance != 1.0f )
			{
				if( aryChance[ i ] == 0 )
					return i-1;
				addChance = 1.f;
			}
			if( addChance > 1.01 )
				XALERT( "warning: Go beyond the sum of the probability of 1.0" );
			if( dice < (DWORD)(maxDice * addChance) )
				return i;
		}
		// 확률표 합이 100미만일경우 여기까지 오는경우도 있다.
		return -1;
	}
	/**
	 @brief 분수형태로 사용하는 버전
	 @param aryChance 확률 배열. 확률들의 합은 maxFraction과 같아야 한다.
	 @param maxFraction 최대 확률
	 @return 선택된 배열 인덱스를 리턴한다.
	 @note 
	 예제: 1/1000 정밀도의 확률 테이블구성
	 ary{ 500, 300, 150, 50 }
	*/
	template<int N>
	int GetDiceChance( const XArrayLinearN<int, N>& aryChance, int maxFraction )
	{
		DWORD maxDice = maxFraction;
		DWORD dice = xRandom( maxDice );
		DWORD addChance = 0;
		int size = aryChance.size();
		for( int i = 0; i < size; ++i )
		{
			addChance += aryChance[ i ];
			if( addChance > maxDice )
				XALERT( "경고: 확률테이블의 합이 100%를 넘어갑니다." );
			if( dice < addChance )
				return i;
		}
		XBREAKF( 1, "경고: 확률테이블의 합이 100%가 되지 않아 확률 선택에 실패했습니다." );
		// 확률표 합이 100미만일경우 여기까지 오는경우도 있다.
		return 0;
	}
// 	template<int N>
// 	int GetDiceChance( const XArrayN<float, N>& aryChance )
// 	{
// //		DWORD maxDice = 1000000000;		// 십억분의 1 정밀도
// 		DWORD maxDice = 1000;		
// 		DWORD dice = random( maxDice );
// 		double addChance = 0;
// 		int size = aryChance.GetMax();
// 		for( int i = 0; i < size; ++i )
// 		{
// 			addChance += (double)aryChance[ i ];
// 			if( addChance > 1.01 )
// 				XALERT( "warning: Go beyond the sum of the probability of 1.0" );
// 			if( dice < (DWORD)(maxDice * addChance) )
// 				return i;
// 		}
// 		// 확률표 합이 100미만일경우 여기까지 오는경우도 있다.
// 		return -1;
// 	}
	int GetDiceChance( const std::vector<float>& aryChance );
	/**
	 @brief 1000면짜리 주사위를 굴려 ratio확률에 걸렸는지 얻는다.
	 @param ratio 0~1
	*/
	inline bool IsTakeChance( float ratio ) {
		int dice = xRandom(1000);
		if( dice < 1000 * ratio )
			return true;
		return false;
	}
};
