#include "stdafx.h"
#ifdef _VER_DX
#include "XFont.h"
#include "XFontDatDX.h"
#include "XFontObjDX.h"
#include "XGraphicsD3D.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseFontObj* XFontDatDX::CreateFontObj( void )
{
	return new XFontObjDX( this );
}

BOOL XFontDatDX::Create( LPCTSTR szFont, float fontSize ) 
{
	HRESULT hr;

//	fontSize += fontSize * 0.195f;		// 윈도우가 기기보다 폰트가 좀 작게나와서 보정함.
	fontSize += 2;
	UINT weight = FW_BOLD;
	XBREAK( XE::IsEmpty(szFont) == TRUE );
	if( XE::IsSame( szFont, FONT_NANUM_BOLD ) )
		weight = FW_EXTRABOLD;
	hr = D3DXCreateFont( GRAPHICS_D3D->GetDevice(), (int)fontSize, 0, weight, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
//		szFullpath, &m_pFont9 );
		szFont, &m_pFont9 );
//		_T("나눔고딕"), &m_pFont9 );
	// "맑은 고딕"
	if( XBREAK( hr != S_OK ) )
		return FALSE;
	if( m_pFont9 ) XV_RETURN( m_pFont9->OnResetDevice() );
	D3DXCreateSprite( GRAPHICS_D3D->GetDevice(), &m_pSprite9 );
	m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, NULL, NULL, (int)fontSize );
	return TRUE;
}

// 텍스트 레이아웃 기능이 없는 단순 출력루틴
float XFontDatDX::DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style, float dist )
{
	x += GRAPHICS->GetViewportLeft();
	y += GRAPHICS->GetViewportTop();
#ifdef _XDYNA_RESO
	x += GRAPHICS->GetvScreenLT().x;
	y += GRAPHICS->GetvScreenLT().y;
#endif
	x *= GRAPHICS->GetRatioWidth();
	y *= GRAPHICS->GetRatioHeight();
//	x = x * ((float)GRAPHICS->GetPhyScreenWidth() / XE::GetGameWidth()) ;
//	y = y * ((float)GRAPHICS->GetPhyScreenHeight() / XE::GetGameHeight());
	BYTE r = XCOLOR_RGB_R(color);
	BYTE g = XCOLOR_RGB_G(color);
	BYTE b = XCOLOR_RGB_B(color);
	BYTE a = XCOLOR_RGB_A(color);
	m_pTxtHelper->Begin();
	if( style == xFONT::xSTYLE_SHADOW && a == 255 )
	{
		m_pTxtHelper->SetInsertionPos( (int)x+1, (int)y+1 );
		if( color == XCOLOR_BLACK )
			m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(255,255,255,a) );
		else
			m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(0,0,0,a) );
		int h = m_pTxtHelper->DrawTextLine( str );
	} else
//	if( IsStroke() )
	if( style == xFONT::xSTYLE_STROKE && a == 255 ) {
		// shadow
		m_pTxtHelper->SetInsertionPos( (int)x + 2, (int)y + 2 );
		if( color == XCOLOR_BLACK )
			m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA( 255, 255, 255, a ) );
		else
			m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA( 0, 0, 0, a ) );
		m_pTxtHelper->DrawTextLine( str );
		// stroke
		m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(0,0,0,a) );
		m_pTxtHelper->SetInsertionPos( (int)x-1, (int)y );
		m_pTxtHelper->DrawTextLine( str );
		m_pTxtHelper->SetInsertionPos( (int)x+1, (int)y );
		m_pTxtHelper->DrawTextLine( str );
		m_pTxtHelper->SetInsertionPos( (int)x, (int)y-1 );
		m_pTxtHelper->DrawTextLine( str );
		m_pTxtHelper->SetInsertionPos( (int)x, (int)y+1 );
		m_pTxtHelper->DrawTextLine( str );
	}
	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
	m_pTxtHelper->SetForegroundColor( color );
	int h = m_pTxtHelper->DrawTextLine( str );
	m_pTxtHelper->End(); 
	// dx버전에선 scale지원 안됨
	XBaseFontDat::SetScale( 1.0f );
	return _tcslen( str ) * GetFontWidth();
}
XBaseFontDat* XFontDatDX::CreateFontDat( LPCTSTR szFont, float fontSize, float outset ) 
{
	return new XFontDatDX( szFont, fontSize, outset );
}


#endif // dx
