#include "stdafx.h"
#include "XClientConnection.h"
#include "XPacketCG.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketSvrForClient.h"
#include "server/XSAccount.h"
#include "XLoginConnection.h"
#include "XGlobalConst.h"
// #ifdef _XPROP_SERIALIZE
// #include "XPropUnit.h"
// #include "XPropHero.h"
// #include "XPropItem.h"
// #include "XPropLegion.h"
// #endif // _XPROP_SERIALIZE

//#include "server/XSAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XClientConnection>* XMemPool<XClientConnection>::s_pPool = NULL;

#define CONSOLE_ACC( TAG, F, ... ) \
	__xLogfTag( TAG, XLOGTYPE_LOG, XTSTR("%s:", F), __TFUNC__, ##__VA_ARGS__); \

#define XVERIFY_BREAK( EXP )	{	\
	if( XBREAK( EXP ) )	{	\
	OnInvalidVerify(); \
	return;	\
	}	\
}

#define XVERIFY_BREAKF( EXP, F, ... )	{	\
	if( XBREAKF( EXP, F, ##__VA_ARGS__ ) )	{	\
	OnInvalidVerify(); \
	return;	\
	}	\
}

// 불량데이타 보내는 유저에 대한 처리
void XClientConnection::OnInvalidVerify( void )
{
	CONSOLE_ACC( "connect", "-----------------invalid client: %s", GetszIP() );
	// 클라에 에러메시지창 보내고
//	SendMsgBox( _T("invalid client") );
	SendInvalidVerify();
	// 즉각 소켓 닫아버림.
#pragma message("check this")
//	DoDisconnect();		보내고 바로 닫아버리니까 클라에서 못받는다. 다른 방법 찾아야 함.
}

#define xQUEUE_SIZE		0x10000

XClientConnection::XClientConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( _T("connClientBygsvr"), MAIN->GetpSocketForClient(), socket, szIP, xQUEUE_SIZE )
{
	Init();
	m_CryptObj.MakeRandomKeyTable();
	XTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
}

// 클라이언트로부터 패킷을 받음.
BOOL XClientConnection::ProcessPacket(DWORD idPacket, ID idKey, XPacket& p)
{
	TRACE( "recv packet: %d - \n", idPacket );

	int retv = 1;
	if ( idPacket == xCL2GS_ACCOUNT_REQ_LOGIN )
		RecvLogin(p);
	else {
		auto pUser = SafeCast<XGameUser*>(GetspUser().get());
		if (XBREAK(pUser == nullptr)) {
			CONSOLE("idPacket=%d, idConnect=0x%08x", idPacket, GetidConnect());
			OnInvalidVerify();
// 			DoDisconnect();
			return FALSE;
		}
		switch (idPacket)
		{
//		case xCL2GS_ACCOUNT_REQ_LOGIN:			RecvLogin(p);	break;
			//case xCL2GS_ACCOUNT_NICKNAME_DUPLICATE:	retv = pUser->RecvAccountNickNameDuplicate(p);	break;
			//case xCL2GS_ACCOUNT_NAME_DUPLICATE:		retv = pUser->RecvAccountNameDuplicate(p);	break;
		case xCL2GS_ACCOUNT_NAME_REGIST:		retv = pUser->RecvAccountNameRegist(p);	break;
// 		case xCL2GS_ACCOUNT_LOGIN_FROM_ID:		retv = pUser->RecvAccountNameLogin(p);	break;
		case xCL2GS_ACCOUNT_NICKNAME_CHANGE:	retv = pUser->RecvAccountNickNameChange(p);	break;
		case xCL2GS_LOBBY_SPOT_RECON:			retv = pUser->RecvSpotRecon(p);	break;
		case xCL2GS_LOBBY_SPOT_ATTACK:			retv = pUser->RecvSpotAttack(p);	break;
		case xCL2GS_INGAME_REQ_FINISH_BATTLE:	retv = pUser->RecvReqFinishBattle2(p); break;
		case xCL2GS_LOBBY_SPOT_COLLECT:			retv = pUser->RecvSpotCollect(p);	break;
		case xCL2GS_LOBBY_OPEN_CLOUD:			retv = pUser->RecvOpenCloud(p); break;
//		case xCL2GS_LOBBY_ADD_HERO_EXP:			retv = pUser->RecvAddHeroExp(p); break;			//영웅 정보가 변경 되었습니다.
		case xCL2GS_LOBBY_CHANGE_HERO_LEGION:	retv = pUser->RecvChageHeroLegion(p); break;
		case xCL2GS_LOBBY_CHANGE_HERO_EQUIPMENT:retv = pUser->RecvChageHeroEquip(p); break;
		case xCL2GS_LOBBY_SUMMON_HERO:			retv = pUser->RecvSummonHero(p);	break;
		case xCL2GS_LOBBY_NEW_SQUAD:			retv = pUser->RecvNewSquad(p);	break;
		case xCL2GS_LOBBY_MOVE_SQUAD:			retv = pUser->RecvMoveSquad(p);	break;
		case xCL2GS_LOBBY_CHANGE_SQUAD:			retv = pUser->RecvChangeSquad(p);	break;
		case xCL2GS_LOBBY_ITEM_SHOP_LIST_CASH:	retv = pUser->RecvItemSellList(p); break;
//		case xCL2GS_LOBBY_ITEM_SHOP_LIST_TIME:	retv = pUser->RecvItemSellList(p); break; 			///< 시간 만료 아이템 상점 판매 목록
		case xCL2GS_LOBBY_ITEM_BUY:				retv = pUser->RecvBuyItem(p); break; 				///< 아이템 구매
		case xCL2GS_LOBBY_ITEM_SELL:			retv = pUser->RecvSellItem(p); break; 				///< 아이템 상점에 판매
		case xCL2GS_LOBBY_ITEM_SPENT:			retv = pUser->RecvItemSpent(p); break;				///< 아이템 사용
		case xCL2GS_LOBBY_INVENTORY_EXPAND:		retv = pUser->RecvInventoryExpand(p); break;		///< 인벤토리 확장
		case xCL2GS_CAMP_REWARD:				retv = pUser->RecvCampaignReward( p );	break;
		case xCL2GS_TRAIN_HERO_BY_GOLD:			retv = pUser->RecvTrainHeroByGold( p );	break;
		case xCL2GS_INIT_ABIL: retv = pUser->RecvInitAbil( p );	break;
// #ifdef _INAPP_GOOGLE
		case xCL2GS_LOBBY_CASH_ITEM_BUY_PAYLOAD:retv = pUser->RecvCashItemBuyPayload(p); break; 	///< Inapp 구글플레이_payload
		case xCL2GS_LOBBY_CASH_ITEM_BUY_IAP:	retv = pUser->RecvBuyCashtemIAP(p); break; 		///< Inapp 구글플레이
//#endif
// #ifdef _INAPP_APPLE
// 		case xCL2GS_LOBBY_CASH_ITEM_BUY_IAP_IOS:retv = pUser->RecvCashItemBuyApple(p); break; 		///< Inapp 구매_애플
// #endif

		case xCL2GS_CHEAT:						retv = pUser->RecvCheat(p); break;
		case xCL2GS_CHEAT_CREATE_ITEM:			retv = pUser->RecvCheatCreateItem(p);	break;

		case xCL2GS_LOBBY_POST_INFO:			pUser->RecvPostInfoRequest(p);		break;			//클라이언트 우편함 정보 요청
			//case xCL2GS_LOBBY_POST_READ:			pUser->RecvPostReadRequest(p);		break;			//클라이언트 우편함 읽음.(상대편에게 알려 줄까 말까?)
		case xCL2GS_LOBBY_POST_GETITEM_ALL:		pUser->RecvPostItemGetRequestAll(p);	break;			//클라이언트 우편물 수령요청
			//case xCL2GS_LOBBY_POST_DELETE:			pUser->RecvPostDeleteRequest(p);		break;			//클라이언트 우편물 삭제 요청
		case xCL2GS_LOBBY_TRADE:				retv = pUser->RecvTrade(p);	break;
		case xCL2GS_LOBBY_CHANGE_SCALP_TO_BOOK: retv = pUser->RecvChangeScalpToBook(p);	break;
// 		case xCL2GS_LOOBY_UPGRADE_SQUAD:		retv = pUser->RecvUpgradeSquad(p);	break;
		case xCL2GS_LOBBY_RELEASE_HERO:			retv = pUser->RecvReleaseHero(p);	break;
// 		case xCL2GS_LOBBY_LEVELUP_SKILL:		retv = pUser->RecvLevelUpSkill(p);	break;
		case xCL2GS_LOBBY_RANKING_INFO:			retv = pUser->RecvRankingInfo(p);		break; // pUser->SendSubscribePackage();  break;//pUser->Cheat_AddPostInfo(); break;
		case xCL2GS_LOBBY_CASH_TRADE_CALL:		retv = pUser->RecvTradeCashCall(p);		break; // 캐쉬로 무역상 호출.
//		case xCL2GS_LOBBY_TIME_TRADE_CALL:		retv = pUser->RecvTradeTimeCall(p);		break; // 시간으로 무역상 호출.
		case xCL2GS_PROMOTION_HERO:			retv = pUser->RecvPromotionHero( p );	break;
		case xCL2GS_SUMMON_HERO_BY_PIECE:	retv = pUser->RecvSummonHeroByPiece( p );	break;
		case xCL2GS_UI_ACTION:			retv = pUser->RecvUIAction( p );	break;
		case xCL2GS_LOBBY_GUILD_INFO:			retv = pUser->RecvGuildInfo(p);			break;
//		case xCL2GS_LOBBY_GUILD_ALL_INFO:			retv = pUser->RecvAllGuildInfo(p);			break;
		case xCL2GS_LOBBY_GUILD_CREATE:			retv = pUser->RecvGuildCreate(p);		break;
		case xCL2GS_LOBBY_GUILD_CHANEGE_OPTION:	retv = pUser->RecvGuildChangeOption(p);	break;
		case xCL2GS_LOBBY_GUILD_CHANEGE_CONTEXT:retv = pUser->RecvGuildChangeContext(p);break;
		case xCL2GS_LOBBY_GUILD_CHANGE_MEMBER:	retv = pUser->RecvGuildChangeMemberInfo(p); break;
		case xCL2GS_LOBBY_GUILD_JOIN_REQ:		retv = pUser->RecvGuildJoinReq(p);		break;
		case xCL2GS_LOBBY_GUILD_JOIN_ACCEPT:	retv = pUser->RecvGuildJoinAccept(p);	break;
		case xCL2GS_LOBBY_GUILD_OUT:			retv = pUser->RecvGuildOut(p);			break;
		case xCL2GS_LOBBY_GUILD_KICK:			retv = pUser->RecvGuildKick(p);			break;
		case xCL2GS_LOBBY_GUILD_BUY_ITEM:		retv = pUser->RecvBuyGuildItem( p );	break;

		case xCL2GS_QUEST_REQUEST_REWARD:		retv = pUser->RecvReqQuestReward(p);	break;
		case xCL2GS_LOBBY_SPOT_TOUCH:			retv = pUser->RecvSpotTouch(p);	break;
		case xCL2GS_CAMP_CLICK_STAGE:			retv = pUser->RecvClickStageInCampaign(p);	break;
		case xCL2GS_TECH_RESEARCH:				retv = pUser->RecvResearch(p);	break;
		case xCL2GS_TECH_LOCK_FREE:				retv = pUser->RecvAbilLockFree(p);	break;
		case xCL2GS_TECH_RESEARCH_COMPLETE_NOW:	retv = pUser->RecvResearchCompleteNow(p);	break;
		case xCL2GS_RESEARCH_COMPLETE:			retv = pUser->RecvCheckResearchComplete(p);	break;
		case xCL2GS_LOBBY_REGEN_SPOT:			retv = pUser->RecvRegenSpot(p);	break;
		case xCL2GS_LOBBY_REMATCH_SPOT:			retv = pUser->RecvReMatchSpot(p);	break;
		case xCL2GS_INGAME_SULFUR_RETREAT:		retv = pUser->RecvSulfurRetreat(p);	break;
		case xCL2GS_INGAME_BATTLE_START:		retv = pUser->RecvBattleStart(p);	break;
		case xCL2GS_LOBBY_WITHDRAW_MANDRAKE:	retv = pUser->RecvWithdrawMandrake(p);	break;
//		case xCL2GS_TRAIN_HERO:			retv = pUser->RecvTrainHero(p);	break;
		case xCL2GS_CHECK_TRAIN_COMPLETE:				retv = pUser->RecvCheckTrainComplete(p);	break;
		case xCL2GS_CHANGE_HELLO_MSG: retv = pUser->RecvChangeHelloMsg( p );	break;
		case xCL2GS_UNLOCK_TRAINING_SLOT: retv = pUser->RecvUnlockTrainingSlot( p );	break;
//		case xCL2GS_TRAIN_COMPLETE_TOUCH:		retv = pUser->RecvLevelupConfirm(p);	break;
		case xCL2GS_AP_CHECK_TIMEOVER:			retv = pUser->RecvCheckAPTimeOver( p );	break;
		case xCL2GS_ACCEPT_LEVELUP:			retv = pUser->RecvAccepLevelUp( p );	break;
		case xCL2GS_BIT_FLAG_TUTORIAL:			retv = pUser->RecvFlagTutorial( p );	break;
		case xCL2GS_TECH_UNLOCK_UNIT:			retv = pUser->RecvUnlockUnit( p );	break;
		case xCL2GS_FILL_AP:					retv = pUser->RecvSendReqFillAP( p );	break;
		case xCL2GS_TRAIN_COMPLETE_QUICK:		retv = pUser->RecvTrainCompleteQuick( p );	break;
		case xCL2GS_CLICK_FOG_SQUAD:		retv = pUser->RecvClickFogSquad( p );	break;
		case xCL2GS_CHECK_UNLOCK_UNIT: retv = pUser->RecvCheckUnlockUnit( p );	break;
		case xCL2GS_TOUCH_SQUAD_IN_READY: retv = pUser->RecvTouchSquadInReadyScene( p );	break;
		case xCL2GS_CONTROL_SQUAD_IN_BATTLE: retv = pUser->RecvControlSquadInBattle( p );	break;
		case xCL2GS_CHECK_ENCOUNTER_LOG: retv = pUser->RecvCheckEncounterLog( p );	break;
		case xCL2GS_GOTO_HOME:			retv = pUser->RecvGotoHome( p );	break;
		case xCL2GS_ACCOUNT_REGISTER_FACEBOOK: retv = pUser->RecvRegisterFacebook( p );	break;
		case xCL2GS_OPEN_GUILD_RAID:	retv = pUser->RecvSendReqGuildRaidOpen( p );	break;
		case xCL2GS_END_SEQ:			retv = pUser->RecvEndSeq( p );	break;
		case xCL2GS_REGIST_PUSHMSG:				pUser->RecvPushMsgRegist(p);		break;
		case xCL2GS_UNREGIST_PUSHMSG:			pUser->RecvPushMsgUnRegist(p);		break;
		case xCL2GS_PUSH_GCM_REGIST_ID: retv = pUser->RecvGCMResistId( p );	break;
		case xCL2GS_LOBBY:						pUser->Cheat_AddPostInfo();				break; //임시 패킷.( 서버 테스트용으로만 사용합니다. 사용을 자재해주세요.)
		case xCL2GS_PAYMENT_ASSET: retv = pUser->RecvPaymentAssetByGem( p );	break;
		case xCL2GS_PRIVATE_RAID_ENTER_LIST: retv = pUser->RecvPrivateRaidEnterList( p );	break;
		case xCL2GS_SPOT_UPDATE_FOR_BATTLE: retv = pUser->RecvUpdateSpotForBattle( p );	break;
		case xCL2GS_ENTER_READY_SCENE: retv = pUser->RecvEnterReadyScene( p );	break;
		case xCL2GS_SYNC:		retv = pUser->RecvSync( p );	break;

		default:
			// 알수없는 패킷이 날아왔다.
			XBREAKF(1, "unknown packet 0x%8x", idPacket);
			{
				DoDisconnect();
				return FALSE;
			}
			break;
		}
	}
	TRACE( "recv packet: end\n" );
	// 불량 패킷 보내서 0을 리턴한 유저는 즉각 끊어버린다.
	if( retv == 0 )
		OnInvalidVerify();
	return TRUE;
}

void XClientConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
#pragma message("여기서 에러를 바로 처리하지말고 에러메시지만 쌓아놨다가 메인스레드에서 일괄 처리하자.")
	if( GetspUser() ) {
		auto pUser = SafeCast<XGameUser*>( GetspUser().get() );
		if( error == XENetworkConnection::xERR_PACKET_SIZE_TO_LARGE ) {
			CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.",  GetszIP(), param1 );
		}
	} else {
		CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.", GetszIP(), param1 );
	}
}

XSPGameUser XClientConnection::CreateUserObj( ID idAccount
																						, XSPSAcc spAccount
																						, BOOL bReconnect )
{
	XSPUserBase spUser = CreateAddUser( spAccount );
	if( spUser ) {
		auto spGUser = std::static_pointer_cast<XGameUser>( spUser );
		spGUser->SuccessLoginBeforeSend( spAccount, bReconnect );
		return spGUser;
	}
	return nullptr;
}

void XClientConnection::RecvLogin( XPacket& p )
{
	MAIN->m_fpsFromClient.Process();
	ID idAccount;
	int verCGPK;
	char c0;
	TCHAR szSessionKey[256];
	p >> verCGPK;		// 클라의 프로토콜 버전(클라<->게임서버간)
	p >> idAccount;
	p.ReadString( szSessionKey );
	p >> c0;		auto platform = (XGAME::xtPlatform)c0;
	p >> c0;
	p >> c0;
	p >> c0;
	// 프로퍼티 시리얼라이즈
#ifdef _XPROP_SERIALIZE
	if( XE::IsEmpty(szSessionKey) ) {
		XPacket ar( (ID)xCL2GS_PROP_SERIALIZE );
		XGame::sGet()->ArchivingProp( ar );
		Send( ar );
	}
#endif // _XPROP_SERIALIZE

	// 로긴서버로부터 받은 계정중에 내 아이디가 있는지 찾는다.
	auto pGame = GetGame();
	auto pAccLogin = pGame->FindLoginAccount( idAccount );
	if( !pAccLogin ) {
		pGame->AddLoginAccount( idAccount, nullptr );
		pAccLogin = pGame->FindLoginAccount( idAccount );
	}
	if( XBREAK( pAccLogin == nullptr ) )
		return;
	if( pAccLogin->m_spAcc ) {
		// 아직 계정데이타가 도착 안함.
		auto spAccount = pAccLogin->m_spAcc;
		spAccount->SetverCGPK( verCGPK );
		// 유저객체 만들고 로그인 성공시킴
		auto spGameUser = CreateUserObj( idAccount, spAccount, FALSE );
		GetGame()->DelLoginAccount( idAccount );
		//
		XPacket ar( (ID)xCL2GS_ACCOUNT_SUCCESS_LOGIN, true, 0x10000 );
		XTRACE("%s", __TFUNC__);
		// reserved
		XBREAK( XGC->m_apPerOnce > 255 );
		ar << (BYTE)XGC->m_apPerOnce;
		ar << (BYTE)m_CryptObj.GetkeyPrivate();
		ar << (WORD)0;
		for( int i = 0; i < 16; ++i )
			ar << (DWORD)0;
		XArchive arAcc(0x10000);
		spAccount->Serialize( arAcc );
		ar << arAcc;
		// 암호를 해제할수 있는 랜덤키를 먼저 보냄
		m_CryptObj.SerializeKeyTable( ar );
		// 퍼블릭키를 암호화해서 보냄.
// 		BYTE cBuff[ 4096 ];
// 		int sizeBuff;
// 		sizeBuff = XMain::sGet()->GetPublicKeyWithEncrypted( cBuff );
// 		ar.WriteBuffer( cBuff, sizeBuff );
//		ar <<  XMain::sGet()->GetstrPublicKey();
		Send( ar );
		///< 클라에 계정정보를 보낸후 처리할일들을 한다.
		spGameUser->SetPlatform( platform );
		spGameUser->SuccessLoginAfterSend();
		MAIN->m_fpsToClientOk.Process();
	} else {
		++pAccLogin->m_cntTry;
		XBREAK( pAccLogin->m_cntTry > 5 );		// 서버에서 직접끊는게 아니라 이런경우가 있을수도 있을듯.
		// 로긴서버로부터 아직 계정정보가 도착안했다
		XTRACE( "idAcc:%d 의 계정정보가 로긴서버로부터 아직 도착하지 않음", idAccount );
		const bool bToLogin = (pAccLogin->m_cntTry >= 3);
		OnAccountInfoNotYetFromLoginSvr( idAccount, bToLogin );
		// 3번시도했는데도 없으면 로긴서버부터 다시 접속하게 하고 삭제시킴
		if( bToLogin )
			pGame->DelLoginAccount( idAccount );
	}
}

/**
 @brief 로긴서버로부터 아직 계정정보가 도착안했다
 @param bToLogin true면 로긴서버부터 다시 접속하도록함.
*/
void XClientConnection::OnAccountInfoNotYetFromLoginSvr( ID idAccount, bool bToLogin )
{
	const float sec = (bToLogin)? xRandomF(3.f,5.f) : xRandomF(1.f,2.f);
	// x초후에 다시 재접할것을 요청함.
	XPacket ar( (ID)xCL2GS_ACCOUNT_RECONNECT_TO_GAMESVR );
	ar << (WORD)(sec * 1000);
	ar << (BYTE)xboolToByte(bToLogin);		//
	Send( ar );
	MAIN->m_fpsToClientRetry.Process();
//	CONSOLE( "idAcc:%d 의 계정정보가 로긴서버로부터 아직 도착하지 않아 대기열에 넣음" );

// #if _DEV_LEVEL <= DLV_DEV_EXTERNAL
// 	CONSOLE( "idAcc:%d 의 계정정보가 로긴서버로부터 아직 도착하지 않아 접속끊음.", idAccount );
// #endif
// 	// 일단은 별다른 처리없이 즉각 끊어버림.
// //	SendMsgBox( NULL, 465 );
// 	SendLoginFailedGameSvr( 0 );
// 	DoDisconnect();
}

// 여러가지 이유로 게임서버에 접속실패함.
void XClientConnection::SendLoginFailedGameSvr( DWORD idMsg )
{
	XPacket ar((ID)xCL2GS_LOGIN_FAILED_GAMESVR);
	ar << idMsg;
	Send( ar );
}

// 접속해제 요청이 들어온 커넥션을 끊음.
void XClientConnection::RecvDisconnectMe( XPacket& ar )
{
	DoDisconnect();
}

void XClientConnection::SendNotify(_tstring strMessage)
{
	auto pUser = SafeCast<XGameUser*>(GetspUser().get());
	if( pUser )
		pUser->SendUserNotify(strMessage);
}

// void XClientConnection::SendGuildInfo(ID GuildIndex, ID GuildUpdateflag, XPacket& ar)
// {
// 	auto pUser = SafeCast<XGameUser*>(GetspUser().get());
// 	if (pUser) {
// 		if (GuildUpdateflag == (ID)XGAME::xtGuildUpdate::xGU_UPDATE_CREATE)	{
// 			if (pUser->GetspAcc()->GetGuildIndex() == 0)	{
// 				pUser->Send( ar);
// 			}
// 		}	else {
// 			if (pUser->GetspAcc()->GetGuildIndex() == GuildIndex) {
// 				pUser->Send(ar);
// 			}
// 		}
// 	}
// }
void XClientConnection::SendGuildInfoUpdate(ID idaccount, XPacket& ar)
{
	auto pUser = SafeCast<XGameUser*>(GetspUser().get());
	if (pUser) {
		if (pUser->GetspAcc()->GetidAccount() == idaccount) {
			pUser->Send(ar);
		}
	}
}

void XClientConnection::SendInvalidVerify()
{
	XPacket ar( (ID)xCL2GS_INVALID_VERIFY );
	Send( ar );
}

void XClientConnection::SendAccountDuplicateLogout()
{
	XPacket ar( (ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT );
	Send( ar );		// 클라이언트로 데이타를 보냄
}

// void XClientConnection::SendPostInfo( XSPSAcc pAcc )
// {
// 	XPacket ar( (ID)xCL2GS_LOBBY_POST_INFO );
// 	pAcc->SerializePostInfo( ar );
// 	Send( ar );		// 클라이언트로 데이타를 보냄
// }