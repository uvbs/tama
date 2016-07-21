#pragma once
#include "xLIB.h"
#include "SkillType.h"
#include "SkillDef.h"
#include "XSkillObj.h"
#include "XSkillReceiver.h"
#include "XESkillMng.h"
#include "XLua.h"

// class XSprObj;
// class XKeyEvent;

XE_NAMESPACE_START( XSKILL )
xtTargetFilter MakeTargetFilter( const EFFECT *pEffect );
// 스킬을 사용할 수 있는 베이스 오브젝트
class XShootingObj;
class XSkillUser;
class XSkillSfx;
class XSkillUser  : public XSKILL::XDelegateSkill
{
public:
	struct xUseSkill {
		XSkillObj *pUseSkill;	///< UseSkill()로 사용된 스킬객체. OnCastSkill()에서 다시 사용된다.
		int level;			///< UseSkill()로 건네준 스킬레벨
		XSkillReceiver *pBaseTarget;	///< 기준타겟
		XE::VEC2 vTarget;				///< 기준좌표
		xtError errCode;
		xUseSkill() {
			pUseSkill = nullptr;
			pBaseTarget = nullptr;
			level = 0;
			errCode = xERR_OK;
		}
	};
private:
	XList<XSkillObj*> m_listUseSkill;		// 보유 스킬
	XESkillMng *m_prefSkillMng;
//	ID m_idEventHitter;		// 피격자, this가 때린자
//	ID m_idEventAttacker;	// 공격자, this를 공격한자
	ID m_idDead;			// 사살된자, this가 죽인넘
// 	XSkillObj *m_pUseSkill;	///< UseSkill()로 사용된 스킬객체. OnCastSkill()에서 다시 사용된다.
// 	int m_Level;			///< UseSkill()로 건네준 스킬레벨
// 	XSkillReceiver *m_pBaseTarget;	///< 기준타겟
// 	XE::VEC2 m_vTarget;				///< 기준좌표
	void Init() {
		m_prefSkillMng = NULL;
//		m_idEventHitter = 0;
//		m_idEventAttacker = 0;
		m_idDead = 0;
// 		m_pUseSkill = nullptr;
// 		m_pBaseTarget = nullptr;
// 		m_Level = 0;
	}
	void Destroy();
	// 효과처리부는 외부에서 부를일이 없으니까 숨겼음
//	xtError TryCastEffect( XSkillDat *pSkillDat, EFFECT *pEffect, int level, XSkillReceiver *pBaseTarget, XSkillReceiver *pCastingTarget, const XE::VEC2 *pvPos=NULL );	// 효과발동시도
	xtError UseEffect( XSkillDat *pSkillDat, EFFECT *pEffect, int level, XSkillReceiver *pTarget, const XE::VEC2& vPos, ID idCallerSkill );
public:
	XSkillUser( XESkillMng *pSkillMng ) { 
		Init(); 
		m_prefSkillMng = pSkillMng;
	}
	virtual ~XSkillUser(void) { Destroy(); }
	// 초기화
	void OnCreate();
	// get/set/is
	// 보유스킬 수 리턴
	int GetNumUseSkill( void ) { return m_listUseSkill.size(); }
	// 주어진 시전방식에 해당하는 스킬들의 개수를 구함
	int GetNumUseSkill( xCastMethod castMethod ) {		
		int n = 0;
		XLIST_LOOP( m_listUseSkill, XSkillObj*, pSkillUseObj ) {
			if( pSkillUseObj->IsSameCastMethod( castMethod ) )
				n++;
		} END_LOOP;
		return n;
	}
	GET_ACCESSOR( XSkillObj_List&, listUseSkill );	// 이건 나중에 내부로 숨겨야한다
//	GET_SET_ACCESSOR( ID, idEventHitter );
//	GET_SET_ACCESSOR( ID, idEventAttacker );
	GET_SET_ACCESSOR( ID, idDead );
//	GET_SET_ACCESSOR( xTYPE_FRIENDSHIP, useFriendship );
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
	void AddUseSkill( XSkillObj *pUseSkill );
	// idSkillDat스킬로 사용스킬객체를 만들어서 보유한다
	XSkillObj* CreateAddUseSkillByID( ID idSkillDat );		
//	XSkillObj* CreateAddUseSkillByName( LPCTSTR szName );
	XSkillObj* CreateAddUseSkillByIdentifier( LPCTSTR szIdentifier );
	void DestroyAllSkill( void ) {
		XLIST_DESTROY( m_listUseSkill, XSkillObj* );
	}

	xUseSkill UseSkillByID( XSkillObj **ppOutUseSkill, int level, ID idSkillDat, XSkillReceiver *pTarget, XE::VEC2 *pvPos );
//	xtError UseSkillByName( XSkillObj **ppOutUseSkill, LPCTSTR szSkillName, XSkillReceiver *pTarget, XE::VEC2 *pvPos );
	xUseSkill UseSkillByIdentifier( LPCTSTR szIdentifier, int level, XSkillReceiver *pTarget, XE::VEC2 *pvPos, XSkillObj **ppOutUseSkill=NULL );
	xUseSkill UseSkill( XSkillObj *pUseSkill, 
					int level, 
					XSkillReceiver *pCurrTarget, 
					XE::VEC2 *pvPos );
// 	xtError	UseSkill( XSkillObj *pUseSkill, 
// 					int level,
// //					xCastMethod castMethod,
// 					XSkillReceiver *pCurrTarget,	// 외부에서 커서로 타겟을 찍었을 경우
// 					XE::VEC2 *pvTouchPos );			// 외부에서 커서로 좌표를 찍었을 경우
	// 첫번째 보유스킬을 사용한다
	xUseSkill UseSkill( XSkillReceiver *pTarget, XE::VEC2 *pvPos );
	xtError OnShootSkill( XSkillObj *pUseSkill, XSkillReceiver *pBaseTarget, int level, const XE::VEC2& vTarget, ID idCallerSkill);
	xtError OnShootSkill( const xUseSkill& infoUseSkill, ID idCallerSkill ) {
		return OnShootSkill( infoUseSkill.pUseSkill, infoUseSkill.pBaseTarget, infoUseSkill.level, infoUseSkill.vTarget, idCallerSkill );
	}
	int FrameMove( float dt );
	xtError CastEffectToCastingTarget( XSkillDat *pSkillDat, EFFECT *pEffect, int level, XSkillReceiver *pBaseTarget, XSkillReceiver *pCastingTarget, const XE::VEC2& vPos, ID idCallerSkill );		// 효과발동
	// virtual
	virtual XLuaSkill* CreateScript( void ); 		// XLua 객체를 생성하고 전역변수와 API들을 등록하여 돌려준다
//	virtual xtError IsValidCastingTarget( XSkillObj *pUseSkill, EFFECT *pEffect, XSkillReceiver *pTarget, XE::VEC2 *pvPos  );		// 시전대상 유효성 검사
	virtual int GetCastingTargetList( XArrayLinearN<XSkillReceiver*, 512> *pAryOutCastingTarget, xtCastTarget castTarget, XSkillDat *pSkillDat, const EFFECT *pEffect, XSkillReceiver *pOutBaseTarget, XE::VEC2 *pvOutPos = nullptr );		// 시전대상얻기
	virtual int GetInvokeTarget( XArrayLinearN<XSkillReceiver*, 512> *plistOutInvokeTarget, XSkillDat *pBuff, int level, xtInvokeTarget invokeTarget, const EFFECT *pEffect, XSkillReceiver *pCastingTarget, const XE::VEC2& vPos );		// 스킬발동대상얻기
	virtual void OnCoolTimeOver( XSkillObj *pUseSkill ) {}	// 쿨타임시간이 끝나면 호출된다
	// XSkillObj에 아직은 버추얼이 많지 않아서 pure로 하지 않았는데 필요해지면 pure로 다시 보내야 한다
	virtual XSkillObj* CreateSkillUseObj( XSkillDat *pSkillDat );
	BIT GetFilterSideCast( XSkillReceiver *pCastingTarget, xtCastTarget targetType, XSkillReceiver *pBaseTarget, xtFriendshipFilt friendshipFilter );	// 시전대상에 타겟타입으로 effectFilter로 시전/발동 될때 검색해야할 대상의 프렌드쉽 플래그
	BIT GetFilterSideInvoke( XSkillUser *pCaster/*, XSkillReceiver *pInvoker*/, XSkillReceiver *pCastingTarget, xtInvokeTarget targetType, xtFriendshipFilt friendshipFilter );
	virtual BOOL IsInvokeAble( XSkillDat *pDat, XSkillReceiver *pCstTarget, const EFFECT *pEffect );
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
	virtual int GetListObjsInRect( XArrayLinearN<XSkillReceiver*, 512> *pAryOutIvkTarget, 
									const XE::VEC2 *pvStart,	// vStart부터 vSize만큼의 직사각형이 되나, vStart의 y지점은 vSize.h의 중간지점이다.
									const XE::VEC2& vSize, 
									BIT bitSideSearchFilt, 
//									xtSide sideSearchFilt,
									int numApply ) { return 0; }
	/**
	 @brief 기준타겟(혹은 기준좌표)을 중심으로 반경내 조건에 맞는 오브젝트를 요청한다.
	 @param pBaseTarget 기준타겟. null일수도 있다.
	 @param pvBasePos 기준좌표. null일수도있다. 그러나 기준타겟과 기준좌표가 모두 null로 넘어오진 않는다.
	 @param bitSideFilter 선택해야할 우호 조건
	 @param numApply 최대 선택해야할 오브젝트수. 0이면 제한이 없다.
	 @param bIncludeCenter 기준타겟을 포함하여 선택할것인지 아닌지.
	*/
	virtual int GetListObjsRadius( XArrayLinearN<XSkillReceiver*, 512> *plistOutInvokeTarget, 
									const EFFECT *pEffect,
									XSkillReceiver *pBaseTarget, 
									const XE::VEC2& vBasePos,
									float meter, 
									BIT bitSideFilter, 
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
//	virtual void OnCreateCasterSfx( EFFECT *pEffect, xtPoint createPoint, LPCTSTR szSpr, ID idAct, float secPlay ) {}
	virtual void OnCreateSkillSfx( XSKILL::XSkillDat *pSkillDat,
									const EFFECT *pEffect,
									XSKILL::xtPoint createPoint,
									LPCTSTR szSpr,
									ID idAct,
									float secPlay,
									const XE::VEC2& vPos) {}
	/**
	 @brief pEffect는 null이 올수 있음.
	*/
	void CreateSfx( XSkillDat *pSkillDat,
					EFFECT *pEffect,
					const _tstring& strEffect,
					ID idAct,
					xtPoint pointSfx,
					float secPlay,
					const XE::VEC2& vPos = XE::VEC2(0) );
	virtual XSkillReceiver* GetGroundReceiver( void ) { return NULL; }
	virtual int GetListPartyMember(XArrayLinearN<XSkillReceiver*, 512> *pAryOutIvkTarget, BIT bitSideSearchFilt, xtTargetFilter filter ) {
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
										int level,
										const XE::VEC2& vPos,
										ID idCallerSkill );
	// copy해서 쓰기 좋으라고 일부러 XSKILL을 다 붙임
	// 발사체오브젝트를 생성하고 월드에 추가시킨다. 효과, 시전대상, 시전자, 좌표
	virtual XSKILL::XSkillUser* CreateAndAddToWorldShootObj( XSKILL::XSkillDat *pSkillDat,
// 												XSKILL::EFFECT *pEffect, 
												int level,
												XSKILL::XSkillReceiver *pBaseTarget,
// 												XSKILL::XSkillReceiver *pCastingTarget, 
												XSKILL::XSkillUser *pCaster, 
												const XE::VEC2& vPos ) = 0;		
//	virtual XE::VEC2 GetCurrentPosForSkill( void ) = 0;
	virtual void CustomInvokeFilter( XArrayLinearN<XSkillReceiver*, 512> *pOutAryIvkTarget,
							XArrayLinearN<XSkillReceiver*, 512>& arySrc,
							const EFFECT *pEffect ) {}
	/**
	 @brief 효과의 발동조건
	*/
// 	virtual BOOL IsInvokeCustomCondition( XSkillDat *pSkillDat, EFFECT *pEffect ) { 
// 		if( pEffect->invokeTargetCondition == 0 )	// 0은 항상으로 침.
// 			return TRUE;
// 		return FALSE;
// 	}
	void InvokePassiveSkill( void );
	virtual void OnSkillPlaySound( ID idSound ) {}
	///< 하위클래스는 cond에 맞는 타겟을 찾아 돌려줘야 한다. this는 시전자가 된다.
	virtual XSKILL::XSkillReceiver* GetTargetObject( XSKILL::EFFECT *pEffect, XSKILL::xtTargetCond cond ) = 0;
	virtual void OnAdjustEffectAbility( XSkillDat *pSkillDat, const EFFECT *pEffect, int invokeParam, float *pOutMin ) {}
	xtError ApplyEffect( XSkillDat *pSkillDat, int level, XSkillReceiver *pTarget, ID idCallerSkill, const XE::VEC2& vTouchPos = XE::VEC2(0) );
	virtual int GetSkillLevel( XSkillObj* pSkillObj ) { return 0; }
	virtual XSKILL::XSkillReceiver* GetSkillBaseTarget( XSkillDat *pDat ) { return nullptr; }
	virtual XE::VEC2 GetSkillBaseTargetPos( XSkillDat *pDat ) { return XE::VEC2(0); }
	virtual XSkillReceiver* CreateSfxReceiver( XSKILL::EFFECT *pEffect, float sec ) { return nullptr; }
	void CastSkillToBaseTarget( XSkillDat *pSkillDat, 
								int level, 
								XSkillReceiver *pBaseTarget, 
								const XE::VEC2& vPos
								, ID idCallerSkill );
	/**
	 @brief 하위클래스에게 기준타겟조건에 맞는 기준타겟을 찾아달라고 의뢰
	*/
	virtual XSkillReceiver* GetBaseTargetByCondition( XSKILL::XSkillDat *pSkillDat,
													XSKILL::xtBaseTargetCond cond,
													float meter,
													int level,
													XSKILL::XSkillReceiver *pCurrTarget,
													const XE::VEC2& vPos ) { 
		XBREAK(1);
		return NULL; 
	}
	virtual XSKILL::XSkillReceiver* GetCurrTarget() { XBREAK(1); return nullptr; }
	/**
	 스킬능력치 추가증폭. 
	 추가될 양을 리턴해야한다. 예를들어 100%가 더 추가되고 싶다면 1.0을,
	 20이 더 추가되고 싶다면 20을 리턴하면 된다.
	 pIvkTarget은 사용되지 않는다. 
	*/
	virtual void OnSkillAmplifyUser( XSkillDat *pDat, 
									XSKILL::XSkillReceiver *pIvkTarget, 
									const XSKILL::EFFECT *pEffect, 
									XSKILL::xtEffectAttr attrParam, 
									float *pOutRatio, float *pOutAdd ) { 
		if( pOutRatio )
			*pOutRatio = 0.f;
		if( pOutAdd )
			*pOutAdd = 0;
	}
	/// 좌표에 생성되어야 하는 지속효과의 경우 발생되는 이벤트. pBaseTarget은 null일수 있다. 시전자는 this가 된다
	virtual void OnCastEffectToPos( XSKILL::XSkillDat *pSkillDat,
									XSKILL::EFFECT *pEffect,
									int level,
									float sec,
									float radiusMeter,
									BIT bitSideInvokeTarget,	// 발동되어야 하는 발동대상우호
									XSKILL::XSkillReceiver *pBaseTarget,
									const XE::VEC2& vPos ) {}
	/// GetInvokeTarget()에서 발동범위를 결정하기전에 호출되어진다.
	virtual float OnInvokeTargetSize( XSKILL::XSkillDat *pSkillDat,
									const XSKILL::EFFECT *pEffect,
									int level, 
									XSKILL::XSkillReceiver *pCastingTarget,
									float size ) { return size; }
	bool IsInvokeTargetCondition( XSkillDat *pDat
															, const EFFECT *pEffect
															, XSkillReceiver *pInvokeTarget );
	void ApplyInvokeEffectWithAry( 
														XArrayLinearN<XSkillReceiver*, 512>& aryInvokers
														, XSkillDat *pDat
														, const EFFECT *pEffect
														, XSkillReceiver *pInvoker
														, bool bCreateSfx
														, int level
														, const XE::VEC2& vPos
														, XBuffObj *pBuffObj
														, XLuaSkill *pLua );
}; // XSkillUser

/****************************************************************
* @brief 
* 하위클래스는 OnEventSprObj을 구현하고 거기서 CallCallbackFunc을 호출해야 한다.
* @author xuzhu
* @date	2014/11/26 20:23
*****************************************************************/
class XSkillSfx //: public XDelegateSprObj
{
	struct xCALLBACK {
		XSkillUser *pOwner;
		std::function<void( XSkillUser* )> funcCallback;
		xCALLBACK() {
			pOwner = nullptr;
		}
	};
	xCALLBACK m_Callback;
	void Init() {}
	void Destroy() {}
protected:
public:
	XSkillSfx() { Init(); }
	virtual ~XSkillSfx() { Destroy(); }
	void RegisterCallback( XSkillUser* pOwner, 
												XSkillDat *pSkillDat, 
												int level, 
												XSkillReceiver *pBaseTarget, 
												const XE::VEC2& vPos );
	void CallCallbackFunc( void ) {
//		if( !m_Callback.funcCallback.empty() )
		if( m_Callback.funcCallback )
			m_Callback.funcCallback( m_Callback.pOwner );
	}
}; // class XSkillSfx

XE_NAMESPACE_END

