﻿#pragma once
#include "XPropUnit.h"
#include "XFramework/Game/XEComponents.h"
#include "Skill/xSkill.h"
#include "XHero.h"
#include "XPropSquad.h"
#include "Sprite/Sprdef.h"
////////////////////////////////////////////////////////////////
class XBaseUnit;
class XHero;
class XSquadron;
class XWndBattleField;
class XSquadObj;
class XLegionObj;
class XEWndWorld;
//#include "XBaseUnit.h"

class XSquadObj : public std::enable_shared_from_this <XSquadObj>

{
public:
	static void sCalcBattlePos( const UnitPtr& unit1, const UnitPtr& unit2 );
	enum xtCmd {
		xCMD_NONE,
		xCMD_CHANGE_MOVEMODE,	// 이동모드로 바꿔주길 요청함.
//		xCMD_ATTACK_ENEMY,		// 가장가까운 적을 찾아 공격해주길 요청함.
		xCMD_ATTACK_TARGET,		// 주어진 타겟을 공격.
		xCMD_MOVE_POS,			// 주어진 좌표로 강제 이동
	};
#ifdef _XSINGLE
	CTimer m_timerDie;
	float m_secDie = 0;			// 부대가 전멸한 시간
	bool m_bShowHpInfo = true;		// 부대 hp정보를 표시할건지.
#endif // _XSINGLE
private:
	float m_Radius/* = 150.f*/;
	ID m_snSquadObj;
	XSquadron *m_pSquadron;
	XHero *m_pHero;
	XList4<UnitPtr> m_listUnit;
	XE::VEC3 m_vwPos;		// 분대의 중앙 지점
	BOOL m_bMove;			// 부대 이동모드
	bool m_bInoreCounterAttack = false;			// 반격무시상태
	SquadPtr m_spTarget;	///< 이동시 목표부대
	XE::VEC3 m_vwTarget;	///< 이동시 목표좌표(spTarget이 null일경우)
	XPropUnit::xPROP *m_pProp;	///< 유닛 프로퍼티
	XSPLegionObj m_spLegionObj;	///< this가 속해있는 군단객체
	int m_cntLive;				///< 살아있는 유닛의 수. m_listUnit의 수와는 별개다. list는 시체를 포함.
	BOOL m_bNearOther;			///< 현재 다른 적부대와 붙어있다.
	BOOL m_bMeleeMode;			///< 근접전모드인가?(원거리부대 전용)
//	BOOL m_bRequestMoveModeChange;	///< 외부에서 이동모드로 바꿔주길 요청이 들어옴
	xtCmd m_cmdRequest = xCMD_NONE;			///< 객체 외부에서 부대의 동작을 바꾸기위해 명령이 들어온다.
	SquadPtr m_spTargetForCmd;		///< 명령용 파라메터
	XE::VEC3 m_vDstForCmd;			///< 명령용 좌표파라메터.
	UnitPtr m_spHeroUnit;		///< 영웅 유닛의 포인터
//	SquadPtr m_spManualTarget;		///< 수동으로 직접 지정해준 공격대상이 있는지
	BOOL m_bManualMoving;			///< 현재수동 지정으로 움직이고 있는가
	int m_lvBreakThrough = 0;		///< 팔라딘-돌파 특성의 레벨. 없으면 0
	int m_lvFlameKnight = 0;		///< 팔라딘-화염의기사 특성의 레벨
	int m_lvCourage = 0;		///< 팔라딘-용기 특성의 레벨
	XSKILL::XSkillDat *m_pBreakThrough = nullptr;
	XSKILL::XSkillDat *m_pFlameKnight = nullptr;
	XSKILL::XSkillDat *m_pCourage = nullptr;		// 용기
	CTimer m_timerBreakThrough;
	CTimer m_timerHeal;		// 안수치료 타이머
	SquadPtr m_spBleedingTarget;	///< 출혈걸린 적 타겟
	ID m_idStreamByRun = 0;
	XVector<XGAME::xRES_NUM> m_aryLoots;
	void Init() {
		m_snSquadObj = XE::GenerateID();
		m_pSquadron = nullptr;
		m_pHero = nullptr;
		m_bMove = FALSE;
		m_pProp = NULL;
		m_cntLive = 0;
		m_Radius = 0;
		m_bNearOther = FALSE;
		m_bMeleeMode = FALSE;
//		m_bRequestMoveModeChange = FALSE;
		m_bManualMoving = FALSE;
	}
	void Destroy();
public:
	XSquadObj( XSPLegionObj spLegion, const XSquadron *pSquad, const XE::VEC3& vwPos );
	XSquadObj( XSPLegionObj spLegion, XHero *pHero, const XE::VEC3& vwPos );
	virtual ~XSquadObj() { Destroy(); }
	//
	void Release();
	int GetNumLives();
	int GetNumLists() {
		return m_listUnit.size();
	}
	SquadPtr GetThis() {
		return shared_from_this();

	}
	GET_ACCESSOR( BOOL, bMeleeMode );
	GET_ACCESSOR( SquadPtr&, spTarget );
	GET_ACCESSOR_CONST( ID, snSquadObj );
	GET_SET_ACCESSOR( int, lvBreakThrough );
	GET_ACCESSOR( const XE::VEC3&, vwTarget );
	BOOL IsMove() {
		return m_bMove == TRUE;
	}
	BOOL IsStop() {
		return m_bMove == FALSE;
	}
	ID getid() const {
		return m_snSquadObj;
	}
	XGAME::xtUnit GetUnitType() {
		return m_pProp->GetTypeUnit();
	}
	GET_ACCESSOR_CONST( int, cntLive );
	//GET_ACCESSOR( const XE::VEC3&, vwPos );
	const XE::VEC3& GetvwPos() const {
		return m_vwPos;
	}
	GET_ACCESSOR_CONST( XSPLegionObj, spLegionObj );
	GET_ACCESSOR( XHero*, pHero );
	GET_ACCESSOR( float, Radius );
	GET_SET_ACCESSOR( BOOL, bNearOther );
	GET_ACCESSOR( const UnitPtr&, spHeroUnit );
	GET_ACCESSOR_CONST( const XSquadron*, pSquadron );
	GET_ACCESSOR_CONST( const XList4<UnitPtr>&, listUnit );
	GET_SET_ACCESSOR_CONST( const XVector<XGAME::xRES_NUM>&, aryLoots );
	const UnitPtr& GetspLeaderUnit() {
		return m_spHeroUnit;
	}
	ID GetsnHero() const {
		if( !m_pHero )
			return 0;
		return m_pHero->GetsnHero();
	}
	/**
	 @brief this와 비교해서 pDst는 적인가
	*/
	XECompCamp& GetCamp();
	BOOL IsEnemy( XSquadObj *pDst ) {
		return GetCamp().IsEnemy( pDst->GetCamp() );
	}
	bool IsPlayer() {
		return GetCamp().GetbitCamp() == XGAME::xSIDE_PLAYER;
	}
	//
	void SetAI( BOOL bFlag );
	BOOL CreateSquad( XWndBattleField *pWndWorld, XECompCamp& camp );
#ifdef _CHEAT
	BOOL CreateSquadsDebug( XWndBattleField *pWndWorld, XECompCamp& camp );
#endif // _DEBUG
	UnitPtr AddUnit( XBaseUnit *pUnit );
	int AddCntLive( int add ) {
		m_cntLive += add;
		XBREAK( m_cntLive < 0 );
		return m_cntLive;
	}
	BOOL IsLive() const {
		return m_cntLive > 0;
	}
	void FrameMove( float dt );
private:
	// 외부에서 적부대를 공격하거나 특정좌표로 이동을 하고 싶다면 DoAttackSquad/DoMoveSquad를 쓴다.
	void DoMoveTo( SquadPtr spTarget );
	void DoMoveTo( const XE::VEC3& vwDst );
public:
	BOOL IsRange() {
		return m_pProp->typeAtk == XGAME::xAT_RANGE;
	}
	BOOL IsSpeed() {
		return m_pProp->typeAtk == XGAME::xAT_SPEED;
	}
	BOOL IsTanker() {
		return m_pProp->typeAtk == XGAME::xAT_TANKER;
	}
	///< 근접전(맞붙어 싸우는) 부대인가.
	BOOL IsMelee() {
		return m_pProp->typeAtk == XGAME::xAT_TANKER || 
				m_pProp->typeAtk == XGAME::xAT_SPEED;
	}
	// 일단은 유닛프로퍼티의 값으로 이동하고, 나중엔 부대도 XAdjParam을 상속받아 부대단위로 버프를 받아야 한다.
	float GetSpeedMoveForPixel() {
		return m_pProp->movSpeedPerSec / XFPS;
	}
	void AddPos( const XE::VEC3& vwDelta ) {
		m_vwPos += vwDelta;
	}
	SquadPtr FindAttackSquad();
	UnitPtr FindAttackTarget( BOOL bIncludeHero );
	void OnDieMember( XBaseUnit *pUnit );
	XE::VEC2 GetDistUnit( XPropUnit::xPROP *pProp );
	int GetRow( XPropUnit::xPROP *pProp );
	XE::VEC2 GetDistHero( XPropUnit::xPROP *pProp );
	void CreateHero( XWndBattleField *pWndWorld, XPropUnit::xPROP *pProp, XECompCamp& camp );
	void CreateUnit( XWndBattleField *pWndWorld,
					XPropUnit::xPROP *pProp,
					int numUnit,
					XECompCamp& camp );
	int GetListMember( XArrayLinearN<XSKILL::XSkillReceiver*, 512> *pAry );
	int GetListMember( XArrayLinearN<XBaseUnit*, 512> *pAry );
	XBaseUnit* GetLiveMember();
	BOOL IsNear( XSquadObj *pTarget );
private:
	UnitPtr GetNewTargetInTargetSquad( BOOL bIncludeHero );
public:
	BOOL IsHaveTargetSquad() {
		return m_spTarget != nullptr;
	}
	void DoAllUnitsChase( const SquadPtr& spTarget );
	UnitPtr GetAttackTargetForUnit( const UnitPtr& unit );
	void OnAttacked( const SquadPtr& spAttacker );
	XE::VEC3 GetvCenterByUnits();
	void DoAttackSquad( const SquadPtr& spTarget );
	void DoMoveSquad( const XE::VEC3& vwDst );
	float GetDistAttack();
	void DoDamage( float damage, BOOL bCritical );
	void DoDamageByPercent( float ratio, BOOL bCritical );
	void CreateAndAddUnit( XPropUnit::xPROP *pProp
											, XWndBattleField *pWndWorld
											, int idxUnit
											, XECompCamp& camp
											, const XE::VEC3& vCurr
											, float mulByLvSquad );
	float GetTotalHp();
	float GetTotalHpRate();
	float GetAvgSpeedUnits();
	void OnStartBattle();
	void OnAfterStartBattle();
	void HardcodingBreakthrough( float& speedMultiply, XArrayLinearN<SquadPtr, 64>& aryNear );
	float GetAttackMeleeDamage();
	void AddAdjParam( XGAME::xtParameter adjParam, XSKILL::xtValType valType, float adj );
	void DoHeal( float addHp );
	void DoHealByPercent( float ratio );
	int GetLevelByAbil( XGAME::xtUnit unit, LPCTSTR sid );
	int GetLevelByAbil( XGAME::xtUnit unit, ID idAbil );
	float GetInvokeRatioAbil( LPCTSTR sid, XGAME::xtUnit unit );
	void OnNewTarget( UnitPtr spTarget );
// 	XAccount* GetAccount();
	bool IsState( XGAME::xtState state );
	XSPLegionObj GetEnemyLegion();
	void ProcessLycan( float dt );
	void ProcessHardcode( float dt );
	bool IsAbleTarget();
	SquadPtr DoAttackAutoTargetEnemy();
	void DoRequestMoveMode();
	float GetSumHpAllMember();
	float GetMaxHpAllMember() const;
	float DrawMembersHp( const XE::VEC2& v );
	float GetDistBetweenSquad( SquadPtr spOther );
	bool IsInAttackRadius( SquadPtr spOther );
	bool IsResourceSquad() const;
	
private:
	void SetCmdRequest( xtCmd cmd, const XE::VEC3& vDst ) {
		m_cmdRequest = cmd;
		m_vDstForCmd = vDst;
	}
	void SetCmdRequest( xtCmd cmd, SquadPtr& spTarget ) {
		m_cmdRequest = cmd;
		m_vDstForCmd.Set( 0 );
		m_spTargetForCmd = spTarget;
	}
	void SetCmdRequest( xtCmd cmd, const SquadPtr& spTarget ) {
		m_cmdRequest = cmd;
		m_vDstForCmd.Set( 0 );
		m_spTargetForCmd = spTarget;
	}
	void SetCmdRequest( xtCmd cmd ) {
		m_cmdRequest = cmd;
		m_vDstForCmd.Set(0);
		m_spTargetForCmd.reset();
	}
	void ProcessCmd();
	float GetMulByLvSquad( XPropUnit::xPROP *pProp, int idxUnit );
	void DoTeleport();
}; // XSquadObj
