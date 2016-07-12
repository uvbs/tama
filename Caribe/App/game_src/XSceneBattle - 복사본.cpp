#include "StdAfx.h"
#include "XSceneBattle.h"
#include "XGame.h"
#include "XSockGameSvr.h"
#include "XGameWnd.h"
#include "XBattleField.h"
#include "XWndBattleField.h"
#include "XBaseUnit.h"
#include "XLegionObj.h"
#include "XSceneProcess.h"
#include "XUnitArcher.h"
#include "XUnitCyclops.h"
#include "XUnitTreant.h"
#include "XObjEtc.h"
#include "XSkillMng.h"
#include "XSquadObj.h"
#include "XUnitHero.h"
#include "XBaseItem.h"
#include "XFramework/client/XClientMain.h"
#include "XParticleFramework.h"
#include "XFramework/client/XPropParticle.h"
#include "XSpots.h"
#include "XAccount.h"
#include "XQuestMng.h"
#include "XWorld.h"
#ifdef _XSINGLE
#include "XLegion.h"
#endif // _XSINGLE
#include "XSoundMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneBattle *SCENE_BATTLE = nullptr;
XGAME::xBattleStart XSceneBattle::s_BattleStart;

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XSceneBattle::XSceneBattle( XGame *pGame/*, SceneParamPtr& spBaseParam*/ ) 
	: XSceneBase( pGame, XGAME::xSC_INGAME )
	, m_Layout(_T("layout_battle.xml"))
{ 
	// 전투때는 필요치 않으므로 날린다.
	IMAGE_MNG->DoForceDestroy( PATH_UI("bg_popup_dark.png") );
	IMAGE_MNG->DoForceDestroy( PATH_UI("map.png") );
	//
	XBREAK( SCENE_BATTLE != nullptr );
	SCENE_BATTLE = this;
	Init(); 
	m_Layout.CreateLayout("world", this);
	// 파라메터가 세팅안되어 있으면 에러.
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
#ifdef _XSINGLE
#else
	// 싱글모드가 아니면 스팟아이디는 모두 있어야 함.
	XSpot *pSpot = sGetpWorld()->GetSpot( s_BattleStart.m_idSpot );
	XBREAK( pSpot == nullptr );
#endif
	// 결과는 OnRecvReconSpot로 온다
	//
#ifdef _XMEM_POOL
	XUnitCommon::s_pPool = new XPool<XUnitCommon>( XGAME::MAX_UNIT_SMALL * 30 );
	XObjArrow::s_pPool = new XPool<XObjArrow>( XGAME::MAX_UNIT_SMALL * 30 );
#endif // _XMEM_POOL
	// XBattleField(XWorld)객체 생성
	m_pWorld = XWndBattleField::sGet();
#ifdef _XSINGLE
	auto strBg = XGC->GetBgBattle( XGAME::xSPOT_NPC );
#else
	auto strBg = XGC->GetBgBattle( s_BattleStart.m_typeSpot );
#endif // _XSINGLE
	m_pWorld->LoadImg( XE::MakePath( DIR_IMG, strBg ) );
	Add( m_pWorld );
	m_pWorld->SetScaleCamera(0.5f);	// ( 0.5f );
	m_pWorld->SetFocus( XE::VEC2( 956, 450 ) );
	// 전투타입을 지정한다.
	XBattleField::sGet()->SettypeBattle( s_BattleStart.m_typeBattle );
	//
	m_pWorld->GetprefBattleField()
		->CreateLegionObj( s_BattleStart.m_spLegion[ 0 ], XGAME::xSIDE_PLAYER, FALSE );	// 아군 군대
	m_pWorld->GetprefBattleField()
		->CreateLegionObj( s_BattleStart.m_spLegion[ 1 ], XGAME::xSIDE_OTHER, TRUE );	// 적군 군대
	auto pLegionObj = m_pWorld->GetprefBattleField()->GetLegionObj( 0 );
	if( pLegionObj ) {
		if( s_BattleStart.m_Defense > 0 ) {
#ifndef _XSINGLE
			XASSERT( s_BattleStart.m_typeSpot == XGAME::xSPOT_JEWEL );
			float defAdd = ( s_BattleStart.m_Defense / 500000.f ) * 100.f;
			pLegionObj->SetDef( defAdd );
#endif // not _XSINGLE
		}
	}
	XObjDamageNumber::s_strFont = FONT_BADABOOM;
	CreateBattleUI();
	if( ACCOUNT->GetLevel() < 10 ) {
		xSET_SHOW( this, "butt.play", false );
	} else {
		xSET_SHOW( this, "butt.play", true );
		SetButtHander( this, "butt.play", &XSceneBattle::OnClickPlay );
	}
//	XBREAK( m_pEnemy == nullptr );
// 	XParticleMng::sGet()->SetpDelegate( this );

	// 분수형태 이미터
	XE::VEC2 sizeWorld = m_pWorld->GetpWorld()->GetvwSize();
// 	auto pPropParticle = XPropParticle::sGet()->GetpProp( "bg_sulfur" );
// 	if( pPropParticle ) {
// 		XParticleMng::sGet()->CreateSfx( "bg_sulfur", sizeWorld / 2.f );
// 	}
	//
	CreateParticleSfx();
// 	XE::VEC2 v(1377,821);
// 	XParticleMng::sGet()->CreateSfx( "bg_sulfur", v );
// 	XParticleMng::sGet()->SetBlendFunc( XE::xBF_ADD );
// 	auto pEmitter = new XEmitter( sizeWorld/2.f, new XSprObj( _T( "particle.spr" ) ), 0 );
// 	pEmitter->AddSpeedComponent( new xParticle::XCompFuncRandom( 1.f, 2.f ) );
// 	pEmitter->AddAngleComponent( new xParticle::XCompFuncLinear( 225.f, 315.f, 1.f ) );
// 	pEmitter->AddScaleComponent( new xParticle::XCompFuncFixed( 1.f ) );
// 	XParticleMng::sGet()->AddEmitter( pEmitter );
// 	//
// 	pEmitter = new XEmitter( sizeWorld/2.f, _T( "eff_hit01.spr" ) );
// 	pEmitter->AddSpeedComponent( new xParticle::XCompFuncRandom( 1.f, 2.f ) );
// 	pEmitter->AddAngleComponent( new xParticle::XCompFuncLinear( 45.f, 135.f, 1.f ) );
// 	pEmitter->AddScaleComponent( new xParticle::XCompFuncFixed( 1.f ) );
// 	XParticleMng::sGet()->AddEmitter( pEmitter );
	SetbUpdate( true );
}

void XSceneBattle::Destroy()
{
	SOUNDMNG->RemoveAll();
	XWndBattleField::sDestroy();
	XBREAK( SCENE_BATTLE == nullptr );
	XBREAK( SCENE_BATTLE != this );
	SCENE_BATTLE = nullptr;
#ifdef _XMEM_POOL
	SAFE_DELETE( XUnitCommon::s_pPool );
#endif // _XMEM_POOL
}

void XSceneBattle::Create( void )
{
	XEBaseScene::Create();
}

BOOL XSceneBattle::OnCreate()
{
	CreateDebugButtons();
#ifdef _XSINGLE
	sSetAbilHeroes();
#endif // _XSINGLE
// #ifdef _XUZHU
// #ifdef _DEBUG
// #ifdef _XSINGLE
// 	auto pHero = ACCOUNT->GetCurrLegion()->GetpLeader();
// 	XGAME::xtUnit unit = XGAME::xUNIT_LYCAN;
// 	auto pNode = XPropTech::sGet()->GetpNodeBySkill( unit, _T("poison_claw") );
// // 	auto& abil = pHero->GetAbilNode( unit, pNode->idNode );
// 	pHero->SetAbilPoint( unit, pNode->idNode, 5 );
// //  	auto pAbil = ACCOUNT->GetAbilNode( unit, pNode->idNode );
// //  	pAbil->point = 1;
// // 	pNode = XPropTech::sGet()->GetpNodeBySkill( unit, _T( "cuticle_up" ) );
// // 	pAbil = ACCOUNT->GetAbilNode( unit, pNode->idNode );
// // 	pAbil->point = 1;
// // 	pNode = XPropTech::sGet()->GetpNodeBySkill( unit, _T( "gaiant_rock" ) );
// // 	pAbil = ACCOUNT->GetAbilNode( unit, pNode->idNode );
// // 	pAbil->point = 1;
// #endif // single	
// #else
// //#error("빌드전에 위 하드코드 지울것");
// #endif
// #endif
	// 군단 객체를 만들고 유닛객체를 만들어 군단객체에 넣는다.
//	XLegionObj *pLegionObj = new XLegionObj( m_aryLegion[0], FALSE );
	XBattleField::sGet()->CreateSquadObj( m_pWorld );;
	// OnEnterScene에서 시작
	SetbUpdate( TRUE );

	return TRUE;
}

#ifdef _XSINGLE
void XSceneBattle::sSetAbilHeroes()
{
	auto spLegion = ACCOUNT->GetCurrLegion();
	auto pHero = spLegion->GetpLeader();
	sSetAbilHero( pHero, XGAME::xUNIT_LYCAN, _T( "poison_claw" ), 5 );
}
void XSceneBattle::sSetAbilHero( XHero *pHero, XGAME::xtUnit unit, LPCTSTR idsAbil, int point )
{
	auto pNode = XPropTech::sGet()->GetpNodeBySkill( unit, idsAbil );
	pHero->SetAbilPoint( unit, pNode->idNode, point );
}
#endif // _XSINGLE

void XSceneBattle::CreateDebugButtons( void )
{
#ifdef _CHEAT
//	if( XAPP->m_bDebugMode )
	{
		XE::VEC2 v(350,0);
		XE::VEC2 size(25,25);
		auto pButt = new XWndButtonDebug( v, size,
													_T( "다시" ),
													XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier("butt.debug.retry");
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugRetry, 1 );
		Add( pButt );
		v.x += size.w;
#if defined(_XSINGLE)
		pButt = new XWndButtonDebug( v.x, v.y,
															size.w, size.h,
															_T( "재생성" ),
															XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.recreate" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugRecreate, 1 );
		Add( pButt );
		v.x += size.w;
#endif
//		v.y += size.h + 2.f;
#if defined(_XPROFILE) && defined(_XUZHU)
		pButt = new XWndButtonDebug( v, size, _T( "profile" ) );
		pButt->SetstrIdentifier( "butt.debug.profile" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugProfile, 1 );
		Add( pButt );
		v.x += size.w;
#endif
		pButt = new XWndButtonDebug( v.x, v.y,
																size.w, size.h,
																_T( "kill" ),
																XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.allkill" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugAllKill, 1 );
		Add( pButt );
		v.x += size.w;
#ifdef _XSINGLE
		pButt = new XWndButtonDebug( v, size,
									_T( "특성" ),
									XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.tech" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugAbil, 1 );
		Add( pButt );
		v.x += size.w;
#endif // _XSINGLE
    pButt = new XWndButtonDebug( v.x, v.y,
                                size.w, size.h,
                                _T( "damage" ),
      XE::GetMain()->GetpGame()->GetpfdSystem() );
    pButt->SetstrIdentifier( "butt.debug.show.damage" );
    pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugAllKill, 2 );
    Add( pButt );
		v.x += size.w;


	}
#endif // CHEAT
}

void XSceneBattle::Update()
{
// 	m_hpMaxLegion[ 0 ] = XBattleField::sGet()->GetLegionObj( 0 )->GetMaxHpAllSquad();
// 	m_hpMaxLegion[ 1 ] = XBattleField::sGet()->GetLegionObj( 1 )->GetMaxHpAllSquad();

	auto pWndButt = Find("butt.play");
	if( pWndButt ) {
		switch( m_mulPlay ) {
		case 1:
//			xSET_SHOW( pWndButt, "img.text.x1", true );
			xSET_SHOW( pWndButt, "img.text.x2", false );
			xSET_SHOW( pWndButt, "img.text.x4", false );
			break;
		case 2:
//			xSET_SHOW( pWndButt, "img.text.x1", false );
			xSET_SHOW( pWndButt, "img.text.x2", true );
			xSET_SHOW( pWndButt, "img.text.x4", false );
			break;
		case 4:
//			xSET_SHOW( pWndButt, "img.text.x1", false );
			xSET_SHOW( pWndButt, "img.text.x2", false );
			xSET_SHOW( pWndButt, "img.text.x4", true );
			break;
		}
	}
#ifdef _CHEAT
	xSET_SHOW( this, "butt.debug.retry", XAPP->m_bDebugMode );
	xSET_SHOW( this, "butt.debug.recreate", XAPP->m_bDebugMode );
	xSET_SHOW( this, "butt.debug.profile", XAPP->m_bDebugMode );
	xSET_SHOW( this, "butt.debug.allkill", XAPP->m_bDebugMode );
	xSET_SHOW( this, "butt.debug.tech", XAPP->m_bDebugMode );
	xSET_SHOW( this, "butt.debug.show.damage", XAPP->m_bDebugMode );
#endif
	XEBaseScene::Update();
}

/**
 씬 프로세스가 끝났다.
*/
void XSceneBattle::OnEndSceneProcess( XSceneProcess *pProcess )
{
	if( pProcess->GetstrIdentifier() == "scene.process.ready" )
	{
		pProcess->SetbDestroy( TRUE );
		m_pProcess = new XSceneProcessBattle( this, m_pWorld->GetprefBattleField() );
		m_pProcess->SetstrIdentifier("scene.process.battle");
		Add( m_pProcess );
		BOOL bRealTimer = TRUE;
#ifdef _DEBUG
		bRealTimer = FALSE;	// 디버깅중엔 편의상 리얼타이머를 사용하지 않음.
#endif
		m_timerPlay.Set( 60 * 2, bRealTimer );    // 제한시간 2분
	} else
	if( pProcess->GetstrIdentifier() == "scene.process.battle" )
	{
//		DoExit( XGAME::xSC_WORLD );
		// 레디씬 다시
//		m_pProcess = new XSceneProcessReady( this );
//		m_pProcess->SetstrIdentifier( "scene.process.ready" );
//		Add( m_pProcess );
	}
}

// 씬이 모두 밝아지고 호출됨
int XSceneBattle::OnEnterScene( XWnd*, DWORD idSeqPopup, DWORD )		// 씬이 모두 밝아지고 호출됨
{
#ifdef _XSINGLE
	// 컷씬이 없으면 3,2,1시작
	OnEnterBattle();
#else // _XSINGLE
	// 로딩이 완전히 끝난 이곳에서부터 3초타이머를 설정해야한다 안그러면 프레임스키핑으로 인해 첫프레임이 튀게 된다.
	// 컷씬 이나 팝업
	if( !GAME->IsPlayingSeq() ) {
		if( IsDialogMsg( idSeqPopup ) ) {
			// 대기
		} else {
			// 전투가 벌어지는 스팟을 파라메터로 넘겨 특정스팟조건의 이벤트가 있으면 실행하도록 한다.
			const DWORD dwParam2 = s_BattleStart.m_idSpot;
			XSceneBase::OnEnterScene( nullptr, 0, dwParam2 );
			// ENTER_SCENE이벤트로 컷씬이 시작되었으면
			if( GAME->IsPlayingSeq() ) {
				// 대기
			} else {
				// 컷씬이 없으면 3,2,1시작
				OnEnterBattle();
			}
		}
	}
#endif // not _XSINGLE
	return 1;
}
/**
 @brief 이 전투에 컷씬 대화가 있는가.
 @return 팝업을 띄우지 않았으면 false리턴
*/
bool XSceneBattle::IsDialogMsg( DWORD idSeqPopup )
{
	if( idSeqPopup == 0 ) {
		if( ACCOUNT->GetFlagTutorial().bControlSquadInBattle == 0 ) {
			// 부대컨트롤퀘가 있을때만 
			if( ACCOUNT->GetpQuestMng()->GetQuestObj( _T( "x.quest.battle.control" ) ) ) {
				auto pWndPopup = XWND_ALERT_T( XTEXT( 70088 ) );	// 부대를 컨트롫보십시오.
				if( pWndPopup ) {
					pWndPopup->SetEvent( XWM_OK, this, &XSceneBattle::OnEnterScene, 1 );
					return true;
				}
			}
		}
	} else
	if( idSeqPopup == 1 ) {
		// popup2
	}
	return false;
}
/**
 @brief 3,2,1로 들어가는 지점
*/
void XSceneBattle::OnEnterBattle()
{
	// 3,2,1,씬
	CreateProcessReady();
}
/**
 @brief 
 컷씬이 종료된 후 서버로 가서 승인받고 다시 클라로 내려와서 xAE_END_SEQ이벤트로 컷씬을 다시 찾을때 
 더이상 컷씬이 없으면 호출됨.
 보통 ENTER_SCENE컷씬이벤트가 끝나면 호출.

*/
int XSceneBattle::OnEndCutScene( const std::string& idsEndSeq )
{
	OnEnterBattle();
	return 1;
}

// int XSceneBattle::OnEnterScene( XWnd*, DWORD, DWORD )		// 씬이 모두 밝아지고 호출됨
// {
// 	// 로딩이 완전히 끝난 이곳에서부터 3초타이머를 설정해야한다 안그러면 프레임스키핑으로 인해 첫프레임이 튀게 된다.
// 	XSceneBase::OnEnterScene( nullptr, 0, 0 );
// 	// 컷씬 이나 팝업
// 	if( !GAME->IsPlayingSeq() ) {
// 		bool bMsgPopup = false;
// 		// 컷씬실행중이 아니라면.
// 		if( ACCOUNT->GetFlagTutorial().bControlSquadInBattle == 0 ) {
// 			// 부대컨트롤퀘가 있을때만 
// 			if( ACCOUNT->GetpQuestMng()->GetQuestObj(_T("x.quest.battle.control")) ) {
// 				bMsgPopup = true;
// 			}
// 		}
// 		if( bMsgPopup ) {
// 			// 메시지 팝업을 띄워야 하면.
// 			auto pWndPopup = XWND_ALERT_T( XTEXT( 70088 ) );	// 부대를 컨트롫보십시오.
// 			if( pWndPopup ) {
// 				pWndPopup->SetEvent( XWM_OK, this, &XSceneBattle::OnClickOkByTutorialPopup );;
// 			}
// 		} else {
// 			// 달리 메시지팝업이 없으면 
// 			if( IsDialogMsg() ) {
// 				// 대화상태가 있으면 대화를 먼저 띄움.
// 			} else {
// 				// 3,2,1씬 시작
// 				CreateProcessReady();
// 			}
// 		}
// 	}
// 
// 	if( XASSERT( !IsPlayingSeq() ) ) {		// 이런일은 없겠지마는.
// 		if( 팝업 띄워야 할게 있는가 ) {
// 			팝업 = XWND_POPUP();
// 			팝업->클릭( OnEnterScene );
// 		}
// 		else {
// 			// 팝업 띄워야 하는거 다 끝난다음에 EnterScene이벤트 전달.
// 			XSceneBase::OnEnterScene();	// Seq이벤트 전달.
// 		}
// 
// 	}
// 
// 	return 1;
// }
/**
 @brief 3,2,1 나오는 씬
*/
void XSceneBattle::CreateProcessReady()
{
	m_pProcess = new XSceneProcessReady( this );
	m_pProcess->SetstrIdentifier( "scene.process.ready" );
	Add( m_pProcess );
}

/**
 @brief 
*/
// int XSceneBattle::OnClickOkByTutorialPopup( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickOkByTutorialPopup");
// 	// 3,2,1,씬
// 	CreateProcessReady();
// 	return 1;
// }


int XSceneBattle::Process( float dt ) 
{ 
	if( m_FocusMng.IsScrolling() ) {
		auto vCurr = m_FocusMng.GetCurrFocus();
		m_pWorld->SetFocus( vCurr );
	}
	for( int i = 0; i < 2; ++i ) {
		auto pLegionObj = XBattleField::sGet()->GetLegionObj( i );
		if( pLegionObj )
			m_hpMaxLegion[ i ] = pLegionObj->GetMaxHpAllSquad();
	}
	float hpLegion[2];
	hpLegion[0] = XBattleField::sGet()->GetLegionObj(0)->GetSumHpAllSquad();
	hpLegion[1] = XBattleField::sGet()->GetLegionObj(1)->GetSumHpAllSquad();
	float hpMax[2];
	hpMax[0] = m_hpMaxLegion[0];
	hpMax[1] = m_hpMaxLegion[1];
	// 일시적으로 hp가 max치를 넘어가는일이 생기면 hp치를 잠시 바꿔서 계산함.
	if( hpLegion[0] > hpMax[0] )
		hpMax[0] = hpLegion[0];
	if( hpLegion[1] > hpMax[1] )
		hpMax[1] = hpLegion[1];
	if( m_pAllyBar )
		m_pAllyBar->SetLerp( hpLegion[0] / hpMax[0] );
	if( m_pEnemyBar )
		m_pEnemyBar->SetLerp( hpLegion[1] / hpMax[1] );

	// 이미 전투가 끝났으면 다시 들어가지 않음.
	if( m_bFinish == false ) {
		// 시간 업데이트
		xSec sec = (DWORD)m_timerPlay.GetRemainSec();
		int h, m, s;
		XSYSTEM::GetHourMinSec( sec, &h, &m, &s );
		xSET_TEXT( this, "text.battle.time", XE::Format( _T( "%02d:%02d" ), m, s ) );
		//	xSET_SHOW( this, "text.battle.time", FALSE );
		if( m_timerPlay.IsOver() ) {
			m_timerPlay.Off();
			// 시간오버되면 패배
			OnFinishBattle( XGAME::xSIDE_OTHER, false );
		}
	}

	return XEBaseScene::Process( dt );
}

//
void XSceneBattle::Draw( void ) 
{
	// 아군 & 적군 게이지 업데이트
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
	//
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )	{
		auto pb = XBattleField::sGet();
 		PUT_STRINGF( 201, 25, XCOLOR_WHITE, "%d/%d", (int)pb->GetLegionObj(0)->GetSumHpAllSquad(), (int)m_hpMaxLegion[0] );
 		PUT_STRINGF( 384, 25, XCOLOR_WHITE, "%d/%d", (int)pb->GetLegionObj(1)->GetSumHpAllSquad(), (int)m_hpMaxLegion[1] );
	}
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewSquadInfo )	{
		SquadPtr spSquad = WND_WORLD->GetspSelectSquad();
		if( spSquad != nullptr )		{
			XBaseUnit *pUnit = spSquad->GetLiveMember();
			if( pUnit )			{
				const float distY = 8.f;
				XE::VEC2 v(539+23,145+36);
				_tstring str;
				str = XE::Format( _T( "장군:%s" ), spSquad->GetpHero()->GetstrName().c_str() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "유닛:%s" ), XGAME::GetStrUnit( spSquad->GetUnitType() ) );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "근접공격력:%.1f" ), pUnit->GetAttackMeleeDamage( UnitPtr() ) );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "원거리공격력:%.1f" ), pUnit->GetAttackRangeDamage( UnitPtr() ) );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "곡격속도:%.1f" ), pUnit->GetSpeedAttack( pUnit->GetspTarget() ) );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "방어력:%.1f" ), pUnit->GetDefensePower() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "체력:%d/%d" ), pUnit->GetHp(), pUnit->GetMaxHp() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "이동속도:%.1f" ), pUnit->GetSpeedMoveForPixel() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = _T( "---------------------" );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "치명타율:%.1f%%" ), pUnit->GetCriticalRatio() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				//str = XE::Format( _T( "회피율:%.1f%%" ), pUnit->GetEvadeRatio( XSKILL::xDMG_NONE ) );
				str = XE::Format(_T("회피율:%.1f%%"), pUnit->GetEvadeRatio( XSKILL::xDMG_NONE, nullptr ));
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "치명타배수:x%.1f" ), pUnit->GetCriticalPower() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "관통율:%.1f%%" ), pUnit->GetPenetrationRatio() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는모든피해율:%.1f%%" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_DAMAGE_RECV )->valPercent ) * 100.f );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는근접피해율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_DAMAGE_RECV )->valPercent) * 100.f);
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는원거리피해율:%.1f%%" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_DAMAGE_RECV )->valPercent ) * 100.f );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는모든치명타율:%.1f%%" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_CRITICAL_RECV )->valPercent ) * 100.f );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는근접치명타율:%.1f%%" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_CRITICAL_RECV )->valPercent ) * 100.f );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "받는원거리치명타율:%.1f%%" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_CRITICAL_RECV )->valPercent ) * 100.f );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
				str = XE::Format( _T( "타격카운트:%d"), pUnit->GetcntAttack() );
				PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
				v.y += distY;
// 				str += XE::Format( _T( "장군:%s\n" ), spSquad->GetpHero()->GetstrName().c_str() );
// 				str += XE::Format( _T( "유닛:%s\n" ), XGAME::GetStrUnit( spSquad->GetUnitType() ) );
// 				str += XE::Format( _T( "근접공격력:%.1f\n" ), pUnit->GetAttackMeleeDamage( UnitPtr() ) );
// 				str += XE::Format( _T( "원거리공격력:%.1f\n" ), pUnit->GetAttackRangeDamage( UnitPtr() ) );
// 				str += XE::Format( _T( "곡격속도:%.1f\n" ), pUnit->GetSpeedAttack( pUnit->GetspTarget() ) );
// 				str += XE::Format( _T( "방어력:%.1f\n" ), pUnit->GetDefensePower() );
// 				str += XE::Format( _T( "체력:%d/%d\n" ), pUnit->GetHp(), pUnit->GetMaxHp() );
// 				str += XE::Format( _T( "이동속도:%.1f\n" ), pUnit->GetSpeedMoveForPixel() );
// 				str += _T( "---------------------\n" );
// 				str += XE::Format( _T( "치명타율:%.1f%%\n" ), pUnit->GetCriticalRatio() );
// 				//str += XE::Format( _T( "회피율:%.1f%%\n" ), pUnit->GetEvadeRatio( XSKILL::xDMG_NONE ) );
// 				str += XE::Format(_T("회피율:%.1f%%\n"), pUnit->GetEvadeRatio( XSKILL::xDMG_NONE, nullptr ));
// 				str += XE::Format( _T( "치명타배수:x%.1f\n" ), pUnit->GetCriticalPower() );
// 				str += XE::Format( _T( "관통율:%.1f%%\n" ), pUnit->GetPenetrationRatio() );
// 				str += XE::Format( _T( "받는모든피해율:%.1f%%\n" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_DAMAGE_RECV )->valPercent ) * 100.f );
// 				str += XE::Format( _T( "받는근접피해율:%.1f%%\n" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_DAMAGE_RECV )->valPercent) * 100.f);
// 				str += XE::Format( _T( "받는원거리피해율:%.1f%%\n" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_DAMAGE_RECV )->valPercent ) * 100.f );
// 				str += XE::Format( _T( "받는모든치명타율:%.1f%%\n" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_CRITICAL_RECV )->valPercent ) * 100.f );
// 				str += XE::Format( _T( "받는근접치명타율:%.1f%%\n" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_CRITICAL_RECV )->valPercent ) * 100.f );
// 				str += XE::Format( _T( "받는원거리치명타율:%.1f%%\n" ), ( 1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_CRITICAL_RECV )->valPercent ) * 100.f );
// 				str += XE::Format( _T( "타격카운트:%d\n"), pUnit->GetcntAttack() );
// 				PUT_STRING_SMALL( 539+23, 145+36, XCOLOR_WHITE, str.c_str() );
			}
		}
	}
	// 부대원 hpbar 보이기
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewSquadsHp ) {
		auto pBattleField = m_pWorld->GetprefBattleField();
		if( pBattleField ) {
			pBattleField->DrawLegionBar( XE::VEC2( 10, 56 ), 0 );
			pBattleField->DrawLegionBar( XE::VEC2( (int)XE::GetGameWidth()-(48+10), 56 ), 1 );
		}
	}
#endif

}

void XSceneBattle::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneBattle::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneBattle::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

// int XSceneBattle::OnReconOk( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	// 전투가 끝나고 스킬사용등의 플레이 데이타를 서버로 보낸다.
// 	XArchive arParam;
// 	arParam << m_bvsNPC;
// 	arParam << m_idEnemy;		// 배틀 상대의 계정아이디(NPC면 0)
// 	GAMESVR_SOCKET->SendReqFinishBattle( this, m_idSpot, arParam );	
// 	return 1;
// }
// 
/**
 @brief 전투가 끝나면 배틀필드로부터 호출된다.
 @param bitCampWin 승리한 진영
*/
void XSceneBattle::OnFinishBattle( XGAME::xtSide bitCampWin, bool bRetreatSulfur )
{
	XAPP->m_fAccel = 1.f;
	if( bRetreatSulfur ) {
		//
	} else {
		if( m_bFinish )
			return;
		m_bFinish = true;
		XBattleField::sGet()->SetAI( false );
	}

#if defined(_XSINGLE)
	auto pWnd = new XWndStatistic( XBattleField::sGet()->GetLegionObj(0),
									XBattleField::sGet()->GetLegionObj(1) );
	Add( pWnd );
	pWnd->SetCancelButton( "butt.close" );
#else
	SendFinishBattle( XGAME::xEB_FINISH
									, bitCampWin
									, s_BattleStart.m_idxStage
									, bRetreatSulfur );

#endif
}

void XSceneBattle::SendFinishBattle( XGAME::xtExitBattle ebCode, 
									XGAME::xtSide bitWinner, 
									int idxStgae, 
									bool bRetreatSulfur )
{
	XGAME::xBattleFinish battle;
	battle.ebCode = ebCode;
#ifndef _XSINGLE
	battle.idSpot = s_BattleStart.m_idSpot;
#endif // not _XSINGLE
//	battle.idEnemy = m_idEnemy;
	battle.snSession = ACCOUNT->GetsnSession();
	battle.bitWinner = bitWinner;
	battle.idxStage = s_BattleStart.m_idxStage;
	battle.secPlay = (int)m_timerPlay.GetPassSec();
	battle.bRunAwaySulfur = bRetreatSulfur;
	{
		float hpLegion[ 2 ];
		hpLegion[ 0 ] = XBattleField::sGet()->GetLegionObj( 0 )->GetSumHpAllSquad();
		hpLegion[ 1 ] = XBattleField::sGet()->GetLegionObj( 1 )->GetSumHpAllSquad();
		battle.m_rateHpAlly = hpLegion[ 0 ] / m_hpMaxLegion[ 0 ];
		battle.m_rateHpEnemy = hpLegion[ 1 ] / m_hpMaxLegion[ 1 ];
	}
	// 아군과 적군 상황을 모두 보냄.
	for( int i = 0; i < 2; ++i ) {
		auto pLegionObj = XBattleField::sGet()->GetLegionObj( i );
		if( XASSERT( pLegionObj ) ) {
			pLegionObj->SerializeForGuildRaid( battle.arLegion[i] );
		}
	}
#ifndef _XSINGLE
// 	auto pBaseSpot = sGetpWorld()->GetSpot( s_BattleStart.m_idSpot );
// 	if( XASSERT(pBaseSpot) ) {
// 		if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_COMMON ) {
// 			auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
// 			if( XASSERT( pSpot ) ) {
// 				if( pSpot->IsGuildRaid() ) {
// 					XBREAK(1);	// 서버에서 받는 부분 수정할것.
// 					// 길드레이드 전 이었을때.
// 					// 적군부대의 상태를 묶어서 보냄.
// 					auto pLegionObj = XBattleField::sGet()->GetLegionObj( 1 );
// 					if( XASSERT( pLegionObj ) ) {
// 						pLegionObj->SerializeForGuildRaid( battle.arLegion[1] );
// 					}
// 				}
// 			}
// 		}
// 	}
	GAMESVR_SOCKET->SendReqFinishBattle( this, battle );
#endif // not _XSINGLE
}

/**
 서버로부터 배틀 결과가 옴
 @param pAryLoot 루트배열이 필요없는 경우도 있어서 포인터로 함.
*/
void XSceneBattle::OnRecvBattleResult( XGAME::xBattleResult& result )
{
#ifndef _XSINGLE
	m_resultBattle = result;
	auto pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_result", nullptr);
	Add(pPopup);
	auto pBaseSpot = sGetpWorld()->GetSpot( s_BattleStart.m_idSpot );
	XBREAK( pBaseSpot == nullptr );
	xSET_TEXT( pPopup, "text.use.ap", XFORMAT( "%+d", -pBaseSpot->GetNeedAP( ACCOUNT ) ) );
	xSET_TEXT( pPopup, "text.add.score", XFORMAT( "%+d", result.logForAttacker.addScore ) );
	if( result.IsWin() ) {
		SOUNDMNG->OpenPlaySound(5);	// 승리소리
		int idxLegion = ACCOUNT->GetCurrLegionIdx();
// 		xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, 
//                                 &XSceneBattle::OnOkBattleResult, idxLegion);
		pPopup->SetEvent( XWM_OK, this, &XSceneBattle::OnOkBattleResult, idxLegion );
		pPopup->SetOkButton( "butt.ok" );
		xSET_SHOW(pPopup, "img.victory", TRUE);
		int idx = 0;
		{
			XWnd *pWndItems = Find( "wnd.loot.item" );
			if( pWndItems ) {
				auto vStart = pWndItems->GetPosLocal();
				auto v = vStart;
				//
//				XArrayLinearN<XWnd*, 256> aryAdd;
				XARRAYLINEARN_LOOP_AUTO( result.aryDrops, itemBox ) {
					auto pProp = std::get<0>( itemBox );
					int numItem = std::get<1>( itemBox );
					// 아이템아이콘 컨트롤을 생성시켜서 슬롯에 넣는다.
					auto pWndItem = new XWndStoragyItemElem( v, pProp->idProp );
					pWndItem->SetNum( numItem );
					pWndItem->SetEventItemTooltip();
					pPopup->Add( pWndItem );
					const auto vSize = pWndItem->GetSizeLocal();
					v.x += vSize.w + 6.f;
					if( ++idx >= 5 ) {
						v.x = vStart.x;
						v.y = vStart.y + vSize.h + 6.f;
						idx = 0;
					}
				}END_LOOP;
				if( result.aryDrops.size() == 0 ) {
					auto pText = xGET_TEXT_CTRL( this, "text.tip" );
					if( pText ) {	
						pText->SetText( XTEXT( 2224 ) );	// 획득아이템 없음.
						pText->SetbShow( true );
					}
				}
			}
		}
		// 전투 별점 표시
		for( int i = 0; i < result.numStar; ++i ) {
			auto pStarEmpty = pPopup->Findf( "spr.star.%d", i + 1 );
			if( pStarEmpty ) {
				auto pSpr = SafeCast<XWndSprObj*>( pStarEmpty );
				if( pSpr ) {
//					pSpr->SetActionDelayed( 2, 0.5f );	// 0.5초후에 액션2로 바꿔라.
					pSpr->SetAction( 2 );
				}
			}
		}
		xSET_SHOW( pPopup, "butt.statistic", false );
		// 정복도 패널티
		xSET_TEXT( pPopup, "text.star.penalty",
			XFORMAT("%s(%+d%%)", XTEXT(2237), -(100 - result.m_mulByStar) ) );
	} else {
    // 패배시
		SOUNDMNG->OpenPlaySound(4);
		xSET_SHOW(pPopup, "img.defeat", true );
		xSET_SHOW(pPopup, "butt.statistic", true );
		xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneBattle::OnOkBattleResult, (DWORD)-1);
		xSetButtHander( pPopup, this, "butt.statistic", &XSceneBattle::OnClickStatistic );
		ID idText = xRandom( 55000, 55024 );
		auto pText = xGET_TEXT_CTRL( this, "text.tip" );
		if( pText ) {
			pText->SetText( XTEXT(idText) );
			pText->SetbShow( true );
		}
	}
	// 획득 자원을 표시해야 하면.
	if ( result.logForAttacker.aryLoot.size() > 0) {
		XWnd *pRoot = Find( "wnd.loot.res" );
		if( pRoot ) {
			auto pWndResources = new XWndResourceCtrl( XE::VEC2(0), 
														result.logForAttacker.aryLoot, true, 1.f );
			pRoot->Add( pWndResources );
			pWndResources->AutoLayoutCenter();
		}
	}
#endif // not _XSINGLE
}

/**
 서버로부터 유황스팟 배틀 결과가 옴(인카운터!)
*/
void XSceneBattle::OnRecvBattleResultSulfurEncounter( XSpotSulfur *pSpot
																										, const XGAME::xBattleStartInfo& info )
{
	// 결과데이터를 받아둠.
// 	m_arParam = ar;
// 	ID idAcc;
// 	_tstring strName;
// 	int score, level, power;
// 
// 	ar >> idAcc;
// 	ar >> strName;
//   ar >> power;
// 	ar >> score;
// 	ar >> level;
// 
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
	//
	auto pPopup = new XWndEncounter( pSpot, info );
//	pPopup->AddButtonOk(_T("ok"), BUTT_MID );
	Add( pPopup );
	pPopup->SetbModal( TRUE );
	xSET_BUTT_HANDLER( pPopup, "butt.attack", &XSceneBattle::OnOkBattleResultSulfurEncounter );
	xSET_BUTT_HANDLER( pPopup, "butt.retreat", &XSceneBattle::OnSulfurRetreat );
// 	_tstring strTitle = XE::Format(_T("Encounter(%d)!"), idAcc);
// 	XWND_ALERT_PARAM_RET( pAlert, strTitle.c_str(), XWnd::xOK, XCOLOR_WHITE, "%s", _T( "Win" ) );
// 	if( pAlert )
// 		pAlert->SetEvent( XWM_OK, this, &XSceneBattle::OnOkBattleResultSulfurEncounter );
}
// void XSceneBattle::OnRecvBattleResultSulfurEncounter( XArchive& ar
// 																										, int numSulfur )
// {
// 	// 결과데이터를 받아둠.
// 	m_arParam = ar;
// 	ID idAcc;
// 	_tstring strName;
// 	int score, level, power;
// 
// 	ar >> idAcc;
// 	ar >> strName;
//   ar >> power;
// 	ar >> score;
// 	ar >> level;
// 
// 	//
// 	auto pPopup = new XWndEncounter( numSulfur, idAcc, level, strName.c_str() );
// //	pPopup->AddButtonOk(_T("ok"), BUTT_MID );
// 	Add( pPopup );
// 	pPopup->SetbModal( TRUE );
// 	xSET_BUTT_HANDLER( pPopup, "butt.attack", &XSceneBattle::OnOkBattleResultSulfurEncounter );
// 	xSET_BUTT_HANDLER( pPopup, "butt.retreat", &XSceneBattle::OnSulfurRetreat );
// // 	_tstring strTitle = XE::Format(_T("Encounter(%d)!"), idAcc);
// // 	XWND_ALERT_PARAM_RET( pAlert, strTitle.c_str(), XWnd::xOK, XCOLOR_WHITE, "%s", _T( "Win" ) );
// // 	if( pAlert )
// // 		pAlert->SetEvent( XWM_OK, this, &XSceneBattle::OnOkBattleResultSulfurEncounter );
// }

int XSceneBattle::OnOkBattleResult( XWnd* pWnd, DWORD p1, DWORD p2 )
{
//	int idxLegion = (int)p1;
//	XBREAK( idxLegion < 0 );
// 	if (pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(TRUE);

	auto pPopup = new XWndBattleAfterHeroExp( 0, &m_resultBattle );
	if( pPopup ) {
		Add( pPopup );
	}

// 	if (idxLegion >= 0)
// 	{
// 		XWnd *pPopup = new XWndView(m_Layout.GetpLayout(), "popup_exp", nullptr);
// 		Add(pPopup);
// 		xSET_BUTT_HANDLER(pPopup, "butt.ok", &XSceneBattle::OnOkHerosExp);
// 		xSetButtHander( pPopup, this, "butt.statistic", &XSceneBattle::OnClickStatistic );

// 		XLegion *pLegion = ACCOUNT->GetLegionByIdx(idxLegion).get();
// 		if (XBREAK(pLegion == nullptr))
// 			return 1;
// 		XArrayLinearN<XHero*, XGAME::MAX_SQUAD> ary;
// 		pLegion->GetHerosToAry(ary);
// 
// 		int nAdjustY = 0, size = ary.size();
// 		
// 		nAdjustY = ary.size() / 5;
// 		if (ary.size() % 5 == 0)
// 			nAdjustY--;
// 		nAdjustY *= 42;
// 
// 		if (ary.size() > 5)
// 			size = 5;
// 		XE::VEC2 vStart((414 - 70) / 2 + 114 - 43 * (size - 1), (250 - 81) / 2 + 45 - nAdjustY);
// 
// 		XARRAYLINEARN_LOOP(ary, XHero*, pHero) {
// 			if (_i != 0 && _i % 5 == 0) {
// 				if (_i == ary.size() - (ary.size() % 5))
// 					size = ary.size() % 5;
// 				vStart.x = (float)(414 - 70) / 2 + 114 - 43 * (size - 1);
// 				vStart.y += 84;
// 			}
// 			char key[256];
// 			sprintf_s(key, "wnd.slot%d", _i + 1);
// 			XWnd *pSlot = Find(key);
// 			if (pSlot) {
// 				pSlot->SetPosLocal(vStart);
// 				pSlot->SetbShow(TRUE);
// 			}
// 
// 			sprintf_s(key, "img.hero.slot%d", _i+1);
// 			_tstring resImg = XE::MakePath(DIR_IMG, pHero->GetpProp()->strFace.c_str());
// 			xSET_IMG_RET(pImg, this, key, resImg.c_str());
// 			pSlot = pImg->GetpParent();
// 
// 			sprintf_s(key, "img.hero.bg%d", _i + 1);
// 			xSET_IMG(this, key, XE::MakePath(DIR_UI, _T("common_unit_bg_m.png")));
// 
// 			sprintf_s(key, "text.hero.slot%d", _i+1);
// 			xSET_TEXT(this, key, pHero->GetstrName().c_str());
// 
// 			sprintf_s(key, "pbar.hero.slot%d", _i+1);
// 			auto pPBar = SafeCast<XWndProgressBar*>(Find(key));
// 			if (pPBar) {
// 				pPBar->SetbShow(TRUE);
// //				pPBar->SetLerp((float)pHero->GetExp() / pHero->GetXFLevelObj().GetMaxExpCurrLevel());
// 				pPBar->SetLerp((float)pHero->GetExp() / pHero->GetMaxExpCurrLevel());
// 			}
// 			if (pHero->GetbLevelUpAndClear( XGAME::xTR_LEVEL_UP )) {
// 				pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("ingame_levelup.png")), 2.f, 2.f);
// 				pSlot->Add(pImg);
// 			}
// 			for (int i = 0; i < pHero->GetGrade(); ++i) {
// 				pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("common_etc_smallstar.png")), 10.f + i * 13, 6.f);
// 				pSlot->Add(pImg);
// 			}
// 			XWndTextString* pText = new XWndTextString(XE::VEC2(0,7),
// 											XE::Format(_T("Lv%d"), pHero->GetLevel()),
// 											FONT_NANUM, FONT_SIZE_DEFAULT);
// 			pText->SetStyle(xFONT::xSTYLE_STROKE);
// 			pText->SetLineLength(72.f);
// 			pText->SetAlign(XE::xALIGN_CENTER);
// 			pSlot->Add(pText);
// 
// 			vStart.x += 86;
// 		}END_LOOP;
// 	}
// 	else
// 	{
// 		DoExit(XGAME::xSC_WORLD);
// 	}
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneBattle::OnOkHerosExp( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnOkHerosExp");
	//
	auto spParam = std::shared_ptr<XGAME::xSPM_BASE>(new XGAME::xSPM_BASE);
	spParam->idParam = 1;
	DoExit( XGAME::xSC_WORLD, spParam);
	return 1;
}

/**
 인카운터전 OK눌렀을때 핸들러.
*/
int XSceneBattle::OnOkBattleResultSulfurEncounter( XWnd* pWnd, DWORD p1, DWORD p2 )
{
// 	GAME->DoEnterBattleScene( )
// 
// 	ID idAcc;
// 	_tstring strName;
// 	int score;
// 	int level;
// 	int power;
// 	//
// 	m_arParam >> idAcc;
// 	m_arParam >> strName;
// 	m_arParam >> power;
// 	m_arParam >> score;
// 	m_arParam >> level;
// 	XLegion *pLegion = nullptr;
// 	{
// 		XArchive arLegion;
// 		m_arParam >> arLegion;
// 		pLegion = XLegion::sCreateDeserializeFull( arLegion );
// 	}
// 	auto spParam = std::shared_ptr<XGAME::xSPM_BATTLE>( new XGAME::xSPM_BATTLE );
// 	spParam->idSpot = m_idSpot;
// //	spParam->idEnemy = idAcc;
// 	spParam->level = level;
// 	spParam->strName = strName;
// 	spParam->spLegion[0] = ACCOUNT->GetCurrLegion();
// 	spParam->spLegion[1] = LegionPtr(pLegion);
// 	spParam->defense = 0;
// 	spParam->bvsNPC = FALSE;
//	DoExit( XGAME::xSC_INGAME, spParam );

	// 배틀씬 파라메터가 모두 세팅되어 있어야 함.
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
	DoExit( XGAME::xSC_INGAME );
	return 1;
}

/****************************************************************
* @brief 유황전투에서 기습이 발생했을때 도주를 누름.
*****************************************************************/
int XSceneBattle::OnSulfurRetreat( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnSulfurRetreat");
	//
	if( ACCOUNT->IsEnoughCash( XGAME::xCS_SULFUR_RETREAT ) )
	{
		OnFinishBattle( XGAME::xSIDE_PLAYER, true );
		pWnd->GetpParent()->SetbDestroy( TRUE );
	} else
	{
		// 캐쉬를 구매할거냐는 창이 뜨고 캐쉬 구매팝업을 띄운다.
	}
	
	return 1;
}


/****************************************************************
* @brief 배치된 유닛 그대로 다시 싸움시킨다.
*****************************************************************/
int XSceneBattle::OnDebugRetry( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _XSINGLE
	CONSOLE("OnDebugButton");
	if( p1 == 1 )
	{
#if defined(WIN32) && (defined(_DEBUG) || defined(_DEV_TEST))	// 디버그이거나 내부테스트용일때만
		SAFE_DELETE( CONSTANT );
		GAME->LoadConstant();
		DestroyWndByIdentifier( "butt.skill" );
		DestroyWndByIdentifier( "butt.skill.cancel" );
		SAFE_DELETE( PROP_UNIT );
		PROP_UNIT = new XPropUnit;
		if( PROP_UNIT->Load( _T( "propUnit.txt" ) ) == FALSE )
			XERROR( "load error! %s", PROP_UNIT->GetstrFilename() );
		CONSOLE("Reloaded PropUnit...");
		CONSOLE( "Load propSkill..." );
		SAFE_DELETE( SKILL_MNG );
		SKILL_MNG = new XSkillMng;
		if( SKILL_MNG->Load( _T( "propSkill.xml" ) ) == FALSE )
			XERROR( "propSkill load error" );
#endif
		XBattleField::sGet()->ResetLegionObj( m_pWorld );

		// 배틀 씬프로세스를 종료시켜 다시 레디상태로 가게 한다.
		m_pProcess->SetbDestroy( TRUE );
		m_pProcess = new XSceneProcessBattle( this, m_pWorld->GetprefBattleField() );
		m_pProcess->SetstrIdentifier( "scene.process.battle" );
		Add( m_pProcess );
		m_pWorld->OnReset( false );
	}
	SetbUpdate( true );
//	m_ptBrave = 0;
#endif // _XSINGLE
	return 1;
}
/**
 @brief 유닛의 배치를 바꾼다.
*/
int XSceneBattle::OnDebugRecreate( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _XSINGLE
	CONSOLE( "OnDebugButton" );
	if( p1 == 1 )
	{
#if defined(_DEBUG) || defined(_DEV_TEST)	// 디버그이거나 내부테스트용일때만
		SAFE_DELETE( CONSTANT );
		GAME->LoadConstant();
		DestroyWndByIdentifier( "butt.skill" );
		DestroyWndByIdentifier( "butt.skill.cancel" );
		CONSOLE( "Load propSkill..." );
		SAFE_DELETE( SKILL_MNG );
		SKILL_MNG = new XSkillMng;
		if( SKILL_MNG->Load( _T( "propSkill.xml" ) ) == FALSE )
			XERROR( "propSkill load error" );
#endif
		XBattleField::sGet()->RecreateLegionObj( m_pWorld );
		// 배틀 씬프로세스를 종료시켜 다시 레디상태로 가게 한다.
		m_pProcess->SetbDestroy( TRUE );
		m_pProcess = new XSceneProcessBattle( this, m_pWorld->GetprefBattleField() );
		m_pProcess->SetstrIdentifier( "scene.process.battle" );
		Add( m_pProcess );
		m_pWorld->OnReset( true );
		SetbUpdate( true );
	}
	SetbUpdate( true );
//	m_ptBrave = 0;
#endif // _XSINGLE
	return 1;
}

int XSceneBattle::OnDebugProfile( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	if( XEProfile::sIsActive() == FALSE ) 
	{
		CONSOLE("profiler start!");
		XEProfile::sGet()->sDoStart();
	}
	else
	{
		XEProfile::sGet()->sDoFinish();
		XEProfile::sGet()->DoEvaluation();
		XARRAYLINEAR_LOOP( XEProfile::sGetResult(), XEProfile::xRESULT, result )
		{
			CONSOLE("%s %.2f %d %d %d %d", result.strFunc.c_str(),
											result.ratioShare,
											result.mcsExecAvg,
											result.mcsExecMin,
											result.mcsExecMax,
											result.depth );
		} END_LOOP;
	}
	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneBattle::OnUseSkill( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnUseSkill" );
	auto pButt = dynamic_cast<XWndSkillButton*>( pWnd );
	if( XBREAK( pButt == NULL ) )
		return 1;
	if( !pButt->IsCoolTime() ) {
		ID snSquad = (ID)p1;
		SquadPtr spSquad = WORLD->GetSquadBySN( XGAME::xLI_PLAYER, snSquad );
		if( spSquad != nullptr && spSquad->IsLive() ) {
			if( spSquad->GetspHeroUnit() != nullptr ) {
				XUnitHero *pUnitHero = SafeCast<XUnitHero*, XBaseUnit*>( spSquad->GetspHeroUnit().get() );
				XBREAK( pUnitHero == NULL );
				pUnitHero->DoUseSkill();
			}
		}
	}
	return 1;
}

/**
 @brief 아군 부대를 선택했을때
*/
void XSceneBattle::OnSelectSquad( const SquadPtr& squadSelect )
{
	XE::VEC2 sizeGame = XE::GetGameSize();
	XBREAK( squadSelect == nullptr );
	// 캔슬버튼은 이미 떠있으면 새로 생성하지 않는다.
	XWnd *pExist = Find( "butt.skill.cancel" );
	if( pExist == nullptr )	{
		XWndButton *pCancel = new XWndButton( sizeGame.w - 70.f,
																					sizeGame.h - 60.f,
																					_T( "common_butt_cancel.png" ), nullptr );
		pCancel->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnCancelSelect );
		pCancel->SetstrIdentifier( "butt.skill.cancel" );
		Add( pCancel );
	}
	auto pSkillDat = squadSelect->GetpHero()->GetSkillDatActive();
	if( pSkillDat == NULL )
		return;
	const char *cKey = "butt.skill";
	pExist = Find( cKey );
	if( pExist )
		pExist->SetbDestroy( TRUE );

	XUnitHero *pUnitHero = NULL;
	if( squadSelect->GetspHeroUnit() != nullptr )
		pUnitHero = SafeCast<XUnitHero*>( squadSelect->GetspHeroUnit().get() );
	auto pButt = new XWndSkillButton( sizeGame.w - 135.f, 
												sizeGame.h - 60.f, pSkillDat, pUnitHero );
	pButt->SetstrIdentifier( cKey );
	pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnUseSkill, squadSelect->GetsnSquadObj() );
	Add( pButt );
}

void XSceneBattle::OnDieSquad( XSquadObj* pSquadObj )
{
	auto pButtSkill = dynamic_cast<XWndSkillButton*>( Find("butt.skill") );
	if( pButtSkill ) {
		if( pButtSkill->GetsnHero() == pSquadObj->GetpHero()->GetsnHero() ) {
			OnCancelSelect( nullptr, 0, 0 );
		}
	}
}

/**
 @brief 스킬모션이 발동되면 호출된다.
*/
void XSceneBattle::OnStartSkillMotion( XUnitHero *pUnitHero, float secCool )
{
	// pUnitHero가 현재 선택된 부대라면 스킬버튼이 있을것으로 그때만 버튼UI를 검색한다.
	if( WND_WORLD->GetidSelectSquad() == pUnitHero->GetpSquadObj()->GetsnSquadObj() )
	{
		const char *cKey = "butt.skill";
		XWnd *pExist = Find( cKey );
		if( pExist )
		{
			XWndSkillButton *pButt = dynamic_cast<XWndSkillButton *>( pExist );
			if( XBREAK( pButt == NULL ) )
				return;
			pButt->SetCoolTimer( secCool );
		}
	}


}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneBattle::OnCancelSelect( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnCancelSelect");
	DestroyWndByIdentifier( "butt.skill.cancel" );
	DestroyWndByIdentifier( "butt.skill" );
	///< 선택한 부대를 해제시킨다.
	WND_WORLD->ClearSelectSquad();

	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneBattle::OnDebugAllKill( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnDebugAllKill");
	///< 
  if( p1 == 1 ) {
	  XLegionObj *pLegionObj = XBattleField::sGet()->GetLegionObj(1);
	  if( pLegionObj )
		  pLegionObj->KillAllUnit();
  } else {
    XAPP->m_bDebugViewDamage = !XAPP->m_bDebugViewDamage;
    XAPP->XClientMain::SaveCheat();
  }
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneBattle::OnDebugAbil( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnDebugAbil");
	//
	DoExit( XGAME::xSC_TECH );
	return 1;
}

int XSceneBattle::OnSurrrender(XWnd* pWnd, DWORD p1, DWORD p2)
{
	XBattleField::sGet()->SetAI( false );
	m_bFinish = true;
	SendFinishBattle( XGAME::xEB_RETREAT, XGAME::xSIDE_OTHER, s_BattleStart.m_idxStage, 0 );
	return 1;
}

void XSceneBattle::CreateBattleUI()
{
	// 전투 화면 UI
	m_Layout.CreateLayout("battle", this);

	xSET_TEXT(this, "text.battle.level", XE::Format(_T("%d"), ACCOUNT->GetLevel()));						// 아군 레벨
	xSET_TEXT(this, "text.battle.id", XE::Format(_T("%s"), ACCOUNT->GetstrName()) );		// 아군 ID
	xSET_TEXT(this, "text.battle.enemylevel", XE::Format(_T("%d"), s_BattleStart.m_Level));				// 적군 레벨
  _tstring strName = XE::Format(_T("%s"), s_BattleStart.m_strName.c_str());
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && s_BattleStart.m_idEnemy ) {
		strName = XFORMAT( "%s(%d)", s_BattleStart.m_strName.c_str(), s_BattleStart.m_idEnemy );
	}
//   if( XAPP->m_bDebugMode && m_pEnemy ) {
//     strName = XFORMAT("%s(%d)", m_strNameEnemy.c_str(), m_pEnemy->GetidAccount() );
//   }
#endif
	xSET_TEXT(this, "text.battle.enemyid", strName );	// 적군 ID
	xSET_ACTIVE(this, "wnd.battle.top", FALSE);		// UI 터치 안되게
	xSET_BUTT_HANDLER(this, "butt.battle.surrrender", &XSceneBattle::OnSurrrender);		// 항복 버튼
	// 업데이트 해야할 것들
	m_pAllyBar = XLayout::sGetCtrl<XWndProgressBar*>(this, "pbar.battle.ally");
	if (m_pAllyBar)
	{
		m_pAllyBar->SetbReverse( true );
// 		m_pAllyBar->SetLerp((float)ACCOUNT->GetExp() / ACCOUNT->GetMaxExpCurrLevel());
//		m_pAllyBar->SetRotateLocal(30);
	}

	m_pEnemyBar = XLayout::sGetCtrl<XWndProgressBar*>(this, "pbar.battle.enemy");
// 	if (m_pEnemyBar)
// 		m_pEnemyBar->SetLerp((float)ACCOUNT->GetExp() / ACCOUNT->GetMaxExpCurrLevel());

// 	float sec = m_timerPlay.GetPassSec();
// 	int h, m, s;
// 	XSYSTEM::GetHourMinSec( (XINT64)sec, &h, &m, &s );
// 	xSET_TEXT(this, "text.battle.time", XE::Format(_T("%02d:%02d"), m, s));
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneBattle::OnClickStatistic( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStatistic");
	//
	auto pPopup = new XWndStatistic( XBattleField::sGet()->GetLegionObj( 0 ),
								   XBattleField::sGet()->GetLegionObj( 1 ) );
	Add( pPopup );
	pPopup->SetCancelButton( "butt.close" );
	pPopup->SetbModal( TRUE );
	
	return 1;
}

XE::VEC2 XSceneBattle::OnDelegateProjection2( const XE::VEC3& vPos ) 
{
	return m_pWorld->GetPosWorldToWindow( vPos );
}
bool XSceneBattle::OnDelegateIsInScreen( const XE::VEC2& vScr )
{
	return true;
}
float XSceneBattle::OnDelegateGetScaleViewport() 
{
	return m_pWorld->GetscaleCamera();
}

void XSceneBattle::OnReload()
{
//	XParticleMng::sGet()->DestroyAll();
	auto pWndLayer = static_cast<XWndParticleLayer*>( Find( "layer.particle" ) );
	if( pWndLayer )
		pWndLayer->DestroyEmittersAndParticles();
	XPropParticle::sGet()->Reload();
//	XE::VEC2 sizeWorld = m_pWorld->GetpWorld()->GetvwSize();
	CreateParticleSfx();
// 	XE::VEC2 v( 1377, 821 );
// 	XParticleMng::sGet()->CreateSfx( "bg_sulfur", v );
}
/**
 @brief 파티클 레이어 생성
*/
void XSceneBattle::CreateParticleSfx()
{
#ifdef _XSINGLE
	return;
#else
	if( s_BattleStart.m_typeSpot != XGAME::xSPOT_SULFUR )
		return;
#endif // not _XSINGLE
	auto pWndLayer = static_cast<XWndParticleLayer*>( Find( "layer.particle" ) );
	if( pWndLayer == nullptr ) {
		pWndLayer = new XWndParticleLayer( -1.f, this );
		pWndLayer->SetstrIdentifier( "layer.particle" );
		Add( pWndLayer );
	}
//	XParticleMng::sGet()->SetBlendFunc( XE::xBF_ADD );
	XE::VEC2 v;
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 1377, 821 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 440, 312 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 144, 287 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 24, 570 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 176, 584 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 267, 703) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 263, 593) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 724, 704) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 840, 701) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 10, 758) );
 	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 1552, 675 ) );
 	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 1190, 440 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 1849, 382 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 1619, 1031) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 508, 1027 ) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 710, 987) );
	pWndLayer->AddEmitter( "bg_sulfur", XE::VEC2( 820, 991 ) );
}
/**
 @brief 아군부대를 선택해서 적부대나 바닥을 클릭함.
*/
void XSceneBattle::OnControlSquad( XHero *pHero )
{
	if( ACCOUNT->GetFlagTutorial().bControlSquadInBattle == 0 ) {
		XBREAK( pHero == nullptr );
		if( ACCOUNT->GetpQuestMng()->GetQuestObj( _T( "x.quest.battle.control" ) ) ) {
			ACCOUNT->GetFlagTutorial().bControlSquadInBattle = 1;
			GAMESVR_SOCKET->SendControlSquadInBattle( pHero );
		}
	}
}

void XSceneBattle::DoMoveCamera( const XE::VEC2& vwDstCenter, float secMove )
{
	m_FocusMng.DoMove( m_pWorld->GetvwCamera(), vwDstCenter, secMove );
}

XE::VEC2 XSceneBattle::GetvwCamera()
{
	return m_pWorld->GetvwCamera();
}

/**
 @brief 컷씬 대화창이 생성되면 이벤트가 발생한다.
*/
void XSceneBattle::OnCreateOrderDialog( ID idHero )
{
	// idHero를 객체중에 찾을수 있으면 그 객체의 머리위해 말풍선애니메이션을 띄운다.
	auto spUnit = XBattleField::sGet()->GetHeroUnit( idHero );
	if( spUnit ) {
		auto pObj = new XObjLoop( spUnit->GetvwTop(), _T("ui_speech.spr"), 1 );
		XBattleField::sGet()->AddpObj( pObj );
	}
}

XHero* XSceneBattle::GetpHero( ID idHero )
{
	auto spUnit = XBattleField::sGet()->GetHeroUnit( idHero );
	if( spUnit ) {
		return spUnit->GetHero();
	}
	return nullptr;
}

/**
 @brief 
*/
int XSceneBattle::OnClickPlay( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickPlay");
	//
	if( m_mulPlay == 1 ) {
		m_mulPlay = 2;
	}	else if( m_mulPlay == 2 ) {
		m_mulPlay = 4;
	}	else if( m_mulPlay == 4 ) {
		m_mulPlay = 1;
	}
	XAPP->m_fAccel = (float)m_mulPlay;
	SetbUpdate( true );
	return 1;
}
