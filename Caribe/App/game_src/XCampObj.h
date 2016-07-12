/********************************************************************
	@date:	2015/05/08 18:54
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XCampObj.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XPropCamp.h"

XE_NAMESPACE_START( xCampaign )

// XSPStageObj sCreateStageObj( XSPPropStage spProp );
XSPCampObj sCreateCampObj( xtType typeCamp, XPropCamp::xProp* pPropCamp, int grade );

/****************************************************************
* @brief 캠페인 인스턴스
* @author xuzhu
* @date	2015/05/08 18:54
*****************************************************************/
class XCampObj : public XLock		// 이방식은 파괴시 보호되지 않는 문제가 있지만 shared_ptr과 함께쓰면 안전하다.
							 , public std::enable_shared_from_this<XCampObj>
{
public:
	static int sSerialize( XSPCampObj& spCampObj, XArchive& ar );
	static XSPCampObj sCreateDeserialize( XArchive& ar );
	static int sDeserializeUpdate( XSPCampObj spCampObj, XArchive& arCamp );
	static int sSerializeLegion( XSPCampObj& spCampObj, XArchive& ar );
	static int sDeSerializeLegion( XSPCampObj& spCampObj, XArchive& ar );
	static XSPCampObj sCreateCampObj( XPropCamp::xProp* pPropCamp, int grade );
#ifdef _CLIENT
	static int s_idxStage;	// 클라에서 파라메터를 넘기기가 마땅찮아서 궁여지책으로 이렇게....;;;
	static int s_idxFloor;	// 클라에서 파라메터를 넘기기가 마땅찮아서 궁여지책으로 이렇게....;;;
#endif // _CLIENT
private:
	ID m_keyProp = 0;
	xtType m_Type = xCT_NONE;
	XPropCamp::xProp *m_pProp = nullptr;
	XVector<XVector<XSPStageObj>> m_aryStages;
//	XVector<XSPStageObj> m_aryStages;
	ID m_snCampaign = 0;	// 캠페인 인스턴스 고유번호
	int m_idxLastUnlock = 0;	// 현재 깨야할 스테이지의 인덱스.
	int m_idxLastPlay = -1;						///< 현재 전투한 스테이지
	ID m_idAccTrying = 0;     // 현재 전투중인 유저의 아이디
	_tstring m_strTryer;		// 현재 전투중인 유저의 이름.
	XTimerTiny m_timerEnter;    // 누가 전투중이면 on상태가 된다.
	XTimerTiny m_timerOpen;		// 캠페인이 열리면 타이머가 작동을 시작한다.
	int m_cntTry = 0;			// 총 전투시도한 횟수
	bool m_bSendReward = false;	// 보상을 다 받았는가.
	int m_idxPlaying = -1;		// 현재 플레이중인 스테이지 인덱스
	bool m_bAutoReset = false;	// 타이머에 의해서 자동으로 캠페인이 리셋되는가 수동으로 리셋해야하는가.
	int m_secResetCycle = 0;	// 몇초만에 리셋되는가.
	int m_Grade = 0;			// 메달캠페인처럼 등급이 따로있는경우 지정한다.
//	int m_numStars = 0;			// 누적 별개수
	void Init() {
		m_snCampaign = XE::GenerateID();
	}
	void Destroy();
public:
	XCampObj( xtType typeCamp, XPropCamp::xProp *pProp, int grade = 0 );
	virtual ~XCampObj() { Destroy(); }
	//
//   int GetNumStages() {
//     return (int)m_aryStages.size();
//   }
	GET_LOCK_ACCESSOR( xtType, Type );
	GET_SET_LOCK_ACCESSOR( int, idxLastPlay );
	GET_READONLY_ACCESSOR( XPropCamp::xProp*, pProp );
	GET_LOCK_ACCESSOR( int, cntTry );	
	GET_LOCK_ACCESSOR( ID, idAccTrying );
	GET_LOCK_ACCESSOR( const _tstring&, strTryer );
	GET_LOCK_ACCESSOR( int, idxPlaying );
	GET_LOCK_ACCESSOR( bool, bAutoReset );
	GET_LOCK_ACCESSOR( int, secResetCycle );
	GET_LOCK_ACCESSOR( int, Grade );
//	GET_LOCK_ACCESSOR( std::vector<XSPStageObj>&, aryStages );
	GET_LOCK_ACCESSOR( int, idxLastUnlock );
	XVector<XSPStageObj>& GetaryStages( int idxFloor = 0 ) {
		return m_aryStages[ idxFloor ];
	}
	const XVector<XSPStageObj>& GetaryStagesConst( int idxFloor = 0 ) const {
		return m_aryStages[idxFloor];
	}
	XVector<XVector<XSPStageObj>>& GetaryFloor() {
		return m_aryStages;
	}
protected:
	SET_LOCK_ACCESSOR( int, idxLastUnlock );
	SET_LOCK_ACCESSOR( int, cntTry );
public:
//	GET_SET_ACCESSOR( int, numStars );
	int GetnumStar( int idxFloor = 0 );
	void InitCamp( int lvBase, int idxFloor = 0 );
	XSPCampObj GetThis() {
		return shared_from_this();
	}
	ID GetidProp() {
		return GetpProp()->idProp;
	}
	GET_SET_LOCK_ACCESSOR( bool, bSendReward );
	virtual XSPStageObj CreateStageObj( XSPPropStage spPropStage, int idxFloor = 0 );
	/// 마지막스테이지 까지 클리어했는가.
	bool IsEndStage( int idxFloor = 0 ) const {
		return m_idxLastUnlock >= GetNumStages( idxFloor );
	}
	virtual int Serialize( XArchive& ar );
	int SerializeLegion( XArchive& ar, int idxFloor = 0 );
	XSPPropStage GetspStageProp( int idxStage, int idxFloor = 0 );
	int GetNumStages( int idxFloor = 0 ) const {
		return GetaryStagesConst( idxFloor ).size();
	}
	XSPStageObj GetspStageLastUnlock( int idxFloor = 0 ) {
		XLOCK_OBJ;
		if( GetidxLastUnlock() >= GetNumStages() )
			return nullptr;
		return GetaryStages(idxFloor)[ m_idxLastUnlock ];
	}
	XSPStageObj GetspStageLastPlay( int idxFloor = 0 ) {
		XLOCK_OBJ;
		if( m_idxLastPlay < 0 )
			return nullptr;
		return GetspStage( m_idxLastPlay, idxFloor );
	}
private:
	/// idxStage를 클리어했다. 모든 스테이지를 클리어했으면 true리턴
	virtual bool ClearStage( int idxStage, int idxFloor = 0 );
protected:
	void ClearStageAll( int idxFloor = 0 ) {
		GetaryStages( idxFloor ).clear();
	}
public:
	// 진입했던 스테이지를 클리어한다.
	bool ClearStageLastPlay( int idxFloor = 0 ) {
		XBREAK( m_idxPlaying == -1 );
		return ClearStage( m_idxPlaying, idxFloor );
	}
	bool IsClearCampaign() const {
		if( IsEndStage() )
			return true;
		return false;
	}
	virtual XSPStageObj GetspStage( int idxStage, int idxFloor = 0 ) {
		XLOCK_OBJ;
		if( idxFloor >= m_aryStages.Size() )
			return nullptr;
		if( idxStage >= (int)m_aryStages[ idxFloor ].size() )
			return nullptr;
		return m_aryStages[ idxFloor ][idxStage];
	}
	XSPStageObj GetspStageObjWithidProp( ID idPropStage, int idxFloor = 0 );
	void SetspStageObj( int idx, XSPStageObj spStageObj, int idxFloor = 0 ) {
		XLOCK_OBJ;
		const int size = GetaryStages( idxFloor ).size();
		if( XASSERT( size > 0  && idx >= 0 && idx < size ) )
			GetaryStages( idxFloor )[ idx ] = spStageObj;
	}
	/// DoEnterStage로 진입한후 진입한 스테이지의 객체를 꺼낸다. DoEnter를 하지 않으면 null이다.
	XSPStageObj GetspStageObjCurrPlaying( int idxFloor = 0 ) {
		if( m_idxPlaying >= 0 )
			return GetspStage( m_idxPlaying, idxFloor );
		return nullptr;
	}
	/// 현재스테이지(아직 안깬스테이지)의 군대정보
	LegionPtr GetspLegionLastUnlockStage( int idxFloor = 0 );
	LegionPtr GetspLegionLastPlayStage( int idxFloor = 0 );
	virtual XGAME::xtError IsAbleTry( const XSPStageObj spStageObj, int idxFloor = 0 );
	bool IsAbleTry( int idxStage, int idxFloor = 0 );
	/// 캠페인 이름
	LPCTSTR GetNameCamp() {
		return XTEXT(m_pProp->idName);
	}
	void DestroyLegion( int idxStage, int idxFloor = 0 );
	void DestroyLegionbyLastPlayStage( int idxFloor = 0 ) {
		XLOCK_OBJ;
		if( m_idxLastPlay < 0 )
			return;   // try한적이 없다.
		DestroyLegion( m_idxLastPlay, idxFloor );
	}
	// 현재 어떤 유저가 전투중인가.
	bool IsTryingUser() const {
		return m_idAccTrying != 0;
	}
	// 누가 전투중일때 시작시간
	xSec GetsecStartTry() {
		XLOCK_OBJ;
		if( m_idAccTrying == 0 )
			return 0;
		return m_timerEnter.GetsecStart();
	}
	// 누가 전투중일때 남은시간
	xSec GetsecRemainTry();
	// idAcc유저가 도전을 시작
	void DoStartBattle( ID idAcc, _tstring& strName ) {
		XLOCK_OBJ;
		m_idAccTrying = idAcc;
		m_strTryer = strName;
		m_timerEnter.DoStart();
		m_timerEnter.AddSec( 150 );
		++m_cntTry;
	}
	// 전투를 끝나게함.
	void DoFinishBattle() {
		XLOCK_OBJ;
		m_idAccTrying = 0;
		m_strTryer.clear();
		m_timerEnter.Off();
	}
	// 스테이지 트라잉이 시간오버되었는가.
	bool IsTimeOverTry() {
		XLOCK_OBJ;
		return GetsecRemainTry() <= 0;
	}
	// 캠페인 닫히기(재오픈)까지 남은 시간
	xSec GetsecRemainClose();
	// 리셋까지 남은 시간
	xSec GetsecRemainReset();
	bool IsOpenCampaign() {
		XLOCK_OBJ;
		return m_timerOpen.IsOn();
	}
	void DoOpenCampaign( int lvBase, int idxFloor = 0 );
	// 값을 초기화시키고 closed상태로 만듬.
	void DoCloseCampaign( int idxFloor = 0 ) {
		XLOCK_OBJ;
		InitCamp(0, idxFloor );
	}
	bool IsCloseCampaign() {
		XLOCK_OBJ;
		return m_timerOpen.IsOff();
	}
	void CreateLegionAll( int lvBase, int idxFloor = 0 );
	void SetParamLegionAllStage( XGAME::xLegionParam& paramLegion, int idxFloor = 0 );
	virtual void CreateLegionIfEmpty( int lvBase, int idxFloor = 0 );
	bool FindTryer( ID idAcc, int idxFloor = 0 );
	int GetRewardPointByClear( int lvAcc, int idxFloor = 0 );
	int GetRewardPointLastStage( int lvAcc, int idxFloor = 0 );
	float GetRewardPerStage( int lvAcc, int idxFloor = 0 );
	bool IsAllClearStage( XSPStageObj spStageObj );
//	bool IsActiveStage( XSPStageObj spStageObj );
//	bool IsEnterableStage( XSPStageObj spStageObj );
	bool DoEnterStage( int idxStage, int idxFloor = 0 );
#ifdef _GAME_SERVER
	virtual void Update( XSPAcc spAcc );
#endif // _GAME_SERVER
	void _DoClear( int idxFloor = 0 );
	bool IsRecvAllReward( int idxFloor = 0 );
	void CreateStageObjManual( int numStage, int idxFloor = 0 );
	void AddStage( XSPStageObj spStageObj, int idxFloor = 0 );
	/// 전투가 끝났을때 반드시 들어오는 핸들러
	void OnFinishBattle() {
		AddcntTry();
	}
	virtual void OnCreate();
	virtual int GetlvLegion( XPropCamp::xStage* pPropStage, int lvBase, int idxFloor );
	virtual int GetlvLimit( int idxStage, int idxFloor = 0 );
	virtual ID GetidHeroByCreateSquad( int idxSquad, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage );
	virtual XGAME::xtUnit GetUnitWhenCreateSquad( int idxSquad, XGAME::xtAttack typeAtk, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage, int idxFloor );
protected:
	virtual int DeSerialize( XArchive& ar, int verCamp );
	int DeSerializeLegion( XArchive& ar, int verCamp );
	virtual void ResetCamp( int lvAcc, int idxFloor = 0 );
	virtual void CreateFloor( int idxFloor = 0 );	// 영웅의전장땜에 임시로 virtual로 해둠.
private:
	void AddcntTry() {
		++m_cntTry;
	}
	virtual void ResetCampAllFloor( int lvAcc );
}; // class XCampObj




XE_NAMESPACE_END;   // namespace xCampiagn
