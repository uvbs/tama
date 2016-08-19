#pragma once
//#include "xGraphics.h"
#include "XRefRes.h"

namespace xFONT {
	enum xtStyle { xSTYLE_NORMAL, xSTYLE_SHADOW };
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

	void Init() {
		_m_FontSize = 0;
		m_StrokeDepth = 0;
		m_pStroke = nullptr;
		m_colStroke = XCOLOR_BLACK;
		m_pStroke = nullptr;
	}
	void Destroy();
public:
	XBaseFontDat( LPCTSTR szFont, float fontSize, float strokeDepth=0, XCOLOR colStroke=0 ) : XRefRes( szFont ) { 
		Init(); 
		_m_FontSize = fontSize; 
		// 스트로크 파라메터가 지정되면 스트로크용 FontDat를 하나더 만든다
		XBREAK( strokeDepth < 0 );
		// 구현하다 말음
//		if( strokeDepth )
//			m_pStroke = FACTORY->CreateFontDat( szFont, fontSize, strokeDepth );

	}
	virtual ~XBaseFontDat() { Destroy(); }	
	//
	float GetFontSize() { return _m_FontSize; }		
	// 외곽선(stroke)효과가 있는 폰트인가
	BOOL IsStroke() { return (m_pStroke)? TRUE : FALSE; }
	// 폰트에 스트로크 효과를 준다. 파라메터는 두께. 실시간으로 사용하지 않고 생성자 호출직후에 사용한다
/*	void InitStroke( float strokeDepth, XCOLOR colStroke ) {
		XBREAK( m_pStroke != nullptr );
		XBREAK( strokeDepth == 0 );
		m_colStroke = colStroke;
		m_StrokeDepth = strokeDepth;
		m_pStroke = CreateFontDat( GetszFont(), GetFontSize(), strokeDepth );	// virtual. outset폰트를 생성
	} */
	//
	template<typename T>
	float DrawString( T x, T y, XCOLOR color, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
		va_list         vl;
		va_start(vl, format);
		_vstprintf_s(szBuff, format, vl);
		float len = DrawString( (float)x, (float)y, szBuff, color );
		va_end(vl);
		return len;
	}
	template<typename T>
	float DrawStringStyle( T x, T y, XCOLOR color, xFONT::xtStyle style, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
		va_list         vl;
		va_start(vl, format);
		_vstprintf_s(szBuff, format, vl);
		float len = DrawString( (float)x, (float)y, szBuff, color, style );
		va_end(vl);
		return len;
	}
	// virtual
	virtual float GetFontHeight() { return GetFontSize();	}
	virtual float GetFontWidth() { return GetFontSize();	}
	virtual XBaseFontDat* CreateFontDat( LPCTSTR szFont, float fontSize, float outset=0 )=0;
	virtual XBaseFontObj* CreateFontObj()=0;
	virtual float DrawString( float x, float y, LPCTSTR str, XCOLOR color=XCOLOR_WHITE, xFONT::xtStyle style=xFONT::xSTYLE_NORMAL )=0;
};

