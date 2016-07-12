#pragma once
#ifdef _VER_DX
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
	XE::VEC2 m_vArea;			// WIN32에서만 쓰는것. 세로 중앙정렬을 위함.
	void Init( void ) {
		m_pFDOwned = NULL;
		m_pTxtHelper = NULL;
	}
	void Destroy( void ) {
		SAFE_RELEASE2( FONTMNG, m_pFDOwned );		// this가 Load()시킨것이니 여기서 해제시켜줘야 한다
		SAFE_DELETE( m_pTxtHelper );
	}
public:
	XFontObjDX( XBaseFontDat *pDat );
	XFontObjDX( const char *cFont, float fontSize );	// for lua
	XFontObjDX( LPCTSTR szFont, float fontSize );
	virtual ~XFontObjDX() { Destroy(); }	
	//
	void SetAlign( XE::xAlign align );
	void SetLineLength( float screenw );
	void SetColor( XCOLOR col );
	//
	// virtual
	virtual float GetFontHeight( void ) { 
		return GetpDat()->GetFontHeight();
	}
	virtual float GetFontWidth( void ) {
		return GetpDat()->GetFontWidth();
	}
	virtual float GetLengthPixel( LPCTSTR str );
	XE::VEC2 GetLayoutSize( LPCTSTR szString ) override;
	XE::VEC2 GetLayoutPos( LPCTSTR szString ) override { 
		// DX에서는 지원안되므로 사용하지 말것.
		XBREAK(1);
		return XE::VEC2( 0, 0 ); 
	}
	virtual void SetArea( const XE::VEC2& vSize ) {
		m_vArea = vSize;
	}
	virtual float _RenderText( float x, float y, LPCTSTR str );
	void _RenderStroke( int x, int y, LPCTSTR str, DWORD flag, XCOLOR col);
	// lua
	void LuaDrawText( float x, float y, const char *cStr ) {
		DrawString( x, y, Convert_char_To_TCHAR( cStr ) );
	}
	void LuaDrawNumber( float x, float y, int num ) {
		DrawNumber( x, y, num );
	}
};

#endif // dx
