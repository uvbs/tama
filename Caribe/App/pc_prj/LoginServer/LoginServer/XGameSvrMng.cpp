#include "StdAfx.h"
#if 0
#include "XGameSvrSocket.h"
#include "XGameSvrMng.h"
#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// #define USER_LOOP( ELEM ) \
// 	XLIST_LOOP( m_listUser, XEUser*, _pUser )	\
// 		XGameUser *ELEM = static_cast<XGameUser *>( _pUser );

XGameSvrMng *XGAME_GSVRMNG = NULL;

XGameSvrMng::XGameSvrMng( int maxSvr) 
	:XGSvrMng(  MAIN->GetpSocketForGameSvr(), maxSvr) 
{ 
	XGAME_GSVRMNG = this;
	Init(); 
}

void XGameSvrMng::Destroy() 
{
}

#endif // 0