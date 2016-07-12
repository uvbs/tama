#include "StdAfx.h"
#include "XSceneTitle.h"
#ifdef WIN32
#include "DlgSelectConnect.h"
#endif // WIN32
#include "XGame.h"
#ifdef _XSINGLE
#include "XSceneBattle.h"
#include "XSpot.h"
#endif
#include "client/XAppMain.h"
#include "XSockGameSvr.h"
#include "XConnector.h"
#include "XLoginInfo.h"
#include "XSockLoginSvr.h"
#include "XSceneTech.h"

#ifdef _VER_ANDROID
#include "android/JniHelper.h"
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
	auto pImg = new XWndImage( XE::MakePath( DIR_UI, PROP_WORLD->GetstrImg() )
														, XE::xPF_RGB565, 0, 0 );
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
	XWnd *pWnd = Find("spr.title");
	if (pWnd)
		pWnd->SetEvent(XWM_CLICKED, this, &XSceneTitle::OnClickTitle);
	_tstring strVer = _T(__DATE__);
	strVer += _T(" ");
	strVer += _T(__TIME__);

	XGame::s_strVerBuild = strVer;
	xSET_TEXT(this, "text.ver.build",
		XFORMAT("build: %s", XGame::s_strVerBuild.c_str()));
#if defined(_DEBUG) || defined(_VER_ANDROID)
#ifdef _CHEAT
	if (XAPP->m_bDebugMode)
	{
		auto pButt = new XWndButtonDebug(XE::GetGameWidth() - 64.f, 128, 64, 64, _T("앱스토어로 이동"));
		Add(pButt);
		pButt->SetEvent(XWM_CLICKED, this, &XSceneTitle::OnGotoAppStore);
	}
#endif
#endif
//	xSET_SHOW( this, "img.title", false );
	SetbUpdate(true);
}

void XSceneTitle::Create(void)
{
	XEBaseScene::Create();

#ifdef _VER_ANDROID
	XFacebook::sSet( new XFacebook );
// 	XFacebook::sGet()->DoRequestCertification( this );
//	JniHelper::GetUserProfileImage( "100000786878603" );	// 
	JniHelper::GetUserProfileImage( "100000786878903" );	// 
#endif 
#if defined(WIN32) && !defined(_XSINGLE)
//  static bool bFlag = false;
//	if( bFlag == false ) {
	static int s_idxLogin = -1;
	if( s_idxLogin == -1 ) {
    CDlgSelectConnect dlg;
    if( dlg.DoModal() == IDOK ) {
      if( dlg.m_idxSelect >= 0 )
				s_idxLogin = dlg.m_idxSelect;
//         m_strLogin = XFORMAT("login%d.txt", dlg.m_idxSelect);
    }
//    bFlag = true;
  }
	XBREAK( s_idxLogin == -1 );
	if( s_idxLogin == 0 )
		m_strLogin = _T( "login.txt" );
	else
		m_strLogin = XFORMAT( "login%d.txt", s_idxLogin );
#else
	m_strLogin = _T( "login.txt" );
#endif // WIN32

#pragma message("============================= 코드삭제 ===========================================")
  // 소켓 객체 생성
#ifdef WIN32
  XConnector::sGet()->OnCreate();
  //
  if( s_secLoginLockStart > 0 ) {
    OnRecvLoginLockForBattle( s_secLoginLockStart, s_secLoginLockTotal );
  } else {
    OnInitLogin();
  }
#endif // WIN32
}

/**
@brief 로그인에 필요한 초기화를 한다.
*/
void XSceneTitle::OnInitLogin()
{
	// login.txt가 있는가.
	XBREAK( m_strLogin.empty() );
	if( LOGIN_INFO.Load( m_strLogin.c_str() ) )	{
		if (LOGIN_INFO.IsLastLoginUUID())
			XConnector::sGet()->DoLoginUUID();
		else
		if (LOGIN_INFO.IsLastLoginIDPW())
			XConnector::sGet()->DoLoginIDPW();
	}	else	{
		// 계정을 이미 가지고 있습니까?
		DoPopupSelectLogin();
	}
}

void XSceneTitle::DoPopupSelectLogin()
{
	auto pPopup = new XWndLoginFirst( this );
	pPopup->SetstrIdentifier( "alert.first" );
	Add( pPopup );
// 	auto pAlert = XWND_ALERT_YESNO( "alert.first", "%s", XTEXT(2116));
// 	if (pAlert) {
// #if defined(_CHEAT)
// 		if (XAPP->m_bDebugMode) {
// 			auto pButt = new XWndButtonDebug(170, 0, 64, 32, _T("ip교체"));
// 			pButt->SetstrIdentifier("butt.ipchange");
// 			pButt->SetEvent(XWM_CLICKED, GAME, &XSceneTitle::OnClickIpChange);
// 			pAlert->Add(pButt);
// 		}
// #endif
// 		pAlert->SetEvent(XWM_YES, this, &XSceneTitle::OnYesHaveAccount);
// 		pAlert->SetEvent(XWM_NO, this, &XSceneTitle::OnNoHaveNoAccount);
// 	}
}

void XSceneTitle::Update()
{
#if defined(_CHEAT)
	if (XAPP->m_bDebugMode)
	{
		if (Find("butt.init.acc") == nullptr) {
			auto pButt = new XWndButtonDebug(100, 0, 64, 32, _T("계정초기화"));
			pButt->SetstrIdentifier("butt.init.acc");
			pButt->SetEvent(XWM_CLICKED, GAME, &XGame::OnClickClearAcc);
			Add(pButt);
		}
		if (Find("butt.ipchange") == nullptr) {
			auto pButt = new XWndButtonDebug(170, 0, 64, 32, _T("ip교체"));
			pButt->SetstrIdentifier("butt.ipchange");
			pButt->SetEvent(XWM_CLICKED, GAME, &XSceneTitle::OnClickIpChange);
			Add(pButt);
		}
		if( Find("butt.test") == nullptr ) {
			auto pButt = new XWndButtonDebug( 57, 138, 64,64, _T("Test") );
			pButt->SetstrIdentifier( "butt.test" );
			pButt->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickTest );
			Add( pButt );
		}
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
	// 시간다될때까지 종료신호가 안오면 다시한번 접속을 시도해본다.
	if( m_timerLoginLock.IsOver( s_secLoginLockTotal ) ) {
		ClearLoginLock();
		OnInitLogin();
	}
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
	XAccount *pAcc = new XAccount(1);
	GAME->SetpAccount(pAcc);
	pAcc->SetpDelegateLevel(GAME);
	pAcc->CreateFakeAccount();
	XGAME::xBattleStart bs;
	bs.m_Level = 1;
	bs.m_strName = _T("babarian");
	bs.m_spLegion[0] = ACCOUNT->GetCurrLegion();
// 	auto spParam = std::shared_ptr<XGAME::xSPM_BATTLE>(new XGAME::xSPM_BATTLE);
// 	spParam->idSpot = 1;
// 	spParam->level = 1;
// 	spParam->strName = _T("babarian");
// 	spParam->spLegion[0] = ACCOUNT->GetCurrLegion();
	// 최초 NPC군단생성
	int lvLegion = 50;
	XGAME::xLegionParam info;
	info.x_gradeLegion = XGAME::xGL_NORMAL;
	info.unit = (XGAME::xtUnit)XAPP->m_nDebugRespawnUnit;
	info.bRandom = XAPP->m_bDebugRespawnNumRandom != FALSE;
 	auto pLegion = XLegion::sCreateLegionForNPC( lvLegion, 0, info );
// 	spParam->spLegion[1] = LegionPtr(pLegion);
// 	spParam->defense = 0;
// 	spParam->bvsNPC = TRUE;
	bs.m_spLegion[1] = LegionPtr(pLegion);
	bs.m_Defense = 0;
	bs.m_idEnemy = 0;
	bs.m_typeBattle = XGAME::xBT_NORMAL;
	XSceneBattle::sSetBattleStart( bs );

	DoExit(XGAME::xSC_INGAME );
#else
	if (GAME->GetpAccount())
	{
		//길드 정보를 못받았다 다시 요청함
		if (ACCOUNT->GetGuildIndex() != 0 && GAME->GetpGuild() == nullptr)
		{
			GAMESVR_SOCKET->SendReqGuildInfo(GAME);
//			return 1;
		}
		BOOL bSound = !ACCOUNT->GetbSound();
		SOUNDMNG->SetBGMMute(bSound);
		SOUNDMNG->SetSoundMute(bSound);
		// 		GAMESVR_SOCKET->SendCheat( GAME, 10, (XGame::s_bXuzhuMode)?1:0 );
		DoExit(XGAME::xSC_WORLD);
	}
#endif 
	return 1;
}

void XSceneTitle::OnConnectedGameSvr(void)
{
}

void XSceneTitle::OnConnectedLoginSvr()
{
	//닉네임 등록, 로그인 성공 했으니 팝업 꺼준다
	XWnd *pWnd = Find("img.popup.bg");
	if (pWnd && pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	DestroyWndByIdentifier("popup.login");
	if (!Find("popup.update")) {
		XBaseRes *pFile;
		XGET_RESOURCE(pFile, _T("update.txt"));
		if (pFile == NULL)
			return;
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
		_tstring strUpdate = szUpdate;
		XWndPopupTextScroll *pPopup = new XWndPopupTextScroll(strUpdate.c_str());
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
	}
	//길드에 가입되어있는 계정이니 길드 정보를 요청한다
	if (ACCOUNT->GetGuildIndex() != 0 && GAME->GetpGuild() == nullptr)
	{
		GAMESVR_SOCKET->SendReqGuildInfo(GAME);
	}
}

void XSceneTitle::RecvDuplicateNickName(void)
{
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_fail", nullptr);
	pPopup->SetbModal(TRUE);
	Add(pPopup);

	xSET_BUTT_HANDLER(this, "butt.popup.fail.ok", &XSceneTitle::OnClosePopup);
}

//void XSceneTitle::RecvNickNameChage(_tstring strName, int result)
//{
//	if (result == 1)
//	{
//		//ACCOUNT->SetstrName(strName.c_str());
//		BOOL bSound = !ACCOUNT->GetbSound();
//		SOUNDMNG->SetBGMMute(bSound);
//		SOUNDMNG->SetSoundMute(bSound);
//		DoExit(XGAME::xSC_WORLD);
//	}
//	else
//	{
//		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_fail", nullptr);
//		pPopup->SetbModal(TRUE);
//		Add(pPopup);
//
//		xSET_BUTT_HANDLER(this, "butt.popup.fail.ok", &XSceneTitle::OnClosePopup);
//	}
//}

void XSceneTitle::OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString)
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
	if (pBox)
	{
		XWndEdit *pEdit = new XWndEdit(10, 4, 262, 37, GAME->GetpfdSystem(), XCOLOR_WHITE, 0);
		pEdit->SetbShow(FALSE);
		pEdit->SetpDelegate(this);
		pEdit->OnLButtonUp(pEdit->GetPosFinal().x, pEdit->GetPosFinal().y);
		pBox->Add(pEdit);
	}
	return 1;
}

/**
@brief 닉네임 입력후 다시 접속시도.
*/
int XSceneTitle::OnCreateTryConnect(XWnd* pWnd, DWORD p1, DWORD p2)
{
	auto pText = dynamic_cast<XWndTextString*>(Find("text.nickname"));
	if (pText) {
		_tstring strString = pText->GetszString();
		if (!strString.empty()) {
			XConnector::sGet()->SetstrNickname(pText->GetszString());
			XConnector::sGet()->ReconnectLoginSvr();
		} else
			RecvDuplicateNickName();
	}
	return 1;
}

void XSceneTitle::RecvNoAccount(XGAME::xtConnectParam param)
{
	XConnector::sGet()->StopFSM();
	if (param == XGAME::xCP_LOGIN_UUID) {
		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_nickname", nullptr);
		pPopup->SetbModal(TRUE);
		Add(pPopup);
		XWnd *pImg = Find("img.input.box");
		if (pImg)
			pImg->SetEvent(XWM_CLICKED, this, &XSceneTitle::OnClickNicknameBox);
		xSET_BUTT_HANDLER(this, "butt.popup.ok", &XSceneTitle::OnCreateTryConnect);
	} else
	if( param == XGAME::xCP_LOGIN_IDPW ) {
		auto pAlert = XWND_ALERT( "%s", _T( "계정이 없습니다." ) );
		if( pAlert )
			pAlert->SetEvent( XWM_OK, this, &XSceneTitle::OnOkWrongPassword );
	} else {
		XBREAK( 1 );
	}
}

// void XSceneTitle::RecvReconnectTry(void)
// {
// 	m_TimerReconn.Set(1.f);
// }

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnGotoAppStore(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnGotoAppStore");
	// 여기에 앱스토어 이동코드를 넣으시오

#ifdef _VER_ANDROID
	//	JniHelper::LoadMarketURL("market://details?id=com.nhn.android.search");		// 예제. 잘됨
	JniHelper::LoadMarketURL("market://details?id=com.mtricks.caribe");			// 현재 테스트라 안됩니다
#endif

	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnClickIpChange(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickIpChange");
	//
	auto pAlert = XWND_ALERT( "ip 교체");
	auto pWndEdit = new XWndEdit(0, 0, 128, 32, BASE_FONT);
	pWndEdit->SetEvent(XWM_ENTER, this, &XSceneTitle::OnEnterIP);
	pWndEdit->SetszString(C2SZ(CONNECT_INI.m_cIP));
	pAlert->Add(pWndEdit);

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
	//
	auto pPopup = new XWndPopup(_T("layout_login.xml"), "popup_login");
	pPopup->SetstrIdentifier("popup.login");
	pPopup->SetbModal(TRUE);
	Add(pPopup);
	xSetButtHander(pPopup, this, "butt.login.ok", &XSceneTitle::OnClickLogin);
	xSetButtHander(pPopup, this, "butt.close", &XSceneTitle::OnClickLoginCancel);

	return 1;
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneTitle::OnNoHaveNoAccount(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnNoHaveNoAccount");
	// UUID로그인으로 시도
	if (LOGIN_INFO.GetstrUUID().empty())
		LOGIN_INFO.MakeUUIDLogin();
	LOGINSVR_SOCKET->SetdwConnectParam(XGAME::xCP_LOGIN_UUID);
	XConnector::sGet()->TryConnectLoginSvr(false);
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
	_tstring strid = C2SZ( cUserId );
	_tstring strUserName = C2SZ( cUsername );
	_tstring strName = _T("");		// 실제 이름
	CONSOLE( "XSceneTitle::DelegateFacebookCertResult: userid=%s, username=%s, param=%d", strid.c_str(), strUserName.c_str(), param );
#ifdef _VER_ANDROID
	JniHelper::GetUserProfileImage( cUserId );	// 프로필 사진 요청
//	JniHelper::GetUserProfileImage( "100000786878603" );	// 
#endif // _VER_ANDROID
	// 서버에 보내 계정을 새로 만들거나 계정정보를 받는다.
	// UUID+fb_userid로그인으로 시도
	if( LOGIN_INFO.GetstrUUID().empty() )
		LOGIN_INFO.MakeUUIDLogin();
	LOGIN_INFO.SetFacebookLogin( cUserId, strUserName.c_str(), strName.c_str() );
	LOGINSVR_SOCKET->SetdwConnectParam( XGAME::xCP_LOGIN_UUID );
	XConnector::sGet()->TryConnectLoginSvr( false );
	// XConnector::DelegateConnect()로 받는다.
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
* @brief
*****************************************************************/
int XSceneTitle::OnClickLogin(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickLogin");
	//
	auto pEditEmail = SafeCast<XWndEdit*>(Find("edit.email"));
	auto pEditPassword = SafeCast<XWndEdit*>(Find("edit.password"));
	if (pEditEmail && pEditPassword)
	{
		_tstring strEmail = pEditEmail->GetszString();
		_tstring strPassword = pEditPassword->GetszString();
		if (strEmail.empty() || strPassword.empty())
			return 1;
		// 		strEmail = _T("xuzhu");
		// 		strPassword = _T("1234");
		_tstring strNickname;
		LOGIN_INFO.SetIDPwLogin(strEmail.c_str(), strPassword.c_str());
		LOGINSVR_SOCKET->SetdwConnectParam(XGAME::xCP_LOGIN_IDPW);
		XConnector::sGet()->TryConnectLoginSvr(false);
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
	DoPopupSelectLogin();
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
		DoPopupSelectLogin();
	return 1;
}

void XSceneTitle::RecvClosedSvr()
{
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_close_svr", nullptr);
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
  XConnector::sGet()->DoDisconnectLoginSvr();
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
	xSET_SHOW( this, "text.send.dump", sizeDump > 0 );
}



