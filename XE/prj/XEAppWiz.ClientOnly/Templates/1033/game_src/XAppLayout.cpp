#include "stdafx.h"
#include "XAppLayout.h"
#include "_Wnd2/XWindow.h"
#include "XAutoPtr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XAppLayout* XAppLayout::s_pLayout = NULL;

XAppLayout* XAppLayout::sCreate( LPCTSTR szXml, 
								XBaseDelegate *pDelegate/*=NULL*/) 
{ 
	s_pLayout = new XAppLayout( szXml, pDelegate );
	return s_pLayout;
}

//////////////////////////////////////////////////////////////////////////
XAppLayout::XAppLayout( LPCTSTR szXml, XBaseDelegate *pDelegate ) 
: XLayout( szXml, pDelegate, s_pLayout ) 
{
	Init(); 
	//
}

XCOLOR XAppLayout::GetConstantColor( const char *cKey )
{
	XCOLOR col = XCOLOR_WHITE;
	TiXmlElement *elemConst = GetElement( "constant" );
	if( elemConst )
	{
		TiXmlElement *elemVal = GetElement( cKey, elemConst );
		if( elemVal )
			GetAttrColor( elemVal, "text_col", &col );	
	}
	return col;
}

XWnd* XAppLayout::CreateCustomControl( const char *cCtrlName, 
										TiXmlElement *elemCtrl, 
										XWnd *pParent, 
										const xATTR_ALL& attrAll )
{
	XWnd *pAddWnd = NULL;
	//
	return pAddWnd;
}


