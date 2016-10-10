#include "StdAfx.h"
#include "XWndTech.h"
#include "XGame.h"
#include "XPropWorld.h"
#include "XPropCloud.h"
#include "XAccount.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSceneBattle.h"
#include "XWndCloudLayer.h"
#include "client/XAppMain.h"
#include "XQuest.h"
#include "XQuestMng.h"
#include "XSceneTech.h"
#include "XPropHelp.h"
#include "XSeq.h"
#include "XWndObj.h"
#include "XSpots.h"
#include "XSpotDaily.h"
#include "XLegion.h"
#include "XWndSpots.h"
#include "XFramework/XFacebook.h"
#include "XWndWorld.h"
#include "XWndTemplate.h"
#include "XWndQuest.h"
#include "XWorld.h"
#include "XSoundMng.h"
#include "XOption.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndProgressBar.h"
#include "XWndCampaign.h"
#include "XWndCampaignHero.h"
#include "XWndResCtrl.h"
#include "XSystem.h"
#include "XCampObj.h"
#include "XWndPopupDaily.h"
#include "opengl2/XTextureAtlas.h"
#include "XWndStorageItemElem.h"
#include "XImageMng.h"
#include "XWndPrivateRaid.h"
#include "XSpotPrivateRaid.h"
//#include "Sprite/SprMng.h"
#ifdef _xIN_TOOL
#include "XDlgPropCloud.h"
#include "XTool.h"
#endif
#ifdef WIN32
#include "CaribeView.h"
#endif
#include "XSceneWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace xHelp;
using namespace XGAME;

#define WND_QUEST_LIST		"wnd.quest.list"

#define ID_SPOT_TOOL		50000		// 툴용 스팟의 윈도우 아이디 시작번호
#define ID_WND_AREA_COST	60000		// XWnd id예약

#define POS_GREEN_ALERT		XE::VEC2( 71, 2 )

XSceneWorld *SCENE_WORLD = nullptr;

void XSceneWorld::xLayout::DestroyAll() {
	SAFE_DELETE( pShop );
	SAFE_DELETE( pRank );
	SAFE_DELETE( pSocial );
	SAFE_DELETE( pMail );
}

//////////////////////////////////////////////////////////////////////////
void XSceneWorld::Destroy() 
{	
	for( auto psfc : m_aryWorldSurface ) {
		SAFE_RELEASE2( IMAGE_MNG, psfc );
	}
#ifdef _xIN_TOOL
//	SAFE_RELEASE2(IMAGE_MNG, m_psfcBrush );
// 	if( XAPP )
// 		XAPP->m_ToolMode = xTM_NONE;
#endif
	SOUNDMNG->RemoveAll();

	m_spAcc.reset();
	XBREAK( SCENE_WORLD == nullptr );
	XBREAK( SCENE_WORLD != this );
	SCENE_WORLD = nullptr;
}

void XSceneWorld::OnDestroy()
{
#ifdef _xIN_TOOL
	XBaseTool::sDoChangeMode( xTM_NONE );
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->Save();
// 	if( m_pTool )
// 		m_pTool->Save();
#endif // _xIN_TOOL
}
//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
static XUINT64 s_llStart = 0;
static XUINT64 s_llPass = 0;
XSceneWorld::XSceneWorld(XGame *pGame, XSPSceneParam& spBaseParam)
	: XSceneBase( pGame, XGAME::xSC_WORLD )
	, m_Layout(_T("layout_world.xml") )
{ 
	s_llStart = XE::GetFreqTime();
	XBREAK( SCENE_WORLD != nullptr );
	SCENE_WORLD = this;
	Init(); 
} // 생성자

BOOL XSceneWorld::OnCreate()
{
	m_spAcc = XAccount::sGetPlayer();
	// 월드맵을 스크롤뷰에 넣어서 바탕에 깜
	{
// 		auto pImg = new XWndImage( XE::MakePath( DIR_UI, PROP_WORLD->GetstrImg() ), XE::xPF_RGB565, 0, 0 );
// 		pImg->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickWorld );
		auto pView = new XWndScrollView( XE::VEC2( 0 ), XE::GetGameSize() );
		pView->SetpDelegate( this );
		pView->SetstrIdentifier( "scroll.view" );
//		pView->SetViewSize( pImg->GetSizeLocal() );
		pView->SetViewSize( XE::VEC2(2048, 2048) );
		pView->SetEvent( XWM_SCROLL, this, &XSceneWorld::OnScrollWorldmap );
		Add( pView );
		m_pScrollView = pView;
		LoadWorldMap( pView );
		const XE::VEC2 sizeWorld( 2048, 2048 );
// 		pView->Add( pImg );
		// 오브젝트 레이어 바닥용
		{
			XWnd *pLayer = new XWndBgObjLayer( sizeWorld );
			pLayer->SetstrIdentifier( "root.world.obj.under" );
			pView->Add( pLayer );
		}
		// 스팟밑에깔리는 빛 레이어
// 		{
// 			XWnd *pRoot = new XWndBatchRender( "spot_light", true,
// 																				 XE::VEC2( 0, 0 ),
// 																				 sizeWorld );
// 			pRoot->SetstrIdentifier( "root.spot.light" );
// 			pView->Add( pRoot );
// 		}
		// 스팟들의 루트
		{
			XWnd *pRoot = new XWndBatchRender( "spot", false, true,
																				 false, false,
																				 XE::VEC2( 0, 0 ),
																				 sizeWorld );
			pRoot->SetstrIdentifier( "root.spot" );
			pView->Add( pRoot );
		}
		// 자원풍선
// 		{
// 			XWnd *pRoot = new XWndBatchRender( "spot_res", true,
// 																				 XE::VEC2( 0, 0 ),
// 																				 sizeWorld );
// 			pRoot->SetstrIdentifier( "root.spot.res" );
// 			pView->Add( pRoot );
// 		}
		// 오브젝트 레이어
		{
			XWnd *pBirdLayer = new XWndBatchRender( "bird", false, true, 
																							false, false,
																							XE::VEC2(0,0),
																							sizeWorld );
			pBirdLayer->SetstrIdentifier( "root.world.obj" );
			pView->Add( pBirdLayer );
		}
		// 툴용 스팟 레이어
#ifdef _xIN_TOOL
		{
			auto pRootSpot = new XWnd;
			pRootSpot->SetstrIdentifier( "wnd.root.tool.scrollview" );
			pView->Add( pRootSpot );
		}
#endif // _xIN_TOOL
		// 구름레이어
		{
			auto pCloudLayer = new XWndCloudLayer( XE::VEC2( 0 ), sizeWorld );
			pCloudLayer->SetstrIdentifier( "scroll.cloud.view" );
			pView->Add( pCloudLayer );
			if( m_spAcc->GetvFocusWorld().IsZero() )
				pView->SetFocusView( c_vHome );
			else
				pView->SetFocusView( m_spAcc->GetvFocusWorld() );
		}
		// 스팟들 프로필사진 레이어.
		{
			auto pRootProfilePicture = new XWnd();
			pRootProfilePicture->SetstrIdentifier( "root.profile.picture" );
			pView->Add( pRootProfilePicture );
		}
		// 지역명 + 가격 배너 레이어
		{
			auto pRootBanner = new XWnd();
			pRootBanner->SetstrIdentifier( "root.cost.label" );
			pView->Add( pRootBanner );
		}
		// 플레이중 나타나는 배너들 레이어
		{
			auto pRootBanner = new XWnd();
			pRootBanner->SetstrIdentifier( "root.area.banner" );
			pView->Add( pRootBanner );
		}
	}
	// 모든 스팟을 업데이트 한다.
//	UpdateSpots();
	// UI
	// 월드씬에 고정되어있는 UI들(상단자원바, 왼쪽옵션바, 퀘스트리스트, 하단 버튼들, 이름/레벨표시 등)을 
	// 하나의 레이어에 다 몰아넣는다. 툴모드 등으로 전환했을때 ui들을 한꺼번에 사라지게 하기 위함임.
	// 그러므로 이 레이어에는 팝업같은건 넣으면 안됨.
	XWnd *pWndUI = new XWnd();	// ui layer
	pWndUI->SetstrIdentifier( "root.ui" );
	Add( pWndUI );
	m_Layout.CreateLayout( "world", pWndUI );

//	CreateDebugButtons();
	//	UpdateUI();
	// gold/cash/자원 강제 업데이트
// 	for( int i = 0; i < XGAME::xIU_MAX; ++i ) {
// 		UpdateResourceUI2( ( XGAME::xtIndexUI )i, true );
// 	}

	xSET_BUTT_HANDLER_RET( pButt, this, "butt.unitorg", &XSceneWorld::OnClickUnitOrg );
	if( pButt ) {
		pButt->SetpDelegate( GAME );
		pButt->SetEvent( XWM_SOUND_DOWN, GAME, &XGame::OnSoundDown, 29 );
	}
	pButt = xSET_BUTT_HANDLER( this, "butt.tech", &XSceneWorld::OnClickLaboratory );
	pButt->SetEvent( XWM_SOUND_DOWN, GAME, &XGame::OnSoundDown, 29 );
	pButt = xSET_BUTT_HANDLER( this, "butt.hero", &XSceneWorld::OnClickHero );
	pButt->SetEvent( XWM_SOUND_DOWN, GAME, &XGame::OnSoundDown, 29 );
	pButt = xSET_BUTT_HANDLER( this, "butt.storage", &XSceneWorld::OnClickStorage );
	pButt->SetEvent( XWM_SOUND_DOWN, GAME, &XGame::OnSoundDown, 29 );
	pButt = xSET_BUTT_HANDLER( this, "butt.market", &XSceneWorld::OnClickShop );
	pButt->SetEvent( XWM_SOUND_DOWN, GAME, &XGame::OnSoundDown, 29 );
	xSET_BUTT_HANDLER( this, "img.player.face", &XSceneWorld::OnClickFace );
	xSetButtHander( this, this, "butt.log.battle", &XSceneWorld::OnClickBattleLog );

	xSET_BUTT_HANDLER( this, "butt.world.rank", &XSceneWorld::OnClickRank );
	xSET_BUTT_HANDLER( this, "butt.world.social", &XSceneWorld::OnClickSocial );
	xSET_BUTT_HANDLER( this, "butt.world.mailbox", &XSceneWorld::OnClickMailbox );
	xSET_BUTT_HANDLER( this, "butt.world.option", &XSceneWorld::OnClickOption );
	xSET_BUTT_HANDLER( this, "butt.world.hide", &XSceneWorld::OnClickLeftMenuHide );
	OnClickLeftMenuHide( this, 0, 0 );
	// 화살표들 안눌리게
// 	xSET_ACTIVE( this, "img.world.left.right", FALSE );
// 	xSET_ACTIVE( this, "img.world.left.left", FALSE );

#ifdef WIN32
#ifdef _xIN_TOOL
	// 툴빌드에서는 처음에 propCloud갱신을 위해 일단 모든구름을 다 생성한다.
	// 개발버전에서는 스팟위치가 바꼈을수도 있으므로 구름프로퍼티를 모두 다시 돌며
	// 구름밑에 깔리는 스팟리스트를 다시 갱신한다.
	BOOL bChanged = UpdateCloudSpotList( true );
	if( bChanged ) {
		AfxMessageBox( _T( "propCloud가 갱신되었습니다. 서버를 다시실행시켜 주십시오." ) );
		BOOL bRet = PROP_CLOUD->Save( _T( "propCloud2.xml" ) );
		XBREAK( bRet == FALSE );
	}
#endif
#endif
	SetbUpdate( TRUE );
	// 우편물 체크
	if( m_spAcc->GetNumPostInfo() != 0 ) {
		xSET_SHOW( this, "img.world.left.mailboxalert1", TRUE );
		xSET_SHOW( this, "img.world.left.mailboxalert2", FALSE );
	}
	// UI 파티클 레이어
	{
		auto pLayer = new XWnd();
		pLayer->SetstrIdentifier( "root.layer.ui.particle" );
		Add( pLayer );
	}
	return TRUE;
} // OnCreate()

void XSceneWorld::Create( void )
{
	XEBaseScene::Create();
}
/**
 @brief 배경 월드맵을 로딩한다.
 기기 최대 텍스쳐가 4096이 안된다면 1024단위로 4개를 잘라서 올린다.
*/
void XSceneWorld::LoadWorldMap( XWndScrollView* pScrlView )
{
	// 기기 최대 텍스쳐 크기를 얻는다.
	const _tstring resImg = XE::MakePath( DIR_UI, PROP_WORLD->GetstrImg() );
	if( XSurface::GetMaxSurfaceWidth() >= 4096 ) {
		auto pImg = new XWndImage( resImg, XE::xPF_RGB565, 0, 0 );
		pImg->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickWorld );
		pScrlView->Add( pImg );
	} else {
		XE::xImage img;
		GRAPHICS->LoadImg( resImg, &img );
		if( XASSERT(img.m_pImg) ) {
			for( int i = 0; i < 4; ++i ) {
				for( int k = 0; k < 4; ++k ) {
					const XE::POINT posMemSrc( k * 1024, i * 1024 );
					const XE::POINT sizeArea( 1024, 1024 );
					const XE::POINT sizeMemSrc( 4096, 4096 );
//					auto pSurface = GRAPHICS->CreateSurface();
					const std::string strKey = XE::Format( "world%d.%d", i, k );
					auto pSurface = IMAGE_MNG->CreateSurface( strKey );
					// 레퍼런스 카운트 설계가 잘못되서 m_aryWorldSurface로 받아뒀다가 직접 해제시켜야한다. 
					m_aryWorldSurface.push_back( pSurface );
					pSurface->CreateSub( posMemSrc
															, sizeArea
															, sizeMemSrc
															, img.m_pImg 
															, XE::xPF_ARGB8888
															, sizeArea.ToVec2() * 0.5f
															, XE::VEC2(0)
															, XE::xPF_RGB565
															, false, false );
					const XE::VEC2 posImg( k * 512, i * 512 );
					auto pWndImg = new XWndImage( posImg );
					pWndImg->SetSurfacePtr( pSurface );
					pWndImg->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickWorld );
					pScrlView->Add( pWndImg );
				}
			}
		}
	}
	// png를 RAW로 읽는다.
	// 1024단위로 4개로 잘라서 서피스 4개를 만든다
	// 각 서피스를 월드스크롤뷰에 나눠 배치한다.
	
}
/**
 @brief 모든로딩이 끝난상태에서 페이드인되고 최초 씬에 진입함.
*/
int XSceneWorld::OnEnterScene( XWnd*, DWORD p1, DWORD )
{
	const bool bReEnter = (p1 != 0);			// OnEnterScene으로 재진입
	if( !bReEnter ) {
		auto pLayerBgObj = GetpLayerBgObjUnder();
		if( pLayerBgObj ) {
			const auto vwCenter = GetvwCamera();
			const auto sizeView = GetvwSizeView();
			pLayerBgObj->UpdateCurrFocus( vwCenter, sizeView );
		}
		SOUNDMNG->OpenPlaySound( 25 );			// 월드씬 진입사운드.
		auto idLastScene = GAME->GetpSceneMng()->GetidLastScene();
		if( idLastScene == XGAME::xSC_INGAME )
			SOUNDMNG->OpenPlaySound( 7 );		// 함성소리
	}
	//
	if( m_spAcc->GetXFLevelObj().GetbLevelUp() ) {
		DoPopupLevelup();
		return 1;
	} else
	if( m_spAcc->GetnumAttaccked() > 0 && m_spAcc->IsAblePvP() ) {
		OnAttackedLog();
		return 1;
	} else
	if( GAME->GetlistAlertWorld().size() > 0 ) {
		auto& listAlert = GAME->GetlistAlertWorld();
		auto pElem = listAlert.GetpFirst();
		if( XASSERT(pElem) ) {
			if( pElem->m_Type == xAW_RESEARCH_COMPLETE ) {
				auto pHero = XAccount::sGetPlayer()->GetpHeroBySN( pElem->m_snHero );
				auto pPopup = new XWndResearchComplete( pHero, 
																								pElem->m_idParam, 
																								pElem->m_Level );
				Add( pPopup );
				listAlert.pop_front();
			} else 
			if( pElem->m_Type == xAW_TRAIN_COMPLETE ) {
				switch( pElem->m_Train ) {
				case xTR_LEVEL_UP: {

				} break;
				case xTR_SQUAD_UP: {
				} break;
				case xTR_SKILL_ACTIVE_UP: {
				} break;
				case xTR_SKILL_PASSIVE_UP: {
				} break;
				default:
					XBREAK(1);
					break;
				}
			} else {
				auto pAlert = new XGameWndAlert( pElem->m_strMsg, nullptr, XWnd::xOK );
				if( pAlert ) {
					pAlert->SetEvent( XWM_OK, this, &XSceneWorld::OnEnterScene, 1 );
					Add( pAlert );
					pAlert->SetbModal( TRUE );
					listAlert.DelByIdx( 0 );
				}
			}
			return 1;
		}
	}
	XSceneBase::OnEnterScene( nullptr, 0, 0 );
	// 오늘의 팁
//#ifndef _DEBUG
	if( !GAME->IsPlayingSeq() && ACCOUNT->GetLevel() > 8 ) {
		if( GAME->GetbTodaysTip() == false ) {
			ID idText = xRandom( 55500, 55519 );;
			auto pTip = XWND_ALERT( "%s", XTEXT( idText ) );
			if( pTip ) {
				pTip->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnEnterScene,1 );
				pTip->SetbModal( TRUE );
				GAME->SetbTodaysTip( true );
				return 1;
			}
		}
	}
//#endif // not _DEBUG
	if( XAccount::s_bTraderArrived ) {
		// 다른씬에 있을때 무역상이 도착했었음.
		OnTraderArrive();		
	}
	s_llPass = XE::GetFreqTime() - s_llStart;
	XLOGP( "%s, %llu", _T("XSceneWorld"), s_llPass );
	return 1;
}

/**
 @brief 씬이 모두 밝아지고 호출됨
*/
void XSceneWorld::DoPopupLevelup()
{
	auto pPopup = new XWndLevelup();
	// 팝업의 확인을 누르면 다시 OnEnterScene으로 돌아가게 했다.
	pPopup->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnEnterScene, 1 );
	AddWndTop( pPopup );
	pPopup->SetbModal( TRUE );
	//	GAME->SetbLevelUp( false );
	m_spAcc->GetXFLevelObj().SetbLevelUp( FALSE );
	GAMESVR_SOCKET->SendAccepLevelUp();
	auto pWndSound = new XWndPlaySound( 14 );
	pPopup->Add( pWndSound );
//	SOUNDMNG->OpenPlaySound( 14 );
	//
#if defined(WIN32) && defined(_CHEAT) && defined(_XUZHU)
	GAME->WriteGoldLog( "Lv%d: pow:%d g:%d %d/%d/%d/%d/%d",
											m_spAcc->GetLevel(), m_spAcc->GetPowerExcludeEmpty(),
											m_spAcc->GetGold(),
											m_spAcc->GetWood(),
											m_spAcc->GetIron(),
											m_spAcc->GetJewel(),
											m_spAcc->GetSulphur(),
											m_spAcc->GetMandrake() );
#endif // defined(WIN32) && defined(_CHEAT)&& defined(_XUZHU)
}


/**
 @brief 오프라인동안 침공당한 기록을 안내한다.
*/
void XSceneWorld::OnAttackedLog()
{
	XBREAK( m_spAcc->GetnumAttaccked() == 0 );
	auto pAlert = XWND_ALERT( "%s", XTEXT( 2117 ) );   // 없는동안 공격받았다.
	if( pAlert )
		// 더이상 띄울팝업이 없을때까지 계속 이쪽으로 호출된다.
		pAlert->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnEnterScene, 1 );
	m_spAcc->SetnumAttaccked( 0 );
	XWndBattleLog::s_bAlert = true;
	SetbUpdate( true );
}

void XSceneWorld::SpotcollectLock(ID idSpot)
{
	// 연속해서 누르지 못하게.
	if (m_timerClickSpot.IsOff() || m_timerClickSpot.IsOver())
	{
		GAMESVR_SOCKET->SendSpotTouch(idSpot);
		if (m_timerClickSpot.IsOff())
			m_timerClickSpot.Set(3.f);
		else
			m_timerClickSpot.Reset();
	}
}

int XSceneWorld::OnCloseCenter(XWnd* pWnd, DWORD p1, DWORD p2)
{
	if (pWnd)
		pWnd->SetbDestroy(TRUE);

	return 1;
}

/**
 보석광산 스팟 UI를 생성
*/
BOOL XSceneWorld::CreateJewelSpot( XSpotJewel *pSpot )
{
	XWnd *pView = Find("scroll.view");
	if( pView )
	{
		XWndJewelSpot *pWndSpot = new XWndJewelSpot( pSpot );
		pView->Add( pSpot->GetidSpot(), pWndSpot );
		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
		return TRUE;
	}
	return FALSE;
}

/**
 만드레이크 스팟 UI를 생성
*/
BOOL XSceneWorld::CreateMandrakeSpot( XSpotMandrake *pSpot )
{
	XWnd *pView = Find("scroll.view");
	if( pView )
	{
		XWndMandrakeSpot *pWndSpot = new XWndMandrakeSpot( pSpot );
		pView->Add( pSpot->GetidSpot(), pWndSpot );
		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
		return TRUE;
	}
	return FALSE;
}
// 
// 
// void XSceneWorld::UpdateCastleSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	ID idSpot = pBaseSpot->GetidSpot();
// 	auto pSpot = static_cast<XSpotCastle*>( pBaseSpot );
// 	XWnd *pExist = pRoot->Find( idSpot );
// 	if( pExist == nullptr )	{
// 		auto pWndSpot = new XWndCastleSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add(idSpot, pWndSpot);
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, idSpot );
// 	} 
// 	UpdateResIcon( pSpot, pRoot );
// }
// 
// 
// void XSceneWorld::UpdateJewelSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotJewel*>( pBaseSpot );
// 	ID idSpot = pSpot->GetidSpot();
// 	XWnd *pExist = pRoot->Find( idSpot );
// 	if( pExist == nullptr ) {
// 		auto pWndSpot = new XWndJewelSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( idSpot, pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, idSpot );
// 	}
// 	UpdateResIcon( pSpot, pRoot );
// }
// 
// 
// /**
//  모든 유황스팟 UI를 업데이트 한다.
// */
// void XSceneWorld::UpdateSulfurSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotSulfur*>( pBaseSpot );
// 	std::string strIdentifier = SZ2C(pSpot->GetpProp()->strIdentifier.c_str());
// 	XWnd *pWnd = pRoot->Find( strIdentifier.c_str() );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndSulfurSpot( pSpot );
// 		pWndSpot->SetstrIdentifier( strIdentifier.c_str() );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// 	UpdateResIcon( pSpot, pRoot );
// }
// 
// /**
//  모든 Npc스팟 UI를 업데이트 한다.
// */
// void XSceneWorld::UpdateNpcSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotNpc*>( pBaseSpot );
// 	XBREAK( pSpot == nullptr );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndNpcSpot( pSpot );
// 		XBREAK( pSpot->GetpBaseProp() == nullptr );
// 		std::string ids = SZ2C(pSpot->GetpBaseProp()->strIdentifier.c_str());
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// }
// 
// /**
//  모든 Daily스팟 UI를 업데이트 한다.
// */
// void XSceneWorld::UpdateDailySpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotDaily*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// // 	if( pSpot->GetType() != XGAME::xDS_NONE ) {
// 		if( pWnd == nullptr ) {
// 			auto pWndSpot = new XWndDailySpot( pSpot );
// 			std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 			pWndSpot->SetstrIdentifier( ids );
// 			pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 			pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 		}
// // 	}
// }
// 
// /**
//  모든 만드레이크스팟 UI를 업데이트 한다.
// */
// void XSceneWorld::UpdateMandrakeSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotMandrake*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndMandrakeSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// 	UpdateResIcon( pBaseSpot, pRoot );
// }
// 
// /**
//  @brief 캠페인 스팟을 업데이트 한다.
// */
// void XSceneWorld::UpdateCampaignSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotCampaign*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// 	if( pWnd == nullptr )	{
// 		auto pWndSpot = new XWndCampaignSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// }
// 
// void XSceneWorld::UpdateVisitSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotVisit*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndVisitSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// }
// 
// void XSceneWorld::UpdateCashSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	auto pSpot = static_cast<XSpotCash*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( pSpot->GetidSpot() );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndCashSpot( pSpot );
// 		std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 		pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( pSpot->GetidSpot(), pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
// 	}
// }
// 
// void XSceneWorld::UpdatePrivatgeRaidSpot( )
// 
// void XSceneWorld::UpdateCommonSpot( XSpot *pBaseSpot, XWnd *pRoot )
// {
// 	XBREAK( pRoot == nullptr );
// 	ID idSpot = pBaseSpot->GetidSpot();
// 	auto pSpot = static_cast<XSpotCommon*>( pBaseSpot );
// 	XWnd *pWnd = pRoot->Find( idSpot );
// 	if( pWnd == nullptr ) {
// 		auto pWndSpot = new XWndCommonSpot( pSpot );
// 	std::string ids = SZ2C( pSpot->GetpBaseProp()->strIdentifier.c_str() );
// 	pWndSpot->SetstrIdentifier( ids );
// 		pRoot->Add( idSpot, pWndSpot );
// 		pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, idSpot );
// 	}
// }


/**
 @brief 자원버블(bubble) 업데이트
*/
void XSceneWorld::UpdateResIcon( XSpot *pSpot, XWnd *pRoot )
{
	XBREAK( pSpot == nullptr );
	XBREAK( pRoot == nullptr );
	auto pWnd = SafeCast<XWndResIcon*>( pRoot->Find( pSpot->GetsnSpot() ) );
	//이부분 테스트 필요함
	if( pSpot->IsAbleGetLocalStorage() ) {
		if( pWnd == nullptr ) {
			auto vPos = pSpot->GetPosWorld() - XE::VEC2( 25, 79 );
			pWnd = new XWndResIcon( pSpot->GettypeSpot(), vPos );
			pRoot->Add( pSpot->GetsnSpot(), pWnd );
			pWnd->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, pSpot->GetidSpot() );
		}
	} else {
		if( pWnd )
			pWnd->SetbDestroy( TRUE );
	}
}


void XSceneWorld::Update( void )
{
#ifdef _CHEAT
	UpdateDebugButtons();
 #endif // cheat
#ifdef _CHEAT
	auto pLayerBg = GetpLayerBgObjUnder();
	if( pLayerBg )
		pLayerBg->SetbShow( XAPP->m_bViewBgObj );
	auto pLayerBird = GetpRootWorldObj();
	if( pLayerBird ) {
		pLayerBird->SetbShow( XAPP->m_bViewBgObj );
	}
	auto pLayerSpot = SafeCast<XWndBatchRender*>( Find("root.spot") );
#else
#endif // _CHEAT
	UpdateUI();
	UpdateSpots();
	_UpdateCloud();
	// 구름과 그구름의 배너를 업데이트 한다.
	UpdateCostLabel();
	// 레벨업
	if( m_spAcc->GetXFLevelObj().GetbLevelUp() ) {
		DoPopupLevelup();
	}
	XEBaseScene::Update();
}

/**
 @brief 구름목록에 따라 구름이미지를 업데이트 한다.
*/
void XSceneWorld::_UpdateCloud()
{
	// 구름 레이어
	auto pCloudLayer = SafeCast<XWndCloudLayer*>( Find( "scroll.cloud.view" ) );
	if( XBREAK( !pCloudLayer ) )
		return;
	XArrayLinearN<ID, 512> aryCloud;
#ifdef _xIN_TOOL
	pCloudLayer->SetidSelected( m_idSelected );
// 	if( m_pTool && ( m_pTool->IsCloudTool() || m_pTool->IsSpotTool() ) )
	const auto modeCurr = XBaseTool::sGetCurrMode();
	if( modeCurr == xTM_CLOUD || modeCurr == xTM_SPOT )
		PROP_CLOUD->GetPropToAry( &aryCloud );
	else
#endif // _xIN_TOOL
		sGetpWorld()->GetClosedCloudsToAry( &aryCloud );
	// 구름레이어에 닫힌구름들 목록을 카피시킨다.
	pCloudLayer->UpdateClouds( aryCloud );
	pCloudLayer->SetbUpdate( true );
}
/**
 @brief 다음 열릴수 있는 지역의 가격라벨을 표시한다.
 현재레벨+1까지는 다 열수 있다.
 선행오픈지역이 있는경우 그 지역이 오픈된상태여야 오픈가능해진다
*/
void XSceneWorld::UpdateCostLabel()
{
	// 지역명 + 가격 레이어
	auto pLabelLayer = Find("root.cost.label");
	if( XBREAK( !pLabelLayer ) )
		return;
#ifdef _xIN_TOOL
//	if( IsToolMode() ) {
	if( XBaseTool::s_pCurr ) {
		pLabelLayer->SetbShow( false );	// 툴모드에서는 가격레이블 감춤
		return;
	}
#endif // _xIN_TOOL
	// 오픈한 지역중에 가장 높은 레벨을 얻는다.
	int lvMaxOpenedArea = sGetpWorld()->GetMaxLevelOpenedArea();
	// 프로퍼티에서 모든 지역프롭을 꺼낸다.
	XVector<XPropCloud::xCloud*> aryArea;
	PROP_CLOUD->GetPropToAry( &aryArea );
	//
	for( auto pProp : aryArea ) {
		if( pProp->lvArea == 0 )
			break;
		// 닫힌 지역은 지역명+가격 배너를 생성시키고 오픈된 지역은 삭제시킨다.
		std::string idsWnd = XE::Format( "cost.label.%s", SZ2C( pProp->strIdentifier ) );
//		auto pExist = pLabelLayer->Find( idsWnd );
		bool bOpened = sGetpWorld()->IsOpenedArea( pProp->idCloud );
		bool bCreate = false;
		bool bLock = false;
#ifdef _CHEAT
		if( XAPP->m_bDebugViewAreaLabel )
			bCreate = true;	// 모두표시 상태면 무조건 염.
#endif // _CHEAT
		if( bOpened ) {
			// 이미 열린지역.라벨삭제(있다면)
#ifdef _CHEAT
			// 치트모드가 아니라면 여긴 항상 false여야 한다.
			if( XAPP->m_bDebugViewAreaLabel == false ) 
#endif // _CHEAT
				XASSERT( bCreate == false );
		} else {
			// 닫힌지역.조건 판정
			xtError errCode = xE_OK;
			// 지역이 잠금상태(자물쇠)인가.
			bLock = m_spAcc->IsLockArea( pProp, &errCode );
			if( bLock ) {
				// pProp지역은 현재 잠김상태
				// pProp지역을 퀘스트가 가리키고 있으면 표시함.
				if( IsQuestDirectionToArea( pProp ) ) {
					bCreate = true;
				} else {
					if( errCode == xE_MUST_PRECEDE_AREA_OPEN ) {
						// 선행지역이 잠겨있어 pProp을 못염.
						// x단계이내 precedeArea가 열려있으면 표시함.
						if( !pProp->idsPrecedeArea.empty() )	// 가장 첫번째 지역은 null이다.
							if( sGetpWorld()->IsOpenPrecedeAreaWithDepth( pProp, 2 ) ) {
								bCreate = true;
							}
					} else 
					if( errCode == XGAME::xE_NOT_ENOUGH_LEVEL ) {
						// 이전지역은 열려있는데 레벨이 안되서 못염.
						bCreate = true;		// 라벨은 표시
					} else
					if( errCode == xE_MUST_NEED_KEY_ITEM ) {
						// 키 아이템이 없음.
						bCreate = true;
					} else {
						XBREAK(1);		// 이런상황 나오면 처리해줘야함.
					}
				}
				// 잠금상태이지만 선행지역 오픈형태만 아니면 다 표시
				// 유닛 언락지역이면 무조건 표시
				if( pProp->unitUnlock ) 
					bCreate = true;
			} else {
				// 닫혀있으나 깔수 있는 상태
				bCreate = true;
			}
			// !bOpened
		}
		// true면 생성 false면 기존거 삭제(처리안함이 아님)
		const XE::VEC2 vCenter = PROP_CLOUD->GetPosByArea( pProp ) + pProp->vAdjust;
		auto pExist = pLabelLayer->Find( idsWnd );
		if( bCreate ) {
//			if( !pLabelLayer->Find( idsWnd ) ) {
			if( pExist == nullptr ) {
				XBREAK( vCenter.IsZero() );
				if( !vCenter.IsZero() ) {
					// 가격레이블 컨트롤 생성 및 위치조절
					auto pLabel = new XWndAreaBanner( vCenter, pProp->idCloud, XTEXT( pProp->idName ) );
					pExist = pLabel;
					pLabel->SetstrIdentifier( idsWnd );
					pLabel->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickCloudLabel, pProp->idCloud );
					pLabelLayer->Add( ID_WND_AREA_COST + pProp->idCloud, pLabel );
				}
			}
		} else {
			if( pExist )
				pExist->SetbDestroy( true );
		}
		if( pExist && !bLock ) {
			UpdateAreaAttension( vCenter );
		}
	}
}
/**
 @brief pPropArea를 현재 보유한 퀘스트중 하나가 가리키고 있으면 true를 리턴한다.
 //recon_spot같은것처럼 스팟이나 지역을 가리키는 타입은 퀘가 지역을 가리킨다고 인식하고 true리턴해줘야함.
*/
bool XSceneWorld::IsQuestDirectionToArea( XPropCloud::xCloud* pPropArea )
{
	auto pQuestMng = m_spAcc->GetpQuestMng();
	bool bHave = pQuestMng->IsHaveQuestTypeWithParam( XGAME::xQC_EVENT_OPEN_AREA, pPropArea->idCloud, 0 );
	if( bHave )
		return true;
	// what param의 타입이 AREA이며 pPropArea를 가리키는 퀘를 얻는다.
	auto pQObj = pQuestMng->IsHaveQuestParamType( XGAME::xPT_AREA, pPropArea->idCloud );
	if( pQObj )
		return true;
	// pPropArea에 속한 스팟을 가리키는 퀘가 있는지 검색.
	XVector<ID> arySpots;
	pPropArea->GetSpotsToAry( &arySpots );
	for( auto idSpot : arySpots ) {
		if( pQuestMng->IsHaveQuestParamType( XGAME::xPT_SPOT, idSpot ) )
			return true;
	}
	return bHave;
}

/**
 @brief 모든 스팟들 업데이트
*/
void XSceneWorld::UpdateSpots( void )
{
	XWnd *pView = Find( "scroll.view" );
	if( pView == nullptr )
		return;
#ifdef _xIN_TOOL
	// 게임모드에선 툴용 스팟UI모두 감춤..
	XWnd *pRoot = pView->Find( "wnd.root.tool.scrollview" );
	if( pRoot )
		pRoot->SetbShow( XBaseTool::sIsToolMode() );
//		pRoot->SetbShow( m_pTool != nullptr );
	if( XBaseTool::sIsToolMode() ) {
		UpdateSpotForTool();
		return;
	} else {
//			pRoot->SetbDestroy( TRUE );
	}
#endif // _xIN_TOOL
	XArrayLinearN<XSpot*, 1024> ary;
	sGetpWorld()->GetSpotsToAry( &ary );
	do  {
		XWnd *pRoot = GetpRootSpot();
		if( pRoot == nullptr )
			break;
		int idx = 0;
		// 각 스팟 UI를 생성
		XARRAYLINEARN_LOOP_AUTO( ary, pBaseSpot ) {
			const auto typeSpot = pBaseSpot->GettypeSpot();
			const ID idSpot = pBaseSpot->GetidSpot();
			XWndSpot* pWndSpot = SafeCast<XWndSpot*>( pRoot->Find( idSpot ) );
			if( pWndSpot == nullptr ) {
				// 이미 생성되었으면 다시 생성하지 않음.
				pWndSpot = XWndSpot::sCreate( pBaseSpot );
				const std::string ids = SZ2C( pBaseSpot->GetpBaseProp()->strIdentifier );
				pWndSpot->SetstrIdentifier( ids );
				pRoot->Add( idSpot, pWndSpot );
				pWndSpot->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickSpot, idSpot );
			}
			if( XASSERT(pWndSpot) ) {
				// 자원수거하는 형태면 아이콘 컨트롤도 생성.
				UpdateResIcon( pBaseSpot, pRoot );
			}
// 			switch( typeSpot ) {
// 			case XGAME::xSPOT_CASTLE: UpdateCastleSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_JEWEL: UpdateJewelSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_SULFUR: UpdateSulfurSpot( pBaseSpot, pRoot );	break;
// 			case XGAME::xSPOT_MANDRAKE: UpdateMandrakeSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_NPC: UpdateNpcSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_DAILY: UpdateDailySpot( pBaseSpot, pRoot ); break;
// //			case XGAME::xSPOT_SPECIAL: UpdateSpecialSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_CAMPAIGN: UpdateCampaignSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_VISIT: UpdateVisitSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_CASH: UpdateCashSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_PRIVATE_RAID: UpdatePrivateSpot( pBaseSpot, pRoot ); break;
// 			case XGAME::xSPOT_COMMON: UpdateCommonSpot( pBaseSpot, pRoot ); break;
// 			default:
// 				XBREAKF( 1, "unknown spot type:type=%d", pBaseSpot->GettypeSpot() );
// 				break;
// 			}
			++idx;
		} END_LOOP;
	} while (0);
}

/**
 @brief 프로필사진 액자와 사진의 일반화된 레이아웃
*/
bool XGAME::UpdateProfileImage( XWnd *pRoot, const std::string& strIds, bool bShow, XSurface *psfcPicture )
{
	auto pRootLayout = pRoot->Find( strIds );
	if( pRootLayout ) {
		pRootLayout->SetbShow( bShow );
		if( bShow == false )
			return true;
		auto pImgBg = xGET_IMAGE_CTRL( pRoot, "img.profile.bg" );
		if( pImgBg ) {
			// 프로필이미지를 서피스로 사용.
			auto pImg = xGET_IMAGE_CTRL( pRoot, "img.profile" );
			if( pImg ) {
				pImg->SetSurface( psfcPicture );		// 프로필 사진을 갱신한다.
			}
		}
		return pImgBg != nullptr;
	}
	return false;
}
/**
 @brief 
*/
void XSceneWorld::UpdateUI( void )
{
	auto pLayerUI = GetpFixedUIRoot();
	XBREAK( pLayerUI == nullptr );
	XGAME::CreateUpdateTopResource( pLayerUI );
#ifdef _xIN_TOOL
//  if( m_pTool && !m_pTool->IsGameMode() ) {
	if( XBaseTool::sIsToolMode() ) {
		xSET_SHOW( this, "wnd.user.info", false );
//    xSET_SHOW( this, "wnd.res.info", false );
		xSET_SHOW( this, "wnd.left.butts", false );
		xSET_SHOW( this, "wnd.butt.main", false );
		return;
	} else {
		xSET_SHOW( this, "wnd.user.info", true );
//    xSET_SHOW( this, "wnd.res.info", true );
		xSET_SHOW( this, "wnd.left.butts", true );
		xSET_SHOW( this, "wnd.butt.main", true );
	}
#endif
	auto spAcc = ACCOUNT;
	if( spAcc == nullptr )
		return;
	auto pTextAp = 
	xSET_SHOW( this, "text.ap", TRUE );
	xSET_TEXT( this, "text.ap", XFORMAT("%d/%d", m_spAcc->GetAP(), m_spAcc->GetmaxAP()));
	pTextAp->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickCtrlWnd );
	UpdateUnitOrg();	// 부대편성메뉴 업데이트
	UpdateTech();		// 기술연구메뉴 업데이트
	UpdateSummonHero();	// 영웅소환메뉴 업데이트
	UpdateStorage();	// 창고메뉴 업데잍느
	UpdateMarket();		// 시장메뉴 업데이트.
	XWnd *_pWnd = xGET_TEXT_CTRL( this, "text.name");
	int expMax = spAcc->GetMaxExpCurrLevel();
	if( expMax == 0 )
		expMax = 1;
	// 플레이어 프로필 이미지
	{
		auto pImgName = xGET_IMAGE_CTRL( this, "img.name.bg" );		// 이름/레벨등이 표시되는 컨트롤
		auto pImgScore = xGET_IMAGE_CTRL( this, "img.score.bg" );		// 이름/레벨등이 표시되는 컨트롤
		auto pImgPower = xGET_IMAGE_CTRL( this, "img.power.bg" );		// 이름/레벨등이 표시되는 컨트롤
		static XE::VEC2 s_vPosOrig = (pImgName)? pImgName->GetPosLocal() : XE::VEC2(0);
		static XE::VEC2 s_vPosOrig2 = ( pImgScore ) ? pImgScore->GetPosLocal() : XE::VEC2( 0 );
		static XE::VEC2 s_vPosOrig3 = ( pImgPower ) ? pImgPower->GetPosLocal() : XE::VEC2( 0 );
		if( pImgName ) {
			static auto s_strFilePrev = pImgName->GetpSurface()->GetstrRes();
			if( ACCOUNT->IsLvLimit() ) {
				pImgName->SetSurfaceRes( PATH_UI("world_top_exp_bg_red.png") );
			} else {
				pImgName->SetSurfaceRes( s_strFilePrev );
			}
		}
		bool bHaveProfile = (GAME->GetpsfcProfile() != nullptr);
		if( bHaveProfile ) {
			SetButtHander( this, "img.profile", &XSceneWorld::OnClickProfile );
			bool bOk = XGAME::UpdateProfileImage( this, "wnd.profile", bHaveProfile, GAME->GetpsfcProfile() );
			if( bOk ) {
				if( pImgName ) {
					// 기존 이름/레벨 컨트롤의 위치를 옮겨준다.
					pImgName->SetPosLocal( s_vPosOrig + XE::VEC2(53,0) );
					pImgScore->SetPosLocal( s_vPosOrig2 + XE::VEC2(53,0) );
					pImgPower->SetPosLocal( s_vPosOrig3 + XE::VEC2(53,0) );
				}
			}
		} else {
			// 페북로그인이 아님.
			if( pImgName ) {
				// 기존 이름/레벨 컨트롤의 위치를 옮겨준다.
				pImgName->SetPosLocal( s_vPosOrig );
				pImgScore->SetPosLocal( s_vPosOrig2 );
				pImgPower->SetPosLocal( s_vPosOrig3 );
			}
		}
	}
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pText =
		xSET_TEXT( this, "text.name", XE::Format( _T( "%s(%d)" ), spAcc->GetstrName(), spAcc->GetidAccount() ) );
		if( pText ) {
			pText->SetAlign( XE::xALIGN_LEFT );
			pText->SetLineLength( 256.f );
		}
		pText = 
		xSET_TEXT( this, "text.exp", XE::Format( _T( "%d/%d(%.1f%%)" )
																	, spAcc->GetExp(), spAcc->GetMaxExpCurrLevel()
																	, ( (float)spAcc->GetExp() / expMax * 100.0f )) );
		if( pText )
			pText->SetbShow( true );
	} else
#endif
	{
		xSET_TEXT( this, "text.name", XE::Format( _T( "%s" ), spAcc->GetstrName() ) );
	}
	auto pWnd = Find("img.name.bg");
	if( pWnd ) {
		xSET_TEXT( pWnd, "text.level", XE::Format( _T( "%d" ), spAcc->GetLevel() ) );
	}
	xSET_TEXT( this, "text.trophy", XE::Format( _T( "%d" ), spAcc->GetLadder() ) /*XE::Format(_T("%d"), pAcc->GetLevel())*/ );		// 랭크 점수
//	int powerTotal = XLegion::sGetMilitaryPower( m_spAcc->GetCurrLegion(), ACCOUNT );
	int powerTotal = m_spAcc->GetPowerExcludeEmpty();
	xSET_TEXT( this, "text.power", XFORMAT( "%s", XE::NumberToMoneyString( powerTotal ) )  );		// 랭크 점수
#ifdef _CHEAT
// 	if( XAPP->m_bDebugMode )
// 		xSET_TEXT( this, "text.special.remain", XE::Format( _T( "스페셜스팟 출현 남은기간:%d일" ), m_spAcc->GetRemainDaySpecialSpot() ) );
#endif

	// 로그버튼에 느낌표
	{
		auto pWnd = Find( "butt.log.battle" );    // 로그버튼
		if( pWnd ) {
			if( XWndBattleLog::s_bAlert )
				// 느낌표가 안붙어 있으면 달아준다.
				GAME->AddAlert( pWnd, 1, XE::VEC2(24,5) );
			else
				// 느낌표 삭제
				GAME->DelAlert( pWnd, 1 );
			// 최초 성지역(8-1)이 열리면 그때부터 보여줌.
			if( m_spAcc->GetpWorld()->IsOpenedArea(_T("area.1012")) )
				pWnd->SetbShow( true );
			else
				pWnd->SetbShow( false );
		}
	}
	auto pBar = XLayout::sGetCtrl<XWndProgressBar*>( this, "pbar.exp" );
	if( pBar == nullptr ) {
		pBar = new XWndProgressBar( 80.f, 14.f, 64.f, 10.f );
		pBar->SetstrIdentifier("pbar.exp");
		GetpFixedUIRoot()->Add( pBar );
	}
	pBar->SetLerp( (float)m_spAcc->GetExp() / m_spAcc->GetMaxExpCurrLevel() );
	bool bFlag = !m_spAcc->IsLockAcademy();
	//
	UpdateQuestUI();
	// 버프 아이콘 리스트를 만든다.
	// 계정 버프가 있을때만 만든다.
	if( m_spAcc->GetNumBuff() > 0 )	{
		// 버프아이콘의 루트윈도우를 만든다.
		XWnd *pWnd = Find("wnd.root.buff");
		if( pWnd == nullptr ) {
			auto pWndBuffs = new XWndBuffList( 101.f, 4.f );
			pWndBuffs->SetstrIdentifier( "wnd.root.buff" );
			GetpFixedUIRoot()->Add( pWndBuffs );
		}
	} else {
		// 계정버프가 없을땐 버프윈도우도 삭제시킨다.
		XWnd *pWnd = Find( "wnd.root.buff" );
		if( pWnd )
			pWnd->SetbDestroy( TRUE );

	}
	if( m_spAcc->IsLockEmbassy() )
		xSET_SHOW( this, "butt.world.social", false );
	else
		xSET_SHOW( this, "butt.world.social", true );
} // void XSceneWorld::UpdateUI( void )
/**
 @brief 병영 버튼 업데이트
*/
void XSceneWorld::UpdateUnitOrg()
{
	auto pButt = Find( "butt.unitorg" );
	if( pButt == nullptr )
		return;
	if( m_spAcc->IsLockBarrack() ) {
		pButt->SetbShow( FALSE );
		return;
	}
	pButt->SetbShow( TRUE );
	bool bTraining = false;
	bool bAlert = false;		// 느낌표마크
	// zZZ마크
	auto pSleep = pButt->Find( "spr.sleep" );
	// 빈 훈련소가 있다면.
	if( m_spAcc->GetNumRemainFreeSlot() > 0 ) {
		if( pSleep == nullptr ) {
			pSleep = new XWndSprObj( _T( "ui_sleep.spr" ), 1, 76.f, 20.f );
			pSleep->SetstrIdentifier( "spr.sleep" );
			pButt->Add( pSleep );
		}
	} else {
		bTraining = true;
	}
	if( m_spAcc->IsLockEmbassy() )
		if( pSleep )
			pSleep->SetbShow( FALSE );
	// 남는 부대슬롯이 있으면 느낌표 마크
	if( m_spAcc->IsRemainSquad() )
		bAlert = true;
//	GAME->DelBrilliant( pButt->getid() );
//	pButt->DestroyWndByIdentifier( "icon.alert" );
	// 톱니 마크
	if( bTraining )
		AddGear( pButt, XE::VEC2( 79, 16 ) );	// 훈련중 마크
	else
		DelGear( pButt );
	// 느낌표마크
	if( bAlert )
		GAME->AddAlert( pButt, 1, XE::VEC2( 79, 16 ) );
	else
		GAME->DelAlert( pButt, 1 );
	// 훈련이나 장비장착이 가능한 영웅이 있으면 녹색점.
	if( m_spAcc->IsHaveBetterThanPartsEnteredHero() 
		|| m_spAcc->IsAbleSummonHero() || m_spAcc->IsAblePromotionHero() 
		|| m_spAcc->IsNoCheckUnlockUnit() )
		GAME->SetGreenAlert( pButt, true, POS_GREEN_ALERT );
	else
		GAME->SetGreenAlert( pButt, false );
}
/**
 @brief 버튼에 느낌표를 붙임.
*/
void XSceneWorld::AddGear( XWnd *pButt, const XE::VEC2& _vPos )
{
	if( pButt->Find( "icon.gear" ) == nullptr ) {
		auto vPos = XE::VEC2(79,7);
		if( _vPos.IsValid() )
			vPos = _vPos;
		auto pMark = new XWndSprObj( _T( "ui_loading.spr" ), 1, vPos );
		pMark->SetstrIdentifier( "icon.gear" );
		pButt->Add( pMark );
		auto pWndSound = new XWndPlaySound( 26, true );
		pMark->Add( pWndSound );
// 		SOUNDMNG->OpenPlaySoundOneSec( 26 );
	}
}
void XSceneWorld::DelGear( XWnd *pButt )
{
	pButt->DestroyWndByIdentifier( "icon.gear" );
}
/**
 @brief 
*/
void XSceneWorld::UpdateTech()
{
	auto pButt = xGET_BUTT_CTRL( this, "butt.tech" );
	if( pButt == nullptr )
		return;
//	if( m_spAcc->GetbitUnlockMenu().IsBit( XGAME::xBM_LABORATORY ) == false ) {
	if( m_spAcc->IsLockLaboratory() ) {
		pButt->SetbShow( FALSE );
		return;
	}
	pButt->SetbShow( TRUE );
	GAME->DelBrilliant( pButt->getid() );
///	pButt->DestroyWndByIdentifier( "icon.alert" );
	// 기술연구중인가.
	auto pImgBg = Find( "bg.blank.research" );
	if( m_spAcc->IsResearching() ) {
		GAME->AddBrilliant( pButt->getid() );
		//DestroyWndByIdentifier( "icon.alert" );		// 느낌표알림은 제거(???)
		GAME->AddAlert( pButt, 2, XE::VEC2(79,16) );	// 기술연구 마크
		// 남은 시간 표시
		if( pImgBg == nullptr ) {
			pImgBg = new XWndImage( PATH_UI( "common_blank_ts.png" ), 12, -15 );
			pImgBg->SetstrIdentifier( "bg.blank.research" );
			pButt->Add( pImgBg );
			auto pText = new XWndTextString( XE::VEC2( 0, 0 ), nullptr, FONT_NANUM, 18.f );
			pText->SetstrIdentifier( "text.research.remain" );
			pText->SetSizeLocal( pImgBg->GetSizeLocalNoTrans() );
			pText->SetLineLength( pImgBg->GetSizeValidNoTrans().w );
			pText->SetAlignCenter();
			pImgBg->Add( pText );
		}
		if( !IsAutoUpdate() )
			SetAutoUpdate( 0.1f );
		pImgBg->SetbShow( TRUE );
	} else {
		GAME->DelAlert( pButt, 2 );
		// 기술연구중이 아닐때
		ClearAutoUpdate();
//		xSET_SHOW( this, "bg.blank.research", FALSE );
		if( pImgBg )
			pImgBg->SetbShow( false );
		// 특성포인트가 남았는가.
// 		if( m_spAcc->IsHaveTechPoint() ) {
// 			GAME->AddBrilliant( pButt->getid() );
// 			AddAlert( pButt, 1 );
// 		}
	}
	// 연구를 할수 있는상태인가(자원)
	if( m_spAcc->IsAbleResearch() )
		GAME->SetGreenAlert( pButt, true, POS_GREEN_ALERT );
	else
		GAME->SetGreenAlert( pButt, false );

// 	GAME->SetGreenAlert( pButt, true, XE::VEC2( 68, 2 ) );
// 	else
// 		GAME->SetGreenAlert( pButt, false );
	// 유닛을 잠금해제 할 수 있는 상태인가.
	if( m_spAcc->IsUnlockableAnyUnit() ) {
		if( pButt->Find( "icon.unlock" ) == nullptr ) {
			// 느낌표가 안붙어 있으면 달아준다.
			GAME->AddAlert( pButt, 1 );
// 			// 여기는 느낌표로 바꾸자.
// 			auto pMark = new XWndSprObj( _T( "ui_unlock.spr" ), 1, 79, 23 );
// 			pMark->SetstrIdentifier( "icon.unlock" );
// 			pButt->Add( pMark );
		}
	} else
		GAME->DelAlert( pButt, 1 );
//		pButt->DestroyWndByIdentifier( "icon.unlock" );
}
void XSceneWorld::UpdateSummonHero()
{
	auto pButt = xGET_BUTT_CTRL( this, "butt.hero" );
	if( pButt == nullptr )
		return;
// 	if( m_spAcc->GetbitUnlockMenu().IsBit( XGAME::xBM_TAVERN ) == false ) {
	if( m_spAcc->IsLockTavern() ) {
		pButt->SetbShow( FALSE );
		return;
	}
	pButt->SetbShow( TRUE );
}
void XSceneWorld::UpdateStorage()
{
	auto pButt = xGET_BUTT_CTRL( this, "butt.storage" );
	if( pButt == nullptr )
		return;
	// 창고기능은 일단 영웅고용버튼이 활성화 되면 같이 활성화되게 한다.
// 	if( m_spAcc->GetbitUnlockMenu().IsBit( XGAME::xBM_TAVERN ) == false ) {
	if( m_spAcc->IsLockTavern() ) {
		pButt->SetbShow( FALSE );
		return;
	}
	pButt->SetbShow( TRUE );
}
void XSceneWorld::UpdateMarket()
{
	auto pButt = xGET_BUTT_CTRL( this, "butt.market" );
	if( pButt == nullptr )
		return;
	if( XAccount::s_bTraderArrived ) {
		OnTraderArrive();
	}
	if( m_spAcc->IsLockMarket() ) {
		pButt->SetbShow( FALSE );
		return;
	}
	pButt->SetbShow( TRUE );
}

/**
 @brief 책퀘중 하나라도 완료한게 있으면 true
*/
BOOL XSceneWorld::IsCompleteQuest( void )
{
	int numItem = 0;
	int maxItem = 10;
	numItem = m_spAcc->GetNumItems( _T( "scalp_crow" ) );
	if( numItem >= maxItem )
		return TRUE;
	numItem = m_spAcc->GetNumItems( _T( "scalp_ironlord" ) );
	if( numItem >= maxItem )
		return TRUE;
	numItem = m_spAcc->GetNumItems( _T( "scalp_freedom" ) );
	if( numItem >= maxItem )
		return TRUE;
	numItem = m_spAcc->GetNumItems( _T( "scalp_ancient" ) );
	if( numItem >= maxItem )
		return TRUE;
	numItem = m_spAcc->GetNumItems( _T( "scalp_flame" ) );
	if( numItem >= maxItem )
		return TRUE;
	return FALSE;
}

void XSceneWorld::OnRecvPacket( ID idPacket )
{
	switch( idPacket )
	{
	case xCL2GS_LOBBY_CHANGE_SCALP_TO_BOOK:
	{
		SetbUpdate( TRUE );
		XWnd *pWnd = Find( "butt.book.quest" );
		if( pWnd )
			GAME->DelBrilliant( pWnd->getid() );
	} break;
	}
}

void XSceneWorld::UpdateQuestUI( void )
{
#ifdef _CHEAT
	if( XAPP->m_bDebugQuestList )
	{
		XWnd *pWnd = Find( "wnd.quest" );
		if( pWnd == nullptr ) {
			auto pWndQuest = new XWndCheatQuestList( 32.f, 161.f );
			pWndQuest->SetstrIdentifier( "wnd.quest" );
			GetpFixedUIRoot()->Add( pWndQuest );
		} else {
			auto pWndQuest = SafeCast<XWndCheatQuestList*>( pWnd );
			if( pWndQuest ) {
				pWndQuest->SetbUpdate( TRUE );
			}
		}
	} else
		DestroyWndByIdentifier( "wnd.quest" );
#endif // CHEAT
	auto pWndQuest = GetCtrl<XWndQuestList2*>(WND_QUEST_LIST);
	if( pWndQuest == nullptr ) {
		pWndQuest = new XWndQuestList2( m_Layout.GetpLayout(), 55.f, 140.f );
		pWndQuest->SetstrIdentifier( WND_QUEST_LIST );
		GetpFixedUIRoot()->Add( pWndQuest );
	}
	pWndQuest->SetbUpdate( TRUE );
}

XE::VEC2 XSceneWorld::GetvwCamera()
{
	if( XASSERT(m_pScrollView) )
		return m_pScrollView->GetFocusView();
	return XE::VEC2(0);
}

void XSceneWorld::SetvwCamera( const XE::VEC2& vwCamera )
{
	DoMovePosInWorld( vwCamera );
}

int XSceneWorld::Process( float dt ) 
{ 
	if( m_pScrollView ) {
		if( m_FocusMng.IsScrolling() ) {
			auto vCurr = m_FocusMng.GetCurrFocus();
			m_pScrollView->SetFocusView( vCurr );
		}
//		GAME->SetvFocusWorld( m_pScrollView->GetFocusView() );
		if( ACCOUNT )
			m_spAcc->SetvFocusWorld( m_pScrollView->GetFocusView() );
		// 홈버튼
		auto pWndHome = Find( "butt.home" );
		if( (m_pScrollView->GetFocusView() - c_vHome).Lengthsq() > 512.f * 512.f ) {
			if( pWndHome == nullptr ) {
				auto pButt = new XWndButton( 540.f, 16.f, _T("icon_home.png"), nullptr );
				pButt->SetstrIdentifier("butt.home");
				pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickHome );
				GetpFixedUIRoot()->Add( pButt );
				pWndHome = pButt;
			}
			pWndHome->SetbShow( TRUE );
		} else {
			if( pWndHome )
				pWndHome->SetbShow( FALSE );
		}
		if (m_timerExitGame.IsOver()) {
			GAME->OnExitApp(nullptr, 0, 0);
		}
	}
	if( ACCOUNT && sGetpWorld() ) {
		if( sGetpWorld()->GetbUpdate() ) {
			sGetpWorld()->SetbUpdate( FALSE );
			SetbUpdate( TRUE );
		}
	}
	// 새 이미터
	DoEmitBird();
	///< 
#ifdef _xIN_TOOL
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->Process( dt );
#endif // _xIN_TOOL
#ifdef _CHEAT
#ifdef WIN32
	auto pButt = dynamic_cast<XWndButtonDebug*>( Find("butt.debug.ctrl") );
	if( pButt ) {
		pButt->SetbCheck( XE::GetMain()->m_bCtrl );
	}
	pButt = dynamic_cast<XWndButtonDebug*>( Find("butt.debug.alt") );
	if( pButt ) {
		pButt->SetbCheck( XE::GetMain()->m_bAlt );
	}
#endif // WIN32
#endif // _CHEAT
	return XEBaseScene::Process( dt );
}

void XSceneWorld::DoEmitBird()
{
// 	if( !XAPP->IsGameMode() )
#ifdef _xIN_TOOL
	if( XBaseTool::sIsToolMode() )
		return;
#endif // _xIN_TOOL
	if( m_timerBird.IsOff() )
		m_timerBird.Set( 3.0f );
	if( m_timerBird.IsOver() ) {
		m_timerBird.Reset();
		auto vPos = m_pScrollView->GetvCenter();
		const auto vSizeScr = XE::GetGameSize();
		vPos.y = vPos.y + ( ( vSizeScr.y * 0.5f ) * 1.5f );
// 		int l = int( vPos.x - ( vSizeScr.w * 2.f ) );
// 		int r = int( vPos.x + ( vSizeScr.w * 2.f ) );
		int l = int( vPos.x - ( vSizeScr.w * 0.5f ) );
		int r = int( vPos.x + ( vSizeScr.w * 0.5f ) );
		vPos.x = (float)xRandom( l, r );
		float dAng = xRandomF( 240.f, 300.f );
		float speed = xRandomF( 0.5f, 0.7f );
		int num = 3 + xRandom(10);
		for( int i = 0; i < num; ++i ) {
			auto v = vPos;
			v.x += xRandomF( -40.f, 40.f );
			v.y += xRandomF( -40.f, 40.f );
			float a = dAng + xRandomF(-5.f, 5.f );
			float s = speed + xRandomF(0.1f);
			auto pWndBird = new XWndObjBird( v, a, s );
			GetpRootWorldObj()->Add( pWndBird );
		}
		SOUNDMNG->OpenPlaySoundBySec( 50, xRandomF(10.f, 15.f) );		// bird
	}
}
//
void XSceneWorld::Draw( void ) 
{
// 	if( m_pScrollView )
// 		m_pScrollView->SortPriority();
	XEBaseScene::Draw();
	
#ifdef _CHEAT
	XE::VEC2 vWorldMouse = m_pScrollView->GetvAdjScroll();
	vWorldMouse.Abs();
	XE::VEC2 vMouse = INPUTMNG->GetMousePos();
	vWorldMouse += vMouse;
	#ifdef _xIN_TOOL
	m_vMouseWorld = vWorldMouse;
//	if( XAPP->m_ToolMode == xTM_CLOUD )
//	{
	if( XBaseTool::sIsToolCloudMode() ) {
		DrawTool();
		XE::VEC2 vWorld = -m_pScrollView->GetvAdjScroll();
		int idx = PROP_CLOUD->GetIdxHexaFromWorld( vWorldMouse.x, vWorldMouse.y );
		PUT_STRINGF( vMouse.x, vMouse.y - 15.f, XCOLOR_WHITE, "%d", idx );
		XE::VEC2 v = PROP_CLOUD->GetCenterFromIdxHexa( idx );
		v -= vWorld;	// 화면좌표로 변환.
		GRAPHICS->DrawCircle( v.x, v.y, XGAME::GetRadiusHexa(), XCOLOR_RED );
	}
	#endif
	if( XAPP->m_bDebugMode ) {
		PUT_STRINGF_STYLE( 158, 0, XCOLOR_WHITE, xFONT::xSTYLE_SHADOW, "w(%d,%d)", (int)vWorldMouse.x, (int)vWorldMouse.y );
	}
#endif // cheat

//	XParticleMng::sGet()->Draw();
	XEBaseScene::DrawTransition();
}

void XSceneWorld::OnLButtonDown( float lx, float ly ) 
{
//	CONSOLE("XSceneWorld::OnLButtonDown");
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneWorld::OnLButtonUp( float lx, float ly ) {
//	CONSOLE( "XSceneWorld::OnLButtonUp" );
	XEBaseScene::OnLButtonUp( lx, ly );
}

/**
 @brief 원형 메뉴 생성
*/
XWndPopupSpotMenu* XSceneWorld::CreateSpotPopup2(ID idSpot )
{
	auto pExist = Find( "menu.circle" );
	if( pExist )
		pExist->SetbDestroy( TRUE );
	XWnd *pWndSpot = Find( idSpot );
	XBREAK( pWndSpot == nullptr );
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	XBREAK( pBaseSpot == nullptr );
	// 일단은 이렇게 하고 나중에 월드에서 보유한 스팟포인트가 shared_ptr로 바뀌면 거기에 맞춰서 바꿈.
	auto pPopup = new XWndPopupSpotMenu( pBaseSpot );
	if( pPopup ) {
		pPopup->SetbModal( TRUE );
		auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
		XBREAK( pBaseSpot == nullptr );
		// 스팟에 드랍아이템이 장착되어 있을경우 표시한다.
		std::vector<XGAME::xDropItem> aryDrops;
		pBaseSpot->GetDropItems( &aryDrops );
		pPopup->SetItems( aryDrops );
// 		Add( pPopup );	// Add후 AddMenu를 하다보니 Update에서 원형버튼들을 인식못하는 문제가 생겨 Add()는 AddMenu후에 하도록 바꿈
	}
	return pPopup;
}

/**
 @brief 스팟을 터치하면 호출된다.
*/
int XSceneWorld::OnClickSpot( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	const ID idSpot = p1;
	CONSOLE( "click spot:%d", idSpot );
	//
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XBREAK( pBaseSpot == nullptr ) )
		return 1;
	//
	int bSend = 0;
	switch( pBaseSpot->GettypeSpot() ) {
	case XGAME::xSPOT_CASTLE:
		bSend = OnClickCastleSpot( pBaseSpot );
		//		bCollect = 1;
		break;
	case XGAME::xSPOT_JEWEL:
		bSend = OnClickJewelSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_SULFUR:
		bSend = OnClickSulfurSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_MANDRAKE:
		bSend = 1;
		break;
	case XGAME::xSPOT_NPC:
		bSend = 1;
		break;
	case XGAME::xSPOT_DAILY:
		bSend = 1;
		break;
	case XGAME::xSPOT_CAMPAIGN:
		bSend = OnClickCampaignSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_VISIT:
		bSend = 1;
		OnClickVisitSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_CASH:
		bSend = 1;
		OnClickCashSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_PRIVATE_RAID:
		bSend = 1;
		break;
	case XGAME::xSPOT_COMMON:
		bSend = OnClickCommonSpot( pBaseSpot );
		break;
	default:
		XBREAKF( 1, "unknown spot type: %d", pBaseSpot->GettypeSpot() );
		break;
	}
	// 스팟 터치.(방문스팟을 의미하는게 아님)
	if( bSend ) {
		// 연속해서 누르지 못하게.
		if( m_timerClickSpot.IsOff() || m_timerClickSpot.IsOver() ) {
			GAMESVR_SOCKET->SendSpotTouch( pBaseSpot->GetidSpot() );
			m_timerClickSpot.Set( 1.f );
		} else {
			//			CONSOLE("스팟 연속 누름 방지");
		}
	}
	return 1;
}

int XSceneWorld::OnClickCastleSpot( XSpot *pBaseSpot )
{
	auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	int bCollect = 0;
	if( pSpot->IsAbleGetLocalStorage() ) {
		// 지역창고에 자원이 쌓여있으면 터치시 자원수거행동이 우선된다.
		GAMESVR_SOCKET->SendReqSpotCollect( this, idSpot );
		bCollect = 1;
	} else {
		auto pMenu = CreateSpotPopup2( idSpot );
		if( pMenu ) {
			if( pSpot->IsQuestion() )
				pMenu->AddMenuRecon();
			else {
				if (pSpot->GetidOwner() == m_spAcc->GetidAccount()) {
					// 내소유면 리젠버튼만 추가.
					pMenu->AddMenuRegen();
				} else {
					// 내소유가 아니면 정찰/공격/교체 버튼 추가.
					pMenu->AddMenuRecon();
					if (pSpot->IsQuestion() == false) {
						pMenu->AddMenuAttack();			// ?상태가 아닌 스팟은 공격버튼 추가.
						pMenu->AddMenuCollect();			// 수거버튼 추가.
						AddMenuKill( pMenu, idSpot, pSpot->GetPower(), pSpot->IsNpc() );
					}
					pMenu->AddMenuChangePlayerByGold();
				}
			}
			Add( pMenu );
		}
	}
	return bCollect;
}

void XSceneWorld::AddMenuKill( XWndPopupSpotMenu* pPopup, ID idSpot, int enemyPower, bool bNPC )
{
	auto pButt = pPopup->AddMenuKill();		// 소탕버튼
	if( pButt ) {
		bool bAbleKill = true;
		auto errCode = ACCOUNT->IsAbleKill( idSpot );
		// 소탕불가능한 상태면 버튼 disable
		if( errCode != xE_OK )
			bAbleKill = false;
		// 
		if( bAbleKill == false && errCode != xE_CAN_NOT ) {
			// 버튼에 자물쇠를 담.
			auto pImg = new XWndImage( PATH_UI("worldmap_lock.png"), XE::VEC2(0,-10) );
			pImg->SetScaleLocal( 0.75f );
			pButt->Add( pImg );
			pImg->AutoLayoutHCenter();
			pButt->SetbLock( true );
		}
		if( pButt ) {
			XBREAK( idSpot > 0xffff || errCode > 0xffff );
			DWORD param = ( idSpot << 16 ) | ( errCode & 0xffff );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickKill, param );
		}
	}
}

void XSceneWorld::OnRecvBattleResult( XGAME::xBattleResult& result )
{
#ifndef _XSINGLE
	auto pSpot = m_spAcc->GetpWorld()->GetpSpot( result.idSpot );
	if( XASSERT(pSpot) ) {
		auto pPopup = XGAME::DoPopupBattleResult( result, this, pSpot );
		if( XASSERT(pPopup) ) {
//			pPopup->SetEvent( XWM_OK, this, &XSceneBattle::OnOkBattleResult, idxLegion );
//			xSetButtHander( pPopup, this, "butt.statistic", &XSceneBattle::OnClickStatistic );
			xSET_SHOW( pPopup, "butt.statistic", false );
		}
	}
#endif // not _XSINGLE
}

/****************************************************************
* @brief 소탕
*****************************************************************/
int XSceneWorld::OnClickKill( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "%s", __TFUNC__ );
	//
	ID idSpot = (p1 >> 16);
	auto errCode = (XGAME::xtError)(p1 & 0xffff);
	bool bLock = p2 != 0;
	if( bLock ) {
		switch( errCode ) {
		case xE_OK:
			break;
		case xE_NOT_ENOUGH_LEVEL: {
//			const _tstring strTitle = XFORMAT( "%s", XTEXT(2089) );	// 레벨이 부족합니다.
			const _tstring strText = XE::Format( XTEXT(80093), XAccount::LV_KILL );	// 필요레벨 x
			auto pAlert = new XGameWndAlert();
//			pAlert->SetstrTitle( strTitle );
			pAlert->SetstrText( strText );
			Add( pAlert );
		} break;
		case xE_NOT_ENOUGH_NUM_CLEAR:
			XWND_ALERT( "%s", XTEXT(2337) );		// 3별로 클리어해야함.
			break;
		case xE_NOT_ENOUGH_ITEM:
			XGAME::DoAlertWithItem( _T("item_kill"), XTEXT(2002), XWnd::xOK );
			break;
		case xE_NOT_ENOUGH_AUTHORITY:
			XWND_ALERT( "%s", XTEXT( 2338 ) );		// 녹색이하여야함
			break;
		case xE_CAN_NOT:
			break;
		default:
			XBREAK(1);
			break;
		}
	} else {
		XGAME::xBattleFinish battle;
		battle.bCheatKill = true;
		battle.ebCode = XGAME::xEB_FINISH;
		battle.idSpot = idSpot;
		battle.bitWinner = XGAME::xSIDE_PLAYER;
		battle.secPlay = 1;
		auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
		if( XASSERT( pBaseSpot ) ) {
			if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
				auto pSpot = static_cast<XSpotCastle*>( pBaseSpot );
			}
		}
		GAME->DestroyWndByIdentifier( "menu.circle" );
		int apNeed = pBaseSpot->GetNeedAP( ACCOUNT );
		if( m_spAcc->GetAP() < apNeed ) {
			// AP부족
			auto pPopup = new XWndPaymentByCash();
			const int fullLack = m_spAcc->GetmaxAP() - m_spAcc->GetAP();
			pPopup->SetAP( fullLack );		// 다채우는 비용으로 바뀜
			Add( pPopup );
			pPopup->SetEvent( XWM_OK, GAME, &XGame::OnClickFillAPByCash );
			return 1;
		}
		GAMESVR_SOCKET->SendReqFinishBattle( this, battle );
	}

	return 1;
}

int XSceneWorld::OnClickJewelSpot( XSpot *pBaseSpot )
{
	auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	int bCollect = 0;
	if( pSpot->IsAbleGetLocalStorage() ) {
		// 지역창고에 자원이 쌓여있으면 터치시 자원수거행동이 우선된다.
		GAMESVR_SOCKET->SendReqSpotCollect( this, idSpot );
		bCollect = 1;
	} else {
		auto pMenu = CreateSpotPopup2( idSpot );
		if( pMenu ) {
			if( pSpot->IsQuestion() )
				pMenu->AddMenuRecon();
			else {
				if( pSpot->GetidOwner() != m_spAcc->GetidAccount() ) {
					// 내소유가 아닐땐 공격/정찰/교체 버튼 추가.
					pMenu->AddMenuRecon();
					if( pSpot->IsQuestion() == false ) {
						if( !pSpot->IsNoAttack() )
							pMenu->AddMenuAttack();			// ?상태가 아닌 스팟은 공격버튼 추가.
						pMenu->AddMenuCollect();
					}
				}
				pMenu->AddMenuChangePlayerByCash();
			}
			Add( pMenu );
		}
	}
	return bCollect;
}

int XSceneWorld::OnClickSulfurSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickSulfurSpot" );
	auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	int bCollect = 0;

	if( pSpot->IsAbleGetLocalStorage() ) {
		// 지역창고에 자원이 쌓여있으면 터치시 자원수거행동이 우선된다.
		GAMESVR_SOCKET->SendReqSpotCollect( this, idSpot );
		bCollect = 1;
	} else {
		auto pMenu = CreateSpotPopup2( idSpot );
		if( pMenu ) {
			if( pSpot->IsActive() ) {
				pMenu->AddMenuRecon();
				pMenu->AddMenuAttack();			// 활성화된 스팟은 공격버튼 추가.
				pMenu->AddMenuChangePlayerByCash();
			} else
				pMenu->AddMenuRegen();
			pMenu->AddMenuLogSulfur();	// 약탈기록 버튼 추가.
			Add( pMenu );
		}
	}
	return bCollect;
}


int XSceneWorld::OnClickMandrakeSpot( XSpot *pBaseSpot )
{
	const ID idAcc = m_spAcc->GetidAccount();
	auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	int bCollect = 0;
	if( pSpot->IsAbleGetLocalStorage() ) {
		// 지역창고에 자원이 쌓여있으면 터치시 자원수거행동이 우선된다.o
		GAMESVR_SOCKET->SendReqSpotCollect( this, idSpot );	// 이제 서버갔다가 오므로 서버에서 먼저 처리할것.
		bCollect = 1;
	} else {
		auto pMenu = CreateSpotPopup2(idSpot );
		if( pMenu ) {
			const auto state = pSpot->GetState( idAcc );
// 			if( pSpot->GetidOwner() == 0 ) {
			if( state == xSpot::xSM_NO_MATCH ) {
				// 아직 상대가 매치안된상태면 정찰 버튼만.
				pMenu->AddMenuRecon();
			} else
// 			if( pSpot->GetidOwner() != m_spAcc->GetidAccount() ) {
			if( state == xSpot::xSM_CHALLENGE ) {
				// 내소유가 아니면 공격/교체
				pMenu->AddMenuRecon();
				pMenu->AddMenuAttack();
				pMenu->AddMenuChangePlayerByCash();
			} else 
			if( state == xSpot::xSM_DEFENSE ) {
				// 내 소유일땐 회수버튼 추가
				auto pButt = pMenu->AddMenuWithdraw();
// 				if( pSpot->GetWin() == 1 )
// 					pButt->SetbEnable( FALSE );
				pMenu->AddMenuCollect();
			} else {
				XBREAK(1);
			}
			Add( pMenu );
		}
	}
	return bCollect;
}

int XSceneWorld::OnClickNpcSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickNpcSpot" );
	auto pSpot = SafeCast<XSpotNpc*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	//
	{
		auto pMenu = CreateSpotPopup2( idSpot );
		if( pMenu ) {
			if( pSpot->IsActive() ) {
				// 활성화된 스팟은 정찰/공격
				pMenu->AddMenuRecon();
				if( pSpot->IsNoAttack() == false ) {
					pMenu->AddMenuAttack();	
					pMenu->AddMenuChangePlayerByCash();
					AddMenuKill( pMenu, idSpot, pSpot->GetPower(), pSpot->IsNpc() );
// #if defined(_CHEAT) 
// #	if defined(WIN32)
// #		ifdef _DEBUG
// 					pMenu->AddMenuKill();
// #		else
// 					if( XAPP->m_bDebugMode )
// 						pMenu->AddMenuKill();
// #		endif
// #	else
// 					if( XAPP->m_bDebugMode )
// 						pMenu->AddMenuKill();
// #	endif
// #endif // _CHEAT
				}
			} else {
				// 비활성화된 스팟은 리젠
				pMenu->AddMenuRegen();
			}
			Add( pMenu );
		}
	}
	return 1;
}

int XSceneWorld::OnClickDailySpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickDailySpot" );
	auto pSpot = SafeCast<XSpotDaily*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	auto pPopup = new XWndPopupDaily( pSpot );
	pPopup->SetbModal( TRUE );
	Add( pPopup );
	return 1;
}

int XSceneWorld::OnClickCampaignSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickCampaignSpot" );
	ID idSpot = pBaseSpot->GetidSpot();
	auto pSpot = SafeCast<XSpotCampaign*>( pBaseSpot );
	if( XASSERT(pSpot) ) {
		auto pPopup = new XWndPopupCampaign( pBaseSpot, pSpot->GetspCampObj() );
		Add( pPopup );
	}
	return 1;
}

int XSceneWorld::OnClickVisitSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickVisitSpot" );
	ID idSpot = pBaseSpot->GetidSpot();
	auto pSpot = SafeCast<XSpotVisit*>( pBaseSpot );
	if( XASSERT( pSpot ) ) {
		if( !pSpot->GetpProp()->idsSeq.empty() ) {
			std::string str = SZ2C(pSpot->GetpBaseProp()->strIdentifier);
			GAME->DispatchEvent( XGAME::xAE_VISIT_SPOT, str );
		} else {
			XWndVisit *pPopup = new XWndVisit( pSpot );
			pPopup->SetbModal( TRUE );
			Add( pPopup );
		}
	}
	return 1;
}

int XSceneWorld::OnClickCashSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickCashSpot" );
	auto pSpot = SafeCast<XSpotCash*>( pBaseSpot );
	if( XBREAK( pSpot == nullptr ) )
		return 0;
	ID idSpot = pSpot->GetidSpot();
	//
	{
		XSpotCash *pSpot = static_cast<XSpotCash*>( pBaseSpot );
		if( XASSERT( pSpot ) )
		{
		}

	}
	return 1;
}

int XSceneWorld::OnClickCommonSpot( XSpot *pBaseSpot )
{
	CONSOLE( "OnClickCommonSpot" );
	if( pBaseSpot->IsGuildRaid() ) {
		if( m_spAcc->GetGuildIndex() == 0 ) {
			XWND_ALERT( "%s", XTEXT( 2123 ) );		// 길드가 필요함.
			return 0;
		}
	}
	return 1;
}

/**
 @brief 스팟에 자원이 생산(리젠)될때마다 호출된다.
 자원이 생산된 해당스팟위에 생산된자원UI를 뿌려줘야 한다.
*/
void XSceneWorld::OnProduceResAtSpot( XSpot* pBaseSpot
																		, const XTimer2& timerCalc
																		, const std::vector<xRES_NUM>& aryRes )
{
	const ID idSpot = pBaseSpot->GetidSpot();
	auto pWndSpot = GetpWndSpot( idSpot );
	if( pWndSpot ) {
//		const auto vwPos = pWndSpot->GetPosLocal() + XE::VEC2(0,-50);
		auto pScrollView = GetpScrollView();
		//
		auto pCtrl = new XWndProduceMsg( idSpot, aryRes, XCOLOR_GREEN );
		if( pScrollView ) {
			pScrollView->Add( pCtrl );
		}
	}
}

/**
 서버로부터 자원수거 허락이 떨어졌다. 자원 터치
*/
void XSceneWorld::OnRecvSpotCollect( XSpot *pBaseSpot, const std::vector<xRES_NUM>& aryRes )
{
	auto pWnd = SCENE_WORLD->Find( pBaseSpot->GetidSpot() );
	if( !pWnd )
		return;
	DoMakeResourceParticle( pWnd->GetPosLocal(), aryRes );
}

void XSceneWorld::DoMakeResourceParticle( const XE::VEC2& _vPos
																				, XGAME::xtResource typeRes
																				, float _num )
{
	std::vector<xRES_NUM> aryRes;
	aryRes.push_back( xRES_NUM(typeRes, _num) );
	DoMakeResourceParticle( _vPos, aryRes );
}
/**
 @brief 자원 파티클을 만들어서 뿌린다.
 @param _vPos 스팟의 GetPosLocal()값.
*/
void XSceneWorld::DoMakeResourceParticle( const XE::VEC2& _vPos
																				, const std::vector<xRES_NUM>& aryRes )
{
	{
		auto pWndMsg = new XWndProduceMsg( _vPos, aryRes, XCOLOR_GREEN );
		m_pScrollView->Add( pWndMsg );
		const auto sizeCtrl = pWndMsg->GetSizeLocal();
		const auto vwLT = _vPos + XE::VEC2( -(sizeCtrl.w * 0.5f), -50.f );
		pWndMsg->SetPosLocal( vwLT );
	}
	// 자원 파티클 생성
	for( const auto& res : aryRes ) {
		if( res.num > 0 ) {
			int numObj = 0;
			if( res.type == xRES_CASH ) {
				numObj = (int)res.num;
			} else {
				numObj = (int)(res.num / 1000 + 1);
			}
			if( numObj > 20 )
				numObj = 20;
			const _tstring strFile = C2SZ( XE::Format( "world_top_%s.png", XGAME::GetIdsRes( res.type ) ) );
			XE::VEC2 vPos;
			int start = 0;
			if( !strFile.empty() ) {
				auto pWndCtrl = XGAME::GetpWndTopRes( res.type, this );
				if( XASSERT( pWndCtrl ) ) {
					start = m_spAcc->GetResource( res.type );
					vPos = pWndCtrl->GetPosLocal();
				}
				for( int i = 0; i < numObj; ++i ) {
					auto pParticle = new XWndResParticle( strFile.c_str()
																							, _vPos
																							, vPos
																							, m_pScrollView
																							, res.type
																							, start );
					pParticle->SetbActive( false );	// 터치안되게
					Add( pParticle );
				}
			}
		}
	}
	SOUNDMNG->OpenPlaySoundBySec( 24, 1.f );		// 자원 수거 사운드.
}

//스팟에 저장된 자원을 클라에서 지우고 스팟을 업데이트시켜줌
void XSceneWorld::RecvClearStorageWithWndSpot( XSpot *pBaseSpot )
{
	pBaseSpot->ClearLocalStorage();
	XWnd *pWnd = Find( pBaseSpot->GetidSpot() );
	if( XBREAK( pWnd == nullptr ) )
		return;
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE )
	{
		auto pWndSpot = SafeCast<XWndCastleSpot*>( pWnd );
//		pWndSpot->Update();
		pWndSpot->SetbUpdate( TRUE );
	} else
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_JEWEL )
	{
		XWndJewelSpot *pWndSpot = SafeCast<XWndJewelSpot*, XWnd*>( pWnd );
		pWndSpot->Update();
	} else
	if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_SULFUR )
	{
		pWnd->SetbDestroy( TRUE );
	}
	UpdateSpots();
}

void XSceneWorld::CloseMenuReconAttack( void )
{
	XWnd *pDlg = Find("wnd.recon.attack");
	if( pDlg )
		pDlg->SetbDestroy(TRUE);
}

int XSceneWorld::OnClickWorld( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickWorld");
#ifdef _xIN_TOOL
	auto pWndLayer = SafeCast2<XWndBgObjLayer*>( Find("root.world.obj.under" ) );
	if( pWndLayer ) {
		pWndLayer->OnClickWorld();
	}
#endif // _xIN_TOOL
	return 1;
}

int XSceneWorld::OnScrollWorldmap( XWnd* pWnd, DWORD p1, DWORD p2 )
{
//	CONSOLE("OnScrollWorldmap");
	{
		auto pLayer = GetpLayerBgObjUnder();
		if( pLayer ) {
			pLayer->SendMsgToChilds( "update_sound" );
		}
	}
#ifdef _xIN_TOOL
//	m_idTouchDownCloud = 0;		// 스크롤되면 터치다운시 선택된 구름은 취소된다.
#endif
	return 1;
}

int XSceneWorld::OnReconSpot( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idSpot = p1;
	CloseMenuReconAttack();
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) )
	{
		// 결과는 OnRecvReconSpot로 온다
		DestroyWndByIdentifier("menu.circle");
// 		OnClosePopup(pWnd, 0, 0);
		GAMESVR_SOCKET->SendReqReconSpot( this, idSpot );
	}
	return 1;
}

/**
 SendReqReconSpot의 결과
 pEnemy는 지금 안쓰긴 하는데 나중에 적의 특성트리까지 정찰할수 있게 되면 사용되므로 놔둠.
*/
void XSceneWorld::OnRecvReconSpot( ID idSpot, const XSPLegion& spLegion )
{
	SetbUpdate( true );
	//
	// 부대가 없으면 null이 올수도 있음.
	auto pSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pSpot) ) {
		// 나중에 spLegion을 그대로 넘겨주도록 바꿀것.
		auto pWnd = new XWndSpotRecon( /*pEnemy, */spLegion.get(), pSpot );
		pWnd->SetCancelButton( "butt.close" );
		xSetButtHander( pWnd, this, "butt.attack", &XSceneWorld::OnAttackSpot, idSpot );
		XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> aryRes;
		pSpot->GetLootInfo( &aryRes );
		XARRAYLINEARN_LOOP_AUTO( aryRes, res ) {
			pWnd->AddLoot( res );
		} END_LOOP;
		Add( pWnd );
	}
}

int XSceneWorld::OnAttackSpot( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idSpot = p1;	
	if (m_spAcc->GetCurrLegion()->GetNumSquadrons() <= 0) {
		auto pAlarm = XWND_ALERT( "%s", XTEXT(80175));
		return 1;
	}
	if( m_spAcc->IsInvenOver() ) {
		XWND_ALERT_T( XTEXT(80171) );		// "창고를 정리해야 전투에 참여할 수 있습니다"
		return 1;
	}
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT( pBaseSpot ) ) {
		if( pBaseSpot->IsCampaignType() ) {
			XBREAK( XCampObj::s_idxStage < 0 );	// 캠페인타입일경우 값이 지정되어있어야 한다.
		}
		int apNeed = pBaseSpot->GetNeedAP( ACCOUNT );
		if( m_spAcc->GetAP() < apNeed ) {
			// AP부족
			auto pPopup = new XWndPaymentByCash();
			const int fullLack = m_spAcc->GetmaxAP() - m_spAcc->GetAP();
			pPopup->SetAP( fullLack );		// 다채우는 비용으로 바뀜
			Add( pPopup );
			pPopup->SetEvent( XWM_OK, GAME, &XGame::OnClickFillAPByCash );
			return 1;
		}
		// 적부대정보 최종 업데이트 요청
		//GAMESVR_SOCKET->SendReqUpdateEnemyLegion( GAME, idSpot );
		// 레디씬 요청
		//GAMESVR_SOCKET->SendReqReadyScene( GAME );
		// 서버로 스팟 공격 신호 보냄
		GAMESVR_SOCKET->SendReqSpotAttack( GAME, idSpot, XCampObj::s_idxStage, XCampObj::s_idxFloor );
	}
	DestroyWndByIdentifier("menu.circle");
	DestroyWndByIdentifier("wnd.recon.result");
// 	if( pWnd && pWnd->GetpParent() )
// 		pWnd->GetpParent()->SetbDestroy( TRUE );
	return 1;
}

int XSceneWorld::OnCollectSpot(XWnd* pWnd, DWORD p1, DWORD p2)
{
	SpotcollectLock(p1);
	DestroyWndByIdentifier( "menu.circle" );
	GAMESVR_SOCKET->SendReqSpotCollect(this, p1);

	return 1;
}

/**
 전투전 상대유저의 부대정보가 왔다.
*/
void XSceneWorld::OnRecvBattleInfo( XSPSceneParam spParam )
{
	// 배틀씬 파라메터가 모두 세팅되어 있어야 함.
	XBREAK( spParam->IsInvalid() );
	DoExit( XGAME::xSC_READY, spParam );
}

/**
 성이 다른 유저에게 점령당했다.
*/
void XSceneWorld::OnRecvAttackedCastle( ID idSpot, ID idAccount, int level, LPCTSTR szName )
{
	const _tstring str = XE::Format(_T("성 점령당함:idAcc=0x%x(%s)"), idAccount, szName);
	CONSOLE("%s", str.c_str());
	// idSpot성을 찾아서 
	SetbUpdate( TRUE );

}



//수거한 자원 오브젝트가 UI에 도착함
void XSceneWorld::ArriveResObj(XGAME::xtResource typeRes, int start)
{
	// 구현의 난해함때문에 도착한 후 카운팅애니메이션이 되는건 취소한다.
}

/**
 @brief 리소스양 UI를 갱신한다.
*/
void XSceneWorld::UpdateResourceUI( const char *cKey, int amount )
{
	XWnd *pWnd = Find(cKey);
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextNumberCounter*>( pWnd );
		if( pText )
			pText->SetCurrNum( amount );
	}
}


/**
 구름레이어 보이기 감추기
*/
void XSceneWorld::OnViewCloudLayer( void )
{
#ifdef _CHEAT
	m_bViewCloudLayer = !m_bViewCloudLayer;
	XWnd *pLayer = Find("scroll.cloud.view");
	if( pLayer == nullptr )
		return;
	pLayer->SetbShow( m_bViewCloudLayer );
#endif // cheat
}

void XSceneWorld::OnRButtonDown( float lx, float ly )
{
	XEBaseScene::OnRButtonDown( lx, ly );
}

void XSceneWorld::OnRButtonUp( float lx, float ly )
{
	XE::VEC2 vWorldMouse = -m_pScrollView->GetvAdjScroll();
	XE::VEC2 vMouse(lx, ly);
	vWorldMouse += vMouse;

	#ifdef _xIN_TOOL
//	if( XAPP->m_ToolMode == xTM_CLOUD ) {
	if( XBaseTool::sIsToolCloudMode() ) {
		BOOL bEdited = FALSE;
		m_bAction = FALSE;
		XWndCloudLayer *pCloudLayer = SafeCast<XWndCloudLayer*, XWnd*>( Find( "scroll.cloud.view" ) );
		if( pCloudLayer ) {
			XE::VEC2 vWorld = -m_pScrollView->GetvAdjScroll();
			int idx = PROP_CLOUD->GetIdxHexaFromWorld( vWorldMouse.x, vWorldMouse.y );
			XPropCloud::xCloud *pProp = PROP_CLOUD->GetpPropByidxHexa( idx );
			ID idSelected = 0;
			// 우클릭한곳에 구름이 없었으면 
			if( pProp == nullptr ) {
				// 이미 선택한 그룹이 있으면 거기에 추가.
				if( m_idSelected ) {
					auto pPropSelected = PROP_CLOUD->GetpProp( m_idSelected );
					if( pPropSelected ) {
						// 이미 선택된 그룹이 있었다면 그 그룹에 인덱스를 추가함.
						pPropSelected->AddIdx( idx );
						bEdited = TRUE;
					}
				}
			} else {
			// 우클릭한곳에 구름이 있었으면.
				// 이미 선택한 그룹의 구름을 한번터 클릭하면 지움
				if( m_idSelected == pProp->idCloud ) {
					pProp->DelIdx( idx );
					if( pProp->GetSizeIdxs() == 0 ) {
						PROP_CLOUD->DestroyCloud( m_idSelected );
						m_idSelected = 0;
					}
					bEdited = TRUE;
				} else
					// 선택한 구름과 다른 구름이면 선택.
					m_idSelected = pProp->idCloud;
			}
			if( bEdited )
				EditedCloud();
			m_idxClickedHexa = idx;
			// 구름 프로퍼티 다시 갱신
			UpdateCloudSpotList( true );
		}
	}
	#endif // xIN_TOOL
	//
	XEBaseScene::OnRButtonUp( lx, ly );
}

#ifdef _xIN_TOOL
// xTool start
/**
 pProp의 구름밑에 깔리는 스팟들을 갱신한다.
*/
bool XSceneWorld::UpdateSpotListByCloud( XPropCloud::xCloud *pProp )
{
	if( pProp == nullptr )
		return false;
	XArrayLinearN<ID, 256> aryOld;
	pProp->GetSpotsToAry( aryOld );		// 이전 리스트를 받아둠
	pProp->ClearSpots();
	XArrayLinearN<XPropWorld::xBASESPOT*, 1024> ary;
	PROP_WORLD->GetPropToAry( XGAME::xSPOT_NONE, ary );
	XARRAYLINEARN_LOOP( ary, XPropWorld::xBASESPOT*, pSpotProp ) {
		if( IsCoveredSpotByCloud( pSpotProp, pProp ) )
			pProp->AddSpot( pSpotProp->idSpot );
	} END_LOOP;
	///< 
	bool bChanged = false;
	XArrayLinearN<ID, 256> aryCurr;
	pProp->GetSpotsToAry( aryCurr );	// 바뀐 리스트를 받음.
	if( aryOld.size() == aryCurr.size() ) {
		// 리스트가 바꼈는지 검사
		XARRAYLINEARN_LOOP_IDX( aryOld, ID, i, idSpot ) {
			if( idSpot != aryCurr[ i ] ) {
				bChanged = true;
				break;
			}
		} END_LOOP;
	} else
		bChanged = true;	// 리스트 사이즈가 다르면 바뀐거임
	return bChanged;
}

/**
 @brief 구름밑에 깔리는 스팟리스트를 다시 갱신한다.
*/
bool XSceneWorld::UpdateCloudSpotList( bool bAllCloud )
{
	bool bChanged = false;
#ifdef _xIN_TOOL
	XArrayLinearN<ID, 512> ary;
#ifdef WIN32
	if( bAllCloud )
		PROP_CLOUD->GetPropToAry( &ary );
	else
#endif
		sGetpWorld()->GetClosedCloudsToAry( &ary );
	if( bAllCloud ) {
		XARRAYLINEARN_LOOP( ary, ID, idCloud ) {
			auto pProp = PROP_CLOUD->GetpProp( idCloud );
			if( pProp == NULL )
				continue;
			// pProp구름 밑에 깔리는 스팟리스트를 다시 갱신한다.
			bool bc = UpdateSpotListByCloud( pProp );
			if( !bChanged )
				bChanged = bc;
		} END_LOOP;
	}
	SetbUpdate( true );
#endif
	return bChanged;
}

/**
 @brief 툴모드에서 스팟UI들 모두 업데이트
*/
void XSceneWorld::UpdateSpotForTool( void )
{
	if( !XBaseTool::sIsToolMode() )
		return;
// 	if( m_pTool == nullptr )
// 		return;
	XWnd *pView = Find( "scroll.view" );
	if( pView == nullptr )
		return;
	XWnd *pRoot = pView->Find( "wnd.root.tool.scrollview" );
	XBREAK( pRoot == nullptr );
// 	XWnd *pRoot = pView->Find( "wnd.root.tool.scrollview" );
// 	// 툴모드에서 스팟들이 올라갈 루트
// 	if( pRoot == nullptr ) {
// 		pRoot = new XWnd;
// 		pRoot->SetstrIdentifier( "wnd.root.tool.scrollview" );
// 		pView->Add( pRoot );
// 	}
	XArrayLinearN<XPropWorld::xBASESPOT*, 2048> ary;
	XArrayLinearN<XWndSpotForTool*, 2048> arySpots;
	PROP_WORLD->GetSpotsPropToAry( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, pBaseProp ) {
		ID idWnd = ID_SPOT_TOOL + pBaseProp->idSpot;
		XWnd *pExist = Find( idWnd );
		if( pExist == nullptr ) {
			XWndSpotForTool *pWndSpot = new XWndSpotForTool( pBaseProp );
			pRoot->Add( idWnd, pWndSpot );
			arySpots.Add( pWndSpot );
		} else
			arySpots.Add( static_cast<XWndSpotForTool*>( pExist ) );

	} END_LOOP;
	// 필터에 따라 가려준다.
	XARRAYLINEARN_LOOP_AUTO( arySpots, pWndSpot ) {
		if( pWndSpot->GetpBaseProp()->idWhen == 0 ) {
			switch( pWndSpot->GetpBaseProp()->type ) {
			case XGAME::xSPOT_CASTLE:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_CASTLE ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			case XGAME::xSPOT_JEWEL:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_JEWEL ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			case XGAME::xSPOT_SULFUR:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_SULFUR ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			case XGAME::xSPOT_MANDRAKE:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_MANDRAKE ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			case XGAME::xSPOT_NPC:
			case XGAME::xSPOT_DAILY:
//			case XGAME::xSPOT_SPECIAL:
			case XGAME::xSPOT_CAMPAIGN:
			case XGAME::xSPOT_VISIT:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_NPC ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			case XGAME::xSPOT_COMMON:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_COMMON ||
					XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			default:
				if( XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
					pWndSpot->SetbShow( TRUE );
				else
					pWndSpot->SetbShow( FALSE );
				break;
			}
			// idWhen == 0
		} else {
			// idWhen != 0
			// 이벤트스팟만 보이기 옵션일때 idWhen값이 있는 스팟만 보인다.
			if( XAPP->m_modeToolSpotView == XGAME::xSPOT_WHEN ||
				XAPP->m_modeToolSpotView == XGAME::xSPOT_NONE )
				pWndSpot->SetbShow( TRUE );
			else
				pWndSpot->SetbShow( FALSE );
		}
	} END_LOOP;
}

void XSceneWorld::EditedCloud( void ) 
{
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->UpdateAutoSave();
}

/**
 @brief 툴모드가 전환된다.
*/
void XSceneWorld::DelegateChangeToolMode( xtToolMode modeOld, xtToolMode modeCurr )
{
	XBREAK( modeOld == modeCurr );
	auto pWndScrl = Find( "scroll.view" );
	XBREAK( pWndScrl == nullptr );
	switch( modeOld )
	{
	case xTM_NONE: {
		GetpFixedUIRoot()->SetbShow( false );				// 게임용 UI를 가림
		xSET_SHOW( this, "root.spot", false );	// 게임용 스팟레이어를 가림
	} break;
	case xTM_CLOUD:
		break;
	case xTM_TECH:
		break;
	case xTM_SPOT: {
		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
		if( pCloudLayer )
			pCloudLayer->SetbTouchable( true );
	} break;
	case xTM_OBJ: {
		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
		if( pCloudLayer )
			pCloudLayer->SetbShow( true );		// 복구
		XAPP->m_nToolShowCloud = 2;
		auto pLayerSpotForTool = GetpLayerSpotForTool();
		if( pLayerSpotForTool )
			pLayerSpotForTool->SetbActive( true );
	} break;
	default:
		break;
	}
	//
	XWnd *pWndToolUI = Find( "ui.tool" );
	if( modeCurr && pWndToolUI == nullptr ) {
		// 툴모드일때는 툴 UI레이어를 생성.
		pWndToolUI = new XWnd;
		pWndToolUI->SetstrIdentifier( "ui.tool" );
		Add( pWndToolUI );
	}
	switch( modeCurr )
	{
	case xTM_NONE: {
		if( pWndToolUI )
			pWndToolUI->SetbDestroy( true );
		GetpFixedUIRoot()->SetbShow( true );
		xSET_SHOW( this, "root.spot", true );
	} break;
	case xTM_CLOUD:
		break;
	case xTM_TECH:
		break;
	case xTM_SPOT: {
		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
		if( pCloudLayer )
			pCloudLayer->SetbTouchable( false );		// 구름이 터치가 되지 않게 한다.
	} break;
	case xTM_OBJ: {
		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
		if( pCloudLayer ) {
			pCloudLayer->SetbShow( false );		// bgObj모드에선 구름을 표시하지 않는다.
			XAPP->m_nToolShowCloud = 0;
		}
		auto pLayerSpotForTool = GetpLayerSpotForTool();
		if( pLayerSpotForTool )
			pLayerSpotForTool->SetbActive( false );
		UpdateBgObjs();
	} break;
	default:
		break;
	}
	SetbUpdate( true );
//	UpdateUIForTool();
}

void XSceneWorld::DrawTool( void )
{
	// 선택된 구름의 테두리에 사각형을 그린다.
	if( m_idSelected ) {
		// 연결된 스팟들을 그린다.
		XPropCloud::xCloud *pProp = PROP_CLOUD->GetpProp( m_idSelected );
		if( pProp ) {
			XArrayLinearN<ID, 512> ary;
			pProp->GetSpotsToAry( ary );
			BOOL bFound = FALSE;
			XARRAYLINEARN_LOOP( ary, ID, idSpot ) {
//					XSpot *pSpot = sGetpWorld()->GetSpot( idSpot );
				XPropWorld::xBASESPOT *pSpotProp = PROP_WORLD->GetpProp( idSpot );
				if( IsCoveredSpotByCloud(pSpotProp, pProp) ) {
					bFound = TRUE;
					XE::VEC2 vLocal = GetPosWorldToLocal( pSpotProp->vWorld );
					GRAPHICS->DrawCircle( vLocal.x, vLocal.y, 64.f, XCOLOR_BLUE );
				}
			} END_LOOP;
		}
	}
}

int XSceneWorld::OnTouchDown( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnTouchDown");
	//
	return 1;
}

void XSceneWorld::OnMouseMove( float lx, float ly )
{
#ifdef _xIN_TOOL
	XE::VEC2 vDist = XE::VEC2(lx, ly) - m_vMouse;
	//
	if( XBaseTool::sGetCurrMode() == xTM_SPOT )
		XWndSpotForTool::s_pMouseOver = dynamic_cast<XWndSpotForTool*>( XWnd::s_pMouseOver );
	//

	m_vMouse.x = lx;
	m_vMouse.y = ly;
#endif
	XEBaseScene::OnMouseMove( lx, ly );
}

/**
 pSpot이 구름(pProp)아래에 가려지는지 확인한다.
*/
BOOL XSceneWorld::IsCoveredSpotByCloud( XPropWorld::xBASESPOT *pSpotProp, 
										XPropCloud::xCloud *pProp )
{
	for( auto& hexa : pProp->idxs ) {
		XE::VEC2 vWorld = -m_pScrollView->GetvAdjScroll();
		XE::VEC2 vCenter = PROP_CLOUD->GetCenterFromIdxHexa( hexa.idx );
		XE::VEC2 vDist = pSpotProp->vWorld - vCenter;	// 헥사의 중심점과 스팟의 좌표의 거리
		float dist = vDist.Lengthsq();
		float radiusHexa = XGAME::GetRadiusHexa();
		if( dist < radiusHexa * radiusHexa )
			return TRUE;
	}
	return FALSE;
}

/**
 @brief 현재 선택된 스팟을 삭제한다.
*/
void XSceneWorld::DelSpotT()
{
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->DelSelected();
}

void XSceneWorld::CopySpotT()
{
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->CopySelected();
}

void XSceneWorld::Undo()
{
	if( XBaseTool::s_pCurr )
		XBaseTool::s_pCurr->Undo();
}
// xTool end
#endif // XCLOUD_TOOL

/**
 구름 가격 레이블을 클릭함.
*/
int XSceneWorld::OnClickCloudLabel( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idCloud = (ID)p1;
	CONSOLE("OnClickCloudLabel:%d", idCloud );;
	auto pProp = PROP_CLOUD->GetpProp( idCloud );
	if( XBREAK( pProp == nullptr ) )
		return 1;
	XGAME::xtError err = m_spAcc->IsOpenableCloud( pProp );
	switch( err )
	{
	case XGAME::xE_NOT_ENOUGH_LEVEL: {
		XWND_ALERT_T( XTEXT( 80093 ), pProp->lvArea );	// x레벨에 열립니다.
	}	break;
	case XGAME::xE_NOT_ENOUGH_GOLD: {
//		XWND_ALERT( "%s", XTEXT( 2143 ) );	// 금화부족
		// 금화가 부족함. 캐시로 지불할지 물어보고 지불방법을 바꿔서 올림.
		auto pWnd = new XWndPaymentByCash();
		const int goldLack = (int)((DWORD)pProp->cost - ACCOUNT->GetGold());
		pWnd->SetGold( goldLack );
//		pWnd->SetEvent( XWM_OK, this, &XSceneWorld::OnClickChangePlayerByCashLack, idSpot );
		pWnd->SetEvent2( XWM_OK, [this, idCloud]( XWnd* pWnd ) {
			GAMESVR_SOCKET->SendReqOpenCloud( this, idCloud, xTP_GOLD_AND_CASH );
		} );
		Add( pWnd );	}	break;
	case XGAME::xE_MUST_PRECEDE_AREA_OPEN:
		// xx지역을 먼저 열어야 합니다.
		if( XASSERT(!pProp->idsPrecedeArea.empty()) ) {
			auto pPropPrecede = PROP_CLOUD->GetpProp( pProp->idsPrecedeArea );
			if( pPropPrecede ) {
				if( pPropPrecede->idName ) {
					XWND_ALERT_T( XTEXT( 2142 ), XTEXT( pPropPrecede->idName ) );
				} else {
					XWND_ALERT_T( _T("%s"), XTEXT(2239) );	// 선행지역을 먼저열어야 합니다.
				}
				DoMoveToArea( pPropPrecede->idCloud );
			}
		}
		break;
	case XGAME::xE_MUST_PRECEDE_QUEST:
		// xxx퀘스트를 먼저 클리어 해야합니다.
		if( XASSERT(!pProp->idsQuest.empty()) ) {
			auto pPropQuest = XQuestProp::sGet()->GetpProp( pProp->idsQuest );
			if( XASSERT(pPropQuest) ) {
				if( m_spAcc->GetpQuestMng()->IsHaveCurrQuest( pPropQuest->idProp ) ) {
					XWND_ALERT_T( XTEXT( 2220 ), XTEXT( pPropQuest->idName ) );		// xx퀘를 완료해야함
				} else {
					XWND_ALERT_T( XTEXT( 2221 ), XTEXT( pPropQuest->idName ) );		// xx퀘를 완료해야하지만 아직 못받음.
				}
			}
		}
		break;
	case XGAME::xE_MUST_NEED_KEY_ITEM: {
		XGAME::DoAlertWithItem( pProp->idsItem, XTEXT(2002), XWnd::xOK );		// 필요아이템 부족
	}	break;
	case XGAME::xE_OK:
		GAMESVR_SOCKET->SendReqOpenCloud( this, idCloud, xTP_GOLD );
		break;
	default:
		// 알수없는 에러
		XBREAK(1);
		break;
	}
	return 1;
}


/**
 서버로부터 구름 오픈이 성공했다.
*/
void XSceneWorld::OnRecvOpenCloud( ID idCloud, DWORD dwGold, DWORD addCash )
{
	// 지역 가격레이블 객체 삭제.
	auto pCloudLayer = SafeCast<XWndCloudLayer*>( Find( "scroll.cloud.view" ) );
	if( pCloudLayer == nullptr )
		return;
	SOUNDMNG->OpenPlaySound(8);			// 구름 제거 소리
	pCloudLayer->DoDisappear( idCloud );
	DestroyAreaAttension();
	// idCloud가 선행오픈지역으로 지정되어있는 지역을 얻어서 효과를 붙인댜ㅏ.
	auto pPropArea = PROP_CLOUD->GetpProp( idCloud );
	if( XASSERT( pPropArea ) ) {
		auto vCenter = PROP_CLOUD->GetPosByArea( pPropArea );
		if( vCenter.IsValid() )
			DoMakeResourceParticle( vCenter, XGAME::xRES_CASH, (float)addCash );
		_tstring idsArea = pPropArea->strIdentifier;
		std::string cidsArea = SZ2C( idsArea );
		auto pPropArea2 = PROP_CLOUD->GetAreaByPrecedeArea( pPropArea->strIdentifier );
		if( pPropArea2 ) {
			CONSOLE("%s지역이 열릴수 있게 됨.", pPropArea2->strIdentifier.c_str() );
		}
	}
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneWorld::OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnClickHero" );
	DoExit( XGAME::xSC_HERO );
	return 1;
}

int XSceneWorld::OnClickUnitOrg(XWnd* pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_UNIT_ORG);
	return 1;
}

/****************************************************************
* @brief 기술연구
*****************************************************************/
int XSceneWorld::OnClickLaboratory( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLaboratory");

	DoExit( XGAME::xSC_TECH );
	return 1;
}

void XSceneWorld::UpdateDebugButtons()
{
#ifdef _CHEAT
	const bool bDebugMode = (XAPP->m_bDebugMode != FALSE);
//	if( XAPP->m_bDebugMode ) {
		const XE::VEC2 vOrig( 2, 265 );
		XE::VEC2 v = vOrig;
		const XE::VEC2 vSize(25,25);
		auto 
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.ctrl", v, vSize, _T( "ctrl" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XE::GetMain()->m_bCtrl );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 21 );
		}
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.alt", v, vSize, _T( "shift" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XE::GetMain()->m_bAlt );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 22 );
		}
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.bgm", v, vSize, _T( "bgm" ), bDebugMode );
		if( pButt ) {
			const bool bBgm = GAME->GetpOption()->GetbMusic();
			pButt->SetbCheck( bBgm );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 23 );
		}
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.snd", v, vSize, _T( "snd" ), bDebugMode );
		if( pButt ) {
//			bool bSnd = (SOUNDMNG->GetbMuteSound() == false);
			const bool bSnd = GAME->GetpOption()->GetbSound();
			pButt->SetbCheck( bSnd );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 24 );
		}
		v.x = vOrig.x;
		v.y += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.exp", v, vSize, _T("exp"), bDebugMode );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 1 );
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.gold", v, vSize, _T( "gold" ), bDebugMode );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 3 );
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.cash", v, vSize, _T( "cash" ), bDebugMode );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 4 );
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.res", v, vSize, _T( "자원" ), bDebugMode );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 7 );
		v.x += 25.f;
// 		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.ladder", v, vSize, _T( "점수" ), bDebugMode );
// 		pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 12 );
// 		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.ap", v, vSize, _T( "AP" ), bDebugMode );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 13 );
		v.x += 25.f;
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.mode", v, vSize, _T( "debug mode" ), bDebugMode );
		if( pButt ) {
//			pButt->SetbCheck( m_spAcc->m_bDebugMode );
			if( m_spAcc->m_bDebugMode )
				pButt->SetText( _T("debug") );
			else
				pButt->SetText( _T("normal") );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 14 );
			v.x += 25.f;
		}
#ifdef _VER_ANDROID
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.crash", v, vSize, _T( "crash" ), bDebugMode );
		if( pButt ) {
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 15 );
		}
		v.x += 25.f;
#endif // _VER_ANDROID
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.mem", v, vSize, _T( "mem" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XAPP->m_bViewMemoryInfo );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 16 );
			v.x += 25.f;
		}
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.fps", v, vSize, _T( "fps" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XAPP->GetbViewFrameRate() );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 17 );
			v.x += 25.f;
		}
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.load.profile", v, XE::VEC2(40,25), _T( "profile load" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XAPP->m_bDebugProfilingLoad );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 18 );
			v.x += 25.f;
		}
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.bgobj", v, vSize, _T( "bgObj" ), bDebugMode );
		if( pButt ) {
			pButt->SetbCheck( XAPP->m_bViewBgObj );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 19 );
			v.x += 25.f;
		}
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.push", v, vSize, _T( "push" ), bDebugMode );
		if( pButt ) {
//			pButt->SetbCheck( XAPP->m_bViewBgObj );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 20 );
			v.x += 25.f;
		}


		v.Set( XE::GetGameWidth() - vSize.w, 240.f );
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.debug.battle", v, vSize, _T( "battle" ), bDebugMode );
		if( pButt ) {
			pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnCheat, 99 );
			v.x -= 25.f;
		}
//	}
#endif // CHEAT

}

/****************************************************************
* @brief 
*****************************************************************/
#ifdef _CHEAT
int XSceneWorld::OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnCheat");
	//
	const float multiply = ( XE::GetMain()->m_bCtrl )? 10.f : 1.f;
	const float plusMinus = (XE::GetMain()->m_bAlt)? -1.f : 1.f;
	int type = (int)p1;
	// exp증가
	if( type == 1 ) {
		int exp = (int)(m_spAcc->GetXFLevelObj().GetMaxExpCurrLevel() * multiply * plusMinus);
// 		if( XE::GetMain()->m_bCtrl )
// 			exp *= 10;
// 		exp = (int)(exp * plusMinus);
		GAMESVR_SOCKET->SendCheat( this, 1, (exp / 3)-1 );
	} else
	if( type == 2 )	{
		// 영웅 레벨 업으로 예약
	} else
	if( type == 3 )	{
		auto addGold = 500000.f * multiply * plusMinus;
		GAMESVR_SOCKET->SendCheat( this, type, (int)addGold );
	} else
	if( type == 4 ) {
		float add = 500.f * multiply * plusMinus;
		m_spAcc->AddCashtem( (int)add );
		GAMESVR_SOCKET->SendCheat( this, type, (int)add );
	} else
	if( type == 7 ) {
		auto add = 50000.f * multiply * plusMinus;
		for( int i = 0; i < XGAME::xRES_MAX; ++i )
			m_spAcc->AddResource( ( XGAME::xtResource )i, (int)add );
		GAMESVR_SOCKET->SendCheat( this, type, (int)add );
	} else
	if( type == 12 ) {
		int add = int(50 * multiply * plusMinus);
		m_spAcc->AddScore( add );
		GAMESVR_SOCKET->SendCheat( this, type, m_spAcc->GetLadder() );
	} else
	if( type == 13 ) {
		int add = int(100 * multiply * plusMinus);
		m_spAcc->AddAP( add );
		GAMESVR_SOCKET->SendCheat( this, type, m_spAcc->GetAP() );
	} else
	if( type == 14 ) {
		m_spAcc->m_bDebugMode = !m_spAcc->m_bDebugMode;
		GAMESVR_SOCKET->SendCheat( this, type, xboolToByte(m_spAcc->m_bDebugMode) );
	} else
	if( type == 15 ) {
		XLOG("Do Crash!");
		int *p = nullptr;
		*p = 1;
	} else
	if( type == 16 ) {
		XAPP->m_bViewMemoryInfo = !XAPP->m_bViewMemoryInfo;
		XAPP->XClientMain::SaveCheat();
	} else
	if( type == 17 ) {
		XAPP->SetbViewFrameRate( !XAPP->GetbViewFrameRate() );
		XAPP->XClientMain::SaveCheat();
	} else
	if( type == 18 ) {
		XAPP->m_bDebugProfilingLoad = !XAPP->m_bDebugProfilingLoad;
	} else
	if( type == 19 ) {
		XAPP->m_bViewBgObj = !XAPP->m_bViewBgObj;
	} else
	if( type == 20 ) {
		static int a = 0;
		static int b = 0;
		_tstring str = XFORMAT("Title%d/Desc%d", a++, b++ );
		GAMESVR_SOCKET->SendRegistPushMsg( NULL, m_spAcc->GetidAccount(), XGAME::xTECH_TRAINING, 1, str, 1 );
	} else
	if( type == 21 ) {
		XE::GetMain()->m_bCtrl = !XE::GetMain()->m_bCtrl;
	} else
	if( type == 22 ) {
		XE::GetMain()->m_bAlt = !XE::GetMain()->m_bAlt;
	} else
	if( type == 23 ) {
		GAME->ToggleBGM();
	} else
	if( type == 24 ) {
		GAME->ToggleSound();
	} else
	if( type == 25 ) {
		// 특성초기화. XSceneTech::OnCheat에서 사용
	} else
	if( type == 99 ) {
	}
	SetbUpdate( true );
	return 1;
}

/**
 @brief 스팟 리젠
*/
// int XSceneWorld::OnCheatRegen( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE( "OnCheatRegen" );
// 	//
// 	ID idSpot = (ID)p1;
// 	GAMESVR_SOCKET->SendCheat( this, 5, idSpot );
// 	pWnd->GetpParent()->SetbDestroy( TRUE );
// 	return 1;
// }

int XSceneWorld::OnCheatClearQuest( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnCheatClearQuest" );
	//
	ID idQuestProp = (ID)p1;
	GAMESVR_SOCKET->SendCheat( this, 6, idQuestProp );
	XWndQuestInfo::sDestroyWnd();
	return 1;
}
int XSceneWorld::OnCheatDelQuest( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnCheatDelQuest" );
	//
	ID idQuestProp = (ID)p1;
	m_spAcc->GetpQuestMng()->_DelQuest( idQuestProp );
	GAMESVR_SOCKET->SendCheat( this, 18, idQuestProp );
	XWndQuestInfo::sDestroyWnd();
	SetbUpdate( true );
	return 1;
}

#endif // _CHEAT

int XSceneWorld::OnClickStorage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	DoExit( XGAME::xSC_STORAGE );
	return 1;
}

int XSceneWorld::OnClickShop( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickShop");
//	m_Layout.CreateLayout("shop", this);
	m_LayoutGroup.pShop = new XLayout(_T("shop_in_world.xml"),nullptr, XLayout::sGetMain() );
	m_LayoutGroup.pShop->CreateLayout( "shop", this );

	XWnd *pPopup = Find("wnd.popup.shop");
	if (pPopup) {
		pPopup->SetbModal(TRUE);
	}
	if( XAccount::s_bTraderArrived ) {
		auto pButt = xGET_BUTT_CTRL( this, "butt.tab.trade" );
		if( pButt ) {
			GAME->AddAlert( pButt, 1, XE::VEC2(128,5) );
		}
	}

	xSET_BUTT_HANDLER(pPopup, "butt.close", &XSceneWorld::OnClosePopupBySideMenu);
	xSET_BUTT_HANDLER(pPopup, "butt.tab.trade", &XSceneWorld::OnTrader);
	xSET_BUTT_HANDLER(pPopup, "butt.tab.weapon", &XSceneWorld::OnArmory);
	xSET_BUTT_HANDLER(pPopup, "butt.tab.sundries", &XSceneWorld::OnPremium);

	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnTrader( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnTrader");

	DoExit(XGAME::xSC_TRADER);
	
	/*
	for( int i = 0; i < XGAME::xRES_MAX; ++i )
	{
		XGAME::xtResource type = (XGAME::xtResource)i;
		int amount = m_spAcc->GetResource( type );
		if( amount > 0 )
			GAMESVR_SOCKET->SendReqTrade( this, type, amount );
	}//*/

	return 1;
}


/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnArmory( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnArmory");
	//
	DoExit(XGAME::xSC_ARMORY);
	
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnPremium( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnPremium");
	//
	DoExit( XGAME::xSC_SHOP );
	
	return 1;
}

/****************************************************************
* @brief 징표를 책으로 교환해준다.
*****************************************************************/
// int XSceneWorld::OnBookQuest( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnBookQuest");
// 	//
// 	for( int i = 1; i < XGAME::xCL_MAX; ++i )
// 	{
// 		XGAME::xtClan clan = (XGAME::xtClan) i;
// 		if( m_spAcc->GetNumScalp( clan, 1 ) >= XGAME::NUM_SCALP_FOR_BOOK )
// 		{
// 			GAMESVR_SOCKET->SendReqChangeScalpToBook( this, clan );
// 			CONSOLE("징표 교환 시도:clan=%d", (int)clan );
// 		}
// 	}
// 		
// 	return 1;
// }

/****************************************************************
* @brief 왼쪽 사이드 메뉴쪽 팝업창들을 닫는 공통 모듈.
*****************************************************************/
int XSceneWorld::OnClosePopupBySideMenu(XWnd* pWnd, DWORD p1, DWORD p2)
{
	m_LayoutGroup.DestroyAll();
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	return 1;
}

int XSceneWorld::OnClickRank(XWnd* pWnd, DWORD p1, DWORD p2)
{
	GAMESVR_SOCKET->SendReqRankingList(this);
	return 1;
}

int XSceneWorld::OnClickSocial(XWnd* pWnd, DWORD p1, DWORD p2)
{
	// 팝업
	/*m_Layout.CreateLayout("socialpopup", this);

	XWnd *pPopup = Find("img.world.social");
	if (pPopup)
	{
		pPopup->SetbModal(TRUE);

		xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneWorld::OnClosePopup);
	}*/
	DoExit(XGAME::xSC_GUILD);
	return 1;
}

int XSceneWorld::OnClickMailbox(XWnd* pWnd, DWORD p1, DWORD p2)
{
	UpdateMailbox();

	return 1;
}

// void XSceneWorld::DestroyWndOption()
// {
// 	DestroyWndByIdentifier( "img.world.option" );
// }

int XSceneWorld::OnClickOption(XWnd* pWnd, DWORD p1, DWORD p2)
{
	auto pPopup = new XWndOption( m_spAcc );
	Add( pPopup );
	return 1;
}

/**
 @brief 
*/
int XSceneWorld::OnClickFacebook( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickFacebook");
	//
	XFacebook::sGet()->DoRequestCertification( GAME );
	//
	return 1;
}

int XSceneWorld::OnClickCheckInvite(XWnd* pWnd, DWORD p1, DWORD p2)
{
	//초대거부
	m_bOption[3] = !m_bOption[3];
	xSET_SHOW(this, "img.world.check3", m_bOption[3]);

	return 1;
}

int XSceneWorld::OnClickOptionClose(XWnd* pWnd, DWORD p1, DWORD p2)
{
	// 여기서 옵션 설정하자

	return 1;
}

int XSceneWorld::OnClickRankPrev(XWnd* pWnd, DWORD p1, DWORD p2)
{
	xSET_TEXT(this, "text.world.rank", XTEXT(80043));

	return 1;
}

int XSceneWorld::OnClickRankNext(XWnd* pWnd, DWORD p1, DWORD p2)
{
	xSET_TEXT(this, "text.world.rank", XTEXT(80043));

	return 1;
}

void XSceneWorld::UpdateMailbox()
{
	DestroyWndByIdentifier( "popup.mailbox" );
	//
	auto pPopup = new XWndMail();
	pPopup->SetstrIdentifier( "popup.mailbox" );
	Add( pPopup );
}
void XSceneWorld::RecvGetItem()
{
	UpdateMailbox();		// 아이템 갱신될걸로 메일함 팝업 다시 띄움
	SetbUpdate( true );
}

/**
 @brief 메일리스트에서 메일 항목을 누름.
 @param pWnd Wndlist
*/
int XSceneWorld::OnClickMailboxElem(XWnd* pWnd, DWORD p1, DWORD p2)
{
	ID idWndElem = p2;
	auto pElem = SafeCast<XWndMailElem*>( pWnd->Find( idWndElem ) );
	if( pElem ) {
		DestroyWndByIdentifier( "popup.mailbox" );		// 메일함 팝업을 닫음.
		//
		auto pPostInfo = pElem->GetpPostInfo();
		XBREAK( pPostInfo == nullptr );
		ID snPost = pPostInfo->GetsnPost();
		// 메일 내용 보기 팝업
		auto pPopup = new XWndMailEach( ACCOUNT, snPost );
		pPopup->SetstrIdentifier( "popup.mail.each" );
		xSetButtHander( pPopup, this, "butt.get", &XSceneWorld::OnClickMailGetItem, snPost );
		Add( pPopup );
		xSET_TEXT( pPopup, "text.mail.sender", pPostInfo->GetstrSenderName() );
		xSET_TEXT( pPopup, "text.mail.title", pPostInfo->GetstrTitle() );
		xSET_TEXT( pPopup, "text.mail.content", pPostInfo->GetstrMessage() );
		//
		XE::VEC2 vPos( 50, 271 );
		for( auto spItem : pPostInfo->GetlistPostItems() ) {
			auto pWndItem = new XWndStoragyItemElem( spItem->GetReward() );
			pWndItem->SetPosLocal( vPos );
			pPopup->Add( pWndItem );
			const auto sizeWnd = pWndItem->GetSizeLocal();
			XBREAK( sizeWnd.w <= 0 );
			vPos.x += sizeWnd.w + 1.f;
		}
		pPopup->SetOkButton( "butt.get" );
		pPopup->SetEvent( XWM_OK, this, &XSceneWorld::OnClickMailGetItem, snPost );
		pPopup->SetEvent( XWM_CANCEL, this, &XSceneWorld::OnClickMailbox );
	}
	
	return 1;
}

/**
 @brief 아이템을 수령하고 메일을 삭제
*/
int XSceneWorld::OnClickMailGetItem(XWnd* pWnd, DWORD p1, DWORD p2)
{
	ID snPost = p1;
	if( XASSERT(snPost) ) {
		GAMESVR_SOCKET->SendPostGetItemAll( GAME, snPost );
	}
	return 1;
}

void XSceneWorld::UpdateMailBoxAlert()
{
	// 우편물 체크
	if( m_spAcc->GetNumPostInfo() != 0 ) {
		XWnd *pView = Find( "wnd.left.butts" );
		if( pView ) {
			XWnd *pBG = Find( "img.world.left.bg" );
			if( pBG ) {
				if( pView->GetPosLocal().x >= 0 ) {
					xSET_SHOW( this, "img.world.left.mailboxalert1", FALSE );
					xSET_SHOW( this, "img.world.left.mailboxalert2", TRUE );
				} else {
					xSET_SHOW( this, "img.world.left.mailboxalert1", TRUE );
					xSET_SHOW( this, "img.world.left.mailboxalert2", FALSE );
				}
			}
		}
	} else {
		xSET_SHOW( this, "img.world.left.mailboxalert1", FALSE );
		xSET_SHOW( this, "img.world.left.mailboxalert2", FALSE );
	}
}

int XSceneWorld::OnClickLeftMenuHide(XWnd* pWnd, DWORD p1, DWORD p2)
{
	XWnd *pView = Find("wnd.left.butts");
	if (pView)
	{
		XWnd *pBG = Find("img.world.left.bg");
		if (pBG)
		{
			if (pView->GetPosLocal().x >= 0)
			{
				// 감추기
				pView->SetPosLocal(pView->GetPosLocal().x - pBG->GetSizeLocal().x, pView->GetPosLocal().y);
// 				xSET_SHOW(this, "img.world.left.right", TRUE);
// 				xSET_SHOW(this, "img.world.left.left", FALSE);
				xSET_SHOW( this, "butt.log.battle", TRUE );
			}
			else
			{
				// 보이기
				pView->SetPosLocal(pView->GetPosLocal().x + pBG->GetSizeLocal().x, pView->GetPosLocal().y);
// 				xSET_SHOW(this, "img.world.left.right", FALSE);
// 				xSET_SHOW(this, "img.world.left.left", TRUE);
				xSET_SHOW( this, "butt.log.battle", FALSE );
			}
			UpdateMailBoxAlert();
		}
	}

	return 1;
}

BOOL XSceneWorld::OnKeyUp( int keyCode )
{
	auto bCatch = XWnd::OnKeyUp( keyCode );
	if( !bCatch ) {
		if( keyCode == XE::KEY_BACK ) {
			// 백버튼 누르면 현재 씬에도 돌아가기 버튼이 있는지 보고 있으면 OnBack을 불러준다. 
			XTRACE( "OnKeyUp==back" );
			auto pAlert = XWND_ALERT_YESNO( "wnd.exit", "%s", _T( "exit?" ) );
			if( pAlert )
				pAlert->SetEvent( XWM_YES, GAME, &XGame::OnExitApp );
		}
	}
	return TRUE;
}

/**
 @brief 새 퀘스트가 발생했다.
*/
void XSceneWorld::OnOccurQuest( XQuestObj *pObj )
{
	CONSOLE( "OnOccurQuest" );
	UpdateQuestUI();
	if( pObj && pObj->GetstrIdentifer() == _T("scout_attack") )
		GAME->DispatchEvent( XGAME::xAE_QUEST_NEW, "scout_attack" );
	SOUNDMNG->OpenPlaySoundBySec(9, 1.f);  // 새퀘스트 사운드.
}

/**
 @brief 서버로부터 퀘 조건이 모두 완료되었다. 관련 UI처리함.
 @note 주의: 퀘 파괴시점이 아님. 단지 퀘 목표를 달성했을때임.
*/
void XSceneWorld::RecvCompleteQuest( XQuestObj *pObj )
{
	CONSOLE( "OnCompleteQuest" );
	UpdateQuestUI();
	GAME->DispatchEvent( XGAME::xAE_QUEST_COMPLETE, SZ2C( pObj->GetstrIdentifer() ) );
	SOUNDMNG->OpenPlaySound(10);		// 퀘 완료소리(V자?)
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnCompleteQuest( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnCompleteQuest");
	ID idProp = (ID)p1;
	//
	XQuestObj *pQuestObj = m_spAcc->GetpQuestMng()->GetQuestObj( idProp );
	if( XASSERT(pQuestObj) ) {
		if( pQuestObj->IsAllComplete() )
			GAMESVR_SOCKET->SendReqReqQuestReward( GAME, pQuestObj->GetidProp() );
	}
	
	return 1;
}


/**
 @brief 퀘완료후 보상이 떨어졌다 관련 UI처리.
*/
void XSceneWorld::OnRecvQuestReward( XQuestObj *pObj )
{
	SetbUpdate( TRUE );
	CONSOLE("OnRecvQuestReward");
	DestroyWndByIdentifier("popup.quest.info");
//	UpdateQuestUI();
	GAME->DispatchEvent( XGAME::xAE_QUEST_DESTROY, SZ2C( pObj->GetstrIdentifer() ) );

}

void XSceneWorld::OnUpdateQuest( XQuestObj *pObj )
{
	CONSOLE( "OnUpdateQuest" );
	UpdateQuestUI();
}

void XSceneWorld::OnSpotSync( ID idSpot )
{
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( pBaseSpot && pBaseSpot->IsDestroy() ) {
		ID idDel = DestroyID( idSpot );
		CONSOLE("del spot:id=%d", idDel );
	}
	SetbUpdate( TRUE );
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnClickFace( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickFace");
	//
	auto pPopup = new XWndPopup( XE::VEC2(256, 128), _T("popup01.png"))	;
	Add( pPopup );
	pPopup->SetbModal( TRUE );
	pPopup->AddButtonOk(_T("ok"), BUTT_SMALL );;
	int h, m, s;
	XSYSTEM::GetHourMinSec( m_spAcc->GetsecPlay(), &h, &m, &s );
	auto pText = new XWndTextString( 20, 20, XFORMAT("총 플레이시간: %0d:%02d:%02d", h, m, s ) );
	pPopup->Add( pText );
	return 1;
}

void XSceneWorld::OnRecvRankList(const XVector<xRankInfo*>& aryRank)
{
	auto pWndPopup = new XWndRank( aryRank );
	Add( pWndPopup );
	// 팝업
}


void XSceneWorld::RecvRegistID(BOOL bSuccess)
{
	if (bSuccess) {
		//팝업 끄기
		GAME->DestroyWndByIdentifier("popup.account.regist");
	} else {
		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_fail", nullptr);
		pPopup->SetbModal(TRUE);
		Add(pPopup);
		xSET_BUTT_HANDLER(this, "butt.close", &XSceneWorld::OnClosePopupBySideMenu);
	}
}

/****************************************************************
* @brief 상대 바꾸기
*****************************************************************/
int XSceneWorld::OnClickChangePlayerByGold( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickChangePlayerGold");
	//
	ID idSpot = (ID)p1;
	int costGold = XGC->GetGoldChangePlayer( m_spAcc->GetLevel() );;
	auto pSpot = sGetpWorld()->GetSpot( idSpot );
	if( pSpot == nullptr )
		return 1;
	if( pSpot->IsSuccessiveDefeat() )
		costGold = 0;
	if( m_spAcc->IsEnoughGold( costGold ) ) {
		GAMESVR_SOCKET->SendReqReMatchSpot( GAME, idSpot );
	} else {
		int goldNeed = costGold - m_spAcc->GetGold();
		int cash = m_spAcc->GetCashFromGold( goldNeed );
		// 캐쉬로 구매 창을 연다.
//     auto pWnd = new XWndPaymentByCash( XGAME::xRES_GOLD, goldNeed, cash, XTEXT(2119) );
		auto pWnd = new XWndPaymentByCash();
		pWnd->SetGold( goldNeed );
		pWnd->SetEvent( XWM_OK, this, &XSceneWorld::OnClickChangePlayerByCashLack, idSpot );
//     auto pButt = pWnd->GetBuyButton();
//     if( pButt )
//       pButt->SetEvent( XWM_CLICKED, this, &XSceneWorld::OnClickChangePlayerByCashLack, idSpot );
		Add( pWnd );
		pWnd->SetbModal( TRUE );
	}
	XWndPopupSpotMenu::sDestroyWnd();
	//pWnd->GetpParent()->SetbDestroy( TRUE );
	
	return 1;
}
/**
 @brief 부족한 금화는 캐쉬로 대신함.
*/
int XSceneWorld::OnClickChangePlayerByCashLack( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickChangePlayerCash");
	//
	ID idSpot = (ID)p1;
	int costGold = XGC->GetGoldChangePlayer( m_spAcc->GetLevel() );
	int goldNeed = costGold - m_spAcc->GetGold();
	int cash = m_spAcc->GetCashFromGold( goldNeed );
	if( m_spAcc->IsEnoughCash( cash ) ) {
		GAMESVR_SOCKET->SendReqReMatchSpot( GAME, idSpot );
	} else {
		XWND_ALERT("%s", XTEXT(80140));
	}
	XWndPopupSpotMenu::sDestroyWnd();
//	pWnd->GetpParent()->SetbDestroy( TRUE );
	
	return 1;
}

/****************************************************************
* @brief 상대 바꾸기(캐쉬)
*****************************************************************/
int XSceneWorld::OnClickChangePlayerByCash( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickChangePlayerCash");
	//
	ID idSpot = (ID)p1;
	if( m_spAcc->IsEnoughCash( XGC->m_cashChangePlayer ) ) {
		GAMESVR_SOCKET->SendReqReMatchSpot( GAME, idSpot );
	} else {
		XWND_ALERT("%s", XTEXT(80140));
	}
	XWndPopupSpotMenu::sDestroyWnd();
	
	return 1;
}


/****************************************************************
* @brief 새 상대 리젠
*****************************************************************/
int XSceneWorld::OnClickRegen( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickRegen");
	//
	ID idSpot = (ID)p1;
	if( m_spAcc->IsEnoughCash( XGAME::xCS_REGEN_SPOT ) )
		GAMESVR_SOCKET->SendReqRegenSpot( GAME, idSpot );
	XWndPopupSpotMenu::sDestroyWnd();
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnOkRegenSpot( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnOkRegenSpot");
	//
	ID idSpot = (ID)p1;
	return 1;
}

void XSceneWorld::OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld)
{
	_tstring strString = szString;
	_tstring strStar;
	for (unsigned int i = 0; i < strString.size(); i++)
		strStar += _T("*");

	switch (m_SelInputBox)
	{
	case 1:	xSET_TEXT(this, "text.email", strString);	break;	//이메일
	case 2:				//패스워드
	{
		m_strPassword = szString;
		xSET_TEXT(this, "text.password", strStar);
	}break;
	case 3:				//패스워드 재입력
	{
		m_strVerify = szString;
		xSET_TEXT(this, "text.password.verify", strStar);
	}break;
	}

	pWndEdit->SetbDestroy(TRUE);
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnClickBattleLog( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickBattleLog");
	//
	auto pPopup = new XWndBattleLog;
	Add( pPopup );
	pPopup->SetbModal( TRUE );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnMandrakeWithdraw( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnMandrakeWithdraw");
	//
	ID idSpot = p1;
	GAMESVR_SOCKET->SendReqWithdrawMandrake( GAME, idSpot );	
	return 1;
}


#ifdef _CHEAT
/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnClickXuzhuMode( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	//
	GAME->ToggleXuzhuMode();
	return 1;
}

#endif // _CHEAT

void XSceneWorld::OnAutoUpdate()
{
	if( m_spAcc->IsResearching() )
	{
		auto pImgBg = Find("bg.blank.research");
		if( pImgBg )
		{
			_tstring str;
			auto& r = m_spAcc->GetResearching();
			xSec secStart = r.GetsecStart();
			xSec secTotal = r.GetsecTotal();
			XSceneTech::sGetRemainResearchTime( &str, secStart, secTotal );
			auto pText = xSET_TEXT( pImgBg, "text.research.remain", str );
			if( pText )
				pText->AutoLayoutHCenter();
		}
	}
}

XWndSpot* XSceneWorld::GetpWndSpot( ID idSpot )
{
	return SafeCast<XWndSpot*>( Find( idSpot ) );
}
/**
 @brief sec초간 해당 스팟을 깜빡거리게 한다.
*/
void XSceneWorld::DoBlinkSpot( ID idSpot, float sec )
{
	auto pWndSpot = SafeCast<XWndSpot*>(Find( idSpot ));
	if( pWndSpot ) {
		pWndSpot->DoBlink( sec );
	}
}

/**
 @brief sec초간 code스팟들을 깜빡거리게 한다.
*/
void XSceneWorld::DoBlinkSpotCode( ID code, float sec )
{
	XArrayLinearN<XSpot*,256> ary;
	sGetpWorld()->GetSpotsToAry( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, pBaseSpot )
	{
		if( pBaseSpot->GetpBaseProp()->idCode )
		{
			auto pWndSpot = SafeCast<XWndSpot*>( Find( pBaseSpot->GetidSpot() ) );
			if( XASSERT( pWndSpot ) )
				pWndSpot->DoBlink( sec );
		}
	} END_LOOP;
}

/**
 @brief sec초간 type타입의 스팟들을 깜빡거리게 한다.
 @param idCloud 가 지정되어있으면 해당 지역의 type스팟들만 깜빡임.
*/
void XSceneWorld::DoBlinkSpotType( XGAME::xtSpot type, float sec, ID idCloud )
{
	bool bMoved = false;
	XArrayLinearN<XSpot*, 256> ary;
	sGetpWorld()->GetSpotsToAry( &ary, type );
	XARRAYLINEARN_LOOP_AUTO( ary, pBaseSpot ) {
		if( idCloud == 0 || 
			(idCloud && idCloud == pBaseSpot->GetidArea()) ) {
			int power = pBaseSpot->GetPower();
			// ?상태이거나 녹색이상인 스팟들만 블링크 시킨다.
				auto pWndSpot = SafeCast<XWndSpot*>( Find( pBaseSpot->GetidSpot() ) );
				if( XASSERT( pWndSpot ) ) {
					if( bMoved == false ) {
						bMoved = true;
						DoMoveToSpot( pBaseSpot->GetidSpot() );	// 첫번째스팟으로 이동
					}
					pWndSpot->DoBlink( sec );
				}
		}
	} END_LOOP;
}

/**
 @brief vCenter좌표로 스크롤이 되게 한다.
 @param secMove 스크롤 이동시간.
*/
void XSceneWorld::DoMovePosInWorld( const XE::VEC2& vCenter, float secMove )
{
	m_FocusMng.DoMove( m_pScrollView->GetvCenter(), vCenter, secMove );
}

/**
 @brief idCloud지역으로 이동
*/
void XSceneWorld::DoMoveToArea( ID idCloud, float secMove )
{
	auto vCenter = PROP_CLOUD->GetPosByArea( idCloud );
	m_FocusMng.DoMove( m_pScrollView->GetvCenter(), vCenter, secMove );
}

void XSceneWorld::DoMoveToSpot( ID idSpot, float secMove )
{
	auto pSpot = sGetpWorld()->GetSpot( idSpot );
	if( pSpot ) {
		auto vCenter = pSpot->GetPosWorld();
		m_FocusMng.DoMove( m_pScrollView->GetvCenter(), vCenter, secMove );
	}
}

/**
 @brief code스팟들의 중간지점으로 이동
*/
void XSceneWorld::DoMoveToCodeSpots( ID code, float sec )
{
	XE::VEC2 vSum;
	int cnt = 0;
	XArrayLinearN<XSpot*, 256> ary;
	sGetpWorld()->GetSpotsToAry( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, pBaseSpot )
	{
		if( pBaseSpot->GetpBaseProp()->idCode )
		{
			vSum += pBaseSpot->GetPosWorld();
			++cnt;
		}
	} END_LOOP;
	if( cnt == 0 )
	{
		m_FocusMng.Stop();
		return;
	}
	XE::VEC2 vCenter = vSum / (float)cnt;
	m_FocusMng.DoMove( m_pScrollView->GetvCenter(), vCenter, sec );
}

/**
 @brief 특정 좌표를 화살표가 가리키도록
*/
void XSceneWorld::DoDirectToPos( const XE::VEC2& vPos, float sec )
{

}

/**
 @brief 서버로부터 길드레이드 캠페인 정보가 도착함.
*/
void XSceneWorld::RecvCampaignByGuildRaid( ID idSpot, CampObjPtr spCampObj )
{
	// 길드레이드 창을 띄운다.
	auto pSpot = SafeCast<XSpotCommon*>( sGetpWorld()->GetSpot( idSpot ) );
	if( XASSERT( pSpot ) ) {
		// 길드레이드의 캠페인데이타는 스팟에 없고 길드정보에 있다.
//		XASSERT( pSpot->GetspCampObj() == nullptr );
		auto pProp = spCampObj->GetpProp();
		if( XASSERT( pProp ) ) {
			auto pPopup = new XWndGuildRaid( pSpot, spCampObj );
			pPopup->SetbModal( TRUE );
			Add( pPopup );
		}
	}
}

// xuzhu end

/****************************************************************
* @brief 
*****************************************************************/
int XSceneWorld::OnClickHome( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickHome");
	//
	if( m_pScrollView )
		/*m_pScrollView->SetFocusView( c_vHome );*/
		DoMovePosInWorld( c_vHome, 0.5f );
	return 1;
}

// /****************************************************************
// * @brief 
// *****************************************************************/
// int XSceneWorld::OnClickFillAPByCash( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickFillAPByCash");
// 	//
// 	if( GAMESVR_SOCKET->SendReqFillAP( GAME ) )
// 		DestroyWndByIdentifier("alert.ap");
// 	
// 	return 1;
// }

void XSceneWorld::OnReload()
{
}

/**
 @brief 스팟을 터치하고 서버로부터 정보가 옴.
*/
void XSceneWorld::OnRecvSpotTouch( XSpot *pBaseSpot, std::vector<xDropItem>& aryDropItem, XArchive& arAdd )
{
	// 스팟별 처리를 따로 할게 있다면 한다.
	switch( pBaseSpot->GettypeSpot() ) {
	case xSPOT_MANDRAKE:
		OnClickMandrakeSpot( pBaseSpot );
		break;
	case XGAME::xSPOT_CASH: {
		int numGet;
		arAdd >> numGet;
		auto pSpot = SafeCast<XSpotCash*>( pBaseSpot );
		_tstring str = XFORMAT( "+%d", numGet );
		auto pWndMsg = new XWndProduceMsg( pBaseSpot->GetidSpot(),
																				xRES_CASH,
																				numGet,
																				XCOLOR_GREEN );
		GetpScrollView()->Add( pWndMsg );
	} break;
	case xSPOT_NPC: {
		// 서버로부터 받은 드랍목록을 스팟에 교체한다.(이것은 휘발성정보다)
		pBaseSpot->SetDropItems( aryDropItem );
		OnClickNpcSpot( pBaseSpot );
	} break;
	case xSPOT_DAILY:
		OnClickDailySpot( pBaseSpot );
		break;
	case xSPOT_CASTLE:
	case xSPOT_JEWEL:
	case xSPOT_SULFUR:
//	case xSPOT_SPECIAL:
	case xSPOT_CAMPAIGN:
	case xSPOT_VISIT:
		break;
	case XGAME::xSPOT_GUILD_RAID:
		XBREAK( 1 );
		break;
	case XGAME::xSPOT_PRIVATE_RAID: {
		auto pSpot = SafeCast<XSpotPrivateRaid*>( pBaseSpot );
		auto pPopup = new XWndPrivateRaid( pSpot );
		pPopup->SetOkButton( "butt.ok" );
		pPopup->SetEvent2( XWM_OK, [this, pSpot](XWnd*) {
			// 서버로 내 출전영웅 리스트 보냄.
			GAMESVR_SOCKET->SendReqPrivateRaidEnterList( GAME, pSpot->GetlistEnter(0), pSpot->GetidSpot() );
			GAMESVR_SOCKET->SendReqEnterReadyScene( GAME, pSpot->GetidSpot() );
		});
		Add( pPopup );
	} break;
	case xSPOT_COMMON: {
		auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
		if( pSpot ) {
			if( pSpot->IsMedalCamp() ) {
				auto spCampObj = pSpot->GetspCampObj( nullptr );
				// 스팟정보를 받으면 캠페인 윈도우를 띄운다.
				XBREAK( spCampObj == nullptr );
				auto pPopup = new XWndPopupCampaignMedal( pBaseSpot, spCampObj );
				Add( pPopup );
			} else
			if( pSpot->IsHeroCamp() ) {
				auto spCampObj = pSpot->GetspCampObj( nullptr );
				// 스팟정보를 받으면 캠페인 윈도우를 띄운다.
				XBREAK( spCampObj == nullptr );
				auto pPopup = new XWndCampaignHero2( pSpot, spCampObj );
				Add( pPopup );
			}
		}
	} break;
	default:
		XBREAK( 1 );
		break;
	}
}

/**
 @brief 현재보이는 화면내의 모든지역의 이름라벨객체를 생성한다.
 이미 생성되어있다면 다시 생성시키지 않는다.
*/
void XSceneWorld::ApplyAreaBannersByOpened()
{
	// 현재 화면영역내에 보이는 지역리스트를 뽑는다.
	XE::xRECT rectVisible;
	XE::VEC2 vWorldMouseLT = m_pScrollView->GetvAdjScroll();
	vWorldMouseLT.Abs();
	rectVisible.vLT = vWorldMouseLT;
	rectVisible.vRB = vWorldMouseLT	+ GRAPHICS->GetViewportSize();
	// 보이는 영역내의 모든 지역을 얻는다.
	std::vector<ID> aryArea;
	PROP_CLOUD->GetAreaAryByRect( rectVisible, &aryArea );
	ApplyAreaBannerByAry( aryArea, true, false, 3.f );
}

/**
 @brief 지역아이디가 든 배열을 넘겨주면 지역명배너를 생성시킨다.
*/
void XSceneWorld::ApplyAreaBannerByAry( const std::vector<ID>& aryArea, bool bShowOpened, bool bShowClosed, float secLife )
{
	// 지역들을 루프
	for( auto idArea : aryArea ) {
		auto pProp = PROP_CLOUD->GetpProp( idArea );
		if( pProp ) {
			bool bCreate = false;
			bool bOpened = sGetpWorld()->IsOpenedArea( idArea );
			if( bShowOpened &&  bOpened ) 
				bCreate = true;
			if( bShowClosed && !bOpened )
				bCreate = true;
			if( bCreate ) {
				// 동적 배너전용 레이어에
				XWnd *pRoot = Find( "root.area.banner" );
				if( XASSERT( pRoot ) ) {
					std::string idsArea = SZ2C( pProp->strIdentifier );
					if( pRoot->Findf( "banner.%s", idsArea.c_str() ) == nullptr ) {
						// 배너객체 생성
						auto vCenter = PROP_CLOUD->GetPosByArea( pProp ) + pProp->vAdjust;
						auto pWndBanner = new XWndAreaBanner( vCenter, idArea, XTEXT( pProp->idName ) );
						if( secLife > 0 )
							pWndBanner->SetsecLife( secLife );
						pWndBanner->SetstrIdentifierf( "banner.%s", idsArea.c_str() );
						pRoot->Add( pWndBanner );
					}
				}
			}
		}
	}
}

// /**
//  @brief 
// */
// #ifdef _CHEAT
// int XSceneWorld::OnClickDebugShowLog( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE( "OnClickDebugShowLog" );
// 	char cFullpath[ 1024 ];
// 	XE::GetLogFilename( cFullpath );
// 	//
// 	if( p1 == 1 ) {
// 		XSYSTEM::RemoveFile( cFullpath );
// 		return 1;
// 	} else 
// 	if( p1 == 2 ) {
// 		// log toggle
// 		XE::SetbAllowLog( !XE::GetbAllowLog() );
// 		auto pButt = dynamic_cast<XWndButtonDebug*>( pWnd );
// 		if( pButt ) {
// 			pButt->SetbCheck( XE::GetbAllowLog() );
// 			pButt->SetbUpdate( true );
// 		}
// 	} else 
// 	if( p1 == 0 ) {
// 		//
// 		auto pPopup = new XWndPopup( XE::VEC2(455,340), _T("popup01.png") );
// 		pPopup->AddButtonOk(_T("ok"), _T("common_butt_small.png") );
// 		Add( pPopup );
// 		auto pScrlView = new XWndScrollView( XE::VEC2(7,9), XE::VEC2(451,302) );
// 	//	pScrlView->SetScrollVertOnly();
// 		pPopup->Add( pScrlView );
// 		TCHAR szLine[ 1024 ];
// 		TCHAR szBuff[ 1024 ];
// 		XE::VEC2 v;
// 		int cntLine = 0;
// 		float sizeFont = 18.f;
// 		int cntBlock = 0;
// 		FILE *fp;
// 		_tfopen_s( &fp, C2SZ(cFullpath), _T( "rt" ) );
// 		if( fp == nullptr ) {
// 			XWND_ALERT("file not found[%s]", C2SZ(cFullpath) );
// 			return 1;
// 		}
// 		long offset = ftell( fp );
// 		fseek( fp, 0, SEEK_END );
// 		int size = ftell( fp );			// file size
// 		fseek( fp, offset, SEEK_SET );
// 		while( !feof( fp ) ) {
// 			szBuff[ 0 ] = 0;
// 			if( cntBlock == 0 ) {
// 	#ifdef _VER_ANDROID
// 				_tcscat_s( szBuff, XFORMAT("size=%d",size) );
// 	#endif // _VER_ANDROID
// 			}
// 			while(1) {
// 				if( feof( fp ) )
// 					break;
// 	#ifdef WIN32
// 				fgetws( szLine, 1024, fp );		// utf8
// 	#else
// 				fgets( szLine, 1024, fp );		// utf8
// 	#endif
// 				if( _tcslen( szBuff ) + _tcslen(szLine) > 1000 )
// 					break;
// 				_tcscat_s( szBuff, szLine );
// 	#ifdef _VER_ANDROID
// 	// 				_tcscat_s( szBuff, _T("\r\n"));
// 	#endif // _VER_ANDROID
// 				++cntLine;
// 			}
// 			XSYSTEM::strReplace( szBuff, _T( '\r' ), _T( ' ' ) );
// 			auto pText = new XWndTextString( v,
// 																			szBuff,
// 																			FONT_NANUM,
// 																			sizeFont );
// 	//			pText->SetLineLength( 100 );
// 			pText->SetLineLength( pScrlView->GetSizeLocal().w - 10 );
// 			XBREAK( pText == NULL );
// 			pScrlView->Add( pText );
// 			v.y += pText->GetSizeNoTransLayout().h;
// 			++cntBlock;
// 		}
// 		fclose( fp );
// 		pScrlView->SetScrollViewAutoSize();
// 		auto vSizeView = pScrlView->GetsizeScroll();
// 		vSizeView.h = vSizeView.h * 1.01f;
// 		pScrlView->SetViewSize( vSizeView );
// 		pScrlView->SetFocusView( 0, vSizeView.h );	
// 	} // p1 == 0
// 	return 1;
// }
// #endif // _CHEAT

XE::VEC2 XSceneWorld::OnDelegateProjection2( const XE::VEC3& vPos )
{
	return XE::VEC2( vPos.x, vPos.y + vPos.z );
}

void XSceneWorld::OnRecvUnlockMenu( XGAME::xtMenus bitUnlock )
{
	switch( bitUnlock )
	{
	case XGAME::xBM_LABORATORY: {
		auto pButt = Find("butt.tech");
		if( pButt ) {
			GAME->AddBrilliant( pButt->getid() );
		}
	} break;
	case XGAME::xBM_CATHEDRAL:
		break;
	case XGAME::xBM_TAVERN:
		break;
	case XGAME::xBM_ACADEMY:
		break;
	case XGAME::XBM_UNIT_HANGOUT:
		break;
	case XGAME::xBM_EMBASSY:
		break;
	case XGAME::xBM_MARKET:
		break;
	case XGAME::xBM_DISPOSITION:
		break;
	case XGAME::xBM_EQUIP:
		break;
	case XGAME::xBM_BARRACK:
		break;
	case XGAME::xBM_ALL:
		break;
	default:
		break;
	}
}
/**
 @brief 씬에게 idHero의 XHero객체가 있는지 요청받는다.
*/
XSPHero XSceneWorld::GetpHero( ID idHero ) 
{ 
	return m_spAcc->GetHeroByidProp( idHero );
}

/**
 @brief 
*/
int XSceneWorld::OnClickProfile( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickProfile");
	//
	auto pPopup = new XWndPlayerInfo();
	pPopup->SetstrIdentifier("popup.player.info");
	Add( pPopup );
	return 1;
}

XWndBgObjLayer* XSceneWorld::GetpLayerBgObjUnder() 
{
	if( m_pScrollView == nullptr )
		return nullptr;
	return SafeCast2<XWndBgObjLayer*>( m_pScrollView->Find( "root.world.obj.under" ) );
}

/**
 @brief vwPos위치에 원형 주목UI를 띄운다.
*/
void XSceneWorld::UpdateAreaAttension( const XE::VEC2& vwPos )
{
	auto pLayer = GetpLayerAreaCost();
	if( pLayer ) {
		XWnd* pWnd = pLayer->Find("spr.area.attention");
		if( pWnd == nullptr ) {
			pWnd = new XWndSprObj( _T( "ui_alert.spr" ), 3, vwPos );
			pWnd->SetstrIdentifier( "spr.area.attention" );
			pLayer->Add( pWnd );
			pWnd->SetbTouchable( false );
		} else {
			pWnd->SetPosLocal( vwPos );
		}
	}
}

void XSceneWorld::DestroyAreaAttension()
{
	DestroyWndByIdentifier( "spr.area.attention" );
}

/**
 @brief 월드맵을 스크롤하다가 멈춤
*/
void XSceneWorld::DelegateStopScroll( const XE::VEC2& vCurr )
{
// 	CONSOLE("stop scroll=%.0f,%.0f", vCurr.x, vCurr.y );
	UpdateBgObjs();
}

void XSceneWorld::UpdateBgObjs()
{
	auto pLayer = GetpLayerBgObjUnder();
	if( pLayer ) {
		const auto vFocus = GetvwCamera();
		const auto sizeView = m_pScrollView->GetsizeView() * 1.25f;
		pLayer->UpdateCurrFocus( vFocus, sizeView );
	}
}

// void XSceneWorld::OnRecvSyncAcc( XGAME::xtParamSync type )
// {
// 	if( type == xPS_TRADER ) {
// 		// 무역상이 돌아왔으면 알림을 붙인다.
// 	}
// }

/**
 @brief 무역상이 도착함.
*/
void XSceneWorld::OnTraderArrive()
{
	auto pButt = xGET_BUTT_CTRL( this, "butt.market" );
	if( pButt ) {
		GAME->AddAlert( pButt, 1, XE::VEC2( 79, 16 ) );
//		XAccount::s_bTraderArrived = false;
	}
}

#ifdef _xIN_TOOL
void XSceneWorld::UpdateBgObjEach( xnBgObj::xProp* pProp )
{
	auto pLayer = GetpLayerBgObjUnder();
	if( pLayer ) {
//		auto pWndBgObj = pLayer->UpdateSelected();
	}
}

void XSceneWorld::UpdateBgObjSelected()
{
	auto pLayer = GetpLayerBgObjUnder();
	if( pLayer ) {
		pLayer->UpdateSelected();
	}
}

#endif // _xIN_TOOL
