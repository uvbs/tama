#include "stdafx.h"
#include "XGame.h"
#include "sprite/SprMng.h"
#include "sprite/SprObj.h"
#include "sprite/Key.h"
#include "XParticleMng.h"
#include "XFramework/XEToolTip.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "XResMng.h"
#include "XSystem.h"
#ifdef WIN32
#include "CaribeView.h"
#include "XSceneTest.h"
#endif // WIN32
#ifdef _VER_IOS
	#include "objc/xe_ios.h"
#endif
#ifdef _VER_ANDROID
#include "XFramework/android/com_mtricks_xe_Cocos2dxHelper.h"
#include "XFramework/android/JniHelper.h"
//#include "XWndEdit.h"
#endif
#include "XGameWnd.h"
#include "client/XAppMain.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/XSoundTable.h"
#include "XAccount.h"
#include "XSockGameSvr.h"
#include "XSockLoginSvr.h"
#include "XLoginInfo.h"
#include "XFramework/XFacebook.h"
#include "XSceneWorld.h"
#include "XSceneTitle.h"
#include "XSceneBattle.h"
#include "XSceneUnitOrg.h"
#include "XSceneShop.h"
#include "XSceneHero.h"
#include "XSceneLegion.h"
#include "XSceneStorage.h"
#include "XSceneTrader.h"
#include "XSceneArmory.h"
#include "XSceneTech.h"
#include "XSceneTechSel.h"
#include "XSceneEquip.h"
#include "XSceneGuild.h"
#include "XSceneReady.h"
#include "XScenePatchClient.h"
#include "XSceneLoading.h"
#include "XExpTableHero.h"
#include "XExpTableUser.h"
#include "XConnector.h"
#include "XPropHelp.h"
#include "XSeq.h"
#include "XPropSeq.h"
#include "XSceneGuildShop.h"
#include "XSkillMng.h"
#include "XSpots.h"
#include "XHero.h"
#include "XGameEtc.h"
#include "XPropBgObj.h"
#include "XWndDelegator.h"
#include "XFramework/XReceiverCallback.h"
#include "XWorld.h"
#include "XSoundMng.h"
#include "XOption.h"
#include "XWndTemplate.h"
#include "client/XAppDelegate.h"
#include "XDefNetwork.h"
#include "XFramework/XEProfile.h"
#ifdef _CHEAT
#include "OpenGL2/XTextureAtlas.h"
#endif // _CHEAT

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#include <vld.h>

using namespace xHelp;
using namespace XGAME;

#ifdef _XSINGLE
#pragma message("--------------------------single version-------")
#endif 

#if defined(_SOFTNYX)
#pragma message("SOFTNYX build=============================================")
#elif defined(_XGMO)
#pragma message("GMO build=============================================")
#elif defined(_VER_ANDROID)
#pragma message("GoogleStore build=============================================")
#elif defined(_VER_IOS)
#pragma message("AppleStore build=============================================")
#elif defined(WIN32)
#pragma message("Windows Store build=============================================")
#else
#error "마켓 전처리기가 지정되지 않음."
#endif

#define XWM_TIMEOUT	1001

XGame *GAME = nullptr;
XSockGameSvr *GAMESVR_SOCKET = nullptr;			// 월드서버와의 커넥션
XSockLoginSvr *LOGINSVR_SOCKET = nullptr;			// 로긴서버와의 커넥션
_tstring XGame::s_strVerBuild;		// 빌드버전
std::string XGame::s_strGcmRegid;		// gcm 푸쉬용 regid
bool XGame::s_bLoaded = false;

_tstring XGame::s_strSessionKey;		// 임시
std::list<_tstring> XGame::s_masterMessages;
XGame* XGame::s_pInstance = nullptr;


//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XGame::XGame() 
{ 
	GAME = this;
	s_pInstance = this;
	Init();
	XWndButton::s_modeAnimationDefault = XE::xBA_MOVE;	// 버튼들의 눌릴때 애니메이션 방식
#ifdef _XUZHU
// 	s_bXuzhuMode = true;		// 내컴에선 항상 트루
#endif
	XDelegator::sGet().SetpRoot( this );
	// 소켓객체 싱글톤을 생성한다. 아직 소켓을 만들진 않음.
	XConnector::sGet()->OnCreate();
}

void XGame::Destroy() 
{
	GAME = nullptr;
	s_pInstance = nullptr;
	XTRACE("XGame::Destroy\n");
	SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgPopup );
	SAFE_DELETE(m_pGuild);
	SAFE_DELETE(m_psoWait);
	SAFE_DELETE( m_pOption );
	DestroyGameResource();
//	XAppLayout::sDestroy();
//	SAFE_RELEASE_REF( m_pAccount );
	//DestroyWnd( m_pScene );		// 여기서 먼저 삭제해야 밑에 SPRMNG에서 해제안된spr이 남질 않게 된다.
//	SAFE_DELETE( TIMEOUT_MNG );
//	SAFE_DELETE( PARTICLE_MNG );
	SAFE_RELEASE_REF( GAMESVR_SOCKET );
	SAFE_RELEASE_REF( LOGINSVR_SOCKET );
	SAFE_DELETE( TEXT_TBL );
	XEProfile::sDestroy();
}

BOOL XGame::RestoreDevice()
{
//	XParticleMng::sGet()->RestoreDevice();
	//
	return XEContent::RestoreDevice();
}

const std::string XGame::OnSelectLanguageKey()
{
	// 옵션 로딩및 적용
	m_pOption = new XOption();
	m_pOption->Load();
//	XE::LANG.SetSelectedKey( m_pOption->GetstrcKeyCurrLang() );
	return m_pOption->GetstrcKeyCurrLang();
}

bool XGame::IsbFirst() const
{
	return LOGIN_INFO.GetstrUUID().empty();
}

//void XGame::Create()
void XGame::DidFinishCreated()
{
	XTRACE( "XGame::Create()" );
	const auto bMusic = m_pOption->GetbMusic();
	const auto bSound = m_pOption->GetbSound();
	
// 	SOUNDMNG->SetbMuteBGM( !bMusic );
// 	SOUNDMNG->SetbMuteSound( !bSound );
	if( bMusic ) {
		SOUNDMNG->SetBGMMasterVolume( m_pOption->GetvolMusic() );
	} else {
		SOUNDMNG->SetBGMMasterVolume( 0 );
	}
	if( bSound ) {
		SOUNDMNG->SetSoundMasterVolume( m_pOption->GetvolSound() );
	} else {
		SOUNDMNG->SetSoundMasterVolume( 0 );
	}
#ifdef WIN32
	// gcm 시뮬레이트
	std::string strGcmRegid("APA91bHELKNHPFrZEslHDF3vlpdRQwYzxz-tc5FEjPxUPvzoLG6jRQfz-3kgNovBQNKmuKNGS8x0FX8pUxmIYWDFzCm0NB3eghaRG0h-lDh0I2JStwjaOq39rT1ypLLYzQO34tPLP8kl");
	// 디바이스에서 새로운 gcm_regid가 생성된것을 시뮬레이션한다.
	XReceiverCallback::sGet()->cbOnReceiveCallback( "gcm_regid", strGcmRegid, "" );
#else
	// 최초실행시 보관되어있던 regid를 받아온다.
	std::string strRegid = CppToJava( "gcm_regid" );
	if( !strRegid.empty() ) {
		XBREAK( strRegid.length() > 255 );
		if( strRegid.length() < 256 ) {
			XLOGXNA( "stored gcm regid=%s", strRegid.c_str() );
			s_strGcmRegid = strRegid;
		}
}
#endif // WIN32
	LoadTextTable();

	std::string strPath = XE::MakePath( "", "LastUpdate.txt" );
	CONSOLE( "Find LastUpdate.txt.........." );
	if( !XE::IsExistFileInWork( strPath.c_str() ) ) {
		CONSOLE( "no have LastUpdate.txt....copy package to work" );
		XE::CopyPackageToWork( strPath.c_str() );
	}
	FONTMNG->SetpDelegate( this );
	//
	//////////////////////////////////////////////////////////////////////////
	// 루아파일들을 로딩타입과 관계없이 워킹으로 카피한다.
	CopyLuaToWork();
// 	// 텍스트 테이블
	// 클라/서버 공통으로 쓰는 리소스 로딩
	XGameCommon::Create();
	//
	XTRACE("Load connect.ini");
	// 마지막 로그인했던 아이디와 비번 정보
#if defined(_XSINGLE)
	const int idxLogin = 0;
	const _tstring strLoginFile = _T( "login.txt" );
#else
	CONNECT_INI.Load( _T( "connect.ini" ) );
	if( CONNECT_INI.GetstrcIPLogin().empty() ) {
		XALERT("%s", "로긴서버 아이피 지정되지 않음.");
	}
#ifdef _XPATCH
	if( CONNECT_INI.GetstrcIPPatch().empty() ) {
		XALERT( "%s", "패치서버 아이피 지정되지 않음." );
	}
#endif // _XPATCH
	#ifdef WIN32
		const int idxLogin = CaribeView::s_idxLogin;
		XBREAK( idxLogin < 0 );
		const _tstring strLoginFile = (idxLogin == 0)? _T("login.txt")
			: XFORMAT( "login%d.txt", idxLogin );
	#else
		const _tstring strLoginFile = _T( "login.txt" );
	#endif // WIN32
#endif
	XBREAK( strLoginFile.empty() );
// 	m_bFirst = (LOGIN_INFO.Load( strLoginFile.c_str() ) == FALSE);
	LOGIN_INFO.Load( strLoginFile.c_str() );

	m_pSceneMng = new XESceneMng( this, this );
	m_pSceneMng->SetidNextScene( XGAME::xSC_START );		// START부터 시작
	Add( m_pSceneMng );
	{
		m_pSceneMng->AddSceneInfo( xSC_WORLD, xSC_LOADING );		// 월드씬 진입은 항상 로딩화면을 브릿지로 사용한다.
		m_pSceneMng->AddSceneInfo( xSC_UNIT_ORG, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_TECH, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_HERO, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_STORAGE, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_TRADER, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_ARMORY, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_SHOP, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_READY, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_TITLE, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_LEGION, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_INGAME, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_EQUIP, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_GUILD, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_GUILD_SHOP, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_OPENNING, xSC_LOADING );
		m_pSceneMng->AddSceneInfo( xSC_INGAME, xSC_LOADING );
	}
//	CONSOLE( "GetViewportHeight:%d", (int)GRAPHICS->GetViewportHeight() );
//	CONSOLE( "vc.y:%d", (int)( GRAPHICS->GetViewportHeight() * 2.f - 0 ) );
	// 서버접속, 로비, 인게임 에서 사용될 리소스들을 로딩
#ifdef _XPATCH
#else
	CreateGameResource();
#endif 
	// 디폴트 프로핈하진 읽음.
	m_psfcProfile = IMAGE_MNG->Load( true, PATH_UI("fb_empty.png"), XE::xPF_RGB565 );
	//
	m_timerSec.Set( 3.f );
	m_timerMin.Set( xMIN_TO_SEC(1) );
	m_wndMasterText = new XWndTextString(XE::VEC2(70, 100), _T(""), FONT_NANUM, 50.f);
	if (m_wndMasterText)	{
		m_wndMasterText->SetAlign(XE::xALIGN_HCENTER);
		m_wndMasterText->SetStyle(xFONT::xSTYLE_STROKE);
		m_wndMasterText->SetLineLength(500.f);
		m_wndMasterText->SetColorText(XCOLOR_RGBA(255, 100, 50, 255));
		m_wndMasterText->SetPriority(-500);
		m_wndMasterText->SetbActive(FALSE);
		m_wndMasterText->SetColorBackground(XCOLOR_RGBA(0, 0, 0, 100));
		Add(m_wndMasterText);
	}
	/*XParticleMng::sGet()->SetpDelegate( this );
	// 분수형태 이미터
	auto pEmitter = new XEmitter( XE::GetGameSize() / 2.f,
								new XSprObj(_T("particle.spr")), 0 );
	pEmitter->AddSpeedComponent( new xParticle::XCompSpeedRandom( 1.f, 2.f ) );
	pEmitter->AddDeltaComponent( new xParticle::XCompDeltaLinear( 225.f, 315.f, 1.f ) );
	pEmitter->AddScaleComponent( new xParticle::XCompScaleFixed(1.f) );
	XParticleMng::sGet()->AddEmitter( pEmitter );
	//
	pEmitter = new XEmitter( XE::GetGameSize() / 2.f,
								_T( "eff_hit01.spr" ) );
	pEmitter->AddSpeedComponent( new xParticle::XCompSpeedRandom( 1.f, 2.f ) );
	pEmitter->AddDeltaComponent( new xParticle::XCompDeltaLinear( 45.f, 135.f, 1.f ) );
	pEmitter->AddScaleComponent( new xParticle::XCompScaleFixed( 1.f ) );
	XParticleMng::sGet()->AddEmitter( pEmitter );
*/
	/*
	방향 225~315도 랜덤, 스피드 1~2랜덤
	생존시간:1~2초랜덤
	중력처리기 붙임, 충돌처리기 붙임.
	*/

	/*// 운영자 메세지 테스트
		_tstring str(_T("동해물과 백두산이 마르고 닳도록\
		하느님이 보우하사 우리나라 만세.\
		무궁화 삼천리 화려강산\
		대한 사람, 대한으로 길이 보전하세\
		"));
		PushMasterMessage(str);
		PushMasterMessage(str);
		PushMasterMessage(str);
		PushMasterMessage(str);
		PushMasterMessage(str);
		PushMasterMessage(str);
	//*/
	//
//  	m_psoTest = new XSprObj(_T("unit_paladin.spr"));
//  	m_psoTest->SetAction( 98 );
	SetbUpdate( true );
} // Create()

// 루아파일들을 로딩타입과 관계없이 워킹으로 카피한다.
void XGame::CopyLuaToWork()
{
}

void XGame::CreateGameResource()
{
	XBREAK( s_bLoaded == true );
// #ifdef _XPATCH
// 	// 패치로 text_ko.txt가 갱신되었을 수도 있으니 다시 로딩함.
// 	SAFE_DELETE( TEXT_TBL );
// //	LoadTextTable();
// #endif
	// 공통 리소스 로딩.
	XGameCommon::CreateCommon();
// 	XPropBgObj::sGet()->LoadProp( _T("propObj.xml") );
	// 패치 클라가 들어가면 게임관련 리소스는 패치클라가 끝난후 로딩된다.
	XWndButton::SetDefaultEvent( XWM_SOUND_DOWN, this, &XGame::OnSoundDown );
	//
	XLOGXN("load main layout: layout.xml");
	// 메인 레이아웃 생성
//	XAppLayout::sCreate(_T("layout.xml"), nullptr );
	XLayout::sCreateMain(_T("layout.xml"), XAppDelegate::sGet() );
	//
	m_psoBrilliant = new XSprObj( _T("brilliant.spr") );
	m_psoBrilliant->SetAction( 1 );
	s_bLoaded = true;
} // createGameResource

void XGame::OnAfterPropSerialize()
{
	XGameCommon::OnAfterPropSerialize();
	XPropBgObj::sGet()->LoadProp( _T( "propObj.xml" ) );
}

/**
 @brief 
*/
void XGame::DestroyGameResource()
{
	XLOGXN("XGame::DestroyGameResource");
//	SAFE_RELEASE2( IMAGE_MNG, m_psfcWorldMap );
	SAFE_DELETE( m_psoBrilliant );
	XLayout::sDestroyMain();
	//
	XPropBgObj::sDestroyInstance();		// 이런걸 쓰지말고 디바이스자원 아닌것들은 나갈때 해제하지 말자. 램에 가진채로 나갔다가 다시들어올때와 한참있다가 다른프로세스에 의해 KILL됐을때 어떻게 들어오는지 봐야함.
	XGameCommon::DestroyCommon();
	//
#ifdef _XPATCH
	SAFE_DELETE( TEXT_TBL );
#endif
	XLOGXN("--XGame::DestroyGameResource");
}

XSceneBase* XGame::GetpScene() 
{
	auto pScene = SafeCast<XSceneBase*>( m_pSceneMng->GetpScene() );
	return pScene;
}

// idScene에 해당하는 씬객체를 생성해서 돌려줘야 한다.
XEBaseScene* XGame::DelegateCreateScene( XESceneMng *pSceneMng, ID idScene, SceneParamPtr& spParam )
{
	XSceneBase *pScene = nullptr;
	GAME->ClearBrilliant();
	switch( idScene )	{
	case XGAME::xSC_START:
		XAccount::sGetPlayer().reset();
#ifdef _XSINGLE
//		pSceneMng->SetidNextScene( XGAME::xSC_TEST );
	 		pSceneMng->SetidNextScene( XGAME::xSC_INGAME );
#else
#ifdef _XPATCH
		pSceneMng->SetidNextScene( XGAME::xSC_PATCH );
#else
		pSceneMng->SetidNextScene( XGAME::xSC_TITLE );
#endif
#endif
		break;
	case XGAME::xSC_OPENNING:
// 		pScene = new XSceneOpening( this, spParam );
// 		pScene->SetstrIdentifier( "scene.opening" );
		break;
	case XGAME::xSC_TITLE:
		pScene = new XSceneTitle( this );
		pScene->SetstrIdentifier( "scene.title");
		break;
	case XGAME::xSC_WORLD:
		pScene = new XSceneWorld(this, spParam);
		pScene->SetstrIdentifier( "scene.world" );
		break;
	case XGAME::xSC_UNIT_ORG:
		pScene = new XSceneUnitOrg(this, spParam);
		pScene->SetstrIdentifier( "scene.unitorg" );
		break;
	case XGAME::xSC_SHOP:
		pScene = new XSceneShop( this );
		pScene->SetstrIdentifier( "scene.shop" );
		break;
	case XGAME::xSC_HERO:
		pScene = new XSceneHero( this );
		pScene->SetstrIdentifier( "scene.hero" );
		break;
	case XGAME::xSC_LEGION:
		pScene = new XSceneLegion( this );
		pScene->SetstrIdentifier( "scene.legion" );
		break;
	case XGAME::xSC_INGAME:
		pScene = new XSceneBattle( this/*, spParam */);
		pScene->SetstrIdentifier( "scene.battle" );
		break;
	case XGAME::xSC_STORAGE:
		pScene = new XSceneStorage(this);
		pScene->SetstrIdentifier( "scene.storage" );
		break;
	case XGAME::xSC_TRADER:
		pScene = new XSceneTrader(this);
		pScene->SetstrIdentifier( "scene.trader" );
		break;
	case XGAME::xSC_ARMORY:
		pScene = new XSceneArmory(this);
		pScene->SetstrIdentifier( "scene.armory" );
		break;
	case XGAME::xSC_TECH:
		pScene = new XSceneTech( this );
		pScene->SetstrIdentifier( "scene.tech" );
		break;
	case XGAME::xSC_TECH_SEL:
		pScene = new XSceneTechSel( this );
		pScene->SetstrIdentifier( "scene.tech.sel" );
		break;
	case XGAME::xSC_EQUIP:
		pScene = new XSceneEquip(this, spParam);
		pScene->SetstrIdentifier( "scene.equip" );
		break;
	case XGAME::xSC_GUILD:
		pScene = new XSceneGuild(this);
		pScene->SetstrIdentifier( "scene.guild" );
		break;
	case XGAME::xSC_GUILD_SHOP:
		pScene = new XSceneGuildShop( this );
		pScene->SetstrIdentifier( "scene.guild.shop" );
		break;
	case XGAME::xSC_READY:
		pScene = new XSceneReady( this, spParam );
		pScene->SetstrIdentifier( "scene.ready" );
		break;
	case XGAME::xSC_PATCH:
		pScene = new XScenePatchClient();
		pScene->SetstrIdentifier( "scene.patch" );
		break;
#ifdef WIN32
	case XGAME::xSC_TEST:
		pScene = new XSceneTest( this, spParam );
		pScene->SetstrIdentifier( "scene.test" );
		break;
#endif // WIN32
	case XGAME::xSC_LOADING: {
		pScene = new XSceneLoading( this, spParam );
		pScene->SetstrIdentifier( "scene.loading" );
	} break;
	}
	if( pScene ) {
		XBREAK( pScene->GetstrIdentifier().empty() );		// 이제 씬은 반드시 식별자를 가져야함.
		// 씬이 파괴된 후 this에 이벤트가 발생하도록 이벤트를 등록함.
// 		pScene->SetpAfterDestroyEvent( this );
// 		// 씬이 생성되면서 최초 파일로딩이 모두 끝나면 핸들러를 호출하도록 한다.
// 		pScene->SetEvent( XWM_FINISH_LOADED, this, &XGame::OnFinishLoadedByScene, pScene->getid() );
	}
	return pScene;
}
void XGame::DelegateOnDestroy( XEBaseScene *pScene )
{
	DestroySeq();
}

void XGame::DelegateOnDestroyAfter( ID idSceneDestroy, ID idSceneNext, SceneParamPtr spParam ) 
{
	if( idSceneDestroy == xSC_WORLD && idSceneNext == xSC_START ) {
		if( spParam && spParam->m_strParam == "change_lang" ) {
			// 선택된 언어버전의 텍스트 다시 읽음.
			SAFE_DELETE( TEXT_TBL );
			GAME->LoadTextTable();
			// 캐싱되어있는 폰트 모두 삭제
// 			XEContent::DestroySystemFont();
// 			FONTMNG->DoFlushCache();
// 			XEContent::CreateSystemFont();
			FONTMNG->ReloadAll();
		}
	}
}

//
/**
 @brief 
*/
int XGame::Process( float dt )
{
	XPROF_OBJ_AUTO();
	if( GetbUpdate() )	{
		SetbUpdateChilds();		// 바로 아래 자식들에게 모두 업데이트 명령을 내림
		SetbUpdate( FALSE );
	}
	// 소켓객체의 프로세스(패킷펌핑)및 리커넥트 처리
	ProcessConnection();
	XConnector::sGet()->Process( dt );
	//
// 	if( m_psoTest ) {
// 		m_psoTest->SetAction( 98 );
// 		m_psoTest->SetFlipHoriz( TRUE );
// 		m_psoTest->SetmultiplySpeed( 4.95f );
// 		m_psoTest->FrameMove( dt );
// 	}
		//
	XEContent::Process( dt );
		//
	if( XAccount::sGetPlayer() ) {
		XAccount::sGetPlayer()->Process( dt );		// UI측에서 삭제될객체에 대한 처리를 먼저하고 여기서 삭제를 하기위해 위치를 옮겨옴.
	}
	//
	XTimeoutMng::sGet()->Process( dt );
	FrameMoveBrilliant( dt );
	// 1초 타이머
	if( m_timerSec.IsOver() ) {
		m_timerSec.Reset();
		if( ACCOUNT )
			ACCOUNT->ProcessClientPerSec( dt );
		// 연구시간 체크
		ProcessResearch( dt );
		// 훈련시간 체크
		ProcessTraining( dt );
		// 행동력 시간 체크
		ProcessAP( dt );
	}
	// 1분 타이머
	if( m_timerMin.IsOver() ) {
		m_timerMin.Reset();
		// 무역상 데이터 동기화 요청.
#ifndef _XSINGLE
		GAMESVR_SOCKET->SendReqSync( this, xPS_TRADER );
#endif // not _XSINGLE
	}
	if( m_psoWait )
		m_psoWait->FrameMove( dt );

	// 운영자 메세지
	if (m_masterMessgeTimer.IsOver()) {
		if (!s_masterMessages.empty()) {
			// 메세지가 비어있지 않으면
			if (m_wndMasterText) {
				m_wndMasterText->SetText(*s_masterMessages.begin());
				m_wndMasterText->SetbShow(TRUE);
			}
			s_masterMessages.erase(s_masterMessages.begin());
			m_masterMessgeTimer.Set(5);
			SortPriority();
		} else {
			// 비어 있으면 끝
			m_masterMessgeTimer.Off();
			if (m_wndMasterText) {
				m_wndMasterText->SetbShow(FALSE);
			}
		}
	} else 
	if (m_masterMessgeTimer.IsOn()) {
		// 계속해서 알파값 조정
		if (m_wndMasterText)	{
			float a = (float)m_masterMessgeTimer.GetRemainTime() / 5000;
			m_wndMasterText->SetAlphaLocal((float)m_masterMessgeTimer.GetRemainTime() / 5000);
		}
	}
	++m_Count;
	return 1;
}

void XGame::ProcessResearch( float dt )
{
	if( XAccount::sGetPlayer() == nullptr )
		return;
	// 게임서버에 정상적으로 연결되어있을때만 처리함.
	if( !XConnector::sGet()->IsOnline() )
		return;
	auto& research = XAccount::sGetPlayer()->GetResearching();
	ID idAbilResearchNow = research.GetidAbil();
	if( idAbilResearchNow == 0 )
		return;		// 연구중인 특성이 없음.
	XBREAK( research.IsResearching() == false );
	auto pHero = XAccount::sGetPlayer()->GetHero( research.GetsnHero() );
	if( XBREAK(pHero == nullptr) )
		return;
	int numPoint = 1 + pHero->GetNumSetAbilPoint( /*pProp->unit*/ );
	auto& costAbil = XPropTech::sGet()->GetResearchWithNum( numPoint );
	if( research.IsComplete() ) {
		// 연구가 다끝남.
		// 서버측에서 완료신호를 보내줄때까지 x초마다 계속 확인한다.
		if( !XTimeoutMng::sGet()->IsRequesting() ) {
			static int s_cnt = 0;
			XTRACE( "Check Research:%d", ++s_cnt );
			GAMESVR_SOCKET->SendReqResearchComplete( this, pHero->GetsnHero(), idAbilResearchNow );
		}
	}

}

/**
 @brief 주기적으로 검사해서 훈련이 끝난 슬롯은 확인패킷을 보냄.
*/
void XGame::ProcessTraining( float dt )
{
	if( XAccount::sGetPlayer() == nullptr )
		return;
	if( !XConnector::sGet()->IsOnline() )
		return;
	if( XAccount::sGetPlayer()->GetNumActiveSlot() == 0 )
		return;
	XArrayLinearN<ID,64> ary;
	XAccount::sGetPlayer()->GetTrainCompleteSlots( &ary );
	XARRAYLINEARN_LOOP_AUTO( ary, snSlot ) {
		if( !XTimeoutMng::sGet()->IsRequesting() ) {
			static int s_cnt = 0;
			XTRACE("Check Training:%d", ++s_cnt );
			GAMESVR_SOCKET->SendReqCheckTrainComplete( this, snSlot );
		}
	} END_LOOP
}

void XGame::ProcessAP( float dt )
{
	if( XAccount::sGetPlayer() && XAccount::sGetPlayer()->IsAPTimeOver() ) {
		// 지금 뭔가 응답을 기다리고 있는중이면 안보냄.
		if( !XTimeoutMng::sGet()->IsRequesting() ) {
			if( XConnector::sGet()->IsOnline() ) {
				static int s_cnt = 0;
				CONSOLE( "Check AP:%d", ++s_cnt );
				GAMESVR_SOCKET->SendReqCheckAPTimeOver( this );
			}
		}
	}
}
/**
 @brief 
*/
void XGame::Draw()
{
	XEContent::Draw();
	if( TOOLTIP )
		TOOLTIP->Draw();
	// 
#ifndef _XSINGLE
	bool bWaiting = false;
	if( XTimeoutMng::sGet()->IsRequesting() ) {
		// 응답요청하고 조금 지나도 안오면 화면 어둡게 하고 메시지 띄우기 시작.
		if( XTimeoutMng::sGet()->IsOverMsgTime() ) {
			auto pReq = XTimeoutMng::sGet()->GetRequesting();
			XBREAK( pReq == nullptr );
			GRAPHICS->FillRectSize( XE::VEC2(0), XGAME_SIZE, XCOLOR_RGBA( 0, 0, 0, 64 ) );
			XAPP->GetpGame()->GetpfoSystem()->SetAlign( XE::xALIGN_HCENTER );
			XAPP->GetpGame()->GetpfoSystem()->SetLineLength( XE::GetGameWidth() );
//			_tstring str = XFORMAT("no response(%.0f): %d", pReq->GetsecRemain(), pReq->GetidPacket() );
// 			XAPP->GetpGame()->GetpfoSystem()->DrawString( 0.f, XE::GetGameWidth() / 2.f, str.c_str() );
			// FTGL이 std::string으로 하면 가끔 죽는버그가 있어서 정적메모리로 바꿔봄. 아마 내부에서 이전 어드레스를 캐시로 가지고 쓴느듯.
			static TCHAR szBuff[ 1024 ];
			_tcscpy_s( szBuff, XFORMAT("no response(%.0f): %d", pReq->GetsecRemain(), pReq->GetidPacket() ) );
			XAPP->GetpGame()->GetpfoSystem()->DrawString( 0.f, XE::GetGameWidth() / 2.f, szBuff );
			bWaiting = true;
		}
	}
	if( XConnector::sGet()->IsConnecting() )
		bWaiting = true;
	if( bWaiting ) {
		if( m_psoWait == nullptr ) {
			m_psoWait = new XSprObj( _T( "ui_loading.spr" ) );
			m_psoWait->SetAction( 1 );
		}
		m_psoWait->Draw( XE::GetGameSize() / 2.f );
	}
#endif // not _XSINGLE
	XConnector::sGet()->Draw();
	// 컷씬 시퀀스 표시
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && ACCOUNT ) {
		XE::VEC2 v(3,217);
		_tstring str = _T("idsSeqLast:");
		str += C2SZ(ACCOUNT->GetidsLastSeq());
		PUT_STRING_STYLE( v.x, v.y, XCOLOR_SKY, xFONT::xSTYLE_SHADOW, str.c_str() );
		v.y += 12.f;
		for( auto& idsSeq : m_listSeq ) {
			str = C2SZ(idsSeq);
			PUT_STRING_SHADOW( v.x, v.y, XCOLOR_BLUE, str.c_str() );
			v.y += 10.f;
		}
		bool bBlocking = false;
		if( GetpScene() ) 
			if( !GetpScene()->GetbActive() )
				bBlocking = true;
		if( bBlocking )
			PUT_STRING_SHADOW( 597, 0, XCOLOR_RED, _T("blocking"));
		if( m_listAllowWnd.size() > 0 ) {
			XE::VEC2 v(587,0);
			PUT_STRING_SHADOW( v.x, v.y, XCOLOR_RED, _T( "Allow touch" ) );
			v.y += 10.f;
			for( auto& elem : m_listAllowWnd ) {
				str = C2SZ(elem);
				PUT_STRING_SHADOW( v.x, v.y, XCOLOR_RED, str.c_str() );
				v.y += 10.f;
			}
		}
	}
	if( XAPP->m_bViewMemoryInfo ) {
		const auto sizeTotalVMem = XSurface::sGetsizeTotalVMem();
		XE::VEC2 v(39,92);
		PUT_STRING_STROKE( v.x, v.y, XCOLOR_YELLOW
									, XFORMAT("TotalXSurface vMem=%sM"
											, XE::NumberToMoneyString(sizeTotalVMem / 1024 / 1024)) );
		v.y += 10.f;
		PUT_STRING_STROKE( v.x, v.y, XCOLOR_YELLOW
									, XFORMAT("ImageMng vMem=%sM"
											, XE::NumberToMoneyString(XImageMng::s_sizeTotalVMem / 1024 / 1024)) );
		v.y += 10.f;
		PUT_STRING_STROKE( v.x, v.y, XCOLOR_YELLOW
									, XFORMAT("SprMng vMem=%sM"
											, XE::NumberToMoneyString(XSprMng::s_sizeTotalVM / 1024 / 1024)) );
	}
#ifdef WIN32
	if( XWnd::s_bDrawMouseOverWins && XWnd::s_aryMouseOver.size() ) {
		const auto vMouse = INPUTMNG->GetMousePos();
		XE::VEC2 vSpace(0, 10);
		XE::VEC2 vLT = vMouse + XE::VEC2( 100, 0 );		// 시작위치 
		auto aryWins = XWnd::sGetStringByMouseOverWins();
		XE::VEC2 size( 100, 100 );
		size.h = 10.f * aryWins.Size();
		if( vLT.x + size.w > XE::GetGameWidth() )
			vLT.x = XE::GetGameWidth() - size.w;
		if( vLT.x < 0 )
			vLT.x = 0;
		auto vDist = vLT - vMouse;
		if( vDist.x > 0 && vDist.x < size.w )
			vLT.x = vMouse.x - size.w;
		if( vDist.y > 0 && vDist.y < size.h )
			vLT.y = vMouse.y - size.h * 2.f;
		if( vLT.y + size.h > XE::GetGameHeight() )
			vLT.y = XE::GetGameHeight() - size.h;
		if( vLT.y < 0 )
			vLT.y = 0;
		for( auto& win : aryWins ) {
			_tstring strt = C2SZ( win.m_str );
			PUT_STRING_STROKE( vLT.x + win.m_Depth * 5.f, vLT.y, XCOLOR_YELLOW, strt.c_str() );
			vLT.y += vSpace.h;
		}
// 		PUT_STRING_STROKE( vMouse.x + 100.f, vMouse.y, XCOLOR_YELLOW, strt.c_str() );
	}
#endif // WIN32
	if( XAPP->m_idxViewAtlas >= 0 ) {
		auto idTex = XTextureAtlas::sGet()->GetidTex( XAPP->m_idxViewAtlas );
		if( idTex )
			GRAPHICS_GL->DrawTexture( idTex, 0, 0, 356.f, 356.f, FALSE );
	}
#endif // _CHEAT
// 	if( m_psoTest ) {
// 		auto vMouse = INPUTMNG->GetMousePos();
// //		m_psoTest->Draw( vMouse );
//  		m_psoTest->Draw( XE::GetGameSize() * 0.75f );
// 	}
} // void XGame::Draw()

void XGame::OnLButtonDown( float x, float y )
{
	// 뭔가 응답을 기다리고 있는중이면 입력을 막음.
	if( XTimeoutMng::sGet()->IsRequesting() ) {
		CONSOLE("waiting network request.");
		return;
	}
	bool bOk = true;
	if( IsPlayingSeq() ) {
		auto pButt = dynamic_cast<XWndButtonString*>( Find( "butt.tutor.stop" ) );
		if( pButt ) {
			if( pButt->IsWndAreaInByScreen( XE::VEC2(x,y) ) )
				bOk = false;		// 튜토중단 버튼위치를 눌렀으면 아래 listAllow검사는 안함.
		}
	} else {
		// 컷씬중이 아닌데 블로킹 되어있으면 해제함.
		GetpScene()->SetActive( true );
	}
	if( m_listAllowWnd.size() && bOk ) {
		if( IsOutsideClickedAllowWnd( XE::VEC2( x, y ) ) ) {
			XTRACE("IsOutsideClickedAllowWnd");
			return;
		}
	}
	XEContent::OnLButtonDown( x, y );
}
void XGame::OnMouseMove( float x, float y )
{
#ifdef xIN_TOOL


#endif // xIN_TOOL
#ifdef WIN32
	XWnd::s_pMouseOver = nullptr;
#endif // WIN32
	XEContent::OnMouseMove( x, y );
}
#ifdef _DEBUG
// #include "Sound/Windows/OpenAL/XOpenAL.h"
// #include "Sound/Windows/OpenAL/XSoundMngOpenAL.h"
// XVector<int> arySrc;
// XOALObj alObj;
#endif // _DEBUG

void XGame::OnLButtonUp( float x, float y ) 
{
#ifdef _DEBUG
// 	const std::string strFile = XSOUND_FILE( 50 );
// 	if( !alObj.IsLoaded() ) {
// 		SOUNDMNG->OpenPlaySound( 50 );
// 	}
// 	auto pSoundMng = static_cast<XSoundMngOpenAL*>( SOUNDMNG );
// 	auto alBuffer = pSoundMng->GetalBuffer( 50 );
// 	int alSrc = XOALObj::CreateSouce( alBuffer );
// 	XOALObj::Play2( alSrc );
#endif // _DEBUG
#ifdef _CHEAT
	const XE::VEC2 vMouse(x, y);
	if( XAPP->m_ModeTest == XAppMain::xTEST_PARTICLE ) {
		auto pLayer = new XWndParticleLayer( m_strcNodeParticle.c_str(), vMouse, 3.f, nullptr );
		Add( pLayer );
	}
#endif
	// 뭔가 응답을 기다리고 있는중이면 입력을 막음.
	if( XTimeoutMng::sGet()->IsRequesting() )
		return;
	bool bOk = true;
	if( IsPlayingSeq() ) {
		auto pButt = dynamic_cast<XWndButtonString*>( Find( "butt.tutor.stop" ) );
		if( pButt ) {
			if( pButt->IsWndAreaInByScreen( XE::VEC2(x,y) ) )
				bOk = false;		// 튜토중단 버튼위치를 눌렀으면 아래 listAllow검사는 안함.
		}
	}
	if( m_listAllowWnd.size() && bOk ) {
		if( IsOutsideClickedAllowWnd( XE::VEC2( x, y ) ) )
			return;
	}
// 	if( CheckAllowWnd( XE::VEC2(x,y) ) )
// 		return;
	XEContent::OnLButtonUp( x, y );
}
/**
 @brief 터치가 허용된 윈도우들외에 영역을 터치했는가.
*/
bool XGame::IsOutsideClickedAllowWnd( const XE::VEC2& vPos )
{
// 	if( !m_idsAllow.empty() ) {
// 		XWnd *pWnd = Find( m_idsAllow );
// 		if( pWnd ) {
// 			if( !pWnd->IsWndAreaInByScreen( vPos ) )
// 				return true;
// 		}
// 	}
//	return false;
	bool bClicked = false;
	for( auto& idsAllow : m_listAllowWnd ) {
		if( !idsAllow.empty() ) {
			XWnd *pWnd = Find( idsAllow );
			if( pWnd ) {
				if( pWnd->IsWndAreaInByScreen( vPos ) ) {
					bClicked = true;
					break;
				}
			}
		}
	}
	return !bClicked;
}
void XGame::OnRButtonDown( float x, float y )
{
	XEContent::OnRButtonDown( x, y );
}

void XGame::OnRButtonUp( float x, float y )
{
	XEContent::OnRButtonUp( x, y );
}


int XGame::OnExitApp( XWnd *pWnd, DWORD, DWORD )
{
	XAPP->SetbDestroy( TRUE );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XGame::OnGotoStart( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnGotoStart");
	//
	XConnector::sGet()->StopFSM();	// 커넥팅 처리를 중지시켜서 홈상태에서 재연결을 시도하지 않도록 한다.
	GAMESVR_SOCKET->DoDisconnect();
	GAMESVR_SOCKET->ClearConnection();
//	XAccount::sGetPlayer().reset();
	ID idParam = p1;
	SceneParamPtr spParam = std::make_shared<XGAME::xSPM_BASE>();
	if( idParam == 1 )
		spParam->m_strParam = "change_lang";
	GAME->GetpScene()->DoExit( XGAME::xSC_START, spParam );
	return 1;
}


void XGame::OnCheatMode()
{
	if( m_pSceneMng )
		m_pSceneMng->OnCheatMode();
}

int XGame::OnIpConfig( XWnd *pWnd, DWORD, DWORD )
{
#ifdef _VER_IOS
		IOS::DoModalEditBox( XGAME::EID_CHANGE_IP );
#endif
		return 1;
}

// IP 어드레스 바꿈
void XGame::OnEndEditBox( int idEditField, const char *cStr )
{
#ifndef WIN32
		switch( idEditField )
		{
				case XGAME::EID_CHANGE_IP:
						exit(1);
						break;
		}
#endif
}

// only android
int XGame::OnEnterIP( XWnd *pWnd, DWORD p1, DWORD p2 )
{
#ifdef _VER_ANDROID
	LPCTSTR szStr = (LPCTSTR) p2;
	AXLOGXN("OnEnterIP");
	AXLOGXN("OnEnterIP:%s", szStr );
	OnEndEditBox( XGAME::EID_CHANGE_IP, szStr );
#endif
	return 1;
}

// 현재 어느씬에 있건 다 종료하고 접속도 끊고 초기화면의 로그인화면으로 간다.
int XGame::OnGotoLogin( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	exit(1);		// 일단 걍 종료
}

XBaseFontDat* XGame::OnDelegateCreateFontDat( XFontMng *pFontMng, 
											LPCTSTR szFont, 
											float sizeFont )
{
	if( XE::IsSame( szFont, _T("font_meter.spr") ) )
	{
		XFontDatSpr *pFontDat = new XFontDatSprGold( szFont );
		pFontDat->SetfDist( -2.f );
		return pFontDat;
	} else
	if( XE::IsSame( szFont, _T("font_gold.spr") ) )
	{
		XFontDatSpr *pFontDat = new XFontDatSprGold( szFont );
		pFontDat->SetfDist( -2.f );
		return pFontDat;
	} else
	{
		LPCTSTR szExt = XE::GetFileExt( szFont );
		if( XE::IsSame( szExt, _T("spr") ) )
		{
			XFontDatSpr *pFontDat = new XFontDatSprNumber( szFont );
			if( pFontDat->IsError() )
			{
				SAFE_DELETE( pFontDat );
				CONSOLE("font: %s loading error");
			}
			return pFontDat;
		}
	}
	return nullptr;
}

void XGame::OnDelegateLayoutPlaySound( ID idSnd )
{
	SOUNDMNG->OpenPlaySound( idSnd );
}

void XGame::OnDelegateLayoutPlayMusic( LPCTSTR szBgm )
{
	char cBgm[ 256 ];
	strcpy_s( cBgm, SZ2C( szBgm ) );
	SOUNDMNG->OpenPlayBGMusic( cBgm );
}


void XGame::ProcessConnection()
{
	// 접속이 끊어져도 남아있는 패킷은 다 처리해아하므로 계속 돌아가게함.
	if( GAMESVR_SOCKET )
		GAMESVR_SOCKET->Process();
	if( LOGINSVR_SOCKET )
		LOGINSVR_SOCKET->Process();
}

/**
 @brief 홈에서 돌아옴
*/
void XGame::OnResume() 
{
	/* OnResume될때 패치를 다시 검사해야하지만 현재 패치검사가 씬형태로 되어있어서
	패치를 검사해버리면 원래 플레이중이던 씬이 없어져서 곤란하다.
	적어도 패치 검사는 씬에서 하면 안된다. 
	*/
	// 게임서버로 알림
	XBREAK( m_pSceneMng == nullptr );
	if( GetpScene() ) {
		if( GAMESVR_SOCKET->IsDisconnected() ) {
			// 다시 돌아왔는데 접속이 끊어진상태라면
			XTRACE( "OnResume: disconnected server" );
			// 큐에 남아있던 버퍼 다 클리어하고 네트워크시스템 클리어 시키고 타이틀로 이동해서 다시 접속해야함.
			XTRACE( "L:curr qSize=%d", LOGINSVR_SOCKET->GetSizeQueue() );
			XTRACE( "G:curr qSize=%d", GAMESVR_SOCKET->GetSizeQueue() );
			// 홈에서 돌아왔는데 연결이 끊겨있다. 이때 전투중인경우는 그대로 재연결을 시도하고 그 외 상황은 타이틀부터 다시 시작함.
			if( GetpScene()->GetidScene() == XGAME::xSC_INGAME ) {
				// 전투중이었으면 씬은 그대로 두고 재접만 시도한다.
				XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
			} else {
				LOGINSVR_SOCKET->DoDisconnect();
				LOGINSVR_SOCKET->ClearConnection();	// 패킷큐를 클리어하고 최초상태로 만든다.
				GAMESVR_SOCKET->DoDisconnect();
				GAMESVR_SOCKET->ClearConnection();	// 패킷큐를 클리어하고 최초상태로 만든다.
				GetpScene()->DoExit( XGAME::xSC_START );
				/*
				 원래 계획:
					 .시작화면으로 가야되는 상황이면 안쓰는 각종 리소스들을 다시 로딩하여 restore시킬필요가 없다.
					 .따라서 여기서 씬을 실제로 날려버리고 리소스 매니저들을 flush시킨 후 외부에서 Restore를 하면 로딩시간(resume시간)이 줄어든다.
				 문제점:
					 .현재 씬을 지금 당장 날려버릴 방법이 없다. XWnd객체의 삭제는 플래그만 주고 process에서 삭제하기 때문.
					 .restore를 잠시 미루고 씬에게 파괴후 델리게이터를 만들어 그곳에서 리스토어를 하면 되긴됨.
				*/
//				m_pSceneMng->DoForceDestroyCurrScene();
				// img,spr,font등 현재 안쓰고 있는것들 모두 해제하여 다음 restore때 필요없는것들을 restore하지 않도록 한다. 이것은 resume시간을 빠르게 하기 위해서다.
				IMAGE_MNG->DoFlushCache();
				SPRMNG->DoFlushCache();
				FONTMNG->DoFlushCache();
			}
		} else  {
			XConnector::sGet()->SetidNextFSM( xConnector::xFID_ONLINE );
			XTRACE( "OnResume: connected server" );
			XTRACE( "L:curr qSize=%d", LOGINSVR_SOCKET->GetSizeQueue() );
			XTRACE( "G:curr qSize=%d", GAMESVR_SOCKET->GetSizeQueue() );
			// 서버에 알려야 하나?-잠깐 나갔다 들어온경우는 큐에 남아있던 패킷 그대로 다 처리하면서 그냥 그대로 진행해도 될듯
			// 리쥼되면 페북이미지 서피스들 모두 날아갔으므로 페북 프로필 이미지들 다시 요청해서 받아야 함.
			DoRequestPlayerProfileImg( ACCOUNT );
			DoRequestProfileImgAllSpot();
			if( sGetpWorld() ) {
				sGetpWorld()->Restore();
			}
			IMAGE_MNG->DoFlushCache();
			SPRMNG->DoFlushCache();
			FONTMNG->DoFlushCache();
		}
	}
	CONSOLE("OnResume: finished");
}
// 홈버튼으로 나가기전 호출됨
void XGame::OnPause() 
{
	XTRACE("XGame::OnPause");
	XConnector::sGet()->StopFSM();	// 커넥팅 처리를 중지시켜서 홈상태에서 재연결을 시도하지 않도록 한다.
#ifdef _XHOME_PROC
	

#endif // _XHOME_PROC
	if( GAMESVR_SOCKET->IsConnected() ) {
		GAMESVR_SOCKET->SendGotoHome( 10 );	// xx초 이내 다시 돌아오면 끊기지않음.
	}
}		


// 구글/iOS의 인앱결제로 szSku상품을 사라. 이것의 결과는 비동기로 돌아온다.
int XGame::DoAsyncBuyIAP( LPCTSTR szSku, const std::string& strcPayload, int price )
{
	// 서버에 hb검사를 잠시(xx초) 보류해달라고 요청.
	GAMESVR_SOCKET->SendGotoHome( 30 );
	// 결과는 XGame::OnReceiveCallbackData로 온다.
#if defined(_VER_ANDROID)
	//
 #ifdef _SOFTNYX
	XBREAK( price == 0 );
	XBREAK( strcPayload.empty() );
	XBREAK( XE::IsEmpty(szSku) );
	JniHelper::SoftnyxBuyItem( szSku, price, strcPayload );
 #else
	JniHelper::OnClickedBuyItem( szSku, strcPayload );
 #endif // _SOFTNYX
#elif defined(WIN32)

	#ifdef _SOFTNYX
	auto pItemInfo = XGC->GetCashItem( szSku );
	if( XASSERT(pItemInfo) ) {
		std::string strSku = SZ2C( szSku );
		xnReceiverCallback::xData data;
		data.m_strIds = "iap_buy_request";
		data.m_aryParam[ 0 ] = "{\"returnCode\":\"0\",\"point\":\"0\",\"paymentToken\":\"d6ff798f7fdd546901fef3ec5c0fff3a9bea3372b9044294b60dc12492576f\",\"orderID\":\"10961\"}";
		data.m_aryParam[ 1 ] = strcPayload;
		data.m_aryParam[ 2 ] = SZ2C(szSku);
		OnReceiveCallbackData( data );
	}
	#else
	// win32는 샀다고 치고 시뮬레이션만 한a다.
	std::string strSku = SZ2C(szSku);
	xnReceiverCallback::xData data;
	data.m_strIds = "iap_buy_request";
	data.m_aryParam[ 0 ] = "{\"orderId\":\"GPA.1323-8171-3401-29065\", \"packageName\" : \"com.gemtree.caribe\", \"productId\" : \"inapp_cash00001\", \"purchaseTime\" : 1456728561530, \"purchaseState\" : 0, \"developerPayload\" : \"58281C8F977B4FFAA35B75687CE8A2FF\", \"purchaseToken\" : \"nncdakdlhamfmfenmdmkfppm.AO-J1OwUMeCm6pxBRGSDnPCL7N49W9D3mhQgsHSxyR0k0WjE2ajzqpJEDF6afhh0qtB7rHEtpBqbn3tuANbvKlEkzfb7aDJEUCbpq4y-CA04CvqPQKkIhxPKpeUmsmj7avbOliWVSnbN\"}";
	data.m_aryParam[ 1 ] = "etTgzD99U2f5UyEncC1BECCNNpWpFef5mFXOvebYqXsSguopFia6EIfP6A+Hf0sY04azuZdHN3rqWfGo1qgpKLSoYNmlIcuG3BV9E/PE3GklQC6UpCrSPFLKxtZUdRh/W9xuDEKjQMUUQawxeTJW8ydebZPqzrO4TIQxQj6vwjtjcjqhzN8zqyHo8tdZ8KOPUse9pvZZOCQwWEOfG+nP830LxXrg4DlwjxCspHLy7VoPh5OqVpNly7CTcZobSf2dAWZjZsd3X19vtZ8cqDEUUZTJtABHqKjITf/DGziod/uy3UgDYjKKD1/gH4HU560D0BPXeAPW0vrzJgO1Fc6uDQ==";
	OnReceiveCallbackData( data );
	#endif // _SOFTNYX
#elif defined(_VER_IOS)
	// 아직 구현안됨
	IOS::SendReqPurchase( szSku );
#endif
	CONSOLE("%s:", __TFUNC__);
	return 1;
}

void XGame::OnPurchaseError( const char *cErr )
{
	CONSOLE("OnPurchaseError: %s", C2SZ(cErr) );
	XTimeoutMng::sGet()->Arrive( xTIMEOUT_BUY_IAP );
}

/**
 구매를 위해 비번을 치고 실제 구매요청을 보낸직후 호출되는 이벤트
 */
void XGame::OnPurchaseStart( const char *cSku )
{
	CONSOLE("XGame::OnPurchaseError: %s", C2SZ(cSku) );
	XTimeoutMng::sGet()->Add( this->GetID(), xTIMEOUT_BUY_IAP );
}

// 버튼 누를때 나는 일반적인 소리를 지정한다.
int XGame::OnSoundDown( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	int idSnd = (int)p1;
	if( idSnd == 0 )
		idSnd = 3;
	SOUNDMNG->OpenPlaySound( idSnd );
	return 1;
}

/**
 pSpot의 자원이 리젠될때마다 호출
*/
void XGame::DelegateOnCalcTimer( XSpot *pBaseSpot, const XTimer2& timerCalc )
{
	if( !pBaseSpot->IsOpenedSpot( sGetpWorld() ) )
		return;
	// 타이머의 주기를 60초로 나누어 60초단위로 바꾼다.
	const float sec = timerCalc.GetsecWaitTime();
	const auto typeSpot = pBaseSpot->GettypeSpot();
	if( typeSpot == XGAME::xSPOT_CASTLE ) {
		auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
		XBREAK( pSpot == nullptr );
		// 다른씬에 있더라도 생산된 자원이 쌓이도록
		std::vector<xRES_NUM> aryRes;
		pSpot->RegenResource( sec
												, ACCOUNT->GetidAccount()
												, &aryRes );
		if( SCENE_WORLD ) {
			SCENE_WORLD->OnProduceResAtSpot( pBaseSpot, timerCalc, aryRes );
		}
	} else
	if( typeSpot == XGAME::xSPOT_JEWEL ) {
		auto pSpot = SafeCast<XSpotJewel*>( pBaseSpot );
		XBREAK( pSpot == nullptr );
		auto pProp = pSpot->GetpProp();
		float income = (pProp->GetProduce() / 60.f) * sec;
		if( pSpot->IsEnemy() )
			income *= 0.1f;
		pBaseSpot->DoStackToLocal( income, true );
		std::vector<xRES_NUM> aryRes;
		aryRes.push_back( xRES_NUM(xRES_JEWEL, income) );
		// 메시지 주기당 생산량
		if( SCENE_WORLD ) {
			SCENE_WORLD->OnProduceResAtSpot( pBaseSpot, timerCalc, aryRes );
		}
	} else
	if( typeSpot == XGAME::xSPOT_MANDRAKE ) {
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		XBREAK( pSpot == nullptr );
		auto pProp = pSpot->GetpProp();
		float income = ( pProp->GetProduce() / 60.f ) * sec;
		if( pSpot->IsEnemy() )
			income *= 0.1f;
		pBaseSpot->DoStackToLocal( income, true );
		std::vector<xRES_NUM> aryRes;
		aryRes.push_back( xRES_NUM( xRES_MANDRAKE, income ) );
		// 메시지 주기당 생산량
		if( SCENE_WORLD ) {
			SCENE_WORLD->OnProduceResAtSpot( pBaseSpot, timerCalc, aryRes );
		}
	}
//	sGetpWorld()->SetbUpdate( TRUE );
}

DWORD XGame::OnDelegateGetMaxExp( const XFLevel *pLevel, int level, DWORD param1, DWORD param2 ) const
{
	return EXP_TABLE_USER->GetMaxExp( level );
}

int XGame::OnDelegateGetMaxLevel( const XFLevel *pLevel, DWORD param1, DWORD param2 ) const
{ 
	return EXP_TABLE_USER->GetMaxLevel();
}


void XGame::AddBrilliant( ID idWnd, float scale )
{
	if( FindBilliant(idWnd) == nullptr ) {
		xBrilliant br;
		br.idWnd = idWnd;
		br.scale = scale;
		m_listBrilliant.Add( br );
	}
}
void XGame::DelBrilliant( ID idWnd )
{
	XLIST_LOOP_MANUAL( m_listBrilliant, xBrilliant, itor, pBr ) {
		if( (*pBr).idWnd == idWnd )
		{
			XLIST_DEL( m_listBrilliant, itor );
			break;
		} else
			++itor;
	} END_LOOP;
}
XGame::xBrilliant* XGame::FindBilliant( ID idWnd )
{
	XLIST_LOOP_REF( m_listBrilliant, xBrilliant, refBr ) {
		if( refBr.idWnd == idWnd )
			return &refBr;
	} END_LOOP;
	return nullptr;
}

void XGame::FrameMoveBrilliant( float dt )
{
	if( m_psoBrilliant )
		m_psoBrilliant->FrameMove( dt );
}

void XGame::DrawBrilliant( xBrilliant *pBr, XWnd *pWnd )
{
	if( m_psoBrilliant == nullptr )
		return;
	if( pBr == nullptr )
		return;
	XE::VEC2 vCenter = pWnd->GetPosFinal() + pWnd->GetSizeFinal() / 2.f;
//	XE::VEC2 vCenter(320,160);
	m_psoBrilliant->SetScale( pBr->scale );
	m_psoBrilliant->Draw( vCenter );
}

void XGame::DelegateBeforeDraw( XWnd *pWnd )
{
	if( !pWnd->GetbShow() )		// 버튼이 감추기로 되어있으면 효과도 안그림
		return;
	auto pBr = GAME->FindBilliant( pWnd->getid() );
	if( pBr )
		GAME->DrawBrilliant( pBr, pWnd );
}

void XGame::Update()
{
	XEContent::Update();	
}

void XGame::sPushMasterMessage(_tstring& str)
{
	s_masterMessages.push_back(str);
	if (GAME)
		GAME->MasterMessageStart();
}

void XGame::MasterMessageStart()
{
	if( s_masterMessages.size() == 1 )
		m_masterMessgeTimer.Set(0.1f);		// 메시지가 하나밖에 안쌓여있으면 즉시표시
	else
		m_masterMessgeTimer.Set( 5 );
}

void XGame::RecvDuplicateLogout()
{
	XConnector::sGet()->StopFSM();
	GAMESVR_SOCKET->DoDisconnect();
	// ok누르면 앱 종료.
	auto pWnd = new XWndPopupDuplicateAcc();
	Add(pWnd);
}

/****************************************************************
* @brief
*****************************************************************/
int XGame::OnClickClearAcc( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE( "OnClickClearAcc" );
	//
// 	char cUUID[ 48 ];
// 	XE::GetUUID( cUUID, 48 );
// 	LOGIN_INFO.SetUUIDLogin( cUUID );
	LOGIN_INFO.ClearLoginInfo();
	LOGIN_INFO.Save();
	
	XSYSTEM::DeleteFile( XE::MakeDocFullPath( "", "login.txt" ) );
	OnExitApp( NULL, 0, 0 );

	return 1;
}

/****************************************************************
* @brief XWndStoragyItemElem로부터 이벤트를 받음.
*****************************************************************/
int XGame::OnClickItemTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE("OnClickItemTooltip");
	// 151.f, 198.f
	ID idItem = p1;
	XBREAK( idItem == 0 );
	auto pProp = PROP_ITEM->GetpProp( idItem );
	if( XBREAK( pProp == nullptr ) )
		return 1;
	std::string strLayout;
	if( pProp->IsSoul() )
		strLayout = "hero_tooltip";
	else
		strLayout = "item_tooltip";
	auto pPopup = new XGameWndAlert( _T( "layout_item.xml" ), strLayout.c_str(), true );
	pPopup->SetType(  XWnd::xOK );
	pPopup->SetstrFrameImg(_T("popup02.png") );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pButt = pPopup->AddButtonDirectly( "butt.create.item", _T( "아이템 생성" ), FONT_NANUM, FONT_SIZE_DEFAULT, BUTT_SMALL, nullptr );
		if( pButt ) {
			pButt->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickCreateItemFromTooltip, idItem );
		}
	}
#endif // _CHEAT
	if( pProp->IsSoul() )
		XGAME::UpdateHeroTooltip( pProp->strIdHero, pPopup, 0 );
	else
		XGAME::UpdateItemTooltip( pProp, pPopup, 0 );
//	pPopup->SetLayout( _T( "layout_item.xml" ) );
	AddWndTop( pPopup );
	return 1;
}

int XGame::OnClickSkillTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE("OnClickSkillTooltip");
	ID idSkill = p1;
	XBREAK( idSkill == 0 );
	auto pPopup = new XGameWndAlert( _T( "layout_item.xml" ), "skill_tooltip", true );
	pPopup->SetType(  XWnd::xOK );
	pPopup->SetstrFrameImg(_T("popup02.png") );
	XGAME::UpdateSkillTooltip( idSkill, pPopup );
	AddWndTop( pPopup );
	return 1;
}

int XGame::OnClickHeroTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE( "OnClickHeroTooltip" );
	ID idHero = p1;
	XBREAK( idHero == 0 );
	auto pPopup = new XGameWndAlert( _T( "layout_item.xml" ), "hero_tooltip", true );
	pPopup->SetType( XWnd::xOK );
	pPopup->SetstrFrameImg( _T( "popup02.png" ) );
	XGAME::UpdateHeroTooltip( idHero, pPopup, 0 );
	AddWndTop( pPopup );
	return 1;
}

void XGame::OnClickTooltip( XWnd* pWnd )
{
	const auto vPos = pWnd->GetPosFinal();
	auto datTooltip = pWnd->GetdatTooltip();
	//
	auto pPopup = new XWndTooltip( datTooltip.m_xml, datTooltip.m_strNode, _T("popup_tooltip.png") );
	if( datTooltip.m_Length > 0 )
		pPopup->SetLineLength( datTooltip.m_Length );
	if( datTooltip.m_idText != 0xffffffff && datTooltip.m_idText > 0 )
		pPopup->SetText( XTEXT(datTooltip.m_idText) );
	pPopup->SetAutoSize();
	Add( pPopup );
	pPopup->AutoPosByWinSrc( pWnd );
	auto vEnd = pWnd->GetPosFinal();
	vEnd.y += pWnd->GetSizeFinal().h * 0.5f;
	pPopup->SetDirection( vEnd );
// 	auto pPopup = new XGameWndAlert( datTooltip.m_xmlClick, datTooltip.m_strNodeClick );
// 	pPopup->SetType( XWnd::xOK );
// 	pPopup->SetstrFrameImg( _T( "popup_tooltip.png" ) );
// 	Add( pPopup );
// 	auto pWndText = xGET_TEXT_CTRL( pPopup, "__text.tooltip" );
// 	if( pWndText ) {
// 		pWndText->SetLineLength( 150.f );
// 		pWndText->SetText( XTEXT(2113) );
// 		pPopup->SetAutoSize();
// 	}
// 	const auto vPosSrc = pWnd->GetPosFinal();
// 	const auto sizePopup = pPopup->GetSizeFinal();
// 	auto vLT = vPosSrc;
// 	vLT.y -= sizePopup.h + 32.f;
// 	pPopup->SetSizePopup( sizePopup * XE::VEC2(1.f, 1.f) );
// 	pPopup->SetPosLocal( vLT );

}

/****************************************************************
* @brief 
*****************************************************************/
int XGame::OnClickCreateItemFromTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _CHEAT
	XTRACE("OnClickCreateItemFromTooltip");
	//
	ID idItem = (ID)p1;
	auto pProp = PROP_ITEM->GetpProp( idItem );
	if( pProp )
	{
		GAMESVR_SOCKET->SendReqCheatCreateItem( this, pProp, 5 );
		pWnd->GetpParent()->SetbDestroy( TRUE );
	}
#endif // _CHEAT
	return 1;
}


/****************************************************************
* @brief 
*****************************************************************/
int XGame::OnGotoAppStore( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE("OnGotoAppStore");
	//
#ifdef _VER_ANDROID
	//	JniHelper::LoadMarketURL("market://details?id=com.nhn.android.search");		// 예제. 잘됨
#ifdef _SOFTNYX
	JniHelper::LoadMarketURL( "http://store.softnyx.com/app/detail.aspx?seq=3237" );			// 현재 테스트라 안됩니다
#else
	JniHelper::LoadMarketURL( "market://details?id=com.gemtree.caribe" );			// 현재 테스트라 안됩니다
#endif
#endif
// #ifdef _VER_ANDROID
// 	//	JniHelper::LoadMarketURL("market://details?id=com.nhn.android.search");		// 예제. 잘됨
// 	JniHelper::LoadMarketURL( "market://details?id=com.gemtree.caribe" );			// 현재 테스트라 안됩니다
// #endif
	
	return 1;
}

#ifdef _CHEAT
bool XGame::ToggleXuzhuMode() 
{
//	s_bXuzhuMode = !s_bXuzhuMode;
// 	ACCOUNT->m_bXuzhuMode = !ACCOUNT->m_bXuzhuMode;
// 	GAMESVR_SOCKET->SendCheat( this, 10, (ACCOUNT->m_bXuzhuMode)? 1 : 0 );
// 	CONSOLE( "xuzhu mode:%s", ( ( ACCOUNT->m_bXuzhuMode ) ? _T( "on" ) : _T( "off" ) ) );
// 	SetbUpdate( true );
// 	XAPP->XClientMain::SaveCheat();
// 
// 	return ACCOUNT->m_bXuzhuMode;
	return true;
}

void XGame::WriteGoldLog( const char *cFormat, ... )
{
	char cBuff[ 1024 ];
	va_list vl;
	va_start( vl, cFormat );
	vsprintf_s( cBuff, cFormat, vl );
	va_end( vl );
	std::string strName = "_";
	strName += SZ2C( ACCOUNT->GetstrName() );
	strName += ".txt";
//	XE::MakeFilenameFromTime( &strName, "" );
	FILE *fp = nullptr;
	fopen_s( &fp, strName.c_str(), "a" );
	if( fp ) {
		std::string strLog = cBuff;
		strLog += "\n";
		fputs( strLog.c_str(), fp );
		fclose( fp );
	}
}

#endif // _CHEAT

void XGame::OnCreateFailedSocket()
{
	auto pAlert = XWND_ALERT( "%s", _T("network socket create failed!"));
	if( pAlert )
		pAlert->SetEvent( XWM_OK, this, &XGame::OnExitApp );

}

void XGame::OnTimeout2( DWORD idPacket, ID idSubPacket )
{
	CONSOLE("OnTimeout2: %d", idPacket );
#ifndef _DEBUG
	XWND_ALERT("%s %d", _T("no response:"), idPacket);
#endif // not _DEBUG
#ifdef AUTO_CONNECT
	if( LOGINSVR_SOCKET )
		LOGINSVR_SOCKET->DoDisconnect();
	if( GAMESVR_SOCKET )
		GAMESVR_SOCKET->DoDisconnect();
	XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
#endif // AUTO_CONNECT
}

void XGame::OnOnline()
{
	if( ACCOUNT )
		ACCOUNT->ResetAPTimer();
}

/**
 @brief 알림용 녹색점을 생성한다.
*/
void XGame::SetGreenAlert( XWnd *pParent, bool bShow, const XE::VEC2& vLocal, float scale )
{
	if( pParent == nullptr )
		return;
	auto pImgGreen = pParent->Find( "img.green" );
	if( bShow )	{
		if( pImgGreen == nullptr )	{
			pImgGreen = new XWndImage( PATH_UI( "ui_green.png" ), vLocal );
			pImgGreen->SetstrIdentifier( "img.green" );
			pImgGreen->SetScaleLocal( scale );
			pParent->Add( pImgGreen );
//			SOUNDMNG->OpenPlaySoundOneSec( 26 );
		}
		pImgGreen->SetbShow( TRUE );
	} else
	if( pImgGreen )
		pImgGreen->SetbShow( FALSE );
}

void XGame::DelGuild()	//길드 삭제(탈퇴, 추방)
{
	if (XAccount::sGetPlayer() == nullptr)
		return;
	SAFE_DELETE(m_pGuild);
	XAccount::sGetPlayer()->ClearGuild();
// 	XAccount::sGetPlayer()->SetGuildIndex(0);
// 	XAccount::sGetPlayer()->SetGuildgrade(XGAME::xGGL_NONE);
}
void XGame::SetpGuild(XGuild* pGuild)
{
	if (XAccount::sGetPlayer() == nullptr)
		return;
	XAccount::sGetPlayer()->ClearGuild();
	XAccount::sGetPlayer()->ClearlistGuildJoinReq();
	SAFE_DELETE(m_pGuild);	//기존 길드가 남아있을지 모르니 삭제
	m_pGuild = pGuild;
	XAccount::sGetPlayer()->SetGuildIndex( pGuild->GetGuildIndex() );
	const auto grade = pGuild->GetGradeByMember( XAccount::sGetPlayer()->GetidAccount() );
	XAccount::sGetPlayer()->SetGuildgrade( grade );
// 	pGuild->get
//	XAccount::sGetPlayer()->SetGuildgrade( )
}

void XGame::OnReload()
{
	SetActive( true );
	ClearidsAllow();
	XEContent::OnReload();
	SAFE_DELETE( TEXT_TBL );
	LoadTextTable();
	DestroySeq();
	XPropHelp::sGet()->DestroyAll();
	XPropHelp::sGet()->Load( _T( "propHelp.xml" ) );
	if( GetpScene() )
		GetpScene()->OnReload();
// 	XParticleMng::sGet()->DestroyAll();
	XPropParticle::sGet()->Load( _T( "particles/particles.xml" ) );
	GAME->DestroyForReload();
	GAME->Reload();
//	SPRMNG->Reload();
}

/**
 @brief 시퀀스객체를 생성한다.
 @return 시퀀스객체를 성공적으로 생성했으면 true를 리턴한다
*/
bool XGame::DoCutScene( const char* cidsSeq )
{
#ifdef _XSINGLE
	return true;
#endif // _XSINGLE
#ifdef _CHEAT
	if( !XAPP->m_bViewCutScene )
		return true;
#endif // _CHEAT
// 	if( IsPlayingSeq() )	// 퀘스트창에서 Go를 하면 현재 튜토리얼진행중에 다시 들어올수 있어서 이렇게 못함.
// 		return true;
	// 이미 한건 다시 하지 않는다.
	bool bAlready = ACCOUNT->IsCompletedSeq( std::string(cidsSeq) );
	if( bAlready )
		return false;
	auto spProp = XPropHelp::sGet()->GetspProp( cidsSeq );
	if( XBREAK( spProp == nullptr ) )
		return false;
	std::string idsWnd = "help.seq." + spProp->m_strIdentifier;
	auto pExist = Find( idsWnd );
	if( pExist == nullptr ) {
		XWnd* pRoot = GetpRootSeq();
		if( pRoot == nullptr ) {
			pRoot = new XWnd();
			pRoot->SetbTouchable( FALSE );
			pRoot->SetstrIdentifier( "root.seq" );
//			AddidsAllowWnd( pButt->GetstrIdentifier() );
			Add( pRoot );
		}
		DestroyTutorialStopButton();
		auto pSeq = new XSeq( spProp );
		pSeq->SetstrIdentifier( idsWnd );
		pRoot->Add( pSeq );
		// 튜토 중단버튼
		auto pButt = new XWndButtonString( spProp->m_vStopPos, XTEXT(16), 20.f, _T("common_butt_small.png") );
		pButt->SetstrIdentifier( "butt.tutor.stop" );
		pButt->SetEvent( XWM_CLICKED, this, &XGame::OnClickStopTutorial );
		Add( pButt );
#ifdef _CHEAT
		m_pCurrSeq = pSeq;
#endif // _CHEAT
		XTRACE("DoCutScene:%s", C2SZ(cidsSeq) );
		std::string idsSeqWnd = pSeq->GetstrIdentifier();
		auto pExist = m_listSeq.FindpIf( [idsSeqWnd]( std::string& elem )->bool {
			return elem == idsSeqWnd;
		} );
		if( XASSERT( pExist == nullptr ) )
			m_listSeq.Add( idsSeqWnd );
	}
	return true;
}

void XGame::DestroyTutorialStopButton()
{
	DestroyWndByIdentifier( "butt.tutor.stop" );
}
std::string XGame::GetidsSeqWnd( const std::string& idsSeq )
{
	char cBuff[1024];
	sprintf_s( cBuff, "help.seq.%s", idsSeq.c_str() );
	return std::string(cBuff);
}

void XGame::DestroySeq()
{
	for( auto& idsSeq : m_listSeq ) {
		XSeq *pSeq = static_cast<XSeq*>( Find( idsSeq ) );
		if( pSeq )
			pSeq->SetDestroyForce();
//		DestroyWndByIdentifier( idsSeq );
	}
	m_listSeq.clear();
}

/*
* @brief 
*/
int XGame::OnClickUnlockMenu( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE("OnClickUnlockMenu");
	//
	auto bitUnlock = (XGAME::xtMenus)p1;
	DispatchEvent( XGAME::xAE_UNLOCK_MENU, (DWORD)bitUnlock );
	return 1;
}
/**
 @brief 컷씬/튜토리얼 이벤트가 넘어오면 해당 시퀀스를 호출한다.
*/
bool XGame::DispatchEvent( XGAME::xtActionEvent event, 
													DWORD dwParam, 
													const std::string& strParam,
													DWORD dwParam2 )
{
	if( ACCOUNT == nullptr )
		return false;
//	return;
//	AXLOGXN( "DispatchEvent start" );
	const std::string idsSeqLast = (ACCOUNT)? ACCOUNT->GetidsLastSeq() : "";
	PropSeqPtr spPropSeq;
	std::vector<PropSeqPtr> ary;
	XPropHelp::sGet()->GetPropToAry( &ary );
	for( auto& _spProp : ary ) {
		XPropSeq *sp = _spProp.get();
		XBREAK( sp == nullptr );
		if( sp /*&& !idsSeqLast.empty()*/ ) {
//		AXLOGXN( "DispatchEvent loop:%s", sp->m_strIdentifier.c_str() );
		if( sp->m_SeqEvent == event )
			if( ( !idsSeqLast.empty() && sp->m_idsPrev == idsSeqLast ) || sp->m_idsPrev.empty() )
			if( ( dwParam != DEFAULT && sp->m_dwEventParam == dwParam ) || sp->m_dwEventParam == 0 )
				if( ( dwParam2 != DEFAULT && sp->m_dwEventParam2 == dwParam2 ) || sp->m_dwEventParam2 == 0 )
					if( ( !sp->m_strEventParam.empty() && sp->m_strEventParam == strParam ) || sp->m_strEventParam.empty() )
						if( (!sp->m_idsCompletedSeq.empty() && ACCOUNT->IsCompletedSeq(sp->m_idsCompletedSeq)) || sp->m_idsCompletedSeq.empty() )
							if( ( !sp->m_idsFinishededQuest.empty() && ACCOUNT->IsFinishedQuest( C2SZ(sp->m_idsFinishededQuest) ) ) || sp->m_idsFinishededQuest.empty() ) {
								if( !ACCOUNT->IsCompletedSeq( sp->m_strIdentifier ) ) {
//									AXLOGXN( "DispatchEvent finded" );
									spPropSeq = _spProp;
									break;
								} else {
									XTRACE("%s:컷씬을 찾았으나 이미 실행한 컷씬이어서 실행하지 않음.", C2SZ(sp->m_strIdentifier.c_str()) );
								}
							}
		}
	}
	if( spPropSeq ) {
//		AXLOGXN("DoCutScene:%s", spPropSeq->m_strIdentifier.c_str() );
		DoCutScene( spPropSeq->m_strIdentifier );
//		AXLOGXN( "DoCutScene: end" );
		return true;
	}
//	AXLOGXN( "DispatchEvent end" );
	return false;
} // DispatchEvent
/**
 @brief 모든 시퀀스객체들의 모든 명령객체에 이벤트를 전달한다.
*/
void XGame::DispatchEventToSeq( xHelp::xtEvent event )
{
	for( auto& idsSeq : m_listSeq ) {
		auto pSeq = static_cast<XSeq*>( Find( idsSeq ) );
		if( pSeq ) {
			pSeq->DispatchEvent( event );
		}
	}
}

void XGame::DelSeqWnd( const std::string& idsSeq )
{
	std::string idsWnd = GetidsSeqWnd( idsSeq );
	for( auto itor = m_listSeq.begin(); itor != m_listSeq.end(); ) {
		std::string& idsElem = (*itor);
		if( idsElem == idsWnd )
			m_listSeq.erase( itor++ );
		else
			++itor;
	}
}

void XGame::OnEndSeq( XSeq *pSeq ) 
{
	DestroyWndByIdentifier( "butt.tutor.stop" );
	GAMESVR_SOCKET->SendReqEndSeq( this, pSeq->GetidsSeq() );
	// dialog.으로 시작하는 윈도우 모두 삭제,.
	for( int i = 0; i < 10; ++i ) {
		auto pWndPrev = GAME->FindWithPrefix( "__dialog." );		// 이 접두사로 시작하는 윈도우를 찾음.
		if( pWndPrev ) {
			pWndPrev->SetbDestroy( true );
			// 더이상 안나올때까지 반복.
		} else {
			break;
		}
	}
}
/**
 @brief 서버로부터 endSeq가 성공함.
*/
void XGame::OnRecvEndSeq( const std::string& idsEndSeq )
{
//	m_idsSeqLast = idsEndSeq;
	auto pWndCutScene = static_cast<XWndDialogMode*>( Find("wnd.curtain") );
	if( pWndCutScene )
		pWndCutScene->DoDecovering();
	DelSeqWnd( idsEndSeq );
	if( !DispatchEvent( xAE_END_SEQ, 0, idsEndSeq ) ) {
		// 컷씬 실행된게 없으면 완전종료로 인식.
		GetpScene()->OnEndCutScene( idsEndSeq );
	}
}

void XGame::OnClickWnd( XWnd *pWnd )
{
	for( auto& idsSeq : m_listSeq ) {
		auto pSeq = static_cast<XSeq*>( Find( idsSeq ) );
		if( pSeq ) {
			pSeq->OnClickWnd( pWnd->GetstrIdentifier() );
		}
	}
}

/**
 @brief 
*/
int XGame::OnClickStopTutorial( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XTRACE("OnClickStopTutorial");
	//
	if( m_listSeq.size() > 0 ) {
		auto pIdsSeq = m_listSeq.GetpFirst();
		auto pSeq = static_cast<XSeq*>( Find( (*pIdsSeq) ) );
		if( pSeq ) {
			OnEndSeq( pSeq );
			pSeq->SetbDestroy( true );
			SetActive( true );
#ifdef _CHEAT
			m_pCurrSeq = nullptr;
#endif // _CHEAT
		}
	}
	return 1;
}

void XGame::SetActive( bool bFlag )
{
	if( bFlag == false )
		XTRACE( "scene locked" );
	else
		XTRACE( "scene unlocked" );
	if( GetpScene() )
		GetpScene()->SetActive( bFlag );
}

XGameWndAlert* XGame::WndAlert( LPCTSTR format, ... )
{
	// 여기있는 경고창은 안뜨는상황이 없어야할거같아서 기존에 다른거 떠있으면 없애고 띄우게 함.
	XGameWndAlert *pAlert = nullptr;
	TCHAR _szBuff[ 1024 ];
	va_list vl;
	va_start( vl, format );
	_vstprintf_s( _szBuff, format, vl );
	if( !IsPlayingSeq() ) {
		pAlert = new XGameWndAlert( _szBuff );
		if( GetpScene() )
			GetpScene()->Add( pAlert );	// 씬이있으면 씬에다ㅏ 생성한다.
		else
			Add( pAlert );
	}
	return pAlert;
}

void XGame::OnFinishAppearPopup( XWndView *pView )
{
	XBREAK( pView == nullptr );
	if( !pView->GetstrIdentifier().empty() )
		DispatchEvent( xAE_POPUP, pView->GetstrIdentifier() );
}

void XGame::OnClosePopup( const std::string& idsWnd )
{
	if( !idsWnd.empty() )
		DispatchEvent( xAE_CLOSE_POPUP, idsWnd );
	if( GetpScene() )
		GetpScene()->OnCloseDialog( idsWnd );
}
/**
 @brief 모든 팝업의 배경을 어둡게 칠하기 위한 draw함수
*/
void XGame::OnBeforeDrawByPopup( XWndView *pView )
{
	if( pView->GetbModal() ) {
		if( m_psfcBgPopup == nullptr ) {
			m_psfcBgPopup = IMAGE_MNG->Load( PATH_UI("bg_popup_dark.png") );
			++m_cntRefBgDark;
		}
		m_psfcBgPopup->Draw( 0, 0 );
	}
}
void XGame::ReleaseBgDark() {
	-- m_cntRefBgDark;
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgPopup );
}


/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/07/05 15:55
*****************************************************************/
class XWndDarkGradation : public XWndDarkBg
{
public:
	XWndDarkGradation();
	virtual ~XWndDarkGradation() { Destroy(); }
	// get/setter
	// public member
// private member
private:
// private method
	XSurface *m_psfcDark = nullptr;
private:
	void Init() {}
	void Destroy();
	void Draw() override;
}; // class XWndDarkGradation
////////////////////////////////////////////////////////////////
XWndDarkGradation::XWndDarkGradation()
{
	Init();
	m_psfcDark = IMAGE_MNG->Load( PATH_UI("bg_popup_dark.png"));
}

void XWndDarkGradation::Destroy()
{
//	IMAGE_MNG->DoForceDestroy( m_psfcDark );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcDark );
}
void XWndDarkGradation::Draw()
{
	if( m_psfcDark ) {
		m_psfcDark->SetfAlpha( GetAlpha() );
		m_psfcDark->Draw( 0, 0 );
	}
}

XWndDarkBg* XGame::CreateDarkBg()
{
	return new XWndDarkGradation();
}

/**
 @brief 페북 프로필 이미지가 도착함.
*/
void XGame::OnRecvProfileImageByFacebook( const std::string& strcFbUserId, DWORD* pImg, int w, int h, int bpp )
{
//	CONSOLE("XGame::OnRecvProfileImageByFacebook: fbid=%s pImg=0x%08x size(%dx%d) bpp=%d", C2SZ(strcFbUserId), (DWORD)pImg, w, h, bpp );
	if( pImg == nullptr ) {
		// 페북 프로필사진을 못읽어온경우.
		GRAPHICS->LoadImg( PATH_UI("fb_empty.png"), &w, &h, &pImg );
	}
	XE::xImage imgInfo;
	imgInfo.Set( pImg, w, h );	// return할때 자동파괴됨

	if( XFacebook::sGet()->GetstrcFbUserId() == strcFbUserId ) {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
		m_psfcProfile = IMAGE_MNG->CreateSurface( strcFbUserId, imgInfo );
	}
	if( ACCOUNT && sGetpWorld() ) {
		XVector<XSpotCastle*> ary;
		sGetpWorld()->GetSpotsToAry( &ary, XGAME::xSPOT_CASTLE );
		for( auto pSpot : ary ) {
			if( pSpot->GetstrcFbUserId() == strcFbUserId )
				pSpot->cbOnRecvProfileImage( imgInfo );
		}
	}
	// 콜백이 돌아올때까지 1초마다 계속 확인하지 않고 실제 돌아왔을때만 업데이트 하도록 바꿈.
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( true );
//	CONSOLE( "XGame::OnRecvProfileImageByFacebook 2" );
}

void XGame::DoRequestPlayerProfileImg( XSPAcc spAcc )
{
	if( spAcc->GetstrFbUserId().empty() )
		return;
	XFacebook::sGet()->SetstrcFbUserId( SZ2C( spAcc->GetstrFbUserId() ) );
	XFacebook::sGet()->GetRequestProfileImage( spAcc->GetstrFbUserId() );
}
/**
 @brief 모든 스팟의 프로필이미지를 다시 요청한다.
*/
void XGame::DoRequestProfileImgAllSpot()
{
	if( ACCOUNT && sGetpWorld() ) {
		XVector<XSpotCastle*> ary;
		sGetpWorld()->GetSpotsToAry( &ary, XGAME::xSPOT_CASTLE );
		for( auto pSpot : ary ) {
			if( !pSpot->GetstrcFbUserId().empty() )
				pSpot->DoRequestProfileImage();
		}
	}
}

/**
 @brief XFacebook::DoRequestCertification의 응답콜백
*/
void XGame::DelegateFacebookCertResult( const char *cUserId, const char *cUsername, DWORD param )
{
#pragma message("인증 실패시 처리코드 넣을것.")
	if( XE::IsEmpty(cUserId) ) {
		XWND_ALERT("%s", XTEXT(18) );
		return;
	}
	XFacebook::sGet()->SetstrcFbUserId( cUserId );
	_tstring strid = C2SZ( cUserId );
	_tstring strUserName = C2SZ( cUsername );
	_tstring strName = _T("");		// 실제 이름
	CONSOLE( "XGame::DelegateFacebookCertResult: userid=%s, username=%s, param=%d", strid.c_str(), strUserName.c_str(), param );
	// 페북아이디 얻으면 곧바로 프로필사진 요청한다.
	XFacebook::sGet()->GetRequestProfileImage( cUserId );
	//
	const auto strFbUsername = std::string();
	GAMESVR_SOCKET->SendReqRegisterFacebook( GAME, XFacebook::sGet()->GetstrcFbUserId(), strFbUsername );
}

void XGame::OnDelegateEnterEditBox( XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld )
{
	const _tstring strInput = szString;
	// 이전 입력값과 다를때만 처리한다.
	if( strInput != strOld ) {
		if( pWndEdit->GetstrIdentifier() == "edit.hello.msg" ) {
			ACCOUNT->SetstrHello( strInput );
			GAMESVR_SOCKET->SendReqChangeHelloMsg( this, strInput );
		}
	}
}

/**
 @brief idWndChild가 파괴된 직후에 이벤트 발생
*/
void XGame::OnDestroyAfterByWnd( ID idWndChild, const std::string& idsChild )
{
	if( idsChild.compare(0,6,"scene.") == 0 ) {
		// 이벤트 등록된 윈도우중에 말머리가 scene.으로 시작하는건 일단 다 씬으로 간주함.
		auto pRootScene = GetpRootScene();
		auto pImg = new XWndImage( PATH_UI("bg_patch.png"), XE::xPF_RGB565, 0, 0 );
		if( pImg ) {
			pImg->SetstrIdentifier( "__bg.loading");
			if( XASSERT(pRootScene) ) {
				DestroyWndByIdentifier( "__bg.loading" );
				Insert( pRootScene->GetstrIdentifier(), pImg );
			}
		}
	}
}

/**
 @brief pLoadedScene씬의 파일로딩이 모두 끝나면 호출된다.
*/
int XGame::OnFinishLoadedByScene( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnFinishLoadedByScene");
	// 로딩화면 삭제.
	DestroyWndByIdentifier( "__bg.loading" )	;
	return 1;
}

/**
 @brief 버튼에 느낌표를 붙임.
 1: 느낌표
 2: 연구중
 3: 퍼지는원(천천히)
 4: 퍼지는원 빠르게
*/
void XGame::AddAlert( XWnd *pButt, ID idAct, const XE::VEC2& _vPos ) const
{
	if( pButt->Find( "icon.alert" ) == nullptr ) {
		// 연구중 알림.
		auto vPos = XE::VEC2(79,7);
		if( _vPos.IsValid() )
			vPos = _vPos;
		auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), idAct, vPos );
		pMark->SetstrIdentifier( "icon.alert" );
		pButt->Add( pMark );
		auto pWndSound = new XWndPlaySound( 26, true );
		pMark->Add( pWndSound );
//		SOUNDMNG->OpenPlaySoundOneSec( 26 );
	}
}
void XGame::DelAlert( XWnd *pButt, ID idAct ) const
{
	auto pWndSpr = SafeCast2<XWndSprObj*>( pButt->Find( "icon.alert" ) ) ;
	if( pWndSpr ) {
		if( pWndSpr->GetidAct() == idAct ) {
			pWndSpr->SetbDestroy( true );
		}
	}
}

/**
 @brief 
*/
int XGame::OnClickCtrlWnd( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickCtrlWnd");
	//
// 	if( pWnd ) {
// 		if( pWnd->GetstrIdentifier() == "text.ap" ) {
// 			XWND_ALERT( "%s", _T("행동력은 오프라인동안에만 차오릅니다."));
// 		}
// 	}
	return 1;
}

/**
 @brief 디바이스의 콜백스레드로부터 결과값을 받음.
*/
bool XGame::OnReceiveCallbackData( const xnReceiverCallback::xData& data )
{
	bool bOk = XEContent::OnReceiveCallbackData( data );
	if( !bOk )
		return false;
	if( data.m_strIds == "gcm_regid" ) {
		const auto& strRegId = data.m_aryParam[0];
		XBREAK( strRegId.empty() );	
		if( XASSERT(strRegId.length() < 256) ) {		// 쓰레기값이 오는거 같아서 일단 막음.
			XTRACE( "received gcm regid=%s", strRegId.c_str() );
			XLOGXNA( "received gcm regid=%s", strRegId.c_str() );
			s_strGcmRegid = strRegId;		// 일단 메모리에 올려둠.
		}
//		GAMESVR_SOCKET->SendReqGCMResistId( this, strRegId );
		// bOk = false;		// 처리가 실패해서 다시 하길 원한다면..
	} else
	if( data.m_strIds == "iap_unconsumed_item" ) {
		// 소진안된 아이템 발견. 서버에 보내서 미처리된 거래인지 확인.
		const std::string& strcJsonReceipt = data.m_aryParam[0];
		const std::string& strcSignature = data.m_aryParam[1];
		const auto& strcIdsProduct = data.m_aryParam[2];		// google store에선 사용하지 않음.
		CONSOLE("소진안된 아이템 발견. 서버에 보내서 미처리된 거래인지 확인.: %s", strcJsonReceipt.c_str() );
#ifdef _NEW_INAPP
		if( !GAMESVR_SOCKET->SendBuyCashItemByIAP( this, strcJsonReceipt, strcSignature, strcIdsProduct ) ) {
			// 네트웍이 끊어져 보내지 못함. 연결될때까지 다시 시도.
			bOk = false;
		}
#endif // _NEW_INAPP
	} else
	if( data.m_strIds == "iap_buy_request" ) {
		// IAP시스템에 구매요청한 결과가 돌아옴.
		const auto& strcJsonReceipt = data.m_aryParam[0];
		const auto& strcSignature = data.m_aryParam[1];
		const auto& strcIdsProduct = data.m_aryParam[2];		// google store에선 사용하지 않음.
#ifdef _NEW_INAPP
		do {
			if( strcJsonReceipt.empty() ) {
				bOk = true;			// 결제시스템에 문제가 있으므로 다시 시도하지 않음.
				XWND_ALERT("%s%d", XTEXT(2288), XGAME::xIAPP_ERROR_CRITICAL );
				break;
			}
			if( strcSignature.empty() ) {
				bOk = true;			// 결제시스템에 문제가 있으므로 다시 시도하지 않음.
				XWND_ALERT("%s%d", XTEXT(2288), XGAME::xIAPP_ERROR_CRITICAL );
				break;
			}
#ifdef _SOFTNIX
			XBREAK( strcIdsProduct.empty() );
#endif // _SOFTNIX
			if( !GAMESVR_SOCKET->SendBuyCashItemByIAP( this, strcJsonReceipt, strcSignature, strcIdsProduct ) ) {
				// 네트웍이 끊어져 보내지 못함. 연결될때까지 다시 시도.
				bOk = false;
			}
		} while (0);
#endif // _NEW_INAPP
	}
	return bOk;	// false면 콜백을 다시 받는다.
}

std::string XGame::GetGCMRegId() const
{
#if defined(_VER_ANDROID)
	return CppToJava("gcm_regid");
#elif defined(_VER_IOS)
#elif defined(WIN32)
	return "";
#else
#error "unknown platform"
#endif 
}

/**
 @brief 오프닝 텍스트 출력
*/
void XGame::OnEventEtc( const xSpr::xEvent& event )
{
	static std::string s_strIdsPrev;
	const ID idText = event.m_idEvent;
	auto pScene = GetpScene();
	// 현재 플레이되고 있는 텍스트를 찾는다.
	if( !s_strIdsPrev.empty() ) {
		auto pWndOld = SafeCast2<XWndTextString*>( pScene->Find( s_strIdsPrev ) );
		if( pWndOld ) {
			auto pCompFade = SafeCast<XECompFade*>( pWndOld->GetcompMngByAlpha().FindComponentByIds( "fade" ) );
			pCompFade->OnFinishFade( [pWndOld]() {
				pWndOld->SetbDestroy( true );
			} );
			pCompFade->DoStartFadeOut( 0.5f );		// 기존텍스트는 페이드아웃 시작
		}
	}
	// 새 텍스트 생성
	if( idText > 0 ) {	// 0값은 더미 키로 쓴다.
		s_strIdsPrev = XE::Format( "text.opening.%d", event.m_pKey->GetidLocalInLayer() );
		if( Find( s_strIdsPrev ) == nullptr )  {
			const XE::VEC2 vPos = (XE::GetGameSize() * 0.5f) + XE::VEC2(0, 90);
			auto pWndText = new XWndTextString( XTEXT(idText), FONT_NANUM, 30.f, XCOLOR_WHITE );
			pWndText->SetstrIdentifier( s_strIdsPrev );
			const auto sizeText = XE::VEC2(440, 40);
			pWndText->SetSizeLocal( sizeText );
			pWndText->SetPosLocal( vPos );
			auto& compMng = pWndText->GetcompMngByAlpha();
			// fade 컴포넌트 추가(FadeIn). 페이드인 시작
			auto pComp = new XECompFade( "alpha" );
			pComp->SetstrIdentifier( "fade" );
			pComp->DoStartFadeIn( 0.5f );
			compMng.AddComponent( pComp );
	//		pWndText->GetcompMngByAlpha().AddComponent( new XECompTime( "life", ))
			if( pScene ) {
				pScene->Add( pWndText );
				pWndText->AutoLayoutHCenter();
			}
		}
	}
}

int XGame::OnClickTopGemUI( XWnd*, DWORD, DWORD )
{
	XGameWndAlert* pPopup = XWND_ALERT_YESNO( "popup.goto.gemshop", "%s", _T("젬상점으로 이동") );
	if( pPopup ) {
		auto pButtYes = pPopup->GetButtYes();
		if( pButtYes ) {
			pButtYes->SetEvent( XWM_CLICKED, this, &XGame::OnClickGotoGemShop );
		}
	}
	return 1;
}

int XGame::OnClickGotoGemShop( XWnd*, DWORD, DWORD )
{
	if( GetpScene() )
		GetpScene()->DoExit( XGAME::xSC_SHOP );
	return 1;
}

bool XGame::ToggleBGM()
{
	static float s_backup = 1.f;
	const auto vol = SOUNDMNG->GetBGMMasterVolume();
	if( vol != 0 )
		s_backup = vol;
	if( vol != 0 ) {
		SOUNDMNG->SetBGMMasterVolume( 0.f );
//		SOUNDMNG->SetbMuteBGM( true );
		m_pOption->SetbMusic( false );
	} else {
		SOUNDMNG->SetBGMMasterVolume( s_backup );
//		SOUNDMNG->SetbMuteBGM( false );
		m_pOption->SetbMusic( true );
	}
	m_pOption->Save();
	return m_pOption->GetbMusic();
}

bool XGame::ToggleSound()
{
	static float s_backup = 1.f;
	const auto vol = SOUNDMNG->GetSoundMasterVolume();
	if( vol != 0 )
		s_backup = vol;
	if( vol != 0 ) {
		SOUNDMNG->SetSoundMasterVolume( 0.f );
		m_pOption->SetbSound( false );
	} else {
		SOUNDMNG->SetSoundMasterVolume( s_backup );
		m_pOption->SetbSound( true );
	}
	m_pOption->Save();
	return m_pOption->GetbSound();
}

/****************************************************************
* @brief
*****************************************************************/
int XGame::OnClickFillAPByCash( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "%s", __TFUNC__ );
	//
	GAMESVR_SOCKET->SendReqFillAP( this );

	return 1;
}

/**
 @brief 
*/
#ifdef _CHEAT
int XGame::OnClickDebugShowLog( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnClickDebugShowLog" );
	char cFullpath[ 1024 ];
	XE::GetLogFilename( cFullpath );
	//
	if( p1 == 1 ) {
		XSYSTEM::RemoveFile( cFullpath );
		return 1;
	} else 
	if( p1 == 2 ) {
		// log toggle
		XE::SetbAllowLog( !XE::GetbAllowLog() );
		auto pButt = dynamic_cast<XWndButtonDebug*>( pWnd );
		if( pButt ) {
			pButt->SetbCheck( XE::GetbAllowLog() );
			pButt->SetbUpdate( true );
		}
	} else 
	if( p1 == 0 ) {
		//
		auto pPopup = new XWndPopup( XE::VEC2(455,340), _T("popup01.png") );
		pPopup->AddButtonOk(_T("ok"), _T("common_butt_small.png") );
		Add( pPopup );
		auto pScrlView = new XWndScrollView( XE::VEC2(7,9), XE::VEC2(451,302) );
	//	pScrlView->SetScrollVertOnly();
		pPopup->Add( pScrlView );
		TCHAR szLine[ 1024 ];
		TCHAR szBuff[ 1024 ];
		XE::VEC2 v;
		int cntLine = 0;
		float sizeFont = 18.f;
		int cntBlock = 0;
		FILE *fp;
		_tfopen_s( &fp, C2SZ(cFullpath), _T( "rt" ) );
		if( fp == nullptr ) {
			XWND_ALERT("file not found[%s]", C2SZ(cFullpath) );
			return 1;
		}
		long offset = ftell( fp );
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );			// file size
		fseek( fp, offset, SEEK_SET );
		while( !feof( fp ) ) {
			szBuff[ 0 ] = 0;
			if( cntBlock == 0 ) {
	#ifdef _VER_ANDROID
				_tcscat_s( szBuff, XFORMAT("size=%d",size) );
	#endif // _VER_ANDROID
			}
			while(1) {
				if( feof( fp ) )
					break;
	#ifdef WIN32
				fgetws( szLine, 1024, fp );		// utf8
	#else
				fgets( szLine, 1024, fp );		// utf8
	#endif
				if( _tcslen( szBuff ) + _tcslen(szLine) > 1000 )
					break;
				_tcscat_s( szBuff, szLine );
	#ifdef _VER_ANDROID
	// 				_tcscat_s( szBuff, _T("\r\n"));
	#endif // _VER_ANDROID
				++cntLine;
			}
			XSYSTEM::strReplace( szBuff, _T( '\r' ), _T( ' ' ) );
			auto pText = new XWndTextString( v,
																			szBuff,
																			FONT_NANUM,
																			sizeFont );
	//			pText->SetLineLength( 100 );
			pText->SetLineLength( pScrlView->GetSizeLocal().w - 10 );
			XBREAK( pText == NULL );
			pScrlView->Add( pText );
			v.y += pText->GetSizeNoTransLayout().h;
			++cntBlock;
		}
		fclose( fp );
		pScrlView->SetScrollViewAutoSize();
		auto vSizeView = pScrlView->GetsizeScroll();
		vSizeView.h = vSizeView.h * 1.01f;
		pScrlView->SetViewSize( vSizeView );
		pScrlView->SetFocusView( 0, vSizeView.h );	
	} // p1 == 0
	return 1;
}
#endif // _CHEAT
