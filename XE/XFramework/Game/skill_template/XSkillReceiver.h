//#ifndef __XSkillReceiver_H__
//#define __XSkillReceiver_H__
#pragma once
#ifdef _XSKILL_SYS
#include "xLib.h"
#include "SkillDef.h"
#include "XBuffObj.h"
#include "XLuaSkill.h"
#include "XList.h"
#include "XAdjParam.h"
#include "SkillType.h"

class XECompCamp;
NAMESPACE_XSKILL_START
// 스킬의 효과를 받을 수 있는 베이스오브젝트
class XSkillReceiver : public XAdjParam 
{
	XList2<XBuffObj>		m_listSkillRecvObj;							// 버프리스트
	float m_MP, m_MaxMP;						// 보유마나. 구조가 이상하다 마나를 보유할수 있는건 Recv타겟 Use타겟 모두 쓰는건데
	void Init() {
		m_MP = m_MaxMP = 100;
	}
	void Destroy() {
		XLIST2_DESTROY( m_listSkillRecvObj, XBuffObj* );
	}	
public:
//	XSkillReceiver() { 
//		Init(); 
//	}
	/// 최대버프 개수를 지정한다.
	XSkillReceiver( int maxBuff, int maxParam ) 
	: XAdjParam(maxParam ) {
		Init();
		m_listSkillRecvObj.Create( maxBuff );
	}
	virtual ~XSkillReceiver() { Destroy();	}
	// get/set
	GET_ACCESSOR( XList2<XBuffObj>&, listSkillRecvObj );
	// this에게 pEffect의 발동효과를 적용시킨다. pInvoker는 효과를 발동시킨자다. 
	int ApplyInvokeEffect( XSkillUser *pCaster, XSkillReceiver* pInvoker, XBuffObj *pBuffObj, EFFECT *pEffect );		// 효과적용
	XBuffObj* FindBuffSkill( ID idDat );
	// 시전자와 스킬id가  같은 버프를 찾음
	XBuffObj* FindBuffSkill( ID idDat, XSkillUser *pCaster );
	// 

	BOOL FindBuff( XBuffObj *pSkillRecvObj ) {		// this에 pSkillRecvObj버프가 걸려있는지 확인
		return m_listSkillRecvObj.Find( pSkillRecvObj );
	}
//	int ApplyBuff( XBuffObj_List *plistOutApplyBuff, XSkillReceiver *pCastingTarget, XSkillReceiver *pInvokeTarget, BOOL bFirst );	// 
	int FrameMove( float dt );
	
	// virtual
	virtual ID GetId( void ) = 0;
	virtual int			ApplyEffectNotAdjParam( XSkillUser* pCaster, EFFECT *pEffect );	// 비보정파라메터에 대한 효과적용
	virtual int			ApplyEffectAdjParam( EFFECT *pEffect );		// 보정파라메터에 대한 효과적용
//	virtual BOOL		IsInvokeAble( XSkillReceiver *pCastingTarget, const EFFECT *pEffect ); // { XBREAKF( 1, "%s: %s가 구현되지 않았습니다", _T(__FUNCTION__), GetObjName() ); return 0; }
	virtual BOOL		IsCastAble( XSkillUser *pCaster, const EFFECT *pEffect ); // { XBREAKF( 1, "%s: %s가 구현되지 않았습니다", _T(__FUNCTIOIN__), GetObjName() ); return 0; }
	virtual BOOL		IsInvoking( XBuffObj *pSkillRecvObj ) { return FindBuff( pSkillRecvObj ); }
	virtual LPCTSTR	GetObjName( void ) { return _T("이름없음"); }
	virtual int			AddSkillRecvObj( XBuffObj *pSkillRecvObj );		// 버프리스트추가
	virtual void		DestroySkillRecvObj( XBuffObj *pSkillRecvObj ) { SAFE_DELETE( pSkillRecvObj ); }		// 버프리스트삭제
	virtual int			OnClearSkill( XSkillDat *pSkillDat, EFFECT_OBJ *pEffObj ) { return 1; }		// 버프효과가 끝나면 호출됨
	virtual void		OnFirstApplyBuff( XBuffObj *pSkillRecvObj, BOOL bInnerApply ) {}	// 이 대상에 pSkillRecvObj버프(효과아님)가 최초 적용되는 순간
	virtual void		OnEndApplyBuff( XBuffObj *pSkillRecvObj ) {}		// 이 대상에 pSkillRecvObj버프(효과아님)가 사라지는 순간
	virtual void		OnEndApplyBuffFromCastingTarget( XBuffObj *pSkillRecvObj ) {}		// 이 대상에 pSkillRecvObj버프(효과아님)가 사라지는 순간 캐스팅타겟에게서 버프가 사라짐을 알림.
	virtual void		OnAddSkillRecvObj( XBuffObj *pSkillRecvObj, EFFECT *pEffect ) {}		// 이대상에게 버프스킬이 추가된 직후 호출된다.
/*
	virtual float	GetMP( void ) {
		float val = m_MP;		
		return CalcAdjParam( val, xADJ_MP );
	}
	virtual SET_ACCESSOR( float, MP );
	virtual float GetMaxMP( void ) {
		float val = m_MaxMP;		
		return CalcAdjParam( val, xADJ_MAX_MP );
	}

	virtual SET_ACCESSOR( float, MaxMP );
	virtual float AddMP( float mp ) {
		m_MP += mp;
		if( mp > 0 ) {
			if( GetMP() > GetMaxMP() )
				SetMP( GetMaxMP() );
		} else 
		if( mp < 0 ) {
			if( GetMP() < 0 )
				SetMP( 0 );
		}
		return GetMP();
	}
*/
	//--------------------- script
//	virtual XLuaSkill* CreateScript( void ) { return new XLuaSkill; } 		// XSkillUser로 감 XLua 객체를 생성하고 전역변수와 API들을 등록하여 돌려준다
	// XCivLua에 모두 통합됨
//	virtual void RegisterScript( XLua *pLua ) {		// 클래스/상속관계/API등을 등록한다
//		pLua->Register_Class<XSkillReceiver>( "XSkillReceiver" );
//	}	
	virtual BOOL IsApplyBuff( XBuffObj *pBuff, XSkillReceiver *pCastingTarget, XSkillReceiver *pInvokeTarget ) { return TRUE;}
	// secLife==0.f는 1번플레이
	virtual ID OnCreateInvokeSFX( XBuffObj *pSkillRecvObj, EFFECT *pEffect, XSkillUser *pCaster, const XE::VEC2 *pvPos, LPCTSTR szSFX, ID idAct, xtPoint pointEffect, float secLife=0.f ) { return 0;}
	virtual void OnDestroySFX( XBuffObj *pSkillRecvObj, ID idSFX ) {}
	virtual void OnPlaySoundRecv( ID id ) { XLOG("경고: 가상함수가 구현되지 않았음"); }
	virtual BOOL SetSkillState( XSkillUser *pCaster, XSkillReceiver* pInvoker, EFFECT *pEffect ) { return TRUE; }
	// 지속형 효과에서 pEffect가 처음 발동되면 호출된다.
	virtual void OnStartSkillEffect( XSkillUser *pCaster, XSkillReceiver* pInvoker, EFFECT *pEffect ) {}
	virtual void OnStartSkillState( XSkillUser *pCaster, XSkillReceiver* pInvoker, EFFECT *pEffect ) {}
	virtual void RegisterInhValRecv( XLua *pLua, const char *szVal ) {} 
	// this에 fDamage만큼의 데미지를 준다
/*
	virtual int OnSkillDamage( XSkillUser* pAttacker, float val, xtValType opType ) {
		XBREAKF(1,"경고: 가상함수가 구현되지 않았음");
		return 0;
	}		
	// this에 fHP만큼의 체력을 회복시킴
	virtual int OnSkillHeal( float val, xtValType opType ) {
		XBREAKF(1,"경고: 가상함수가 구현되지 않았음");
		return 0;
	}		
*/
	/**
	 @brief 비보정 파라메터의 효과가 적용될때 호출된다. 비보정파라메터는 음수값을 쓴다.
	 @return 효과를 적용했으면 1을 리턴하고 그렇지 않으면 0을 리턴시켜야 한다.
	*/
	virtual int OnApplyEffectNotAdjParam( XSkillUser *pCaster, EFFECT *pEffect, float abilMin, float abilMax ) = 0;
	/**
	 스킬능력치 추가증폭. 
	 추가될 양을 리턴해야한다. 예를들어 100%가 더 추가되고 싶다면 1.0을,
	 20이 더 추가되고 싶다면 20을 리턴하면 된다.
	*/
	virtual void OnSkillAmplify( XSkillReceiver *pIvkTarget, const EFFECT *pEffect, float *pOutRatio, float *pOutAdd ) { 
		if( pOutRatio )
			*pOutRatio = 0.f;
		if( pOutAdd )
			*pOutAdd = 0;
	}
	// pure virtual
	// szVal변수를(보통 invokeObj같은) 상속된 하위클래스형으로 루아변수로 등록한다. 말하자면 XSkillReceiver타입의 invokeObj가 아니고 XUnit타입의 invokeObj로 등록해야한다. 이런 코드 사용-> pLua->RegisterVar( szVal, this ); 	
	virtual XECompCamp& GetCamp( void ) = 0;		///< this의 진영을 리턴
//	virtual xtSide GetFriendship( void ) = 0;		///< this의 Friendship을 리턴
	virtual xtPlayerType GetPlayerType( void ) = 0;		///< this의 플레이어 타입을
	virtual XE::VEC2 GetCurrentPosForSkill( void ) = 0;
	virtual int GetGroupList( XArrayLinearN<XSKILL::XSkillReceiver*, 100> *pAryOutGroupList, 
							XSKILL::XSkillObj *pUseSkill, 
							XSKILL::EFFECT *pEffect ) { 
		XBREAKF(1, "아직 구현되지 않았습니다.");
		return 0; 
	}
//	virtual ID GetReceiverID( void ) = 0;
	// handler
	// 누군가에게 맞으면 이 이벤트를 호출해준다.
	void OnMeleeHitFromAttacker( ID idAttacker );
	void OnRangeHitFromAttacker( ID idAttacker );
	void OnAttackToDefender( ID idDefender );
	void OnSkillEventKillEnemy( ID idDead );
};

NAMESPACE_XSKILL_END


#endif // xskill_sys
