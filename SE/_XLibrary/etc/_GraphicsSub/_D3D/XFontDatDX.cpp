#include "stdafx.h"
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

XBaseFontObj* XFontDatDX::CreateFontObj()
{
	return new XFontObjDX( this );
}

BOOL XFontDatDX::Create( LPCTSTR szFont, float fontSize ) 
{
	HRESULT hr;
	hr = D3DXCreateFont( GRAPHICS_D3D->GetDevice(), (int)fontSize, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		_T("나눔고딕"), &m_pFont9 );
	// "맑은 고딕"
	if( XBREAK( hr != S_OK ) )
		return FALSE;
	if( m_pFont9 ) XV_RETURN( m_pFont9->OnResetDevice() );
	m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, nullptr, nullptr, (int)fontSize );
	return TRUE;
}

// 텍스트 레이아웃 기능이 없는 단순 출력루틴
float XFontDatDX::DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style )
{
	x = x * ((float)GRAPHICS->GetPhyScreenWidth() / GRAPHICS->GetScreenWidth()) ;
	y = y * ((float)GRAPHICS->GetPhyScreenHeight() / GRAPHICS->GetScreenHeight());
	m_pTxtHelper->Begin();
	if( style == xFONT::xSTYLE_SHADOW )
	{
		m_pTxtHelper->SetInsertionPos( (int)x+1, (int)y+1 );
		if( color == XCOLOR_BLACK )
			m_pTxtHelper->SetForegroundColor( XCOLOR_WHITE );
		else
			m_pTxtHelper->SetForegroundColor( XCOLOR_BLACK );
		int h = m_pTxtHelper->DrawTextLine( str );
	}
	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
	m_pTxtHelper->SetForegroundColor( color );
	int h = m_pTxtHelper->DrawTextLine( str );
	m_pTxtHelper->End(); 
	return _tcslen( str ) * GetFontWidth();
}


