#pragma once
#include "XWnd.h"
#include "SprObj.h"
#include "XFontMng.h"
#include "XFactory.h"
#include "XFont.h"

class XWndMng;
class XSprObj;

/////////////////////////////////////////////////////////////////////////////
class XWndEdit : public XWnd
{
	XSprObj *m_psoField;
	TCHAR m_szString[256];
	XBaseFontObj *m_pFont;
public:
	template<typename T>
	XWndEdit( T x, T y, T w, T h, LPCTSTR szSpr, ID idAct, XBaseFontDat *pFontDat ) 
		: XWnd( NULL, x, y, w, h )
	{
		m_psoField = new XSprObj( szSpr );
		m_psoField->SetAction( idAct );
		m_pFont = FACTORY->CreateFontObj( pFontDat );
		m_pFont->SetColor( XCOLOR_WHITE );
		XCLEAR_ARRAY( m_szString );
//		_tcscpy_s( m_szString, _T("테스트당") );
	}
	virtual ~XWndEdit() {
		SAFE_DELETE( m_psoField );
		SAFE_DELETE( m_pFont );
	}
	//
	GET_ACCESSOR( LPCTSTR, szString );
	void SetszString( LPCTSTR szStr ) {
		_tcscpy_s( m_szString, szStr );
	}
	GET_ACCESSOR( XBaseFontObj*, pFont );

	void DrawField( void ) {
		XBREAK( m_psoField == NULL );
		m_psoField->Draw( GetPosFinal() );		// 바탕 입력필드를 그린다.
	}
	//
	virtual void Draw( void );
	virtual ID OnLButtonUp( float lx, float ly );
	
};
