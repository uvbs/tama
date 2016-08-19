#include "stdafx.h"
#include "XFontObjDX.h"
#include "XFontMng.h"
#include "XGraphics.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XFontObjDX::XFontObjDX( XBaseFontDat *pDat ) 
: XBaseFontObj( pDat ) 
{
	Init(); 
	XFontDatDX *pFontDat = SafeCast<XFontDatDX*, XBaseFontDat*>( pDat );
	m_pTxtHelper = new CDXUTTextHelper( pFontDat->m_pFont9, pFontDat->m_pSprite9, nullptr, nullptr, (int)pFontDat->GetFontSize() );
}

XFontObjDX::XFontObjDX( const char *cFont, float fontSize )
: XBaseFontObj( m_pFDOwned = FONTMNG->Load( Convert_char_To_TCHAR( cFont ), fontSize ) ) 
{
	Init(); 
	XFontDatDX *pFontDat = SafeCast<XFontDatDX*, XBaseFontDat*>( m_pFDOwned );
	m_pTxtHelper = new CDXUTTextHelper( pFontDat->m_pFont9, pFontDat->m_pSprite9, nullptr, nullptr, (int)pFontDat->GetFontSize() );
}

void XFontObjDX::SetAlign( xAlign align ) 
{ 
	XBaseFontObj::SetAlign( align );
	// DX전용 코드
}
void XFontObjDX::SetLineLength( float screenw ) 
{
	XBaseFontObj::SetLineLength( screenw );
	// DX전용 코드
}

void XFontObjDX::SetColor( XCOLOR col )
{
	XBaseFontObj::SetColor( col );
	// DX전용 코드
}

float XFontObjDX::GetLengthPixel( LPCTSTR str )
{
	XBREAK(1);
	return 0;
}

XE::VEC2 XFontObjDX::GetLayoutSize( LPCTSTR szString )
{
	CRect rc( 0, 0, (int)m_LineLength * 2, (int)200 );
	m_pTxtHelper->DrawTextLine( rc, DT_WORDBREAK | DT_CALCRECT, szString );
	return XE::VEC2( rc.Width()/2, rc.Height()/2 );
}

float XFontObjDX::_RenderText( float x, float y, LPCTSTR str )
{
	XBREAK( str == nullptr );
	x = x * ((float)GRAPHICS->GetPhyScreenWidth() / GRAPHICS->GetScreenWidth()) ;
	y = y * ((float)GRAPHICS->GetPhyScreenHeight() / GRAPHICS->GetScreenHeight());
	DWORD flag = DT_WORDBREAK | DT_NOCLIP;
	if( m_Align & xALIGN_HCENTER )
		flag |= DT_CENTER;
	if( m_Align & xALIGN_VCENTER )
		flag |= DT_VCENTER;
	m_pTxtHelper->Begin();
	if( m_Style == xFONT::xSTYLE_SHADOW )
	{
		int xx = (int)x + 1;
		int yy = (int)y + 1;
		m_pTxtHelper->SetInsertionPos( xx, yy );
		if( m_Color == XCOLOR_BLACK )
			m_pTxtHelper->SetForegroundColor( XCOLOR_WHITE );
		else
			m_pTxtHelper->SetForegroundColor( XCOLOR_BLACK );
		RECT rc={ xx, yy, xx+((int)m_LineLength*2), yy };
		m_pTxtHelper->DrawTextLine( rc, flag, str );
	}
	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
	m_pTxtHelper->SetForegroundColor( m_Color );
	RECT rc={ (int)x, (int)y, (int)(x+m_LineLength*2.f), (int)y };
	m_pTxtHelper->DrawTextLine( rc, flag, str );
	m_pTxtHelper->End(); 
	return _tcslen( str ) * GetpDat()->GetFontWidth();
}
