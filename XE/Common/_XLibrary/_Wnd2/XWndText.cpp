#include "stdafx.h"
#include "XWndText.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/Game/XEComponentMng.h"
#include "XFramework/Game/XEComponents.h"
#include "XFramework/XEProfile.h"

//using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

//////////////////////////////////////////////////////////////////////////
XWndTextString* XWndTextString::sUpdateCtrl( XWnd *pRoot
																					, const char *cIdentifier
																					, const XE::VEC2& vPos
																					, LPCTSTR szFont
																					, int sizeFont
																					, bool bShow )
{
	auto pCtrl = SafeCast2<XWndTextString*>( pRoot->Find( cIdentifier ) );
	if( pCtrl == nullptr ) {
		if( bShow ) {
			pCtrl = new XWndTextString( vPos, _T( "" ), szFont, (float)sizeFont );
			pCtrl->SetstrIdentifier( cIdentifier );
			XBREAK( pCtrl == nullptr );
			pRoot->Add( pCtrl );
		}
	}
	if( pCtrl ) {
		pCtrl->SetbShow( bShow );
		pCtrl->SetPosLocal( vPos );
	}
	return pCtrl;
}

/**
 @brief 부모(pParent)의 중앙으로 정렬하는 버전
*/
XWndTextString* XWndTextString::sUpdateCtrl( XWnd* pParent
																					, const std::string& ids
																					, LPCTSTR szFont
																					, int sizeFont
																					, bool bShow )
{
	auto pCtrl = sUpdateCtrl( pParent, ids.c_str(), XE::VEC2(0), szFont, sizeFont, bShow );
	if( pCtrl ) {
		pCtrl->SetSizeLocal( pParent->GetSizeLocal() );
		pCtrl->SetAlignCenter();
	}
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

XWndTextString::XWndTextString( const XE::VEC2& vPos
															, const XE::VEC2& vSize
															, LPCTSTR szString
															, XBaseFontDat *pFontDat
															, XCOLOR col )
	: XWnd( vPos, vSize ) 
{
	Init();
	Create( szString, pFontDat, col );
	SetLineLength( vSize.w );
}

XWndTextString::XWndTextString( const XE::VEC2& vPos, 
																const XE::VEC2& vSize, 
																LPCTSTR szString, 
																LPCTSTR szFont, float sizeFont, 
																XCOLOR col ) 
	: XWnd( vPos, vSize ) 
{
	Init();
	Create( vSize, szString, szFont, sizeFont, col );
}
XWndTextString::XWndTextString( const XE::VEC2& vPos, LPCTSTR szString, 
																LPCTSTR szFont, float sizeFont, 
																XCOLOR col ) 
	: XWnd( vPos.x, vPos.y ) 
{
	Init();
	Create( XE::VEC2(0), szString, szFont, sizeFont, col );
}

/**
	@brief 부모의 크기에 맞게 자동으로 중앙정렬된다.
*/
XWndTextString::XWndTextString( LPCTSTR szString,
																LPCTSTR szFont, float sizeFont,
																XCOLOR col )
 	: XWndTextString( XE::VEC2(0), XE::VEC2(0), szString, szFont, sizeFont, col ) 
{
	SetAlign( xALIGN_CENTER );
	// lineLength는 draw나 layout계산전에 부모로부터 얻어서 세팅한다.
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
void XWndTextString::Create( const XE::VEC2& vSize
														, LPCTSTR _szString
														, LPCTSTR _szFont
														, float _sizeFont
														, XCOLOR col )
{
	const _tstring strString = (_szString)? _szString : _T("");	// 밖에서 FORMAT(...)으로 직접 건넬수도 있어서 받자마자 즉시 카피해놓고 진행함.
	LPCTSTR szFont = (XE::IsHave(_szFont))? _szFont : FONT_SYSTEM;
	const float sizeFont = (_sizeFont > 0)? _sizeFont : FONT_SIZE_DEFAULT;
	m_pFontDat = FONTMNG->Load( szFont, sizeFont );
	if( XBREAK( m_pFontDat == nullptr ) )
		return;
	m_bRetain = TRUE;
	m_pFont = m_pFontDat->CreateFontObj();
	m_pFont->SetColor( col );
	XWnd::SetAlign( xALIGN_LEFT );
	m_Color = col;
	SetLineLength( vSize.w );
	SetText( strString );
}

/**
 @brief 
*/
void XWndTextString::Create( LPCTSTR szString,
															XBaseFontDat *pFontDat/* = nullptr*/,
															XCOLOR col/*=XCOLOR_WHITE*/ )
{
	const auto strString = ( szString ) ? szString : _T( "" );	// 밖에서 FORMAT(...)으로 직접 건넬수도 있어서 받자마자 즉시 카피해놓고 진행함.
	XWnd::SetAlign( xALIGN_LEFT );
#ifndef _XNEW_WND_TEXT
	if( XE::IsHave( szString ) )
#endif // not new_wnd_text
		SetText( strString );
	if( pFontDat == nullptr )
		pFontDat = BASE_FONT;
	m_pFontDat = pFontDat;
	m_pFont = pFontDat->CreateFontObj();
	m_pFont->SetColor( col );
	m_Color = col;
	SetLineLength( 0 );
// 	m_pFont->SetLineLength( XE::GetGameWidth() );
}

void XWndTextString::Update()
{
	const auto align = GetAlign();
	// 일반적인 상황에선 GetLayoutSize를 호출하면 크기 업데이트가 되지만 
	// 객체 생성후 SetAlign, SetLineLength등을 하고 GetLayoutsize를 호출하는일 없이 바로 Draw()에 들어갈수도 있으므로
	// 최종적으로 Update()에서 필요하다면 계산을 하도록 한다.
	// linelength는 정렬과 관계없다. 왼쪽정렬이라도 문단이 내려갈순 있음.
	if( XE::IsHave( m_szString ) ) {
		if( m_lineLength == 0 ) {
			if( m_vSize.w > 0 ) {
				// 문단크기는 없는데 윈도사이즈가 있으면 윈도사이즈를 문단크기로 함.
				const auto sizeFont = (m_pFontDat)? m_pFontDat->GetFontSize() : 20.f;
				SetLineLength( m_vSize.w + sizeFont );
			}
		}
		if( align != xALIGN_LEFT ) {
			const auto sizeLocalNoTrans = GetSizeLocalNoTrans();
			if( sizeLocalNoTrans.w <= 0 ) {
				if( m_lineLength > 0 ) {
					SetLineLength( m_lineLength );
					// lineLength는 있는데 윈도우 크기가 없는경우 윈도우크기를 문단크기로 맞춤.
//					맘대로 부모크기로 바꿔버리면 안됨. 텍스트컨트롤자체도 m_vPos이 있기때문에 부모크기로 맞춰버리면 부모크기를 벗어남. m_vPos가 0이면 해도 될지도?
				} else {
					// 가로폭이 필요한 정렬
					if( align & (xALIGN_HCENTER | xALIGN_RIGHT) ) {
						auto pParentValid = GetpParentValidWidth();
						if( pParentValid ) {
							const auto sizeParent = pParentValid->GetSizeValidNoTrans();		// NoTrans로 하는게 맞는거 같은뎅. linelength는 스케일 되기전 크기로 해야하는뎅(영웅던전2 UI의 난이도버튼 가운데 숫자 출력땜에 바꿈
//							const auto sizeParent = pParentValid->GetSizeLocal();
							// 부모의 크기로 맞춰진다.
							SetLineLength( sizeParent.w );
						}
					}
					// 세로폭이 필요한 정렬
					if( align & (xALIGN_VCENTER | xALIGN_BOTTOM) ) {
						auto pParentValid = GetpParentValidHeight();
						if( pParentValid ) {
//							const auto sizeParent = pParentValid->GetSizeValidNoTrans();
							const auto sizeParent = pParentValid->GetSizeLocal();
							// 부모의 크기로 맞춰진다.
						}
					}
// 					auto pParentValid = GetpParentValid();
// 					if( pParentValid ) {
// 						const auto sizeParent = pParentValid->GetSizeValidNoTrans();
// 						// 부모의 크기로 맞춰진다.
// 						SetLineLength( sizeParent.w );
// 					}
				}
			}
		}
		if( m_bUpdateSizeAndPos 
			|| (align != xALIGN_LEFT && (m_sizeString.IsZero() || m_posString.IsZero())) ) {
				ForceCalcLayoutSize();
				ForceCalcLayoutPos();
				m_bUpdateSizeAndPos = false;
		}
	}
}

/**
 @brief 정렬속성이나 사이즈캐싱에 관계없이 다시 강제로 재계산한다.
*/
void XWndTextString::ForceCalcLayoutSize()
{
	const auto align = GetAlign();
	XBREAK( !align );
	if( XE::IsEmpty( m_szString ) ) {		// 텍스트가 없으면 크기는 0임
		m_sizeString.Set( 0 );
		return;
	}
	XBREAK( m_pFont == nullptr );
#ifdef _XNEW_WND_TEXT
	XBREAK( m_szString == nullptr );
#endif
	const auto sizePrev = m_sizeString.ToInt();
	m_sizeString = m_pFont->GetLayoutSize( m_szString );
	if( sizePrev.w != m_sizeString.ToInt().w ) {
		// 사이즈가 변했으면 부모중에 가로정렬이 있는 부모를 찾아서 업데이트를 요구한다.
		auto pValidParent = GetpParentHaveAlignH();
		if( pValidParent )
			pValidParent->SetbUpdate( true );
	}
	if( sizePrev.h != m_sizeString.ToInt().h ) {
		// 사이즈가 변했으면 부모중에 가로정렬이 있는 부모를 찾아서 업데이트를 요구한다.
		auto pValidParent = GetpParentHaveAlignV();
		if( pValidParent )
			pValidParent->SetbUpdate( true );
	}

}
/**
 @brief 현재 렌더된 텍스트레이아웃의 좌상귀 로컬좌표를 강제로 계산한다.
*/
void XWndTextString::ForceCalcLayoutPos()
{
	const auto align = GetAlign();
	XBREAK( !align );
	if( XE::IsEmpty( m_szString ) ) {		// 텍스트가 없으면 위치는 0임
		m_posString.Set( 0 );
		return;
	}
	XBREAK( m_pFont == nullptr );
#ifdef _XNEW_WND_TEXT
	XBREAK( m_szString == nullptr );
#endif
	m_posString = m_pFont->GetLayoutPos( m_szString );
}
/**
 @brief 크기 재계산을 해야하는 상황인가.
*/
bool XWndTextString::IsUpdateLayoutSize()
{
	const auto align = GetAlign();
	XBREAK( !align );
	// 업데이트 요청을 받았거나, 왼쪽정렬이 아닌데도 크기가 없는경우는 재계산함.
//	return m_bUpdateSizeAndPos || (m_Align != xALIGN_LEFT && m_sizeString.IsZero() );
	return m_bUpdateSizeAndPos || m_sizeString.IsZero();
}

/**
 @brief 위치 재계산을 해야하는 상황인가.
*/
bool XWndTextString::IsUpdateLayoutPos()
{
	const auto align = GetAlign();
	XBREAK( !align );
	// 업데이트 요청을 받았거나, 왼쪽정렬이 아닌데도 좌표가 없는경우는 재계산함.
	return m_bUpdateSizeAndPos || ( align != xALIGN_LEFT && m_posString.IsZero() );
}
/**
 @brief 렌더된 텍스트 레이아웃의 크기를 돌려준다.
*/
XE::VEC2 XWndTextString::GetLayoutSizeNoTrans()
{
	// 업데이트 요청을 받았거나, 왼쪽정렬이 아닌데도 크기가 없는경우는 재계산함.
	if( IsUpdateLayoutSize() ) {
		ForceCalcLayoutSize();
		m_bUpdateSizeAndPos = false;
	}
	return m_sizeString;
}

/**
 @brief 레이아웃 좌상귀 좌표를 얻는다. 이것은 트랜스폼의 영향을 받지 않는다.
*/
XE::VEC2 XWndTextString::GetLayoutPosNoTrans()
{
	if( IsUpdateLayoutPos() ) {
		ForceCalcLayoutPos();
		m_bUpdateSizeAndPos = false;
	}
	return m_posString;			// 보통은 이미 구해놓은 값을 쓴다(최적화 때문)
}

/**
 @brief 컨트롤의 텍스트를 바꾼다.
 이전과 같은 텍스트가 온다면 다시 카피나 계산을 하지 않는다.
*/
void XWndTextString::SetText( LPCTSTR szString )
{
	const auto align = GetAlign();
	XBREAK( szString == nullptr );
	XBREAK( !align );
#ifdef _XNEW_WND_TEXT
	int size = 1024;	// 기본 1024크기로 버퍼를 잡고
	int len = 0;
	if( szString )
		len = _tcslen( szString );
	if( len >= 1024 )	// 큰사이즈 스트링의 경우 큰 버퍼를 잡음.
		size = len + 1;
	if( m_szString == nullptr )	{
		m_szString = new TCHAR[ size ];
		m_szString[ 0 ] = 0;
		m_maxSize = size;
	}
	if( XBREAK( len >= m_maxSize ) )
		return;
#endif // _XNEW_WND_TEXT
	// 텍스트가 달라졌을때
	if( !XE::IsSame( m_szString, szString ) ) {
		_tcscpy_s( m_szString, size, szString );
		// 정렬이 왼쪽상태가 아닌이상 크기/위치 계산 다시함.
		if( align != xALIGN_LEFT ) {
			m_bUpdateSizeAndPos = true;
			m_sizeString.Set(0);
			m_posString.Set(0);
		} 
		if( m_bUpdateSizeIfChangeText ) {
			ForceCalcLayoutSize();
			// 플래그는 지우지 않았음. 값이 계속 바뀔수 있으므로.
		}

	}
}

void XWndTextString::SetNumberText( int num )
{
	if( IsComma() ) {
		const _tstring str = XE::NtS( num );
		SetText( str );
	} else {
		const _tstring str = XFORMAT("%d", num);
		SetText( str );
	}
}

/**
 @brief 폰트를 바꾼다.
*/
void XWndTextString::SetFont( LPCTSTR szFont, float size )
{
	const auto align = GetAlign();
	XBREAK( !align );
	const auto len = m_pFont->GetLineLength();
	const auto style = m_pFont->GetStyle();
	const auto scale = m_pFont->GetvScale();
	const auto alpha = m_pFont->GetAlpha();
	const bool bDifferent = ( m_pFontDat->GetFilename() != _tstring(szFont) );
	if( bDifferent ) {
		m_bUpdateSizeAndPos = true;
		m_sizeString.Set( 0 );
		m_posString.Set( 0 );
	}
	if( m_bRetain )
		SAFE_RELEASE2( FONTMNG, m_pFontDat );
	m_bRetain = TRUE;
	m_pFontDat = FONTMNG->Load( szFont, size );
	XBREAK( m_pFontDat == nullptr );
	if( bDifferent ) {
		SAFE_DELETE( m_pFont );
		m_lineLength = 0;
		XWnd::SetAlign( XE::xALIGN_NONE );
		m_Style = xFONT::xSTYLE_NORMAL;
		m_pFont = m_pFontDat->CreateFontObj();
		SetLineLength( len );
		SetAlign( align );
		SetStyle( style );
		m_pFont->SetScale( scale );
		m_pFont->SetAlpha( alpha );
	}
}

int XWndTextString::Process( float dt )
{
	m_compMngByAlpha.Process( dt );
	auto pCompAlpha = m_compMngByAlpha.FindComponentByFunc( "alpha" );
	if( pCompAlpha ) {
		SetAlphaLocal( pCompAlpha->GetValue() );
	}
// 	// 윈도우 사이즈가 없을경우 텍스트레이아웃의 사이즈가 있으면 그걸로 채운다.
// 	auto vSize = GetSizeLocalNoTrans();
// 	if( vSize.w <= 0 ) {
// 		if( m_sizeString.w > 0 )
// 			vSize.w = m_sizeString.w;
// 	}
// 	if( vSize.h <= 0 ) {
// 		if( m_sizeString.h > 0 )
// 			vSize.h = m_sizeString.h;
// 	}
	return XWnd::Process( dt );
}


void XWndTextString::Draw()
{
	//	return;
	if( XE::IsHave( m_szString ) ) {
		Draw( GetPosFinal() );
	}
}
/**
 @brief 복잡하니까 이제 DX는 지원안하는걸로.
*/
void XWndTextString::Draw( const XE::VEC2& vFinal )
{
	const auto align = GetAlign();
	XBREAK( !align );
	XPROF_OBJ_AUTO();
	if( XE::IsEmpty(m_szString) )
		return;
	if( m_pFont == nullptr )
		return;
	XE::VEC2 vAdj;		// 정렬에 따른 보정치
	// 수직 정렬
	if( align != xALIGN_LEFT ) {
		if( XBREAKF( m_sizeString.IsMinus(), "size minus, %s", m_szString ) )
			m_sizeString.Set( 10 );
		// 윈도우의 크기를 얻는다. 만약 윈도우 크기가 지정되어 있지 않다면 부모의 크기로 정렬한다.
		auto sizeLocal = GetSizeLocalNoTrans();
		if( sizeLocal.w == 0 && m_lineLength > 0 )
			sizeLocal.w = m_lineLength;
		if( sizeLocal.w <= 0 || sizeLocal.h <= 0 ) {
			const auto sizeParent = GetpParentValid()->GetSizeValidNoTrans();		// 부모의 크기로 사용한다.
			if( sizeLocal.w <= 0 )
				sizeLocal.w = sizeParent.w;
			if( sizeLocal.h <= 0 )
				sizeLocal.h = sizeParent.h;
		}
// 		if( sizeLocal.IsZero() )
// 			sizeLocal = GetSizeValidNoTrans();		// 부모의 크기로 사용한다.
		if( m_sizeString.IsZero() )		// 그리기 직전까지 기다렸다가 그때까지도 사이즈가 없으면 강제 계산함.
			ForceCalcLayoutSize();
		if( align & XE::xALIGN_VCENTER ) {
			if( XBREAKF( m_sizeString.h <= 0, "h <= 0, %s", m_szString ) )
				m_sizeString.h = 20;
			vAdj.y = (sizeLocal.h * 0.5f) - (m_sizeString.h * 0.5f);
		}
		// 수평 정렬
		if( align & XE::xALIGN_HCENTER ) {
			if( XBREAKF( m_sizeString.w <= 0, "w <= 0, %s", m_szString ) )
				m_sizeString.w = 10;
			vAdj.x = (sizeLocal.w * 0.5f) - (m_sizeString.w * 0.5f);
		} else
		if( align & XE::xALIGN_RIGHT ) {
			if( XBREAKF( m_sizeString.h <= 0, "h <= 0, %s", m_szString ) )		// 에러났을때 로그(혹은 창)를 너무 많이 남겨서.
				m_sizeString.h = 20;
			vAdj.x = sizeLocal.w - m_sizeString.w;
		}
	}

	const float alpha = GetAlphaFinal();
	const auto vScaleLocal = GetScaleLocal();
	const auto vScaleFinal = GetScaleFinal();
	const XE::VEC2 vPosFinal = vFinal + GetvAdjDrawFinal();
	// 배경색이 있다면 먼저 그린다.
	if( m_colBackground ) {
		if( m_sizeString.IsValid() ) {		// 레이아웃 사이즈가 없으면 그냥 찍지 않도록 한다. 여기서는 write 동작을 하지 않도록 하고 size를 채울책임은 외부에만 둔다.
			const auto vLT = vPosFinal + vAdj;
			const auto sizeFinal = m_sizeString * vScaleFinal;
			BYTE r = XCOLOR_RGB_R( m_colBackground );
			BYTE g = XCOLOR_RGB_G( m_colBackground );
			BYTE b = XCOLOR_RGB_B( m_colBackground );
			BYTE a = XCOLOR_RGB_A( m_colBackground );
			GRAPHICS->FillRectSize( vLT, sizeFinal, XCOLOR_RGBA( r, g, b, (BYTE)(a*alpha)) );
// 			const auto vLT = GetPosFinal() + m_posString * vScaleFinal;
// 			const auto sizeFinal = m_sizeString * vScaleFinal;
// 			GRAPHICS->FillRectSize( vLT, sizeFinal, m_colBackground );
		}
	}
	if( GetbEnable() )
		m_pFont->SetColor( m_Color );
	else
		m_pFont->SetColor( m_colDisable );
	m_pFont->SetScale( vScaleFinal );		// final에는 폰트에만 써야함.
	m_pFont->SetAlign( xALIGN_LEFT );		// 정렬은 컨트롤이 하므로 렌더링은 항상 LEFT로 한다.
	m_pFont->SetColor( m_Color );
	m_pFont->SetAlpha( alpha );
	m_pFont->SetStyle( m_Style );
	SetLineLength( m_lineLength );

// 	vAdj.x = (float)((int)vAdj.x);
// 	vAdj.y = (float)((int)vAdj.y);
	if( IsPassword() ) {
		TCHAR szString[ 256 ];
		_tcscpy_s( szString, m_szString );
		int len = _tcslen( szString );
		for( int i = 0; i < len; ++i )
			szString[ i ] = _T('*');
		m_pFont->DrawString( vPosFinal + vAdj * vScaleFinal, szString );
	} else {
		m_pFont->DrawString( vPosFinal + vAdj * vScaleFinal, m_szString );
	}
}

/**
 @brief 텍스트의 정렬방식을 바꾼다
 정렬은 항상 LEFT로만 하고 실제 정렬은 컨트롤이 하므로 문단폭이나 폰트크기가 변하지 않는한 크기는 변할리가 없다.
*/
void XWndTextString::SetAlign( XE::xAlign _align ) 
{
	XBREAK( !_align );
	const auto alignOld = GetAlign();
	XWnd::SetAlign( _align );		// this에는 받은 그대로 갖고 있어야함.
	if( alignOld != _align ) {
// 		XWnd::SetAlign( _align );		// this에는 받은 그대로 갖고 있어야함.
		if( _align == xALIGN_LEFT ) {		// 왼쪽정렬로 바뀌면 사이즈는 클리어.
			m_posString.Set(0);
			m_sizeString.Set(0);
		} else {
			m_bUpdateSizeAndPos = true;			// 그외는 일단 변경대상.
			m_sizeString.Set( 0 );
			m_posString.Set( 0 );
		}
//		m_pFont->SetAlign( (align == xALIGN_VCENTER)? xALIGN_LEFT : align );
	}
}

/**
 @brief 문장 줄바꿈을 하는 문단 가로폭을 지정한다.
*/
void XWndTextString::SetLineLength( float len )
{
	const auto align = GetAlign();
	XBREAK( !align );
//	const int lenLine = (int)m_pFont->GetLineLength();
	// 기존과 값이 바꼈으면 새로 계산한다.
	if( len != m_lineLength 
		|| (align != xALIGN_LEFT && (m_sizeString.IsZero() || m_posString.IsZero())) ) {
		m_bUpdateSizeAndPos = true;
		m_sizeString.Set( 0 );
		m_posString.Set( 0 );
		m_lineLength = len;
		if( len == 0 ) {
			m_pFont->SetLineLength( XE::GetGameWidth() );
		} else {
			m_pFont->SetLineLength( len );
			// m_vSize는 이제 자동으로 바꾸지 않음.
		}
// 		if( XE::IsHave( m_szString ) )
// 			PreCalcLayoutSize();		// 텍스트가 있을때만 재계산함.
	}
}

/**
 @brief 텍스트컨트롤의 경우 m_vPos를 수정하는게 아니고 자체 정렬옵션을 사용한다.
*/
void XWndTextString::AutoLayoutByAlign( XWnd *pParent, XE::xAlign align )
{
	if( !align )
		align = xALIGN_LEFT;
	SetAlign( align );
}

/**
 @brief 자식들의 크기를 포함
*/
XE::VEC2 XWndTextString::GetSizeNoTransLayout() 
{	
	const auto sizeLocalNoTrans = GetSizeLocalNoTrans();
	// 윈도우 크기가 지정되어 있으면 그것을 우선적으로 돌려준다.
	if( sizeLocalNoTrans.IsValid() )
		return sizeLocalNoTrans;
	// 이함수가 불렸다는것은 이 컨트롤의 크기를 요구받았던것인데 실행순서상. 이 컨트롤에 텍스트가 없어서 사이즈 계산을 못했을수가 있다. 
	// 그래서 다음에 텍스트가 들어오면 left정렬이라도 강제로 사이즈를 계산해서 부모에게 알려줘야 한다.
	if( m_sizeString.IsZero() )
		m_bUpdateSizeIfChangeText = true;
	// 윈도우 크기가 없으면 렌더영역의 크기를 돌려준다.
	return GetLayoutSizeNoTrans();
}

//////////////////////////////////////////////////////////////////////////
XWndTextString* xGET_TEXT_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	return SafeCast<XWndTextString*>( pRoot->Find( cKey ) );
}

XWndTextString* xGET_TEXT_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	return dynamic_cast<XWndTextString*>( pRoot->Findf( cKey ) );
}

XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, int num ) {
	if( pRoot == nullptr )	// if( pRoot ) 이런거 안쓰고 간편하게 하려고 넣음.
		return nullptr;
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText ) {
			pText->SetText( XFORMAT( "%s", XE::NtS( num ) ) );
		}
		return pText;
	} else
		XLOGXNA( "xSET_TEXT: %s not found. ", cKey );
	return nullptr;
}

XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, LPCTSTR szText ) {
	if( pRoot == nullptr )	// if( pRoot ) 이런거 안쓰고 간편하게 하려고 넣음.
		return nullptr;
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText )
			pText->SetText( szText );
		return pText;
	} else
		XLOGXNA( "xSET_TEXT: %s not found. ", cKey );
	return nullptr;
}
XWndTextString* xSET_TEXTF( XWnd *pRoot, LPCTSTR text, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText )
			pText->SetText( text );
		return pText;
	} else
		XLOGXNA( "xSET_SHOW: %s not found. ", cKey );
	return nullptr;
}

XWndTextString* xSET_TEXTF( XWnd *pRoot, const _tstring& text, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText )
			pText->SetText( text );
		return pText;
	} else
		XLOGXNA( "xSET_SHOW: %s not found. ", cKey );
	return nullptr;
}

XWndTextString* xSET_TEXT_FORMAT( XWnd *pRoot, const char *cKey, LPCTSTR strFormat, ... ) {
	XBREAK( pRoot == NULL );
	TCHAR szStr[1024] = {0, };
	va_list vl;
	va_start( vl, strFormat );
	_vstprintf_s( szStr, strFormat, vl );
	va_end( vl );
	auto pText = dynamic_cast<XWndTextString*>( pRoot->Find( cKey ) );
	if( pText )
		pText->SetText( szStr );
	else
		XLOGXN( "xSET_TEXT_FORMAT: %s not found.", C2SZ( cKey ) );
	return pText;
};

void XWndTextString::SetbBatch( bool bFlag )
{
	m_pFont->SetbBatch( bFlag );
}
