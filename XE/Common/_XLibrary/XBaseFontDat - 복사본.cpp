#include "stdafx.h"
#include "XBaseFontDat.h"
#include "_Wnd2/XWnd.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XBaseFontDat::Destroy( void ) 
{
	SAFE_DELETE( m_pStroke );
}

/*
float XBaseFontDat::DrawTest( float x, float y, XCOLOR color, LPCTSTR format, float dy ) 
{
	return 0;
}
*/

float XBaseFontDat::DrawString( float x, float y, XCOLOR color, LPCTSTR format, ... ) 
{
	TCHAR szBuff[1024];	// utf8이 길어서 넉넉하게 잡았다.
	va_list         vl;
	va_start(vl, format);
	_vstprintf_s(szBuff, format, vl);
//	if( XWnd::s_bDebugMode )
//		CONSOLE("DrawStr:%f,%f, format=%s, buff=%s", x, y, format, szBuff );
	float len = DrawString( x, y, szBuff, color );
	va_end(vl);
	m_vScale.x = 1.0f;
	m_vScale.y = 1.0f;
	return len;
}

