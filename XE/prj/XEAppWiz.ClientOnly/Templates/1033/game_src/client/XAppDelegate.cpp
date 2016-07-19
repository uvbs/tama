#include "stdafx.h"
#include "XAppDelegate.h"
#include "client/XAppMain.h"
//#include "Main.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XAppDelegate::XAppDelegate()
{
	Init();
}

XClientMain* XAppDelegate::OnCreateAppMain( XE::xtDevice device, float widthPhy, float heightPhy )
{
	return XAppMain::sCreate( device, widthPhy, heightPhy );
//	return XMain::sCreate( device, widthPhy, heightPhy );
}