/********************************************************************
	@date:	2015/09/18 17:36
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XStageObjHero2.h
	@author:	xuzhu
	
	@brief:	영웅의전장 전용 스테이지객체
*********************************************************************/
#pragma once
#ifdef _XCAMP_HERO2
#include "XStageObj.h"

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/09/18 17:33
*****************************************************************/

XE_NAMESPACE_START( xCampaign )

class XStageObjHero2 : public XStageObj
{
	friend CampObjPtr sCreateCampObj( xtType typeCamp, XPropCamp::xProp* pPropCamp, int grade );
// 	static void sSetDefaultProp( XPropCamp::xStage *pProp );
public:
// 	static XSPPropStage sCreatePropStage( XPropCamp::xProp *pPropCamp, ID idProp, int idxStage, ID idHeroDrop );
// 	static void sSetLevelLegion( XSPPropStage spPropStage, int lvLegion );
public:
	XStageObjHero2( XSPPropStage spPropStage );
	virtual ~XStageObjHero2() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR( ID, idHero );
	XSPPropStageHero GetspProp() {
		return std::static_pointer_cast<XPropCamp::xStageHero>( GetspPropStage() );
	}
	// public member
	int Serialize( XArchive& ar ) override;
	int DeSerialize( const CampObjPtr spCampObj, int idxStage, XArchive& ar, int verCamp ) override;
//	int AddnumClear() override;
	bool SetDropItemsFromProp( const CampObjPtr spCampObj ) override;
private:
	// private member
	ID m_idHero = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XStageObjHero2

XE_NAMESPACE_END; // xCampaign

#endif // _XCAMP_HERO2
