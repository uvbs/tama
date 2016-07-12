#pragma once
#include "XSpot.h"
#include "XPropWorld.h"
//#include "XPropCamp.h"
#include "XArchive.h"
#include "etc/XTimer2.h"
//#include "XLegion.h"
//#include "XCampObj.h"
#include "XPropCloud.h"
#include "XSpotH.h"

////////////////////////////////////////////////////////////////
class XSpotMandrake : public XSpot
{
	ID m_idOwner;		// 현재 점유자 아이디
	XSpot::xLOCAL_STORAGE m_lsLocal;		// 지역창고
	XTimer2 m_timerOwn;		// 소유타이머. 자원지를 소유한 순간부터 작동되기 시작해서 뺏길때까지 계속 흘러간다.
	XTimer2 m_timerCalc;	// 정산타이머. 자원지를 소유하고 정산하고나면 리셋된다. 클라이언트에서는 리젠타이머로 사용한다.
	ID m_idMatchEnemy = 0;	// 현재 상대(만드렉은 내가 이겼을때 기억해야할 상대가 없기때문에 사용안함)
	int m_idxLegion;	// 수비중인 m_idOwner의 군단번호
	int m_Win;		// 현재 연승수
	int m_Reward;	// 현재 연승 보상.
	xSec m_secLastEvent;	// 매치가되었거나 마지막으로 전투가일어난 시간
	int m_OffWin = 0;
	std::string m_strcFbUserId;
	_tstring m_strHello;
	void Init() {
		m_idOwner = 0;
//		m_loot = 0;
		m_idxLegion = 0;
		m_secLastEvent = 0;
		m_Win = 0;
		m_Reward = 0;
		m_OffWin = 0;
	}
	void Destroy();
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotMandrake( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {
		Init();
	}
public:
	XSpotMandrake( XWorld *pWorld, XPropWorld::xMANDRAKE *pProp );
	virtual ~XSpotMandrake() { Destroy(); }
	//
  XPropWorld::xMANDRAKE* GetpProp() {
    return static_cast<XPropWorld::xMANDRAKE*>( GetpBaseProp() );
  }
//	GET_ACCESSOR_CONST( ID, idOwner );
	ID GetidOwner() const override {
		return m_idOwner;
	}
	GET_SET_ACCESSOR_CONST( ID, idMatchEnemy );
	GET_SET_ACCESSOR_CONST( int, idxLegion );
	GET_ACCESSOR_CONST( xSec, secLastEvent );
	GET_ACCESSOR( const XSpot::xLOCAL_STORAGE&, lsLocal );
	GET_ACCESSOR_CONST( int, OffWin );
	GET_SET_ACCESSOR_CONST( const std::string&, strcFbUserId );
//	GET_SET_ACCESSOR_CONST( const _tstring&, strHello );
	_tstring GetstrHello() const override {
		return m_strHello;
	}
	void SetstrHello( const _tstring& strHello ) override {
		m_strHello = strHello;
	}
//	GET_SET_ACCESSOR( int, loot );
	int Getloot() const {
		return m_Reward;
	}
	GET_SET_ACCESSOR_CONST( int, Reward );
	GET_SET_ACCESSOR_CONST( int, Win);
// 	GET_SET_ACCESSOR( int, Score );
	//
	void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xMANDRAKE*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
public:
	bool IsNpc() const override {
		return false;			// 만드레이크는 npc상태가 없음.
	}
	bool IsPC() const override {
		return !IsQuestion();
	}
	void DoStackToLocal( float num, bool bFullCheck ) override {
		m_lsLocal.Add( num, bFullCheck );
	}
//	void SetPlayerOwner( ID idAcc, LPCTSTR szName, ID idEnemy, int level, int win, int reward, int score );
	void SetPlayerOwner( ID idAcc, LPCTSTR szName );
	void CalculateResource( ID idAcc, xSec secAdjustOffline );
	void CalculateResourceByPassTime( ID idAcc, xSec secPassTime, xSec secAdjustOffline );
	void Process( float dt );
	// 지역창고에 자원이 쌓여있는가.
// 	BOOL IsHaveLocalStorage( void ) const override {
// 		if( m_lsLocal.numCurr )
// 			return TRUE;
// 		return FALSE;
// 	}
	// 지역창고에 쌓여있던 자원을 계정창고로 옮긴다.
	void MoveResourceLocalToMain( XSPAcc spAccount, XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry = nullptr ) override;
	void ClearLocalStorage() override {
		m_lsLocal.Clear();
	}
	// 지역창고에 자원을 rateLoss율 만큼 소실시킨다.
	void LossLocalStorage( float rateLoss, std::vector<int> *pOutAry ) override {
		auto loss = m_lsLocal.Loss( rateLoss );
		if( pOutAry )
			( *pOutAry )[ XGAME::xRES_MANDRAKE ] += (int)loss;
	}
	// 지역창고에 쌓여있는 자원양을 돌려준다. 단 유저스팟은 목재/철 두가지 이므로 이것으로 알수 없다.
	float GetLocalStorageAmount( XGAME::xtResource typeRes = XGAME::xRES_NONE ) const override {
		return m_lsLocal.numCurr;
	}
	float GetLocalStoragePercent( void ) const override {
		return (float)m_lsLocal.numCurr / m_lsLocal.maxSize;
	}
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc ) override;
	void AddWin( int add ) {
		m_Win += add;
		XBREAK( m_Win < 0 || m_Win > 100 );
	}
	void GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const override {
		XGAME::xRES_NUM res( XGAME::xRES_MANDRAKE, /*m_loot*/m_Reward );
		pAry->Add( res );
	}
	void Initialize( XSPAcc spAcc ) override;
	bool IsQuestion() const {
		return ( m_idOwner == 0  );
	}
// 	int GetNeedAP() override {
// 		float gold = 0;
// 		gold += m_Reward * XGC->m_resourceRate[ XGAME::xRES_MANDRAKE ];
// 		int ap = (int)( log10( gold / 5000.f ) * 45.f );
// 		if( ap <= 0 )
// 			ap = 1;
// 		return ap;
// 	}
	void AddLocalStorageResources( xResourceAry *pOutAry ) override {
		(*pOutAry)[ XGAME::xRES_MANDRAKE ] += (int)m_lsLocal.numCurr;
	}
// 	void GetLocalStorageResources( XGAME::xRES_NUM *pOut ) override {
// 		pOut->type = XGAME::xRES_MANDRAKE;
// 		pOut->num = m_lsLocal.numCurr;
// 	}
	void OnSpawn( XSPAcc spAcc );
	bool IsEnemy() const override;
// 	ID GetidEnemy() override {
// 		return m_idOwner;
// 	}
	xSpot::xtStateMandrake GetState( ID idAccPlayer );
	void OnOpenedArea( XPropCloud::xCloud* pPropArea ) override;
	void ClearSpot() override {
		XSpot::ClearSpot();
		m_idOwner = 0;
		m_lsLocal.Clear();
		m_timerOwn.Off();
		m_timerCalc.Off();
//		m_idMatchEnemy = 0;
		m_idxLegion = 0;
		m_Win = 0;
		m_Reward = 0;
		m_secLastEvent = 0;
		m_OffWin = 0;
		m_strHello.clear();
		m_strcFbUserId.clear();
	}
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
private:
	void SetMaxLocal( XPropWorld::xMANDRAKE *pProp );
friend class CUserDB;
}; // spotmandrake
