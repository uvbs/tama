#include "stdafx.h"
#include "XLayoutObj.h"
#include "XLayout.h"
//#include "XWindow.h"
#include "XFramework/XConstant.h"
#include "XClientMain.h"
#include "etc/xLang.h"
#include "XAutoPtr.h"
#include "XSoundMng.h"



//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XLayoutObj* XLayoutObj::sCreateLayout( LPCTSTR szXml, 
										const char *cNodeName, XWnd *pRoot )
{
	auto pLayoutObj = new XLayoutObj( szXml );
	pLayoutObj->CreateLayout( cNodeName, pRoot );
	return pLayoutObj;
}

//////////////////////////////////////////////////////////////////////////
XLayoutObj::XLayoutObj( LPCTSTR szXml, XDelegateLayout *pDelegate/* = nullptr*/ )
{
	_tstring strXml = szXml;
	m_pLayout = new XLayout( strXml.c_str(), pDelegate, XLayout::sGetMain() );
	if( m_pLayout->IsError() ) {
		SAFE_DELETE( m_pLayout );
	}
}

BOOL XLayoutObj::CreateLayout( const char *cKey, XWnd *pParent, const char *cKeyGroup/* = NULL*/ )
{
	XBREAK( m_pLayout == nullptr );
	return m_pLayout->CreateLayout( cKey, pParent, cKeyGroup );
}

void XLayoutObj::Destroy() 
{
	SAFE_DELETE( m_pLayout );
}
