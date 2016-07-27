#include "stdafx.h"
#include "XEComponents.h"
#include "XArchive.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
int XECompBit::Serialize( XArchive& ar ) const 
{
	ar << (DWORD)m_Bit;
	return 1;
}
int XECompBit::DeSerialize( XArchive& ar, int ) 
{
	DWORD bit;
	ar >> bit;	m_Bit = bit;

	return 1;
}
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void XECompWave::Process( float dt )
{
	const float dist = m_maxValue - m_minValue;
	if( m_State == -1 ) {
		m_State = 0;
		m_timerDelay.Set( m_secDelay );
		m_Value = m_minValue;
	}
	if( m_State == 0 ) {		// 없어진상태
		if( m_timerDelay.IsOver() ) {
			m_State = 1;				// 나타나기 시작
			m_timerFade.Set( m_secFade );
		}
	} else
	if( m_State == 1 ) {		// 나타나는중
		float lerp = m_timerFade.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		float alpha = m_minValue + XE::xiLinearLerp( lerp ) * dist;
		m_Value = alpha;
		if( lerp >= 1.f ) {
//			m_Value = 1.f;
			m_State = 2;
			m_timerDelay.Set( m_secDelay );
		}
	} else
	if( m_State == 2 ) {		// 다 나타남
		if( m_timerDelay.IsOver() ) {
			m_State = 3;
			m_timerFade.Set( m_secFade );
		}
	} else
	if( m_State == 3 ) {		// 사라지는중
		float lerp = m_timerFade.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		float alpha = m_minValue + XE::xiLinearLerp( 1.f - lerp ) * dist;
		m_Value = alpha;
		if( lerp >= 1.f ) {
			if( m_pDelegate )
				m_pDelegate->DelegateEndState( this, m_State );
//			m_Value = 0;
			m_State = 0;
			m_timerDelay.Set( m_secDelay );

		}
	}
	XEComponent::Process( dt );
}

////////////////////////////////////////////////////////////////
void XECompFade::Process( float dt )
{
	if( m_timerFade.IsOn() ) {
		auto lerp = m_timerFade.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		m_Value = ( m_bFadeIn ) ? lerp : 1.f - lerp;
		if( lerp >= 1.f ) {
			if( m_funcFinish )
				m_funcFinish();		// 종료 이벤트 핸들러
			m_timerFade.Off();
		}
	}
	XEComponent::Process( dt );
}
