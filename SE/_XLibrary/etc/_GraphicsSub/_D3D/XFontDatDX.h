#pragma once
#include <d3dx9.h>
//#include "SDKmisc.h"
#include "DXUTText.h"
#include "XBaseFontDat.h"
#include "XGraphics.h"

class XFontDatDX;
class XFontDatDX : public XBaseFontDat
{
	void Init() {
		m_pFont9 = nullptr;
		m_pSprite9 = nullptr;
		m_pTxtHelper = nullptr;
	}
	void Destroy() {
		SAFE_RELEASE( m_pSprite9 );
		SAFE_RELEASE( m_pFont9 );
		SAFE_DELETE( m_pTxtHelper );
	}
public:
	ID3DXFont*                  m_pFont9;
	ID3DXSprite*                m_pSprite9;
	CDXUTTextHelper*            m_pTxtHelper;

	XFontDatDX( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 ) 
		: XBaseFontDat( szFont, fontSize, strokeDepth, colStroke ) { 
		Init(); 
		Create( szFont, fontSize );
	}
	virtual ~XFontDatDX() { Destroy(); }	
	//
	BOOL Create( LPCTSTR szFont, float fontSize );
	// virtual
	virtual float GetFontHeight() { 
#ifdef _GAME
		return GetFontSize() * (GRAPHICS->GetScreenHeight() / 768.f); 
#else
		return GetFontSize(); 
#endif
	}
	virtual float GetFontWidth() {
#ifdef _GAME
		return GetFontSize() * (GRAPHICS->GetScreenWidth() / 1024.f); 
#else
		return GetFontSize(); 
#endif
	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 ) {
		return new XFontDatDX( szFont, fontSize, outset );
	}
	virtual XBaseFontObj* CreateFontObj();
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle style );
};

