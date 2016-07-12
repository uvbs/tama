/********************************************************************
	@date:	2015/07/17 22:38
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XProps.h
	@author:	xuzhu
	
	@brief:	각 프로퍼티 클래스내의 struct xProp을 쓰기 위한 헤더
*********************************************************************/
#pragma once

#include "XPropItem.h"
#include "XPropHero.h"
#include "XPropCloud.h"
#include "XPropWorld.h"

class XEXmlNode;
XE_NAMESPACE_START( XGAME )

XPropItem::xPROP* sReadItemIdentifier( XEXmlAttr& attr, LPCTSTR szTag );
ID sReadItemIdsToId( XEXmlNode& node, const char *cKey, LPCTSTR szNodeName );
DWORD sReadResourceConst( XEXmlAttr& attr, LPCTSTR szTag );
DWORD sReadSpotConst( XEXmlAttr& attr, LPCTSTR szTag );
DWORD sReadConst( XEXmlAttr& attr, LPCTSTR szTag );
XPropHero::xPROP* sReadHeroIdentifier( XEXmlAttr& attr, LPCTSTR szTag );
XPropCloud::xCloud* sReadAreaIdentifier( XEXmlAttr& attr, LPCTSTR szTag );
XPropWorld::xBASESPOT* sReadSpotIdentifier( XEXmlAttr& attr, LPCTSTR szTag );

XE_NAMESPACE_END;
