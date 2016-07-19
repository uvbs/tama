#include "stdafx.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XSocketForClient.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define USER_LOOP( ELEM ) \
	XLIST_LOOP( m_listUser, XEUser*, _pUser )	\
		XGameUser *ELEM = static_cast<XGameUser *>( _pUser );

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

