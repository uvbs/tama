#pragma once
#include "xLib.h"
#include "XFont.h"

class XEToolTip;
extern XEToolTip *TOOLTIP;

class XEToolTip
{
	XBaseFontObj *m_pFontBasic;			// 기본 폰트
	void Init() {
//		m_szText[0] = 0;
		m_pFontBasic = NULL;
//		m_len = 0;
	}
	void Destroy() {
		SAFE_DELETE( m_pFontBasic );
//		SAFE_DELETE_ARRAY( m_szText );
		TOOLTIP = NULL;		// 일단은 이렇게 땜빵 ㅋㅋ
	}
protected:
	XE::VEC2 m_vPos;	// 화면출력 위치
	XE::VEC2 m_Size;
//	TCHAR *m_szText;
	_tstring m_strText;
//	int m_len;
public:
	XEToolTip( LPCTSTR szToolTip, float w, XBaseFontDat *pDat );
	virtual ~XEToolTip(void) { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( const XE::VEC2&, vPos );
	GET_SET_ACCESSOR( const XE::VEC2&, Size );
	GET_ACCESSOR( XBaseFontObj*, pFontBasic );
	void SetText( LPCTSTR szToolTip, BOOL bUpdateSize=TRUE );
	//
	void DrawToolTipText( const XE::VEC2& vPos, LPCTSTR szText, XCOLOR col ) { 
		DrawToolTipText( vPos.x, vPos.y, szText, col );
	}
	void DrawToolTipText( float x, float y, LPCTSTR szText, XCOLOR col );
	// virtual 
	virtual void Draw( void );
	virtual void DrawText( const XE::VEC2& vLT );
	virtual void Update( void ) {}
};

