#include "stdafx.h"
#include "xVector.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE::VEC2 XE::POINT::ToVec2() const 
{
	return XE::VEC2( x, y );
}

