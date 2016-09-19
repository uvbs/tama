#pragma once
#ifdef _VER_OPENGL
#include "XBaseFontDat.h"
#include "etc/xGraphics.h"
#include "FTGL/ftgles.h"


class XFontDatFTGL;
class XFontDatFTGL : public XBaseFontDat
{
	ID m_idFTGLFont;		// m_font가 새로 할당되면 아이디가 바뀜.(안드로이드를 위한 Restore가 생기면서 추가)
	FTFont *m_font;
	void Init() {
		m_font = NULL;
		m_idFTGLFont = 0;
	}
	void Destroy();
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
	float GetFontHeight() override;
	float GetFontWidth() override;
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 );
	virtual XBaseFontObj* CreateFontObj();
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style, float dist );
	virtual void RestoreDevice();
	virtual void Reload() { 
		RestoreDevice(); 
	}
	void Reload2() override;
	void _RenderStroke( float x, float y, LPCTSTR szStr, const MATRIX& mVP, float alpha );
};

#endif // android