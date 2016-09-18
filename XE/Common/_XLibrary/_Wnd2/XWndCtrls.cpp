#include "stdafx.h"
#include "XWndCtrls.h"
#include "XWndButton.h"
#include "XWndImage.h"
#include "XFramework/client/XLayout.h"
//#include "../XDrawGraph.h"
#include "XSoundMng.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XEProfile.h"
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef max
#undef  max;
#endif
#ifdef min
#undef  min;
#endif

//////////////////////////////////////////////////////////////////////////
XWndAlert::XWndAlert( float w, float h )
	: XWndView( 0, 0, w, h, _T("popup01.png") )
{
	Init();
	// 32단위로 올림처리 한다.
	SetSizePopup( w, h );
	AutoLayoutCenter( XEContent::sGet() );
}

XWndAlert::XWndAlert( LPCTSTR szLayout, const char *cKey )
	: XWndView( 0, 0, 1, 1, _T( "popup01.png" ) )
	, m_Layout( szLayout )
{
	Init();
	m_Layout.CreateLayout( cKey, this );
	SetAutoSize();
//	auto size = GetSizeFinal();
	auto size = GetSizeLocalNoTrans();
	SetSizePopup( size.w, size.h );
	AutoLayoutCenter( XEContent::sGet() );
}

XWndAlert::XWndAlert( XWndAlertDelegate *pDelegate,
						float w, float h, 
						LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
						LPCTSTR szImgUp,		// 버튼이미지
						LPCTSTR szImgDown,
						LPCTSTR szImgDisable,
						LPCTSTR _szText, 
						LPCTSTR _szTitle, 
						XWnd::xtAlert type, 
						XCOLOR col )
	: XWndView( 0, 0, w, h, szFrameImg  )
{
	Init();
	// 32단위로 올림처리 한다.
	w = (float)XE::RoundUpDivVal( (int)w, 32 );
	h = (float)XE::RoundUpDivVal( (int)h, 32 );
	float x = XE::GetGameWidth() / 2.f - w / 2.f;
	float y = XE::GetGameHeight() / 2.f - h / 2.f;
	
	CreateWndAlert( pDelegate, x, y, w, h, szFrameImg, szImgUp, szImgDown, szImgDisable, _szText, _szTitle, type, col );
}
XWndAlert::XWndAlert( XWndAlertDelegate *pDelegate,
						float x, float y, float w, float h, 
						LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
						LPCTSTR szImgUp,		// 버튼이미지
						LPCTSTR szImgDown,
						LPCTSTR szImgDisable,
						LPCTSTR _szText, 
						LPCTSTR _szTitle, 
						XWnd::xtAlert type, 
						XCOLOR col )
	: XWndView( x, y, w, h, szFrameImg  )
{
	Init();
	CreateWndAlert( pDelegate, x, y, w, h, szFrameImg, szImgUp, szImgDown, szImgDisable, _szText, _szTitle, type, col );
}

void XWndAlert::SetSizePopup( float w, float h )
{
	w = (float)XE::RoundUpDivVal( (int)w, 32 );
	h = (float)XE::RoundUpDivVal( (int)h, 32 );
// 	float x = XE::GetGameWidth() / 2.f - w / 2.f;
// 	float y = XE::GetGameHeight() / 2.f - h / 2.f;
	SetSizeLocal( w, h );
// 	SetPosLocal( x, y );
}

void XWndAlert::CreateWndAlert( XWndAlertDelegate *pDelegate )
{
	auto vPos = GetPosLocal();
	auto vSize = GetSizeFinal();
	CreateWndAlert( pDelegate, 
					vPos.x, vPos.y, vSize.w, vSize.h,
					m_strFrameImg.c_str(),
					m_strImgUp.c_str(), nullptr, nullptr,
					m_strText.c_str(),
					m_strTitle.c_str(),
					m_Type, m_Color );
}

void XWndAlert::CreateWndAlert( XWndAlertDelegate *pDelegate,
								float x, float y, float w, float h, 
								LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
								LPCTSTR szImgUp,		// 버튼이미지
								LPCTSTR szImgDown,
								LPCTSTR szImgDisable,
								LPCTSTR _szText, 
								LPCTSTR _szTitle, 
								XWnd::xtAlert type, 
								XCOLOR col )
{
	SetbModal( TRUE );		// 디폴트로 모달상태다
	SetPosLocal( x, y );
	SetSizeLocal( w, h );
	m_pDelegate = pDelegate;
	m_Color = col;
	m_pFDText = FONTMNG->Load( FONT_SYSTEM, m_sizeFont );
	const XE::VEC2 vSizeThis = GetSizeLocal();
// 	TCHAR szText[ 1024 ] = { 0, };
// 	TCHAR szTitle[ 256 ] = { 0, };
	if( XE::IsHave(_szText) ) {
		m_strText = _szText;
//		_tcscpy_s( szText, _szText );	// 외부에서 s_szTemp같은 공용버퍼가 넘어올수 있으므로 따로 받아서 쓴다.
	}
	if( XE::IsHave(_szTitle) ) {
		m_strTitle = _szTitle;
	}
	//	_tcscpy_s( szTitle, _szTitle );
	// 타이틀
	XWndTextString *pText = NULL;
	XE::VEC2 vPos = XE::VEC2( 0, 4 );
	XE::VEC2 vSize = XE::VEC2( vSizeThis.w, 20.f );
	// 타이틀
	if( m_strTitle.empty() )
		pText = new XWndTextString( vPos, vSize, GetText(1), m_pFDText, col );	// 디폴트로 "알림"
	else
		pText = new XWndTextString( vPos, vSize, m_strTitle, m_pFDText, col );	
	pText->SetStyle( xFONT::xSTYLE_STROKE );
	m_pTextTitle = pText;
	if( pText )
		Add( pText );
	// 알림창 가운데 알림메시지를 중앙에.
	vPos = XE::VEC2( 0, 30 );
	vSize = XE::VEC2( vSizeThis.w, 71.f );	
	// 설명 텍스트
	pText = new XWndTextString( vPos, vSize, m_strText, m_pFDText, col );
	pText->SetStyle( xFONT::xSTYLE_SHADOW );
	pText->SetAlign( XE::xALIGN_CENTER );		// 중앙 정렬
//	pText->SetColorText( XCOLOR_ORANGE );
	m_pTextDesc = pText;
	Add( pText );
	// 타입에 따라 버튼
	XWndButtonString *pWnd = NULL;
	if( type == xOK ) {
		// 116, 131
		pWnd = new XWndButtonString( 0, 0, GetText(2), col, m_pFDText, szImgUp, szImgDown, szImgDisable );
		XE::VEC2 size = pWnd->GetSizeFinal();
		XE::VEC2 vPos;
		vPos.x = (vSizeThis.w / 2.f) - (size.w / 2.f);
		vPos.y = vSizeThis.h - m_vRBMargin.h - size.h;
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnOk );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		Add( XE::xID_OK, pWnd );
		m_pButton1 = pWnd;
		pWnd->SetstrIdentifier( "butt.ok" );
	} else
	if( type == xCLOSE ) {
		// 116, 131
		pWnd = new XWndButtonString( 0, 0, GetText(8), col, m_pFDText, szImgUp, szImgDown, szImgDisable );
		XE::VEC2 size = pWnd->GetSizeFinal();
		XE::VEC2 vPos;
		vPos.x = (vSizeThis.w / 2.f) - (size.w / 2.f);
		vPos.y = vSizeThis.h - m_vRBMargin.h - size.h;
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnCancel );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		Add( XE::xID_CANCEL, pWnd );
		m_pButton1 = pWnd;
		pWnd->SetstrIdentifier( "butt.close" );
	} else
	if( type == xOKCANCEL )
	{
		XE::VEC2 size;
		XE::VEC2 vPos;
		// 49, 131
		pWnd = new XWndButtonString( 0, 0, GetText(2), col, m_pFDText, szImgUp, szImgDown, szImgDisable );		// 확인
		size = pWnd->GetSizeFinal();
		vPos.x = (vSizeThis.w / 4.f) - (size.w / 2.f);
		vPos.y = vSizeThis.h - m_vRBMargin.h - size.h;
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnOk );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		pWnd->SetstrIdentifier( "butt.ok" );
		Add( XE::xID_OK, pWnd );
		m_pButton1 = pWnd;
		pWnd = new XWndButtonString( 181.f, 131.f, GetText(3), col, m_pFDText, szImgUp, szImgDown, szImgDisable );		// 취소
		size = pWnd->GetSizeFinal();
		vPos.x = ((vSizeThis.w / 4.f) * 3.f) - (size.w / 2.f);
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnCancel );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		pWnd->SetstrIdentifier( "butt.cancel" );
		Add( XE::xID_CANCEL, pWnd );
		m_pButton2 = pWnd;
	} else
	if( type == xYESNO )
	{
		XE::VEC2 size;
		XE::VEC2 vPos;
		pWnd = new XWndButtonString( 49.f, 131.f, GetText(4), col, m_pFDText, szImgUp, szImgDown, szImgDisable );		// 확인
		size = pWnd->GetSizeFinal();
		vPos.x = (vSizeThis.w / 4.f) - (size.w / 2.f);
		vPos.y = vSizeThis.h - m_vRBMargin.h - size.h;
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnYes );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		Add( XE::xID_YES, pWnd );
		m_pButton1 = pWnd;
		pWnd->SetstrIdentifier( "butt.yes" );
		pWnd = new XWndButtonString( 181.f, 131.f, GetText(5), col, m_pFDText, szImgUp, szImgDown, szImgDisable );		// 취소
		size = pWnd->GetSizeFinal();
		vPos.x = ((vSizeThis.w / 4.f) * 3.f) - (size.w / 2.f);
		pWnd->SetPosLocal( vPos );
		pWnd->SetEvent( XWM_CLICKED, this, &XWndAlert::OnNo );
		pWnd->SetStyle( xFONT::xSTYLE_SHADOW );
		Add( XE::xID_NO, pWnd );
		m_pButton2 = pWnd;
		pWnd->SetstrIdentifier( "butt.no" );
	} else
	if( type == xALERT_NONE )
	{
		// 버튼이 없는 알림창
	} else
		XBREAK(1);

}

void XWndAlert::Update()
{
	XE::VEC2 vPos = XE::VEC2( 0, 4 );
	const XE::VEC2 vSizeThis = GetSizeLocal();
	XE::VEC2 vSize = XE::VEC2( vSizeThis.w, 20.f );
	if( !m_pTextTitle ) {
		auto pText = new XWndTextString( vPos + XE::VEC2(0,-10), vSize, _T(""), m_pFDText, m_Color );
		pText->SetStyle( xFONT::xSTYLE_STROKE );
		pText->SetAlignHCenter();
		m_pTextTitle = pText;
		Add( pText );
	}
	m_pTextTitle->SetText( m_strTitle );
	m_pTextTitle->SetColorText( m_Color );

	if( !m_pTextDesc ) {
		// 설명 텍스트
		auto pText = new XWndTextString( vPos, vSize, _T(""), m_pFDText, m_Color );
		pText->SetStyle( xFONT::xSTYLE_SHADOW );
		pText->SetAlign( XE::xALIGN_CENTER );		// 중앙 정렬
		m_pTextDesc = pText;
		Add( pText );
	}
	m_pTextDesc->SetText( m_strText );
	m_pTextDesc->SetColorText( m_Color );
	XWndView::Update();
}

XWndTextString* XWndAlert::SetText( LPCTSTR szText )
{
	if( m_pTextDesc == NULL )
		return NULL;
	m_pTextDesc->SetText( szText );
	return m_pTextDesc;
}

XWndButtonString* XWndAlert::CreateButtonAndAdd( LPCTSTR szLabel, 
												LPCTSTR szFont,
												float sizeFont,
												LPCTSTR szImgUp,
												LPCTSTR szImgDown,
												LPCTSTR szImgOff )
{
	_tstring strLabel = (szLabel)? szLabel : _T("");
	_tstring strUp = (szImgUp == nullptr)? _T("") : szImgUp;
	_tstring strDown = (szImgDown == nullptr)? _T("") : szImgDown;
	_tstring strDis = (szImgOff == nullptr)? _T("") : szImgOff;
	XWndButtonString *pButt = new XWndButtonString( 0, 0, strLabel.c_str(), 
												XCOLOR_WHITE,
												szFont, sizeFont, 
												strUp.c_str(),
												strDown.c_str(),
												strDis.c_str() );
	pButt->SetStyle( xFONT::xSTYLE_SHADOW );
	Add( pButt );
	m_listButtons.Add( pButt );
	AdjustButtons();
	return pButt;
}

/**
 @brief 버튼 이미지 이름을 직접 입력하는 방식
 이미지 하나만 사용하는 바운스식 버튼에만 유효하다.
*/
XWndButtonString* XWndAlert::AddButtonDirectly( const char *cIdentifier, 
										LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont, 
										LPCTSTR szImgUp,
										LPCTSTR szImgDown,
										LPCTSTR szImgOff/*=nullptr*/ )
{
	XWndButtonString *pButt = CreateButtonAndAdd( szLabel, szFont, sizeFont, szImgUp, szImgDown, szImgOff );
	XBREAK( pButt == nullptr );
	pButt->SetstrIdentifier( cIdentifier );
	m_pButton1 = pButt;		// 터치하면 자동으로 안꺼지게 하려고 한 땜빵처리
	return pButt;
}

/**
 @brief 미리지정된 버튼 핸들러를 사용하는 버전.
*/
XWndButtonString* XWndAlert::AddButtonDirectly( XWnd::xtAlert type, 
										LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont, 
										LPCTSTR szImgUp,
										LPCTSTR szImgDown,
										LPCTSTR szImgOff/*=nullptr*/ )
{
	XWndButtonString *pButt = CreateButtonAndAdd( szLabel, szFont, sizeFont, szImgUp, szImgDown, szImgOff );
	XBREAK( pButt == nullptr );
	// 디폴트 타입 핸들러
	if( type == XWnd::xNO )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnNo );
	else
	if( type == XWnd::xCANCEL )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnCancel );
	else
	if( type == XWnd::xYES )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnYes );
	else
	if( type == XWnd::xOK )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnOk );
	return pButt;
}

/**
 @brief 버튼이미지를 사용할때 정해진 규격에 따른 네이밍을 사용하는 버전
 up버튼:  헤더_u.png
 down버튼: 헤더_d.png
 disable버튼: 헤더_off.png
*/
XWndButtonString* XWndAlert::AddButtonWithHeader( const char *cIdentifier, 
										LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont, 
										LPCTSTR szPngHeader )
{
	_tstring strUp = szPngHeader;
	_tstring strDown = szPngHeader;
	_tstring strDis = szPngHeader;
	strUp += _T("_u.png");
	strDown += _T("_d.png");
	strDis += _T("_off.png");
	XWndButtonString *pButt = CreateButtonAndAdd( szLabel, 
												szFont, 
												sizeFont, 
												strUp.c_str(),
												strDown.c_str(),
												strDis.c_str() );
	XBREAK( pButt == nullptr );
	pButt->SetstrIdentifier( cIdentifier );
	return pButt;
}

/**
 @brief 버튼의 타입을 넣어 미리 지정된 디폴트 핸들러를 사용하도록 하는 버전.

*/
XWndButtonString* XWndAlert::AddButtonWithHeader( XWnd::xtAlert type, 
										LPCTSTR szLabel,
										LPCTSTR szFont,
										float sizeFont,
										LPCTSTR szPngHeader )
{
	_tstring strUp = szPngHeader;
	_tstring strDown = szPngHeader;
	_tstring strDis = szPngHeader;
	strUp += _T("_u.png");
	strDown += _T("_d.png");
	strDis += _T("_off.png");
	XWndButtonString *pButt = CreateButtonAndAdd( szLabel, 
												szFont, 
												sizeFont, 
												strUp.c_str(),
												strDown.c_str(),
												strDis.c_str() );
	// 디폴트 타입 핸들러
	if( type == XWnd::xNO )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnNo );
	else
	if( type == XWnd::xCANCEL )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnCancel );
	else
	if( type == XWnd::xYES )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnYes );
	else
	if( type == XWnd::xOK )
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnOk );
	return pButt;
}


void XWndAlert::AdjustButtons( void )
{
	const XE::VEC2 vSizeThis = GetSizeLocal();
	int numButt = m_listButtons.size();
	if( numButt == 0 )
		return;
	XE::VEC2 v;
	float wSection = vSizeThis.w / numButt;
	XLIST_LOOP2( m_listButtons, XWndButton*, ppButt )
	{
		XE::VEC2 vSize = (*ppButt)->GetSizeLocal();
		XE::VEC2 vPos;
		vPos.x = v.x + (wSection / 2.f) - (vSize.w / 2.f);
//		vPos.y = vSizeThis.h - m_vRBMargin.h - (vSize.h + vSize.h / 2.f);
		vPos.y = vSizeThis.h - m_vRBMargin.h - vSize.h;
		(*ppButt)->SetPosLocal( vPos );
		v.x += wSection;
	} END_LOOP;
}

void XWndAlert::Destroy()
{
	SAFE_RELEASE2( FONTMNG, m_pFDText );
}

int XWndAlert::OnOk( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_OK );
	return 1;
}
int XWndAlert::OnCancel( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_CANCEL );
	return 1;
}
int XWndAlert::OnYes( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_YES );
	return 1;
}
int XWndAlert::OnNo( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	SetbDestroy( TRUE );
	CallEventHandler( XWM_NO );
	return 1;
}

LPCTSTR XWndAlert::GetText( ID idText )
{
	// 델리게이트가 있으면 델리게이트에게 먼저 물어본다.
	if( m_pDelegate )
		return m_pDelegate->OnDelegateAlertGetTextLable( this, idText );
	LPCTSTR szText = XTEXT(idText);
	if( XE::IsHave(szText) )
		return szText;
	// 텍스트가 없으면 디폴트 문자열을 돌려준다.
	switch( idText )
	{
	case 1:	return _T("alert");
	case 2:	return _T("ok");
	case 3:	return _T("cancel");
	case 4: return _T("yes");
	case 5: return _T("no");
	default:
		XBREAKF(1, "unknown idText:%d", idText );
	}
	return _T("");
}

void XWndAlert::SetCancelButton( const char *cKey )
{
	auto pButt = dynamic_cast<XWndButtonString*>( Find( cKey ) );
	if( pButt )
	{
		m_pButton1 = pButt;
		pButt->SetEvent( XWM_CLICKED, this, &XWndAlert::OnCancel );
	}
}

BOOL XWndAlert::OnKeyUp( int keyCode )
{
	if( keyCode == XE::KEY_BACK ) {
		if( Find( "butt.cancel" ) || Find( "butt.close" ) )
			OnCancel( NULL, 0, 0 );
		else if( Find( "butt.ok" ) )
			OnOk( nullptr, 0, 0 );
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 시간기반으로 바꿔야 할듯하다. 1초에서 3초사이 이런식으로 지정할수 있게
int XWndTextNumberCounter::Process( float dt )
{
	if( m_bSwitch )	{
		int add = 0;
		if( m_numMax && m_Divide ) {
			add = (int)(((m_numMax - m_numStart) / m_Divide) * dt);
			if( add == 0 )
				add = 1;
			m_numCurr += add;
		}
		if( (add > 0 && m_numCurr >= m_numMax) ||
			(add < 0 && m_numCurr <= m_numMax) ||
			m_numMax == 0 || m_numMax < m_Divide ) {
			m_numCurr = m_numMax;
			m_bSwitch = FALSE;
			CallEventHandler( XWM_FINISH_NUMBER_COUNTER );
		}
		UpdateText();
	}
	return 1;
}


void XWndTextNumberCounter::UpdateText( void )
{
	if( m_bComma ) {
		TCHAR szBuff[ 256 ];
		if( m_bSymbol && m_numCurr > 100000 )		// bComma버전에서만 지원된다.
			XE::NumberToMoneyStringK( szBuff, m_numCurr );
		else
			XE::NumberToMoneyString( szBuff, m_numCurr );
		SetText( szBuff );
	} else
		SetText( XE::Format( _T("%d"), m_numCurr ) );
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
XWndNotification::XWndNotification( float w, float h, LPCTSTR szTitle, LPCTSTR szMsg, XBaseFontDat *pFontDat ) 
	: XWndView( 0, 0, w, h, _T("baseDialog.png") ) 
{ 
	Init(); 
	float lw=0;
	XWndTextString *pWnd1=NULL, *pWnd2=NULL; 
	if( szTitle )
	{
//		_tcscpy_s( m_szTitle, szTitle );
		pWnd1 = new XWndTextString( w/2.f, 5.f, szTitle, pFontDat, XCOLOR_WHITE );
		Add( 1, pWnd1 );
		pWnd1->SetAlign( XE::xALIGN_CENTER );
		XE::VEC2 size = pWnd1->GetpFont()->GetLayoutSize( szTitle );
		lw = xmax( lw, size.x );
	}
	if( szMsg ) 
	{
//		_tcscpy_s( m_szMsg, szMsg );
		pWnd2 = new XWndTextString( w/2.f, 20.f, szMsg, pFontDat, XCOLOR_WHITE );
		Add( 2, pWnd2 );
		pWnd2->SetAlign( XE::xALIGN_CENTER );
		XE::VEC2 size = pWnd2->GetpFont()->GetLayoutSize( szMsg );
		lw = xmax( lw, size.x );
	}
	//
	if( pWnd1 )
		pWnd1->SetPosLocal( lw/2.f, 5.f );
	if( pWnd2 )
		pWnd2->SetPosLocal( lw/2.f, 20.f );
	
	SetSize( lw, 32.f );
	m_Timer.Set( 3.0f );
	//
}
void XWndNotification::Destroy() 
{
//	SAFE_DELETE( m_pTitle );
//	SAFE_DELETE( m_pMsg );
}
void XWndNotification::Draw( void )
{
	XWndView::Draw();
//	if( m_pTitle )
//		m_pTitle->Draw();
//	if( m_pMsg )
//		m_pMsg->Draw();
}

//------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// 등장 이펙트

int XWndEffectGradually::Process( float dt )
{
	switch( m_State )
	{
	case 0:	break;		// 암것도 안하는 상태
	case 2:	break;		// 암것도 안하는 상태
	case 1:				// 진행중
		{
			XE::VEC2 vPosLocal = GetPosLocal();
			float lerpTime = (float)m_Timer.GetPassTime() / m_Timer.GetWaitTime();	// 진행 시간
			if( lerpTime > 1.0f )
				lerpTime = 1.0f;
			float slerp = XE::xiCatmullrom( lerpTime, -10.0f, 0, 1, 1 );		// 스플라인 방식 보간
			XE::VEC2 vLen = m_vPosEnd - m_vPosStart;		// 이동구간의 길이벡터
			XE::VEC2 vPos = m_vPosStart + vLen * slerp;	// 현재 좌표 계산
			float scale = slerp;		// 현재 스케일링
			SetPosLocal( vPos );		// 좌표 갱신
			SetScaleLocal( scale );	// 크기 갱신
			float lerpAlpha = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
			SetAlphaLocal( lerpAlpha );	// 알파 갱신. 알파는 선형보간 하자
			if( lerpTime >= 1.0f )
				m_State = 2;		// 끝
		}
		break;
	}
	return 1;		// 현재 사용안하는듯
}
//////////////////////////////////////////////////////////////////
// 슬라이더 컨트롤
void XWndSlider::OnLButtonDown( float lx, float ly )
{
	m_Push = 1;
	XE::VEC2 size = GetSizeFinal();
	float lerp = lx / size.w;		// 0 ~ 1로 환산
	float w = m_Max - m_Min;
	m_Curr = m_Min + w * lerp;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
	SetCapture();
}
void XWndSlider::OnMouseMove( float lx, float ly )
{
	if( m_Push )
	{
		XE::VEC2 size = GetSizeFinal();
		float lerp = lx / size.w;		// 0 ~ 1로 환산
		float w = m_Max - m_Min;
		m_Curr = m_Min + w * lerp;
		if( m_Curr < m_Min )
			m_Curr = m_Min;
		else
		if( m_Curr > m_Max )
			m_Curr = m_Max;
		m_Push = 2;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
	}
}
void XWndSlider::OnLButtonUp( float lx, float ly )
{
	ReleaseCapture();
	m_Push = 0;
	CallEventHandler( XWM_SLIDING, *((DWORD*)(&m_Curr)) );
}
//======================================================
// 탭 컨트롤에 탭을 추가한다. x,y,w,h는 탭클릭 영역이다
XWndTab::XWndTab( float x, float y, float w, float h, LPCTSTR szFont ) 
	: XWnd( x, y, w, h )	
{ 
	Init(); 
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
}
void XWndTab::Destroy( void )
{
	SAFE_DELETE( m_pFontObj );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		SAFE_DELETE( pTab->psfcImg );
		SAFE_DELETE( pTab );
	} END_LOOP;
}
void XWndTab::AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImg, float x, float y, float w, float h )
{
	TAB *pTab = new TAB;
	pTab->idTab = idTab;
	pTab->strImg = szImg;
	pTab->x = x;
	pTab->y = y;
	pTab->w = w;
	pTab->h = h;
	if( szLabel )
		pTab->strLabel = szLabel;
//	pTab->psfcImg = GRAPHICS->CreateSurfaceFromFile( XE::MakePath( DIR_UI, szImg ), 281, 414 );
	pTab->psfcImg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
	m_listTab.Add( pTab );
	if( m_pSelectedTab == NULL )
		m_pSelectedTab = pTab;
}

void XWndTab::OnLButtonDown( float lx, float ly )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( XE::IsArea2( pTab->x, pTab->y, pTab->w, pTab->h, lx, ly ) )
		{
			m_pSelectedTab = pTab;
			OnClickedTab( pTab );
//			CallEventHandler( XWM_CLICKED, pTab->idTab );
			break;
		}
	} END_LOOP;
	XWnd::OnLButtonDown( lx, ly );
}

void XWndTab::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 vPos = GetPosFinal();
	m_pSelectedTab->psfcImg->Draw( vPos );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->strLabel.empty() == false )
			DrawTabLabel( pTab );
	} END_LOOP;
	XWnd::Draw();
}

void XWndTab::DrawTabLabel( TAB *pTab )
{
	m_pFontObj->SetLineLength( pTab->w );
	float yCenter = pTab->y + (pTab->h / 2.f);
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += pTab->x;
	vPos.y += yCenter - 5.f;
	m_pFontObj->DrawString( vPos.x, vPos.y, pTab->strLabel.c_str() );
}

XWndTab::TAB* XWndTab::GetTab( ID idTab )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->idTab == idTab )
			return pTab;
	} END_LOOP;
	return NULL;
}
//======================================================
// 탭 컨트롤에 탭을 추가한다. x,y,w,h는 탭클릭 영역이다
XWndTab2::XWndTab2( float x, float y, LPCTSTR szImg, LPCTSTR szFont, XLayout *pLayout/*=NULL*/ ) 
	: XWnd( x, y )	
{ 
	Init(); 
	m_pLayout = pLayout;
	// 뒷배경
	if( szImg )
	{
		m_psfcBg = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImg ) );
		SetSizeLocal( m_psfcBg->GetWidth(), m_psfcBg->GetHeight() );
	}
	// 폰트
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
	m_pFontObj->SetLineLength( GetSizeFinal().w );
}
XWndTab2::XWndTab2( float x, float y, float w, float h, LPCTSTR szFont, XLayout *pLayout/*=NULL*/ ) 
	: XWnd( x, y, w, h )	
{ 
	Init(); 
	m_pLayout = pLayout;
	// 폰트
	m_pFontObj = FONTMNG->CreateFontObj( szFont, 20.f );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
	m_pFontObj->SetLineLength( GetSizeFinal().w );
}
void XWndTab2::Destroy( void )
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
	SAFE_DELETE( m_pFontObj );
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		SAFE_RELEASE2(IMAGE_MNG, pTab->psfcImgOn );
		SAFE_RELEASE2(IMAGE_MNG, pTab->psfcImgOff );
		SAFE_DELETE( pTab );
	} END_LOOP;
}
// szImgOn : 활성탭 이미지
// szImgOff : 비활성탭 이미지(생략하면 szImgOn으로 같이 씀)
// szLayout: 탭안에 들어갈 레이아웃의 노드이름
void XWndTab2::AddTab( ID idTab, 
						LPCTSTR szLabel, 
						LPCTSTR szImgOn, LPCTSTR szImgOff, 
						float x, float y, float w, float h,
						const char *cLayoutKey )
{
	TAB *pTab = new TAB;
	pTab->idTab = idTab;
	pTab->strImgOn = szImgOn;
	pTab->strImgOff = szImgOff;
	if( XE::IsHave(cLayoutKey) )
		pTab->cstrLayoutKey = cLayoutKey;
	pTab->x = x;
	pTab->y = y;
	pTab->w = w;
	pTab->h = h;
	XE::VEC2 vSize = GetSizeLocal();
	// 탭이 붙으면서 크기가 더 커지는 경우는 전체 윈도우 영역을 더 늘여준다.
	if( pTab->x + pTab->w > vSize.w )
		vSize.w = pTab->x + pTab->w;
	if( pTab->y + pTab->h > vSize.h )
		vSize.h = pTab->y + pTab->h;
	SetSizeLocal( vSize );
	if( szLabel )
		pTab->strLabel = szLabel;
	pTab->psfcImgOn = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgOn ) );
	if( szImgOff )
		pTab->psfcImgOff = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, szImgOff ) );

	m_listTab.Add( pTab );
	// 아직 선택된 탭이 없다면 시작탭아이디가 들어왔을때 자동으로 선택시킨다.
	if( m_pSelectedTab == NULL )
		if( m_idStartTab && m_idStartTab == idTab )
			DoSelectTab( m_idStartTab );
//	if( m_pSelectedTab == NULL )
//		m_pSelectedTab = pTab;
}

void XWndTab2::OnLButtonDown( float lx, float ly )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( XE::IsArea2( pTab->x, pTab->y, pTab->w, pTab->h, lx, ly ) )
		{
			DoSelectTab( pTab->idTab );
			break;
		}
	} END_LOOP;
	XWnd::OnLButtonDown( lx, ly );
}

/**
 idTab탭을 선택한다. 탭을 터치한것과 같은 효과가 난다.
*/
int XWndTab2::DoSelectTab( ID idTab )
{
	TAB *pTab = GetTab( idTab );
	// 기존에 선택 되었던 탭의 레이아웃을 삭제시킨다.
	if( m_pLayout && m_pSelectedTab )
		DestroyID( m_pSelectedTab->idRootLayout );
	m_pSelectedTab = pTab;
	if( m_pLayout && pTab->cstrLayoutKey.empty() == false ) {
		XWnd *pRoot = new XWnd(0,0);
		// 레이아웃 객체에서 cstrLayoutKey를 찾아서 자식으로 붙인다.
		m_pLayout->CreateLayout( pTab->cstrLayoutKey.c_str(), pRoot );
		// 자식들의 최대 크기를 구해서 루트윈도우의 크기로 잡아준다.
// 		XE::VEC2 vSize = pRoot->GetMaxSizeByChild();
		auto sizeLocal = pRoot->GetSizeNoTransLayout();
		pRoot->SetSizeLocal( sizeLocal );
		// 탭뷰에 레이아웃을 붙인다.
		Add( pRoot );
		// 나중에 삭제할수 있도록 아이디를 받아둠.
		m_pSelectedTab->idRootLayout = pRoot->GetID();
	}
	OnClickedTab( pTab );	// 삭제될 예정
	if( m_pDelegate )
		m_pDelegate->OnDelegateClickedTab( this, pTab );
	return 1;
}

void XWndTab2::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 vPos = GetPosFinal();
	// 뒷배경 찍음
	if( m_psfcBg )
		m_psfcBg->Draw( vPos );
	// 탭 이미지 찍음
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab != m_pSelectedTab )		// 현재 선택된 탭은 나중에 찍기위해 건너뜀
		{
			if( pTab->psfcImgOff )
				pTab->psfcImgOff->Draw( vPos.x + pTab->x, vPos.y + pTab->y );
			else
				pTab->psfcImgOn->Draw( vPos.x + pTab->x, vPos.y + pTab->y );
			if( pTab->strLabel.empty() == false )
				DrawTabLabel( pTab );
		}
	} END_LOOP;
	// 선택된 탭을 덮어 그림
	if( m_pSelectedTab )
	{
		m_pSelectedTab->psfcImgOn->Draw( vPos.x + m_pSelectedTab->x, vPos.y + m_pSelectedTab->y );
		if( m_pSelectedTab->strLabel.empty() == false )
			DrawTabLabel( m_pSelectedTab );
	}
	XWnd::Draw();
}

void XWndTab2::DrawTabLabel( TAB *pTab )
{
	m_pFontObj->SetLineLength( pTab->w );
	float yCenter = pTab->y + (pTab->h / 2.f);
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += pTab->x;
	vPos.y += yCenter - 5.f;
	m_pFontObj->DrawString( vPos.x, vPos.y, pTab->strLabel.c_str() );
}

XWndTab2::TAB* XWndTab2::GetTab( ID idTab )
{
	XLIST_LOOP( m_listTab, TAB*, pTab )
	{
		if( pTab->idTab == idTab )
			return pTab;
	} END_LOOP;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 바디와 슬라이더를 하나의 XWnd객체로 추상화시켜서 버튼같은걸로도 넣을수 있도록 하면 좋을듯.
XWndSlideDown::XWndSlideDown( float x, float y,
							 LPCTSTR szImgBody,	// 기본 바디가 될 이미지
							 LPCTSTR szImgSlide )	// 슬라이더가 될 이미지
: XWnd( x, y )
{
	Init();
	// 슬라이더 이미지
	XWndImage *pImg = new XWndImage( TRUE, XE::MakePath( DIR_UI, szImgSlide ), 0, 0 );	// 뷰포트에 들어갈것이기 때문에 로컬좌표로 넣음.
	pImg->SetbShow( TRUE );	// 처음엔 일단 감춰둠.
	Add( pImg );
	m_pSlider = pImg;
	XE::VEC2 vSize = pImg->GetSizeLocal();
	pImg->SetPosLocal( 0.f, -vSize.h );
	// 바디이미지
	pImg = new XWndImage( TRUE, XE::MakePath( DIR_UI, szImgBody ), 0, 0 );
	Add( pImg );
	m_pBody = pImg;
	vSize = pImg->GetSizeLocal();
	SetSizeLocal( vSize );		// this윈도우의 초기 크기는 바디이미지의 크기
}

// 바디에 새겨질 텍스트.
void XWndSlideDown::SetTextBody( LPCTSTR szText, LPCTSTR szFont, float sizeFont, XCOLOR color, xFONT::xtStyle style )
{
	XE::VEC2 vSize = GetSizeLocal();
	XBREAK( vSize.IsZero() == TRUE );
	//
	auto pText = new XWndTextString( XE::VEC2(0), vSize, szText, szFont, sizeFont, color );
	Add( pText );
	pText->SetStyle( style );
	
	
}

int XWndSlideDown::Process( float dt )
{
	// 열리는 중.
	if( m_State == 1 )
	{
		XE::VEC2 vSize = m_pBody->GetSizeLocal();
		XE::VEC2 sizeSlider = m_pSlider->GetSizeLocal();
		float lerp = m_ipoSlider.GetInterpolationValueWithSet( m_secSlide );
		XE::VEC2 vPos;
		vPos.h = -sizeSlider.h + (vSize.h + sizeSlider.h + m_vAdjSlide.y) * lerp;
		m_pSlider->SetPosLocal( vPos );
		if( m_ipoSlider.IsFinish() )
		{
			m_ipoSlider.Off();
			m_State = 2;
			// 열리고 나면 슬라이더의 크기까지 합한 크기가 this의 전체 크기로 바뀐다.
			XE::VEC2 sizeOpen = vSize;
			sizeOpen.h += sizeSlider.h + m_vAdjSlide.y;
			SetSizeLocal( sizeOpen );
			//
			OnFinishSlideOpen();
		}
	}  else
		// 닫히는 중.
		if( m_State == 3 )
		{
			XE::VEC2 sizeBody = m_pBody->GetSizeLocal();
			XE::VEC2 sizeSlider = m_pSlider->GetSizeLocal();
			float lerp = m_ipoSlider.GetInterpolationValueWithSet( m_secSlide );
			XE::VEC2 vPos;
			vPos.h = -sizeSlider.h + (sizeBody.h + sizeSlider.h + m_vAdjSlide.y) * ( 1.0f - lerp);
			m_pSlider->SetPosLocal( vPos );
			if( m_ipoSlider.IsFinish() )
			{
				m_ipoSlider.Off();
				m_State = 0;
				XWnd *pSliderView = m_pSlider->Find( "xe.wnd.slidedown.slider" );
				if( XASSERT( pSliderView != NULL ) )
					XWnd::DestroyWnd( pSliderView );
				// 닫히면 다시 바디사이즈가 된다.
				SetSizeLocal( sizeBody );
				//
				OnFinishSlideClose();
			}
		}
	
	return XWnd::Process( dt );
}

void XWndSlideDown::Draw( void )
{
	XPROF_OBJ_AUTO();
	// this는 바디보다 더큰 슬라이더를 숨기기위해 항상 뷰포트를 사용한다.
	// 뷰포트 설정.
	XE::VEC2 v = XWnd::GetPosFinal();
	XE::VEC2 size = GetSizeFinal();
	size.h += m_pSlider->GetSizeLocal().h;
	XE::VEC2 vpLT = GRAPHICS->GetViewportLT();		// 스택에 백업.
	XE::VEC2 vpRB = GRAPHICS->GetViewportRB();
	XE::VEC2 vpSize = GRAPHICS->GetViewportSize();
	//	GRAPHICS->BackupViewport();
	// this가 뷰포트 내에 있을지도 모르기때문에 v를 스크린좌표로 변환한다.
	XE::VEC2 vScr = GRAPHICS->ViewportPosToScreenPos( v );
	// 부모쪽에서 먼저 뷰포트를 걸어놨다면 그영역을 벗어나선 안된다.
	m_vAdjustLT.Set(0);
	if( vScr.y > vpRB.y )			// 뷰포트 아래로 완전히 벗어난 경우
		return;
	if( vScr.y + size.h < vpLT.y )	// 뷰포트 위로 완전히 벗어난 경우
		return;
	if( vScr.y < vpLT.y )			// 부모뷰포트보다 위로 좀더 올라갔으면 잘라줌.
	{
		// this의 뷰포트가 부모뷰포트에의해 잘렸기때문에 잘린만큼을 자식들의 좌표에 보정해줘야 한다.
		m_vAdjustLT.y = vScr.y - vpLT.y;
		vScr.y = vpLT.y;
		size.h += m_vAdjustLT.y;
		
	}
	if( vScr.y + size.h > vpRB.y )		// 부모뷰포트보다 아래로 내려갔으면 사이즈를 줄여줌.
	{
		size.h -= (vScr.y + size.h) - (vpRB.y);
	}
	if( vScr.x < vpLT.x )
	{
		m_vAdjustLT.x = vScr.x - vpLT.x;
		vScr.x = vpLT.x;
		size.w += m_vAdjustLT.x;
		
	}
	//
	GRAPHICS->SetViewport( vScr, size );
	XE::SetProjection( size.w, size.h );
	// 자식들 그림(포인터 받아서 직접 뷰포트 따로 지정하면서 그려도 될듯).
	XWnd::Draw();
	// viewport 해제
	GRAPHICS->SetViewport( vpLT, vpSize );		// 백업받았던 뷰포트 복구.
	XE::SetProjection( vpSize.w, vpSize.h );
}

void XWndSlideDown::OnLButtonDown( float lx, float ly )
{
}

void XWndSlideDown::OnLButtonUp( float lx, float ly )
{
	if( m_State == 0 )	// 닫혀있는 상태
	{
		m_State = 1;		// 슬라이더가 열리기 시작. 1->2
		if( m_pLayout )
		{
			XE::VEC2 sizeBody = m_pBody->GetSizeLocal();
			XE::VEC2 sizeSlider = m_pSlider->GetSizeLocal();
			XE::VEC2 sizeWill = sizeBody;
			sizeWill.w = 0;
			sizeWill.h = sizeSlider.h + m_vSliderLayout.y;	// 슬라이더 크기만큼 더 커질예정.
			// 부모한테 "난 이제 사이즈가 바뀔것이오"라고 알려준다.
			GetpParent()->OnStartChangeSizeOfChild( this, sizeWill, m_secSlide );
			// 슬라이더뷰위에 슬라이더 레이아웃을 생성시킨다.
			XWnd *pWnd = new XWnd( m_vSliderLayout.x, m_vSliderLayout.y, sizeSlider.w, sizeSlider.h + m_vAdjSlide.y );
			m_pSlider->Add( pWnd );
			pWnd->SetstrIdentifier( "xe.wnd.slidedown.slider" );
			m_pLayout->CreateLayout( m_strSliderLayout.c_str(), pWnd );
		}
		OnStartSlideOpen();		// virtual
	}
	else if( m_State == 2 )		// 열려있는 상태
	{
		m_State = 3;		// 슬라이더가 닫히는 중. 3->0
		if( m_pLayout )
		{
			XE::VEC2 sizeBody = m_pBody->GetSizeLocal();
			XE::VEC2 sizeSlider = m_pSlider->GetSizeLocal();
			XE::VEC2 sizeWill = sizeBody;
			sizeWill.h = sizeSlider.h + m_vSliderLayout.y;	// 슬라이더 크기만큼 더 작아질예정.
			// 부모한테 "난 이제 사이즈가 바뀔것이오"라고 알려준다.
			GetpParent()->OnStartChangeSizeOfChild( this, -sizeWill, m_secSlide );
		}
		OnStartSlideClose();		// virtual
	}
}


////////////////////////////////////////////////////////////////
/**
 @brief 파티클 레이어
 @param cKeyParticle particle.xml의 이미터 노드 이름.
 @param vPos cKeyParticle이미터의 위치
 @param secLife 레이어의 생존시간
*/
XWndParticleLayer::XWndParticleLayer( const char *cKeyParticle
																		, const XE::VEC2& vPos
																		, float secLife
																		, XParticleDelegate *pDelegate )
{
	Init();
	m_pMng = XParticleMng::sCreate();
	XBREAK( m_pMng == nullptr );
	m_pMng->SetpDelegate( pDelegate );
	if( XE::IsHave(cKeyParticle) )
		m_pMng->CreateSfx( cKeyParticle, vPos );
	SetsecLife( secLife );
}

void XWndParticleLayer::Destroy() 
{
	SAFE_DELETE( m_pMng );
}
/**
 @brief emitter추가
*/
void XWndParticleLayer::AddEmitter( const char *cKeyParticle
																	, const XE::VEC2& vPos )
{
	XBREAK( XE::IsEmpty(cKeyParticle) )	;
	if( cKeyParticle ) {
		m_pMng->CreateSfx( cKeyParticle, vPos );
	}
}

int XWndParticleLayer::Process( float dt )
{
	if( m_timerLife.IsOver() )
		SetbDestroy( true );
	else
		m_pMng->FrameMove( dt );
	return XWnd::Process( dt );
}
void XWndParticleLayer::Draw()
{
	m_pMng->Draw();
	XWnd::Draw();
}

BOOL XWndParticleLayer::RestoreDevice()
{
	m_pMng->RestoreDevice();
	return TRUE;
}

void XWndParticleLayer::DestroyEmittersAndParticles()
{
	m_pMng->DestroyAll();
}

void XWndParticleLayer::SetpDelegate( XParticleDelegate *pDelegate )
{
	m_pMng->SetpDelegate( pDelegate );
}

////////////////////////////////////////////////////////////////
int XWndPlaySound::Process( float dt )
{
	if( m_idSnd ) {
		if( m_bOneSec )
			SOUNDMNG->OpenPlaySoundBySec( m_idSnd, 1.f );
		else
			SOUNDMNG->OpenPlaySound( m_idSnd );
	}
	SetbDestroy( true );
	return XWnd::Process( dt );
}

////////////////////////////////////////////////////////////////
int XWndPlayMusic::Process( float dt )
{
	if( !m_strBgm.empty() ) {
		SOUNDMNG->OpenPlayBGMusic( m_strBgm.c_str() );
// 		SOUNDMNG->SetBGMVolumeLocal( 0.5f );
	}
	SetbDestroy( true );
	return XWnd::Process( dt );
}


