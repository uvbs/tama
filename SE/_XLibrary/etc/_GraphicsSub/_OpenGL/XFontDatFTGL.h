#pragma once
#include "XBaseFontDat.h"
#include "XGraphics.h"
#include "FTGL/ftgles.h"

class XFontDatFTGL;
class XFontDatFTGL : public XBaseFontDat
{
	FTFont *m_font;
	void Init( void ) {
		m_font = NULL;
	}
	void Destroy( void ) {
		SAFE_DELETE( m_font );
	}
public:
	XFontDatFTGL( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 ) 
		: XBaseFontDat( szFont, fontSize, strokeDepth, colStroke ) { 
		Init(); 
		Create( szFont, fontSize, strokeDepth );
	}
	virtual ~XFontDatFTGL() { Destroy(); }	
	//
	GET_ACCESSOR( FTFont*, font );
	//
	BOOL Create( LPCTSTR szFont, float fontSize, float outset );
	// 1024좌표계를 스크린좌표계(480)로 변환
	static void ToScreenCood( XE::VEC2 *pvInOut ) { 
		pvInOut->x *= (GRAPHICS->GetScreenWidth() / 1024.f);	// ex: 480/1024
		pvInOut->y *= (GRAPHICS->GetScreenHeight() / 768.f);
	}
	// 스크린좌표계(480)를 1024좌표계로 변환
	static void To1024Cood( XE::VEC2 *pvInOut ) { 
		pvInOut->x *= (1024.f / GRAPHICS->GetScreenWidth());
		pvInOut->y = 768.f - (pvInOut->y * (768.f / GRAPHICS->GetScreenHeight()));		// 위아래를 뒤집어준다
	}
	// virtual
	virtual float GetFontHeight( void ) { 
#ifdef _GAME
		return GetFontSize() * (GRAPHICS->GetScreenHeight() / 768.f); 
#else
		return GetFontSize(); 
#endif
	}
	virtual float GetFontWidth( void ) {
#ifdef _GAME
		return GetFontSize() * (GRAPHICS->GetScreenWidth() / 1024.f); 
#else
		return GetFontSize(); 
#endif
	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 ) {
		return new XFontDatFTGL( szFont, fontSize, outset );
	}
	virtual XBaseFontObj* CreateFontObj( void );
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style );
};

