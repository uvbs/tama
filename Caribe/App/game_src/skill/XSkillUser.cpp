﻿#include "stdafx.h"
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
XLuaSkill* XSkillUser::CreateScript( void ) { 
	return new XLuaSkill; 
} 		// XLua 객체를 생성하고 전역변수와 API들을 등록하여 돌려준다

void XSkillUser::Destroy()
{
	XLIST_DESTROY( m_listUseSkill, XSkillObj* );
}

void XSkillUser::OnCreate()
{
	// 시작시 패시브스킬을 실행시킨다.
	InvokePassiveSkill();
}

/**
 @brief 스킬 최초 사용
 pUseSkill을 사용하기 위한 시작함수. 이 함수가 성공하면 스킬모션이 시작된다.
 캐스팅 시작이라고 봐도 됨.
 @param pCurrTarget 현재 this가 잡고 있는 공격타겟. 혹은 직접 터치로 찍은 타겟. 스킬의 타입에 따라서 이 타겟은 안쓰여 질수도 있다.
 @param pvTouchPos 유저가 직접 터치로 바닥을 찍었다면 그 좌표
*/
XSkillUser::xUseSkill XSkillUser::UseSkill( XSkillObj *pUseSkill,
							int level,
//							xCastMethod castMethod,
							XSkillReceiver *pCurrTarget,
							XE::VEC2 *pvTouchPos )
{
	xUseSkill infoUseSkill;
	XE::VEC2 vTouchPos = ( pvTouchPos ) ? *pvTouchPos : XE::VEC2( 0 );
	if( XBREAK( pUseSkill == NULL ) ) {
		infoUseSkill.errCode = xERR_CRITICAL_ERROR;
		return infoUseSkill;
	}
	// 쿨타임 검사.(쿨타임은 사용하지 않는 경우도 많으니 콤포넌트형태로 빼서 옵션처럼 사용할수 있도록 하자)
	if( pUseSkill->GettimerCool().IsOn() ) {				// 쿨타임이 꺼져있으면 쿨타임 다 돈거니까 걍 통과
		if( pUseSkill->GettimerCool().IsOver() == FALSE ) {		// 쿨링이 아직 안끝났으면
			infoUseSkill.errCode = xERR_READY_COOLTIME;			// 재사용 대기중입니다
			return infoUseSkill;
		}
		pUseSkill->GettimerCool().Off();	// 쿨링타이머 끔
	}
	XSkillDat *pSkillDat = pUseSkill->GetpDat();
	// 이미 시전된 패시브는 다시 시전하지 않게 한다
	if( pUseSkill->GetpDat()->IsPassiveType() )  {
		if( pUseSkill->GetbAlreadyCast() == TRUE ) {
			infoUseSkill.errCode = xERR_ALREADY_APPLY_SKILL;			// 이미 시전된 스킬입니다
			return infoUseSkill;
		}
	}
	// 쿨타이머 작동-
	// 쿨타이머셋이 UseEffect보다 먼저 있는 이유: 
	// 만약 지역마법을 썼는데 그자리에 아무도 없었다면 캐스팅타겟 없음으로 해서 UseEffect가 리턴된다 
	// 그러나 이경우에도 쿨타이머는 돌아가야 하기때문이다
	if( pSkillDat->GetfCoolTime() != 0 )
		pUseSkill->GettimerCool().Set( pSkillDat->GetfCoolTime() );
	// 시전자 이펙트(시전시작 시점에 발생되는 이펙트)
	if( pSkillDat->GetCasterEff().m_strSpr.empty() == false ) {
//		xtPoint pointSfx = XSKILL::xPT_TARGET_BOTTOM;
		float secPlay = 0.f;	// play once
		CreateSfx( pSkillDat, 
//					nullptr,
					pSkillDat->GetCasterEff().m_strSpr, 
					pSkillDat->GetCasterEff().m_idAct,
					pSkillDat->GetCasterEff().m_Point,
					secPlay, vTouchPos );
	}

	// 패시브 시전이 성공하면 플래그 바꿈
	if( pUseSkill->GetpDat()->IsPassiveType() )
			pUseSkill->SetbAlreadyCast( TRUE );
	infoUseSkill.pUseSkill = pUseSkill;
	infoUseSkill.level = level;
	// 기준타겟이 자신이면 this로 타겟을 바꾼다.
	if( pSkillDat->GetbaseTarget() == xBST_SELF )
		infoUseSkill.pBaseTarget = GetThisRecv();
	else
	if( pSkillDat->GetbaseTarget() == xBST_CURR_TARGET ) {
//		XBREAK( pCurrTarget == nullptr );
		// 현재타겟과 기준타겟우호가 맞지 않으면 일단 상속 클래스에 의뢰한다.
		infoUseSkill.pBaseTarget = pCurrTarget;
		if( pCurrTarget == nullptr ||
			(pSkillDat->GetbitBaseTarget() == xfALLY && 
			 pCurrTarget->GetCamp().IsFriendly(GetCampUser()) == FALSE) ||
			( pSkillDat->GetbitBaseTarget() == xfHOSTILE &&
			 pCurrTarget->GetCamp().IsEnemy( GetCampUser() ) == FALSE ) )
				infoUseSkill.pBaseTarget = GetSkillBaseTarget( pSkillDat );
		// 타겟을 그래도 못찾은경우 에러코드 리턴(주변에 적당한 타겟이 없다거나 하면 생길수 있음.)
		if( infoUseSkill.pBaseTarget == nullptr ) {
			infoUseSkill.errCode = xERR_MUST_SELECT_TARGET;
			return infoUseSkill;
		}
	} else
	if( pSkillDat->GetbaseTarget() == xBST_POSITION ) {
		if( pvTouchPos )
			infoUseSkill.vTarget = *pvTouchPos;
		// 좌표가 없다면 하위클래스에 일단 의뢰해본다.
		if( infoUseSkill.vTarget.IsZero() )
			infoUseSkill.vTarget = GetSkillBaseTargetPos( pSkillDat );
		// 기준타겟을 좌표로 하겠다고 했으면 좌표가 있어야함.
		if( XBREAK(infoUseSkill.vTarget.IsZero()) ) {
			infoUseSkill.errCode = xERR_MUST_SELECT_TARGET;
			return infoUseSkill;
		}
	} else
	// 기준타겟조건
	if( pSkillDat->GetbaseTarget() == xBST_CONDITION ) {
		infoUseSkill.pBaseTarget = GetBaseTargetByCondition( pSkillDat, 
																					pSkillDat->GetcondBaseTarget(),
																					pSkillDat->GetrangeBaseTargetCond(),
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
void XSkillUser::CreateSfx( XSkillDat *pSkillDat,
//														EFFECT *pEffect,
														const _tstring& strEffect, 
														ID idAct,
														xtPoint pointSfx,
														float secPlay,
														const XE::VEC2& vPos )
{
// 	if( XBREAK( strEffect.empty() == true ) )
// 		return;
	if( strEffect.empty() )
		return;
	float secLife = 0.f;
	// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
	if( pointSfx == xPT_NONE )
		pointSfx = xPT_TARGET_BOTTOM;
	if( idAct == 0 )
		idAct = 1;
	OnCreateSkillSfx( pSkillDat, 
//										pEffect,
										pointSfx,
										strEffect.c_str(),
										idAct,
										secPlay,
										vPos );
}

/**
 @brief 첫번째 보유스킬을 사용한다
 그냥 편의 함수.
*/
XSkillUser::xUseSkill XSkillUser::UseSkill( XSkillReceiver *pTarget,
							XE::VEC2 *pvPos )
{
	xUseSkill info;
	if( XBREAK( m_listUseSkill.size() <= 0 ) ) {
		info.errCode = xCANCEL;
		return info;
	}
	XSkillObj *pUseSkill = m_listUseSkill.GetFirst();
	int level = GetSkillLevel( pUseSkill );
	return UseSkill( pUseSkill, level, pTarget, NULL );
}

/**
 @brief 스킬모션의 타점때마다 호출되어 스킬을 시전한다.
*/
xtError XSkillUser::OnShootSkill( XSkillObj *pUseSkill
																, XSkillReceiver *pBaseTarget
																, int level
																, const XE::VEC2& vTarget
																, ID idCallerSkill )
{
	if( XBREAK(pUseSkill == nullptr || pBaseTarget == nullptr) )
		return xERR_GENERAL;

//	XSkillObj *pUseSkill = m_pUseSkill;
	XSkillDat *pSkillDat = pUseSkill->GetpDat();
//	const XE::VEC2 *pvPos = ( m_vTarget.IsZero() ) ? nullptr : &m_vTarget;
	// 시전자의 타점에 생성되는 이펙트(루핑없음)
	if( pSkillDat->GetShootEff().m_strSpr.empty() == false )
	{
		float secPlay = 0;	// once
		CreateSfx( pSkillDat,
//					nullptr,
					pSkillDat->GetShootEff().m_strSpr,
					pSkillDat->GetShootEff().m_idAct,
					pSkillDat->GetShootEff().m_Point,
					secPlay, vTarget );
	}
	// 슈팅타겟이펙트(슈팅시점에 타겟에게 발생한다. 보통 타점을 포함하고 있다)
	if( pSkillDat->GetShootTargetEff().m_strSpr.empty() == false )
	{
		float secPlay = 0;	// once
		XSkillSfx *pSfx = pBaseTarget->OnCreateSkillSfxShootTarget( 
								pSkillDat,
								pBaseTarget,
								level,
								pSkillDat->GetShootTargetEff().m_strSpr,
								pSkillDat->GetShootTargetEff().m_idAct,
								pSkillDat->GetShootTargetEff().m_Point,
								secPlay, vTarget );
		pSfx->RegisterCallback( this, pSkillDat, level, pBaseTarget, vTarget );
		// 슈팅타겟이펙트는 여기서 sfx만 생성하고 리턴한다음 sfx의 타점에서 CastSkillToBaseTarget이 호출된다.
		return xOK;
	}
	// 발사체방식인가
	if( pSkillDat->GetstrShootObj().empty() == false &&
		pSkillDat->GetidShootObj() )
	{
		// 타겟까지 날아가서 시전자->스킬발동()을 한다
		// virtual. 발사체오브젝트를 생성하고 오브젝트 매니저에 등록한다
		auto pShootObj = CreateAndAddToWorldShootObj( pSkillDat,
													level,
													pBaseTarget,
// 													pCastingTarget,
													this,
													vTarget );
		// 타겟에 도달하면 CastEffectToCastingTarget( 넘겨줬던 파라메터 )를 해준다.
		XASSERT( pShootObj );
	} else
	{
		// 기준타겟에 스킬을 시전한다.
		CastSkillToBaseTarget( pSkillDat, level, pBaseTarget, vTarget, idCallerSkill );
	}
	return xOK;
}

/**
 @brief 기준타겟에 스킬을 시전한다.
 이함수는 슈팅타겟이펙트 사용시 단독으로도 호출될수 있다.
*/
void XSkillUser::CastSkillToBaseTarget( XSkillDat *pSkillDat
																			, int level
																			, XSkillReceiver *pBaseTarget
																			, const XE::VEC2& vPos
																			, ID idCallerSkill)
{
// 	// 시전자 이펙트
// 	if( !pSkillDat->GetCasterEff().m_strSpr.empty() ) {
// 		auto& eff = pSkillDat->GetCasterEff();
// 		float secPlay = 0;
// 		CreateSfx( pSkillDat,
// 								eff.m_strSpr,
// 								eff.m_idAct,
// 								eff.m_Point,
// 								secPlay );
// 	}
	// 스킬이 가진 효과들 기준타겟에게 사용한다.
	for( auto pEffect : pSkillDat->GetlistEffects() )	{
		xtError err = UseEffect( pSkillDat, pEffect, level, pBaseTarget, vPos, idCallerSkill );
		if( err != xOK )
			return;
	}
}

/**
 @brief 효과 시전
 pEffect의 효과를 기준타겟(pBaseTarget)을 기준으로 시전대상을 선정하여 시전한다.
 현재 _vPos는 사용하지 않는다
*/
xtError XSkillUser::UseEffect( XSkillDat *pSkillDat,
								EFFECT *pEffect,
								int level,
								XSkillReceiver *pBaseTarget,
								const XE::VEC2& _vPos
								, ID idCallerSkill )
{
	XVector<XSkillReceiver*> aryCastingTargets;		// 시전대상
	// 시전대상을 선정한다. 대상은 객체일수도 있고 좌표일수도 있다.
	// 기준타겟이 객체일경우
	XE::VEC2 vPos = _vPos;		// 시전대상이 좌표로 넘어올경우 여기로 받는다. 디폴트로는 외부좌표를 받는다.
	if( pBaseTarget )	{
		// 기준타겟의 좌표를 시전대상으로 한다.
		if( pEffect->castTarget == xCST_BASE_TARGET_POS )		{
			if( pEffect->IsDuration() )			{
				// 버프를 가질 sfx용 리시버 객체를 생성.
				float sec = pEffect->GetDuration( level );
				XBREAK( sec <= 0 );
				XSkillReceiver *pSfx = CreateSfxReceiver( pEffect, sec );
				aryCastingTargets.Add( pSfx );
			}
			// 
		} else
		// 객체류를 시전대상으로 한다.
		{
			// 타겟조건에 따라 시전대상을 가려낸다. 시전대상이 좌표일경우 vPos로 받는다.
			int numCastingTarget = GetCastingTargetList( &aryCastingTargets,
														pEffect->castTarget,
														pSkillDat,
														pEffect,
														pBaseTarget,
														&vPos );
		}
	} else
	// 기준타겟이 좌표일경우
	{
		XBREAK( pBaseTarget == nullptr );
		if( vPos.IsZero() == FALSE )
			XALERT( "%s", _T("아직 구현되지 않음. 기준타겟:좌표") );
		return xERR_GENERAL;
	}
	if( aryCastingTargets.size() == 0 && vPos.IsZero() )
		return xERR_NOT_FOUND_CASTING_TARGET;		// 시전대상을 찾지 못함
	// 캐스팅 대상들의 루프
//	XARRAYLINEARN_LOOP( listCastingTargets, XSkillReceiver*, pCastingTarget )
	for( auto pCastingTarget : aryCastingTargets ) {
		{	// 즉시발동 or 버프방식
			CastEffectToCastingTarget( pSkillDat, 
									pEffect, 
									level, 
									pBaseTarget, 
									pCastingTarget, 
									vPos
									, idCallerSkill );
		}
	}
	// "사용"스크립트 실행
	if( pEffect->scriptUse.empty() == false )	// 스크립이 있을때만
	{
		XLuaSkill *pLua = CreateScript();	// virtual
		pLua->RegisterScript( this, NULL );
		pLua->TargetDoScript( GetThisRecv(), pEffect->scriptUse.c_str() );
		SAFE_DELETE( pLua );
	}

	return xOK;
/*
	발동시점은 디폴트로 "시전시"가 된다.
	시전시점은 디폴트로 "사용시"가 되며 스킬이 사용될때 바로 시전대상에게 시전된다.
	패시브의 	발동대상이 기준타겟이라면 패시브가 걸리는 순간(시전시) 발동대상에게 효과가 간다.
	발동시점이 "공격시"일때 평타공격이 시작되면 발동효과가 발동대상에게 발동된다.
	평타공격이 원거리공격이라면 발동효과가 발동대상에게 적용되는 시점이 발사체가 도착한후로 미뤄진다.
	그러나 원거리공격에 발동확률이 있다면 발동확률은 발사시점에 이미 결정되어야 한다.(그래야 다른 비주얼의 발사체를 쓸수 있으니까.)
	스킬자체가 발사체형태라면 도착하고 발동확률이 계산되고
*/
}

// pTarget을 시전대상으로 했을때 얻어지는 모든 시전대상을 얻는다.
/**
 @brief 
*/
int XSkillUser::GetCastingTargetList( 
					XVector<XSkillReceiver*> *pAryOutCastingTarget,			// 시전대상얻기 결과가 담겨짐
					xtCastTarget castTarget,	// 시전대상타입
					XSkillDat *pSkillDat,				// 스킬사용 오브젝트
					const EFFECT *pEffect,					// 효과
					XSkillReceiver *pBaseTarget, 
					XE::VEC2 *pvOutPos )
{
	XBREAK( pBaseTarget == NULL );
//	XBREAK( pBaseTarget == nullptr && vPos.IsZero() );
	switch( castTarget )
	{
	case xCST_BASE_TARGET:		// 기준타겟
		pAryOutCastingTarget->Add( pBaseTarget );
		break;
		// 기준타겟좌표
// 	case xCST_BASE_TARGET_POS: {
// 		*pvOutPos = pBaseTarget->GetCurrentPosForSkill();	// 기준타겟의좌표
// 		//		XBREAK(1);		// 이 파라메터로 이함수를 부르면 안됨.
// 	} break;
	case xCST_BASE_TARGET_RADIUS:		// 기준타겟반경
	case xCST_BASE_TARGET_SURROUND:		// 기준타겟주변
	{
		// 기준타겟을 중심으로 원형 반경내에서의 객체를 선택한다.
		BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(),
													castTarget,
													pBaseTarget,
													pEffect->castfiltFriendship );
		int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
		// 기준타겟포함이면 기준타겟은 우선으로 리스트에 들어가고 기준타겟제외로 검색한다.
		if( castTarget == xCST_BASE_TARGET_RADIUS )
		{
			pAryOutCastingTarget->Add( pBaseTarget );
			if( numApply > 1 )
				--numApply;
		}
		float castSize = pEffect->castSize.w;
		// 시스템화는 이런식으로...
// 		if( pEffect->증폭파라메터 == 시전범위 )
// 		{
// 			float sizeAdd = 0.f;
// 			float sizeAddMultiply = 0.f;
// 			OnSkillAmplifyUser( pSkillDat, nullptr, pEffect, &sizeAddMultiply, &sizeAdd );
// 			castSize += sizeAdd;
// 			castSize = castSize + castSize * sizeAddMultiply;
// 		}
		// 일단은 그냥 하드코딩
		float sizeAdd = 0.f;
		float sizeAddMultiply = 0.f;
		OnSkillAmplifyUser( pSkillDat, nullptr, pEffect, xEA_CAST_RADIUS, &sizeAddMultiply, &sizeAdd );
		castSize += sizeAdd;
		castSize = castSize + castSize * sizeAddMultiply;
		XE::VEC2 vPosExtern;	// 외부에서 넘어온 원의중심좌표(현재는 사용하지 않음)
		GetListObjsRadius( pAryOutCastingTarget,
							pEffect,
							pBaseTarget,		// 기준타겟
							vPosExtern,		// 중심이 타겟형태가 아니고 좌표형태로 넘어오는경우 사용됨.현재는 사용되지 않음
							castSize,
							bitSideSearchFilter,
							numApply,
							FALSE );
		break;
	}
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


/* 중복시전에 관한 여러가지 케이스
	1. 시전대상이 같은 효과가 2개 있을경우 최초버프: 정상
	2. 시전대상이 다른 효과가 2개 있을경우 최초버프: 정상
	3. 시전대상이 같은 효과가 2개있는 스킬을 내가 한번더 쓸때: 시간만 리셋되고 정상시전됨
	4. 시전대상이 다른 효과가 2개있는 스킬을 내가 한번더 쓸때: 상동
	if( 중복불가 )
		3. 시전대상이 같은 효과가 2개 있는경우 다른사람의 같은버프가 이미 걸려있는경우: 그 버프에 시간만 리셋시킬수 있다.
		4. 시전대상이 다른 효과가 2개 있을경우 다른사람의 같은버프가 이미 걸려있는경우: 그 버프에 시간은 리셋되고, 다른대상에 걸리는 버프는 새로 생성된다
	if( 중복가능 )
		5. 시전대상이 같은 효과가 2개 있는경우 다른사람의 같은버프가 이미 걸려있는경우: 내버프가 따로 걸리면서 정상시전됨
		6. 시전대상이 다른 효과가 2개 있을경우 다른사람의 같은버프가 이미 걸려있는경우
		*/
/**
 @brief 시전대상에게 효과시전
 이 함수는 시전대상의 수만큼 불린다.
 @param pBaseTarget 기준타겟
 @param pCastingTarget 시전대상들
*/
xtError XSkillUser::CastEffectToCastingTarget( 
								XSkillDat *pSkillDat, 
								EFFECT *pEffect, 
								int level,
								XSkillReceiver *pBaseTarget, 
								XSkillReceiver *pCastingTarget, 
								const XE::VEC2& vPos
								, ID idCallerSkill)		
{
	XBREAK( pSkillDat == NULL );
	XBREAK( pEffect == NULL );
	// 기준타겟에게 생성되는 이펙트(루핑없음)
	if( pBaseTarget == pCastingTarget &&
		pSkillDat->GetTargetEff().m_strSpr.empty() == false )
	{
		float secPlay = 0;	// once
		// 이펙트반복으로 되어있으면 지속시간동안 플레이되도록 값을 넣어줌.
		if( pSkillDat->GetTargetEff().m_Loop == xAL_LOOP )
			secPlay = pEffect->GetDuration( level );
		pBaseTarget->CreateSfx( pSkillDat,
//								pEffect,
								pSkillDat->GetTargetEff().m_strSpr,
								pSkillDat->GetTargetEff().m_idAct,
								pSkillDat->GetTargetEff().m_Point,
								secPlay, vPos );
	}
	// 시전대상들에게 모두 이펙트를 붙인다.
	if( pEffect->m_CastTargetEff.m_strSpr.empty() == false )
	{
		float secPlay = 0;
		pCastingTarget->CreateSfx( pSkillDat,
//									pEffect,
									pEffect->m_CastTargetEff.m_strSpr,
									pEffect->m_CastTargetEff.m_idAct,
									pEffect->m_CastTargetEff.m_Point,
									secPlay );
	}
	// 지속시간형 타입인가
	if( pSkillDat->IsBuff(pEffect) )		{
		// 발사체의 형태로 시전대상에게 날아오다 시전대상이 사라지면 대상이 널이 될수 있음
		if( pCastingTarget )		{
			XBuffObj *pSkillBuff = NULL;
			BOOL bCreateBuff = FALSE;
			// 사용하려는 스킬의 버프객체가 시전대상에 이미 있는지 검사. 
			// 시전자가 다른 같은버프가 중복으로 걸릴수도 있으므로 시전자까지 같아야 한다
			pSkillBuff = pCastingTarget->FindBuffSkill( pSkillDat->GetidSkill(), this );
			if( pSkillBuff == nullptr )			{	
				// 시전자도 같고 아이디도 같은 버프는 없다. 그렇다면
				// 시전자는 달라도 아이디는 같은 버프는 있는가?
				pSkillBuff = pCastingTarget->FindBuffSkill( pSkillDat->GetidSkill() );
				if( pSkillBuff ) {	// 시전자는 다르고 아이디만 같은 경우
					if( pEffect->bDuplicate == FALSE ) {
						// 중복불가의 경우는 이렇게....
						// 이런경우는 기존버프에 시간만 리셋해준다
					} else
						// 중복가능한 스킬이면 버프를 새로 생성한다
						bCreateBuff = TRUE;
				} else
					// 아이디 같은 버프도 없다
					bCreateBuff = TRUE;
			}
			// 지속시간형 객체를 생성한다.
			if( bCreateBuff ) {
				XBREAK( pSkillBuff != NULL );
				// 버프오브젝트만 일단 생성하고 이펙트오브젝트는 시전대상에 맞는것만 따로 추가한다
				pSkillBuff = CreateSkillBuffObj( this, 
												pCastingTarget, 
												pSkillDat, 
												level,
												vPos,
												idCallerSkill );	
				XASSERT( pSkillBuff );	
				// 시전대상의 버프리스트에 추가
				pCastingTarget->AddSkillRecvObj( pSkillBuff );	
				// 시전대상에게 버프가 추가된 직후 이벤트가 발생한다.
				pCastingTarget->OnAddSkillRecvObj( pSkillBuff, pEffect );		
			}
			XBREAK( pSkillBuff == NULL );
			// 효과를 추가하고 타이머를 작동시킴
			// 이펙트오브젝트를 추가. 이미 효과가 있으면 기존거를 리턴
			EFFECT_OBJ *pEffObj = pSkillBuff->AddEffect( pEffect );			
			// "시전"스크립트 실행
			pSkillBuff->ExecuteScript( GetThisRecv(), pEffect->scriptCast.c_str() );		
			// 시전사운드 플레이
			if( pEffect->idCastSound )
				OnPlaySoundUse( pEffect->idCastSound );		// virtual
			// 타이머 켬
			if( pSkillDat->IsActive() )		// 지속시간 무한대는 타이머가 켜지지 않는다
			{ 
				// 지속시간 타이머를 셋
				if( pEffect->IsDuration() ) {
					pEffObj->timerDuration.Set( pEffect->GetDuration(level) );		
					pEffObj->cntDot = 0;
				}
			}
			// 버프객체에 이벤트 발생
			pSkillBuff->OnCastedEffect( pCastingTarget, pEffObj );
			if( pEffect->m_PersistEff.m_strSpr.empty() == false )
			{
				float secPlay = pEffect->GetDuration(level);	// 지속이펙트는 무조건 루핑.
				pCastingTarget->CreateSfx( pSkillDat,
//										pEffect,
										pEffect->m_PersistEff.m_strSpr,
										pEffect->m_PersistEff.m_idAct,
										pEffect->m_PersistEff.m_Point,
										secPlay );
			}
		} // if(pCastingTarget)
	} else
	{	// 즉시발동형(지속시간 0)
		// 시전대상들에게 즉시 효과가 발동된다.
		XVector<XSkillReceiver*> listInvokeTarget;
		// 1회성이기때문에 루아도 쓰고 바로 파괴되는걸로 함
		XLuaSkill *pLua = NULL;
		// "시전"스크립트 실행
		if( pEffect->scriptCast.empty() == false ) 
		{	// 스크립이 있을때만
			pLua = CreateScript();	// virtual
			pLua->RegisterScript( this, pCastingTarget );
			pLua->TargetDoScript( GetThisRecv(), pEffect->scriptCast.c_str() );
			SAFE_DELETE( pLua );
		}
		// 시전대상자(pCastingTarget)기준 발동대상 얻음
		int numInvoke = GetInvokeTarget( &listInvokeTarget,
																		pSkillDat,
																		level,
																		pEffect->invokeTarget,
																		pEffect,
																		pCastingTarget,
																		vPos );
		if( numInvoke > 0 ) {
			// 시작스크립트 실행. 
			if( pEffect->scriptInit.empty() == false ) {// 스크립이 있을때만
				if( pLua == NULL ) {
					pLua = CreateScript();	// virtual
					pLua->RegisterScript( this, pCastingTarget );
				}
			}
			bool bCreateSfx = ( pEffect->m_invokeTargetEff.m_strSpr.empty() )? false : true;
			// 발동대상들에게 효과 적용
			ApplyInvokeEffectWithAry( listInvokeTarget
															, pSkillDat
															, pEffect
															, GetThisRecv()		// invoker
															, bCreateSfx
															, level
															, vPos
															, nullptr
															, nullptr );
 		}
		SAFE_DELETE( pLua );
		// 즉시발동형
		//////////////////////////////////////////////////////////////////////////
	}

	if( pEffect->idCastSound )
		OnSkillPlaySound( pEffect->idCastSound );

	return xOK;
}

// 스킬발동대상얻기
// this는 시전자
int XSkillUser::GetInvokeTarget( 
						XVector<XSkillReceiver*> *_plistOutInvokeTarget,		// 결과를 이곳에 받습니다
						XSkillDat *pBuff,
						int level,
						xtInvokeTarget invokeTarget,		// virtual이라서 사용자쪽에서 어떤변수를 써야하는지 분명히 알게 하기위해 직접 변수를 넘김
						const EFFECT *pEffect,				// 효과.		
						XSkillReceiver *pCastingTarget,		// 시전대상
						const XE::VEC2& vPos )				// 기준타겟(좌표형)
{
	// virtual. invokeTarget을 하위클래스에서 가공하기 위한 땜빵.
	invokeTarget = pCastingTarget->OnGetInvokeTarget( pBuff, pEffect, invokeTarget );
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
		if( IsInvokeAble( pBuff, pCastingTarget, pEffect ) )
			aryTempInvokes.push_back( pCastingTarget );
		break;
	case xIVT_CAST_TARGET_RADIUS:
	case xIVT_CAST_TARGET_SURROUND:
	case xIVT_ATTACKED_TARGET_RADIUS:
	case xIVT_ATTACKED_TARGET_SURROUND: {
	// 여기에 case를 추가시킨다면 아래 if에도 추가할것.
		XSkillUser *pCaster = this;
		XSkillReceiver *pInvoker = pCastingTarget;
		if( pInvoker == NULL || pInvoker->GetCamp() == 0/*xSIDE_NONE*/ )
			pInvoker = pCaster->GetThisRecv();
		XBREAK( pInvoker == NULL );
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
		size = pCaster->OnInvokeTargetSize( pBuff, pEffect, level, pCastingTarget, size );
		GetListObjsRadius( &aryTempInvokes,
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
																	pBuff,
																	pEffect );
		break;
	case xIVT_CURR_TARGET: {
		// 현재 공격대상을 얻는다.
		XSkillReceiver *pCurrTarget = GetCurrTarget();  // virtual
		if( pCurrTarget && IsInvokeAble( pBuff, pCurrTarget, pEffect ) )
			aryTempInvokes.push_back( pCurrTarget );
	} break;
	// 타격자
	case xIVT_ATTACKER: {
		auto pAttacker = pCastingTarget->GetpAttacker();
		if( pAttacker && IsInvokeAble( pBuff, pAttacker, pEffect ) )
			aryTempInvokes.push_back( const_cast<XSkillReceiver*>( pAttacker ) );
	} break;
 	case xIVT_RANDOM_PARTY: {
		xtGroup typeGroup;
		if( filtIvkFriendship == xfALLY )
			typeGroup = xGT_RANDOM_PARTY_FRIENDLY;
		else
			typeGroup = xGT_RANDOM_PARTY_ENEMY;
		auto pRecv = GetThisRecv();
 		pRecv->GetGroupList( &aryTempInvokes,
 												pBuff,
 												pEffect,
												typeGroup );
 	} break;
	// 모두
	case xIVT_ALL: {
		xtGroup typeGroup;
		if( filtIvkFriendship == xfALLY )
			typeGroup = xGT_FRIENDLY_ALL;
		else
			typeGroup = xGT_ENEMY_ALL;
		auto pRecv = GetThisRecv();
		pRecv->GetGroupList( &aryTempInvokes,
												pBuff,
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
 @brief ary에 담긴 Invoke대상들에게 pEffect효과를 적용시킨다.
 @param pInvoker pEffect효과를 발동시킨측. pCaster와는 다르다.
*/
void XSkillUser::ApplyInvokeEffectWithAry( 
																			const XVector<XSkillReceiver*>& aryInvokTarget
																			, XSkillDat *pDat
																			, const EFFECT *pEffect
																			, XSkillReceiver *pInvoker
																			, bool bCreateSfx
																			, int level
																			, const XE::VEC2& vPos
																			, XBuffObj *pBuffObj
																			, XLuaSkill *pLua )
{
//	XARRAYLINEARN_LOOP_AUTO( aryInvokers, pInvokeTarget ) {
	for( auto pInvokeTarget : aryInvokTarget ) {
		// 발동확률이 있다면 확률검사를 통과해야 한다.(XBuffObj에 이것이 있을때는 GetInvokeTarget을 하기전에 수행되었는데 지금은 발동타겟 개별적으로 확률검사를 하도록 바뀜. 이게 맞는거 같아서)
		bool bOk = XSKILL::DoDiceInvokeApplyRatio( pEffect, level );
		if( bOk ) {
			bool bOrCond = false;
			// 조건블럭이 없으면 무조건 true
			if( pEffect->aryInvokeCondition.size() == 0 )
				bOrCond = true;
			else
			// 발동조건 블럭검사.
			if( IsInvokeTargetCondition( pDat, pEffect, pInvokeTarget ) )
				bOrCond = true;
			if( bOrCond ) {
				// 발동타겟에게 실제 효과적용
				pInvokeTarget->ApplyInvokeEffect( pDat, this
																				, pInvoker, pBuffObj
																				, pEffect, level );
				// 발동대상이펙트가 있다면 생성해준다.
				if( bCreateSfx ) {
					const float secPlay = 0.f;		// 1play. 발동이펙트는 반복플레이가 없음.
					pInvokeTarget->CreateSfx( pDat, /*pEffect,*/
																		pEffect->m_invokeTargetEff.m_strSpr,
																		pEffect->m_invokeTargetEff.m_idAct,
																		pEffect->m_invokeTargetEff.m_Point,
																		secPlay, vPos );
				}
				if( pEffect->idInvokeSound )
					OnSkillPlaySound( pEffect->idInvokeSound );
				// "발동시작"스크립 실행
	// 			if( pLua )
	// 				pLua->InvokeDoScript( pInvokeTarget, pEffect->scriptInit.c_str() );
				// 이벤트 핸들러
				//m_pDelegate->OnDotApply( pEffect );
			}
		}
	}
}
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

// xtError XSkillUser::ApplyEffect( XSkillDat *pSkillDat, 
// 								int level, 
// 								XSkillReceiver *pTarget,
// 								ID idCallerSkill,
// 								const XE::VEC2& vPos )
// {
// 	// 효과 사용
// 	LIST_LOOP( pSkillDat->GetlistEffects(), EFFECT*, itor, pEffect )	{
// 		xtError err;
// 		if( ( err = UseEffect( pSkillDat, pEffect, level, pTarget, vPos, idCallerSkill ) ) != xOK )
// 			return err;
// 	}
// 	END_LOOP;
// 	return XSKILL::xERR_OK;
// }

int XSkillUser::FrameMove( float dt )
{
	InvokePassiveSkill();
	return 1;
}

// 패시브형 스킬을 실행(발동)시킨다.
void XSkillUser::InvokePassiveSkill( void )
{
	XLIST_LOOP( m_listUseSkill, XSkillObj*, pUseSkill )
	{
		// 액티브형을 제외한 패시브나 기타 어떤 조건에 의해 발동되는 스킬들은 여기서 발동시킨다.
		if( pUseSkill->IsPassive() || pUseSkill->IsAbility() )
		{
			int level = GetSkillLevel( pUseSkill );
			xUseSkill info = UseSkill( pUseSkill, level, NULL, NULL ); 
			if( info.errCode == xOK )
				OnShootSkill( info, 0 );
		}
	}
	END_LOOP;
}


// 시전대상우호가 시전대상에 비교하던것을 시전자와 비교하는것으로 바뀌었다.
BIT XSkillUser::GetFilterSideCast( XSkillReceiver *pCaster,	// XSkillUser로 바꿔야함.
									xtCastTarget targetType, 
									XSkillReceiver *pBaseTarget,
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pCaster == NULL );
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
	XBREAK( pCaster == NULL );
	XBREAK( pCaster == NULL );
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

XSkillObj* XSkillUser::FindUseSkillByID( ID idSkillDat ) 
{
	XLIST_LOOP( m_listUseSkill, XSkillObj*, pUseSkill )
	{
		if( pUseSkill->GetidSkill() == idSkillDat )
			return pUseSkill;
	}
	END_LOOP;
	return NULL;
}
/*
XSkillObj* XSkillUser::FindUseSkillByName( LPCTSTR szName ) 
{
XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );
	XSkillDat *pSkillDat;
	pSkillDat = m_prefSkillMng->FindByName( szName );
	if( XBREAKF( pSkillDat == NULL, "%s: skill not found!", szName ) )
		return NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( pSkillDat->GetidSkill() );
	return pUseSkill;
}
*/
XSkillObj* XSkillUser::FindUseSkillByIdentifier( LPCTSTR szIdentifier ) 
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );	
	XSkillDat *pSkillDat;
	pSkillDat = m_prefSkillMng->FindByIdentifier( szIdentifier );
	if( XBREAKF( pSkillDat == NULL, "%s: skill not found!", szIdentifier ) )
		return NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( pSkillDat->GetidSkill() );
	return pUseSkill;
}

XSkillUser::xUseSkill XSkillUser::UseSkillByID( XSkillObj **ppOutUseSkill, 
										int level, 
										ID idSkill, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos ) 
{ 
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( idSkill );
	XASSERTF( pUseSkill, "idSkill:%d 스킬을 보유스킬목록에서 찾지못했습니다.", idSkill );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, level, pTarget, NULL );
}
/*
xtError XSkillUser::UseSkillByName( XSkillObj **ppOutUseSkill, 
										LPCTSTR szSkillName, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos ) 
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	XSkillObj *pUseSkill = FindUseSkillByName( szSkillName );
	XASSERTF( pUseSkill, "Skill:%s 스킬을 보유스킬목록에서 찾지못했습니다.", szSkillName );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, pTarget, NULL );
}
*/
// 식별자로 스킬을 사용케한다. 만약 스킬이 없다면 스킬사용 오브젝트를 새로만들어
// 등록시키고 사용한다. 스킬발동에 사용한다.
XSkillUser::xUseSkill XSkillUser::UseSkillByIdentifier( LPCTSTR szIdentifier, 
										int level, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos,
										XSkillObj **ppOutUseSkill ) 
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	
	XSkillObj *pUseSkill = CreateAddUseSkillByIdentifier( szIdentifier );
	if( XBREAK( pUseSkill == NULL ) )
	{
		xUseSkill info;
		info.errCode = xERR_CRITICAL_ERROR;
		return info;
	}
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	// 액티브로 실행시킴. 스킬발동에 사용
	XBREAK( pUseSkill->IsActive() == FALSE );
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

XSkillObj* XSkillUser::CreateSkillUseObj( XSkillDat *pSkillDat ) 
{ 
	return new XSkillObj( pSkillDat ); 
}	

// idSkillDat스킬로 사용스킬객체를 만들어서 보유한다
XSkillObj* XSkillUser::CreateAddUseSkillByID( ID idSkillDat )		
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );
	XSkillDat *pSkillDat;
	XSkillObj *pUseSkill = NULL;
	if(( pSkillDat = m_prefSkillMng->FindByID( idSkillDat ) ))
	{
		pUseSkill = CreateSkillUseObj( pSkillDat ) ;	// 사용스킬생성은 버추얼로 맡긴다
		if( XASSERT( pUseSkill ) ) 
		{
//			pSkillDat->AddRefCnt();		// 레퍼런스 카운트 올림
			AddUseSkill( pUseSkill );		// 보유스킬목록에 추가
		}
	} else
		XLOG_ALERT( "ID:%d 스킬을 찾지못했습니다. 스킬목록을 확인해주십시요", idSkillDat );
	return pUseSkill;
}

// this가 시전자
// 식별자로 스킬사용객체를 만들어 보유목록에 추가한다.
XSkillObj* XSkillUser::CreateAddUseSkillByIdentifier( LPCTSTR szIdentifier )
{
	XBREAKF( m_prefSkillMng == nullptr, "XSkillUser::OnCreate를 누락했음." );
	XSkillDat *pSkillDat;
	XSkillObj *pUseSkill = NULL;
	if(( pSkillDat = m_prefSkillMng->FindByIdentifier( szIdentifier ) )) {
		// 이미 내가 보유하고 있는 스킬이면 걍 그걸 리턴한다
		pUseSkill = FindUseSkillByIdentifier( szIdentifier );
		if( pUseSkill == nullptr ) {
			// 사용스킬생성은 버추얼로 맡긴다
			pUseSkill = CreateSkillUseObj( pSkillDat ) ;	
			if( XASSERT( pUseSkill ) ) {
				AddUseSkill( pUseSkill );		// 보유스킬목록에 추가
			}
		}
	} else
		XBREAKF( 1, "%s 스킬을 찾지못했습니다. 스킬목록을 확인해주십시요", szIdentifier );
	return pUseSkill;
}

/**
 @brief 버프 인스턴스를 만든다.
*/
XBuffObj* XSkillUser::CreateSkillBuffObj( XSkillUser *pCaster,
										XSkillReceiver *pCastingTarget,
										XSkillDat *pSkillDat,
										int level,
										const XE::VEC2& vPos,
										ID idCallerSkill ) 
{
	return new XBuffObj( GetpDelegate(),
						pCaster,
						pCastingTarget,
						pSkillDat,
						level,
						vPos,
						idCallerSkill );
}

void XSkillUser::AddUseSkill( XSkillObj *pUseSkill ) 
{
	XSkillObj *pExistObj = FindUseSkillByIdentifier( pUseSkill->GetpDat()->GetstrIdentifier().c_str() );
	XBREAKF( pExistObj != NULL, "warning: Already tried to add the skill. %s", pUseSkill->GetpDat()->GetstrIdentifier().c_str() );
	m_listUseSkill.push_back( pUseSkill );
}

XE_NAMESPACE_END
