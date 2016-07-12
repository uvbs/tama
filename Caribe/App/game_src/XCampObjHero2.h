/********************************************************************
	@date:	2015/09/18 17:39
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XCampObjHero2.h
	@author:	xuzhu
	
	@brief:	영웅의전장 전용 캠페인 객체
*********************************************************************/
#pragma once
#ifdef _XCAMP_HERO2
#include "XCampObj.h"
#include "XPropHero.h"

XE_NAMESPACE_START( xCampaign )
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/09/18 17:39
*****************************************************************/
class XCampObjHero2 : public XCampObj
{
public:
	static ID sGetidStageByidHero( ID idHero, int idxStage, int cycle );
	static int sGetCycleByidStage( ID idStage, ID idHero );
public:
	XCampObjHero2( XPropCamp::xPropHero *pProp );
	virtual ~XCampObjHero2() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR_CONST( int, idxFloorByOpen );
	GET_SET_ACCESSOR_CONST( int, idxFloorByBattle );
//	GET_ACCESSOR( bool, bUpdated );
	// public member
	XSPStageObj CreateStageObj( XSPPropStage spPropStage, int idxFloor = 0 ) override;
	XPropCamp::xPropHero* GetpProp() {
		return SafeCast<XPropCamp::xPropHero*>( XCampObj::GetpProp() );
	}
#ifdef _GAME_SERVER
	void UpdateStages();
	void Update( XSPAcc spAcc ) override;
#endif // _GAME_SERVER
	int Serialize( XArchive& ar ) override;
	int DeSerialize( XArchive& ar, int verCamp ) override;
	int GetlvLimit( int idxStage, int idxFloor = 0 ) override;
	XGAME::xtError IsAbleTry( const XSPStageObj spStageObj, int idxFloor = 0 ) override;
	XSPStageObjHero2 GetspStageObjHero( int idxStage, int idxFloor );
private:
	// private member
//	bool m_bUpdated = false;
	int m_idxFloorByOpen = 0;			// 클리어한 단계중에 가장높은 번호(층)
	int m_idxFloorByBattle = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
	int GetidxByAryHeroes( const XVector<XPropHero::xPROP*>& ary, ID idHero );
//	static int sGetlvLegionByidxHero( int idxStage, int maxStage ) const;
	void CreateLegionIfEmpty( int lvBase, int idxFloor = 0 ) override {
		// 영웅의 전장은 군단을 미리 만들어두지 않는다.
	}
	int GetlvLegion( XPropCamp::xStage* pPropStage, int lvBase, int idxFloor ) override;
	ID GetidHeroByCreateSquad( int idxSquad, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage ) override;
	bool ClearStage( int idxStage, int idxFloor = 0 ) override;
	void ResetCampAllFloor( int lvAcc ) override;
	XGAME::xtUnit GetUnitWhenCreateSquad( int idxSquad, XGAME::xtAttack typeAtk, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage, int idxFloor ) override;
}; // class XCampObjHero2


XE_NAMESPACE_END;	// xCampaign

#endif // _XCAMP_HERO2
