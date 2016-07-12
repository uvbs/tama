/********************************************************************
	@date:	2015/09/18 17:39
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XCampObjHero.h
	@author:	xuzhu
	
	@brief:	영웅의전장 전용 캠페인 객체
*********************************************************************/
#pragma once
#include "XCampObj.h"
#include "XPropHero.h"

XE_NAMESPACE_START( xCampaign )
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/09/18 17:39
*****************************************************************/
class XCampObjHero : public XCampObj
{
public:
	static ID sGetidStageByidHero( ID idHero, int idxStage, int cycle );
	static int sGetCycleByidStage( ID idStage, ID idHero );
public:
	XCampObjHero( XPropCamp::xProp *pProp );
	virtual ~XCampObjHero() { Destroy(); }
	// get/setter
	GET_ACCESSOR( bool, bUpdated );
	// public member
	StageObjPtr CreateStageObj( StagePtr spPropStage, int idxFloor = 0 ) override;
	void CreateFloor( int idxFloor = 0 ) override {
		m_bUpdated = false;
	}	// 여기선 무시함.
#ifdef _GAME_SERVER
	void UpdateStages();
	void Update( XSPAcc spAcc ) override;
#endif // _GAME_SERVER
	int DeSerialize( XArchive& ar, int verCamp ) override;
// 	StageObjHeroPtr GetspStage( int idxStage ) override {
// 		return std::static_pointer_cast<XStageObjHero>( XCampObj::GetspStage(idxStage) );
// 	}
private:
	// private member
	bool m_bUpdated = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
	int GetidxByAryHeroes( const XVector<XPropHero::xPROP*>& ary, ID idHero );
	int GetlvLegionByidxHero( int idxStage, int maxStage ) const;
	void CreateLegionIfEmpty( int lvBase, int idxFloor = 0 ) override;
	void ResetCamp( int lvAcc, int idxFloor = 0 ) override;
}; // class XCampObjHero


XE_NAMESPACE_END;	// xCampaign

