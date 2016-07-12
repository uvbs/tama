#include "stdafx.h"
#include "XMainDelegate.h"
#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XMainDelegate::XMainDelegate()
{
	Init();
}

void XMainDelegate::Destroy()
{
}

// 서버의 메인객체를 생성시킨다.
XEServerMain* XMainDelegate::OnCreateMain( _tstring& strINI )
{
	MAIN = new XMain;
	MAIN->Create( strINI.c_str() );
	return MAIN;
}









