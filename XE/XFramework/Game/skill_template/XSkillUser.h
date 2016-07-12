#pragma once
#include "xLIB.h"
#include "SkillType.h"
#include "SkillDef.h"
#include "XSkillObj.h"
#include "XSkillReceiver.h"
#include "XESkillMng.h"
#include "XLua.h"

NAMESPACE_XSKILL_START
xtTargetFilter MakeTargetFilter( const EFFECT *pEffect );
// 스킬을 사용할 수 있는 베이스 오브젝트
class XShootingObj;
class XSkillUser;
class XSkillUser 
{
	XList<XSkillObj*> m_listUseSkill;		// 보유 스킬
	XESkillMng *m_prefSkillMng;
	ID m_idEventHitter;		// 피격자, this가 때린자
	ID m_idEventAttacker;	// 공격자, this를 공격한자
	ID m_idDead;			// 사살된자, this가 죽인넘

	void Init() {
		m_prefSkillMng = NULL;
		m_idEventHitter = 0;
		m_idEventAttacker = 0;
		m_idDead = 0;
	}
	void Destroy();
	// 효과처리부는 외부에서 부를일이 없으니까 숨겼음
	xtError TryCastEffect( XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver *pBaseTarget, XSkillReceiver *pCastingTarget, const XE::VEC2 *pvPos=NULL );	// 효과발동시도
	virtual xtError UseEffect( XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver *pTarget, const XE::VEC2 *pvPos );
public:
	XSkillUser( XESkillMng *pSkillMng ) { 
		Init(); 
		m_prefSkillMng = pSkillMng;
	}
	virtual ~XSkillUser(void) { Destroy(); }
	// get/set/is
	// 보유스킬 수 리턴
	int GetNumUseSkill( void ) { return m_listUseSkill.size(); }
	// 주어진 시전방식에 해당하는 스킬들의 개수를 구함
	int GetNumUseSkill( xCastMethod castMethod ) {		
		int n = 0;
		XLIST_LOOP( m_listUseSkill, XSkillObj*, pSkillUseObj ) {
			if( pSkillUseObj->GetpDat()->GetCastMethod() == castMethod )
				n++;
		} END_LOOP;
		return n;
	}
	GET_ACCESSOR( XSkillObj_List&, listUseSkill );	// 이건 나중에 내부로 숨겨야한다
	GET_SET_ACCESSOR( ID, idEventHitter );
	GET_SET_ACCESSOR( ID, idEventAttacker );
	GET_SET_ACCESSOR( ID, idDead );
//	GET_SET_ACCESSOR( xTYPE_FRIENDSHIP, useFriendship );
//	BOOL IsBuffType( XSkillDat *pSkillDat, const EFFECT *pEffect ) { return (pSkillDat->IsPassive() || pEffect->secDuration > 0 || pEffect->secDuration < 0); }		// 이펙트가 버프타입인가
	XSkillObj* GetSkillObjByIndex( int idx ) {
		if( m_listUseSkill.size() == 0 )
			return NULL;
		return m_listUseSkill.GetFromIndex(0);
	}
	//
	XSkillObj* FindUseSkillByID( ID idSkillDat );
//	XSkillObj* FindUseSkillByName( LPCTSTR szName );
	XSkillObj* FindUseSkillByIdentifier( LPCTSTR szIdentifier );

	// pUseSkill을 보유스킬목록에 추가합니다
	void AddUseSkill( XSkillObj *pUseSkill ) {		
		XSkillObj *pExistObj = FindUseSkillByIdentifier( pUseSkill->GetpDat()->GetstrIdentifier().c_str() );
		XBREAKF( pExistObj != NULL, "warning: Already tried to add the skill. %s", pUseSkill->GetpDat()->GetstrIdentifier().c_str() );
		m_listUseSkill.push_back( pUseSkill );
	}
	// idSkillDat스킬로 사용스킬객체를 만들어서 보유한다
	XSkillObj* CreateAddUseSkillByID( ID idSkillDat );		
//	XSkillObj* CreateAddUseSkillByName( LPCTSTR szName );
	XSkillObj* CreateAddUseSkillByIdentifier( LPCTSTR szIdentifier );
	void DestroyAllSkill( void ) {
		XLIST_DESTROY( m_listUseSkill, XSkillObj* );
	}

	xtError UseSkillByID( XSkillObj **ppOutUseSkill, ID idSkillDat, XSkillReceiver *pTarget, XE::VEC2 *pvPos );
//	xtError UseSkillByName( XSkillObj **ppOutUseSkill, LPCTSTR szSkillName, XSkillReceiver *pTarget, XE::VEC2 *pvPos );
	xtError UseSkillByIdentifier( LPCTSTR szIdentifier, XSkillReceiver *pTarget, XE::VEC2 *pvPos, XSkillObj **ppOutUseSkill=NULL );
	xtError UseSkill( XSkillObj *pUseSkill, XSkillReceiver *pTarget, XE::VEC2 *pvPos );
	xtError	UseSkill( XSkillObj *pUseSkill, 
					xCastMethod castMethod,
					XSkillReceiver *pTouchTarget,	// 외부에서 커서로 타겟을 찍었을 경우
					XE::VEC2 *pvTouchPos );			// 외부에서 커서로 좌표를 찍었을 경우
	// 첫번째 보유스킬을 사용한다
	xtError UseSkill( XSkillReceiver *pTarget, XE::VEC2 *pvPos );
	xtError	UseSkill( XSkillObj *pUseSkill );
	int FrameMove( float dt );
	xtError CastEffectToCastingTarget( XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver *pBaseTarget, XSkillReceiver *pCastingTarget, const XE::VEC2 *pvPos=NULL );		// 효과발동
	// virtual
	virtual XLuaSkill* CreateScript( void ) { return new XLuaSkill; } 		// XLua 객체를 생성하고 전역변수와 API들을 등록하여 돌려준다
	virtual xtError IsValidCastingTarget( XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver *pTarget, XE::VEC2 *pvPos  );		// 시전대상 유효성 검사
	virtual int GetCastingTargetList( XArrayLinearN<XSkillReceiver*, 100> *pAryOutCastingTarget, XE::VEC2 *pvOutTarget, xtCastTarget castTarget, XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver **ppOutBaseTarget, const XE::VEC2 *pvPos );		// 시전대상얻기
	virtual int GetInvokeTarget( XArrayLinearN<XSkillReceiver*, 100> *plistOutInvokeTarget, XSkillDat *pBuff, xtInvokeTarget invokeTarget, const EFFECT *pEffect, XSkillReceiver *pCastingTarget, const XE::VEC2 *pvPos );		// 스킬발동대상얻기
	virtual void OnCoolTimeOver( XSkillObj *pUseSkill ) {}	// 쿨타임시간이 끝나면 호출된다
	// XSkillObj에 아직은 버추얼이 많지 않아서 pure로 하지 않았는데 필요해지면 pure로 다시 보내야 한다
	virtual XSkillObj* CreateSkillUseObj( XSkillDat *pSkillDat );
	virtual BIT GetFilterSideCast( XSkillReceiver *pCastingTarget, xtCastTarget targetType, xtFriendshipFilt friendshipFilter );	// 시전대상에 타겟타입으로 effectFilter로 시전/발동 될때 검색해야할 대상의 프렌드쉽 플래그
	virtual BIT GetFilterSideInvoke( XSkillUser *pCaster, XSkillReceiver *pInvoker, xtInvokeTarget targetType, xtFriendshipFilt friendshipFilter );
	virtual BOOL IsInvokeAble( XSkillReceiver *pCstTarget, const EFFECT *pEffect );
//	virtual void RegisterScriptUse( XLua *pLua ) {};	// 클래스/상속관계/API등을 등록한다
	virtual void RegisterInhValUse( XLua *pLua, const char *szVal ) = 0; ///< szVal변수를(보통 casterObj) 상속된 하위클래스형으로 루아변수로 등록한다 말하자면 XSkillReceiver타입의 invokeObj가 아니고 XUnit타입의 invokeObj로 등록해야한다. 이런 코드 사용-> pLua->RegisterVar( szVal, this ); 	
	virtual BIT GetCampUser( void ) { return 0; }		// this의 진영을 리턴. 지형속성같은건 진영이 없기때문에 pure virtual 로 하지 않았다
//	virtual xtSide GetFriendship( void ) { return xSIDE_NONE; }		// this의 Friendship을 리턴. 지형속성같은건 프레임드쉽이 없기때문에 pure virtual 로 하지 않았다
	virtual void OnPlaySoundUse( ID id ) { XLOG("경고: PlaySound가 구현되지 않았음"); }
	// pvPos를 중심으로 발동반경내의 friendship의 우호를 가진...// 이 조건에 맞는 오브젝트들 리스트 invokeNumApply만큼을 버추얼로 요청한다
/*
	virtual int GetListObjsInArea( XArrayLinearN<XSkillReceiver*, 100> *plistOutInvokeTarget, 
									const XE::VEC2 *pvPos, 
									float radius, 
									xtSide sideSearchFilt, 
									int numApply ) { return 0; }
*/
	// vStart를 시작으로 vSize크기의 사각영역내의 객체를 찾는다.
	// pvStart가 널이면 상속받는 하위 클래스에서 this의 좌표를 사용해야 한다.
	virtual int GetListObjsInRect( XArrayLinearN<XSkillReceiver*, 100> *pAryOutIvkTarget, 
									const XE::VEC2 *pvStart,	// vStart부터 vSize만큼의 직사각형이 되나, vStart의 y지점은 vSize.h의 중간지점이다.
									const XE::VEC2& vSize, 
									BIT bitSideSearchFilt, 
//									xtSide sideSearchFilt,
									int numApply ) { return 0; }
	// 시전대상을 중심으로(시전대상을 포함하거나/제외하거나) 반경내 friendship의 우호를 가진 오브젝트 invokeNumApply개의 리스트를 요청한다
	// numApply가 0이면 개수제한이 없다.
	virtual int GetListObjsRadius( XArrayLinearN<XSkillReceiver*, 100> *plistOutInvokeTarget, 
									XSkillReceiver *pCastingTarget, 
									const XE::VEC2& vCenter,
									float radius, 
									BIT bitSideFilter, 
//									xtSide bitSideFilter,
									int numApply, 
									BOOL bIncludeCenter ) { return 0; }
	// 스킬 사용이 성공했음. mp등의 깎는 처리를 할것.
	virtual void OnSuccessUseSkill( XSkillObj *pUseSkill ) {}
	// 발사체가 목표에 도착하면 호출된다.
	virtual void OnArriveShooingObj( XShootingObj *pShootObj,
									XSkillObj *pUseSkill,
									EFFECT *pEffect,
									XSkillReceiver *pCastingTarget,
									const XE::VEC2& vPos ) {}
	virtual void OnCreateCasterSfx( EFFECT *pEffect, xtPoint createPoint, LPCTSTR szSpr, ID idAct, float secPlay ) {}
	virtual XSkillReceiver* GetGroundReceiver( void ) { return NULL; }
	virtual int GetListPartyMember(XArrayLinearN<XSkillReceiver*, 100> *pAryOutIvkTarget, BIT bitSideSearchFilt, xtTargetFilter filter ) {
		XLOG("virtual 함수가 구현되지 않음");
		return 0;
	}
	// pure virtual
	/// XSkillReceiver타입까지 상속받은 하위클래스는 이 함수를 구현해야한다. 상속받지 않았다면 NULL을 리턴하게 한다
	virtual XSkillReceiver* GetThisRecv( void ) = 0;		
	// 스킬버프객체 생성. 웬만하면 하위클래스 만들어서 구현할것
private:
	/// GetTarget( ID )를 사용하기 위해 델리게이터를 지정해준다. 보통 월드다.
	virtual XSKILL::XDelegateSkill* GetpDelegate( void ) = 0;
public:
	// copy해서 쓰기 좋으라고 일부러 XSKILL을 다 붙임
	virtual XSKILL::XBuffObj* CreateSkillBuffObj( XSKILL::XSkillUser *pCaster, 
										XSKILL::XSkillReceiver *pCastingTarget, 
										XSKILL::XSkillDat *pSkillDat, 
										const XE::VEC2 *pvPos ) = 0; 
	// copy해서 쓰기 좋으라고 일부러 XSKILL을 다 붙임
	// 발사체오브젝트를 생성하고 월드에 추가시킨다. 효과, 시전대상, 시전자, 좌표
	virtual XSKILL::XSkillUser* CreateAndAddToWorldShootObj( XSKILL::XSkillObj *pUseSkill,
												XSKILL::EFFECT *pEffect, 
												XSKILL::XSkillReceiver *pCastingTarget, 
												XSKILL::XSkillUser *pCaster, 
												const XE::VEC2 *pvPos ) = 0;		
	virtual XE::VEC2 GetCurrentPosForSkill( void ) = 0;
	virtual void CustomInvokeFilter( XArrayLinearN<XSkillReceiver*, 100> *pOutAryIvkTarget,
							XArrayLinearN<XSkillReceiver*, 100>& arySrc,
							EFFECT *pEffect ) {}
	virtual BOOL IsInvokeCustomCondition( EFFECT *pEffect ) { return TRUE; }
	void InvokePassiveSkill( void );
	virtual void OnSkillPlaySound( ID idSound ) {}
	///< 하위클래스는 cond에 맞는 타겟을 찾아 돌려줘야 한다. this는 시전자가 된다.
	virtual XSKILL::XSkillReceiver* GetTargetObject( XSKILL::EFFECT *pEffect, XSKILL::xtTargetCond cond ) = 0;
	virtual void OnAdjustEffectAbility( float *pOutMin, float *pOutMax ) {}
	xtError ApplyEffect( XSkillObj *pUseSkill, XSkillReceiver *pTarget, XE::VEC2 *pvTouchPos );
}; // XSkillUser

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/*
class XDelegateShootingObj
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateShootingObj() { Init(); }
	virtual ~XDelegateShootingObj() { Destroy(); }
	//
};
*/
//////////////////////////////////////////////////////////////////////////
class XShootingObj
{
	XDelegateSkill *m_pDelegate;
	XSkillObj *m_pUseSkill;
	EFFECT *m_pEffect;
	ID m_snTarget;
	ID m_snCaster;
	XSkillReceiver *m_pTarget;
	XSkillUser *m_pCaster;
	XE::VEC3 m_vTarget;		// 목표좌표(캐스터나 타겟이 죽어없어지더라도 계속 날아갈수 있도록하기 위함)
	XE::VEC3 m_vSource;		// 출발좌표
	void Init() {
		m_pUseSkill = NULL;
		m_pEffect = NULL;
		m_snTarget = 0;
		m_snCaster = 0;
		m_pTarget = NULL;
		m_pCaster = NULL;
	}
	void Destroy() {}
public:
	// 발사체가 날아가는동안 캐스터와 타겟모두 사라졌다면 목표에 도착한후 이벤트를 보낼대상
	// 을 알아낼수 없으므로 제 3자에게 이것을 맡겨야 한다. 이것을 델리게이트 객체에게 위임했다.
	XShootingObj( XDelegateSkill *pDelegate,
				XSkillObj *pUseSkill,
				EFFECT *pEffect, 
				ID snTarget, 
				ID snCaster, 
				const XE::VEC3& vTarget, 
				const XE::VEC3& vSource ) { 
		Init(); 
		XBREAK( pDelegate == NULL );
		m_pDelegate = pDelegate;
		m_pUseSkill = pUseSkill;
		m_pEffect = pEffect;
		m_snTarget = snTarget;
		m_snCaster = snCaster;
		m_vTarget = vTarget;
		m_vSource - vSource;
	}
	virtual ~XShootingObj() { Destroy(); }
	//
	GET_ACCESSOR( EFFECT*, pEffect );
	GET_ACCESSOR( XSkillObj*, pUseSkill );
	GET_ACCESSOR( ID, snTarget );
	//
	void OnArrive( XSkillReceiver *pTarget ) {
		// 목표에 도달했으면 델리게이트에게 아이디를 던져 캐스터를 받아와서
		// 이벤트를 던진다
		XSkillUser *pCaster = m_pDelegate->GetCaster( m_snCaster );
		if( pCaster ) {
//			XSkillReceiver *pTarget = NULL;
			if( m_snTarget && pTarget == NULL )
				pTarget = m_pDelegate->GetTarget( m_snTarget );
			if( pTarget )
				pCaster->OnArriveShooingObj( this, m_pUseSkill, m_pEffect, pTarget, m_vTarget.ToVec2() );
		}
	}
};

NAMESPACE_XSKILL_END

