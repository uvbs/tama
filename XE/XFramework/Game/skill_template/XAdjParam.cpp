#include "stdafx.h"
#include "XAdjParam.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

NAMESPACE_XSKILL_START

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
float XAdjParam::CalcAdjParam( const float& val, int adjParam )
{
	XSKILL::ADJ_PARAM *pAdjParam = &m_adjParam[ adjParam ];
	float v = val;
	if( pAdjParam->valFixedImm >= 0 )		// 고정값이 디폴트가 아니면 고정치값만 적용한다
		return pAdjParam->valFixedImm;
	v = v + (v * pAdjParam->valPercent);		// val = val + (val * 0.1);;;;;
	v += pAdjParam->valImm;			// 즉치보정치 적용
//	*val = v;
	return v;
}

NAMESPACE_XSKILL_END