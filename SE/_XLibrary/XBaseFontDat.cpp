#include "stdafx.h"
#include "XBaseFontDat.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XBaseFontDat::Destroy() 
{
	SAFE_DELETE( m_pStroke );
}

