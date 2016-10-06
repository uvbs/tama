#pragma once
#include "XSpot.h"

////////////////////////////////////////////////////////////////
/**
 @brief NPC스팟은 용량을 줄이기 위해 군단생성 시점을 최대한 뒤로 늦춘다.
 다시말해 스팟이 스폰되더라도 플레이어가 정찰이나 공격을 하지 않는한.
 군단정보는 생성되지 않는다.
 npc스팟의 경우 퀘스트용 스팟인경우는 레벨이 있을 수 있다.
 그러므로 ?를 표시하는것은 레벨과 이름을 따로두고 군단정보가 없을때는(색을 알수 없음)
 ?로 표시한다.
*/
class XSpotNpc : public XSpot
{
public:
private:
	//
	XGAME::xtResource m_typeResource;	// 보유하고 있는 자원
	int m_numResource;					// 자원의 양
	void Init() {
		m_typeResource = XGAME::xRES_WOOD;
		m_numResource = 0;
	}
	void Destroy() {}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotNpc( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotNpc( XWorld *pWorld, XPropWorld::xNPC* pProp, XDelegateSpot *pDelegate ) 
		: XSpot( pProp, XGAME::xSPOT_NPC, pWorld, pDelegate ) { 
		Init();
		// 시간이 지나면 스폰이 일어날수 있도록 한다.
		if( pProp )
			ResetSpawn();
	}
	virtual ~XSpotNpc() { Destroy(); }
	//
	XPropWorld::xNPC* GetpProp(){
		return static_cast<XPropWorld::xNPC*>( GetpBaseProp() );
	}
	GET_ACCESSOR( XGAME::xtResource, typeResource );
	GET_ACCESSOR( int, numResource );
// 	XGAME::xtClan GetClan() {
// 		return GetpProp()->clan;
// 	}
	//
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
	//
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
	bool IsQuestion() const override {
		return GetLevel() == 0;
	}
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc ) override;
	void ResetSpawn( void );
	void CreateLegion( XSPAcc spAccount ) override;
//	void ClearLegionAndSetRespawn( void );
	BOOL IsActive( void ) { 
		return GettimerSpawn().IsOff() == TRUE;	// 이제?상태는 스팟레벨이 0이므로 타이머로 검사함.
	}
	void ResetSpot() override {
		ResetSpawn();
	}
	void OnSpawn( XSPAcc spAcc ) override;
	void GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const override {
		if( m_numResource ) {
			XGAME::xRES_NUM res( m_typeResource, m_numResource );
			pAry->Add( res );
		}
	}
// 	int GetNeedAP() override {
// 		float gold = 0;
// 		auto typeRes = m_typeResource;
// 		if( typeRes == XGAME::xRES_IRON )
// 			typeRes = XGAME::xRES_WOOD;
// 		gold += m_numResource * XGC->m_resourceRate[ m_typeResource ];
// 		int ap = (int)( (log10( gold / 5000.f ) * 45.f)/* * 0.5f*/ );
// 		if( ap <= 0 )
// 			ap = 1;
// 		return ap;
// 	}
	// 레벨을 다시 잡는다.
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetPower( int lvSpot ) override;
	void ResetName( XSPAcc spAcc ) override;
	void DoSpawnSpot() override;
	bool IsElite() override;
	int GetDropableList( std::vector<ID> *pOutAry );
	void ClearSpot() override {
		XSpot::ClearSpot();
		m_typeResource = XGAME::xRES_WOOD;
		m_numResource = 0;
	}
// 	float GetsecRemainRegen() override { 
// 		return GettimerSpawn().GetsecRemainTime();
// 	}
	bool Update( XSPAcc spAcc ) override;
private:
	void ResetResource( int lvSpot, XSPAcc spAcc );
//	XLegion* CreateLegion( XPropWorld::xNPC *pProp, int lvSpot ) const;
	XSPLegion CreateLegion( XPropWorld::xNPC *pProp, int lvSpot );
	BOOL DeSerialize( XArchive& ar, DWORD ver );
}; // spotnpc
