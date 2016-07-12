#include "stdafx.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XSocketForGameSvr.h"

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
XGameUserMng *XGAME_USERMNG = NULL;

XGameUserMng::XGameUserMng( int maxUser ) 
	: XEUserMng( MAIN->GetpSocketForGameSvr(), maxUser ) 
{ 
	XGAME_USERMNG = this;
	Init(); 
}

void XGameUserMng::Destroy() 
{
//	AUSERMNG = NULL;
}

XSPGameUser XGameUserMng::GetUser( ID idAccount ) 
{
	return std::static_pointer_cast<XGameUser>( GetspUserFromidAcc( idAccount ) );
}
