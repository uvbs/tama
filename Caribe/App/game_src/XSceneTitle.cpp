#include "StdAfx.h"
#include "XSceneTitle.h"
// #ifdef WIN32
// #include "DlgSelectConnect.h"
// #endif // WIN32
#include "XGame.h"
#ifdef _XSINGLE
#include "XSceneBattle.h"
#include "XSpot.h"
#include "XLegion.h"
#endif
#include "client/XAppMain.h"
#include "XSockGameSvr.h"
#include "XConnector.h"
#include "XLoginInfo.h"
#include "XSockLoginSvr.h"
#include "XSceneTech.h"
#include "XFramework/Game/XEComponentMng.h"
#include "XFramework/Game/XEComponents.h"
#include "XResObj.h"
#include "_Wnd2/XWndScrollView.h"
#include "_Wnd2/XWndButton.h"
#include "XResMng.h"
#include "XSystem.h"

#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneTitle *SCENE_TITLE = nullptr;

xSec XSceneTitle::s_secLoginLockStart = 0;   // 로그인락 시작시간
xSec XSceneTitle::s_secLoginLockTotal = 0;   // 로그인락 총 대기시간

void XSceneTitle::Destroy() 
{	
	GAME->DestroyWndByIdentifier("butt.init.acc");
	GAME->DestroyWndByIdentifier("butt.ipchange");
	GAME->DestroyWndByIdentifier("butt.logout");
	GAME->DestroyWndByIdentifier("butt.appstore");
	ClearLoginLock();
	XBREAK( SCENE_TITLE == nullptr );
	XBREAK( SCENE_TITLE != this );
	SCENE_TITLE = nullptr;
}

XSceneTitle::XSceneTitle(XGame *pGame)
	: XSceneBase(pGame, XGAME::xSC_TITLE)
	, m_Layout(_T("layout_title.xml"))
{
	XBREAK(SCENE_TITLE != nullptr);
	SCENE_TITLE = this;
	Init();
	// 월드맵 스크롤 뷰
// 	auto pImg = new XWndImage( XE::MakePath( DIR_UI, PROP_WORLD->GetstrImg() )
// 														, XE::xPF_RGB565, 0, 0 );
	auto pImg = new XWndImage( PATH_UI("map.png"), XE::xPF_RGB565, 0, 0 );
	auto pView = new XWndScrollView( XE::VEC2( 0 ), XE::GetGameSize() );
	pView->SetstrIdentifier( "scroll.view" );
	pView->SetViewSize( pImg->GetSizeLocal() );
	Add( pView );
	m_pScrollView = pView;
	pView->Add( pImg );
	m_sizeMap = pImg->GetSizeLocal();
	m_vFocus = m_sizeMap * 0.5f;
	m_deltaScroll = XE::VEC2(0.2f);
	//
	m_Layout.CreateLayout("title", this);
// 	XWnd *pWnd = Find("spr.title");
// 	if (pWnd)
// 		pWnd->SetEvent(XWM_CLICKED, this, &XSceneTitle::OnClickTitle);
	SetButtHander( this, "butt.touch", &XSceneTitle::OnClickTitle );
	_tstring strVer = _T(__DATE__);
	strVer += _T(" ");
	strVer += _T(__TIME__);
#if defined(_SOFTNYX)
	strVer += _T(" For Softnyx");
#elif defined(_VER_ANDROID)
	strVer += _T(" For Google");
#elif defined(_VER_IOS)
	strVer += _T(" For iOS");
#elif defined(WIN32)
	strVer += _T(" For win32");
#else
#error "unknown platform"
#endif 

	XGame::s_strVerBuild = strVer;
	xSET_TEXT(this, "text.ver.build",
		XFORMAT("build: %s", XGame::s_strVerBuild.c_str()));
	SetbUpdate(true);

}

void XSceneTitle::Create(void)
{
	XEBaseScene::Create();
	//
#ifdef _VER_ANDROID
// 	XFacebook::sGet()->DoRequestCertification( this );
//	JniHelper::GetUserProfileImage( "100000786878603" );	// 
//	JniHelper::GetUserProfileImage( "100000786878903" );	// 
#endif 

#if !defined(_XSINGLE)
	// 로긴서버의 ip와 포트를 지정
	const _tstring strIp = C2SZ(CONNECT_INI.m_cIP);
	XTRACE("ip/port:%s(%d)", strIp.c_str(), CONNECT_INI.m_Port );
	LOGINSVR_SOCKET->SetIpAndPort( CONNECT_INI.m_cIP, CONNECT_INI.m_Port );
	//
	if( s_secLoginLockStart > 0 ) {
		// 공격당해서 로그인이 잠시 막힘.
		OnRecvLoginLockForBattle( s_secLoginLockStart, s_secLoginLockTotal );
	} else {
		OnInitLogin();
	}
#endif
}

/**
@brief 로그인에 필요한 초기화를 한다.
*/
void XSceneTitle::OnInitLogin()
{
	// login.txt가 있는가.
//	XBREAK( m_strLogin.empty() );
//	if( LOGIN_INFO.Load( m_strLogin.c_str() ) )	{
//		if (LOGIN_INFO.IsLastLoginUUID())
	if( !GAME->GetbFirst() ) {
		XASSERT( LOGIN_INFO.GetstrUUID().empty() == false );
		XConnector::sGet()->SetConnectParamByUUID( LOGIN_INFO.GetstrUUID() );
		XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
	}	else	{
		// 계정을 이미 가지고 있습니까?
//		m_bFirst = true;
		DoPopupFirstUser();
	}
}

/**
 @brief 최초 로그인유저를 위한 창
 이미 계정을 갖고 있음?
*/
void XSceneTitle::DoPopupFirstUser()
{
	// 이미 계정이 있습니까?
	DestroyWndByIdentifier("popup.first.user");
	auto pPopup = new XWndLoginFirst( this );
	pPopup->SetstrIdentifier( "popup.first.user" );
	Add( pPopup );
// 	XWndLoginFirst::xSetButtHander( this, pScene, "butt.no", &XSceneTitle::OnNoHaveNoAccount );
// 	XWndLoginFirst::xSetButtHander( this, pScene, "butt.yes", &XSceneTitle::OnYesHaveAccount );
// 	XWndLoginFirst::xSetButtHander( this, pScene, "butt.facebook", &XSceneTitle::OnClickLoginFacebook );
}



void XSceneTitle::Update()
{
#if defined(_CHEAT)
	if (XAPP->m_bDebugMode)	{
		XE::VEC2 v(100, 0);
		XE::VEC2 sizeButt( 48, 48 );
		if (GAME->Find("butt.init.acc") == nullptr) {
			auto pButt = new XWndButtonDebug(v, sizeButt, _T("계정초기화"));
			pButt->SetstrIdentifier("butt.init.acc");
			pButt->SetEvent(XWM_CLICKED, GAME, &XGame::OnClickClearAcc);
			GAME->Add(pButt);
			v.x += sizeButt.w;
		}
		if (GAME->Find("butt.ipchange") == nullptr) {
			auto pButt = new XWndButtonDebug(v, sizeButt, _T("ip교체"));
			pButt->SetstrIdentifier("butt.ipchange");
			pButt->SetEvent(XWM_CLICKED, GAME, &XSceneTitle::OnClickIpChange);
			GAME->Add(pButt);
			v.x += sizeButt.w;
		}
		if( GAME->Find( "butt.logout" ) == nullptr ) {
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "fb logout" ) );
			pButt->SetstrIdentifier( "butt.logout" );
			pButt->SetEvent( XWM_CLICKED, GAME, &XSceneTitle::OnClickFaceboopkLogout );
			GAME->Add( pButt );
			v.x += sizeButt.w;
		}
		if( GAME->Find( "butt.appstore" ) == nullptr ) {
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "goto store" ) );
			pButt->SetstrIdentifier( "butt.appstore" );
			pButt->SetEvent( XWM_CLICKED, GAME, &XGame::OnGotoAppStore );
			GAME->Add( pButt );
			v.x += sizeButt.w;
		}
		if( GAME->Find( "butt.log" ) == nullptr ) {
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "log" ) );
			pButt->SetstrIdentifier( "butt.log" );
			pButt->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickDebugShowLog, 0 );
			GAME->Add( pButt );
			v.x += sizeButt.w;
		}
		v.x = XE::GetGameWidth() - sizeButt.w;
		v.y = 128.f;
#ifdef _SOFTNYX
		if( Find( "softnyx.butt.login") == nullptr ) {
			v.y = 128.f;
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "Softnyx Login" ) );
			pButt->SetstrIdentifier( "softnyx.butt.login" );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnSoftnyxLogin, 0 );
			Add( pButt );
			v.y += sizeButt.h;
		}
		if( Find( "softnyx.butt.buy") == nullptr ) {
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "Softnyx Buy" ) );
			pButt->SetstrIdentifier( "softnyx.butt.buy" );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnSoftnyxLogin, 1 );
			Add( pButt );
			v.y += sizeButt.h;
		}
		if( Find( "softnyx.butt.logout") == nullptr ) {
			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "Softnyx Logout" ) );
			pButt->SetstrIdentifier( "softnyx.butt.logout" );
			pButt->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnSoftnyxLogin, 2 );
			Add( pButt );
			v.y += sizeButt.h;
	}
#else
#endif // _SOFTNYX
// 		if( Find( "butt.appstore" ) == nullptr ) {
// 			auto pButt = new XWndButtonDebug( v, sizeButt, _T( "앱스토어로 이동" ) );
// 			pButt->SetstrIdentifier( "butt.appstore" );
// 			pButt->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnGotoAppStore );
// 			Add( pButt );
// 			v.y += sizeButt.h;
// 		}
// 		if( Find("butt.test") == nullptr ) {
// 			auto pButt = new XWndButtonDebug( 57, 138, 64,64, _T("Test") );
// 			pButt->SetstrIdentifier( "butt.test" );
// 			pButt->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickTest );
// 			Add( pButt );
// 		}
	} else {
		GAME->DestroyWndByIdentifier("butt.init.acc");
		GAME->DestroyWndByIdentifier("butt.ipchange");
		GAME->DestroyWndByIdentifier("butt.logout");
		GAME->DestroyWndByIdentifier("butt.appstore");
		GAME->DestroyWndByIdentifier("butt.log");
	}
#endif
}

void XSceneTitle::OnAutoUpdate()
{
	_tstring strRemain;

	// 로그인락이 걸렸을때 팝업에 남은시간을 띄워준3다.
	if( m_timerLoginLock.IsOn() ) {
		auto pWnd = Find("popup.login.lock");
		if( pWnd ) {
			XSceneTech::sGetRemainResearchTime( &strRemain, 
																					m_timerLoginLock.GetsecStart(),
																					s_secLoginLockTotal );
			xSET_TEXT( pWnd, "text.info", XFORMAT( "%s\n%s:%s", XTEXT( 2114 ), XTEXT( 80131 ), strRemain.c_str()) );
		}
	}
}

int XSceneTitle::Process( float dt ) 
{ 
	if( !GAME->GetbFirst() ) {
		if( m_timerOpenning.IsOff() )
			m_timerOpenning.Set( 10.f );
		if( m_timerOpenning.IsOver() ) {
			if( Find( "spr.opening" ) == nullptr ) {
				xSET_SHOW( this, "wnd.title", false );		// 타이틀 루트 레이아웃을 가림
				m_pScrollView->SetbShow( false );
				auto pWndSpr = new XWndSprObj( _T( "opening.spr" ), 1, XE::GetGameSize() * 0.5f + XE::VEC2(0, -20) );
				pWndSpr->SetstrIdentifier( "spr.opening" );
				pWndSpr->SetpDelegateBySprObj( GAME );
				pWndSpr->SetScaleLocal( 1.5f );
				pWndSpr->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnClickTitle );
				Add( pWndSpr );
			}
		}
	}

	// 시간다될때까지 종료신호가 안오면 다시한번 접속을 시도해본다.
#if !defined(_XSINGLE)
	if( m_timerLoginLock.IsOver( s_secLoginLockTotal ) ) {
		ClearLoginLock();
		OnInitLogin();
	}
#endif 
	if( m_pScrollView ) {
		m_pScrollView->SetFocusView( m_vFocus );
		m_vFocus += m_deltaScroll;
		const auto vViewSize = XE::GetGameSize();
		if( m_vFocus.x + vViewSize.w * 0.5f >= m_sizeMap.w  )
			m_deltaScroll.x = -m_deltaScroll.x;
		if( m_vFocus.x - vViewSize.w * 0.5f <= 0 )
			m_deltaScroll.x = -m_deltaScroll.x;
		if( m_vFocus.y + vViewSize.h * 0.5f >= m_sizeMap.h )
			m_deltaScroll.y = -m_deltaScroll.y;
		if( m_vFocus.y - vViewSize.h * 0.5f <= 0 )
			m_deltaScroll.y = -m_deltaScroll.y;
	}
	return XEBaseScene::Process( dt );
}

//
void XSceneTitle::Draw(void)
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneTitle::OnLButtonDown(float lx, float ly)
{
	XEBaseScene::OnLButtonDown(lx, ly);
}
void XSceneTitle::OnLButtonUp(float lx, float ly) {
	XEBaseScene::OnLButtonUp(lx, ly);
}
void XSceneTitle::OnMouseMove(float lx, float ly) {
	XEBaseScene::OnMouseMove(lx, ly);
}

int XSceneTitle::OnClickTitle(XWnd* pWnd, DWORD p1, DWORD p2)
{
#ifdef _XSINGLE
	XSPAcc spAcc = std::make_shared<XAccount>(1); //new XAccount(1);
	XAccount::sSetPlayer( spAcc );
	spAcc->SetpDelegateLevel(GAME);
	spAcc->CreateFakeAccount();
	XGAME::xBattleStart bs;
	bs.m_Level = 1;
	bs.m_strName = _T("babarian");
	bs.m_spLegion[0] = ACCOUNT->GetCurrLegion();
	// 최초 NPC군단생성
	int lvLegion = 50;
	XGAME::xLegionParam info;
	info.x_gradeLegion = XGAME::xGL_NORMAL;
	info.unit = (XGAME::xtUnit)XAPP->m_nDebugRespawnUnit;
	info.bRandom = XAPP->m_bDebugRespawnNumRandom != FALSE;
	auto pLegion = XLegion::sCreateLegionForNPC( lvLegion, 0, info );
	bs.m_spLegion[1] = LegionPtr(pLegion);
	bs.m_Defense = 0;
	bs.m_idEnemy = 0;
	bs.m_typeBattle = XGAME::xBT_NORMAL;
	XSceneBattle::sSetBattleStart( bs );

	DoExit(XGAME::xSC_INGAME );
#else
	if (ACCOUNT)	{
		//길드 정보를 못받았다 다시 요청함
		if (ACCOUNT->GetGuildIndex() != 0 && GAME->GetpGuild() == nullptr)		{
			GAMESVR_SOCKET->SendReqGuildInfo(GAME);
//			return 1;
		}
		DoExit( XGAME::xSC_WORLD );
	} else {
		CONSOLE("아직 계정정보를 받지 못함.");
	}
#endif 
	return 1;
}

void XSceneTitle::OnConnectedGameSvr(void)
{
	//길드에 가입되어있는 계정이니 길드 정보를 요청한다
	if( ACCOUNT->GetGuildIndex() != 0 && GAME->GetpGuild() == nullptr ) {
		GAMESVR_SOCKET->SendReqGuildInfo( GAME );
	}

	auto pText = xGET_IMAGE_CTRL( this, "img.touch" );
	if( pText ) {
		pText->SetbShow( true );
		pText->GetcompMngByAlpha().AddComponentWave( "alpha", nullptr, 0.f, 1.f, 0.5f, 1.f );
	}
}

void XSceneTitle::OnConnectedLoginSvr()
{
	//닉네임 등록, 로그인 성공 했으니 팝업 꺼준다
	DestroyWndByIdentifier("popup.first.user");
	// 닉넴입력창이 떠있었으면 꺼준다.
	DestroyWndByIdentifier("popup.nickname");
	// ID/PW입력창이 있었으면 꺼준다.
	DestroyWndByIdentifier("popup.login");
	// 게임 업데이트 정보가 있으면 창을 띄운다.
	ProcPopupGameUpdateList();

}

/**
 @brief 게임컨텐츠 업데이트 리스트창의 처리
*/
void XSceneTitle::ProcPopupGameUpdateList()
{
	do {
		XBaseRes *pFile;
		XGET_RESOURCE( pFile, _T( "update.txt" ) );
		if( !pFile )
			return;
		// 이미 떠있었으면 삭제한다.
		DestroyWndByIdentifier("popup.update");
			const int buff = 1024;
			char szBuff[buff] = { 0, };
			//szBuff = (char*)malloc(sizeof(char)* file.Size());
			pFile->Read(szBuff, buff);
			XLOG("update.txt file path : %s", pFile->GetszResName());
			LPCTSTR szUpdate = nullptr;
			//XPLATFORM_PATH(szText);
	#ifdef WIN32
			szUpdate = ConvertUTF8ToUTF16(szBuff);
	#else
			szUpdate = szBuff;
	#endif
			const _tstring strUpdate = szUpdate;
			auto pPopup = new XWndPopupTextScroll(strUpdate.c_str());
			pPopup->SetEnableNcEvent( FALSE );
			pPopup->SetstrIdentifier("popup.update");
			Add(pPopup);
			LPCTSTR szFileName = pFile->GetszResName();
			pFile->Close();
			const char *cFileName = nullptr;
	#ifdef WIN32
			cFileName = Convert_TCHAR_To_char(szFileName);
	#else
			cFileName = szFileName;
	#endif
			std::string strPath = XE::GetPathWorkA();
			strPath += "update.txt";
			XLOG("remove result = %d", XSYSTEM::RemoveFile(cFileName));
	} while(0);

}

void XSceneTitle::RecvDuplicateNickName( XGAME::xtConnectParam typeLogin, const _tstring& strNick )
{
	ClearEditBoxByNickname();

	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_fail", nullptr);
	pPopup->SetEnableNcEvent( FALSE );
	pPopup->SetstrIdentifier("popup.duplicate.nick");
	pPopup->SetbModal(TRUE);
	Add(pPopup);

	SetButtHander(pPopup, "butt.popup.fail.ok", &XSceneTitle::OnClickDuplicateNickOk, (DWORD)typeLogin );
}

/**
 @brief 
*/
int XSceneTitle::OnClickDuplicateNickOk( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickDuplicateNickOk");
	//
	DestroyWndByIdentifier("popup.duplicate.nick");
	auto typeLogin = (XGAME::xtConnectParam)p1;
	DoPopupInputNickname( typeLogin );
	return 1;
}

/**
 @brief 닉네임 입력창 띄움.
*/
XWndPopup* XSceneTitle::DoPopupInputNickname( const XGAME::xtConnectParam paramLogin )
{
	DestroyWndByIdentifier( "popup.nickname" );
	auto pPopup = new XWndPopup( m_Layout.GetpLayout(), "popup_nickname", nullptr );
	pPopup->SetEnableNcEvent( FALSE );
	pPopup->SetstrIdentifier( "popup.nickname" );
	pPopup->SetbModal( TRUE );
	Add( pPopup );
	XWnd *pImg = pPopup->Find( "img.input.box" );
	if( XASSERT(pImg) )
		pImg->SetEvent( XWM_CLICKED, this, &XSceneTitle::OnClickNicknameBox );
	auto typeLogin = paramLogin;
	if( typeLogin == XGAME::xCP_LOGIN_NEW_ACC )
		typeLogin = XGAME::xCP_LOGIN_UUID;
	xSetButtHander( this, this, "butt.ok", &XSceneTitle::OnClickFinishInputNickname, typeLogin );
	xSetButtHander( this, this, "butt.close", &XSceneTitle::OnClickFinishInputNickname, 0 );
	return pPopup;
}

void XSceneTitle::ClearEditBoxByNickname()
{
	auto pPopup = SafeCast2<XWndPopup*>( Find("popup.nickname") );
	if( pPopup ) {
		auto pText = dynamic_cast<XWndTextString*>( Find( "text.nickname" ) );
		if( pText ) {
			pText->SetText( _T("") );
		}
	}
}

void XSceneTitle::OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld)
{
	xSET_TEXT(this, "text.nickname", szString);
	pWndEdit->SetbDestroy(TRUE);
}

int XSceneTitle::OnClosePopup(XWnd* pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	return 1;
}

int XSceneTitle::OnClickNicknameBox(XWnd* pWnd, DWORD p1, DWORD p2)
{
	XWnd *pBox = Find("img.input.box");
	if (pBox)	{
		XWndEdit *pEdit = new XWndEdit( XE::VEC2(10,4), XE::VEC2(262,37), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
//		XWndEdit *pEdit = new XWndEdit(10, 4, 262, 37, GAME->GetpfdSystem(), XCOLOR_WHITE, 0);
		pEdit->SetbShow(FALSE);
		pEdit->SetpDelegate(this);
		pEdit->OnLButtonUp(pEdit->GetPosFinal().x, pEdit->GetPosFinal().y);
		pBox->Add(pEdit);
	}
	return 1;
}

/**
@brief 닉네임 입력후 새 계정생성
*/
int XSceneTitle::OnClickFinishInputNickname(XWnd* pWnd, DWORD p1, DWORD p2)
{
	if( p1 == 0 ) {
		DestroyWndByIdentifier("popup.nickname");
		DoPopupFirstUser();
		return 1;
	}

	auto typeLogin = (XGAME::xtConnectParam)p1;
	// 입력창에서 닉네임값을 읽어온다.
	auto pText = dynamic_cast<XWndTextString*>( Find("text.nickname"));
	if (pText) {
		_tstring strNickname = pText->GetszString();
		if ( !strNickname.empty() ) {
			if( typeLogin == XGAME::xCP_LOGIN_UUID ) {
				XConnector::sGet()->SetConnectParamByNewAcc( strNickname, typeLogin, std::string() );
				XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
			} else
			if( typeLogin == XGAME::xCP_LOGIN_FACEBOOK ) {
				const auto strFbUserId = XFacebook::sGet()->GetstrcFbUserId();
				XConnector::sGet()->SetConnectParamByNewAcc( strNickname, typeLogin, strFbUserId );
				XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
			} else {
				XBREAK(1);
			}
//			LOGINSVR_SOCKET->SendReqCreateAccount( GAME, strNickname );
// 				XConnector::sGet()->SetstrNickname(pText->GetszString());
// 			XConnector::sGet()->ReconnectLoginSvr();
		} else {
			// 입력안하고 확익만 눌렀을때
			if( Find( "popup.login" ) == nullptr )
				DoPopupFirstUser();		// 다시 첫 팝업으로 돌아감.
//  			RecvDuplicateNickName( typeLogin, _T("") );
		}
	}
	// 닉넴 입력창 끔.
	DestroyWndByIdentifier("popup.nickname");
	return 1;
}

/**
 @brief uuid로 계정을 검색했으나 없다고 나옴.
*/
void XSceneTitle::RecvNoAccount(XGAME::xtConnectParam param)
{
	XBREAK( param == XGAME::xCP_NONE );
	if( param == XGAME::xCP_LOGIN_UUID )
		// 닉네임 입력창 띄움.
		DoPopupInputNickname( param );
	else
	if( param == XGAME::xCP_LOGIN_IDPW ) {
		// id를 못찾음.
		XWND_ALERT("%s", XTEXT(12) );		// 
		// 다시 ID/PW입력 받을 수 있도록.
	} else
	if( param == XGAME::xCP_LOGIN_FACEBOOK ) {
		// 닉네임 입력창 띄움.
		DoPopupInputNickname( param );
	} else {
		XBREAKF(1, "unknown login type:%d", param );
	}
}

// void XSceneTitle::RecvReconnectTry(void)
// {
// 	m_TimerReconn.Set(1.f);
// }

/****************************************************************
* @brief
*****************************************************************/
// int XSceneTitle::OnGotoAppStore(XWnd* pWnd, DWORD p1, DWORD p2)
// {
// 	CONSOLE("OnGotoAppStore");
// 	// 여기에 앱스토어 이동코드를 넣으시오
// 
// #ifdef _VER_ANDROID
// 	//	JniHelper::LoadMarketURL("market://details?id=com.nhn.android.search");		// 예제. 잘됨
// #ifdef _SOFTNYX
// 	JniHelper::LoadMarketURL( "http://store.softnyx.com/app/detail.aspx?seq=3237" );			// 현재 테스트라 안됩니다
// #else
// 	JniHelper::LoadMarketURL("market://details?id=com.gemtree.caribe");			// 현재 테스트라 안됩니다
// #endif
// #endif
// 
// 	return 1;
// }

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnClickIpChange(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickIpChange");
	//
#ifdef _CHEAT
	auto pAlert = XWND_ALERT( "ip 교체");
	auto pWndEdit = new XWndEdit( XE::VEC2( 0 ), XE::VEC2( 128, 32 ), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
//	auto pWndEdit = new XWndEdit(0, 0, 128, 32, BASE_FONT);
	pWndEdit->SetEvent(XWM_ENTER, this, &XSceneTitle::OnEnterIP);
	pWndEdit->SetszString(C2SZ(CONNECT_INI.m_cIP));
	pAlert->Add(pWndEdit);
#endif // _CHEAT

	return 1;
}

/**
 @brief 
*/
int XSceneTitle::OnClickFaceboopkLogout( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickFaceboopkLogout");
	//
#ifdef _CHEAT
#ifndef WIN32
	JniHelper::DoLogoutFacebook();
#endif
#endif // _CHEAT
	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnEnterIP(XWnd* pWnd, DWORD p1, DWORD p2)
{
	LPCTSTR szStr = (LPCTSTR)p2;
	CONSOLE("OnEnterIP");
	//
	// #ifndef WIN32
	XLOGXN("OnEnterIP:%s", szStr);
	strcpy_s(CONNECT_INI.m_cIP, SZ2C(szStr));
	CONNECT_INI.Save(_T("connect.ini"));
	// #endif

	return 1;
}

/****************************************************************
* @brief 이미 계정을 가지고 있음.
*****************************************************************/
int XSceneTitle::OnYesHaveAccount(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnYesHaveAccount");
	// 계정을 이미 가지고 있음? 창 없앰.
	DestroyWndByIdentifier( "popup.first.user" );
	// ID/pw입력창
	auto pPopup = new XWndPopup(_T("layout_login.xml"), "popup_login");
	pPopup->SetEnableNcEvent( FALSE );
	pPopup->SetstrIdentifier("popup.login");
	pPopup->SetbModal(TRUE);
	Add(pPopup);
	xSetButtHander(pPopup, this, "butt.login.ok", &XSceneTitle::OnClickLoginByIDPW);
	xSetButtHander(pPopup, this, "butt.close", &XSceneTitle::OnClickLoginCancel);

	return 1;
}

/****************************************************************
* @brief 가진 계정이 없음 => 닉네임입력 => 새 계정생성
*****************************************************************/
int XSceneTitle::OnNoHaveNoAccount(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnNoHaveNoAccount");
	// UUID로그인으로 시도
// 	if (LOGIN_INFO.GetstrUUID().empty())
// 		LOGIN_INFO.MakeUUIDLogin();
// 	LOGINSVR_SOCKET->SetdwConnectParam(XGAME::xCP_LOGIN_UUID);
// 	XConnector::sGet()->TryConnectLoginSvr(false);
	// 계정을 이미 가지고 있음? 창 없앰.
	DestroyWndByIdentifier("popup.first.user");
	// 닉네임 입력창 띄움
	DoPopupInputNickname( XGAME::xCP_LOGIN_UUID );
	return 1;
}

/**
 @brief 
*/
int XSceneTitle::OnClickLoginFacebook( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLoginFacebook");
	// 비동기로 페북 인증 요청
	XFacebook::sGet()->DoRequestCertification( this );	
	return 1;
}
/**
 @brief XFacebook::DoRequestCertification의 응답콜백
*/
void XSceneTitle::DelegateFacebookCertResult( const char *cUserId, const char *cUsername, DWORD param )
{
#pragma message("인증 실패시 처리코드 넣을것.")
	XFacebook::sGet()->SetstrcFbUserId( cUserId );
	_tstring strid = C2SZ( cUserId );
	_tstring strUserName = C2SZ( cUsername );
	_tstring strName = _T("");		// 실제 이름
	CONSOLE( "XSceneTitle::DelegateFacebookCertResult: userid=%s, username=%s, param=%d", strid.c_str(), strUserName.c_str(), param );
	// 페북아이디 얻으면 곧바로 프로필사진 요청한다.
	XFacebook::sGet()->GetRequestProfileImage( cUserId );
//	JniHelper::GetUserProfileImage( "100000786878603" );	// 
	// 서버에 보내 계정을 새로 만들거나 계정정보를 받는다.
//	XConnector::sGet()->SetConnectParamByFacebook( std::string(cUserId) );
	XParamObj paramObj;
	XConnector::sGet()->SetConnectParamByFacebook( std::string(cUserId) );
	XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
}



/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnClickEmailBox(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickEmailBox");
	//

	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnClickPasswordBox(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickPasswordBox");
	//
	return 1;
}

/****************************************************************
* @brief ID/PW를 입력하고 로그인을 시도.
*****************************************************************/
int XSceneTitle::OnClickLoginByIDPW(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickLogin");
	//
	auto pEditEmail = SafeCast<XWndEdit*>(Find("edit.email"));
	auto pEditPassword = SafeCast<XWndEdit*>(Find("edit.password"));
	if (pEditEmail && pEditPassword) {
		const _tstring strEmail = pEditEmail->GetszString();
		const _tstring strPassword = pEditPassword->GetszString();
		if (strEmail.empty() || strPassword.empty())
			return 1;
		const std::string cstrID = SZ2C( strEmail );
		const std::string cstrPW = SZ2C( strPassword );
		XConnector::sGet()->SetConnectParamByIDPW( cstrID, cstrPW );
		XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
	}
	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnClickLoginCancel(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickLoginCancel");
	// 로긴창 꺼주고.
	DestroyWndByIdentifier("popup.login");
	// 다시 계정을 가지고 있습니까? 팝업으로
	DoPopupFirstUser();
	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::RecvWrongPassword(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("RecvWrongPassword");
	//
	auto pAlert = XWND_ALERT( "%s", _T("패스워드가 틀렸습니다."));
	if (pAlert)
		pAlert->SetEvent(XWM_OK, this, &XSceneTitle::OnOkWrongPassword);
	auto pEditPassword = SafeCast<XWndEdit*>(Find("edit.password"));
	if (pEditPassword)
		pEditPassword->SetszString(_T(""));

	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnOkWrongPassword(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnOkWrongPassword");
	// 로그인창이 안떠있으면 로그인창 띄움.
	if (Find("popup.login") == nullptr)
		DoPopupFirstUser();
	return 1;
}

void XSceneTitle::RecvClosedSvr()
{
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_close_svr", nullptr);
	pPopup->SetEnableNcEvent( FALSE );
	pPopup->SetbModal(TRUE);
	Add(pPopup);

	xSET_BUTT_HANDLER(this, "butt.popup.close.ok", &XGame::OnExitApp);
}

int XSceneTitle::OnClickUpdatePopupOk(XWnd* pWnd, DWORD p1, DWORD p2)
{

	return OnClosePopup(pWnd, p1, p2);
}

/**
@brief 로그인시도중에 로그인락을 받음.
*/
void XSceneTitle::OnRecvLoginLockForBattle( xSec secStart, xSec secTotal )
{
	// 자동접속 프로세스 잠시 중단.
	XConnector::sGet()->StopFSM();
	// 현재 공격받고 있어서 로그인이 잠시 대기중이다.
	if( Find("popup.login.lock") == nullptr ) {
		auto pPopup = new XWndPopup( _T( "login_lock.xml" ), "popup_login_lock" );
		pPopup->SetEnableNcEvent( FALSE );
		pPopup->SetstrIdentifier( "popup.login.lock" );
		Add( pPopup );
		pPopup->SetbModal( TRUE );
	}
	m_timerLoginLock.DoStart( secStart );

	SetAutoUpdate( 0.1f );
}

/**
@brief 로그인락 상태가 해제됨.
*/
void XSceneTitle::OnRecvLoginLockFree()
{
	ClearLoginLock();
//   XConnector::sGet()->DoDisconnectLoginSvr();
	// 로그인 시작
	OnInitLogin();
}

/**
 @brief 로그인락 상태를 해제시킨다.
*/
void XSceneTitle::ClearLoginLock()
{
	s_secLoginLockStart = 0;
	s_secLoginLockTotal = 0;
	m_timerLoginLock.Off();
	ClearAutoUpdate();
	DestroyWndByIdentifier( "popup.login.lock" );
}

void XSceneTitle::OnSendCrashDump( int sizeDump )
{
	// 크래시덤프 파일을 보냈음을 알림
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		xSET_SHOW( this, "text.send.dump", sizeDump > 0 );
	}
#endif // _CHEAT
}

/**
 @brief 
*/
#ifdef _SOFTNYX
int XSceneTitle::OnSoftnyxLogin( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnSoftnyxLogin");
	//
#ifdef _VER_ANDROID
	if( p1 == 0 )
		JniHelper::SoftnyxLogin();
	else
	if( p1 == 1 ) 
		JniHelper::SoftnyxBuyItem( std::string("inapp_cash00002"), 100, std::string("dev_payload") );
	else
	if( p1 == 2 )
		JniHelper::SoftnyxLogout();

#endif // _VER_ANDROID
	return 1;
}
#endif // _SOFTNYX
