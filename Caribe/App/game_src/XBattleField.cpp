#include "stdafx.h"
#include "XBattleField.h"
#include "XEObjMngWithType.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XLegion.h"
#include "XBaseUnit.h"
#include "XWndBattleField.h"
#include "XHero.h"
#ifdef _XSINGLE
#include "XAccount.h"
#endif // _XSINGLE
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif
#ifdef _CLIENT
#include "XGame.h"
#include "XSceneBattle.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XBattleField* XBattleField::s_pInstance = nullptr;

////////////////////////////////////////////////////////////////
XBattleField::XBattleField( const XE::VEC2& vSize )
	: XEWorld( vSize )
{
	XBREAK( XBattleField::sGet() != nullptr );
	XBattleField::s_pInstance = this;
	Init();
	// 타입별로 관리되는 오브젝트매니저를 사용한다.
	int max = XGAME::MAX_UNIT_SMALL * 50 + 200;
	SetObjMng( new XEObjMngWithType( max, XGAME::xOT_MAX ) );
}

void XBattleField::Destroy()
{
	//
	DestroyAllObj();
	XBattleField::s_pInstance = nullptr;
}

void XBattleField::Release( void )
{
// 	m_spLegionEnemy.reset();
// 	m_aryLegion[0]->Release();
// 	m_aryLegion[1]->Release();
	XEWorld::Release();
}

void XBattleField::DestroyAllObj( void )
{
// 	m_aryLegion.Clear();
	XEWorld::DestroyAllObj();
}

// BOOL XBattleField::CreateLegionObj( LegionPtr& spLegion
// 																, BIT bitSide, BOOL bDestroyLegion )
// {
// 	auto spLegionObj = XSPLegionObj( new XLegionObj( /*pAcc, */spLegion, bitSide, bDestroyLegion ) );
// 	m_aryLegion.Add( spLegionObj );
// 	if( m_aryLegion.size() == 2 )
// 		m_spLegionEnemy = spLegion;		// 싱글모드에서 군단객체 돌려쓰기 위해서는 파괴가 되지 말아야 해서 참조카운터증가용으로 받아둠.
// 	return TRUE;
// }
// 

/**
 
*/
int XBattleField::Process( XEWndWorld *pWndWorld, float dt )
{
	XPROF_OBJ_AUTO();
	auto& listUnit = XEObjMngWithType::sGet()->GetlistUnitsMutable();
	for( auto spUnit : listUnit ) {
		if( XASSERT( spUnit ) ) {
			spUnit->FlipMsgQ();
		}
	}
	if( GetpObjMng() )	{
		GetpObjMng()->FrameMoveDelegate( this, 4, dt );
		// 캐릭터상태와 보정치를 클리어한다.
		GetpObjMng()->FrameMoveDelegate( this, 1, dt );
		// XSkillUser::FrameMove()를 일괄 실행한다.
		GetpObjMng()->FrameMoveDelegate( this, 2, dt );
		// XSkillReceiver::FrameMove()를 일괄 실행한다.
		GetpObjMng()->FrameMoveDelegate( this, 3, dt );
	}
	GetLegionObj(0)->FrameMove( dt );
	GetLegionObj(1)->FrameMove( dt );
	//
	auto ret = XEWorld::Process( pWndWorld, dt );
	// 모든 프로세스가 끝난 후 유니트들은 각자 쌓인 메시지큐들을 처리한다.
	// 만약 위 프로세스에서 A가 B를 타격했다면 B가 피격받은 모습을 즉시 draw해야할거 같아서 process() 아래에 넣음.
	for( auto spUnit : listUnit ) {
		if( XASSERT(spUnit) ) {
			spUnit->ProcessMsgQ();
		}
	}
	return ret;
}

void XBattleField::OnDelegateFrameMoveEachObj( float dt, 
											ID idEvent, 
											WorldObjPtr spObj ) 
{
	if( XGAME::xOT_UNIT == spObj->GetClassType() ) {
		XBaseUnit *pUnit = SafeCast<XBaseUnit*, XEBaseWorldObj*>( spObj.get() );
		XBREAK( pUnit == nullptr );
		if( pUnit ) {
			switch( idEvent ) {
			case 1:
				pUnit->ClearAdjParam();
				pUnit->OnBeforeFrameMove();
				break;
			case 2:
				pUnit->XSkillUser::FrameMove( dt );
				break;
			case 3:
				pUnit->XSkillReceiver::FrameMove( dt );
				break;
			case 4:
				pUnit->ClearDebugAdjParam();

			}
		}
	}
	if( XGAME::xOT_SKILL_EFFECT == spObj->GetClassType() ) {
		auto pUnit = dynamic_cast<XSKILL::XSkillReceiver*>(spObj.get());
		XBREAK( pUnit == nullptr );
		if( pUnit ) {
			switch( idEvent ) {
			case 1:
				pUnit->ClearAdjParam();
				break;
			case 3:
				pUnit->XSkillReceiver::FrameMove( dt );
				break;
			case 4:
				pUnit->ClearDebugAdjParam();

			}
		}
	}
}

/**
 
*/
void XBattleField::Draw( XEWndWorld *pWndWorld )
{
	XEWorld::Draw( pWndWorld );
	GetLegionObj( 0 )->Draw( pWndWorld );
	GetLegionObj( 1 )->Draw( pWndWorld );
// 	m_aryLegion[0]->Draw( pWndWorld );
// 	m_aryLegion[1]->Draw( pWndWorld );
}

XSKILL::XSkillReceiver* XBattleField::GetTarget( ID snObj )
{
	WorldObjPtr* pspObj = GetpObjMng()->Find( snObj );
	if( pspObj )
	{
		if( XBREAK(pspObj->get() == nullptr ) )
			return nullptr;
		return dynamic_cast<XSKILL::XSkillReceiver*>( pspObj->get() );
	}
	return nullptr;
}
XSKILL::XSkillUser* XBattleField::GetCaster( ID snObj )
{
	WorldObjPtr* pspObj = GetpObjMng()->Find( snObj );
	if( pspObj )
	{
		if( XBREAK( pspObj->get() == nullptr ) )
			return nullptr;
		return dynamic_cast<XSKILL::XSkillUser*>( pspObj->get() );
	}
	return nullptr;
}

UnitPtr XBattleField::GetHeroUnit( ID idProp )
{
	auto spUnitHero = GetLegionObj(1)->GetHeroUnit( idProp );
	if( spUnitHero == nullptr ) {
		spUnitHero = GetLegionObj(0)->GetHeroUnit( idProp );
	}
	return spUnitHero;
}

/**
 @brief pFinder의 적군단에서 가장가까운 적부대를 찾는다.
*/
SquadPtr XBattleField::FindNearSquadEnemy( XSquadObj *pFinder )
{
	XSPLegionObj spEnemy;
	// pFinder의 적부대를 찾는다.
	if( pFinder->GetCamp().IsEnemy( GetLegionObj(0)->GetCamp() ) )
		spEnemy = GetLegionObj(0);
	else
		spEnemy = GetLegionObj(1);

	// 적 군단내에서 가장가까운 부대를 찾는다.
	return spEnemy->FindNearSquad( pFinder );

}

#if defined(_XSINGLE) || !defined(_CLIENT)
/**
 @brief 군단객체(XLegionObj)만 다시 만든다. XLegion은 그대로 쓴다.
*/
// void XBattleField::ResetLegionObj( /*XWndBattleField *pWndWorld*/ )
// {
// // 	auto spLegion = m_spLegionEnemy;
// //	auto spLegion = GetLegionObj( xSI_OTHER );
// 	Release();
// // 	m_spLegionEnemy = spLegion;
// 	DestroyAllObj();
// // 	auto spLegionPlayer = ACCOUNT->GetCurrLegion();
// // 	CreateLegionObj( spLegionPlayer, XGAME::xSIDE_PLAYER, FALSE );	// 아군 군대
// // 	CreateLegionObj( m_spLegionEnemy, XGAME::xSIDE_OTHER, TRUE );	// 적군 군대
// // 	CreateSquadObj( pWndWorld);
// }

/**
 @brief XLegion까지 새로 만든다.
*/
// void XBattleField::RecreateLegionObj( XWndBattleField *pWndWorld )
// {
// 	Release();
// 	DestroyAllObj();
// 
// 	ACCOUNT->DestroyLegion();
// 	ACCOUNT->DestroyHeros();
// //	SAFE_DELETE( pLegion[1] );
// 	m_spLegionEnemy.reset();
// //	m_aryLegion[1]->GetspLegion().reset();
// 	int lvAcc = 50;
// 	ACCOUNT->CreateLegionByRandom( 15, lvAcc );	// 어카운트의 군단을 새로 만든다.
// 	auto spLegion = ACCOUNT->GetCurrLegion();
// 	spLegion->SetpLeader( spLegion->GetpHeroByIdxSquad( 2 ) );
// 	XSceneBattle::sSetAbilHeroes();
// 	XGAME::xLegionParam info;
// 	info.unit = (XGAME::xtUnit)XAPP->m_nDebugRespawnUnit;
// 	info.bRandom = XAPP->m_bDebugRespawnNumRandom != FALSE;
// 	auto pLegion = XLegion::sCreateLegionForNPC( lvAcc, 0, info );
// 	m_spLegionEnemy = LegionPtr(pLegion);
// 	CreateLegionObj( ACCOUNT->GetCurrLegion(), XGAME::xSIDE_PLAYER, FALSE );	// 아군 군대
// 	CreateLegionObj( m_spLegionEnemy, XGAME::xSIDE_OTHER, TRUE );	// 적군 군대
// 	//
// 	CreateSquadObj( pWndWorld );
// }


/**
 @brief XLegion과 XLegionObj를 다시 만든다.
*/
void XBattleField::Clear()
{
	Release();
	DestroyAllObj();
}


#endif // defined(_XSINGLE) || !defined(_CLIENT)

/**
 @brief 기존 호환성을 위해 int로 남겨둠.
*/
XSPLegionObj XBattleField::GetLegionObj( int idx ) const
{
	const auto& camp = SCENE_BATTLE->GetCamp( (xtSideIndex)idx );
	return camp.m_spLegionObj;
}



// void XBattleField::CreateSquadObj( XWndBattleField *pWndWorld )
// {
// 	XE::VEC3 vCenterWorld = GetvwSize() / 2.f;
// 	vCenterWorld.y = 231.f;
// 	for( int i = 0; i < 2; ++i )
// 	{
// 		auto spLegionObj = m_aryLegion[i];
// 		// m_pLegion을 바탕으로 내부에서 분대 객체를 만듬. 분대객체안에서는 다시 유닛 객체를 만듬.
// #ifdef _CHEAT
// #ifdef _XSINGLE
// 		if( XAPP->GetbDebugMode() && XAPP->m_bDebugSquad )
// 		{
// 			auto bitOption = XGAME::xLO_DEFAULT;
// 			if( i == 0 )
// 				spLegionObj->CreateLegionDebug( pWndWorld, vCenterWorld - XE::VEC3( 240, 0 ), bitOption );
// 			else {
// 				spLegionObj->CreateLegionDebug( pWndWorld, vCenterWorld + XE::VEC3( 240, 0 ), bitOption );
// 				spLegionObj->MakeResource();
// 			}
// 		} else
// #endif // _XSINGLE
// 		{
// 			BIT bit = 0;
// 			if( i == 0 )
// 				spLegionObj->CreateLegion( pWndWorld, vCenterWorld - XE::VEC3( 240, 0 ), (XGAME::xtLegionOption)bit );
// 			else {
// 				if( m_typeBattle == XGAME::xBT_GUILD_RAID ) {
// 					bit |= XGAME::xLO_NO_CREATE_DEAD;		// 길드레이드시 적부대엔 사망부대는 생성하지 않음.
// 				}
// 				spLegionObj->CreateLegion( pWndWorld, vCenterWorld + XE::VEC3( 240, 0 ), (XGAME::xtLegionOption)bit );
// //				pLegionObj->MakeResource();
// 			}
// 
// 		}
// #else
// 		spLegionObj->CreateLegion( pWndWorld, vCenterWorld + XE::VEC3( 240, 0 ) );
// #endif // CHEAT
// 	}
// }

/**
 @brief 부대생성 치트기능
 vPos화면위치에 unit부대 하나를 생성한다.
*/
void XBattleField::SpawnSquadByCheat( const XE::VEC3& vwPos, XGAME::xtUnit unit, bool bEnemy )
{
#ifdef _XSINGLE
// 	XSPLegionObj spLegionObj;
// 	if( bEnemy )
// 		spLegionObj = m_aryLegion[1];
// 	else
// 		spLegionObj = m_aryLegion[0];
	const auto idxSide = (bEnemy)? xSI_OTHER : xSI_PLAYER;
	auto spLegionObj = SCENE_BATTLE->GetspLegionObjMutable( idxSide );

	_tstring strHeroName;
	auto pPropHero = PROP_HERO->GetPropRandomByAtkType( XGAME::GetAtkType(unit), 50 );
	if( pPropHero == nullptr )
		return;
	int numUnit = 1;
	auto pHero = new XHero( pPropHero, 1, unit );
	auto pSquadObj = new XSquadObj( spLegionObj, pHero, vwPos );
	pSquadObj->CreateSquad( XWndBattleField::sGet(), spLegionObj->GetCamp() );
	spLegionObj->AddSquad( SquadPtr( pSquadObj ) );
	pSquadObj->OnStartBattle();	// 치트로 생성한 부대는 첨에 전투시작 이벤트 한번 날려줌.
#endif // _XSINGLE

}

/**
 @brief 3,2,1,스타트!와 동시에 ai가 켜질때 이벤트.
*/
void XBattleField::OnStartBattle()
{
	for( int i = 0; i < 2; ++i ) {
		auto spLegionObj = SCENE_BATTLE->GetspLegionObjMutable( (xtSideIndex)i );
		if( XASSERT(spLegionObj) )  {
			spLegionObj->OnStartBattle();
			spLegionObj->OnAfterStartBattle();
		}
	}
}

void XBattleField::SetAI( bool bFlag )
{
	for( int i = 0; i < 2; ++i ) {
		auto spLegionObj = SCENE_BATTLE->GetspLegionObjMutable( (xtSideIndex)i );
		if( XASSERT( spLegionObj ) ) {
			spLegionObj->SetAI( bFlag );
		}
	}
}
/**
 @brief 군단이 전멸했다.
 @param pLegion 전멸한 군단.
*/
void XBattleField::OnDieLegion( XSPLegionObj spLegion )
{
//	CONSOLE("군단전멸");
	XGAME::xtSide bitCampWin;
	if( spLegion->GetCamp() == XGAME::xSIDE_PLAYER )
		bitCampWin = XGAME::xSIDE_OTHER;
	else
		bitCampWin = XGAME::xSIDE_PLAYER;

	SCENE_BATTLE->OnFinishBattle( bitCampWin, false );
}

void XBattleField::OnDieSquad( XSPSquad spSquadObj )
{
//	CONSOLE( "%s부대전멸", pSquadObj->GetpHero()->GetstrName().c_str() );
#ifndef _XSINGLE
	auto& aryLoots = spSquadObj->GetaryLoots();
	if( aryLoots.size() > 0 ) {
		auto spLegion = spSquadObj->GetspLegionObj()->GetspLegion();
		// 자원부대였으면 자원오브젝트를 떨어트린다.
		// 부대가 할당받은 떨어트릴양을 얻는다.ㅣ
		// 자원부대가 죽으면 자원오브젝트를 생성한다.
		if( spLegion ) {
			for( auto& res : aryLoots ) {
				auto pObj = new XObjRes( spSquadObj->GetvwPos(), res.type, (int)res.num );
				AddpObj( pObj );
			}
		}
	}
#else
	auto pObj = new XObjRes( spSquadObj->GetvwPos(), xRES_WOOD, 100 );
	AddpObj( pObj );
#endif // not _XSINGLE
	SCENE_BATTLE->OnDieSquad( spSquadObj );
}

SquadPtr XBattleField::GetPickSquad( const XE::VEC3& vwPick, 
																			BIT bitCamp, 
																			ID snExclude /*= 0*/ )
{
	auto spLegionObj = SCENE_BATTLE->GetspLegionObjMutable( (xtSide)bitCamp );
	return spLegionObj->GetPickSquad( vwPick, bitCamp, snExclude );
// 	if( bitCamp == XGAME::xSIDE_PLAYER )
// 		return m_aryLegion[0]->GetPickSquad( vwPick, bitCamp, snExclude );
// 	else
// 		return m_aryLegion[1]->GetPickSquad( vwPick, bitCamp, snExclude );
}

/**
 @brief src측 부대와 인접해있는 아군 부대를 모두 찾는다.
 아군이란 src의 아군을 의미한다.
*/
int XBattleField::GetNearSquad( XSquadObj *pSquadSrc, 
								XArrayLinearN<SquadPtr,64> *pOutAry, 
								float radius )
{
	auto spFriendly = pSquadSrc->GetspLegionObj();
	XBREAK( spFriendly == nullptr );
	return spFriendly->GetNearSquad( pSquadSrc, pOutAry, radius );
}


/**
 @brief src측 부대로부터 인접해있는 적 부대를 모두 찾는다.
 적이란 src의 상대적이다.
*/
int XBattleField::GetNearSquadEnemy( XSquadObj *pSquadSrc, 
								XArrayLinearN<SquadPtr,64> *pOutAry, 
								float radius )
{
	auto spEnemy = GetEnemyLegionObj( pSquadSrc );
	XBREAK( spEnemy == nullptr );
	return spEnemy->GetNearSquad( pSquadSrc, pOutAry, radius );
}

/**
 @brief pSrc부대의 적부대를 돌려준다.
*/
XSPLegionObj XBattleField::GetEnemyLegionObj( XSquadObj *pSrc )
{
	if( pSrc->GetCamp() == XGAME::xSIDE_PLAYER )
		return SCENE_BATTLE->GetspLegionObjMutable( xSIDE_OTHER );
	else
		return SCENE_BATTLE->GetspLegionObjMutable( xSIDE_PLAYER );
// 	return SCENE_BATTLE->GetspLegionObjMutable( (xtSide)pSrc->GetCamp().GetbitCamp() );
// 	if( pSrc->GetCamp() == XGAME::xSIDE_PLAYER )
// 		return m_aryLegion[1];
// 	return m_aryLegion[0];
}

SquadPtr XBattleField::GetSquadBySN( XGAME::xtLegionIdx idxLegion, ID snSquad )
{
	auto spLegionObj = SCENE_BATTLE->GetspLegionObjMutable( (xtSideIndex)idxLegion );
	return (spLegionObj)? spLegionObj->GetSquadBySN( snSquad ) : nullptr;
//	return m_aryLegion[idxLegion]->GetSquadBySN( snSquad );
}

void XBattleField::DrawLegionBar( const XE::VEC2& vPos, int idxLegion )
{
	auto spLegionObj = GetLegionObj( idxLegion );
	if( spLegionObj )
		spLegionObj->DrawSquadsBar( vPos );
// 	auto pLegion = m_aryLegion[ idxLegion ];
// 	if( pLegion ) {
// 		pLegion->DrawSquadsBar( vPos );		
// 	}
}

WorldObjPtr XBattleField::AddpObj( XEBaseWorldObj *pNewObj )	
{
	auto spObj = WorldObjPtr( pNewObj );
	XEWorld::AddObj( spObj );
	return spObj;
}

void XBattleField::SetLootRes( const XVector<XGAME::xRES_NUM>& aryLoots ) 
{
	m_aryLoots = aryLoots;
}
