#pragma once
#include "SkillType.h"
#include "XSkillDat.h"
#include "etc/Timer.h"
#include "XLuaSkill.h"
#include "xe.h"
#include "XPool.h"

class CTimer;
NAMESPACE_XSKILL_START
// 동적할당 되는 효과 구조체객체
//	typedef struct tagEFFECT_OBJ
struct EFFECT_OBJ
{
	int		Active;
	EFFECT	*refEffect;			// XSkillDat에 있는 원본 이펙트 참조포인터(ref붙은건 여기서 파괴하면 안된다)
	CTimer	timerDuration;		// 지속시간타이머
	CTimer	timerDOT;			// 도트 타이머
	int		cntDot;				// 도트카운터(지속시간3초 도트간격1초면 최대 도트카운터는 3)
	ID		idCastSFX;			// 시전SFX가 있었다면 나중에 꺼주기 위해 아이디를 보관해둠
	ID		idInvokeSFX;			
	EFFECT_OBJ( EFFECT *pEffect ) {
		Active = 1;
		refEffect = pEffect;
		idCastSFX = idInvokeSFX = 0;
		cntDot = 0;
	}
};
//	} EFFECT_OBJ;
typedef list<EFFECT_OBJ *>			EFFECT_OBJ_LIST;
typedef list<EFFECT_OBJ *>::iterator	EFFECT_OBJ_ITOR;


class XSkillUser;
class XSkillReceiver;
class XBuffObj;
class XBuffObj //: public XMemPool<XBuffObj>
{
//	DWORD			timeStamp;
	XDelegateSkill *m_pDelegate;
	ID					m_idBuff;			// 고유아이디
	XSkillDat			*m_pDat;			// 스킬데이타 참조용 포인터(파괴금지)
	XSkillUser		*m_pCaster;		// 시전자
	XSkillReceiver		*m_pCastingTarget;		// 시전대상
	XE::VEC2			m_vCastPos;				// 지역시전으로 했을때 그 중심좌표
	EFFECT_OBJ_LIST m_listEffectObjs;		// 스킬효과 오브젝트
	BOOL				m_bDestroy;			// this를 삭제하라
	XLuaSkill			*m_pluaScript;			// 스킬당 1개의 루아스크립트 객체
	int					m_nCount;			// 생성시부터 1씩 계속 증가
	XList<ID> m_listImmunity;				// 발동스킬 발동시 면역된 대상들 리스트
	void Init() {
//		m_bDestroyed = 0;
//		timeStamp = CTimer::GetTime();	// 언제생성이 된건지 디버깅하려고
		m_pDelegate = NULL;
		m_idBuff = XE::GenerateID();
		m_pDat = NULL;
		m_pCaster = NULL;
		m_pCastingTarget = NULL;
		m_bDestroy = FALSE;
		m_pluaScript = NULL;
		m_nCount = 0;
	}
	void Destroy() {
		SAFE_DELETE( m_pluaScript );		// 루아가 있었다면 삭제해줌
		LIST_DESTROY( m_listEffectObjs, EFFECT_OBJ* );
//		CONSOLE( "XBuffObj::Destroy(): ID:%d Caster:0x%08x Skill:%s addr=0x%08x", m_idBuff, (int)m_pCaster, m_pDat->GetszName(), (int)this );
	}
protected:
	GET_ACCESSOR( EFFECT_OBJ_LIST&, listEffectObjs );
public:
//	BOOL m_bDestroyed;
	XBuffObj( XDelegateSkill *pDelegate ) { 
		Init(); 
		XBREAK( pDelegate == NULL );
		m_pDelegate = pDelegate;
	}
	XBuffObj( XDelegateSkill *pDelegate, 
				XSkillUser *pCaster, 
				XSkillReceiver *pCastingTarget, 
				XSkillDat *pSkillDat, 
				const XE::VEC2 *pvPos ) { 
		Init();
		XBREAK( pDelegate == NULL );
		m_pDelegate = pDelegate;
		m_pCaster = pCaster;
		m_pCastingTarget = pCastingTarget;
		m_pDat = pSkillDat;
		if( pvPos )
		m_vCastPos = *pvPos;
	}
	virtual ~XBuffObj() { Destroy(); }
	// get/set/is
	//GET_ACCESSOR( ID, id );
	GET_ACCESSOR( XDelegateSkill*, pDelegate );
	ID GetidBuff( void ) { return m_idBuff; }
	GET_ACCESSOR( XSkillDat*, pDat );
	EFFECT_LIST& GetEffectList( void ) { 
		return GetpDat()->GetlistEffects(); 
	}
	GET_ACCESSOR( XSkillUser*, pCaster );
	GET_SET_ACCESSOR( BOOL, bDestroy );
	GET_SET_ACCESSOR( XLuaSkill*, pluaScript );
	GET_ACCESSOR( XSkillReceiver*, pCastingTarget );
	ID GetidSkill( void ) {
		XBREAK( m_pDat == NULL );
		return m_pDat->GetidSkill();
	}
	//
	EFFECT_OBJ* FindEffect( EFFECT *pEffect ) {		// pEffect를 멤버로 가지고 있는 이펙오브젝트를 찾음
		LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
		{
			if( pEffect == pEffObj->refEffect )
				return pEffObj;
		}
		END_LOOP;
		return NULL;
	}
	EFFECT_OBJ* AddEffect( EFFECT *pEffect );		// 이펙트오브젝트를 추가한다
	int ProcessApplyEffect( XSkillReceiver *pInvokeTarget, XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj, BOOL bSetTimerDOT=TRUE );		
	int Process( XSkillReceiver *pOwner );			// 버프/디버프/도트류로 걸려있는 스킬들은 매루프 이걸 실행한다. pOwner:버프소유자
	int IsClearCondition( EFFECT_OBJ *pEffObj, XSkillDat *pSkillDat, XSkillReceiver *pOwner );		// 효과 해제조건인가
//	int ApplyBuff( XSkillReceiver *pCastingTarget, XSkillReceiver *pInvokeTarget, BOOL bFirst=FALSE );	// pInvokeTarget에게 this버프의 효과를 적용한다. 타이머는 건드리지 않는다
	void ApplyEffectToInvokeTargetList( 
							XArrayLinearN<XSkillReceiver*, 100>& aryInvokeTarget,
							EFFECT *pEffect, 
							XSkillReceiver *pOwner,
							const char *cScript );
	void GetInvokeTargetsAndApplyEffect( 
						XArrayLinearN<XSkillReceiver*, 100> *pOutAryInvokeTarget,
						EFFECT *pEffect, 
						XSkillReceiver *pOwner,
						const char *cScript );
	void CreateInvokeSfx( EFFECT *pEffect, XSkillReceiver *pInvokeTarget );
	// 시전대상에게 시전후 최초 발동대상들에게 발동되는 전용 발동함수. DOT초기화를 포함.
	void FirstApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner );
	void PersistApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner );
	void ExecuteInvokeScript( XSkillReceiver *pInvokeTarget, const char *szScript  );		// szScript를 실행함
	void ExecuteScript( XSkillReceiver *pTarget, const char *szScript  );
	XLuaSkill* CreateScript( void );		// 루아객체를 생성한다. 이미 있으면 다시 생성하지 않는다.
	void InvokeDoScript( XLua *pLua, XSkillReceiver *pInvokeTarget, const char *szScript );	// 발동대상에게 스크립트를 실행한다
	void OnFirstApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj );	// 이 효과가 대상에게 처음 적용되는 시점. bInnerApply: Process()에서 호출된건지 ApplyBuff(외부적용상황)에서 불려진건지 플래그
	void OnEndApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj );	// 이 효과가 해제될때
	void OnSkillEventMeleeHitFromAttacker( XSkillReceiver *pOwner, ID idAttacker );
	void OnSkillEventAttackToDefender( XSkillReceiver *pOwner, ID idDefender );
	void OnSkillEventKillEnemy( XSkillReceiver *pOwner, ID idDead );
	void OnCastedEffect( XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj );
	// 효과발동 동작의 일원화를 위한 최종 추상화 함수
	void ApplyInvokeEffect( EFFECT *pEffect, 
							XSkillReceiver *pOwner, 
							const char *cScript, 
							BOOL bCreateSfx,
							XArrayLinearN<XSkillReceiver*, 100> *pOutAryIvkTarget=NULL, 
							BOOL bGetListMode=FALSE );
	void AddImmunity( ID idObj ) {
		m_listImmunity.Add( idObj );
	}
	BOOL FindImmunity( ID idObj ) {
		return m_listImmunity.Find( idObj );
	}
	// virtual
	virtual BOOL OnDOTTimerSet( const EFFECT *pEffect ) { return TRUE; }		// 도트효과적용후 도트타이머를 리셋시킬지 여부
};


NAMESPACE_XSKILL_END




