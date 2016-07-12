#include "stdafx.h"
#include "XFramework/XScroll.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XScroll::XScroll( const XE::VEC2& vStartScroll, 
	const XE::VEC2& vEndScroll,
	const XE::VEC2& vCurrent,
	const XE::VEC2& sizeView, 
	XScroll::xtType typeScroll )
{
	Init();
	//
	InitScroll( vStartScroll, vEndScroll, vCurrent, sizeView, typeScroll );
}

void XScroll::InitScroll( const XE::VEC2& vStartScroll, 
	const XE::VEC2& vEndScroll,
	const XE::VEC2& vCurrent,
	const XE::VEC2& sizeView, 
	XScroll::xtType typeScroll )
{
	Init();
	float scaleInv = 1.f / m_Scale;
	m_vStartScroll = vStartScroll;
	m_vEndScroll = vEndScroll;
	m_sizeScroll = vEndScroll - vStartScroll;
	m_vCurr = vCurrent;
	m_vCenter = m_vCurr + ( sizeView * scaleInv ) * 0.5f;
	m_sizeView = sizeView;
	if( typeScroll )
		m_typeScroll = typeScroll;
}

void XScroll::SetFocus( float wx, float wy ) 
{
	float scaleInv = 1.f / m_Scale;
 	XE::VEC2 sizeWin = m_sizeView;
 	XE::VEC2 vCenter(wx, wy);
 	XE::VEC2 vLT = vCenter - (sizeWin * 0.5f) * scaleInv;
// 	m_vCurr = -vLT;
	m_vCurr = vLT;
	m_vCenter = m_vCurr + ( m_sizeView * scaleInv ) * 0.5f;
}

void XScroll::SetvLT( const XE::VEC2& vCurr ) 
{
	const float scaleInv = 1.f / m_Scale;
	m_vCurr = vCurr;
	m_vCenter = vCurr + ( m_sizeView * scaleInv ) * 0.5f;
}

void XScroll::SetFocusTop()
{
	const float scaleInv = 1.f / m_Scale;
	m_vCurr.y = 0;
	m_vCenter.y = m_vCurr.y + ( m_sizeView.h * scaleInv ) * 0.5f;
}

void XScroll::SetFocusBottom()
{
	const float scaleInv = 1.f / m_Scale;
	m_vCurr.y = m_sizeScroll.h - m_sizeView.h;
	m_vCenter.y = m_vCurr.y + ( m_sizeView.h * scaleInv ) * 0.5f;
}

void XScroll::SetFocusLeft()
{
	const float scaleInv = 1.f / m_Scale;
	m_vCurr.x = 0;
	m_vCenter.x = m_vCurr.x + ( m_sizeView.w * scaleInv ) * 0.5f;
}

void XScroll::SetFocusRight()
{
	const float scaleInv = 1.f / m_Scale;
	m_vCurr.x = m_sizeScroll.w - m_sizeView.w;
	m_vCenter.x = m_vCurr.x + ( m_sizeView.w * scaleInv ) * 0.5f;
}

int XScroll::Process( float dt )
{
	if( m_bFirstUpdate == false ) {
		OnStopScroll();		// 최초한번 불러준다.
		m_bFirstUpdate = true;
	}
	XE::VEC2 vLTOutPrev, vRBOutPrev;
	float scaleInv = 1.f / m_Scale;
	//	XE::VEC2 vSize = GetSizeFinal();
	if( m_bTouchDown == FALSE )	{	// 손을 놨을때만 처리
		if( m_vScroll.x > 30.f )
			m_vScroll.x = 30.f;
		if( m_vScroll.x < -30.f )
			m_vScroll.x = -30.f;
		if( m_vScroll.y > 30.f )
			m_vScroll.y = 30.f;
		if( m_vScroll.y < -30.f )
			m_vScroll.y = -30.f;
		// 마찰로 서서히 선다.
		if( m_vScroll.x > 0 ) {
			m_vScroll.x -= 0.15f;
			if( m_vScroll.x < 0 ) {
				m_vScroll.x = 0;
//				XTRACE("stop scroll");
				OnStopScroll();
			}
		} else
		if( m_vScroll.x < 0 ) {
			m_vScroll.x += 0.15f;
			if( m_vScroll.x > 0 ) {
				m_vScroll.x = 0;
//				XTRACE( "stop scroll" );
				OnStopScroll();
			}
		}
		if( m_vScroll.y > 0 ) {
			m_vScroll.y -= 0.15f;
			if( m_vScroll.y < 0 ) {
				m_vScroll.y = 0;
//				XTRACE( "stop scroll" );
				OnStopScroll();
			}
		} else
		if( m_vScroll.y < 0 ) {
			m_vScroll.y += 0.15f;
			if( m_vScroll.y > 0 ) {
				m_vScroll.y = 0;
//				XTRACE( "stop scroll" );
				OnStopScroll();
			}
		}
		// 스크롤값 더하기전 경계를 얼마나 벗어났는가
		vLTOutPrev = m_vCurr - m_vStartScroll;
		vRBOutPrev = ( m_vCurr + m_sizeView * scaleInv ) - m_vEndScroll;
	} else {
		// 과거스크롤평균은 실제 터치가 움직일때만 더해지므로 프로세스내부에서 평균스크롤값을 계속 줄여줘야 한다.
//		m_vScrollAvg += m_vScroll;
		m_vScrollAvg /= 2.f;
	}
//	m_vMousePrev = m_vMouse;
	m_vCurr = m_vCenter - (m_sizeView * scaleInv) / 2.f;
	m_vCurr += m_vScroll / m_Scale;
	if( m_vCurr.x != 0 && fabs(m_vCurr.x) < 1 )
		m_vCurr.x = 0;
	if( m_vCurr.y != 0 && fabs(m_vCurr.y) < 1 )
		m_vCurr.y = 0;
	//
	// 좌표이동을 한 후 경계를 넘어가는 부분 체크
	if( m_bTouchDown == FALSE )	{	// 손을 놨을때
		const float valDiv = 4.f;
		// 스크롤값 더하고 난후 경계를 넘어갔다.
		// x축
		// 스크롤값을 더하고 난 후 경계를 벗어났다.
		if( m_vCurr.x < m_vStartScroll.x ) {
			float dist = m_vCurr.x - m_vStartScroll.x;			
			m_vScroll.x -= ( dist / valDiv );		// 스크롤값을 벗어난양에 비례해 감속
			if( m_vScroll.x > 15.f )
				m_vScroll.x = 15.f;
		} else
		if( m_vCurr.x > 0 
			&& m_vCurr.x + m_sizeView.w * scaleInv > m_vEndScroll.x ) {
			float dist = (m_vCurr.x + m_sizeView.w * scaleInv) - m_vEndScroll.x;
			m_vScroll.x -= ( dist / valDiv );
			if( m_vScroll.x < -15.f )
				m_vScroll.x = -15.f;
		}
		if( vLTOutPrev.x < 0 ) {		// 스크롤값 더하기 전 왼쪽으로 벗어나있었다.
			// 우측으로 되돌아가던중
			if( m_vScroll.x > 0 && 
				m_vCurr.x > m_vStartScroll.x ) {	// 다시 경계를 넘어서면 그냥 멈춤
				m_vCurr.x = m_vStartScroll.x;
				m_vScroll.x = 0;
				OnStopScroll();
//				XTRACE( "stop scroll2" );
			}
		} else
		if( vRBOutPrev.x > 0 ) {
			// 좌측으로 되돌아가던중
			if( m_vScroll.x < 0 &&
				m_vCurr.x + m_sizeView.w * scaleInv < m_vEndScroll.x ) {	// 다시 경계를 넘어서면 그냥 멈춤
				m_vCurr.x = m_vEndScroll.x - m_sizeView.w * scaleInv;
				m_vScroll.x = 0;
				OnStopScroll();
//				XTRACE( "stop scroll2" );
			}
		}
		// y축
		// 스크롤값을 더하고 난 후 경계를 벗어났다.
		if( m_vCurr.y < m_vStartScroll.y ) {
			float dist = m_vCurr.y - m_vStartScroll.y;
			m_vScroll.y -= ( dist / valDiv );		// 스크롤값을 벗어난양에 비례해 감속
			if( m_vScroll.y > 15.f )
				m_vScroll.y = 15.f;
		} else
		if( m_vCurr.y > 0 
			&& m_vCurr.y + m_sizeView.h * scaleInv > m_vEndScroll.y ) {
			float dist = ( m_vCurr.y + m_sizeView.h * scaleInv ) - m_vEndScroll.y;
			m_vScroll.y -= ( dist / valDiv );
			if( m_vScroll.y < -15.f )
				m_vScroll.y = -15.f;
		}
		if( vLTOutPrev.y < 0 ) {		// 스크롤값 더하기 전 왼쪽으로 벗어나있었다.
			// 우측으로 되돌아가던중
			if( m_vScroll.y > 0 &&
				m_vCurr.y > m_vStartScroll.y ) {	// 다시 경계를 넘어서면 그냥 멈춤
				m_vCurr.y = m_vStartScroll.y;
				m_vScroll.y = 0;
//				XTRACE( "stop scroll2" );
				OnStopScroll();
			}
		} else
		if( vRBOutPrev.y > 0 ) {
			// 좌측으로 되돌아가던중
			if( m_vScroll.y < 0 &&
				m_vCurr.y + m_sizeView.h * scaleInv < m_vEndScroll.y ) {	// 다시 경계를 넘어서면 그냥 멈춤
				m_vCurr.y = m_vEndScroll.y - m_sizeView.h * scaleInv;
				m_vScroll.y = 0;
//				XTRACE( "stop scroll2" );
				OnStopScroll();
			}
		}
	}
	// 모든 m_vCurr의 갱신이 끝난후에 센터좌표를 구해놓는다.,
	m_vCenter = m_vCurr + ( m_sizeView * scaleInv ) / 2.f;
	return 1;
}

/**
 현재 스크롤 위치가 스크롤영역의 경계를 벗어났을때 끝에 맞춰준다.
*/
void XScroll::FitScroll( void )
{
	float scaleInv = 1.f / m_Scale;
	XBREAK( m_sizeView.IsZero() );
	m_vCurr = m_vCenter - ( m_sizeView * scaleInv ) / 2.f;
	XE::VEC2 vOld = m_vCurr;
	if( m_vCurr.x < m_vStartScroll.x )
		m_vCurr.x = m_vStartScroll.x;
	if( m_vCurr.x + m_sizeView.w * scaleInv > m_vEndScroll.x )
		m_vCurr.x = m_vEndScroll.x - m_sizeView.w * scaleInv;
	if( m_vCurr.y < m_vStartScroll.y )
		m_vCurr.y = m_vStartScroll.y;
	if( m_vCurr.y + m_sizeView.h * scaleInv > m_vEndScroll.y )
		m_vCurr.y = m_vEndScroll.y - m_sizeView.h * scaleInv;
	if( vOld != m_vCurr )
	{
		m_vCenter = m_vCurr + ( m_sizeView * scaleInv ) / 2.f;
		m_vScroll.Set(0);
		m_vScrollAvg.Set(0);
	}
}


void XScroll::OnLButtonDown( float lx, float ly )
{
	if( IsActive() == FALSE )
		return;
	if( IsLock() )
		return;
	if( m_bZoom == FALSE )
	{
		m_bTouchDown = TRUE;
		m_bTouchUp = FALSE;
		m_vMouse = XE::VEC2( lx, ly );
		m_vMousePrev = m_vMouse;
		m_vScroll.Set( 0 );
//		XTRACE( "stop scroll3" );
		OnStopScroll();
		m_vScrollAvg.Set( 0 );
//		m_vScrollLast.Set( 0 );
//		m_Timer.Off();
	}
//	XLOGXNA("onTouch down:%d,%d", (int)lx, (int)ly );
}

void XScroll::OnMouseMove( float lx, float ly )
{
	if( IsActive() == FALSE )
		return;
	if( IsLock() )
		return;
	m_vMouse = XE::VEC2( lx, ly );
	if( m_bTouchDown && m_bZoom == FALSE )
	{
		float scaleInv = 1.f / m_Scale;
		m_bScrolling = TRUE;		// 스크롤이 시작됐다.
		{
			m_vScroll = m_vMousePrev - m_vMouse;
			if( IsVertOnly() )
				m_vScroll.x = 0;
			if( IsHorizOnly() )
				m_vScroll.y = 0;
			m_vMousePrev = m_vMouse;
			// 현재 스크롤위치가 시작위치를 벗어나면 스크롤이 당겨지는 듯한 느낌이 들도록 한다.
			if( m_vCurr.x < m_vStartScroll.x )
			{
				float dist = (float)( (int)( m_vStartScroll.x - m_vCurr.x ) );
				if( dist < 1.0f )
					dist = 1.f;
				m_vScroll.x /= dist;
			}
			if( m_vCurr.y < m_vStartScroll.y )
			{
				float dist = ( m_vStartScroll.y - m_vCurr.y );
				if( dist < 1.0f )
					dist = 1.f;
				m_vScroll.y /= dist;
			}
			if( m_vCurr.x + m_sizeView.w * scaleInv > m_vEndScroll.x )
			{
				float dist = ( m_vCurr.x + m_sizeView.w * scaleInv ) - m_vEndScroll.x;
				if( dist < 1.0f )
					dist = 1.f;
				m_vScroll.x /= dist;
			}
			if( m_vCurr.y + m_sizeView.h * scaleInv > m_vEndScroll.y )
			{
				float dist = ( m_vCurr.y + m_sizeView.h * scaleInv ) - m_vEndScroll.y;
				if( dist < 1.0f )
					dist = 1.f;
				m_vScroll.y /= dist;
			}
// 			if( m_vScroll.IsZero() == FALSE )
// 			{
// 				m_Timer.Set( 0 );		// 스크롤중에는 타이머가 계속 리셋된다
// 				m_vScrollLast = m_vScroll;
// 			}
 			m_vScrollAvg += m_vScroll;
 			m_vScrollAvg /= 2.f;
			//
//			m_vCurr = m_vCenter - ( m_sizeView * scaleInv ) / 2.f;
			m_vCurr += m_vScroll / m_Scale;
			m_vCenter = m_vCurr + ( m_sizeView * scaleInv ) / 2.f;
			// 
			OnScroll();
			m_vScroll.Set( 0 );
		}
	}
//	XLOGXNA( "onTouch move:%d,%d", (int)lx, (int)ly );
}

void XScroll::OnLButtonUp( float lx, float ly )
{
	if( IsActive() == FALSE )
		return;
	if( IsLock() )
		return;
	m_vMouse = XE::VEC2( lx, ly );
	if( m_bTouchDown && m_bZoom == FALSE )
	{
		m_bTouchUp = TRUE;
		// 스크롤되던 평균속도를 스크롤값으로 씀.
		m_vScroll = m_vScrollAvg;
		m_vScrollAvg.Set( 0 );
		m_bTouchUp = FALSE;
//		XLOGXNA( "scrollAvg:%f,%f", m_vScroll.x, m_vScroll.y );
	}
	m_bTouchDown = FALSE;
	m_bScrolling = FALSE;
	m_bZoom = FALSE;
//	XLOGXNA( "onTouch up:%d,%d", (int)lx, (int)ly );
}

void XScroll::OnZoom( float lx, float ly, float scale )
{
	m_bTouchDown = FALSE;	// 드래그 모드 해제
#ifndef WIN32
	m_bZoom = TRUE;	// win32에서는 마우스 휠로 하므로 이게 필요없다.
#endif
	m_vScroll.Set(0);
	m_vScrollAvg.Set(0);
//	XLOGXNA( "onTouch zoom:%d,%d", (int)lx, (int)ly );
}
