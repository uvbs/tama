#include "stdafx.h"
#include "XAdjParam.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )

/**
 @brief 모든 adjParam값을 초기치로 초기화한다
*/
void XAdjParam::ClearAdjParam()
{
	// 현재버퍼를 클리어
	for( auto& adj : m_pBackAry->m_adjParam )
		adj.Init();
	for( auto& state : m_pBackAry->m_States )
		state.bActive = false;
}

/**
 @brief // 모든 adjParam값을 초기치로 초기화한다
*/
// void XAdjParam::ClearDebugAdjParam() 
// {		
// 	int max = m_adjParam.Size();
// 	for( int i = 0; i < max; ++i )
// 		m_adjParam[i].ClearDebug();
// }

void XAdjParam::AddAdjParam( int idxAdjParam,
														 xtValType valType,
														 float adj )
{
	XBREAK( idxAdjParam == 0 );
	XBREAK( idxAdjParam >= m_pBackAry->m_adjParam.Size() );		// 버퍼 오버플로우
																								// 값 타입에 따라 따로 보정치를 누적시켜준다
	auto& adjParam = m_pBackAry->m_adjParam[ idxAdjParam ];
	switch( valType ) {
	case xPERCENT:		adjParam.valPercent += adj;		break;
	case xVAL:				adjParam.valImm += adj;				break;
	case xFIXED_VAL:	adjParam.valFixedImm = adj;		break;		// 고정치는 누적시키지 않는다. 그리고 adj값을 꺼낼때 최우선순위로 적용된다
	}
}

// 오리지널값 val에 보정치를 연산해서 되돌려줌
float XAdjParam::CalcAdjParam( float val, int adjParam, float addAdjRatio, float addAdjVal ) const
{
	const auto pAdjParam = &(m_pCurrAry->m_adjParam[ adjParam ]);
	float v = val;
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return pAdjParam->valFixedImm;
	v = v + (v * (pAdjParam->valPercent + addAdjRatio));		// val = val + (val * 0.1);;;;;
	v += pAdjParam->valImm + addAdjVal;			// 즉치보정치 적용
//	*val = v;
	return v;
}

/**
 @brief val값을 기준으로 adjParam을 적용했을때 증감치를 리턴한다.
 예) 10 = GetAdjValue( 100, xADJ_HP_REGEN )	// valPercent = 0.1
*/
float XAdjParam::GetAdjValue( float val, int adjParam )
{
	const auto pAdjParam = &(m_pCurrAry->m_adjParam[ adjParam ]);
	float v = 0;
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return pAdjParam->valFixedImm;
	v = val * pAdjParam->valPercent;
	v += pAdjParam->valImm;			// 즉치보정치 적용
	return v;
}

/**
 @brief 증감량만 리턴한다.
 valImm과 valPercent가 공존하는 경우는 고려치 않았다.
*/
float XAdjParam::GetAdjValue( int adjParam )
{
	const auto pAdjParam = &(m_pCurrAry->m_adjParam[ adjParam ]);
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return pAdjParam->valFixedImm;
	if( pAdjParam->valPercent )
		return pAdjParam->valPercent;
	return pAdjParam->valImm;
}

/**
 @brief 증감량을 문자열로 만들어 돌려준다.
*/
_tstring XAdjParam::GetstrAdjParam( int adjParam ) const
{
	const auto pAdjParam = &(m_pCurrAry->m_adjParam[ adjParam ]);
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return XFORMAT("%+d", (int)pAdjParam->valFixedImm );
	if( pAdjParam->valPercent )
		return XFORMAT( "%+.1f%%", pAdjParam->valPercent * 100.f );
	return XFORMAT( "%+d", (int)pAdjParam->valImm );
}

XE_NAMESPACE_END