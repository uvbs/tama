#include "stdafx.h"
#include "XUnitHero.h"
#include "XPropHero.h"
#include "XHero.h"
#include "client/XAppMain.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XObjEtc.h"
#include "XSpot.h"
#include "XWndBattleField.h"
#include "XSceneBattle.h"
#include "XPropTech.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XSKILL;

////////////////////////////////////////////////////////////////
XUnitHero::XUnitHero( XSquadObj *pSquadObj, 
					XHero *pHero,
					ID idPropUnit, 
					BIT bitSide, 
					const XE::VEC3& vPos,
					float multipleAbility )
	: XBaseUnit( pSquadObj, pHero->GetidProp(), bitSide, vPos, multipleAbility ) {
	Init();
	m_pHero = pHero;
	m_pProp = pHero->GetpProp();
	XBREAK( m_pProp == NULL );
//	m_pPropUnit = PROP_UNIT->GetpProp( idPropUnit );
//	XBREAK( m_pPropUnit == NULL );
	SetTribe( m_pProp->tribe );
	XBREAK( m_pProp->tribe == 0 );
	m_timerCool.Set( 10.f );
#ifdef _XSINGLE
	m_timerCool.SetPassSec( 9.f );
#else
	m_timerCool.SetPassSec( xRandomF(10.f) );
#endif // not _XSINGLE
	///< 
// 	if( IsPlayer() )
// 		CreateAddUseSkillByIdentifier( _T( "smash" ) );
//		CreateAddUseSkillByIdentifier( _T( "shield_defense" ) );
// 	else
// 		CreateAddUseSkillByIdentifier( _T("spirit_of_wolf") );
//	CreateAddUseSkillByIdentifier( _T( "curse" ) );
//	if( pHero->GetpProp()->strActive.empty() == false )
//#ifndef _XSINGLE
	// 싱글이 아닐때만 스킬 적용함.
	if( pHero->GetSkillDatActive() )
		m_pSkillActive = CreateAddUseSkillByIdentifier( pHero->GetpProp()->strActive.c_str() );
	// 이 영웅이 군단장이냐
 	if( IsLeader() ) {
		if( pHero->GetpProp()->strPassive.empty() == false ) {
			// 스킬은 아군 전체에 적용되도록 설정되어야 함.
			m_pSkillPassive = CreateAddUseSkillByIdentifier( pHero->GetpProp()->strPassive.c_str() );
		}
	} else {
		if( pHero->GetpProp()->strPassive.empty() == false ) {
			// 스킬은 아군 전체에 적용되도록 설정되어야 함.
			m_pSkillPassive = CreateAddUseSkillByIdentifier( pHero->GetpProp()->strPassive.c_str() );
		}
	}
//#endif // not _XSINGLE
}

void XUnitHero::Destroy()
{
}

void XUnitHero::OnCreate( void )
{
	XBaseUnit::OnCreate();
}

int XUnitHero::GetSkillLevel( XSKILL::XSkillObj* pSkillObj )
{
	return XBaseUnit::GetSkillLevel( pSkillObj );
}

// float XUnitHero::GetAttackMeleePower( void )
// {
// 	XBREAK( m_pHero->GetUnit() != m_pPropUnit->idProp );
// //	int numUnits = XLegion::sCreateUnitNum( m_pHero->GetUnit(), FALSE );
// // 	int numUnits = m_pHero->GetnumUnit();
// // 
// // 	// 부대dps를 구한다.
// // 	float dpsSquad = ((m_pPropUnit->atkMelee / m_pPropUnit->atkSpeed) 
// // 						* numUnits ) * m_pHero->GetAttackMeleeRatio();
// // 	float dpsHero = dpsSquad / 3.f;
// // 	float power = dpsSquad * m_pPropUnit->atkSpeed;
// 	// 영웅은 hp만 3배뻥튀기가 되고 다른 스탯은 유닛의 능력치를 그대로 사용하는걸로 바뀜.
// 	float mulHero = m_pHero->GetAttackMeleeRatio();
// 	auto mulByLvSquad = GetmultipleAbility();
// 	float power = (m_pPropUnit->atkMelee * mulByLvSquad) * mulHero;
// 	// 버프 보정치를 더한다.
// 	// m_pPropUnit->atkMelee을 보정하는거나 power를 보정하는거나 똑같다.
// 	return CalcAdjParam( power, XGAME::xADJ_ATTACK );
// }
// float XUnitHero::GetAttackRangePower( void )
// {
// 	XBREAK( m_pHero->GetUnit() != m_pPropUnit->idProp );
// //	int numUnits = XLegion::sCreateUnitNum( m_pHero->GetUnit(), FALSE );
// // 	int numUnits = m_pHero->GetnumUnit();
// // 	// 부대dps를 구한다.
// // 	float dpsSquad = ( ( m_pPropUnit->atkRange / m_pPropUnit->atkSpeed )
// // 						* numUnits ) * m_pHero->GetAttackRangeRatio();
// // 	float dpsHero = dpsSquad / 3.f;
// // 	float power = dpsSquad * m_pPropUnit->atkSpeed;
// 	// 버프 보정치를 더한다.
// 	// m_pPropUnit->atkMelee을 보정하는거나 power를 보정하는거나 똑같다.
// 	float mulHero = m_pHero->GetAttackRangeRatio();
// 	auto mulByLvSquad = GetmultipleAbility();
// 	float power = ( m_pPropUnit->atkRange * mulByLvSquad ) * mulHero;
// 	return CalcAdjParam( power, XGAME::xADJ_ATTACK );
// }

// float XUnitHero::GetAttackMeleeDamage( UnitPtr spTarget ) 
// {
// 	return XBaseUnit::GetAttackMeleeDamage( spTarget );
// }
// 
// float XUnitHero::GetAttackRangeDamage( UnitPtr spTarget ) 
// {
// 	return XBaseUnit::GetAttackRangeDamage( spTarget );
// }

// float XUnitHero::GetSpeedMovePerSec( void )
// {
// 	float speed = m_pPropUnit->movSpeedPerSec * m_pHero->GetMoveSpeed();
// 	return CalcAdjParam( speed, XGAME::xADJ_MOVE_SPEED );
// }
// float XUnitHero::GetMovePixelPerFrame( float movPixelPerFrame )
// {
// 	float speed = movPixelPerFrame * m_pHero->GetMoveSpeed();
// 	return CalcAdjParam( speed, XGAME::xADJ_MOVE_SPEED );
// }
// float XUnitHero::GetAttackRadiusByPixel( void )
// {
// 	float radius = 0;
// 	if( IsRange() )
// 	{
// 		radius = m_pPropUnit->radiusAtkByPixel;
// 		radius = CalcAdjParam( radius, XGAME::xADJ_ATTACK_RANGE );
// 	}
// 	else
// 		radius = XGAME::DIST_MELEE_ATTACK;	// 근접유닛은 사거리 2미터로 고정
// 	// 사거리는 일단 증폭이 안되는걸로 하자
// 	return radius/* * m_pHero->GetAttackRadiusRatio()*/;	
// }
// float XUnitHero::GetSpeedAttackBase( UnitPtr *pspTarget )
// {
// 	float speed = m_pPropUnit->atkSpeed;	// 영웅의 공속은 증폭되지 않고 유닛의 공속을 그대로 사용
// // 	float addAdjRatio = 0;
// // 	float addAdjVal = 0;
// // 
// // 	if( pspTarget )
// // 	{
// // 		switch( ( *pspTarget )->GetUnitSize() )
// // 		{
// // 		case XGAME::xSIZE_SMALL: {
// // 			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_SMALL )->valPercent;
// // 		} break;
// // 		case XGAME::xSIZE_MIDDLE: {
// // 			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_MIDDLE )->valPercent;
// // 		} break;
// // 		case XGAME::xSIZE_BIG: {
// // 			addAdjRatio += GetAdjParam( XGAME::xADJ_ATTACK_SPEED_BIG )->valPercent;
// // 		} break;
// // 		}
// // 	}
// 	return CalcAdjParam( speed, XGAME::xADJ_ATTACK_SPEED/*, addAdjRatio, addAdjVal*/ );
// }
// int XUnitHero::GetMaxHp( void )
// {
// 	auto hpBase = m_pPropUnit->hpMax;
// 	auto gradeLegion = GetpLegionObj()->GetspLegion()->GetgradeLegion();
// 	if( gradeLegion == XGAME::xGL_ELITE )
// 		hpBase *= 2.f;
// 	else if( gradeLegion == XGAME::xGL_RAID )
// 		hpBase *= 4.f;
// 	int numUnits = m_pHero->GetnumUnit();
// 	float hpSquad = hpBase * m_pHero->GetHpMaxRatio() * numUnits;
// 	float hpHero = hpSquad / 2.f;		// 영웅의 hp는 부대전체 hp의 50%수준
// 	return (int)CalcAdjParam( hpHero, XGAME::xADJ_MAX_HP );
// }
// float XUnitHero::GetDefensePower( void )
// {
// 	float def = m_pPropUnit->def * m_pHero->GetDefenseRatio() * 3.f;
// 	return CalcAdjParam( def, XGAME::xADJ_DEFENSE );
// }
// float XUnitHero::GetAdjDamage( float damage )
// {
// 	return CalcAdjParam( damage, XGAME::xADJ_DAMAGE_RECV );
// }
// float XUnitHero::GetCriticalRatio( void )
// {
// 	float ratioCri = 0.f;
// 	return CalcAdjParam( ratioCri, XGAME::xADJ_CRITICAL_RATE );
// }

/**
 @brief 치명타배수
*/
// float XUnitHero::GetCriticalPower( void )
// {
// 	float power = 2.f;		// 기본 크리티컬 데미지는 두배
// 	power = CalcAdjParam( power, XGAME::xADJ_CRITICAL_POWER );
// 	XBREAK( power == 0.f );
// 	return power;
// }

/**
 @brief 회피율
*/
// float XUnitHero::GetEvadeRatio( XSKILL::xtDamage typeDamage )
// {
// 	float ratio = 0.f;
// 	return CalcAdjParam( ratio, XGAME::xADJ_EVADE_RATE );
// }

/**
 @brief 관통율
*/
// float XUnitHero::GetPenetrationRatio( void )
// {
// 	float ratio = 0.f;
// 	return CalcAdjParam( ratio, XGAME::xADJ_PENETRATION_RATE );
// }


/*
. 검기가 날아가는류의 스킬이 아닌이상 근접스킬은 평타치고 있을때 발동한다.
.스킬 쿨타임이 끝났는데 평타로 치고 있지 않은상황이면 스킬사용은 보류된다.
*/
void XUnitHero::FrameMove( float dt )
{
	auto spSelected = XWndBattleField::sGet()->GetspSelectSquad();
	bool bSelected = (spSelected && spSelected->GetsnSquadObj() == GetpSquadObj()->GetsnSquadObj());
	if( IsSkillCool() && m_timerCool.GetRemainSec() < 1.f ) {
		// 스킬이름 외치기(쿨완료 1초전에 미리 외침)
// 		if( IsPlayer() && m_bYell == false && IsLive() ) {
// 			auto pUseSkill = m_pSkillActive;
// 			if( pUseSkill == nullptr )
// 				return;
// 			if( pUseSkill->IsPassive() )
// 				return;
// 			if( IsState( XGAME::xST_SILENCE ) )
// 				return;
// 			auto v = GetvwPos();
// //			v.x -= 40.f;
// // 			v.z -= 80.f;
// 			v.z -= 100.f;
// 			_tstring str = XFORMAT( "%s!", pUseSkill->GetpDat()->GetSkillName() );
// 			auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v );
// 			AddObj( pObjText );
// 			m_bYell = true;
// 		}
	}

	if( IsAI() && !bSelected )
		// 자신에게 쓰는 버프타입이고
		if( m_pSkillActive && m_pSkillActive->GetpDat()->IsSelfBuff() )
			// 공속딜레이중이 아니며
			if( !IsNowAttackDelay() )
				// 스킬 쿨타임도 아니고 AI가 작동중일때.
				if( !IsSkillCool() && IsLive() )
					// 스킬 시작
					DoSkillMotion();
	XBaseUnit::FrameMove( dt );
}

void XUnitHero::OnDebugStr( _tstring& str )
{
#ifdef _CHEAT
// 	if( XAPP->m_bDebugViewBuff )
// 	{
// 		XLIST2_LOOP( GetlistSkillRecvObj(), XSKILL::XBuffObj*, pBuffObj )
// 		{
// 			_tstring s = XE::Format( _T( "+%s\n" ), pBuffObj->GetpDat()->GetszIdentifier() );
// 			str += s;
// 		} END_LOOP;
// 	}
#endif
}

/**
 @brief 평타공격모션이 끝났을때 호출되는 핸들러
 @todo 모션으로 판정하지 말고 평타쿨타임 시작때로 바꿔야 할듯.
*/
int XUnitHero::OnEndNormalAttackMotion( void )
{
	return 0;
}

/**
 @brief 평타 공격모션직전 호출된다.
 @return 1이 리턴되면 커스텀 공격모션이므로 FSM측에서 공격모션을 수행하지 않는다.
*/
int XUnitHero::OnBeforeAttackMotion( void )
{
	// 
	if( WND_WORLD->IsSelectedSquad( GetpSquadObj() ) )
	{
		// 스킬 수동발동 모드
		if( m_bUseSkill )
			return DoSkillMotion();
	}
	else
	{
		// 스킬 자동발동 모드
		if( IsSkillCool() == FALSE )
			return DoSkillMotion();
	}
	return 0;
}

/**
 @brief 스킬모션을 시작한다.
*/
int XUnitHero::DoSkillMotion( void )
{
	const float secCool = CalcAdjParam( 10.f, XGAME::xADJ_SKILL_COOL );
	// 스킬을 사용할 시간이 되었다.
//	XSKILL::XSkillObj *pUseSkill = GetSkillObjByIndex( 0 );
	XSKILL::XSkillObj *pUseSkill = m_pSkillActive;
	m_bUseSkill = FALSE;
	if( pUseSkill == nullptr )
		return 0;
	if( pUseSkill->IsPassive() )
		return 0;
	if( IsState( XGAME::xST_SILENCE ) )
		return 0;
	XBREAK( IsDead() );
	///< 
	int level = GetSkillLevel( pUseSkill );
	m_infoUseSkill = UseSkill( pUseSkill, level, GetspTarget().get(), nullptr );
	// 현재 동작 FSM설계에 문제가 있어서 이동중에 스킬을 사용하지 못한다.
	// 일단 스킬 모션과 관계없이 모션이 시작하면 바로 스킬을 쓰도록 바꿔두고 나중에 고친다.
	if( m_infoUseSkill.errCode == xOK ) {
		// 스킬모션이 지정되어있다면 그걸 쓰고 없다면 디폴트값을 쓴다.
		ID idAct = ACT_SKILL1;
		if( pUseSkill->GetpDat()->GetidCastMotion() ) {
			idAct = pUseSkill->GetpDat()->GetidCastMotion();
			if( GetpSprObj()->IsHaveAction( idAct ) == FALSE )
				idAct = ACT_SKILL1;
		}
		GetpSprObj()->SetAction( idAct, xRPT_1PLAY );
		SCENE_BATTLE->OnStartSkillMotion( this, secCool );

		if( m_timerCool.IsOff() )
			m_timerCool.Set( secCool );
		else
			m_timerCool.Reset();
		// 스킬도 공속딜레이를 발생시킨다.
		float secMotion = GetpSprObj()->GetPlayTime();
		StartAttackDelay( nullptr, secMotion );		// 스킬의 공속은 타겟에 영향받지 않는다.
		//
		OnShootSkill( m_infoUseSkill );
		// 스킬이름 외치기
		if( IsPlayer() ) {
			auto v = GetvwPos();
			v.z -= 80.f;
			_tstring str = XFORMAT( "%s!", pUseSkill->GetpDat()->GetSkillName() );
			auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v );
			AddObj( pObjText );
		}
		if( IsPlayer() )
			m_bYell = false;
	}

	return 1;
}

/**
 @brief 영웅의 액티브 스킬을 수동으로 사용하도록 한다.
 만약 영웅이 공격모션중이라면 모션이 끝난 후 발동된다.
*/
int XUnitHero::DoUseSkill( void )
{
	// 스킬쿨 중엔 사용이 안된다.
	if( IsSkillCool() )
		return 0;
	if( IsNowAttackDelay() )
	{
		// 공격딜레이중에는 에약만 걸어둔다.
		m_bUseSkill = TRUE;
	} else
	{
		// 공격딜레이중이 아닐때 즉시 모션이 발동된다.
		DoSkillMotion();
	}
#ifdef _DEBUG
	XSKILL::XSkillDat *pDat = m_pHero->GetSkillDatActive();
	if( pDat )
		CONSOLE("DoUseSkill:%s", pDat->GetstrIdentifier().c_str() );
#endif
	return 1;
}
/**
 @brief 스킬모션의 타점때 호출된다.
*/
void XUnitHero::OnHitEventSkill( const XE::VEC3& vwSrc )
{
/*	이동중에 스킬쓸수 있게 바꾸고 나서 다시 살릴예정
	XSKILL::XSkillObj *pUseSkill = GetSkillObjByIndex( 0 );
	if( pUseSkill )
	{
		// 하드코딩 스킬이외는 모두 시스템으로
		if( m_infoUseSkill.errCode == xOK )
			OnShootSkill( m_infoUseSkill );
	}
*/

}


void XUnitHero::OnArriveBullet( XObjBullet *pBullet,
								const UnitPtr& spAttacker,
								const UnitPtr& spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam ) 

{
	if( pBullet->GetstrIdentifier() == _T("frozen_arrow") )
	{
		XSKILL::XSkillObj *pUseSkill = GetSkillObjByIndex( 0 );
		if( pUseSkill )
		{
			int level = GetSkillLevel( pUseSkill );
			ApplyEffect( pUseSkill->GetpDat(), level, NULL );
		}
	}
	XBaseUnit::OnArriveBullet( pBullet, spAttacker, spTarget, vwDst, damage, bCritical, sprArrive, idActArrive, dwParam );
}

/**
 @brief 모션의 이벤트 타점때 호출된다.
*/
void XUnitHero::OnEventSprObj( XSprObj *pSprObj, 
								XKeyEvent *pKey, 
								float lx, float ly, 
								WORD idEvent, 
								float fAngle, 
								float fOverSec )
{
	// 
	if( GettypeCurrMeleeType() != XGAME::xMT_MELEE && 
		GettypeCurrMeleeType() != XGAME::xMT_RANGE )
	{
		// 영웅은 액티브 스킬이 있으므로 추가됨.
		auto vlActionPos = GetvlActionEvent();
		TransformByObj( &vlActionPos );
		OnHitEventSkill( GetvwPos() + vlActionPos );
	} else
    XBaseUnit::OnEventSprObj( pSprObj, pKey, lx, ly, idEvent, fAngle, fOverSec );
}

void XUnitHero::ShootRangeAttack( UnitPtr& spTarget,
								const XE::VEC3& vwSrc,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical )
{
// 	if( 궁수영웅 )
		ShootArrow( spTarget, vwSrc, vwDst, damage, bCritical );
// 	else
// 	if( 마법사영웅 )
// 	{
// 		ShootMagic( 기본데미지 );
// 	}
}

/**
 @brief 리더가 없을수도 있음.
*/
bool XUnitHero::IsLeader() 
{
	if( GetpLegionObj()->GetpLeader() )
		return GetpLegionObj()->GetpLeader()->GetidProp() == m_pHero->GetidProp();
	return false;
}

bool XUnitHero::OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster,
										XSKILL::XBuffObj *pBuffObj,
										XSKILL::XSkillDat *pSkillDat,
										XSKILL::EFFECT *pEffect,
										int level )
{
	if( pSkillDat->GetstrIdentifier() == _T( "morale" ) 
		|| pSkillDat->GetstrIdentifier() == _T( "divine_protection" ) 
		|| pSkillDat->GetstrIdentifier() == _T( "blessing" ) 
		|| pSkillDat->GetstrIdentifier() == _T( "prayer" )
		|| pSkillDat->GetstrIdentifier() == _T( "chill_blast_arrow" )
// 		|| pSkillDat->GetstrIdentifier() == _T( "chill_explosion" )
		|| pSkillDat->GetstrIdentifier() == _T( "divine_protection" )
		)
	{
		// 특성발동 외치기
//		if( pCaster->IsPlayer() )
		{
			auto pAttacker = static_cast<XBaseUnit*>( pCaster );
			auto v = pAttacker->GetvwPos();
			v.z -= 80.f;
			auto pNode = XPropTech::sGet()->GetpNodeBySkill( pAttacker->GetUnitType(), pSkillDat->GetstrIdentifier() );
			if( XASSERT(pNode) )
			{
				_tstring str = XFORMAT( "%s!", XTEXT( pNode->idName ) );
				auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v );
				AddObj( pObjText );
			}
		}
	}
	return XBaseUnit::OnEventApplyInvokeEffect( pCaster, pBuffObj, pSkillDat, pEffect, level );
}
XSKILL::xtInvokeTarget 
XUnitHero::OnGetInvokeTarget( XSKILL::XSkillDat *pDat, 
								XSKILL::EFFECT *pEffect,
								XSKILL::xtInvokeTarget invokeTarget )
{
	if( pDat->GetstrIdentifier() == _T("paralysis_arrow") )
		return invokeTarget; 
	if( IsLeader() && pDat->IsPassive() ) {
		// 아래처럼 한것은 아마 리더스킬로 발동되게 하기 위해 한것같다.
		invokeTarget = XSKILL::xIVT_ALL;
	}
	return invokeTarget;
}

// XGAME::xtSize XUnitHero::GetUnitSize( void ) 
// {
// 	return m_pHero->GetSizeUnit();	// 유닛의 사이즈를 따른다.
// //		return XGAME::xSIZE_MIDDLE;
// }
