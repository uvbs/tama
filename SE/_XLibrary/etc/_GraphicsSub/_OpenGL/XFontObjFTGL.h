#pragma once
#include "XBaseFontObj.h"
#include "XFontDatFTGL.h"
#include "FTGL/ftgles.h"
#include "XFontMng.h"

class XFontObjFTGL;
class XFontObjFTGL : public XBaseFontObj
{
	FTSimpleLayout m_Layout;
	XFontDatFTGL *m_pFTGLDat;
	float m_R, m_G, m_B, m_A;
//    XE::VEC2 m_sizeLayout;
//    XE::VEC2 m_vAdjust;
    
	void Init( void ) {
		m_R = m_G = m_B = m_A = 1.0f;
		m_pFTGLDat = NULL;
	}
	void Destroy( void ) {
	}
public:
	XFontObjFTGL( XBaseFontDat *pDat );
	XFontObjFTGL( const char *cFont, float fontSize, float outset=0 );	// for lua
	virtual ~XFontObjFTGL() { Destroy(); }	
	//
	void SetAlign( xAlign align );
	//
	// virtual
	virtual float GetFontHeight( void ) { 
		return GetpDat()->GetFontHeight();
	}
	virtual float GetFontWidth( void ) {
		return GetpDat()->GetFontWidth();
	}
	virtual void SetColor( XCOLOR col );
	virtual float GetLengthPixel( LPCTSTR str );
	// 480좌표계 기준
	virtual void SetLineLength( float screenw );
//    virtual void SetStaticText( LPCTSTR str );
	virtual XE::VEC2 GetLayoutSize( LPCTSTR szString );
	virtual float _RenderText( float x, float y, LPCTSTR str );
	// lua
	void LuaDrawText( float x, float y, const char *cStr ) {
		DrawString( x, y, Convert_char_To_TCHAR( cStr ) );
	}
	void LuaDrawNumber( float x, float y, int num ) {
		DrawNumber( x, y, num );
	}
};

