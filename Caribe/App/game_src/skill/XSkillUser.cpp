#include "stdafx.h"
#include "xSkill.h"
#include "XSkillUser.h"
#include "XESkillMng.h"
#include "XSkillDat.h"
#include "XLua.h"
#include "Sprite/Sprdef.h"
#include "XFramework/Game/XEComponents.h"
#ifdef _VER_IPHONE
#undef min
#undef max
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )


void XSkillSfx::RegisterCallback( XSkillUser* pOwner
																, XSkillDat *pSkillDat
																, int level
																, XSkillReceiver *pBaseTarget
																, const XE::VEC2& vPos ) 
{
	m_Callback.pOwner = pOwner;
	m_Callback.funcCallback = std::bind( &XSkillUser::CastSkillToBaseTarget,
																					std::placeholders::_1,
																					pSkillDat,
																					level,
																					pBaseTarget,
																					vPos,
																					pSkillDat->GetidSkill() );
}

//////////////////////////////////////////////////////////////////////////
XLuaSkill* XSkillUser::CreateScript() { 
	return new XLuaSkill; 
} 		// XLua 객체를 생성하고 전역변수와 API들을 등록하여 돌려준다

void XSkillUser::Destroy()
{
	DestroyAllSkill();
}

void XSkillUser::DestroyAllSkill() 
{
//	XLIST4_DESTROY( m_listUseSkill );
}

void XSkillUser::OnCreate()
{
	// 시작시 패시브스킬을 실행시킨다.
//	InvokePassiveSkill();
}

/**
 @brief 스킬 최초 사용
 pUseSkill을 사용하기 위한 시작함수. 이 함수가 성공하면 스킬모션이 시작된다.
 캐스팅 시작이라고 봐도 됨.
 @param pCurrTarget 현재 this가 잡고 있는 공격타겟. 혹은 직접 터치로 찍은 타겟. 스킬의 타입에 따라서 이 타겟은 안쓰여 질수도 있다.
 @param pvTouchPos 유저가 직접 터치로 바닥을 찍었다면 그 좌표
*/
XSkillUser::xUseSkill XSkillUser::UseSkill( XSkillDat* pDat,
																						int level,
																						XSkillReceiver *pCurrTarget,
																						XE::VEC2 *pvTouchPos )
{
	xUseSkill infoUseSkill;
	XE::VEC2 vTouchPos = ( pvTouchPos ) ? *pvTouchPos : XE::VEC2( 0 );
	if( XBREAK( pDat == nullptr ) ) {
		infoUseSkill.errCode = xERR_CRITICAL_ERROR;
		return infoUseSkill;
	}
//	auto pDat = pUseSkill->GetpDatMutable();
	// 시전자 이펙트(시전시작 시점에 발생되는 이펙트)
	if( !pDat->GetCasterEff().m_strSpr.empty() ) {
		const float secPlay = 0.f;	// play once
		CreateSfx( pDat, pDat->GetCasterEff(), secPlay, vTouchPos );
	}
	infoUseSkill.pDat = pDat;
	infoUseSkill.level = level;
	const auto baseTarget = pDat->GetbaseTarget();
	// 기준타겟이 자신이면 this로 타겟을 바꾼다.
	if( baseTarget == xBST_SELF )
		infoUseSkill.pBaseTarget = GetThisRecv();
	else
	if( baseTarget == xBST_CURR_TARGET ) {
		// 현재타겟과 기준타겟우호가 맞지 않으면 일단 상속 클래스에 의뢰한다.
		const auto bitBaseTarget = pDat->GetbitBaseTarget();
		infoUseSkill.pBaseTarget = pCurrTarget;
		if( pCurrTarget == nullptr 
				|| (bitBaseTarget == xfALLY && pCurrTarget->GetCamp().IsFriendly(GetCampUser()) == FALSE) 
				|| (bitBaseTarget == xfHOSTILE && pCurrTarget->GetCamp().IsEnemy( GetCampUser() ) == FALSE ) ) {
			infoUseSkill.pBaseTarget = GetSkillBaseTarget( pDat );
		}
		// 타겟을 그래도 못찾은경우 에러코드 리턴(주변에 적당한 타겟이 없다거나 하면 생길수 있음.)
		if( infoUseSkill.pBaseTarget == nullptr ) {
			infoUseSkill.errCode = xERR_MUST_SELECT_TARGET;
			return infoUseSkill;
		}
	} else
	if( baseTarget == xBST_POSITION ) {
		if( pvTouchPos )
			infoUseSkill.vTarget = *pvTouchPos;
		// 좌표가 없다면 하위클래스에 일단 의뢰해본다.
		if( infoUseSkill.vTarget.IsZero() )
			infoUseSkill.vTarget = GetSkillBaseTargetPos( pDat );
		// 기준타겟을 좌표로 하겠다고 했으면 좌표가 있어야함.
		if( XBREAK(infoUseSkill.vTarget.IsZero()) ) {
			infoUseSkill.errCode = xERR_MUST_SELECT_TARGET;
			return infoUseSkill;
		}
	} else
	// 기준타겟조건
	if( baseTarget == xBST_CONDITION ) {
		infoUseSkill.pBaseTarget 
			= GetBaseTargetByCondition( pDat,
																	pDat->GetcondBaseTarget(),
																	pDat->GetrangeBaseTargetCond(),
																	level,
																	pCurrTarget,
																	vTouchPos );
		if( infoUseSkill.pBaseTarget == nullptr ) {
			infoUseSkill.errCode = xOK;
			return infoUseSkill;
		}
	} else {
		// 하드코딩
	}
	XBREAK( infoUseSkill.pBaseTarget == nullptr && infoUseSkill.vTarget.IsZero() );
	infoUseSkill.errCode = xOK;
	return infoUseSkill;
} // UseSkill

/**
 @brief 이펙트를 만드는 공통 함수.
 @param secPlay 0:once 0>:해당시간동안 루핑 -1:무한루핑
*/
ID XSkillUser::CreateSfx( const XSkillDat *pSkillDat,
													const _tstring& strEffect,
													ID idAct,
													xtPoint pointSfx,
													float secPlay,
													const XE::VEC2& vPos ) // const못씀. new XObjSfx같은거 할때 시전자포인터를 넘겨주기때문. 넘겨주는 시전자포인터를 const로 해야함.
{
	if( strEffect.empty() )
		return 0;
	float secLife = 0.f;
	// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
	if( pointSfx == xPT_NONE )
		pointSfx = xPT_TARGET_BOTTOM;
	if( idAct == 0 )
		idAct = 1;
	return OnCreateSkillSfx( pSkillDat,
													 pointSfx,
													 strEffect.c_str(),
													 idAct,
													 secPlay,
													 vPos );
}

ID XSkillUser::CreateSfx( const XSkillDat *pSkillDat
														, const xEffSfx& effSfx
														, float secPlay
														, const XE::VEC2& vPos )
{
	return CreateSfx( pSkillDat, effSfx.m_strSpr, effSfx.m_idAct, effSfx.m_Point, secPlay, vPos );
}

/**
 @brief 첫번째 보유스킬을 사용한다
 그냥 편의 함수.
*/
XSkillUser::xUseSkill
XSkillUser::UseSkill( XSkillReceiver *pTarget,
											XE::VEC2 *pvPos )
{
	xUseSkill info;
	if( XBREAK( m_listUseSkill.empty() ) ) {
		info.errCode = xCANCEL;
		return info;
	}
	XSkillDat *pUseSkill = m_listUseSkill.GetFirst();
	int level = GetSkillLevel( pUseSkill );
	return UseSkill( pUseSkill, level, pTarget, nullptr );
}

/**
 @brief 스킬을 사용(Shoot)한다. 
 발사체일경우는 발사체객체를 생성하고 즉시시전형이면 기준타겟에 바로 스킬을 시전한다.
*/
xtError XSkillUser::OnShootSkill( XSkillDat *pDat
																, XSkillReceiver *pBaseTarget
																, int level
																, const XE::VEC2& vTarget
																, ID idCallerSkill )
{
	if( XBREAK(pDat == nullptr || pBaseTarget == nullptr) )
		return xERR_GENERAL;
//	auto pDat = pUseSkill->GetpDatMutable();
	// 시전자의 타점에 생성되는 이펙트(루핑없음)
	if( !pDat->GetShootEff().m_strSpr.empty() ) {
		float secPlay = 0;	// once
		CreateSfx( pDat, pDat->GetShootEff(), secPlay, vTarget );
	}
	// 슈팅타겟이펙트(슈팅시점에 타겟에게 발생한다. 보통 타점을 포함하고 있다)-메테오
	if( !pDat->GetShootTargetEff().m_strSpr.empty() ) {
		float secPlay = 0;	// once
		XSkillSfx *pSfx 
			= pBaseTarget->OnCreateSkillSfxShootTarget( pDat,
																									pBaseTarget,
																									level,
																									pDat->GetShootTargetEff(),
																									secPlay, vTarget );
		pSfx->RegisterCallback( this, pDat, level, pBaseTarget, vTarget );
		// 슈팅타겟이펙트는 여기서 sfx만 생성하고 리턴한다음 sfx의 타점에서 CastSkillToBaseTarget이 호출된다.
		return xOK;
	}
	// 발사체방식인가
	if( !pDat->GetstrShootObj().empty() ) {
		// 타겟까지 날아가서 스킬시전을 한다. 발사체스킬은 반드시 발동스킬로 구현되어야 한다.// 시전자->스킬발동()을 한다
		// virtual. 발사체오브젝트를 생성하고 오브젝트 매니저에 등록한다
		CreateAndAddToWorldShootObj( pDat,
																 level,
																 pBaseTarget,
																 this,
																 vTarget );
	} else {
		// 지금 바로 기준타겟에 스킬을 시전한다.
		CastSkillToBaseTarget( pDat, level, pBaseTarget, vTarget, idCallerSkill );
	}
	return xOK;
} // OnShootSkill

/**
 @brief 기준타겟에 스킬을 시전한다.
 이함수는 슈팅타겟이펙트 사용시 단독으로도 호출될수 있다.
*/
void XSkillUser::CastSkillToBaseTarget( XSkillDat *pDat
																			, int level
																			, XSkillReceiver *pBaseTarget
																			, const XE::VEC2& vPos
																			, ID idCallerSkill)
{
	// (기준)타겟이펙트
	if( pDat->GetTargetEff().IsHave() ) {
//		const float secPlay = pDat->GetDuration( );	// once
		const float secPlay = 0;
		pBaseTarget->CreateSfx( pDat, pDat->GetTargetEff(), secPlay, vPos );
	}
	// 스킬이 가진 효과들 기준타겟에게 사용한다.
	for( auto pEffect : pDat->GetlistEffects() )	{
		xtError err = UseEffect( pDat, pEffect, level, pBaseTarget, vPos, idCallerSkill );
		if( err != xOK )
			return;
	}
}

/**
 @brief 효과 시전
 pEffect의 효과를 기준타겟(pBaseTarget)을 기준으로 시전대상을 선정하여 시전한다.
 현재 _vPos는 사용하지 않는다
*/
xtError XSkillUser::UseEffect( XSkillDat *pDat,
															 EFFECT *pEffect,
															 int level,
															 XSkillReceiver *pBaseTarget,
															 const XE::VEC2& _vPos, 
															 ID idCallerSkill )
{
	XVector<XSkillReceiver*> aryCastingTargets;		// 시전대상
	// 시전대상을 선정한다. 대상은 객체일수도 있고 좌표일수도 있다.
	XE::VEC2 vPos = _vPos;		// 시전대상이 좌표로 넘어올경우 여기로 받는다. 디폴트로는 외부좌표를 받는다.
	if( pBaseTarget )	{
		// 기준타겟이 객체일경우
		// 기준타겟의 좌표를 시전대상으로 한다.
		if( pEffect->castTarget == xCST_BASE_TARGET_POS )		{
			if( pEffect->IsDuration() )			{
				// 버프를 가질 sfx용 리시버 객체를 생성.
				const float sec = pEffect->GetDuration( level );
				XBREAK( sec <= 0 );
				auto pSfx = CreateSfxReceiver( pEffect, sec );
				aryCastingTargets.Add( pSfx );
			}
			// 
		} else {
		// 객체류를 시전대상으로 한다.
			// 타겟조건에 따라 시전대상을 가려낸다. 시전대상이 좌표일경우 vPos로 받는다.
			GetCastingTargetList( &aryCastingTargets,
														pEffect->castTarget,
														pDat,
														pEffect,
														pBaseTarget,
														&vPos );
		}
	} else {
	// 기준타겟이 좌표일경우
		XBREAK( pBaseTarget == nullptr );
		if( !vPos.IsZero() ) {
			XALERT( "%s", _T("아직 구현되지 않음. 기준타겟:좌표") );
		}
		return xERR_GENERAL;
	}
	//
	if( aryCastingTargets.empty() && vPos.IsZero() ) {
		return xERR_NOT_FOUND_CASTING_TARGET;		// 시전대상을 찾지 못함
	}
	// 캐스팅 대상들에게 효과를 시전한다.
	for( auto pCastingTarget : aryCastingTargets ) {
		// 즉시시전 or 버프방식
		CastEffToCastTarget( pDat,
												 pEffect,
												 level,
												 pBaseTarget,
												 pCastingTarget,
												 vPos,
												 idCallerSkill );
	}
	// "사용"스크립트 실행
	if( pEffect->scriptUse.empty() == false )	{// 스크립이 있을때만
		auto pLua = CreateScript();	// virtual
		pLua->RegisterScript( this, nullptr );
		pLua->TargetDoScript( GetThisRecv(), pEffect->scriptUse.c_str() );
		SAFE_DELETE( pLua );
	}
	return xOK;
}

// pTarget을 시전대상으로 했을때 얻어지는 모든 시전대상을 얻는다.
/**
 @brief 
*/
int XSkillUser::GetCastingTargetList( XVector<XSkillReceiver*> *pAryOutCastingTarget,			// 시전대상얻기 결과가 담겨짐
																			xtCastTarget castTarget,	// 시전대상타입
																			XSkillDat *pSkillDat,				// 스킬사용 오브젝트
																			const EFFECT *pEffect,					// 효과
																			XSkillReceiver *pBaseTarget,
																			XE::VEC2 *pvOutPos )
{
	XBREAK( pBaseTarget == nullptr );
	//
	switch( castTarget ) {
	case xCST_BASE_TARGET:		// 기준타겟
		pAryOutCastingTarget->Add( pBaseTarget );
		break;
	case xCST_BASE_TARGET_RADIUS:		// 기준타겟반경
	case xCST_BASE_TARGET_SURROUND:	{	// 기준타겟주변
		// 기준타겟을 중심으로 원형 반경내에서의 객체를 선택한다.
		BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(),
													castTarget,
													pBaseTarget,
													pEffect->castfiltFriendship );
		int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
		// 기준타겟포함이면 기준타겟은 우선으로 리스트에 들어가고 기준타겟제외로 검색한다.
		if( castTarget == xCST_BASE_TARGET_RADIUS ) {
			pAryOutCastingTarget->Add( pBaseTarget );
			if( numApply > 1 )
				--numApply;
		}
		float castSize = pEffect->castSize.w;
		// 일단은 그냥 하드코딩
		float sizeAdd = 0.f;
		float sizeAddMultiply = 0.f;
		OnSkillAmplifyUser( pSkillDat, nullptr, pEffect, xEA_CAST_RADIUS, &sizeAddMultiply, &sizeAdd );
		castSize += sizeAdd;
		castSize = castSize + castSize * sizeAddMultiply;
		XE::VEC2 vPosExtern;	// 외부에서 넘어온 원의중심좌표(현재는 사용하지 않음)
		GetListObjsRadius( pAryOutCastingTarget,
											 pSkillDat,
												pEffect,
												pBaseTarget,		// 기준타겟
												vPosExtern,		// 중심이 타겟형태가 아니고 좌표형태로 넘어오는경우 사용됨.현재는 사용되지 않음
												castSize,
												bitSideSearchFilter,
												numApply,
												FALSE );
		// 기준타겟을 제외하고 numApply에 따라 타겟들을 뽑은 후 "반경"이면 기준타겟을 추가시킨다.
// 		if( castTarget == xCST_BASE_TARGET_RADIUS ) {
// 			pAryOutCastingTarget->Add( pBaseTarget );
// 		}
	} break;
	case xCST_BASE_TARGET_PARTY:	// 기준타겟파티
		// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
		pBaseTarget->GetGroupList( pAryOutCastingTarget,
									pSkillDat,
									pEffect );
		break;
	default:
		XBREAKF( 1, "%d: 처리안된 castTarget", castTarget );
	}
	return pAryOutCastingTarget->size();
}

/**
 @brief 시전대상에게 효과시전
 이 함수는 시전대상의 수만큼 불린다.
 @param pBaseTarget 기준타겟
 @param pCastingTarget 시전대상들
*/
xtError XSkillUser::CastEffToCastTarget( XSkillDat *pDat,
																				 EFFECT *pEffect,
																				 int level,
																				 XSkillReceiver *pBaseTarget,
																				 XSkillReceiver *pCastingTarget,
																				 const XE::VEC2& vPos,
																				 ID idCallerSkill )
{
	XBREAK( pDat == nullptr );
	XBREAK( pEffect == nullptr );
	// 시전대상들에게 모두 이펙트를 붙인다.
	if( pEffect->m_CastTargetEff.IsHave() ) {
		float secPlay = pEffect->GetDuration( level );
		pCastingTarget->CreateSfx( pDat, pEffect->m_CastTargetEff, secPlay );
	}
	if( pEffect->IsDuration() ) {
		if( pCastingTarget ) {
			// 지속시간이 있는 버프형 효과를 타겟에게 시전한다.
			CastEffToCastTargetByBuff( pDat, pEffect, level, pCastingTarget, vPos, idCallerSkill );
		}
	} else {	// 즉시발동형(지속시간 0)
		// 시전대상에게 즉시 효과가 발동된다.
		CastEffToCastTargetByDirect( pDat, pEffect, level, pCastingTarget, vPos );
	}
	if( pEffect->idCastSound ) {
		OnSkillPlaySound( pEffect->idCastSound );
	}
	return xOK;
}

/**
 @brief 버프타입의 효과를 시전대상에게 시전한다.
*/
xtError XSkillUser::CastEffToCastTargetByBuff( XSkillDat *pDat,
																							 EFFECT *pEffect,
																							 int level,
																							 XSkillReceiver *pCastingTarget,
																							 const XE::VEC2& vPos,
																							 ID idCallerSkill )
{
	XASSERT( pCastingTarget );
	// 발사체의 형태로 시전대상에게 날아오다 시전대상이 사라지면 대상이 널이 될수 있음
	XBuffObj *pBuffObj = nullptr;
	bool bCreateBuff = true;
	// 사용하려는 스킬의 버프객체가 시전대상에 이미 있는지 검사. 
	// 시전자가 다른 같은버프가 중복으로 걸릴수도 있으므로 시전자까지 같아야 한다
	pBuffObj = pCastingTarget->FindBuffSkill( pDat->GetidSkill(), this );
	if( pBuffObj == nullptr ) {
		// 시전자도 같고 아이디도 같은 버프는 없다. 그렇다면
		// 시전자는 달라도 아이디는 같은 버프는 있는가?
		pBuffObj = pCastingTarget->FindBuffSkill( pDat->GetidSkill() );
		if( pBuffObj ) {	// 시전자는 다르고 아이디만 같은 경우
			if( pEffect->bDuplicate ) {
				// 중복생성가능. 새로 버프를 생성시키도록 null로 클리어 한다.
				pBuffObj = nullptr;
			}
		}
	}
	// 지속시간형 객체를 생성한다.
	if( pBuffObj == nullptr ) {
		// 버프오브젝트만 일단 생성하고 이펙트오브젝트는 시전대상에 맞는것만 따로 추가한다
		pBuffObj = CreateSkillBuffObj( this,
																	 pCastingTarget,
																	 pDat,
																	 level,
																	 vPos,
																	 idCallerSkill );
		XASSERT( pBuffObj );
		// 시전대상의 버프리스트에 추가
		pCastingTarget->AddSkillRecvObj( pBuffObj );
		// 시전대상에게 버프가 추가된 직후 이벤트가 발생한다.
		pCastingTarget->OnAddSkillRecvObj( pBuffObj, pEffect );
		// 지속이펙트생성(효과가 여러개여도 지속이펙트는 1개)
		if( pEffect->m_PersistEff.IsHave() ) {
//			const float secPlay = pEffect->GetDuration( level );	// 지속이펙트는 무조건 루핑.
			const float secPlay = pEffect->GetDuration(level);		// 무한으로 돌리고 버프객체측에서 효과가 종료될때 삭제시킨다.
			ID idSfx = pCastingTarget->CreateSfx( pDat, pEffect->m_PersistEff, secPlay );
			if( idSfx ) {
				pBuffObj->SetidSfx( idSfx );
			}
		}
		// 시전사운드 플레이
		if( pEffect->idCastSound ) {
			OnPlaySoundUse( pEffect->idCastSound );		// virtual
		}
	}
	XBREAK( pBuffObj == nullptr );
	// 효과를 추가하고 타이머를 작동시킴
	// 이펙트오브젝트를 추가. 이미 효과가 있으면 기존거를 리턴

	EFFECT_OBJ *pEffObj = pBuffObj->FindEffect( pEffect );
	if( pEffObj == nullptr ) {
		pEffObj = pBuffObj->AddEffect( pEffect );
		// "시전"스크립트 실행
		pBuffObj->ExecuteScript( GetThisRecv(), pEffect->scriptCast.c_str() );
		// 버프객체에 이벤트 발생
		pBuffObj->OnCastedEffect( pCastingTarget, pEffObj );
	}
	// 지속시간 타이머를 셋. 이미 효과가 걸려있으면 타이머만 리셋
	if( pEffect->IsDuration() ) {
		const auto sec = pEffect->GetDuration( level );
		pEffObj->timerDuration.Set( sec );
		pEffObj->cntDot = 0;
	}
	return xOK;
} // CastEffToCastTargetByBuff

/**
 @brief 즉시시전형태의 효과를 시전대상에게 시전한다.
*/
xtError XSkillUser::CastEffToCastTargetByDirect( XSkillDat *pDat,
																								 const EFFECT *pEffect,
																								 int level,
																								 XSkillReceiver *pCastingTarget,
																								 const XE::VEC2& vPos )
{
	XVector<XSkillReceiver*> aryInvokeTarget;
	// 1회성이기때문에 루아도 쓰고 바로 파괴되는걸로 함
	// 시전대상자(pCastingTarget)기준 발동대상 얻음
	int numInvoke = GetInvokeTarget( &aryInvokeTarget,
																	 pDat,
																	 level,
																	 pEffect->invokeTarget,
																	 pEffect,
																	 pCastingTarget,
																	 vPos );
	if( numInvoke > 0 ) {
		bool bCreateSfx = (pEffect->m_invokeTargetEff.m_strSpr.empty()) ? false : true;
		// 발동대상들에게 효과 적용
		for( auto pInvokeTarget : aryInvokeTarget ) {
			ApplyInvokeEffToIvkTarget( pInvokeTarget, 
																			 pDat, 
																			 pEffect, 
																			 GetThisRecv(),		// invoker
																			 bCreateSfx, 
																			 level, 
																			 vPos, 
																			 nullptr );
		}
	}
	return xOK;
} // CastEffToCastTargetByDirect

// 스킬발동대상얻기
// this는 시전자
int XSkillUser::GetInvokeTarget( XVector<XSkillReceiver*> *_plistOutInvokeTarget,		// 결과를 이곳에 받습니다
																 XSkillDat *pDat,
																 int level,
																 xtInvokeTarget invokeTarget,		// virtual이라서 사용자쪽에서 어떤변수를 써야하는지 분명히 알게 하기위해 직접 변수를 넘김
																 const EFFECT *pEffect,				// 효과.		
																 XSkillReceiver *pCastingTarget,		// 시전대상
																 const XE::VEC2& vPos )				// 기준타겟(좌표형)
{
	// virtual. invokeTarget을 하위클래스에서 가공하기 위한 땜빵.
	invokeTarget = pCastingTarget->OnGetInvokeTarget( pDat, pEffect, invokeTarget );
	//
	auto& aryTempInvokes = *_plistOutInvokeTarget;
//	XArrayLinearN<XSkillReceiver*, 512> aryTempInvokes;
	xtFriendshipFilt filtIvkFriendship = pEffect->invokefiltFriendship;
	if( filtIvkFriendship == xfNONESHIP )
		filtIvkFriendship = pEffect->castfiltFriendship;
	XBREAK( filtIvkFriendship == xfNONESHIP );
	//
	switch( invokeTarget ) {
	// 시전대상자
	case xIVT_CAST_TARGET:
	case xIVT_ATTACKED_TARGET:	// 피격자. 이때는 pCastingTarget이 피격자가 된다.
		if( IsInvokeAble( pDat, pCastingTarget, pEffect ) )
			aryTempInvokes.push_back( pCastingTarget );
		break;
	case xIVT_CAST_TARGET_RADIUS:
	case xIVT_CAST_TARGET_SURROUND:
	case xIVT_ATTACKED_TARGET_RADIUS:
	case xIVT_ATTACKED_TARGET_SURROUND: {
	// 여기에 case를 추가시킨다면 아래 if에도 추가할것.
		XSkillUser *pCaster = this;
		XSkillReceiver *pInvoker = pCastingTarget;
		if( pInvoker == nullptr || pInvoker->GetCamp() == 0/*xSIDE_NONE*/ )
			pInvoker = pCaster->GetThisRecv();
		XBREAK( pInvoker == nullptr );
		BIT bitSideSearchFilt = GetFilterSideInvoke( pCaster,
//													pCaster->GetThisRecv(),
													pCastingTarget,
													pEffect->invokeTarget,
													filtIvkFriendship );

		int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
//		BOOL bIncludeCenter = ( invokeTarget == xIVT_CAST_TARGET_RADIUS ) ? TRUE : FALSE;	// RADIUS(반경내)의 경우는 센터타겟을 포함하고 서라운드는 포함하지 않는다
		if( invokeTarget == xIVT_CAST_TARGET_RADIUS ||
			invokeTarget == xIVT_ATTACKED_TARGET_RADIUS ) {
			aryTempInvokes.Add( pCastingTarget );
			if( numApply > 1 )
				--numApply;			// 시전대상은 포함시켰으므로 적용수가 있었다면 하나 빼준다. 적용수가 1이면 그냥 추가한마리로 처리함.

		}
		// 발동자를 중심으로(발동자를 포함하거나/제외하거나) 반경내 sideSearchFilt에 해당하는
		// side편 대로 검색하여 invokeNumApply개의 리스트를 요청한다
		// 아직 구현안되어 있음
		float size = pEffect->GetInvokeSize( level );
		if( size == 0 )
			size = pEffect->castSize.w;
		size = pCaster->OnInvokeTargetSize( pDat, pEffect, level, pCastingTarget, size );
		GetListObjsRadius( &aryTempInvokes,
											 pDat,
											pEffect,
											pCastingTarget,		// 원의 중심이 되는 타겟
											vPos,			// 원의 중심 좌표
											size,
											bitSideSearchFilt,
											numApply,
											FALSE );
	} break;
	case xIVT_CAST_TARGET_PARTY:
	case xIVT_ATTACKED_TARGET_PARTY:
		// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
		pCastingTarget->GetGroupList( &aryTempInvokes,
																	pDat,
																	pEffect );
		break;
	case xIVT_CURR_TARGET: {
		// 현재 공격대상을 얻는다.
		XSkillReceiver *pCurrTarget = GetCurrTarget();  // virtual
		if( pCurrTarget && IsInvokeAble( pDat, pCurrTarget, pEffect ) )
			aryTempInvokes.push_back( pCurrTarget );
	} break;
	// 타격자
	case xIVT_ATTACKER: {
		auto pAttacker = pCastingTarget->GetpAttacker();
		if( pAttacker && IsInvokeAble( pDat, pAttacker, pEffect ) )
			aryTempInvokes.push_back( const_cast<XSkillReceiver*>( pAttacker ) );
	} break;
 	case xIVT_RANDOM_PARTY: {
		xtGroup typeGroup;
		if( filtIvkFriendship & xfALLY )
			typeGroup = xGT_RANDOM_PARTY_FRIENDLY;
		else
		if( filtIvkFriendship & xfHOSTILE )
			typeGroup = xGT_RANDOM_PARTY_ENEMY;
		auto pRecv = GetThisRecv();
 		pRecv->GetGroupList( &aryTempInvokes,
 												pDat,
 												pEffect,
												typeGroup );
 	} break;
	// 모두
	case xIVT_ALL: {
		xtGroup typeGroup;
		if( filtIvkFriendship & xfALLY )
			typeGroup = xGT_FRIENDLY_ALL;
		else
		if( filtIvkFriendship & xfHOSTILE )
			typeGroup = xGT_ENEMY_ALL;
		auto pRecv = GetThisRecv();
		pRecv->GetGroupList( &aryTempInvokes,
												pDat,
												pEffect,
												typeGroup );

	} break;
	default:
		XBREAKF(1, "잘못된 발동파라메터:%d", invokeTarget );
		break;
	} // switch
	//
	if( aryTempInvokes.size() > 0 ) {
		// 현재 사용되는곳이 없는듯.
//		CustomInvokeFilter( _plistOutInvokeTarget, aryTempInvoker, pEffect );
		(*_plistOutInvokeTarget) = aryTempInvokes;
	}
	return _plistOutInvokeTarget->size();
}
/**
 @brief Invoke대상에게 pEffect효과를 적용시킨다.
 @param pInvoker pEffect효과를 발동시킨측. pCaster와는 다르다.
 @return 효과적용에 성공하면 true를 리턴한다.
*/
bool XSkillUser::ApplyInvokeEffToIvkTarget( XSkillReceiver* pInvokeTarget,
																									XSkillDat *pDat,
																									const EFFECT *pEffect,
																									XSkillReceiver *pInvoker,
																									bool bCreateSfx,
																									int level,
																									const XE::VEC2& vPos,
																									XBuffObj *pBuffObj )
{
	// 발동적용확률이 있다면 확률검사를 통과해야 한다.(XBuffObj에 이것이 있을때는 GetInvokeTarget을 하기전에 수행되었는데 지금은 발동타겟 개별적으로 확률검사를 하도록 바뀜. 이게 맞는거 같아서)
	bool bOk = XSKILL::DoDiceInvokeApplyRatio( pEffect, level );
	if( !bOk )
		return false;
	bool bOrCond = false;
	// 조건블럭이 없으면 무조건 true
	if( pEffect->aryInvokeCondition.size() == 0 ) {
		bOrCond = true;
	} else
	// 발동조건 블럭검사.
	if( IsInvokeTargetCondition( pDat, pEffect, pInvokeTarget ) ) {
		bOrCond = true;
	}
	if( !bOrCond )
		return false;
	// 발동타겟에게 실제 효과적용
	int retApplied = pInvokeTarget->ApplyInvokeEffect( pDat, this
																										, pInvoker, pBuffObj
																										, pEffect, level );
	// 발동대상이펙트가 있다면 생성해준다.
	if( bCreateSfx && pEffect->m_invokeTargetEff.IsHave() ) {
		const float secPlay = 0.f;		// 1play. 발동이펙트는 반복플레이가 없음.
		pInvokeTarget->CreateSfx( pDat, pEffect->m_invokeTargetEff, secPlay, vPos );
	}
	if( pEffect->idInvokeSound ) {
		OnSkillPlaySound( pEffect->idInvokeSound );
	}
	return retApplied != 0;
} // ApplyInvokeEffectToInvokeTarget

/**
@brief ary에 담긴 Invoke대상들에게 pEffect효과를 적용시킨다.
@param pInvoker pEffect효과를 발동시킨측. pCaster와는 다르다.
*/
// void XSkillUser::ApplyInvokeEffectWithAry( 
// 																			const XVector<XSkillReceiver*>& aryInvokTarget
// 																			, XSkillDat *pDat
// 																			, const EFFECT *pEffect
// 																			, XSkillReceiver *pInvoker
// 																			, bool bCreateSfx
// 																			, int level
// 																			, const XE::VEC2& vPos
// 																			, XBuffObj *pBuffObj
// 																			, XLuaSkill *pLua )
// {
// 	for( auto pInvokeTarget : aryInvokTarget ) {
// 		// 발동확률이 있다면 확률검사를 통과해야 한다.(XBuffObj에 이것이 있을때는 GetInvokeTarget을 하기전에 수행되었는데 지금은 발동타겟 개별적으로 확률검사를 하도록 바뀜. 이게 맞는거 같아서)
// 		bool bOk = XSKILL::DoDiceInvokeApplyRatio( pEffect, level );
// 		if( bOk ) {
// 			bool bOrCond = false;
// 			// 조건블럭이 없으면 무조건 true
// 			if( pEffect->aryInvokeCondition.size() == 0 )
// 				bOrCond = true;
// 			else
// 			// 발동조건 블럭검사.
// 			if( IsInvokeTargetCondition( pDat, pEffect, pInvokeTarget ) )
// 				bOrCond = true;
// 			if( bOrCond ) {
// 				// 발동타겟에게 실제 효과적용
// 				pInvokeTarget->ApplyInvokeEffect( pDat, this
// 																				, pInvoker, pBuffObj
// 																				, pEffect, level );
// 				// 발동대상이펙트가 있다면 생성해준다.
// 				if( bCreateSfx ) {
// 					const float secPlay = 0.f;		// 1play. 발동이펙트는 반복플레이가 없음.
// 					pInvokeTarget->CreateSfx( pDat, /*pEffect,*/
// 																		pEffect->m_invokeTargetEff.m_strSpr,
// 																		pEffect->m_invokeTargetEff.m_idAct,
// 																		pEffect->m_invokeTargetEff.m_Point,
// 																		secPlay, vPos );
// 				}
// 				if( pEffect->idInvokeSound )
// 					OnSkillPlaySound( pEffect->idInvokeSound );
// 				// "발동시작"스크립 실행
// 	// 			if( pLua )
// 	// 				pLua->InvokeDoScript( pInvokeTarget, pEffect->scriptInit.c_str() );
// 				// 이벤트 핸들러
// 				//m_pDelegate->OnDotApply( pEffect );
// 			}
// 		}
// 	}
// }
/**
 @brief 발동조건이 있다면 조건검사를 한다.
*/
bool XSkillUser::IsInvokeTargetCondition( XSkillDat *pDat
																				, const EFFECT *pEffect
																				, XSkillReceiver *pInvokeTarget )
{
	XARRAYLINEARN_LOOP_AUTO( pEffect->aryInvokeCondition, const &condOr ) {
		bool bAnd = true;
		XARRAYLINEARN_LOOP_AUTO( condOr.aryParam, param ) {
			if( !pInvokeTarget->IsInvokeTargetCondition( pDat, pEffect
																									, param.cond, param.val ) ) {
				// 하나라도 틀리면 and조건 실패
				bAnd = false;
				break;
			}
		} END_LOOP;
		if( bAnd )
			// 하나라도 조건이 맞으면 무조건 true
			return true;
	} END_LOOP;
	return false;
}


int XSkillUser::FrameMove( float dt )
{
//	InvokePassiveSkill();		// 이제 패시브도 버프형태로 할 생각이므로 삭제
	return 1;
}

int XSkillUser::GetNumUseSkill( xCastMethod castMethod ) const 
{
	int n = 0;
	for( auto pSkillUseObj : m_listUseSkill ) {
		if( pSkillUseObj->IsSameCastMethod( castMethod ) )
			++n;
	}
	return n;
}

/**
 @brief 범용 이벤트 처리기
 skillUser용이므로 주로 스킬 발동에 관한 이벤트처리를 한다.
*/
void XSkillUser::OnEventBySkillUser( xtJuncture event )
{
	for( auto pUseSkill : m_listUseSkill ) {
		if( event == xJC_START ) {	// 땜빵
			// jc_start이벤트때는 패시브나 특성스킬이 시전된다.
			if( pUseSkill->IsPassiveCategory() || pUseSkill->IsAbilityCategory() ) {
				const int level = GetSkillLevel( pUseSkill );
				const auto info = UseSkill( pUseSkill, level, nullptr, nullptr );
				if( info.errCode == xOK ) {
					OnShootSkill( info, 0 );
				}
			}
		}
	}
}

// 시전대상우호가 시전대상에 비교하던것을 시전자와 비교하는것으로 바뀌었다.
BIT XSkillUser::GetFilterSideCast( XSkillReceiver *pCaster,	// XSkillUser로 바꿔야함.
									xtCastTarget targetType, 
									XSkillReceiver *pBaseTarget,
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pCaster == nullptr );
	BIT side = 0;
	if( targetType == xCST_BASE_TARGET )	
		side = pBaseTarget->GetCamp();	// 시전대상이 기준타겟이면 기준타겟의 우호를 그대로 사용
	else 
	{
		if( XECompCamp::sIsNeutrality( pCaster->GetCamp() ) )	// 시전자가 중립일때
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetNeutralitySideFilter();
			// 중립의 적은 원래 없는데 표현이 애매해서 중립이 아닌편은 모두 적으로 간주
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() );
		} else
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetSide( pCaster->GetCamp() );	// 시전자와 같은편 필터를 만든다.
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() ); // 시전자와 반대되는 Side필터를 만든다.
		}
	}
	return side;
}
// 발동자(pInvoker)를 기준으로 sideFilter를 적용했을때 검색해야 하는 필터를 돌려준다.
// 발동자가 적일 경우 sideFilter가 "아군"이면 필터가 반대로 반대로 돌아가야하므로...
// this는 invoker
/**
 @brief 발동대상의 우호를 얻는다. 시전자 기준의 우호임.
*/
BIT XSkillUser::GetFilterSideInvoke( XSkillUser *pCaster, 
// 									XSkillReceiver *pInvoker, 
 									XSkillReceiver *pCastingTarget,
									xtInvokeTarget targetType, 
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pCaster == nullptr );
	XBREAK( pCaster == nullptr );
	BIT bitSide = 0;
	if( targetType == xIVT_CAST_TARGET )	// 발동대상이 시전대상자 자신이면
		bitSide = pCaster->GetCampUser();	// 시전대상자의 프렌드쉽을 검색한다
	else
	{
		if( XECompCamp::sIsNeutrality( pCaster->GetCampUser() ) )	// 시전자가 중립일때
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				bitSide = XECompCamp::sGetNeutralitySideFilter();
			// 중립의 적은 원래 없는데 표현이 애매해서 중립이 아닌편은 모두 적으로 간주
			if( friendshipFilter & xfHOSTILE )
				bitSide = XECompCamp::sGetOtherSideFilter( pCaster->GetCampUser() );
		}
		else
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				bitSide = XECompCamp::sGetSide( pCaster->GetCampUser() );	// 시전자와 같은편 필터를 만든다.
			if( friendshipFilter & xfHOSTILE )
				bitSide = XECompCamp::sGetOtherSideFilter( pCaster->GetCampUser() ); // 시전자와 반대되는 Side필터를 만든다.
		}
	}
	return bitSide;
}

// this가 pCstTarget에게 발동이 가능한지 검사
BOOL XSkillUser::IsInvokeAble( XSkillDat *pDat, const XSkillReceiver *pCstTarget, const EFFECT *pEffect )
{
	xtFriendshipFilt invokefiltFriendship = pEffect->invokefiltFriendship;
	// 발동대상우호가 지정되지 않았으면 시전대상우호를 가져다 쓴다.
	// 스킬로드시에 미리 보정하도록 바뀜
	if( invokefiltFriendship == xfNONESHIP )
	{
		invokefiltFriendship = pEffect->castfiltFriendship;
		if( invokefiltFriendship == xfNONESHIP )
			invokefiltFriendship = pDat->GetbitBaseTarget();
		XBREAKF( invokefiltFriendship == xfNONESHIP, "스킬\"%s\":발동대상우호가 없다.", pDat->GetstrIdentifier().c_str() );
	}
	// AI/PLAYER검사를먼저 하는게 좋을듯 하다. AI가 발동대상우호 AI같은거일때 아래에서 실패걸릴듯
	if( (invokefiltFriendship & xfHOSTILE) == 0 )	// 적에게는 쓸수 없는스킬인데
		// 시전대상이 중립도 아닌데 나랑 우호가 다르면(적) 쓸수 없음.
		if( XECompCamp::sIsNotFriendly( pCstTarget->GetCamp(), GetCampUser()) && 
			XECompCamp::sIsNeutrality( pCstTarget->GetCamp() ) )
			return FALSE;							// 적에겐 사용할수 없습니다
	if( (invokefiltFriendship & xfALLY) == 0 )		// 아군에게는 쓸수 없는스킬인데
		// 시전자와 타겟이 같은편이면 쓸수 없음.
		if( XECompCamp::sIsFriendly( pCstTarget->GetCamp(), GetCampUser() ) )
			return FALSE;					// 아군에게는 쓸수 없습니다.
	if( XECompCamp::sIsNeutrality(invokefiltFriendship ) )	// 중립에게는 쓸수 없는스킬인데
		// 발동타겟(this)이 중립이면 사용할수 없음.
		if( XECompCamp::sIsNeutrality( GetCampUser() ) )		
			return FALSE;						

	return TRUE;
}

XSkillDat* XSkillUser::FindUseSkillByID( ID idSkillDat )
{
	return m_listUseSkill.FindByID( idSkillDat );
}
XSkillDat* XSkillUser::FindUseSkillByIds( LPCTSTR ids )
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );	
	auto pDat = m_prefSkillMng->FindByIds( ids );
	if( XBREAKF( pDat == nullptr, "%s: skill not found!", ids ) )
		return nullptr;
	auto pUseSkill = FindUseSkillByID( pDat->GetidSkill() );
	return pUseSkill;
}

XSkillUser::xUseSkill XSkillUser::UseSkillByID( XSkillDat **ppOutUseSkill,
																								int level,
																								ID idSkill,
																								XSkillReceiver *pTarget,
																								XE::VEC2 *pvPos )
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = nullptr;
	auto pUseSkill = FindUseSkillByID( idSkill );
	XASSERTF( pUseSkill, "idSkill:%d 스킬을 보유스킬목록에서 찾지못했습니다.", idSkill );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, level, pTarget, nullptr );
}
// 식별자로 스킬을 사용케한다. 만약 스킬이 없다면 스킬사용 오브젝트를 새로만들어
// 등록시키고 사용한다. 스킬발동에 사용한다.
XSkillUser::xUseSkill 
XSkillUser::UseSkillByIds( LPCTSTR idsSkill,
													 int level,
													 XSkillReceiver *pTarget,
													 XE::VEC2 *pvPos,
													 XSkillDat **ppOutUseSkill )
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = nullptr;
	
// 	auto pUseSkill = CreateAddUseSkillByIds( szIdentifier );
	// 발동스킬은 m_liseUseSkill에 들어가면 안되므로 안쓰는 버전으로 바꿈.
	auto pUseSkill = XESkillMng::sGet()->FindByIds( idsSkill );
	if( XBREAK( pUseSkill == nullptr ) )	{
		xUseSkill info;
		info.errCode = xERR_CRITICAL_ERROR;
		return info;
	}
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, level, pTarget, pvPos );
}

xtTargetFilter MakeTargetFilter( const EFFECT *pEffect )
{
	xtTargetFilter tfFilter = xTF_NONE;
	DWORD dwFilter = tfFilter;
	if( pEffect->liveTarget == xTL_LIVE )
		dwFilter |= xTF_LIVE;
	else if( pEffect->liveTarget == xTL_DEAD )
		dwFilter |= xTF_DEAD;
	tfFilter = (xtTargetFilter) dwFilter;
	return tfFilter;
}

// XSkillObj* XSkillUser::CreateSkillUseObj( XSkillDat *pSkillDat ) 
// { 
// 	return new XSkillObj( pSkillDat ); 
// }	

// idSkillDat스킬로 사용스킬객체를 만들어서 보유한다
XSkillDat* XSkillUser::CreateAddUseSkillByID( ID idSkillDat )
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );
	auto pDat = m_prefSkillMng->FindByID( idSkillDat );
	if( XASSERT(pDat) ) {
		AddUseSkill( pDat );		// 보유스킬목록에 추가
	}
	return pDat;
}

// this가 시전자
// 식별자로 스킬사용객체를 만들어 보유목록에 추가한다.
XSkillDat* XSkillUser::CreateAddUseSkillByIds( LPCTSTR szIdentifier )
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );
	auto pDat = m_prefSkillMng->FindByIds( szIdentifier );
	if( XASSERT(pDat) ) {
		AddUseSkill( pDat );		// 보유스킬목록에 추가
	}
	return pDat;
}

/**
 @brief 버프 인스턴스를 만든다.
*/
XBuffObj* XSkillUser::CreateSkillBuffObj( XSkillUser *pCaster,
																					XSkillReceiver *pCastingTarget,
																					XSkillDat *pDat,
																					int level,
																					const XE::VEC2& vPos,
																					ID idCallerSkill )
{
	return new XBuffObj( GetpDelegate(),
											 pCaster,
											 pCastingTarget,
											 pDat,
											 level,
											 vPos,
											 idCallerSkill );
}

void XSkillUser::AddUseSkill( XSkillDat* pUseSkill ) 
{
	if( FindUseSkillByIds( pUseSkill->GetstrIdentifier().c_str() ) )
		return;
	m_listUseSkill.push_back( pUseSkill );
}




XE_NAMESPACE_END		// XSKILL
