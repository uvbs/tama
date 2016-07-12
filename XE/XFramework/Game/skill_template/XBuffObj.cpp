#include "stdafx.h"
#include "xSkill.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XSKILL::XBuffObj>* XMemPool<XSKILL::XBuffObj>::s_pPool = NULL;

NAMESPACE_XSKILL_START

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
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		EFFECT *pEffect = pEffObj->refEffect;
		// 이 버프의 효과가 죽이면 발동하는 효과인가?
		if( pEffect->invokeJuncture == xJC_KILL_ENEMY )
		{
			ApplyInvokeEffect( pEffect, pOwner, NULL, TRUE );
		}
	} END_LOOP;
}

// 이 버프의 주인이 idAttacker로부터 맞았다.
// 이 버프가 가진 효과중 피격시 발동하는 효과가 있다면 그 효과를 발동타겟들에게 발동시킨다.
void XBuffObj::OnSkillEventMeleeHitFromAttacker( XSkillReceiver *pOwner, ID idAttacker )
{
	// 공격자를 세팅함.
	m_pCaster->SetidEventAttacker( idAttacker );
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		EFFECT *pEffect = pEffObj->refEffect;
		// 이 버프의 효과가 맞으면 발동하는 효과인가?
		if( pEffect->invokeJuncture == xJC_CLOSE_HIT )
		{
			ApplyInvokeEffect( pEffect, pOwner, NULL, TRUE );
		}
/*
			XArrayLinearN<XSkillReceiver*, 100> aryInvokeTarget;
			// 발동대상이 공격자이면 발동타겟을 공격자로 넣어준다.
			if( pEffect->invokeTarget == xIVT_ATTACKER )
			{
				XSkillReceiver *pTarget = m_pDelegate->GetTarget( idAttacker );
				if( pTarget )
					aryInvokeTarget.Add( pTarget );
			}
			else
			{
				m_pCaster->GetInvokeTarget( &aryInvokeTarget,
												GetpDat(),
												pEffect->invokeTarget,
												pEffect,
												pOwner,
												NULL );
			}
			// 타겟리스트에 있는 발동타겟들에게 스킬효과를 발동시킨다.
			ApplyEffectToInvokeTargetList( aryInvokeTarget, pEffect, pOwner, NULL );
			// 발동이펙트가 있다면 생성해준다.
			XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
			{
				CreateInvokeSfx( pEffect, pInvokeTarget );
			} END_LOOP;
		}
		*/
	} END_LOOP;
}

// pOwner가 idDefender를 근접공격으로 때렸다.
void XBuffObj::OnSkillEventAttackToDefender( XSkillReceiver *pOwner, ID idDefender )
{
	// 피격자를 세팅함.
	m_pCaster->SetidEventHitter( idDefender );
	//
	LIST_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		EFFECT *pEffect = pEffObj->refEffect;
		// 이 버프의 효과가 공격시 발동하는 효과인가
		if( pEffect->invokeJuncture == xJC_CLOSE_ATTACK )
		{
			ApplyInvokeEffect( pEffect, pOwner, NULL, TRUE );
		}
/*
		DWORD dice = xRandom(1000);
		DWORD ratioInvoke = (DWORD)(1000 * pEffect->invokeRatio);
		// 피격자를 세팅함.
		m_pCaster->SetidEventHitter( idDefender );
		if( dice < ratioInvoke )
		{
			// 이 버프의 효과가 공격시 발동하는 효과인가
			if( pEffect->invokeJuncture == xJC_CLOSE_ATTACK )
			{
				XArrayLinearN<XSkillReceiver*, 100> aryInvokeTarget;
				// 발동대상이 피격자이면 발동타겟을 피격자로 넣어준다.

				if( pEffect->invokeTarget == xIVT_DEFENDER )
				{
					XSkillReceiver *pTarget = m_pDelegate->GetTarget( idDefender );
					if( pTarget )
						aryInvokeTarget.Add( pTarget );
				}
				else
				{
					m_pCaster->GetInvokeTarget( &aryInvokeTarget,
												GetpDat(),
												pEffect->invokeTarget,
												pEffect,
												pOwner,
												NULL );
				}
				// 타겟리스트에 있는 발동타겟들에게 스킬효과를 발동시킨다.
				ApplyEffectToInvokeTargetList( aryInvokeTarget, pEffect, pOwner, NULL );
				// 발동이펙트가 있다면 생성해준다.
				XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
				{
					CreateInvokeSfx( pEffect, pInvokeTarget );
				} END_LOOP;
			}
		}
*/
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
/*
	EFFECT *pEffect = pEffObj->refEffect;
	// 이 효과에 최초 적용되는 순간에 할일을 호출한다
	if( m_nCount == 0 )
	{
		OnFirstApplyEffect( pInvokeTarget, pEffObj );
		// 이런식으로 처리하면 이함수를 외부에서 독자적으로 호출했을때
		// OnFirst...()가 두번호출되는 경우가 생김.
	}
	// 스킬버프 적용
	if( pEffect->secInvokeDOT > 0 )	// 도트류 스킬
	{
		XBREAK( pEffObj->timerDOT.IsOff() );		// 도트타이머가 아예 켜지지도 않은경우가 있다면 에러
		if( pEffObj->timerDOT.IsOver() )		// 도트타이머 오버되면
		{
			// 이 버프의 발동대상에게 효과 적용
			pInvokeTarget->ApplyInvokeEffect( m_pCaster, pOwner, pEffect );
			// 효과적용
//			pInvokeTarget->ApplyEffect( m_pCaster, pEffect );
			// "도트" 스크립트 실행
			ExecuteInvokeScript( pInvokeTarget, pEffect->scriptDOT.c_str() );
		}
	} else
	{	// 도트류가 아닌 버프스킬류
		pInvokeTarget->ApplyEffect( m_pCaster, pEffect );
		// "실행" 스크립트 실행
		ExecuteInvokeScript( pInvokeTarget, pEffect->scriptProcess.c_str() );
	}
	*/
	return 1;
}

/*
void XBuffObj::ApplyBuffToInvokeTargets( XSkillReceiver *pOwner )
{
	LIST_MANUAL_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		const EFFECT *pEffect = pEffObj->refEffect;
		ApplyEffectToInvokeTargets( pEffObj, pOwner );
	} END_LOOP;
}
*/
// 인수로 주어진 타겟리스트에 스킬효과를 적용시킨다.
void XBuffObj::ApplyEffectToInvokeTargetList( 
							XArrayLinearN<XSkillReceiver*, 100>& aryInvokeTarget,
							EFFECT *pEffect, 
							XSkillReceiver *pOwner,
							const char *cScript )
{
	XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
	{
		// 발동대상에게 스킬효과를 적용
		pInvokeTarget->ApplyInvokeEffect( m_pCaster, pOwner, this, pEffect );
		// 해당 스크립트 실행
		if( XE::IsHave( cScript ) )
			ExecuteInvokeScript( pInvokeTarget, cScript );
	} END_LOOP;

}

// pEffect의 발동타겟들을 얻은후 타겟들에게 효과를 발동시킨다.
void XBuffObj::GetInvokeTargetsAndApplyEffect( 
					XArrayLinearN<XSkillReceiver*, 100> *pOutAryInvokeTarget,
											EFFECT *pEffect, 
											XSkillReceiver *pOwner,
											const char* cScript )
{
	XArrayLinearN<XSkillReceiver*, 100> ary;
	int num = GetpCaster()->GetInvokeTarget( 
									&ary, GetpDat(), 
									pEffect->invokeTarget, pEffect, 
									pOwner, 
									(m_vCastPos.IsZero())? NULL: &m_vCastPos );
	if( num > 0 )
	{
		// 발동대상들을 사용자 정의 필터로 한번더 거른다.
		GetpCaster()->CustomInvokeFilter( pOutAryInvokeTarget, ary, pEffect );
		ApplyEffectToInvokeTargetList( *pOutAryInvokeTarget, pEffect, pOwner, cScript );
	} else
		pOutAryInvokeTarget->Clear();
}

// 효과발동 동작의 일원화를 위한 최종 추상화 함수
void XBuffObj::ApplyInvokeEffect( EFFECT *pEffect, 
								XSkillReceiver *pOwner, 
								const char *cScript, 
								BOOL bCreateSfx,
								XArrayLinearN<XSkillReceiver*, 100> *pOutAryIvkTarget,
								BOOL bGetListMode )
{
	BOOL bPassProb = TRUE;
	// 발동확률이 있고 지속형이 아닐경우(DOT는 예외) 확률검사를 한다.
	if( pEffect->invokeRatio < 1.0f && 
		(pEffect->invokeJuncture != xJC_PERSIST ||
		 pEffect->secInvokeDOT > 0 ||
		 pEffect->strInvokeSkill.empty() == false ||
		 pEffect->idInvokeSkill ) )
	{
		DWORD dice = xRandom(1000);
		DWORD ratioInvoke = (DWORD)(1000 * pEffect->invokeRatio);
		if( dice >= ratioInvoke )
			bPassProb = FALSE;
	}
	// 커스텀 발동조건을 검사한다.
	if( m_pCaster->IsInvokeCustomCondition( pEffect ) == FALSE )
		bPassProb = FALSE;
	if( bPassProb )
	{
		// 발동대상들을 뽑음.
		XArrayLinearN<XSkillReceiver*, 100> aryTemp;
		int num = GetpCaster()->GetInvokeTarget( 
											&aryTemp, GetpDat(), 
											pEffect->invokeTarget, pEffect, 
											pOwner, 
											(m_vCastPos.IsZero())? NULL: &m_vCastPos );
		if( num > 0 )
		{
			XArrayLinearN<XSkillReceiver*, 100> ary;
			ary = aryTemp;
			// 발동대상들을 사용자 정의 필터로 한번더 거른다.
			GetpCaster()->CustomInvokeFilter( &ary, aryTemp, pEffect );
			if( ary.size() > 0 )
			{
				if( bGetListMode )
				{
					// 리스트를 받기만 하는 모드.
					XBREAK( pOutAryIvkTarget == NULL );
					if( pOutAryIvkTarget )
						*pOutAryIvkTarget = ary;
					return;
				}
				// 발동대상들에게 실제 스킬효과를 적용 & 이벤트 스크립트 실행
				ApplyEffectToInvokeTargetList( ary, pEffect, pOwner, cScript );
				// 추가 처리
				XARRAYLINEARN_LOOP( ary, XSkillReceiver*, pInvokeTarget )
				{
					// 발동이펙트가 있다면 생성해준다.
					if( bCreateSfx )
						CreateInvokeSfx( pEffect, pInvokeTarget );
					if( pEffect->idInvokeSound )
						m_pCaster->OnSkillPlaySound( pEffect->idInvokeSound );
					// 이벤트 핸들러
					//m_pDelegate->OnDotApply( pEffect );
				} END_LOOP;
			}
		}
	}
}


void XBuffObj::CreateInvokeSfx( EFFECT *pEffect, XSkillReceiver *pInvokeTarget )
{
	// 발동sfx가 있으면 
	if( pEffect->strInvokeEffect.empty() == false )
	{
		float secLife = 0.f;
		// 플레이모드가 루프로 되어있으면 지속시간동안 계속 루프플레이 되고
		// 아니면 한번플레이되고 끝나게 한다.
		if( pEffect->invokeEffectPlayMode == xAL_LOOP )
			secLife = pEffect->secDuration;
		else if( pEffect->invokeEffectPlayMode == xAL_ONCE )
			secLife = 0.f;
		else if( pEffect->invokeEffectPlayMode == xAL_NONE )
			secLife = 0.f;
		else
			XBREAKF( 1, "%s:unknown invokeEffectPlayMode: %d", 
						GetpDat()->GetstrIdentifier().c_str(), 
						pEffect->invokeEffectPlayMode );
		// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
		xtPoint pointSfx = pEffect->invokeEffectPoint;
		if( pointSfx == xPT_NONE )
			pointSfx = xPT_TARGET_BOTTOM;
	
		ID idSfx = pInvokeTarget->OnCreateInvokeSFX( this, 
										pEffect,
										m_pCaster,
										NULL,
										pEffect->strInvokeEffect.c_str(), 
										(pEffect->idInvokeEffect==0)? 1 : pEffect->idInvokeEffect,
										pointSfx,	// 일단 이렇게
										secLife );
	}
}

// 시전대상에게 시전후 최초 발동대상들에게 발동되는 전용 발동함수. DOT초기화를 포함.
void XBuffObj::FirstApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner )
{
	EFFECT *pEffect = pEffObj->refEffect;
//	XArrayLinearN<XSkillReceiver*, 100> aryInvokeTarget;
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
		ApplyInvokeEffect( pEffect, pOwner, pEffect->scriptDOT.c_str(), bCreateSfx );
/*
		GetInvokeTargetsAndApplyEffect( &aryInvokeTarget, 
										pEffect, 
										pOwner, 
										pEffect->scriptDOT.c_str() );
		// 추가 처리
		XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
		{
			// 이벤트 핸들러
			//m_pDelegate->OnDotApply( pEffect );
		} END_LOOP;
*/
	} else
	{
		// 발동대상들에게 효과적용
		BOOL bCreateSfx = TRUE;
		ApplyInvokeEffect( pEffect, pOwner, pEffect->scriptProcess.c_str(), bCreateSfx );
/*
		GetInvokeTargetsAndApplyEffect( &aryInvokeTarget, 
										pEffect, 
										pOwner, 
										pEffect->scriptProcess.c_str() );
*/
	}
	// 발동이펙트 처리
/*
	XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
	{
		// 발동이펙트가 있다면 만들어준다.
		CreateInvokeSfx( pEffect, pInvokeTarget );
	} END_LOOP;
*/
}

void XBuffObj::PersistApplyEffectToInvokeTargets( EFFECT_OBJ *pEffObj, XSkillReceiver *pOwner )
{
	EFFECT *pEffect = pEffObj->refEffect;
	if( pEffect->secInvokeDOT > 0 )
	{
		// 도트효과의 도트시간 처리.
		// 예) 3초지속시간 1초간격도트의 경우
		// 0초에 첫번째적용, 1초에 두번째적용, 2초에 세번째적용, 3초에 효과끝.
		if( pEffObj->timerDOT.IsOff() || pEffObj->timerDOT.IsOver() ) 
		{
			if( pEffObj->cntDot++ < (int)(pEffect->secDuration / pEffect->secInvokeDOT) )
			{
				// 발동대상들에게 효과적용
				BOOL bCreateSfx = TRUE;
				ApplyInvokeEffect( pEffect, pOwner, pEffect->scriptDOT.c_str(), bCreateSfx );
/*
				XArrayLinearN<XSkillReceiver*, 100> aryInvokeTarget;
				// 발동대상들에게 효과적용
				GetInvokeTargetsAndApplyEffect( &aryInvokeTarget, 
												pEffect, 
												pOwner, 
												pEffect->scriptDOT.c_str() );
				// 추가 처리
				XARRAYLINEARN_LOOP( aryInvokeTarget, XSkillReceiver*, pInvokeTarget )
				{
					// 발동이펙트가 있다면 생성해준다.
					CreateInvokeSfx( pEffect, pInvokeTarget );
					// 이벤트 핸들러
					//m_pDelegate->OnDotApply( pEffect );
				} END_LOOP;
*/
				// 도트타이머 재설정
				if( pEffObj->timerDOT.IsOff() ) 
				{
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
		ApplyInvokeEffect( pEffect, pOwner, pEffect->scriptProcess.c_str(), bCreateSfx );
/*
		XArrayLinearN<XSkillReceiver*, 100> aryInvokeTarget;
		// 발동대상들에게 효과적용
		GetInvokeTargetsAndApplyEffect( &aryInvokeTarget, 
										pEffect, 
										pOwner, 
										pEffect->scriptProcess.c_str() );
*/
		// 보정치 지속효과나 상태지속효과는 발동효과를 매프레임 생성할수 없으므로 하지 않음.
	}
}
/*
// 시전대상에 버프객체가 시전된 직후 이벤트가 발생한다.
void XBuffObj::OnCastedBuffObj( XSkillReceiver *pOwner )
{
	LIST_MANUAL_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		const EFFECT *pEffect = pEffObj->refEffect;
		// 발동시점이 "최초"면 시전대상에 시전이 성공한 직후에 
		// 발동대상들에게 발동효과가 적용된다.
		if( pEffect->invokeJuncture == xJC_FIRST )
		{
			FirstApplyEffectToInvokeTargets( pOwner );
		}
	} END_LOOP;
} */

// 시전대상에 버프객체의 효과하나가 시전된 직후 이벤트가 발생한다.
void XBuffObj::OnCastedEffect( XSkillReceiver *pOwner, EFFECT_OBJ *pEffObj )
{
	EFFECT *pEffect = pEffObj->refEffect;
	// 발동시점이 "최초"면 시전대상에 시전이 성공한 직후에 
	// 발동대상들에게 발동효과가 적용된다.
	if( pEffect->invokeJuncture == xJC_FIRST )
	{
		FirstApplyEffectToInvokeTargets( pEffObj, pOwner );
	} else 
	if( m_pDat->GetCastMethod() == xPASSIVE )
	{
		// 패시브면 스킬이 시전된후 효과를 한번 실행시킴.
		BOOL bGetListMode = FALSE;
		ApplyInvokeEffect( pEffect, pOwner, NULL, FALSE, NULL, bGetListMode );
	} else
	// 지속형일경우 발동대상에게 최초한번 시작이벤트가 발생하도록 한다.
	if( pEffect->invokeJuncture == xJC_PERSIST )
	{
		XArrayLinearN<XSkillReceiver*, 100> aryIvTarget;
		BOOL bGetListMode = TRUE;
		ApplyInvokeEffect( pEffect, pOwner, NULL, FALSE, &aryIvTarget, bGetListMode );
/*
		int num = GetpCaster()->GetInvokeTarget( 
									&aryIvTarget, GetpDat(), 
									pEffect->invokeTarget, pEffect, 
									pOwner, 
									(m_vCastPos.IsZero())? NULL: &m_vCastPos );
*/
		// 발동대상들에게 스킬효과의 시작을 알리는 이벤트 발생
		XARRAYLINEARN_LOOP( aryIvTarget, XSkillReceiver*, pIvTarget )
		{
			pIvTarget->OnStartSkillEffect( m_pCaster, pOwner, pEffect );
			if( pEffect->invokeState )
				pIvTarget->OnStartSkillState( m_pCaster, pOwner, pEffect );
		} END_LOOP;
	}
}
/*
void XBuffObj::ProcessApplyEffectToInvokeTargets( XSkillReceiver *pOwner )
{
	PersistApplyEffectToInvokeTargets( pOwner );
} */

XLuaSkill* XBuffObj::CreateScript( void )
{
	XLuaSkill *pLua = GetpluaScript();
	if( pLua == NULL ) {
//		pLua = GetpCastingTarget()->CreateScript();		// virtual
		pLua = GetpCaster()->CreateScript();		// virtual
		pLua->RegisterScript( GetpCaster(), GetpCastingTarget() );
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

	LIST_MANUAL_LOOP( m_listEffectObjs, EFFECT_OBJ*, itor, pEffObj )
	{
		EFFECT *pEffect = pEffObj->refEffect;
		XBREAK( GetpDat()->IsBuff( pEffect ) == FALSE );		// 효과가 버프/도트류가 아니면 에러
		// 효과끝난건 리스트에서 삭제
		if( pEffObj->Active == 0 )		
		{
			m_listEffectObjs.erase( itor++ );		// 끝 스크립까지 다실행한 효과는 아예 날려버린다. pUnit쪽에서 끝스크립을 또 실행하기 때문이다
			continue;		// 효과끝난건 걍 스킵
		}
		if( pEffect->invokeCondition != xNONE_COND )		
		{
			bAllRelease = FALSE;		// 발동조건으로 continue된건 버프삭제하면 안된다
			itor++;
			continue;	// 발동조건 없음일때만 여기서 효과를 적용한다.
		}
		itor++;
		// 발동시점 지속발동형이나 도트형 효과들을 위해 매프레임 프로세스를 돌며
		// 효과를 적용시킨다.
		if( pEffect->invokeJuncture == xJC_PERSIST )
			PersistApplyEffectToInvokeTargets( pEffObj, pOwner );

/*
		// 발동대상얻기
		XArrayLinearN<XSkillReceiver*, 100> listInvokeTarget;
		if( GetpCaster()->GetInvokeTarget( &listInvokeTarget, 
										GetpDat(), 
										pEffect->invokeTarget, 
										pEffect, 
										pOwner, 
									(m_vCastPos.IsZero())? NULL: &m_vCastPos ) )
		{
			XARRAYLINEARN_LOOP( listInvokeTarget, XSkillReceiver*, pBuffTarget )
			{
				// 발동대상에게 효과발동처리 서브 프로세스
				ProcessApplyEffect( pBuffTarget, pOwner, pEffObj );
				// 이 버프가 대상에게 최초 적용되는 순간
				if( m_nCount == 0 )
					if( bApply == FALSE )		// 효과가 여러개일경우는 이검사를 해줘야 여러번 호출되지 않음
						pBuffTarget->OnFirstApplyBuff( this, TRUE );
				bApply = TRUE;		// 
			} END_LOOP;
		}
		*/
/*
		if( pEffect->secInvokeDOT > 0 )				// 도트류 스킬
		{
			XBREAK( pEffObj->timerDOT.IsOff() );		// 도트타이머가 아예 켜지지도 않은경우가 있다면 에러
			if( pEffObj->timerDOT.IsOver() )		// 도트타이머 오버되면
				if( OnDOTTimerSet( pEffect ) )
					pEffObj->timerDOT.Set( pEffect->secInvokeDOT );		// 도트타이머 재셋팅
		}
		*/
		// 효과 해제조건인가
		if( IsClearCondition( pEffObj, GetpDat(), pOwner ) ) 
		{
			// 발동시점이 "마지막" 이면 효과가 해제되는 시점에 발동된다.
			if( pEffect->invokeJuncture == xJC_LAST )
				ApplyInvokeEffect( pEffect, pOwner, NULL, TRUE );
			// 버프가 끝날때 하는일을 요청한다
			pEffObj->Active = 0;		// 효과정지
			pOwner->OnClearSkill( GetpDat(), pEffObj );		// 버프가 끝날때 버프가지고 있던놈에게 호출됨
/*			일단은 잠시 지워둠 13/8/29
			XARRAYLINEARN_LOOP( listInvokeTarget, XSkillReceiver*, pBuffTarget )
			{
//				CONSOLE( "XBuffObj::Process: Call OnEndApplyEffect: target:0x%08x Skill:%s(%d) cnt=%d", (int)pBuffTarget, m_pDat->GetszName(), m_idBuff, m_nCount );
				OnEndApplyEffect( pBuffTarget, pEffObj );
			} END_LOOP;
			*/
		} // IsClearCondition
		else
			bAllRelease = FALSE;		// 하나라도 돌아가고 있다면 해제하면 안됨
	}
	END_LOOP;
	// 효과가 전부 정지됐는가?
	if( bAllRelease ) {
		m_nCount ++;
		return 0;		// this 삭제시켜라
	}
	m_nCount ++;
	return 1;
}

// 버프 해제조건인가
int XBuffObj::IsClearCondition( EFFECT_OBJ *pEffObj, XSkillDat *pSkillDat, XSkillReceiver *pOwner )
{
	if( pEffObj->timerDuration.IsOver() )		// 지속시간이 끝났는가?
		return 1;
	// 토글이나 패시브스킬의경우는 지속시간이 무한대여서 타이머가 첨부터 켜지지 않는다. 그러므로 항상 0을 리턴
	// 패시브나 토글스킬도 아닌데 타이머가 꺼져있는경우가 있다면 에러
	XBREAK( pEffObj->timerDuration.IsOff() && 
			(GetpDat()->GetCastMethod() != xTOGGLE && 
			!GetpDat()->IsPassive() ) );	
	return 0;
}

// 이 스킬이 발동대상에게 처음 적용되는 시점. bInnerApply: Process()에서 호출된건지 ApplyBuff(외부적용상황)에서 불려진건지 플래그
void XBuffObj::OnFirstApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj )
{
	EFFECT *pEffect = pEffObj->refEffect;
	ExecuteInvokeScript( pInvokeTarget, pEffect->scriptInit.c_str() );
	// 발동SFX생성
	CreateInvokeSfx( pEffect, pInvokeTarget );
	if( pEffect->idInvokeSound )
		m_pCaster->OnSkillPlaySound( pEffect->idInvokeSound );
/*	if( XE::IsHave(pEffect->strInvokeEffect.c_str()) )
	{
		BOOL bLoop = (pEffect->invokeEffectPlayMode == xAL_LOOP )? TRUE : FALSE;
		// 스킬발동SFX 생성 요청. 나중에 버프끝나면 sfx삭제를 위해서 아이디를 받아둠
		pEffObj->idInvokeSFX = pInvokeTarget->OnCreateInvokeSFX( this, 
													pEffect->strInvokeEffect.c_str(), 
													pEffect->idInvokeEffect, 
													bLoop );		
//		pEffObj->idInvokeSFX = pInvokeTarget->OnCreateSFX( this, pEffect->szInvokeEffect, pEffect->idInvokeEffectAct, bLoop );		
	} */
	// 발동 사운드
	if( pEffect->idInvokeSound )
		pInvokeTarget->OnPlaySoundRecv( pEffect->idInvokeSound );
}

// 이 버프가 대상에서 해제될때 각 발동대상에게 보내는 메시지
void XBuffObj::OnEndApplyEffect( XSkillReceiver *pInvokeTarget, EFFECT_OBJ *pEffObj )
{
	const EFFECT *pEffect = pEffObj->refEffect;
	if( XE::IsHave( pEffect->scriptUninit.c_str() ) )		// 스크립이 있을때만
	{			
		XLuaSkill *pLua = CreateScript();
		if( pLua )
			pLua->InvokeDoScript( pInvokeTarget, pEffect->scriptUninit.c_str() );
	}
	// 발동SFX가 있었다면 각 타겟들에게 삭제하라고 요청. id가 0이어도 보내야함
	if( XE::IsHave( pEffect->strInvokeEffect.c_str() ) )
	{
		pInvokeTarget->OnDestroySFX( this, pEffObj->idInvokeSFX );		// 각 발동대상들에게 SFX파괴를 요청
	}
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

NAMESPACE_XSKILL_END
