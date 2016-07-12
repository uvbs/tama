#pragma once
#include "XBaseFontDat.h"
/*
enum XE::xAlign 
{
	XE::xALIGN_NONE = 0,
	XE::xALIGN_LEFT = 0x01,
	XE::xALIGN_RIGHT = 0x02,
	XE::xALIGN_HCENTER = 0x04,
	XE::xALIGN_VCENTER = 0x08,		// 현재 지원되지 않음.
	XE::xALIGN_CENTER = XE::xALIGN_HCENTER | XE::xALIGN_VCENTER,
	XE::xALIGN_BOTTOM = 0x10,
};
*/
class XBaseFontObj;
class XBaseFontObj
{
	struct xtState {
		XE::VEC2 vScale;		// 폰트의 스케일링비율. (주의:실시간으로 글자를 키우려면 fFontSize
		float alpha;
		float fDist;			// 글자사이 간격
		float lineLength;		
		XE::xAlign align;
		XCOLOR color;
		xFONT::xtStyle style;
		xtState() {
			align = XE::xALIGN_LEFT;
			fDist = 0;
			alpha = 1.0f;
			vScale.Set( 1.0f );
			lineLength = 0;
			color = XCOLOR_WHITE;
			style = xFONT::xSTYLE_NORMAL;
		}
	};
	XBaseFontDat *m_pDat;		// 폰트의 경우는 m_pDat를 this에서 Load한게 아니므로 Release책임도 없다
	XBaseFontDat *m_pFontDat;	// this가 릴리즈 시켜야함.
	XBaseFontObj *m_pStrokeObj;
	void Init( void ) {
		m_pDat = NULL;
		m_pFontDat = NULL;
		m_pStrokeObj = NULL;
		m_Align = XE::xALIGN_LEFT;
		m_fDist = 0;
		m_Alpha = 1.0f;
		m_vScale.Set( 1.0f );
		m_LineLength = 0;
		m_Color = XCOLOR_WHITE;
		m_Style = xFONT::xSTYLE_NORMAL;
	}
	void Destroy( void );
protected:
	XE::VEC2 m_vScale;		// 폰트의 스케일링비율. (주의:실시간으로 글자를 키우려면 fFontSize
	float m_Alpha;
	float m_fDist;			// 글자사이 간격
	float m_LineLength;		
	XE::xAlign m_Align;
	XCOLOR m_Color;
	xFONT::xtStyle m_Style;
public:
	// 파일명과 폰트크기만으로 생성하면 편할텐데 하지 않은 이유.
	// 같은 폰트에 여러사이즈의 폰트가 마구 생겨나 메모리를 낭비하지 않게 하기 위해.
	// Dat의 로딩을 수동 명확하게 함으로써 메모리절약의 경각심을 주기 위함
	XBaseFontObj( XBaseFontDat *pDat );
	XBaseFontObj( LPCTSTR szFont, float fontSize );
	virtual ~XBaseFontObj() { Destroy(); }	
	//
	GET_ACCESSOR( XBaseFontDat*, pDat );
	GET_ACCESSOR( XBaseFontDat*, pFontDat );
	GET_ACCESSOR( const XE::VEC2&, vScale );
	GET_ACCESSOR( XCOLOR, Color );
	GET_SET_ACCESSOR( float, Alpha );
	GET_SET_ACCESSOR( float, fDist );
	GET_SET_ACCESSOR( xFONT::xtStyle, Style );
	float GetScaleX() { return m_vScale.x; }
	float GetScaleY() { return m_vScale.y; }
	void SetScale( float sx, float sy ) { m_vScale.Set( sx, sy ); }	
	void SetScale( float scalexy ) { SetScale( scalexy, scalexy ); }
	void SetScale( const XE::VEC2& vScale ) { m_vScale = vScale; }
	XE::xAlign GetAlign( void ) {
		return m_Align;
	}
	float GetLineLength( void ) {
		return m_LineLength;
	}
	virtual void SetAlign( XE::xAlign align );
	virtual void SetColor( XCOLOR col );
	virtual void SetLineLength( float screenw );
	virtual XE::VEC2 GetLayoutSize( LPCTSTR szString ) { XBREAK(1); return XE::VEC2(0,0); }
	virtual XE::VEC2 GetLayoutPos( LPCTSTR szString ) { XBREAK(1); return XE::VEC2(0,0); }
//    virtual void SetStaticText( LPCTSTR str ) {}
	//
	template<typename T>
	float DrawNumber( const XE::VEC2& vPos, T num ) { return DrawNumber( vPos.x, vPos.y, num ); }
	template<typename T>
	float DrawNumber( float x, float y, T num ) {
		TCHAR szBuff[64];
		_stprintf_s( szBuff, _T("%d"), (int)num );
		return DrawString( x, y, szBuff );
	}
	template<typename T>
	float DrawFloat( T x, T y, float num ) {
		TCHAR szBuff[64];
		_stprintf_s( szBuff, _T("%.2f"), num );
		return DrawString( x, y, szBuff );
	}
	template<typename T>
	float DrawStringF( T x, T y, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
		va_list         vl;
		va_start(vl, format);
		_vstprintf_s(szBuff, format, vl);
		float len = DrawString( x, y, szBuff );
		va_end(vl);
		return len;
	}
	float DrawStringFit(  float x, float y, float wFit, float hFit, LPCTSTR str ) {
		XE::VEC2 vOldScale = m_vScale;
		SetScale( 1.0f, 1.0f );
		float origw = GetLengthPixel( str );		// 스케일링 1.0일때 스트링 길이를 구함
		float r = wFit / (origw * 1.1f);
		SetScale( r, r );
		float ret = DrawString( x, y, str );
		m_vScale = vOldScale;
		return ret;
	}
	float DrawString( const XE::VEC2& vPos, LPCTSTR str ) { 
		return DrawString( vPos.x, vPos.y, str );
	}
	float DrawString( const XE::VEC2& vPos, const _tstring& str ) { 
		return DrawString( vPos.x, vPos.y, str.c_str() );
	}
	template<typename T>
	float DrawString( T x, T y, LPCTSTR str ) { 
		if( m_pStrokeObj )
			m_pStrokeObj->DrawString( x, y, str );		// 스트로크가 지정되어 있다면 스트로크폰트를 먼저 그린다
		float w = _RenderText( (float)x, (float)y, str ); 
		return w;
	}
	float GetFontHeight( void ) { return m_pDat->GetFontHeight();	}
	float GetFontWidth( void ) { return m_pDat->GetFontWidth();	}
	void ChangeFontDat( LPCTSTR szFont, float size );
	// virtual
	virtual void SetArea( const XE::VEC2& vSize ) {}
	virtual float GetLengthPixel( LPCTSTR str ) = 0;
	virtual float _RenderText( float x, float y, LPCTSTR str ) = 0;
	virtual XBaseFontObj* CreateFontObj( XBaseFontDat *pDat ) { XBREAK(1); return NULL; }
	// lua
	void LuaDrawText( float x, float y, const char *cStr ) {
		DrawString( x, y, Convert_char_To_TCHAR( cStr ) );
	}
	void LuaDrawNumber( float x, float y, int num ) {
		DrawNumber( x, y, num );
	}
};

