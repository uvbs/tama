#pragma once
#include "client/XLayout.h"

class XAppLayout : public XLayout
{
public:
	static XAppLayout* sGet() { return s_pLayout; }
	static XAppLayout* sCreate( LPCTSTR szXml, XBaseDelegate *pDelegate=NULL);
private:
	static XAppLayout *s_pLayout;
	void Init() {}
	void Destroy() {}
public:
	XAppLayout( LPCTSTR szXml, XBaseDelegate *pDelegate=NULL );
	virtual ~XAppLayout() { Destroy(); }
	//
	//
	virtual XWnd* CreateCustomControl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	//
	XCOLOR GetConstantColor( const char *cKey );

};
