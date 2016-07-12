#pragma once
#include "XSpot.h"
#include "XPropWorld.h"
#include "XPropCamp.h"
////////////////////////////////////////////////////////////////
class XSpotCampaign : public XSpot
{
public:
private:
	xCampaign::CampObjPtr m_spCampObj;		///< 캠페인 동적데이타

	void Init() {}
	void Destroy() {}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotCampaign( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotCampaign( XWorld *pWorld, XPropWorld::xCampaign* pProp, XPropCamp::xProp *pPropCamp );
	virtual ~XSpotCampaign() { Destroy(); }
	//
	void Release() override {
		m_spCampObj.reset();
	}
	XPropWorld::xCampaign* GetpProp() {
		return static_cast<XPropWorld::xCampaign*>( GetpBaseProp() );
	}
// 	xCampaign::CampObjPtr GetspCampObj() {
// 		return m_spCampaignObj;
// 	}
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
	//
	void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
public:
	//
	void CreateLegion( XSPAcc spAccount ) override {
		XBREAK(1);
	};
	void ClearLegion( void );
	void CreateLegion( xCampaign::XStageObj *pStageObj );
	// 현재 전투한 스테이지의 군단을 없앤다.
	void ClearSpot() override;
	bool IsCampaignType() const override { return true; }
	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum = 1.f ) const override;
	bool Update( XSPAcc spAcc ) override;
	xCampaign::CampObjPtr GetspCampObj() override { 
		return m_spCampObj;
	}
private:
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetName( XSPAcc spAcc ) override;
	void OnBeforeBattle( XSPAcc spAcc ) override;
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;

};	// XSpotCampaign
