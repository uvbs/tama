#pragma once
#include "XSpot.h"
#include "XPropWorld.h"
//#include "XPropCamp.h"
#include "XArchive.h"
#include "etc/XTimer2.h"
//#include "XLegion.h"
//#include "XCampObj.h"
#include "XPropCloud.h"

////////////////////////////////////////////////////////////////
// 보석광산 인스턴스
class XSpotJewel : public XSpot
{
public:
private:
	ID m_idOwner;		// 현재 점유자 아이디
	XTimer2 m_timerOwn;	// 소유타이머. 자원지를 소유한 순간부터 작동되기 시작해서 뺏길때까지 계속 흘러간다.
	XTimer2 m_timerCalc;	// 정산타이머. 자원지를 소유하고 정산하고나면 리셋된다. 클라이언트에서는 리젠타이머로 사용한다.
//#ifdef _DEV
	_tstring m_strNameByMatchEnemyForSimul;		// 매치된 상대의 이름.(광산침공시뮬용. 정상플레이에선 이긴상대방측에서 내 DB에 자기 이름을 써넣기 때문에 이게 필요가 없다.)
	int m_lvMatchEnemyForSimul = 0;		// 매치된 상대의 레벨(광산침공시뮬용)
//#endif // _DEV
	//////////////////////////////////////////////////////////////////////////
	// 계정 DB에 따로 저장되는 부분
	int m_levelMine;	// 거점 레벨
	int m_Defense;	// 현재 방어도 포인트
	ID m_idMatchEnemy;	// 현재 상대(내가 현재 점유중이라도 짝지어진 상대의 아이디)
	int m_idxLegion;	// 수비중인 m_idOwner의 군단번호
	double m_secLastEvent;	// 매치가되었거나 마지막으로 전투가일어난 시간
	XSpot::xLOCAL_STORAGE m_lsLocal;		// 지역창고
	std::string m_strcFbUserId;
	_tstring m_strHello;
	//
	//////////////////////////////////////////////////////////////////////////

	void Init() {
		m_idOwner = 0;
		m_levelMine = 1;
		m_Defense = 0;
		m_idMatchEnemy = 0;
		m_idxLegion = 0;
		m_secLastEvent = 0;
//		m_lootJewel = 0;
	}
	void Destroy();
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotJewel( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) { Init(); }
public:
	XSpotJewel( XWorld *pWorld, XPropWorld::xJEWEL *pProp );
	virtual ~XSpotJewel() { Destroy(); }
	//
//	GET_ACCESSOR( XPropWorld::xJEWEL*, pProp );
//	GET_ACCESSOR( ID, idOwner );
	ID GetidOwner() const override {
		return m_idOwner;
	}
	GET_ACCESSOR( int, levelMine );
	GET_ACCESSOR( int, Defense );
	GET_SET_ACCESSOR( ID, idMatchEnemy );
	GET_SET_ACCESSOR( int, idxLegion );
	GET_ACCESSOR( double, secLastEvent );
	GET_ACCESSOR( const XSpot::xLOCAL_STORAGE&, lsLocal );
//	GET_SET_ACCESSOR( int, lootJewel );
	float GetBonusHPRateByDefense() const {
		return 1.f + (m_Defense * 0.1f);
	}
	void SetlootJewel( int num ) {
		SetLootRes( XGAME::xRES_JEWEL, num );
	}
	int GetlootJewel() const {
		return GetLootAmount( XGAME::xRES_JEWEL );
	}
	GET_SET_ACCESSOR_CONST( const std::string&, strcFbUserId );
//	GET_SET_ACCESSOR_CONST( const _tstring&, strHello );
	_tstring GetstrHello() const override {
		return m_strHello;
	}
	void SetstrHello( const _tstring& strHello ) override {
		m_strHello = strHello;
	}
//#ifdef _DEV
	GET_SET_ACCESSOR_CONST( const _tstring&, strNameByMatchEnemyForSimul );
	GET_SET_ACCESSOR_CONST( int, lvMatchEnemyForSimul );
	bool IsMatchEnemyIsDummy() const {
//		return m_strNameByMatchEnemyForSimul == _T( "TEST_USER" );
		return !m_strNameByMatchEnemyForSimul.empty();
	}
//#endif // _DEV
	bool IsMySpot( ID idAccMy ) {
		return m_idOwner == idAccMy;
	}
	//
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
	void SerializeForBattle( XArchive* pOut, const XParamObj2& param ) override;
	void DeSerializeForBattle( XArchive& arLegion, XArchive& arAdd, XSPAcc spAcc ) override;
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
public:
	void Process( float dt );
	//
  XPropWorld::xJEWEL* GetpProp() {
    return static_cast<XPropWorld::xJEWEL*>( GetpBaseProp() );
  }
	void SetPlayerOwner( ID idAcc, LPCTSTR szName );
	void CalculateResource( ID idAcc, xSec secAdjustOffline );
	void CalculateResourceByPassTime( ID idAcc, xSec secPassTime, xSec secAdjustOffline );
	void OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pUser, ID idAcc, xSec secLastCalc ) override;
	int AddLevelMine( int add ) {
		m_levelMine += add;
		return m_levelMine;
	}
	// 방어도 포인트를 리셋시킨다.
	int ResetDefense( int levelOwner ) {
//		m_Defense = levelOwner * 5000 + m_levelMine * 500;
//		m_Defense = levelOwner + m_levelMine;	// 계산의 편의를 위해 식을 단순화 시킴.
		XBREAK( m_levelMine == 0 );
		m_Defense = m_levelMine;		// 단순히 광산 레벨로 해도 충분.
		return m_Defense;
	}
	void ClearLevelMine() {
		m_levelMine = 1;
		ResetDefense( GetidOwner() );
	}
	// 스팟내 정보들을 매칭된 결과로 다시 세팅한다. 이 스팟의 주인은 이미 idAccOwner상태여야 한다.
	void SetMatch( const XGAME::xJewelMatchEnemy& infoMatch  );
	int AddDefense( int add ) {
		m_Defense += add;
		if( m_Defense < 0 )
			m_Defense = 0;
		return m_Defense;
	}
	void DoStackToLocal( float num, bool bFullCheck ) override {
		m_lsLocal.Add( num, bFullCheck );
	}
	virtual void MoveResourceLocalToMain( XSPAcc spAccount, XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry=nullptr );
	virtual void ClearLocalStorage() {
		m_lsLocal.Clear();
	}
	virtual _tstring GetStrLocalStorage( void ) {
		_tstring str;
		str = XE::Format(_T("%d/%d"), m_lsLocal.numCurr, m_lsLocal.maxSize );
		return str;
	}
//   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
//     m_pProp = static_cast<XPropWorld::xJEWEL*>( pBaseProp );
//     XBREAK( m_pProp == nullptr );
//   }
	virtual BOOL IsHaveLocalStorage( void ) {
		if( m_lsLocal.numCurr > 0 )
			return TRUE;
		return FALSE;
	}
	virtual void AdjustTimer( xSec secAdjust ) {
		XSpot::AdjustTimer( secAdjust );
		m_timerCalc.AddTime( secAdjust );
	}
	void LossLocalStorage( float rateLoss, std::vector<int> *pOutAry ) override {
		auto loss = m_lsLocal.Loss( rateLoss );
		if( pOutAry )
			(*pOutAry)[ XGAME::xRES_JEWEL ] += (int)loss;
	}
	float GetLocalStorageAmount( XGAME::xtResource typeRes = XGAME::xRES_NONE ) const override { 
		return m_lsLocal.numCurr;
	}
	float GetLocalStoragePercent( void ) const override {
		return (float)m_lsLocal.numCurr / m_lsLocal.maxSize;
	}

	bool IsQuestion() const override {
		return (m_idOwner == 0 && GetstrName().empty() );
	}
	void GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const override {
		XGAME::xRES_NUM res( XGAME::xRES_JEWEL, GetlootJewel() );
		pAry->Add( res );
	}
	bool IsNpc() const override {
		return m_idOwner == 0 && !GetstrName().empty();
	}
	bool IsPC() const override {
		return !IsNpc() && !IsQuestion();
	}
// 	int GetNeedAP() override {
// 		float gold = 0;
// 		gold += m_lootJewel * XGC->m_resourceRate[ XGAME::xRES_JEWEL ];
// 		int ap = (int)( log10( gold / 5000.f ) * 45.f );
// 		if( ap <= 0 )
// 			ap = 1;
// 		return ap;
// 	}
	/// 점령시간을 얻는다. 내소유가 아니라면 0이 리턴된다.
	int GetsecCaptured( ID idAcc ) {
		if( m_idOwner != idAcc )
			return 0;
		return (int)m_timerOwn.GetsecPassTime();
	}
	void AddLocalStorageResources( xResourceAry *pOutAry ) override {
		(*pOutAry)[ XGAME::xRES_JEWEL ] += (int)m_lsLocal.numCurr;
	}
// 	void GetLocalStorageResources( XGAME::xRES_NUM *pOut ) override {
// 		pOut->type = XGAME::xRES_JEWEL;
// 		pOut->num = m_lsLocal.numCurr;
// 	}
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
	void OnSpawn( XSPAcc spAcc ) override;
	bool IsEnemy() const override;
// 	ID GetidEnemy() override {
// //		return m_idMatchEnemy;
// 		return m_idOwner;		// 
// 	}
	void DoDefenseDamage();
	int GetidxJewel() {
		return GetpProp()->idx;
	}
// 	void SerializeUpdateInfo( XArchive& ar );
	void GetJewelInfo( XGAME::xJewelInfo* pOut );
	void ChangeOwnerByWinForDummy();
	void ClearMatchEnemy() {
		m_idMatchEnemy = 0;
//#ifdef _DEV
		m_strNameByMatchEnemyForSimul.clear();
		m_lvMatchEnemyForSimul = 0;
//#endif // _DEV
	}
// #ifdef _DEV
// 		void MakeSimulSpot( XSPAcc spAcc );
// #endif // _DEV
		void ClearSpot() override;
private:
	void SetMaxLocal( XPropWorld::xJEWEL *pProp );
friend class CUserDB;
}; // spotjewel
