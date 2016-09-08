#include "stdafx.h"
#include "XWndView.h"
#include "XWndText.h"
#include "XWndButton.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XEProfile.h"
#include "sprite/SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XWndView::XWndView( XLayout *pLayout, const char *cKey, const char *cGroup ) 
{
	Init();
	XWnd *pRoot = nullptr;
	pLayout->CreateLayout( cKey, this, cGroup );
	// this의 최대 사이즈로 this의 사이즈를 정한다.
	auto sizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocal );
}

/**
 @brief 위 생성자버전에 버그가 있다. 기존호환성을 유지하기 위해 Layout으로 생성하는 새버전을 만듬.
*/
XWndView::XWndView( XLayout *pLayout, const char *cNodeName )
{
	Init();
	XWnd *pRoot = nullptr;
	pLayout->CreateLayout( cNodeName, this );
	auto sizeLocalLayout = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocalLayout );
	// 모든 차일드의 위치와 크기를 계산해서 가장 좌상귀의 좌표와 최대 크기를 얻어낸다
// 	XE::VEC2 vLT, vRB, vSize;
// 	GetMaxLTPosByChild( &vLT, &vRB );
// 	// 하위자식들의 절대좌표를 this기준 상대좌표로 바꾼다.
// 	ConvertChildPosToLocalPos( vLT );
// 	// 자식들을 먼저 상대좌표로 바꿔놓고 this의 좌표를 바꾼다.
// 	SetPosLocal( vLT );
// 	vSize = vRB - vLT;
// 	// 차일드의 최대 사이즈로 this의 사이즈를 정한다.
// 	//	XE::VEC2 vSize = GetMaxSizeByChild();
// 	SetSizeLocal( vSize );
}

/**
 @brief szImg를 배경이미지로 한 팝업뷰. 화면의 정중앙에 배치된다.
*/
XWndView::XWndView( LPCTSTR szImg )
	: XWnd( 0, 0 ) 
{
	Init();
	m_psfcFrame = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
	XBREAK( m_psfcFrame == NULL );
	SetSizeLocal( m_psfcFrame->GetSize() );
	AutoLayoutCenter( XE::GetMain()->GetpGame() );
}
void XWndView::Destroy() 
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcFrame );
}

void XWndView::OnDestroy()
{
	if( GetbModal() ) {
		auto pDark = static_cast<XWndDarkBg*>( XE::GetGame()->Find( "bg.dark" ) );
		if( pDark ) {
			if( pDark->DecRefCnt() == 0 )
				pDark->DoLighten();
		}
	}
}

BOOL XWndView::OnCreate()
{
	if( GetbModal() ) {
		XWndDarkBg *pDark = static_cast<XWndDarkBg*>( XE::GetGame()->Find("bg.dark") );
		if( pDark  == nullptr ) {
			pDark = XE::GetGame()->CreateDarkBg();
			pDark->SetstrIdentifier( "bg.dark" );
			XBREAK( getid() == 0 );
			XBREAK( GetpParent() == nullptr );
			GetpParent()->Insert( getid(), pDark );
			pDark->DoDarken();
		}
		pDark->AddRefCnt();
	}
	return TRUE;
}

BOOL XWndView::LoadRes( LPCTSTR szFrame )
{
	XBREAK( szFrame == NULL );
  if( szFrame )
	{
		XWnd::LoadResFrame( szFrame, &m_frameImage );
		XBREAKF( m_frameImage.nBlockSize == 0, "UI 리소스가 로딩되지 않았습니다" );
		m_nBlockSize = m_frameImage.nBlockSize;	// 편의상 받아옴
		m_bDrawFrame = TRUE;
		XE::VEC2 vSize = GetSizeLocal();
		XE::VEC2 vSector = vSize / (float)m_frameImage.nBlockSize;
		vSector.x = (float)((int)vSector.x);
		vSector.y = (float)((int)vSector.y);
//		vSector += XE::VEC2( 2 );		// 양쪽 프레임까지 합해서 섹터단위 크기
		vSize = vSector * (float)m_frameImage.nBlockSize;
		SetSizeLocal( vSize );
	}
    return TRUE;
}

void XWndView::ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp )
{
}

BOOL XWndView::ProcessEnterLeaveAnimation( float dt )
{
	// 애니메이션을 원하지 않는다면.
	if( GetbAnimationEnterLeave() == FALSE && m_nStateAppear == 0 ) {
		m_nStateAppear = 1;		// 1이 Appear끝
		m_vLT = GetPosFinal();
		SetScaleLocal( 1.0f );		// 최종크기 100%
#ifdef _CHEAT
		m_timerSpeed.Set(0, TRUE );		// 로딩전에 타이머 작동시킴
#endif
		CallEvent( "OnEventAddWnd" );
		OnFinishAppear();
		CallEventHandler( XWM_FINISH_APPEAR );
		return FALSE;
	}
	float slerp = m_timerAppear.GetSlerp();
	if( m_nStateAppear == 0 ) {		// 창 팝업효과 Init
		SetScaleLocal( 1.0f );
		m_vLT = GetPosFinal();
		SetScaleLocal( 0 );		// 스케일 0부터 시작
		m_timerAppear.Set( 0.05f );	// 0.15초동안 커진다
		m_nStateAppear = 2;	// start
		SetbActiveInternal( false );		// 애니메이션중엔 비활성 시킴
		SetAlphaLocal( 0.f );
	} else
	if( m_nStateAppear == 2 ) {		// init
		float scale = sinf( D2R(slerp * 90.f) );		// 0 ~ 1까지 사인파형으로 커진다
		float lerpTime = m_timerAppear.GetSlerp();
		SetAlphaLocal( lerpTime / 2.f );
		SetScaleLocal( scale );	
		if( m_timerAppear.IsOver() ) {
			m_timerAppear.Set( 0.15f );	// 100% +- x%구역에서 0.3초간 띠요요옹 한다
			SetAlphaLocal( 0.5f );
			m_nStateAppear = 3;		
		}
	} else
	if( m_nStateAppear == 3 ) {
		float scale = 1.0f + (sinf( D2R(slerp * (360.f*1.f)) ) * 0.2f) * (1.f - slerp);		// 0 -> 1 -> 0 -> -1 -> 0.. slerp에 따라 진폭이 점점 작아진다. 360도까지는 아래위한파동이므로 두파동을 만들기 위해 720도까지 돌림
		SetScaleLocal( scale );	// 110% -> 100%로 작아진다
		float lerpTime = m_timerAppear.GetSlerp();
		float lerp = XE::xiHigherPowerDeAccel( lerpTime, 1.f, 0.f );
		SetAlphaLocal( 0.5f + lerp );
		if( m_timerAppear.IsOver() ) {
			m_nStateAppear = 1;		// 1이 Appear끝
			SetbActiveInternal( true );
// 			SetbActive( TRUE );
			SetbAnimationEnterLeave( FALSE );		// 사라질땐 바로 사라지도록
			SetAlphaLocal( 1.f );
			SetScaleLocal( 1.0f );		// 최종크기 100%
#ifdef _CHEAT
			m_timerSpeed.Set(0, TRUE );		// 로딩전에 타이머 작동시킴
#endif
			CallEvent( "OnEventAddWnd" );
			OnFinishAppear();
			CallEventHandler( XWM_FINISH_APPEAR );
			return FALSE;
		}
	} else
	if( m_nStateAppear == 1 )
		return FALSE;
	return TRUE;
}

int XWndView::Process( float dt )
{
	XSprObj::s_LuaDt = dt;		// 루아글루에 사용
	return XWnd::Process( dt );
}

void XWndView::Draw( void )
{
	XPROF_OBJ_AUTO();
	if( m_nStateAppear == 0 )		// 아직 Process를 거치지 않은 상태라서 찍지 않음.
		return;
// 	XE::GetGame()->OnBeforeDrawByPopup( this );	// virtual
	if( m_psfcFrame ) {
		XE::VEC2 vScale = GetScaleFinal();
		XE::VEC2 vSize = m_psfcFrame->GetSize();
		float alpha = GetAlphaFinal();
		m_psfcFrame->SetfAlpha( alpha );
		m_psfcFrame->SetAdjustAxis( vSize / 2.f );
		m_psfcFrame->SetScale( vScale );
		m_psfcFrame->Draw( m_vLT );
	} else {
		if( m_bDrawFrame ) {
			if( m_frameImage.nBlockSize ) {
				XWND_RES_FRAME *pRes = &m_frameImage;
				XWnd::DrawFrame( *pRes );
			}
		} else
		if( m_colBg ) {
			XE::VEC2 vPos = m_vLT; // GetPosFinal();
			XE::VEC2 vSize = GetSizeLocal(); // GetSizeFinal();
			XE::VEC2 vScale = GetScaleFinal();
			XE::VEC2 vCenter = vPos + vSize / 2.f;
			XE::VEC2 vLT = vCenter + (vPos - vCenter) * vScale;
			vSize *= vScale;
			GRAPHICS->FillRectSize( vLT, vSize, m_colBg );
			GRAPHICS->DrawRectSize( vLT, vSize, m_colBorder );
		}
	}
	if( m_nStateAppear == 1 ) {		// Appear상태가 끝나면 차일드를 그린다
		if( m_prefLua )
			m_prefLua->Call<void,XWndView*>( m_cDrawHandler, this );	// lua의 draw handler를 호출
		//
		XWnd::Draw();		// child 그림
#ifdef _CHEAT
		if( m_timerSpeed.IsOn() ) {
			m_timeLoad = m_timerSpeed.GetPassTime();
			m_timerSpeed.Off();
		}
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
//		PUT_STRINGF( vPos.x, vPos.y + vSize.h - 20.f, XCOLOR_WHITE, "%d", m_timeLoad );
#endif
	}
}
void XWndView::OnFinishAppear()
{
	XE::GetGame()->OnFinishAppearPopup( this );
}

void XWndView::SetBgImg( LPCTSTR szImg, XE::xtPixelFormat formatSurface )
{
	m_psfcFrame = IMAGE_MNG->Load( true, XE::MakePath( DIR_UI, szImg ), formatSurface );
	if( XASSERT( m_psfcFrame ) ) {
		SetSizeLocal( m_psfcFrame->GetSize() );
	} else {
		XBREAKF( 1, "%s", XE::MakePath( DIR_UI, szImg ) );
	}
}

void XWndView::OnFinishCreatedChildLayout( XLayout *pLayout )
{
	SetbUpdate( true );
}

void XWndView::Update()
{
	// frame방식의 뷰일경우 크기가 명시되어 있지 않으면 자식크기에 따라 자동으로 결정된다.
	if( m_bDrawFrame ) {
		const auto sizeView = GetSizeLocalNoTrans();
		const auto sizeViewAligned = sizeView.RoundUpDiv( 16 );
		// 자식들의 전체 크기를 구한다.
		auto sizeChilds = GetSizeNoTransLayout();
		auto sizeChildsAligned = sizeChilds.RoundUpDiv( 16 );
		if( sizeChilds.w > 0 || sizeChilds.h > 0 ) {
			if( sizeView.w <= 0 ) {
				SetWidth( sizeChildsAligned.w );
			} else {
				// 크기가 명시되어있으면 그 사이즈를 사용한다.
				SetWidth( sizeViewAligned.w );
			}
			if( sizeView.h <= 0 ) {
				SetHeight( sizeChildsAligned.h );
			} else {
				// 크기가 명시되어있으면 그 사이즈를 사용한다.
				SetHeight( sizeViewAligned.h );
			}
		}
	}
}

////////////////////////////////////////////////////////////////
// dialog
XWndButton* XWndDialog::AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown  )
{
	XWndButton *pButton = new XWndButton( x, y, szSpr, idActUp, idActDown );
	Add( id, pButton );
	return pButton;
}
// static text
XWnd* XWndDialog::AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col, xFONT::xtStyle style )
{
	w = _tcslen( szText ) * 6.f;	// 10=font width(임시)
	h = 10;
	XWndTextString *wndStr = new XWndTextString( x, y, szText, pFontDat, col );
	wndStr->SetAlign( align );
	wndStr->SetStyle( style );
	return Add( id, wndStr );
}

int XWndDialog::Process( float dt )
{
	return XWndView::Process( dt );
}
void XWndDialog::Draw( void )
{
	if( m_psfcFrame )
	{
		XWndView::Draw();	
	} else
	{
		XWND_RES_FRAME *pRes = NULL;
		XBREAK( m_frameImage.nBlockSize == 0 );
		if( m_frameImage.nBlockSize )
			pRes = &m_frameImage;
		XWnd::DrawFrame( *pRes );
		if( m_nStateAppear == 1 )		// Appear상태가 끝나면 차일드를 그린다
			XWnd::Draw();	
	}
}


void XWndDialog::OnLButtonUp( float lx, float ly ) 
{
	XWnd::OnLButtonUp( lx, ly );
}
int XWndDialog::OnOk( XWnd*, DWORD, DWORD ) 
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_OK, 0 );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}
int XWndDialog::OnCancel( XWnd*, DWORD, DWORD ) 
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_CANCEL, 0 );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}

//
/*int XWndDialog::LuaDoModal( lua_State *L )
{
	GetpWndMng()->SetModalDialog( this );		// 자신(Dialog)을 모달다이얼로그로 등록시킴
	return lua_yield( L, 0 );
}*/


//////////////////////////////////////////////////////////////////////////
//XWndDarkBg* XWndDarkBg::s_pInstance = nullptr;
int XWndDarkBg::Process( float dt ) 
{
	if( m_State == 0 ) {
		if( m_timerAlpha.IsOff() )
			m_timerAlpha.Set( 0.5f );
		float lerpTime = m_timerAlpha.GetSlerp();
		if( lerpTime > 1.f )
			lerpTime = 1.f;
		m_Alpha = lerpTime * c_maxAlpha;
		if( lerpTime >= 1.f ) {
			++m_State;
			m_Alpha = c_maxAlpha;
			m_timerAlpha.Off();
		}
	} else
	if( m_State == 2 ) {
		if( m_timerAlpha.IsOff() )
			m_timerAlpha.Set( 0.5f );
		float lerpTime = m_timerAlpha.GetSlerp();
		if( lerpTime > 1.f )
			lerpTime = 1.f;
		m_Alpha =  (1.f - lerpTime) * c_maxAlpha;
		if( lerpTime >= 1.f ) {
			++m_State;
			m_Alpha = 0;
			m_timerAlpha.Off();
			SetbDestroy( TRUE );
		}
	}
	return XWnd::Process( dt );
}
