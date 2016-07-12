#include "stdafx.h"
#include "XError.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

TCHAR XError::s_szError[ 1024 ] = { 0, };

