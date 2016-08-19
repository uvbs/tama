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
	float _m_FontSize;		// ��Ʈ ũ��
	float m_StrokeDepth;		// �ܰ���
	XCOLOR m_colStroke;	// �ܰ�����
	XBaseFontDat *m_pStroke;		// stroke�� ��Ʈ

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
		// ��Ʈ��ũ �Ķ���Ͱ� �����Ǹ� ��Ʈ��ũ�� FontDat�� �ϳ��� �����
		XBREAK( strokeDepth < 0 );
		// �����ϴ� ����
//		if( strokeDepth )
//			m_pStroke = FACTORY->CreateFontDat( szFont, fontSize, strokeDepth );

	}
	virtual ~XBaseFontDat() { Destroy(); }	
	//
	float GetFontSize() { return _m_FontSize; }		
	// �ܰ���(stroke)ȿ���� �ִ� ��Ʈ�ΰ�
	BOOL IsStroke() { return (m_pStroke)? TRUE : FALSE; }
	// ��Ʈ�� ��Ʈ��ũ ȿ���� �ش�. �Ķ���ʹ� �β�. �ǽð����� ������� �ʰ� ������ ȣ�����Ŀ� ����Ѵ�
/*	void InitStroke( float strokeDepth, XCOLOR colStroke ) {
		XBREAK( m_pStroke != nullptr );
		XBREAK( strokeDepth == 0 );
		m_colStroke = colStroke;
		m_StrokeDepth = strokeDepth;
		m_pStroke = CreateFontDat( GetszFont(), GetFontSize(), strokeDepth );	// virtual. outset��Ʈ�� ����
	} */
	//
	template<typename T>
	float DrawString( T x, T y, XCOLOR color, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8�� �� �˳��ϰ� ��Ҵ�.
		va_list         vl;
		va_start(vl, format);
		_vstprintf_s(szBuff, format, vl);
		float len = DrawString( (float)x, (float)y, szBuff, color );
		va_end(vl);
		return len;
	}
	template<typename T>
	float DrawStringStyle( T x, T y, XCOLOR color, xFONT::xtStyle style, LPCTSTR format, ... ) {
		TCHAR szBuff[1024];	// utf8�� �� �˳��ϰ� ��Ҵ�.
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

