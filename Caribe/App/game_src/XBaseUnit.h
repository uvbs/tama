/********************************************************************
	@date:	2014/09/15 12:16
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XBaseUnit.h
	@author:	xuzhu
	
	@brief:	유닛들의 기본형 객체
*********************************************************************/
#pragma once
#include "XFramework/Game/XEBaseWorldObj.h"
#include "XPropUnit.h"
//#include "Skill/xSkill.h"
#include "XFramework/Game/XEComponents.h"
#include "XFSMUnit.h"
#include "XPropHero.h"
#include "XObjEtc.h"
#include "skill/XSkillUser.h"
#include "skill/XSkillReceiver.h"

namespace XSKILL {
class XSkillDat;
struct EFFECT;
}
class XWndBattleField;
class XEBaseFSM;
class XFSMIdle;
class XSquadObj;
class XLegionObj;
class XHero;
class XStatistic;
////////////////////////////////////////////////////////////////
/**
 @brief 유닛들의 기본형 객체
*/
class XBaseUnit : public XEBaseWorldObj, public XEControllerFSM
										, public XSKILL::XSkillReceiver
										, public XSKILL::XSkillUser	// 유닛은 개별적으로 액티브 스킬을 쓰지 않는다고 가정하고 없앰.
										, public XDelegateObjBullet
{
public:
	static XBaseUnit* sCreateUnit( XSquadObj *pSquadObj, ID idProp, BIT bitSide, const XE::VEC3& vwPos, float multipleAbility );
	static XBaseUnit* sCreateHero( XSquadObj *pSquadObj, XHero *pHero, ID idPropUnit, BIT bitSide, const XE::VEC3& vwPos, float multipleAbility );
	static BOOL sGetMoveDstDelta( const XE::VEC3& vwCurr, const XE::VEC3& vwDst, float speedMove, XE::VEC3 *pOutDelta );
	static bool s_bNotUseActiveByEnemy;		// 적영웅 스킬사용금지
#ifdef WIN32
//	_tstring m_strLog;			// 전투로그
#endif // _XSINGLE
protected:
	struct xInvokeEffect {
		XSKILL::XSkillDat *pDat = nullptr;
		XSKILL::EFFECT *pEffect = nullptr;
	};
	XArrayLinearN<xInvokeEffect,4> m_aryInvokeSkillByAttack;		// 평타공격시 발동확률이 있는 스킬이 있을경우 확률에 걸리면 이곳에 정보가 쌓인다
private:
	enum { 
		xFL_NONE = 0,
		xFL_AI = 0x0001,	// ai중인가.
		};
	/**
	 @brief 어느진영인지 구분하는 비트
	 예를들어 아군2팀 적2팀 이라면 총 4개의 진영이된다.
	*/
//#ifdef WIN32
	_tstring m_strLog;
	std::string m_strcIds;
//#endif // WIN32
	ID m_idProp;			// 자주쓰는거라 최적화를 위해 변수로 빼놓음. 
	XECompCamp m_Camp;
	XE::VEC3 m_vDelta;
	CTimer m_Timer;
	int m_HP;
	XSprObj *m_psoHit;	// 임시
	int m_cntTargeting;		///< this가 몇명의 유닛들로부터 타겟팅을 당했나.
	XSurface *m_psfcShadow;
	XSprObj *m_psoSelect;	///< 부대가 선택되었을때 유닛밑에 표시되는 인디케이터
	/// 바운딩박스 구하는 동작이 사이즈가 필요없는 잡스런 오브젝트에게는 필요가 없는것이라 XbaseUnit으로 내려옴
	XE::xRECT m_bbLocal;	///< 일반적인 모션의 바운딩박스를 미리구해둔다.(주로 Idle모션을 사용)
	XE::VEC2 m_vlHitSfx;	///< 타격시 이펙트가 발생될 로컬기준좌표
	XE::xtHorizDir m_Dir;		///< 바라보고 있는 방향
	XSquadObj *m_pSquadObj;		///< 이 유닛이 속해있는 분대객체(weak)
	UnitPtr m_spTarget;		///< 추적목표 객체
//	SquadPtr m_spTargetSquad;		///<< 타겟 부대
	XE::VEC3 m_vwTarget;		///< spTarget이 null일때 좌표만으로 이동할 목표
	const XE::VEC3 m_vLocalFromSquad;	///< 분대내에서의 로컬좌표
	int m_idxAttackedAngle;			///< 근접공격자가 this를 타겟잡을때마다 겹치지 않고 둘러쌀수 있도록 적당한 위치를 내보내준다. 그것을 위해 필요한 인덱스
	XArrayN<UnitPtr,8> m_aryAttacked;	///< this를 둘러싸고 공격하고 있는 공격자들.
//	XE::VEC2 m_vlVecFromTarget;		///< 타겟을 잡으면 타겟의 어느쪽에 설건지 벡터를 받아둔다.
	XE::VEC2 m_vwBind;				///< 전투를 위해 이동할 좌표가 확정됨.
	XE::VEC3 m_vlActionEvent;		///< 액션이벤트때 타점좌표를 저장해둔다.(유닛로컬좌표)
	CTimer m_timerDamage;			///< 데미지 입으면 타이머가 작동한다.
	XE::VEC2 m_vHitOffset;			///< 타격이펙트 약간 랜덤성위치에 찍히도록
	float m_speedBind;			///< 바인드좌표로 이동시 이동할 속도
	XE::VEC2 m_vsPos;			///< 계산최소화를 위하여 Draw할때마다 프로젝션계산된 윈도우 좌표를 받아둔다. 이전프레임의 좌표라는것에 주의,
	CTimer m_timerAttack;		///< 공속타이머
	float m_multipleAbility;		///< 부대레벨에 따른 능력치 배수
	XGAME::xtMelee m_typeCurrMeleeType;	///< 현재 어떤 공격중이었는가.
	XGAME::xtTribe m_Tribe;		///< 종족
	CTimer m_timerHpRegen;		///< 체력리젠 타이머
	int m_cntAttack = 0;			///< 평타 휘두른 횟수
	int m_cntHit = 0;				///< 피격횟수
//	UnitPtr m_spProtectorGolem;		///< this유닛을 보호중인 골렘.
//	bool m_bInvokeHitter = false;			///< 쏜즈어택류를 처리할때 한프레임에 한번의 피격이벤트만 발생시키기 위해 플래그를 둠.
	CTimer m_timerInvokeHitter;		///< 쏜즈어택류를 처리할때 피격이벤트가 일어날때마다 데미지를 입히니까 너무 쎄서 일정 시간이 지나야 다시 쏜즈효과가 발동되도록 함.
	int m_cntShell = 0;					///< 골렘의 "보호"특성을 위한 껍질 개수.
// 	int m_cntPerSec = 0;				///< 1초마다 하나씩 올라가는 카운터(엔트의 광합성 특성에 사용)
	BIT m_bitFlags = 0;				///< 각종플래그들(xFL_...)
  int m_Count = 0;          ///< 디버깅용 FrameMove카운터
	XPropUnit::xPROP *m_pPropUnit = nullptr;	///< 유닛 프로퍼티.(this가 영웅이어도 갖고 있다)
	struct xIconBuff {
		ID m_idSkill = 0;
		XSurface *m_psfcIcon = nullptr;
	};
	std::vector<xIconBuff> m_aryBuffIcon;				// 버프용 아이콘 
	CTimer m_timerDead;
	CTimer m_timerDisappear;
public:
//	CTimer m_timerSlow;			// 테스트용
private:
	void Init() {
		m_idProp = 0;
		m_HP = 10;
		m_psoHit = nullptr;
		m_cntTargeting = 0;
		m_psfcShadow = nullptr;
		m_Dir = XE::HDIR_NONE;
		m_pSquadObj = NULL;
		m_idxAttackedAngle = 0;
		m_vwBind.Set(-1.f);		// 초기화 값
		m_speedBind = 0;
		m_psoSelect = NULL;
		m_multipleAbility = 1.f;
		m_typeCurrMeleeType = XGAME::xMT_NONE;
		m_Tribe = XGAME::xTB_NONE;
	}
	void Destroy();
	///< 
	SET_ACCESSOR( const XE::VEC3&, vwTarget );
protected:
// 	GET_ACCESSOR( int, cntPerSec );
public:
	XBaseUnit( XSquadObj *pSquadObj
					, ID idProp
					, BIT bitSide
					, const XE::VEC3& vPos
					, float multipleAbility );
	virtual ~XBaseUnit() { Destroy(); }
	//
	GET_ACCESSOR_CONST( ID, idProp );
	GET_ACCESSOR( const XE::VEC3&, vDelta );
	GET_ACCESSOR( UnitPtr&, spTarget );
	SET_ACCESSOR( const UnitPtr&, spTarget );
	GET_ACCESSOR( const XE::VEC2&, vsPos );
	GET_ACCESSOR( XPropUnit::xPROP*, pPropUnit );
	GET_SET_ACCESSOR_CONST( const std::string&, strcIds );
//	GET_ACCESSOR( XHero*, pHero );
 	GET_ACCESSOR( float, multipleAbility );
	GET_SET_ACCESSOR( XGAME::xtMelee, typeCurrMeleeType );
// 	void SetspTarget( const UnitPtr& spTarget ) {
// 		m_spTarget = spTarget;
// 	}
	GET_ACCESSOR( int, cntTargeting );
	GET_SET_ACCESSOR( XE::xtHorizDir, Dir );
	GET_ACCESSOR( XSquadObj*, pSquadObj );	///< this가 속한 부대를 리턴한다.
//	GET_ACCESSOR( const SquadPtr&, spTargetSquad );		///< 현재 타겟으로 잡은 부대
	GET_ACCESSOR( const XE::VEC3&, vwTarget );
//	GET_ACCESSOR( const XE::VEC2&, vlVecFromTarget );
	GET_SET_ACCESSOR( const XE::VEC2&, vwBind );
	GET_SET_ACCESSOR( float, speedBind );	///< 프레임당 이동픽셀
	GET_SET_ACCESSOR( XGAME::xtTribe, Tribe );
// 	ID GetIdProp() const {
// 		return m_pProp->idProp;
// 	}
	GET_ACCESSOR( const XE::VEC3&, vLocalFromSquad );
	GET_SET_ACCESSOR( const XE::VEC3&, vlActionEvent );
	GET_SET_ACCESSOR( int, cntAttack );
	GET_ACCESSOR( int, cntShell );
	int AddCntAttack() {
		return ++m_cntAttack;
	}
	GET_SET_ACCESSOR( int, cntHit );
	int AddCntHit() {
		return ++m_cntHit;
	}
// 	virtual BOOL IsHero() {
	BOOL IsHero() const {
		if( m_idProp > 0 && m_idProp < XGAME::xUNIT_MAX )
			return FALSE;
		return TRUE;
	}
	bool IsUnit() const {
		return !IsHero();
	}
	XPropHero::xPROP* GetpPropHero();
	XHero* GetHero();
	virtual bool IsLeader() {return FALSE;}
	UnitPtr GetspLeader();
	UnitPtr GetThisUnit() {
		return std::static_pointer_cast<XBaseUnit>( GetThis() );
	}
	/**
	 @brief 현재 타겟의 아이디를 돌려준다.
	*/
	ID GetidTarget() {
		if( m_spTarget != nullptr )
			return m_spTarget->GetsnObj();
		return 0;
	}
	/// 타겟이 살아있는지 검사
	BOOL IsTargetLive() {
		if( m_spTarget != nullptr )
			return m_spTarget->IsLive();
		return FALSE;
	}
	BOOL IsBindTarget() {
		return m_vwBind.IsMinus() == FALSE;
	}
	void ClearBind() {
		m_vwBind.Set(-1.f);
	}
	void DoDirToTarget( const XE::VEC3& vwDst );
	/**
	 @brief 바운딩박스를 기반으로 오브젝트의 크기를 구함
	*/
	virtual XE::VEC3 GetSize() const {
		XE::VEC2 vSize = m_bbLocal.GetSize();
		return XE::VEC3(vSize.w, 0.f, vSize.h);
	}
	BOOL IsBig() {
		return GetUnitSize() == XGAME::xSIZE_BIG;
	}
	BOOL IsMiddle() {
		return GetUnitSize() == XGAME::xSIZE_MIDDLE;
	}
	BOOL IsSmall() {
		return GetUnitSize() == XGAME::xSIZE_SMALL;
	}

	/**
	 @brief 로컬 바운드박스 좌표를 리턴한다.
	*/
	XE::xRECT& GetBoundBox() {
		return m_bbLocal;
	}
	// XBaseUnit은 영웅이 될수도 있으므로 아래같은건 사실 여기다 두면 안되는데.
	inline XGAME::xtUnit GetType() {
 		return GetUnitType();
	}
	// 이 객체의 유닛종류.(영웅이면 자기가이끄는 유닛이된다)
	inline XGAME::xtUnit GetUnitType() {
		auto unit = GetSquadUnit();
		XBREAK( unit != m_pPropUnit->GetTypeUnit() );	// 이런경우 발생안하면 바로 프로퍼티에서 뽑아쓰도록 고칠것.
		return GetSquadUnit();
// 		return m_pPropUnit->GetTypeUnit();
//  		return GetSquadUnit();
	}
	XGAME::xtUnit GetSquadUnit();

	XFSMBase* GetpFSM() {
		return SafeCast<XFSMBase*, XEBaseFSM*>( XEControllerFSM::GetpBaseFSM() );
	}
	/**
	 @brief 플레이어가 직접 조종하는 진영인가.
	*/
	bool IsPlayer() const {
		return (m_Camp == XGAME::xSIDE_PLAYER);
	}
	bool IsEnemy() const {
		return !IsPlayer();
	}
	/**
	 @brief this와 비교해서 pDst는 적인가
	*/
	BOOL IsEnemyWithUnit( XBaseUnit *pDst ) {
		return m_Camp.IsEnemy( pDst->GetCamp() );
	}
	BOOL IsFriendlyWithUnit( XBaseUnit *pDst ) {
		return !IsEnemyWithUnit( pDst );
	}
	/**
	 @brief this가 속한 군단을 리턴한다.
	*/
	XSPLegionObj GetspLegionObj();
//	BOOL DoChaseNewTarget();
	BOOL RequestNewMission();
	BOOL IsDead() {
		return !IsLive();
	}
	void AddDelta( const XE::VEC3& vDelta ) {
		m_vDelta += vDelta;
	}
	void AddCntTargeting( int add ) {
		m_cntTargeting += add;
		if( m_cntTargeting < 0 )
			m_cntTargeting = 0;
	}
	void SetAI( BOOL bFlag );
	/**
	 @brief this의 적에 대한 타입을 얻는다.
	*/
	XGAME::xtSide GetEnemySide() {
		if( m_Camp.GetbitCamp() == XGAME::xSIDE_PLAYER )
			return XGAME::xSIDE_OTHER;
		return XGAME::xSIDE_PLAYER;
	}
	void CreateHitSfx( XBaseUnit *pAttacker, BOOL bCritical, BOOL bAbsolute = FALSE );
//	void DoDamage( XEBaseWorldObj *pAttacker, float damage, BOOL bCritical, float ratioPenetration, XSKILL::xtDamage typeDamage, bool bBySkill );
	void DoDamage( XEBaseWorldObj *pAttacker, float damage, float ratioPenetration, XSKILL::xtDamage typeDamage, const BIT bitAttrHit, XGAME::xtDamageAttr attrDamage );

	float AddHp( float add ) {
		if( m_HP == 0 )
			return 0;
		if( add < 0 && add > -1.f )
			add = -1.f;					// 1보다 작은 데미지는 무조건 1이 들어가도록
		m_HP += (int)add;
		if( m_HP <= 0 )
			m_HP = 0;
		else 
		{
			int hpMax = GetMaxHp();
			if( m_HP > hpMax )
				m_HP = hpMax;
		}
		return (float)m_HP;
	}
	BOOL IsNear( const UnitPtr& spUnit );
	virtual void OnCreate();
	//////////////////////////////////////////////////////////////////////////
	// stat관련
	// 프레임당 이동속도(픽셀단위)
	/// 유닛의 반지름크기
	float GetSizeRadius() {
		return GetSize().w / 2.f;
	}
	int GetHp();
	int DoFullHp() {
		int max = GetMaxHp();
		m_HP = max;
		return m_HP;
	}
	BOOL IsRange() {
		return m_pPropUnit->IsRange();
	}
	virtual LPCTSTR GetszSpr() = 0;
	virtual XE::VEC3 GetHSL() { return XE::VEC3(); }
	virtual XE::xHSL GetHSL2() { return XE::xHSL(); }
	/// 공격사거리(픽셀)
	float GetAttackRadiusByPixel();
	float GetSpeedMoveForPixel() {
		return GetSpeedMovePerSec() / XFPS;
	}
	float GetMovePixelPerFrame( float movPixelPerFrame );
	float GetSpeedMovePerSec();
	inline float GetSpeedAttackBase() {
		return m_pPropUnit->atkSpeed;
	}
	float GetSpeedAttack( UnitPtr spTarget );
	int GetMaxHp();
	float GetAttackMeleePower();
	float GetAttackRangePower();
	float GetAttackMeleeDamage( UnitPtr spTarget );
	float GetAttackRangeDamage( UnitPtr spTarget );
	float GetAddRateByStat( XGAME::xtStat statType, XSPUnit spTarget );
	float GetDefensePower();
	inline XGAME::xtSize GetUnitSize() {
		return m_pPropUnit->size;		// 이제 영웅도 유닛사이즈를 따른다.(상성때문)
	}
	virtual float GetPropScale() = 0;
	inline XGAME::xtAttack GetTypeAtk() {
		return m_pPropUnit->typeAtk;
	}
	inline int GetMovSpeedNormal() {
		return m_pPropUnit->movSpeedNormal;
	}
	float GetCriticalRatio();
	float GetCriticalPower();
	float GetEvadeRatio( XSKILL::xtDamage typeDamage, XBaseUnit *pAttacker );
	float GetPenetrationRatio();
	// stat관련
	//////////////////////////////////////////////////////////////////////////
	XFSMChase* DoChaseAndAttack( const UnitPtr& spTarget );
	void DoChaseAndAttackCurrent();
	void DoChase( const XE::VEC3& vwDst );
	void DoIdle();
	BOOL IsPenaltyMOS( XBaseUnit *pDefender );
	BOOL IsPenaltySize( XBaseUnit *pDefender );
	bool IsSuperiorMOS( XBaseUnit *pDefender );
	bool IsSuperiorSize( XBaseUnit *pDefender );
	float GetAdjDamage( float damage, BOOL bCritical, XSKILL::xtDamage typeDamage, XGAME::xtDamageAttr attrDamage );
	void DoAttackTargetByBind( const UnitPtr& spTarget, BOOL bFirstDash );
	virtual XSKILL::xtPlayerType GetPlayerType() {
		return ( m_Camp == XGAME::xSIDE_PLAYER ) ? XSKILL::xHUMAN : XSKILL::xAI;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// SKILL
	float OnInvokeTargetSize( XSKILL::XSkillDat *pSkillDat,
							const XSKILL::EFFECT *pEffect,
							int level,
							XSKILL::XSkillReceiver *pCastingTarget,
							float size ) override;
	int	OnClearSkill( XSKILL::XSkillDat *pSkillDat, XSKILL::EFFECT_OBJ *pEffObj ) override;		// 버프효과가 끝나면 호출됨
	void OnCastEffectToPos( XSKILL::XSkillDat *pSkillDat,
							XSKILL::EFFECT *pEffect,
							int level,
							float sec,
							float radiusMeter,
							BIT bitSideInvokeTarget,	// 발동되어야 하는 발동대상우호
							XSKILL::XSkillReceiver *pBaseTarget,
							const XE::VEC2& vPos ) override;
// 	XE::VEC2 GetCurrentPosForSkill() override {
// 		return GetvwPos().ToVec2();
// 	}
	BOOL IsLive() override {
		if( IsDestroy() )
			return FALSE;
		if( m_HP <= 0 )
			return FALSE;
		return TRUE;
	}
	virtual void OnAdjustEffectAbility( XSKILL::XSkillDat *pSkillDat, const XSKILL::EFFECT *pEffect, int invokeParam, float *pOutMin ) override;
//	virtual void OnSkillAmplifyReceiver( XSKILL::XSkillDat *pDat, XSKILL::XSkillReceiver *pIvkTarget, const XSKILL::EFFECT *pEffect, float *pOutRatio, float *pOutAdd ) override;
	virtual void OnSkillAmplifyUser( XSKILL::XSkillDat *pDat, XSKILL::XSkillReceiver *pIvkTarget, const XSKILL::EFFECT *pEffect, XSKILL::xtEffectAttr attrParam, float *pOutRatio, float *pOutAdd ) override;
	virtual XSKILL::XSkillReceiver* GetTarget( ID snObj ) override;
	virtual XSKILL::XSkillUser* GetCaster( ID snObj ) override;
	virtual void DelegateResultEventBeforeAttack( XSKILL::XBuffObj *pBuffObj, XSKILL::EFFECT *pEffect ) override;
	BOOL IsInvokeTargetCondition( XSKILL::XSkillDat *pSkillDat, const XSKILL::EFFECT *pEffect, XSKILL::xtCondition condition, DWORD condVal ) override;
	virtual XECompCamp& GetCamp() override {		///< this의 진영을 리턴
		return m_Camp;
	}
	virtual BIT GetCampUser() override { 
		return m_Camp;
	}		
// 	virtual XE::VEC2 GetCurrentPosForSkill() {
// 		return GetvwPos().ToVec2();
// 	}
	/// 이게임에선 루아를 사용하지 않으므로 비워둠.
	virtual void RegisterInhValUse( XLua *pLua, const char *szVal ) { }
	virtual XSKILL::XSkillReceiver* GetThisRecv() override {
		return this;
	}
	virtual XSKILL::XDelegateSkill* GetpDelegate() override;
	// 발사체오브젝트를 생성하고 월드에 추가시킨다. 효과, 시전대상, 시전자, 좌표
	XSKILL::XSkillUser* CreateAndAddToWorldShootObj( 
									XSKILL::XSkillDat *pSkillDat,
// 									XSKILL::EFFECT *pEffect,
									int level,
									XSKILL::XSkillReceiver *pBaseTarget,
// 									XSKILL::XSkillReceiver *pCastingTarget,
									XSKILL::XSkillUser *pCaster,
									const XE::VEC2& vPos ) override;
	int OnApplyEffectNotAdjParam( XSKILL::XSkillUser *pCaster
															, XSKILL::XSkillDat* pSkillDat
															, const XSKILL::EFFECT *pEffect
															, float abilMin ) override;
	int GetListObjsRadius( XArrayLinearN<XSKILL::XSkillReceiver*, 512> *plistOutInvokeTarget,
									const XSKILL::EFFECT *pEffect,
									XSKILL::XSkillReceiver *pBaseTarget,
									const XE::VEC2& vBasePos,
									float meter,
									BIT bitSideFilter,
									int numApply,
									BOOL bIncludeCenter ) override;
	ID GetId() override { return GetsnObj(); }
	void OnCreateSkillSfx( XSKILL::XSkillDat *pSkillDatw,
									const XSKILL::EFFECT *pEffect,
									XSKILL::xtPoint createPoint,
									LPCTSTR szSpr,
									ID idAct,
									float secPlay,
									const XE::VEC2& vPos ) override;
	XSKILL::XSkillSfx* OnCreateSkillSfxShootTarget( XSKILL::XSkillDat *pSkillDat,
									XSKILL::XSkillReceiver *pBaseTarget,
									int level,
									const _tstring& strSpr,
									ID idAct,
									XSKILL::xtPoint createPoint,
									float secPlay,
									const XE::VEC2& vPos) override;
	void OnDestroySFX( XSKILL::XBuffObj *pSkillRecvObj, ID idSFX ) override;
	int GetGroupList( XArrayLinearN<XSKILL::XSkillReceiver*, 512> *pAryOutGroupList,
					XSKILL::XSkillDat *pSkillDat,
					const XSKILL::EFFECT *pEffect,
					XSKILL::xtGroup typeGroup ) override;
	XSKILL::XSkillReceiver* GetTargetObject( XSKILL::EFFECT *pEffect, XSKILL::xtTargetCond cond ) override;
	XSKILL::XSkillReceiver* GetSkillBaseTarget( XSKILL::XSkillDat *pDat ) override;
	XSkillReceiver* CreateSfxReceiver( XSKILL::EFFECT *pEffect, float sec ) override;
	void cbOnArriveBullet( XObjArrow *pArrow, float damage );
	void cbOnArriveSkillObj( XSkillShootObj *pArrow,
							XSKILL::XSkillDat *pSkillDat,
// 							XSKILL::EFFECT *pEffect,
							int level,
							XSKILL::XSkillReceiver *pBaseTarget );
//							XSKILL::XSkillReceiver *pCastingTarget );
	BOOL IsInvokeAddTarget( ID idAddTarget );
	virtual BOOL OnApplyState( XSKILL::XSkillUser *pCaster, XSKILL::XSkillReceiver* pInvoker, XSKILL::EFFECT *pEffect, int idxState, BOOL flagState );
	virtual BOOL OnFirstApplyState( XSKILL::XSkillUser *pCaster, XSKILL::XSkillReceiver* pInvoker, const XSKILL::EFFECT *pEffect, int idxState, BOOL flagState, int level ) override;
	virtual XSKILL::XSkillReceiver* GetBaseTargetByCondition( XSKILL::XSkillDat *pSkillDat,
													XSKILL::xtBaseTargetCond cond,
													float meter,
													int level,
													XSKILL::XSkillReceiver *pCurrTarget,
													const XE::VEC2& vPos );
	virtual int GetSkillLevel( XSKILL::XSkillObj* pSkillObj );
	virtual XSkillReceiver* GetCurrTarget();
	virtual bool OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster, XSKILL::XBuffObj *pBuffObj, XSKILL::XSkillDat *pSkillDat, const XSKILL::EFFECT *pEffect, int level ) override;
	virtual void OnEventFirstApplyEffect( XSKILL::XSkillDat *pDat, XSKILL::XSkillUser* pCaster, XSKILL::EFFECT *pEffect, int level ) override;
	virtual bool OnInvokeSkill( _tstring& strOut, 
								XSKILL::XSkillDat *pDat,
								const XSKILL::EFFECT *pEffect,
								XSKILL::XSkillReceiver* pTarget,
								int level ) override;
	// SKILL
	//////////////////////////////////////////////////////////////////////////
	void OnBeforeFrameMove();
	virtual void FrameMove( float dt ) override;
	void FrameMoveAI( float dt );
	void FrameMoveLive( float dt );
	virtual void Draw( const XE::VEC2& vPos, float scale, float alpha=1.f ) override;
	virtual XE::VEC2 DrawName( const XE::VEC2& vPos, float scaleFactor, float scale, const XE::VEC2& vDrawHp ) { return vPos; }
	void DrawShadow( const XE::VEC2& vPos, float scale );
	virtual void Release();
	void OnEventCreateSfx( XSprObj *pSprObj, XBaseKey *pKey, float lx, float ly, float scale, LPCTSTR szSpr, ID idAct, xRPT_TYPE typeLoop, float secLifeTime, BOOL bTraceParent, float fAngle, float fOverSec );
//	virtual void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, ID idEvent, float fAngle, float fOverSec ) override;
	virtual void OnEventHit( const xSpr::xEvent& event ) override;
	virtual void CreateFSMObj();
	XE::VEC2 GetAttackedPos( const UnitPtr& unitAttacker );
	/// this에 비해 pTarget이 오른쪽에 있는가.
	BOOL IsRight( XBaseUnit *pTarget ) {
		if( pTarget->GetvwPos().x > GetvwPos().x )
			return TRUE;
		return FALSE;
	}
	void TransformByObj( XE::VEC2 *pOutLocal );
	void TransformByObj( XE::VEC3 *pOutLocal );
	/// 추가로 디버깅용 스트링을 만들게 있다면 이핸들러를 이용한다.
	virtual void OnDebugStr( _tstring& str ) {}
	virtual void OnHitEventSkill( const XE::VEC3& vwSrc ) {}
	virtual int OnEndNormalAttackMotion() { return 0; }
// 	XObjArrow* ShootArrow( LPCTSTR szIdentifier, LPCTSTR szSpr, ID idAct, UnitPtr spTarget, const XE::VEC3& vwSrc, float factorSpeed = 8.f );
	XObjArrow* ShootArrow( UnitPtr& spTarget, 
							const XE::VEC3& vwSrc,
							const XE::VEC3& vwDst,
							float damage,
							bool bCritical );
	virtual void OnArriveBullet( XObjBullet *pBullet,
								const UnitPtr& spAttacker,
								const UnitPtr& spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam ) override;

	virtual bool OnDie( XBaseUnit *pAttacker );
	// 현재 바인드타겟을 추적중인가.
	BOOL IsBindChase() {
		return ((GetpFSM()->GetidFSM() == XFSMBase::xFSM_CHASE) && IsBindTarget());
	}
	/**
	 @brief 현재 공속 딜레이중인가
	*/
	BOOL IsNowAttackDelay() {
		return m_timerAttack.IsOn() && m_timerAttack.IsOver() == FALSE;
	}
	/**
	 @brief 공속딜레이가 끝났는가
	*/
	BOOL IsOverAttackDelay() {
		return m_timerAttack.IsOver();
	}
	/**
	 @brief 공속딜레이를 시작한다.
	*/
	void StartAttackDelay( UnitPtr spTarget = nullptr, float secMin = 0.f );
	/**
	 @brief 공속딜레이를 끈다.
	*/
	void StopAttackDelay() {
		m_timerAttack.Off();
	}
	// 공격모션을 정하기전 호출된다.
	virtual int OnBeforeAttackMotion() { return 0; }
	/// 공격모션이 시작되고난 직후 호출된다.
	virtual void OnAfterAttackMotion( XSKILL::xtJuncture junc );
	XObjLoop* CreateSfxObj( XSKILL::xtPoint createPoint,
									LPCTSTR szSpr,
									ID idAct,
									float secPlay,
									BOOL bScale = FALSE,
									float wAdjZ = 0.f,
									const XE::VEC2& vPos = XE::VEC2(0) );
	float GetScaleFactor();
	XFSMStun* ChangeFSMStun( float secStun ) {
		XFSMStun *pFsm = static_cast<XFSMStun*>( ChangeFSM( XFSMBase::xFSM_STUN ) );
		pFsm->Init( secStun );
		return pFsm;
	}
	UnitPtr GetNearUnit( float meter, BIT bitCamp, bool bFindOutRange );
	WorldObjPtr AddObj( XEBaseWorldObj *pNewObj );
	bool IsCritical( UnitPtr spTarget );
	bool IsEvade( XSKILL::xtDamage typeDamage, XBaseUnit *pAttacker );
	bool IsHit( UnitPtr spTarget );
	float hardcode_OnToDamage( UnitPtr& spTarget, float damage, XGAME::xtMelee typeMelee );
	void OnDamage( XBaseUnit *pAttacker, float damage, BOOL bCritical, XSKILL::xtDamage typeDamage, const BIT bitAttrHit );
	void OnAttackToDefender( XBaseUnit *pAttacker, float damage, BOOL bCritical, float ratioPenetration, XSKILL::xtDamage typeDamage );
	void OnStartBattle();
	void OnAfterStartBattle();
	// pDst
	float GetDistSq( XBaseUnit *pDst ) {
		float distSq = (pDst->GetvwPos() - GetvwPos()).Lengthsq();
		return distSq;
	}
	/// this와 pDst간의 거리가 meter안쪽이냐.
	BOOL IsInDistToUnit( XBaseUnit *pDst, float meter ) {
		float distsq = GetDistSq( pDst );
		float cood = xMETER_TO_PIXEL(meter);
		if( distsq < cood * cood )
			return TRUE;
		return FALSE;
	}
	virtual void ShootRangeAttack( UnitPtr& spTarget,
									const XE::VEC3& vwSrc,
									const XE::VEC3& vwDst,
									float damage,
									bool bCritical,
									const std::string& strType,
									const _tstring& strSpr ) {
		// 원거리유닛 아닌것이 이쪽으로 들어올수 없다.
		XBREAKF(1, "not ranged obj");		
	}
	void SetTarget( UnitPtr& spTarget ) {
		m_spTarget = spTarget;
		if( m_spTarget != nullptr )
			m_spTarget->AddCntTargeting( 1 );
	}
	float GetAddRatioDamage( UnitPtr spTarget );
	void DoHeal( float addHp );
	XSKILL::XBuffObj* GetAdjParamByBuff( float *pOutAdj, LPCTSTR sid, XGAME::xtParameter adjParam );
	float GetScaleUnitOrg() {
		return GetPropScale();
	}
	// 현재 맞은 수의 100에 대한 비율
	float GetRatioHit() {
		float ratio = m_cntHit / 100.f;
		if( ratio > 1.f )
			ratio = 1.f;
		return ratio;
	}
	float GetInvokeRatioByBuff( XGAME::xtUnit unit, LPCTSTR sid );
	float GetVampiricRatio();
	UnitPtr GetSacrificeGolemInAttacker();
	void DoDie( XBaseUnit *pUnitAttacker );
	bool IsInvisible() {
		return IsState( XGAME::xST_INVISIBLE ) == TRUE;
	}
	XStatistic* GetpStatObj();
	float AddDamageDeal( float damage );
	bool IsAI() {
		return m_bitFlags & xFL_AI;
	}
	virtual void OnArriveTarget( UnitPtr spUnit, const XE::VEC3& vwDst );
	// AI가 작동하기 시작하거나 꺼지면 호출된다.
	virtual void OnAISet( bool bSet ) {}
	virtual _tstring GetstrIds() {
		return _tstring();
	}
	// 유닛의 크기비용
	virtual int GetSizeCost();
protected:
	void CreateDamageNumber( float damage, BIT bitAttrHit );
	void OnApplyEffectAdjParam( XSKILL::XSkillUser *pCaster, XSKILL::XSkillDat* pSkillDat, const XSKILL::EFFECT *pEffect, float abilMin ) override;
	void DrawDebugStr( XE::VEC2* pvLT, XCOLOR col, float sizeFont, const _tstring& strDebug );
// 	bool IsCheatFilterPlayer() const;
// 	bool IsCheatFilterHero() const;
	bool IsCheatFiltered();
	friend class XFSMBase;
friend class XFSMIdle;
};

