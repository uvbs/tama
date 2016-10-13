#include "stdafx.h"
#include "XEWorldCamera.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
/**
@brief
@param sizeWorld 카메라 스크롤가능한 전체 영역의 크기
@param sizeView 카메라이 비춰야 할 윈도우의 크기,
*/
XEWorldCamera::XEWorldCamera( const XE::VEC2& sizeWorld,
															const XE::VEC2& sizeView,
															float scaleCamera )
	: m_scaleCamera( scaleCamera )
	, m_scaleMin( 1.f )
	, m_scaleMax( 1.f )
{
	Init();
	//	const auto sizeWorld = m_spWorld->GetvwSize();
	XBREAK( sizeWorld.IsZero() );
	XBREAK( sizeView.IsZero() );
	XE::VEC2 vwCamera = sizeWorld * 0.5f;
	XE::VEC2 vwLT = vwCamera - (sizeView * 0.5f) * (1 / scaleCamera);
	InitScroll( XE::VEC2( 0 ),
							sizeWorld,
							vwLT,	// 화면좌상귀에 해당하는 월드좌표
							sizeView,
							XScroll::xTP_ALL );
	XBREAK( XScroll::GetvCenter().IsZero() );
	SetFocus( XScroll::GetvCenter() );
}

void XEWorldCamera::Process( float dt )
{
	SetvwCamera( XScroll::GetvCenter() );
	XScroll::Process( dt );
}

/**
 월드좌표를 윈도우내 로컬좌표로 변환한다.
*/
XE::VEC2 XEWorldCamera::GetPosWorldToWindow( const XE::VEC3& vwPos, 
																						 float *pOutScale )
{
	// 평행투영 머전
	const XE::VEC2 sizeWin = GetsizeView();
	XE::VEC2 vwLT = m_vwCamera - ((sizeWin * 0.5f) / m_scaleCamera);		// 현재 윈도우뷰의 좌상귀 월드좌표
	XE::VEC2 v2wPos = XE::VEC2( vwPos.x, vwPos.y );
	v2wPos.y += vwPos.z;		// z축은 마이너스가 위쪽.
	XE::VEC2 vlsPos = (v2wPos - vwLT) * m_scaleCamera;		// 로컬스크린(윈도우)좌표계로 변환
	vlsPos.x += ((vwPos.y - 525.f) * 0.5f) * m_scaleCamera;		// 쿼터뷰로 표시할때..
	if( pOutScale )
		*pOutScale = m_scaleCamera;
	return vlsPos;
}

/**
 윈도우좌표 vlsPos를 월드좌표로 변환한다.
*/
XE::VEC2 XEWorldCamera::GetPosWindowToWorld( const XE::VEC2& vlsPos )
{
	const XE::VEC2 sizeWin = GetsizeView();
	XE::VEC2 vwLT = m_vwCamera - ( ( sizeWin / 2.f ) / m_scaleCamera );		// 현재 윈도우뷰의 좌상귀 월드좌표
	XE::VEC2 vWinPos = vlsPos;
	XE::VEC2 vwPos = vwLT + vlsPos / m_scaleCamera;
	vwPos.x -= ((vwPos.y - 525.f) / 2.f) * m_scaleCamera;
	return vwPos;
}


void XEWorldCamera::OnZoom( float scale, float lx, float ly )
{
	m_scaleCamera *= scale;
	if( m_scaleCamera < m_scaleMin )
		m_scaleCamera = m_scaleMin;
	else
	if( m_scaleCamera > m_scaleMax )
		m_scaleCamera = m_scaleMax;
	SetScaleCamera( m_scaleCamera );
	XScroll::OnZoom( lx, ly, scale );
}
