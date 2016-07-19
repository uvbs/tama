#include "stdafx.h"
#include "XGameWnd.h"
#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XConstant.h"
#include "_Wnd2/XWindow.h"
#include "XImageMng.h"
#include "XAppLayout.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//////////////////////////////////////////////////////////////////////////
XGameWndAlert::XGameWndAlert( LPCTSTR _szText, LPCTSTR _szTitle, XWnd::xtAlert type, XCOLOR col )
	: XWndAlert( NULL, 
				20.f, 174.f, 
				XE::GetGameWidth()-32.f, 160.f, 
				_T("popup01.png"), 
				BUTT01, 
				_szText, 
				_szTitle, 
				type, 
				col )
//	: XWndAlert( 16.f, 174.f, 288.f, 160.f, _T("popup01.png"), BUTT_MID, _szText, _szTitle, type, col )
{
	Init();
}


