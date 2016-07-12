#include "stdafx.h"
#include "XELaserDX.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XELaserDX::XELaserDX()
{
	Init();
}

void XELaserDX::Destroy()
{
}