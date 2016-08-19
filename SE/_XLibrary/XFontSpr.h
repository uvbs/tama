#pragma once

#include "XFont.h"
#include "XFontMng.h"
#include "SprMng.h"
// .spr파일로 폰트를 쓰는 버전

class XFontDatSpr : public XBaseFontDat
{
	void Init( void ) {	
		m_pSprDat = NULL;
		m_Alpha = 1.0f;
		m_vScale.Set( 1.0f );
		m_fDist = 0;
	}
	void Destroy( void ) {
		SAFE_RELEASE2( SPRMNG, m_pSprDat );
	}
protected:
	XSprDat *m_pSprDat;
	float m_Alpha;
	XE::VEC2 m_vScale;
	float m_fDist;
public:
	XFontDatSpr( LPCTSTR szFontSpr ) : XBaseFontDat( szFontSpr, 0 ) { 
		Init(); 
		m_pSprDat = SPRMNG->Load( szFontSpr );
		if( m_pSprDat == NULL )
			XERROR( "%s읽기 실패", szFontSpr );
	}
	virtual ~XFontDatSpr() { Destroy(); }	
	//s
	float GetCharWidth( TCHAR tc );
	SET_ACCESSOR( float, Alpha );
	SET_ACCESSOR( const XE::VEC2&, vScale );
	SET_ACCESSOR( float, fDist );
	//
	virtual int GetCharToIdx( TCHAR c );
	virtual float GetFontHeight( void );
	virtual float GetFontWidth( void );
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color, xFONT::xtStyle );
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 ) { XBREAK(1); return NULL; }
	virtual XBaseFontObj* CreateFontObj( void );
};

class XFontObjSpr : public XBaseFontObj
{
	void Init( void ) {	
		m_pDatSpr = NULL;
	}
	void Destroy( void ) {	}
protected:
	XFontDatSpr *m_pDatSpr;
public:
	XFontObjSpr( XFontDatSpr *pDat ) 
		: XBaseFontObj( pDat ) { 
		Init(); 
		m_pDatSpr = pDat;
	}
	virtual ~XFontObjSpr() { Destroy(); }	
	//
	virtual float GetFontHeight( void ) { return m_pDatSpr->GetFontHeight() * m_vScale.y; }
	virtual float GetLengthPixel( LPCTSTR str );
	virtual float _RenderText( float x, float y, LPCTSTR str );
};

/*
	a = new XFontObjSpr( new XFontDatSprNumber( "ui_font_number.spr" ) );
	b = new XFontObjSpr( new XFontDatSprNumber( "ui_font_number.spr" ) );
	c = new XFontObjSpr( new XFontDatSprTime( "ui_font_time.spr" ) );
*/
// 숫자폰트만 있는버전
class XFontDatSprNumber : public XFontDatSpr
{
public:
	XFontDatSprNumber( LPCTSTR szFilename ) : XFontDatSpr( szFilename ) {	}
	virtual ~XFontDatSprNumber() {}

	virtual int GetCharToIdx( TCHAR c ) {
		int idx = -1;
		if( c >= _T('0') && c <= _T('9') )
			idx = c - _T('0');
		else 
			idx = -1;	// *
		return idx; 
	}
};

// 숫자폰트 + : m s
class XFontDatSprTime : public XFontDatSpr
{
public:
	XFontDatSprTime( LPCTSTR szFilename ) : XFontDatSpr( szFilename ) {
	}
	virtual ~XFontDatSprTime() {}

	virtual int GetCharToIdx( TCHAR c ) {
		int idx = -1;
		if( c >= _T('0') && c <= _T('9') )
			idx = c - _T('0');
		else if( c == ':' )
			idx = 10;
		else if( c == 'm' )
			idx = 11;
		else if( c == 's' )
			idx = 12;
		else
			idx = -1;	// 
		return idx; 
	}
};

// 알파벳과 숫자 특수문자 폰트 모두 있는 버전
class XFontDatSprAll : public XFontDatSpr
{
	void Init( void ) {	}
	void Destroy( void ) {	}
public:
	XFontDatSprAll( LPCTSTR szFilename ) : XFontDatSpr( szFilename )	{ Init(); }	// ww:글자사이 간격(기본이 0)
	virtual ~XFontDatSprAll() { Destroy(); }

	virtual int GetCharToIdx( TCHAR c ) {
		int idx = 0;
		if( c >= _T('!') && c <= _T('~') )
		{
			idx = c - _T('!');
		}
		return idx; 	
	}
};

// 업그레이드 씬에서 레벨표시에 쓸 숫자폰트
class XFontDatSprUpgrade : public XFontDatSpr
{
public:
	XFontDatSprUpgrade( LPCTSTR szFilename ) : XFontDatSpr( szFilename ) {	}
	virtual ~XFontDatSprUpgrade() {}

	virtual int GetCharToIdx( TCHAR c ) {
		int idx = -1;
		if( c >= _T('0') && c <= _T('9') )
			idx = c - _T('0');
		else
		if( c == _T('-') || c == _T('+') || c == _T('/') )
			idx = 10;
		else 
			idx = -1;	// *
		return idx; 
	}
};
