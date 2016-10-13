#pragma once
#ifdef _VER_OPENGL
#include "XBaseFontObj.h"
#include "XFontDatFTGL.h"
#include "FTGL/ftgles.h"
#include "XFontMng.h"

class XFontObjFTGL;
class XBatchRenderer;
class XFontObjFTGL : public XBaseFontObj
{
	FTSimpleLayout _m_Layout;
	ID m_idCurrFTGLDat;		// 현재 m_Layout에 연결된 FTGL font객체의 아이디. 아이디값이 바뀐건 restore되면서 새로 할당되었으니 여기서도 다시 붙여야함
	XFontDatFTGL *m_pFTGLDat;
	float m_R, m_G, m_B, m_A;
    
	void Init( void ) {
		m_R = m_G = m_B = m_A = 1.0f;
		m_pFTGLDat = NULL;
		m_idCurrFTGLDat = 0;
	}
	void Destroy( void ) {
	}
	FTSimpleLayout& GetLayout() {
		if( m_pFTGLDat->GetidFTGLFont() != m_idCurrFTGLDat )	{
			_m_Layout.SetFont( m_pFTGLDat->Getfont() );		// 아이디가 달라지면 다시 붙임.
			m_idCurrFTGLDat = m_pFTGLDat->GetidFTGLFont();
		}
		return _m_Layout;
	}
public:
	XFontObjFTGL( XBaseFontDat *pDat );
	XFontObjFTGL( LPCTSTR szFont, float fontSize, float outset=0 );	// for lua
	virtual ~XFontObjFTGL() { Destroy(); }	
	//
	void SetAlign( XE::xAlign align );
	//
	// virtual¡
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
	XE::VEC2 GetLayoutSize( LPCTSTR szString ) override;
  XE::VEC2 GetLayoutPos( LPCTSTR szString ) override;
//	void GetLayoutPosAndSize( XE::VEC2* pOutPos, XE::VEC2* pOutSize );
	virtual float _RenderText( float x, float y, LPCTSTR str );
	void _RenderStroke( float x, float y, LPCTSTR szStr, const MATRIX& mVP );
	void RenderTextBatch( XBatchRenderer* pRenderer, const XE::VEC2& _vPos, LPCTSTR szString );
	// lua
	void LuaDrawText( float x, float y, const char *cStr ) {
		DrawString( x, y, Convert_char_To_TCHAR( cStr ) );
	}
	void LuaDrawNumber( float x, float y, int num ) {
		DrawNumber( x, y, num );
	}
};

#endif // gl