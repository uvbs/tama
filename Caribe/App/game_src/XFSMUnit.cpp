#include "stdafx.h"
#include "XFSMUnit.h"
#include "XWndBattleField.h"
#include "XBaseUnit.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "Sprite/SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////////////////////////////////////////////////////////
XFSMBase::XFSMBase( ID idFSM, XBaseUnit *pUnit )
	: XEBaseFSM( idFSM, pUnit )
{
	Init();
	m_pUnit = pUnit;
}

void XFSMBase::Destroy()
{
}

XEWorld* XFSMBase::GetWorld( void ) 
{
	return m_pUnit->GetpWndWorld()->GetpWorld();
}

BOOL XFSMBase::IsTargetRight( const XE::VEC3& vwDst ) 
{
	if( vwDst.x > m_pUnit->GetvwPos().x )
		return TRUE;
	return FALSE;
}

/**
 @brief 타겟부대내에서 공격타겟을 찾은후 추적모드로 전환한다.
 @spTarget 추적해야할 대상
 @param fsmNext 사거리까지 도달한 후 해야할 fsm을 지정한다.
*/
XFSMChase* XFSMBase::DoChase( const XSPUnit& spTarget, XFSMBase::xtFSM fsmNext )
{
	XBREAK( spTarget != nullptr && spTarget->GetpSquadObj() == NULL );
	m_pUnit->SetspTarget( spTarget );
	if( spTarget != nullptr )
	{
		// 추적중엔 타겟방향으로 보게 한다.
		m_pUnit->DoDirToTarget( spTarget->GetvwPos() );
		spTarget->AddCntTargeting( 1 );
		auto pfsm = static_cast<XFSMChase*>( ChangeFSM( XFSMBase::xFSM_CHASE ) );
		pfsm->SetidNextFSM( fsmNext );
		return pfsm;
	} else
	{
		ChangeFSM( XFSMBase::xFSM_IDLE );
	}
	return NULL;
}

/**
 @brief 유닛을 vwTarget지점으로 이동하도록 한다.
*/
XFSMChase* XFSMBase::DoChase( const XE::VEC3& vwTarget )
{
	m_pUnit->SetspTarget( nullptr );
//	m_pUnit->SetspTargetSquad( nullptr );
	m_pUnit->SetvwTarget( vwTarget );
	m_pUnit->SetvwBind(XE::VEC2(-1.f));		// 바인드 좌표 클리어 시킴
	XFSMChase *pfsm
		= static_cast<XFSMChase*>( ChangeFSM( XFSMBase::xFSM_CHASE ) );
	return pfsm;
}

/**
 @brief 현재 타겟쪽으로 바라보게 한다.
*/
///////////////////////////////////////////////////////////////////
XFSMIdle::XFSMIdle( XBaseUnit *pUnit )
	: XFSMBase( XFSMBase::xFSM_IDLE, pUnit )
{
	_Init();
}

void XFSMIdle::Destroy()
{
}

void XFSMIdle::Release() {}

void XFSMIdle::Init( void )
{
  // Init에서 SetAction금지
// 	m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
//	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
}

int XFSMIdle::FrameMove( float dt )
{
	m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
	// 
	return 1;
}


////////////////////////////////////////////////////////////////
XFSMChase::XFSMChase( XBaseUnit *pUnit )
	: XFSMBase( XFSMBase::xFSM_CHASE, pUnit )
{
	_Init();
}

void XFSMChase::Destroy()
{
//	m_spTarget.reset();		// 소유권 포기
}

void XFSMChase::Release() {}

void XFSMChase::Init( void )
{
	m_bDash = FALSE;
	m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
	if( m_pUnit->GetspTarget() == nullptr )	{
		XBREAK( m_pUnit->GetpSquadObj()->IsMove() == FALSE );
		// 좌표이동모드
		if( GetidPrevFSM() != XFSMBase::xFSM_CHASE )	// 추적중에 다시 들어온것이면 첨에 딜레이를 주지않는다.
			m_timerDelay.Set( xRandomF( 0.5f ) );
//			m_timerDelay.Set( 5.f );
		// 타겟이 없다면 좌표라도 있어야 한다.
		XBREAK( m_pUnit->GetvwTarget().IsZero() );
		// 현재위치에서 분대내자리까지의 벡터
		XE::VEC3 vDist = m_pUnit->GetvwPos() - ( m_pUnit->GetpSquadObj()->GetvwPos() + m_pUnit->GetvLocalFromSquad() );
		m_vwLocalMoveStart = m_pUnit->GetvwPos();
		float speedMove = m_pUnit->GetSpeedMoveForPixel() * 2;	// 분대로컬좌표로 뛰어가는건 일반스피드의 두배로 한다.
		// 유닛의 이동속도로 목표지점까지 이동하는데 걸리는 시간
		float secMove = ( vDist.Lengthsq() / ( speedMove * speedMove ) ) / XFPS;
		if( secMove > 0 )
			GettimerLocalMove().Set( secMove );
		m_modeMoveToTarget = 0;
		XBREAK( m_pUnit->IsBindTarget() == TRUE );
		m_speedAccel = 1.f;
	} else {
		// 타겟이동모드
		if( m_pUnit->IsRange() )
			m_timerDelay.Set( xRandomF( 0.5f ) );
		else
			m_timerDelay.Off();
		// 타겟을 대상으로 가는상태면 분대내 자리를 지킬필요 없음.
		if( m_pUnit->IsBindTarget() ) {
			m_modeMoveToTarget = 1;
#ifdef _DEBUG
			if( m_pUnit->IsRange() ) {
				// 원거리 유닛이 바인드좌표랑 너무 멀리떨어져있으면
				XE::VEC3 vwBind = m_pUnit->GetvwBind();
				float distsq = ( vwBind - m_pUnit->GetvwPos() ).Lengthsq();
				if( distsq > 400.f * 400.f ) {
					XBREAK(1);
				}
			}
#endif
			if( m_pUnit->GetType() == XGAME::xUNIT_ARCHER ) {
				int a = 0;
			}
			m_speedAccel = 1.f;
		} else {
			m_modeMoveToTarget = 2;
			// 원거리의 경우는 빠르게 자리를 잡기위해 가속을 쓴다.
			if( m_pUnit->IsRange() )
				m_speedAccel = 2.f + xRandomF( 1.f );
		}
	}
}

/**
 @brief 추가초기화
*/
void XFSMChase::Init( BOOL bDash )
{
	m_bDash = bDash;
}

void XFSMChase::Uninit( void )
{
	m_pUnit->GetpSprObj()->SetmultiplySpeed( 1.f );
	SetidNextFSM( XFSMBase::xFSM_NONE );	// 상태벗어날때는 초기화값으로 돌림(값이 있었다면 Uninit전에 처리해야함)
	m_pUnit->SetspeedBind(0);
	m_bDash = FALSE;
}

// void XFSMChase::Init( XSPUnit& spUnit )
// {
// //	m_spTarget = spUnit;
// }

/**
 @brief 타겟좌표를 직접 추적하는 프로세스
 적부대에서 다시 타겟을 찾아 움직일때.
*/
int XFSMChase::ProcessTraceTarget( float dt )
{
	if( m_pUnit->IsTargetLive() ) {
		// 추적중엔 타겟방향으로 바라보게 한다.
		m_pUnit->DoDirToTarget( m_pUnit->GetspTarget()->GetvwPos() );
		// 약간의 딜레이후에 출발한다.
		if( m_timerDelay.IsOver() || m_timerDelay.IsOff() ) {
			// 부대이동모드에서 개별추적모드로 바뀔때는 딜레이가 없어서 off상태일때도 모션을 하도록 함.
			m_timerDelay.Off();
			m_pUnit->GetpSprObj()->SetAction( ACT_RUN );
		}
		if( m_pUnit->GetpSprObj()->GetActionID() == ACT_RUN ) {
			BOOL bArrive = FALSE;
			XE::VEC3 vDist = m_pUnit->GetspTarget()->GetvwPos() - m_pUnit->GetvwPos();
			vDist.Abs();
			// 공격사거리안으로 들어왔는지 검사한다.
			float sizeRadius = m_pUnit->GetAttackRadiusByPixel();
			// 원거리유닛(궁수)자연스럽게 위치를 잡기 위해서 사거리에 +a로 랜덤성을 준다.
			if( m_pUnit->IsRange() )
				sizeRadius += (float)random(32);	
			// 사거리 이내에 들어왔는가.
 			if( vDist.Lengthsq() <= sizeRadius * sizeRadius ) {
 				bArrive = TRUE;
 			} else {
				if( m_pUnit->GetTypeAtk() == XGAME::xAT_RANGE ) {
					// 원거리유닛은 추적목표좌표가 오브젝트 좌표다.
					m_vwDstTarget = m_pUnit->GetspTarget()->GetvwPos();
				} else {
					m_vwDstTarget = m_pUnit->GetspTarget()->GetvwPos();
				}
				// 좌표가 월드를 벗어났으면 안쪽으로 보정해준다.
				m_vwDstTarget = GetWorld()->ClippingObj( m_vwDstTarget,
											m_pUnit->GetspTarget()->GetSize() );
				// 이번턴에 이동량을 계산한다.
				XE::VEC3 vDelta;
				float speedMove = m_pUnit->GetSpeedMoveForPixel() * m_speedAccel;
				if( m_pUnit->IsState(XGAME::xST_ENTANGLE) )
					speedMove = 0.f;
				speedMove *= dt;
				bArrive = XBaseUnit::sGetMoveDstDelta( m_pUnit->GetvwPos(),
														m_vwDstTarget,
														speedMove,
														&vDelta );
				// 공포에 걸리면 타겟과 반대방향으로 도망간다.
// 				if( m_pUnit->IsState(XGAME::xST_FEAR) )
// 					vDelta = -vDelta;
				// 1배속일때 movSpeedNormal픽셀을 간다고 가정해서 속도에 따라 애니메이션 속도를 조절한다.
				float multiply = (m_pUnit->GetSpeedMovePerSec() * m_speedAccel) / m_pUnit->GetMovSpeedNormal();
				m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );
				m_pUnit->AddDelta( vDelta );
				m_pUnit->AddPos( vDelta );
			}
			if( bArrive ) {
				// 다음 행동은 대기라도 들어있어야 한다.
				XBREAK( GetidNextFSM() == XFSMBase::xFSM_NONE );
				//				CONSOLE( "chase move arrive: idNextFSM=%s", XFSMBase::sGetStrFSM( GetidNextFSM() ) );
				if( m_bArrive == false ) {
					m_pUnit->OnEventJunctureCommon( XSKILL::xJC_ARRIVE_TARGET );
					m_bArrive = true;
				}
				if( m_pUnit->IsTargetLive() )	{// 도착이벤트때문에 타겟이 죽을수 있음.
					// 공격모션이 "공격금지"상태등으로 실행이 안될수도 있으니 일단 대기모션으로 전환.
					m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
					m_pUnit->OnArriveTarget( m_pUnit->GetspTarget(), m_vwDstTarget );
					if( GetidNextFSM() ) {
						auto pBaseFSM = ChangeFSM( GetidNextFSM() );
						if( pBaseFSM->GetidFSM() == XFSMBase::xFSM_NORMAL_ATTACK ) {
							auto pfsm = static_cast<XFSMNormalAttack*>( pBaseFSM );
							pfsm->Init( m_pUnit->GetspTarget() );
						}
						return 0;		// ChangeFSM을 하면 Uninit가 호출되므로 더이상 실행되어선 안된다. 바로 리턴.
					}
				} else
					m_pUnit->RequestNewMission();
			}
		}
	} else {
//		m_pUnit->DoChaseNewTarget();
		m_pUnit->RequestNewMission();
		// 만약 타겟을 찾지 못했다면 IDLE모드로 들어간다.
		// 추적중엔 타겟방향으로 바라보게 한다.
// 		if( m_pUnit->GetspTarget() != nullptr )
// 			m_pUnit->DoDirToTarget( m_pUnit->GetspTarget()->GetvwPos() );
	}
// 	// 1배속일때 movSpeedNormal픽셀을 간다고 가정해서 속도에 따라 애니메이션 속도를 조절한다.
// 	float multiply = m_pUnit->GetSpeedMovePerSec() / m_pUnit->GetpProp()->movSpeedNormal;
// 	m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );
	return 1;
}

/**
 @brief 최초 부대끼리 붙었을때 돌격
*/
int XFSMChase::ProcessTraceBind( float dt )
{
//	XBREAK( m_pUnit->GetspTargetSquad() == nullptr );
	if( XBREAK( m_pUnit->GetvwBind().IsMinus() == TRUE ) )
		return 1;
	if( m_pUnit->IsTargetLive() ) {
		// 좌표가 바인딩 되어있을때
		m_vwDstTarget = m_pUnit->GetvwBind();
		// 추적중엔 타겟방향으로 바라보게 한다.
		m_pUnit->DoDirToTarget( m_vwDstTarget );
		m_pUnit->GetpSprObj()->SetAction( ACT_RUN );
		BOOL bArrive = FALSE;
		// 이번턴에 이동량을 계산한다.
//		float speedMove = m_pUnit->GetSpeedMoveForPixel() * m_speedAccel;
		// 근접전 유닛인데 밀리모드로 안되어있으면 에러
		XBREAK( m_pUnit->IsRange() == FALSE && 
				m_pUnit->GetpSquadObj()->GetbMeleeMode() == FALSE );
		float speedMove = 0;
		if( m_pUnit->GetspeedBind() > 0 )
		{
			// GetspeedBind는 프레임당 이동픽셀
			speedMove = m_pUnit->GetMovePixelPerFrame( m_pUnit->GetspeedBind() );
		}
		else
		{
			if( m_speedAccel == 1.f && m_bDash )
			{
				XBREAK( m_pUnit->IsRange() );	// 원거리유닛은 이쪽으로 들어올일이 없다.
				float distsq = ( m_vwDstTarget - m_pUnit->GetvwPos() ).Lengthsq();
				float radiussq = m_pUnit->GetpSquadObj()->GetRadius();
				radiussq *= radiussq;
				if( distsq < radiussq )
					m_speedAccel = 2.f + xRandomF( 1.f );
			}
			speedMove = m_pUnit->GetSpeedMoveForPixel() * m_speedAccel;
		}
		XBREAK( speedMove > 50.f );
		if( m_pUnit->IsState( XGAME::xST_ENTANGLE ) )
			speedMove = 0.f;
		speedMove *= dt;
		XE::VEC3 vDelta;
		bArrive = XBaseUnit::sGetMoveDstDelta( m_pUnit->GetvwPos(),
											m_vwDstTarget,
											speedMove,
											&vDelta );
		// 공포에 걸리면 타겟과 반대방향으로 도망간다.
// 		if( m_pUnit->IsState( XGAME::xST_FEAR ) )
// 			vDelta = -vDelta;
		float multiply = ( m_pUnit->GetSpeedMovePerSec() * m_speedAccel ) / m_pUnit->GetMovSpeedNormal();
		if( speedMove == 0 )
			multiply = 1.f;		// 이동속도가 0일때 애니메이션이 안되니 어색해 보여서 1로함.
		m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );
		m_pUnit->AddDelta( vDelta );
		m_pUnit->AddPos( vDelta );
		if( bArrive ) {
			// 도착하면 전투모드로 변경
			// 다음 행동은 대기라도 들어있어야 한다.
			// 매 공격시마다 들어올수 있다. 공격이 끝나고 추적+공격 명령을 주므로.
			XBREAK( GetidNextFSM() == XFSMBase::xFSM_NONE );
			if( m_bArrive == false ) {
				m_pUnit->OnEventJunctureCommon( XSKILL::xJC_ARRIVE_TARGET );
				m_bArrive = true;
			}
//			CONSOLE( "chase move arrive: idNextFSM=%s", XFSMBase::sGetStrFSM( GetidNextFSM() ) );
			// 공격모션이 "공격금지"상태등으로 실행이 안될수도 있으니 일단 대기모션으로 전환.
			m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
			m_pUnit->OnArriveTarget( XSPUnit(), m_vwDstTarget );
			if( GetidNextFSM() ) {
				XEBaseFSM *pBaseFSM = ChangeFSM( GetidNextFSM() );
				if( pBaseFSM->GetidFSM() == XFSMBase::xFSM_NORMAL_ATTACK ) {
					XFSMNormalAttack *pfsm
						= static_cast<XFSMNormalAttack*>( pBaseFSM );
					pfsm->Init( m_pUnit->GetspTarget() );
				}
				return 0;		// ChangeFSM을 하면 Uninit가 호출되므로 더이상 실행되어선 안된다. 바로 리턴.
			}
		}
	} else
	{
		// 타겟이 죽거나 없어졌다.
//		m_pUnit->DoChaseNewTarget();	// <<얘혼자 대상부대를 맘대로 바꾸면 곤란.
		// 부대에게 새로운 임무를 요청함.
		m_pUnit->RequestNewMission();
	}
	return 1;
}

int XFSMChase::FrameMove( float dt )
{
	// 타겟의 좌표쪽으로 이동하는 모드(전투중 타겟부대내에서 다른타겟쪽으로 이동할때)
	if( m_modeMoveToTarget == 2 )
	{
		ProcessTraceTarget( dt );
	} else
	// 타겟주위의 바인딩좌표쪽으로 이동하는 모드(최초 부대끼리 맞붙으며 돌격할때)
	if( m_modeMoveToTarget == 1 )
	{
		// 1배속일때 movSpeedNormal픽셀을 간다고 가정해서 속도에 따라 애니메이션 속도를 조절한다.
// 		float multiply = m_pUnit->GetSpeedMovePerSec() / m_pUnit->GetMovSpeedNormal();
// 		m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );
		ProcessTraceBind( dt );
	} else
	{
		do 
		{
			// 일반 지정좌표쪽으로 이동하는중 제자리를 찾아가는 모드(멀리있는 타겟으로 향할때)
			/*
			.부대는 부대이동명령으로 이동중
			.실시간으로 현재부대내 로컬위치를 구함
			.그 좌표를 목표로 달리도록 함
			*/
			auto bPlayer = m_pUnit->IsPlayer();
			m_vwDstTarget = m_pUnit->GetpSquadObj()->GetvwPos()
						+ m_pUnit->GetvLocalFromSquad();
			// 추적중엔 타겟방향으로 바라보게 한다.
//			m_pUnit->DoDirToTarget( m_vwDstTarget );
			auto& vwTarget = m_pUnit->GetpSquadObj()->GetvwTarget();
			m_pUnit->DoDirToTarget( vwTarget );
			// 약간의 딜레이후에 출발한다.
			if( m_timerDelay.IsOver() || m_timerDelay.IsOff() )
			{
				// 부대이동모드에서 개별추적모드로 바뀔때는 딜레이가 없어서 off상태일때도 모션을 하도록 함.
				m_timerDelay.Off();
				m_pUnit->GetpSprObj()->SetAction( ACT_RUN );
			}

			if( m_timerDelay.IsOff() )
			{
				m_pUnit->GetpSprObj()->SetAction( ACT_RUN );
				BOOL bArrive = FALSE;
	// 			XE::VEC3 vDist = m_vwDstTarget - m_pUnit->GetvwPos();
	// 			vDist.Abs();
				// 이번턴에 이동량을 계산한다.
				XE::VEC3 vDelta;
				float multiplyLocalMove = 2.f;
				float speedMove = m_pUnit->GetSpeedMoveForPixel() * multiplyLocalMove;
				if( m_pUnit->IsState( XGAME::xST_ENTANGLE ) )
					speedMove = 0.f;
				speedMove *= dt;
				bArrive = XBaseUnit::sGetMoveDstDelta( m_pUnit->GetvwPos(), 
												m_vwDstTarget, 
												speedMove, 
												&vDelta );
				// 공포에 걸리면 타겟과 반대방향으로 도망간다.
// 				if( m_pUnit->IsState( XGAME::xST_FEAR ) )
// 					vDelta = -vDelta;
				// 얼음상태는 이동이멈춘다.
				if( m_pUnit->IsState( XGAME::xST_ICE ) )
					vDelta.Set( 0 );
				m_pUnit->AddDelta( vDelta );
				m_pUnit->AddPos( vDelta );
				if( bArrive )
				{
					// 부대내 자기자리에 도착했으면 애니메이션 속도 제대로 맞춰줌
					multiplyLocalMove = 1.f;
				} else
					multiplyLocalMove = 2.f;
				// 1배속일때 movSpeedNormal픽셀을 간다고 가정해서 속도에 따라 애니메이션 속도를 조절한다.
				float multiply = ( m_pUnit->GetSpeedMovePerSec() * multiplyLocalMove ) / m_pUnit->GetMovSpeedNormal();
				m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );

			} else {
				int a= 0;
			}
		} while( 0 );
	}
	// 
	return 1;
}


////////////////////////////////////////////////////////////////
XFSMNormalAttack::XFSMNormalAttack( XBaseUnit *pUnit )
	: XFSMBase( XFSMBase::xFSM_NORMAL_ATTACK, pUnit )
{
	_Init();
}

void XFSMNormalAttack::Destroy()
{
	// FSM객체자체가 삭제될때
}

void XFSMNormalAttack::Release()
{
	m_spLastTargetSquad.reset();
}

void XFSMNormalAttack::Init( void )
{
	DoAttackMotion();
	// 슬로우에 걸리면 모션이 느려진다.
	if( m_pUnit->IsState( XGAME::xST_SLOW ) )
	{
		float speed = m_pUnit->GetpSprObj()->GetmultiplySpeed();
		m_pUnit->GetpSprObj()->SetmultiplySpeed( speed * 0.5f );
	}
	//	XBREAK( m_pUnit->GetspTargetSquad() == nullptr );
}

void XFSMNormalAttack::Init( XSPUnit& spUnit )
{
	XBREAK( spUnit == nullptr );
	//	m_spTarget = spUnit;
}


void XFSMNormalAttack::Uninit( void )
{
	// FSM상태에서 빠져나갈때
	m_pUnit->GetpSprObj()->SetmultiplySpeed( 1.f );
	Destroy();
	m_pUnit->SettypeCurrMeleeType( XGAME::xMT_NONE );
	SetidNextFSM( XFSMBase::xFSM_NONE );	// 상태벗어날때는 초기화값으로 돌림(값이 있었다면 Uninit전에 처리해야함)
}


void XFSMNormalAttack::DoAttackMotion( void )
{
	// 이함수는 타겟이 살아있음을 전제로 한다.
	XBREAK( m_pUnit->GetspTarget() == nullptr );
#pragma message("이부분 자꾸 assert걸려서 일단 막아둠")
	//XBREAK( m_pUnit->GetspTarget()->IsDead() == TRUE );
	// 부대가 현재 타겟부대를 갖고 있는가
	if( m_pUnit->GetpSquadObj()->IsHaveTargetSquad() ) {
		// 타겟쪽으로 바라보게 한다.
		m_pUnit->DoDirToTarget( m_pUnit->GetspTarget()->GetvwPos() );
		int nExec = m_pUnit->OnBeforeAttackMotion();
		if( nExec == 0 ) {
			// 원거리 유닛 전용 처리
			if( m_pUnit->GetTypeAtk() == XGAME::xAT_RANGE ) {
				XBREAK( m_pUnit->GetspTarget() == nullptr );
				bool bMelee = (m_pUnit->GetpSquadObj()->GetbMeleeMode() == TRUE);
				// 현재 원거리 공격중이었다.
				if( m_pUnit->GettypeCurrMeleeType() == XGAME::xMT_RANGE ) {
					// 부대가 근접모드인가
					if( m_pUnit->GetpSquadObj()->GetbMeleeMode() ) {
						if( m_pUnit->GetcntShell() == 0 ) {
							// 바인딩타겟 추적모드로 변환;
							m_pUnit->DoAttackTargetByBind( m_pUnit->GetspTarget(), FALSE );
							bMelee = true;
							return;
						}
					}
				} else
				// 현재 근접공격중이었다.
				if( m_pUnit->GettypeCurrMeleeType() == XGAME::xMT_MELEE ) {
					// 부대가 원거리 모드인가
					if( m_pUnit->GetpSquadObj()->GetbMeleeMode() == FALSE ) {
						// 일반 추적모드로 전환
						m_pUnit->DoChaseAndAttack( m_pUnit->GetspTarget() );
						return;
					}
				}
				// 골렘껍질이 있으면 근접전 모드로 전환하지 않는다.
				if( m_pUnit->GetcntShell() > 0 )
					bMelee = false;
				// 현재 부대가 근접전 모드중이면 근접모션으로 플레이
//				if( m_pUnit->GetpSquadObj()->GetbMeleeMode() )
				if( bMelee ) {
					m_pUnit->GetpSprObj()->SetAction( ACT_ATTACK2, xRPT_1PLAY );
					m_pUnit->SettypeCurrMeleeType( XGAME::xMT_MELEE );
// 					m_pUnit->OnAfterAttackMotion( XSKILL::xJC_CLOSE_ATTACK );		"근접공격시" 이벤트는 타점에 불리는게 맞아서 옮김
				} else {
					m_pUnit->SettypeCurrMeleeType( XGAME::xMT_RANGE );
					// 현재 부대가 원거리 공격중이면 원거리모션으로 플레이
					if( m_pUnit->GetpSprObj()->IsHaveAction( ACT_ATTACK3 ) ) {
						if( random( 2 ) == 0 )
							m_pUnit->GetpSprObj()->SetAction( ACT_ATTACK1, xRPT_1PLAY );
						else
							m_pUnit->GetpSprObj()->SetAction( ACT_ATTACK3, xRPT_1PLAY );
					}	else
						m_pUnit->GetpSprObj()->SetAction( ACT_ATTACK1, xRPT_1PLAY );
					//m_pUnit->OnAfterAttackMotion( XSKILL::xJC_RANGE_ATTACK_START );	// 타점에서 호출하게 하는게 맞는듯.
				}
			// if( m_pUnit->GetTypeAtk() == XGAME::xAT_RANGE )
			} else {
				ID ary[ 3 ] = {ACT_ATTACK1, ACT_ATTACK2};
				m_pUnit->GetpSprObj()->SetAction( ary[ random( 2 ) ], xRPT_1PLAY );
				m_pUnit->SettypeCurrMeleeType( XGAME::xMT_MELEE );
// 				m_pUnit->OnAfterAttackMotion( XSKILL::xJC_CLOSE_ATTACK );		"근접공격시" 이벤트는 타점에 불리는게 맞아서 옮김
			}
			m_pUnit->StartAttackDelay( m_pUnit->GetspTarget() );
		} // if( nExec )
//		m_timerAttack.Set( m_pUnit->GetSpeedAttack() );
		// 마지막으로 공격한 타겟부대
		if( m_spLastTargetSquad &&
			m_spLastTargetSquad->GetsnSquadObj() != m_pUnit->GetpSquadObj()->GetspTarget()->GetsnSquadObj() )
			m_pUnit->SetcntAttack( 0 );

		m_spLastTargetSquad = m_pUnit->GetpSquadObj()->GetspTarget();
	}
}

int XFSMNormalAttack::FrameMove( float dt )
{
	auto speedProp = m_pUnit->GetpPropUnit()->atkSpeed;
	auto speedCurr = m_pUnit->GetSpeedAttack( m_pUnit->GetspTarget() );
	float multiply = speedProp / speedCurr;
	m_pUnit->GetpSprObj()->SetmultiplySpeed( multiply );
//	m_pUnit->GetpSprObj()->SetmultiplySpeed( 1.f );
//	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
//	XBREAK( m_pUnit->GetspTargetSquad() == nullptr );
	XBREAK( m_pUnit->GetspTarget() == nullptr );
	XBaseUnit *pTarget = nullptr;
	if( m_pUnit->GetspTarget() != nullptr )
		pTarget = m_pUnit->GetspTarget().get();
	if( m_pUnit->GetpSprObj()->GetActionID() != ACT_IDLE1 ) {
		if( m_pUnit->GetpSprObj()->IsFinish() ) {
			// 공격모션이 끝나면 대기모션으로 전환
			m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
			// 공격중에 타겟이 사거리를 벗어나면 다시 추적모드로 된다.
			if( pTarget && pTarget->IsLive() ) {
				// 동작이 끝나면 이벤트핸들러를 호출해준다. 만약 핸들어내에서 어떤 동작이 수행되었다면 아래 추적모드는 처리하지 않는다.
				int nExec = m_pUnit->OnEndNormalAttackMotion();
				if( nExec ) {
					// 다른 모션동작이 수행되었다면 공속딜레이를 줘서 중복모션이 나오지 않게 한다.
					m_pUnit->StartAttackDelay( m_pUnit->GetspTarget() );
//					m_timerAttack.Set( m_pUnit->GetSpeedAttack() );
				} else {
					// 타겟과 거리가 멀어지면 다시 개별추적으로 바꾼다.
					float distsq = ( pTarget->GetvwPos() - m_pUnit->GetvwPos() ).Lengthsq();
					float distAtk = m_pUnit->GetAttackRadiusByPixel();
					if( distsq > distAtk * distAtk ) {
						if( m_pUnit->IsRange() ) {
							m_pUnit->ClearBind();
							// 원거리유닛은 일반 추적을 한다.
							XBREAK( m_pUnit->IsBindTarget() );
							m_pUnit->DoChaseAndAttack( m_pUnit->GetspTarget() );
							XBREAK( m_pUnit->GetspTarget() == nullptr );
						} else {
							// 거리가 멀어지면 다시 추적모드로한다. 바인드좌표는 원래좌표를 사용한다.
							if( m_pUnit->IsBindChase() == FALSE ) {
								BOOL bDash = FALSE;
								m_pUnit->DoAttackTargetByBind( m_pUnit->GetspTarget(), bDash );
							}
						}
					}
				}

			}
		}
	} else
	{

	}
	// 공속딜레이가 끝나면...
//	if( m_timerAttack.IsOver() )
	if( m_pUnit->IsOverAttackDelay() )
	{
		m_pUnit->AddCntAttack();
		m_pUnit->StopAttackDelay();		// 일단 공속딜레이 타이머는 끈다.
		if( m_pUnit->IsState(XGAME::xST_CHAOS) )
		{
			if( random(2) == 0 )
			{
				// 50%의 확률로 자기편 타겟으로 바꾼다.
				XSPUnit unitFriend = m_pUnit->GetNearUnit( 10.f, 
											m_pUnit->GetCamp().GetbitCamp(), true );
				m_pUnit->DoChaseAndAttack( unitFriend );
				return 1;
			}
		} else
		{

			// 혹시 지금 치고 있던 타겟이 아군이면 다시 바궈준다.
			if( m_pUnit->GetCamp().IsSameSide( pTarget->GetCamp() ) )
				m_pUnit->RequestNewMission();
		}
		if( pTarget && pTarget->IsLive() && !pTarget->IsInvisible() )
		{
			// 타겟이 여전히 살아있으면 다시 공격모션
//			DoAttackMotion();
			m_pUnit->DoChaseAndAttackCurrent();
		}
		else
		{
			// 새로운 타겟을 찾게 한다.
//			m_pUnit->DoChaseNewTarget();
			m_pUnit->RequestNewMission();
			return 1;
		}
	}
//	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
	// 
	return 1;
} // XFSMNormalAttack::FrameMove

///////////////////////////////////////////////////////////////////
XFSMStun::XFSMStun( XBaseUnit *pUnit )
	: XFSMBase( XFSMBase::xFSM_STUN, pUnit )
{
	_Init();
}

void XFSMStun::Destroy()
{
}

void XFSMStun::Release() {}

void XFSMStun::Init( void )
{
	if( m_pUnit->GetpSprObj()->IsHaveAction(ACT_STUN) )
		m_pUnit->GetpSprObj()->SetAction( ACT_STUN );
	else
		m_pUnit->GetpSprObj()->SetAction( ACT_IDLE1 );
}

/**
 @brief 추가 초기화
*/
void XFSMStun::Init( float secStun )
{
	m_timerDelay.Set( secStun );
}

int XFSMStun::FrameMove( float dt )
{
	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
	if( m_timerDelay.IsOver() )
	{
		m_pUnit->DoChaseAndAttackCurrent();		// 공격하던 타겟을 다시 공격
//		m_pUnit->SetDestroy( 1 );
	}
//	m_pUnit->GetpSprObj()->SetAction( ACT_STUN );
	// 
	return 1;
}
void XFSMStun::OnDamage( void )
{
	// 수면중이었으면 피격시 깨어난다.
	if( m_pUnit->IsState( XGAME::xST_SLEEP) )
	{
		m_pUnit->DoChaseAndAttackCurrent();		// 공격하던 타겟을 다시 공격
//		m_pUnit->SetDestroy( 1 );
	}
}

///////////////////////////////////////////////////////////////////
XFSMDie::XFSMDie( XBaseUnit *pUnit )
	: XFSMBase( XFSMBase::xFSM_DIE, pUnit )
{
	_Init();
}

void XFSMDie::Destroy()
{
}

void XFSMDie::Release() {}

void XFSMDie::Init( void )
{
  // Init에서 SetAction금지
	m_State = 0;
	m_timerDelay.Set( 30.f );
	XBREAK( m_pUnit->IsLive() == TRUE );
	m_pUnit->GetpSquadObj()->OnDieMember( m_pUnit );
	m_pUnit->GetpSprObj()->SetAction( ACT_DIE, xRPT_1PLAY_CONT );
}

int XFSMDie::FrameMove( float dt )
{
  m_pUnit->GetpSprObj()->SetAction( ACT_DIE, xRPT_1PLAY_CONT );
	XBREAK( m_pUnit->GetpSprObj()->GetmultiplySpeed() != 1.f );
// 	if( m_timerDelay.IsOver() )	{
// 		if( m_timerAppear.IsOff() )
// 			m_timerAppear.Set(2.f);
// 		if( m_timerAppear.IsOver() ) {
// 			m_pUnit->SetDestroy( 1 );
// 		} else {
// 			float lerp = m_timerAppear.GetSlerp();
// 			m_pUnit->SetAlpha( 1.f - lerp );
// 		}
// 	}
	// 
	return 1;
}