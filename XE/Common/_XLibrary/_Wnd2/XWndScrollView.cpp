#include "stdafx.h"
#include "XWndScrollView.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
XWndScrollView::XWndScrollView( float x, float y, float w, float h )
	: XWnd( x, y, w, h )	
{
	Init();
//	SetbTouchable( FALSE );		// 뷰 자체는 터치이벤트를 지나치게 한다.
	SetScrollAllDir();
	SetViewSize( XE::VEC2(w, h) );
}
XWndScrollView::XWndScrollView( const XE::VEC2& vPos, const XE::VEC2& vSize )
	: XWnd( vPos.x, vPos.y, vSize.w, vSize.h )	
{
	Init();
	SetbTouchable( FALSE );		// 뷰 자체는 터치이벤트를 지나치게 한다.
	SetScrollAllDir();
	SetViewSize( vSize );
}

/**
	전체 스크롤뷰의 크기를 세팅
*/
void XWndScrollView::SetViewSize( const XE::VEC2& vSize ) 
{
	m_vTotalSize = vSize;
	XE::VEC2 vLocal = GetSizeLocal();
	XScroll::xtType type = XScroll::GettypeScroll();
	XBREAK( !type );
	XScroll::InitScroll( XE::VEC2( 0 ), vSize, XE::VEC2( 0 ),
							GetSizeLocal(), type );
}

// 
// 
/**
 @brief 스크롤뷰의 최대 사이즈를 계산하여 스크롤뷰의 사이즈를 세팅한다.
 스크롤뷰에 Wnd들을 모두 추가시킨 후 자식Wnd의 위치와 사이즈를 모두 고려해서 크기를 계산한다.
 Layout에서 scrollview를 생성했다면 자동으로 불러주므로 수동으로 불러줄필요 없다.,
*/
// XE::VEC2 XWndScrollView::SetScrollViewAutoSize( void )
// {
// 	auto vBackup = XScroll::GetvCurrLT();
// 	XScroll::SetvLT( XE::VEC2(0) );
// 	m_vAdjust.Set(0);
// 	// 가장 큰크기로 맞춘다.
// 	XE::VEC2 vSize = GetSizeFinal();
// 	LIST_LOOP( m_listItems, XWnd*, itor, pWnd ) {
// 		// 파괴중인 윈도우는 사이즈 계산에 넣지 않음.
// 		if( !pWnd->IsDestroy() ) {
// 			XE::VEC2 vPos = pWnd->GetPosFinal();
// 			XE::VEC2 size = pWnd->GetSizeForceFinal();
// 			if( vPos.x + size.w > vSize.w )
// 				vSize.w = vPos.x + size.w;
// 			if( vPos.y + size.h > vSize.h )
// 				vSize.h = vPos.y + size.h;
// 		}
// 	} END_LOOP;
// 	SetViewSize( vSize );
// 	XScroll::SetvLT( vBackup );
// 	m_vAdjust = -XScroll::GetvCurrLT();
// 	return vSize;
// }
XE::VEC2 XWndScrollView::SetScrollViewAutoSize( void )
{
	auto vBackup = XScroll::GetvCurrLT();
	XScroll::SetvLT( XE::VEC2(0) );
	m_vAdjScroll.Set(0);
	// 가장 큰크기로 맞춘다.
	XE::VEC2 vSize = GetSizeFinal();
	LIST_LOOP( m_listItems, XWnd*, itor, pWnd ) {
		// 파괴중인 윈도우는 사이즈 계산에 넣지 않음.
		if( !pWnd->IsDestroy() ) {
			auto vPos = pWnd->GetPosLocal();
			auto size = pWnd->GetSizeNoTransLayout();
			if( vPos.x + size.w > vSize.w )
				vSize.w = vPos.x + size.w;
			if( vPos.y + size.h > vSize.h )
				vSize.h = vPos.y + size.h;
		}
	} END_LOOP;
	vSize.y *= 1.1f;			// 버그가 있어서 세로로 조금더 크게함.
	SetViewSize( vSize );
	XScroll::SetvLT( vBackup );
	m_vAdjScroll = -XScroll::GetvCurrLT();
//	GetSizeLocalLayout();
	return vSize;
}

// 레이아웃에서 스크롤뷰안의 자식들이 모두 생성이 끝나면 호출된다.
void XWndScrollView::OnFinishCreatedChildLayout( XLayout *pLayout )
{
	SetScrollViewAutoSize();
}


int XWndScrollView::Process( float dt )
{
//	Scroll();
	XScroll::Process( dt );
	m_vAdjScroll = -XScroll::GetvCurr();
	// 어태치 스크롤뷰가 있다면 this의 스크롤과 똑같이 맞춰준다.
	if( m_pAttach && GetbShow() )
	{
		m_pAttach->SetvAdjScroll( m_vAdjScroll );
		m_pAttach->SetvLT( XScroll::GetvCurr() );
//		if( m_vScroll.IsZero() == FALSE )
		if( IsScroll() )
			m_pAttach->CallEventHandler( XWM_SCROLL );
	}
	return XWnd::Process( dt );
}

/**
 스크롤뷰의 현재 위치를 wx,wy를 중심으로 이동한다.
 wx, wy는 스크롤뷰 전체 좌표계이다
*/
void XWndScrollView::SetFocusView( float wx, float wy )
{
// 	XE::VEC2 sizeWin = GetSizeLocal();
// 	XE::VEC2 vCenter(wx, wy);
// 	XE::VEC2 vLT = vCenter - sizeWin/2.f;
// 	m_vAdjust = -vLT;
	XScroll::SetFocus( wx, wy );
	m_vAdjScroll = -XScroll::GetvCurr();
}

void XWndScrollView::OnScroll( void )
{
	if( XScroll::IsScroll() )
		CallEventHandler( XWM_SCROLL );
}

void XWndScrollView::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 v = XWnd::GetPosFinal();
	XE::VEC2 size = XWnd::GetSizeFinal();
	// viewport 설정
	BACKUP_VIEWPORT;
	GRAPHICS->SetViewport( v, size );
	XE::SetProjection( size.w, size.h );
	// 자식들 그림
	XWnd::Draw();
	//
	RESTORE_VIEWPORT;
	DrawScrollBar();
}

void XWndScrollView::DrawScrollBar( void )
{
	XE::VEC2 vPos = XWnd::GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	float ratioH = vSize.h / m_vTotalSize.h;
	float hBar = vSize.h * ratioH;		// 스크롤바 길이
	float ratioY = -m_vAdjScroll.y / m_vTotalSize.h;
	float yBar = vSize.h * ratioY;
	XE::VEC2 v, vs;
	v.x = vSize.w - 3.f;
	v.y = yBar;
	vs.w = 3.f;
	vs.h = hBar;
	if( hBar < 0 )
		hBar = 0;
	if( v.y < 0 )
	{
		vs.h += v.y;
		v.y = 0;
	}
	if( v.y + vs.h > vSize.h )
	{
		vs.h -= (v.y + vs.h) - vSize.h;
	}
	if( vs.h < 0 )
		vs.h = 0;
	BYTE a = (BYTE)( m_Alpha * 255.f );
	GRAPHICS->FillRectSize( vPos + v, vs, XCOLOR_RGBA( 216, 223, 231, a ) );
//	if( m_bLButtonDown || !m_vScroll.IsZero() )
	if( XScroll::IsScroll() )
	{
		m_Alpha += 0.03f;
		if( m_Alpha > 0.5f )
			m_Alpha = 0.5f;
	} else
	{
		m_Alpha -= 0.03f;
		if( m_Alpha < 0 )
			m_Alpha = 0;
	}
}

void XWndScrollView::OnLButtonDown( float lx, float ly ) 
{
//	CONSOLE("XWndScrollView::OnLButtonDown");
	XE::VEC2 vMouse = INPUTMNG->GetMousePos();
	m_vMouse = vMouse;
	m_vTouch = vMouse;
	m_vPrev = vMouse;
	m_bToggle = true;
	XScroll::OnLButtonDown( lx, ly );
	XWnd::OnLButtonDown( -m_vAdjScroll.x + lx, -m_vAdjScroll.y + ly );
}

void XWndScrollView::OnLButtonDownCancel( void )
{
	CONSOLE("XWndScrollView::OnLButtonDownCancel");
	XScroll::OnLButtonDownCancel();
}

void XWndScrollView::OnMouseMove( float lx, float ly ) 
{
	XWnd *pWndRet = this;
	const auto vCurr = XE::VEC2( lx, ly );
	XScroll::OnMouseMove( lx, ly );
	if( XScroll::GetbTouchDown() ) {
		const XE::VEC2 vDist = m_vTouch - INPUTMNG->GetMousePos();
		// 처음 누른곳에서 5픽셀이상 벗어나면 움직인것으로 인정
		if( fabs( vDist.Lengthsq() ) > 10.f * 10.f ) {
			if( m_bScroll == FALSE )
				SetCapture();
			m_bScroll = TRUE;
		}
		if( m_bScroll ) {	// 스크롤이 한번이라도 일어났어야 함.
			// 이전 마우스 위치에 비해 5픽셀 이내로 움직였으면 스크롤을 멈춘것으로 인정
			const auto vDistPrev = vCurr - m_vPrev;
			if( vDistPrev.Lengthsq() < 2.f * 2.f ) {
				if( !m_vStopScrollPrev.IsZero() ) {
					const auto vDistUpdate = vCurr - m_vStopScrollPrev;
					// 이전 업데이트 위치와 일정이상 떨어진곳이어야 다시 업데이트 한다.
					if( vDistUpdate.Lengthsq() > 32.f * 32.f ) {
						// 스탑 스크롤 업데이트
						if( m_pDelegate )
							m_pDelegate->DelegateStopScroll( vCurr );
						m_vStopScrollPrev = vCurr;	// 마지막 업데이트 좌표
					}
				} else {
					// 최초 한번 값 세팅
					m_vStopScrollPrev = vCurr;
				}
			}
		}
		m_vMouse = INPUTMNG->GetMousePos();
		// 스크롤중이면 자식에겐 이벤트 보내지 않음.
		if( m_bScroll == FALSE )
			XWnd::OnMouseMove( -m_vAdjScroll.x + lx, -m_vAdjScroll.y + ly );
	}
#ifdef WIN32
	else
		XWnd::OnMouseMove( -m_vAdjScroll.x + lx, -m_vAdjScroll.y + ly );
#endif
	m_vPrev = vCurr;
}
void XWndScrollView::OnNCMouseMove( float lx, float ly )
{
//	m_vMouse = XE::VEC2( lx, ly );
}
void XWndScrollView::OnLButtonUp( float lx, float ly ) 
{
	do {
		if( XWnd::sGetDragWnd() ) {
			XWnd::OnLButtonUp( -m_vAdjScroll.x + lx, -m_vAdjScroll.y + ly );
		} else {
			if( m_bToggle == false )	// LDown없이 왔다는것. Up이 두번불려졌다는등...이때는 취소시킨다. (임시방편)
				break;
			ReleaseCapture();
			XScroll::OnLButtonUp( lx, ly );
			// 스크롤중에 터치를 떼면 자식에겐 Touch Up 메시지 보내지 않음.
			if( m_bScroll == FALSE ) {
				XWnd::OnLButtonUp( -m_vAdjScroll.x + lx, -m_vAdjScroll.y + ly );
			}
		}
	} while (0);
	m_bScroll = FALSE;
	m_bToggle = false;
}
void XWndScrollView::OnNCLButtonUp( float lx, float ly )
{
//	m_bLButtonDown = FALSE;
	XScroll::CancelTouchDown();
	m_bScroll = FALSE;
	XWnd::OnNCLButtonUp( lx, ly );

}
