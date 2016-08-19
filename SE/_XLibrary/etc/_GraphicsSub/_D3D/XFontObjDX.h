#pragma once
#include <d3dx9.h>
//#include "SDKmisc.h"
#include "DXUTText.h"
#include "XBaseFontObj.h"
#include "XFontDatDX.h"
#include "XFontMng.h"

class XFontObjDX;
class XFontObjDX : public XBaseFontObj
{
	XBaseFontDat *m_pFDOwned;			// lua용이다...
	CDXUTTextHelper*            m_pTxtHelper;
	void Init() {
		m_pFDOwned = nullptr;
		m_pTxtHelper = nullptr;
	}
	void Destroy() {
		SAFE_RELEASE2( FONTMNG, m_pFDOwned );		// this가 Load()시킨것이니 여기서 해제시켜줘야 한다
		SAFE_DELETE( m_pTxtHelper );
	}
public:
	XFontObjDX( XBaseFontDat *pDat );
	XFontObjDX( const char *cFont, float fontSize );	// for lua
	virtual ~XFontObjDX() { Destroy(); }	
	//
	void SetAlign( xAlign align );
	void SetLineLength( float screenw );
	void SetColor( XCOLOR col );
	//
	// virtual
	virtual float GetFontHeight() { 
		return GetpDat()->GetFontHeight();
	}
	virtual float GetFontWidth() {
		return GetpDat()->GetFontWidth();
	}
	virtual float GetLengthPixel( LPCTSTR str );
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

