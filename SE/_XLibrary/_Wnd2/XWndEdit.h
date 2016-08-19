#pragma once
#include "XWnd.h"
#include "SprObj.h"
#include "XFontMng.h"
#include "XFont.h"

class XWndMng;
class XSprObj;

/////////////////////////////////////////////////////////////////////////////
class XWndEdit : public XWnd
{
	XSprObj *m_psoField;
	TCHAR m_szString[256];
	XBaseFontObj *m_pFont;
	void Init() {
		m_psoField = NULL;
		m_szString[0] = 0;
		m_pFont = NULL;
	}
public:
	template<typename T>
	XWndEdit( T x, T y, T w, T h, LPCTSTR szSpr, ID idAct, XBaseFontDat *pFontDat ) 
		: XWnd( NULL, x, y, w, h )
	{
		Init();
		if( szSpr )
		{
			m_psoField = new XSprObj( szSpr );
			m_psoField->SetAction( idAct );
		}
		m_pFont = pFontDat->CreateFontObj();
		m_pFont->SetColor( XCOLOR_WHITE );
		XCLEAR_ARRAY( m_szString );
//		_tcscpy_s( m_szString, _T("") );
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

	void DrawField( void );
	//
	virtual void Draw( void );
	virtual ID OnLButtonUp( float lx, float ly );
	
};
