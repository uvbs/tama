#pragma once
#include "XSpot.h"
#include "XPropCamp.h"
#include "constGame.h"
////////////////////////////////////////////////////////////////
class XSpotVisit : public XSpot
{
public:
private:
	//
//	XPropWorld::xVisit *m_pProp;
	void Init() {}
	void Destroy() { }
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotVisit( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotVisit( XWorld *pWorld, XPropWorld::xVisit* pProp )
		: XSpot( pProp, XGAME::xSPOT_VISIT, pWorld )
	{
		Init();
		if( pProp )
			SetstrName( XTEXT(pProp->idName) );
	}
	virtual ~XSpotVisit() { Destroy(); }
	//
	XPropWorld::xVisit* GetpProp() {
		return static_cast<XPropWorld::xVisit*>( GetpBaseProp() );
	}
//	GET_ACCESSOR( XPropWorld::xVisit*, pProp );
	LPCTSTR GetName() {
		return XTEXT( GetpProp()->idName );
	}
	LPCTSTR GetDialog() {
		return XTEXT( GetpProp()->idDialog );
	}
	//
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD verWorld );
public:
	//
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
private:
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xVisit*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
}; // XSpotVisit

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/18 17:34
*****************************************************************/
class XSpotCash : public XSpot
{
	int m_numCash = 0;
	void Init() {}
	void Destroy() {}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotCash( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotCash( XWorld *pWorld, XPropWorld::xCash* pProp )
		: XSpot( pProp, XGAME::xSPOT_CASH, pWorld )
	{
		Init();
		if( pProp )
			OnSpawn( nullptr );
	}
	virtual ~XSpotCash() { Destroy(); }
	//
	XPropWorld::xCash* GetpProp() {
		return static_cast<XPropWorld::xCash*>( GetpBaseProp() );
	}
	GET_SET_ACCESSOR( int, numCash );
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD verWorld );
public:
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
	void OnCreateNewOnServer( XSPAcc spAcc ) override {}
	BOOL IsActive() const override { 
		return m_numCash > 0;
	}
	void Initialize( XSPAcc spAcc ) override {
		// 0으로 만들어 Deactive상태로 만든다.
		m_numCash = 0;
		// 리젠타이머를 작동한다.
		float sec = GetpProp()->secRegen + xRandomF( GetpProp()->secRegen * 0.1f );
		GettimerSpawnMutable().Set( sec );
	}
	void OnSpawn( XSPAcc spAcc ) override {
		// 스폰이 되면 젬 개수를 설정하고 리젠 타이머는 끈다.
		m_numCash = xRandom( (int)GetpProp()->produceMin, (int)GetpProp()->produceMax );
		GettimerSpawnMutable().Off();
	}
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pUser, ID idAcc, xSec secLastCalc ) override;
private:
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xCash*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
}; // class XSpotCash

//////////////////////////////////////////////////////////////////////////
/**
 @brief 범용 스팟
*/
class XSpotCommon : public XSpot
{
//	bool m_bPc = false;		// 플레이어 스팟인가
	xCampaign::CampObjPtr m_spCampObj;		// 길드캠페인객체는 스팟에 없어야 하는게 정상이지만 현재 구조적문제때문에 일단 스팟에 있기로 함.
	int m_DayOfCamp = 0;			// 캠페인이 생성된 일수.
	void Init() {}
	void Destroy() {}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotCommon( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotCommon( XWorld *pWorld, XPropWorld::xCommon* pProp )
		: XSpot( pProp, XGAME::xSPOT_COMMON, pWorld ) {
		Init();
		if( pProp )
			SetstrName( XTEXT( pProp->idName ) );
	}
	virtual ~XSpotCommon() { Destroy(); }
	//
	void Release() override {
		m_spCampObj.reset();
	}
	void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
	BOOL DeSerialize( XArchive& ar, DWORD ver );
	bool IsNpc() const override {
		return !IsPC();
	}
	bool IsPC() const override {
		if( IsHomeSpot() )
			return true;
		return false;
	}
	XPropWorld::xCommon* GetpProp() {
		auto pBaseProp = GetpBaseProp();
		return static_cast<XPropWorld::xCommon*>( pBaseProp );
	}
	SET_ACCESSOR( xCampaign::CampObjPtr, spCampObj );	// 임시로 만듬.
	bool IsGuildRaid() const override;
	bool IsMedalCamp() const;
	// 영웅의전장인가.
	bool IsHeroCamp() const;
	XPropCamp::xProp* GetpPropCamp();
	xCampaign::CampObjPtr GetspCampObj( XGuild *pGuild );
#ifdef _SERVER
	xCampaign::CampObjPtr GetspCampObj( ID idGuild );
#endif // _SERVER
	virtual XGAME::xtBattle GetBattleType() { 
		if( IsGuildRaid() )
			return XGAME::xBT_GUILD_RAID; 
		return XGAME::xBT_NORMAL;
	}
	// 이 스팟이 캠페인타입이냐.
	bool IsCampaignType() const override {
		return IsGuildRaid() || IsMedalCamp() || IsHeroCamp();
	}
	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum ) const override;
	bool Update( XSPAcc spAcc ) override;
	void UpdateDropItems();
	xCampaign::CampObjPtr GetspCampObj() override {
		return m_spCampObj;
	}
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetName( XSPAcc spAcc ) override;
	void OnBeforeBattle( XSPAcc spAcc ) override;
//	bool IsPrivateRaid() const;
private:
}; // class XSpotCommon



