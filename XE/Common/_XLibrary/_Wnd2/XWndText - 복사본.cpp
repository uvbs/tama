#include "stdafx.h"
#include "XWndText.h"
#include "client/XClientMain.h"
#include "client/XEContent.h"

//using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XWndTextString* XWndTextString::sUpdateCtrl( XWnd *pRoot
																					, const char *cIdentifier
																					, const XE::VEC2& vPos
																					, LPCTSTR szFont
																					, int sizeFont)
{
	auto pCtrl = SafeCast2<XWndTextString*>( pRoot->Find( cIdentifier ) );
	if( pCtrl == nullptr ) {
		pCtrl = new XWndTextString( vPos, _T(""), szFont, (float)sizeFont );
		pCtrl->SetstrIdentifier( cIdentifier );
		XBREAK( pCtrl == nullptr );
		pRoot->Add( pCtrl );
	}
	pCtrl->SetPosLocal( vPos );
	return pCtrl;
}
////////////////////////////////////////////////////////////////
XWndTextString::XWndTextString() 
{
	Init(); 
#ifdef _XNEW_WND_TEXT
	m_szString = new TCHAR[ 1024 ];
#endif
}
XWndTextString::XWndTextString( const XE::VEC2& vPos, 
								const XE::VEC2& vSize, 
								LPCTSTR szString, 
								LPCTSTR szFont, float sizeFont, 
								XCOLOR col ) 
	: XWnd( vPos, vSize ) 
{
		Init();
#ifndef _XNEW_WND_TEXT
		if( szString )
#endif
			SetText( szString );
		m_pFontDat = FONTMNG->Load( szFont, sizeFont );
		if( XBREAK( m_pFontDat == NULL ) )
			return;
		m_bRetain = TRUE;
		m_pFont = m_pFontDat->CreateFontObj();
		m_pFont->SetColor( col );
		m_Color = col;
		m_pFont->SetLineLength( vSize.w );
}
XWndTextString::XWndTextString( const XE::VEC2& vPos, LPCTSTR szString, 
						LPCTSTR szFont, float sizeFont, 
						XCOLOR col ) 
	: XWnd( vPos.x, vPos.y ) 
{
		Init();
#ifndef _XNEW_WND_TEXT
		if( szString )
#endif
			SetText( szString );
		m_pFontDat = FONTMNG->Load( szFont, sizeFont );
		if( XBREAK( m_pFontDat == NULL ) )
			return;
		m_bRetain = TRUE;
		m_pFont = m_pFontDat->CreateFontObj();
		m_pFont->SetColor( col );
		m_Color = col;
		m_pFont->SetLineLength( XE::GetGameWidth() );
}

XWndTextString::XWndTextString( const XE::VEC2& vPos, const _tstring& strString, 
						LPCTSTR szFont, float sizeFont, 
						XCOLOR col )  
	: XWndTextString( vPos, strString.c_str(), szFont, sizeFont, col ) 
{
}

XWndTextString::XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, const _tstring& strString,
				LPCTSTR szFont, float sizeFont,
				XCOLOR col )
	: XWndTextString( vPos, vSize, strString.c_str(), szFont, sizeFont, col ) 
{
}
/**
	@brief 부모의 크기에 맞게 자동으로 중앙정렬된다.
*/
XWndTextString::XWndTextString( LPCTSTR szString,
				LPCTSTR szFont, float sizeFont,
				XCOLOR col )
 	: XWndTextString( XE::VEC2(0), XE::VEC2(0), szString, szFont, sizeFont, col ) 
{
	// SetLineLength를 해주지 않고 만약 draw를 먼저한번하고 
	// 다음 턴에서 SetLineLength가 들어가면 오동작한다
	// FTGL의 버그인듯 하다.
	m_pFont->SetLineLength( XE::GetGameWidth() );
	SetbUpdate( true );
	m_bSizeFollowByParent = true;
}

//////////////////////////////////////////////////////////////////////////
void XWndTextString::Create( LPCTSTR szString,
	XBaseFontDat *pFontDat/* = nullptr*/,
	XCOLOR col/*=XCOLOR_WHITE*/ )
{
#ifndef _XNEW_WND_TEXT
	if( XE::IsHave( szString ) )
#endif // not new_wnd_text
		SetText( szString );
	if( pFontDat == nullptr )
		pFontDat = BASE_FONT;
	m_pFontDat = pFontDat;
	m_pFont = pFontDat->CreateFontObj();
	m_pFont->SetColor( col );
	m_Color = col;
	m_pFont->SetLineLength( XE::GetGameWidth() );
}

void XWndTextString::Update()
{
	// 사이즈가 지정되지 않았을때는 자동으로 부모크기에 맞춰 사이즈를 잡고 중앙정렬한다.
	if( m_Align & XE::xALIGN_VCENTER )
		if( GetSizeLocal().h <= 1 )
			m_bSizeFollowByParent = true;
	if( m_Align & XE::xALIGN_HCENTER )
		if( GetSizeLocal().w <= 1 )
			m_bSizeFollowByParent = true;
	if( m_bSizeFollowByParent ) {
		UpdateSize();
// 		if( m_pFont && GetpParent() ) {
// 			auto sizeParent = GetpParent()->GetSizeFinal();
// 			SetSizeLocal( sizeParent );		// 윈도우의 사이즈는 부모의 크기로 한다.
// 			XBREAK( sizeParent.w == 0 );
// 			if( XE::IsEmpty( m_szString ) ) {
// 				m_sizeString.Set( 0 );		// 스트링이 없으면 사이즈를 0으로 하다.
// 				return;
// 			}
// 			m_sizeString = m_pFont->GetLayoutSize( m_szString );
// 	#ifdef WIN32
// 			m_pFont->SetArea( sizeParent );
// 	#endif // WIN32
// 			SetAlign( XE::xALIGN_CENTER );
// 			m_pFont->SetLineLength( sizeParent.w );
// 			m_bSizeFollowByParent = false;
// // 			CONSOLE("XWndTextString::Update(): %d/%d", (int)sizeParent.w, (int)sizeParent.h);
// 		} else {
// 			CONSOLE("m_bSizeFollowByParent가 지정되었는데 Update()실패함");
// 		}
	}
	XWnd::Update();
}
/**
 @brief 현재 텍스트를 기반으로 컨트롤의 크기를 다시 계산한다.
*/
void XWndTextString::UpdateSize()
{
	auto pParentValid = GetpParentValid();
	if( m_pFont && pParentValid ) {
		auto sizeParent = pParentValid->GetSizeFinal();
		SetSizeLocal( sizeParent );		// 윈도우의 사이즈는 부모의 크기로 한다.
		XBREAK( sizeParent.w == 0 );
		if( XE::IsEmpty( m_szString ) ) {
			m_sizeString.Set( 0 );		// 스트링이 없으면 사이즈를 0으로 하다.
			return;
		}
		m_sizeString = m_pFont->GetLayoutSize( m_szString );
#ifdef _VER_DX
		m_pFont->SetArea( sizeParent );
#endif // _VER_DX
//		SetAlign( XE::xALIGN_CENTER );
		SetAlign( m_Align );
		m_pFont->SetLineLength( sizeParent.w );
		m_bSizeFollowByParent = false;
// 			CONSOLE("XWndTextString::Update(): %d/%d", (int)sizeParent.w, (int)sizeParent.h);
	} else {
		CONSOLE("m_bSizeFollowByParent가 지정되었는데 Update()실패함");
	}
}

/**
 @brief 현재 문자열의 레이아웃 크기를 구한다.
 @note: FTGL에선 이함수를 호출할때마다 BBox를 새로 계산하므로 매우 느림. 필요할때만 호출할것.
*/
void XWndTextString::PreCalcLayoutSize( void ) 
{
	if( XE::IsEmpty( m_szString ) ) {
		m_sizeString.Set( 0 );
		return;
	}
	XBREAK( m_pFont == NULL );
#ifdef _XNEW_WND_TEXT
	XBREAK( m_szString == NULL );
#endif
	m_sizeString = m_pFont->GetLayoutSize( m_szString );
	// 사이즈가 지정되어 있지 않을때만 텍스트레이아웃을 사이즈로 사용하게 바뀜.
	if( !GetSizeLocal().IsValid() )
// 	const auto& sizeLocal = GetSizeLocal();
// 	if( sizeLocal.x <= 1 || sizeLocal.y <= 1 )
		SetSizeLocal( m_sizeString );
}

void XWndTextString::SetText( LPCTSTR szString )
{
	BOOL recalcLayout = FALSE;
	// 레이아웃크기가 구해져있었고 스트링이 바뀌었다면 레이아웃을 새로 계산해줘야 한다.
	if( m_sizeString.IsZero() == FALSE &&
		XE::IsSame( m_szString, szString ) == FALSE )
		recalcLayout = TRUE;
	int size = 1024;	// 기본 1024크기로 버퍼를 잡고
	int len = 0;
	if( szString )
		len = _tcslen( szString );
	if( len >= 1024 )	// 큰사이즈 스트링의 경우 큰 버퍼를 잡음.
		size = len + 1;
	if( m_szString == NULL )
	{
		m_szString = new TCHAR[ size ];
		m_szString[ 0 ] = 0;
		m_maxSize = size;
	}
	if( XBREAK( len >= m_maxSize ) )
		return;
	if( szString )
	{
		_tcscpy_s( m_szString, size, szString );
	}
	if( m_pFont && m_Align == XE::xALIGN_VCENTER )
		SetAlign( XE::xALIGN_VCENTER );
	if( recalcLayout )
		PreCalcLayoutSize();
}

/**
 @brief 폰트를 바꾼다.
*/
void XWndTextString::SetFont( LPCTSTR szFont, float size )
{
	auto len = m_pFont->GetLineLength();
	auto align = m_Align;
	auto scale = m_pFont->GetvScale();
	auto style = m_pFont->GetStyle();
	auto alpha = m_pFont->GetAlpha();
	DestroyFont();
	m_bRetain = TRUE;
	m_pFontDat = FONTMNG->Load( szFont, size );
	XBREAK( m_pFontDat == nullptr );
	m_pFont = m_pFontDat->CreateFontObj();
	SetLineLength( len );
	SetAlign( align );
	m_pFont->SetScale( scale );
	SetColorText( m_Color );
	SetStyle( style );
	m_pFont->SetAlpha( alpha );
}

void XWndTextString::ChangeText( LPCTSTR szString )
{
	float lineLen = 0;
	if( m_pFont )
		lineLen = m_pFont->GetLineLength();
	SAFE_DELETE( m_pFont );
	m_pFont = m_pFontDat->CreateFontObj();
	m_pFont->SetColor( m_Color );
	if( lineLen == 0 )
		m_pFont->SetLineLength( XE::GetGameWidth() );
	else
		m_pFont->SetLineLength( lineLen );
	BOOL recalcLayout = FALSE;
	// 레이아웃크기가 구해져있었고 스트링이 바뀌었다면 레이아웃을 새로 계산해줘야 한다.
	if( m_sizeString.IsZero() == FALSE &&
		XE::IsSame( m_szString, szString ) == FALSE )
		recalcLayout = TRUE;
#ifdef _XNEW_WND_TEXT
	// 기존에 작은 스트링이었는데 큰 스트링으로 바꿀 수 없다. FTGL의 버그때문
	XBREAK( m_szString == NULL );
	if( XBREAK( _tcslen( m_szString ) < 1024 && _tcslen( szString ) >= 1024 ) )
		return;
	_tcscpy_s( m_szString, m_maxSize, szString );
#else
	_tcscpy_s( m_szString, szString );
#endif
	if( recalcLayout )
		PreCalcLayoutSize();
}

// 텍스트 영역의 폭과 텍스트레이아웃의 세로크기만큼의 전체 크기를 계산해서 돌려준다.
XE::VEC2 XWndTextString::GetSizeForceFinal( void )
{
	XE::VEC2 vSize;
	vSize.w = m_pFont->GetLineLength();
	if( m_sizeString.IsZero() )
		PreCalcLayoutSize();
	XE::VEC2 sizeWin = GetSizeFinal();
	if( m_sizeString.h < sizeWin.h )
		vSize.h = sizeWin.h;
	else
		vSize.h = m_sizeString.h;
	return vSize;
}
void XWndTextString::Draw( void )
{
	//	return;
	if( XE::IsHave( m_szString ) )
	{
		Draw( GetPosFinal() );
	}
}
/**
 @brief 복잡하니까 이제 DX는 지원안하는걸로.
*/
void XWndTextString::Draw( const XE::VEC2& v )
{
	//	return;
	XPROF_OBJ_AUTO();
	if( XE::IsHave( m_szString ) ) {
		if( m_pFont ) {
			const XE::VEC2 vPos = v;
			if( GetbEnable() )
				m_pFont->SetColor( m_Color );
			else
				m_pFont->SetColor( m_colDisable );
			const float alpha = GetAlphaFinal();
			m_pFont->SetAlpha( alpha );
			const auto vScale = GetScaleFinal();
			m_pFont->SetScale( vScale );
			if( m_colBackground ) {
				// 제대로 작동안함.
			}
			// 컨트롤 윈도우영역의 크기를 얻는다. 사이즈가 없을수도 있다.
			const auto sizeCtrl = GetSizeFinal();
			auto size = GetSizeFinal();
			// FTGL의 속도때문에 중앙정렬등의 계산을 직접하게 바꼈으므로 lineLength는 그냥 화면값을 넣고 left정렬 한다.
			// LineLength는 layout시점에서 0일수가 있으므로 0일경우 실시간으로 수정하도록 바꿈.
			if( m_pFont->GetLineLength() == 0 ) {
				if( m_bAlignParent )
					m_pFont->SetLineLength( GetpParent()->GetSizeLocal().w );
				else
					m_pFont->SetLineLength( size.w );
			}
			/*
			중앙정렬의 경우 FTGL의 center기능을 사용하지 않고 FTGL에는 LEFT정렬로 맞추고
			좌상귀 좌표는 직접 계산해준다.
			*/
			// y중앙정렬이 있다면 y좌표를 보정해준다.
			if( GetSizeLocal().w != m_pFont->GetLineLength() )
				size.w = m_pFont->GetLineLength();
			if( m_bAlignParent )
				size = GetpParent()->GetSizeLocal();	// 부모정렬의 경우는 실시간으로 부모크기를 얻어서 사용한다.
			if( m_pFont ) {
				XE::VEC2 vAdj;
				if( m_Align & XE::xALIGN_VCENTER ) {
					if( m_sizeString.IsValid() == false )
						PreCalcLayoutSize();
					if( m_bAlignParent == false && size.h <= 0 ) {
						UpdateSize();
						size = GetSizeFinal();
					}
#ifdef _VER_OPENGL
					vAdj.y = ( size.h / 2.f ) - ( m_sizeString.h / 2.f );	// 윈도우의 중앙에서 다시 스트링크기의 절반을 빼준다.
#endif // _VER_OPENGL
				} 
				if( m_Align & XE::xALIGN_HCENTER ) {
					if( m_sizeString.IsValid() == false )
						PreCalcLayoutSize();
//					XBREAK( m_bAlignParent == false &&size.w <= 0 );			// y보정시에는 반드시 세로크기가 있어야 한다.
					if( m_bAlignParent == false &&size.w <= 0 ) {
						UpdateSize();
						size = GetSizeFinal();
					}
#ifdef _VER_OPENGL
					vAdj.x = ( size.w / 2.f ) - ( m_sizeString.w / 2.f );	// 윈도우의 중앙에서 다시 스트링크기의 절반을 빼준다.
#endif // _VER_OPENGL
				}
				// win32는 Area를 이용하므로 y를 따로 보정하지 않는다.
#ifdef _VER_DX
				m_pFont->SetArea( size );
#endif // win32
				m_pFont->DrawString( vPos + vAdj, m_szString );
			}
		}
	}
}

void XWndTextString::SetAlign( XE::xAlign align ) 
{
	// 레이아웃 크기가 구해져있었고 정렬방식이 바뀌면 레이아웃 사이즈 다시 구함.
	bool bUpdate = false;
	if( m_sizeString.IsZero() == FALSE && m_Align != align )
		bUpdate = true;	
	m_Align = align;		// this에는 받은 그대로 저장한다.
#ifdef _VER_DX
	m_pFont->SetAlign( align );
#else
	if( align & XE::xALIGN_HCENTER )	// FTGL XWndText에서는 속도문제로 중앙정렬을 직접 계산한다.
	{
		// FTGL에 넘길때는 LEFT정렬로 넘긴다.
		align = XE::xALIGN_LEFT;
		if( m_sizeString.IsZero() )
			bUpdate = true;
	}
	m_pFont->SetAlign( align );
#endif
	if( align & XE::xALIGN_VCENTER )
	{
		if( m_sizeString.IsZero() )
			bUpdate = true;
//			m_vAdjust.y = -(m_sizeString.h / 2.f);		// 과거엔 중앙정렬의 의미가 y라인의 중앙에 텍스트가 걸치는것이었으나 이제 영역상의 중앙개념으로 바뀌어서 삭제.
	}
	if( bUpdate )
		PreCalcLayoutSize();
}


/*
	case 1: left정렬. y정렬 없음. win32나 FTGL이나 같음.
	case 2: left정렬, y중앙정렬. 
		win32: 텍스트레이아웃의 사이즈를 알수 없으므로 SetArea를 사용해야 함. h사이즈 있어야 함.
		FTGL: 텍스트레이아웃의 사이즈를 구해서 수동으로 맞춰주면 됨. align은 FTGL::LEFT로 맞춤
	case 3: 중앙정렬, y정렬 없음. 
		win32: SetArea사용. w사이즈 있어야 함.
		FTGL: FTGL::ALIGN_CENTER로 맞춤. 
	case 4: x중앙정렬, y중앙정렬
		win32: SetArea사용. w,h사이즈 필요
		FTGL: FTGL::ALIGN_CENTER로 맞추고 y좌표는 사이즈 계산해서 수동으로 맞춤.
*/

//////////////////////////////////////////////////////////////////////////
// void XWndTextStringArea::Create( float w, float h, LPCTSTR szString,
// 								XBaseFontDat *pFontDat/* = nullptr*/,
// 								XCOLOR col/*=XCOLOR_WHITE*/ )
// {
// 	if( pFontDat == nullptr )
// 		pFontDat = BASE_FONT;
// 	m_pFontDat = pFontDat;
// 	m_Color = col;
// 	if( szString )
// 	{
// 		m_pFont = pFontDat->CreateFontObj();
// 		SetColorText( col );
// 		m_pFont->SetLineLength( w );
// #ifdef WIN32
// 		m_pFont->SetArea( XE::VEC2( w, h ) );
// 		m_pFont->SetAlign( XE::xALIGN_CENTER );
// #endif
// 		SetText( szString );
// 	}
// }
// void XWndTextStringArea::Create( LPCTSTR szString,
// 								XBaseFontDat *pFontDat/* = nullptr*/,
// 								XCOLOR col/*=XCOLOR_WHITE*/ )
// {
// 	if( pFontDat == nullptr )
// 		pFontDat = BASE_FONT;
// 	m_pFontDat = pFontDat;
// 	m_Color = col;
// 	m_pFont = pFontDat->CreateFontObj();
// 	SetColorText( col );
// 	if( szString )
// 		SetText( szString );
// }
// void XWndTextStringArea::Create( XBaseFontDat *pFontDat/* = nullptr*/ )
// {
// 	if( pFontDat == nullptr )
// 		pFontDat = BASE_FONT;
// 	m_pFontDat = pFontDat;
// 	m_pFont = pFontDat->CreateFontObj();
// #ifdef WIN32
// 	m_pFont->SetAlign( XE::xALIGN_CENTER );
// #endif
// }
// 
// void XWndTextStringArea::Update()
// {
// //	if( m_Align == XE::xALIGN_CENTER )	// 현재버전은 언제나 center인것을 전제로함.
// 	{
// 		// 사이즈가 지정되지 않았을때는 자동으로 부모크기에 맞춰 사이즈를 잡고 중앙정렬한다.
// 		if( m_pFont && GetpParent() && GetSizeLocal().w <= 1 )
// 		{
// 			auto sizeParent = GetpParent()->GetSizeLocal();
// 			SetSizeLocal( sizeParent );
// 			XBREAK( sizeParent.w == 0 );
// 			UpdateAlign();
// 		}
// 	}
// 	XWnd::Update();
// }
// void XWndTextStringArea::Draw( void )
// {
// 	XPROF_OBJ_AUTO();
// 	if( IsHave( m_szString ) )
// 	{
// 		XE::VEC2 vPos = GetPosFinal();
// 		XE::VEC2 vSize = GetSizeFinal();
// 		if( m_pFont )
// 		{
// #ifdef WIN32
// 			m_pFont->DrawString( vPos, m_szString );
// #else
// 			XE::VEC2 v = ( vSize / 2.f ) - ( m_sizeString / 2.f );		// 텍스트가 찍힐 좌상귀
// 			m_pFont->DrawString( vPos + v, m_szString );
// #endif
// 		}
// 	}
// }
// 
// void XWndTextStringArea::Draw( const XE::VEC2& vPos )
// {
// 	XPROF_OBJ_AUTO();
// 	if( IsHave( m_szString ) )
// 	{
// 		XE::VEC2 vSize = GetSizeFinal();
// 		if( m_pFont )
// 		{
// #ifdef WIN32
// 			m_pFont->DrawString( vPos, m_szString );
// #else
// 			XE::VEC2 v = ( vSize / 2.f ) - ( m_sizeString / 2.f );		// 텍스트가 찍힐 좌상귀
// 			m_pFont->DrawString( vPos + v, m_szString );
// #endif
// 		}
// 	}
// }
// 

