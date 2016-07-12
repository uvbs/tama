#pragma once
#ifdef _VER_OPENGL
#include "XBaseFontDat.h"
#include "etc/xGraphics.h"
#include "FTGL\ftgles.h"


class XFontDatFTGL;
class XFontDatFTGL : public XBaseFontDat
{
	ID m_idFTGLFont;		// m_font가 새로 할당되면 아이디가 바뀜.(안드로이드를 위한 Restore가 생기면서 추가)
	FTFont *m_font;
	void Init( void ) {
		m_font = NULL;
		m_idFTGLFont = 0;
	}
	void Destroy( void );
public:
	XFontDatFTGL( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 ) 
		: XBaseFontDat( szFont, fontSize, strokeDepth, colStroke ) { 
			Init(); 
		Create( szFont, fontSize, strokeDepth );
	}
	virtual ~XFontDatFTGL() { Destroy(); }	
	//
	GET_ACCESSOR( ID, idFTGLFont );
	GET_ACCESSOR( FTFont*, font );
	//
	BOOL Create( LPCTSTR szFont, float fontSize, float outset );
	// 1024좌표계를 스크린좌표계(480)로 변환
	static void ToScreenCood( XE::VEC2 *pvInOut ) {
		pvInOut->x *= 0.5f;
		pvInOut->y *= 0.5f;
	}
	// 스크린좌표계(480)를 1024좌표계로 변환
/*	static void To1024Cood( XE::VEC2 *pvInOut ) {
		float sh = (float)GRAPHICS->GetScreenHeight();
		pvInOut->x *= 2.f;
		pvInOut->y = (sh * 2.f) - (pvInOut->y * 2.f);
	}*/
	// virtual
	virtual float GetFontHeight( void ) { 
#ifdef _XGAME
		return (GetFontSize() * 0.5f)-2.0f;
//		return (GetFontSize() * (GRAPHICS->GetScreenHeight() / (float)GRAPHICS->GetPhyScreenHeight()))-2.0f;
/*		if( GRAPHICS->GetScreenWidth() > GRAPHICS->GetScreenHeight() )
			return GetFontSize() * (GRAPHICS->GetScreenHeight() / 640.f);
		else
			return (GetFontSize() * (GRAPHICS->GetScreenHeight() / 960.f))-2.0f; */
#else
		return GetFontSize(); 
#endif
	}
	virtual float GetFontWidth( void ) {
#ifdef _XGAME
		return GetFontSize() * 0.5f;
//		return GetFontSize() * (GRAPHICS->GetScreenWidth() / (float)GRAPHICS->GetPhyScreenWidth());
/*		if( GRAPHICS->GetScreenWidth() > GRAPHICS->GetScreenHeight() )
			return GetFontSize() * (GRAPHICS->GetScreenWidth() / 960.f);
		else
			return GetFontSize() * (GRAPHICS->GetScreenWidth() / 640.f); */
#else
		return GetFontSize(); 
#endif
	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 );
	virtual XBaseFontObj* CreateFontObj( void );
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style, float dist );
	virtual void RestoreDevice( void );
	virtual void Reload( void ) { RestoreDevice(); }
	void _RenderStroke( float x, float y, LPCTSTR szStr, const MATRIX& mVP );
};

#endif // android