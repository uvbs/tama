#include "stdafx.h"
#include "XFramework/XGlobal.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XGlobalVal::XGlobalVal()
{
	Init();
	//
}

void XGlobalVal::Destroy()
{
}


bool XGlobalVal::OnDidFinishLoad()
{
	CONSOLE( "loaded global.xml" );
	// 루트노드를 구해놓는다.
	m_pRoot = GetDoc().FirstChild( "global" );
	nodeRoot.SetpRoot( m_pRoot->ToElement() );
	XBREAKF( m_pRoot == NULL, "global node not found" );
	return true;
}

