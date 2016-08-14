#pragma once
#include "xLib.h"
#include "SkillDef.h"
//#include "XBuffObj.h"
#include "XLuaSkill.h"
#include "XList.h"
#include "XAdjParam.h"
#include "SkillType.h"

class XECompCamp;
XE_NAMESPACE_START( XSKILL )
class XBuffObj;
struct EFFECT_OBJ;
class XSkillSfx;
class XSkillDat;
// 스킬의 효과를 받을 수 있는 베이스오브젝트
class XSkillReceiver : public XAdjParam 
{
	XList4<XBuffObj*> m_listSkillRecvObj;							// 버프리스트
	XSkillReceiver *m_pAttacker = nullptr;		// this를 공격한자.
	void Init() {	}
	void Destroy();
public:
	/// 최대버프 개수를 지정한다.
	XSkillReceiver( int maxBuff, int maxParam, int maxState );
	virtual ~XSkillReceiver() { Destroy();	}
	// get/set
	GET_ACCESSOR_CONST( const XList4<XBuffObj*>&, listSkillRecvObj );
	GET_ACCESSOR_CONST( const XSkillReceiver*, pAttacker );
	SET_ACCESSOR( XSkillReceiver*, pAttacker );
	// this에게 pEffect의 발동효과를 적용시킨다. pInvoker는 효과를 발동시킨자다. 
	int ApplyInvokeEffect( XSkillDat *pSkillDat, XSkillUser *pCaster, XSkillReceiver* pInvoker, XBuffObj *pBuffObj, const EFFECT *pEffect, int level );		// 효과적용
	XBuffObj* FindBuffSkill( ID idDat );
	// 시전자와 스킬id가  같은 버프를 찾음
	XBuffObj* FindBuffSkill( ID idDat, XSkillUser *pCaster );
	XBuffObj* FindBuffSkill( LPCTSTR idsSkill );
	bool FindBuff( XBuffObj *pSkillRecvObj ) const;
	int FrameMove( float dt );
	// virtual
	virtual ID GetId( void ) = 0;
	int	ApplyEffectNotAdjParam( XSkillDat *pSkillDat, XSkillUser* pCaster, const EFFECT *pEffect, int level );	// 비보정파라메터에 대한 효과적용
	int	ApplyEffectAdjParam( XSkillDat *pSkillDat, XSkillUser *pCaster, const EFFECT *pEffect, int level, XBuffObj *pBuffObj );		// 보정파라메터에 대한 효과적용
	virtual BOOL IsInvoking( XBuffObj *pSkillRecvObj ) { return FindBuff( pSkillRecvObj ); }
	virtual LPCTSTR	GetObjName( void ) { return _T("이름없음"); }
	virtual int	AddSkillRecvObj( XBuffObj *pSkillRecvObj );		// 버프리스트추가
//	virtual void DestroySkillRecvObj( XBuffObj *pSkillRecvObj ) { SAFE_DELETE( pSkillRecvObj ); }		// 버프리스트삭제
	virtual int	OnClearSkill( XBuffObj* pBuffObj, XSkillDat *pSkillDat, EFFECT_OBJ *pEffObj ) { return 1; }		// 버프효과가 끝나면 호출됨
	virtual void OnFirstApplyBuff( XBuffObj *pSkillRecvObj, BOOL bInnerApply ) {}	// 이 대상에 pSkillRecvObj버프(효과아님)가 최초 적용되는 순간
	virtual void OnEndApplyBuff( XBuffObj *pSkillRecvObj ) {}		// 이 대상에 pSkillRecvObj버프(효과아님)가 사라지는 순간
	virtual void OnEndApplyBuffFromCastingTarget( XBuffObj *pSkillRecvObj ) {}		// 이 대상에 pSkillRecvObj버프(효과아님)가 사라지는 순간 캐스팅타겟에게서 버프가 사라짐을 알림.
	virtual void OnAddSkillRecvObj( XBuffObj *pSkillRecvObj, EFFECT *pEffect ) {}		// 이대상에게 버프스킬이 추가된 직후 호출된다.
	virtual BOOL IsApplyBuff( XBuffObj *pBuff, XSkillReceiver *pCastingTarget, XSkillReceiver *pInvokeTarget ) { return TRUE;}
	// secLife==0.f는 1번플레이
	virtual ID OnCreateInvokeSFX( XBuffObj *pSkillRecvObj, EFFECT *pEffect, XSkillUser *pCaster, const XE::VEC2 *pvPos, LPCTSTR szSFX, ID idAct, xtPoint pointEffect, float secLife=0.f ) { return 0;}
	virtual ID OnCreateSkillSfx( const XSKILL::XSkillDat *pSkillDat,
																 XSKILL::xtPoint createPoint,
																 LPCTSTR szSpr,
																 ID idAct,
																 float secPlay,
																 const XE::VEC2& vPos ) { return 0;}
	/**
	 @brief 슈팅타겟이펙트 전용 생성기
	 슈팅타겟이펙트에는 반드시 타점이 있어야 한다.
	*/
	virtual XSKILL::XSkillSfx* OnCreateSkillSfxShootTarget( XSKILL::XSkillDat *pSkillDat,
																													XSKILL::XSkillReceiver *pBaseTarget,
																													int level,
																													const _tstring& strSpr,
																													ID idAct,
																													XSKILL::xtPoint createPoint,
																													float secPlay,
																													const XE::VEC2& vPos ) {		return nullptr;	}
	inline XSKILL::XSkillSfx* OnCreateSkillSfxShootTarget( XSKILL::XSkillDat *pSkillDat,
																												 XSKILL::XSkillReceiver *pBaseTarget,
																												 int level,
																												 const xEffSfx& effSfx,
																												 float secPlay,
																												 const XE::VEC2& vPos ) {
		return OnCreateSkillSfxShootTarget( pSkillDat, pBaseTarget, level, effSfx.m_strSpr, effSfx.m_idAct, effSfx.m_Point, secPlay, vPos );
	}

	ID CreateSfx( XSkillDat *pSkillDat,
									const _tstring& strEffect,
									ID idAct,
									xtPoint pointSfx,
									float secPlay,
									const XE::VEC2& vPos = XE::VEC2( 0 ) );
	ID CreateSfx( XSkillDat *pSkillDat, const xEffSfx& effSfx, float secPlay, const XE::VEC2& vPos = XE::VEC2() );
	virtual void OnDestroySFX( XBuffObj *pSkillRecvObj, ID idSFX ) {}
	virtual void OnPlaySoundRecv( ID id ) { XLOG("경고: 가상함수가 구현되지 않았음"); }
	/**
	 @brief 상태이상 변화에 따른 처리를 해야한다. idxState와 flag를 기반으로 오브젝트의 적절한 상태(모션등..)를 처리한다.
	*/
	virtual BOOL OnApplyState( XSkillUser *pCaster, XSkillReceiver* pInvoker, const EFFECT *pEffect, int idxState, BOOL flagState ) { return TRUE; }
	/**
	 @brief 상태적용이 최초로 이루어졌을때.
	 근데 넉백같은건 지속시간이 없기때문에 이게 호출되지 않음. 버프객체가 생성되는 시점에서 호출되어야 함.
	*/
	virtual BOOL OnFirstApplyState( XSkillUser *pCaster, XSkillReceiver* pInvoker, const EFFECT *pEffect, int idxState, BOOL flagState, int level ) { return TRUE; }
	// 지속형 효과에서 pEffect가 처음 발동되면 호출된다.
	virtual void OnStartSkillEffect( XSkillUser *pCaster, XSkillReceiver* pInvoker, EFFECT *pEffect ) {}
	virtual void OnStartSkillState( XSkillUser *pCaster, XSkillReceiver* pInvoker, EFFECT *pEffect ) {}
	virtual void RegisterInhValRecv( XLua *pLua, const char *szVal ) {} 
	// this에 fDamage만큼의 데미지를 준다
	/**
	 @brief 비보정 파라메터의 효과가 적용될때 호출된다. 비보정파라메터는 음수값을 쓴다.
	 @return 효과를 적용했으면 1을 리턴하고 그렇지 않으면 0을 리턴시켜야 한다.
	*/
	virtual int OnApplyEffectNotAdjParam( XSkillUser *pCaster
																			, XSKILL::XSkillDat* pSkillDat
																			, const EFFECT *pEffect
																			, float abilMin ) = 0;
	// 스킬효과의 adjParam이 적용될때 핸들러.
	virtual void OnApplyEffectAdjParam( XSkillUser *pCaster
																	, XSKILL::XSkillDat* pSkillDat
																	, const EFFECT *pEffect
																	, float abilMin ) {}
	/**
	 스킬능력치 추가증폭. 
	 추가될 양을 리턴해야한다. 예를들어 100%가 더 추가되고 싶다면 1.0을,
	 20이 더 추가되고 싶다면 20을 리턴하면 된다.
	*/
	virtual void OnSkillAmplifyReceiver( XSkillDat *pDat, XSKILL::XSkillReceiver *pIvkTarget, const XSKILL::EFFECT *pEffect, float *pOutRatio, float *pOutAdd ) { 
		if( pOutRatio )
			*pOutRatio = 0.f;
		if( pOutAdd )
			*pOutAdd = 0;
	}
	// pure virtual
	// szVal변수를(보통 invokeObj같은) 상속된 하위클래스형으로 루아변수로 등록한다. 말하자면 XSkillReceiver타입의 invokeObj가 아니고 XUnit타입의 invokeObj로 등록해야한다. 이런 코드 사용-> pLua->RegisterVar( szVal, this ); 	
	virtual const XECompCamp& GetCamp( void ) const = 0;		///< this의 진영을 리턴
	virtual int GetGroupList( XVector<XSKILL::XSkillReceiver*> *pAryOutGroupList, 
							XSKILL::XSkillDat *pSkillDat, 
							const XSKILL::EFFECT *pEffect,
							xtGroup typeGroup = xGT_ME ) { 
		XBREAKF(1, "아직 구현되지 않았습니다.");
		return 0; 
	}
//	virtual ID GetReceiverID( void ) = 0;
	// handler
	// 누군가에게 맞으면 이 이벤트를 호출해준다.
	void OnHitFromAttacker( const XSkillReceiver *pAttacker, xtDamage typeDamage );
	virtual void OnAttackMelee( const XSkillReceiver *pAttacker, xtDamage typeDamage ) {}
	void OnRangeHitFromAttacker( ID idAttacker, int level );
	void OnAttackToDefender( XSkillReceiver *pDefender, 
							float damage, 
							BOOL bCritical, 
							float ratioPenetration, 
							XSKILL::xtDamage typeDamage );
	void OnSkillEventKillEnemy( ID idDead );
	bool OnEventJunctureCommon( ID idEvent, DWORD dwParam = 0, const XSkillReceiver *pRecvParam = nullptr );
//	void OnEventBeforeAttack( XSKILL::xtJuncture junc );
	void OnEventInvokeFromSkill( XSkillDat *pFromSkill, const EFFECT *pFromEffect, XSkillUser *pCaster, XSkillReceiver *pBaseTarget );
	virtual BOOL IsInvokeAddTarget( ID idAddTarget ) const { return FALSE; }
	// this에게 pEffect가 발동이 될수 있는지 검사
	virtual BOOL IsInvokeTargetCondition( XSKILL::XSkillDat *pSkillDat, const XSKILL::EFFECT *pEffect, XSKILL::xtCondition condition, DWORD condVal ) { return TRUE; }
	virtual bool OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster, XSKILL::XBuffObj *pBuffObj, XSKILL::XSkillDat *pSkillDat, const XSKILL::EFFECT *pEffect, int level ) { return true; }
	virtual void OnEventFirstApplyEffect( XSKILL::XSkillDat *pDat, XSKILL::XSkillUser* pCaster, XSKILL::EFFECT *pEffect, int level ) {}
	/**
	 @brief 발동스킬이 발동되기전에 호출된다. false가 리턴되면 발동스킬을 발동시키지 않는다.
	 strOut에 다른 스킬의 식별자를 넣어돌려주면 그 스킬이 대신 발동된다.
	*/
	virtual bool OnInvokeSkill( XSKILL::XSkillDat *pDat,
															const XSKILL::EFFECT *pEffect,
															XSKILL::XSkillReceiver* pTarget,
															int level,
															_tstring* pstrOut ) { return true; }
	virtual BOOL IsLive( void ) { return TRUE; }
	// GetInvokeTarget전 invokeTarget을 한번더 가공한다.
	virtual XSKILL::xtInvokeTarget 
	OnGetInvokeTarget( XSKILL::XSkillDat *pDat, 
										const XSKILL::EFFECT *pEffect,
										XSKILL::xtInvokeTarget invokeTarget ) {
		return invokeTarget;
	}
};

XE_NAMESPACE_END


