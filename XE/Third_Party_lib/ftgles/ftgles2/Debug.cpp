#ifdef WIN32
#include <Windows.h>
#endif
#include "Debug.h"

void _NSLog( const char *str, ... )
{
	char            szBuff[8192];	// utf8이 길어서 넉넉하게 잡았다.
    va_list         vl;
	
    va_start(vl, str);
    vsprintf(szBuff, str, vl);
	::OutputDebugStringA( szBuff );
    va_end(vl);
}
