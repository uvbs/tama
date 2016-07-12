/********************************************************************
	@date:	2015/12/23 14:18
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XGameEtc.h
	@author:	xuzhu
	
	@brief:	XGAME namespace에서 쓰는 기타등등 코드들.
*********************************************************************/
#pragma once
#include "XPropHero.h"

class XWnd;

XE_NAMESPACE_START( XGAME )

void UpdateHeroTooltip( XPropHero::xPROP *pProp, XWnd *pRoot, int cost );
void UpdateHeroTooltip( LPCTSTR idsHero, XWnd *pRoot, int cost );
inline void UpdateHeroTooltip( const _tstring& idsHero, XWnd *pRoot, int cost ) {
	UpdateHeroTooltip( idsHero.c_str(), pRoot, cost );
}
void UpdateHeroTooltip( ID idHero, XWnd *pRoot, int cost );

XE_NAMESPACE_END; // XGAME
