#include "stdafx.h"
#include "XAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XAccount::XAccount()
{
	Init();
}

void XAccount::Destroy()
{
}

