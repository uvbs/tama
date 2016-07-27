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
#include "XUnitTreant.h"
#include "XUnitCyclops.h"
#include "skill/XSkillDat.h"

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
#ifdef WIN32
	SetstrcIds( SZ2C( m_pProp->strIdentifier ) );
#endif // WIN32
	SetTribe( m_pProp->tribe );
	XBREAK( m_pProp->tribe == 0 );
	m_pfdName = FONTMNG->Load( FONT_NANUM, 13.f );
	m_pfoName = m_pfdName->CreateFontObj();
	m_pfoName->SetStyle( xFONT::xSTYLE_STROKE );
	m_pfoName->SetColor( XCOLOR_WHITE );
	// 싱글이 아닐때만 스킬 적용함.
#if defined(WIN32) && defined(_CHEAT)
	if( !XAPP->m_bStopActive && IsCheatFiltered() )
#endif // 
	{
		if( pHero->GetSkillDatActive() )
			m_pSkillActive = CreateAddUseSkillByIdentifier( m_pProp->strActive.c_str() );
	}
	// 이 영웅이 군단장이냐
#if defined(WIN32) && defined(_CHEAT)
	if( !XAPP->m_bStopPassive && IsCheatFiltered() )
#endif // defined(WIN32) && defined(_CHEAT)
	{
		// 3성 이상일때만 적용
#ifndef _XSINGLE
		if( pHero->GetGrade() >= XGAME::xGD_RARE ) 
#endif // not _XSINGLE
		{
			if( IsLeader() ) {
				// this가 리더영웅이면 패시브가 아군전체에 적용됨
				if( m_pProp->strPassive.empty() == false ) {
					// 스킬을 useSkill로 만들어 this가 소유
					m_pSkillPassive = CreateAddUseSkillByIdentifier( m_pProp->strPassive.c_str() );
				}
			} else {
				// 리더가 아니면 개인에게만 적용됨.
				if( m_pProp->strPassive.empty() == false ) {
					m_pSkillPassive = CreateAddUseSkillByIdentifier( m_pProp->strPassive.c_str() );
				}
			}
		}
	}
//#endif // not _XSINGLE
}

void XUnitHero::Destroy()
{
	SAFE_RELEASE2( FONTMNG, m_pfdName );
	SAFE_DELETE( m_pfoName );
}

void XUnitHero::OnCreate( void )
{
	XBaseUnit::OnCreate();
}

int XUnitHero::GetSkillLevel( XSKILL::XSkillObj* pSkillObj )
{
	return XBaseUnit::GetSkillLevel( pSkillObj );
}

/*
. 검기가 날아가는류의 스킬이 아닌이상 근접스킬은 평타치고 있을때 발동한다.
.스킬 쿨타임이 끝났는데 평타로 치고 있지 않은상황이면 스킬사용은 보류된다.
*/
void XUnitHero::FrameMove( float dt )
{
#ifdef _DEBUG
	if( IsPlayer() ) {
		int a = 0;
		if( IsHero() ) {
			int b = 0;
		}
	} else {
		int a = 0;
		if( IsHero() ) {
			int b = 0;
		}
	}
#endif // _DEBUG
	auto spSelected = XWndBattleField::sGet()->GetspSelectSquad();
	bool bSelected = (spSelected && spSelected->GetsnSquadObj() == GetpSquadObj()->GetsnSquadObj());
	XBaseUnit *pTarget = (GetspTarget())? GetspTarget().get() : nullptr;
	// AI(플레이어가 컨트롤X)가 켜져있고 선택되어 있지 않을때
	if( IsAI() && !bSelected )
		// 자신에게 쓰는 버프타입이고
		if( m_pSkillActive ) {
			auto pSkillDat = m_pSkillActive->GetpDat();
			// 공속딜레이중이 아니며
				// 스킬 쿨타임도 아니고 AI가 작동중일때.
				if( !IsSkillCool() && IsLive() ) {
					float distAttack = GetAttackRadiusByPixel();
					if( pSkillDat->GetwhenCasting() == xWC_IMMEDIATELY ) {
						// 시전시점:즉시(자신에게 쓰는 버프나 힐류)
						DoSkillMotion();
					} else 
					// 시전시점: 기준타겟근접
					// 시전시점: 공격타겟근접
					if( pSkillDat->GetwhenCasting() == xWC_ATTACK_TARGET_NEAR
						|| pSkillDat->GetwhenCasting() == xWC_BASE_TARGET_NEAR ) {
						if( pTarget ) {
							float distsq = ( pTarget->GetvwPos() - GetvwPos() ).Lengthsq();
							if( distsq < distAttack * distAttack ) {
								// 스킬 시작
								DoSkillMotion();
							}
						}
					} else {
						// 시전시점: 기타.
						DoSkillMotion();	// 일단 즉시사용으로 한다.
					}
				}
		}
	XBaseUnit::FrameMove( dt );
}

void XUnitHero::OnDebugStr( _tstring& str )
{
#ifdef _CHEAT
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
	return 0;
}

/**
 @brief 스킬모션을 시작한다.
*/
int XUnitHero::DoSkillMotion( void )
{
	const float secCool = CalcAdjParam( 10.f, XGAME::xADJ_SKILL_COOL );
	// 스킬을 사용할 시간이 되었다.
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
// 		float secMotion = GetpSprObj()->GetPlayTime();
// 		StartAttackDelay( nullptr, secMotion );		// 스킬의 공속은 타겟에 영향받지 않는다.
		//
		OnShootSkill( m_infoUseSkill, 0 );
//		CONSOLE("OnShootSkill:%s", pUseSkill->GetpDat()->GetstrIdentifier().c_str() );
		// 스킬이름 외치기
		if( IsPlayer() ) {
			SCENE_BATTLE->OnUseSkill( GetpSquadObj(), pUseSkill->GetpDat()->GetSkillName() );
			auto v = GetvwPos();
			v.z -= 85.f;
			XCOLOR col = (IsPlayer())? XCOLOR_WHITE : XCOLOR_RED;
			_tstring str = XFORMAT( "%s!", pUseSkill->GetpDat()->GetSkillName() );
			auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v, col );
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
	// 적영웅이고 스킬사용금지 되어있으면 사용안함.
	if( IsEnemy() && XBaseUnit::s_bNotUseActiveByEnemy )
		return 0;
		// 공격딜레이중이 아닐때 즉시 모션이 발동된다.
		// 이제 공격딜레이와 관계없이 쿨타임 되면 스킬이 바로 발동되도록 바뀜
		DoSkillMotion();
// 	}
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
}


void XUnitHero::OnArriveBullet( XObjBullet *pBullet,
								UnitPtr spAttacker,
								UnitPtr spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam ) 

{
	if( pBullet->GetstrIdentifier() == _T("frozen_arrow") )
	{
		XSKILL::XSkillObj *pUseSkill = GetSkillObjByIndex( 0 );
		if( pUseSkill )	{
			const int level = GetSkillLevel( pUseSkill );
//			ApplyEffect( pUseSkill->GetpDat(), level, nullptr, pUseSkill->GetidSkill() );
			CastSkillToBaseTarget( pUseSkill->GetpDat(), level, spTarget.get(), XE::VEC2(), pUseSkill->GetidSkill() );
		}
	}
	XBaseUnit::OnArriveBullet( pBullet, spAttacker, spTarget, vwDst, damage, bCritical, sprArrive, idActArrive, dwParam );
}

void XUnitHero::OnEventHit( const xSpr::xEvent& event )
{
	if( IsDead() )
		return;
	// 
	if( GettypeCurrMeleeType() != XGAME::xMT_MELEE &&
		GettypeCurrMeleeType() != XGAME::xMT_RANGE ) {
		// 영웅은 액티브 스킬이 있으므로 추가됨.
		auto vlActionPos = GetvlActionEvent();
		TransformByObj( &vlActionPos );
		OnHitEventSkill( GetvwPos() + vlActionPos );
	} else {
		XBaseUnit::OnEventHit( event );
// 		XBaseUnit::OnEventSprObj( pSprObj, pKey, lx, ly, idEvent, fAngle, fOverSec );
	}
}
void XUnitHero::ShootRangeAttack( UnitPtr& spTarget,
								const XE::VEC3& vwSrc,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								const std::string& strType,
								const _tstring& strSpr )
{
	if( strType == "laser" ) {
		XUnitCyclops::sShootLaser( GetThisUnit(), spTarget, vwSrc, vwDst, damage, bCritical, strSpr );
	}	else
	if( strType == "rock" ) {
		XUnitTreant::sShootRock( GetThisUnit(), spTarget, vwSrc, vwDst, damage, bCritical, strSpr );
	}	else {
		// 특별히 지정되어있지 않으면 화살을 쏜다.
		ShootArrow( spTarget, vwSrc, vwDst, damage, bCritical );
	}
}

/**
 @brief 리더가 없을수도 있음.
*/
bool XUnitHero::IsLeader() 
{
	if( GetspLegionObj()->GetpLeader() )
		return GetspLegionObj()->GetpLeader()->GetidProp() == m_pHero->GetidProp();
	return false;
}

bool XUnitHero::OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster,
										XSKILL::XBuffObj *pBuffObj,
										XSKILL::XSkillDat *pSkillDat,
										const XSKILL::EFFECT *pEffect,
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
				XCOLOR col = ( IsPlayer() ) ? XCOLOR_WHITE : XCOLOR_RED;
				auto pObjText = new XObjYellSkill( str.c_str(), GetThisUnit(), v, col );
				AddObj( pObjText );
			}
		}
	}
	return XBaseUnit::OnEventApplyInvokeEffect( pCaster, pBuffObj, pSkillDat, pEffect, level );
}
XSKILL::xtInvokeTarget 
XUnitHero::OnGetInvokeTarget( XSKILL::XSkillDat *pDat, 
															const XSKILL::EFFECT *pEffect,
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
/**
 @brief 목표에게 향하다가 목표(객체나 좌표)에 도달함.
*/
void XUnitHero::OnArriveTarget( UnitPtr spUnit, const XE::VEC3& vwDst )
{
}

void XUnitHero::OnAISet( bool bSet )
{
	if( bSet ) {
		// AI가 작동하기 시작하면 그때부터 쿨타이머를 돌린다.
		m_timerCool.Set( 10.f );
	#ifdef _XSINGLE
		m_timerCool.SetPassSec( 10.f );
	#else
		m_timerCool.SetPassSec( xRandomF(10.f) );
	#endif // not _XSINGLE
	}
}

XE::VEC2 XUnitHero::DrawName( const XE::VEC2& vPos, float scaleFactor, float scale, const XE::VEC2& vDrawHp )
{
	auto pHero = m_pHero;
	XE::VEC2 vDrawName; // = vPos;
//	const XE::VEC2 sizeNameBar = XE::VEC2( 100 * scaleFactor * scale, 10 * scale );
//	vDrawName.x -= sizeNameBar.w * 0.5f;
	vDrawName.x = vDrawHp.x;
	vDrawName.y = vDrawHp.y - ( 3.f * scaleFactor * m_pProp->scale );
// XCOLOR col = (IsPlayer())? XCOLOR_WHITE : XCOLOR_RED;
	XCOLOR col = XGAME::GetGradeColor( m_pHero->GetGrade() );
	XBREAK( m_pfoName == nullptr );
//	m_pfoName->SetLineLength( sizeNameBar.w );
	m_pfoName->SetColor( col );
#ifdef _DEBUG
//	const _tstring str = XFORMAT( "Lv%d %s", pHero->GetLevel(), pHero->GetstrName().c_str() );
// 	if( GetpSquadObj()->IsResourceSquad() )
// 		str += _T("(R)");
#else
	const _tstring str = XFORMAT( "Lv%d %s", pHero->GetLevel(), pHero->GetstrName().c_str() );
#endif
// 	m_pfoName->DrawString( vDrawName, str );
#ifdef _DEBUG
	if( GetpSquadObj()->IsResourceSquad() ) {
		m_pfoName->DrawString( vDrawName
													 , XFORMAT( "Lv%d %s(R)", pHero->GetLevel(), pHero->GetstrName().c_str() ) );
	} 
	else 
#endif
	{
		m_pfoName->DrawString( vDrawName
													 , XFORMAT( "Lv%d %s", pHero->GetLevel(), pHero->GetstrName().c_str() ) );
	}
	return vDrawName;
}
_tstring XUnitHero::GetstrIds()
{
	return GetpPropHero()->strIdentifier;
}
