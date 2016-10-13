#pragma once
#include "etc/xGraphics.h"
#include "XRefRes.h"
#include <stdio.h>

namespace xFONT {
	enum xtStyle { xSTYLE_NORMAL, xSTYLE_SHADOW, xSTYLE_STROKE };
};
class XBaseFontDat;
class XBaseFontObj;
class XBaseFontDat : public XRefRes
{
private:
	float _m_FontSize;		// 폰트 크기
	float m_StrokeDepth;		// 외곽선
	XCOLOR m_colStroke;	// 외곽선색
	XBaseFontDat *m_pStroke;		// stroke용 폰트
	_tstring m_strFont;
	bool m_bBatch = false;

	void Init() {
		_m_FontSize = 0;
		m_StrokeDepth = 0;
		m_pStroke = NULL;
		m_colStroke = XCOLOR_BLACK;
		m_pStroke = NULL;
		m_vScale.x = 1.0f;
		m_vScale.y = 1.0f;
	}
	void Destroy();
protected:
	XE::VEC2 m_vScale;		// this는 독립적인 Obj가 아니므로 이 변수는 한번쓰고 버리는 휘발성 객체다.
public:
	XBaseFontDat( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 )
	: XRefRes( szFont ) {
		Init(); 
		_m_FontSize = fontSize;
		m_strFont = szFont;
		// 스트로크 파라메터가 지정되면 스트로크용 FontDat를 하나더 만든다
		XBREAK( strokeDepth < 0 );
		m_StrokeDepth = strokeDepth;
		m_colStroke = colStroke;
	}
	virtual ~XBaseFontDat() { Destroy(); }	
	//
	inline float GetFontSize() const { 
		return _m_FontSize; 
	}	
	inline void SetScale( float xyScale ) {
		m_vScale.x = xyScale;
		m_vScale.y = xyScale;
	}
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScale );
	GET_ACCESSOR_CONST(const _tstring&, strFont);
	GET_SET_BOOL_ACCESSOR( bBatch );
	// 외곽선(stroke)효과가 있는 폰트인가
//	BOOL IsStroke() { return (m_pStroke)? TRUE : FALSE; }
	inline bool IsStroke() const { 
		return (m_StrokeDepth >= 1.0f); 
	}
	// 폰트에 스트로크 효과를 준다. 파라메터는 두께. 실시간으로 사용하지 않고 생성자 호출직후에 사용한다
	float DrawString( float x, float y, XCOLOR color, LPCTSTR format, ... );
	//
	template<typename T>
	float DrawStringStyle( T x, T y, XCOLOR color, xFONT::xtStyle style, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
		va_list         vl;
		va_start(vl, format);
		_vstprintf_s(szBuff, format, vl);
		float len = DrawString( (float)x, (float)y, szBuff, color, style );
		va_end(vl);
		m_vScale.x = 1.0f;
		m_vScale.y = 1.0f;
		return len;
	}
//	float DrawTest( float x, float y, XCOLOR color, LPCTSTR format, float dy ) ;
	// FTGL버전에서는 최초 어떤글자를 렌더링할때 텍스쳐를 만드는 과정이 필요해서 순간 멈춤현상이 생긴다.
	// 그것을 해결하기위해 사전에 사용할 텍스트를 다 등록시켜두고(사전로딩개념) 렌더링을 한다.
//	void AddStaticText( LPCTSTR szText ) {
//		DrawString( 0.f, XE::GetGameHeight(), szText );
//	}
	// virtual
	virtual float GetFontHeight() { return GetFontSize();	}
	virtual float GetFontWidth() { return GetFontSize();	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 )=0;
	virtual XBaseFontObj* CreateFontObj()=0;
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color=XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL, float dist=0.f )=0;
	inline float DrawStringWithStyle( const XE::VEC2& vPos, LPCTSTR str, xFONT::xtStyle style, XCOLOR color=XCOLOR_WHITE ) {
		return DrawString( vPos.x, vPos.y, str, color, style, 0 );
	}
	inline float DrawStringWithStyle( const XE::VEC2& vPos, const _tstring& str, xFONT::xtStyle style, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( vPos.x, vPos.y, str.c_str(), color, style, 0 );
	}
#ifdef WIN32
	inline float DrawString( const XE::VEC2& vPos, const _tstring& str, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( vPos.x, vPos.y, str.c_str(), color );
	}
	inline float DrawString( const XE::VEC2& vPos, LPCTSTR szStr, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( vPos.x, vPos.y, szStr, color );
	}
	inline float DrawString( float x, float y, const _tstring& str, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( XE::VEC2( x, y ), str, color );
	}
#endif // WIN32
	inline float DrawString( const XE::VEC2& vPos, const char* cStr, XCOLOR color = XCOLOR_WHITE ) {
		_tstring strt = C2SZ( cStr );
		return DrawString( vPos, strt.c_str(), color );
	}
	inline float DrawString( const XE::VEC2& vPos, const std::string& str, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( vPos, str.c_str(), color );
	}
	inline float DrawString( float x, float y, const std::string& str, XCOLOR color = XCOLOR_WHITE ) {
		return DrawString( XE::VEC2(x, y), str, color );
	}
	virtual void RestoreDevice() {}
	virtual void Reload() {}
	virtual void Reload2() {}		// 임시
};

