#include "stdafx.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XGame.h"
#include "client/XAppMain.h"
#include "XEObjMngWithType.h"
#include "XBaseUnit.h"
#include "XSquadObj.h"
#include "XSceneBattle.h"
#include "XUnitHero.h"
#include "opengl2/XRenderCmd.h"
#include "XFramework/XEProfile.h"

using namespace XSKILL;
using namespace XGAME;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XWndBattleField* XWndBattleField::s_pInstance = nullptr;
XWndBattleField* XWndBattleField::sGet() 
{
	if( s_pInstance == nullptr )
		s_pInstance = new XWndBattleField;
	return s_pInstance;
}

////////////////////////////////////////////////////////////////
XWndBattleField::XWndBattleField()
	: XEWndWorldImage( nullptr )
{
	Init();
//	SetscaleMin( 0.336f );
	SetscaleMin( 0.4f );
	SetscaleMax( 2.f );
}

void XWndBattleField::Destroy()
{
	XWndBattleField::s_pInstance = nullptr;
}

BOOL XWndBattleField::OnCreate()
{
	auto bRet = XEWndWorldImage::OnCreate();
	const auto vwSize = GetpWorld()->GetvwSize();
// 	SetvwCamera( (vwSize * 0.5f) + XE::VEC2(0,-30) );
//	XScroll::SetvCurr( (vwSize * 0.5f) + XE::VEC2(0,-30) );
	// 최초 카메라 위치설정은 XSceneBattle에 있ㄷ음(SetFocus)
	return bRet;
}

XEWorld* XWndBattleField::OnCreateWorld( const XE::VEC2& vwSize )
{
	m_prefBattleField = new XBattleField( vwSize );
//	SetvwCamera( m_pBattleField->GetvwSize() / 2.f );
	return m_prefBattleField;
}

ID XWndBattleField::GetidSelectSquad( void ) 
{
	return ( m_spSelectSquad ) ? m_spSelectSquad->GetsnSquadObj() : 0;
}

/**
 
*/
int XWndBattleField::Process( float dt )
{
	// 선택하고 있던 부대가 전멸하면 선택꺼줌
	if( m_spSelectSquad && m_spSelectSquad->IsLive() == FALSE )
		m_spSelectSquad.reset();
	// 임시선택중이던 부대가 죽으면 선택취소
	if( m_spTempSelect && m_spTempSelect->IsLive() == FALSE )
	{
		m_spTempSelect.reset();
		m_vTouch.Set(-1.f);
	}
	// 
	if( m_spTempSelectEnemy && m_spTempSelectEnemy->IsLive() == FALSE )
		m_spTempSelectEnemy.reset();

	return XEWndWorldImage::Process( dt );
}
/**
 
*/
void XWndBattleField::Draw( void )
{
	XEWndWorldImage::Draw();
	{
		XPROF_OBJ( "render_batch" );
		XRenderCmdMng::sGet()->RenderBatch();	// 레이어방식으로 해서 레이어별로 가지고 있어야 할듯.
	}
	//	XParticleMng::sGet()->Draw();
}

void XWndBattleField::DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat )
{
	if( XAPP->m_bDebugSlowFrame )
	{
		for( int i = 0; i < 300; ++i )
			XEWndWorldImage::DrawDebugInfo( x, y, col, pFontDat );
	} else
		XEWndWorldImage::DrawDebugInfo( x, y, col, pFontDat );
}

void XWndBattleField::OnLButtonDown( float lx, float ly )
{
	m_spTempSelect.reset();		// 터치하기전 이전 임시셀렉트가 있으면 해제시킴
	m_vTouch.Set(-1.f);

	// 부대 픽킹검사를 한다.
	// 걸린 유닛이 있으면 해당 부대를 임시선택한다.
	if( m_spSelectSquad == nullptr ) {
		m_spTempSelect = PickingSquad( lx, ly, XGAME::xSIDE_PLAYER );
#ifdef WIN32
		if( XAPP->m_bDebugMode ) {
			if( m_spTempSelect == nullptr )
				m_spTempSelect = PickingSquad( lx, ly, XGAME::xSIDE_OTHER );
		}
#endif
	} else {
		auto sideRival = XGAME::xSIDE_OTHER;
#ifdef WIN32
		if( XAPP->m_bDebugMode && XAPP->m_bCtrl ) {
			// 강제 공격(선택되어있는 부대의 상대진영부대를 우선으로 강제공격한다.
			sideRival = m_spSelectSquad->GetSideRival();
		}
#endif // WIN32
		const auto sideRivalOfRival = (sideRival == xSIDE_PLAYER)? xSIDE_OTHER : xSIDE_PLAYER;
		// 이미 아군이 선택되어 있는상태라면 아군적군이 동시에 있을때 적을 우선으로 찍음.
		m_spTempSelectEnemy = PickingSquad( lx, ly, sideRival );
		if( m_spTempSelectEnemy == nullptr )
			m_spTempSelect = PickingSquad( lx, ly, sideRivalOfRival );
	}
	// 만약 새로 픽킹한 부대가 이전에 선택했던 부대라면 선택을 꺼줌
	if( (m_spTempSelect && m_spSelectSquad) &&
		(m_spTempSelect->GetsnSquadObj() == m_spSelectSquad->GetsnSquadObj()) )	{
			m_spSelectSquad.reset();
			m_spTempSelect.reset();
	}
	// 현재 아군부대가 선택되어있는 상태에서
// 	if( m_spSelectSquad )
// 	{
// 		// 적진영이 픽킹되었는지 검사
// 		m_spTempSelectEnemy = PickingSquad( lx, ly, XGAME::xSIDE_OTHER );
// 	}

	// 적진이 픽킹되지 않았으나 현재 선택된아군 부대가 있으면 해당 위치로 이동명령을 내린다.
	if( m_spTempSelect || 
		m_spTempSelectEnemy || 
		m_spSelectSquad )
		m_vTouch.Set( lx, ly );

	// 픽킹한 터치좌표를 받아둔다.
	XEWndWorldImage::OnLButtonDown( lx, ly );
}

void XWndBattleField::OnMouseMove( float lx, float ly )
{
	// 일정픽셀이상 드래깅을 했다면 선택을 취소한다.
	if( m_vTouch.IsMinus() == FALSE )
	{
		XE::VEC2 vDist = XE::VEC2(lx, ly) - m_vTouch;
		float distsq = vDist.Lengthsq();
		if( distsq > 3.f * 3.f )
		{
			m_spTempSelect.reset();
			m_spTempSelectEnemy.reset();
			m_vTouch.Set(-1.f);
		}
	}
	XEWndWorldImage::OnMouseMove( lx, ly );
}

void XWndBattleField::OnLButtonUp( float lx, float ly )
{
	// 임시선택한 유닛을 확정선택한다.
	// 임시선택한 유닛이 여전히 있으면
	if( m_spTempSelect )	{
		m_spSelectSquad = m_spTempSelect;
		m_spTempSelect.reset();
		XBREAK( SCENE_BATTLE == NULL );
		SCENE_BATTLE->OnSelectSquad( m_spSelectSquad );
	} else
	// 선택된부대가 있을때 적임시선택이 있으면 공격
	if( m_spSelectSquad ) {
		if( m_spTempSelectEnemy ) {
			if( m_spTempSelectEnemy->IsLive() ) {
//				CONSOLE( "공격: %d => %d", m_spSelectSquad->GetUnit(), m_spTempSelectEnemy->GetUnit() );
				m_spSelectSquad->DoAttackSquad( m_spTempSelectEnemy );
			}
		} else {
			if( m_vTouch.IsMinus() == FALSE )	{// 터치가 취소되었을수도 있으므로
				XE::VEC3 vwPick = GetPosWindowToWorld( XE::VEC2( lx, ly ) );
//				CONSOLE( "이동: %d,%d", (int)vwPick.x, (int)vwPick.y );
				m_spSelectSquad->DoMoveSquad( vwPick );
			}
		}
		if( SCENE_BATTLE )
			SCENE_BATTLE->OnControlSquad( m_spSelectSquad->GetpHero() );
		m_spTempSelectEnemy.reset();	// 다썼으니 해제
	}
	m_vTouch.Set(-1.f);
	XEWndWorldImage::OnLButtonUp( lx, ly );
}

/**
 @brief lx, ly윈도우좌표의 부대를 픽킹한다.
 @param lx 윈도우좌표x
 @param ly 윈도우좌표y
 @param camp 픽킹조건진영
*/
XSPSquad XWndBattleField::PickingSquad( float lx, float ly, BIT bitCamp )
{
	// 윈도우 좌표를 월드좌표로 변환
	XE::VEC2 vTouch(lx, ly);
	XE::VEC3 vwPick = GetPosWindowToWorld( vTouch );
	ID snExcludeSquad = 0;
	if( m_spSelectSquad )
		snExcludeSquad = m_spSelectSquad->GetsnSquadObj();
	BOOL bPickExclude = FALSE;	// 제외부대도 픽킹될수 있는 자리인가.
	XSPUnit unitPick = XEObjMngWithType::sGet()->GetPickUnit( this, 
															vTouch, 
															bitCamp, 
															snExcludeSquad, 
															&bPickExclude );
	XSPSquad spSquad;
	if( unitPick == nullptr )	{
		// 해당 픽킹위치가 반경내에 들어가는 부대가 있는지 검사.
		spSquad = XBattleField::sGet()->GetPickSquad( vwPick, bitCamp );
		if( spSquad == nullptr )	// 이걸로도 못찾았는데
			if( bPickExclude )		// 이전선택된 부대는 픽킹되었다면
				return m_spSelectSquad;	// 이전선택된 부대를 돌려준다.
	} else	{
		// 유닛이 걸렸으면 그 유닛의 부대를 선택한다.
		spSquad = unitPick->GetspSquadObj();
	}
	return spSquad;
}

/**
 @brief pSquadObj가 현재 선택된 부대인가.
*/
BOOL XWndBattleField::IsSelectedSquad( XSquadObj *pSquadObj )
{
	if( m_spSelectSquad )	{
		if( m_spSelectSquad->GetsnSquadObj() == pSquadObj->GetsnSquadObj() )
			return TRUE;
	}
	return FALSE;
}

#ifdef _CHEAT
void XWndBattleField::OnReset( bool bReCreate )
{
	m_spSelectSquad.reset();
	m_spTempSelect.reset();
	m_spTempSelectEnemy.reset();
}
#endif // _CHEAT