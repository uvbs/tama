#include "stdafx.h"
#include "xSkill.h"
#include "XFramework/Game/XEComponents.h"
#include "XBuffObj.h"
//using namespace XSKILL;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )

XSkillReceiver::XSkillReceiver( int maxBuff, int maxParam, int maxState )
	: XAdjParam( maxParam, maxState )
{
	Init();
//	m_listSkillRecvObj.Create( maxBuff );
}

void XSkillReceiver::Destroy() 
{
	XLIST4_DESTROY( m_listSkillRecvObj );
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
int XSkillReceiver::ApplyEffectNotAdjParam( XSkillDat *pDat, 
																						XSkillUser *pCaster, 
																						const EFFECT *pEff, 
																						int level )
{
	// 보정파라메터이거나 invalid한것은 0을 리턴
	///< 발동되는 효과를 직접 하드코딩 하고 싶을때는 발동파라메터가 보통 0이기때문에 하위에서 상속받을수 있는 이곳에서 검사하게 했다. 웒하면 상속받는쪽에서 검사안할수 있으니까
//	if( IsAdjParam( pEffect->invokeParameter ) >= 0 )	return 0;		// 함수명이 모호해서 삭제
	if( pEff->invokeParameter >= 0 )
		return 0;
//	if( pEffect->invokeParameter < 0 ) {
	float ivkAbilMin = pEff->GetAbilityMin(level);
	if( pEff->invokeAddAbility != 0.f ) {
		if( pEff->idAddAbilityToClass == 0 )
			XALERT("경고:%s:추가능력치는 있는데 추가능력치대상이 없습니다.", pDat->GetstrIdentifier().c_str() );
		if( IsInvokeAddTarget( pEff->idAddAbilityToClass ) == TRUE )
			ivkAbilMin += pEff->invokeAddAbility;
	}
	// 커스텀 추가 증폭
// 		루프( 증폭버프 = 증폭스킬리스트 )	이와같은 방식으로 시스템화.
// 		{
// 			증폭Effect = 증폭버프->GetEffectIndex(0);
// 			if( 증폭이펙트->증폭파라메터 == 능력치 )
// 			{
// 				float add = 0.f;
// //				float addMultiply = 0.f;
// 				add = 증폭이펙트->능력치[level];
// 				invokeAbilityMin += add;
// //				invokeAbilityMin = invokeAbilityMin + invokeAbilityMin * addMultiply;
// 			}
// 		}
	float add = 0.f;
	float addMultiply = 0.f;
	pCaster->OnSkillAmplifyUser( pDat, this, pEff, xEA_ABILITY, &addMultiply, &add );
	ivkAbilMin += add;
	ivkAbilMin = ivkAbilMin + ivkAbilMin * addMultiply;
	// 능력치값을 다른 다른형태로 변형해서 사용하고 싶다면 아래 핸들러를 정의해서 바꾼다.
	pCaster->OnAdjustEffectAbility( pDat, pEff, pEff->invokeParameter, &ivkAbilMin );
	//
	return OnApplyEffectNotAdjParam( pCaster, pDat, pEff, ivkAbilMin );
//	}
// 	return 0;
}
/**
 @brief 보정 파라메터의 값을 보정한다.
*/
int XSkillReceiver::ApplyEffectAdjParam( XSkillDat *pSkillDat, 
																				XSkillUser *pCaster,
																				const EFFECT *pEffect, 
																				int level,
																				XBuffObj *pBuffObj )
{
	if( pEffect->invokeParameter <= 0 || pEffect->invokeParameter >= XGAME::xMAX_PARAM )	
		return 0;
	XBREAKF( IsAdjParam( pEffect->invokeParameter ) < 0, 
		"처리하지 못한 비보정파라메터(%d)가 있습니다.", pEffect->invokeParameter );
	if( pEffect->invokeAbilityMin.size() == 1 )
		level = 0;
	float ability = pEffect->invokeAbilityMin[ level ];
	// 능력치값을 다른 다른형태로 변형해서 사용하고 싶다면 아래 핸들러를 정의해서 바꾼다.
	pCaster->OnAdjustEffectAbility( pSkillDat, pEffect, pEffect->invokeParameter, &ability );
	XBREAK( pBuffObj == nullptr );
	if( pBuffObj && pBuffObj->IsFirstProcess() ) {
		OnApplyEffectAdjParam( pCaster, pSkillDat, pEffect, ability );
	}
	AddAdjParamMsg( pEffect->invokeParameter
						, pEffect->valtypeInvokeAbility
						, ability );
	//

	return 1;
}


/**
 @brief	this에게 발동효과 적용
 xApply
*/
int XSkillReceiver::ApplyInvokeEffect( XSkillDat *pSkillDat,
																			 XSkillUser *pCaster,
																			 XSkillReceiver* pInvoker,
																			 XBuffObj *pBuffObj,
																			 const EFFECT *pEffect,
																			 int level )
{
	bool bApplied = false;
	// 발동조건스킬이 지정되어있을땐 그 스킬(버프)을 인보커가 가지고 있는지 검사해야 한다.
	if( pEffect->strInvokeIfHaveBuff.empty() == false )
		if( !pInvoker->FindBuffSkill( pEffect->strInvokeIfHaveBuff.c_str() ) )
			return 0;
	/*
	// 효과가 적용될때마다 이벤트를 발생시킨다.
	// DoT형태의 경우 DoT시점마다 호출되면 만약 발동효과가 2개인경우는 DoT때마다 두번씩 호출된다.
	*/
	if( !OnEventApplyInvokeEffect( pCaster, pBuffObj, pSkillDat, pEffect, level ) )	// virtual
		return 0;		// 사용자가 0을 리턴하면 효과 적용시키지 않는다.
	// 상태발동되어야 할게 있으면 버추얼로 호출시킨다.
	if( pEffect->invokeState ) {
		if( OnApplyState( pCaster, 
											pInvoker, 
											pEffect, 
											pEffect->invokeState, 
											IsState(pEffect->invokeState) ) == FALSE ) {	// virtual
			// 상태이상을 저항하면 효과를 적용하지 않는다.
			return 0;	
		}
		SetStateMsg( pEffect->invokeState, true );
		if( pBuffObj && pBuffObj->IsFirstProcess() ) {
			OnFirstApplyState( pCaster, 
												pInvoker, 
												pEffect, 
												pEffect->invokeState, 
												IsState(pEffect->invokeState),
												level );	// virtual
			bApplied = true;
		}
	} // invokeState
	// 발동스킬이 있는가?
	if( !pEffect->strInvokeSkill.empty()) {
		const ID idCallerBuff = (pBuffObj)? pBuffObj->GetidSkill() : 0;
		// this에게 스킬을 발동스킨다.
		bApplied = DoInvokeSkill( this, XE::VEC2(), pSkillDat, pEffect, level, pCaster, idCallerBuff );
// 		_tstring strInvokeSkillMut = pEffect->strInvokeSkill;
// 		// strInvokeSkill이 바뀔수 있음.
// 		bool bInvoke = pInvoker->OnInvokeSkill( pSkillDat, pEffect, this, level, &strInvokeSkillMut );
// 		if( bInvoke ) {
// #ifdef _XSINGLE
// //			XLOGXN( "스킬발동: %s", pEffect->strInvokeSkill.c_str() );
// #endif // _XSINGLE
// 			// 발동스킬을 실행시킨다. 기준타겟은 this로 된다.
// 			// 발동스킬이 m_listUseSkill에 들어가는 문제가 있어 안들어가도록 수정함.
// 			auto infoUseSkill 
// 				= pCaster->UseSkillByIds( strInvokeSkillMut, // 이게 맞는듯. pEffect->strInvokeSkill,
// 																	level,
// 																	this, nullptr );
// 			XASSERT( infoUseSkill.errCode == xOK /*|| infoUseSkill.errCode == xERR_ALREADY_APPLY_SKILL*/ );
// 			const ID idCallerBuff = (pBuffObj)? pBuffObj->GetidSkill() : 0;
// 			if( infoUseSkill.errCode == xOK ) {
// 				// 발동스킬은 지속형일수도 있고 즉시발동형일수도 있다.
// 				pCaster->OnShootSkill( infoUseSkill, idCallerBuff );
// 				bApplied = true;
// 			}
// 		}
	}
	// 효과발동자(시전대상)에게 이 스킬로 부터 발동될 스킬이 있으면 발생시킨다.
	pInvoker->OnEventInvokeFromSkill( pSkillDat, pEffect, pCaster, this );
	// 비보정파라메터와 보정파라메터를 나눠서 버추얼로 처리한다
	if( pEffect->invokeParameter < 0 ) {
		return ApplyEffectNotAdjParam( pSkillDat, pCaster, pEffect, level );
	} else 
	if( pEffect->invokeParameter > 0 ) {
		return ApplyEffectAdjParam( pSkillDat, pCaster, pEffect, level, pBuffObj );
	}
	return bApplied;
}

/**
 @brief pEff의 발동스킬을 발동대상(혹은 발동좌표)에 사용한다.
 this는 발동자.
 @param pIvkTarget 발동대상. 발동대상이 좌표형이면 null이다
 @param vIvkPos 발동대상이 좌표형태일때 IzNotZero가 된다. 
*/
bool XSkillReceiver::DoInvokeSkill( XSkillReceiver* pIvkTarget,
																		const XE::VEC2& vIvkPos, 
																		const XSkillDat* pDat, 
																		const EFFECT* pEffect, 
																		int level, 
																		XSkillUser* pCaster,
																		ID idCallerBuff )
{
	// 좌표에 값이 있으면 타겟은 널이어야 한다.,
	XBREAK( vIvkPos.IsNotZero() && pIvkTarget != nullptr );
	bool bApplied = false;
	auto pInvoker = this;		// this는 발동자
	// 발동스킬이 있는가?
	_tstring strInvokeSkillMut = pEffect->strInvokeSkill;
	XBREAK( strInvokeSkillMut.empty() );
	// strInvokeSkill이 바뀔수 있음.
	bool bInvoke = (pIvkTarget)? pInvoker->OnInvokeSkill( pDat, pEffect, pIvkTarget, level, &strInvokeSkillMut ) : true;
	if( bInvoke ) {
#ifdef _XSINGLE
	XTRACE( "스킬발동: %s", pEffect->strInvokeSkill.c_str() );
#endif // _XSINGLE
		// 발동스킬을 실행시킨다. 기준타겟은 pIvkTarget로 된다.
		// 발동스킬이 m_listUseSkill에 들어가는 문제가 있어 안들어가도록 수정함.
		auto infoUseSkill = pCaster->UseSkillByIds( strInvokeSkillMut,
																								level, 
																								pIvkTarget, vIvkPos );
		XASSERT( infoUseSkill.errCode == xOK );
		if( infoUseSkill.errCode == xOK ) {
			// 발동스킬은 지속형일수도 있고 즉시발동형일수도 있다.
			pCaster->OnShootSkill( infoUseSkill, idCallerBuff );
			bApplied = true;
		}
	}
	return bApplied;
}

/**
 @brief 
*/
int XSkillReceiver::FrameMove( float dt )
{
#ifdef _DEBUG
// 	XBREAK( IsClearAdjParam() == FALSE );
#endif
	m_pAttacker = nullptr;		// 이것은 휘발성으로 매 프레임 초기화 된다.
	//
	for( auto itor = m_listSkillRecvObj.begin(); itor != m_listSkillRecvObj.end(); ) {
		auto pBuff = (*itor);
		if( pBuff->GetbDestroy() ) {
			SAFE_DELETE( pBuff );
			m_listSkillRecvObj.erase( itor++ );
		} else
		if( pBuff->Process( this ) == 0 ) {		// 버프소멸됨
			pBuff->SetbDestroy( TRUE );		// XUnit::Draw()에서 아직 써야하므로 당장삭제시키진 않는다.
			++itor;
		} else {
			++itor;
		}
	}
	//
	return 1;
}

/**
 @brief // this에 pSkillRecvObj버프가 걸려있는지 확인
*/
bool XSkillReceiver::FindBuff( XBuffObj *pSkillRecvObj ) const
{		
	for( auto pBuffObj : m_listSkillRecvObj ) {
		if( pBuffObj->GetidBuff() == pSkillRecvObj->GetidBuff() )
			return true;
	}
	return false;
// 	return m_listSkillRecvObj.Find( pSkillRecvObj );
}

XBuffObj* XSkillReceiver::FindBuffSkill( ID idSkillDat ) 
{
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )	
			continue;
		if( pBuff->GetpDat()->GetidSkill() == idSkillDat )
			return pBuff;
	}
	return nullptr;
}
// 스킬id도 같고 시전자도 같은 같은 버프를 찾음
XBuffObj* XSkillReceiver::FindBuffSkill( ID idSkillDat, XSkillUser *pCaster ) 
{
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )	
			continue;
		if( pBuff->GetidSkill() == idSkillDat 
				&& pBuff->GetpCaster() == pCaster ) {
			return pBuff;
		}
	}
	return nullptr;
}

XBuffObj* XSkillReceiver::FindBuffSkill( LPCTSTR idsSkill )
{
	auto pDat = XESkillMng::sGet()->FindByIds( idsSkill );
	XBREAK( pDat == nullptr );
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )	
			continue;
		if( pBuff->GetpDat()->GetidSkill() == pDat->GetidSkill() )
			return pBuff;
	}
	return nullptr;
}
/**
 @brief 공격자 idAttacker로부터 근접공격으로 맞았다.
*/
void XSkillReceiver::OnHitFromAttacker( const XSkillReceiver *pAttacker, xtDamage typeDamage )
{
	OnAttackMelee( pAttacker, typeDamage );	// virtual
	// 피격자가 가진 버프중에 맞으면 발동하는 스킬이 있으면 이벤트핸들러를 날려준다.
	for( auto pBuff : m_listSkillRecvObj )	{
		if( pBuff->GetbDestroy() )	
			continue;
		pBuff->OnHitFromAttacker( pAttacker, this, this, typeDamage );
	}
}

// 공격자 idAttacker로부터 원거리공격으로 맞았다.
void XSkillReceiver::OnRangeHitFromAttacker( ID idAttacker, int level )
{
}

/**
 @brief this가 방어자 pDefender에게 공격했다. 
 @note 주의: 실제 방어자에게 데미지가 들어갔을때가 아님. 공격자가 공격을 시도한시점임.
*/
void XSkillReceiver::OnAttackToDefender( XSkillReceiver *pDefender, 
										float damage, 
										BOOL bCritical, 
										float ratioPenetration, 
										XSKILL::xtDamage typeDamage )
{
	// 공격자의 버프중에서 타격시 발동하는 이벤트가 있다면 발동시킨다.
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )	
			continue;
		// 각 버프객체에 이벤트 전달
		pBuff->OnAttackToDefender( pDefender, damage, bCritical, ratioPenetration, typeDamage );
	}
}

/**
 @brief 범용 이벤트 전달기
 @return 효과가 하나라도 적용되었다면 true를 리턴한다.
 **/
bool XSkillReceiver::OnEventJunctureCommon( ID idEvent, DWORD dwParam, const XSkillReceiver* pRecvParam )
{
	bool bApplied = false;
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )
			continue;
		if( pBuff->OnEventJunctureCommon( idEvent, dwParam, pRecvParam ) )
			bApplied = true;
	}
	return bApplied;
}

/**
 @brief pFromEffect가 발동되어서 발생한 이벤트.
 @param pBaseTarget pFromSkill을 맞은(invoke)대상
*/
void XSkillReceiver::OnEventInvokeFromSkill( XSkillDat *pFromSkill
																						, const EFFECT *pFromEffect
																						, XSkillUser *pCaster
																						, XSkillReceiver *pBaseTarget )
{
	// 피격자가 가진 버프중에 맞으면 발동하는 스킬이 있으면 이벤트핸들러를 날려준다.
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )
			continue;
		pBuff->OnEventInvokeFromSkill( pFromSkill, pFromEffect, pCaster, pBaseTarget );
	}
}


/**
 @brief 평타공격전(모션발생직후)에 호출되는 이벤트.
 junc에는 원거리인지 근접인지 정보가 온다.
*/
// void XSkillReceiver::OnEventBeforeAttack( XSKILL::xtJuncture junc )
// {
// 	// 피격자가 가진 버프중에 맞으면 발동하는 스킬이 있으면 이벤트핸들러를 날려준다.
// 	for( auto pBuff : m_listSkillRecvObj ) {
// 		if( pBuff->GetbDestroy() )
// 			continue;
// 		pBuff->OnEventBeforeAttack( junc );
// 	}
// }

void XSkillReceiver::OnSkillEventKillEnemy( ID idDead )
{
	for( auto pBuff : m_listSkillRecvObj ) {
		if( pBuff->GetbDestroy() )
			continue;
		// 각 버프객체에 이벤트 전달
		pBuff->OnSkillEventKillEnemy( this, idDead );
	}
}

/**
 @brief this에게 붙는 이펙트를 만드는 공통 함수.
 @param secPlay 0:once 0>:해당시간동안 루핑 -1:무한루핑
*/
ID XSkillReceiver::CreateSfx( const XSkillDat *pSkillDat,
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
	return OnCreateSkillSfx( pSkillDat,
													 pointSfx,
													 strEffect.c_str(),
													 idAct,
													 secPlay, vPos );
}

/**
 @brief this에게 붙는 이펙트를 만드는 공통 함수.
 @param secPlay 0:once 0>:해당시간동안 루핑 -1:무한루핑
*/
ID XSkillReceiver::CreateSfx( const XSkillDat *pSkillDat
															, const xEffSfx& effSfx
															, float secPlay
															, const XE::VEC2& vPos )
{
	return CreateSfx( pSkillDat, effSfx.m_strSpr, effSfx.m_idAct, effSfx.m_Point, secPlay, vPos );
}

XE_NAMESPACE_END
