#include "StdAfx.h"
#include "XSceneBattle.h"
#include "XWndBattle.h"
#include "XGame.h"
#include "XSockGameSvr.h"
#include "XGameWnd.h"
#include "XBattleField.h"
#include "XWndBattleField.h"
#include "XFramework/Game/XEWndWorld.h"
#include "XFramework/Game/XEWndWorldImage.h"
#include "XFramework/Game/XEWorldCamera.h"
#include "XWndTemplate.h"
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
#include "XWndResCtrl.h"
#include "XWndBattle.h"
#include "_Wnd2/XWndProgressBar.h"
#include "XSystem.h"
#include "XHero.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif // _CHEAT
#ifdef _XSINGLE
#include "XLegion.h"
#include "XSquadron.h"
#include "XPropLegion.h"
#include "XPropLegionH.h"
#include "Sprite/SprMng.h"
#endif // _XSINGLE
#include "XSoundMng.h"
#include "XMsgUnit.h"
#include "XFramework/XEProfile.h"
#ifdef _XTEST
#include "sprite/SprObj.h"
#endif // _XTEST
#include "OpenGL2/XTextureAtlas.h"
//#include "OpenGL2/XBatchRenderer.h"
#include "XFramework/client/XWndBatchRender.h"
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace XSKILL;

XSceneBattle *SCENE_BATTLE = nullptr;
XGAME::xBattleStart XSceneBattle::s_BattleStart;
//////////////////////////////////////////////////////////////////////////

XE_NAMESPACE_START( XGAME )
	
void xsCamp::Release() {
	m_spAcc.reset();
	m_spLegionObj->Release();

}

#ifdef _XSINGLE
void xsCamp::CreateLegion( const std::string& idsLegion, XGAME::xtSide bitSide ) {
	XBREAK( bitSide == 0 );
	XBREAK( idsLegion.empty() );
	m_idsLegion = idsLegion;
	m_bitSide = bitSide;
	auto pPropLegion = XPropLegion::sGet()->GetpProp( idsLegion );
	if( XASSERT( pPropLegion ) ) {
		m_spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, 50, false );
	}
}
// void xsCamp::ReCreateLegion( XWndBattleField* pWndWorld) 
// {
// 	CreateLegion( m_idsLegion, m_bitSide );
// 	CreateLegionObj();
// 	CreateSquadObj( pWndWorld, m_bitOption );
// }
#endif // _XSINGLE
// 
void xsCamp::CreateLegionObj()
{
	XBREAK( m_spLegion == nullptr );
	XBREAK( m_bitSide == 0 );
//	m_spLegionObj = XSPLegionObj( new XLegionObj( m_spLegion, m_bitSide ) );
	m_spLegionObj = std::make_shared<XLegionObj>( m_spLegion, m_bitSide );
}

/**
 @brief m_spLegionObj의 분대객체들을 생성한다.
*/
void xsCamp::CreateSquadObj( XWndBattleField* pWndWorld, XGAME::xtBattle typeBattle )
{
#ifdef _XSINGLE
	XBREAK( m_idsLegion.empty() );
#endif // _XSINGLE
	XBREAK( m_spLegionObj == nullptr );
	XBREAK( pWndWorld == nullptr );
	auto pBattleField = pWndWorld->GetspBattleField();
	XBREAK( pBattleField == nullptr );
	m_bitOption = typeBattle;
	const XE::VEC3 vCenterWorld( pBattleField->GetvwSize().w * 0.5f, 231.f, 0 );
	BIT bitOption = XGAME::xLO_DEFAULT;
	if( typeBattle == xBT_GUILD_RAID && m_bitSide != xSIDE_PLAYER ) {
		bitOption |= xLO_NO_CREATE_DEAD;
	}
	const auto vwBasePos = (m_bitSide == xSIDE_PLAYER)? 
														vCenterWorld - XE::VEC3( 240, 0 ) :
														vCenterWorld + XE::VEC3( 240, 0 );
	// m_pLegion을 바탕으로 내부에서 분대 객체를 만듬. 분대객체안에서는 다시 유닛 객체를 만듬.
	m_spLegionObj->CreateLegion2( pWndWorld, vwBasePos, (xtLegionOption)bitOption );
#ifdef _XSINGLE
	auto pPropLegion = XPropLegion::sGet()->GetpProp( m_idsLegion );
	if( pPropLegion ) {
		for( auto& squad : pPropLegion->arySquads ) {
			auto spSquadObj = m_spLegionObj->GetspSquadObjByIdx( squad.idxPos );
			if( spSquadObj ) {
				spSquadObj->m_bShowHpInfo = squad.m_bShow;
			}
		}
	}
#endif // _XSINGLE
}

XE_NAMESPACE_END;

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XSceneBattle::XSceneBattle( XGame *pGame/*, SceneParamPtr& spBaseParam*/ ) 
	: XSceneBase( pGame, XGAME::xSC_INGAME )
	, m_aryCamp(2)
	, m_aryBar(2)
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
	//
#ifdef _XMEM_POOL
	XUnitCommon::s_pPool = new XPool<XUnitCommon>( XGAME::MAX_UNIT_SMALL * 30 );
	XObjArrow::s_pPool = new XPool<XObjArrow>( XGAME::MAX_UNIT_SMALL * 30 );
#endif // _XMEM_POOL
#ifdef _XSINGLE
	// 전투파라메터를 외부에서 넘어온것처럼 시뮬레이션 한다.
	sSetBattleParamForSingle();
	auto strBg = XGC->GetBgBattle( XGAME::xSPOT_NPC );
#else
	// 파라메터가 세팅안되어 있으면 에러.
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
	// 싱글모드가 아니면 스팟아이디는 모두 있어야 함.
	XSpot *pSpot = sGetpWorld()->GetSpot( s_BattleStart.m_idSpot );
	XBREAK( pSpot == nullptr );
	m_pSpot = pSpot;
	if( pSpot && pSpot->IsNpc() && pSpot->GetLevel() < 10 ) {
		XBaseUnit::s_bNotUseActiveByEnemy = true;
	} else {
		XBaseUnit::s_bNotUseActiveByEnemy = false;
	}
	auto strBg = XGC->GetBgBattle( s_BattleStart.m_typeSpot );
	if( s_BattleStart.m_Defense > 0 ) {
		// defense값이 있으면 스팟타입은 반드시 jewel이어야 한다.
		XASSERT( s_BattleStart.m_typeSpot == xSPOT_JEWEL );
	}
#endif
	// XBattleField(XWorld)객체 생성
	// 배경 레이어
	// 배경레이어용 이미지 로딩
	auto psfcBG = IMAGE_MNG->LoadByRetina( XE::MakePath( DIR_IMG, strBg ),
																				 XE::xPF_RGB565,
																				 false,			// atlas
																				 false );		// no async
	XBREAK( psfcBG == nullptr );
	const auto sizeBg = psfcBG->GetSize();
//	const XE::VEC2 sizeBg( 1920, 1024 );
	// 월드객체 생성
	auto spWorld = std::make_shared<XBattleField>( sizeBg );
	// 전투필드 윈도우 생성
	m_pWndWorld = new XWndBattleField( spWorld );
	Add( m_pWndWorld );
	// 카메라 객체 생성
	const float scaleCamera = 0.5f;
	auto spCamera = std::make_shared<XEWorldCamera>( sizeBg, 
																									GetSizeLocal(),
																									scaleCamera );
	spCamera->SetscaleMin( 0.4f );
	spCamera->SetscaleMax( 2.f );
	// 카메라 바인딩
	m_pWndWorld->SetspCamera( spCamera );
	// 배경레이어 추가
	auto pLayerBg = m_pWndWorld->AddBgLayer( psfcBG, spWorld );
	// 오브젝트 레이어 추가
	auto pLayerObj = m_pWndWorld->AddObjLayer( spWorld );
	spCamera->SetScaleCamera( 0.5f );
	spCamera->SetFocus( XE::VEC2( 956, 450 ) );
	// 초상화리스트 레이어
	auto pWnd = new XWndBatchRender( "layer.faces", true );
	pWnd->SetstrIdentifier( "wnd.faces" );
	Add( pWnd );
	// 전투타입을 지정한다.
	XBattleField::sGet()->SettypeBattle( s_BattleStart.m_typeBattle );
	// ui메인
 	pWnd = new XWndBatchRender( "layer.ui", false );
	pWnd->SetstrIdentifier("wnd.ui");
	Add( pWnd );
	SetbUpdate( true );
}

void XSceneBattle::Release()
{
	if( XBattleField::sGet() )
		XBattleField::sGet()->GetpObjMngMutable()->Release();
	s_BattleStart.Release();
	XSceneBase::Release();
	for( auto& camp : m_aryCamp ) {
		camp.m_spLegionObj->Release();
		camp.m_spLegionObj.reset();
	}
	CheckLeak();
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
	XAPP->m_fAccel = (float)1.f;
}

void XSceneBattle::Create( void )
{
	XEBaseScene::Create();
}

// 양측 진영 군단객체 생성
void XSceneBattle::CreateCamps()
{
	// 플레이어 진영 생성,
	{
		const int idxSide = xSI_PLAYER;
		auto& camp = m_aryCamp[idxSide];
		//		camp.m_spAcc = ACCOUNT;
		// 플레이어측 정보도 외부에서 받아서 세팅하게 해서 ACCOUNT와의 연관성을 완전히 끊어야 한다.
		camp.SetAccInfo( xSIDE_PLAYER, 0, ACCOUNT->GetLevel(), ACCOUNT->Get_strName() );
		camp.m_spLegion = s_BattleStart.m_spLegion[idxSide];
#ifdef _XSINGLE
		camp.m_idsLegion = "single1_player";
#endif // _XSINGLE
		camp.CreateLegionObj();
		camp.CreateSquadObj( m_pWndWorld, s_BattleStart.m_typeBattle );
		camp.m_psfcProfile = GAME->GetpsfcProfile();
	}
	// 적진영 생성
	{
		const int idxSide = xSI_OTHER;
		auto& camp = m_aryCamp[idxSide];
		//		camp.m_spAcc = ;
		camp.SetAccInfo( xSIDE_OTHER, s_BattleStart.m_idEnemy, s_BattleStart.m_Level, s_BattleStart.m_strName );
		camp.m_spLegion = s_BattleStart.m_spLegion[idxSide];
#ifdef _XSINGLE
		camp.m_idsLegion = "single1_enemy";
#endif // _XSINGLE
		camp.CreateLegionObj();
		camp.CreateSquadObj( m_pWndWorld, s_BattleStart.m_typeBattle );
		camp.m_psfcProfile = nullptr;
	}
	// 각 유닛들에게 OnStart이벤트
	for( auto& camp : m_aryCamp ) {
		camp.m_spLegionObj->OnSkillEvent( xJC_START );
	}
	//
	s_BattleStart.Release();		// spAcc가 파괴되기전에 미리 해제시켜줘야 함.
}
BOOL XSceneBattle::OnCreate()
{
#ifdef _XASYNC_SPR
	// 비동기로딩에서는 3,2,1을 먼저 시작한다.
	CreateProcessReady();
#endif // _XASYNC_SPR
#ifdef _XSINGLE
	sSetAbilHeroes();
#endif // _XSINGLE
	// 군단 객체를 만들고 유닛객체를 만들어 군단객체에 넣는다.
	// 양측 진영 생성
	SET_ATLASES( XWndBattleField::sGetObjLayer()->GetpAtlas() ) {
		CreateCamps();
	} END_ATLASES;
// 	XBattleField::sGet()->CreateSquadObj( m_pWndWorld );;
	CreateBattleUI();
	if( ACCOUNT->GetLevel() < 5 ) {
		xSET_SHOW( this, "butt.play", false );
	} else {
		xSET_SHOW( this, "butt.play", true );
		SetButtHander( this, "butt.play", &XSceneBattle::OnClickPlay );
	}

	// 분수형태 이미터
	XE::VEC2 sizeWorld = m_pWndWorld->GetspWorld()->GetvwSize();
	//
	CreateParticleSfx();

#ifdef _XSINGLE
	if( XAPP->m_bDebugMode && XAPP->m_bShowFace )
#endif // _XSINGLE
	{
		CreateHeroesFace();
	}
	//
#ifndef _XSINGLE
	if( m_pSpot && m_pSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
		// 자원부대 리스트를 얻는다.
		XVector<XSPSquad> arySquads;
		auto pLegionObj = XBattleField::sGet()->GetAILegionObj();
		pLegionObj->GetArySquadByResource( &arySquads );
		XVector<xRES_NUM> aryRes, aryLoots;
		// 총 루팅할수 있는 양을 얻는다.
		m_pSpot->GetLootInfo( &aryLoots );
		// 각 부대가 나눠가져야 하는양 얻는다.
		for( auto& res : aryLoots ) {
			auto numDrop = XGC->GetLootAmountByEach( res.num );
			if( numDrop > 0 )
				aryRes.Add( xRES_NUM( res.type, numDrop ) );
		}
		if( aryRes.size() > 0 ) {
			// 각 부대가 나눠가진다.
			for( auto spSquadObj : arySquads ) {
				spSquadObj->SetaryLoots( aryRes );
			}
		}
		XBattleField::sGet()->SetLootRes( aryLoots );
	}
#endif // not _XSINGLE
	// OnEnterScene에서 시작
	CreateDebugButtons();
	SetbUpdate( TRUE );

	return TRUE;
}

/**
 @brief 군단객체 생성에 필요한 파라메터들을 생성
 for 싱글모드
*/
#ifdef _XSINGLE
XSPAcc XSceneBattle::sCreateAcc()
{
	XSPAcc spAcc = XAccount::sGetPlayer();
	if( spAcc == nullptr ) {
		spAcc = std::make_shared<XAccount>( 1 );
		spAcc->SetpDelegateLevel( GAME );
		XAccount::sSetPlayer( spAcc );
		spAcc->CreateFakeAccount();
		// 플레이어측 군단
		auto pPropLegion = XPropLegion::sGet()->GetpProp( "single1_player" );
		if( pPropLegion ) {
//			auto spLegion = spAcc->CreateLegion( pPropLegion );
			auto spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, 50, false );
// 			XVector<XSquadron*> arySquad;
// 			spLegion->GetSquadronToAry( &arySquad );
// 			for( auto pSquad : arySquad ) {
			for( auto pSquad : spLegion->GetarySquadrons() ) {
				if( pSquad && pSquad->GetpHero() ) {
					ACCOUNT->AddHero( pSquad->GetpHero() );
					pSquad->SetbCreateHero( FALSE );
				}
			}
			ACCOUNT->SetspLegion( 0, spLegion );
		}
	}
	return spAcc;
}
void XSceneBattle::sSetBattleParamForSingle()
{
	XGAME::xBattleStart bs;
	auto spAcc = sCreateAcc();
	bs.m_spLegion[0] = spAcc->GetCurrLegion();
	// 적군단 생성.
	bs.m_Level = 50;
	bs.m_strName = _T( "babarian" );
	{
		auto pPropLegion = XPropLegion::sGet()->GetpProp( "single1_enemy" );
		if( pPropLegion ) {
			auto spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, bs.m_Level, false );
			bs.m_spLegion[1] = spLegion;
		}
	}
	bs.m_Defense = 0;
	bs.m_idEnemy = 0;
	bs.m_typeBattle = XGAME::xBT_NORMAL;
	XSceneBattle::sSetBattleStart( bs );
}
#endif // _XSINGLE

#ifdef _XSINGLE
void XSceneBattle::SaveSingle()
{
	XXMLDoc xml;
	xml.SetDeclaration();
	auto nodeVs = xml.AddNode( "vs" );

}

#endif // _XSINGLE

XWndBatchRender* XSceneBattle::GetpLayerUI()
{
	return SafeCast<XWndBatchRender*>( Find( "wnd.ui" ) );
}

XWndBatchRender* XSceneBattle::GetpLayerFaces()
{
	return SafeCast<XWndBatchRender*>( Find( "wnd.faces" ) );
}

void XSceneBattle::CreateBattleUI()
{
	auto pWndUI = GetpLayerUI();
	SET_ATLASES( pWndUI->GetpAtlas() ) {
		// 전투 화면 UI
		m_Layout.CreateLayout( "battle", pWndUI );

		auto pRootTop = pWndUI->Find( "wnd.bars" );
		for( int side = 0; side < 2; ++side ) {
			auto& camp = m_aryCamp[side];
			auto pRootName = pRootTop->Findf( "mod.name.%d", side );
			if( pRootName ) {
				_tstring strName = camp.m_strName;
#ifdef _CHEAT
				if( XAPP->m_bDebugMode && camp.m_idAcc ) {
					strName += XFORMAT( "(%d)", camp.m_idAcc );
				}
#endif
				xSET_TEXT( pRootName, "text.level", XFORMAT( "%d", camp.m_Level ) );
				xSET_TEXT( pRootName, "text.name", XFORMAT( "%s", strName.c_str() ) );
				auto pImgBg = pRootTop->Findf( "img.bar.%d", side );
				if( pImgBg ) {
					m_aryBar[side] = XLayout::sGetCtrl<XWndProgressBar2*>( pImgBg, "pbar.hp" );
				}
			}
			// 프로필 이미지
			if( camp.m_psfcProfile ) {
				auto pImg = xGET_IMAGE_CTRLF( pRootTop, "img.profile.%d", side );
				if( pImg ) {
					pImg->SetSurfaceWithpSurface( camp.m_psfcProfile );
				}
			}
		} // for
		xSET_ACTIVE( pWndUI, "wnd.bars", FALSE );		// UI 터치 안되게
		auto pButt =
			xSET_BUTT_HANDLER( pWndUI, "butt.battle.surrrender", &XSceneBattle::OnSurrrender );		// 항복 버튼
#ifdef _XSINGLE
		if( pButt ) {
			pButt->SetbShow( false );
		}
#endif
	} END_ATLASES;
}

void XSceneBattle::CreateHeroesFace()
{
	// 여기서 생성되는 모든 서피스는 배치렌더용으로 생성.
	auto bPrevBatch = XGraphics::sSetEnableBatchLoading( true );
	do 
	{
		auto pLayerFaces = GetpLayerFaces();		// ui batch renderer layer
		SET_ATLASES( pLayerFaces->GetpAtlas() ) {
			// 좌우 영웅리스트
			for( int i = 0; i < 2; ++i ) {
				auto pLegionObj = XBattleField::sGet()->GetLegionObj( i );
				if( pLegionObj ) {
					XWnd* pWndLayer = new XWnd();
					pWndLayer->SetstrIdentifierf( "wnd.layer.faces%d", i );
					pLayerFaces->Add( pWndLayer );
					XWnd* pFirst = nullptr;
					//
					for( auto spSquadObj : pLegionObj->GetlistSquad() ) {
						auto pHero = spSquadObj->GetpHero();
						if( pHero ) {
							auto pWnd = new XWndFaceInBattle( spSquadObj, i + 1 );
							pWnd->SetstrIdentifierf( "face.%8x", pHero->GetsnHero() );
							pWnd->SetPosLocal( i * 609, 0 );
	#if !defined(_DEBUG)
							if( i == 0 )		// 디버깅모드일때는 제한없이 양쪽편을 다 누를 수 있음.
	#endif
							{
								pWnd->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnTouchHeroFace, spSquadObj->GetsnSquadObj() );
							}
							pWndLayer->Add( pWnd );
							if( !pFirst )
								pFirst = pWnd;
						}
					}
					const float marginTB = 5.f;	// 위아래 마진
					if( pFirst ) {
						const float hDist = 2.f;
						const float hElem = pFirst->GetSizeNoTransLayout().h * pFirst->GetScaleLocal().y;	// elem하나의 크기
						const float hScene = GetSizeLocal().h - (marginTB * 2.f);	// 마진을 뺀 씬 높이
						float hSector = hScene / pWndLayer->GetNumChild();		// 각 elem당 섹터 크기
						float scale = 0.65f;
						if( hElem + hDist > hSector ) {
							scale = hSector / (hElem + hDist);
						}
						float y = marginTB;
						for( auto pWnd : pWndLayer->GetlistItems() ) {
							pWnd->SetScaleLocal( scale );
							if( scale == 0.65f ) {
								pWnd->SetY( y );
								y += (hElem * scale) + hDist;
							}
							pWnd->SetbUpdate( true );
						}
						if( scale != 0.65f )
							pWndLayer->AutoLayoutVCenterByChilds( marginTB );
					}
				} // if( pLegionObj ) {
			}
		} END_ATLASES;
	} while (0);
	XGraphics::sSetEnableBatchLoading( bPrevBatch );
}


#ifdef _XSINGLE
void XSceneBattle::sSetAbilHeroes()
{
// 	auto spLegion = ACCOUNT->GetCurrLegion();
// 	auto pHero = spLegion->GetpLeader();
//	sSetAbilHero( pHero, XGAME::xUNIT_LYCAN, _T( "poison_claw" ), 5 );
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
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugButton, 1 );
		Add( pButt );
		v.x += size.w;
#if defined(_XSINGLE)
		pButt = new XWndButtonDebug( v.x, v.y,
															size.w, size.h,
															_T( "재생성" ),
															XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.recreate" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugButton, 2 );
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
																_T( "show dmg" ),
			XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.show.damage" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugAllKill, 2 );
		Add( pButt );
		v.x += size.w;
		pButt = new XWndButtonDebug( v.x, v.y,
																size.w, size.h,
																_T( "suicide" ),
			XE::GetMain()->GetpGame()->GetpfdSystem() );
		pButt->SetstrIdentifier( "butt.debug.suicide" );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnDebugAllKill, 3 );
		Add( pButt );
		v.x += size.w;
// 		pButt = new XWndButtonDebug( v.x, v.y,
// 																 size.w, size.h,
// 																 _T( "<-" ),
// 																 XE::GetMain()->GetpGame()->GetpfdSystem() );
// 		pButt->SetstrIdentifier( "butt.debug.minus" );
// 		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnCheat, 0 );
// 		Add( pButt );
// 		v.x += size.w;
// 		pButt = new XWndButtonDebug( v.x, v.y,
// 																 size.w, size.h,
// 																 _T( "->" ),
// 																 XE::GetMain()->GetpGame()->GetpfdSystem() );
// 		pButt->SetstrIdentifier( "butt.debug.plus" );
// 		pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnCheat, 1 );
// 		Add( pButt );
// 		v.x += size.w;


	}
#endif // CHEAT
}

void XSceneBattle::Update()
{
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
	xSET_SHOW( this, "butt.debug.suicide", XAPP->m_bDebugMode );
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
		if( m_bFinish )
			return;
		m_pProcess = new XSceneProcessBattle( this, 
																					m_pWndWorld->GetspBattleFieldMutable() );
		m_pProcess->SetbTouchable( false );
		m_pProcess->SetstrIdentifier("scene.process.battle");
		Add( m_pProcess );
		BOOL bRealTimer = TRUE;
// #ifdef _DEBUG
		bRealTimer = FALSE;	// 디버깅중엔 편의상 리얼타이머를 사용하지 않음.
// #endif
		m_timerPlay.Set( 60 * 2, bRealTimer );    // 제한시간 2분
	} else
	if( pProcess->GetstrIdentifier() == "scene.process.battle" )
	{

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
					pWndPopup->SetEvent( XWM_CANCEL, this, &XSceneBattle::OnEnterScene, 1 );
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
#ifndef _XASYNC_SPR
	CreateProcessReady();
#endif // not _XASYNC_SPR

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

/**
 @brief 3,2,1 나오는 씬
*/
void XSceneBattle::CreateProcessReady()
{
	if( m_bFinish )
		return;
	m_pProcess = new XSceneProcessReady( this );
	m_pProcess->SetstrIdentifier( "scene.process.ready" );
	Add( m_pProcess );
}


int XSceneBattle::Process( float dt ) 
{
	if( m_FocusMng.IsScrolling() ) {
		auto vCurr = m_FocusMng.GetCurrFocus();
		m_pWndWorld->SetFocus( vCurr );
	}
	if( XBattleField::sGet()->GetLegionObj( 0 ) 
			&& XBattleField::sGet()->GetLegionObj( 1 ) ) {
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
		if( m_aryBar[0] ) {
			for( int i = 0; i < 2; ++i ) {
				m_aryBar[i]->SetLerp( hpLegion[i] / hpMax[i] );
			}
		}

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
		} else {
			// 전투 종료됨
			if( m_timerResult.IsOver() ) {
				// 약간 기다렸다가 보냄,
				m_timerResult.Off();
				DoPopupBattleResult( m_resultBattle );
			}
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
		if( pb->GetLegionObj(0) && pb->GetLegionObj(1) ) {
			PUT_STRINGF( 201, 25, XCOLOR_WHITE, "%d/%d", (int)pb->GetLegionObj( 0 )->GetSumHpAllSquad(), (int)m_hpMaxLegion[0] );
			PUT_STRINGF( 384, 25, XCOLOR_WHITE, "%d/%d", (int)pb->GetLegionObj( 1 )->GetSumHpAllSquad(), (int)m_hpMaxLegion[1] );
		}
	}
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewSquadInfo )	{
		XSPSquad spSquad = WND_WORLD->GetspSelectSquad();
		if( spSquad != nullptr )		{
			auto spUnit = spSquad->GetspLiveMember();
			if( spUnit )			{
				const float distY = 8.f;
				XE::VEC2 v(500,145+36);
				XVector<_tstring> aryStr;
				GetSquadInfoToAry( spSquad, spUnit.get(), &aryStr );
				for( auto& str : aryStr ) {
					PUT_STRING_SMALL( v.x, v.y, XCOLOR_WHITE, str.c_str() );
					v.y += distY;
				}
			}
		}
	}
	// 부대원 hpbar 보이기
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewSquadsHp ) {
		auto spBattleField = m_pWndWorld->GetspBattleField();
		if( spBattleField ) {
			spBattleField->DrawLegionBar( XE::VEC2( 10, 10 ), 0 );
			spBattleField->DrawLegionBar( XE::VEC2( (int)XE::GetGameWidth()-(48+10), 10 ), 1 );
		}
	}
	if( m_pWndWorld && m_pWndWorld->GetpObjLayer() ) {
		int fpsDpCall = m_pWndWorld->GetpObjLayer()->GetavgDPCall();
		PUT_STRINGF( 0, 10, XCOLOR_WHITE, "dpcall:%d", fpsDpCall );
	}
#endif

}

void XSceneBattle::GetSquadInfoToAry( XSPSquad spSquad, XBaseUnit* pUnit, XVector<_tstring>* pOut ) const
{
	pOut->clear();
	_tstring str;
	{
		auto spTarget = spSquad->GetspTarget();
		if( spTarget ) {
			str = XFORMAT( "타겟:%s(%s)(0x%x)", spTarget->GetpHero()->GetstrName().c_str()
																				, XGAME::GetStrUnit( spTarget->GetpHero()->GetUnit() )
																				, spTarget->GetsnSquadObj() );
			pOut->Add( str );
		}
	}
	auto pHero = spSquad->GetpHero();
	str = XE::Format( _T( "영웅:%s(%s)" ), pHero->GetstrName().c_str(), pHero->GetstrIdentifer().c_str() );
	pOut->Add( str );
	str = XE::Format( _T( "유닛:%s" ), pUnit->GetstrIds().c_str() );
	pOut->Add( str );
	auto spTargetUnit = pUnit->GetspTarget();
	auto secAtk = pUnit->GetSpeedAttack( pUnit->GetspTarget() );
	auto dmgMelee = pUnit->GetAttackMeleeDamage( spTargetUnit, true );
	auto dmgRange = pUnit->GetAttackRangeDamage( spTargetUnit, true );
	float dmgMeleeAdd = dmgMelee - pUnit->GetBaseAtkMeleeDmg();		// 버프보정으로 늘어난 데미지량
	float dmgRangeAdd = dmgRange - pUnit->GetBaseAtkRangeDmg();
	_tstring 
	strAdd = XFORMAT("%+.0f%%", pUnit->GetAddRateByStat( xSTAT_ATK_MELEE, pUnit->GetspTarget() ) * 100.f);
	//
	str = XE::Format( _T( "근접피해량:%.1f(%+.1f,%s)(dps:%1.f)" ), dmgMelee, dmgMeleeAdd, strAdd.c_str(), dmgMelee / secAtk );
	pOut->Add( str );
	strAdd = XFORMAT( "%+.0f%%", pUnit->GetAddRateByStat( xSTAT_ATK_RANGE, pUnit->GetspTarget() ) * 100.f );
	str = XE::Format( _T( "원거리피해량:%.1f(%+.1f,%s)(dps:%1.f)" ), dmgRange, dmgRangeAdd, strAdd.c_str(), dmgRange / secAtk );
	pOut->Add( str );
	strAdd = XFORMAT( "%+.1f%%", pUnit->GetAddRateByStat( xSTAT_SPEED_ATK, pUnit->GetspTarget() ) * 100.f );
	str = XE::Format( _T( "공격속도:%.1f(%s)" ), secAtk, strAdd.c_str() );
	pOut->Add( str );
	strAdd = XFORMAT( "%+.0f%%", pUnit->GetAddRateByStat( xSTAT_DEF, pUnit->GetspTarget() ) * 100.f );
	str = XE::Format( _T( "방어력:%.1f(%s)" ), pUnit->GetDefensePower(), strAdd.c_str() );
	pOut->Add( str );
	strAdd = XFORMAT( "%+.0f%%", pUnit->GetAddRateByStat( xSTAT_HP, pUnit->GetspTarget() ) * 100.f );
	str = XE::Format( _T( "체력:%d/%d(%s)" ), pUnit->GetHp(), pUnit->GetMaxHp(), strAdd.c_str() );
	pOut->Add( str );
	strAdd = XFORMAT( "%+.0f%%", pUnit->GetAddRateByStat( xSTAT_SPEED_MOV, pUnit->GetspTarget() ) * 100.f );
	str = XE::Format( _T( "이동속도:%.1f(%s)" ), pUnit->GetSpeedMovePerSec(), strAdd.c_str() );
	pOut->Add( str );
	str = _T( "---------------------" );
	pOut->Add( str );
	str = XE::Format( _T( "치명타율:%.1f%%" ), pUnit->GetCriticalRatio() * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "치명타배수:x%.1f" ), pUnit->GetCriticalPower() );
	pOut->Add( str );
	str = XE::Format( _T( "적중율:%.1f%%" ), pUnit->GetHitRate( spTargetUnit ) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "회피율:%.1f%%" ), pUnit->GetEvadeRatio( XSKILL::xDMG_NONE, spTargetUnit.get() ) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "관통율:%.1f%%" ), pUnit->GetPenetrationRatio() * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는모든피해율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_DAMAGE_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는근접피해율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_DAMAGE_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는원거리피해율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_DAMAGE_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는모든치명타율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_CRITICAL_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는근접치명타율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_MELEE_CRITICAL_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "받는원거리치명타율:%.1f%%" ), (1.f - -pUnit->GetAdjParam( XGAME::xADJ_RANGE_CRITICAL_RECV )->valPercent) * 100.f );
	pOut->Add( str );
	str = XE::Format( _T( "타격카운트:%d" ), pUnit->GetcntAttack() );
	pOut->Add( str );
}

void XSceneBattle::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
#include "XObjEtc.h"
void XSceneBattle::OnLButtonUp( float lx, float ly ) 
{
#if defined(_DEBUG) || defined(_XUZHU_HOME)

#endif
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneBattle::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}


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
	SET_ATLASES(GetpLayerUI()->GetpAtlas()) {
		auto pWnd = new XWndStatistic( XBattleField::sGet()->GetLegionObj( 0 ),
																	 XBattleField::sGet()->GetLegionObj( 1 ) );
		GetpLayerUI()->Add( pWnd );
		pWnd->SetCancelButton( "butt.close" );
	} END_ATLASES;
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
	XBREAK( !bitWinner );
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
	m_timerResult.Set( 2.f );
#endif // not _XSINGLE
}

void XSceneBattle::DoPopupBattleResult( XGAME::xBattleResult& result )
{
#ifndef _XSINGLE
	auto pBaseSpot = sGetpWorld()->GetSpot( s_BattleStart.m_idSpot );
	XBREAK( pBaseSpot == nullptr );
	auto pPopup = XGAME::DoPopupBattleResult( result, this, pBaseSpot );
	if( pPopup ) {
		int idxLegion = ACCOUNT->GetCurrLegionIdx();
		pPopup->SetEvent( XWM_OK, this, &XSceneBattle::OnOkBattleResult, idxLegion );
		xSetButtHander( pPopup, this, "butt.statistic", &XSceneBattle::OnClickStatistic );
	}
#endif // not _XSINGLE
}

/**
 서버로부터 유황스팟 배틀 결과가 옴(인카운터!)
*/
void XSceneBattle::OnRecvBattleResultSulfurEncounter( XSpotSulfur *pSpot
																										, const XGAME::xBattleStartInfo& info )
{
	XBREAK( XSceneBattle::sIsEmptyBattleStart() );
	//
	auto pPopup = new XWndEncounter( pSpot, info );
	pPopup->SetEnableNcEvent( FALSE );		// 창밖터치로 꺼지지 못하게.
	Add( pPopup );
	pPopup->SetbModal( TRUE );
	xSET_BUTT_HANDLER( pPopup, "butt.attack", &XSceneBattle::OnOkBattleResultSulfurEncounter );
	xSET_BUTT_HANDLER( pPopup, "butt.retreat", &XSceneBattle::OnSulfurRetreat );
}

int XSceneBattle::OnOkBattleResult( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	auto pPopup = new XWndBattleAfterHeroExp( 0, &m_resultBattle );
	if( pPopup ) {
		pPopup->SetEnableNcEvent( FALSE );		// 창밖터치로 꺼지지 못하게.
		Add( pPopup );
	}
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
	if( ACCOUNT->IsEnoughCash( XGAME::xCS_SULFUR_RETREAT ) )	{
		OnFinishBattle( XGAME::xSIDE_PLAYER, true );
		pWnd->GetpParent()->SetbDestroy( TRUE );
	} else	{
		// 캐쉬를 구매할거냐는 창이 뜨고 캐쉬 구매팝업을 띄운다.
	}
	
	return 1;
}

/**
 @brief 
*/
int XSceneBattle::OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _CHEAT
	CONSOLE("%s", __TFUNC__);
	//
// 	auto fmtAtlas = XE::xPF_NONE;
// 	if( p1 == 0 ) {
// 		if( XAPP->m_idxViewAtlas >= 0 )
// 			--XAPP->m_idxViewAtlas;
// 		fmtAtlas = XTextureAtlas::sGet()->GetfmtByidxAtlas( XAPP->m_idxViewAtlas );
// 	} else
// 	if( p1 == 1 ) {
// 		if( XAPP->m_idxViewAtlas < XTextureAtlas::sGet()->GetnumAtlas() - 1 )
// 			++XAPP->m_idxViewAtlas;
// 		fmtAtlas = XTextureAtlas::sGet()->GetfmtByidxAtlas( XAPP->m_idxViewAtlas );
// 	}
// 	if( p1 == 0 || p1 == 1 ) {
// 		CONSOLE( "Atlas=(%d/%d), fmt=%s",
// 						 XAPP->m_idxViewAtlas,
// 						 XTextureAtlas::sGet()->GetnumAtlas(),
// 						 XE::GetstrPixelformat( fmtAtlas ) );
// 	}
	return 1;
#endif // _CHEAT
}


int XSceneBattle::OnDebugButton( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#if defined(_XSINGLE) && defined(WIN32)
	CONSOLE( "%s", __TFUNC__ );
	const bool bRetry = (p1 == 1 );			// 군단배치만 다시 한다.
	const bool bRecreate = (p1 == 2);		// 군단을 다시 생성한다.
	if( bRetry || bRecreate ) {	
		DestroyWndByIdentifier( "butt.skill" );
		DestroyWndByIdentifier( "butt.skill.cancel" );
		if( XAPP->m_bReloadWhenRetryConstant ) {
			CONSOLE( "Load constant..." );
			SAFE_DELETE( CONSTANT );
			GAME->LoadConstant();
		}
		if( XAPP->m_bReloadWhenRetryPropSkill ) {
			SAFE_DELETE( CONSTANT );
			GAME->LoadConstant();		// 스킬은 내부에서 AddConstant를 하므로 삭제하고 다시 읽어야 함.
			CONSOLE( "Load propSkill..." );
			SAFE_DELETE( SKILL_MNG );
			SKILL_MNG = new XSkillMng;
			if( SKILL_MNG->Load( _T( "propSkill.xml" ) ) == FALSE ) {
				XERROR( "propSkill load error" );
			}
		}
		if( XAPP->m_bReloadWhenRetryPropUnit ) {
			CONSOLE( "Load propUnit..." );
			SAFE_DELETE( PROP_UNIT );
			PROP_UNIT = new XPropUnit;
			if( PROP_UNIT->Load( _T( "propUnit.txt" ) ) == FALSE ) {
				XERROR( "load error! %s", PROP_UNIT->GetstrFilename() );
			}
		}
		if( XAPP->m_bReloadWhenRetryPropLegion ) {
			GAME->LoadPropLegion();
//			XPropLegion::sGet()->Save( _T( "test.xml" ) );
		}
		// objmng의 모든 객체를 해제
		XBattleField::sGet()->GetpObjMngMutable()->Release();
		// 군단객체 삭제
		for( auto& camp : m_aryCamp ) {
			camp.m_spLegionObj->Release();
			camp.m_spLegionObj.reset();
			if( bRecreate ) {
				camp.m_spLegion.reset();
			}
		}
		XEBaseScene::Release();
		CheckLeak();
		XLegionObj::sClearnumObj();
		XSquadObj::sClearnumObj();
		xnUnit::XMsgBase::sClearnumObj();
		XEBaseWorldObj::sClearnumObj();
		if( bRecreate ) {
			// XLegion과 XHero들을 모두 파괴한다.
			XAccount::sDestroyPlayer();
			// 계정과 XLegion을 새로 구성
			sCreateAcc();
		}

		// 참조가 없는 모든 자원 해제
		if( XAPP->m_dwFilter & xBIT_FLUSH_SPR )
			SPRMNG->DoFlushCache();
		if( XAPP->m_dwFilter & xBIT_FLUSH_IMG )
			IMAGE_MNG->DoFlushCache();
		//
		m_aryCamp[xSI_PLAYER].m_idsLegion = "single1_player";
		m_aryCamp[xSI_OTHER].m_idsLegion = "single1_enemy";
		for( auto& camp : m_aryCamp ) {
			if( bRecreate ) {
				camp.CreateLegion( camp.m_idsLegion, camp.m_bitSide );
			}
			camp.CreateLegionObj();
			camp.CreateSquadObj( m_pWndWorld, camp.m_bitOption );
		}
		// 각 유닛들에게 OnStart이벤트
		for( auto& camp : m_aryCamp ) {
			camp.m_spLegionObj->OnSkillEvent( xJC_START );
		}
		// 배틀 씬프로세스를 종료시켜 다시 레디상태로 가게 한다.
		m_pProcess->SetbDestroy( TRUE );
		m_pProcess = new XSceneProcessBattle( this, 
																					m_pWndWorld->GetspBattleFieldMutable() );
		m_pProcess->SetstrIdentifier( "scene.process.battle" );
		Add( m_pProcess );
		m_pWndWorld->OnReset( true );
	} // if( bRetry || bRecreate ) {	
	SetbUpdate( true );
	//	m_ptBrave = 0;
#endif // _XSINGLE
	return 1;
}

int XSceneBattle::OnDebugProfile( XWnd*, DWORD, DWORD )
{
	if( XEProfile::sIsActive() == FALSE ) {
		CONSOLE( "profiler start!" );
		XEProfile::sGet()->sDoStart();
	} else {
		XEProfile::sGet()->sDoFinish();
		XEProfile::sGet()->DoEvaluation();
		CONSOLE( "depth(*)\tfunc\tshare\tavg\tmin\tmax\tdepth" );
		XARRAYLINEAR_LOOP( XEProfile::sGetResult(), xnProfile::xRESULT, result ) {
			_tstring str = _T(" ");
			for( int i = 0; i < result.depth; ++i )
				str += _T("+");
			CONSOLE( "%s %s\t%.1f%%\t%d\t%d",
							 str.c_str(),
							 result.strFunc.c_str(),
							 result.ratioShare * 100.f,
							 result.mcsExecAvg,
							 result.cntExec );
		} END_LOOP;
	}
	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneBattle::OnUseSkillByButton( XWnd* pWnd, DWORD p1, DWORD p2 )
{
//	CONSOLE( "OnUseSkill" );
	ID snSquad = (ID)p1;
	auto pButt = dynamic_cast<XWndSkillButton*>( pWnd );
	if( XBREAK( pButt == NULL ) )
		return 1;
	if( !pButt->IsCoolTime() ) {
		XSPSquad spSquad = WORLD->GetSquadBySN( XGAME::xLI_PLAYER, snSquad );
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
 @brief 초상화 윈도우를 찾는다.
*/
XWndFaceInBattle* XSceneBattle::GetpWndFace( ID snHero )
{
	return SafeCast<XWndFaceInBattle*>( Findf( "face.%8x", snHero ) );
}

/**
 @brief 영웅유닛이 스킬을 쓰면 호출된다.
 영웅 초상화에 스킬사용 연출이 등장한다.
*/
void XSceneBattle::OnUseSkill( XSPSquad spSquadObj, const _tstring& strText )
{
	auto pHero = spSquadObj->GetpHero();
	auto pWndFace = GetpWndFace( pHero->GetsnHero() );
	if( pWndFace ) {
		pWndFace->OnUseSkill( strText );
	}
}

/**
 @brief 아군 부대를 선택했을때
*/
void XSceneBattle::OnSelectSquad( const XSPSquad& spSquadSelect )
{
	XE::VEC2 sizeGame = XE::GetGameSize();
	XBREAK( spSquadSelect == nullptr );
	// 캔슬버튼은 이미 떠있으면 새로 생성하지 않는다.
	XWnd *pExist = Find( "butt.skill.cancel" );
	if( pExist == nullptr )	{
		auto pCancel = new XWndButton( sizeGame.w - 70.f,
																	sizeGame.h - 60.f,
																	_T( "common_butt_cancel.png" ), nullptr );
		pCancel->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnCancelSelect );
		pCancel->SetstrIdentifier( "butt.skill.cancel" );
		Add( pCancel );
	}
	auto pSkillDat = spSquadSelect->GetpHero()->GetSkillDatActive();
	if( pSkillDat == NULL )
		return;
	const char *cKey = "butt.skill";
	pExist = Find( cKey );
	if( pExist )
		pExist->SetbDestroy( TRUE );

	XUnitHero *pUnitHero = NULL;
	if( spSquadSelect->GetspHeroUnit() != nullptr )
		pUnitHero = SafeCast<XUnitHero*>( spSquadSelect->GetspHeroUnit().get() );
	auto pButt = new XWndSkillButton( sizeGame.w - 135.f, 
												sizeGame.h - 60.f, pSkillDat, pUnitHero );
	pButt->SetstrIdentifier( cKey );
	pButt->SetEvent( XWM_CLICKED, this, &XSceneBattle::OnUseSkillByButton, spSquadSelect->GetsnSquadObj() );
	Add( pButt );
	//
#ifdef _XSINGLE
	// 부대선택시 정보출력이 켜져있고 영웅정보는 콘솔에 출력도 켜져있으면 콘솔에 출력.
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewSquadInfo && (XAPP->m_dwFilter & xBIT_HERO_INFO_CONSOLE))	{
		auto pUnit = spSquadSelect->GetspHeroUnit().get();
		XVector<_tstring> aryStr;
		GetSquadInfoToAry( spSquadSelect, pUnit, &aryStr );
		for( auto& str : aryStr ) {
			CONSOLE("%s", str.c_str() );
		}
	}
#endif
}

void XSceneBattle::OnDieSquad( XSPSquad spSquadObj )
{
	auto pButtSkill = dynamic_cast<XWndSkillButton*>( Find("butt.skill") );
	if( pButtSkill ) {
		if( pButtSkill->GetsnHero() == spSquadObj->GetpHero()->GetsnHero() ) {
			OnCancelSelect( nullptr, 0, 0 );
		}
	}
#ifdef _XSINGLE
	if( XAPP->m_bWaitAfterWin ) {
		const auto snHero = spSquadObj->GetpHero()->GetsnHero();
		auto idxSquad = spSquadObj->GetspLegionObj()->GetspLegion()->GetIdxSquadByHeroSN( snHero );
		// 한 부대가 전멸하면 같은 라인의 부대를 모두 멈춘다.
		for( auto& camp : m_aryCamp ) {
			int idxCol = (idxSquad % 5);
			auto spSquadObjTarget = camp.m_spLegionObj->GetspSquadObjByIdx( idxCol );
			if( spSquadObjTarget )
				spSquadObjTarget->SetAI( FALSE );
			idxCol += 5;
			spSquadObjTarget = camp.m_spLegionObj->GetspSquadObjByIdx( idxCol );
			if( spSquadObjTarget )
				spSquadObjTarget->SetAI( FALSE );
			idxCol += 5;
			spSquadObjTarget = camp.m_spLegionObj->GetspSquadObjByIdx( idxCol );
			if( spSquadObjTarget )
				spSquadObjTarget->SetAI( FALSE );
		}
	}
#endif // _XSINGLE
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
		auto spLegionObj = XBattleField::sGet()->GetLegionObj(1);
		if( spLegionObj )
			spLegionObj->KillAllUnit();
	} else 
	if( p1 == 2 ) {
		XAPP->m_bDebugViewDamage = !XAPP->m_bDebugViewDamage;
		XAPP->XClientMain::SaveCheat();
	} else
	if( p1 == 3 ) {
		auto spLegionObj = XBattleField::sGet()->GetLegionObj( 0 );
		if( spLegionObj )
			spLegionObj->DoDamage();
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
	if( m_bFinish )
		return 1;
	XBattleField::sGet()->SetAI( false );
	m_bFinish = true;
#ifndef _XSINGLE
	SendFinishBattle( XGAME::xEB_RETREAT, XGAME::xSIDE_OTHER, s_BattleStart.m_idxStage, 0 );
#endif // not _XSINGLE
	return 1;
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
	return m_pWndWorld->GetPosWorldToWindow( vPos );
}
bool XSceneBattle::OnDelegateIsInScreen( const XE::VEC2& vScr )
{
	return true;
}
float XSceneBattle::OnDelegateGetScaleViewport() 
{
	return m_pWndWorld->GetscaleCamera();
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
void XSceneBattle::OnControlSquad( const XHero *pHero )
{
#ifndef _XSINGLE
	if( ACCOUNT->GetFlagTutorial().bControlSquadInBattle == 0 ) {
		XBREAK( pHero == nullptr );
		if( ACCOUNT->GetpQuestMng()->GetQuestObj( _T( "x.quest.battle.control" ) ) ) {
			ACCOUNT->GetFlagTutorial().bControlSquadInBattle = 1;
			GAMESVR_SOCKET->SendControlSquadInBattle( pHero );
		}
	}
#endif // not _XSINGLE
}

void XSceneBattle::DoMoveCamera( const XE::VEC2& vwDstCenter, float secMove )
{
	m_FocusMng.DoMove( m_pWndWorld->GetvwCamera(), vwDstCenter, secMove );
}

XE::VEC2 XSceneBattle::GetvwCamera()
{
	return m_pWndWorld->GetvwCamera();
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
		return spUnit->GetpHeroMutable();
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

XSPSquad XSceneBattle::GetspSquadObj( ID snSquad )
{
	auto spLegionObj = XBattleField::sGet()->GetLegionObj( 0 );
	if( spLegionObj ) {
		auto spSquadObj = spLegionObj->GetSquadBySN( snSquad );
		if( spSquadObj )
			return spSquadObj;
	}
#ifdef _DEBUG
	// 디버그에선 적군까지 검색
	spLegionObj = XBattleField::sGet()->GetLegionObj( 1 );
	if( spLegionObj ) {
		auto spSquadObj = spLegionObj->GetSquadBySN( snSquad );
		if( spSquadObj )
			return spSquadObj;
	}
#endif
	return nullptr;
}

/**
 @brief 영웅 초상화를 눌러 부대를 선택함.
*/
int XSceneBattle::OnTouchHeroFace( XWnd* pWnd, DWORD snSquad, DWORD )
{
	auto spSquadObj = GetspSquadObj( snSquad );
	if( spSquadObj ) {
		// 이전에 선택된게 있었으면 없앰.
		auto spSelectedOld = XWndBattleField::sGet()->GetspSelectSquad();
		if( spSelectedOld ) {
			auto pWndFace = GetpWndFace( spSelectedOld->GetpHero()->GetsnHero() );
			if( pWndFace ) {
				pWndFace->SetbSelected( false );
				pWndFace->SetbUpdate( true );
			}
		}
		// 새로 선택
		XWndBattleField::sGet()->SetspSelectSquad( spSquadObj );
		auto pWndFace = GetpWndFace( spSquadObj->GetpHero()->GetsnHero() );
		if( pWndFace ) {
			pWndFace->SetbSelected( true );
			OnSelectSquad( spSquadObj );
			pWndFace->SetbUpdate( true );
		}
	}
	return 1;
}

void XSceneBattle::CheckLeak()
{
	XBREAK( XLegionObj::sGetnumObj() );
	XBREAK( XSquadObj::sGetnumObj() );
	xnUnit::XMsgQ::sCheckLeak();
	XBREAK( xnUnit::XMsgBase::sGetnumObj() );
	if( XBattleField::sGet() )
		if( XBattleField::sGet()->GetpObjMng() )
			XBattleField::sGet()->GetpObjMngMutable()->_CheckLeak();
	XBREAK( XEBaseWorldObj::sGetnumObj() );
}