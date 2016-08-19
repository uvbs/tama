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
 @brief 이펙트를 만드는 공통 함수.
 @param secPlay 0:once 0>:해당시간동안 루핑 -1:무한루핑
 @param vPos 값이 있다면 좌표를 기준으로 생성하고 없다면 this를 기준으로 생성한다.
*/
ID XSkillUser::CreateSfx( XSkillReceiver* pTarget,
													const XSkillDat *pSkillDat,
													const _tstring& strEffect,
													ID idAct,
													xtPoint pointSfx,
													float secPlay,
													const XE::VEC2& vPos ) 
{
	if( strEffect.empty() )
		return 0;
	const float secLife = 0.f;
	// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
	if( pointSfx == xPT_NONE )
		pointSfx = xPT_TARGET_BOTTOM;
	if( idAct == 0 )
		idAct = 1;
	return OnCreateSkillSfx( pTarget,
													 pSkillDat,
													 pointSfx,
													 strEffect.c_str(),
													 idAct,
													 secPlay,
													 vPos );
}

/**
 @brief sfx를 생성한다.
 @param vPos 값이 있다면 좌표를 기준으로 생성하고 없다면 this를 기준으로 생성한다.
*/
ID XSkillUser::CreateSfx( XSkillReceiver* pTarget, const XSkillDat *pSkillDat, const xEffSfx& effSfx, float secPlay, const XE::VEC2& vPos )
{
	return CreateSfx( pTarget, pSkillDat, effSfx.m_strSpr, effSfx.m_idAct, effSfx.m_Point, secPlay, vPos );
}


/**
 @brief 스킬 최초 사용
 pUseSkill을 사용하기 위한 시작함수. 이 함수가 성공하면 스킬모션이 시작된다.
 캐스팅 시작이라고 봐도 됨.
 @param pCurrTarget 현재 this가 잡고 있는 공격타겟. 혹은 직접 터치로 찍은 타겟. 스킬의 타입에 따라서 이 타겟은 안쓰여 질수도 있다.
 @param vPos 기준타겟이 좌표형태라면 IsZero가 아닌값이 들어온다.
*/
XSkillUser::xUseSkill XSkillUser::UseSkill( XSkillDat* pDat,
																						int level,
																						XSkillReceiver *pCurrTarget,
																						const XE::VEC2& vPos )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vPos.IsNotZero() && pCurrTarget != nullptr );
	xUseSkill infoUseSkill;
	if( XBREAK( pDat == nullptr ) ) {
		infoUseSkill.errCode = xERR_CRITICAL_ERROR;
		return infoUseSkill;
	}
//	auto pDat = pUseSkill->GetpDatMutable();
	// 시전자 이펙트(시전시작 시점에 발생되는 이펙트)
	if( !pDat->GetCasterEff().m_strSpr.empty() ) {
		const float secPlay = 0.f;	// play once
		const XE::VEC2 vZero;
		CreateSfx( GetThisRecv(), pDat, pDat->GetCasterEff(), secPlay, vZero );
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
		if( vPos.IsZero() )
			// 좌표가 없다면 하위클래스에 일단 의뢰해본다.
			infoUseSkill.vTarget = GetSkillBaseTargetPos( pDat );
		else
			infoUseSkill.vTarget = vPos;
		// 기준타겟을 좌표로 하겠다고 했으면 좌표가 있어야함.
		if( XBREAK(infoUseSkill.vTarget.IsZero()) ) {
			infoUseSkill.errCode = xERR_MUST_SELECT_TARGET;
			return infoUseSkill;
		}
	} else
	// 기준타겟조건
	if( baseTarget == xBST_CONDITION ) {
#pragma message("=========================================기준타겟조건 쓰는것들 재점검")
		infoUseSkill.pBaseTarget 
			= GetBaseTargetByCondition( pDat,
																	pDat->GetcondBaseTarget(),
																	pDat->GetrangeBaseTargetCond(),
																	level,
																	pCurrTarget,
																	vPos );
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
 @brief 첫번째 보유스킬을 사용한다
 그냥 편의 함수.
*/
XSkillUser::xUseSkill XSkillUser::UseSkill( XSkillReceiver *pTarget, const XE::VEC2& vPos )
{
	xUseSkill info;
	if( XBREAK( m_listUseSkill.empty() ) ) {
		info.errCode = xCANCEL;
		return info;
	}
	XSkillDat *pUseSkill = m_listUseSkill.GetFirst();
	int level = GetSkillLevel( pUseSkill );
	return UseSkill( pUseSkill, level, pTarget, vPos );
}

/**
 @brief 스킬을 사용(Shoot)한다. 
 발사체일경우는 발사체객체를 생성하고 즉시시전형이면 기준타겟에 바로 스킬을 시전한다.
*/
xtError XSkillUser::OnShootSkill( XSkillDat *pDat
																, XSkillReceiver *pBaseTarget
																, int level
																, const XE::VEC2& vBaseTarget
																, ID idCallerSkill )
{
	if( XBREAK(pDat == nullptr 
		||( pBaseTarget == nullptr && vBaseTarget.IsZero() )
		|| (vBaseTarget.IsNotZero() && pBaseTarget != nullptr)) )
		return xERR_GENERAL;
//	auto pDat = pUseSkill->GetpDatMutable();
	// 시전자의 스킬동작 타점에 시전자에게 생성되는 이펙트(루핑없음)
	if( !pDat->GetShootEff().m_strSpr.empty() ) {
		float secPlay = 0;	// once
		const XE::VEC2 vZero;
		CreateSfx( GetThisRecv(), pDat, pDat->GetShootEff(), secPlay, vZero );
	}
	// 슈팅타겟이펙트(슈팅시점에 타겟에게 발생한다. 보통 타점을 포함하고 있다)-메테오
	if( !pDat->GetShootTargetEff().m_strSpr.empty() ) {
		float secPlay = 0;	// once
		XSkillSfx *pSfx 
			= pBaseTarget->OnCreateSkillSfxShootTarget( pDat,
																									pBaseTarget,
																									level,
																									pDat->GetShootTargetEff(),
																									secPlay, vBaseTarget );
		if( pSfx )
			pSfx->RegisterCallback( this, pDat, level, pBaseTarget, vBaseTarget );
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
																 vBaseTarget );		// 여기서 기준타겟의 좌표를 실제로 사용해야함.(따라서 좌표는 이시점에 미리 얻어야 함.
	} else {
		// 지금 바로 기준타겟에 스킬을 시전한다.
		CastSkillToBaseTarget( pDat, level, pBaseTarget, vBaseTarget, idCallerSkill );
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
																			, const XE::VEC2& vBaseTarget
																			, ID idCallerSkill)
{
	// (기준)타겟이펙트
	if( pDat->GetTargetEff().IsHave() ) {
//		const float secPlay = pDat->GetDuration( );	// once
		const float secPlay = 0;
		CreateSfx( pBaseTarget, pDat, pDat->GetTargetEff(), secPlay, vBaseTarget );
	}
	// 스킬이 가진 효과들 기준타겟에게 사용한다.
	for( auto pEffect : pDat->GetlistEffects() )	{
		xtError err = UseEffect( pDat, pEffect, level, pBaseTarget, vBaseTarget, idCallerSkill );
		if( err != xOK )
			return;
	}
}

/**
 @brief 효과 시전
 pEffect의 효과를 기준타겟(pBaseTarget)을 기준으로 시전대상을 선정하여 시전한다.
 현재 _vPos는 사용하지 않는다
*/
xtError XSkillUser::UseEffect( const XSkillDat *pDat,
															 EFFECT *pEffect,
															 int level,
															 XSkillReceiver *pBaseTarget,
															 const XE::VEC2& vBaseTarget,		// 기준타겟이 좌표형일경우.
															 ID idCallerSkill )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vBaseTarget.IsNotZero() && pBaseTarget != nullptr );
	XVector<XSkillReceiver*> aryCastTargets;		// 시전대상
	// 기준타겟이나 기준좌표로 시전대상을 얻는다.
	XBREAK( pBaseTarget == nullptr && vBaseTarget.IsZero() );
	//
	XE::VEC2 vCastTarget;
	GetCastingTargetList( &aryCastTargets,
												&vCastTarget,
												pEffect->castTarget,
												pDat,
												pEffect,
												level,
												pBaseTarget,
												vBaseTarget );
	//
	if( vCastTarget.IsZero() && aryCastTargets.empty() ) {
		return xERR_NOT_FOUND_CASTING_TARGET;		// 시전대상을 찾지 못함
	}
	if( aryCastTargets.size() ) {
		// 캐스팅 대상들에게 효과를 시전한다.
		for( auto pCastingTarget : aryCastTargets ) {
			// 즉시시전 or 버프방식
			const XE::VEC2 vZero;
			CastEffToCastTarget( pDat,
													 pEffect,
													 level,
//												 pBaseTarget,
													 pCastingTarget,
													 vZero,
													 idCallerSkill );
		}
	} else 
	if( vCastTarget.IsNotZero() ) {
		// 시전대상이 좌표형
		CastEffToCastTarget( pDat,
												 pEffect,
												 level,
												 nullptr,
												 vCastTarget,
												 idCallerSkill );
	} else {
		XBREAK(1);
	}
	return xOK;
}

// 
/**
 @brief pBaseTarget(혹은 vBaseTarget)을 기준으로 얻어지는 모든 시전대상을 얻는다.
*/
int XSkillUser::GetCastingTargetList( XVector<XSkillReceiver*> *pOutAry,			// 시전대상얻기 결과가 담겨짐
																			XE::VEC2* pOutCastTarget,		// 시전대상이 좌표형태가 될때.
																			xtCastTarget castTarget,	// 시전대상타입
																			const XSkillDat *pDat,				// 스킬사용 오브젝트
																			const EFFECT *pEff,					// 효과
																			int level,
																			XSkillReceiver *pBaseTarget,
																			const XE::VEC2& vBaseTarget )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vBaseTarget.IsNotZero() && pBaseTarget != nullptr );
	// 둘다 값이 없을순 없다.
	XBREAK( vBaseTarget.IsZero() && pBaseTarget == nullptr );
	//
	switch( castTarget ) {
	case xCST_BASE_TARGET:		// 기준타겟
		if( pBaseTarget ) {	// 시전대상이 기준타겟인데 기준타겟이 없는경우는 없어야 한다. 이런경우의 시전대상은 리시버객체이므로 이전에 이미 처리되어 들어온다.
			pOutAry->Add( pBaseTarget );
		} else 
		if( vBaseTarget.IsNotZero() ) {
			// 좌표가 있고 시전대상이 기준타겟(좌표)이고 지속효과라면 바닥에 뿌리는 스킬이므로 리시버 객체를 만들어 그것을 시전대상으로 해야함.
			const float sec = pEff->GetDuration( level );
			XBREAK( sec <= 0 );
			auto pSfx = CreateSfxReceiver( vBaseTarget, pEff, sec );
			pOutAry->Add( pSfx );
		}
		break;
	case xCST_BASE_TARGET_RADIUS:		// 기준타겟반경
	case xCST_BASE_TARGET_SURROUND:	{	// 기준타겟주변
		// 기준타겟을 중심으로 원형 반경내에서의 객체를 선택한다.
		BIT bitSearchFilter = GetFilterSideCast( GetThisRecv(),
																						 castTarget,
																						 pBaseTarget,
																						 pEff->castfiltFriendship );
		int numApply = pEff->invokeNumApply;	// 0이면 제한없음.
		// 기준타겟포함이면 기준타겟은 우선으로 리스트에 들어가고 기준타겟제외로 검색한다.
		if( castTarget == xCST_BASE_TARGET_RADIUS ) {
			if( pBaseTarget ) {
				pOutAry->Add( pBaseTarget );
				if( numApply > 1 )
					--numApply;
			}
		}
		float castSize = pEff->castSize.w;
		float sizeAdd = 0.f;
		float sizeAddMultiply = 0.f;
		// 시전범위 증폭
		OnSkillAmplifyUser( pDat, nullptr, pEff, xEA_CAST_RADIUS, &sizeAddMultiply, &sizeAdd );
		castSize += sizeAdd;
		castSize = castSize + castSize * sizeAddMultiply;
		numApply += (int)(pEff->invokeNumApply * sizeAddMultiply);
		// 기준타겟(혹은 기준좌표)를 기준으로 대상을 얻는다.
		GetListObjsRadius( pOutAry,
											 pDat,
											 pEff,
											 pBaseTarget,		// 기준타겟
											 vBaseTarget,		// 기준타겟이 좌표형태일경우
											 castSize,
											 bitSearchFilter,
											 numApply,
											 FALSE );
	} break;
	case xCST_BASE_TARGET_PARTY:	// 기준타겟파티
		if( XASSERT(pBaseTarget) ) {	// 기준타겟파티를 하면 기준타겟은 반드시 있어야 한다.
			// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
			pBaseTarget->GetGroupList( pOutAry,
																 pDat,
																 pEff );
		}
		break;
	case xCST_BASE_TARGET_POS: {  // 기준타겟좌표
		auto vPos = vBaseTarget;
		if( pBaseTarget ) {		// 기준타겟이 있었다면 기준타겟의 좌표/없으면 파라메터로 온 좌표에
			vPos = pBaseTarget->GetvPosFromSkill();
		}
		if( XASSERT(vPos.IsNotZero()) ) {
			// 좌표가 있고 시전대상이 기준타겟(좌표)이고 지속효과라면 바닥에 뿌리는 스킬이므로 리시버 객체를 만들어 그것을 시전대상으로 해야함.
			const float sec = pEff->GetDuration( level );
			if( sec > 0 ) {
				// 시전대상(좌표)에 지속시간형이면 리시버 객체가 필요함.
				auto pSfx = CreateSfxReceiver( vPos, pEff, sec );
				pOutAry->Add( pSfx );
			} else {
				*pOutCastTarget = vPos;		// 지속형이 아니면 시전대상은 좌표형이 된다.(파이어볼 폭발같은..)
			}
		}

	} break;
	default:
		XBREAKF( 1, "%d: 처리안된 castTarget", castTarget );
	}
	return pOutAry->size();
}

/**
 @brief 시전대상에게 효과시전
 이 함수는 시전대상의 수만큼 불린다.
 @param pBaseTarget 기준타겟
 @param pCastingTarget 시전대상들
*/
xtError XSkillUser::CastEffToCastTarget( const XSkillDat *pDat,
																				 EFFECT *pEffect,
																				 int level,
//																				 XSkillReceiver *pBaseTarget,
																				 XSkillReceiver *pCastingTarget,
																				 const XE::VEC2& vCastTarget,
																				 ID idCallerSkill )
{
	XBREAK( pDat == nullptr );
	XBREAK( pEffect == nullptr );
	// 시전대상들에게 모두 이펙트를 붙인다.
	if( pEffect->m_CastTargetEff.IsHave() ) {
		float secPlay = pEffect->GetDuration( level );
		XBREAK( pCastingTarget == nullptr );	// 시전대상이 널인경우도 있을까?
		CreateSfx( pCastingTarget, pDat, pEffect->m_CastTargetEff, secPlay, vCastTarget );
	}
	if( pEffect->IsDuration() ) {
		if( pCastingTarget ) {
			// 지속시간이 있는 버프형 효과를 타겟에게 시전한다.
			CastEffToCastTargetByBuff( pDat, pEffect, level, pCastingTarget, idCallerSkill );
		}
	} else {	// 즉시발동형(지속시간 0)
		// 시전대상에게 즉시 효과가 발동된다.
		CastEffToCastTargetByDirect( pDat, pEffect, level, pCastingTarget, vCastTarget );
	}
	if( pEffect->idCastSound ) {
		OnSkillPlaySound( pEffect->idCastSound );
	}
	return xOK;
}

/**
 @brief 버프타입의 효과를 시전대상에게 시전한다.
*/
xtError XSkillUser::CastEffToCastTargetByBuff( const XSkillDat *pDat,
																							 EFFECT *pEffect,
																							 int level,
																							 XSkillReceiver *pCastingTarget,
//																							 const XE::VEC2& vPos,
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
//																	 vPos,
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
			const XE::VEC2 vZero;
			ID idSfx = CreateSfx( pCastingTarget, pDat, pEffect->m_PersistEff, secPlay, vZero );
			if( idSfx ) {
#ifdef _CLIENT
				pBuffObj->SetidSfx( idSfx );
#endif // _CLIENT
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
 @brief 즉시시전형태(비버프형)의 효과를 시전대상에게 시전한다.
*/
xtError XSkillUser::CastEffToCastTargetByDirect( const XSkillDat *pDat,
																								 const EFFECT *pEffect,
																								 int level,
																								 XSkillReceiver *pCastingTarget,
																								 const XE::VEC2& vCastTarget )
{
	XVector<XSkillReceiver*> aryInvokeTarget;
	// 1회성이기때문에 루아도 쓰고 바로 파괴되는걸로 함
	// 시전대상자(pCastingTarget)기준 발동대상 얻음
	XE::VEC2 vIvkTarget;			// 발동대상이 좌표형태일때 여기로 좌표가 담긴다.
	int numInvoke = GetInvokeTarget( &aryInvokeTarget,
																	 &vIvkTarget,
																	 pDat,
																	 level,
																	 pEffect->invokeTarget,
																	 pEffect,
																	 pCastingTarget,
																	 vCastTarget );
	if( numInvoke > 0 || vIvkTarget.IsNotZero() ) {
		bool bCreateSfx = ( pEffect->m_invokeTargetEff.m_strSpr.empty() ) ? false : true;
		if( aryInvokeTarget.size() ) {
			for( auto pInvokeTarget : aryInvokeTarget ) {
				// 발동대상들에게 효과 적용
				ApplyInvokeEffToIvkTarget( pInvokeTarget,
																	 vIvkTarget,
																	 pDat,
																	 pEffect,
																	 GetThisRecv(),		// invoker
																	 bCreateSfx,
																	 level,
																	 nullptr );		// pBuff
			} // for
		} else
		if( vIvkTarget.IsNotZero() ) {
			// 발동좌표에 효과적용
			ApplyInvokeEffToIvkTarget( nullptr,
																 vIvkTarget,
																 pDat,
																 pEffect,
																 GetThisRecv(),		// invoker
																 bCreateSfx,
																 level,
																 nullptr );		// pBuff
		}
	}
	return xOK;
} // CastEffToCastTargetByDirect

/**
 @brief 시전대상(혹은 시전좌표)를 기준으로 발동대상(혹은 발동좌표)을 얻는다.
 this는 시전자
*/
int XSkillUser::GetInvokeTarget( XVector<XSkillReceiver*> *_plistOutInvokeTarget,		// 결과를 이곳에 받습니다
																 XE::VEC2* pOutIvkTarget,		// 발동대상이 좌표형일경우 좌표가 담긴다.
																 const XSkillDat *pDat,
																 int level,
																 xtInvokeTarget typeIvkTarget,	// 발동대상
																 const EFFECT *pEffect,				// 효과.		
																 XSkillReceiver *pCastTarget,		// 시전대상
																 const XE::VEC2& vCastTarget )			// 시전대상이 좌표형일때
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vCastTarget.IsNotZero() && pCastTarget != nullptr );
	XBREAK( pCastTarget == nullptr && vCastTarget.IsZero() );
	// virtual. typeIvkTarget을 하위클래스에서 가공하기 위한 땜빵.
	typeIvkTarget = pCastTarget->OnGetInvokeTarget( pDat, pEffect, typeIvkTarget );
	//
	auto& aryTempInvokes = *_plistOutInvokeTarget;
	xtFriendshipFilt filtIvkFriendship = pEffect->invokefiltFriendship;
	if( filtIvkFriendship == xfNONESHIP )
		filtIvkFriendship = pEffect->castfiltFriendship;
	XBREAK( filtIvkFriendship == xfNONESHIP );
	//
	switch( typeIvkTarget ) {
	// 시전대상
	case xIVT_CAST_TARGET:
#pragma message("==========================피격자라는 발동대상이 필요하나?")
	case xIVT_ATTACKED_TARGET:	// 피격자. 이때는 pCastingTarget이 피격자가 된다.
#pragma message("==========================IsInvokeAble 이거 꼭 필요함?")
		if( pCastTarget ) {
			if( IsInvokeAble( pDat, pCastTarget, pEffect ) )
				aryTempInvokes.push_back( pCastTarget );
		} else {
			*pOutIvkTarget = vCastTarget;
		}
		break;
	case xIVT_CAST_TARGET_RADIUS:
	case xIVT_CAST_TARGET_SURROUND:
#pragma message("==========================피격자라는 발동대상이 필요하나?")
	case xIVT_ATTACKED_TARGET_RADIUS:
	case xIVT_ATTACKED_TARGET_SURROUND: {
	// 여기에 case를 추가시킨다면 아래 if에도 추가할것.
		XSkillUser *pCaster = this;
// 		XSkillReceiver *pInvoker = pCastTarget;
// 		if( pInvoker == nullptr || pInvoker->GetCamp() == 0/*xSIDE_NONE*/ )
// 			pInvoker = pCaster->GetThisRecv();
// 		XBREAK( pInvoker == nullptr );
		BIT bitSideSearchFilt = GetFilterSideInvoke( pCaster,
																								 pCastTarget,
																								 pEffect->invokeTarget,
																								 filtIvkFriendship );

		int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
		if( typeIvkTarget == xIVT_CAST_TARGET_RADIUS 
				|| typeIvkTarget == xIVT_ATTACKED_TARGET_RADIUS ) {
			if( pCastTarget ) {
				aryTempInvokes.Add( pCastTarget );
				if( numApply > 1 )
					--numApply;			// 시전대상은 포함시켰으므로 적용수가 있었다면 하나 빼준다. 적용수가 1이면 그냥 추가한마리로 처리함.
			}
		}
		// 발동자를 중심으로(발동자를 포함하거나/제외하거나) 반경내 sideSearchFilt에 해당하는
		// side편 대로 검색하여 invokeNumApply개의 리스트를 요청한다
		float size = pEffect->GetInvokeSize( level );
		if( size == 0 ) {
			size = pEffect->castSize.w;
		}
		size = pCaster->OnInvokeTargetSize( pDat, pEffect, level, pCastTarget, size );
		GetListObjsRadius( &aryTempInvokes,
											 pDat,
											 pEffect,
											 pCastTarget,		// 원의 중심이 되는 타겟
											 vCastTarget,			// 원의 중심 좌표
											 size,
											 bitSideSearchFilt,
											 numApply,
											 FALSE );
	} break;
	case xIVT_CAST_TARGET_PARTY:		// 시전대상파티
	case xIVT_ATTACKED_TARGET_PARTY: // 피격자파티
		if( XASSERT(pCastTarget) ) {
			// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
			pCastTarget->GetGroupList( &aryTempInvokes,
																 pDat,
																 pEffect );
		}
		break;
	case xIVT_CURR_TARGET: {
		// 현재 공격대상을 얻는다.(시전대상과 관계없이 현재 타겟을 얻는다)
		auto pCurrTarget = GetCurrTarget();  // virtual
		if( pCurrTarget && IsInvokeAble( pDat, pCurrTarget, pEffect ) )
			aryTempInvokes.push_back( pCurrTarget );
	} break;
	// 현재타겟좌표
	case xIVT_CURR_TARGET_POS: {
		auto pCurrTarget = GetCurrTarget();  // virtual
		if( pCurrTarget ) {
			*pOutIvkTarget = pCurrTarget->GetvPosFromSkill();
		}
	} break;
	// 타격자
	case xIVT_ATTACKER: {
		auto pAttacker = pCastTarget->GetpAttacker();
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
		XBREAKF(1, "잘못된 발동파라메터:%d", typeIvkTarget );
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
 @param vIvkPos 발동대상이 좌표인경우 0이아닌값이 들어온다. 이때 pInvokeTarget은 null이된다.
 @return 효과적용에 성공하면 true를 리턴한다.
*/
bool XSkillUser::ApplyInvokeEffToIvkTarget( XSkillReceiver* pIvkTarget, // null일수도 있음.
																						const XE::VEC2& vIvkPos,	// 발동대상이 좌표형태인경우.
																						const XSkillDat *pDat,
																						const EFFECT *pEffect,
																						XSkillReceiver *pInvoker,
																						bool bCreateSfx,
																						int level,
																						XBuffObj *pBuffObj )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vIvkPos.IsNotZero() && pIvkTarget != nullptr );	
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
	if( pIvkTarget && IsInvokeTargetCondition( pDat, pEffect, pIvkTarget ) ) {
		bOrCond = true;
	}
	if( !bOrCond )
		return false;
	if( pEffect->idInvokeSound ) {
		OnSkillPlaySound( pEffect->idInvokeSound );
	}
	// 발동타겟에게 실제 효과적용
	if( pIvkTarget ) {
		int retApplied = pIvkTarget->ApplyInvokeEffect( const_cast<XSkillDat*>(pDat), this
																										, pInvoker, pBuffObj
																										, pEffect, level );
		// 발동대상이펙트가 있다면 생성해준다.
		if( bCreateSfx && pEffect->m_invokeTargetEff.IsHave() ) {
			const float secPlay = 0.f;		// 1play. 발동이펙트는 반복플레이가 없음.
			const XE::VEC2 vZero;
			CreateSfx( pIvkTarget, pDat, pEffect->m_invokeTargetEff, secPlay, vZero );
		}
		return retApplied != 0;
	} else
	if( vIvkPos.IsNotZero() ) {
		// 발동타겟이 좌표형태
		// 발동타겟이 좌표이면 바닥에 놓는 버프객체를 뜻한다.
		// 발동좌표를 기준타겟좌표로해서 시전한다. 기준타겟좌표로 시전된 스킬은 가상의 리시버객체가 생성되고 거기에 버프가 걸리는 형태.
		if( XASSERT(!pEffect->strInvokeSkill.empty()) ) {		// 발동대상이 좌표형태인경우 그 효과는 반드시 발동스킬로 구현되어야 한다.
			const ID idCallerBuff = ( pBuffObj ) ? pBuffObj->GetidSkill() : 0;
			pInvoker->DoInvokeSkill( nullptr, vIvkPos, pDat, pEffect, level, this, idCallerBuff );
		}

		// 발동대상이펙트가 있다면 생성해준다.
		if( bCreateSfx && pEffect->m_invokeTargetEff.IsHave() ) {
			const float secPlay = 0.f;		// 1play. 발동이펙트는 반복플레이가 없음.
			CreateSfx( pIvkTarget, pDat, pEffect->m_invokeTargetEff, secPlay, vIvkPos );
		}
		return true;
	}
	return false;
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
bool XSkillUser::IsInvokeTargetCondition( const XSkillDat *pDat
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
				const XE::VEC2 vZero;
				const xUseSkill info = UseSkill( pUseSkill, level, GetThisRecv(), vZero );
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
	if( targetType == xCST_BASE_TARGET ) {
		if( pBaseTarget ) {
			side = pBaseTarget->GetCamp();	// 시전대상이 기준타겟이면 기준타겟의 우호를 그대로 사용
		} else {
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetSide( pCaster->GetCamp() );	// 시전자와 같은편 필터를 만든다.
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() ); // 시전자와 반대되는 Side필터를 만든다.
		}
	} else 	{
		if( XECompCamp::sIsNeutrality( pCaster->GetCamp() ) )	{// 시전자가 중립일때
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetNeutralitySideFilter();
			// 중립의 적은 원래 없는데 표현이 애매해서 중립이 아닌편은 모두 적으로 간주
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() );
		} else {
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
																		 XSkillReceiver *pCastingTarget,
																		 xtInvokeTarget targetType,
																		 xtFriendshipFilt friendshipFilter )
{
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
BOOL XSkillUser::IsInvokeAble( const XSkillDat *pDat, const XSkillReceiver *pCstTarget, const EFFECT *pEffect )
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
																								XSkillReceiver *pBaseTarget,
																								const XE::VEC2& vBaseTarget )
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = nullptr;
	auto pUseSkill = FindUseSkillByID( idSkill );
	XASSERTF( pUseSkill, "idSkill:%d 스킬을 보유스킬목록에서 찾지못했습니다.", idSkill );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, level, pBaseTarget, vBaseTarget );
}

// 식별자로 스킬을 사용케한다. 만약 스킬이 없다면 스킬사용 오브젝트를 새로만들어
// 등록시키고 사용한다. 스킬발동에 사용한다.
XSkillUser::xUseSkill 
XSkillUser::UseSkillByIds( LPCTSTR idsSkill,
													 int level,
													 XSkillReceiver *pBaseTarget,
													 const XE::VEC2& vBaseTarget,
													 XSkillDat **ppOutUseSkill )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vBaseTarget.IsNotZero() && pBaseTarget != nullptr );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = nullptr;
	// 발동스킬은 m_liseUseSkill에 들어가면 안되므로 안쓰는 버전으로 바꿈.
	auto pUseSkill = XESkillMng::sGet()->FindByIds( idsSkill );
	if( XBREAK( pUseSkill == nullptr ) )	{
		xUseSkill info;
		info.errCode = xERR_CRITICAL_ERROR;
		return info;
	}
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, level, pBaseTarget, vBaseTarget );
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
																					const XSkillDat *pDat,
																					int level,
//																					const XE::VEC2& vPos,
																					ID idCallerSkill )
{
	return new XBuffObj( GetpDelegate(),
											 pCaster,
											 pCastingTarget,
											 pDat,
											 level,
//											 vPos,
											 idCallerSkill );
}

void XSkillUser::AddUseSkill( XSkillDat* pUseSkill ) 
{
	if( FindUseSkillByIds( pUseSkill->GetstrIdentifier().c_str() ) )
		return;
	m_listUseSkill.push_back( pUseSkill );
}




XE_NAMESPACE_END		// XSKILL
