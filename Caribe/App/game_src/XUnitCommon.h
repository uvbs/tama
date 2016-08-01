/********************************************************************
	@date:	2014/10/16 14:09
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitCommon.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XBaseUnit.h"
#include "XPool.h"
#include "XPropUnit.h"

class XHero;
class XUnitCommon : public XBaseUnit
#ifdef _XMEM_POOL
	, public XMemPool < XUnitCommon >
#endif
{
public:
private:
	void Init() {
	}
	void Destroy();
public:
	XUnitCommon( XSPSquad spSquadObj, ID idProp, BIT bitSide, const XE::VEC3& vPos, float multipleAbility );
	//
	virtual LPCTSTR GetszSpr( void ) {
		return GetpPropUnit()->strSpr.c_str();
	}
	float GetPropScale( void ) {
		return GetpPropUnit()->scale;
	}
	_tstring GetstrIds() override;
};
