#include "stdafx.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XSocketSvrForClient.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGameUserMng *XGAME_USERMNG = NULL;

XGameUserMng::XGameUserMng( int maxUser ) 
	: XEUserMng( MAIN->GetpSocketForClient(), maxUser ) 
{ 
	XGAME_USERMNG = this;
	Init(); 
}

void XGameUserMng::Destroy() 
{
//	AUSERMNG = NULL;
}

XSPGameUser XGameUserMng::GetspUser( ID idAccount ) 
{
	return std::static_pointer_cast<XGameUser>( GetspUserFromidAcc( idAccount ) );
}
