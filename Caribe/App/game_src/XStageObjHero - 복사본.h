/********************************************************************
	@date:	2015/09/18 17:36
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XStageObjHero.h
	@author:	xuzhu
	
	@brief:	영웅의전장 전용 스테이지객체
*********************************************************************/
#pragma once
#include "XStageObj.h"

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/09/18 17:33
*****************************************************************/

XE_NAMESPACE_START( xCampaign )

class XStageObjHero : public XStageObj
{
	friend CampObjPtr sCreateCampObj( xtType typeCamp, XPropCamp::xProp* pPropCamp, int grade );
public:
	static StagePtr sCreatePropStage( XPropCamp::xProp *pPropCamp, ID idProp, int idxStage, ID idHeroDrop );
	static void sSetLevelLegion( StagePtr spPropStage, int lvLegion );
public:
	XStageObjHero( StagePtr spPropStage );
	virtual ~XStageObjHero() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR( ID, idHero );
	// public member
	int Serialize( XArchive& ar ) override;
	int DeSerialize( XPropCamp::xProp* pPropCamp, int idxStage, XArchive& ar, int verCamp ) override;
	int AddnumClear() override {
		XLOCK_OBJ;
		if( GetnumStar() > 0 )	// 별이 없으면(최초클리어) numClear수를 올리지 않음.
			return XStageObj::AddnumClear();
		return 0;
	}
private:
	// private member
	ID m_idHero = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XStageObjHero

XE_NAMESPACE_END; // xCampaign

