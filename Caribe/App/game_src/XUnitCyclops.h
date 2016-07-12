/********************************************************************
	@date:	2014/10/06 14:23
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitCyclops.h
	@author:	xuzhu
	
	@brief:	cyclops
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
class XUnitCyclops : public XUnitCommon
{
	void Init() {}
	void Destroy() {}
public:
	XUnitCyclops( XSquadObj *pSquadObj, 
								ID idProp,
								BIT bitSide, 
								const XE::VEC3& vPos,
								float multipleAbility );
	~XUnitCyclops() { Destroy(); }
	//
//	static void 
	static void sShootLaser( UnitPtr spShooter,
																	UnitPtr& spTarget,
																	const XE::VEC3& vwSrc,
																	const XE::VEC3& vwDst,
																	float damage,
																	bool bCritical,
																	const _tstring& _strSpr );
	void ShootRangeAttack( UnitPtr& spTarget,
							const XE::VEC3& vwSrc,
							const XE::VEC3& vwDst,
							float damage,
							bool bCritical,
							const std::string& strType,
							const _tstring& _strSpr ) override;
};

