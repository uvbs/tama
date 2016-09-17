#include "stdafx.h"
#if 0
#include "XEWndWorld.h"
#include "XFramework/Game/XEWorld.h"
#include "xFont.h"
#include "OpenGL2/XRenderCmd.h"
#include "OpenGL2/XTextureAtlas.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XEWndWorldObjs::XEWndWorldObjs()
	: m_pRenderer( new XRenderCmdMng( __FUNCTION__ ) )
	, m_pAtlas( new XTextureAtlas( __FUNCTION__ ) )
{
	Init();
	//
	SetSizeLocal( XE::GetGameSize() );	// 디폴트로 화면에 꽉차는 윈도우
}

XEWndWorldObjs::XEWndWorldObjs( XEWorld *pWorld ) 
//	: m_pBatchRenderer( new XRenderCmdMng() )
{
	Init();
	//
	XBREAK( pWorld == NULL );
	m_pWorld = pWorld;
	SetSizeLocal( XE::GetGameSize() );	// 디폴트로 화면에 꽉차는 윈도우
}

void XEWndWorldObjs::Destroy()
{
//	SAFE_DELETE( m_pBatchRenderer );
	SAFE_DELETE( m_pWorld );
}

void XEWndWorldObjs::Release( void )
{
	XBREAK( m_pWorld == NULL );
	m_pWorld->Release();
}

int XEWndWorldObjs::Process( float dt ) 
{
	{
		auto pPrev = XTextureAtlas::sSetpCurrMng( m_pAtlas );
		SetvwCamera( XScroll::GetvCenter() );
		m_pWorld->Process( this, dt );
		XTextureAtlas::sSetpCurrMng( pPrev );
	}
	XWnd::Process( dt );
	XScroll::Process( dt );
	return 1;
}

void XEWndWorldObjs::Draw( void )
{
	SET_RENDERER( m_pRenderer ) {
		m_pWorld->Draw( this );
	} END_RENDERER;

	XWnd::Draw();		// 현재 차일드가 없으면 이부분 삭제
	// 여기는 순수하게 월드렌더링을 위한 레이어여야 하는게 맞다. 그래서 ui용 배치렌더러는 놓지 않았다.
}
void XEWndWorldObjs::DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat )
{
	x = 2.f;
	y = 0.f;
	XE::VEC2 vlsPos = INPUTMNG->GetMousePos() - GetPosLocal();
	XE::VEC2 vwMouse = GetPosWindowToWorld( vlsPos );
	for( int i = 0; i < 1; ++i )
	{
		pFontDat->DrawString( x, y, col,
			XE::Format( _T( "vwCamera:%d,%d scaleCamera:%.1f vwMouse:%d,%d visibleObj:%d" ),
			(int)m_vwCamera.x, (int)m_vwCamera.y,
			m_scaleCamera,
			(int)vwMouse.x, (int)vwMouse.y ),
			(int)(GetpWorld()->GetpObjMng()->GetNumVisibleObj()) );
	}
}
/**
 월드좌표를 윈도우내 로컬좌표로 변환한다.
*/
XE::VEC2 XEWndWorldObjs::GetPosWorldToWindow( const XE::VEC3& vwPos, float *pOutScale/*=NULL*/ ) 
{
	// 카메라 퍼스펙티브 버전
	// 화면의 중심
/*
	XE::VEC2 vsCenter = XE::GetGameSize() / 2.f;
	// 카메라 중심의 로컬월드 좌표
	XE::VEC2 vwLocalFromCamera = vwPos.ToVec2() - m_vwCamera;
	float zScale = ( vwPos.y / m_vwCamera.y ) * m_scaleCamera;
	XE::VEC2 vlsPos = vsCenter + vwLocalFromCamera * zScale;
	if( pOutScale )
		*pOutScale = zScale;
	return vlsPos;
*/

	// 평행투영 머전
	XE::VEC2 sizeWin = GetSizeLocal();
	XE::VEC2 vwLT = m_vwCamera - ((sizeWin * 0.5f) / m_scaleCamera);		// 현재 윈도우뷰의 좌상귀 월드좌표
	XE::VEC2 v2wPos = XE::VEC2( vwPos.x, vwPos.y );
	v2wPos.y += vwPos.z;		// z축은 마이너스가 위쪽.
	XE::VEC2 vlsPos = (v2wPos - vwLT) * m_scaleCamera;		// 로컬스크린(윈도우)좌표계로 변환
	vlsPos.x += ((vwPos.y - 525.f) * 0.5f) * m_scaleCamera;		// 쿼터뷰로 표시할때..
	if( pOutScale )
		*pOutScale = m_scaleCamera;
	return vlsPos;

	// 평행 패스펙티브 버전(이게 진짜임)
/*
	XE::VEC2 sizeWin = GetSizeLocal();
	XE::VEC2 vwLT = m_vwCamera - ( ( sizeWin / 2.f ) / m_scaleCamera );		// 현재 윈도우뷰의 좌상귀 월드좌표
	XE::VEC2 v2wPos = XE::VEC2( vwPos.x, vwPos.y );
	v2wPos.y += vwPos.z;		// z축은 마이너스가 위쪽.
	XE::VEC2 vlsPos = ( v2wPos - vwLT ) * m_scaleCamera;		// 로컬스크린(윈도우)좌표계로 변환
	// 월드y좌표 400을 기준으로 위쪽에 있는 오브젝트는 왼쪽으로 빗겨찍고 아래쪽에 있는 오브젝트는 오른쪽으로 빗겨찍는다.
	XE::VEC2 vsCenter = XE::GetGameSize() / 2.f;
//	XE::VEC2 vwCenter = GetpWorld()->GetvwSize() / 2.f;	// 월드의 중심
	// 화면중심으로부터의 좌표로 변환
	XE::VEC2 vsLocalCenter = vlsPos - vsCenter;
	XE::VEC2 vsPosPerspective;
	// 화면좌표로 퍼스펙티브를 한다.
	float y = ((vlsPos.y - vsCenter.y) * 0.3f) + vsCenter.y;;
	float y2 = ((vlsPos.y - vsCenter.y) * 0.3f) + vsCenter.y;;
	float zScale1 = (y / vsCenter.y) * m_scaleCamera;
	float zScale2 = (y2 / vsCenter.y) * m_scaleCamera;
//	float zScale = ( vlsPos.y / vsCenter.y );
	vsPosPerspective.x = vsCenter.x + ((vsLocalCenter.x * 2.f) * zScale2);
	vsPosPerspective.y = vsCenter.y + ((vsLocalCenter.y * 2.f) * zScale2);
	if( pOutScale )
		*pOutScale = zScale1;
	return vsPosPerspective;
*/
	
}

/**
 윈도우좌표 vlsPos를 월드좌표로 변환한다.
*/
XE::VEC2 XEWndWorldObjs::GetPosWindowToWorld( const XE::VEC2& vlsPos )
{
	XE::VEC2 sizeWin = GetSizeLocal();
	XE::VEC2 vwLT = m_vwCamera - ( ( sizeWin / 2.f ) / m_scaleCamera );		// 현재 윈도우뷰의 좌상귀 월드좌표
	XE::VEC2 vWinPos = vlsPos;
	XE::VEC2 vwPos = vwLT + vlsPos / m_scaleCamera;
	vwPos.x -= ((vwPos.y - 525.f) / 2.f) * m_scaleCamera;
	return vwPos;
}


void XEWndWorldObjs::OnZoom( float scale, float lx, float ly ) 
{
	m_scaleCamera *= scale;
	if( m_scaleCamera < m_scaleMin )
		m_scaleCamera = m_scaleMin;
	else
	if( m_scaleCamera > m_scaleMax )
		m_scaleCamera = m_scaleMax;
// 	m_scaleCamera += scale;
// 	if( scale < 0 && m_scaleCamera < m_scaleMin )
// 		m_scaleCamera = m_scaleMin;
// 	else
// 	if( scale > 0 && m_scaleCamera > m_scaleMax )
// 		m_scaleCamera = m_scaleMax;
	SetScaleCamera( m_scaleCamera );
//	XLOGXNA("m_scaleCamera:%f", m_scaleCamera );
	XScroll::OnZoom( lx, ly, scale );
}

/**
 rectBB바운딩 박스가 윈도우 영역을 벗어났는지 확인
*/
BOOL XEWndWorldObjs::IsOutBoundary( const XE::xRECT& rectBB )
{
	XE::VEC2 vSize = GetSizeLocal();
	if( rectBB.vLT.x > vSize.w )
		return TRUE;
	if( rectBB.vRB.x < 0 )
		return TRUE;
	if( rectBB.vLT.y > vSize.h )
		return TRUE;
	if( rectBB.vRB.y < 0 )
		return TRUE;

	return FALSE;
}

#endif // 0