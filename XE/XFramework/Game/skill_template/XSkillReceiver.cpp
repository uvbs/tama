#include "stdafx.h"
#include "xSkill.h"
#include "game/XEComponents.h"
//using namespace XSKILL;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

NAMESPACE_XSKILL_START
// this에 pcEffect가 시전가능한지 검사
BOOL XSkillReceiver::IsCastAble( XSkillUser *pCaster, const EFFECT *pEffect )
{
//	CIV_EFFECT *pcEffect = (CIV_EFFECT *)pEffect;
#pragma message( "발동대상 플레이어/AI는 발동대상진영 같은걸 따로 만들어서 해야할듯" )
/*	if( pEffect->castfiltFriendship == xfAI ) {			// AI에게쓰는 스킬인데
		if( xAI != GetFriendship() )			// 타겟이 AI가 아니면 실패
			return FALSE;						
	} else
	if( pEffect->castfiltFriendship == xfPLAYER ) {			// 플레이어에게 쓰는스킬인데
		if( xPLAYER != GetFriendship() )			// 타겟이 인간이 아니면 실패
			return FALSE;						
	}
*/
	if( (pEffect->castfiltPlayerType & xfAI) == 0 ) {			// AI에게 쓸수 없는 스킬인데
		if( xAI == GetPlayerType() )			// 타겟이 AI면 실패
			return FALSE;						
	} else
	if( (pEffect->castfiltPlayerType & xfHUMAN ) == 0 ) {			// 인간플레이어에게 쓸수 없는 스킬인데
		if( xHUMAN == GetPlayerType() )			// 타겟이 인간이면 실패
			return FALSE;						
	}
	//
	if( (pEffect->castfiltFriendship & xfHOSTILE) == 0 )	
	{
		// 적에게는 쓸수 없는스킬인데
		// 타겟우호가 나랑다르고 타겟이 중립도 아닐때는 적용금지
		if( XECompCamp::sIsNotFriendly(pCaster->GetCampUser(), GetCamp()) &&
			XECompCamp::sIsNeutrality( GetCamp() ) == FALSE )
			return FALSE;
/*
		// 타겟우호가 나랑다르고 타겟이 중립이 아닐때
		if( (pCaster->GetCamp() != GetCamp()) && 
			(pCaster->GetCamp() != xSIDE_NEUTRALITY) )			
			return FALSE;							// 적에겐 사용할수 없습니다
*/
	}
	if( (pEffect->castfiltFriendship & xfALLY) == 0 )	
	{
		// 아군에게는 쓸수 없는스킬인데
		// 대상이 아군이면 적용금지
		if( XECompCamp::sIsFriendly(pCaster->GetCampUser(), GetCamp()) )
			return FALSE;
// 		if( pCaster->GetCamp() == GetCamp() )	// 시전자와 타겟이 같은편이다면
// 			return FALSE;					// 아군에게는 쓸수 없습니다.
	}
	if( (pEffect->castfiltFriendship & xfNEUTRALITY) == 0 )			
	{
		// 중립에게는 쓸수 없는스킬인데 
		// 타겟이 중립이면 적용금지.
		if( XECompCamp::sIsNeutrality( GetCamp() ) )
			return FALSE;
//		if( xSIDE_NEUTRALITY == GetCamp() )			// 타겟이 중립이면
//			return FALSE;						// 중립에겐 사용할수 없습니다
	}
	return TRUE;
}

// 버프리스트에 버프객체를 추가함
int XSkillReceiver::AddSkillRecvObj( XBuffObj *pSkillRecvObj )
{
	// 버프 중복적용검사는 InvokeEffect에서 했음
	m_listSkillRecvObj.Add( pSkillRecvObj );
	return 1;
}
/**
 @brief 비보정 파라메터 효과를 적용한다.
*/
int XSkillReceiver::ApplyEffectNotAdjParam( XSkillUser *pCaster, EFFECT *pEffect )
{
	// 보정파라메터이거나 invalid한것은 0을 리턴
	///< 발동되는 효과를 직접 하드코딩 하고 싶을때는 발동파라메터가 보통 0이기때문에 하위에서 상속받을수 있는 이곳에서 검사하게 했다. 웒하면 상속받는쪽에서 검사안할수 있으니까
	if( IsAdjParam( pEffect->invokeParameter ) >= 0 )	return 0;		
	if( pEffect->invokeParameter < 0 )
	{
		float invokeAbilityMin = pEffect->invokeAbilityMin;
		float invokeAbilityMax = pEffect->invokeAbilityMax;
		// 능력치값을 다른 다른형태로 변형해서 사용하고 싶다면 아래 핸들러를 정의해서 바꾼다.
		pCaster->OnAdjustEffectAbility( &invokeAbilityMin, &invokeAbilityMax );
		return OnApplyEffectNotAdjParam( pCaster, pEffect, invokeAbilityMin, invokeAbilityMax );
	}
/*
	float ability = pEffect->invokeAbilityMin;
	switch( pEffect->invokeParameter )
	{
	case xHP:
		if( pEffect->invokeAbilityMin < 0 )		// 데미지일때만 온데미지 호출
			OnSkillDamage( pCaster, -ability, pEffect->valtypeInvokeAbility );		// 일단은 min값만 쓴다. "데미지값"을 주는거기땜에 음수는 양수로 바꾼다
		else
			OnSkillHeal( ability, pEffect->valtypeInvokeAbility );
		return 1;
	}
*/
	return 0;
}
/**
 @brief 보정 파라메터의 값을 보정한다.
*/
int XSkillReceiver::ApplyEffectAdjParam( EFFECT *pEffect )
{
	if( pEffect->invokeParameter <= 0 )	return 0;
	XBREAKF( IsAdjParam( pEffect->invokeParameter ) < 0, 
		"처리하지 못한 비보정파라메터(%d)가 있습니다.", pEffect->invokeParameter );
	float ability = pEffect->invokeAbilityMin;
	AddAdjParam( pEffect->invokeParameter, pEffect->valtypeInvokeAbility, ability );
	//

	return 1;
}

// this에게 발동효과 적용
int XSkillReceiver::ApplyInvokeEffect( XSkillUser *pCaster, 
										XSkillReceiver* pInvoker, 
										XBuffObj *pBuffObj, 
										EFFECT *pEffect )
{
	static EFFECT tempEffect;
	// 캐스터가 스킬파워 보정이 되어있는상태면 이펙트버퍼를 보정해준다
	// 스킬파워 보정구현은 하위객체에게 보정값을 요구하는식으로 바꿀것.
// 	if( pCaster && pCaster->GetThisRecv() )	// 캐스터가 XSkillReceiver로부터 상속받은것이라야 한다
// 	{
// 		XSkillReceiver *pRecvCaster = pCaster->GetThisRecv();
// 		if( pRecvCaster->IsHaveAdjParam( xADJ_SKILL_POWER ) )	// 캐스터가 스킬파워보정 상태인가?
// 		{
// 			tempEffect = *pEffect;	// 임시버퍼에 복사하고
// 			float mul = 1.0f;
// 			mul = pRecvCaster->CalcAdjParam( mul, xADJ_SKILL_POWER );	// 캐스터의 스킬파워 보정치 획득
// 			tempEffect.invokeAbilityMin *= mul;
// 			tempEffect.invokeAbilityMax *= mul;
// 			pEffect = &tempEffect;			// 이펙트 버퍼를 임시버퍼로 교체
// 		}
// 		// 스킬능력증폭 및 추가. virtual
// 		float ratioAdd = 0.f, add = 0.f;
// 		pRecvCaster->OnSkillAmplify( this, pEffect, &ratioAdd, &add );
// 		if( ratioAdd != 0.f || add != 0.f )
// 		{
// 			tempEffect = *pEffect;
// 			pEffect = &tempEffect;
// 			if( ratioAdd != 0.f )
// 			{
// 				tempEffect.invokeAbilityMin += tempEffect.invokeAbilityMin * ratioAdd;	// 증폭
// 				tempEffect.invokeAbilityMax += tempEffect.invokeAbilityMax * ratioAdd;
// 			}
// 			tempEffect.invokeAbilityMin += add;	// 추가
// 			tempEffect.invokeAbilityMax += add;
// 		}
// 	}
		// 발동스킬이 있는가?
		if( pEffect->strInvokeSkill.empty() == false )
		{
			XLOGXN( "스킬발동: %s", pEffect->strInvokeSkill.c_str() );
			// 발동스킬을 액티브로 실행시킨다.
			pCaster->UseSkillByIdentifier( pEffect->strInvokeSkill.c_str(),
											this, NULL );
		}
		// 상태발동되어야 할게 있으면 버추얼로 호출시킨다.
		if( pEffect->invokeState )
		{
			SetSkillState( pCaster, pInvoker, pEffect );	// virtual
		}
/*
		// 면역체크를 해야하는 효과면 대상의 아이디를 면역리스트에 넣는다.
		if( pEffect->bImmunity && pBuffObj && bCheck )
			// 한번 발동확률검사를 거친건 면역을 건다.
			pBuffObj->AddImmunity( idObj );
	}
	*/
	// 비보정파라메터와 보정파라메터를 나눠서 버추얼로 처리한다
	int ret;
	if( (ret = ApplyEffectNotAdjParam( pCaster, pEffect )) == 0 )
	{
		return ApplyEffectAdjParam( pEffect );
	}
	return ret;
}
/*
// pInvokeTarget에게 this가 가진 버프효과를 적용시켜라
int XSkillReceiver::ApplyBuff( XBuffObj_List *plistOutApplyBuff, XSkillReceiver *pCastingTarget, XSkillReceiver *pInvokeTarget, BOOL bFirst )
{
	int numApply = 0;
//	CONSOLE("XSkillReceiver::ApplyBuff" );
//	CONSOLE("XSkillReceiver::ApplyBuff %d: addr=0x%08x tgaddr=0x%08x, Skill:%s destroy=%d", li, (int)this, (int)pInvokeTarget, pBuff->GetpDat()->GetszName(), pBuff->GetbDestroy() );
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )		// 버프리스트
	{
//		CONSOLE("XSkillReceiver::ApplyBuff %d: addr=0x%08x tgaddr=0x%08x, Skill:%s destroy=%d", li, (int)this, (int)pInvokeTarget, pBuff->GetpDat()->GetszName(), pBuff->GetbDestroy() );
		if( IsApplyBuff( pBuff, pCastingTarget, pInvokeTarget ) )		// 이 버프를 적용할수 있을지 검사한다. 디폴트는 TRUE. virtual
		{
			if( pBuff->ApplyBuff( pCastingTarget, pInvokeTarget, bFirst ) )
			{
				numApply ++;
				if( plistOutApplyBuff )
					plistOutApplyBuff->push_back( pBuff );		// 적용받은 버프를 리스트에 담음
			}
		}
	}
	END_LOOP; 
	return numApply;
}
*/
// FrameMove
int XSkillReceiver::FrameMove( float dt )
{
	// 보정치를 초기화한다
//	ClearAdjParam();
#ifdef _DEBUG
	XBREAK( IsClearAdjParam() == FALSE );
#endif
	// this가 받은 버프의 프로세스를 돈다
	XLIST2_MANUAL_LOOP( m_listSkillRecvObj, XBuffObj*, itor, pBuff )
	{
		if( pBuff->GetbDestroy() )
		{
			DestroySkillRecvObj( pBuff );		// 여기서 실제로 버프객체 삭제
//			XLOG( "id:%d %s 삭제", pBuff->GetidBuff(), pBuff->GetpDat()->GetszName() );
			m_listSkillRecvObj.Delete( itor++ );
		} else
		if( pBuff->Process( this ) == 0 )		// 버프소멸됨
		{
//			CONSOLE("XSkillReceiver:pBuff->Process==0: addr=0x%08x buffaddr=0x%08x, Skill:%s", (int)this, (int)pBuff, pBuff->GetpDat()->GetszName() );
			pBuff->SetbDestroy( TRUE );		// XUnit::Draw()에서 아직 써야하므로 당장삭제시키진 않는다.
			itor ++;
		} else 
			itor ++;
	}
	END_LOOP; 
	//
#ifdef _DEBUG
	// 클리어 검사용 값으로 클리어 시킨다.
//	ClearDebugAdjParam();
#endif
	return 1;
}

XBuffObj* XSkillReceiver::FindBuffSkill( ID idDat ) 
{
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pSkill )
	{
		if( pSkill->GetbDestroy() )	continue;
		if( pSkill->GetpDat()->GetidSkill() == idDat )
			return pSkill;
	}
	END_LOOP; 
	return NULL;
}
// 스킬id도 같고 시전자도 같은 같은 버프를 찾음
XBuffObj* XSkillReceiver::FindBuffSkill( ID idDat, XSkillUser *pCaster ) 
{
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )
	{
		if( pBuff->GetbDestroy() )	continue;
		if( pBuff->GetidSkill() == idDat && 
			pBuff->GetpCaster() == pCaster )
			return pBuff;
	} END_LOOP; 
	return NULL;
}

// 공격자 idAttacker로부터 근접공격으로 맞았다.
void XSkillReceiver::OnMeleeHitFromAttacker( ID idAttacker )
{
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )
	{
		if( pBuff->GetbDestroy() )	
			continue;
		pBuff->OnSkillEventMeleeHitFromAttacker( this, idAttacker );
	} END_LOOP;
}

// 공격자 idAttacker로부터 원거리공격으로 맞았다.
void XSkillReceiver::OnRangeHitFromAttacker( ID idAttacker )
{
/*
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )
	{
		if( pBuff->GetbDestroy() )	
			continue;
		pBuff->OnHitFromAttacker( this, idAttacker );
	} END_LOOP;
*/
}

// 방어자 idDefender를 때렸다.
void XSkillReceiver::OnAttackToDefender( ID idDefender )
{
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )
	{
		if( pBuff->GetbDestroy() )	
			continue;
		// 각 버프객체에 이벤트 전달
		pBuff->OnSkillEventAttackToDefender( this, idDefender );
	} END_LOOP;
}

void XSkillReceiver::OnSkillEventKillEnemy( ID idDead )
{
	XLIST2_LOOP( m_listSkillRecvObj, XBuffObj*, pBuff )
	{
		if( pBuff->GetbDestroy() )	
			continue;
		// 각 버프객체에 이벤트 전달
		pBuff->OnSkillEventKillEnemy( this, idDead );
	} END_LOOP;
}



NAMESPACE_XSKILL_END
