#pragma once
#include "XSpot.h"
#include "XPropCamp.h"
////////////////////////////////////////////////////////////////
/**
 요일별 이벤트 스팟
*/
class XSpotDaily : public XSpot
{
public:
	enum {
		xNUM_ENTER=5,		// 입장 최대 횟수.
	};
	enum xtError {
		xERR_OK,
		xERR_TIMEOUT,			// 제한시간이 끝남
		xERR_OVER_ENTER_NUM,	// 입장횟수 초과
	};
	static XGAME::xtDailySpot sGetDowToType( XE::xtDOW dow ) {
		if( dow >= XE::xDOW_MONDAY && dow <= XE::xDOW_FRIDAY )
			return XGAME::xDS_SCALP;			// 징표드랍
		else if( dow == XE::xDOW_SATURDAY )
			return XGAME::xDS_SKILL;			// 스킬템 드랍
		else if( dow == XE::xDOW_SUNDAY )
			return XGAME::xDS_GENERAL;		// 책사장군 드랍
		XBREAKF(1,"undown day of week:%d", (int)dow );
		return XGAME::xDS_NONE;
	}
private:
//	XPropWorld::xDaily *m_pProp = nullptr;
	CTimer m_timerCreate;		// 스팟 생성 타이머
//	XTimer2 m_timerEnter;		// 입장 타이머(시간지나면 사라지는 요소 일단 삭제.입장횟수가 있으므로 시간은 필요없을듯하다)
	XGAME::xtDailySpot m_Type = XGAME::xDS_NONE;	// 스팟 타입.
	int m_numEnter = 0;
	XE::xtDOW m_dowToday = XE::xDOW_MONDAY;		// 어느 요일꺼냐
	int m_Day = 0;				// 소/중/대 번갈아가며 나오게 하기위해 오늘 일수를 저장.
	void Init() {}
	void Destroy() {
	}
	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
	XSpotDaily( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
public:
	XSpotDaily( XWorld *pWorld, XGAME::xtDailySpot type, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) {
		Init();
		m_Type = type;
	}
	// 요일을 넣는 버전
	XSpotDaily( XWorld *pWorld, XE::xtDOW dow, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) {
		Init();
		m_Type = XSpotDaily::sGetDowToType( dow );
	}
	XSpotDaily( XWorld *pWorld, XPropWorld::xDaily* pProp, XDelegateSpot *pDelegate )
		: XSpot( pProp, XGAME::xSPOT_DAILY, pWorld, pDelegate ) {
		Init();
	}
	virtual ~XSpotDaily() { Destroy(); }
	//
  XPropWorld::xDaily* GetpProp() {
    return static_cast<XPropWorld::xDaily*>( GetpBaseProp() );
  }
//	GET_ACCESSOR( XPropWorld::xDaily*, pProp );
	GET_ACCESSOR( XGAME::xtDailySpot, Type );
	GET_ACCESSOR( const CTimer&, timerCreate );
//	GET_ACCESSOR( const XTimer2&, timerEnter );
	GET_SET_ACCESSOR( int, numEnter );
	GET_ACCESSOR( XE::xtDOW, dowToday );
	GET_ACCESSOR( int, Day );
	int GetRemainEnter( void ) {
		return xNUM_ENTER - m_numEnter;
	}
	void AddnumEnter() {
		if( m_numEnter < xNUM_ENTER )
			++m_numEnter;
	}
	BOOL IsActive( void ) {
		return m_Type != XGAME::xDS_NONE;
	}
	// hPassHour이미 흘러간 시간
	void SetSpot( XE::xtDOW dow, int secPass, XSPAcc spAccount );
	//
	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
	void Serialize( XArchive& ar );
private:
	BOOL DeSerialize( XArchive& ar, DWORD ver );
public:
	//
	void Process( float dt );
	void CreateLegion( XSPAcc spAccount ) override;
//	void ClearLegion( void );
	// 스팟을 해제시킨다.
	void ReleaseSpot( void ) {
		m_Type = XGAME::xDS_NONE;	// 제한시간이 끝나 스팟이 사라짐
	}
	int GetLegionType() {
		return (m_Day % 3) + 1;
	}
	XGAME::xtAttack GetAtkType() {
		return (XGAME::xtAttack)(( m_Day % 3 ) + 1);
	}
	/**
	 스팟에 입장했다.
	*/
	xtError DoEnter();
	int DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum = 1.f ) const override;
	bool IsNpc() const override {
		return true;
	}
	bool IsPC() const override {
		return false;
	}
	bool Update( XSPAcc spAcc ) override;
	bool IsAttackable( XSPAcc spAcc, xtError *pOut = nullptr );
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat ) override;
private:
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetPower( int lvSpot ) override;
	void ResetName( XSPAcc spAcc ) override;
}; // XSpotDaily

////////////////////////////////////////////////////////////////
// /**
//  비정기 스페셜 스팟
// */
// class XSpotSpecial : public XSpot
// {
// public:
// 	enum {
// 		xNUM_ENTER=5,		// 입장 최대 횟수.
// 		xRECHARGING_TIME=20,	// 입장 횟수가 채워지는 시간(분)
// 	};
// 	enum xtError {
// 		xERR_OK,
// 		xERR_TIMEOUT,			// 제한시간이 끝남
// 		xERR_OVER_ENTER_NUM,	// 입장횟수 초과
// 	};
// private:
// //	XPropWorld::xSpecial *m_pProp;
// 	XGAME::xtSpecialSpot m_Type;	// 스팟 타입.
// //	CTimer m_timerCreate;		// 스팟 생성 타이머
// 	CTimer m_timerSec;
// 	xSec m_secCreate = 0;			// 스팟 생성시간(초)
// 	xSec m_secReleased = 0;			// 스팟 사라진 시간.
// 	CTimer m_timerRecharge;		// 입장횟수 재생성 타이머
// 	int m_numEnter;				// 입장한 총 횟수
// 	int m_numEnterTicket;		// 입장할수 있는 횟수
// 	int m_idxRound;				// 현재 라운드.
// 	void Init() {
// 		m_numEnter = 0;
// 		m_numEnterTicket = 0;
// 		m_Type = XGAME::xSS_NONE;
// 		m_idxRound = 0;
// 	}
// 	void Destroy() {
// 	}
// 	friend XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld );
// 	XSpotSpecial( XWorld *pWorld, XGAME::xtSpot typeSpot ) : XSpot( pWorld, typeSpot ) {}
// public:
// 	XSpotSpecial( XWorld *pWorld, XPropWorld::xSpecial* pProp, XDelegateSpot *pDelegate )
// 		: XSpot( pProp, XGAME::xSPOT_SPECIAL, pWorld, pDelegate ) {
// 		Init();
// 	}
// 	XSpotSpecial( XWorld *pWorld, XGAME::xtSpecialSpot type, XPropWorld::xSpecial* pProp, XDelegateSpot *pDelegate )
// 		: XSpot( pProp, XGAME::xSPOT_SPECIAL, pWorld, pDelegate ) {
// 		Init();
// 		m_Type = type;
// 	}
// 	virtual ~XSpotSpecial() { Destroy(); }
// 	//
//   XPropWorld::xSpecial* GetpProp() {
//     return static_cast<XPropWorld::xSpecial*>( GetpBaseProp() );
//   }
// 	GET_ACCESSOR( XGAME::xtSpecialSpot, Type );
// 	GET_ACCESSOR( const CTimer&, timerRecharge );
// 	GET_ACCESSOR( int, numEnter );
// 	GET_SET_ACCESSOR( int, numEnterTicket );
// 	GET_ACCESSOR( int, idxRound );
// 	BOOL IsActive( void ) {
// 		return (m_Type != XGAME::xSS_NONE);
// 	}
// 	// 남은 입장 횟수
// 	int GetRemainEnter( void ) {
// 		return m_numEnterTicket;
// 	}
// 	// 
// 	// 스팟을 활성화 시킨다. secPass:이미 흘러간 시간
// 	void SetSpot( int levPlayer, int secPass, XSPAcc spAccount ) {
// 		if( levPlayer < 30 ) {
// 			m_Type = XGAME::xSS_LOW;
// 			SetstrName(_T("증오의 군단"));
// 		}
// 		else {
// 			m_Type = XGAME::xSS_HIGH;
// 			SetstrName(_T("파멸의 군단"));
// 		}
// //		m_timerCreate.Set( (float)(24*60*60) );		// 24시간을 세팅
// 		m_secCreate = XTimer2::sGetTime();			// 스팟 생성한 시간을 기록한다.
// 		m_secReleased = 0;
// //		m_timerCreate.SetPassSec( (float)secPass );	// 흘러간 시간을 더함.
// 		m_numEnterTicket = xNUM_ENTER;
// 		// 군단 생성
// //		CreateLegion( spAccount );
// 	}
// 	//
// 	virtual void OnCreateNewOnServer( XSPAcc spAcc ) override;
// 	void Serialize( XArchive& ar );
// private:
// 	BOOL DeSerialize( XArchive& ar, DWORD ver );
// public:
// 	//
// 	void Process( float dt );
// 	void CreateLegion( XSPAcc spAccount ) override;
// //	void ClearLegion( void );
// 	// 스팟을 해제(지도상에서 없앰)시킨다.
// 	void ReleaseSpot( void ) {
// 		m_Type = XGAME::xSS_NONE;
// 	}
// 	void NextRound( XSPAcc spAccount ) {
// //		ResetSpot( spAccount );
// 		if( ++m_idxRound >= 5) {
// 			m_idxRound = 0;
// 		}
// 	}
// // 	void ClearSpot() override {
// // 		DestroyLegion();
// // 	}
// 	/**
// 	 스팟에 입장했다.
// 	*/
// 	xtError DoEnter( void ) {
// 		// 제한시간이 초과되어 입장이 실패
// // 		if( m_timerCreate.IsOver() )
// // 			return xERR_TIMEOUT;
// 		xSec secCurr = XTimer2::sGetTime();
// 		// 스팟생성한지 24시간이 지났으면 에러
// 		if( secCurr - m_secCreate > 24 * 60 * 60 )
// 			return xERR_TIMEOUT;
// 		if( m_numEnterTicket <= 0 )
// 			return xERR_OVER_ENTER_NUM;
// 		++m_numEnter;
// 		--m_numEnterTicket;
// 		if( m_timerRecharge.IsOff() )
// 			m_timerRecharge.Set( xMIN( 20 ) );
// 		return xERR_OK;
// 	}
// 	bool IsNpc() const override {
// 		return true;
// 	}
// 	bool IsPC() const override {
// 		return false;
// 	}
// 	bool Update( XSPAcc spAcc ) override;
// // 	int GetNeedAP() override {
// // 		int ap = (int)( log10( GetLevel() ) * 50.f );
// // 		if( ap <= 0 )
// // 			ap = 1;
// // 		return ap;
// // 	}
// private:
// //   void ResetProp( XPropWorld::xBASESPOT *pBaseProp ) override {
// //     m_pProp = static_cast<XPropWorld::xSpecial*>( pBaseProp );
// //     XBREAK( m_pProp == nullptr );
// //   }
// }; // spotSpecial

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
	BOOL IsActive( void ) const override { 
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
// 	bool IsNpc() override {
// 		return !m_bPc;
// 	}
// 	bool IsPC() override {
// 		return m_bPc;
// 	}
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
// 	bool IsGuildRaid() override {
// 		return GetpProp()->strType == _T( "guild_raid" );
// 	}
// 	bool IsMedalCamp() {
// 		return GetpProp()->strType == _T("fixed_camp");
// 	}
// 	// 영웅의전장인가.
// 	bool IsHeroCamp() {
// 		return GetpProp()->strType == _T("hero_camp");
// 	}
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
	void OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat ) override;
	void ResetLevel( XSPAcc spAcc ) override;
	void ResetName( XSPAcc spAcc ) override;
	void OnBeforeBattle( XSPAcc spAcc ) override;
private:
}; // class XSpotCommon



