#include "stdafx.h"
#include "XSquadObj.h"
#include "XLegionObj.h"
#include "XLegion.h"
#include "XHero.h"
#include "XBaseUnit.h"
#include "XWndBattleField.h"
#include "XPropUnit.h"
#include "XPropHero.h"
#include "XFSMUnit.h"
#include "XBattleField.h"
#include "XSkillMng.h"
#include "XAccount.h"
#include "XGame.h"
#include "XPropTech.h"
#include "XSquadron.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif
#include "XSoundMng.h"
#include "XMsgUnit.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace XSKILL;

#define LIVE_UNIT_LOOP( PSPUNIT ) {		\
	for( auto& spUnit : m_listUnit ) { \
		if( !spUnit->IsLive() ) \
			continue;


////////////////////////////////////////////////////////////////
XSquadObj::XSquadObj( XSPLegionObj spLegionObj, 
						const XSquadron *pSquad, 
						const XE::VEC3& vwPos )
	: XSquadObj( spLegionObj, pSquad->GetpHero(), vwPos )
{
	m_pSquadron = const_cast<XSquadron*>( pSquad );
}

XSquadObj::XSquadObj( XSPLegionObj spLegionObj, 
						XHero *pHero, 
						const XE::VEC3& vwPos )
{
	Init();
//	m_pSquadron = pSquad;
	m_spLegionObj = spLegionObj;
	XBREAK( pHero == nullptr );
	m_pHero = pHero;
	m_vwPos = vwPos;
	m_pProp = PROP_UNIT->GetpProp( m_pHero->GetUnit() );
	XBREAK( m_pProp == NULL );
	m_Radius = 150.f;
	switch( m_pProp->size ) {
	case XGAME::xSIZE_SMALL:	m_Radius = 180.f;	break;
	case XGAME::xSIZE_MIDDLE:	m_Radius = 180.f;	break;
	case XGAME::xSIZE_BIG:		m_Radius = 180.f;	break;
	}
	if( IsRange() == FALSE )
		m_bMeleeMode = TRUE;	// 근접전 부대는 무조건 TRUE
	if( m_pProp->GetTypeUnit() == XGAME::xUNIT_PALADIN ) {
		// 팔라딘-돌파 특성레벨
		auto pNode = XPropTech::sGet()->GetpNodeBySkill( XGAME::xUNIT_PALADIN, _T( "breakthrough" ) );
		m_lvBreakThrough = pHero->GetLevelAbil( XGAME::xUNIT_PALADIN, pNode->idNode );
		if( m_lvBreakThrough > 0 ) {
			m_pBreakThrough = SKILL_MNG->FindByIdentifier(_T("breakthrough"));
			XBREAK( m_pBreakThrough == nullptr );
		}
		pNode = XPropTech::sGet()->GetpNodeBySkill( XGAME::xUNIT_PALADIN, _T( "flame_knight" ) );
		m_lvFlameKnight = pHero->GetLevelAbil( XGAME::xUNIT_PALADIN, pNode->idNode );
		if( m_lvFlameKnight >= 0 ) {
			m_pFlameKnight = SKILL_MNG->FindByIdentifier(_T("flame_knight"));
			XBREAK( m_pFlameKnight == nullptr );
		}
		pNode = XPropTech::sGet()->GetpNodeBySkill( XGAME::xUNIT_PALADIN, _T( "courage" ) );
		m_lvCourage = pHero->GetLevelAbil( XGAME::xUNIT_PALADIN, pNode->idNode );
		if( m_lvCourage >= 0 ) {
			m_pCourage = SKILL_MNG->FindByIdentifier(_T("courage"));
			XBREAK( m_pCourage == nullptr );
		}
	}
}

void XSquadObj::Destroy()
{
}

void XSquadObj::Release()
{
	m_spTarget.reset();
	//m_listUnit.clear();
	for( auto spUnit : m_listUnit )
		spUnit->Release();
	m_listUnit.Clear();
	m_spHeroUnit.reset();
	m_spTargetForCmd.reset();
	m_spBleedingTarget.reset();
	m_spLegionObj.reset();
	m_listAttackMe.clear();
//	m_spManualTarget.reset();
}

const XECompCamp& XSquadObj::GetCamp() const
{
	return m_spLegionObj->GetCamp();
}

/// 디버깅용. 살아있는 유닛수를 센다.
int XSquadObj::GetNumLives()
{
	int cnt = 0;
	LIVE_UNIT_LOOP( pspUnit )
	{
		++cnt;
	} END_LOOP;
	return cnt;
}

XE::VEC2 XSquadObj::GetDistUnit( XPropUnit::xPROP *pProp )
{
	XE::VEC2 vDist;
	if( pProp->idProp == XGAME::xUNIT_PALADIN )
		return vDist = XE::VEC2( xMETER_TO_PIXEL(4.f), xMETER_TO_PIXEL(2.f) );
	switch( pProp->size )
	{
	case XGAME::xSIZE_SMALL:	vDist = XE::VEC2( xMETER_TO_PIXEL(2.6f), xMETER_TO_PIXEL(1.4f) );		break;
	case XGAME::xSIZE_MIDDLE:	vDist = XE::VEC2( xMETER_TO_PIXEL(5.4f), xMETER_TO_PIXEL(4.0f) );	break;
	case XGAME::xSIZE_BIG:		vDist = XE::VEC2( xMETER_TO_PIXEL(10.0f), xMETER_TO_PIXEL(8.0f) );	break;
// 	case XGAME::xSIZE_SMALL:	vDist = XE::VEC2( 39, 19 );		break;
// 	case XGAME::xSIZE_MIDDLE:	vDist = XE::VEC2( 50, 30 ) * 1.5f;	break;
// 	case XGAME::xSIZE_BIG:		vDist = XE::VEC2( 70, 40 ) * 2.5f;	break;
	}
	return vDist;
}

int XSquadObj::GetRow( XPropUnit::xPROP *pProp )
{
	int row = 5;
	switch( pProp->size )
	{
	case XGAME::xSIZE_SMALL:	row = 5;		break;
	case XGAME::xSIZE_MIDDLE:	row = 2;	break;
	case XGAME::xSIZE_BIG:		row = 2;	break;
	}
	return row;
}

XE::VEC2 XSquadObj::GetDistHero( XPropUnit::xPROP *pProp )
{
	XE::VEC2 vDist;
	switch( pProp->size )
	{
	case XGAME::xSIZE_SMALL:	vDist = XE::VEC2( 39, 0 );		break;
	case XGAME::xSIZE_MIDDLE:	vDist= XE::VEC2( 52, 0 );	break;
	case XGAME::xSIZE_BIG:		vDist = XE::VEC2( 75, 0 );	break;
	}
	return vDist;
}

/**
 @brief 영웅 생성
*/
void XSquadObj::CreateHero( XWndBattleField *pWndWorld, 
							XPropUnit::xPROP *pProp, 
							const XECompCamp& camp )
{
	XE::VEC3 v = m_vwPos;
	XE::VEC2 vDistHero = GetDistHero( pProp );
	if( camp == XGAME::xSIDE_PLAYER )
		v += vDistHero;
	else
		v -= vDistHero;
	auto mulByLvSquad = PROP_SQUAD->GetMulByLvSquad( pProp->size, 0, m_pHero->GetlevelSquad() );
//	auto pPropHero = PROP_HERO->GetpProp( m_pHero->GetstrIdentifer() );
	auto pUnit = XBaseUnit::sCreateHero( GetThis(), 
												m_pHero,
												pProp->idProp,
												camp.GetbitCamp(),
												v, 
												mulByLvSquad );
	auto spUnit = AddUnit( pUnit );
	pWndWorld->GetpWorld()->AddObj( spUnit );
	m_spHeroUnit = spUnit;
	int a = 0;
}

/**
 @brief 유닛부대 생성
*/
void XSquadObj::CreateUnit( XWndBattleField *pWndWorld,  
							XPropUnit::xPROP *pProp, 
							int numUnit, 
							const XECompCamp& camp )
{
	XE::VEC2 vDist = GetDistUnit( pProp );
	XE::VEC2 vDistHero = GetDistHero( pProp );
	int row = GetRow( pProp );	// 수평으로 몇명씩 서는지
	int col = numUnit / row;	// 5명씩 몇줄로 생기는지
	int mod = numUnit % row;
	XE::VEC3 vStart = m_vwPos;
//	const auto& propSquad = PROP_SQUAD->GetTable( m_pHero->GetlevelSquad() );
	float mulByLvSquad = PROP_SQUAD->GetAvgMulByLvSquad( pProp->size, m_pHero->GetlevelSquad() );
	int idxUnit = 0;
	if( col == 0 )
		vStart.y -= ( mod - 1 ) * ( vDist.y / 2.f );
	else
		vStart.y -= ( row - 1 ) * ( vDist.y / 2.f );
	for( int i = 0; i < col; ++i ) {
		for( int j = 0; j < row; ++j ) {
			XE::VEC3 vCurr;
			if( camp == XGAME::xSIDE_PLAYER )
				vCurr.x = vStart.x - vDist.w * i;
			else
				vCurr.x = vStart.x + vDist.w * i;
			vCurr.y = vStart.y + vDist.h * j;
			CreateAndAddUnit( pProp, /*propSquad,*/ pWndWorld, idxUnit, camp, vCurr, mulByLvSquad );
			++idxUnit;
		}
	}
	for( int j = 0; j < mod; ++j ) {
		XE::VEC3 vCurr;
		if( camp == XGAME::xSIDE_PLAYER )
			vCurr.x = vStart.x - vDist.w * col;
		else
			vCurr.x = vStart.x + vDist.w * col;
		vCurr.y = vStart.y + vDist.h * j;
		CreateAndAddUnit( pProp, /*propSquad,*/ pWndWorld, idxUnit, camp, vCurr, mulByLvSquad );
		++idxUnit;
	}
}

/**
 @brief 유닛을 생성하고 리스트에 add시키는 공통 작업
 @param 이제 유닛 개별로 배수를 지정하지 않고 전체 유닛수 평균으로 한다.
*/
void XSquadObj::CreateAndAddUnit( XPropUnit::xPROP *pProp
																, XWndBattleField *pWndWorld
																, int idxUnit
																, const XECompCamp& camp
																, const XE::VEC3& vCurr
																, float mulByLvSquad )
{
	// 부대레벨에 따른 능력치 배수를 넣음.
// 	float mulByLvSquad = PROP_SQUAD->GetMulByLvSquad( pProp->size, idxUnit, m_pHero->GetlevelSquad() );
	auto pUnit = XBaseUnit::sCreateUnit( GetThis(), pProp->idProp,
																			camp.GetbitCamp(),
																			vCurr, mulByLvSquad );

	UnitPtr spUnit = AddUnit( pUnit );
	pWndWorld->GetpWorld()->AddObj( spUnit );
}
/**
 m_pSquad를 바탕으로 대장과 유닛들을 생성한다.
*/
BOOL XSquadObj::CreateSquad( XWndBattleField *pWndWorld, const XECompCamp& camp )
{
	auto pProp = PROP_UNIT->GetpProp( m_pHero->GetUnit() );
	int numUnit = m_pHero->GetnumUnit();

	// 대장 유닛
	XE::VEC3 v = m_vwPos;
	CreateUnit( pWndWorld, pProp, numUnit, camp );
	CreateHero( pWndWorld, pProp, camp );

	return TRUE;
}
#ifdef _CHEAT
BOOL XSquadObj::CreateSquadsDebug( XWndBattleField *pWndWorld, 
								XECompCamp& camp )
{
	auto pProp = PROP_UNIT->GetpProp( GetpHero()->GetUnit() );
	XBREAK( GetpHero()->GetpProp()->IsRange() && pProp->IsRange() == FALSE );
	int numUnit = m_pHero->GetnumUnit();
// 	if( camp == XGAME::xSIDE_PLAYER )
// 		numUnit = DEBUG_NUM1;
// 	else
// 		numUnit = DEBUG_NUM2;
// #if defined(_XSINGLE)
// 	numUnit = 1;
// #endif 
	// 대장 유닛
	CreateUnit( pWndWorld, pProp, numUnit, camp );
	if( 1 )	// 테스트에선 대장은 생성하지 않음.
		CreateHero( pWndWorld, pProp, camp );
	return TRUE;
}
#endif // DEBUG
UnitPtr XSquadObj::AddUnit( XBaseUnit *pUnit ) 
{
//	pUnit->SetScaleObj( pUnit->GetPropScale() );	// XBaseUnit내부로 옮김
	AddCntLive( 1 );
// 	XGAME::xtSize size = pUnit->GetUnitSize();
// 	XBREAK( size == XGAME::xSIZE_SMALL && m_cntLive > XGAME::MAX_UNIT_SMALL );
// 	XBREAK( size == XGAME::xSIZE_MIDDLE && m_cntLive > XGAME::MAX_UNIT_MIDDLE );
// 	XBREAK( size == XGAME::xSIZE_BIG && m_cntLive > XGAME::MAX_UNIT_BIG );
	UnitPtr spUnit( pUnit );
	m_listUnit.Add( spUnit );
	return spUnit;
}

/**
 유닛들의 AI를 켜거나 끈다.
*/
void XSquadObj::SetAI( BOOL bFlag )
{
	if( !bFlag ) {
		// AI가 꺼질때 루핑되던소리 있으면 꺼줌.
		if( m_idStreamByRun ) {
			SOUNDMNG->StopSound( m_idStreamByRun );
		}
	}
//  		SOUNDMNG->StopAllSound();
	for( auto& spUnit : m_listUnit ) {
		spUnit->SetAI( bFlag );
	}
	if( bFlag )
		// this와 가장가까운 적부대를 찾는다.
		DoAttackAutoTargetEnemy();

}

/**
 @brief 가까운 적부대를 찾아서 공격을 시작한다.
*/
XSPSquad XSquadObj::DoAttackAutoTargetEnemy()
{
	XSPSquad spTarget = XBattleField::sGet()->FindNearSquadEnemy( this );
//	DoMoveTo( spTarget );
	if( spTarget != nullptr )
		SetCmdRequest( xCMD_ATTACK_TARGET, spTarget );
	return spTarget;
}

/**
 @brief 외부에서 이 부대에게 무브모드로 바꿔달라는 요청이 온다.
*/
void XSquadObj::DoRequestMoveMode()
{
	SetCmdRequest( xCMD_CHANGE_MOVEMODE );
}

/**
 @brief 범용 이벤트 전달기
*/
void XSquadObj::OnSkillEvent( XSKILL::xtJuncture event )
{
	for( auto& spUnit : m_listUnit ) {
		if( !spUnit->IsLive() ) {
			continue;
		}
		spUnit->OnEventBySkillUser( event );
	}
}

void XSquadObj::OnStartBattle()
{
	SetAI( TRUE );
	///< 
	LIVE_UNIT_LOOP( spUnit ) {
		spUnit->OnStartBattle();
	} END_LOOP;
#ifdef _XSINGLE
	m_timerDie.Set(0);
	m_secDie = 0;
#endif // _XSINGLE
}

void XSquadObj::OnAfterStartBattle()
{
	SetAI( TRUE );
	///< 
	LIVE_UNIT_LOOP( pspUnit )	{
		spUnit->OnAfterStartBattle();
	} END_LOOP;
}

/**
 @brief 라이칸 전용 프로세스
*/
void XSquadObj::ProcessLycan( float dt )
{
	// 수동이동이 아닐때
	if( m_bManualMoving )
		return;
	if( GetUnit() != XGAME::xUNIT_LYCAN )
		return;
	if( m_timerHeal.IsOff() )
		m_timerHeal.Set( 1.f );
	if( !m_timerHeal.IsOver() )
		return;
	m_timerHeal.Reset();
	// 야비한 특성 스킬이 있을때는 1초마다 한번씩 출혈에 걸린적을 찾는다.
	int point = m_pHero->GetLevelAbil( GetUnit(), _T( "scurrilous_attack" ) );
	if( point <= 0 )
		return;
	// 출혈걸린 타겟이 없거나, 타겟을 추적중이거나 공격중인데 타겟이 죽으면 새로 찾음.
	if( m_spBleedingTarget != nullptr && !m_spBleedingTarget->IsLive() )
		m_spBleedingTarget = nullptr;
	if( m_spBleedingTarget == nullptr ) {
		// 출혈에 걸린적을 찾는다.
		m_spBleedingTarget = GetEnemyLegion()->FindNearSquad( this,
			[]( XSPSquad& spSquad )->bool {
			if( spSquad && spSquad->IsState( XGAME::xST_BLEEDING ) ) {
				return true;
			}
			return false;
		} );
		// 찾은 목표부대로 이동하게 한다. 만약 못찾았다면 현재자리에서 Idle로 들어간다.
		if( m_spBleedingTarget != nullptr ) {
			DoMoveTo( m_spBleedingTarget );
			// 만약 이동모드가 예약되었다면 취소시킴.
			SetCmdRequest( xCMD_CHANGE_MOVEMODE );
// 			m_bRequestMoveModeChange = FALSE;
			// 여기서 취소시켰으나 곧바로 다시 명령이 들어와서 타겟이 바뀔가능성 있음.
		}
	}
}

/**
 @brief 외부에서 들어와 예약된 명령 처리기
*/
void XSquadObj::ProcessCmd()
{
	// 이동모드로 바꿔주길 요청이 들어옴
//	if( m_bRequestMoveModeChange )
	// 명령 처리기
	if( m_cmdRequest ) {
		if( m_cmdRequest == xCMD_CHANGE_MOVEMODE ) {
			// 이동모드 바꾸라는 명령이 왔을때 출혈타겟이 없거나 있어도 죽은거면 명령을 수행함.
			if( m_spBleedingTarget == nullptr || 
				(m_spBleedingTarget != nullptr && !m_spBleedingTarget->IsLive()) ) {
				// 월드에게 새 타겟을 찾아달라고 요청함
				XSPSquad spTargetSquad = XBattleField::sGet()->FindNearSquadEnemy( this );
				// 찾은 목표부대로 이동하게 한다. 만약 못찾았다면 현재자리에서 Idle로 들어간다.
				DoMoveTo( spTargetSquad );
			}
		} else
		if( m_cmdRequest == xCMD_ATTACK_TARGET ) {
			XBREAK( m_spTargetForCmd == nullptr );
			DoMoveTo( m_spTargetForCmd );
		} else
		if( m_cmdRequest == xCMD_MOVE_POS ) {
			XBREAK( m_vDstForCmd.IsZero() );
			DoMoveTo( m_vDstForCmd );
		}
		SetCmdRequest( xCMD_NONE );
	}
}
/**
 @brief 부대가 이동할때 처리를 해준다.
*/
void XSquadObj::FrameMove( float dt )
{
	ProcessLycan( dt );
	// 외부에서 들어온 명령 처리기
	ProcessCmd();
	if( m_bMove ) {
		float speedMultiply = 1.f;
		// 스피드부대의 경우 적근접부대곁을 지나가면 속도가 느려진다.
		if( IsSpeed()) {
			// GetNearSquad내에서 IsTanker까지 하면 더 최적화 가능.
			XArrayLinearN<XSPSquad,64> ary;
			bool bSlow = false;
			XBattleField::sGet()->GetNearSquadEnemy( this, &ary, m_Radius / 2.f );
			XARRAYLINEARN_LOOP_REF( ary, XSPSquad, spSquad ) {
				if( spSquad->IsTanker() ) {
					bSlow = true;
					break;
				}
			} END_LOOP;
			if( bSlow ) {
				if( m_pBreakThrough )
					HardcodingBreakthrough( speedMultiply, ary );
				else
					speedMultiply = 0.5f;	// 탱커부대와 스쳐지나면 속도가 절반으로 준다.
			}
		}
		BOOL bArrive = FALSE;
		if( m_spTarget != nullptr ) {
			m_vwTarget = m_spTarget->GetvwPos();	// 타겟이 있을때는 계속 갱신해줘야 한다.
			float distsq = (m_vwTarget - GetvwPos()).Lengthsq();
			// 근처까지 다가갔는지 검사
			float sizeRadius = GetDistAttack();
			if( distsq <= sizeRadius * sizeRadius ) {
				// 타겟이 있을때 타겟 근처에 도달함.
				bArrive = TRUE;
				// 도착후 행동은 아래 if( bArrive )에 할것.
			}
		} else {
			// 타겟이 없는데 목표좌표도 없는건 에러
			XBREAK( m_vwTarget.IsZero() || m_vwTarget.IsMinus() );
		}
		if( bArrive == FALSE ) {
			// 이번턴에 이동량을 계산한다.
			XE::VEC3 vDelta;
			// 현재 살아있는 유닛들의 평균 이동속도,
			float speedAvg = GetAvgSpeedUnits();
			float speedMove = speedAvg * speedMultiply;
			speedMove *= dt;
			bArrive = XBaseUnit::sGetMoveDstDelta( GetvwPos(), 
												m_vwTarget, 
												speedMove, 
												&vDelta );
			AddPos( vDelta );
		}
		//////////////////////////////////////////////////////////////////////////
		if( bArrive ) {
			// bArrive == TRUE, 도착했다
			m_bManualMoving = FALSE;
			m_vwTarget.Set( 0 );	// 도착했으므로 목표좌표 클리어
//			CONSOLE("부대도착:%s", m_pProp->strName.c_str() );
			if( m_idStreamByRun ) {
				SOUNDMNG->StopSound( m_idStreamByRun );
			}
			// 각부대 싸우는 소리 루핑시작.
			switch( GetUnit() ) {
			case xUNIT_PALADIN:
				SOUNDMNG->OpenPlaySound( 32 );
				break;
			case xUNIT_MINOTAUR:
				SOUNDMNG->OpenPlaySound( 35, FALSE );
				break;
			case xUNIT_LYCAN:
				SOUNDMNG->OpenPlaySound( 34 );				
				break;
			default:
				break;
			}
			// 유닛 개별적으로 타겟을 잡고 추적하도록 한다.
			if( m_spTarget != nullptr ) {
				// 타겟이 부대일경우는 공격했다는 이벤트를 날림
				m_spTarget->OnAttacked( GetThis() );
				// 이미 타겟이 지정되어있었으면 그 타겟을 공격한다.
				DoAllUnitsChase( m_spTarget );
			} else {
				//좌표로 도착했으면 가까운 적을 찾아 공격한다.
				auto spTarget = XBattleField::sGet()->FindNearSquadEnemy( this );
				DoMoveTo( spTarget );
			}

		}
	// m_bMove(부대이동중)
	} else {
	// 맞붙어 싸우는중.
		// 현재 부대중심을 실제 유닛들이 모여있는곳으로 한다. 부대끼리 맞붙고 나서 서로 떨어지는 거리를 측정하기위해 좀더 자연스럽값으로 하기위함.
		if( m_spTarget != nullptr || m_vwTarget.IsHave() ) {
			// 어떤타겟이라도 있어야 갱신함.
			XE::VEC3 vwCenter = GetvCenterByUnits();
			// 중심좌표가 0이면 갱신하면 안된다. 부대원이 없다는뜻임
			if( vwCenter.IsZero() == FALSE )
				m_vwPos = vwCenter;
		}
		// m_bMove == FALSE. 부대정지 상태
		if( IsRange() ) {
			// 원거리 모드인가
			if( m_bMeleeMode == FALSE ) {
				// 타겟부대와 붙었는가
// 				if( m_spTarget != nullptr && IsNear( m_spTarget.get() ) ) {
				if( m_spTarget != nullptr && GetNumAttackMeByMelee() > 0 ) {
					bool bCounterAttack = true;
					if( m_bInoreCounterAttack == false ) {
						LPCTSTR szAbil = _T( "concentration" );
						float prob = GetInvokeRatioAbil( szAbil, XGAME::xUNIT_CYCLOPS );
						if( prob > 0 && XE::IsTakeChance( prob ) ) {
							bCounterAttack = false;
							m_bInoreCounterAttack = true;	// 반격무시상태 시작
							CONSOLE("반격무시 발동");
						}
					} else
						bCounterAttack = false;
					if( bCounterAttack ) {
						m_bMeleeMode = TRUE;	// 근접전 모드로 전환
						// 텔레포트 하드코딩
						if( m_spHeroUnit ) {
							auto pBuff = m_spHeroUnit->FindBuffSkill( _T( "teleport" ) );
							if( pBuff ) 
								DoTeleport();
						}
#ifdef _XSINGLE
						if( XAPP->m_bDebugSquad )
							CONSOLE( "근접전 모드로 전환" );
#endif
						// 여기서는 플래그만 세팅하고 실제 동작전환은 각 유닛들이 현재 하던 공격동작을 끝내고 바꾸도록 한다.
					}
				}
			} else {
			// 근접전 모드인가.
				// 원거리 부대의 경우 타겟부대가 멀어지면 다시 부대단위 추적모드로 바꿈
//				if( m_spTarget != nullptr && IsNear( m_spTarget.get() ) == FALSE ) {
				if( m_spTarget != nullptr ) {
					if( GetNumAttackMeByMelee() == 0 ) {		// 더이상 날 공격중인 근접부대가 없으면 원거리모드 해제
						m_bMeleeMode = FALSE;
#ifdef _XSINGLE
						if( XAPP->m_bDebugSquad )
							CONSOLE( "원거리 모드로 전환" );
#endif // _XSINGLE
					}
				} else {
					if( m_spHeroUnit ) {
						auto pBuff = m_spHeroUnit->FindBuffSkill( _T( "teleport" ) );
						if( pBuff )
							DoTeleport();
					}
				}
			} 
		// IsRange()
		} else {
		// IsNotRange()
			if( m_spTarget != nullptr ) {
				// 타겟부대가 다시 멀어지는지 감지
				m_vwTarget = m_spTarget->GetvwPos();	// 타겟이 있을때는 계속 갱신해줘야 한다.
				float distsq = ( m_vwTarget - GetvwPos() ).Lengthsq();
				// 부대간 공격유효거리를 벗어났는가
				float sizeRadius = GetDistAttack();
				sizeRadius *= 2;	// 다시 멀어졌음을 판단할때는 서로의 원이 완전히 겹치지 않아야 한다..
				if( distsq > sizeRadius * sizeRadius ) {
					m_spTarget->OnAttackLeave( GetThis() );
					DoMoveTo( m_spTarget );
				}
			}
		}
	// not m_bMove
	}
	// 하드코딩 프로세스
	ProcessHardcode( dt );
}

void XSquadObj::ProcessHardcode( float dt )
{
	if( m_pFlameKnight && IsLive() ) {
		if( m_timerBreakThrough.IsOff() )
			m_timerBreakThrough.Set( 1.f );
		if( m_timerBreakThrough.IsOver() ) {
			m_timerBreakThrough.Reset();
			XArrayLinearN<XSPSquad, 64> ary;
			XBattleField::sGet()->GetNearSquadEnemy( this, &ary, m_Radius / 2.f );
			auto pEffect = m_pFlameKnight->GetEffectByIdx( 0 );
			float abil = pEffect->invokeAbilityMin[ m_lvFlameKnight ];
			float damage = GetAttackMeleeDamage();
			XBREAK( abil == 0 );
			damage *= abil;
			XARRAYLINEARN_LOOP_AUTO( ary, spSquad ) {
				spSquad->DoDamage( damage, false );
			} END_LOOP;
		}
	}
	// 용기
	if( m_pCourage && IsLive() )
	{
		XArrayLinearN<XSPSquad, 64> ary;
		XBattleField::sGet()->GetNearSquad( this, &ary, m_Radius / 2.f );
		auto pEffect = m_pCourage->GetEffectByIdx( 0 );
		float abil = pEffect->invokeAbilityMin[ m_lvCourage ];
		if( ary.size() > 0 )
		{
			XBREAK( pEffect->invokeAbilityMin[ m_lvCourage ] == 0 );
			auto& spSquad = ary[ 0 ];
			spSquad->AddAdjParam( ( XGAME::xtParameter )pEffect->invokeParameter,
				pEffect->valtypeInvokeAbility,
				pEffect->invokeAbilityMin[ m_lvCourage ] );
			if( m_timerHeal.IsOff() )
				m_timerHeal.Set( 1.f );
			if( m_timerHeal.IsOver() )
			{
				m_timerHeal.Reset();
				// 안수치료 특성이 있는지 확인
				LPCTSTR szAbil = _T( "ordination_treatment" );
				int point = GetLevelByAbil( XGAME::xUNIT_PALADIN, szAbil );
				// 				auto pNode = XPropTech::sGet()->GetpNodeBySkill( XGAME::xUNIT_PALADIN, 
				// 															_T("ordination_treatment"));
				// 				if( XASSERT(pNode) )
				// 				{
				// 					auto pAbil = GetAccount()->GetAbilNode( XGAME::xUNIT_PALADIN, pNode->idNode );
				if( point > 0 )
				{
					auto pSkillDat = SKILL_MNG->FindByIdentifier( szAbil );
					if( XASSERT( pSkillDat ) )
					{
						auto pEffect = pSkillDat->GetEffectByIdx( 0 );
						XBREAK( pEffect == nullptr );
						float adjVal = pEffect->invokeAbilityMin[ point ];
						// 각 부대원들에게 adjVal만큼씩 힐링하도록 한다.,.
						spSquad->DoHealByPercent( adjVal );
					}
				}
				//				}
			}
		}
	} // 용기 특성
}

/**
 @brief sid특성이 있는지 검사해서 레벨을 돌려준다.
 없다면 0을 리턴한다.
*/
int XSquadObj::GetLevelByAbil( XGAME::xtUnit unit, LPCTSTR sid )
{
	// 이 부대가 해당특성의 부대가 아니면 리턴
	if( GetUnit() != unit )
		return 0;
	auto pNode = XPropTech::sGet()->GetpNodeBySkill( unit, sid);
	if( XASSERT(pNode) ) {
		const auto abil = m_pHero->GetAbilNode( unit, pNode->idNode );
		if( abil.point < 0 )
			return 0;
		return abil.point;
	}
	return 0;
}

/**
 @brief 현재 부대의 유닛이 unit일경우 idAbil특성의 레벨을 돌려준다.
*/
int XSquadObj::GetLevelByAbil( XGAME::xtUnit unit, ID idAbil )
{
	// 이 부대가 해당특성의 부대가 아니면 리턴
	if( GetUnit() != unit )
		return 0;
	const auto abil = m_pHero->GetAbilNode( unit, idAbil );
// 	if( XBREAK( pAbil == nullptr ) )
// 		return 0;
	return abil.point;
}

/**
 @brief 이 스킬(특성)의 발동확률을 얻는다.
*/
float XSquadObj::GetInvokeRatioAbil( LPCTSTR sid, XGAME::xtUnit unit )
{
	auto pSkillDat = SKILL_MNG->FindByIdentifier( sid );
	if( pSkillDat == nullptr )
		return 0;
	int level = 0;
	if( pSkillDat->IsAbility() ) {
		// 특성
		XBREAK( unit == XGAME::xUNIT_NONE );
		level = GetLevelByAbil( unit, sid );
	} else
	if( pSkillDat->IsPassive() ) {
		// 패시브
		level = m_pHero->GetlvPassive();
	} else
	if( pSkillDat->IsActive() ) {
		level = m_pHero->GetlvActive();
	} else {
		XBREAK(1);
	}
	if( level == 0 )
		return 0;
	XBREAK( pSkillDat->GetlistEffects().size() != 1 );
	auto pEffect = pSkillDat->GetEffectByIdx( 0 );
	XBREAK( pEffect == nullptr );
	XBREAK( level > 0 && level < XGAME::MAX_SKILL_LEVEL - 1 );
	return pEffect->aryInvokeRatio[ level ];
}
/**
 @brief unit1과 unit2사이의 중간점을 기준으로 서서싸울 위치를 정한다.
*/
void XSquadObj::sCalcBattlePos( const UnitPtr& unit1, const UnitPtr& unit2 )
{
//	XBREAK( unit1->IsRange() );
	XE::VEC2 v1 = unit1->GetvwPos().ToVec2();
	XE::VEC2 v2 = unit2->GetvwPos().ToVec2();
	XE::VEC2 vDist = v2 - v1;
	XE::VEC2 vCenter = v1 + (vDist / 2.f);	// 둘간의 중간지점
	XE::VEC2 vLocal = unit2->GetAttackedPos( unit1 );
	vCenter.x += (64 - random(128));
	vCenter.y += (64 - random(128));
	float secArrive = 1.0f;		// 바인드 위치까지 뛰어갈시간을 정함.
	int numFrame = (int)( XFPS * secArrive );	// 프레임수로 환산
	if( unit2->IsBindTarget() ) {
		// 타겟이 이미 바인딩되어있다면 중심점 기준으로 하지말고 타겟기준으로 서도록 한다.
		XBREAK( unit1->IsRange() && unit1->GettypeCurrMeleeType() == XGAME::xMT_RANGE );		// 레인지 유닛은 바인딩 하지 않음.
		unit1->SetvwBind( unit2->GetvwPos().GetXY() + vLocal );
		float dist = ( unit1->GetvwBind() - unit1->GetvwPos().GetXY() ).Length();
		unit1->SetspeedBind( dist / numFrame );
#ifdef _DEBUG
		if( unit1->IsRange() ) {
			// 원거리 유닛이 바인드좌표랑 너무 멀리떨어져있으면
			XE::VEC3 vwBind = unit1->GetvwBind();
			float distsq = ( vwBind - unit1->GetvwPos() ).Lengthsq();
			if( distsq > 400.f * 400.f ) {
				XBREAK( 1 );
			}
		}
#endif
	} else {
		unit1->SetvwBind( vCenter + vLocal / 2.f);		// 1번측 좌표를 고정시킨다.
#ifdef _DEBUG
		// 원거리 유닛이 바인드좌표랑 너무 멀리떨어져있으면
		if( unit1->IsRange() ) {
			XE::VEC3 vwBind = unit1->GetvwBind();
			float distsq = ( vwBind - unit1->GetvwPos() ).Lengthsq();
			if( distsq > 400.f * 400.f ) {
				XBREAK(1);
			}
		}
#endif // _DEBUG
		// 2번측이 좌표가 고정되지 않았다면 좌표를 고정시킨다.
		// unit2의 좌표가 이미 고정되었다면 다시 바인드 시키지 않는데 만약 그 상대가 unit1이라면 좌표를 다시 잡아서 고정시킨다.
		if( unit2->GetpSquadObj()->IsStop() && unit2->GetpSquadObj()->GetbMeleeMode() )	{// 상대 유닛이 근접전 모드상태여야 한다.
			if( unit2->GetvwBind().IsMinus() == TRUE ||
				unit2->GetidTarget() == unit1->GetsnObj() ) {
				unit2->SetvwBind( vCenter - vLocal / 2.f );
#ifdef _DEBUG
				// 원거리 유닛이 바인드좌표랑 너무 멀리떨어져있으면
				if( unit1->IsRange() ) {
					XE::VEC3 vwBind = unit2->GetvwBind();
					float distsq = ( vwBind - unit2->GetvwPos() ).Lengthsq();
					if( distsq > 400.f * 400.f ) {
						XBREAK( 1 );
					}
				}
#endif // _DEBUG
				// 현재위치에서 바인딩좌표까지의 거리를 잼
				float dist = ( unit1->GetvwBind() - unit1->GetvwPos().ToVec2() ).Length();
				unit1->SetspeedBind( dist / numFrame );
				dist = ( unit2->GetvwBind() - unit2->GetvwPos().ToVec2() ).Length();
				unit2->SetspeedBind( dist / numFrame );
				// unit2의 대상은 unit1이 되도록 함께 맞추고 unit2는 따로 타겟을 검색하지 않는다.
				unit2->DoChaseAndAttack( unit1 );
			}
		}
	}
}

/**
 @brief 현재 타겟부대에서 새로운 목표를 하나 찾아서 돌려줌
*/
UnitPtr XSquadObj::GetNewTargetInTargetSquad( BOOL bIncludeHero )
{
	return m_spTarget->FindAttackTarget( bIncludeHero );
}

/**
 @brief 목표부대로 이동시켜라.
*/
void XSquadObj::DoMoveTo( XSPSquad spTarget )
{
	if( m_spTarget && ( ( spTarget && m_spTarget->getid() != spTarget->getid() ) || spTarget == nullptr ) )
		m_spTarget->OnAttackLeave( GetThis() );		// 기존에 공격받던 부대에게 이벤트.
	m_spTarget = spTarget;
	XBREAK( IsRange() == FALSE && m_bMeleeMode == FALSE );
	if( spTarget != nullptr )	{
		DoMoveTo( spTarget->GetvwPos() );
	} else {
		m_bMove = FALSE;
		// 현재 추적할 부대가 없으므로 제자리에서 대기를 명령
		LIVE_UNIT_LOOP( spUnit ) {
			spUnit->DoIdle();
		} END_LOOP;
	}
}

/**
 @brief 목표위치로 부대를 이동시켜라
*/
void XSquadObj::DoMoveTo( const XE::VEC3& vwDst )
{
	if( m_bMove ) {
		int a = 0;
	}
	// 각부대 달리는 소리 루핑시작.
	switch( GetUnit() ) {
	case xUNIT_PALADIN:
		if( m_idStreamByRun == 0 )
			m_idStreamByRun = SOUNDMNG->OpenPlaySound( 30, TRUE );
		break;
	case xUNIT_LYCAN:
		SOUNDMNG->OpenPlaySound( 44, FALSE );
		break;
	default:
		break;
	}
	m_bMove = TRUE;
	m_vwTarget = vwDst;
	// 각 유닛들에게 목표위치로 이동하도록 명령.
	LIVE_UNIT_LOOP( spUnit ) {
		// 이동위치는 부대목표위치 + 유닛로컬위치
		XE::VEC3 v = vwDst + spUnit->GetvLocalFromSquad();
		spUnit->DoChase( v );
	} END_LOOP;
}

/**
 @brief this부대와 가장가까운 부대를 찾는다.
*/
XSPSquad XSquadObj::FindAttackSquad()
{
	return m_spLegionObj->FindNearSquad( this );
}

/**
 @brief 타겟카운팅이 가장적은유닛을 찾는다.
*/
UnitPtr XSquadObj::FindAttackTarget( BOOL bIncludeHero )
{
	UnitPtr *pspCompObj = nullptr;
	UnitPtr *pspHero = nullptr;
//	std::list<UnitPtr>::iterator itor;
	LIVE_UNIT_LOOP( spUnit ) {
		// 타겟카운팅이 가장 적은 유닛을 찾아낸다.
		if( pspCompObj == nullptr ||
			spUnit->GetcntTargeting() < (*pspCompObj)->GetcntTargeting() ) {
			if( spUnit->IsState(XGAME::xST_INVISIBLE) == FALSE ) {
				// 영웅포함해서 검색하는 조건일때
				if( bIncludeHero ) {
					// 조건없이 바로 선택한다.
					pspCompObj = &spUnit;
				} else {
					// 영웅빼고 검색하는 조건이면
					// 영웅이 아닐때만 선택함.
					if( spUnit->IsHero() == FALSE )
						pspCompObj = &spUnit;
					else
						pspHero = &spUnit;		// 영웅은 별도로 받아둔다.
				}
			}
		}
	} END_LOOP;
	// 결과를 못찾았을때
	if( pspCompObj == nullptr ) {
		// 영웅빼고검색조건에서 영웅찾아놓은거 있으면 그걸 리턴
		if( bIncludeHero == FALSE && pspHero )
			return (*pspHero);
		// 없으면 널리턴
		return UnitPtr();
	}
	return (*pspCompObj);
}

/**
 @brief 이 부대의 멤버 pUnit이 죽었다.
*/
void XSquadObj::OnDieMember( XBaseUnit *pUnit )
{
	int cntOld = m_spLegionObj->GetcntLive();
	XBREAK( pUnit->GetpSquadObj()->GetsnSquadObj() != GetsnSquadObj() );
//	BOOL bSuccess = m_listUnit.DelByID( pUnit->GetsnObj() );
	UnitPtr *pspFind = m_listUnit.FindpByID( pUnit->GetsnObj() );
	XBREAKF( pspFind == NULL, "not found squad member:unit_id=%d, sn=0x%08x", pUnit->GetidProp(), pUnit->GetsnObj() );
	if( pspFind ) {
		int oldCntLive = m_cntLive;
		AddCntLive( -1 );
//		XALERT("헬로우:%s",_T("테스트"));
		if( XBREAK( m_cntLive != GetNumLives() ) ) {
			// 보통 이미 죽은 대상에게 다시 DoDamage()를 불러 다시 죽인 경우다.
			// 주로 DoDamage()내부의 스킬이벤트에서 죽으면 이런현상이 나타남.
			int numLive = GetNumLives();
			int b = 0;
		}
		if( pUnit->IsHero() )
			m_spHeroUnit.reset();	// 죽은게 영웅이었으면 shared_ptr도 지워줌
		if( m_cntLive == 0 ) {
			m_spLegionObj->OnDieSuqad( GetThis() );
			for( auto& spUnit : m_listUnit ) {
				XBREAK( spUnit->IsLive() );
			}
		}
	}

}

/**
 @brief 분대내 살아있는 유닛의 리스트를 ary에 담아준다.
*/
int XSquadObj::GetListMember( XVector<XSKILL::XSkillReceiver*> *pAry )
{
	LIVE_UNIT_LOOP( spUnit )	{
		pAry->Add( spUnit.get() );
	} END_LOOP;
	return pAry->size();
}

int XSquadObj::GetListMember( XVector<XBaseUnit*> *pAry )
{
	LIVE_UNIT_LOOP( spUnit )	{
		pAry->Add( spUnit.get() );
	} END_LOOP;
	return pAry->size();
}


/**
 @brief 살아있는 유닛한명을 리턴해준다.
*/
XBaseUnit* XSquadObj::GetLiveMember()
{
	LIVE_UNIT_LOOP( spUnit )	{
		return spUnit.get();
	} END_LOOP;
	return NULL;
}

/**
 @brief spTarget부대와 인접해 있는가
*/
BOOL XSquadObj::IsNear( XSquadObj *pTarget )
{
	XE::VEC3 vDist = pTarget->GetvwPos() - GetvwPos();
	vDist.Abs();
	float sizeRadius = m_Radius;
	if( vDist.Lengthsq() <= sizeRadius * sizeRadius )
		return TRUE;
	return FALSE;
}

/**
 @brief unit을 위해 적절한 타겟을 찾아준다.
 만약 unit이 영웅이라면 상대영웅을 우선으로 찾고
 영웅이 아니라면 일반병사를 우선으로 찾도록 한다.
*/
UnitPtr XSquadObj::GetAttackTargetForUnit( const UnitPtr& unit )
{
//	XBREAK( m_spTarget == nullptr );
	if( m_spTarget == nullptr )
		return UnitPtr();
	UnitPtr spUnitTarget;
	// 모든 영웅과 병사는 상대의 병사들부터 먼저 찾는다.
	{
		BOOL bIncludeHero = FALSE;		// 영웅을 우선순위에서 뒤로한다.
		spUnitTarget = m_spTarget->FindAttackTarget( bIncludeHero );
	}
	return spUnitTarget;
}

/**
 @brief 모든 부대원들에게 개별타겟을 찾아 공격하라고 알림
*/
void XSquadObj::DoAllUnitsChase( XSPSquad spTarget )
{
	m_spTarget = spTarget;
	if( IsRange() == FALSE )
		m_bMeleeMode = TRUE;	// 근접전 부대는 무조건 TRUE
	m_bMove = FALSE;
	
	// 각 유닛들 바인딩좌표 계산
	LIVE_UNIT_LOOP( pspUnit )	{
		if( spUnit->IsBindTarget() == FALSE )	{// 이미 바인드 되어있으면 타겟을 검색하지 않는다.
			// 타겟부대내에서 개별타겟을 찾음 
			XBREAK( m_spTarget == nullptr );
			UnitPtr spUnitTarget = GetAttackTargetForUnit( spUnit );
			if( spUnitTarget == nullptr ) {
				// 적당한 타겟을 못찾았을땐 
				BOOL bFindSuccess = spUnit->RequestNewMission();
				if( bFindSuccess == FALSE )
					// 유닛루프 종료
					break;
			} else {
				if( spUnit->IsRange() == FALSE ) {
					// 근접유닛은 바인드 좌표로 이동하게 한다.
					BOOL bDash = TRUE;
					spUnit->DoAttackTargetByBind( spUnitTarget, bDash );
				} else {
					// 원거리유닛은 일반 추적을 한다.
					XBREAK( spUnit->IsBindTarget() );
					spUnit->DoChaseAndAttack( spUnitTarget );
					XBREAK( spUnit->GetspTarget() == nullptr );
				}
			}
		}
	} END_LOOP;
}

/**
 @brief spAttacker부대로부터 공격받기시작함.
*/
void XSquadObj::OnAttacked( const XSPSquad spAttacker )
{
	XBREAK( spAttacker == nullptr );
	if( spAttacker->IsRange() )		// 공격자가 원거리유닛이면 반격안함.
		return;
	// this를 공격하는 부대를 리스트로 받아둠.
	AddAttackMe( spAttacker );

	// 이미 치고있던 부대가 공격자부대면 반격할일 없음
	if( m_spTarget != nullptr 
		&& m_spTarget->GetsnSquadObj() == spAttacker->GetsnSquadObj() )		
		return;
#ifdef _DEBUG
// 	CONSOLE( "%s:부대 근접공격받음:%s", m_pHero->GetstrName().c_str()
// 																	,	spAttacker->GetpHero()->GetstrName().c_str() );
#endif // _DEBUG
	// 공격자가 근접전 부대
	// 수동이동중이었으면 반격하지 않는다.
	if( m_bManualMoving )
		return;
	// 다른 타겟부대를 향해 뛰고 있었으나 중간에 다른적부대에게 공격당함.
	if( m_bMove )	{
		// 우리부대가 원거리부대라면 그냥 하던거함
		if( IsRange() )		
			return;
		// 반격. this는 근접부대
		if( m_spTarget && m_spTarget->getid() != spAttacker->getid() )
			m_spTarget->OnAttackLeave( GetThis() );		// 기존에 공격받던 부대에게 이벤트.
		DoAllUnitsChase( spAttacker );
	} else	{
		// 부대 이동중이 아닐때
		bool bCounterAttack = true;
		if( IsRange() ) {
#ifdef _DEBUG
			float distsq = ( spAttacker->GetvwPos() - m_vwPos ).Lengthsq();
			if( distsq > 200.f * 200.f ) {
				XBREAK(1);
			}
#endif // _DEBUG
			/*
			// 레인지부대가 공격당해 반격을 했다면 짧은시간(약1~2초)동안 다시 강제로 타겟을 바꿀수 없게 한다.
			// 원거리가 덮쳐도 수동으로 다시 타겟을 바꿔버리면 소용없으므로.
			// 원거리가 근접에게 공격당하면 1~2초간 혼란에 빠져서 원거리공격을 할수 없다는 설정이면 될듯.
			*/
			// 이곳은 원거리가 A로부터 근접공격을 당했을때 원거리가 B를 타겟으로 잡고 있었다면 들어오는곳이다.
			// 이미 원거리가 A를 공격하던중이면 이곳이 아니고 FrameMove의 m_bMove == false인곳에서 처리한다.
			// 사이클롭스 집중
			if( GetUnit() == xUNIT_CYCLOPS ) {
				LPCTSTR szAbil = _T( "concentration" );
				float prob = GetInvokeRatioAbil( szAbil, XGAME::xUNIT_CYCLOPS );
				if( prob > 0 && XE::IsTakeChance( prob ) )
					bCounterAttack = false;
			}
			// 반격
			if( bCounterAttack  ) {
				if( m_spTarget && m_spTarget->getid() != spAttacker->getid() )
					m_spTarget->OnAttackLeave( GetThis() );		// 기존에 공격받던 부대에게 이벤트.
				DoAllUnitsChase( spAttacker );
			}
		} else {
		// 근접부대
			// 반격
			if( bCounterAttack && m_spTarget == nullptr ) {
				if( m_spTarget && m_spTarget->getid() != spAttacker->getid() )
					m_spTarget->OnAttackLeave( GetThis() );		// 기존에 공격받던 부대에게 이벤트.
				// attacker를 공격
				DoAllUnitsChase( spAttacker );
			}
		}
	}
}

/**
 @brief this를 공격하던 spAttacker가 나에대한 타겟을 풀었다.
*/
void XSquadObj::OnAttackLeave( XSPSquad spAttacker )
{
	DelAttackMe( spAttacker );
	// this가 원거리부대일경우 다시 this를 공격중인 리스트를 검색해서 melee류가 하나도 없으면 원거리 공격모드로 전환.
	if( IsRange() && m_bMeleeMode ) {
		bool bAttackedMelee = false;
		for( auto spSquad : m_listAttackMe ) {
			if( spSquad->IsMelee() )
				bAttackedMelee = true;
		}
		if( !bAttackedMelee ) {
			// 근접부대로부터 공격받고 있지 않으면 근접전모드 해제
		}
	}
}

/**
 @brief 부대원들의 서있는 위치를 기반으로 부대의 중심점을 계산한다.
*/
XE::VEC3 XSquadObj::GetvCenterByUnits()
{
	int cnt = 0;
	XE::VEC3 vSum;
	LIVE_UNIT_LOOP( pspUnit )
	{
		vSum += spUnit->GetvwPos();
		++cnt;
	} END_LOOP
		if( cnt == 0 )
			return XE::VEC2(0);
	return vSum / (float)cnt;;
}

/**
 @brief spTarget부대를 강제공격시킨다. 
 명령받은 부대는 도착해서 해당부대가 전멸할때까지 공격대상을 바꾸지 않는다.
 단, 도발 및 상대스킬이나 특성에의한 타겟변경은 있을 수 있다.
*/
void XSquadObj::DoAttackSquad( const XSPSquad& spTarget )
{
	// 강제지정부대를 설정.
	m_bManualMoving = TRUE;
//	m_bRequestMoveModeChange = FALSE;
	SetCmdRequest( xCMD_ATTACK_TARGET, spTarget );
//	m_bMeleeMode = FALSE;
	// 타겟부대로 이동명령
//	DoMoveTo( spTarget );
}

/**
 @brief vwDst좌표로 강제이동시킨다.
*/
void XSquadObj::DoMoveSquad( const XE::VEC3& vwDst )
{
	m_spTarget.reset();		// 기존에 타겟이 잡혀있었으면 해제시킨다.
	m_bManualMoving = TRUE;
	SetCmdRequest( xCMD_MOVE_POS, vwDst );
//	m_bRequestMoveModeChange = FALSE;
//	m_bMeleeMode = FALSE;
//	DoMoveTo( vwDst );
}

/**
 @brief 부대간이동시 얼마나 근접해야 공격을 시작하는가
*/
float XSquadObj::GetDistAttack()
{
	// 근처까지 다가갔는지 검사
	float sizeRadius = m_Radius;	// 상대부대에게 얼만큼 근접할건가
	// 원거리의 경우 원거리사거리까지 더해준다.
	if( m_pProp->typeAtk == XGAME::xAT_RANGE ) {
		// 영웅이있으면 영웅의 사거리를 가져와 더한다. 없다면 기본값으로 사용한다.
		if( m_spHeroUnit != nullptr ) {
			// 부대가 원거린데 영웅이 원거리가 아닌경우
			XBREAK( m_spHeroUnit->IsRange() == FALSE );
			sizeRadius += m_spHeroUnit->GetAttackRadiusByPixel();
		} else
			sizeRadius += m_pProp->radiusAtkByPixel;
	}
	return sizeRadius;
}

/**
 @brief 부대원 전체에게 절대치의 데미지를 준다.
*/
void XSquadObj::DoDamage( float damage, BOOL bCritical )
{
	LIVE_UNIT_LOOP( spUnit ) {
		BIT bitHit = XGAME::xBHT_HIT;
		if( bCritical )
			bitHit |= XGAME::xBHT_CRITICAL;
		if( damage == 0 )
			bitHit &= ~XGAME::xBHT_HIT;
//		spUnit->DoDamage( NULL, damage, -1, XSKILL::xDMG_NONE, bitHit, XGAME::xDA_NONE );
		auto spMsg = std::make_shared<xnUnit::XMsgDmg>( nullptr
																								, spUnit
																								, damage
																								, -1.f
																								, xDMG_NONE
																								, bitHit
																								, xDA_NONE );
		spUnit->PushMsg( spMsg );
	} END_LOOP;
}

/**
 @brief 부대원 전체에게 maxhp에 따른 비율로 데미지를 준다.
*/
void XSquadObj::DoDamageByPercent( float ratio, BOOL bCritical )
{
	LIVE_UNIT_LOOP( spUnit ) {
		float maxHp = (float)spUnit->GetMaxHp();
		float damage = maxHp * ratio;
		BIT bitHit = XGAME::xBHT_HIT;
		if( bCritical )
			bitHit |= XGAME::xBHT_CRITICAL;
		if( damage == 0 )
			bitHit &= ~XGAME::xBHT_HIT;
//		spUnit->DoDamage( NULL, damage, -1, XSKILL::xDMG_NONE, bitHit, XGAME::xDA_NONE );
		auto pMsg = std::make_shared<xnUnit::XMsgDmg>( nullptr
																								, spUnit
																								, damage
																								, -1.f
																								, xDMG_NONE
																								, bitHit
																								, xDA_NONE );
		spUnit->PushMsg( pMsg );
	} END_LOOP;
}

void XSquadObj::AddAdjParam( XGAME::xtParameter adjParam, XSKILL::xtValType valType, float adj )
{
	LIVE_UNIT_LOOP( spUnit )
	{
		spUnit->AddAdjParam( adjParam, valType, adj );
	} END_LOOP;
}

void XSquadObj::DoHeal( float addHp )
{
	LIVE_UNIT_LOOP( spUnit )
	{
		spUnit->DoHeal( addHp );
	} END_LOOP;
}

/**
 @brief 전체 체력에 비례한 비율로 힐
*/
void XSquadObj::DoHealByPercent( float ratio )
{
	LIVE_UNIT_LOOP( spUnit )
	{
		float hpAdd = spUnit->GetMaxHp() * ratio;
		spUnit->DoHeal( hpAdd );
	} END_LOOP;
}


/**
 @brief 이부대의 살아있는 유닛의 토탈 hp를 계산한다.
*/
float XSquadObj::GetTotalHp()
{
	float sum = 0;
	LIVE_UNIT_LOOP( spUnit )
	{
		if( spUnit->IsLive() )
			sum += spUnit->GetHp();
	} END_LOOP;
	return sum;
}
/**
 @brief 부대의 현재 체력을 %로 돌려준다.
 @return 0~1사이의 값
*/
float XSquadObj::GetTotalHpRate()
{
	float sum = 0;
	LIVE_UNIT_LOOP( spUnit ) {
		if( spUnit->IsLive() )
			sum += spUnit->GetHp();
	} END_LOOP;
	return sum / GetMaxHpAllMember();
}

/**
 @brief 현재 살아있는 유닛들의 평균 이동속도를 구한다.
 유닛들의 이동속도 보정치가 반영되어야 한다.
*/
float XSquadObj::GetAvgSpeedUnits()
{
	float sum = 0;
	int numLive = 0;
	LIVE_UNIT_LOOP( spUnit )
	{
		sum += spUnit->GetSpeedMoveForPixel();
		++numLive;
	} END_LOOP;
	
	if( numLive == 0 )
		return 0;
	return sum / numLive;
	
}

/**
 @brief 팔라딘-돌파 특성 하드코딩
*/
void XSquadObj::HardcodingBreakthrough( float& speedMultiply, XArrayLinearN<XSPSquad,64>& aryNear )
{
	XBREAK( m_pBreakThrough == nullptr );
	auto pEffect = m_pBreakThrough->GetEffectByIdx( 0 );
	XBREAK( pEffect == nullptr );
	XBREAK( m_lvBreakThrough == 0 );
	// 이속 보정.	
	float abil = pEffect->invokeAbilityMin[ m_lvBreakThrough ];
	speedMultiply = abil;
	// 인접부대에게 데미지
	if( m_timerBreakThrough.IsOff() )
		m_timerBreakThrough.Set( 1.f );
	if( m_timerBreakThrough.IsOver() )
	{
		m_timerBreakThrough.Reset();
		// 매초당 인접한 적부대에게 데미지를 입힌다.
		XARRAYLINEARN_LOOP_AUTO( aryNear, &spSquad )
		{
			spSquad->DoDamageByPercent( 0.05f, false );
		} END_LOOP;
	}
}

/**
 @brief 부대단위 유닛공격력
 유닛의 기본능력치에 영웅의 능력치만 곱해서 쓴다. 주로 스킬에 사용된다.
*/
float XSquadObj::GetAttackMeleeDamage()
{
	float power = m_pProp->atkMelee * m_pHero->GetAttackRangeRatio();
	return power;
}

/**
 @brief 타겟을 새로 바꾸면 발생한다.
*/
void XSquadObj::OnNewTarget( UnitPtr spTarget )
{
	// 자신의 부대에게 이동모드로 바꿔달라고 요청
//	m_bRequestMoveModeChange = TRUE;
	m_bInoreCounterAttack = false;		// 새 타겟으로 바뀌면 기존 반격무시상태는 해제된다.
}

/**
 @brief 이부대의 부대원중에 하나라도 state에 걸린 병사가 있다면 true
*/
bool XSquadObj::IsState( XGAME::xtState state )
{
	LIVE_UNIT_LOOP( spUnit ) {
		if( spUnit->IsState( state ) )
			return true;
	} END_LOOP;
	return false;
}

/**
 @brief this의 적군단객체를 얻는다.
*/
XSPLegionObj XSquadObj::GetEnemyLegion() 
{
	return XBattleField::sGet()->GetEnemyLegionObj( this );
}

/**
 @brief 이 부대가 타겟팅할수 있는 부대인가.
*/
bool XSquadObj::IsAbleTarget()
{
	bool bAble = false;
	LIVE_UNIT_LOOP( spUnit )
	{
		// 한명이라도 투명화아닌 병사가 있으면 true로 리턴한다.
		if( spUnit->IsState( XGAME::xST_INVISIBLE ) == FALSE )
		{
			bAble = true;
			break;
		}
	} END_LOOP;
	return bAble;
}

/**
 @brief 살아있는 멤버의 hp합산을 돌려준다.
*/
float XSquadObj::GetSumHpAllMember()
{
	float sum = 0;
	LIVE_UNIT_LOOP( spUnit )
	{
		sum += spUnit->GetHp();
	} END_LOOP;
	return sum;
}

float XSquadObj::GetMaxHpAllMember() const 
{
	float sum = 0;
	int numLive = 0;
	bool bHeroLive = false;
	for( auto& spUnit : m_listUnit ) { 
		if( spUnit->IsLive() ) {
			sum += spUnit->GetMaxHp();
			++numLive;
			if( spUnit->IsHero() )
				bHeroLive = true;
		}
	}  
	if( m_spLegionObj ) {
		auto spLegion = m_spLegionObj->GetspLegion();
		if( spLegion ) {
			if( bHeroLive == false ) {
				// 영웅이 죽어서 위에서 집계가 안됐다면 여기서 추가함.
				auto hpMax = spLegion->GethpMaxEach( m_pHero->GetsnHero()
					, true );
				sum += hpMax;
			}
			int numDie = ( m_pHero->GetnumUnit() + 1 ) - numLive;
			XBREAK( numDie < 0 );
			if( numDie > 0 ) {
				//
				for( int i = 0; i < numDie; ++i ) {
					auto hpMax = spLegion->GethpMaxEach( m_pHero->GetsnHero()
						, false );
					sum += hpMax;
				}
			}
		}
	}
	return sum;
}

float XSquadObj::DrawMembersHp( const XE::VEC2& vPos )
{
	auto v = vPos;
#ifdef _XSINGLE
	auto sizeFont = XE::GetMain()->GetpGame()->GetpfdSystemSmall()->GetFontSize() * 0.5f;
	sizeFont += 1.f;
	PUT_STRINGF_SMALL( v.x, v.y, XCOLOR_WHITE, _T("%s(lvSq:%d)"), XGAME::GetStrUnit( GetpHero()->GetUnit() )
																															, m_pHero->GetlevelSquad() );
	v.y += sizeFont;
	for( auto& spUnit : m_listUnit ) {
		if( !spUnit->IsDestroy() ) {
			XCOLOR col = XCOLOR_WHITE;
			if( spUnit->IsDead() )
				col = XCOLOR_RED;
			PUT_STRINGF_SMALL( v.x, v.y, col
												 , "%d/%d", (int)spUnit->GetHp(), spUnit->GetMaxHp() );
			v.y += sizeFont;
		}
	}
	if( m_secDie > 0 ) {
		PUT_STRINGF_SMALL( v.x, v.y, XCOLOR_YELLOW
											, "die=%.2f", m_secDie );
		v.y += sizeFont;
	}
#endif // _XSINGLE
	return v.y;
}
/**
 @brief 부대간 거리를 잰다.
*/
float XSquadObj::GetDistBetweenSquad( XSPSquad spOther )
{
	auto vwDist = spOther->GetvwPos() - GetvwPos();
	return vwDist.Length();
}
/**
 @brief this와 spOther간의 거리가 this의 사거리 이내인가.
*/
bool XSquadObj::IsInAttackRadius( XSPSquad spOther )
{
	auto vwDist = spOther->GetvwPos() - GetvwPos();
	auto distAttack = GetDistAttack();
	auto distSquadSq = vwDist.Lengthsq();
	return distSquadSq < distAttack;
}

void XSquadObj::DoTeleport()
{
	// 부대좌표를 랜덤좌표로 이동시킨다.
	auto vOld = m_vwPos;
	auto vwSize = XWndBattleField::sGet()->GetpWorld()->GetvwSize();
	XE::VEC2 vNew;
	vNew.x = (float)xRandom((int)vwSize.w);
	vNew.y = 350.f + (float)xRandom((int)(vwSize.h - 350.f));
	m_vwPos = vNew;
// 	static bool s_bFlag = false;
// 	if( s_bFlag == false )
//  		m_vwPos.Set( 1669, 285, 0 );
// 	else
// 		m_vwPos.Set( 300, 700, 0 );
// 	s_bFlag = !s_bFlag;
	auto pSfx = new XObjLoop( XGAME::xOT_SFX, vOld, _T("sfx_heal2.spr"), 1 );
	pSfx->SetDir( m_spHeroUnit->GetDir() );
	XBattleField::sGet()->AddObj( WorldObjPtr(pSfx) );
	// 유닛들의 좌표와 상태도 그에 맞춰 변환시킨다.
	for( auto spUnit : m_listUnit ) {
		spUnit->SetvwPos( m_vwPos + spUnit->GetvLocalFromSquad() );
		spUnit->DoIdle();
	}
	if( m_spTarget && !IsInAttackRadius( m_spTarget ) ) {
		// 타겟과의 사거리를 벗어나면 다른 타겟 찾음.
		DoAttackAutoTargetEnemy();
	}
}

/**
 @brief 자원부대인가
*/
bool XSquadObj::IsResourceSquad() const
{
	XBREAK( m_pHero == nullptr );
	return GetspLegionObj()->GetspLegion()->IsResourceSquad( m_pHero->GetsnHero() );
}

/**
 @brief 부대단위 사운드 
*/
// void XSquadObj::PlaySound( const std::string& strKey )
// {
// 	// 부대원중에 같은 키의 사운드를 플레이 시킨게 있는가.
// 	xSound sound;
// 	sound.strKey = strKey;
// 	sound.idSound = idSound;
// 	sound.m_timer.Set(0);
// 	sound.m_secMax = ??;
// 	map[ strKey ] = sound;
// 
// 	SOUNDMNG->OpenPlaySoundOneSec( )
// 	// 있으면 취소
// 	// 없으면 플레이
// 	// 
// }
// 
// 

ID XSquadObj::GetsnHero() const 
{
	if( !m_pHero )
		return 0;
	return m_pHero->GetsnHero();
}
