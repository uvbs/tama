/********************************************************************
	@date:	2016/06/16 12:24
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XPropCampH.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XPropCamp.h"


XE_NAMESPACE_START( xCampaign )
	
struct xPropCampHero : public XPropCamp::xProp {

	int DeSerialize( XPropCamp *pPropCamp, XArchive& ar, int ver );
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int ver );
}
	
XE_NAMESPACE_END;
