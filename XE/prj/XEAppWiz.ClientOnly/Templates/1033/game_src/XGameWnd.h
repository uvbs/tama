#pragma once
#include "_Wnd2/XWindow.h"
#include "etc/Timer.h"
#include "../XDrawGraph.h"
#include "client/XTimeoutMng.h"
#include "Game/XFLevel.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

class XAppLayout;
extern XAppLayout *LAYOUT;

class XGameWndAlert : public XWndAlert
{
	void Init() {}
	void Destroy() {}
public:
	XGameWndAlert( LPCTSTR szText, LPCTSTR szTitle=NULL, XWnd::xtAlert type=XWnd::xOK, XCOLOR col=XCOLOR_WHITE );
	virtual ~XGameWndAlert() { Destroy(); }
};

#define	XWND_ALERT(F,...) { \
	TCHAR _szBuff[ 256 ];	\
	_tcscpy_s( _szBuff, XE::Format( _T(F), ##__VA_ARGS__ ) );	\
	XWnd *_pDlg = new XGameWndAlert( _szBuff );	\
	GAME->AddWndTop( _pDlg );	\
}

#define	XWND_ALERT_RET( RET, F,...) \
	TCHAR _szBuff[ 256 ];	\
	_tcscpy_s( _szBuff, XE::Format( _T(F), ##__VA_ARGS__ ) );	\
	XWnd *RET = new XGameWndAlert( _szBuff );	\
	GAME->AddWndTop( RET );	\

// TYPE: XWnd::xOK 등등.
#define	XWND_ALERT_PARAM_RET( RET, TITLE, TYPE, COL, F,...) \
	TCHAR __szBuff[ 256 ];	\
	_tcscpy_s( __szBuff, XE::Format( _T(F), ##__VA_ARGS__ ) );	\
	XWndAlert *RET = new XGameWndAlert( __szBuff, TITLE, TYPE, COL );	\
	GAME->AddWndTop( RET );	\


#define	XWND_ALERT_T(F,...) { \
	TCHAR _szBuff[ 256 ];	\
	_tcscpy_s( _szBuff, XE::Format( F, ##__VA_ARGS__ ) );	\
	XWnd *_pDlg = new XGameWndAlert( _szBuff );	\
	GAME->AddWndTop( _pDlg );	\
}

#define	XWND_ALERT_YESNO( RET, IDS, F,...) \
	XGameWndAlert *RET = NULL; \
	if( GAME->Find(IDS) == NULL ) { \
		TCHAR _szBuff[ 256 ];	\
		_tcscpy_s( _szBuff, XE::Format( _T(F), ##__VA_ARGS__ ) );	\
		RET = new XGameWndAlert( _szBuff, NULL, XWnd::xYESNO );	\
		RET->SetstrIdentifier(IDS); \
		GAME->AddWndTop( RET ); \
	}




#pragma warning ( default : 4250 )
