/**
 @file	XSockGameSvr.cpp
 @brief  
 @note Send류 함수에 있는 _XCHECK_CONNECT는 연결이 끊겨있을때 에러팝업창을 출력하고 동시에 FALSE를 리턴한다. 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @author xuzhu
 @date 2014-09-15
 @todo nothing
*/
#include "stdafx.h"
#include "XFramework/XReceiverCallback.h"
#include "XSockGameSvr.h"
#include "XPacketCG.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XAccount.h"
#include "XSceneTitle.h"
#include "XSceneWorld.h"
#include "XSceneBattle.h"
#include "XSceneUnitOrg.h"
#include "XSceneLegion.h"
#include "XSceneStorage.h"
#include "XSceneTrader.h"
#include "XSceneHero.h"
#include "XSceneArmory.h"
#include "XSceneShop.h"
#include "XSceneTech.h"
#include "XSceneReady.h"
#include "XWorld.h"
#include "XBaseItem.h"
#include "XQuestProp.h"
#include "XQuest.h"
#include "XQuestMng.h"
#include "XSkillMng.h"
#include "XSceneGuild.h"
#include "XConnector.h"
#include "XLoginInfo.h"
#include "XSquadron.h"
#include "XStruct.h"
#include "XStageObj.h"
#include "XSpots.h"
#include "XLegion.h"
#include "XWndDelegator.h"
#include "XFramework/XInApp.h"
#include "XPropLegion.h"
#include "XPropCamp.h"
#include "XPropHelp.h"
#include "XImageMng.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#endif // _VER_ANDROID

#define BTRACE		XTRACE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xCampaign;
//#define XWND_ALERT(...)	{}

#define SIZE_QUEUE		(1024 * 1024 * 10)
// std::shared_ptr<XSockGameSvr> XSockGameSvr::s_spInstance;
// 
// ////////////////////////////////////////////////////////////////
// std::shared_ptr<XSockGameSvr>& XSockGameSvr::sGet() {
// 	if( s_spInstance == nullptr )
// 		s_spInstance = std::shared_ptr<XSockGameSvr>( new XSockGameSvr );
// 	return s_spInstance;
// }
//////////////////////////////////////////////////////////////////////////
#ifdef _VER_IOS
 #ifdef _XBSD_SOCKET
XSockGameSvr::XSockGameSvr( XNetworkDelegate *pDelegate )
: XBSDSocketClient( pDelegate, SIZE_QUEUE )
 #else // bsd socket
XSockGameSvr::XSockGameSvr( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
: XCFSocketClient( pDelegate, SIZE_QUEUE )
 #endif // not bsd socket
#endif // ios
#ifdef WIN32
XSockGameSvr::XSockGameSvr(XNetworkDelegate *pDelegate)
: XEWinSocketClient(pDelegate, SIZE_QUEUE, 0 ) 
#endif
#ifdef _VER_ANDROID
XSockGameSvr::XSockGameSvr( XNetworkDelegate *pDelegate )
: XBSDSocketClient( pDelegate, SIZE_QUEUE, 0 )
#endif
{
	Init();
	XGuild::sSetpDelegate( this );
	//
	AddResponse( xCL2GS_INVALID_VERIFY,
					&XSockGameSvr::RecvInvalidVerify, nullptr );
	AddResponse( xCL2GS_LOBBY_ATTACKED_SPOT_RESULT, 
					&XSockGameSvr::RecvAttackedSpotResult, nullptr );
	AddResponse( xCL2GS_LOBBY_ATTACKED_CASTLE,
					&XSockGameSvr::RecvAttackedCastle, nullptr );
	AddResponse( xCL2GS_LOBBY_SULFUR_SPAWN,
					&XSockGameSvr::RecvSulfurSpawn, nullptr );
	AddResponse( xCL2GS_SPOT_SYNC,
					&XSockGameSvr::RecvSpotSync, nullptr );
	AddResponse( xCL2GS_LEVEL_SYNC,
					&XSockGameSvr::RecvLevelSync, nullptr );
// 	AddResponse( xCL2GS_LOBBY_MANDRAKE_MATCH_RESULT,
// 					&XSockGameSvr::RecvMandrakeMatchResult, nullptr );
// 	AddResponse( xCL2GS_LOBBY_MANDRAKE_SPOT_RECON,
// 					&XSockGameSvr::RecvMandrakeLegionResult, nullptr );
	AddResponse( xCL2GS_LOBBY_NPC_SPAWN,
					&XSockGameSvr::RecvNpcSpawn, nullptr );
// 	AddResponse( xCL2GS_UPDATE_SPECIAL_SPOT,
// 					&XSockGameSvr::RecvUpdateSpecialSpot, nullptr );
	AddResponse( xCL2GS_ADD_ITEM,
					&XSockGameSvr::RecvAddItem, nullptr );
	AddResponse( xCL2GS_ADD_ITEM_LIST,
					&XSockGameSvr::RecvItemsList, nullptr );
	AddResponse(xCL2GS_LOBBY_ITEM_BUY,
					&XSockGameSvr::RecvItemBuy, nullptr);
	AddResponse(xCL2GS_QUEST_OCCUR,
					&XSockGameSvr::RecvQuestOccur, nullptr);
	
	AddResponse(xCL2GS_LOBBY_POST_ADD,
					&XSockGameSvr::RecvPostAdd, nullptr);
	AddResponse(xCL2GS_LOBBY_POST_INFO,
					&XSockGameSvr::RecvPostInfo, nullptr);
	AddResponse(xCL2GS_LOBBY_POST_GETITEM_ALL,
					&XSockGameSvr::RecvPostGetItemAll, nullptr);
	AddResponse( xCL2GS_QUEST_COMPLETE,
					&XSockGameSvr::RecvQuestComplete, nullptr );
	AddResponse( xCL2GS_QUEST_UPDATE,
					&XSockGameSvr::RecvQuestUpdate, nullptr );
	AddResponse( xCL2GS_QUEST_REQUEST_REWARD, 
					&XSockGameSvr::RecvReqQuestReward, nullptr );
	AddResponse( xCL2GS_ITEM_INVEN_SYNC, 
					&XSockGameSvr::RecvItemItemsSync, nullptr );

	AddResponse(xCL2GS_SYSTEM_GAME_NOTIFY,
					&XSockGameSvr::RecvSystemGameNotify, nullptr);
	AddResponse( xCL2GS_OCCUR_BUFF,
					&XSockGameSvr::RecvOccurBuff, nullptr );
	AddResponse( xCL2GS_FINISH_BUFF,
					&XSockGameSvr::RecvFinishBuff, nullptr );
	AddResponse( xCL2GS_INGAME_ADD_BATTLE_LOG,
					&XSockGameSvr::RecvAddBattleLog, nullptr );
// 	AddResponse(xCL2GS_LOBBY_GUILD_UPDATE,
// 					&XSockGameSvr::RecvUpdateGuild, nullptr);
	AddResponse( xCL2GS_LOBBY_GUILD_EVENT,
							 &XSockGameSvr::RecvGuildEvent, nullptr );
	AddResponse( xCL2GS_RESEARCH_COMPLETE,
					&XSockGameSvr::RecvResearchComplete, nullptr );
	AddResponse( xCL2GS_CHECK_TRAIN_COMPLETE,
					&XSockGameSvr::RecvCheckTrainComplete, nullptr );
	AddResponse(xCL2GS_LOBBY_GUILD_JOIN_RESULT,
					&XSockGameSvr::RecvJoinResult, nullptr);
// 	AddResponse(xCL2GS_LOBBY_GUILD_KICK_RESULT,
// 					&XSockGameSvr::RecvKickGuild, nullptr);
	AddResponse(xCL2GS_CREATE_HERO,
					&XSockGameSvr::RecvCreateHero, nullptr );
	AddResponse( xCL2GS_SYNC_BASE_INFO,
					&XSockGameSvr::RecvSyncBaseInfo, nullptr );
	AddResponse( xCL2GS_ACCOUNT_DUPLICATE_LOGOUT,  
					&XSockGameSvr::RecvDuplicateLogout, nullptr);
// 	AddResponse(xCL2GS_LOBBY_TIME_TRADE_CALL,
// 					&XSockGameSvr::RecvTimeTradeCall, nullptr);
	AddResponse(xCL2GS_LOBBY_ITEM_SHOP_LIST_TIME,
					&XSockGameSvr::RecvArmoryListTimeChange, nullptr);
// 	AddResponse( xCL2GS_LOBBY_SPOT_VISIT,
// 					&XSockGameSvr::RecvSpotTouch, nullptr );
	AddResponse( xCL2GS_SYNC_TRAIN_SLOT,
					&XSockGameSvr::RecvSyncTrainSlot, nullptr );
	AddResponse( xCL2GS_SYNC_RESOURCE,
					&XSockGameSvr::RecvSyncResource, nullptr );
	AddResponse( xCL2GS_UNLOCK_MENU,
					&XSockGameSvr::RecvUnlockMenu, nullptr );
	AddResponse( xCL2GS_SYNC_HELLO, &XSockGameSvr::RecvSyncHello, nullptr );
	AddResponse( xCL2GS_ACCOUNT_PUBLIC_KEY,
					&XSockGameSvr::RecvPublicKey, nullptr );
	AddResponse( xCL2GS_ACCOUNT_RECONNECT_TO_GAMESVR,
					&XSockGameSvr::RecvReconnectTry, nullptr );
	AddResponse( xCL2GS_PROP_SERIALIZE,
					&XSockGameSvr::RecvProp, nullptr );
	AddResponse( xCL2GS_SYNC,
					&XSockGameSvr::RecvSyncAcc, nullptr );
	
//#ifdef _DEV
	AddResponse( xCL2GS_LOBBY_SIMUL_ATTACKED_HOME,
					&XSockGameSvr::RecvAttackedSimulByHome, nullptr );
//#endif // _DEV
}

BOOL XSockGameSvr::CallRecvCallback( ID idPacket, XPacket& p )
{
	std::map<ID, xCALLBACK>::iterator itor;
	itor = m_mapCallback.find( idPacket );
	if( itor == m_mapCallback.end() )	{
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		return FALSE;
	}
	xCALLBACK c = itor->second;
//	XBREAK( c.pCallback == nullptr );
  if( c.pCallback != nullptr )
	  (this->*c.pCallback)( p, c );
	return TRUE;
}

void XSockGameSvr::cbOnDisconnectByExternal()
{
	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_DISCONNECTED_GAMESVR );
}



BOOL XSockGameSvr::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	BTRACE( "packet:%d", idPacket );
	xtPacket packet = (xtPacket) idPacket;

	/*
		원래는 SendRequest()함수를 쓰면 아래 Arrive로 자동으로 타임아웃 처리를 했는데
		기존 Pvp코드에서 자동으로 Arrive를 하면 안되는 코드들이 있어 호환성을 위해 삭제함
		SendRequest()를 사용하는 함수는 수동으로 TIMEOUT_MNG->Arrive() 를 해줘야 함.
	*/
	XTimeoutMng::sGet()->ArriveWithKey( (DWORD)packet );	// 요청했던 응답이 돌아와서 타임아웃 해제시킴
	if( CallRecvCallback( idPacket, p ) == FALSE )
		return FALSE;

	return TRUE;
}


BOOL XSockGameSvr::SendNormal( XPacket& ar, ID idTextDisconnect )
{
	_XCHECK_CONNECT(idTextDisconnect);
	//
	return Send( ar );
}

/**
 패킷헤더만 보내는 다용도 send
 bTimeout: 타임아웃을 적용할것인지
*/
BOOL XSockGameSvr::SendPacketHeaderOnly( xtPacket idPacket )
{
	//
	XPacket ar((ID)idPacket);
	// 응답없는 일반 send
	return SendNormal( ar );
}

void XSockGameSvr::SendRequestPacketHeaderOnly( XWnd *pCallback, xtPacket idPacket )
{
/*
	XPacket ar(idPacket);
	Send( ar );
	XBREAK( pCallback == nullptr );
	TIMEOUT_MNG->Add( pCallback->GetID(), idPacket, 10.f );
	CONSOLE("send packet header only:%d", idPacket );
*/
}

/**
*/
BOOL XSockGameSvr::SendReqLogin( XWnd *pCallback, ID idAccount )
{
	_XCHECK_CONNECT( 0 );
	xtPacket idPacket = xCL2GS_NONE;
//	if( XAccount::sGetPlayer() && XE::IsHave(XAccount::sGetPlayer()->GetSessionKey()) ) {
	if( XGame::sIsHaveSessionKey() ) {
//		idPacket = xCL2GS_ACCOUNT_SESSION_UUID;
		idPacket = xCL2GS_ACCOUNT_REQ_LOGIN;		// 세션키 로그인이 현재 안되서 이걸로 함.
		CONSOLE( "send with session key" );
	} else {
		idPacket = xCL2GS_ACCOUNT_REQ_LOGIN;
		CONSOLE( "send without session key" );
	}
	XPacket ar((ID)idPacket);
	ar << VER_CGPK;
	ar << idAccount;
	if( XGame::sIsHaveSessionKey() )
		ar << XGame::sGetSessionKey();
	else
		ar << _T("");
	ar << (char)XGAME::PLATFORM_STORE;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	ID idKey =
		AddResponse(xCL2GS_ACCOUNT_SUCCESS_LOGIN, &XSockGameSvr::RecvSuccessLogin, pCallback);
	Send( ar ); 

	return TRUE;
} 

// 로그인 성공하여 계정정보를 받음.
void XSockGameSvr::RecvSuccessLogin( XPacket& p, const xCALLBACK& c )
{
	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_LOGINED_GAMESVR );
	// 재접시에도 ap때문에 데이타 받아야함.
	CONSOLE( "Accont size: %d bytes", p.size() );
	BYTE b0;
	p >> b0;	XGlobalConst::sGetMutable()->m_apPerOnce = b0;	// 서버에서 주는 값으로 사용
	p >> b0;	m_CryptoObj.SetkeyPrivate( b0 );
	p >> b0 >> b0;
	DWORD dw0;
	for( int i = 0; i < 16; ++i )
		p >> dw0;
	auto spAcc = XAccount::sGetPlayer();
	_tstring _strPublicKey;
	if (spAcc == nullptr)	{
		spAcc = std::make_shared<XAccount>();
		XAccount::sSetPlayer( spAcc );
		spAcc->SetpDelegateLevel( GAME );
		XArchive arAcc(0x10000);
		p >> arAcc;
		spAcc->DeSerialize( arAcc );
		XGame::sSetSessionKey(_T("123456789"));
		sGetpWorld()->OnAfterDeSerialize( GAME, spAcc->GetidAccount() );
		// 암호화 키테이블
		m_CryptoObj.DeSerializeKeyTable( p );
		// 플레이어의 프로필사진 요청.
		if( !spAcc->GetstrFbUserId().empty() ) {
			GAME->DoRequestPlayerProfileImg( spAcc );
		}
		// 계정을 받으면 fbid가 있는 성들을 대상으로 페북사진을 요청한다.
		GAME->DoRequestProfileImgAllSpot();
		CONSOLE( "login successed: create acc" );
	} else {
		// pAccount에다 직접 Deserialize해도 문제없도록 해야하는데 시간이걸리니 일단 땜빵.
#pragma message("재접시에는 업데이트가 필요한 데이타가 한정되어있을것이다. 그 데이타들만 다시 받게 해서 패킷절약을 시켜야할듯. 홈갔다가 올때마다 풀로 받는건 좀 그럼.")		
		/*
		앱을 켜둔채로 있다가 서버가 만약 죽었다가 새로 떠서 비동기가 발생한 상황이면....
		서버의 계정정보를 다시 갱신해야한다.
		하지만 매번 재접때마다 풀데이타를 받는것은 네트워크의 낭비가 심하다.
		만약 이런일이 생기면 비동기 상태로 다시 패킷을 보낼것이고 만약 비동기면 "비동기로 재시작"을 하도록 하는게 경제적일듯 하다.
		*/
		XArchive arAcc( 0x10000 );
		p >> arAcc;
		auto spAcc = XSPAcc( new XAccount() );	// 객체 정적생성 금지. XAcc는 무조건 XSPAcc시리즈로 생성해야함. 내부에서 GetThis()를 하기 때문.
		spAcc->DeSerialize( arAcc );
		ACCOUNT->SetAP( spAcc->GetAP() );
		ACCOUNT->SetmaxAP( spAcc->GetmaxAP() );
		m_CryptoObj.DeSerializeKeyTable( p );
		CONSOLE( "login successed: relogin" );
	}
	if( !XGame::s_strGcmRegid.empty() ) {
		// 서버로 보내서 저장.
		SendReqGCMResistId( GAME, XGame::s_strGcmRegid );
		XGame::s_strGcmRegid.clear();		// 해킹할수 있으니 보내고 바로 지움.
	}
	CONSOLE("login successed:" );
	if( SCENE_TITLE )
		SCENE_TITLE->OnConnectedGameSvr();
}

// 같은 계정으로 다른곳에서 접속을 해서 this는 끊어야 한다.
void XSockGameSvr::RecvDuplicateLogout(XPacket& ar, const xCALLBACK& c)
{
	CONSOLE("XSockGameSvr::RecvDuplicateLogout");
	GAME->RecvDuplicateLogout();
}

/**
 @brief 접속을 시도했으나 이미 로그인되어있어 THIS가 다시 로그인을 시도해야함.7
*/
void XSockGameSvr::RecvReconnectTry( XPacket& ar, const xCALLBACK& c )
{
	CONSOLE("%s", __TFUNC__);
	BYTE b0;
	WORD w0;
	ar >> w0;		float sec = (float)w0 / 1000.f;
	ar >> b0;		bool bToLogin = xbyteToBool(b0);
	XParamObj paramObj;
	paramObj.Set( "sec", sec );
	if( bToLogin ) {
		XConnector::sGet()->DispatchFSMEvent( 
			xConnector::xFE_RECONNECT_TRY_LOGINSVR, paramObj );
	}
	else {
		XConnector::sGet()->DispatchFSMEvent( 
			xConnector::xFE_RECONNECT_TRY_GAMESVR, paramObj );
	}
}

void XSockGameSvr::RecvProp( XPacket& ar, const xCALLBACK& c )
{
	CONSOLE("RecvProp");
	try {	{
			XTRACE( "deserialize global.xml" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XGlobalConst::sGetMutable()->DeSerialize( arProp, 0 );
		} {
			// 			throw std::exception("xbreak");
			XTRACE( "deserialize constant" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !CONSTANT ) {
				CONSTANT = new XConstant();
			}
			CONSTANT->DeSerialize( arProp, 0 );
			IMAGE_MNG->LoadMap( _T( "img_map.txt" ) );
	}
		{
			XTRACE( "deserialize propunit" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_UNIT ) {
				PROP_UNIT = new XPropUnit();
			}
			PROP_UNIT->DeSerialize( arProp );
		} {
			XTRACE( "deserialize prophero" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_HERO ) {
				PROP_HERO = new XPropHero();
			}
			PROP_HERO->DeSerialize( arProp, 0 );
		}
		{
			XTRACE( "deserialize propitem" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_ITEM ) {
				PROP_ITEM = new XPropItem();
			}
			PROP_ITEM->DeSerialize( arProp, 0 );
		}
		{
			XTRACE( "deserialize proplegion" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XPropLegion::sGet()->DeSerialize( arProp, 0 );
		}
		{
			XTRACE( "deserialize propworld" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_WORLD ) {
				PROP_WORLD = new XPropWorld();
			}
			PROP_WORLD->DeSerialize( arProp, 0 );
		}
		{
			XTRACE( "deserialize propcloud" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_CLOUD ) {
				PROP_CLOUD = new XPropCloud();
			}
			PROP_CLOUD->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize skillmng" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !SKILL_MNG ) {
				SKILL_MNG = new XSkillMng();
			}
			SKILL_MNG->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize proptech" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XPropTech::sGet()->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize propupgrade" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XPropUpgrade::sGet()->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize propsquad" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			if( !PROP_SQUAD ) {
				PROP_SQUAD = new XPropSquad();
			}
			PROP_SQUAD->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize propcamp" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XPropCamp::sGet()->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize propquest" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			XQuestProp::sGet()->DeSerialize( arProp, 0 );
		} {
			XTRACE( "deserialize prophelp" );
			XArchive arProp;
			ar >> arProp;
			arProp.DoUnCompress();
			xHelp::XPropHelp::sGet()->DeSerialize( arProp, 0 );
		}

		//
		XTRACE( "Recv Deserialize finished" );
		GAME->OnAfterPropSerialize();
		CONSOLE( "RecvProp finished" );
	} catch( std::exception& e ) {
		_tstring str = C2SZ( e.what() );
		CONSOLE( "exception: %s ", str.c_str() );
		exit( 1 );
	}
}

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqShowAdsVideo( XWnd *pTimeoutCallback)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_SHOW_ADS);

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvShowAdsVideo, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqShowAdsVideo()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqShowAdsVideo()
*/
void XSockGameSvr::RecvShowAdsVideo( XPacket& p, const xCALLBACK& c )
{
	DWORD dwKey;
	p >> dwKey;
	if( dwKey ) {
		std::string strKey = XE::Format( "%08x", dwKey );
		GAME->SetstrKeyAds( strKey );		// 광고 다보고난 후 이 키값을 서버로 보내 비교한다.
		// success
#if defined(_VER_ANDROID)
		//JniHelper::ShowTapjoyDirectPlay();
		CppToJavaV( "CppToJavaV", "show_ads", "", "" );
#elif defined(WIN32)
		XReceiverCallback::sGet()->cbOnReceiveCallback( "finish_show_ads", "", "", "" );
#endif // _VER_ANDROID
	} else {
		XWND_ALERT( "%s", "failed show ads");
	}
}

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqDidFinishShowAdsVideo( XWnd *pTimeoutCallback, const std::string& strKeyAds )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_FINISH_SHOW_ADS );
	ar << strKeyAds;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvDidFinishShowAdsVideo, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqDidFinishShowAdsVideo()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqDidFinishShowAdsVideo()
*/
void XSockGameSvr::RecvDidFinishShowAdsVideo( XPacket& p, const xCALLBACK& c )
{
	int numGemEarned; // 보상젬 개수
	int numGems;  // 젬을 벌고 난 후의 젬 개수
	p >> numGemEarned >> numGems;
	ACCOUNT->SetCashtem( numGems );
	if( GAME->GetpScene() ) {
		GAME->GetpScene()->OnRecvDidFinishShowAdsVideo( numGemEarned );
	}
}


