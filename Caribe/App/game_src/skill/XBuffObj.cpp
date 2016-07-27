#include "stdafx.h"
#include "xSkill.h"
#include "XEffect.h"
#include "XESkillMng.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _XMEM_POOL
template<> XPool<XSKILL::XBuffObj>* XMemPool<XSKILL::XBuffObj>::s_pPool = nullptr;
#endif // _XMEM_POOL

XE_NAMESPACE_START( XSKILL )

XBuffObj::XBuffObj( XDelegateSkill *pDelegate ) 
{
	Init();
	XBREAK( pDelegate == nullptr );
	m_pDelegate = pDelegate;
}
XBuffObj::XBuffObj( XDelegateSkill *pDelegate,
					XSkillUser *pCaster,
					XSkillReceiver *pOwner,
					XSkillDat *pSkillDat,
					int level,
					const XE::VEC2& vPos,
					ID idCallerSkill )
	: m_idCallerSkill( idCallerSkill )
{
	Init();
	XBREAK( pDelegate == nullptr );
	m_pDelegate = pDelegate;
	m_pCaster = pCaster;
	//m_idCaster = pCaster->get
	m_bitCampCaster = pCaster->GetCampUser();
	m_pOwner = pOwner;
	m_pDat = pSkillDat;
	if( vPos.IsZero() == FALSE )
		m_vCastPos = vPos;
	m_Level = level;
	m_pDatCaller = XESkillMng::sGet()->FindByID( idCallerSkill );
}

void XBuffObj::Destroy() 
{
	SAFE_DELETE( m_pluaScript );		// 루아가 있었다면 삭제해줌
	LIST_DESTROY( m_listEffectObjs, EFFECT_OBJ* );
#ifdef _CLIENT
	SAFE_RELEASE2( IMAGE_MNG, m_psfcIcon );
#endif // _CLIENT
	//		CONSOLE( "XBuffObj::Destroy(): ID:%d Caster:0x%08x Skill:%s addr=0x%08x", m_idBuff, (int)m_pCaster, m_pDat->GetszName(), (int)this );
}

/**
 @brief // pEffect를 멤버로 가지고 있는 이펙오브젝트를 찾음
*/
EFFECT_OBJ* XBuffObj::FindEffect( EFFECT *pEffect ) 
{		
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )	{
		if( pEffect->m_snEffect == pEffObj->m_effect.m_snEffect )
			return pEffObj;
	}
	END_LOOP;
	return nullptr;
}

// 이펙트오브젝트를 추가한다
EFFECT_OBJ* XBuffObj::AddEffect( EFFECT *pEffect ) 
{	
	EFFECT_OBJ *pEffObj;
	if(( pEffObj = FindEffect( pEffect ) ))	// 이미 같은 효과가 있다면
		return pEffObj;
	// 없는효과면 새로 생성해서 리턴
	pEffObj = new EFFECT_OBJ( pEffect );
	m_listEffectObjs.push_back( pEffObj );		// 이펙오브젝트 리스트에 추가
	return pEffObj;
}


// 이 버프의 주인이 idDead를 죽였다.
void XBuffObj::OnSkillEventKillEnemy( XSkillReceiver *pOwner, ID idDead )
{
	m_pCaster->SetidDead( idDead );
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )	{
		EFFECT *pEffect = &pEffObj->m_effect;
		// 이 버프의 효과가 죽이면 발동하는 효과인가?
		if( pEffect->invokeJuncture == xJC_KILL_ENEMY )		{
			ApplyInvokeEffect( pEffect, m_Level, pOwner, nullptr, TRUE);
		}
	} END_LOOP;
}

/**
 @brief 이 버프의 주인이 pAttacker로부터 맞았다.
 이 버프가 가진 효과중 피격시 발동하는 효과가 있다면 그 효과를 발동타겟들에게 발동시킨다.
 @param pAttacker는 null일수 있다.
*/
void XBuffObj::OnHitFromAttacker( const XSkillReceiver *pAttacker, 
								XSkillReceiver *pDefender,
								XSkillReceiver *pOwner, 
								xtDamage typeDamage )
{
	XBREAK( pDefender == nullptr );
	XBREAK( pOwner == nullptr );
	// 공격자를 세팅함.
	pDefender->SetpAttacker( const_cast<XSkillReceiver*>( pAttacker ) );
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )	{
		EFFECT *pEffect = &pEffObj->m_effect;
		// 이 버프의 효과가 맞으면 발동하는 효과인가?
		// 근접타격이 들어왔을때 발동조건이 근접피격시 이거나 모든피격시라면 적용
		if( typeDamage == xDMG_MELEE && 
			(pEffect->invokeJuncture == xJC_CLOSE_HIT || pEffect->invokeJuncture == xJC_ALL_HIT) )		{
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		} else
		if( typeDamage == xDMG_RANGE && pEffect->invokeJuncture == xJC_RANGE_HIT )		{
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		} else
		// 데미지타입이 무속성일때 발동조건이 모든피격시면 적용
		if( typeDamage == 0 && 
			(pEffect->invokeJuncture == xJC_ALL_HIT) )		{
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		}

	} END_LOOP;
}

// 
/**
 @brief m_pOwner가 pDefender를 공격했다.
*/
void XBuffObj::OnAttackToDefender( XSkillReceiver *pDefender, 
									float damage, 
									BOOL bCritical, 
									float ratioPenetration, 
									XSKILL::xtDamage typeDamage )
{
	// 타격자를 세팅함.
	pDefender->SetpAttacker( m_pOwner );
//	m_pCaster->SetidEventHitter( idDefender );
	//
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )	{
		EFFECT *pEffect = &pEffObj->m_effect;
		// 이 버프의 효과가 공격시 발동하는 효과인가
		// 근접공격시
		if( typeDamage == xDMG_MELEE && 
			(pEffect->invokeJuncture == xJC_CLOSE_ATTACK ||
			 pEffect->invokeJuncture == xJC_ATTACK) )		{
			// 이때 pCastingTarget은 피격자가 된다.
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		} else
		if( typeDamage == xDMG_RANGE &&
			( pEffect->invokeJuncture == xJC_RANGE_ATTACK_ARRIVE ||
 			  pEffect->invokeJuncture == xJC_ATTACK ) )		{
			// 이때 pCastingTarget은 피격자가 된다.
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		} else
		// 공격타입이 무속성일땐 모든공격시일때만 발동
		if( typeDamage == 0 && 
			pEffect->invokeJuncture == xJC_ATTACK )		{
			// 이때 pCastingTarget은 피격자가 된다.
			ApplyInvokeEffect( pEffect, m_Level, pDefender, nullptr, TRUE );
		}
	} END_LOOP;
}

/**
 @brief 발동시점 범용 이벤트 처리기
*/
void XBuffObj::OnEventJunctureCommon( ID idEvent, DWORD dwParam, const XSkillReceiver *pRecvParam )
{
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj ) {
		EFFECT *pEffect = &pEffObj->m_effect;
		if( pEffect->invokeJuncture == idEvent ) {
			bool bApply = true;
			if( idEvent == xJC_HP_UNDER && m_pOwner->IsLive() ) {
				bApply = false;
				if( dwParam <= pEffect->dwParam[0] ) {
					if( m_numApply == 0 )
						bApply = true;			// 최초한번만 적용.
				}
// 				if( dwParam > pEffect->dwParam[0] ) {
// 					bApply = false;
// 				} else {
// 					XTRACE("temp");
// 				}
			} else
			if( idEvent == xJC_DEAD )
				m_pOwner->SetpAttacker( const_cast<XSkillReceiver*>( pRecvParam ) );
			if( bApply )
				ApplyInvokeEffect( pEffect, m_Level, m_pOwner, nullptr, TRUE );
		}
	} END_LOOP;
}

/**
 @brief pFromEffect가 발동되어서 발생한 이벤트.
 @param pBaseTarget pFromSkill을 맞은(invoke)대상
*/
void XBuffObj::OnEventInvokeFromSkill( XSkillDat *pFromSkill, 
																			const EFFECT *pFromEffect, 
																			XSkillUser *pCaster, 
																			XSkillReceiver *pBaseTarget )

{
	// 이 버프가 가진 효과들 중에 pFromEffect로부터 발동되어야 할 효과가 있는지 찾는다.
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj ) {
		EFFECT *pEffect = &pEffObj->m_effect;
		if( pEffect->strInvokeTimeSkill == pFromSkill->GetstrIdentifier() ) {
			XLOGXN( "발동시점스킬: %s발동으로 인해 %s발동", pFromSkill->GetstrIdentifier().c_str(),
														m_pDat->GetstrIdentifier().c_str() );
			pBaseTarget->ApplyInvokeEffect( m_pDat, pCaster, m_pOwner, this, pEffect, m_Level );
		}
	} END_LOOP;
}


/**
 @brief 평타공격이 시작되기전에 호출된다.
*/
void XBuffObj::OnEventBeforeAttack( XSKILL::xtJuncture event )

{
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj ) {
		EFFECT *pEffect = &pEffObj->m_effect;
		if( pEffect->invokeJuncture == event ) {
			bool bSuccess = XSKILL::DoDiceInvokeRatio( pEffect, m_Level );
			if( bSuccess && pEffect->invokeJuncture == XSKILL::xJC_RANGE_ATTACK_START )
				if( m_pDelegate )
					m_pDelegate->DelegateResultEventBeforeAttack( this, pEffect );
		}
	} END_LOOP;
}


/**
  버프객체에서 매프레임 버프의 효과를 적용시킨다.(보정치)
  도트효과의 경우 도트시간마다 버프의 효과를 발동(적용)시키고 발동이펙트도 발생시킨다.
  비도트효과의 경우는 효과가 적용되는 최초한번만 발동시킨다.
  비도트효과인데 적용시점이 최초가 아닌경우(타격시라거나)는?
*/
int XBuffObj::ProcessApplyEffect( XSkillReceiver *pInvokeTarget, 
								XSkillReceiver *pOwner,
								EFFECT_OBJ *pEffObj, 
								BOOL bSetTimerDOT )
{
	return 1;
}

/**
 @brief 효과발동 동작의 일원화를 위한 최종 추상화 함수
*/
void XBuffObj::ApplyInvokeEffect( EFFECT *pEffect, 
								int level, 
								XSkillReceiver *pCastingTarget, 
								const char *cScript, 
								BOOL bCreateSfx,
								XVector<XSkillReceiver*> *pOutAryIvkTarget,
								BOOL bGetListMode )
{
	bool bProb = true;
	// 발동확률이 있으면 확률검사. 발동대상 개별적으로 확률적용을 하려면 발동적용확률을 사용해야한다.
	bProb = DoDiceInvokeRatio( pEffect, level );
	if( bProb ) {
		if( !pEffect->m_invokerEff.m_strSpr.empty() ) {
			const float secPlay = 0.f;		// 1play. 발동자이펙트는 반복플레이가 없음.
			m_pCaster->CreateSfx( m_pDat, /*pEffect,*/
																pEffect->m_invokerEff.m_strSpr,
																pEffect->m_invokerEff.m_idAct,
																pEffect->m_invokerEff.m_Point,
																secPlay, XE::VEC2() );
		}
		const auto invokeTarget = pEffect->invokeTarget;
		// 발동대상들을 뽑음.
		XVector<XSkillReceiver*> ary;
		int num = GetpCaster()->GetInvokeTarget( 
											&ary, GetpDat(), level,
											invokeTarget, pEffect, 
											pCastingTarget, 
											m_vCastPos );
		if( ary.size() > 0 ) {
			if( bGetListMode ) {
				// 리스트를 받기만 하는 모드.
				XBREAK( pOutAryIvkTarget == nullptr );
				if( pOutAryIvkTarget )
					*pOutAryIvkTarget = ary;
				// 리스트모드로 호출됐으면 리스트만 받고 리턴
				return;
			}
			// 발동대상들에게 실제 스킬효과를 적용 
			GetpCaster()->ApplyInvokeEffectWithAry( ary
																						, GetpDat()
																						, pEffect
																						, m_pOwner		// invoker
																						, bCreateSfx != 0
																						, m_Level
																						, XE::VEC2(0)
																						, this
																						, nullptr );
			++m_numApply;
			// 발동대상들에게 실제 스킬효과를 적용 & 이벤트 스크립트 실행(루프안에서 pInvokeTarget->ApplyInvokeEffect()로 변경)
		} // ary.size > 0 
	}
}


void XBuffObj::CreateInvokeSfx( EFFECT *pEffect, XSkillReceiver *pInvokeTarget )
{
	float secPlay = 0;
	pInvokeTarget->CreateSfx( m_pDat, 
//							pEffect,
							pEffect->m_invokeTargetEff.m_strSpr,
							pEffect->m_invokeTargetEff.m_idAct,
							pEffect->m_invokeTargetEff.m_Point, 
							secPlay );
}

// 시전대상에게 시전후 최초 발동대상들에게 발동되는 전용 발동함수. DOT초기화를 포함.
void XBuffObj::FirstApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner )
{
	EFFECT *pEffect = &pEffObj->m_effect;
//	XArrayLinearN<XSkillReceiver*, 512> aryInvokeTarget;
	if( pEffect->secInvokeDOT > 0 )
	{
		// 도트효과의 도트시간 처리.
		// 예) 3초지속시간 1초간격도트의 경우
		// 0초에 첫번째적용, 1초에 두번째적용, 2초에 세번째적용, 3초에 효과끝.
		XBREAK( pEffObj->timerDOT.IsOn() == TRUE );
		XBREAK( pEffect->secInvokeDOT == 0 );
		pEffObj->timerDOT.Set( pEffect->secInvokeDOT );
		BOOL bCreateSfx = TRUE;
		// 발동대상들에게 효과적용
		ApplyInvokeEffect( pEffect, m_Level, pOwner, pEffect->scriptDOT.c_str(), bCreateSfx );
	} else
	{
		// 발동대상들에게 효과적용
		BOOL bCreateSfx = TRUE;
		ApplyInvokeEffect( pEffect, m_Level, pOwner, pEffect->scriptProcess.c_str(), bCreateSfx );
	}
	// 발동이펙트 처리
}

void XBuffObj::PersistApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner )
{
	EFFECT *pEffect = &pEffObj->m_effect;
	if( pEffect->secInvokeDOT > 0 )	{
		// 도트효과의 도트시간 처리.
		// 예) 3초지속시간 1초간격도트의 경우
		// 0초에 첫번째적용, 1초에 두번째적용, 2초에 세번째적용, 3초에 효과끝.
		if( pEffObj->timerDOT.IsOff() || pEffObj->timerDOT.IsOver() ) 		{
			int maxDot = 0;
			// 지속시간 / 발동주기로 총 일어나야할 DOT횟수를 구한다.
			if( m_pDat->IsPassive() || m_pDat->IsAbility() )
				maxDot = (int)(99999.f / pEffect->secInvokeDOT);	// 지속시간 무한대
			else
				maxDot = (int)(pEffect->GetDuration(m_Level) / pEffect->secInvokeDOT);
			if( pEffObj->cntDot++ < maxDot )			{
				// 발동대상들에게 효과적용
				BOOL bCreateSfx = TRUE;
				ApplyInvokeEffect( pEffect, m_Level, pOwner, pEffect->scriptDOT.c_str(), bCreateSfx );
				// 도트타이머 재설정
				if( pEffObj->timerDOT.IsOff() ) 				{
					XBREAK( pEffect->secInvokeDOT == 0 );
					pEffObj->timerDOT.Set( pEffect->secInvokeDOT );
				} else
					pEffObj->timerDOT.Reset();
			}	
		}
	} else
	{
		// 발동대상들에게 효과적용
		BOOL bCreateSfx = FALSE;
		ApplyInvokeEffect( pEffect, m_Level, pOwner, pEffect->scriptProcess.c_str(), bCreateSfx );
		// 보정치 지속효과나 상태지속효과는 발동효과를 매프레임 생성할수 없으므로 하지 않음.
	}
}

// 시전대상에 버프객체의 효과하나가 시전된 직후 이벤트가 발생한다.
void XBuffObj::OnCastedEffect( XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj )
{
	EFFECT *pEffect = &pEffObj->m_effect;
	// 발동시점이 "최초"면 시전대상에 시전이 성공한 직후에 
	// 발동대상들에게 발동효과가 적용된다.
	if( pEffect->invokeJuncture == xJC_FIRST )	{
		FirstApplyEffectToInvokeTargets( pEffObj, pOwner );
	} else 
	// 지속형일경우 발동대상에게 최초한번 시작이벤트가 발생하도록 한다.
	if( pEffect->invokeJuncture == xJC_PERSIST )	{
		// 지속형 버프는 버프의 프로세스에서 실시간으로 적용하기위해 여기 처리는 뺌.
	}
}

XLuaSkill* XBuffObj::CreateScript( void )
{
	XLuaSkill *pLua = GetpluaScript();
	if( pLua == nullptr ) {
//		pLua = GetpCastingTarget()->CreateScript();		// virtual
		pLua = GetpCaster()->CreateScript();		// virtual
		pLua->RegisterScript( GetpCaster(), GetpOwner() );
		SetpluaScript( pLua );		// 버프스킬이 갖도록 넘겨줌
	}
	return pLua;
}
void XBuffObj::ExecuteInvokeScript( XSkillReceiver *pInvokeTarget, const char *cScript  )
{
	if( XE::IsHave( cScript ) )	// 스크립이 있을때만
	{			
		// 루아객체를 생성함 이미 있으면 안함
		XLuaSkill *pLua = CreateScript();					
		// 발동대상에 szScript를 실행함
		pLua->InvokeDoScript( pInvokeTarget, cScript );	
	}
}
void XBuffObj::ExecuteScript( XSkillReceiver *pTarget, const char *cScript  )
{
	if( XE::IsHave( cScript ) ) {			// 스크립이 있을때만
		XLuaSkill *pLua = CreateScript();					// 루아객체를 생성함 이미 있으면 안함
		pLua->TargetDoScript( pTarget, cScript );	// 대상에 szScript를 실행함
	}
}



// 버프스킬 프로세스
// 버프/디버프/도트류로 걸려있는 스킬들은 매루프 이걸 실행한다. pOwner:버프소유자
int XBuffObj::Process( XSkillReceiver *pOwner )	
{
	BOOL bAllRelease = TRUE;		// 효과가 전부 소멸됐는가
	BOOL bApply = FALSE;			

	LIST_MANUAL_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj ) {
		EFFECT *pEffect = &pEffObj->m_effect;
		if( XBREAK( GetpDat()->IsBuff( pEffect ) == FALSE ) ) {		// 효과가 버프/도트류가 아니면 에러
			XLOG("buffObj:%s is not buff", GetpDat()->GetstrIdentifier().c_str() );
			return 0;
		}
		// 효과끝난건 리스트에서 삭제
		if( pEffObj->Active == 0 ) {
			m_listEffectObjs.erase( itor++ );		// 끝 스크립까지 다실행한 효과는 아예 날려버린다. pUnit쪽에서 끝스크립을 또 실행하기 때문이다
			continue;		// 효과끝난건 걍 스킵
		}
		itor++;
		// 발동시점 지속발동형이나 도트형 효과들을 위해 매프레임 프로세스를 돌며
		// 효과를 적용시킨다.
		if( pEffect->invokeJuncture == xJC_PERSIST ) {
			if( pOwner->IsLive() ) {
				PersistApplyEffectToInvokeTargets( pEffObj, pOwner );
				if( IsFirstProcess() )
					pOwner->OnEventFirstApplyEffect( m_pDat, m_pCaster, pEffect, m_Level );
			}
		}
		// 효과 해제조건인가
		if( IsClearCondition( pEffObj, GetpDat(), pOwner ) ) {
			// 발동시점이 "마지막" 이면 효과가 해제되는 시점에 발동된다.
			if( pOwner->IsLive() ) {
				if( pEffect->invokeJuncture == xJC_LAST )
					ApplyInvokeEffect( pEffect, m_Level, pOwner, nullptr, TRUE );
			}
			// 버프가 끝날때 하는일을 요청한다
			pEffObj->Active = 0;		// 효과정지
			pOwner->OnClearSkill( GetpDat(), pEffObj );		// 버프가 끝날때 버프가지고 있던놈에게 호출됨
		} // IsClearCondition
		else
			bAllRelease = FALSE;		// 하나라도 돌아가고 있다면 해제하면 안됨
	}
	END_LOOP;
	m_nCount++;
	// 효과가 전부 정지됐는가?
	if( bAllRelease )
		return 0;		// this 삭제시켜라
	return 1;
}

// 버프 해제조건인가
int XBuffObj::IsClearCondition( EFFECT_OBJ *pEffObj, XSkillDat *pSkillDat, XSkillReceiver *pOwner )
{
	if( pEffObj->timerDuration.IsOver() )		// 지속시간이 끝났는가?
		return 1;
	// 토글이나 패시브스킬의경우는 지속시간이 무한대여서 타이머가 첨부터 켜지지 않는다. 그러므로 항상 0을 리턴
	// 패시브나 토글스킬도 아닌데 타이머가 꺼져있는경우가 있다면 에러
	if( XBREAK( pEffObj->timerDuration.IsOff() && 
			(GetpDat()->IsToggle() == FALSE && GetpDat()->IsActive() ) ) ) {
		XLOG( "buffObj:%s", GetpDat()->GetstrIdentifier().c_str() );
	}
	return 0;
}

// 이 스킬이 발동대상에게 처음 적용되는 시점. bInnerApply: Process()에서 호출된건지 ApplyBuff(외부적용상황)에서 불려진건지 플래그
void XBuffObj::OnFirstApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj )
{
	EFFECT *pEffect = &pEffObj->m_effect;
	ExecuteInvokeScript( pInvokeTarget, pEffect->scriptInit.c_str() );
	// 발동SFX생성
	CreateInvokeSfx( pEffect, pInvokeTarget );
	if( pEffect->idInvokeSound )
		m_pCaster->OnSkillPlaySound( pEffect->idInvokeSound );
	// 발동 사운드
	if( pEffect->idInvokeSound )
		pInvokeTarget->OnPlaySoundRecv( pEffect->idInvokeSound );
}

// 이 버프가 대상에서 해제될때 각 발동대상에게 보내는 메시지
void XBuffObj::OnEndApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj )
{
	const EFFECT *pEffect = &pEffObj->m_effect;
	if( XE::IsHave( pEffect->scriptUninit.c_str() ) ) {		// 스크립이 있을때만
		XLuaSkill *pLua = CreateScript();
		if( pLua )
			pLua->InvokeDoScript( pInvokeTarget, pEffect->scriptUninit.c_str() );
	}
	// 발동SFX가 있었다면 각 타겟들에게 삭제하라고 요청. id가 0이어도 보내야함
	if( XE::IsHave( pEffect->m_invokeTargetEff.m_strSpr.c_str() ) )	{
		pInvokeTarget->OnDestroySFX( this, pEffObj->idInvokeSFX );		// 각 발동대상들에게 SFX파괴를 요청
	}
}

/**
 @brief 이 버프의 모든 이펙트의 abilMin값을 조정한다.
*/
void XBuffObj::AddAbilMin( float val )
{
	for( auto pEffObj : m_listEffectObjs ) {
		if( pEffObj->m_effect.invokeAbilityMin.size() > 0 )
			pEffObj->m_effect.invokeAbilityMin[0] += val;
	}
}

ID XBuffObj::GetidSkill() 
{
	XBREAK( m_pDat == nullptr );
	return m_pDat->GetidSkill();
}

EFFECT_LIST& XBuffObj::GetEffectList() {
	return GetpDat()->GetlistEffects();
}
EFFECT* XBuffObj::GetEffectIndex( int idx ) {
	if( idx >= GetpDat()->GetNumEffect() )
		return nullptr;
	return *(GetpDat()->GetlistEffects().GetpByIndex( idx ));
}
int XBuffObj::GetNumEffect() {
	return GetpDat()->GetNumEffect();
}
float XBuffObj::GetAbilMinbyLevel( int idx ) {
	EFFECT *pEffet = GetEffectIndex( idx );
	if( pEffet )
		return pEffet->GetAbilityMin( m_Level );
	return 0;
}
float XBuffObj::GetInvokeRatioByLevel() {
	XBREAK( GetNumEffect() > 1 );
	EFFECT *pEffet = GetEffectIndex( 0 );
	if( pEffet )
		return pEffet->aryInvokeRatio[m_Level];
	return 0;
}
float XBuffObj::GetInvokeSizeByLevel() {
	XBREAK( GetNumEffect() > 1 );
	EFFECT *pEffet = GetEffectIndex( 0 );
	if( pEffet )
		return pEffet->GetInvokeSize( m_Level );
	return 0;
}

/*
스킬발동sfx의해제방법
버프류 {
	발동대상은 여럿일수 있기때문에  sfx도 각 발동타겟별로 생겼을꺼다.
	일반화스킬에선 버프가 직접 타겟에게 걸리기땜에 그 버프가 생성한 이펙트 아이디를 갖고 있다가 삭제를 요청하면 된다 {
		만약 발동대상이 중간에 죽어없어져서 요청할 상대가 없으면? 그 타겟이 없어질때 버프도 같이 없어질거기 땜에 타겟이 죽기전에 삭제요청하면 된다. 그러면 타겟은 삭제하고 자신이 사라진다.
		만약 발동대상이 여럿인경우. ex)타겟에게 10초간 저주를 걸면 그 타겟이 돌아다닐때 주변 적에게 5초간 도트뎀지 디버프가 걸리게 한다 {
			이렇게 복잡한경우는 스킬하나로 해결이 안된다. 일단 발동시간도 2가지나 되기때문에 이경우는 "발동스킬"이라는 형태로 스킬 하나를 더 써서 구현해야 할듯.
			주변에 도트뎀디버프를 걸땐 다시 도트뎀 디버프가 각각 생성되기땜에 해결된다.	그러니 이런경우를 구현하기 위해 XSkillReceiver에 발동sfx의 리스트를 만들필욘 없다.
		}
	}
	토끼게임에선 버프가 장군이나 스테이지에 걸리고 유닛은 그 버프를 받아다 자신에게 적용만 한다 이때는? {
		각 유닛별로 sfx생성을 했다. 그 아이디를 버프가 가지고 있을수 없다. 버프는 하난데 타겟은 여러개기때문이다.
		그렇다면 결국 유닛이 갖고 있어야 한단 얘긴데. 이건 토끼게임에만 특화된 상황이므로 XUnit에 그 리스트를 가지고 있어야 한다
		버프가 삭제될때 각 타겟에게 XSkillDat id와 함께 sfx삭제요청을 보내면 유닛은 자신이 가지고 있던 sfx리스트(맵)에서 idSkillDat에 해당하는
		오브젝트 아이디를 구해서 그넘을 삭제시킨다 같은종류의 버프가 두개걸리는 경우도 있겠지만 그때는 sfx생성이 하나밖에 안생기게 만들어야 한다.
		그러므로 idSkillDat와 OBJID를 짝지어서 보관하고 있으면 해결될듯
	}
		
}
1회성류 {
	이건 그냥 애니메이션 한번 플레이하고 끝나면 된다.
}
*/

LPCTSTR XBuffObj::GetstrIconByCaller() const 
{
	if( m_pDatCaller )
		return m_pDatCaller->GetstrIcon().c_str();
	return _T( "" );
}


XE_NAMESPACE_END
