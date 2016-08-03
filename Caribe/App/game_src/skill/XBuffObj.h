#pragma once
#include "SkillType.h"
//#include "XSkillDat.h"
#include "etc/Timer.h"
#include "XLuaSkill.h"
#include "xe.h"
#include "XPool.h"
#include "XEffect.h"

class CTimer;
XE_NAMESPACE_START( XSKILL )

class XSkillDat;

// 동적할당 되는 효과 구조체객체
struct EFFECT_OBJ {
	int		Active;
	EFFECT	m_effect;			// 원본의 복사본
	CTimer	timerDuration;		// 지속시간타이머
	CTimer	timerDOT;			// 도트 타이머
	int		cntDot;				// 도트카운터(지속시간3초 도트간격1초면 최대 도트카운터는 3)
	ID		idCastSFX;			// 시전SFX가 있었다면 나중에 꺼주기 위해 아이디를 보관해둠
	ID		idInvokeSFX;			
	EFFECT_OBJ( EFFECT *pEffect ) {
		Active = 1;
		m_effect = *pEffect;
		idCastSFX = idInvokeSFX = 0;
		cntDot = 0;
	}
};
//	} EFFECT_OBJ;
typedef std::list<EFFECT_OBJ *>			EFFECT_OBJ_LIST;
typedef std::list<EFFECT_OBJ *>::iterator	EFFECT_OBJ_ITOR;


class XSkillUser;
class XSkillReceiver;
class XBuffObj;
class XBuffObj //: public XMemPool<XBuffObj>
{
	XDelegateSkill *m_pDelegate;
	ID m_idBuff;			// 고유아이디
	XSkillDat *m_pDat;			// 스킬데이타 참조용 포인터(파괴금지)
	XSkillUser *m_pCaster;		// 시전자
	BIT m_bitCampCaster = 0;		// 시전자 진영(땜빵)
	XSkillReceiver *m_pOwner;		// 
	XE::VEC2 m_vCastPos;				// 지역시전으로 했을때 그 중심좌표
	std::list<EFFECT_OBJ*> m_listEffectObjs;		// 스킬효과 오브젝트
	BOOL m_bDestroy;			// this를 삭제하라
	XLuaSkill *m_pluaScript;			// 스킬당 1개의 루아스크립트 객체
	int	m_nCount;			// 생성시부터 1씩 계속 증가
	XList<ID> m_listImmunity;				// 발동스킬 발동시 면역된 대상들 리스트
	int m_Level;
	ID m_idCallerSkill = 0;			// 이 버프객체를 생성한 상위 스킬의 아이디
	XSkillDat* m_pDatCaller = nullptr;		// idCallerSkill의 아이디
	int m_numApply = 0;					// 효과 적용횟수
#ifdef _CLIENT
	XSurface *m_psfcIcon = nullptr;		///< 스킬 아이콘
	ID m_idSfx = 0;							// 지속효과 sfx객체의 아이디
#endif
	void Init() {
		m_pDelegate = nullptr;
		m_idBuff = XE::GenerateID();
		m_pDat = nullptr;
		m_pCaster = nullptr;
		m_pOwner = nullptr;
		m_bDestroy = FALSE;
		m_pluaScript = nullptr;
		m_nCount = 0;
		m_Level = 0;
	}
	void Destroy();
protected:
	GET_ACCESSOR( EFFECT_OBJ_LIST&, listEffectObjs );
public:
	XBuffObj( XDelegateSkill *pDelegate );
	XBuffObj( XDelegateSkill *pDelegate, 
				XSkillUser *pCaster, 
				XSkillReceiver *pOwner, 
				XSkillDat *pSkillDat, 
				int level,
				const XE::VEC2& vPos,
				ID idCallerSkill );
	virtual ~XBuffObj() { Destroy(); }
	// get/set/is
	GET_ACCESSOR( XDelegateSkill*, pDelegate );
	ID GetidBuff() const { return m_idBuff; }
	GET_ACCESSOR_CONST( const XSkillDat*, pDat );
	inline XSkillDat* GetpDatMutable() {
		return m_pDat;
	}
	GET_SET_ACCESSOR_CONST( ID, idSfx );
	GET_ACCESSOR_CONST( ID, idCallerSkill );
	LPCTSTR GetstrIconByCaller() const;
//	GET_ACCESSOR_CONST( ID, idCaster );
	GET_ACCESSOR_CONST( BIT, bitCampCaster );

	void AddAbilMin( float val );
	GET_ACCESSOR( XSkillUser*, pCaster );
	GET_SET_ACCESSOR_CONST( BOOL, bDestroy );
	GET_SET_ACCESSOR( XLuaSkill*, pluaScript );
	GET_ACCESSOR( XSkillReceiver*, pOwner );
	GET_ACCESSOR_CONST( int, Level );
	/**
	 @brief 최초 프로세스 상태인가.
	*/
	inline BOOL IsFirstProcess() const {
		return m_nCount == 0;
	}
	ID GetidSkill() const;
// 	inline ID getid() const {
// 		return GetidSkill();
// 	}
	//
	EFFECT_OBJ* FindEffect( EFFECT *pEffect );
	EFFECT_OBJ* AddEffect( EFFECT *pEffect );		// 이펙트오브젝트를 추가한다
	int ProcessApplyEffect( XSkillReceiver *pInvokeTarget, XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj, BOOL bSetTimerDOT=TRUE );		
	int Process( XSkillReceiver *pOwner );			// 버프/디버프/도트류로 걸려있는 스킬들은 매루프 이걸 실행한다. pOwner:버프소유자
	int IsClearCondition( EFFECT_OBJ *pEffObj, XSkillDat *pSkillDat, XSkillReceiver *pOwner );		// 효과 해제조건인가
	void CreateInvokeSfx( EFFECT *pEffect, XSkillReceiver *pInvokeTarget );
	// 시전대상에게 시전후 최초 발동대상들에게 발동되는 전용 발동함수. DOT초기화를 포함.
	void FirstApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner );
	void PersistApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner );
	void ExecuteInvokeScript( XSkillReceiver *pInvokeTarget, const char *szScript  );		// szScript를 실행함
	void ExecuteScript( XSkillReceiver *pTarget, const char *szScript  );
	XLuaSkill* CreateScript();		// 루아객체를 생성한다. 이미 있으면 다시 생성하지 않는다.
	void InvokeDoScript( XLua *pLua, XSkillReceiver *pInvokeTarget, const char *szScript );	// 발동대상에게 스크립트를 실행한다
	void OnFirstApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj );	// 이 효과가 대상에게 처음 적용되는 시점. bInnerApply: Process()에서 호출된건지 ApplyBuff(외부적용상황)에서 불려진건지 플래그
	void OnEndApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj );	// 이 효과가 해제될때
	void OnHitFromAttacker( const XSkillReceiver *pAttacker,
							XSkillReceiver *pDefender,
							XSkillReceiver *pOwner,
							xtDamage typeDamage );
	void OnAttackToDefender( XSkillReceiver *pDefender, 
							float damage, 
							BOOL bCritical, 
							float ratioPenetration, 
							XSKILL::xtDamage typeDamage );
	void OnSkillEventKillEnemy( XSkillReceiver *pOwner, ID idDead );
	void OnCastedEffect( XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj );
	void OnEventJunctureCommon( ID idEvent, DWORD dwParam = 0, const XSkillReceiver *pRecvParam = nullptr );
	void OnEventBeforeAttack( XSKILL::xtJuncture event );
	void OnEventInvokeFromSkill( XSkillDat *pFromSkill, const EFFECT *pFromEffect, XSkillUser *pCaster, XSkillReceiver *pBaseTarget );
	// 효과발동 동작의 일원화를 위한 최종 추상화 함수
	void ApplyInvokeEffect( EFFECT *pEffect, 
							int level, 
							XSkillReceiver *pOwner, 
							const char *cScript, 
							BOOL bCreateSfx,
							XVector<XSkillReceiver*> *pOutAryIvkTarget = nullptr, 
							BOOL bGetListMode=FALSE );
	void AddImmunity( ID idObj ) {
		m_listImmunity.Add( idObj );
	}
	BOOL FindImmunity( ID idObj ) {
		return m_listImmunity.Find( idObj );
	}
	// virtual
	virtual BOOL OnDOTTimerSet( const EFFECT *pEffect ) { return TRUE; }		// 도트효과적용후 도트타이머를 리셋시킬지 여부
	const EFFECT_LIST& GetEffectList() const;
	const EFFECT* GetEffectIndex( int idx ) const;
	int GetNumEffect() const;
	float GetAbilMinbyLevel( int idx = 0 ) const;
	float GetInvokeRatioByLevel() const;
	float GetInvokeSizeByLevel() const;
	//	bool DoDiceInvokeRatio( EFFECT *pEffect );
	///

}; // XbuffObj


XE_NAMESPACE_END




