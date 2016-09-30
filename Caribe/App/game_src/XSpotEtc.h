#pragma once
#include "XSpot.h"
#include "XPropCamp.h"
#include "constGame.h"
////////////////////////////////////////////////////////////////
/**
 요일별 이벤트 스팟
*/

// class XSpotDaily : public XSpot
// {
// public:
// 	struct xDay {
// 		int m_numStar = 0;
// 		inline void Clear() {
// 			m_numStar = 0;
// 		}
// 	};
// 	static const int c_maxFloor;			// 최대 단계수
// 	static const int c_lvLegionStart;		// 시작 군단레벨
// 	enum {
// 		xNUM_ENTER=5,		// 입장 최대 횟수.
// 	};
// 	enum xtError {
// 		xERR_OK,
// 		xERR_TIMEOUT,			// 제한시간이 끝남
// 		xERR_OVER_ENTER_NUM,	// 입장횟수 초과
// 	};
// private:
// 	CTimer m_timerCreate;		// 스팟 생성 타이머
// 	int m_numEnter = 0;
// 	XE::xtDOW m_dowToday = XE::xDOW_MONDAY;		// 어느 요일꺼냐
// 	int m_idxFloor = 0;		// 난이도 단계 인덱스
// 	int m_cntWeekByFloor = 0;		// 현재 단계에서 몇주째가 흘렀는지. floor변경없이 한주가 지나면 ++된다. 다음단계로 넘어가면 0으로 초기화 된다.
// 	XVector<xDay> m_aryDay;
// 	int m_cntTouch = 0;		// 궁여지책으로 만든변수. ㅠㅠ
// 	void Init() {}
// 	void Destroy() {
// 	}
// 	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
// 	XSpotDaily( XWorld *pWorld, XGAME::xtSpot typeSpot ) 
// 		: XSpot( pWorld, typeSpot ) 
// 		, m_aryDay( XE::xDOW_MAX ) {
// 		Init();
// 	}
// public:
// 	XSpotDaily( XWorld *pWorld, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
// 		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) 
// 		, m_aryDay( XE::xDOW_MAX ) {
// 		Init();
// 	}
// 	// 요일을 넣는 버전
// 	XSpotDaily( XWorld *pWorld, XE::xtDOW dow, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
// 		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) 
// 		, m_aryDay( XE::xDOW_MAX ) {
// 		Init();
// 	}
// 	virtual ~XSpotDaily() { Destroy(); }
// 	//
// 	XPropWorld::xDaily* GetpProp() {
// 		return static_cast<XPropWorld::xDaily*>( GetpBaseProp() );
// 	}
// 	GET_ACCESSOR_CONST( const CTimer&, timerCreate );
// 	GET_SET_ACCESSOR_CONST( int, numEnter );
// 	GET_ACCESSOR_CONST( XE::xtDOW, dowToday );
// 	GET_ACCESSOR_CONST( int, idxFloor );
// 	void IncFloor() {
// 		++m_idxFloor;
// 		if( m_idxFloor >= c_maxFloor ) {
// 			m_idxFloor = c_maxFloor - 1;
// 		}
// 	}
// 	void DecFloor() {
// 		if( m_idxFloor > 0 )
// 			--m_idxFloor;
// 	}
// //	GET_ACCESSOR_CONST( int, Day );
// 	bool IsClearDay( XE::xtDOW dow ) const {
// 		return m_aryDay[ dow ].m_numStar >= 3;
// 	}
// 	int GetnumStar( XE::xtDOW dow ) const {
// 		return m_aryDay[ dow ].m_numStar;
// 	}
// 	int GetRemainEnter() const {
// 		return xNUM_ENTER - m_numEnter;
// 	}
// 	void AddnumEnter() {
// 		if( m_numEnter < xNUM_ENTER )
// 			++m_numEnter;
// 	}
// 	BOOL IsActive() const override {
// 		return TRUE;
// // 		return m_Type != XGAME::xDS_NONE;
// 	}
// 	// hPassHour이미 흘러간 시간
// 	void SetSpot( XE::xtDOW dow, int secPass, XSPAcc spAccount );
// 	//
// 	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
// 	void Serialize( XArchive& ar );
// private:
// 	BOOL DeSerialize( XArchive& ar, DWORD ver );
// public:
// 	//
// 	void Process( float dt );
// 	void CreateLegion( XSPAcc spAccount ) override;
// //	void ClearLegion();
// 	// 스팟을 해제시킨다.
// // 	void ReleaseSpot() {
// // 		m_Type = XGAME::xDS_NONE;	// 제한시간이 끝나 스팟이 사라짐
// // 	}
// // 	int GetLegionType() const {
// // 		return (m_Day % 3) + 1;
// // 	}
// // 	XGAME::xtAttack GetAtkType() const {
// // 		return (XGAME::xtAttack)(( m_Day % 3 ) + 1);
// // 	}
// 	/**
// 	 스팟에 입장했다.
// 	*/
// 	xtError DoEnter();
// 	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum = 1.f ) const override;
// 	bool IsNpc() const override {
// 		return true;
// 	}
// 	bool IsPC() const override {
// 		return false;
// 	}
// 	bool Update( XSPAcc spAcc ) override;
// 	bool IsAttackable( XSPAcc spAcc, xtError *pOut = nullptr );
// 	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat ) override;
// 	// 오늘 나와야할 유닛을 계산한다.
// 	XGAME::xtUnit GetUnitByDow( XE::xtDOW dow ) const;
// 	inline XGAME::xtUnit GetUnitByToday() const {
// 		return GetUnitByDow( GetdowToday() );
// 	}
// 	int GetlvLegionDow( XE::xtDOW dow ) const;
// 	int GetlvLegionCurrFloor() const;
// 	static bool sGetRewardDailyToday( XPropWorld::xDaily* pProp, int lvLegion, XVector<XGAME::xReward>* pOutAry );
// 	static bool sGetRewardDaily( XPropWorld::xDaily* pProp, XE::xtDOW dow, int lvLegion, XVector<XGAME::xReward>* pOutAry );
// private:
// 	void ResetLevel( XSPAcc spAcc ) override;
// 	void ResetPower( int lvSpot ) override;
// 	void ResetName( XSPAcc spAcc ) override;
// 	// 현재 층에서 나타나야할 유닛 크기
// 	XGAME::xtSize GetSizeUnitByFloor() const {
// 		const int maxSize = XGAME::xSIZE_MAX - 1;
// 		return ( XGAME::xtSize )( ( m_idxFloor % maxSize ) + 1 );
// 	}
// 	void OnTouch( XSPAcc spAcc ) override;
// }; // XSpotDaily

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
	bool IsPrivateRaid() const;
private:
}; // class XSpotCommon



