#include "stdafx.h"
#include "XWndPopup.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XWndButton.h"
#include "XFramework/client/XEContent.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


/**
 @brief 팝업 배경이미지가 점점 커지며 나타나는 팝업
 레이아웃의 루트는 반드시 "popup"컨트롤이어야 한다.
 @note 현재는 반드시 "popup"을 루트로 해서 컨트롤들을 배치해야 하지만 편의성 문제로
 "popup"없이도 사용할수 있게 개선되어야 한다.
*/
XWndPopup::XWndPopup( XLayout *pLayout, const char *cKey, const char *cGroup/* = nullptr*/ ) 
//	: XWndView( pLayout, cKey, cGroup ) {
{
	Init();
	XWnd *pRoot = nullptr;
	SetEnableNcEvent( TRUE );
	pLayout->CreateLayout( cKey, this, cGroup );
	// this의 최대 사이즈로 this의 사이즈를 정한다.
	auto sizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocal );
	XE::VEC2 vSizeParent = XE::GetGameSize();
	XE::VEC2 vSizePopup = GetSizeLocal();
	XE::VEC2 vLT = ( vSizeParent / 2.f ) - ( vSizePopup / 2.f );
	SetPosLocal( vLT );
	SetOkButton( "butt.ok" );
}

XWndPopup::XWndPopup( LPCTSTR szLayout, const char *cNodeName, XWnd *pParent ) 
{
	Init();
	SetEnableNcEvent( TRUE );
	SetLayout( _tstring(szLayout), std::string(cNodeName), pParent );
}

void XWndPopup::Destroy() 
{
	SAFE_DELETE( m_pLayoutObj );
}

XLayout* XWndPopup::GetpLayout() 
{
	XBREAK( m_pLayoutObj == nullptr );
	return m_pLayoutObj->GetpLayout();
}

void XWndPopup::SetLayout( const _tstring& strtXml, const std::string& strcNode, XWnd* pParent )
{
	m_pLayoutObj = new XLayoutObj( strtXml );
	if( !m_pLayoutObj->GetpLayout() )
		return;
//	m_pLayoutObj->CreateLayout2( strcNode, this );
	m_pLayoutObj->CreateLayout( strcNode, this );
	// this의 최대 사이즈로 this의 사이즈를 정한다.
	auto sizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocal );
	XE::VEC2 vSizeParent;
	if( pParent )
		vSizeParent = pParent->GetSizeFinal();
	else
		vSizeParent = XE::GetGameSize();
	XE::VEC2 vLT = ( vSizeParent / 2.f ) - ( sizeLocal / 2.f );
	vLT.y += 7.f;
	SetPosLocal( vLT );
	SetCancelButton( "butt.cancel" );
	SetCancelButton( "butt.close" );
	SetOkButton( "butt.ok" );
}
XWndButtonString* XWndPopup::AddButton( const char *cIdentifier, 
										LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont, 
										LPCTSTR szPngHeader )
{
//	XBREAK( XE::IsEmpty(szLabel) );
	_tstring strUp = szPngHeader;
	_tstring strDown = szPngHeader;
	_tstring strDis = szPngHeader;
	strUp += _T("_u.png");
	strDown += _T("_d.png");
	strDis += _T("_off.png");
	XWndButtonString *pButt = new XWndButtonString( 0, 0, szLabel, 
													XCOLOR_WHITE,
													szFont, sizeFont, 
													strUp.c_str(),
													strDown.c_str(),
													strDis.c_str() );
	pButt->SetstrIdentifier( cIdentifier );
	pButt->SetStyle( xFONT::xSTYLE_SHADOW );
	Add( pButt );
	m_listButtons.Add( pButt );
	AdjustButtons();
//	SetEnableNcEvent( TRUE );
	return pButt;
}

XWndButtonString* XWndPopup::AddButtonImg( const char *cIdentifier, 
										LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont, 
										LPCTSTR szImg )
{
//	XBREAK( XE::IsEmpty(szLabel) );
	XWndButtonString *pButt = new XWndButtonString( 0, 0, szLabel, 
													XCOLOR_WHITE,
													szFont, sizeFont, 
													szImg,
													nullptr );
	pButt->SetstrIdentifier( cIdentifier );
	pButt->SetStyle( xFONT::xSTYLE_SHADOW );
	Add( pButt );
	m_listButtons.Add( pButt );
	AdjustButtons();
	return pButt;
}


/**
 @brief 
*/
XWndButton* XWndPopup::AddButton( XWnd::xtAlert type,
										LPCTSTR szLabel,
										LPCTSTR szButtPng )
{
	XWndButton* pButt = nullptr;
	if( szLabel != nullptr ) {
		pButt = new XWndButtonString( 0, 0, szLabel,
														XCOLOR_WHITE,
														NULL, szButtPng );
	} else {
		pButt = new XWndButton( 0, 0, szButtPng, nullptr );
	}
//	pButt->SetStyle( xFONT::xSTYLE_SHADOW );
	// 디폴트 타입 핸들러
	if( type == XWnd::xNO )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnNo );
	else
	if( type == XWnd::xCANCEL )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnCancel );
	else
	if( type == XWnd::xYES )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnYes );
	else
	if( type == XWnd::xOK )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnOk );
	Add( pButt );
	m_listButtons.Add( pButt );
	AdjustButtons();
	return pButt;
}

/**
 @brief 
*/
XWndButtonString* XWndPopup::AddButton( XWnd::xtAlert type, 
										LPCTSTR szLabel,
										LPCTSTR szFont,
										float sizeFont,
										LPCTSTR szPngHeader )
{
//	XBREAK( XE::IsEmpty(szLabel) );
	_tstring strUp = szPngHeader;
	_tstring strDown = szPngHeader;
	_tstring strDis = szPngHeader;
	strUp += _T("_u.png");
	strDown += _T("_d.png");
	strDis += _T("_off.png");
	XWndButtonString *pButt = new XWndButtonString( 0, 0, szLabel, 
													XCOLOR_WHITE,
													szFont, sizeFont, 
													strUp.c_str(),
													strDown.c_str(),
													strDis.c_str() );
	// 디폴트 타입 핸들러
	if( type == XWnd::xNO )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnNo );
	else
	if( type == XWnd::xCANCEL )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnCancel );
	else
	if( type == XWnd::xYES )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnYes );
	else
	if( type == XWnd::xOK )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnOk );
	pButt->SetStyle( xFONT::xSTYLE_SHADOW );
	Add( pButt );
	m_listButtons.Add( pButt );
	AdjustButtons();
	return pButt;
}

/**
 @brief cKey버튼을 cancel타입의 버튼으로 지정한다.
*/
XWndButton* XWndPopup::SetCancelButton( const char *cKey )
{
	auto pButt = xGET_BUTT_CTRL( this, cKey );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnCancel );
	return pButt;
}

/**
 @brief cKey버튼을 ok타입의 버튼으로 지정한다.
*/
XWndButton* XWndPopup::SetOkButton( const char *cKey )
{
	auto pButt = xGET_BUTT_CTRL( this, cKey );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopup::OnOk );
	return pButt;
}

void XWndPopup::AdjustButtons( void )
{
	const XE::VEC2 vSizeThis = GetSizeLocal();
	int numButt = m_listButtons.size();
	if( numButt == 0 )
		return;
	XE::VEC2 v;
	v.x += m_vLTMargin.w;
	float maxWidth = vSizeThis.w - (m_vRBMargin.w + m_vLTMargin.w);
	float wSection = maxWidth / numButt;
	for( auto pButt : m_listButtons )
	{
		XE::VEC2 vSize = pButt->GetSizeLocal();
		XE::VEC2 vPos;
//		vPos.x = v.x + (wSection / 2.f) - (vSize.w / 2.f);
		vPos.x = v.x + ( wSection / 2.f ) - ( vSize.w / 2.f );
		vPos.y = vSizeThis.h - m_vRBMargin.h - (vSize.h/* + vSize.h / 2.f*/);
		pButt->SetPosLocal( vPos );
		v.x += wSection;
	}
}

int XWndPopup::OnOk( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_OK );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}
int XWndPopup::OnCancel( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_CANCEL );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}
int XWndPopup::OnYes( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_YES );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}
int XWndPopup::OnNo( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_NO );
	XE::GetGame()->OnClosePopup( GetstrIdentifier() );
	return 1;
}

XWndButton* XWndPopup::GetButton( const char *csid )
{
	for( auto pButt : m_listButtons ) {
		if( pButt->GetstrIdentifier() == csid )
			return pButt;
	}
	return nullptr;
}

BOOL XWndPopup::OnKeyUp( int keyCode ) 
{
	if( keyCode == XE::KEY_BACK ) {
		if( Find("butt.cancel") || Find("butt.close") )
			OnCancel( NULL, 0, 0 );
		else if( Find("butt.ok")  )
			OnOk( nullptr, 0, 0 );
		return TRUE;
	}
	return FALSE;
}

void XWndPopup::OnNCLButtonDown( float lx, float ly )
{
	m_bNCTouch = true;
}
void XWndPopup::OnNCLButtonUp( float lx, float ly )
{
	if( m_bNCTouch ) {
		// 캔슬이나 close버튼(X)이 있을때만 창밖을 터치했을때 꺼져야한다.
		auto pButt = Find( "butt.cancel" );
		if( !pButt )
			pButt = Find( "butt.close" );
		if( pButt ) {
			OnCancel( pButt, 0, 0 );		// 그냥 죽이면 안되고 정식 close popup프로세스를 거쳐야 함.
		} else {
			SetbDestroy( true );
		}
	}
	m_bNCTouch = false;
}

