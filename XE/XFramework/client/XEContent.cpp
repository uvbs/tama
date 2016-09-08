#include "stdafx.h"
#include "XEContent.h"
#include "XFontMng.h"
#include "_Wnd2/XWndPopup.h"
#include "XImageMng.h"
#include "OpenGL2/XTextureAtlas.h"
#ifdef _XASYNC_SPR
#include "Sprite/SprMng.h"
#endif // _XASYNC_SPR

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEContent* XEContent::s_pInstance = nullptr;

XEContent::XEContent() 
	: XWnd( 0, 0, (int)XE::GetGameWidth(), (int)XE::GetGameHeight() ) 
{
	s_pInstance = this;
	Init();
	SetbTouchable( FALSE );	// 게임 윈도우 자체는 터치이벤트를 발생시키지 않는다.
}
XWnd* XEContent::GetpRootScene()
{
	return Find( "_root.scene" );
}

void XEContent::Destroy()
{
	XTRACE("XEContent::Destroy\n");
	DestroySystemFont();
	XTRACE( "--XEContent::Destroy\n" );
}

BOOL XEContent::OnCreate()
{
//	XTextureAtlas::sSetMaxSizeTex( XE::VEC2(4096, 4096) );
	const auto strKeyLang = OnSelectLanguageKey();		// virtual. lang.txt의 국가키를 받는다.
	XE::LANG.SetSelectedKey( strKeyLang );
	CreateSystemFont();
	// 시스템이 시작하자마자 씬루트를 젤 밑에 깔아준다.
	XWnd *pRoot = new XWnd();
	pRoot->SetstrIdentifier( "_root.scene" );
	Add( pRoot );
	return TRUE;
}

BOOL XEContent::RestoreDevice()
{
//   m_pfdSystem->RestoreDevice();
//   m_pfdSystemSmall->RestoreDevice();
  return XWnd::RestoreDevice();
}

void XEContent::DestroySystemFont()
{
	SAFE_DELETE( m_pfoSystem );
	SAFE_RELEASE2( FONTMNG, m_pfdSystem );
	SAFE_RELEASE2( FONTMNG, m_pfdSystemSmall );
}
BOOL XEContent::CreateSystemFont( void )
{
	XBREAK( m_pfdSystem != nullptr );
	m_pfdSystem = FONTMNG->Load( FONT_SYSTEM, FONT_SIZE_DEFAULT );
	if( XBREAKF( m_pfdSystem == NULL, "load font: %s......failed", FONT_SYSTEM ) )
		return FALSE;
	m_pfoSystem = m_pfdSystem->CreateFontObj();
	if( XBREAKF( m_pfoSystem == NULL, "create system fontobj:......failed" ) )
		return FALSE;
	m_pfdSystemSmall = FONTMNG->Load( FONT_SYSTEM, (int)(FONT_SIZE_DEFAULT / 1.5f) );
	if( XBREAKF( m_pfdSystemSmall == NULL, "load font size10: %s......failed", FONT_SYSTEM ) )
		return FALSE;
	XLOGXN("create system font success.");
	return TRUE;
}

void XEContent::OnBeforeDrawByPopup( XWndView *pView )
{
	if( pView->GetbModal() )
		GRAPHICS->FillRectSize( XE::VEC2( 0 ), XGAME_SIZE, XCOLOR_RGBA( 0, 0, 0, 200 ) );
}

XWndDarkBg* XEContent::CreateDarkBg()
{
	return new XWndDarkBg();
}

/**
 @brief 콜백데이타 큐에서 뽑은 데이타를 받음.
 @return 콜백이벤트를 정상처리했다면 true, 실패해서 콜백큐에서 삭제하길 원하지 않는다면 
*/
bool XEContent::OnReceiveCallbackData( const xnReceiverCallback::xData& data )
{
	bool bOk = true;
	// 엔진단에서 처리할게 있으면 이곳에 구현.
	return bOk;		// 이벤트 처리가 실패하여 다시 한번 받고싶다면 false를 리턴.
}

/**
 @brief 
*/
int XEContent::__OnClickTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("__OnClickTooltip");
	// pWnd에서 툴팁정보를 꺼냄
	// this에 툴팁 윈도우를 띄운다.
	OnClickTooltip( pWnd );		// virtual

	return 1;
}

void XEContent::OnLButtonDown( float lx, float ly )
{
	m_bTouched = true;
	m_bDraging = false;
	XWnd::OnLButtonDown( lx, ly );
}
void XEContent::OnLButtonUp( float lx, float ly )
{
	m_bTouched = false;
	m_bDraging = false;
	XWnd::OnLButtonUp( lx, ly );
}

void XEContent::OnMouseMove( float lx, float ly )
{
	const XE::VEC2 vTouch( lx, ly );
	static XE::VEC2 s_vPrev( vTouch );
	if( !(( vTouch - s_vPrev ).IsZero()) ) {
		// 터치한채 움직이고 있으면 드래깅중.
		m_bDraging = m_bTouched;
	} else {
		m_bDraging = false;
	}
	s_vPrev = vTouch;
	XWnd::OnMouseMove( lx, ly );
}

int XEContent::Process( float dt )
{
	IMAGE_MNG->Process( m_bDraging );
#ifdef _XASYNC_SPR
	SPRMNG->Process();
#endif // _XASYNC_SPR
	return XWnd::Process( dt );
}

