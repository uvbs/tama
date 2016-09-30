#include "stdafx.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
#include "XGame.h"
#endif
#ifdef _GAME_SERVER
	#include "XGameUser.h"
#endif
#include "XLegion.h"
#include "XPropHero.h"
#include "XWorld.h"
#include "XPropItem.h"
#include "XSystem.h"
#include "XSpotPrivateRaid.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;


////////////////////////////////////////////////////////////////
XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld )
	: XSpot( pWorld, xSPOT_PRIVATE_RAID )
{
	Init();
}

XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp )
	: XSpot( pProp, xSPOT_PRIVATE_RAID, pWorld )
{
	Init();
}

void XSpotPrivateRaid::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
}

BOOL XSpotPrivateRaid::DeSerialize( XArchive& ar, DWORD ver )
{
	XSpot::DeSerialize( ar, ver );
	return TRUE;
}