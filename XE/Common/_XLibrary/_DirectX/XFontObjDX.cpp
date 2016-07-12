#include "stdafx.h"
#ifdef _VER_DX
#include "XFontObjDX.h"
#include "XFontMng.h"
#include "etc/XGraphics.h"
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
	m_pTxtHelper = new CDXUTTextHelper( pFontDat->m_pFont9, pFontDat->m_pSprite9, NULL, NULL, (int)pFontDat->GetFontSize() );
}

XFontObjDX::XFontObjDX( const char *cFont, float fontSize )
: XBaseFontObj( m_pFDOwned = FONTMNG->Load( Convert_char_To_TCHAR( cFont ), fontSize ) ) 
{
	Init(); 
	XFontDatDX *pFontDat = SafeCast<XFontDatDX*, XBaseFontDat*>( m_pFDOwned );
	m_pTxtHelper = new CDXUTTextHelper( pFontDat->m_pFont9, pFontDat->m_pSprite9, NULL, NULL, (int)pFontDat->GetFontSize() );
}

XFontObjDX::XFontObjDX( LPCTSTR szFont, float fontSize )
: XBaseFontObj( szFont, fontSize ) 
{
	Init(); 
	XFontDatDX *pFontDat = SafeCast<XFontDatDX*, XBaseFontDat*>( GetpFontDat() );
	m_pTxtHelper = new CDXUTTextHelper( pFontDat->m_pFont9, pFontDat->m_pSprite9, NULL, NULL, (int)pFontDat->GetFontSize() );
}

void XFontObjDX::SetAlign( XE::xAlign align ) 
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

namespace XE 
{
};


XE::VEC2 XFontObjDX::GetLayoutSize( LPCTSTR szString )
{
	float sizeFont = GetpDat()->GetFontSize() * 0.9f;

	int maxLen = 0;
	int lenStr = _tcslen( szString );
	XE::VEC2 vSize(0);
	int wCnt = 0;
	vSize.h += sizeFont / 2.f;
	for( int i = 0; i < lenStr; ++i )
	{
		TCHAR c = szString[i];
		if( c == '\n' )
		{
			float sizeChar = sizeFont / 4.f;		// 영문 한글자 가로크기
			float sizeW = wCnt * sizeChar;
			if( vSize.w < sizeW )
				vSize.w = sizeW;
			wCnt = 0;
			vSize.h += sizeFont / 2.f;
		} else
		{
			if( XE::IsHangul( c ) )
				wCnt += 2;
			else
				++wCnt;
		}
	}
	if( vSize.w == 0 )
	{
		float sizeChar = sizeFont / 3.f;		// 영문 한글자 가로크기
		vSize.w = wCnt * sizeChar;
	}		
// 	int w = (int)(_tcslen( szString ) * sizeFont);
// 	int h = (int)GetpDat()->GetFontSize();
// 	int numCR = XE::GetNumCR( szString );
// 	XE::VEC2 size = XE::VEC2( w, h * numCR ) / 2.f;		// sizeFont는 레티나 기준사이즈 이므로 /2를 한다.
	if( GetLineLength() > 0 && vSize.w > GetLineLength() )
		vSize.w = GetLineLength();
	return vSize;	
}

float XFontObjDX::_RenderText( float x, float y, LPCTSTR str )
{
	XBREAK( str == NULL );
	x += GRAPHICS->GetViewportLeft();
	y += GRAPHICS->GetViewportTop();
#ifdef _XDYNA_RESO
	x += GRAPHICS->GetvScreenLT().x;
	y += GRAPHICS->GetvScreenLT().y;
#endif
	x *= GRAPHICS->GetRatioWidth();
	y *= GRAPHICS->GetRatioHeight();
//	y -= 1.f * GRAPHICS->GetRatioHeight();	// 기기보다 살짝 아래찍혀서 보정함.
//	x = x * ((float)GRAPHICS->GetPhyScreenWidth() / XE::GetGameWidth()) ;
//	y = y * ((float)GRAPHICS->GetPhyScreenHeight() / XE::GetGameHeight());
	DWORD flag = DT_WORDBREAK | DT_NOCLIP;
	if( m_Align & XE::xALIGN_HCENTER )
		flag |= DT_CENTER;
	else if( m_Align & XE::xALIGN_RIGHT )
		flag |= DT_RIGHT;
	if( !m_vArea.IsZero() && m_Align & XE::xALIGN_VCENTER )
		flag |= DT_VCENTER;
	m_pTxtHelper->Begin();
	BYTE r = XCOLOR_RGB_R(m_Color);
	BYTE g = XCOLOR_RGB_G(m_Color);
	BYTE b = XCOLOR_RGB_B(m_Color);
	float alpha = GetAlpha() * (XCOLOR_RGB_A(m_Color) / 255.f);
	BYTE a = (BYTE)(alpha * 255);
	auto col = XCOLOR_RGBA( r, g, b, a );
	if( a == 255 ) {	// 검은색을 배경에 그리고 덧그리는 방식이라서 알파할때 보기싫어서 알파 없을때만 외곽선찍음.
		if( m_Style == xFONT::xSTYLE_SHADOW ) {
			int xx = (int)x + 1;
			int yy = (int)y + 1;
			m_pTxtHelper->SetInsertionPos( xx, yy );
			if( m_Color == XCOLOR_BLACK )
				m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(255,255,255,a) );
			else
				m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(0,0,0,a) );
			float lineLength = m_LineLength * GRAPHICS->GetRatioWidth();
			RECT rc={ xx, yy, (int)xx+(int)(lineLength), (int)(yy + m_vArea.h * 2) };
			m_pTxtHelper->DrawTextLine( rc, flag, str );
		} else
		if( m_Style == xFONT::xSTYLE_STROKE )
		{
			// shadow
			int xx = (int)x + 2;
			int yy = (int)y + 2;
			m_pTxtHelper->SetInsertionPos( xx, yy );
			if( m_Color == XCOLOR_BLACK )
				m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA( 255, 255, 255, a ) );
			else
				m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA( 0, 0, 0, a ) );
			float lineLength = m_LineLength * GRAPHICS->GetRatioWidth();
			RECT rc = {xx, yy, (int)xx + (int)( lineLength ), (int)( yy + m_vArea.h * 2 )};
			m_pTxtHelper->DrawTextLine( rc, flag, str );
			// stroke
			_RenderStroke( (int)x-1, (int)y, str, flag, XCOLOR_RGBA(0,0,0,a) );
			_RenderStroke( (int)x+1, (int)y, str, flag, XCOLOR_RGBA(0,0,0,a) );
			_RenderStroke( (int)x, (int)y-1, str, flag, XCOLOR_RGBA(0,0,0,a) );
			_RenderStroke( (int)x, (int)y+1, str, flag, XCOLOR_RGBA(0,0,0,a) ); 
		}
	}

	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
// 	a = 128;
	m_pTxtHelper->SetForegroundColor( col );
// 	m_pTxtHelper->SetForegroundColor( XCOLOR_RGBA(255,255,255,128) );
	float lineLength = m_LineLength * GRAPHICS->GetRatioWidth();
	RECT rc={ (int)x, (int)y, (int)(x+lineLength), (int)(y + m_vArea.h * 2) };
	m_pTxtHelper->DrawTextLine( rc, flag, str );
	m_pTxtHelper->End(); 
	return _tcslen( str ) * GetpDat()->GetFontWidth();
}

void XFontObjDX::_RenderStroke( int x, int y, LPCTSTR str, DWORD flag, XCOLOR col)
{
	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
	m_pTxtHelper->SetForegroundColor( col );

	float lineLength = m_LineLength * GRAPHICS->GetRatioWidth();
	RECT rc={ x, y, (int)(x+lineLength), (int)(y + m_vArea.h * 2) };
	m_pTxtHelper->DrawTextLine( rc, flag, str );
}
#endif // dx
