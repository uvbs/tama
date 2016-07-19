#include "stdafx.h"
#include "XGame.h"
#include "sprite/SprMng.h"
#include "XParticleMng.h"
#ifdef _VER_OPENGL
    #include "OpenGL2/XParticleOpenGL.h"
#endif
#include "XEToolTip.h"
#include "_Wnd2/XWindow.h"
#ifdef _VER_IOS
	#include "objc/xe_ios.h"
#endif
#ifdef _VER_ANDROID
#include "android/com_mtricks_xe_Cocos2dxHelper.h"
#include "android/JniHelper.h"
//#include "XWndEdit.h"
#endif
#include "XGameWnd.h"
//#include "XGameCutScene.h"
#include "client/XAppMain.h"
//#include "client/XTimeoutMng.h"
#include "XAppLayout.h"
#include "XSoundTable.h"
#include "XLoginInfo.h"
#ifdef _XLOGIN_FACEBOOK
#include "XFacebook.h"
#endif
#include "XSceneTitle.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define XWM_TIMEOUT	1001

XGame *GAME = NULL;
XAppLayout *LAYOUT = NULL;

XGame::XGame() 
{ 
	GAME = this;
	Init();
	XLOGXN("ver:%s", GAME_VER);
}

void XGame::Destroy() 
{
	//DestroyWnd( m_pScene );		// 여기서 먼저 삭제해야 밑에 SPRMNG에서 해제안된spr이 남질 않게 된다.
//	SAFE_DELETE( TIMEOUT_MNG );
	SAFE_DELETE( PARTICLE_MNG );
	SAFE_DELETE( TEXT_TBL );
	GAME = NULL;
}

BOOL XGame::RestoreDevice( void )
{
	PARTICLE_MNG->RestoreDevice();
	//
	return XEContent::RestoreDevice();
}

void XGame::Create( void )
{
	FONTMNG->SetpDelegate( this );
	//
#ifdef _XLOGIN_FACEBOOK
	// 페이스북 인스턴스 생성
	XFacebook::sSet( new XFacebook );
#endif
#ifdef WIN32
	PARTICLE_MNG = new XPointSpriteParticleMng( NULL, 2000 );
#else
	PARTICLE_MNG = new XParticleMngGL( NULL, 2000 );
#endif
	//
//	TIMEOUT_MNG = new XTimeoutMng( this );
	//////////////////////////////////////////////////////////////////////////
	// 루아파일들을 로딩타입과 관계없이 워킹으로 카피한다.
	CopyLuaToWork();
	// 텍스트 테이블
	LoadTextTable();
	// 클라/서버 공통으로 쓰는 리소스 로딩
	XGameCommon::Create();
	//
//	CONNECT_INI.Load( _T("connect.ini") );
	// 마지막 로그인했던 아이디와 비번 정보
//	LOGIN_INFO.Load( XAPP->GetstrUUIDFile() );

	m_pSceneMng = new XESceneMng( this, this );
	m_pSceneMng->SetidNextScene( XGAME::xSC_START );
	Add( m_pSceneMng );

	// 서버접속, 로비, 인게임 에서 사용될 리소스들을 로딩
	CreateGameResource();
} // Create()

// 루아파일들을 로딩타입과 관계없이 워킹으로 카피한다.
void XGame::CopyLuaToWork( void )
{
	// 루아파일은 내부에서 다른 루아파일을 include하기때문에 .lua파일은 미리 워크폴더에 카피되어 있어야 한다.
	// 나중에 이부분은 엔진안으로 집어넣어서 lua파일은 자동으로 다 카피시킬것.
	// 패치가 끝난후에도 워킹폴더에 lua가 없으면 패키징에서 꺼내 워킹으로 카피한다.
	//	if( XE::GetLoadType() != XE::xLT_PACKAGE_ONLY )
/*
	{
		if( XE::IsExistFileInWork( XE::MakePath( DIR_SCRIPTA, "help.lua" ) ) == FALSE )
			XE::CopyPackageToWork( XE::MakePath( DIR_SCRIPTA, "help.lua" ) );
		if( XE::IsExistFileInWork( XE::MakePath( DIR_SCRIPTA, "scr_XGame.lua" ) ) == FALSE )
			XE::CopyPackageToWork( XE::MakePath( DIR_SCRIPTA, "scr_XGame.lua" ) );
	}
*/
}

BOOL XGame::LoadTextTable( void )
{
	XBREAK( TEXT_TBL != NULL );
	// 이것은 장차 엔진단으로 들어가게 된다. 
	// 일단은 패치클라 실행전에 로딩되어야 하므로 여기 잔류한다.
	{
		TCHAR szRes[ 64 ];
		TEXT_TBL = new XTextTable;	
		LPCTSTR szFile  = XE::Format( _T("text_%s.txt"), XE::LANG.GetszFolder() );
		_tcscpy_s( szRes, XE::MakePath( DIR_PROP, szFile ) );
		if( TEXT_TBL->Load( szRes ) == FALSE )
		{
#ifdef _CLIENT
#ifndef _DEBUG
			if( XE::GetLoadType() == XE::xLT_WORK_TO_PACKAGE_COPY )
			{
				// 파일이 손상되었다고 판단하고 패키지폴더에서 다시 카피함
				XE::CopyPackageToWork( szRes );
				XALERT( "%s: The file is damaged. Take action, but if the same problem continues to occur, please delete the app and reinstall.", szRes );
				SAFE_DELETE( TEXT_TBL );
				TEXT_TBL = new XTextTable;	
				TEXT_TBL->Load( szRes );
			}
#endif // not DEBUG
#endif	
		}
	}
	return TRUE;
}

void XGame::CreateGameResource( void )
{
#ifdef _XPATCH
	// 패치로 text_ko.txt가 갱신되었을 수도 있으니 다시 로딩함.
	SAFE_DELETE( TEXT_TBL );
	LoadTextTable();
#endif
	// 공통 리소스 로딩.
	XGameCommon::CreateCommon();
	// 패치 클라가 들어가면 게임관련 리소스는 패치클라가 끝난후 로딩된다.
	//
	//
//	SOUND_TBL = new XSoundTable;
//	SOUND_TBL->Load( _T("sound.txt") );
	//
	XWndButton::SetDefaultEvent( XWM_SOUND_DOWN, this, &XGame::OnSoundDown );
	//
	XLOGXN("load layout.xml");
	// 메인 레이아웃 생성
	XAppLayout::sCreate(_T("layout.xml"), NULL );
	XLayout::sSetMain( XAppLayout::sGet() );
	//

} // createGameResource

void XGame::DestroyGameResource( void )
{
	XLOGXN("XGame::DestroyGameResource");
	XLayout::sDestroyMain();
	//
	XGameCommon::DestroyCommon();
	//
#ifdef _XPATCH
	SAFE_DELETE( TEXT_TBL );
#endif
	XLOGXN("--XGame::DestroyGameResource");
}

// idScene에 해당하는 씬객체를 생성해서 돌려줘야 한다.
XEBaseScene* XGame::DelegateCreateScene( XESceneMng *pSceneMng, ID idScene, XGAME::xSPM_BASE* pParam )
{
	XEBaseScene *pScene = NULL;

	switch( idScene )
	{
	case XGAME::xSC_START:
		pSceneMng->SetidNextScene( XGAME::xSC_TITLE );
		break;
	case XGAME::xSC_TITLE:
		pScene = new XSceneTitle( this );
		break;
	}

	return pScene;
}

//
int XGame::Process( float dt )
{
	// 소켓객체의 프로세스(패킷펌핑)및 리커넥트 처리
    //
	XEContent::Process( dt );
    //
	PARTICLE_MNG->FrameMove( dt );
	//
//	TIMEOUT_MNG->Process( dt );
    return 1;
}

void XGame::Draw( void )
{
	XEContent::Draw();
	if( TOOLTIP )
		TOOLTIP->Draw();
	// 
/*
	if( TIMEOUT_MNG && TIMEOUT_MNG->IsRequesting() )
	{
		// 응답요청하고 조금 지나도 안오면 화면 어둡게 하고 메시지 띄우기 시작.
		if( TIMEOUT_MNG->IsOverMsgTime() )
		{
			GRAPHICS->FillRectSize( XE::VEC2(0), XGAME_SIZE, XCOLOR_RGBA( 0, 0, 0, 64 ) );
			XAPP->GetpGame()->GetpfoSystem()->SetAlign( XE::xALIGN_HCENTER );
			XAPP->GetpGame()->GetpfoSystem()->SetLineLength( XE::GetGameWidth() );
			XAPP->GetpGame()->GetpfoSystem()->DrawString( 0.f, XE::GetGameWidth() / 2.f, _T("no response") );
		}
	}
*/

#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
	{
		LPCTSTR szMsg = NULL;
		if( szMsg )
			PUT_STRING( 213, 13, XCOLOR_WHITE, szMsg );
	}
#endif

}

void XGame::OnLButtonDown( float x, float y )
{
	// 뭔가 응답을 기다리고 있는중이면 입력을 막음.
//	if( TIMEOUT_MNG && TIMEOUT_MNG->IsRequesting() )
//		return;
	XEContent::OnLButtonDown( x, y );
}
void XGame::OnMouseMove( float x, float y )
{
	XEContent::OnMouseMove( x, y );
}
void XGame::OnLButtonUp( float x, float y ) 
{
	// 뭔가 응답을 기다리고 있는중이면 입력을 막음.
//	if( TIMEOUT_MNG && TIMEOUT_MNG->IsRequesting() )
//		return;
	XEContent::OnLButtonUp( x, y );
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

void XGame::OnCheatMode( void )
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
	return NULL;
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


void XGame::OnResume( void ) 
{
}
// 홈버튼으로 나가기전 호출됨
void XGame::OnPause( void ) 
{
}		

// 구글/iOS의 인앱결제로 szSku상품을 사라. 이것의 결과는 비동기로 돌아온다.
int XGame::DoAsyncBuyIAP( LPCTSTR szSku )
{
	// 서버에 나잠시 할일 있으니 끊지 마라고 알림
//	GAMESVR_SOCKET->SendPacketHeaderOnly( (xtPacket)XE::xXEPK_NOW_WORKING );
#if defined(_VER_ANDROID)
	JniHelper::OnClickedBuyItem( szSku );
#elif defined(WIN32)
	// win32는 샀다고 치고 시뮬레이션만 한a다.
	string strSku = SZ2C(szSku);
	XE::OnPurchaseFinishedConsume( TRUE, "", strSku.c_str(), "test_token", "$1,100.99", "1029310293109230192", "1234567890" );
#elif defined(_VER_IOS)
	// 아직 구현안됨
	IOS::SendReqPurchase( szSku );
#endif
	return 1;
}

// 소모성 아이템의 구매가 완료됨
void XGame::OnPurchaseFinishedConsume( BOOL bGoogle,
									const char *cJson,
									const char *cidProduct, 
									const char *cToken,
									const char *cPrice,
									const char *cOrderId,
									const char *cTransactDate )
{
	// 결제정보를 로컬에 씀.
	/*
	파일로 남기려면 서버에서 영수증 처리가 병행되어야 하므로 아직 하지 않겠음.
	서버에서 영수증 검증을 하지 않으면 파일만 만드는것으로 쉽게 해킹이 가능함.
	*/
	//
	XE::SetbResumeBlock( FALSE );	// 구매실패시에도 넣어줄것.
	// 결과 받아서 아래 처리
	_tstring strSku = C2SZ(cidProduct);
	_tstring strToken = C2SZ(cToken);
	float price = XSYSTEM::StrToNumber( cPrice );
	_tstring strOrderId = C2SZ(cOrderId);
	_tstring strTransactDate = C2SZ(cTransactDate);
	_tstring strJson = C2SZ(cJson);
/*
	if( m_pAccount == NULL )
	{
		// 애플의 경우 finishTransaction이 되기전에 앱이 종료되었다면 앱이 시작할때 다시
		// 이곳으로 들어오게 된다. 계정이 아직 없는 상태이므로 트랜잭션 데이타를 임시로 받아놨다가
		// 접속이 되면 서버로 바로 보내도록 한다.
		// 구매데이타가 서버에 이미 반영이 되었는데 finishTransaction만 안된경우도 있을것이다
		// 그런경우는 서버에서 과거영수증을 비교해서 이미 처리된 영수증인지 확인하는 코드가 필요하다.
		XBREAK( m_pIncompletePurchase != NULL );
		SAFE_DELETE( m_pIncompletePurchase );
		m_pIncompletePurchase = new XGAME::PURCHASE;
		m_pIncompletePurchase->bGoogle = bGoogle;
		m_pIncompletePurchase->strJson = strJson;
		m_pIncompletePurchase->strSku = strSku;
		m_pIncompletePurchase->strToken = strToken;
		m_pIncompletePurchase->strOrderId = strOrderId;
		m_pIncompletePurchase->price = price;
//		m_pIncompletePurchase->usTime = usTime;
		m_pIncompletePurchase->strTransactDate = strTransactDate;
		return;
	}
*/
	CONSOLE("OnPurchaseFinishedConsume: sku=%s token=%s orderId=%s price=%f purchaseTime=%s",
														strSku.c_str(), 
														strToken.c_str(),
														strOrderId.c_str(),
														price,
														strTransactDate.c_str()
														 );
	// 클라 NOW_WORKING상태가 끝남을 알림
//	GAMESVR_SOCKET->SendPacketHeaderOnly( NULL, (xtPacket)XE::xXEPK_RELEASE_WORKING, FALSE );
	/*
	GAMESVR_SOCKET->SendPacketHeaderOnly( (xtPacket)XE::xXEPK_RELEASE_WORKING );
	// 서버에 결제정보를 보냄
	if( GAMESVR_SOCKET->SendBuyCashItemByIAP( this,  bGoogle,
										strJson.c_str(),
										strSku.c_str(),
										strToken.c_str(),
										strOrderId.c_str(),
										price,  
										strTransactDate.c_str() ) == FALSE )
	{
		XBREAKF(1, "falied CLIENT->SendBuyGemByIAP");
		GAMESVR_SOCKET->DoDisconnect();
		// 결제가 완전히 끝나지 않음. 대기해 주시오.
		XWND_ALERT("alert.wait.purchase","%s", XTEXT(2051));
		// 서버에 보내는게 실패해서 미완료 결제분이 생겼다.
		XBREAK( m_pIncompletePurchase != NULL );
		SAFE_DELETE( m_pIncompletePurchase );
		m_pIncompletePurchase = new XGAME::PURCHASE;
		m_pIncompletePurchase->bGoogle = bGoogle;
		m_pIncompletePurchase->strJson = strJson;
		m_pIncompletePurchase->strSku = strSku;
		m_pIncompletePurchase->strToken = strToken;
		m_pIncompletePurchase->strOrderId = strOrderId;
		m_pIncompletePurchase->price = price;
//		m_pIncompletePurchase->usTime = usTime;
		m_pIncompletePurchase->strTransactDate = strTransactDate;
		return;
	}
*/
}

void XGame::OnPurchaseError( const char *cErr )
{
/*
	if( GAMESVR_SOCKET )
		GAMESVR_SOCKET->SendPacketHeaderOnly( (xtPacket)XE::xXEPK_RELEASE_WORKING );
//		GAMESVR_SOCKET->SendPacketHeaderOnly( NULL, (xtPacket)XE::xXEPK_RELEASE_WORKING, FALSE );
	CONSOLE("OnPurchaseError: %s", C2SZ(cErr) );
	TIMEOUT_MNG->Arrive( xTIMEOUT_BUY_IAP );
*/
}

/**
 구매를 위해 비번을 치고 실제 구매요청을 보낸직후 호출되는 이벤트
 */
void XGame::OnPurchaseStart( const char *cSku )
{
	CONSOLE("XGame::OnPurchaseError: %s", C2SZ(cSku) );
//	TIMEOUT_MNG->Add( this->GetID(), xTIMEOUT_BUY_IAP );
}

// 버튼 누를때 나는 일반적인 소리를 지정한다.
int XGame::OnSoundDown( XWnd *pWnd, DWORD, DWORD )
{
	//SOUNDMNG->OpenPlaySound( 1 );
	return 1;
}

