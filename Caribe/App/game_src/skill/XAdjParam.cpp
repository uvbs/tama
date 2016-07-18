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

void XAdjParam::AddAdjParam( int adjParam, 
															xtValType valType, 
															float adj )
{
	XBREAK( adjParam == 0 );
	XBREAK( adjParam >= m_adjParam.GetMax() );		// 버퍼 오버플로우
	// 값 타입에 따라 따로 보정치를 누적시켜준다
	switch( valType )
	{
	case xPERCENT:		m_adjParam[ adjParam ].valPercent += adj;		break;
	case xVAL:			m_adjParam[ adjParam ].valImm += adj;		break;
	case xFIXED_VAL:	m_adjParam[ adjParam ].valFixedImm = adj;		break;		// 고정치는 누적시키지 않는다. 그리고 adj값을 꺼낼때 최우선순위로 적용된다
	}
}
// 오리지널값 val에 보정치를 연산해서 되돌려줌
float XAdjParam::CalcAdjParam( float val, int adjParam, float addAdjRatio, float addAdjVal )
{
	XSKILL::ADJ_PARAM *pAdjParam = &m_adjParam[ adjParam ];
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
	XSKILL::ADJ_PARAM *pAdjParam = &m_adjParam[ adjParam ];
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
	XSKILL::ADJ_PARAM *pAdjParam = &m_adjParam[ adjParam ];
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
	const auto pAdjParam = &m_adjParam[ adjParam ];
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return XFORMAT("%+d", (int)pAdjParam->valFixedImm );
	if( pAdjParam->valPercent )
		return XFORMAT( "%+.1f%%", pAdjParam->valPercent * 100.f );
	return XFORMAT( "%+d", (int)pAdjParam->valImm );
}

XE_NAMESPACE_END