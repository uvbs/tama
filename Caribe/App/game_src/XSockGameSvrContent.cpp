/**
 @file	XSockGameSvr.cpp
 @brief  
 @note Send류 함수에 있는 _XCHECK_CONNECT는 연결이 끊겨있을때 에러팝업창을 출력하고 동시에 FALSE를 리턴한다. 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @author xuzhu
 @date 2014-09-15
 @todo nothing
*/
#include "stdafx.h"
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
#include "XSpotDaily.h"
#include "XLegion.h"
#include "XWndDelegator.h"
#include "XFramework/XInApp.h"
#include "XSoundMng.h"
#include "XWndUnitOrg.h"
#include "XWndCampaign.h"
#include "XCampObj.h"
#include "XDefNetwork.h"
#include "XHero.h"

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


//========================================================
// 치트 
#ifdef _CHEAT
BOOL XSockGameSvr::SendCheat( XWnd *pTimeoutCallback, int type, DWORD param1, DWORD param2, DWORD param3, DWORD param4, const _tstring& strParam )
{
#ifdef _XSINGLE
	return TRUE;
#endif // _XSINGLE
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CHEAT);
	ar << type;
	ar << param1 << param2 << param3 << param4;	
	ar << strParam;
	if( type == 1 )	{
//		ACCOUNT->AddExp( param1 );
	} else
	if( type == 2 ) {
		ID snHero = param1;
		XHero *pHero = ACCOUNT->GetHero( snHero );
		if( XASSERT(pHero) ) {
			int level = (int)param2;
			auto type = (XGAME::xtTrain)param3;
			pHero->SetLevel( type, level );
		}
	} else
	if( type == 3 ) {
		ACCOUNT->AddGold( param1 );
	}
	Send( ar );
	GAME->SetbUpdate(TRUE);
	//
	return TRUE;
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
BOOL XSockGameSvr::SendReqCheatCreateItem( XWnd *pTimeoutCallback, XPropItem::xPROP *pProp, int num )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CHEAT_CREATE_ITEM );
	ar << pProp->idProp;
	ar << num;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvCheatCreateItem, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqCheatCreateItem()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqCheatCreateItem()
*/
void XSockGameSvr::RecvCheatCreateItem( XPacket& p, const xCALLBACK& c )
{
	int size, ver;
	p >> size;
	p >> ver;
	for( int i = 0; i < size; ++i )
	{
		// 아카이브로 묶인 아이템정보를 인벤토리에 add한다.
		// 단. 이미 있는 snItem의 경우는 삭제하고 다시 add한다.
		ACCOUNT->DeserializeAddItem( p, ver );
	}
	GAME->SetbUpdate( true );
}

#endif // cheat end
//////////////////////////////////////////////////////////////////////////

// 
void XSockGameSvr::RecvMsgBox( XPacket& p, const xCALLBACK& c )
{
	TCHAR szBuff[ 1024 ];
	DWORD idText;
	p.ReadString( szBuff );
	p >> idText;
//	GAME->RecvMsgBox( szBuff, idText );
}

/**
*/
void XSockGameSvr::RecvInvalidVerify( XPacket& p, const xCALLBACK& c )
{
	XConnector::sGet()->StopFSM();
	XTimeoutMng::sGet()->Clear();
	GAMESVR_SOCKET->DoDisconnect();
	auto pAlert = XWND_ALERT( "%s", _T( "데이터 비동기로 인해 게임을 다시 실행시킵니다." ) );
	pAlert->SetEvent( XWM_OK, GAME, &XGame::OnGotoStart );
}



/**
 @brief 상품을 구매하기 전 서버로부터 페이로드를 요청한다.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqCreatePayload( XWnd *pTimeoutCallback, const _tstring& idsProduct )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_CASH_ITEM_BUY_PAYLOAD  );
	ar << (BYTE)XGAME::GetPlatform();
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << idsProduct;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvCreatePayload, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqCreatePayload()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqCreatePayload()
*/
void XSockGameSvr::RecvCreatePayload( XPacket& p, const xCALLBACK& c )
{
	CONSOLE("%s:", __TFUNC__);
	BYTE b0;
	_tstring idsProduct;
//	_tstring strPayload;
	p >> b0;	auto errCode = ( XGAME::xtErrorIAP )b0;
	p >> b0;	auto platform = ( XGAME::xtPlatform )b0;
	p >> b0;	auto typeProduct = ( XGAME::xtCashType )b0;
	p >> b0;
	p >> idsProduct;
	XBREAK( idsProduct.empty() );
// #ifdef _SOFTNYX
// 	_tstring strPayload;
// 	p >> strPayload;
// 	XBREAK( strPayload.empty() );
// 	const std::string strcPayload = SZ2C( strPayload );
// #else
	// playStore or appStore
	const std::string strcPayload
		= m_CryptoObj.DeSerializeDecryptString( p );
// #endif // _SOFTNYX
	ID idTextResult = 0;
	switch( errCode )
	{
	case XGAME::xIAPP_SUCCESS: {
		XBREAK( strcPayload.empty() );
		auto pItemInfo = XGC->GetCashItem( idsProduct, platform );
		if( XASSERT( pItemInfo ) ) {
			if( !strcPayload.empty() )
				GAME->DoAsyncBuyIAP( idsProduct, strcPayload, (int)pItemInfo->m_price );
		}
	} break;
	case xIAPP_CANT_BUY_ANYMORE: 
		idTextResult = 2312;		// 더이상 구매 못함.
		break;
	default:
		idTextResult = 2288;		// 구입실패
		break;
	}
	if( idTextResult ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		XWND_ALERT( "%s(%d)", XTEXT( idTextResult ), errCode );		
#else
		XWND_ALERT( "%s", XTEXT( idTextResult ));
#endif
	}
}


/**
 @brief 구글(혹은 애플스토어)에서 인앱상품 결제를 마치고 게임서버에 결제했다는것을 알림.
 @details 영수증이 변조되었다면 실패가 떨어질수 있슴.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param bGoogle 구글결제라면 TRUE, 애플결제라면 FALSE
 @param szJson Json포맷의 영수증 문자열
 @param szIdentifier 앱스토어에 등록되어있는 상품 식별자
 @param szToken 
 @param szOrderId 주문 아이디
 @param price 인앱 상품 가격
 @param szTransactDate 거래 시간
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendBuyCashItemByIAP( XWnd *pTimeoutCallback
																			, const std::string& strcJsonReceipt
																			, const std::string& strcSignature
																			, const std::string& strcIdsProduct )
{
	CONSOLE("%s:", __TFUNC__);
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_CASH_ITEM_BUY_IAP);
	ar << (BYTE)XGAME::GetPlatform();
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << strcJsonReceipt;
	ar << strcSignature;
	ar << strcIdsProduct;
	ID idKey = AddResponse( ar.GetidPacket(), 
													&XSockGameSvr::RecvBuyCashItemByIAP, pTimeoutCallback, 10.f );
	XTimeoutMng::sGet()->SetsecRequestTimeout( 0.1f );
	Send( ar );
	return TRUE;
}


void XSockGameSvr::RecvBuyCashItemByIAP( XPacket& p, const xCALLBACK& c ) 
{
	CONSOLE("%s", __TFUNC__);
	BYTE b0;
	_tstring idsProduct;
	DWORD numCash;
	p >> b0;		auto errCode = ( XGAME::xtErrorIAP )b0;
	p >> b0 >> b0 >> b0;
	p >> idsProduct;
	p >> numCash;
	//
	ACCOUNT->SetCashtem( numCash );
	GAME->SetbUpdate( true );

	// idsProduct를 소진시킨다.
	XBREAK( idsProduct.empty() );
#ifdef _SOFTNYX
#else
	XInApp::sGet()->DoConsumeItemAsync( idsProduct );
#endif // _SOFTNYX
	CONSOLE("%s:err=%d, ids=%s, numGem=%d",__TFUNC__, errCode, idsProduct.c_str(), numCash );
	const auto pGoodsInfo = XGlobalConst::sGet()->GetCashItem( idsProduct );
	if( XASSERT(pGoodsInfo) ) {
		if( errCode == XGAME::xIAPP_SUCCESS ) {
			XWND_ALERT_T( XTEXT( 2211 ), XTEXT(pGoodsInfo->m_text) );		// xx구입을 완료
		} else 
		if( errCode == XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD ) {
			// 서버에는 이미 적용이 됐으나 소진이 안되서 그런경우 이므로 그냥 조용히 넘어감.
			CONSOLE("errCode == XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD");
		} else {
			XWND_ALERT( "%s(%d)", XTEXT(2288), errCode );		// 기타 문제로 구입 실패
		}
	}
}

/**
 스팟 정찰
*/
BOOL XSockGameSvr::SendReqReconSpot( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_SPOT_RECON );
	ar << idSpot;

	ID idKey = 
	AddResponse( xCL2GS_SPOT_INFO,
				&XSockGameSvr::RecvSpotInfo, pTimeoutCallback );
//   ID idKey = AddResponse( xCL2GS_LOBBY_SPOT_RECON, 
// 					&XSockGameSvr::RecvSpotRecon, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_ATTACKED_CASTLE,
					&XSockGameSvr::RecvAttackedCastle, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_JEWEL_MATCH_RESULT,
					&XSockGameSvr::RecvJewelMatchResult, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_JEWEL_SPOT_RECON,
					&XSockGameSvr::RecvJewelReconResult, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_MANDRAKE_MATCH_RESULT,
					&XSockGameSvr::RecvMandrakeMatchResult, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_MANDRAKE_SPOT_RECON,
					&XSockGameSvr::RecvMandrakeLegionResult, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}


/**
 스팟 공격(유저/자원지/npc 통합)
*/
BOOL XSockGameSvr::SendReqSpotAttack( XWnd *pTimeoutCallback, ID idSpot, int idxStage, int idxFloor )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_SPOT_ATTACK);
	ar << idSpot;
	ar << ACCOUNT->GetvFocusWorld();
	ar << (char)idxStage;
	ar << (char)idxFloor;
	ar << (short)0;
	
	ID idKey = 
	AddResponse( xCL2GS_SPOT_INFO,
				&XSockGameSvr::RecvSpotInfo, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_MANDRAKE_BATTLE_INFO,
				&XSockGameSvr::RecvMandrakeLegionResult, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_JEWEL_BATTLE_INFO,
					&XSockGameSvr::RecvJewelBattleInfo, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_IS_ONLINE,
					&XSockGameSvr::RecvIsOnline, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_INGAME_CANCEL_BATTLE,
					nullptr, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_SYNC,
					&XSockGameSvr::RecvSyncAcc, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

void XSockGameSvr::RecvSpotInfo( XPacket& p, const xCALLBACK& c )
{
	XGAME::xBattleStartInfo info;
	info.DeSerialize( p, sGetpWorld() );
	ACCOUNT->SetAP( info.GetAP() );
	ACCOUNT->SetmaxAP( info.GetapMax() );
	auto pSpot = sGetpWorld()->GetSpot( info.m_idSpot );
	if( XBREAK( pSpot == nullptr ) )
		return;
	if( info.m_bRecon ) {
		// 정찰 상황
		if( SCENE_WORLD )
			SCENE_WORLD->OnRecvReconSpot( info.m_idSpot, pSpot->GetspLegion() );
	} else {
		// 전투 상황
		ProcSpotInfoBattle( info, pSpot, info.m_arParam );
	} // !bRecon
}

void XSockGameSvr::ProcSpotInfoBattle( const XGAME::xBattleStartInfo& info,
																			XSpot *pBaseSpot,
																			XArchive& arParam )
{
	ID snSession = info.m_snSession;
	ID idAccEnemy = info.m_idEnemy;
	if( XBREAK(snSession == 0) || info.m_bInitSpot ) {	// 상대계정이 없어지거나 하는 이유로 정상적인 정보를 보내주지 못할땐 스팟 초기화명령을 받는다.
		// 세션아이디는 상대가 npc든 유저든 무조건 있어야 함.
		pBaseSpot->Initialize( ACCOUNT );
		GAME->SetbUpdate( true );
		return;
	}
	auto typeSpot = info.m_typeSpot;
	XBREAK( pBaseSpot->GetspLegion() == nullptr );
	XBREAK( info.m_strName.empty() );
	XBREAK( info.m_Power == 0 );
	XBREAK( info.m_Level == 0 );
	// 전투시 필요한 기본정보.
// 	xBattleStart bs;
// 	bs.m_idEnemy = idAccEnemy;
// #ifndef _XSINGLE
// 	bs.m_typeSpot = info.m_typeSpot;
// 	bs.m_idSpot = info.m_idSpot;
// #endif // not _xSINGLE
// 	bs.m_Level = info.m_Level;
// 	bs.m_strName = info.m_strName;
// 	bs.m_spLegion[ 0 ] = ACCOUNT->GetCurrLegion();
// 	bs.m_spLegion[ 1 ] = pBaseSpot->GetspLegion();
// 	bs.m_typeBattle = XGAME::xBT_NORMAL;
	XVector<XSPLegion> aryLegion(2);
	aryLegion[0] = ACCOUNT->GetCurrLegion();
	aryLegion[1] = pBaseSpot->GetspLegion();
	auto spSceneParam 
		= std::make_shared<XGAME::xSceneBattleParam>( idAccEnemy,
																									info.m_typeSpot,
																									info.m_idSpot,
																									info.m_Level,
																									info.m_strName,
																									aryLegion,
																									XGAME::xBT_NORMAL,
																									0,
																									info.m_idxStage,
																									info.m_idxFloor );
		///< 
	XBREAK( snSession == 0 );
	ACCOUNT->SetBattleSession( snSession
													,	pBaseSpot->GetspLegion()
													, idAccEnemy
													, info.m_idSpot, 0 );
	//
	if( typeSpot == XGAME::xSPOT_SULFUR && idAccEnemy ) {
		// 인카운터된 유황스팟.
		auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
		if( XASSERT(pSpot) ) {
			if( XASSERT(pSpot->GetidEncounterUser()) ) {
				XBREAK( idAccEnemy == 0 );
				XBREAK( idAccEnemy != pSpot->GetidEncounterUser() );
				XBREAK( pSpot->GetnumSulfur() == 0 );
				if( SCENE_BATTLE ) {
					// 인카운터 유저 추가.
					spSceneParam->m_idEnemy = pSpot->GetidEncounterUser();
//					bs.m_idEnemy = pSpot->GetidEncounterUser();
					// 전투씬 파라메터를 밀어넣는다.
//					XSceneBattle::sSetBattleStart( bs );
					SCENE_BATTLE->OnRecvBattleResultSulfurEncounter( pSpot, info, spSceneParam );
				}
			}
		}
	} else {
		// 그 외 스팟(vsNPC유황 포함)
		//////////////////////////////////////////////////////////////////////////
		if( typeSpot == XGAME::xSPOT_CASTLE ) {
			auto pSpot = SafeCast<XSpotCastle*>( pBaseSpot );
			if( pSpot ) {
				XBREAK( idAccEnemy != pSpot->GetidOwner() );
	// 			idEnemy = pSpot->GetidOwner();
			}
		} else
		//////////////////////////////////////////////////////////////////////////
		if( typeSpot == XGAME::xSPOT_DAILY ) {
//			XArchive arParam;
			int i0, numEnter, numRemainEnter;
//			p >> arParam;
			arParam >> i0;	auto err = (XSpotDaily::xtError)i0;
			arParam >> numEnter;
			arParam >> numRemainEnter;
			if( err == XSpotDaily::xERR_TIMEOUT ) {
				XWND_ALERT("%s", _T("입장 제한시간 초과"));
				return;
			} else
			if( err == XSpotDaily::xERR_OVER_ENTER_NUM ) {
				GAME->DestroyWndByIdentifier( "popup.daily" );
				auto pPopup = new XWndPaymentByCash();
				pPopup->SetFillTryByDailySpot();
				GAME->GetpScene()->Add( pPopup );
//				pPopup->SetEvent( XWM_OK, GAME, &XGame::OnClickFillAPByCash );
				pPopup->SetEvent2( XWM_OK, []( XWnd* pWnd ) {
					const bool bByItem = false;
					GAMESVR_SOCKET->SendReqPaymentAssetByGem( GAME, xPR_TRY_DAILY, bByItem );
// 					GAMESVR_SOCKET->SendReqOpenCloud( this, idCloud, xTP_GOLD_AND_CASH );
				} );
//				XWND_ALERT("%s", XTEXT(2227));	// 도전횟수 없음.
				return;
			}
		} else
		//////////////////////////////////////////////////////////////////////////
		if( typeSpot == XGAME::xSPOT_SPECIAL ) {
			//XBREAK( pBaseSpot->GetspLegion() == nullptr );		XArchive arParam;		int i0;		p >> arParam;		arParam >> i0;	auto err = ( XSpotDaily::xtError )i0;		if( err == XSpotDaily::xERR_TIMEOUT ) {			XWND_ALERT( "%s", _T( "입장 제한시간 초과" ) );			return;		} else     if( err == XSpotDaily::xERR_OVER_ENTER_NUM ) {			XWND_ALERT( "%s", XTEXT( 2227 ) );	// 도전횟수 없음.			return;		}
		}
		//
		switch( typeSpot )
		{
		case XGAME::xSPOT_CASTLE:
		case XGAME::xSPOT_JEWEL:
		case XGAME::xSPOT_SULFUR:
		case XGAME::xSPOT_MANDRAKE:
		case XGAME::xSPOT_NPC:
		case XGAME::xSPOT_DAILY:
		case XGAME::xSPOT_SPECIAL:
		case XGAME::xSPOT_VISIT:
		case XGAME::xSPOT_CASH: {
		} break;
		case XGAME::xSPOT_CAMPAIGN:
		case XGAME::xSPOT_COMMON: {
			int idxStage, levelLegion, idxFloor;
			arParam >> idxStage;
			arParam >> levelLegion;
			arParam >> idxFloor;
			bool bEnterBattleScene = true;
			if( typeSpot == XGAME::xSPOT_COMMON )  {
				auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
				if( pSpot->IsGuildRaid() ) {
					//bs.m_typeBattle = XGAME::xBT_GUILD_RAID;
					spSceneParam->m_typeBattle = XGAME::xBT_GUILD_RAID;
					int i0;
					arParam >> i0;	auto err = ( XGAME::xtGuildError )i0;
					if( err == XGAME::xGE_ERROR_STILL_TRYING_RAID ) {
						XWND_ALERT( "%s", XTEXT( 2122 ) );    // 누군가 전투중
						return;
					}
				}
			}
			// 전투씬 추가파라메터
// 			bs.m_idxStage = idxStage;
// 			bs.m_idxFloor = idxFloor;
// 			bs.m_Level = levelLegion;
			spSceneParam->m_idxStage = idxStage;
			spSceneParam->m_idxFloor = idxFloor;
			spSceneParam->m_Level = levelLegion;
		} break;
		default:
			XBREAK(1);
			break;
		} // switch( typeSpot )
		if( SCENE_WORLD ) {
			// 전투씬 파라메터를 밀어넣는다.
//			XSceneBattle::sSetBattleStart( bs );
			SCENE_WORLD->OnRecvBattleInfo( spSceneParam );
		}
	}
}

/**
 @brief 공격하려고 했으나 상대유저가 온라인중이다.
*/
void XSockGameSvr::RecvIsOnline( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	ID idEnemy;
	p >> idSpot;
	p >> idEnemy;
	auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		XWND_ALERT("%s", XTEXT(2113));      // 상대가 온라인
	}
}

/**
 @brief 전투로인해 접속이 막힘
*/
void XSockGameSvr::RecvLockLoginForBattle( XPacket& p, const xCALLBACK& c )
{
	DWORD secPass, secTotal;
	p >> secPass;
	p >> secTotal;
	DWORD secStart = XTimer2::sGetTime() - secPass;
	if( SCENE_TITLE )
		SCENE_TITLE->OnRecvLoginLockForBattle( secStart, secTotal );
}

/**
 @brief 소탕이 취소됨
 소탕을 시도했으나 ap가 부족한(ap비동기)이유로 취소됨.
*/
void XSockGameSvr::RecvCancelKill( XPacket& p, const xCALLBACK& c )
{
	short s0;
	p >> s0;		ACCOUNT->SetAP( s0 );
	p >> s0;		ACCOUNT->SetmaxAP( s0 );
	GAME->GetpScene()->SetbUpdate( true );
}

/**
 @brief 전투로인한 로그인락이 해제됨.
*/
void XSockGameSvr::RecvUnlockLoginForBattle( XPacket& p, const xCALLBACK& c )
{
	if( SCENE_TITLE )
		SCENE_TITLE->OnRecvLoginLockFree();
}

/**
 광산 공격전 부대정보를 받는다.
*/
void XSockGameSvr::RecvJewelBattleInfo( XPacket& p, const xCALLBACK& c )
{
	int isFound, idxMine;
	p >> isFound;
	p >> idxMine;
	auto pJewel = sGetpWorld()->GetSpotJewelByIdx( idxMine );
	if( XBREAK(pJewel == nullptr) )
		return;
	if( isFound ) {
		XLegion *pLegion = nullptr;
		XGAME::xJewelMatchEnemy infoMatch;
		ID /*idEnemyAcc,*/ snSession;
//		int level, levelMine, defense, lootJewel, power, score;
//		_tstring strName;
//		XArchive arPacket;
		XArchive arLegion, arAbil;
		float rateHp;
// 		p >> idEnemyAcc >> level >> strName >> levelMine >> defense;
// 		p >> power >> score >> arLegion >> arAbil >> lootJewel/* >> idxMine*/;
		p >> arLegion >> arAbil;
		p >> infoMatch;
		p >> rateHp >> snSession;
		pLegion = XLegion::sCreateDeserializeFull( arLegion );
		if( XASSERT(pLegion) )
			pLegion->SetRateHp( pLegion->GetRateHp() * rateHp );
// 		XSpotJewel *pJewel = sGetpWorld()->GetSpotJewelByIdx( idxMine );
		// 이겼을 때 루팅할수 있는 양을 미리 받아둠.
		//pJewel->SetlootJewel( infoMatch.mloo );
		pJewel->SetMatch( infoMatch );
		LegionPtr spLegion = LegionPtr( pLegion );
		ACCOUNT->SetBattleSession( snSession,
																spLegion,
																infoMatch.m_idAcc,
																pJewel->GetidSpot(), 0 );
		if( SCENE_WORLD ) {
			if( XASSERT(pJewel) ) {
// 				XGAME::xBattleStart bs;
// #ifndef _XSINGLE
// 				bs.m_typeSpot = pJewel->GettypeSpot();
// 				bs.m_idSpot = pJewel->GetidSpot();
// #endif // not _XSINGLE
// 				bs.m_idEnemy = pJewel->GetidOwner();
// 				bs.m_Level = pJewel->GetLevel();
// 				bs.m_strName = pJewel->GetstrName();
// 				bs.m_spLegion[0] = ACCOUNT->GetCurrLegion();
// 				bs.m_spLegion[1] = spLegion;
// 				bs.m_typeBattle = XGAME::xBT_NORMAL;
// 				bs.m_Defense = pJewel->GetDefense();
// 				XSceneBattle::sSetBattleStart( bs );

				XVector<XSPLegion> aryLegion;
				aryLegion.push_back( ACCOUNT->GetCurrLegion() );
				aryLegion.push_back( spLegion );
				auto spParam = std::make_shared<XGAME::xSceneBattleParam>( pJewel->GetidOwner(),
																																	pJewel->GettypeSpot(),
																																	pJewel->GetidSpot(),
																																	pJewel->GetLevel(),
																																	pJewel->GetstrName(),
																																	aryLegion,
																																	XGAME::xBT_NORMAL,
																																	pJewel->GetDefense(),
																																	-1, 0 );
				SCENE_WORLD->OnRecvBattleInfo( spParam );
			}
		}
	} else {
		XWND_ALERT("%s", _T("not found enemy"));
	}
	//
}
/**
 전투종료후 플레이 데이타를 서버로 보내고 전투 결과를 받는다.
*/
BOOL XSockGameSvr::SendReqFinishBattle( XWnd *pTimeoutCallback, xBattleFinish& battle )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_INGAME_REQ_FINISH_BATTLE);
	ar << battle;

	ID idKey = 
		AddResponse( xCL2GS_INGAME_REQ_FINISH_BATTLE, 
					&XSockGameSvr::RecvFinishBattle, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_SPOT_INFO,		// 유황 encounter용
					&XSockGameSvr::RecvSpotInfo, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_INGAME_CANCEL_BATTLE,
					nullptr, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_CANCEL_KILL,		// 소탕이 취소됨
		&XSockGameSvr::RecvCancelKill, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 전투 결과(모든 스팟 공통)
*/
void XSockGameSvr::RecvFinishBattle( XPacket& p, const xCALLBACK& c )
{
	CONSOLE("%s", __TFUNC__);
	XGAME::xBattleResult result;
	BYTE b0;
	WORD w0;
	if( XBREAK( sGetpWorld() == nullptr ) )
		return;
	auto typeSpot = result.typeSpot;
	XBREAK( typeSpot == XGAME::xSPOT_VISIT );
	p >> result;
	// 가급적 서버에서 주는값으로 표시하려고 직접 갱신시킴.
	if( XASSERT(result.m_powerAccIncludeEmpty > 0 ) )
		ACCOUNT->SetPowerIncludeEmpty( result.m_powerAccIncludeEmpty );
	if( XASSERT( result.m_powerAccExcludeEmpty > 0 ) )
		ACCOUNT->SetPowerExcludeEmpty( result.m_powerAccExcludeEmpty );
	if( result.logForAttacker.m_bKill ) {
		ACCOUNT->DestroyItem( _T("item_kill"), 1 );	// 서버에서 처리해서 동기화하는게 클라 의존도를 줄이고 좋을듯.
	}
	// 업데이트된 인벤목록
	if( result.arUpdated.IsHave() ) {
		char c0;
		auto& ar = result.arUpdated;
		ar >> c0;		int sizeUpdated = c0;
		ar >> b0;		int verItem = b0;
		ar >> w0;
		XBREAK( result.IsLose() && sizeUpdated > 0 );	// 이런경우가 있음?
		for( int i = 0; i < sizeUpdated; ++i )
			ACCOUNT->DeserializeUpdateItem( ar, verItem );
	}
	// 전투결과에서 각 영웅들의 렙업 정보를 풀어서 각 영웅에게 업데이트 한다.
	result.DeSerializeHeroes( result.arHeroes, ACCOUNT );
#ifdef WIN32
	CONSOLE("drop items num=%d", result.aryDrops.size() );
	XARRAYLINEARN_LOOP_AUTO( result.aryDrops, itemBox ) {
		auto pProp = std::get<0>( itemBox );
		int numItem = std::get<1>( itemBox );
		CONSOLE("item=%s num=%d", pProp->strIdentifier.c_str(), numItem );
	} END_LOOP;
#endif // WIN32
	// 레벨업 이벤트
	if( result.bLevelupAcc ) {
		ACCOUNT->GetXFLevelObj().SetbLevelUp( TRUE );
	}
	XSpot* pBaseSpot = sGetpWorld()->GetSpot( result.idSpot );
	if( XBREAK( pBaseSpot == nullptr ) )
		return;
	// 이벤트 스팟은 별점 누적안시킴.
	if( !pBaseSpot->IsEventSpot() ) {
//		if( ACCOUNT->GetpQuestMng()->IsHaveGetStarQuest() ) {
		if( ACCOUNT->IsAbleGetStar() ) {
			int numOld = ACCOUNT->GetNumSpotStar( pBaseSpot->GetidSpot() );
			// 나중에 스팟 별 정보만 효과적으로 동기화시킬수 있도록 간략화 한다.
			if( result.numStar > numOld ) {
				ACCOUNT->SetNumStarBySpot( result.idSpot, result.typeSpot, result.numStar );
			}
		}
	}
	ACCOUNT->SetnumStar( result.m_totalStar );
	ACCOUNT->SetaryResource( result.aryAccRes );
	XSpot::sDeSerialize( result.arSpot, pBaseSpot );
	// 전투에 이긴후엔 스팟에 군단정보가 없어야 정상임.
#if _DEV_LEVEL <= DLV_LOCAL
	XBREAK( result.IsWin() && pBaseSpot->GetspLegion() != nullptr );
#endif
//	}
	//////////////////////////////////////////////////////////////////////////
	if( typeSpot == XGAME::xSPOT_CASTLE ) {
		XSpotCastle *pSpot = SafeCast<XSpotCastle*, XSpot*>( pBaseSpot );
	} else
	if( typeSpot == XGAME::xSPOT_JEWEL )
	{
		XSpotJewel *pSpot = SafeCast<XSpotJewel*, XSpot*>( pBaseSpot );
		XBREAK( pSpot == nullptr );
	} else
	if( typeSpot == XGAME::xSPOT_SULFUR )
	{
		XSpotSulfur *pSpot = SafeCast<XSpotSulfur*, XSpot*>( pBaseSpot );
		XBREAK( pSpot == nullptr );
	} else
	if( typeSpot == XGAME::xSPOT_MANDRAKE )
	{
		XSpotMandrake *pSpot = SafeCast<XSpotMandrake*, XSpot*>( pBaseSpot );
	} else
	if( typeSpot == XGAME::xSPOT_NPC )
	{
	} else
	if( typeSpot == XGAME::xSPOT_CAMPAIGN )
	{
	} else
	if( typeSpot == XGAME::xSPOT_COMMON ) {
	} else
	if( typeSpot == XGAME::xSPOT_VISIT )
	{
	} else
	{
		// daily/special
	}
	if( SCENE_BATTLE )
		SCENE_BATTLE->OnRecvBattleResult( result );
	if( SCENE_WORLD ) {
		SCENE_WORLD->OnRecvBattleResult( result );		// 소탕으로...
		SCENE_WORLD->SetbUpdate( true );
	}
	//
	return;
} // RecvFinishBattle

/**
 자원지가 공격받았다. 그 결과가 도착했다.
*/
void XSockGameSvr::RecvAttackedSpotResult( XPacket& p, const xCALLBACK& c )
{
}

/**
 성이 다른 유저에게 점령당했다.
*/
void XSockGameSvr::RecvAttackedCastle( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	p >> idSpot;
	//
	XSpotCastle* pSpot = sGetpWorld()->GetSpot<XSpotCastle*>( XGAME::xSPOT_CASTLE, idSpot );
	if( XBREAK( pSpot == nullptr ) )
		return;
	XSpot::sDeSerialize( p, pSpot );
	if( !pSpot->GetstrcFbUserId().empty() ) {
		pSpot->DoRequestProfileImage();
		pSpot->SetbAttacked( true );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->OnRecvAttackedCastle( idSpot, 
											pSpot->GetidOwner(), 
											pSpot->GetLevel(), 
											pSpot->GetszName() );
}

/**
 보석광산의 매칭결과가 왔다. 이것은 클라의 요청으로 온것이 아님
*/
void XSockGameSvr::RecvJewelMatchResult( XPacket& p, const xCALLBACK& c )
{
	int idxMine;
	ID idEnemy;
	p >> idxMine;
	p >> idEnemy;
	auto pSpot = sGetpWorld()->GetSpotJewelByIdx( idxMine ); 
	if( pSpot == nullptr ) {
		// 스팟이 없으면 새로 생성한다.
		auto pProp = PROP_WORLD->GetpPropJewelByIdx( idxMine ); //PROP_WORLD->GetpPropJewel(strFormat.c_str());
		XBREAK( pProp == nullptr );
		pSpot = static_cast<XSpotJewel*>( ACCOUNT->CreateNewSpot( pProp, nullptr ) );
		if( SCENE_WORLD )
			SCENE_WORLD->CreateJewelSpot( pSpot );
	}
	if( XBREAK( pSpot == nullptr ) )
		return;
	XSpot::sDeSerialize( p, pSpot );
	GAME->SetbUpdate( true );
}

void XSockGameSvr::RecvJewelReconResult( XPacket& p, const xCALLBACK& c )
{
	int isFound, idxMine;
	XArchive arPacket;
	p >> isFound;
	p >> idxMine;
	auto pSpot = sGetpWorld()->GetSpotJewelByIdx( idxMine );
	if( XBREAK(pSpot == nullptr) )
		return;
	if( isFound ) {
//		ID idAcc;
//		int level, levelMine, defense/*, idxMine*/, lootJewel, power, score;
//		_tstring strName;
		XArchive arLegion, arAbil;
		xJewelMatchEnemy infoMatch;
		p >> arLegion >> arAbil;
		p >> infoMatch;
		pSpot->SetMatch( infoMatch );
		LegionPtr spLegion( XLegion::sCreateDeserializeFull( arLegion ) );
		pSpot->SetspLegion( spLegion );   // << 추가
		if( SCENE_WORLD )
			SCENE_WORLD->OnRecvReconSpot( pSpot->GetidSpot(), spLegion );
	} else {
		XWND_ALERT("%s",_T("not found account"));
	}
}

/**
 유황 스팟에 리스폰이 되었다.
*/
void XSockGameSvr::RecvSulfurSpawn( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	p >> idSpot;
	CONSOLE("RecvSulfurSpawn:idSpot=%d", idSpot);
	auto pSpot = sGetpWorld()->GetSpot<XSpotSulfur*>( XGAME::xSPOT_SULFUR, idSpot );
	if( pSpot == nullptr )
	{
		pSpot = static_cast<XSpotSulfur*>( ACCOUNT->CreateNewSpot( idSpot, nullptr ) );
	}
	XSpot::sDeSerialize( p, pSpot );
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( true );
}

/**
 스팟의 로컬창고 자원을 수거한다.
*/
BOOL XSockGameSvr::SendReqSpotCollect( XWnd *pTimeoutCallback, ID idSpot )
{
	XSpot *pSpot = sGetpWorld()->GetSpot( idSpot );
	if( XBREAK(pSpot == nullptr) )
		return FALSE;
	XGAME::xtSpot typeSpot = pSpot->GettypeSpot();

	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_SPOT_COLLECT);
	ar << idSpot;
	ar << (DWORD)typeSpot;

	ID idKey = 
		AddResponse( xCL2GS_LOBBY_SPOT_COLLECT, 
					&XSockGameSvr::RecvSpotCollect, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 스팟의 로컬창고 자원수거의 허락이 떨어졌다.
*/
void XSockGameSvr::RecvSpotCollect( XPacket& p, const xCALLBACK& c )
{
	int sizeAry;
	DWORD dw0;
	XGAME::xtResource typeRes;
	float num;
	ID idSpot;
	XGAME::xtSpot typeSpot;

	p >> idSpot;
	p >> dw0;	typeSpot = (XGAME::xtSpot) dw0;

	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XBREAK( pBaseSpot == nullptr ) )
		return;
	std::vector<xRES_NUM> aryRes;
	p >> sizeAry;
	for( int i = 0; i < sizeAry; ++i )	{
		// 수거한 자원의 타입과 양
		p >> dw0;	typeRes = (XGAME::xtResource) dw0;
		p >> num;
		aryRes.push_back( xRES_NUM(typeRes, num) );
		//
// 		if( SCENE_WORLD )
// 			SCENE_WORLD->OnRecvSpotCollect( pBaseSpot, typeRes, num );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->OnRecvSpotCollect( pBaseSpot, aryRes );
	// 서버에서 모두 수거했으므로 클라에서 지워도 됨.
	
	// 만약 월드씬이 없으면 리소스개수 풀버전을 그대로 풀어서 동기화 시킨다.
	ACCOUNT->DeSerializeResource( p );
	if( SCENE_WORLD == nullptr )
		pBaseSpot->ClearLocalStorage();
	else
	{
		SCENE_WORLD->RecvClearStorageWithWndSpot(pBaseSpot);
//		SCENE_WORLD->Update();
	}
	GAME->DestroyWndByIdentifier( "menu.circle" );
}

/**
 스팟 하나의 동기화를 한다.
*/
void XSockGameSvr::RecvSpotSync( XPacket& p, const xCALLBACK& c )
{
	if( ACCOUNT == nullptr )
		return;
	if( sGetpWorld() == nullptr )
		return;
	ID idSpot, snSpot;
	p >> idSpot;
	p >> snSpot;
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( snSpot ) {
		if( pBaseSpot == nullptr ) {
			// 없으면 만듬.
			pBaseSpot = XSpot::sCreateDeSerialize( p, sGetpWorld() );
			ACCOUNT->AddSpot( pBaseSpot );
			ACCOUNT->AddSpotStar( idSpot );
		} else {
			if( XBREAK( pBaseSpot->GetsnSpot() != snSpot ) )
				return;
			// 이미 있으면 업데이트 함.
			XSpot::sDeSerialize( p, pBaseSpot );
		}
	} else {
		// snSpot이 0으로 오면 삭제명령(뭔가좀 이상하군)
		pBaseSpot->SetbDestroy( true );
//		sGetpWorld()->DestroySpot( idSpot );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->OnSpotSync( idSpot );
}

void XSockGameSvr::RecvLevelSync( XPacket& p, const xCALLBACK& c )
{
	ACCOUNT->DeSerializeLevel( p );// GetLevel().DeSerialize( p );
//	ACCOUNT->DeSerializeTechPoint( p );
	WORD w0;
	int i0;
	p >> w0;	ACCOUNT->SetAP( w0 );
	p >> w0;	ACCOUNT->SetmaxAP( w0 );
	p >> i0;	ACCOUNT->SetPowerIncludeEmpty( i0 );
	p >> i0;	ACCOUNT->SetPowerExcludeEmpty( i0 );
	ACCOUNT->DeSerializeUnlockPoint( p, VER_ETC_SERIALIZE );
	GAME->SetbUpdate( true );
}

void XSockGameSvr::RecvSyncBaseInfo( XPacket& p, const xCALLBACK& c )
{
	ACCOUNT->DeSerializeLevel( p );
//	ACCOUNT->DeSerializeTechPoint( p );
	if( ACCOUNT->GetbLevelUp() )
	{
		// 레벨업한 상황
		GAME->SetbLevelUp( true );
	}
	DWORD gold, cash;
	int dw0, score;
	p >> gold >> cash;
	p >> dw0;		ACCOUNT->SetPowerIncludeEmpty( dw0 );
	p >> dw0;		ACCOUNT->SetPowerExcludeEmpty( dw0 );
	p >> score;
	ACCOUNT->SetGold( gold );
	ACCOUNT->SetCashtem( cash );
//	ACCOUNT->SetptBrave( ptBrave );
	ACCOUNT->SetLadder( score );
	ACCOUNT->DeSerializeResource( p );
	ACCOUNT->DeSerializeUnlockPoint( p, VER_ETC_SERIALIZE );
	if( GAME )
		GAME->SetbUpdate( true );
}

/**
 @brief 각종 파라메터의 동기화 요청.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqSync( XWnd *pTimeoutCallback, xtParamSync type
																											, int param )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_SYNC );
	ar << (int)type;
	ar << param;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSyncAcc, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

void XSockGameSvr::RecvSyncAcc( XPacket& p, const xCALLBACK& c )
{
	int i0;
	short s0;
	p >> i0;		const auto type = (XGAME::xtParamSync)i0;
	switch( type ) {
	case xPS_ACC_LEVEL:		ACCOUNT->DeSerializeLevel( p );		break;
	case xPS_AP: {
		p >> s0;		ACCOUNT->SetAP( s0 );
		p >> s0;		ACCOUNT->SetmaxAP( s0 );
	} break;		
	case xPS_RESROUCE:		ACCOUNT->DeSerializeResource( p );		break;
	case xPS_TRADER:	
		p >> i0;	
		if( !SCENE_TRADER ) {
			XAccount::s_bTraderArrived = ( i0 != 0 );
		}
		ACCOUNT->DeserializeTimerByTrader( p );		break;
	default:
		break;
	}
#if _DEV_LEVEL <= DLV_DEV_PERSONAL
	CONSOLE("동기화:type=%d", type );
#endif
	if( GAME->GetpScene() ) {
		GAME->GetpScene()->OnRecvSyncAcc( type );
	}
	GAME->SetbUpdate( true );
}

/**
 만드레이크 자원지의 매칭결과가 왔다. 이것은 클라의 요청으로 온것이 아님
*/
void XSockGameSvr::RecvMandrakeMatchResult( XPacket& p, const xCALLBACK& c )
{
	int idxMandrake;
	p >> idxMandrake;
	//
	auto pSpot = sGetpWorld()->GetSpotMandrakeByIdx( idxMandrake ); 
	if( pSpot == nullptr )
	{
		// 스팟이 없으면 새로 생성한다.
		auto pProp = PROP_WORLD->GetpPropMandrakeByIdx( idxMandrake ); 
		XBREAK( pProp == nullptr );
		pSpot = static_cast<XSpotMandrake*>( ACCOUNT->CreateNewSpot( pProp, nullptr ) );
		if( SCENE_WORLD )
			SCENE_WORLD->CreateMandrakeSpot( pSpot );
	}
	if( XBREAK( pSpot == nullptr ) )
		return;
	XSpot::sDeSerialize( p, pSpot );
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
}

/**
 만드레이크 스팟의 군단 정보가 옴.
	bBattle에 따라 정찰인지, 공격전 군단정보인지 나뉨.
*/
void XSockGameSvr::RecvMandrakeLegionResult( XPacket& p, const xCALLBACK& c )
{
	XArchive arDB;
	ID idEnemy, snSpot, idSpot, snSession;
	int level, win, reward, idxMandrake/*, bBattle*/, idxLegion;
	_tstring strName;
	p >> idEnemy;
	p >> idxMandrake;
	p >> snSpot;
	p >> idSpot;
	p >> snSession;
//	p >> bBattle;	// 0:정찰  1:전투
	if( idEnemy ) {
		p >> arDB;
		arDB >> level;
		arDB >> strName;
		arDB >> win;
		arDB >> reward;
		arDB >> idxLegion;
		LegionPtr spLegion;
		{
			XArchive arLegion;
			arDB >> arLegion;
			auto pLegion = XLegion::sCreateDeserializeFull( arLegion );
			spLegion = LegionPtr( pLegion );
		}
		XArchive arAbil;
		arDB >> arAbil;
		if( snSession ) {
			ACCOUNT->SetBattleSession( snSession, spLegion, idEnemy, idSpot );
		}
		auto pSpot = sGetpWorld()->GetSpotMandrakeByIdx( idxMandrake );
		if( XASSERT( pSpot ) ) {
			XBREAK( pSpot->GetsnSpot() != snSpot );
			pSpot->SetspLegion( spLegion );
			if( SCENE_WORLD ) {
				if( snSession ) {		// battle mode
// 					XGAME::xBattleStart bs;
// #ifndef _XSINGLE
// 					bs.m_typeSpot = pSpot->GettypeSpot();
// 					bs.m_idSpot = pSpot->GetidSpot();
// #endif // not _XSINGLE
// 					bs.m_idEnemy = idEnemy;
// 					bs.m_Level = level;
// 					bs.m_strName = strName;
// 					bs.m_spLegion[ 0 ] = ACCOUNT->GetCurrLegion();
// 					bs.m_spLegion[ 1 ] = spLegion;
// 					bs.m_typeBattle = XGAME::xBT_NORMAL;
// 					XSceneBattle::sSetBattleStart( bs );
// 					SCENE_WORLD->OnRecvBattleInfo();
					XVector<XSPLegion> aryLegion;
					aryLegion.push_back( ACCOUNT->GetCurrLegion() );
					aryLegion.push_back( spLegion );
					auto spParam = std::make_shared<XGAME::xSceneBattleParam>( idEnemy,
																																		 pSpot->GettypeSpot(),
																																		 pSpot->GetidSpot(),
																																		 level,
																																		 strName,
																																		 aryLegion,
																																		 XGAME::xBT_NORMAL,
																																		 0, -1, 0 );
					SCENE_WORLD->OnRecvBattleInfo( spParam );
				} else {
					SCENE_WORLD->OnRecvReconSpot( pSpot->GetidSpot(), spLegion );
				}
			}
		}
	} else {
		XWND_ALERT( "not found enemy!" );
	}
}

/**
 NPC 스팟에 리스폰이 되었다.
*/
void XSockGameSvr::RecvNpcSpawn( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	p >> idSpot;
	CONSOLE("RecvNpcSpawn:idSpot=%d", idSpot);
	auto pSpot = sGetpWorld()->GetSpot<XSpotNpc*>( XGAME::xSPOT_NPC, idSpot );
	if( pSpot == nullptr )
	{
		pSpot = static_cast<XSpotNpc*>( ACCOUNT->CreateNewSpot( idSpot, nullptr ) );
	}
	XSpot::sDeSerialize( p, pSpot );
	if (SCENE_WORLD)
	{
		SOUNDMNG->OpenPlaySoundBySec(6, 1.f);
		SCENE_WORLD->SetbUpdate( true );
// 		SCENE_WORLD->UpdateNpcSpot(pSpot);
	}
}

/**
 스페셜 스팟의 데이타에 변동이 생겼다.(주로 입장횟수 리차징에 사용)
*/
// void XSockGameSvr::RecvUpdateSpecialSpot( XPacket& p, const xCALLBACK& c )
// {
// 	ID idSpot, snSpot;
// 	int numEnterTicket;
// 
// 	p >> idSpot;
// 	p >> snSpot;
// 	p >> numEnterTicket;
// 	XSpotSpecial *pSpot = sGetpWorld()->GetpSpotSpecial( idSpot );
// 	if( XBREAK( pSpot == nullptr ) )
// 		return;
// 	pSpot->SetnumEnterTicket( numEnterTicket );
// 	if( SCENE_WORLD )
//     SCENE_WORLD->SetbUpdate( true );
// //		SCENE_WORLD->UpdateSpots();
// 
// 
// }

/**
 구름을 오픈하려 한다.
 @param termsPay 지불방법
*/
BOOL XSockGameSvr::SendReqOpenCloud( XWnd *pTimeoutCallback, ID idCloud, xtTermsPayment termsPay )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_OPEN_CLOUD );
	ar << idCloud;
	XBREAK( termsPay > 0xff );
	ar << (char)termsPay;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;

	ID idKey = 
		AddResponse( ar.GetidPacket(),
					&XSockGameSvr::RecvOpenCloud, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

void XSockGameSvr::RecvOpenCloud( XPacket& p, const xCALLBACK& c )
{
	ID idCloud;
	DWORD dwGold;
	int cash;
	char c0;
	p >> idCloud;
	p >> c0;		auto err = (xtError)c0;
	p >> c0;		int lvLimit = c0;
	p >> c0;		auto termsPay = (xtTermsPayment)c0;
	p >> c0;
	p >> dwGold;
	p >> cash;
	ACCOUNT->DeSerializeUnlockPoint( p, 0 );
//	p >> lvLimit;
	//
	auto cashAdd = cash - ACCOUNT->GetCashtem();
	ACCOUNT->SetGold( dwGold );
	ACCOUNT->SetCashtem( cash );
	ACCOUNT->SetlvLimit( lvLimit );
	//
	sGetpWorld()->DoOpenCloud( idCloud );
	// 구름 오픈 성공
	GAME->SetbUpdate( true );
	if( SCENE_WORLD ) {
		SCENE_WORLD->OnRecvOpenCloud( idCloud, dwGold, cashAdd );
	}
	auto pPropArea = PROP_CLOUD->GetpProp( idCloud );
	if( XASSERT( pPropArea ) ) {
		_tstring idsArea = pPropArea->strIdentifier;
		std::string cidsArea = SZ2C( idsArea );
		GAME->DispatchEvent( XGAME::xAE_OPEN_AREA, idCloud, cidsArea );
	}
	// 키 아이템이 있으면 지운다.
	{
		auto pProp = PROP_CLOUD->GetpProp( idCloud );
		if( pProp && !pProp->idsItem.empty() ) {
			auto bOk = ACCOUNT->DestroyItem( pProp->idsItem.c_str(), 1 );
			XBREAK( !bOk );
		}
	}

}

void XSockGameSvr::RecvUnlockMenu( XPacket& p, const xCALLBACK& c )
{
	DWORD dw0;
	p >> dw0;	auto bitUnlock = ( XGAME::xtMenus )dw0;
	ACCOUNT->GetbitUnlockMenu().DeSerialize( p );
	ACCOUNT->DeSerializeUnlockPoint( p, VER_ETC_SERIALIZE );
	if( bitUnlock ) {
		// bitUnlock비트의 기능이 잠금해제 됨.
		_tstring strMsg;
		switch( bitUnlock ) {
		case XGAME::xBM_CATHEDRAL:
			// xxx유닛을 사용할수 있게 되었습니다.
			strMsg = XE::Format( XTEXT( 2141 ), XGAME::GetStrUnit( XGAME::xUNIT_PALADIN ) );
			break;
		case XGAME::xBM_LABORATORY: {
			// xxx 기능을 사용할수 있게 되었습니다.
// 			strMsg = XE::Format( XTEXT( 2129 ), XGAME::GetstrLockMenu( ( XGAME::xtMenus )bitUnlock ) );
			if( SCENE_WORLD )
				SCENE_WORLD->OnRecvUnlockMenu( bitUnlock );
		} break;
		case XGAME::xBM_BARRACK:
		case XGAME::xBM_TAVERN:
		case XGAME::xBM_ACADEMY:
		case XGAME::XBM_UNIT_HANGOUT:
		case XGAME::xBM_EMBASSY:
		case XGAME::xBM_MARKET:
//		case XGAME::xBM_TRAING_CENTER:
			// xxx 기능을 사용할수 있게 되었습니다.
			strMsg = XE::Format( XTEXT( 2129 ), XGAME::GetstrLockMenu( ( XGAME::xtMenus )bitUnlock ) );
			break;
		default:
			XBREAK( 1 );
			break;
		}
		// 잠금해제 안내 팝업
		if( !strMsg.empty() ) {
			auto pPopup = XWND_ALERT( "%s", strMsg.c_str() );
			if( pPopup ) {
				pPopup->SetEvent( XWM_OK, GAME, &XGame::OnClickUnlockMenu, (DWORD)bitUnlock );
			}
		}
	}
}

// BOOL XSockGameSvr::SendReqAddHeroExp(XWnd *pCallback, ID snHero, XArrayN<XBaseItem*, 5>& ary, ID idWnd)
// {
// 	_XCHECK_CONNECT(0);
// 
// 	XPacket ar((ID)xCL2GS_LOBBY_ADD_HERO_EXP);
// 	ar << 0;	//idWnd;
// 	ar << snHero;
// 	ar << ary.GetMax();
// 	XARRAYN_LOOP( ary, XBaseItem*, pItem )
// 	{
// 		if( pItem )
// 			ar << pItem->GetsnItem();
// 		else
// 			ar << (ID)0;
// 	} END_LOOP;
// 
// 	ID idKey =
// 		AddResponse( ar.GetidPacket(),
// 					&XSockGameSvr::RecvAddHeroExp, pCallback );
// 	Send( ar );
// 
// 	return TRUE;
// }


// void XSockGameSvr::RecvAddHeroExp( XPacket& p, const xCALLBACK& c )
// {
// 	ID snHero;
// 	XArrayN<ID, 5> ary;
// 	BOOL bLevelup;
// 	DWORD dw0;
// 
// 	p >> bLevelup;
// 	p >> dw0;	//idWnd;
// 	p >> snHero;
// 	p >> ary;			// 삭제해야할 책 리스트
// 	XHero *pHero = ACCOUNT->GetHero( snHero );
// 	pHero->DeSerializeLevel( p );
// 	pHero->DeSerializeLevelupReady( p );
// 	XBREAK( pHero->GetXFLevelObj().IsFullExp() && !pHero->IsLevelupLevelReady() );
// 	XARRAYN_LOOP( ary, ID, snItem )
// 	{
// 		if( snItem )
// 			ACCOUNT->DestroyItemBySN( snItem );
// 	} END_LOOP;
// 
// 	if (SCENE_UNITORG)
// 		SCENE_UNITORG->OnRecvAddHeroExp(0, pHero, bLevelup);
// }

BOOL XSockGameSvr::SendReqChangeHeroLegion(XWnd *pTimeoutCallback, ID idHero, XGAME::xtUnit unit, ID idWnd )
{
	_XCHECK_CONNECT(0);

	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_HERO_LEGION);
	ar << idWnd;
	ar << idHero;
	ar << (ID)unit;

	ID idKey =
		AddResponse(ar.GetidPacket(),
					&XSockGameSvr::RecvChangeHeroLegion, pTimeoutCallback);
	Send(ar);

	return TRUE;
}


void XSockGameSvr::RecvChangeHeroLegion( XPacket& p, const xCALLBACK& c )
{
	ID snHero, idWnd;
	//ID idLegion;
	DWORD dw0;
	XGAME::xtUnit unit;
	int numUnit;

	p >> idWnd;
	p >> snHero;
	p >> dw0;	unit = (XGAME::xtUnit) dw0;
	p >> numUnit;

	XHero *pHero = ACCOUNT->GetHero( snHero );
	pHero->SetUnit( unit );
//	pHero->SetnumUnit( numUnit );
// 	XList<XHero*> listHero;
// 	ACCOUNT->GetInvenHero(listHero);
// 	XLIST_LOOP(listHero, XHero*, pHero)
// 	{
// 		if (snHero == pHero->GetsnHero())
// 			pHero->SetUnit((XGAME::xtUnit)idLegion);
// 	} END_LOOP;

	if (SCENE_UNITORG)
		SCENE_UNITORG->OnRecvChangeHeroLegion(idWnd);
}

/**
 @brief 영웅소환 요청
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param typeSummon 0:금화소환 1:캐쉬소환
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqSummonHero( XWnd *pTimeoutCallback, XGAME::xtGatha typeSummon )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_SUMMON_HERO );
	ar << (int)typeSummon;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSummonHero, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSummonHero()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSummonHero()
*/
void XSockGameSvr::RecvSummonHero( XPacket& p, const xCALLBACK& c )
{
	DWORD gold, cashtem;
	XHero *pHero = nullptr;
	ID idPropHero = 0;
	BYTE b0;
	p >> idPropHero;
	p >> b0;	bool bPiece = xbyteToBool( b0 );
	p >> b0;	auto type = (XGAME::xtGatha)b0;
	int numPiece = 0;
	if( bPiece ) {
		WORD w0;
		p >> w0;	numPiece = w0;
		ACCOUNT->CreatePieceItemByidHero( idPropHero, numPiece );
	} else {
		WORD w0;
		p >> w0; 
		pHero = XHero::sCreateDeSerialize( p, ACCOUNT );
		XBREAK( idPropHero != pHero->GetidProp() );
	}
	p >> gold;
	p >> cashtem;
	ACCOUNT->SetGold( gold );
	ACCOUNT->SetCashtem( cashtem );
	ACCOUNT->GetFlagTutorial().bSummonHero = 1;
	GAME->GetpScene()->SetbUpdate( true );
	//임시로 소환 타입에 0넣어줌 이거 서버에서 받아서 넣어줘야함
	if (SCENE_HERO) {
		ID snHero = 0;
		if( pHero )
			snHero = pHero->GetsnHero();
		SCENE_HERO->OnRecvSummon( bPiece, idPropHero, snHero, type, numPiece );
	}
}

/**
 @brief snHero영웅을 idxLegion의 idxSlot에 새로 생성한다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqNewSquad( XWnd *pTimeoutCallback, ID snHero, int idxSlot, int idxLegion )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_LOBBY_NEW_SQUAD );
	ar << snHero << idxSlot << idxLegion;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvNewSquad, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqNewSquad()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqNewSquad()
*/
void XSockGameSvr::RecvNewSquad( XPacket& p, const xCALLBACK& c )
{
	ID snHero;
	int idxSlot, idxLegion;
	p >> snHero >> idxSlot >> idxLegion;
	XHero *pHero = ACCOUNT->GetHero( snHero );
	if( XBREAK( pHero == NULL ) )
		return;
	XLegion *pLegion = ACCOUNT->GetLegionByIdx( idxLegion ).get();
	if( XBREAK( pLegion == NULL ) )
		return;
	if( SCENE_LEGION )
	{
		//SCENE_LEGION->CreateSquadToLegion( pHero, pLegion, idxSlot );
	}
	XSquadron *pSq = new XSquadron( pHero );
	pLegion->AddSquadron( idxSlot, pSq, FALSE );
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
BOOL XSockGameSvr::SendReqMoveSquad( XWnd *pTimeoutCallback, 
									int idxSrc, 
									int idxDst, 
									int idxLegion, 
									ID snHeroSrc, 
									ID snHeroDst )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_LOBBY_MOVE_SQUAD );
	ar << idxSrc << idxDst << idxLegion << snHeroSrc << snHeroDst;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvMoveSquad, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqMoveSquad()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqMoveSquad()
*/
void XSockGameSvr::RecvMoveSquad( XPacket& p, const xCALLBACK& c )
{
	int idxSrc, idxDst, idxLegion;
	ID snHeroSrc, snHeroDst;
	p >> idxSrc >> idxDst >> idxLegion >> snHeroSrc >> snHeroDst;
	//
	XLegion *pLegion = ACCOUNT->GetLegionByIdx( idxLegion ).get();
	if( XBREAK( pLegion == NULL ) )
		return;
	
	if( idxDst == -1 )
		pLegion->DestroySquadBysnHero( snHeroSrc );
	else
	{
		// swap
		pLegion->SwapSlotSquad( idxSrc, idxDst );
	}

	if (SCENE_LEGION)
	{
		//SCENE_LEGION->MoveSquadInLegion(idxSrc, idxDst, snHeroSrc, snHeroDst);
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
BOOL XSockGameSvr::SendReqTrade(XWnd *pTimeoutCallback, int wood, int iron, int jewel, int sulfur, int mandrake)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_TRADE  );
	//ar << (int)type << amount;

	ar << wood;
	ar << iron;
	ar << jewel;
	ar << sulfur;
	ar << mandrake;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvTrade, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqTrade()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqTrade()
*/
void XSockGameSvr::RecvTrade(XPacket& p, const xCALLBACK& c)
{
	int bSuccess;
	p >> bSuccess;
	ACCOUNT->DeserializeTimerByTrader( p );
	if (bSuccess) {
		// 통째로 동기화
		ACCOUNT->DeSerializeResource(p);
		DWORD gold = 0;
		p >> gold;
		ACCOUNT->SetGold(gold);
		int secRemain = ACCOUNT->GetsecRemainByTraderRecall();
		_tstring msg;
		msg = XTEXT(90000);
		msg += _T("/");
		msg += XTEXT(90005);
		SendRegistPushMsg(NULL
										, ACCOUNT->GetidAccount()
										, XGAME::xTRADER_RETURN
										, 0
										, msg
										, secRemain);
	} else {
	}
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
	if (SCENE_TRADER) {
		SCENE_TRADER->OnClickTradeCancel(NULL, 0, 0);
		SCENE_TRADER->UpdateCharacter();
		SOUNDMNG->OpenPlaySound(13);		// 거래완료 소리
	}
	GAME->SetbUpdate( true );
}

/**
 @brief clan클랜징표를 모두 소모해서 책으로 교환한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param clan 소모해야할 징표의 클랜
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
// BOOL XSockGameSvr::SendReqChangeScalpToBook( XWnd *pTimeoutCallback, XGAME::xtClan clan )
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar( (ID)xCL2GS_LOBBY_CHANGE_SCALP_TO_BOOK );
// 	ar << (int)clan;
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey = 
// 		AddResponse( ar.GetidPacket(), 
// 					&XSockGameSvr::RecvChangeScalpToBook, pTimeoutCallback );
// 	Send( ar );
// 	//
// 	return TRUE;
// }
// 
// /**
//  SendReqChangeScalpToBook()에 대한 응답함수
//  @param p 패킷이 들어있는 아카이브
//  @see SendReqChangeScalpToBook()
// */
// void XSockGameSvr::RecvChangeScalpToBook( XPacket& p, const xCALLBACK& c )
// {
// 	int size, ver, numDel;
// 	BYTE b0;
// 	XGAME::xtClan clan;
// 	ID idScalp;
// 	p >> b0;	clan = ( XGAME::xtClan )b0;
// 	p >> b0;	size = b0;;
// 	p >> b0;	ver = b0;
// 	p >> b0;	numDel = b0;
// 	for( int i = 0; i < size; ++i )
// 	{
// 		XBaseItem *pItem = ACCOUNT->DeserializeAddItem( p, ver );
// // 		XBaseItem *pItem = XBaseItem::sCreateDeSerialize( p, ver );
// // 		ACCOUNT->AddItem( pItem );
// 	}
// 	p >> idScalp;
// 	ACCOUNT->DestroyItem( idScalp, numDel );
// 	if( SCENE_WORLD )
// 	{
// //		SCENE_WORLD->SetbUpdate( TRUE );
// 		SCENE_WORLD->OnRecvPacket( c.idPacket );
// 	}
// }

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
// BOOL XSockGameSvr::SendReqUpgradeSquad( XWnd *pTimeoutCallback, ID snHero )
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar( (ID) xCL2GS_LOOBY_UPGRADE_SQUAD );
// 	ar << snHero;
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey = 
// 		AddResponse( ar.GetidPacket(), 
// 					&XSockGameSvr::RecvUpgradeSquad, pTimeoutCallback );
// 	Send( ar );
// 	//
// 	return TRUE;
// }
// 
// /**
//  SendReqUpgradeSquad()에 대한 응답함수
//  @param p 패킷이 들어있는 아카이브
//  @see SendReqUpgradeSquad()
// */
// void XSockGameSvr::RecvUpgradeSquad( XPacket& p, const xCALLBACK& c )
// {
// 	ID snHero, idNeed;
// 	int levelSquad, numItem;
// 	p >> snHero;
// 	p >> idNeed >> numItem;		// 필요아이템과 필요개수.
// 	p >> levelSquad;
// 	{
// 		XBREAK( levelSquad != 0 );
// 		BYTE b0;
// 		int ver,idx;
// 		p >> b0;	ver = b0;
// 		p >> b0;	idx = b0;
// 		p >> b0 >> b0;
// 		ACCOUNT->DeSerializeTrainSlot( p, ver );
// 		ACCOUNT->DeSerializeResource( p );
// 		GAME->SetbUpdate( true );
// 	}
// }

/**
 @brief 영웅 소환 해제
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param idHero 해제시킬 영웅
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqReleaseHero( XWnd *pTimeoutCallback, ID idHero )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_LOBBY_RELEASE_HERO );
	ar << idHero;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvReleaseHero, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqReleaseHero()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqReleaseHero()
*/
void XSockGameSvr::RecvReleaseHero( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	int ver, num;
	ID snHero;
	p >> b0;	ver = b0;
	p >> b0;	num = b0;
	p >> b0 >> b0;
	ACCOUNT->DeserializeAddItem( p, ver );
// 	XBaseItem *pItem = XBaseItem::sCreateDeSerialize( p, ver );
// 	ACCOUNT->AddItem( pItem );
	p >> snHero;
	if( SCENE_UNITORG )
		SCENE_UNITORG->OnRecvReleaseHero( snHero );
	ACCOUNT->DestroyHero( snHero );
}

void XSockGameSvr::RecvAddItem( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	int ver, num;
	p >> b0;	ver = b0;
	p >> b0;	num = b0;
	p >> b0 >> b0;
	XBaseItem *pItem = ACCOUNT->DeserializeAddItem( p, ver );
	pItem->SetbNew( true );
// 	XBaseItem *pItem = XBaseItem::sCreateDeSerialize( p, ver );
// 	pItem->SetbNew(true);
// 	ACCOUNT->AddItem( pItem );
}

/**
 @brief 여러개의 아이템을 받는다.
*/
void XSockGameSvr::RecvItemsList( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	int ver, num;
	p >> b0;	ver = b0;
	p >> b0;	
	p >> b0 >> b0;
	p >> num;
	for( int i = 0; i < num; ++i )
	{
		XBaseItem *pItem = ACCOUNT->DeserializeAddItem( p, ver );
		pItem->SetbNew( true );
	}
}

/**
 @brief 서버의 아이템 인벤 전체를 동기화 한다.
*/
void XSockGameSvr::RecvItemItemsSync( XPacket& p, const xCALLBACK& c )
{
	ACCOUNT->DeSerializeItems2( p );

}

/**
 @brief snHero의 스킬 레벨업을 서버에 요청한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param snHero 업그레이드 할 영웅의 sn번호
 @param typeSkill 0:passive 1:active
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
// BOOL XSockGameSvr::SendReqLevelUpSkill( XWnd *pTimeoutCallback, ID snHero, int typeSkill )
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar( (ID)xCL2GS_LOBBY_LEVELUP_SKILL );
// 	ar << snHero;
// 	ar << typeSkill;
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey = 
// 		AddResponse( ar.GetidPacket(), 
// 					&XSockGameSvr::RecvLevelUpSkill, pTimeoutCallback );
// 	Send( ar );
// 	//
// 	return TRUE;
// }
// 
// /**
//  SendReqLevelUpSkill()에 대한 응답함수
//  @param p 패킷이 들어있는 아카이브
//  @see SendReqLevelUpSkill()
// */
// void XSockGameSvr::RecvLevelUpSkill( XPacket& p, const xCALLBACK& c )
// {
// 	BYTE b0;
// 	ID idItem, snHero;
// 	int numDel;
// 	p >> snHero;
// 	XHero *pHero = ACCOUNT->GetHero( snHero );
// 	XBREAK(pHero == nullptr);
// 	p >> b0;	
// 	if( b0 == 0 )
// 	{
// 		int idxSlot;
// 		int verEtc;
// 		p >> b0;	idxSlot = b0;
// 		p >> b0;	numDel = b0;
// 		p >> b0;	verEtc = b0;	
// 		p >> idItem;
// 		ACCOUNT->DeSerializeTrainSlot( p, verEtc );
// 		ACCOUNT->DeSerializeResource( p );
// 	} else
// 	{
// 		pHero->SetlvPassive( b0 );
// 		p >> b0;	pHero->SetlvActive( b0 );
// 		p >> b0;	numDel = b0;
// 		p >> b0;
// 		p >> idItem;
// 	}
// 	XBREAK( idItem == 0 );
// 	XBREAK( numDel == 0 );
// 	int ret = ACCOUNT->DestroyItem( idItem, numDel );
// 	XBREAK( ret == 0 );
// 
// 	if( SCENE_UNITORG )
// 		SCENE_UNITORG->SetbUpdate( true );
// }

BOOL XSockGameSvr::SendItemSell(XWnd *pTimeoutCallback, ID snItem)
{
	_XCHECK_CONNECT(0);

	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SELL);
	ar << snItem;

	ID idKey =
		AddResponse(ar.GetidPacket(), &XSockGameSvr::RecvItemSell, pTimeoutCallback);

	Send(ar);

	return TRUE;
}

void XSockGameSvr::RecvItemSell(XPacket& p, const xCALLBACK& c)
{
	int success = 0;
	p >> success;

	if (success)
	{
		ID snItem = 0;
		DWORD gold = 0;

		p >> snItem;
		p >> gold;

		ACCOUNT->DestroyItemBySN(snItem);
		ACCOUNT->SetGold(gold);

		if (SCENE_STORAGE)
			SCENE_STORAGE->RecvUpdate();
	}
	GAME->SetbUpdate( true );
}

BOOL XSockGameSvr::SendItemSpent(XWnd *pTimeoutCallback, ID snItem)
{
	_XCHECK_CONNECT(0);

	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SPENT);
	ar << snItem;

	ID idKey =
		AddResponse(ar.GetidPacket(), &XSockGameSvr::RecvItemSpent, pTimeoutCallback);

	Send(ar);

	return TRUE;
}

void XSockGameSvr::RecvItemSpent(XPacket& p, const xCALLBACK& c)
{
	int success = 0;
	ID snItem = 0;
	p >> success;

	if (success)
	{
		p >> snItem;
		ACCOUNT->DestroyItemBySN(snItem);

		if (SCENE_STORAGE)
			SCENE_STORAGE->RecvUpdate();
	}
}

BOOL XSockGameSvr::SendInventoryExpand(XWnd *pTimeoutCallback, XGAME::xtSpentCall type)
{
	XPacket ar((ID)xCL2GS_LOBBY_INVENTORY_EXPAND);

	ar << (int)type;

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvInventoryExpand, pTimeoutCallback);

	Send(ar);
	 
	return TRUE;
}

void XSockGameSvr::RecvInventoryExpand(XPacket& p, const xCALLBACK& c)
{
	int success = 0;
	int type = 0;
	int expandResource = 0;

	p >> success;

	if (success)
	{
		p >> type;
		p >> expandResource;

		int maxItemSize = 0;

		p >> maxItemSize;

		if (type == XGAME::xtSpentCall::xSC_SPENT_ITEM)
		{
			ACCOUNT->DestroyItem(expandResource, 1);
			ACCOUNT->SetmaxItems(maxItemSize);
		}
		else if (type == XGAME::xtSpentCall::xSC_SPENT_GEM)
		{
			ACCOUNT->AddCashtem(-expandResource);
			ACCOUNT->SetmaxItems(maxItemSize);
		}

		if (SCENE_STORAGE)
			SCENE_STORAGE->UpdateAll();
	}
	else
	{

	}
}
/**
 @brief 무기상점에서 아이템구입
 @param costType 1:금화 2:캐시 3:메달
 
*/
BOOL XSockGameSvr::SendItemBuy(XWnd *pTimeoutCallback, ID snItem, XGAME::xtShopType shopType, XGAME::xtCoin coinType )
{
	XPacket ar((ID)xCL2GS_LOBBY_ITEM_BUY);
	ar << (int)shopType;
	ar << snItem;
	ar << (int)coinType;
	Send(ar);
	return TRUE;
}

void XSockGameSvr::RecvItemBuy(XPacket& p, const xCALLBACK& c)
{
	int success = 1;
	int gold = 0;
	int cash = 0;

	p >> success;
	
	if (success)
	{
		if (ACCOUNT)
		{
			p >> gold;
			p >> cash;

			ACCOUNT->SetGold(gold);
			ACCOUNT->SetCashtem(cash);
			ACCOUNT->DeSerializeItems(p);
		}
		
		if (SCENE_ARMORY)
		{
			SCENE_ARMORY->SetbUpdate( true );
// 			SCENE_ARMORY->RecvUpdate();
// 			SCENE_ARMORY->NewListRecvUpdate(true);
		}
		if (SCENE_SHOP)
			SCENE_SHOP->RevUpdate(true);
	}
}

// 이쪽에서 보낼 필요 없음
/*
BOOL XSockGameSvr::SendArmoryListTimeChange(XWnd *pTimeoutCallback)
{
	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SHOP_LIST_TIME);

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvArmoryListTimeChange, pTimeoutCallback);

	Send(ar);

	return TRUE;
}
*/

/**
 @brief 시간이 되어서 무기점 아이템 리스트가 갱신됨.
*/
void XSockGameSvr::RecvArmoryListTimeChange(XPacket& p, const xCALLBACK& c)
{
	if( XBREAK(ACCOUNT == nullptr) )	// 이런경우가 있으면 갱신이 안될수 있으니 이렇게 하면 안됨.
		return;
	ACCOUNT->DeSerializeShopList( p );
	if( SCENE_ARMORY )
		SCENE_ARMORY->NewListRecvUpdate( true );

	_tstring msg;
	msg = XTEXT( 90000 );
	msg += _T( "/" );
	msg += XTEXT( 90006 );
	SendRegistPushMsg( NULL, ACCOUNT->GetidAccount(), XGAME::xARMORY_RETURN, 0, msg, 60 * 60 * 12 );
	if( SCENE_ARMORY )
		SCENE_ARMORY->SetbUpdate( true );
// 	int bSuccess = false;
// 
// 	p >> bSuccess;
// 
// 	if (bSuccess)
// 	{
// 		if (ACCOUNT)
// 			ACCOUNT->DeSerializeShopList(p);
//  		if (SCENE_ARMORY)
// 			SCENE_ARMORY->NewListRecvUpdate(true);
// 
// 		_tstring msg;
// 
// 		msg = XTEXT(90000);
// 		msg += _T("/");
// 		msg += XTEXT(90006);
// 
// 		SendRegistPushMsg(NULL, ACCOUNT->GetidAccount(), XGAME::xARMORY_RETURN, 0, msg, 60 * 60 * 12);
// 	}
// 	else
// 	{
// // 		if (SCENE_ARMORY)
// // 		SCENE_ARMORY->NewListRecvUpdate(false);
// 	}
// 	if( SCENE_ARMORY )
// 		SCENE_ARMORY->SetbUpdate( true );
}

BOOL XSockGameSvr::SendArmoryListCashChange(XWnd *pTimeoutCallback, XGAME::xtSpentCall type)
{
	XPacket ar((ID)xCL2GS_LOBBY_ITEM_SHOP_LIST_CASH);

	ar << (int)type;

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvArmoryListCashChange, pTimeoutCallback);

	Send(ar);

	return TRUE;
}

void XSockGameSvr::RecvArmoryListCashChange(XPacket& p, const xCALLBACK& c)
{
	int type = 0;
	int bSuccess = 0;
	int spentResource = 0;
	DWORD numCash;
	p >> type;
	p >> bSuccess;
	p >> numCash;
	ACCOUNT->SetCashtem( numCash );
	if( bSuccess )
		ACCOUNT->DeSerializeShopList( p );
	if (type == XGAME::xSC_SPENT_ITEM) {
		if (bSuccess) {
			const ID idItemForReset = XGC->m_armoryRecallItem;
			ACCOUNT->DestroyItem(idItemForReset, 1);
		} else {
			// 서버에서 보내준 에러값으로 메시지 표시해야함.
			XBREAK(1);
		}
	} else 
	if (type == XGAME::xSC_SPENT_GEM) {
		if (bSuccess) {
		} else {
			XBREAK(1);
		}
	}
	if (bSuccess) {
		if( SCENE_ARMORY )
			SCENE_ARMORY->SetbUpdate( true );
		//
		_tstring msg;
		msg = XTEXT(90000);
		msg += _T("/");
		msg += XTEXT(90006);
		SendRegistPushMsg(NULL, ACCOUNT->GetidAccount(), XGAME::xARMORY_RETURN, 0, msg, XGC->m_armoryCallInterval);
	}
}


// BOOL XSockGameSvr::SendCashItemBuyAndroid(XWnd *pTimeoutCallback, XGlobalConst::cashItem* pCashItem)
// {
// 	XPacket ar((ID)xCL2GS_LOBBY_CASH_ITEM_BUY_IAP_AND);
// 	ar << (BYTE)XGAME::GetPlatform();
// 	ar << pCashItem->type;
// 	ar << pCashItem->m_productID;
// 	ar << pCashItem->m_gem;
// 
// 	ID idKey =
// 		AddResponse(ar.GetidPacket(),
// 		&XSockGameSvr::RecvCashItemBuyAndroid, pTimeoutCallback);
// 
// 	Send(ar);
// 
// 	return 1;
// }
// 
// void XSockGameSvr::RecvCashItemBuyAndroid(XPacket& p, const xCALLBACK& c)
// {
// 	int bSuccess = false;
// 
// 	p >> bSuccess;
// 
// 	if (bSuccess)
// 	{
// 		int cash;
// 		p >> cash;
// 		ACCOUNT->SetCashtem(cash);
// 
// 		if (SCENE_SHOP)
// 			SCENE_SHOP->RevUpdate(true);
// 	}
// }

// BOOL XSockGameSvr::SendCashItemBuyIOS(XWnd *pTimeoutCallback, XGlobalConst::cashItem* pCashItem)
// {
// 	XPacket ar((ID)xCL2GS_LOBBY_CASH_ITEM_BUY_IAP_IOS);
// 
// 	ID idKey =
// 		AddResponse(ar.GetidPacket(),
// 		&XSockGameSvr::RecvCashItemBuyIOS, pTimeoutCallback);
// 
// 	ar << pCashItem->type;
// 	ar << pCashItem->m_productID;
// 	ar << pCashItem->m_gem;
// 
// 	Send(ar);
// 
// 	return 1;
// }
// 
// void XSockGameSvr::RecvCashItemBuyIOS(XPacket& p, const xCALLBACK& c)
// {
// 	int bSuccess = false;
// 
// 	p >> bSuccess;
// 
// 	if (bSuccess)
// 	{
// 		int cash;
// 		p >> cash;
// 		ACCOUNT->SetCashtem(cash);
// 
// 		if (SCENE_SHOP)
// 			SCENE_SHOP->RevUpdate(true);
// 	}
// }

BOOL XSockGameSvr::SendReqEqiupItem(XWnd *pTimeoutCallback, ID snHero, ID snItem, ID typeAction)
{
	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_HERO_EQUIPMENT);
	
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvEqiupItem, pTimeoutCallback);

	ar << snHero;
	ar << snItem;
	ar << typeAction;	//0탈착, 1장착

	Send(ar);

	return 1;
}
void XSockGameSvr::RecvEqiupItem(XPacket& p, const xCALLBACK& c)
{
	ID snHero = 0, snItem = 0, typeAction = 0;
	p >> snHero;
	p >> snItem;
	p >> typeAction;

	if (SCENE_EQUIP && snItem != 0)
		SCENE_EQUIP->RecvReqHeroEquip(snHero, snItem, typeAction);
}
/**
 @brief 퀘스트 발생
 @param p 패킷이 들어있는 아카이브
*/
void XSockGameSvr::RecvQuestOccur( XPacket& p, const xCALLBACK& c )
{
	ID idQuest;
	DWORD secUpdate;
	p >> idQuest;
	p >> secUpdate;
	///< 
	// 중복검사.
	XQuestObj *pObj = ACCOUNT->GetpQuestMng()->CreateAndAddQuestObjCurr( idQuest );
	if( XASSERT(pObj) )	{
		pObj->SetsecUpdate( secUpdate );
		ACCOUNT->GetpQuestMng()->SortBysecUpdate();
		if( !pObj->IsRepeat() )
			pObj->SetstateAlert( XQuestObj::xMS_MUST_HAVE );
	}
	if( SCENE_WORLD ) {
		SCENE_WORLD->OnOccurQuest( pObj );
	}
}

/*
BOOL XSockGameSvr::SendPostInfo(XWnd *pTimeoutCallback)
{
	XPacket ar((ID)xCL2GS_LOBBY_POST_INFO);

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvPostInfo, pTimeoutCallback);

	Send(ar);

	return 1;
}
*/

void XSockGameSvr::RecvPostInfo(XPacket& p, const xCALLBACK& c)
{
	if (ACCOUNT)
		ACCOUNT->DeSerializePostInfo(p, VER_POST );
	if (SCENE_WORLD)
		SCENE_WORLD->UpdateMailBoxAlert();

	//if (SCENE_WORLD)
		//SCENE_WORLD->UpdateMailbox();
}

/*
BOOL XSockGameSvr::SendPostRead(XWnd *pTimeoutCallback, ID postID)
{
	XPacket ar((ID)xCL2GS_LOBBY_POST_READ);

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvPostRead, pTimeoutCallback);

	ar << postID;

	Send(ar);

	return 1;
}

void XSockGameSvr::RecvPostRead(XPacket& p, const xCALLBACK& c)
{
	ID postindex = 0;
	p >> postindex;

	ACCOUNT->DestroyPostInfo(postindex);
}
*/

BOOL XSockGameSvr::SendPostGetItemAll(XWnd *pTimeoutCallback, ID postID)
{
	XPacket ar((ID)xCL2GS_LOBBY_POST_GETITEM_ALL);

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvPostGetItemAll, pTimeoutCallback);

	ar << postID;

	Send(ar);

	return 1;
}

void XSockGameSvr::RecvPostGetItemAll(XPacket& p, const xCALLBACK& c)
{
	ID snPost = 0;
	int result = 0;
	int cash = 0;
	int gold = 0;
	int ptGuild;

	p >> snPost;
	p >> result;
	p >> cash;
	p >> gold;
	p >> ptGuild;
	// 메일 삭제
	ACCOUNT->DestroyPostInfo(snPost);
	if (result) {
		// 수령후 계정정보 동기화
		ACCOUNT->SetCashtem(cash);
		ACCOUNT->SetGold(gold);
		ACCOUNT->SetptGuild( ptGuild );
		ACCOUNT->DeSerializeItems(p);
		if (SCENE_WORLD) {
			SCENE_WORLD->RecvGetItem();
			SCENE_WORLD->UpdateMailBoxAlert();
		}
	}
}
/**
 @brief 퀘스트 완료
*/
void XSockGameSvr::RecvQuestComplete( XPacket& p, const xCALLBACK& c )
{
	ID idQuest;
	p >> idQuest;
	///< 
	// 중복검사.
	XQuestObj *pObj = ACCOUNT->GetpQuestMng()->GetQuestObj( idQuest );
	if( XASSERT(pObj) )
	{
		XQuestObj::sDeSerialize( pObj, p );
		pObj->SetstateAlert( XQuestObj::xMS_MUST_HAVE );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->RecvCompleteQuest( pObj );
}

/**
 @brief 퀘스트 업데이트
*/
void XSockGameSvr::RecvQuestUpdate( XPacket& p, const xCALLBACK& c )
{
	ID idQuest;
	p >> idQuest;
	///< 
	// 중복검사.
	XQuestObj *pObj = ACCOUNT->GetpQuestMng()->GetQuestObj( idQuest );
	if( XASSERT( pObj ) )
	{
		bool bCompleteOld = pObj->IsAllComplete();
		XQuestObj::sDeSerialize( pObj, p );
		// 이제 Complete까지 처리하게 바뀜.
		bool bComplete = pObj->IsAllComplete();
		if( bComplete )
			pObj->SetstateAlert( XQuestObj::xMS_MUST_HAVE );	// 느낌표생성
		else
		{
			// 이전에 완료상태였는데 다시 완료상태가 아니게 되었다.
			if( bCompleteOld )	// 걍 현재 미완료 상태면 있었든 없었든 삭제 명령
				pObj->SetstateAlert( XQuestObj::xMS_MUST_DEL );	// 느낌표삭제
		}
		pObj->SetbUpdate( true );
	}
	GAME->SetbUpdate( true );
}

/**
 @brief 완료된 퀘스트에 대한 보상을 요청한다.
 idQuest는 반드시 완료된 퀘스트여야 한다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqReqQuestReward( XWnd *pTimeoutCallback, ID idQuest )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_QUEST_REQUEST_REWARD  );
	ar << idQuest;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvReqQuestReward, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqReqQuestReward()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqReqQuestReward()
*/
void XSockGameSvr::RecvReqQuestReward( XPacket& p, const xCALLBACK& c )
{
	bool bSound = false;		//퀘템 얻었을때 나는 소리
	ID idQuest;
	int sizeReward, ver;
	BYTE b0;
	p >> idQuest;
	p >> b0;	sizeReward = b0;
	p >> b0;	ver = b0;
	p >> b0 >> b0;
//	ACCOUNT->DeSerializeLevel( p );
	for( int i = 0; i < sizeReward; ++i ) {
		XGAME::xtReward typeReward;
		int i0;
		p >> i0;	typeReward = (XGAME::xtReward)i0;
		switch( typeReward ) {
		case XGAME::xtReward::xRW_ITEM: {
			int sizeItems;
			p >> sizeItems;
			for( int i = 0; i < sizeItems; ++i ) {
				XBaseItem* pItem = ACCOUNT->DeserializeAddItem( p, ver );
				if (pItem && pItem->GetpProp()->type == XGAME::xIT_QUEST)
					bSound = true;
			}
		} break;
		case XGAME::xtReward::xRW_RESOURCE: {
			ID idReward;
			int sum;
			XGAME::xtResource typeRes;
			p >> idReward;
			p >> sum;
			typeRes = (XGAME::xtResource)idReward;
			if( typeRes == XGAME::xRES_GOLD )
				ACCOUNT->SetGold( (DWORD)sum );
			else
				ACCOUNT->SetResource( typeRes, sum );
		} break;
		case XGAME::xtReward::xRW_HERO: {
			XHero::sCreateDeSerialize( p, ACCOUNT );
		} break;
		case XGAME::xtReward::xRW_CASH: {
			DWORD numCash;
			p >> numCash;
			ACCOUNT->SetCashtem( numCash );
		} break;
		default:
			XBREAKF( 1, "unknown reward type:%d", typeReward );
			break;
		}
	}

	XQuestObj *pObj = ACCOUNT->GetpQuestMng()->GetQuestObj( idQuest );
	// 퀘리스트가 아이콘방식으로 변경되면 씬이벤트를 먼저실행해야 한다.
	if( SCENE_WORLD ) {
		if( bSound )
			SOUNDMNG->OpenPlaySoundBySec( 11, 1.f );
		else
			SOUNDMNG->OpenPlaySoundBySec( 10, 1.f );
		SCENE_WORLD->OnRecvQuestReward( pObj );
	}
	if( pObj )
		ACCOUNT->GetpQuestMng()->DestroyQuestObj( pObj );
//	pObj = nullptr;
// 	// 퀘리스트가 아이콘방식으로 변경되면 씬이벤트를 먼저실행해야 한다.
// 	if (SCENE_WORLD)
// 	{
// 		if (bSound)
// 			SOUNDMNG->OpenPlaySoundOneSec(11);
// 		else
// 			SOUNDMNG->OpenPlaySoundOneSec(10);
// 		SCENE_WORLD->OnRecvQuestReward(pObj);
// 	}
	GAME->SetbUpdate( true );
}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendSpotTouch( ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_LOBBY_SPOT_TOUCH );
	ar << idSpot;
	// 이제 모든스팟은 리퀘스트를 받도록 바뀜
	ID idKey = AddResponse( ar.GetidPacket(),
				&XSockGameSvr::RecvSpotTouch, GAME );
	AddResponse( idKey, (ID)xCL2GS_LOBBY_REQ_CAMPAIGN_GUILD_RAID,
					&XSockGameSvr::RecvReqCampaignByGuildRaid, GAME );
	Send( ar );
	//
	return TRUE;
}

/**
 @brief SendSpotVisit로 보냈을때 일부스팟은 Request를 하는경우가 있다.
*/
void XSockGameSvr::RecvSpotTouch( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	ID idSpot;
	p >> idSpot;
	p >> b0;    auto typeSub = ( XGAME::xtSpot )b0;
	p >> b0;
	p >> b0;
	p >> b0;
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XBREAK( pBaseSpot == nullptr ) )
		return;
	// 스팟 통째로 동기화 시킴
	XSpot::sDeSerialize( p, pBaseSpot );
	std::vector<xDropItem> aryDropItem;
	p >> aryDropItem;
	XArchive arAdd;		// 스팟별 추가 패킷
	p >> arAdd;
	// NPC스팟의경우 드랍목록을 받음.
	GAME->SetbUpdate( true );
	if( SCENE_WORLD )
		SCENE_WORLD->OnRecvSpotTouch( pBaseSpot, aryDropItem, arAdd );
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
BOOL XSockGameSvr::SendReqSendReqGuildRaidOpen( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_OPEN_GUILD_RAID  );
	ar << idSpot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSendReqGuildRaidOpen, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSendReqGuildRaidOpen()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSendReqGuildRaidOpen()
*/
void XSockGameSvr::RecvSendReqGuildRaidOpen( XPacket& p, const xCALLBACK& c )
{
	ID idCamp, idSpot;
	XArchive arCamp;
	p >> idCamp >> idSpot;
	p >> arCamp;
	auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		auto pSpot = SafeCast<XSpotCommon*>( pBaseSpot );
		if( pSpot ) {
// 			auto spCampObj = pSpot->GetspCampObj( ACCOUNT->GetidGuild() );
// 			if( XASSERT(spCampObj != nullptr) ) {
// 				XCampObj::sDeserializeUpdate( spCampObj, arCamp );
// 				auto pWnd = GAME->Find( "wnd.guild.raid" );
// 				if( pWnd ) {
// 					pWnd->SetbUpdate( true );
// 				}
// 			}
		}
	}
}

/**
 @brief 길드레이드스팟을 터치하여 캠페인정보가 도착함.
*/
void XSockGameSvr::RecvReqCampaignByGuildRaid( XPacket& p, const xCALLBACK& c )
{
	ID idSpot, idCamp;
	int bFound;
	XArchive arCamp;
	p >> idSpot >> idCamp;
	p >> bFound;
	if( bFound ) {
		p >> arCamp;
		auto pGuild = GAME->GetpGuild();
		if( XASSERT(pGuild) ) {
			// 길드에 캠페인데이타를 업데이트 한다.
			auto bOk = pGuild->DeSerializeUpdateGuildRaidCamp( arCamp, idCamp );
			auto spCampObj = pGuild->FindspRaidCampaign( idCamp );
			if( XASSERT(spCampObj) ) {
				auto pSpot = SafeCast2<XSpotCommon*>( sGetpWorld()->GetpSpot( idSpot ) );
				if( XASSERT( pSpot ) ) {
					pSpot->SetspCampObj( spCampObj );
				}
				// 길드레이드의 캠페인데이타는 스팟에 없고 길드정보에 있(어야 하지만 구조적 문제때문에 일단 스팟에 있기로함)다.
				if( SCENE_WORLD )
					SCENE_WORLD->RecvCampaignByGuildRaid( idSpot, spCampObj );
			}
		}
	}
}
/**
 @brief 캠페인 UI에서 스테이지를 선택했다.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqClickStageInCampaign( XWnd *pTimeoutCallback, 
												ID idSpot, 
												ID snSpot, 
												int idxStage, 
												int idxFloor,
												ID idPropStage,
												ID idWndPopup )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_CAMP_CLICK_STAGE );
	ar << idSpot;
	ar << snSpot;
	ar << (char)idxStage;
	ar << (char)idxFloor;
	ar << (short)0;
	ar << idPropStage;
	ar << idWndPopup;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvClickStageInCampaign, pTimeoutCallback );
// 		AddResponse( (ID)xCL2GS_SPOT_INFO,
// 					&XSockGameSvr::RecvSpotInfo, pTimeoutCallback );
// 		AddResponse( idKey, xCL2GS_RESULT_ENTER_GUILD_RAID, 
// 					&XSockGameSvr::RecvResultEnterGuildRaid, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

void XSockGameSvr::RecvClickStageInCampaign( XPacket& p, const xCALLBACK& c )
{
	ID idSpot, snSpot, idPropStage, idWnd;
	BYTE b0;
	char c0;
	//
	p >> idSpot;
	p >> snSpot;
//	p >> idxStage;
	p >> idPropStage;
	p >> idWnd;
	p >> b0;	auto errCode = (XGAME::xtError)b0;
	p >> b0;	bool bUpdate = xbyteToBool( b0 );
	p >> c0;	const int idxStage = c0;
	p >> c0;	const int idxFloor = c0;
	//
	XSpot *pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XBREAK(pBaseSpot == nullptr) )
		return;
	auto spCampObj = pBaseSpot->GetspCampObj();
	if( spCampObj ) {
		auto spStageObj = spCampObj->GetspStage( idxStage, idxFloor );
		if( spStageObj ) {
			if( bUpdate ) {
				// 스테이지객체 데이터의 업데이트
				XArchive arStage;
				p >> arStage;
				XStageObj::sDeSerialize( arStage, spCampObj, spStageObj );
				// 스테이지정보 받으면 드랍아이템목록을 스팟에 갱신시킨다.(영웅전장의 스테이지땜에 생김),
				pBaseSpot->SetDropItems( spStageObj->GetaryDrops() );
			}
		}
		if( errCode == xE_OK ) {
			xCampaign::sDoPopupSpotWithStageObj( pBaseSpot, spCampObj, spStageObj, idxFloor );
		}
	}
	// 에러 팝업
	if( errCode != xE_OK ) {
		xCampaign::sDoAlertCampaignByError( errCode );
	}
}

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @param bByRemainPoint 여분 특성포인트로 연구.
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqResearch( XWnd *pTimeoutCallback, ID snHero, XGAME::xtUnit unit, ID idNode, bool bByRemainPoint )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_TECH_RESEARCH );
	ar << snHero;
	ar << (BYTE)unit;
	ar << xboolToByte( bByRemainPoint );
	ar << (WORD)idNode;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvResearch, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqResearch()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqResearch()
*/
void XSockGameSvr::RecvResearch( XPacket& p, const xCALLBACK& c )
{
	char c0;
	BYTE b0;
	int point; //, secTotal;
	//
	p >> b0;	bool bByRemainPoint = xbyteToBool( b0 );	// 남는 특성포인트로 올린건지.
	p >> c0;	point = c0;
	if( bByRemainPoint ) {
		WORD w0;
		ID snHero;
		p >> w0;	ID idAbil = w0;
		p >> snHero;
		p >> b0;	int numRemainPoint = b0;
		p >> b0;	auto unit = (XGAME::xtUnit)b0;
		p >> w0;
		auto pHero = ACCOUNT->GetpHeroBySN( snHero );
		if( XASSERT(pHero) ) {
			auto pProp = XPropTech::sGet()->GetpNode( unit, idAbil );
			if( XASSERT(pProp) ) {
				pHero->SetAbilPoint( unit, idAbil, point );
				pHero->SetnumRemainAbilPoint( numRemainPoint );
			}
		}
	} else {
		DWORD gold;
		p >> c0 >> c0;
		ACCOUNT->GetResearching().DeSerialize( p, 0 );
		p >> gold;
		ACCOUNT->SetGold( gold );
		const auto& research = ACCOUNT->GetResearching();
		auto pHero = ACCOUNT->GetHero( research.GetsnHero() );
		if( pHero ) {
			auto pProp = XPropTech::sGet()->GetpNode( research.Getunit(), research.GetidAbil() );
			XBREAK( pProp == nullptr );
			pHero->SetAbilPoint( research.Getunit(), research.GetidAbil(), point );
		}
		ACCOUNT->DeSerializeResource( p );
		// 푸쉬?
		_tstring msg;
		msg = XTEXT( 90000 );
		msg += _T( "/" );
		msg += XTEXT( 90004 );
		ID idNode = research.GetidAbil();
		int secTotal = research.GetsecTotal();
		SendRegistPushMsg( NULL, ACCOUNT->GetidAccount(), XGAME::xTECH_TRAINING, idNode, msg, secTotal );
	}
	if( SCENE_TECH )
		SCENE_TECH->SetbUpdate( TRUE );

}

/**
 @brief 영웅 렙업 훈련 시작
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
// BOOL XSockGameSvr::SendReqTrainHero( XWnd *pTimeoutCallback, XHero *pHero, XGAME::xtTrain type )
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar( (ID) xCL2GS_TRAIN_HERO );
// 	ar << pHero->GetsnHero();
// 	ar << (BYTE)type;
// 	XBREAK(pHero->GetLevel() > 0xff);
// 	ar << (BYTE)pHero->GetLevel();	// 확인용
// 	ar << (BYTE)0;
// 	ar << (BYTE)0;
// 	ar << pHero->GetidProp();
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey = 
// 		AddResponse( ar.GetidPacket(), 
// 					&XSockGameSvr::RecvTrainHero, pTimeoutCallback );
// 	Send( ar );
// 	//
// 	return TRUE;
// }
// 
// /**
//  SendReqTrainHeroLevel()에 대한 응답함수
//  @param p 패킷이 들어있는 아카이브
//  @see SendReqTrainHeroLevel()
// */
// void XSockGameSvr::RecvTrainHero( XPacket& p, const xCALLBACK& c )
// {
// 	int ver;
// 	BYTE b0;
// 	p >> b0;	ver = b0;
// 	p >> b0;	
// 	p >> b0 >> b0;
// 	ACCOUNT->DeSerializeTrainSlot( p, ver );
// 	ACCOUNT->DeSerializeResource( p );
// 	// 훈련 창을 꺼준다.
// 	GAME->DestroyWndByIdentifier( "popup.train.hero.level" );
// 	GAME->SetbUpdate( true );
// 
// 	// 푸쉬 보내기
// 	int sn = 0;
// 	int time = 0;
// 	BYTE type = 0;
// 	p >> sn;
// 	p >> time;
// 	p >> type;
// 
// 	auto type2 = (XGAME::xtTrain)type;
// 	_tstring msg;
// 	int type3 = -1;
// 	msg = XTEXT(90000);
// 
// 	switch (type2)
// 	{
// 	case XGAME::xTR_LEVEL_UP:
// 		msg += _T("/");
// 		msg += XTEXT(90001);
// 		type3 = XGAME::xHERO_LEVELUP_TRAINING;
// 		break;
// 	case XGAME::xTR_SQUAD_UP:
// 		msg += _T("/");
// 		msg += XTEXT(90002);
// 		type3 = XGAME::xSQUAD_LEVELUP_TRAINING;
// 		break;
// 	case XGAME::xTR_SKILL_ACTIVE_UP:
// 		msg += _T("/");
// 		msg += XTEXT(90003);
// 		type3 = XGAME::xSKILL_TRAINING_ACTIVE;
// 		break;
// 	case XGAME::xTR_SKILL_PASSIVE_UP:
// 		msg += _T("/");
// 		msg += XTEXT(90003);
// 		type3 = XGAME::xSKILL_TRAINING_PASSIVE;
// 		break;
// 	}
// 
// 	SendRegistPushMsg(NULL, ACCOUNT->GetidAccount(), type3, sn, msg, time);
// }

/**
 @brief 클라가 봤을때 idxSlot의 훈련이 끝난것 같다 확인해달라고 서버에 보냄.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqCheckTrainComplete( XWnd *pTimeoutCallback, ID snSlot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CHECK_TRAIN_COMPLETE );
	ar << snSlot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvCheckTrainComplete, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqTrainComplete()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqTrainComplete()
*/
void XSockGameSvr::RecvCheckTrainComplete( XPacket& p, const xCALLBACK& c )
{
	int ver;
	DWORD numCash, numGold;
	ID snSlot, snHero;
	BYTE b0;
	WORD w0;
	XGAME::xtTrain type;
	xErrorCode result;
//	int quick;
	p >> b0;	ver = b0;
	p >> b0;	type = ( XGAME::xtTrain )b0;
	p >> w0;	result = (xErrorCode)w0;
	p >> snSlot;
	p >> snHero;
	p >> numCash;
	p >> numGold;
	p >> b0;	auto typeComplete = (XGAME::xtQuickComplete)b0;
	p >> b0;
	p >> b0;
	p >> b0;
	ACCOUNT->SetCashtem( numCash );
	ACCOUNT->SetGold( numGold );
	if( result == xEC_OK ) {
		auto pHero = ACCOUNT->GetHero( snHero );
		if( XBREAK( pHero == nullptr ) )
			return;
		ACCOUNT->DeSerializeTrainSlot( p, ver );
		pHero->DeSerializeUpgrade( p );
		// 콜백요청을 한 모든 UI에게 훈련완료 이벤트를 보낸다.
		XDelegator::sGet().DelegateEvent( "train.complete", type, pHero );
		//
		_tstring str;
		int type2 = -1;
		switch( type ) {
		case XGAME::xTR_LEVEL_UP:
			str = XE::Format( XTEXT( 2096 ), pHero->GetstrName().c_str() );	// 영웅 xxx의 훈련이 끝났습니다.
			type2 = XGAME::xHERO_LEVELUP_TRAINING;
			break;
		case XGAME::xTR_SQUAD_UP:
			str = XE::Format( XTEXT( 2096 ), pHero->GetstrName().c_str() );	// 영웅 xxx의 훈련이 끝났습니다.
			type2 = XGAME::xSQUAD_LEVELUP_TRAINING;
			break;
		case XGAME::xTR_SKILL_ACTIVE_UP: {
			str = XE::Format( XTEXT( 2096 ), pHero->GetstrName().c_str() );	// 영웅 xxx의 훈련이 끝났습니다.
			type2 = XGAME::xSKILL_TRAINING_ACTIVE;
		} break;
		case XGAME::xTR_SKILL_PASSIVE_UP: {
			str = XE::Format( XTEXT( 2096 ), pHero->GetstrName().c_str() );	// 영웅 xxx의 훈련이 끝났습니다.
			type2 = XGAME::xSKILL_TRAINING_PASSIVE;
		} break;
		default:
			break;
		}
		SOUNDMNG->OpenPlaySound( 27 );		// 훈련완료!
		if( pHero->GetbLevelUpAndClear( type ) ) {
			str += XFORMAT( "\n%s", XTEXT( 2234 ) );	// 레벨이 올랐습니다!
// 			if( !GAME->IsPlayingSeq() && !SCENE_BATTLE ) {	// 튜토중이거나 전투중에도 안뜸.
		}
		// 튜토중이거나 월드씬이 아닌상태에선 버퍼에 담는다.
		if( GAME->IsPlayingSeq() || !SCENE_WORLD ) {
			auto& listAlert = GAME->GetlistAlertWorld();
// 			if( !listAlert.FindpByID(pHero->GetsnHero() ) {
			xAlertWorld alert;
			alert.m_Type = xAW_TRAIN_COMPLETE;
			alert.m_snHero = pHero->GetsnHero();
			alert.m_strMsg = str;
			listAlert.Add( alert );
// 			}
		} else {
			auto pAlert = new XGameWndAlert( str.c_str(), nullptr, XWnd::xOK );
			if( pAlert ) {
				GAME->GetpScene()->Add( pAlert );
				pAlert->SetbModal( TRUE );
			}
		}
		if ( typeComplete ) {
			// 즉시완료시키면 렙업창 꺼줌.
			auto pWnd = GAME->Find( "popup.levelup.hero" );
			if( pWnd )
				pWnd->SetbDestroy( true );
			SendUnregistPushMsg(NULL, ACCOUNT->GetidAccount(), type2, snHero);
		}

	} else
	{
		switch( result )
		{
		case xEC_NOT_ENOUGH_CASH:
			XWND_ALERT("%s", _T("not enough cash."))
			break;
		case xEC_NOT_ENOUGH_GOLD:
			XWND_ALERT( "%s", _T( "not enough gold." ) )
			break;
		case xEC_NOT_YET_FINISH:
			// 아직 안끝남. 확인만 한거니까 메시지는 띄우지 않음.
			break;
		case xEC_IS_NOT_TRAINING:
			// 훈련중이 아님.
			break;
		default:
			
			break;
		}
		// 아직 안끝남.
	}
	GAME->SetbUpdate( true );
}

BOOL XSockGameSvr::SendReqRankingList(XWnd *pTimeoutCallback)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_RANKING_INFO);
	ID idKey = AddResponse(ar.GetidPacket(),
						&XSockGameSvr::RecvRankingList, pTimeoutCallback);
	Send(ar);
	return TRUE;
}

void XSockGameSvr::RecvRankingList(XPacket& p, const xCALLBACK& c)
{
	int size = 0, Rank = 0, LadderPoint = 0;
	ID idAccount = 0;
	TCHAR szName[256];
	p >> size;

//	XArrayLinear<XSceneWorld::xRankInfo*> aryRanking;
	XVector<XSceneWorld::xRankInfo*> aryRanking;
// 	if (size >= 0)
// 		aryRanking.Create(size);

	for (int i = 0; i < size; i++) {
		p >> Rank;
		p >> idAccount;
		p.ReadString(szName);
		p >> LadderPoint;
		auto xRank = new XSceneWorld::xRankInfo(Rank, idAccount, szName, LadderPoint);
		aryRanking.Add(xRank);
	}

	if (SCENE_WORLD)
		SCENE_WORLD->OnRecvRankList( aryRanking );

	XVECTOR_DESTROY( aryRanking );
}

/**
 @brief 무역상 귀환함.
*/
// void XSockGameSvr::RecvTimeTradeCall(XPacket& p, const xCALLBACK& c)
// {
// 	ACCOUNT->OffTimerByTrader();
// 	if( SCENE_TRADER )
// 		SCENE_TRADER->UpdateCharacter();
// 	XAccount::s_bTraderArrived = true;	// 다른씬에서 올수도 있으므로.
// 	if( SCENE_WORLD ) {
// 		SCENE_WORLD->OnTraderArrive();
// 	}
// }

/**
 @brief 상인을 호출한다.
*/
BOOL XSockGameSvr::SendCashTradeCall(XWnd *pTimeoutCallback, XGAME::xtSpentCall type)
{
	XPacket ar((ID)xCL2GS_LOBBY_CASH_TRADE_CALL);

	ar << (int)type;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvCashTradeCall, pTimeoutCallback);
	Send(ar);

	return 1;
}

/**
 @brief 무역상 소환요청에 대한 결과
*/
void XSockGameSvr::RecvCashTradeCall(XPacket& p, const xCALLBACK& c)
{
	if( XBREAK( ACCOUNT == nullptr ) )	// 이런경우가 있으면 갱신이 안될수 있으니 이렇게 하면 안됨.
		return;
	int i0;
	p >> i0;		auto type = (XGAME::xtSpentCall)i0;
	ACCOUNT->DeserializeTimerByTrader( p );
	//
	if (type == XGAME::xSC_SPENT_ITEM) {
		ID idRecallItem;
		int numRemainSpent;
		p >> idRecallItem;			// 호출템 아이디
		p >> numRemainSpent;		// 남은 호출템 개수.
		ACCOUNT->DestroyItem( idRecallItem, 1 );
		const int numRemain = ACCOUNT->GetNumItems( idRecallItem );
		XBREAK( numRemain != numRemainSpent );	// 검증
	} else
	//
	if( type == XGAME::xSC_SPENT_GEM ) {
		int numCash;
		p >> numCash;
		ACCOUNT->SetCashtem( numCash );
	}
	SendUnregistPushMsg( NULL, ACCOUNT->GetidAccount(), XGAME::xTRADER_RETURN, 0 );
	if( SCENE_TRADER )
		SCENE_TRADER->UpdateCharacter();
}

// void XSockGameSvr::RecvCashTradeCall(XPacket& p, const xCALLBACK& c)
// {
// 	if (!ACCOUNT)
// 		return;
// 
// 	int type = 0;
// 	int bSuccess = 0;
// 	int num = 0;
// 
// 	p >> type;
// 
// 	if (type == XGAME::xSC_SPENT_ITEM)
// 	{
// 		p >> bSuccess;
// 
// 		if (bSuccess)
// 		{
// 			p >> num;
// 			 
// 			ACCOUNT->GetTradeTimer()->Off();
// 			int nCount = ACCOUNT->GetNumItems(40001);
// 
// 			if (SCENE_TRADER)
// 				SCENE_TRADER->UpdateCharacter();
// 
// 			if (nCount - 1 == num)
// 				ACCOUNT->DestroyItem(40001, 1);
// 			else
// 				XBREAK(0);
// 
// 			SendUnregistPushMsg(NULL, ACCOUNT->GetidAccount(), XGAME::xTRADER_RETURN, 0);
// 		}
// 		else
// 			XBREAK(0);
// 	}
// 	else if (type == XGAME::xSC_SPENT_GEM)
// 	{
// 		p >> bSuccess;
// 
// 		if (bSuccess)
// 		{
// 			p >> num;
// 			ACCOUNT->SetCashtem(num);
// 			ACCOUNT->GetTradeTimer()->Off();
// 
// 			if (SCENE_TRADER)
// 			{
// 				SCENE_TRADER->UpdateCharacter();
// 			}
// 
// 			SendUnregistPushMsg(NULL, ACCOUNT->GetidAccount(), XGAME::xTRADER_RETURN, 0);
// 		}
// 	}
// }

void XSockGameSvr::RecvSystemGameNotify(XPacket& p, const xCALLBACK& c)
{
	_tstring str;
	p >> str;
	XGame::sPushMasterMessage(str);
}

BOOL XSockGameSvr::SendReqChangeSquad(XWnd *pTimeoutCallback, int idx, XLegion *pLegion)
{
	if(pLegion == nullptr)
		return false;
	XPacket ar((ID)xCL2GS_LOBBY_CHANGE_SQUAD);
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvChangeSquad, pTimeoutCallback);

//	auto& arySquad = pLegion->GetarySquadrons();
	ar << idx;
	const int size = pLegion->GetNumSquadrons();
	ar << size;
	if (pLegion->GetpLeader())
		ar << pLegion->GetpLeader()->GetsnHero();
	else
		ar << 0;
	for( auto pSq : pLegion->GetlistSquadrons() ) {
		ar << pSq->GetidxPos();	//슬롯
		//부대의 영웅이 계정에 없는 영웅일수는 없다
		XBREAK(ACCOUNT->GetHero(pSq->GetpHero()->GetsnHero()) == nullptr);
		ar << pSq->GetpHero()->GetsnHero();
	}
	//pLegion->Serialize(ar);
	Send(ar);
	return 1;
}

void XSockGameSvr::RecvChangeSquad(XPacket& p, const xCALLBACK& c)
{
	int idx, size, slot, success;
	ID snHero, snLeader;

	p >> success;
	if (success == 0)	{
		if (SCENE_LEGION)
			SCENE_LEGION->RecvChangeSquad(success);
		return;
	}

	p >> idx;
	p >> size;
	p >> snLeader;

	for (int i = 0; i < XGAME::MAX_SQUAD; i++)	{
		ACCOUNT->GetCurrLegion()->DestroySquadByIdxPos(i);
	}

	for (int i = 0; i < size; i++)	{
		p >> slot;
		p >> snHero;
		XBREAK(ACCOUNT->GetHero(snHero) == nullptr);
		auto pSq = new XSquadron(ACCOUNT->GetHero(snHero));
		ACCOUNT->GetCurrLegion()->AddSquadron(slot, pSq, false);

		if (pSq->GetpHero()->GetsnHero() == snLeader)
			ACCOUNT->GetCurrLegion()->SetpLeader(pSq->GetpHero());
	}

	if (SCENE_LEGION)
		SCENE_LEGION->RecvChangeSquad();
}

BOOL XSockGameSvr::SendReqRegistName(XWnd *pTimeoutCallback, _tstring strID, _tstring strPassword)
{
	XPacket ar((ID)xCL2GS_ACCOUNT_NAME_REGIST);
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvRegistID, pTimeoutCallback);

	ar.WriteString(strID.c_str());
	ar.WriteString(strPassword.c_str());

	Send(ar);

	return 1;
}
void XSockGameSvr::RecvRegistID(XPacket& p, const xCALLBACK& c)
{
	int result;
	TCHAR szName[MAX_PATH], szPassword[MAX_PATH];
	p >> result;
	p.ReadString(szName);
	p.ReadString(szPassword);
	ACCOUNT->SetstrID(szName);
	if (SCENE_WORLD)
		SCENE_WORLD->RecvRegistID((BOOL)result);
	//if (result == 1)
		//ACCOUNT->
}

void XSockGameSvr::RecvPostAdd(XPacket& p, const xCALLBACK& c)
{
	int bSuccess = 0;

	p >> bSuccess;

	XPostInfo* pInfo = new XPostInfo;
	pInfo->DeSerialize(p, VER_POST);
//	ACCOUNT->GetPostInfo()->Add(pInfo);
	ACCOUNT->AddPostInfo( pInfo );

	if (SCENE_WORLD)
		SCENE_WORLD->UpdateMailBoxAlert();
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
BOOL XSockGameSvr::SendReqAbilLockFree( XWnd *pTimeoutCallback
																			, ID snHero
																			, XGAME::xtUnit unit
																			, ID idAbil )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TECH_LOCK_FREE );
	ar << snHero;
	ar << (int)unit;
	ar << idAbil;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvAbilLockFree, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqAbilLockFree()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqAbilLockFree()
*/
void XSockGameSvr::RecvAbilLockFree( XPacket& p, const xCALLBACK& c )
{
	ID idAbil, snHero;
	DWORD dwGold;
	BYTE b0;
	p >> snHero;
	p >> b0;		auto unit = ( XGAME::xtUnit ) b0;
	p >> b0;		int numRemainUnlock = b0;
	p >> b0 >> b0;
	p >> idAbil;
	p >> dwGold;
	ACCOUNT->SetGold( dwGold );
	auto pHero = ACCOUNT->GetHero( snHero );
	if( XASSERT(pHero) ) {
		pHero->SetnumRemainAbilUnlock( numRemainUnlock );
		bool bOk = pHero->SetUnlockAbil( unit, idAbil );
		XBREAK( bOk == false );
	}
	GAME->GetpScene()->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqResearchComplete( XWnd *pTimeoutCallback
																						, ID snHero
																						, ID idResearchAbil )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_RESEARCH_COMPLETE );
	ar << snHero;
	ar << idResearchAbil;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvResearchComplete, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqResearchComplete()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqResearchComplete()
*/
void XSockGameSvr::RecvResearchComplete( XPacket& p, const xCALLBACK& c )
{
	ID idAbil, snHero;
	int point;
	DWORD cash, dw0;
	xErrorCode codeError;
	p >> dw0;	codeError = (xErrorCode)dw0;
	p >> snHero;
	p >> idAbil;
	p >> point;
	p >> cash;
	ACCOUNT->SetCashtem( cash );
	auto pProp = XPropTech::sGet()->GetpNode( idAbil );
	if( XBREAK(pProp == nullptr) )
		return;
	auto pHero = ACCOUNT->GetHero( snHero );
	if( XBREAK(pHero == nullptr) )
		return;
	const auto abil = pHero->GetAbilNode( pProp->unit, idAbil );
//	pAbil->point = point;
	pHero->SetAbilPoint( pProp->unit, idAbil, point );
	if( codeError == xEC_OK || codeError == xEC_QUICK_OK ) {
		// 완료됨.
		ACCOUNT->GetResearching().DoComplete();
		// 여기서 바로 띄우면 안되고 보관해놨다가 월드맵뜨면 띄워야 함.
		_tstring str = XE::Format( XTEXT( 2029 ), XTEXT( pProp->idName ) );	// "아무개"의 연구가 완료되었습니다.
		if( SCENE_WORLD && !GAME->IsPlayingSeq() ) {
			auto pAlert = XWND_ALERT( "%s", str.c_str() );
			if( pAlert ) {
				pAlert->SetbModal( TRUE );
			}
		} else {
			auto& listAlert = GAME->GetlistAlertWorld();
			xAlertWorld alert;
			alert.m_Type = xAW_RESEARCH_COMPLETE;
			alert.m_snHero = snHero;
			alert.m_strMsg = str;
			listAlert.Add( alert );
		}
		if (codeError == xEC_QUICK_OK)
			SendUnregistPushMsg(NULL, ACCOUNT->GetidAccount(), XGAME::xTECH_TRAINING, idAbil);
	} else {
		// 아직 완료안됨
		switch( codeError ) {
		case xEC_NOT_YET_FINISH:
			break;
		case xEC_IS_NOT_RESEARCHING: {
			ACCOUNT->SetCompleteResearch();
			XWND_ALERT("%s", XTEXT(2105));
		} break;
		case xEC_NOT_ENOUGH_CASH:
			XWND_ALERT("%s", XTEXT(80140));
			break;
		default:
			XWND_ALERT( "%s", XTEXT( 2106 ) );
			break;
		}
	}
	GAME->SetbUpdate( true );
	if( SCENE_TECH )
		SCENE_TECH->RecvResearchComplete( pProp, abil );
}

/**
 @brief 즉시 완료
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqResearchCompleteNow( XWnd *pTimeoutCallback
																							, XHero *pHero
																							, ID idAbil )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TECH_RESEARCH_COMPLETE_NOW );
	ar << pHero->GetsnHero();
	ar << idAbil;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( xCL2GS_RESEARCH_COMPLETE, 
					&XSockGameSvr::RecvResearchComplete, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
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
BOOL XSockGameSvr::SendReqRegenSpot( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_REGEN_SPOT );
	ar << idSpot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvRegenSpot, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqRegenSpot()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqRegenSpot()
*/
void XSockGameSvr::RecvRegenSpot( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	int numCash;
	p >> idSpot;
	p >> numCash;
	ACCOUNT->SetCashtem( numCash );
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
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
BOOL XSockGameSvr::SendReqReMatchSpot( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_REMATCH_SPOT );
	ar << idSpot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
	AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvReMatchSpot, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_JEWEL_MATCH_RESULT,
					&XSockGameSvr::RecvJewelMatchResult, pTimeoutCallback );
	AddResponse( idKey, xCL2GS_LOBBY_MANDRAKE_MATCH_RESULT,
					&XSockGameSvr::RecvMandrakeMatchResult, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqReMatchSpot()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqReMatchSpot()
*/
void XSockGameSvr::RecvReMatchSpot( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	int numCash;
	int numGold;
	XGAME::xtSpot type;
	BYTE b0;
	p >> idSpot;
	p >> numCash;
	p >> numGold;
	p >> b0;	type = (XGAME::xtSpot)b0;
	p >> b0;  bool bOk = (b0 != 0);
	p >> b0 >> b0;
	ACCOUNT->SetCashtem( numCash );
	ACCOUNT->SetGold( numGold );
	if( bOk ) {
		switch( type ) {
		case XGAME::xSPOT_SULFUR:
		case XGAME::xSPOT_NPC: {
			auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
			if( XASSERT(pBaseSpot) ) {
				XSpot::sDeSerialize( p, pBaseSpot );
			}
		}	break;
		default:
			break;
		}
	}
	if( !bOk ) {
		XWND_ALERT("%s", XTEXT(80140) );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
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
BOOL XSockGameSvr::SendReqSulfurRetreat( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_INGAME_SULFUR_RETREAT );
	ar << idSpot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSulfurRetreat, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSulfurRetreat()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSulfurRetreat()
*/
void XSockGameSvr::RecvSulfurRetreat( XPacket& p, const xCALLBACK& c )
{
}

BOOL XSockGameSvr::SendPostTest(XWnd *pTimeoutCallback)
{
	XPacket ar((ID)xCL2GS_LOBBY);
	Send(ar);
	return TRUE;
}

//BOOL XSockGameSvr::SendReqChangeNickName(XWnd *pTimeoutCallback, _tstring strName)
//{
//	_XCHECK_CONNECT(0);
//	//
//	XPacket ar((ID)xCL2GS_ACCOUNT_NICKNAME_CHANGE);
//
//	ar << strName;
//
//	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
//	ID idKey =
//		AddResponse(ar.GetidPacket(),
//		&XSockGameSvr::RecvChangeNickName, pTimeoutCallback);
//	Send(ar);
//	return TRUE;
//}
//void XSockGameSvr::RecvChangeNickName(XPacket& p, const xCALLBACK& c)
//{
//	int result;
//	_tstring strName;
//
//	p >> result;
//	p >> strName;
//
//	if (SCENE_TITLE)
//		SCENE_TITLE->RecvNickNameChage(strName, result);
//}
/**
 @brief 서버로부터 계정 버프가 발생함(아이템사용/운영자부여 등)
*/
void XSockGameSvr::RecvOccurBuff( XPacket& p, const xCALLBACK& c )
{
	XGAME::xBuff buff;
	buff.DeSerialize( p, 1 );
	ACCOUNT->AddBuff( buff );
	if( SCENE_STORAGE )
	{
		auto pWnd = SCENE_STORAGE->Find( "popup.storage.use");
		if( pWnd )
			pWnd->SetbDestroy( TRUE );
	}

}
void XSockGameSvr::RecvFinishBuff( XPacket& p, const xCALLBACK& c )
{
	ID idBuff;
	p >> idBuff;
	CONSOLE( "finish buff:%d", idBuff );
	ACCOUNT->DelBuff( idBuff );
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
BOOL XSockGameSvr::SendReqBattleStart( XWnd *pTimeoutCallback )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_INGAME_BATTLE_START );

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvBattleStart, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqBattleStart()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqBattleStart()
*/
void XSockGameSvr::RecvBattleStart( XPacket& p, const xCALLBACK& c )
{
	if( SCENE_READY )
		SCENE_READY->RecvBattleStart();
}

void XSockGameSvr::RecvAddBattleLog( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	XGAME::xBattleLog log;
	int verBattleLog;
	bool bAttack = false;
	p >> b0;	bAttack = (b0!=0);
	p >> b0;  verBattleLog = b0;
	p >> b0 >> b0;
	p.SetverArchiveInstant( verBattleLog );
	p >> log;
	ACCOUNT->AddBattleLog( bAttack, log );
}

/**
 @brief 만드레이크 스팟의 쌓인 보상을 받고 스팟을 초기화 시킨다.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqWithdrawMandrake( XWnd *pTimeoutCallback, ID idSpot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_WITHDRAW_MANDRAKE );
	ar << idSpot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvWithdrawMandrake, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqWithdrawMandrake()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqWithdrawMandrake()
*/
void XSockGameSvr::RecvWithdrawMandrake( XPacket& p, const xCALLBACK& c )
{
	int numMandrake;
	ID idSpot;
	p >> numMandrake;
	p >> idSpot;
	ACCOUNT->SetResource( XGAME::xRES_MANDRAKE, numMandrake );
	auto pSpot = sGetpWorld()->GetpSpotMandrake( idSpot );
	if( XASSERT(pSpot) )
		XSpot::sDeSerialize( p, pSpot );
	GAME->DestroyWndByIdentifier( "menu.circle" );
	GAME->GetpScene()->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqCheckAPTimeOver( XWnd *pTimeoutCallback )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_AP_CHECK_TIMEOVER );
	XBREAK( ACCOUNT->GetmaxAP() > 0xffff );
	ar << (WORD)ACCOUNT->GetAP();
	ar << (WORD)ACCOUNT->GetmaxAP();

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvCheckAPTimeOver, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqCheckAPTimeOver()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqCheckAPTimeOver()
*/
void XSockGameSvr::RecvCheckAPTimeOver( XPacket& p, const xCALLBACK& c )
{
	WORD w0;
	p >> w0;	ACCOUNT->SetmaxAP( w0 );
	p >> w0;	// currAp & bSync
	const bool bSyncOnly = (w0 & 0x8000)? true : false;
	const int apCurr = w0 & (~0x8000);
	p >> w0;	const int secOver = w0;
	if( apCurr > 0 ) {
		XBREAK( apCurr >= 10000 );		// 비정상적으로 큰 값.
		ACCOUNT->SetAP( apCurr );
		XBREAK( secOver >= 60 );		// 뭔가 문제가 있음. 이렇게까지 차이가 날수는 없음.
		ACCOUNT->ResetAPTimer( secOver % 60 );
		if( secOver > 0 ) {
			CONSOLE( "APTimerReset: secOver=%d", secOver );
		}
#ifdef _DEBUG
		CONSOLE("update: AP=%d, maxAP=%d", ACCOUNT->GetAP(), ACCOUNT->GetmaxAP() );
#endif // DEBUG
		GAME->SetbUpdate( true );
	} else {
		ACCOUNT->GettimerAP().RetryTimer( 2 );		// n초후에 다시 시도하도록 함.
	}
}


/*
SendReqCheckAPTimeOver()
{
	ar << (WORD)ACCOUNT->GetAP();
	ar << (WORD)ACCOUNT->GetmaxAP();
}

RecvReqCheckAPTimeOver()
{
	WORD w0;
	p >> w0;		// 0이오는것은 실패임.
	if( w0 > 0 )
	{
		ACCOUNT->SetAP( w0 );
		ACCOUNT->ResetAPTimer();
	}
	p >> w0;	ACCOUNT->SetmaxAP( w0 );

}

// 서버
RecvCheckAPTimeOver()
{
	XBREAK( m_pAccount->GetmaxAP() > 0xffff );
	if( m_pAccount->IsAPTimeOver() )
	{
		int ap = m_pAccount->AddAP( 1 );
		m_pAccount->ResetAPTimer();
		XBREAK( ap > 0xffff );
		XBREAK( ap == 0 );		// 적어도 1은 더했으므로 0이 나올수는 없다.
		ar << (WORD)ap;
		ar << (WORD)m_pAccount->GetmaxAP();
		m_pAccount->SetcntAPFail( 0 );		// 성공하면 계속 초기화
	} else
	{
		ar << (WORD)0;	// 0이면 타이머가 아직 안지났음.
		ar << (WORD)m_pAccount->GetmaxAP();
		int cntFail = m_pAccount->GetcntAPFail();
		XVERIFY_BREAK( ++cntFail >= 5 );	// 5초씩이나 차이가 난다는것은 클라랑 심각하게 비동기되고 있다는것이므로 재접하도록 함.
		m_pAccount->SetcntAPFail( cntFail );
	}

}
*/

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqTrainCompleteQuick( XWnd *pTimeoutCallback, ID snSlot, XGAME::xtTrain typeTrain, ID snHero, bool bCash )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TRAIN_COMPLETE_QUICK );
	ar << snSlot;
	ar << snHero;		// 확인용
	ar << (char)typeTrain;	// 확인용
	ar << xboolToByte(bCash);
	ar << (char)0;
	ar << (char)0;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( xCL2GS_CHECK_TRAIN_COMPLETE, 
					&XSockGameSvr::RecvCheckTrainComplete, pTimeoutCallback );	// 공통 응답함수를 씀.
	Send( ar );
	//
	return TRUE;
}

/**
 @brief 영웅에게 메달이나 보옥등의 전리품 제공하여 렙업
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
// BOOL XSockGameSvr::SendReqProvideBooty( XWnd *pTimeoutCallback, ID snHero, XGAME::xtTrain type, int num )
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar( (ID)xCL2GS_PROVIDE_BOODY );
// 	ar << snHero;
// 	ar << (BYTE)type;
// 	XBREAK( num > 0xff );
// //	ar << (BYTE)num;	// 이건 메달제공이기때문에 대량으로 한꺼번에 제공될 필요 없다. 해서도 안되고.
// 	ar << (BYTE)0;
// 	ar << (BYTE)0;
// 	ar << (BYTE)0;
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey = 
// 		AddResponse( ar.GetidPacket(), 
// 					&XSockGameSvr::RecvProvideBooty, pTimeoutCallback );
// 	Send( ar );
// 	//
// 	return TRUE;
// }
// 
// /**
//  SendReqProvideBooty()에 대한 응답함수
//  @param p 패킷이 들어있는 아카이브
//  @see SendReqProvideBooty()
// */
// void XSockGameSvr::RecvProvideBooty( XPacket& p, const xCALLBACK& c )
// {
// 	ID snHero;
// 	BYTE b0;
// 	WORD w0;
// 	p >> snHero;
// 	p >> b0;	auto type = (XGAME::xtTrain)b0;
// 	p >> b0;	int verEtc = b0;
// 	p >> b0 >> b0;
// 	ID idNeed;
// 	auto pHero = ACCOUNT->GetHero( snHero );
// 	if( XBREAK( pHero == nullptr ) )
// 		return;
// 	switch( type )
// 	{
// 	case XGAME::xTR_SQUAD_UP: {
// 		int numRefund1;
// 		int numRefund2;
// 		p >> numRefund1 >> numRefund2;
// 		if( numRefund1 )
// 			CONSOLE("유황환불:+%d", numRefund1);
// 		if( numRefund2 )
// 			CONSOLE( "만드레이크환불:+%d", numRefund2 );
// 	} break;
// 	case XGAME::xTR_SKILL_ACTIVE_UP:
// 	case XGAME::xTR_SKILL_PASSIVE_UP: {
// 		int numRefund;
// 		p >> numRefund;
// 		if( numRefund )
// 			CONSOLE( "보석환불:+%d", numRefund );
// 	} break;
// 	default:
// 		XBREAK(1);
// 		break;
// 	}
// 	ACCOUNT->DeSerializeTrainSlot( p, verEtc );
// 	pHero->DeSerializeLevelupReady( p );
// 	p >> idNeed;
// 	p >> b0;	bool bLevelup = xbyteToBool(b0);
// 	p >> b0;	pHero->SetNumProvide( type, b0 );
// 	p >> w0;	auto result = (xErrorCode)w0;
// 	if( result == xEC_NO_MORE_PROVIDE )
// 		XWND_ALERT("%s", _T("더이상 할 수 없습니다. 훈련이 끝나기를 기다려 주세요.") );
// 
// 	if( idNeed )
// 		// 무조건 한개씩만 제공하는걸로 가정
// 		XBREAK( 0 == ACCOUNT->DestroyItem( idNeed, 1 ) );
// 	///< 
// 	GAME->SetbUpdate( true );
// 
// 
// }

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqPromotionHero( XWnd *pTimeoutCallback, XHero *pHero )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_PROMOTION_HERO );
	ar << pHero->GetsnHero();
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvPromotionHero, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqPromotionHero()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqPromotionHero()
*/
void XSockGameSvr::RecvPromotionHero( XPacket& p, const xCALLBACK& c )
{
	int i0;
	ID snHero;
	p >> i0;	auto bOk = (XGAME::xtError)i0;
	p >> snHero;
	if( bOk == XGAME::xE_OK ) {
		auto pHero = ACCOUNT->GetHero( snHero );
		auto grade = pHero->GetGrade();
		ACCOUNT->DeserializeHeroUpdate( p );
		if( XASSERT(pHero) ) {
			auto pItem = ACCOUNT->GetSoulStoneByHero( pHero->GetstrIdentifer().c_str() );
			if( XASSERT(pItem) ) {
				// 승급에 필요한 개수만큼 삭제시킴,
				int numNeed = XGAME::GetNeedSoulPromotion( grade );
				auto result = ACCOUNT->DestroyItemBySN( pItem->GetsnItem(), numNeed );
				XBREAK( result == 0 );
			}
		}
		XWND_ALERT("%s", XTEXT(2294) );		// 승급완료
		GAME->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqSummonHeroByPiece( XWnd *pTimeoutCallback, XPropHero::xPROP* pPropHero )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_SUMMON_HERO_BY_PIECE );
	ar << pPropHero->idProp;
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSummonHeroByPiece, pTimeoutCallback );
	Send( ar );
	return TRUE;
}

/**
 SendReqSummonHeroByPiece()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSummonHeroByPiece()
*/
void XSockGameSvr::RecvSummonHeroByPiece( XPacket& p, const xCALLBACK& c )
{
	int i0;
	ID idHero;
	p >> i0;	auto bOk = ( XGAME::xtError )i0;
	p >> idHero;
	if( bOk == XGAME::xE_OK ) {
		// 새로 생성된 영웅을 동기화 시킨다.
		ACCOUNT->DeserializeHeroUpdate( p );
		// 사용된 영혼석을 삭제시킨다.
		auto pPropHero = PROP_HERO->GetpProp( idHero );
		if( XASSERT(pPropHero) ) {
			auto pItem = ACCOUNT->GetSoulStoneByHero( pPropHero->strIdentifier );
			if( XASSERT( pItem ) ) {
				// 소환에 필요한 개수만큼 삭제시킴,
// 				auto grade = pPropHero->GetGrade();
// 				int numNeed = XGAME::GetNeedSoulSummon( grade );
				int numNeed = XGAME::GetNeedSoulSummon();
				auto result = ACCOUNT->DestroyItemBySN( pItem->GetsnItem(), numNeed );
				XBREAK( result == 0 );
			}
		}
		GAME->GetpScene()->SetbUpdate( true );
		XWND_ALERT( "%s", XTEXT(2295) );		// 소환완료
	}
}

// xuzhu end
BOOL XSockGameSvr::SendReqGuildInfo(XWnd *pTimeoutCallback)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_INFO);
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvGuildInfo, pTimeoutCallback);
	Send(ar);
	return TRUE;
}

/**
 @brief 길드정보가 동기화 되어 온다.
 플레이어가 길드가 없으면 길드리스트가 도착하고
 플레이어가 길드가 있으면 내 길드 정보가 온다.
*/
void XSockGameSvr::RecvGuildInfo( XPacket& p, const xCALLBACK& c )
{
	int bList;
	p >> bList;		// 1:only 길드리스트 2:소속된 길드의 정보
	// 일단 플레이어 길드정보가 남아있다면 지움.
	GAME->DelGuild();
	if( bList ) {
		// 리스트를 보냈다는것은 플레이어가 길드가 없다는뜻이다. 근데 이런구조 안좋음.
//		ACCOUNT->SetGuildIndex( 0 );	
		ACCOUNT->ClearGuild();
		// 길드리스트는 길드씬에서만 사용하므로 다른 씬에서 도착했다면 그냥 무시함.
		if( SCENE_GUILD ) {
			int size;
			p >> size;
			XList4<XGuild*> listGuild;
			for( int i = 0; i < size; i++ ) {
				auto pGuild = XGuild::sCreateDeserializeInfo( p );
				// 멤버가 있는것만 리스트에 넣음.
				if( pGuild->GetMemberCount() > 0 ) {
					listGuild.push_back( pGuild );
				} else {
					SAFE_DELETE( pGuild );
				}
			} // for
			// 유저의 길드 기본정보도 동기화.
			XAccount::sDeSerializeGuildInfo( p, ACCOUNT );
			// 갱신
			SCENE_GUILD->RecvGuildList( listGuild );
		} // scene_guild
	} else {
		// 소속된 길드의 정보
		auto pGuild = XGuild::sCreateDeserialize( p );
		// 유저의 길드 기본정보도 동기화.
		XAccount::sDeSerializeGuildInfo( p, ACCOUNT );
		if( pGuild ) {
			GAME->SetpGuild( pGuild );
			if( SCENE_GUILD ) {
				SCENE_GUILD->RecvGuildInfo();
			}
		}
	}
}

// void XSockGameSvr::RecvGuildInfo( XPacket& p, const xCALLBACK& c )
// {
// 	if( ACCOUNT->GetGuildIndex() == 0 ) {		//길드 없음
// 		GAME->DelGuild();	//
// 		if( SCENE_GUILD ) {
// 			int size;
// 			p >> size;
// 			XList4<XGuild*> listGuild;
// 			for( int i = 0; i < size; i++ ) {
// 				XGuild *pGuild = new XGuild();
// 				pGuild->DeSerializeGuildInfo( p );
// 				if( pGuild->GetMemberCount() > 0 )
// 					listGuild.push_back( pGuild );
// 				else
// 					SAFE_DELETE( pGuild );
// 			}
// 			SCENE_GUILD->RecvGuildList( listGuild );
// 		}
// 	} else {
// 		XGuild* pGuild = nullptr;
// 		pGuild = new XGuild();
// 		pGuild->DeSerialize( p );
// 		GAME->SetpGuild( pGuild );
// 		//이미 길드가 있었는데 뭔가 데이터가 안맞아 다시보낸 경우 길드씬을 업데이트해줌
// 		if( SCENE_GUILD )
// 			SCENE_GUILD->RecvGuildInfo();
// 	}
// }

// void XSockGameSvr::RecvGuildInfo(XPacket& p, const xCALLBACK& c)
// {
// 	GAME->DelGuild();
// 	auto pGuild = XGuild::sCreateDeserialize( p );
// 	if( pGuild ) {
// 		GAME->SetpGuild( pGuild );
// 		if( SCENE_GUILD )
// 			SCENE_GUILD->RecvGuildInfo();
// 	}
// }

/**
 @brief 모든 길드리스트를 받는다 길드씬에서만 사용한다.
*/
// BOOL XSockGameSvr::SendReqAllGuildInfo( XWnd *pTimeoutCallback )
// {
// 	_XCHECK_CONNECT( 0 );
// 	//
// 	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_ALL_INFO );
// 
// 	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
// 	ID idKey =
// 		AddResponse( ar.GetidPacket(),
// 		&XSockGameSvr::RecvAllGuildInfo, pTimeoutCallback );
// 	Send( ar );
// 	return TRUE;
// }
// 
// /**
//  @brief 모든 길드의 리스트를 받는다 길드씬에서만 사용한다.
// */
// void XSockGameSvr::RecvAllGuildInfo( XPacket& p, const xCALLBACK& c )
// {
// 	// 현재 길드씬이 아니면 무시
// 	if( !SCENE_GUILD )
// 		return;
// 	int size;
// 	p >> size;
// 	XList4<XGuild*> listGuild;
// 	for( int i = 0; i < size; i++ ) {
// 		XGuild *pGuild = new XGuild();
// 		pGuild->DeSerializeGuildInfo( p );
// 		if( pGuild->GetMemberCount() > 0 )
// 			listGuild.push_back( pGuild );
// 		else
// 			SAFE_DELETE( pGuild );
// 	}
// 	SCENE_GUILD->RecvGuildList( listGuild );
// }

BOOL XSockGameSvr::SendReqCreateGuild(XWnd *pTimeoutCallback
																	, _tstring strName
																	, _tstring strDesc)
{
	_XCHECK_CONNECT(0);
	//
	if (ACCOUNT->GetGuildIndex() != 0)
		return FALSE;
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_CREATE);
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ar << strName;
	ar << strDesc;
	ar << 0;
	ar << 0;
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvCreateGuild, pTimeoutCallback);
	Send(ar);
	return TRUE;
}
void XSockGameSvr::RecvCreateGuild(XPacket& p, const xCALLBACK& c)
{
	DWORD dw0;
	ID guildIndex, guildGrade;
	_tstring strGuildName;
	_tstring strUserName;
	_tstring strGuildDesc;

	p >> dw0;		auto result = (xtGuildError)dw0;
	if( result == XGAME::xGE_SUCCESS ) {
		p >> guildIndex;
		p >> strGuildName;
		p >> strGuildDesc;
		p >> guildGrade;
		ACCOUNT->ClearGuild();
		ACCOUNT->SetGuildIndex( guildIndex );
		ACCOUNT->SetGuildgrade( ( XGAME::xtGuildGrade )guildGrade );
		ACCOUNT->ClearlistGuildJoinReq();
		if( SCENE_GUILD )
			SCENE_GUILD->RecvCreateGuild();
	} else if( result == XGAME::xGE_ERROR_NAME_DUPLICATE ) {
		if( SCENE_GUILD )
			SCENE_GUILD->RecvErrorPopup( ( XGAME::xtGuildError )result );
	}
	//else if (result == XGAME::xGE_ERROR_ALREADY_HAVE_GUILD)
	//	SCENE_GUILD->UpdateAll();
}
BOOL XSockGameSvr::SendReqJoinGuild(XWnd *pTimeoutCallback, ID idIndex)
{
	_XCHECK_CONNECT(0);
	//
	if( ACCOUNT->GetGuildIndex() != 0 )
		return FALSE;
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_JOIN_REQ );
	ar << idIndex;
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse( ar.GetidPacket(),
		&XSockGameSvr::RecvJoinGuild, pTimeoutCallback );
	Send( ar );
	return TRUE;
}
void XSockGameSvr::RecvJoinGuild(XPacket& p, const xCALLBACK& c)
{
	DWORD result, index;
	p >> result;
	p >> index;
	if( result == XGAME::xGE_ERROR_ALREADY_MEMBER ) {
		// 재 동기화 요구
		GAMESVR_SOCKET->SendReqGuildInfo( GAME );
	} else
	if( result == XGAME::xGE_SUCCESS ) {
		ACCOUNT->AddGuildJoinReq( index );
		if( SCENE_GUILD )
			SCENE_GUILD->RecvReqJoinGuild();
	} else if( SCENE_GUILD )
		SCENE_GUILD->RecvErrorPopup( ( XGAME::xtGuildError )result );
}

/**
 @brief 서버로부터 길드 업데이트 정보가 옴.
*/
// void XSockGameSvr::RecvUpdateGuild(XPacket& p, const xCALLBACK& c)
// {
// 	ID  idUpdate;
// 	int idxGuild;
// 	p >> idxGuild;
// 	p >> idUpdate;
// 
// 
// 	// (내)길드 업데이트 정보가 왔는데 클라에 길드정보가 없으면 다시 요청함.
// 	XGuild *pGuild = GAME->GetpGuild();
// 	if (pGuild == nullptr) {
// 		SendReqGuildInfo(GAME);
// 		return;
// 	}
// 	switch (idUpdate)
// 	{
// 	case XGAME::xGU_UPDATE_CREATE:
// 		break;
// 	case XGAME::xGU_UPDATE_MEMBER: {
// 		ID idActMember = 0;
// 		int gradeActMember = 0;
// 		ID idtgtMember = 0;
// 		int gradetgtMember = 0;
// 		ID MasterIdAcc = 0;
// 		_tstring strMemberName;
// 		p >> idActMember;
// 		p >> gradeActMember;
// 		p >> idtgtMember;
// 		p >> gradetgtMember;
// 		p >> MasterIdAcc;
// 		p >> strMemberName;
// 		if (idActMember == ACCOUNT->GetidAccount())
// 			ACCOUNT->SetGuildgrade((XGAME::xtGuildGrade)gradeActMember);
// 		else if (idtgtMember == ACCOUNT->GetidAccount())
// 			ACCOUNT->SetGuildgrade((XGAME::xtGuildGrade)gradetgtMember);
// 		if (pGuild) {
// 			if (MasterIdAcc > 0)
// 				pGuild->SetstrMasterName(strMemberName.c_str());
// 			if (SCENE_GUILD) {
// 				SCENE_GUILD->RecvMemberUpdate(p);
// 			} else {
// 				pGuild->DeSerializeGuildMemberlist(p);
// 			}
// 		} else
// 			SendReqGuildInfo(GAME);
// 		//ACCOUNT->AutoAdjustGuildGrade();
// 	}
// 		break;
// 	//case XGAME::xGU_UPDATE_MEMBER_MASTER:
// 	//{
// 	//	if (pGuild && SCENE_GUILD)
// 	//	{
// 	//		SCENE_GUILD->RecvMemberUpdate(p, true);
// 	//		//ACCOUNT->ClearJoinReqList();
// 	//	}
// 	//	else if (pGuild)
// 	//	{
// 	//		pGuild->DeSerializeGuildMemberlist(p);
// 
// 	//	}
// 	//	ACCOUNT->AutoAdjustGuildGrade();
// 	//}
// 	//	break;
// 	case XGAME::xGU_UPDATE_JOIN_ACCEPT: {
// 		XArchive arMember, arReqList;
// 		p >> arMember >> arReqList;
// 		pGuild->DeSerializeGuildMemberlist( arMember );
// 		pGuild->DeSerializeGuildReqMemberlist( arReqList );
// // 		pGuild->DeSerializeGuildMemberlist( p );
// // 		pGuild->DeSerializeGuildReqMemberlist( p );
// 		if( pGuild && SCENE_GUILD ) {
// 			SCENE_GUILD->RecvAcceptMember();
// 		}
// // 		if (pGuild && SCENE_GUILD)
// // 			SCENE_GUILD->RecvAcceptMember(p);
// // 		else 
// // 		if (pGuild) {
// // 			pGuild->DeSerializeGuildMemberlist(p);
// // 			pGuild->DeSerializeGuildReqMemberlist(p);
// // 		}
// 	} break;
// 	case XGAME::xGU_UPDATE_JOIN_REQ:
// 		if( pGuild )
// 			pGuild->DeSerializeGuildMemberlist( p );
// 		if( SCENE_GUILD )
// 			SCENE_GUILD->RecvListReqUpdate(/* p */);
// 		break;
// // 		if (pGuild && SCENE_GUILD)
// // 			SCENE_GUILD->RecvListReqUpdate(p);
// // 		else if (pGuild)
// // 			pGuild->DeSerializeGuildMemberlist(p);
// // 		break;
// 	case XGAME::xGU_UPDATE_CONTEXT: {
// 		if (pGuild) {
// 			_tstring strDesc;
// 			p >> strDesc;
// 			pGuild->SetstrGuildContext(strDesc.c_str());
// 			/*if (SCENE_GUILD)
// 			SCENE_GUILD->UpdateDesc(strDesc);*/
// 		}
// 	}
// 		break;
// 	case XGAME::xGU_UPDATE_OPTION: {
// 		if( pGuild ) {
// 			pGuild->DeSerializeGuildOption( p );
// 		}
// 		if( SCENE_GUILD )
// 			SCENE_GUILD->UpdateOption();
// // 		ID idAutoAccept, idBlockJoin;
// // 		p >> idAutoAccept;
// // 		p >> idBlockJoin;
// // 		if (SCENE_GUILD)
// // 			SCENE_GUILD->UpdateOption(idAutoAccept, idBlockJoin);
// 	} break;
// 		//SCENE_GUILD->SetbUpdate(true);
// 	case XGAME::xGU_UPDATE_DELETE: {
// 	} break;
// 	}
// 	if (SCENE_GUILD)
// 		SCENE_GUILD->UpdateUI();
// }

/**
 @brief 길드이벤트를 받는다.
*/
void XSockGameSvr::RecvGuildEvent( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	WORD w0;
	p >> b0;	auto event = (xtGuildEvent)b0;
	p >> b0;
	p >> w0;
	XArchive arParam;
	p >> arParam;
	//
	auto pGuild = GAME->GetpGuild();
	//
	switch( event ) {
	case XGAME::xGEV_CREATE: {
		pGuild = new XGuild();
		pGuild->DeSerialize( arParam );
		GAME->SetpGuild( pGuild );
	} break;
	case XGAME::xGEV_DESTROY:
		GAME->DelGuild();
		ACCOUNT->ClearlistGuildJoinReq();
		break;
	case XGAME::xGEV_JOIN_ACCEPT:
	case XGAME::xGEV_JOIN_REJECT:
	case XGAME::xGEV_CHANGE_MEMBER_INFO:
	case XGAME::xGEV_KICK:
	case XGAME::xGEV_OUT:
	case XGAME::xGEV_UPDATE:
		XGuild::sUpdateByEvent( pGuild, event, arParam );
		break;
	default:
		break;
	}
	if( SCENE_GUILD )
		SCENE_GUILD->RecvUpdateGuildUser( event, arParam );
// 	switch( event ) {
// // 	case xGEV_GUILD_INFO: {
// // 		XAccount::sDeSerializeGuildInfo( arParam, ACCOUNT );
// // 	} break;
// 	case xGEV_JOIN_ACCEPT: {
// 		if( pGuild ) {
// 			xnGuild::xMember memTarget;
// 			arParam >> memTarget;
// 			// 해당멤버의 데이터를 동기화 한다.
// 			pGuild->UpdateWithxMember( memTarget );
// 			pGuild->RemoveJoinReqer( memTarget.m_idAcc );
// 		}
// 		if( memTarget.m_idAcc == ACCOUNT->GetidAccount() ) {
// 			// 본인이 가입됨
// 			CONSOLE("%s%s", strTarget.c_str(), _T("님이 동맹에 가입되었습니다."));
// 		} else {
// 			CONSOLE("%s%s", strTarget.c_str(), _T("님이 동맹에 가입되었습니다."));
// 		}
// 	} break;
// 	case xGEV_JOIN_REJECT: {
// 		ID idAccTarget, idAccConfirm, idGuild;
// 		_tstring strGuildName;
// 		arParam >> idGuild;
// 		arParam >> idAccTarget;
// 		arParam >> strGuildName;
// 		arParam >> idAccConfirm;
// 		ACCOUNT->DelGuildJoinReq( idGuild );
// 		if( idAccTarget == ACCOUNT->GetidAccount() ) {
// 			// 가입이 거절됨
// 			CONSOLE( "[%s]%s", strGuildName.c_str(), _T( " 동맹의 가입이 거절 되었습니다." ) );
// 		}
// 	} break;
// 	case xGEV_CHANGE_MEMBER_INFO: {
// 		xnGuild::xMember memberConfirm;
// 		xnGuild::xMember memberTarget;
// 		arParam >> memberConfirm >> memberTarget;
// 		if( pGuild ) {
// 			pGuild->UpdateWithxMember( memberConfirm );
// 			pGuild->UpdateWithxMember( memberTarget );
// 			// 길마가 바뀜
// 			if( memberTarget.m_Grade == xGGL_LEVEL5 ) {
// 				pGuild->SetidAccMaster( memberTarget.m_idAcc );
// 				pGuild->SetstrMasterName( memberTarget.m_strName );
// 			}
// 		}
// 	} break;
// 	case xGEV_KICK: {
// 		ID idAccKicker, idAccTarget;
// 		_tstring strTarget;
// 		arParam >> idAccKicker >> idAccTarget;
// 		arParam >> strTarget;
// 		// 추방당한자가 this플레이어인가
// 		if( idAccTarget == ACCOUNT->GetidAccount() ) {
// 			GAME->DelGuild();
// 			pGuild = nullptr;
// 			ACCOUNT->ClearlistGuildJoinReq();
// 			XWND_ALERT( "%s", _T( "동맹으로부터 추방되었습니다." ) );
// 		} else {
// 			if( pGuild ) {
// 				pGuild->RemoveGuildMember( idAccTarget );
// 			}
// 			CONSOLE("%s", _T("님이 동맹으로부터 추방되었습니다."), strTarget.c_str() );
// 		}
// 	} break;
// 	case xGEV_OUT: {
// 		ID idAcc;
// 		_tstring strName;
// 		arParam >> idAcc;			// 탈퇴자 아이디
// 		arParam >> strName;		// 탈퇴자 이름
// 		if( idAcc == ACCOUNT->GetidAccount() ) {
// 			GAME->DelGuild();
// 			pGuild = nullptr;
// 			ACCOUNT->ClearlistGuildJoinReq();
// 		} else {
// 			CONSOLE( "%s%s", strName.c_str(), _T( "님이 동맹을 탈퇴했습니다." ) );
// 		}
// 	} break;
// 	default:
// 		XBREAK(1);
// 		break;
// 	}
// 	if( SCENE_GUILD )
// 		SCENE_GUILD->RecvUpdateGuildUser( event, arParam );
}

bool XSockGameSvr::SendReqJoinGuildAccept(XWnd *pTimeoutCallback, ID idAcc, ID idFlag)
{
	_XCHECK_CONNECT(0);
	//

	if (ACCOUNT->GetGuildIndex() == 0)
		return FALSE;

	XPacket ar((ID)xCL2GS_LOBBY_GUILD_JOIN_ACCEPT);

	ar << idAcc;
	ar << idFlag;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvReqJoinGuildAccept, pTimeoutCallback);
	Send(ar);
	return TRUE;
}

void XSockGameSvr::RecvReqJoinGuildAccept(XPacket& p, const xCALLBACK& c)
{
	ID  idResult, idIndex;
	p >> idResult;
	p >> idIndex;

	if (SCENE_GUILD)
	{
		SCENE_GUILD->RecvErrorPopup((XGAME::xtGuildError)idResult);
	}
	/*switch (xFlag)
	{
	case XGAME::xGE_SUCCESS:
	if (SCENE_GUILD)
	break;
	case XGAME::XGE_ERROR_GUILD_NOT_FOUND:
	break;
	case XGAME::XGE_ERROR_MEMBER_NOT_FOUND:
	break;
	case XGAME::XGE_ERROR_GUILD_INFO: break;
	case XGAME::XGE_ERROR_GUILD_CREATE: break;
	case XGAME::xGE_ERROR_ACCEPT_MEMBER: break;
	case XGAME::xGE_ERROR_JOIN_MEMBER: break;
	case XGAME::xGE_ERROR_NAME_DUPLICATE: break;
	case XGAME::xGE_ERROR_ALREADY_HAVE_GUILD: break;
	case XGAME::xGE_ERROR_COST_NOT_ENOUGH: break;
	case XGAME::xGE_ERROR_MAX_MEMBER_OVER: break;
	case XGAME::xGE_ERROR_NO_AUTHORITY: break;
	}*/
}

void XSockGameSvr::RecvJoinResult(XPacket& p, const xCALLBACK& c)
{
	DWORD dw0;
	ID idGuild, idAccConfirm, idAccReqer;
	p >> dw0;		auto flagAccept = (xtGuildAcceptFlag)dw0;
	p >> dw0;		auto errCode = (xtGuildError)dw0;
	p >> idAccConfirm;
	p >> idAccReqer;
	p >> idGuild;
	if( idAccReqer == ACCOUNT->GetidAccount() ) {
		if( flagAccept == xGAF_ACCEPT ) {
			// this플레이어가 가입신청자이고 허락이 떨어지면 서버에 가입된 길드정보를 요청.
			ACCOUNT->SetGuildgrade( XGAME::xGGL_LEVEL1 );
			ACCOUNT->SetGuildIndex( idGuild );
			SendReqGuildInfo( GAME );
		} else
		if( flagAccept == xGAF_REFUSE ) {
			// 거절이 떨어지면 내가 길드신청한 목록을 갱신함.
			ACCOUNT->DeSerializeJoinReqGuild( p );
		}
		if( SCENE_GUILD ) {
// 			XWND_ALERT("%s", "길드가입이 거절됨")
			// 정보를 재요청 해서 길드씬 갱신하게 함.
			SendReqGuildInfo( GAME );
		}
	}
}

bool XSockGameSvr::SendReqOutGuild(XWnd *pTimeoutCallback)
{
	_XCHECK_CONNECT(0);
	//
	if (ACCOUNT->GetGuildIndex() == 0)
		return false;
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_OUT);
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvReqOutGuild, pTimeoutCallback);
	Send(ar);
	return true;
}

/**
 @brief 탈퇴
*/
void XSockGameSvr::RecvReqOutGuild(XPacket& p, const xCALLBACK& c)
{
	DWORD dw0;
	p >> dw0;		auto idResult = (xtGuildError)dw0;
	switch( idResult ) {
	case xGE_SUCCESS:
		GAME->DelGuild();
		ACCOUNT->ClearlistGuildJoinReq();
		break;
	default:
		if( SCENE_GUILD )
			SCENE_GUILD->RecvErrorPopup( ( XGAME::xtGuildError )idResult );
	}
	if( SCENE_GUILD )
		SCENE_GUILD->RecvReqOutGuild( idResult );
}

bool XSockGameSvr::SendReqKickGuild(XWnd *pTimeoutCallback, ID idAcc)
{
	_XCHECK_CONNECT(0);
	//
	if (ACCOUNT->GetGuildIndex() == 0)
		return false;
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_KICK);
	ar << idAcc;
	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvReqKickGuild, pTimeoutCallback);
	Send(ar);
	return true;
}
void XSockGameSvr::RecvReqKickGuild(XPacket& p, const xCALLBACK& c)
{
	DWORD dw0;
	ID idAcc;
	p >> dw0;		auto result = (xtGuildError)dw0;
	p >> idAcc;
	if( result != xGE_SUCCESS ) {
		if( SCENE_GUILD )
			SCENE_GUILD->RecvErrorPopup( result );
	}
	if( SCENE_GUILD )
		SCENE_GUILD->RecvReqKickGuild();
}

// RecvGuildEvent에 통합됨
// void XSockGameSvr::RecvKickGuild(XPacket& p, const xCALLBACK& c)
// {
// 	ID idResult;
// 	p >> idResult;
// 	GAME->DelGuild();
// 	if (SCENE_GUILD)
// 		SCENE_GUILD->UpdateChange();
// }

void XSockGameSvr::RecvCreateHero( XPacket& p, const xCALLBACK& c )
{
	auto pHero = XHero::sCreateDeSerialize( p, ACCOUNT );
	GAME->SetbUpdate( true );
// 	if (SCENE_UNITORG)
// 		SCENE_UNITORG->SetbUpdate(true);
}

/**
 @brief 계정레벨업을 확인했음.
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendAccepLevelUp()
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_ACCEPT_LEVELUP );
	Send( ar );
	//
	return TRUE;
}

void XSockGameSvr::RecvSyncTrainSlot( XPacket& p, const xCALLBACK& c )
{
	ACCOUNT->DeSerializeTrainSlot( p, VER_ETC_SERIALIZE );
	GAME->SetbUpdate( true );
}
void XSockGameSvr::RecvSyncResource( XPacket& p, const xCALLBACK& c )
{
	ACCOUNT->DeSerializeResource( p );
	GAME->SetbUpdate( true );
}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendFlagTutorial()
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_BIT_FLAG_TUTORIAL );
	DWORD dw0 = ACCOUNT->GetdwFlagTutorial();
	ar << dw0;

	Send( ar );
	//
	return TRUE;
}

// BOOL XSockGameSvr::SendReqLoginIDPW(XWnd *pTimeoutCallback, const _tstring& strID, const _tstring& strPW)
// {
// 	_XCHECK_CONNECT(0);
// 	//
// 	XPacket ar((ID)xCL2GS_ACCOUNT_LOGIN_FROM_ID);
// 	ar << strID;
// 	ar << strPW;
// 
// 	ID idKey =
// 		AddResponse(ar.GetidPacket(),
// 		&XSockGameSvr::RecvLoginIDPW, pTimeoutCallback);
// 	Send(ar);
// 	//
// 	return TRUE;
// }
// 
// void XSockGameSvr::RecvLoginIDPW(XPacket& p, const xCALLBACK& c)
// {
// 	int result;
// 	_tstring strID, strPW;
// 
// 	p >> result;
// 	p >> strID;
// 	p >> strPW;
// 
// 	if (SCENE_WORLD)
// 	{
// 		if (result == 1)
// 			LOGIN_INFO.SetIDPwLogin(strID.c_str(), strPW.c_str());
// 		SCENE_WORLD->RecvAccLogin(result);
// 	}
// }

/**
 @brief 
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqUnlockUnit( XWnd *pTimeoutCallback, XGAME::xtUnit unit )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TECH_UNLOCK_UNIT );
	ar << (DWORD)unit;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvUnlockUnit, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqUnlockUnit()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqUnlockUnit()
*/
void XSockGameSvr::RecvUnlockUnit( XPacket& p, const xCALLBACK& c )
{
	DWORD dw0;
	XGAME::xtUnit unit;
	p >> dw0;	unit = ( XGAME::xtUnit )dw0;
	ACCOUNT->SetUnlockUnit( unit );
// 	ACCOUNT->AddTechPoint( unit, 1 );
	GAME->DispatchEvent( xAE_UNLOCK_UNIT, (DWORD)unit );
	GAME->DestroyWndByIdentifier("popup.unit.unlock");
	GAME->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqFillAP( XWnd *pTimeoutCallback )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_FILL_AP );
	ar << 0;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvFillAP, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSendReqFillAP()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSendReqFillAP()
*/
void XSockGameSvr::RecvFillAP( XPacket& p, const xCALLBACK& c )
{
	int ap;
	DWORD numCash;
	p >> ap;
	p >> numCash;
	ACCOUNT->SetCashtem( numCash );
	if( ap == 0 )	{
		XWND_ALERT("%s", XTEXT(80140) );		// 캐쉬 부족.
	} else	{
		ACCOUNT->SetmaxAP( ap );
		ACCOUNT->SetAP( ap );
	}
	GAME->SetbUpdate( true );
}

/**
 @brief 각종 asset(자원,금화,시간,AP,횟수등)을 젬이나 특정아이템으로 지불하고 구매한다.
 @param bByItem 젬대신 typeAsset에 맞는 아이템이 있다면 그것을 소모하고 구매(교환)
*/
BOOL XSockGameSvr::SendReqPaymentAssetByGem( XWnd *pTimeoutCallback, xtPaymentRes typeAsset, bool bByItem )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_PAYMENT_ASSET );
	ar << (char)typeAsset;
	ar << xboolToByte( bByItem );
	ar << (char)0;
	ar << (char)0;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvPaymentAssetByGem, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqPaymentAssetByGem()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqPaymentAssetByGem()
*/
void XSockGameSvr::RecvPaymentAssetByGem( XPacket& p, const xCALLBACK& c )
{
	DWORD numCash;		// 현재 젬개수 업데이트
	char c0;
	p >> c0;		const auto errCode = (XGAME::xtError)c0;
	p >> c0;		const auto typeAsset = (xtPaymentRes)c0;
	p >> c0;		const bool bByItem = (c0 != 0);
	p >> c0;
	p >> numCash;
	ACCOUNT->SetCashtem( numCash );
	switch( typeAsset ) {
	case xPR_TRY_DAILY: {
	} break;
	default:
		break;
	}
	GAME->GetpScene()->SetbUpdate( true );

}




BOOL XSockGameSvr::SendRegistPushMsg(XWnd *pTimeoutCallback, int idacc, int type1, int type2, const _tstring& msg, int time)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_REGIST_PUSHMSG);
	
	ar << idacc;
	ar << type1;
	ar << type2;
	ar << time;
	ar << msg;

	Send(ar);
	//
	return TRUE;
}

BOOL XSockGameSvr::SendUnregistPushMsg(XWnd *pTimeoutCallback, int idacc, int type1, int type2)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_UNREGIST_PUSHMSG);
	
	ar << idacc;
	ar << type1;
	ar << type2;

	Send(ar);
	//
	return TRUE;
}

bool XSockGameSvr::SendReqChangeDesc(XWnd *pTimeoutCallback, _tstring strDesc)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT);

	ar << strDesc;

	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvReqChangeDesc, pTimeoutCallback);
	Send(ar);
	return true;
}

void XSockGameSvr::RecvReqChangeDesc(XPacket& p, const xCALLBACK& c)
{
	BYTE b0;
	_tstring strContext;
	p >> b0;		auto result = (xtGuildError)b0;
	p >> b0 >> b0 >> b0;
	p >> strContext;
	auto pGuild = GAME->GetpGuild();
	if( pGuild ) {
		pGuild->SetstrGuildContext( strContext );
	}
	if (SCENE_GUILD) {
		SCENE_GUILD->RecvAdjustDesc( result );
	}
}

bool XSockGameSvr::SendReqChangeOption(XWnd *pTimeoutCallback, BOOL bAutoAccept, BOOL bBlockJoin)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_CHANEGE_OPTION);
	ar << (ID)bAutoAccept;
	ar << (ID)bBlockJoin;
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvReqChangeOption, pTimeoutCallback);
	Send(ar);
	return true;
}
void XSockGameSvr::RecvReqChangeOption(XPacket& p, const xCALLBACK& c)
{
	ID idResult;
	p >> idResult;
	if( SCENE_GUILD ) {
		switch( ( XGAME::xtGuildError )idResult ) {
		case XGAME::xGE_SUCCESS:
			SCENE_GUILD->UpdateChange();
			break;
		default:
			SCENE_GUILD->RecvErrorPopup( ( XGAME::xtGuildError )idResult );
		}
	}
}

bool XSockGameSvr::SendReqChangeMemberGrade(XWnd* pTimeoutCallback, ID idAccount, ID idAccLevel, _tstring strName, XGAME::xtGuildGrade eGrade)
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar((ID)xCL2GS_LOBBY_GUILD_CHANGE_MEMBER);

	ar << idAccount;
	ar << (BYTE)eGrade;
	ar << (BYTE)0;
	ar << (WORD)0;
	ID idKey =
		AddResponse(ar.GetidPacket(),
		&XSockGameSvr::RecvChangeMemberGrade, pTimeoutCallback);
	Send(ar);
	return true;
}
void XSockGameSvr::RecvChangeMemberGrade(XPacket& p, const xCALLBACK& c)
{
	BYTE b0;
	WORD w0;
	ID idTarget;
	p >> idTarget;
	p >> b0;		auto result = (xtGuildError)b0;
	p >> b0;
	p >> w0;		

	if (SCENE_GUILD && result != XGAME::xGE_SUCCESS)
		SCENE_GUILD->RecvErrorPopup( result );
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
BOOL XSockGameSvr::SendReqEndSeq( XWnd *pTimeoutCallback, const std::string& idsSeq )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_END_SEQ );
	ar << idsSeq;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvEndSeq, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqEndSeq()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqEndSeq()
*/
void XSockGameSvr::RecvEndSeq( XPacket& p, const xCALLBACK& c )
{
	_tstring str;
	int num;
// 	ID idHero;
	p >> str;
	p >> num;
// 	p >> idHero;
	std::string idsSeq = SZ2C(str);
	ACCOUNT->AddCompleteSeq( idsSeq );
	XBREAK( num != ACCOUNT->GetNumCompletedSeq() );
// 	if( idHero ) {
// 		ACCOUNT->CreateAddHero( idHero );
// 		GAME->SetbUpdate( true );
// 	}
	GAME->OnRecvEndSeq( idsSeq );

}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendUIAction( XGAME::xtUIAction uiAction, DWORD dwParam2 )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_UI_ACTION );
	ar << (DWORD)uiAction;
	ar << dwParam2;
	Send( ar );
	//
	return TRUE;
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
BOOL XSockGameSvr::SendReqBuyGuildItem( XWnd *pTimeoutCallback, int idxSlot )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_LOBBY_GUILD_BUY_ITEM );
	ar << idxSlot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvBuyGuildItem, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSendReqBuyGuildItem()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSendReqBuyGuildItem()
*/
void XSockGameSvr::RecvBuyGuildItem( XPacket& p, const xCALLBACK& c )
{
	int ptGuild;
	_tstring idsItem;
	p >> ptGuild;
	p >> idsItem;
	ACCOUNT->SetptGuild( ptGuild );
	ACCOUNT->CreateItemToInven( idsItem, 1 );
	XWND_ALERT("%s", XTEXT(2211) );	// 구입성공
	GAME->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqCampaignReward( XWnd *pTimeoutCallback, ID idSpot, ID idCamp, int idxStage )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CAMP_REWARD );
	ar << idSpot;
	ar << idCamp;
	ar << idxStage;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvCampaignReward, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqCampaignReward()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqCampaignReward()
*/
void XSockGameSvr::RecvCampaignReward( XPacket& p, const xCALLBACK& c )
{
	int bRecv;
	ID idSpot, idCamp;
	int idxStage;
	DWORD dw0;
	p >> dw0;	auto errCode = (XGAME::xtError)dw0;
	p >> idSpot >> idCamp >> idxStage;
	p >> bRecv;
	if( bRecv )
		ACCOUNT->SetGold( bRecv );
	p >> bRecv;
	if( bRecv )
		ACCOUNT->SetCashtem( bRecv );
	ACCOUNT->DeSerializeResource2( p );
	auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		if( XASSERT( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CAMPAIGN) ) {
			auto pSpot = SafeCast<XSpotCampaign*>( pBaseSpot );
			if( XASSERT(pSpot) ) {
				auto spCamp = pSpot->GetspCampObj();
				if( XASSERT(spCamp) ) {
					xCampaign::XStageObj::sDeSerialize( p, spCamp, spCamp->GetspStage( idxStage ) );
				}
			}
		}
	}
	if( errCode != XGAME::xE_OK ) {
		if( errCode == XGAME::xE_ALREADY_RECV_REWARD ) {
			XWND_ALERT( "%s", XTEXT(2225) );	// 이미 보상받음.
		} else {
			XWND_ALERT( "%s", XTEXT( 2226 ) );	// 알수없는 에러
		}
	}
	GAME->SetbUpdate( true );

}

/**
 @brief 훈련에 쓸 금화양을 보내서 훈련을 시작한다.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqTrainHeroByGold( XWnd *pTimeoutCallback, 
											XHero *pHero, 
											int gold, 
											XGAME::xtTrain typeTrain )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TRAIN_HERO_BY_GOLD );
	ar << pHero->GetsnHero();
	ar << gold;
	ar << (BYTE)typeTrain;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvTrainHeroByGold, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqTrainHeroByGold()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqTrainHeroByGold()
*/
void XSockGameSvr::RecvTrainHeroByGold( XPacket& p, const xCALLBACK& c )
{
	ID snHero, snSlot;
	int gold, expTrain;
	BYTE b0;
	p >> snHero;
	p >> b0;	int verEtc = b0;
	p >> b0;	auto typeTrain = ( XGAME::xtTrain )b0;
	p >> b0 >> b0;
	p >> expTrain;
	p >> gold;
	p >> snSlot;
	ACCOUNT->DeSerializeTrainSlot( p, verEtc );
	ACCOUNT->SetGold( (DWORD)gold );
	auto pPopup = SafeCast<XWndLevelupHero*>( GAME->Find("popup.levelup.hero") );
	if( pPopup ) {
		pPopup->UpdateTrain( snHero, typeTrain, snSlot );
	}
	{
// 		// 푸쉬 보내기
// 		int sn = 0;
// 		int time = 0;
// 		BYTE type = 0;
// 		p >> sn;
// 		p >> time;
// 		p >> type;
// 
// 		_tstring msg;
// 		int type3 = -1;
// 		msg = XTEXT( 90000 );	// 게임 제목
// 		switch( typeTrain ) {
// 		case XGAME::xTR_LEVEL_UP:
// 			msg += _T( "/" );
// 			msg += XTEXT( 90001 );
// 			type3 = XGAME::xHERO_LEVELUP_TRAINING;
// 			break;
// 		case XGAME::xTR_SQUAD_UP:
// 			msg += _T( "/" );
// 			msg += XTEXT( 90002 );
// 			type3 = XGAME::xSQUAD_LEVELUP_TRAINING;
// 			break;
// 		case XGAME::xTR_SKILL_ACTIVE_UP:
// 			msg += _T( "/" );
// 			msg += XTEXT( 90003 );
// 			type3 = XGAME::xSKILL_TRAINING_ACTIVE;
// 			break;
// 		case XGAME::xTR_SKILL_PASSIVE_UP:
// 			msg += _T( "/" );
// 			msg += XTEXT( 90003 );
// 			type3 = XGAME::xSKILL_TRAINING_PASSIVE;
// 			break;
// 		}
// 		SendRegistPushMsg( NULL, ACCOUNT->GetidAccount(), type3, sn, msg, time );
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
BOOL XSockGameSvr::SendReqClickFogSquad( XWnd *pTimeoutCallback, ID idSpot, ID snHero, int idxStage )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CLICK_FOG_SQUAD );
	ar << idSpot;
	ar << snHero;
	ar << idxStage;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvSendReqClickFogSquad, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqSendReqClickFogSquad()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqSendReqClickFogSquad()
*/
void XSockGameSvr::RecvSendReqClickFogSquad( XPacket& p, const xCALLBACK& c )
{
	ID idSpot, snHero;
	DWORD gold;
	int idxStage;
	p >> idSpot >> snHero;
	p >> idxStage;
	p >> gold;	ACCOUNT->SetGold( gold );
	XSpot* pBaseSpot = sGetpWorld()->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		auto spLegion = pBaseSpot->GetspLegion();
		if( XASSERT(spLegion) ) {
			auto pSquad = spLegion->GetSquadronByHeroSN( snHero );
			if( XASSERT(pSquad) ) {
				auto pHero = pSquad->GetpHero();
				if( XASSERT(pHero) ) {
					bool bFog = spLegion->IsFog( snHero );
					XBREAK( bFog == false );
					spLegion->DelFogSquad( snHero );
				}
			}
		}
	}
	GAME->GetpScene()->SetbUpdate( true );
}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendCheckUnlockUnit( XGAME::xtUnit unit )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CHECK_UNLOCK_UNIT );
	ar << (int)unit;
	Send( ar );
	//
	return TRUE;
}

/**
 @brief 전투준비씬에서 아군부대를 선택하는 행동을 함.
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendTouchSquadInReadyScene( int idxSquad )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_TOUCH_SQUAD_IN_READY );
	ar << idxSquad;
	Send( ar );
	//
	return TRUE;
}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendControlSquadInBattle( const XHero *pHero )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CONTROL_SQUAD_IN_BATTLE );
	ar << pHero->GetsnHero();
	Send( ar );
	//
	return TRUE;
}

//#ifdef _DEV
void XSockGameSvr::RecvAttackedSimulByHome( XPacket& p, const xCALLBACK& c )
{
	BYTE numAttacked;
	BYTE b0;
	std::vector<int> aryResAcc;
	p >> numAttacked;
	p >> b0 >> b0 >> b0;
	ACCOUNT->SetnumAttaccked( numAttacked );
	ACCOUNT->DeSerializeResource( p );
	XGAME::xBattleLog log;
	p >> log;
	auto& rListLog = ACCOUNT->GetlistBattleLogDefense();
	rListLog.Add( log );
	if( log.bWin ) {
		// 방어성공
	} else {
		// 방어실패
		// 모든 스팟에 손실률 적용
		const auto rateLoss = XGC->m_rateLootResourceSquad;
		sGetpWorld()->LossLocalStorageAll( rateLoss, nullptr );
	}
	if( SCENE_WORLD )
		SCENE_WORLD->OnAttackedLog();
}
//#endif // _DEV

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendCheckEncounterLog( ID idSpot )
{
	_XCHECK_CONNECT(0);
	XPacket ar( (ID)xCL2GS_CHECK_ENCOUNTER_LOG );
	ar << idSpot;
	Send( ar );
	return TRUE;
}

/**
 @brief 
 전송하고 응답을 기다릴 필요없는 패킷의 경우에 사용한다.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
*/
BOOL XSockGameSvr::SendGotoHome( int secTimeout )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_GOTO_HOME );
	ar << secTimeout;
	Send( ar );
	//
	return TRUE;
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
BOOL XSockGameSvr::SendReqRegisterFacebook( XWnd *pTimeoutCallback, const std::string& strcFbUserId, const std::string& strFbUsername )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID) xCL2GS_ACCOUNT_REGISTER_FACEBOOK );
	ar << strcFbUserId;
	ar << strFbUsername;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvRegisterFacebook, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqRegisterFacebook()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqRegisterFacebook()
*/
void XSockGameSvr::RecvRegisterFacebook( XPacket& p, const xCALLBACK& c )
{
	ID idAcc;
	_tstring strFbUserId, strfBUsername;
	p >> idAcc;
	p >> strFbUserId;
	p >> strfBUsername;
	if( idAcc ) {
		// 성공
		ACCOUNT->SetstrFbUserId( strFbUserId );
		if( SCENE_WORLD ) {
//			SCENE_WORLD->UpdateOptionPopup();
			SCENE_WORLD->SetbUpdate( true );
		}
	}
}

void XSockGameSvr::RecvSyncHello( XPacket& p, const xCALLBACK& c )
{
	ID idSpot;
	_tstring strHello;
	p >> idSpot >> strHello;
	if( sGetpWorld() == nullptr )
		return;
	auto pBaseSpot = sGetpWorld()->GetpSpot( idSpot );
	if( XASSERT(pBaseSpot) ) {
		pBaseSpot->SetstrHello( strHello );
	}
	if( GAME->GetpScene() )
		GAME->GetpScene()->SetbUpdate( true );

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
BOOL XSockGameSvr::SendReqChangeHelloMsg( XWnd *pTimeoutCallback, const _tstring& strHello )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_CHANGE_HELLO_MSG );
	ar << strHello;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvChangeHelloMsg, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqChangeHelloMsg()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqChangeHelloMsg()
*/
void XSockGameSvr::RecvChangeHelloMsg( XPacket& p, const xCALLBACK& c )
{
	// 서버 저장완료.
	GAME->GetpScene()->SetbUpdate( true );
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
BOOL XSockGameSvr::SendReqInitAbil( XWnd *pTimeoutCallback, ID snHero )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_INIT_ABIL  );
	ar << snHero;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvInitAbil, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqInitAbil()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqInitAbil()
*/
void XSockGameSvr::RecvInitAbil( XPacket& p, const xCALLBACK& c )
{
	BYTE b0;
	ID snHero;
	DWORD gold;
	p >> snHero;
	p >> b0;	int numRemainPoint = b0;
	p >> b0;	int numRemainUnlock = b0;
	p >> b0;	int cntInitAbil = b0;
	p >> b0;
	p >> gold;

	auto pHero = ACCOUNT->GetpHeroBySN( snHero );
	if( XASSERT(pHero) ) {
		const auto bOk = pHero->DoInitAbil();
		XBREAK( bOk == false );
		ACCOUNT->SetGold( gold );
		// 검증만 함.
		XBREAK( numRemainPoint != pHero->GetnumRemainAbilPoint() );
		XBREAK( numRemainUnlock != pHero->GetnumRemainAbilUnlock() );
		XBREAK( cntInitAbil != pHero->GetnumInitAbil() );
	}
	GAME->GetpScene()->SetbUpdate( true );
	
	
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
BOOL XSockGameSvr::SendReqUnlockTrainingSlot( XWnd *pTimeoutCallback )
{
	_XCHECK_CONNECT(0);
	//
	const int numFreeSlot = ACCOUNT->GetnumFreeSlot();
	XPacket ar( (ID)xCL2GS_UNLOCK_TRAINING_SLOT );
	ar << numFreeSlot;

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvUnlockTrainingSlot, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqUnlockTrainingSlot()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqUnlockTrainingSlot()
*/
void XSockGameSvr::RecvUnlockTrainingSlot( XPacket& p, const xCALLBACK& c )
{
	int numFreeSlot;
	DWORD cash;
	p >> numFreeSlot;
	p >> cash;
	ACCOUNT->SetCashtem( cash );
	ACCOUNT->SetnumFreeSlot( numFreeSlot );
	XDelegator::sGet().DelegateEvent( "unlock.train.slot" );
	
	GAME->SetbUpdate( true );
	
}


/**
 @brief 디바이스에서 생성한 GCM regid를 서버로 보내 저장하도록 한다.
 전송하고 응답을 기다려야 하는 류의 구현에 사용.
 _XCHECK_CONNECT의 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @param pTimeoutCallback 서버로부터 응답이 없을때 호출될 콜백객체
 @param param 사용자가 정의해서 쓰시오
 @return 전송에 성공하면 TRUE를 리턴한다. 만약 연결이 끊겨있거나 하면 _XCHECK_CONNECT()에 의해 FALSE가 리턴된다.
 @see AddResponse()
*/
BOOL XSockGameSvr::SendReqGCMResistId( XWnd *pTimeoutCallback, const std::string& strRegid )
{
	XTRACE("XSockGameSvr::SendReqGCMResistId: gcm regid=%s", strRegid.c_str() );
	if( XBREAK( strRegid.size() > 255 ) )		// 쓰레기값이 들어온듯.
		return TRUE;
	_XCHECK_CONNECT(0);
	//
	XPacket ar( (ID)xCL2GS_PUSH_GCM_REGIST_ID  );
	ar << strRegid;
#if defined(_VER_ANDROID)
	ar << _T("google");
#elif defined(_VER_IOS)
	ar << _T("apple");
#elif defined(WIN32)
	ar << _T("win32");
#else
#error("unknown platform")
#endif

	//응답을 받을 콜백함수를 지정한다. 첫번째 파라메터는 응답을 받을때 사용되는 패킷아이디이다.
	ID idKey = 
		AddResponse( ar.GetidPacket(), 
					&XSockGameSvr::RecvGCMResistId, pTimeoutCallback );
	Send( ar );
	//
	return TRUE;
}

/**
 SendReqGCMResistId()에 대한 응답함수
 @param p 패킷이 들어있는 아카이브
 @see SendReqGCMResistId()
*/
void XSockGameSvr::RecvGCMResistId( XPacket& p, const xCALLBACK& c )
{
	DWORD dw0;
	p >> dw0;		auto codeErr = (XGAME::xtError)dw0;
	// 에러처리
	switch( codeErr ) {
	case XGAME::xE_OK:
		CONSOLE( "%s:registed gcm regid:", __TFUNC__ );
		break;
	case XGAME::xE_INVALID_GCM_REGIST_ID:
		CONSOLE( "%s:invalid gcm regid", __TFUNC__ );
		break;
	default:
		CONSOLE( "%s:unknown error", __TFUNC__ );
		XBREAK(1);
		break;
	}
}

/**
 @brief 퍼블릭키를 받은 후 IAP시스템을 초기화 한다.
*/
void XSockGameSvr::RecvPublicKey( XPacket& p, const xCALLBACK& c )
{
#ifndef _SOFTNYX
	// 소프트닉스 빌드로 할것.
	const std::string strcPublicKey = m_CryptoObj.DeSerializeDecryptString( p );
	XBREAK( strcPublicKey.empty() );
		// 인앱시스템 초기화
	if( !strcPublicKey.empty() )
		XInApp::sGet()->InitGoogleIAPAsync( strcPublicKey );
#endif // not _SOFTNYX

}

void XSockGameSvr::DelegateGuildAccept( XGuild* pGuild, const xnGuild::xMember& memberJoin )
{
	if( memberJoin.m_idAcc == ACCOUNT->GetidAccount() ) {
		// 본인이 가입됨
		CONSOLE("%s%s", memberJoin.m_strName.c_str(), _T("님이 동맹에 가입되었습니다."));
	} else {
		CONSOLE("%s%s", memberJoin.m_strName.c_str(), _T("님이 동맹에 가입되었습니다."));
	}
}
void XSockGameSvr::DelegateGuildReject( XGuild* pGuild, const xnGuild::xMember& memberReject )
{
	ACCOUNT->DelGuildJoinReq( pGuild->GetidGuild() );
	if( memberReject.m_idAcc == ACCOUNT->GetidAccount() ) {
		// 가입이 거절됨
		CONSOLE( "[%s]%s", pGuild->Get_strName().c_str(), _T( " 동맹의 가입이 거절 되었습니다." ) );
	}
}
void XSockGameSvr::DelegateGuildChangeMemberInfo( XGuild* pGuild, const xnGuild::xMember& memberTarget, const xnGuild::xMember& memberConfirm )
{

}
void XSockGameSvr::DelegateGuildKick( XGuild* pGuild, const xnGuild::xMember& memberKick )
{
	// 추방당한자가 this플레이어인가
	if( memberKick.m_idAcc == ACCOUNT->GetidAccount() ) {
		GAME->DelGuild();
		pGuild = nullptr;
		ACCOUNT->ClearlistGuildJoinReq();
		XWND_ALERT( "%s", _T( "동맹으로부터 추방되었습니다." ) );
	} else {
		CONSOLE("%s", _T("님이 동맹으로부터 추방되었습니다."), memberKick.m_strName.c_str() );
	}
}
void XSockGameSvr::DelegateGuildOut( XGuild* pGuild, const xnGuild::xMember& memberOut )
{
	if( memberOut.m_idAcc == ACCOUNT->GetidAccount() ) {
		GAME->DelGuild();
		pGuild = nullptr;
		ACCOUNT->ClearlistGuildJoinReq();
	} else {
		CONSOLE( "%s%s", memberOut.m_strName.c_str(), _T( "님이 동맹을 탈퇴했습니다." ) );
	}
}
void XSockGameSvr::DelegateGuildUpdate( XGuild* pGuild, const xnGuild::xMember& member )
{

}

