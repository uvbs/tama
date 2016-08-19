#include "stdafx.h"
#include "XBaseFontObj.h"
#include "XFontMng.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseFontObj::XBaseFontObj( XBaseFontDat *pDat ) 
{
	Init(); 
	m_pDat = pDat;
	// pDat가 스트로크 타입이면 스트로크용 FontObj도 하나 더 만든다
	if( pDat->IsStroke() )
		m_pStrokeObj = CreateFontObj( pDat );
}

void XBaseFontObj::Destroy() 
{
	SAFE_DELETE( m_pStrokeObj );
}

void XBaseFontObj::SetAlign( xAlign align ) 
{ 
	m_Align = align; 
	if( m_pStrokeObj )
		m_pStrokeObj->SetAlign( align );
}
void XBaseFontObj::SetLineLength( float screenw ) 
{
	m_LineLength = screenw;
	if( m_pStrokeObj )
		m_pStrokeObj->SetLineLength( screenw );
}

void XBaseFontObj::SetColor( XCOLOR col )
{
	m_Color = col;
	if( m_pStrokeObj )
		m_pStrokeObj->SetColor( col );
}
