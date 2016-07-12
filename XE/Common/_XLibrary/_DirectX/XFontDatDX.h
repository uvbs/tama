#pragma once
#ifdef _VER_DX
#include <d3dx9.h>
//#include "SDKmisc.h"
#include "DXUTText.h"
#include "XBaseFontDat.h"
#include "etc/XGraphics.h"

class XFontDatDX;
class XFontDatDX : public XBaseFontDat
{
	void Init( void ) {
		m_pFont9 = NULL;
		m_pSprite9 = NULL;
		m_pTxtHelper = NULL;
	}
	void Destroy( void ) {
		SAFE_RELEASE( m_pSprite9 );
		SAFE_RELEASE( m_pFont9 );
		SAFE_DELETE( m_pTxtHelper );
	}
public:
	ID3DXFont*                  m_pFont9;
	ID3DXSprite*                m_pSprite9;
	CDXUTTextHelper*            m_pTxtHelper;

	XFontDatDX( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 ) 
//		: XBaseFontDat( szFont, fontSize, 1.0f, colStroke ) { 
		: XBaseFontDat( szFont, fontSize, strokeDepth, colStroke ) { 
		Init(); 
		Create( szFont, fontSize );
	}
	virtual ~XFontDatDX() { Destroy(); }	
	//
	BOOL Create( LPCTSTR szFont, float fontSize );
	// virtual
	virtual float GetFontHeight( void ) { 
		return GetFontSize() * GRAPHICS->GetRatioHeight();
//		return GetFontSize() * (XE::GetGameHeight() / 768.f); 
	}
	virtual float GetFontWidth( void ) {
		return GetFontSize() * GRAPHICS->GetRatioWidth();
//		return GetFontSize() * (XE::GetGameWidth() / 1024.f); 
	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 );
	virtual XBaseFontObj* CreateFontObj( void );
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style, float dist=0.f );
};

#endif // dx