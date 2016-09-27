#include "StdAfx.h"
#include "XSceneReady.h"
#include "XSceneBattle.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSkillMng.h"
#include "skill/XSkillDat.h"
#include "XPropHelp.h"
#include "XSquadron.h"
#include "XQuestMng.h"
#include "XLegion.h"
#include "XWndResCtrl.h"
#include "XHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneReady *SCENE_READY = NULL;

void XSceneReady::Destroy() 
{	
	XBREAK( SCENE_READY == NULL );
	XBREAK( SCENE_READY != this );
	SCENE_READY = NULL;
}

void XSceneReady::Release()
{
//	m_spParam.reset();
}

XSceneReady::XSceneReady( XGame *pGame, XSPSceneParam& spBaseParam ) 
	: XSceneBase( pGame, XGAME::xSC_READY )
	, m_Layout(_T("layout_ready_battle.xml"))
{ 
	XBREAK( SCENE_READY != NULL );
	SCENE_READY = this;
	Init(); 
	m_Layout.CreateLayout("ready", this);
	//
	// 전투씬 파라메터가 모두 세팅되어 있어야 함.
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
	auto& bs = XSceneBattle::sGetBattleStart();
//	m_spParam = std::static_pointer_cast<XGAME::xSPM_BATTLE>( spBaseParam );
	xSET_TEXT( this, "text.name.my", ACCOUNT->GetstrName() );
	xSET_TEXT( this, "text.name.enemy", bs.m_strName.c_str() );
	SetButtHander( this, "butt.start", &XSceneReady::OnClickStart );
	if( ACCOUNT->GetLevel() >= 10 ) {
		auto pButt = SetButtHander( this, "butt.edit", &XSceneReady::OnClickEdit );
		if( pButt )
			pButt->SetbEnable( true );
	} else {
		xSET_ENABLE( this, "butt.edit", false );
	}

#ifndef _XSINGLE
	SetButtHander( this, "butt.cancel", &XSceneReady::OnClickCancel, bs.m_idSpot );
#endif // not _XSINGLE
	// 아군 군단
//	m_spLegion[0] = m_spParam->spLegion[0];
// 	auto spMyLegion = bs.m_spLegion[0];
//	auto spEnemyLegion = bs.m_spLegion[1];
	XE::VEC2 vStart( 153, 47 );
	XE::VEC2 v = vStart;
	// 우측 적군단
//	m_spLegion[1] = m_spParam->spLegion[ 1 ];
	{
		auto spLegion = bs.m_spLegion[1];
		vStart.Set( 436, 47 );
		v = vStart;
		for( int i = 2; i >= 0; --i ) {
			for( int k = 0; k < 5; ++k ) {
				v.x = vStart.x + 64.f * i;
				v.y = vStart.y + 58.f * k;
				int idx = ( i * 5 + k );
				XSquadron *pSquad = spLegion->GetSquadron( idx );
				XHero *pHero = nullptr;
				bool bFog = false;
				if( pSquad ) {
					pHero = pSquad->GetpHero();
					XBREAK( pHero == nullptr );
					bFog = spLegion->IsFog( pHero->GetsnHero() );
					if( bFog )
						pHero = nullptr;
				}
				auto pWnd = new XWndSquadInLegion( pHero, v, spLegion.get(), true, false, bFog );
				ID idWnd = 200 + idx;
				pWnd->SetEvent( XWM_CLICKED, this, &XSceneReady::OnClickEnemySquad, idx );
				pWnd->SetEvent( XWM_DROP, this, &XSceneReady::OnDropSquad );
				Add( idWnd, pWnd );
				if( bFog ) {
					auto pWndGold = new XWndResourceCtrl( XE::VEC2( 7, 36 ) );
					pWndGold->AddRes( XGAME::xRES_GOLD, 10000 );
					pWndGold->SetScaleLocal( 0.8f );
					pWndGold->SetstrIdentifier( "ctrl.gold" );
					pWnd->Add( pWndGold );
					pWndGold->AutoLayoutHCenter();
				}
			}
		}
	}
	// 제한시간 30초
	if( bs.IsVsNpc() ) {
		xSET_SHOW( this, "text.time", false );
		m_timerLimit.Off();
		ClearAutoUpdate();
	} else {
		m_timerLimit.Set( 60.f, TRUE );
		SetAutoUpdate( 0.1f );
		xSET_SHOW( this, "text.time", true );
	}
	// 아군 리더를 미리 선택시켜둠
	int idxLeader = bs.m_spLegion[0]->GetIdxSquadByLeader();
	XWndSquadInLegion::sSetidxSelectedSquad( idxLeader );
	SetbUpdate( true );
	// 적 군단.
}

void XSceneReady::UpdateMyLegion()
{
	auto& bs = XSceneBattle::sGetBattleStart();
	const XE::VEC2 vStart( 153, 47 );
	XE::VEC2 v = vStart;
	auto spLegion = bs.m_spLegion[0];
	for( int i = 0; i < 3; ++i ) {
		for( int k = 0; k < 5; ++k ) {
			v.x = vStart.x + -64.f * i;
			v.y = vStart.y + 58.f * k;
			int idx = ( i * 5 + k );
			auto pSquad = spLegion->GetSquadron( idx );
			XHero *pHero = nullptr;
			if( pSquad )
				pHero = pSquad->GetpHero();
			auto pWndSquad = XWndSquadInLegion::sUpdateCtrl( this, idx, v, spLegion );
			if( pWndSquad ) {
				pWndSquad->SetFace( pHero );	// 영웅은 바뀔수 있으므로 별도로 처리함.
				pWndSquad->SetbPlayer( true );
				pWndSquad->SetEvent( XWM_CLICKED, this, &XSceneReady::OnClickMySquad, idx );
				pWndSquad->SetEvent( XWM_DROP, this, &XSceneReady::OnDropSquad );
			}
		}
	}
}

void XSceneReady::Create( void )
{
	XEBaseScene::Create();
}

int XSceneReady::Process( float dt ) 
{ 
  if( m_timerLimit.IsOver() ) {
    DoExit( XGAME::xSC_INGAME/*, m_spParam*/ );
    m_timerLimit.Off();
  }

	return XEBaseScene::Process( dt );
}

//
void XSceneReady::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneReady::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneReady::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneReady::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

void XSceneReady::Update()
{
	auto& bs = XSceneBattle::sGetBattleStart();
	// 좌측 아군
	UpdateMyLegion();
	// 리더스킬 표시
	xSET_SHOW( this, "wnd.leader.skill", true );
	xSET_SHOW( this, "text.no.leader", false );
	XHero *pLeader = bs.m_spLegion[0]->GetpLeader();
	if( pLeader ) {
		auto strPassive = pLeader->GetpProp()->strPassive;
		if( !strPassive.empty() ) {
			auto pSkillDat = SKILL_MNG->FindByIds( strPassive );
			if( pSkillDat ) {
				int nSkillLv = pLeader->GetlvPassive();
				_tstring strDesc;
				pSkillDat->GetSkillDesc( &strDesc, nSkillLv );
				xSET_TEXT( this, "text.skill.desc", strDesc.c_str() );
				xSET_TEXT( this, "text.skill.name", XFORMAT( "%s Lv%d", pSkillDat->GetSkillName(), nSkillLv ) );
				xSET_IMG( this, "img.skill.icon", XE::MakePath( DIR_IMG, pSkillDat->GetstrIcon() ) );
			}
		} else {
			xSET_SHOW( this, "wnd.leader.skill", false );
			xSET_SHOW( this, "text.no.leader", true );
		}
	} else {
		xSET_SHOW( this, "wnd.leader.skill", false );
		xSET_SHOW( this, "text.no.leader", true );
	}
	// 전투력 표시
	for( int i = 0; i < 2; ++i ) {
		auto pText = xGET_TEXT_CTRLF( this, "text.power.%d", i);
		if( pText ) {
			int powerExcude = XLegion::sGetMilitaryPower( bs.m_spLegion[i] );
			pText->SetNumberText( powerExcude );
		}
	}
	// 안개 비용 갱신,
	for( int idx = 0; idx < XGAME::MAX_SQUAD; ++idx ) {
		ID idWnd = 200 + idx;
		auto spLegion = bs.m_spLegion[1];
		auto pWndSquad = SafeCast2<XWndSquadInLegion*>( Find( idWnd ) );
		if( XASSERT(pWndSquad) ) {
			auto pSquad = spLegion->GetpSquadronByIdx( idx );
			if( pSquad ) {
				auto pHeroEnemy = pSquad->GetpHero();
				// 상성표시 처리
				auto idxSelected = XWndSquadInLegion::sGetidxSelectedSquad();
				// 선택된 아군 부대영웅
				auto pHeroSelected = ACCOUNT->GetCurrLegion()->GetpHeroByIdxSquad( idxSelected );
				if( pHeroSelected ) {
					int adj = 0;
					if( XGAME::IsSuperiorMOS( pHeroSelected->GetTypeAtk()
																	, pHeroEnemy->GetTypeAtk() ) )
							++adj;
					else
					if( XGAME::IsPenaltyMOS( pHeroSelected->GetTypeAtk()
																	, pHeroEnemy->GetTypeAtk() ) )
							--adj;
					if( XGAME::IsSuperiorSize( pHeroSelected->GetSizeUnit()
																	, pHeroEnemy->GetSizeUnit() ) )
							++adj;
					else
					if( XGAME::IsPenaltySize( pHeroSelected->GetSizeUnit()
																	, pHeroEnemy->GetSizeUnit() ) )
						--adj;
					pWndSquad->SetnSuperior( adj );
				}
				/// 안개처리
				bool bFog = spLegion->IsFog( pSquad->GetpHero()->GetsnHero() );
				if( bFog ) {
					// 안개가 있으면 안개가격 갱신
					auto pWndGold
						= SafeCast2<XWndResourceCtrl*>( pWndSquad->Find( "ctrl.gold" ) );
					if( pWndGold ) {
						int gold = ACCOUNT->GetCostOpenFog( spLegion );
						XCOLOR col = ACCOUNT->IsEnoughGold( gold ) ? XCOLOR_WHITE : XCOLOR_RED;
						pWndGold->EditRes( XGAME::xRES_GOLD, gold, col );
						pWndGold->SetbShow( true );
					}
				} else {
					// 안개가 없어짐. 혹은 원래 없었음.
					pWndSquad->SetFace( pSquad->GetpHero(), false );
					// 가격컨트롤 안보이게함.
					auto pWnd = pWndSquad->Find( "ctrl.gold" );
					if( pWnd )
						pWnd->SetbShow( false );
				}
			}
		}
	}

	//
	XEBaseScene::Update();
}

void XSceneReady::OnAutoUpdate()
{
	xSec secRemain = (xSec)m_timerLimit.GetRemainSec();
	if( secRemain < 0 )
		secRemain = 0;
	xSET_TEXT( this, "text.time", XFORMAT( "%s:%02d", XTEXT( 80131 ), secRemain ) );
}

//void XSceneReady::OnEndTransitionIn( XTransition *pTrans )
int XSceneReady::OnEnterScene( XWnd*, DWORD, DWORD )
{
// 	if( ACCOUNT->GetFlagTutorial().bReadyBattle == 0 ) {
// 		auto pAlert = XWND_ALERT( "%s", XTEXT(2204) );
// 		ACCOUNT->GetFlagTutorial().bReadyBattle = 1;
// 		GAMESVR_SOCKET->SendFlagTutorial();
// 		pAlert->SetEvent( XWM_OK, this, &XSceneReady::OnEnterScene );
// 	}
	// 부대상성 선택퀘가 있는상태만..
	if( ACCOUNT->GetpQuestMng()->GetQuestObj(_T("x.quest.unit.penalty")) ) {
		XWND_ALERT_T(XTEXT(70192));		// 부대선택하면 상성볼수 있음.
	}
	XSceneBase::OnEnterScene( nullptr, 0, 0 );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneReady::OnDropSquad( XWnd* pDropWnd, DWORD p1, DWORD p2 )
{
	XWnd *pDragWnd = Find(p2);
	CONSOLE("OnDropSquad");
	//
	if( pDragWnd == nullptr )
		return 1;
	auto pSlotSrc = static_cast<XWndSquadInLegion*>( pDragWnd );
	auto pSlotDst = static_cast<XWndSquadInLegion*>( pDropWnd );
	if( XBREAK( pSlotSrc == nullptr ) )
		return 1;
	if( XBREAK( pSlotDst == nullptr ) )
		return 1;
	// 드래그했다가 제자리에 놓은 경우.
	if( pSlotSrc->getid() == pSlotDst->getid() )
		return 1;
// 	int idxSrc = pSlotSrc->getid() - 100;
// 	int idxDst = pSlotDst->getid() - 100;
	int idxSrc = pSlotSrc->GetidxSquad();
	int idxDst = pSlotDst->GetidxSquad();
	ID snHeroDst = 0;
	if( pSlotDst->GetpHero() )
		snHeroDst = pSlotDst->GetpHero()->GetsnHero();
	if (pSlotSrc->GetpHero())
		MoveSquadInLegion( idxSrc, idxDst, pSlotSrc->GetpHero()->GetsnHero(),
											snHeroDst );
		XWndSquadInLegion::sSetidxSelectedSquad( idxDst );
	//
	GAME->DispatchEventToSeq( xHelp::xHE_DRAGGED_SLOT );
	SetbUpdate( true );
	
	return 1;
}

/**
 @brief 군단내에서 부대위치를 바꾸고 UI를 갱신한다.
*/
void XSceneReady::MoveSquadInLegion( int idxSrc, int idxDst, ID snHeroSrc, ID snHeroDst )
{
	if( XBREAK( idxSrc == -1 ) )
		return;
	if( XBREAK( idxDst == -1 ) )
		return;
	if( idxSrc == idxDst )
		return;
	auto& bs = XSceneBattle::sGetBattleStart();
	bs.m_spLegion[0]->SwapSlotSquad( idxSrc, idxDst );

	XHero *pHeroSrc = ACCOUNT->GetHero( snHeroSrc );
	XHero *pHeroDst = ACCOUNT->GetHero( snHeroDst );
	XWndSquadInLegion *pSlotSrc = GetWndSquadSlot( idxSrc );
	// 소스측 wnd가 없을순 없다.
	if( XBREAK( pSlotSrc == NULL ) )
		return;
	// swap slot상황
	XWndSquadInLegion *pSlotDst = GetWndSquadSlot( idxDst );
	// drop하는쪽은 drag한 윈도우의 영웅으로 넣어두고
	pSlotDst->SetFace( pSlotSrc->GetpHero() );
	// drag하는쪽은 drop한 윈도우의 영웅으로 바꿔준다.
	pSlotSrc->SetFace( pHeroDst );
}

/**
 @brief 부대슬롯 wnd를 찾는다.,
*/
XWndSquadInLegion* XSceneReady::GetWndSquadSlot( int idxSlot )
{
// 	XWnd *pWndSlot = Find( idxSlot + 100 );
	XWnd *pWndSlot = Find( XWndSquadInLegion::sGetIds( idxSlot ) );
	if( XBREAK( pWndSlot == NULL ) )
		return NULL;
	auto pSlot = SafeCast<XWndSquadInLegion*>( pWndSlot );
	return pSlot;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneReady::OnClickMySquad( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	int idxSquad = (int)p1;
	CONSOLE( "OnClickMySquad:idx=%d", idxSquad );
	auto pHero = ACCOUNT->GetCurrLegion()->GetpHeroByIdxSquad( idxSquad );
	if( pHero ) {
		XWndSquadInLegion::sSetidxSelectedSquad( idxSquad );
		auto pWndHeroes = SafeCast2<XWndSelectHeroesInReady*>( Find("wnd.heroes") );
		if( pWndHeroes ) {
			pWndHeroes->SetbDestroy( true );
			// 영웅선택창이 떠있는상태에서 다른 영웅을 클릭했으면 기존창을 죽이고 다시 띄운다.
			UpdateWndSelectHeroes();
		}
		SetbUpdate( true );
		if( ACCOUNT->GetFlagTutorial().bTouchSquadInReady == 0 ) {
			if( ACCOUNT->GetpQuestMng()->GetQuestObj( _T("x.quest.unit.penalty") ) ) {
				// 만약 서버에 보내는게 실패하더라도 다음 접속시에 다시 시도하게 되니 별문제 없을듯.
				ACCOUNT->GetFlagTutorial().bTouchSquadInReady = 1;
					GAMESVR_SOCKET->SendTouchSquadInReadyScene( idxSquad );
			}
		}
	}
	//
	return 1;
}

int XSceneReady::OnClickEnemySquad( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	int idx = (int)p1;
	CONSOLE( "OnClickMySquad:idx=%d", idx );
	//
	auto& bs = XSceneBattle::sGetBattleStart();
	auto spLegion = bs.m_spLegion[1];
	if( XASSERT(spLegion) ) {
		int goldCost = ACCOUNT->GetCostOpenFog( spLegion );
		if( ACCOUNT->IsEnoughGold( goldCost ) ) {
			auto pSquad = spLegion->GetpSquadronByIdx( idx );
			if( pSquad ) {
				auto pHero = pSquad->GetpHero();
				if( XASSERT(pHero) ) {
#ifndef _XSINGLE
					if( spLegion->IsFog( pHero->GetsnHero() ) )
						GAMESVR_SOCKET->SendReqClickFogSquad( GAME, 
																									bs.m_idSpot, 
																									pHero->GetsnHero(), 0 );
#endif // not _XSINGLE
				}
			}
		}
	}
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneReady::OnClickStart( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStart");
	//
	GAMESVR_SOCKET->SendReqChangeSquad( GAME, 
										ACCOUNT->GetCurrLegionIdx(), 
										ACCOUNT->GetCurrLegion().get() );
	GAMESVR_SOCKET->SendReqBattleStart( GAME );
	return 1;
}


/****************************************************************
* @brief 
*****************************************************************/
int XSceneReady::OnClickCancel( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickCancel");
	//
//	XArchive arParam;
	XGAME::xBattleFinish battle;
	battle.idSpot = p1;
	battle.ebCode = XGAME::xEB_CANCEL;
	battle.snSession = ACCOUNT->GetsnSession();
//	if( GAMESVR_SOCKET->SendReqFinishBattle( this, (ID)p1, XGAME::xEB_CANCEL, arParam ) )
	if( GAMESVR_SOCKET->SendReqFinishBattle( this, battle ) )
		DoExit( XGAME::xSC_WORLD );
	return 1;
}

void XSceneReady::RecvBattleStart()
{
	DoExit( XGAME::xSC_INGAME/*, m_spParam*/ );
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneReady::OnClickTutorial( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickTutorial");
	//
	ACCOUNT->GetFlagTutorial().bReadyBattle = 1;
	GAMESVR_SOCKET->SendFlagTutorial();
	return 1;
}

/**
 @brief 
*/
int XSceneReady::OnClickEdit( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickEdit");
	//
	auto& bs = XSceneBattle::sGetBattleStart();
	m_bEditMode = !m_bEditMode;
	int idxSquad = XWndSquadInLegion::sGetidxSelectedSquad();
	auto pHero = bs.m_spLegion[0]->GetpHeroByIdxSquad( idxSquad );
	if( pHero ) {
		// 영웅선택창을 띄운다.
		UpdateWndSelectHeroes();
		// 해당퀘를 가지고 있을때만 보냄.
		bool bHave = ACCOUNT->GetpQuestMng()->IsHaveQuestTypeWithParam( XGAME::xQC_EVENT_UI_ACTION
																																	, XGAME::xUA_EDIT_SQUAD
																																	, 0 );
		if( bHave )
			GAMESVR_SOCKET->SendUIAction( XGAME::xUA_EDIT_SQUAD, 0 );
	}
	SetbUpdate( true );
	return 1;
}
/**
 @brief 영웅선택창을 다시 생성한다.
*/
XWndSelectHeroesInReady* XSceneReady::UpdateWndSelectHeroes()
{
	auto& bs = XSceneBattle::sGetBattleStart();
	int idxSquad = XWndSquadInLegion::sGetidxSelectedSquad();
	auto pHeroSelected = bs.m_spLegion[ 0 ]->GetpHeroByIdxSquad( idxSquad );
	// 영웅선택 팝업 생성
	XBREAK( Find("wnd.heroes") );		// 기존에 이미 있으면 반드시 파괴시키고 다시 불러야함.
	auto pWndHeroes = new XWndSelectHeroesInReady( pHeroSelected );
	pWndHeroes->SetstrIdentifier( "wnd.heroes" );
	Add( pWndHeroes );
	return pWndHeroes;
}