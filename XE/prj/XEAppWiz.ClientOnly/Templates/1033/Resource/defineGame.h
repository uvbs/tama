#pragma once
// cpp소스에서도 사용되고 리소스로도 사용되는 상수들입니다.
// 여기서 정의한 상수들은 프로퍼티나 스크립트 등에서 사용할 수있습니다.
#ifdef __cplusplus
// 아래에 정의된 상수들은 모두 루아에서 쓸수 있다.
#define DEFINE_SAMPLE		11


namespace XGAME
{
	// 통화
	enum xCurrency {
		xCUR_NONE,
		xCUR_WON_KOREA,
		xCUR_US_DOLLAR,
	};
	// 8방향
	enum xtDir { DIR_E, 
				DIR_ES, 
				DIR_S, 
				DIR_WS, 
				DIR_W, 
				DIR_WN, 
				DIR_N, 
				DIR_EN };	
	
	enum xtEnum { xSIZE_NONE,
					xSIZE_SMALL,
					xSIZE_MIDDLE,
					xSIZE_BIG,
					};
};

#endif
