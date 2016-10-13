#include "stdafx.h"
#include "XGraphicsUtil.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XE )
//
void xImg::Destroy()
{
	SAFE_DELETE_ARRAY( m_pDat );
}



//
XE_NAMESPACE_END; // XE

