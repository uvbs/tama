/********************************************************************
	@date:	2014/09/23 10:00
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitArcher.h
	@author:	xuzhu
	
	@brief:	궁수유닛 클래스
*********************************************************************/
#pragma once
#include "XUnitCommon.h"
#include "XObjEtc.h"
//#include "XLegionObj.h"

class XSquadObj;
/****************************************************************
* @brief 궁수
* @author xuzhu
* @date	2014/09/23 9:59
*****************************************************************/
class XUnitArcher : public XUnitCommon
{
	void Init() {}
	void Destroy();
public:
	XUnitArcher( XSPSquad spSquadObj, 
								ID idProp,
								BIT bitSide, 
								const XE::VEC3& vPos,
								float multipleAbility );
	virtual ~XUnitArcher() { Destroy(); }
	//
	void ShootRangeAttack( UnitPtr& spTarget,
							const XE::VEC3& vwSrc,
							const XE::VEC3& vwDst,
							float damage,
							bool bCritical,
							const std::string& strType,
							const _tstring& strSpr ) override;
};

