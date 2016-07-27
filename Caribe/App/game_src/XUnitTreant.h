/********************************************************************
	@date:	2014/09/24 18:20
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitTreant.h
	@author:	xuzhu
	
	@brief:	트리앤트
*********************************************************************/
#pragma once
#include "XUnitCommon.h"
#include "XObjEtc.h"

/****************************************************************
* @brief 트리앤트
* @author xuzhu
* @date	2014/09/23 9:59
*****************************************************************/
class XUnitTreant : public XUnitCommon
{
	void Init() {}
	void Destroy();
public:
	XUnitTreant( XSquadObj *pSquadObj, 
								ID idProp,
								BIT bitSide, 
								const XE::VEC3& vPos,
								float multipleAbility );
	virtual ~XUnitTreant() { Destroy(); }
	//
	void ShootRangeAttack( UnitPtr& spTarget,
							const XE::VEC3& vwSrc,
							const XE::VEC3& vwDst,
							float damage,
							bool bCritical,
							const std::string& strType,
							const _tstring& strSpr ) override;
	void OnArriveBullet( XObjBullet *pBullet,
								UnitPtr spAttacker,
								UnitPtr spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam ) override;
	void DoDamageToTarget( XSPUnit spTarget, float damage, XSKILL::xtDamage typeDmg, bool bCritical, XGAME::xtDamageAttr typeDmgAttr );
	static void sShootRock( UnitPtr spShooter, UnitPtr& spTarget, const XE::VEC3& vwSrc, const XE::VEC3& vwDst, float damage, bool bCritical, const _tstring& _strSpr = _T( "" ) );
	void FrameMove( float dt ) override;
private:
	CTimer m_timerSec;
	int m_cntPerSec = 0;				///< 1초마다 하나씩 올라가는 카운터(엔트의 광합성 특성에 사용)
};

