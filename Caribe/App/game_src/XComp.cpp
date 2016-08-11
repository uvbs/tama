#include "stdafx.h"
#include "XComp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XCompObjFont::XCompObjFont()
{
	Init();
}

void XCompObjFont::Destroy()
{
	SAFE_RELEASE2( FONTMNG, m_pfdFont );
}

bool XCompObjFont::Load( LPCTSTR szFont, float sizeFont )
{
	XBREAK( m_pfdFont != nullptr );
	m_pfdFont = FONTMNG->Load( szFont, sizeFont );
	return m_pfdFont != nullptr;
}

void XCompObjFont::Draw( const XE::VEC2& vPos, float scale )
{
	m_pfdFont->DrawStringWithStyle( vPos, m_strText, m_Style, m_Col );
}
//////////////////////////////////////////////////////////////////////////
XCompObjMove::XCompObjMove()
	: m_State(0)
{
	Init();

}


//////////////////////////////////////////////////////////////////////////
XCompObjMoveBounce::XCompObjMoveBounce( const float power, const float dAngZ )
{
	auto vDZ = XE::GetAngleVector( dAngZ, power );
	auto vXY = XE::GetAngleVector( xRandomF( 360.f ), vDZ.x );
	SetvwDelta( XE::VEC3( vXY.x, vXY.y, vDZ.y ) );
// 	m_vwDeltaNext.x = vXY.x;
// 	m_vwDeltaNext.y = vXY.y;
// 	m_vwDeltaNext.z = vDZ.y;
}
int XCompObjMoveBounce::FrameMove( float dt )
{
	bool bUpdated = false;			// 상태가 변함.
	if( GetState() == 0 ) {
		AddvwPos( GetvwDelta() * dt );
		AddvwDelta( XE::VEC3( 0, 0, m_Gravity * dt ) );
//		m_vwPos += GetvwDelta() * dt;
//		m_vwDeltaNext.z += m_Gravity * dt;
		if( GetvwPos().z >= 0 ) {
			auto dwDelta = GetvwDelta();
			dwDelta.z *= -0.2f;
			SetvwDelta( dwDelta );		// // 바운스 되어서 벡터 방향 바뀜
//			m_vwDeltaNext.z *= -0.2f;		// 바운스 되어서 벡터 방향 바뀜
// 			m_vwPos.z = -m_vwPos.z;		// 바닥을 뚫고 지나온만큼을 역으로 되돌린다.
			{
				auto vwPos = GetvwPos();
				vwPos.z = -vwPos.z;		// 바닥을 뚫고 지나온만큼을 역으로 되돌린다.
				SetvwPos( vwPos );
			}
//			SetvwPos( XE::VEC3( 0, 0, -GetvwPos().z) );		// 바닥을 뚫고 지나온만큼을 역으로 되돌린다.
			if( GetvwDelta().z > -1.f * dt ) {		// 바운스힘이 일정 이하면 끝냄
//				m_vwDeltaNext.Set( 0 );
				SetvwDelta( XE::VEC3(0) );
				//m_vwPos.z = 0;
				SetvwPos( XE::VEC3( GetvwPos().x, GetvwPos().y, 0) );
				SetState( xST_DISAPPEAR );
				GettimerState().Set( 1.f );
				bUpdated = true;
			}
		}
	}
	return ( bUpdated ) ? 1 : 0;		// 상태변화가 있었으면 1을 리턴
}

//////////////////////////////////////////////////////////////////////////
XCompObjMoveNormal::XCompObjMoveNormal( const XE::VEC3& vwDelta )
	: m_timerLife( 0.1f )
{
}
int XCompObjMoveNormal::FrameMove( float dt )
{
	bool bUpdated = false;			// 상태가 변함.
	AddvwPos( GetvwDelta() * dt );
	if( GetState() == 0 ) {
		// 떠오르기
		if( m_timerLife.IsOver() ) {
			// 1.5초간 대기.
			SetState( 1 );
			m_timerLife.Set(1.0f);
			//SetvwDelta( XE::VEC3(0,0,-0.5f));	// 서서히 올라간다.
			SetvwDelta( XE::VEC3(0,0,-0.5f) );	// 서서히 올라간다.
			bUpdated = true;
		}
	} else 
	if( GetState() == 1 ) {
		// 대기
		if( m_timerLife.IsOver() ) {
			// x초간 빠르게 떠오르며 사라진다.
			SetState( xST_DISAPPEAR );
			m_timerLife.Set(2.0f);
			SetvwDelta( XE::VEC3(0, 0, -2.f) );
			bUpdated = true;
		}
	} else 
	if( GetState() == xST_DISAPPEAR ) {
		if( m_timerLife.IsOver() ) {
			SetState( xST_DESTROYED );
			SetvwDelta( XE::VEC3(0, 0, 0) );
			bUpdated = true;
		}
	} else
	{
// 		float alpha = 1.0f - m_timerLife.GetSlerp();
// 		SetAlpha( alpha );
// 		if( m_timerLife.IsOver() )
// 			SetDestroy(1);
	}
	return ( bUpdated ) ? 1 : 0;		// 상태변화가 있었으면 1을 리턴
}
