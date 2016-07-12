#include "stdafx.h"
#include "XGuildRaid.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START(xGuild)

////////////////////////////////////////////////////////////////
XRaid::XRaid()
{
  Init();
}

void XRaid::Destroy()
{
}

XE_NAMESPACE_END;
