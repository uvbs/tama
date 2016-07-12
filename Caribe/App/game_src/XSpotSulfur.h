#pragma once
#include "XSpot.h"
#include "XPropWorld.h"
//#include "XPropCamp.h"
#include "XArchive.h"
#include "etc/XTimer2.h"
//#include "XLegion.h"
//#include "XCampObj.h"
#include "XPropCloud.h"

class XLegion;
////////////////////////////////////////////////////////////////
/**
 @brief 유황스팟은 용량을 줄이기 위해 군단생성 시점을 최대한 뒤로 늦춘다.
 다시말해 스팟이 스폰되더라도 플레이어가 정찰이나 공격을 하지 않는한.
 군단정보는 생성되지 않는다.
*/
class XSpotSulfur : public XSpot
{
public:
private:
	XSpot::xLOCAL_STORAGE m_lsLocal;		// 지역창고
	ID m_idEncounterUser = 0;		// 현재 유저와 인카운터 전 중이라면 상대id
//	bool m_bEncounter = false;		// 인카운터가 발생했음. DB에서 매치상대가 돌아오면 m_idEncounter가 메워진다.
	XVector<XGAME::xEncounter> m_aryEncounter;		// 인카운터 기록.
	bool m_bCheckEncounterLog = false;			// 인카운터 로그를 확인했는지.
	void Init() {}
	void Destroy();
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotSulfur( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotSulfur( XWorld *pWorld, XPropWorld::xSULFUR* pProp, XDelegateSpot *pDelegate=nullptr );
	virtual ~XSpotSulfur() { Destroy(); }
	//
	XPropWorld::xSULFUR* GetpProp() {
		return static_cast<XPropWorld::xSULFUR*>( GetpBaseProp() );
	}
	GET_ACCESSOR( const XSpot::xLOCAL_STORAGE&, lsLocal );
//	GET_SET_ACCESSOR( float, numSulfur );
	int GetnumSulfur() {
		return GetLootAmount( XGAME::xRES_SULFUR );
	}
	void SetnumSulfur( int numSulfur ) {
		SetLootRes( XGAME::xRES_SULFUR, numSulfur );
	}
	bool IsHaveLog() {
		return m_aryEncounter.size() != 0;
	}
	void AddEncounter( const XGAME::xEncounter& enc ) {
		if( m_aryEncounter.size() < 8 )		// 용량땜에 개수제한둠.
			m_aryEncounter.Add( enc );
	}
	void ClearEncounter() {
		m_aryEncounter.Clear();
	}
	const XVector<XGAME::xEncounter>& GetaryEncounter() {
		return m_aryEncounter;
	}
	GET_SET_ACCESSOR( bool, bCheckEncounterLog );
	GET_SET_ACCESSOR( ID, idEncounterUser );
//	GET_SET_ACCESSOR( bool, bEncounter );
	void SetEncounterName( LPCTSTR szName ) {
		SetstrName( szName );
	}
	LPCTSTR GetEncounterName() {
		return GetstrName().c_str();
	}
	bool IsEncounter() {
		return m_idEncounterUser != 0;
	}
	// 현재 대전중인 상대가 npc인가
	bool IsVsNPC() const override {
		return m_idEncounterUser == 0;
	}
	// 유황스팟은 언제나 npc
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
	void ResetSpawn( void );
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetPower( int lvSpot ) override;
	void ResetName( XSPAcc spAcc ) override;
//	void CreateLegion( XSPAcc spAccount );
//	void ClearLegionAndSetRespawn( void );
	void DoStackToLocal( float num, bool bFullCheck ) override {
		m_lsLocal.Add( num, bFullCheck );
	}
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xSULFUR*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
	// 지역창고에 자원이 쌓여있는가.
	BOOL IsHaveLocalStorage() { 
		if( m_lsLocal.numCurr )
			return TRUE;
		return FALSE;
	}
	// 지역창고에 쌓여있던 자원을 계정창고로 옮긴다.
	void MoveResourceLocalToMain( XSPAcc spAccount, XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry = nullptr ) override;
	void ClearLocalStorage() override {
		m_lsLocal.Clear();
	}
	// 지역창고에 자원을 rateLoss율 만큼 소실시킨다.
	void LossLocalStorage( float rateLoss, std::vector<int> *pOutAry ) override {
		auto loss = m_lsLocal.Loss( rateLoss );
		if( pOutAry )
			( *pOutAry )[ XGAME::xRES_SULFUR ] += (int)loss;
	}
	// 지역창고에 쌓여있는 자원양을 돌려준다. 단 유저스팟은 목재/철 두가지 이므로 이것으로 알수 없다.
	float GetLocalStorageAmount( XGAME::xtResource typeRes = XGAME::xRES_NONE ) const override { 
		return m_lsLocal.numCurr; 
	}
	float GetLocalStoragePercent( void ) const override {
		return (float)m_lsLocal.numCurr / m_lsLocal.maxSize;
	}
	BOOL IsActive( void ) const override {
		return GettimerSpawn().IsOff() == TRUE;	// 이제?상태는 스팟레벨이 0이므로 타이머로 검사함.
	}
	void OnSpawn( XSPAcc spAcc ) override;
	void ClearSpot() override;
	void ResetSpot() override {
//		if( m_bEncounter == false ) {dszfsadf
			GettimerSpawnMutable().Off();
			ResetSpawn();
//		}
	}
	void DoSpawnSpot() override;
	void GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const override {
// 		XGAME::xRES_NUM res( XGAME::xRES_SULFUR, m_numSulfur );
// 		pAry->Add( res );
		auto num = GetLootAmount( XGAME::xRES_SULFUR );
		XGAME::xRES_NUM res( XGAME::xRES_SULFUR, num );
		pAry->Add( res );
	}
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc ) override;
	bool IsQuestion() const override { 
		return GetLevel() == 0; 
	}
	void AddLocalStorageResources( xResourceAry *pOutAry ) override {
		( *pOutAry )[ XGAME::xRES_SULFUR ] += (int)m_lsLocal.numCurr;
	}
	void DoDropRegisterRes( XSPAcc spAcc ) override;
	void CreateLegion( XSPAcc spAcc ) override;
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
private:
	XLegion* CreateLegion( XPropWorld::xBASESPOT *pProp, int lvSpot ) const;
	void Process( float dt ) override;
friend class CUserDB;
}; // XSpotSulfur
