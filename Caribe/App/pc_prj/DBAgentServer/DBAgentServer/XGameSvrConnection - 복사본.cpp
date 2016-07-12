#include "StdAfx.h"
#include "XSocketForGameSvr.h"
#include "XGameSvrConnection.h"
#include "XMain.h"
#include "XPacketGDB.h"
#include "XDBUAccount.h"
#include "XLegion.h"
#include "XFramework/XDBMng2.h"
#include "XDatabase.h"
#include "XUserDB.h"
#include "XWorld.h"
#include "XRanking.h"
#include "server/XGuildMgr.h"
#include "XStageObj.h"
#include "XCampObj.h"
#include "rapidjson\document.h"
#include "XSpots.h"
#include "XFramework/XGoogleVerify.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
//#define BTRACE(F,...)		(0)

using namespace XGAME;
using namespace xCampaign;

extern XRanking* RANKING_DB;

#define DBMNG	XDBMng2<XGameSvrConnection>::sGet()

#define xTO_USER      1     // 받는측에서 유저가 접속해있어야 함
#define xNO_USER      0     // 받는측에서 유저가 접속해 있지 않아도 됨.

#ifdef _INAPP_APPLE
#define ITMS_PROD_VERIFY_RECEIPT_URL        "https://buy.itunes.apple.com/verifyReceipt"
#define ITMS_SANDBOX_VERIFY_RECEIPT_URL     "https://sandbox.itunes.apple.com/verifyReceipt"

#define KNOWN_TRANSACTIONS_KEY              "knownIAPTransactions"
#define ITC_CONTENT_PROVIDER_SHARED_SECRET  "206ba0852e3a4e3780b111e50cbec595"

struct xAPPLE_PURCHASE
{
	std::string cstrPurchaseInfo;
	ID idAcc;
	float price;
	int count;
	_tstring lngUserNo;
	_tstring szUserID;
	_tstring szUserName;
	_tstring szPayAmt;
	_tstring szTID;
	_tstring szTDate;
	_tstring szPGCode;
	_tstring szUserIP;
	_tstring szPackName;
	_tstring szJsonReceiptFromClient;
	_tstring szSignature;
};
#endif

////////////////////////////////////////////////////////////////
class XArchiveResourceWorld
{
	int m_Ver, m_Score;
	DWORD m_secLastCalc;
	XArchive m_arResouce;
	XArchive m_arWorld;
	XWorld *m_pWorld;
	xResourceAry m_aryResource;
	void Init() {
		m_pWorld = NULL;
	}
	void Destroy() {
		SAFE_DELETE(m_pWorld)
	}
	//
	XWorld* DeSerializeWorld() {
		XWorld *pWorld = new XWorld;
		pWorld->DeSerialize(m_arWorld);
		m_pWorld = pWorld;
		return pWorld;
	}
	xResourceAry& DeSerializeResource() {
		XAccount::sDeserializeResource(m_arResouce, m_aryResource);
		//		m_arResouce >> m_aryResource;
		return m_aryResource;
	}
public:
	XArchiveResourceWorld(XArchive& ar)
		: m_aryResource(XGAME::xRES_MAX) {
		Init();
		ar >> m_Ver;
		ar >> m_secLastCalc;
		ar >> m_Score;
		ar >> m_arResouce;
		ar >> m_arWorld;
		DeSerializeResource();
		DeSerializeWorld();
	}
	virtual ~XArchiveResourceWorld() { Destroy(); }
	//
	GET_ACCESSOR(int, Ver);
	GET_ACCESSOR(int, Score);
	GET_ACCESSOR(DWORD, secLastCalc);
	GET_ACCESSOR(XWorld*, pWorld);
	GET_ACCESSOR(xResourceAry&, aryResource);
	void Serialize(XArchive& ar) {
		if (XASSERT(m_pWorld))
		{
			// 바뀐 월드데이타를 다시 아카이빙함.
			m_arWorld.Clear();
			m_pWorld->Serialize(m_arWorld);
		}
		m_arResouce.Clear();
		XAccount::sSerializeResource(m_arResouce, m_aryResource);
		//
		ar << m_arResouce;
		ar << m_arWorld;
		ar << m_Score;

	}
};

#ifdef _DUMMY_GENERATOR
	#define xQUEUE_SIZE		(1024*1024*50)
#else
	#define xQUEUE_SIZE		(1024*1024*10)
#endif // _DUMMY_GENERATOR

//////////////////////////////////////////////////////////////////////////
XGameSvrConnection::XGameSvrConnection(SOCKET socket, LPCTSTR szIP)
	: XEWinConnectionInServer( _T("connGsvrBydbsvr")
													, MAIN->GetpSocketForGameSvr()
													, socket, szIP, xQUEUE_SIZE)
{
	Init();
// 	m_spLock = std::make_shared<XLock>();
	m_numConnectedClient = 0;
	m_GameSvrPort = 0;
	m_cIPExternal[0] = 0;
}

void XGameSvrConnection::OnError(xtError error, DWORD param1, DWORD param2)
{
}

BOOL XGameSvrConnection::ProcessPacket(DWORD idPacket, ID idKey, XPacket& p)
{
	switch (idPacket)
	{
		//	case xLS2GS_PK_CONNECT_SUCCESS:			RecvGameSvrInfo( p );
		//	case xLPK_REGISTER_ACCOUNT:				RecvRegisterAccount( p );					break;
		//	case xLPK_SAVE_ACCOUNT_INFO:			RecvSaveAccountInfo( p );					break;
		//	case xLS2GS_DID_FINISH_LOAD:			RecvDidFinishLoad( p );						break;
	case xGDBPK_ACCOUNT_NICKNAME_DUPLICATE:	RecvAccountNickNameDuplicateCheck(p, idKey); break;
	case xGDBPK_ACCOUNT_NAME_DUPLICATE:		RecvAccountNameDuplicateCheck(p, idKey);	break;
	case xGDBPK_ACCOUNT_NAME_REGIST:		RecvAccountNameRegist(p, idKey);			break;
	case xGDBPK_ACCOUNT_NAME_REGIST_CHECK:	RecvAccountNameRegistCheck(p, idKey);		break;
	case xGDBPK_ACCOUNT_NICKNAME_CHANGE:	RecvAccountNickNameChange(p, idKey);		break;
	case xGDBPK_LOGIN_ACCOUNT_INFO_LOAD:	RecvAccountInfoLoad(p, idKey);			break;
	case xGDBPK_SAVE_ACCOUNT_INFO:			RecvAccountInfoSave(p, idKey);			break;
	case xGDBPK_UPDATE_ACCOUNT_LOGOUT:		RecvAccountLogOut(p, idKey);				break;
	case xGDBPK_SAVE_ACCOUNT_LOGIN:			RecvAccountLoingSave(p, idKey);				break;
	case xGDBPK_GET_USER_LEGION_BY_POWER:	RecvGetUserLegionByPower(p, idKey);		break;
	case xGDBPK_GET_MAIN_LEGION_BY_IDACC:	RecvGetUserLegionByIdAcc(p, idKey);		break;

	case xGDBPK_GET_USER_BY_POWER:	RecvGetUserByPower(p, idKey);	break;
	case xGDBPK_REQ_JEWEL_MINE:		RecvReqMatchJewelMine(p, idKey);	break;
	case xGDBPK_GET_JEWEL_LEGION_BY_IDACC:	RecvGetJewelLegionByIdAcc(p, idKey);	break;
	case xGDBPK_CHANGE_JEWEL_OWN:			RecvChangeJewelOwn(p, idKey);	break;
	case xGDBPK_ADD_DEFENSE_BY_JEWEL:	RecvAddDefenseByJewel(p, idKey);	break;
	case xGDBPK_UPDATE_JEWEL_INFO:		RecvUpdateJewelMineInfo( p, idKey ); break;
	case xGDBPK_DEFEAT_INFO:			RecvDefeatInfo(p, idKey);	break;
	case xGDBPK_ENCOUNTER_WIN:			RecvEncounterWin(p, idKey);	break;
	case xGDBPK_REQ_MATCH_MANDRAKE:		RecvReqMatchMandrake(p, idKey);	break;
	case xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC:	RecvGetMandrakeLegionByIdAcc(p, idKey);	break;
	case xGDBPK_CHANGE_MANDRAKE_OWN:	RecvChangeMandrakeOwn(p, idKey);	break;

	case xGDBPK_SHOP_LISTINFO_UPDATE:	RecvAccountShopInfoUpdate(p, idKey);	break;

	case xGDBPK_POST_INFO:				RecvAccountPostInfo(p, idKey);			break;
	case xGDBPK_POST_INFO_LOGIN:		RecvAccountPostInfo(p, idKey);			break;
	case xGDBPK_POST_INFO_UPDATE:		RecvAccountPostInfoUpdate(p, idKey);	break;
	case xGDBPK_POST_DESTROY:			RecvPostInfoDestroy(p, idKey);			break;
	case xGDBPK_POST_ITEMS_REMOVE:		RecvPostItemsRemove(p, idKey);			break;
	case xGDBPK_POST_ADD:				RecvPostInfoAdd(p, idKey);				break;

	case xGDBPK_RANK_INFO:				RecvRankingInfo(p, idKey);				break;
	case xGDBPK_UPDATE_CASH:			RecvUpdateCashInfo(p, idKey);			break;
		//GUILD		
	case xGDBPK_GUILD_INFO:				RecvGuildInfo(p, idKey);				break;
	case xGDBPK_GUILD_CREATE:			RecvGuildCreate(p, idKey);				break;
	case xGDBPK_GUILD_JOIN_REQ:			RecvGuildJoinReq(p, idKey);				break;
	case xGDBPK_GUILD_JOIN_ACCEPT:		RecvGuildJoinAccept(p, idKey);			break;
	case xGDBPK_GUILD_UPDATE:			RecvGuildUpdate(p, idKey);				break;
	case xGDBPK_GUILD_UPDATE_OPTION:	RecvGuildUpdateOption(p, idKey);		break;
	case xGDBPK_GUILD_UPDATE_CONTEXT:	RecvGuildUpdateContext(p, idKey);		break;
	case xGDBPK_GUILD_UPDATE_MEMBER:	RecvGuildUpdateMember(p, idKey);		break;
	case xGDBPK_GUILD_OUT:				RecvGuildOut(p, idKey);					break;
	case xGDBPK_GUILD_KICK:				RecvGuildKick(p, idKey);					break;
	case xGDBPK_TRADE_GEM_CALL:			RecvTradeGemCall(p, idKey);				break;
	case xGDBPK_SHOP_GEM_CALL:			RecvShopGemCall(p, idKey);				break;
	case xGDBPK_DEFENSE_INFO_TO_DEFENDER:	RecvDefenseInfo(p, idKey);			break;
	case xGDBPK_GAME_PUSH_GCM_REGIST_ID: RecvReqGCMRegistId( p, idKey); break;
	case xGDBPK_GAME_PUSH_REGIST:		RecvPushMsgRegist(p, idKey);			break;
	case xGDBPK_GAME_PUSH_UNREGIST:		RecvPushMsgUnRegist(p, idKey);			break;
	case xGDBPK_LOGIN_LOCK_FOR_BATTLE:  RecvLockLoginForBattle(p, idKey);  break;
	case xGDBPK_LOGIN_UNLOCK_FOR_BATTLE:  RecvUnlockLoginForBattle(p, idKey);  break;
	case xGDBPK_GUILD_REQ_CAMPAIGN_RAID:  RecvReqCampaignByGuildRaid(p, idKey);   break;
	case xGDBPK_GUILD_REQ_ENTER_RAID:	RecvReqGuildEnterRaid(p, idKey);	break;
	case xGDBPK_GUILD_REQ_LEAVE_RAID:	RecvReqGuildLeaveRaid(p, idKey);	break;
	case xGDBPK_GUILD_OPEN_RAID:		RecvReqGuildRaidOpen(p, idKey); break;
	case xGDBPK_REGISTER_FACEBOOK:	RecvReqRegisterFacebook( p, idKey ); break;
	case xGDBPK_REQ_HELLO_MSG_BY_IDACC:	RecvReqHelloMsgByidAcc( p, idKey ); break;

	case xGDBPK_USERLOG_ADD:			RecvUserLogAdd(p, idKey);				break;
#ifdef _INAPP_GOOGLE
	case xGDBPK_GAME_INAPP_PAYLOAD:		RecvGoogleInappPayload(p, idKey);		break;
	case xGDBPK_GAME_INAPP_GOOGLE:		RecvGoogleInappBuyCash(p, idKey);		break;
	case xGDBPK_GAME_INAPP_DELETE_PAYLOAD:	RecvDeletePayload( p, idKey );	break;
#endif
#ifdef _INAPP_APPLE
	case xGDBPK_GAME_INAPP_APPLE:		RecvAppleInappBuyCash(p, idKey);		break;
#endif
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF(1, "unknown packet 0x%x", idPacket);
		//불량 패킷 숫자를 확인 하고 Disconnect 하는 부분
		//Server <->Server 에서는 사용 하지 않음.
		//		if( ++m_cntDoubt > 5 )
		{
			//			MAIN->_DoDisconnectUser( pUser );
			return FALSE;
		}
		break;
	}
	return TRUE;
	//	TRACE( "recv packet: end" );
}

void XGameSvrConnection::RecvGameSvrInfo(XPacket& p, ID idKey)
{
	int n = 0;
}

void XGameSvrConnection::RecvDidFinishLoad(XPacket& p, ID idKey)
{
	_tstring strIPExternal;
	WORD port, w1;
	p >> port;	m_GameSvrPort = port;
	p >> w1;	SetGSvrID(w1);
	p >> strIPExternal;
	strcpy_s(m_cIPExternal, SZ2C(strIPExternal.c_str()));
	CONSOLE("GameServer DidFinishLoad: %s:%d", strIPExternal.c_str(), port);
	// 게임서버로부터 로그인준비가 끝났음을 통보받았으므로 게임클라이언트들에게 소켓을 개방함.
	//	MAIN->CreateSocketForClient();
}

/**
 @brief idAccount의 계정정보를 읽어서 게임서버에 돌려준다.
*/
void XGameSvrConnection::RecvAccountInfoLoad(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID	idAccount = 0;
	ID idConnect = 0;
	BYTE b0;
	p >> idAccount;
	p >> idConnect;
	p >> b0;  bool bSessionKeyLogin = (b0 != 0);
	p >> b0 >> b0 >> b0;
	XTRACE("DB:%s:idAcc=%d", __TFUNC__, idAccount);

//	auto spAccOut = std::make_shared<XDBUAccount>();
	auto spAccOut = XSPDBUAcc( new XDBUAccount );

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbFindAccountByIdAcc,
		spAccOut,
		idAccount,
		idConnect,
		bSessionKeyLogin,
		idKey);
}

void XGameSvrConnection::cbFindAccountByIdAcc(XDatabase *pDB,
																							XSPDBUAcc spAccOut,
																							ID idAccount,
																							ID idConnect,
																							bool bSessionKeyLogin,
																							ID idKey)
{
	XTRACE("DB:%s:idAcc=%d", __TFUNC__, idAccount);
	// 계정정보 full 로드
	xtDB retDB = pDB->GetpDB()->AccountLoadIDAccount(spAccOut, idAccount);
	// 다시 게임서버로 돌려보냄.
	int nPassCount = 0;
	XPacket pk((ID)xGDBPK_LOGIN_ACCOUNT_INFO_LOAD, idKey, true, 0x10000 );
	pk << 0;		// XGameUser의 커넥션 아이디. 새로추가된 프로토콜
	pk << (ID)spAccOut->GetidAccount();
	pk << (ID)idConnect;
	pk << (BYTE)xboolToByte(bSessionKeyLogin);
	pk << (char)retDB;
	pk << (BYTE)0;
	pk << (BYTE)0;
	if (retDB == xDB_OK) {
		XBREAK( spAccOut->GetstrUUID().empty() );
		XTRACE("%s", __TFUNC__);
		spAccOut->Serialize(pk);
		nPassCount = spAccOut->GetSubscribeOffline();
	}
	DBMNG->RegisterSendPacket( GetThis(), pk);
// 	SAFE_RELEASE_REF(spAccOut);
	if (nPassCount > 0) {

	}
}

void XGameSvrConnection::RecvAccountInfoSave(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID	idAccount = 0;
	ID idConnect = 0;
	ID idReqPacket = 0;
	int req, _cnt, cnt = 0;

	p >> idAccount;
	XTRACE("%s", __TFUNC__);
//	auto spAccOut = std::make_shared<XDBUAccount>();
	auto spAccOut = XSPDBUAcc( new XDBUAccount );
	spAccOut->DeSerialize(p);		// 이거 스레드 안에서 풀면 안되나?
	p >> req;	// 11이면 세이브 결과를 다시 알려줘야함.
	p >> idConnect;
	p >> idReqPacket;
	p >> _cnt;
	XBREAK( idReqPacket == 0 );		// 이제 0인경우는 없는걸로.
	BTRACE( "L0:%s:idAcc=%d nick=%s", __TFUNC__, idAccount, spAccOut->GetstrName() );

#ifdef _DUMMY_GENERATOR
	cnt = _cnt & 0x7fffffff;
	if ((_cnt & 0x80000000) && (cnt % 32) == 0)
		CONSOLE("save account info:%d,id=%d,lv=%d,power=%d", cnt, idAccount, spAccOut->GetLevel(), spAccOut->GetPowerIncludeEmpty());
#endif // _DUMMY_GENERATOR
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbSaveAccount,
		spAccOut,
		idAccount,
		idConnect,
		(req) ? TRUE : FALSE,
		idReqPacket,
		idKey);
}

void XGameSvrConnection::cbSaveAccount(XDatabase *pDB,
																				XSPDBUAcc spAccOut,
																				ID idAccount,
																				ID idConnect,
																				BOOL bReq,
																				ID idReqPacket,
																				ID idKey)
{
	BTRACE( "L0:%s:idAcc=%d nick=%s", __TFUNC__, idAccount, spAccOut->GetstrName() );
	// 	static int s_num = 0;
	// 	TRACE("save:%d\n", ++s_num);
	BOOL bFound = pDB->SaveAccount(idAccount, spAccOut);
	//
	if (bReq)	{
		// 재전송 패킷이 있다면 이 패킷일것이다. 다른 패킷이 추가되면 추적이 쉽도록 여기다 추가할것.
		XASSERT( idReqPacket 
					&& (idReqPacket == xGDBPK_SAVE_DUMMY_ACCOUNT 
							|| idReqPacket == xGDBPK_SAVE_ACCOUNT_INFO) );
//		ID idPacket = (idReqPacket != 0) ? idReqPacket : xGDBPK_SAVE_ACCOUNT_INFO;
// 		XPacket ar(idPacket, idKey);
		XBREAK( idReqPacket == 0 );
		XPacket ar( idReqPacket, idKey );
		ar << 0;
		ar << bFound;
		ar << idAccount;
		ar << idConnect;
		ar << spAccOut->GetstrUUID();
		BTRACE( "L1:%s:idAcc=%d nick=%s", __TFUNC__, idAccount, spAccOut->GetstrName() );
		DBMNG->RegisterSendPacket( GetThis(), ar);
	} else {
		// 세이브 결과를 리턴해주지 않아도 됨.
		if( !bFound ) {
			// 저장실패
			BTRACE( "save failed:L2:%s:idAcc=%d nick=%s============================================================"
												, __TFUNC__, idAccount, spAccOut->GetstrName() );
		}
	}
	//SAFE_RELEASE_REF(spAccOut);
}

/**
 @brief 접속종료시간을 기록하고 connectsvrid를 0으로 저장한다.
*/
void XGameSvrConnection::RecvAccountLogOut(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID newidConnect = 0;
//	ID alreadyuseridConnect = 0;
	ID	idAccount = 0;

	p >> newidConnect;
//	p >> alreadyuseridConnect;
	p >> idAccount;
	XBREAK( idAccount == 0 );

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountLogOut,
		newidConnect,
//		alreadyuseridConnect,
		idAccount);

}

void XGameSvrConnection::cbAccountLogOut(XDatabase *pDB
																						, ID newidConnect
// 																						, ID alreadyuseridConnect
																						, ID idAccount)
{
	BOOL bFound = pDB->SaveAccountLogOut(idAccount);
	if (newidConnect > 0 /*|| alreadyuseridConnect > 0*/)	{
		XPacket ar((ID)xGDBPK_UPDATE_ACCOUNT_LOGOUT);
		ar << 0;
		ar << newidConnect;
//		ar << alreadyuseridConnect;
		ar << (ID)idAccount;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

void XGameSvrConnection::cbAccountLoginSave(XDatabase *pDB, ID idAccount, ID gameserverid, _tstring strConnectIP, ID idKey)
{
//	BOOL bFound = pDB->SaveAccountLoginSave(idAccount, gameserverid, strConnectIP);
	auto bFound = pDB->GetpDB()->UpdateAccountLogin(idAccount, gameserverid, strConnectIP);
	if (bFound)
	{
		XPacket ar((ID)xGDBPK_SAVE_ACCOUNT_LOGIN, idKey);
		ar << 1;
		ar << (ID)bFound;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

/**
 @brief 접속한 게임서버의 아이디를 DB에 기록한다.
*/

void XGameSvrConnection::RecvAccountLoingSave(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID	idAccount = 0;
	ID	gameserverid = 0;
	_tstring strConnectIP;
	p >> idAccount;
	p >> gameserverid;
	p >> strConnectIP;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountLoginSave,
		idAccount,
		gameserverid,
		strConnectIP,
		idKey);
}



//#include "XAccount.h"	// 임시
/**
스코어점수 범위에 있는 유저중 한놈을 랜덤으로 뽑아 군단정보를 보내준다.
유황스팟에 사용
*/
void XGameSvrConnection::RecvGetUserLegionByPower(XPacket& p, ID idKey)
{

	int powerMin;
	int powerMax;
	int idxLegion;
	ID idFinder;
	ID idPacket;
	XArchive arRequest;

	p >> idFinder;
	p >> powerMin;
	p >> powerMax;
	p >> idxLegion;
	p >> idPacket;
	p >> arRequest;

	XArchive arParam;
	arParam << idKey;
	arParam << idPacket;
	arParam << idFinder;
	arParam << powerMin;
	arParam << powerMax;
	arParam << idxLegion;
	arParam << arRequest;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbFindAccountLegionByPower,
		arParam);
}

void XGameSvrConnection::cbFindAccountLegionByPower(XDatabase *pDB,	XArchive& arParam)
{
	XArchive arResult, arRequest;
	int powerMin, powerMax, idxLegion;
	ID idFinder, idPacket, idKey;
	arParam >> idKey;
	arParam >> idPacket;
	arParam >> idFinder;
	arParam >> powerMin;
	arParam >> powerMax;
	arParam >> idxLegion;
	arParam >> arRequest;

	BOOL bFound =
		pDB->GetpDB()->FindAccountLegionByPower(powerMin,
		powerMax,
		idxLegion,
		idFinder,
		arResult);

	if (bFound)	{
		XPacket ar((ID)xGDBPK_GET_USER_LEGION_BY_POWER, idKey);
		ar << -1;
		ar << idPacket;
		ar << arRequest;
		ar << arResult;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}	else {
		// 적절한 상대를 못찾았을땐 idAcc=0으로 해서 돌려보내준다.
		XPacket ar((ID)xGDBPK_GET_USER_LEGION_BY_POWER, idKey);
		ar << -1;
		ar << idPacket;
		ar << arRequest;
		arResult.Clear();
		arResult << 0;		// idAcc
		ar << arResult;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

/**
idAccount의 유저를 DB에서 찾아 메인 군단정보를 보낸다.
*/
void XGameSvrConnection::RecvGetUserLegionByIdAcc(XPacket& p, ID idKey)
{
	ID idSpot;
	ID idAccount;
	ID idClientConnect;
	ID idPacket;

	p >> idSpot;
	p >> idAccount;
	p >> idClientConnect;
	p >> idPacket;


	// 비동기로 찾은다음 아래 패킷으로 다시 보냄
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbGetLegionByIdAcc,
		idAccount,
		idSpot, idClientConnect, idPacket, idKey);

}

void XGameSvrConnection::cbGetLegionByIdAcc(XDatabase *pDB,
	ID idAcc,
	ID idSpot, ID idClientConnect,
	ID idPacket,
	ID idKey)
{
	XArchive arResult;
	BOOL bFound = pDB->GetLegionByIdAcc(0, idAcc, arResult);

	{
		XPacket ar((ID)xGDBPK_GET_MAIN_LEGION_BY_IDACC, idKey);
		ar << idClientConnect;
		ar << idSpot;
		ar << idPacket;
		if (bFound)	{
			ar << idAcc;
			ar << arResult;
		}	else {
			ar << 0;
		}
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

/**
 @brief 전투력 범위로 유저를 찾아 기본정보를 돌려준다.
*/
void XGameSvrConnection::RecvGetUserByPower(XPacket& p, ID idKey)
{
	ID idSpot;
	int powerMin, powerMax;
	ID idClientConnect;
	ID idPacket;
	ID idFinder;
	xSec secAdjustOffline;

	p >> idFinder;
	p >> idSpot;
	p >> powerMin >> powerMax;
	p >> secAdjustOffline;
	p >> idClientConnect;
	p >> idPacket;
	// 비동기로 scoreMin~scoreMax사이의 유저를 찾은후 아래 패킷으로 다시 보냄
	//	XSPDBUAcc spAccOut = std::make_shared<XDBUAccount>();

	// 	XBREAK( scoreMin > 0xffff );
	// 	XBREAK( scoreMax > 0xffff );
	// 	// 4바이트에 합침.
	// 	DWORD score_dw = (scoreMin << 16) | (scoreMax & 0xffff);
	XArchive arParam;
	arParam << idKey;
	arParam << powerMin;
	arParam << powerMax;
	arParam << secAdjustOffline;
	arParam << idFinder;
	arParam << idSpot;
	arParam << idClientConnect;
	arParam << idPacket;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbFindUserByPower,
		//						spAccOut,
		arParam);
}

void XGameSvrConnection::cbFindUserByPower(XDatabase *pDB, XArchive& arParam)
{
	do 	{
		ID idKey;
		int powerMin, powerMax;
		xSec secAdjustOffline;
		ID idFinder;
		ID idSpot, idClientConnect;
		ID idPacket;
		int lootWood = 0, lootIron = 0;
		arParam >> idKey;
		arParam >> powerMin;
		arParam >> powerMax;
		arParam >> secAdjustOffline;
		arParam >> idFinder;
		arParam >> idSpot;
		arParam >> idClientConnect;
		arParam >> idPacket;
		XArchive arOut;
//		XDBUAccount accOut;
		XSPDBUAcc spAccOut = XSPDBUAcc( new XDBUAccount() );
		BOOL bFound = FALSE;
		int cnt = 5;
		while( cnt-- ) {
			bFound = pDB->GetpDB()->FindAccountPowerValue( spAccOut.get(), powerMin, powerMax, idFinder, arOut );
			if( !bFound ) {
				// 범위내에서 못찾으면 10%씩 확장해가며 다시 검색한다.
				powerMin -= (int)( powerMin * 0.1f );
				powerMax += (int)( powerMax * 0.1f );
			} else {
				break;
			}
		}
		if( bFound == FALSE )
			break;
// 		if (bFound) 
// 		{
		// 월드 스팟들 정산 업데이트
		XBREAK(spAccOut->GetpWorld() == NULL);
		// 상대스팟의 현재누적된 자원양을 다시 계산한다.
		UpdateCalcWorld(spAccOut->GetidAccount(), spAccOut->GetpWorld());
		// 상대자원정보를 일단 100%그대로 보낸다.
		xResourceAry aryLocals(XGAME::xRES_MAX);
		xResourceAry aryMain( XGAME::xRES_MAX );
		// 상대 지역창고에 쌓여있는 자원양을 구한다.
		spAccOut->GetpWorld()->GetNumResLocalStorageByAllSpot(&aryLocals);
		// 중앙창고의 양은 루팅비율때문에 따로 보내야한다.
		for (int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i) {
			auto type = (XGAME::xtResource) i;
			int num = spAccOut->GetResource(type);
			aryMain[type] += num;
		}
		{
			XPacket pk((ID)xGDBPK_GET_USER_BY_POWER, idKey);
			pk << idClientConnect;
			pk << idPacket;
			pk << idSpot;
			pk << secAdjustOffline;
			pk << spAccOut->GetidAccount();
			pk << aryLocals;
			pk << aryMain;
			pk << (int)spAccOut->GetGold();
			pk << spAccOut->GetLevel();
			pk << spAccOut->GetstrName();
			pk << spAccOut->GetPowerIncludeEmpty();
			pk << spAccOut->GetLadder();
			pk << spAccOut->GetstrFbUserId();
			pk << spAccOut->GetstrHello();
			DBMNG->RegisterSendPacket( GetThis(), pk);
		}
// 		}	
//		else {
// 			xResourceAry aryDummy(XGAME::xRES_MAX);
// 			/*
// 			조건에 맞는 매칭유저를 못찾을경우 npc를 만들지 말고 범위를 더 확장시켜
// 			어쨋든 유저를 찾도록 하는게 좋을것 같다.
// 			만약 몇번의 확장에도 못찾는경우(엄청난 고렙)가 있다면 그냥 매칭실패로해서
// 			계속 자기 성인 상태로 만들자.
// 			못찾는경우를 가급적 없애기위해 다양한 범위로 더미계정을 많이 만들자.
// 		  *이것은 성은 무조건 유저만 있게 한다는 논리를 고정시켜 복잡도를 줄이기 위함.
// 			*/
// 			XPacket pk((ID)xGDBPK_GET_USER_BY_POWER, idKey);
// 			pk << idClientConnect;
// 			pk << idPacket;
// 			pk << idSpot;
// 			pk << secAdjustOffline;
// 			pk << 0;		// 검색못함. 그냥 자기성.
// 			DBMNG->RegisterSendPacket( GetThis(), pk);
//		}
	} while (0);
}

/**
 @brief 전투력으로 적절한 광산매치상대를 찾는다.
*/
void XGameSvrConnection::RecvReqMatchJewelMine(XPacket& p, ID idKey)
{
	ID idClientConnect;
	XArchive arParam;
	p >> idClientConnect;
	p >> arParam;
	// 비동기로 DB에서 powerMin ~ powerMax 사이의 유저중 idxMine번 광산의
	// 상대가 없는 유저를 찾는다.
	// 찾았으면 이사람 DB의 광산정보에 상대자로 idFinder를 기록한다.
	DBMNG->DoAsyncCall( GetThis(),
										&XGameSvrConnection::cbFindJewelMatch,
										arParam,
										idClientConnect,
										idKey );
}

void XGameSvrConnection::cbFindJewelMatch(XDatabase *pDB
																				, XArchive& arParam
																				, ID idClientConnect
																				, ID idKey )
{
	ID idFinder;
	int powerMin, powerMax;
	char c0;
	arParam >> idFinder;
	arParam >> c0;	int idxMine = c0;
	arParam >> c0;	bool bSimul = xbyteToBool(c0);
	arParam >> c0;	bool bSimulWin = xbyteToBool(c0);
	arParam >> c0;
	arParam >> powerMin >> powerMax;
	XGAME::xJewelMatchEnemy infoMatch;
	auto bFound 
		= pDB->GetpDB()->FindAccountMineEnemyZero2( idFinder
																						, idxMine
																						, powerMin
																						, powerMax
																						, &infoMatch );
	//
	XPacket ar( (ID)xGDBPK_REQ_JEWEL_MINE, idKey );
	ar << idClientConnect;
	ar << (char)idxMine;
	ar << (BYTE)xboolToByte( bSimul );
	ar << (BYTE)xboolToByte( bSimulWin );
	ar << (BYTE)xboolToByte( bFound );
	//
	if (bFound) {
		XArchive arMatch;
		infoMatch.Serialize( arMatch );
		ar << arMatch;
	}
	DBMNG->RegisterSendPacket( GetThis(), ar );
}

/**
idAccount의 유저를 DB에서 찾아 보석광산 군단정보를 보낸다.
*/
void XGameSvrConnection::RecvGetJewelLegionByIdAcc(XPacket& p, ID idKey)
{
	int idxMine;
	ID idAccount;
	ID idClientConnect;
	ID idPacket;

	p >> idxMine;
	p >> idAccount;
	p >> idClientConnect;
	p >> idPacket;
	// 비동기로 찾은다음 아래 패킷으로 다시 보냄

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbFindJewelInfoByIdAcc,
		idAccount,
		idClientConnect,
		idPacket,
		idxMine,
		idKey);

}

void XGameSvrConnection::cbFindJewelInfoByIdAcc(XDatabase *pDB,
																								ID idAccount,
																								ID idClientConnect,
																								ID idPacket,
																								int idxMine,
																								ID idKey)
{
	_tstring strName;
	XArchive arPacket;
	//	arPacket << idAccount;
	// idxMine의 레벨, 방어도, 군대정보, secLastCalc, 월드정보를 읽어와 정산하여 idxMine의 로컬창고에 현재 자원이 얼마나 쌓여있는지 
	BOOL bFound = pDB->FindJewelInfoByIdAcc(idAccount, idxMine, arPacket);
	//	arPacket << idxMine;
	if( bFound ) {
		int level, levelMine, defense, ver, power, score;
		XArchive arLegion;
		XArchive arAbil;
		arPacket >> ver;
		arPacket >> level;
		arPacket >> strName;
		arPacket >> levelMine;
		arPacket >> defense;
		arPacket >> power;
		arPacket >> score;
		arPacket >> arLegion;
		arPacket >> arAbil;
		XWorld *pWorld = NULL;
		{
			XArchive arWorld;
			arPacket >> arWorld;
			pWorld = new XWorld;
			pWorld->DeSerialize(arWorld);
			pWorld->OnAfterDeSerialize(NULL, idAccount);
		}
		int lootJewel = 0;
		XSpotJewel *pSpot = pWorld->GetSpotJewelByIdx(idxMine);
		if (pSpot) {
			// 루팅 가능한 양 계산.
			lootJewel = (int)(pSpot->GetLocalStorageAmount() * XGC->m_rateJewelStorageLoss);
		}
		SAFE_DELETE(pWorld);
		// 읽은 정보를 다시 돌려보냄.
		XPacket ar((ID)xGDBPK_GET_JEWEL_LEGION_BY_IDACC, idKey);
		ar << idClientConnect;
		ar << (int)bFound;
		ar << idPacket;
		ar << idxMine;
		ar << ver;
		ar << idAccount;
		ar << level;
		ar << strName;
		ar << levelMine;
		ar << defense;
		ar << power;
		ar << score;
		ar << arLegion;
		ar << arAbil;
		ar << lootJewel;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	} else {
		// 읽은 정보를 다시 돌려보냄.
		XPacket ar((ID)xGDBPK_GET_JEWEL_LEGION_BY_IDACC, idKey);
		ar << idClientConnect;
		ar << (int)bFound;
		ar << idPacket;
		ar << idxMine;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}
/**
idDBAcc가 갖고 있던  idxMine광산의 소유정보를 바꾼다.
*/
void XGameSvrConnection::RecvChangeJewelOwn(XPacket& p, ID idKey)
{
	XArchive arJewel;
	ID idDBAcc;
	p >> idDBAcc;
	p >> arJewel;
	xSec secUpdate = XTimer2::sGetTime();
	DBMNG->DoAsyncCall( GetThis(),
										&XGameSvrConnection::cbChangeOwnerJewelMine,
										idDBAcc, arJewel, secUpdate);
}
void XGameSvrConnection::cbChangeOwnerJewelMine(XDatabase *pDB
																							, ID idDBAcc
																							, XArchive& arJewel
																							, xSec secUpdate )
{
	XGAME::xJewelInfo info;
	info.DeSerialize( arJewel );	// 스레드에서 푸는게 더 빨라서 여기다 넣음.
	// 월드데이타 읽어서 광산 정산.
	{
		XArchive arResult;	// DB에서 읽어온 데이타가 담길 아카이브
		pDB->GetpDB()->LoadResourceWorldByIdAcc( idDBAcc, arResult );
		XArchiveResourceWorld arObj( arResult );
		XWorld *pWorld = arObj.GetpWorld();
		// 현재 시점으로 정산.
		pWorld->OnAfterDeSerialize( NULL, idDBAcc );
		// 해당광산의 지역창고자원 30% 손실시킴
		auto pSpot = pWorld->GetSpotJewelByIdx( info.m_idxJewel );
		if( XASSERT( pSpot ) )
			pSpot->LossLocalStorage( 0.3f, nullptr );
		// 다시 원래주인의 DB에 써넣음.
		{
			XArchive ar;
			arObj.Serialize( ar );
			pDB->GetpDB()->SaveResourceWorldByIdAcc( idDBAcc, ar );
		}
	}
	//
	auto bOk = pDB->GetpDB()->ChangeOwnerJewelMine2( idDBAcc, info, secUpdate );
}

void XGameSvrConnection::RecvAddDefenseByJewel(XPacket& p, ID idKey)
{
	XArchive arJewel;
	p >> arJewel;
	// DB에서 idAccount의 정보를 찾아 idxMine의 방어도를 add만큼 더한다
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbUpdateJewelMine,
		arJewel, idKey);
}

void XGameSvrConnection::cbUpdateJewelMine(XDatabase *pDB,
																						XArchive& arJewel,
																						ID idKey)
{
	int idxMine, levelMine, defense;
	ID idOldOwner, idNewOwner;
	_tstring strName;
	DWORD secLastEvent;
	arJewel >> idxMine >> idOldOwner >> idNewOwner >> strName >> levelMine;
	arJewel >> defense >> secLastEvent;
	BOOL bOk = pDB->ChangeOwnerJewelMine(idxMine,
																			idOldOwner,
																			idNewOwner,
																			strName,
																			levelMine,
																			defense,
																			secLastEvent);
}

/**
 @brief idDBAcc유저의 DB에 광산정보를 업데이트 한다.
*/
void XGameSvrConnection::RecvUpdateJewelMineInfo( XPacket& p, ID idKey )
{
	ID idDBAcc;
	XArchive arInfo;
	p >> idDBAcc;
	p >> arInfo;
	xSec secUpdate = XTimer2::sGetTime();
	DBMNG->DoAsyncCall( GetThis()
										, &XGameSvrConnection::cbUpdateJewelMine2
										, idDBAcc
										, arInfo
										, secUpdate
										, idKey );
}

void XGameSvrConnection::cbUpdateJewelMine2(XDatabase *pDB,
																						ID idDBAcc,
																						XArchive& arInfo,
																						xSec secUpdate,
																						ID idKey)
{
	XGAME::xJewelInfo info;
	info.DeSerialize( arInfo );	// 스레드에서 푸는게 더 빨라서 여기다 넣음.
	auto bOk = pDB->GetpDB()->ChangeOwnerJewelMine2( idDBAcc, info, secUpdate );
	// 상대가 계정을 삭제했을경우 false가 나올수 있음.
}

/**
pWorld의 스팟들을 spAcc를 참조하여 정산한다.
spAcc에는 정산에 필요한 최소한의 정보만 세팅되어 있다.
.m_idAccount
.m_secLastCalc
*/
// void XGameSvrConnection::UpdateCalcWorld(XDBUAccount& acc, XWorld *pWorld) const
void XGameSvrConnection::UpdateCalcWorld( ID idAcc, XWorld *pWorld ) const
{
	XBREAK(pWorld == nullptr);
	if (pWorld)
	{
		XBREAK(pWorld->GetsecLastCalc() == 0);
		pWorld->OnAfterDeSerialize(nullptr, idAcc );
	}
}


/**
현재 pWorld에서 약탈할수 있는 목재/철 최대량
*/
// void XGameSvrConnection::GetLootingAmount( XWorld *pWorld, int *pOutWood, int *pOutIron )
// {
// 	pWorld->GetLootingAmount( pOutWood, pOutIron );
// }

/**
idAcc유저가 본진방어에 실패함.
파워등급에 따라 자원을 손실시킴.

*/
void XGameSvrConnection::RecvDefeatInfo(XPacket& p, ID idKey)
{
	BYTE b0;
	char c0;
	ID idDefender, idAttacker;
	p >> idDefender;
	p >> idAttacker;
	p >> b0;	auto typeSpot = (XGAME::xtSpot)b0;
	p >> c0;	int powerGrade = c0;	// 패배자의 색깔등급.
	p >> b0 >> b0;
	XArchive arParam;
	arParam << idDefender;
	arParam << idAttacker;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbDefeatInfo,
		arParam, typeSpot, powerGrade, idKey);
}

void XGameSvrConnection::cbDefeatInfo(XDatabase *pDB,
	XArchive& arParam,
	XGAME::xtSpot typeSpot,
	int powerGrade,
	ID idKey)
{
	ID idDefender, idAttacker;
	XArchive arResult;

	arParam >> idDefender;
	arParam >> idAttacker;
	// idAcc유저의 자원과 월드정보를 읽어온다.
	if (XBREAK(idDefender == 0))
		return;
	if (typeSpot == XGAME::xSPOT_CASTLE) {
		BOOL bFound = pDB->GetpDB()->LoadResourceWorldByIdAcc(idDefender, arResult);
		if (XBREAK(bFound == FALSE))
			return;
		XArchiveResourceWorld arObj(arResult);
		XWorld *pWorld = arObj.GetpWorld();
		pWorld->OnAfterDeSerialize(NULL, idDefender);
		// 정산후 마지막 정산시간도 업데이트 해야한다.
		const float rateLossMain = XGC->GetRateLossMain( powerGrade );
		// 파워등급에 따라 중앙창고의 자원을 손실시킨다.
		int numWood = arObj.GetaryResource()[XGAME::xRES_WOOD];
		numWood = (int)(numWood * (1.0f - rateLossMain));
		arObj.GetaryResource()[XGAME::xRES_WOOD] = numWood;
		int numIron = arObj.GetaryResource()[XGAME::xRES_IRON];
		numIron = (int)(numIron * (1.0f - rateLossMain));
		arObj.GetaryResource()[XGAME::xRES_IRON] = numIron;
		// 보석/유황/만드레이크는 소실된다.
		int numJewel = arObj.GetaryResource()[XGAME::xRES_JEWEL];
		numJewel = (int)(numJewel * (1.0f - rateLossMain));
		arObj.GetaryResource()[XGAME::xRES_JEWEL] = numJewel;
		int numSulfur = arObj.GetaryResource()[XGAME::xRES_SULFUR];
		numSulfur = (int)(numSulfur * (1.0f - rateLossMain));
		arObj.GetaryResource()[XGAME::xRES_SULFUR] = numSulfur;
		int numMandrake = arObj.GetaryResource()[XGAME::xRES_MANDRAKE];
		numMandrake = (int)(numMandrake * (1.0f - rateLossMain));
		arObj.GetaryResource()[XGAME::xRES_MANDRAKE] = numMandrake;
		// 모든 지역창고에 손실을 입힌다.
		const float rateLossByLocal = XGC->GetRateLossLocal( powerGrade );
		pWorld->LossLocalStorageAll(rateLossByLocal, nullptr );
		{
			XArchive ar;
			arObj.Serialize(ar);
			pDB->GetpDB()->SaveResourceWorldByIdAcc(idDefender, ar);
		}
	} else
		if (typeSpot == XGAME::xSPOT_JEWEL) {
		} else
		if (typeSpot == XGAME::xSPOT_MANDRAKE) {
		}
	// 공격자에게 결과를 다시 보내줌.
	{
		XPacket ar((ID)xGDBPK_DEFEAT_INFO, idKey);
		ar << 1;    // idClientConnect. 현재는 0이 아니기만 하면 됨.
		ar << idDefender;
		ar << idAttacker;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
	// idDefender의 로그인락을 해제하고 게임서버로 알림.
// 	DBMNG->DoAsyncCall( GetThis(),
// 													&XGameSvrConnection::cbUnlockLoginForBattle,
// 													idDefender, idAttacker);
}

/**
@brief 유저가 공격을 받았을때 오프라인 유저에게 발생한다.
*/
void XGameSvrConnection::RecvDefenseInfo(XPacket& p, ID idKey)
{
	ID idAcc;
	XGAME::xBattleLog log;
	int verBattleLog;
	p >> idAcc;
	p >> verBattleLog;
	p.SetverArchiveInstant(verBattleLog);
	p >> log;
	XArchive arParam;
	arParam << idAcc;
	arParam << (int)VER_BATTLE_LOG;
	arParam << log;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbDefenseInfo,
		arParam);
}

void XGameSvrConnection::cbDefenseInfo(XDatabase *pDB, XArchive& arParam)
{
	ID idAcc;
	XArchive arResult;
	XGAME::xBattleLog logNew;
	int ver;

	arParam >> idAcc;
	arParam >> ver;
	arParam.SetverArchiveInstant(ver);
	arParam >> logNew;
	// idAcc유저의 자원과 월드정보를 읽어온다.
	if (XBREAK(idAcc == 0))
		return;
	// 방어자의 방어로그를 읽어온다.
	BOOL bFound = pDB->GetpDB()->LoadDefenseLogByIdAcc(idAcc, arResult);
	if (XBREAK(bFound == FALSE))
		return;
	//XAccount acc;
	auto spAcc = XSPAcc( new XAccount() );
	XArchive arDefenseLog;
	XArchive arEncounter;
	arResult >> arDefenseLog;
	arResult >> arEncounter;
	spAcc->DeSerializeDefenseLog( arDefenseLog );
	// 방어기록을 방어로그에 추가한다.
	spAcc->AddBattleLog(false, logNew);
	// 다시 저장한다. 
	arResult.Clear();
	arDefenseLog.Clear();
	BYTE numAttacked = spAcc->GetnumAttaccked();
	if (numAttacked >= 255)
		numAttacked = 0;
	spAcc->SetnumAttaccked(numAttacked + 1);
	spAcc->SerializeDefenseLog( arDefenseLog );
	arResult << arDefenseLog;
	arResult << arEncounter;
	pDB->GetpDB()->SaveDefenseLogByIdAcc(idAcc, arResult);
}

/**
유황스팟에서 인카운터(난입자)가 이겨서 유황을 가로채게 됨.
*/
void XGameSvrConnection::RecvEncounterWin(XPacket& p, ID idKey)
{
	ID idEncounter, idPlayer;
	ID idSpot;
	int numSulfur;
	_tstring strEncounter;
	p >> idPlayer;
	p >> idEncounter;
	p >> numSulfur;
	p >> idSpot;
	p >> strEncounter;
	XArchive arParam;
	arParam << idPlayer;
	arParam << idEncounter;
	arParam << numSulfur;
	arParam << idSpot;
	xSec secCurr = XTimer2::sGetTime();
	arParam << secCurr;
	arParam << strEncounter;
	DBMNG->DoAsyncCall( GetThis(),
										&XGameSvrConnection::cbEncounterWin,
										arParam);

}

void XGameSvrConnection::cbEncounterWin(XDatabase *pDB, XArchive& arParam)
{
	ID idPlayer, idEncounter, idSpot;
	ID idConnectServer, idBattleTarget;
	DWORD secBattle;
	int numSulfur;
	xSec secCurr;
	_tstring strEncounter;
	XArchive arOut;
	arParam >> idPlayer;
	arParam >> idEncounter;
	arParam >> numSulfur;
	arParam >> idSpot;
	arParam >> secCurr;
	arParam >> strEncounter;
	// 온라인 상태면 저장안하고 오프라인상태일때만 저장한다.
	BOOL bFound = pDB->GetpDB()->SelectTargetbattlestatus(idEncounter
																											, idConnectServer
																											, idBattleTarget
																											, secBattle );
	if( bFound ) {
		if( idConnectServer > 0 ) {
			// idEncounter가 온라인상태
			// 아무것도 저장하지 않고 그냥 나감.
		} else {
			// idEncounter가 오프라인상태
			// 상대 유저에게 락을 건다.
			BOOL bFound = pDB->GetpDB()->AccountUpdateBattleInfo( idEncounter
																													, idPlayer
																													, secCurr );
			if( bFound ) {
				// 디펜스로그를 읽어온다.
				XArchive arResult;
				BOOL bFound = pDB->GetpDB()->LoadDefenseLogByIdAcc( idEncounter, arResult );
				if( bFound ) {
					XArchive arDefenseLog;
					XArchive arEncounter;
					arResult >> arDefenseLog;
					arResult >> arEncounter;
					// 인카운터 정보를 쓴다.
					//XAccount acc;
					auto spAcc = XSPAcc( new XAccount() );
#ifdef _DEBUG
					// 검증용
					arDefenseLog.CurrMoveToStart();
// 					acc.DeSerializeDefenseLog( arDefenseLog );
// 					arDefenseLog.Clear();
// 					acc.SerializeDefenseLog( arDefenseLog );
#endif // _DEBUG
					spAcc->DeSerializeEncounter( arEncounter );
					XGAME::xEncounter enc;
					enc.m_idSpot = idSpot;
					enc.m_idDefender = idPlayer;
					enc.m_numSulfur = numSulfur;
					enc.m_secEncount = secCurr;
					enc.m_strDefender = strEncounter;
					if( spAcc->AddEncounter( enc ) ) {
						// 디펜스로그를 다시 저장한다.
						arEncounter.Clear();
						spAcc->SerializeEncounter( arEncounter );
						arResult.Clear();
						arResult << arDefenseLog;
						arResult << arEncounter;
						BOOL bOk = pDB->GetpDB()->SaveDefenseLogByIdAcc( idEncounter, arResult );
#ifdef _DEBUG
						XBREAK( !bOk );
#endif // _DEBUG
					}
					// 락을 푼다.
					BOOL bOk = pDB->GetpDB()->AccountUpdateBattleInfo( idEncounter, 0, 0 );
//					acc.Release();
				}
			}

		}
	}
	// 인카운터 정보를 쓴다.
	// 디펜스로그를 다시 저장한다.
	// 대상이 온라인일때가 고려안되어서 일단 삭제
	// 	// DB에서 해당 사용자의 월드데이타를 읽어온다.
	// 	pDB->GetpDB()->LoadResourceWorldByIdAcc( idAcc, arOut );
	// 	// 월드바이너르를 메모리에 푼다.
	// 	XArchiveResourceWorld arObj( arOut );
	// 	XWorld *pWorld = arObj.GetpWorld();
	// 	// 해당 스팟을 찾는다.
	// 	XSpotSulfur *pSulfur = pWorld->GetSpot<XSpotSulfur*>( XGAME::xSPOT_SULFUR, idSpot );
	// 	if( pSulfur == NULL )		// 이긴유저가 아직 유황스팟을 못가졌을수도 있다.
	// 		return;
	// 	// 해당스팟에 뺏은 유황을 쌓는다.
	// 	pSulfur->DoStackToLocal( numSulfur, false );
	// 	// 업데이트된 정보를 다시 DB에 써넣음.
	// 	{
	// 		XArchive ar;
	// 		arObj.Serialize( ar );
	// 		pDB->GetpDB()->SaveResourceWorldByIdAcc( idAcc, ar );
	// 	}
}

/**
만드레이크 자원지 매칭 요청.
scoreMin~scoreMax 사이의 유저중 리그참가자가 비어있는 유저를 찾아 돌려준다.
*/
void XGameSvrConnection::RecvReqMatchMandrake(XPacket& p, ID idKey)
{
	XArchive arParam;
	arParam.MoveFromArchive(p);	// p의 현재위치에서 -> arParam로 아카이브를 옮겨옴.
	arParam << idKey;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbReqMatchMandrake,
		arParam);
}

void XGameSvrConnection::cbReqMatchMandrake(XDatabase *pDB, XArchive& arParam)
{
	ID idFinder, snSpot, idKey;
	int powerMin, powerMax, idxMandrake;
	arParam >> idFinder;
	arParam >> idxMandrake;
	arParam >> powerMin;
	arParam >> powerMax;
	arParam >> snSpot;
	arParam >> idKey;
	XArchive arOut;
	/*
	min~max사이의 유저중 idxMandrake자원지가 열려있고 그 자원지의 idEnemy가 0인 유저를 찾아 리턴한다.
	또한 그 유저의 해당 idEnemy에는 idFinder로 채워넣는다.
	*/
	BOOL bFound =
		pDB->GetpDB()->FindMatchMandrake(idFinder, idxMandrake, powerMin, powerMax, arOut);
	if (bFound)	{
		XPacket ar((ID)xGDBPK_REQ_MATCH_MANDRAKE, idKey);
		ar << -1;
		ar << idxMandrake;
		ar << snSpot;
		ar << arOut;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}	else {
		// 못찾았을때는 idAcc에 0을 넣어 보낸다.
		XPacket ar((ID)xGDBPK_REQ_MATCH_MANDRAKE, idKey);
		ar << -1;
		ar << idxMandrake;
		ar << snSpot;
		arOut.Clear();
		arOut << 0;		// idAcc;
		ar << arOut;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}

}

/**
idAccount의 유저를 DB에서 찾아 만드레이크 스팟 군단정보를 보낸다.
*/
void XGameSvrConnection::RecvGetMandrakeLegionByIdAcc(XPacket& p, ID idKey)
{
	XArchive arParam;
	arParam << idKey;
	arParam.MoveFromArchive(p);
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbFindMandrakeInfoByIdAcc,
		arParam);

}

void XGameSvrConnection::cbFindMandrakeInfoByIdAcc(XDatabase *pDB, XArchive& arParam)
{
	ID idKey, idOwner, snSpot, idPacket;
	int idxMandrake;
	arParam >> idKey;
	arParam >> idxMandrake;
	arParam >> snSpot;
	arParam >> idOwner;
	arParam >> idPacket;
	XArchive arOut;
	BOOL bFound
		= pDB->GetpDB()->FindMandrakeInfoByIdAcc(idOwner, idxMandrake, arOut);
	//
	if (bFound)
	{
		XPacket ar((ID)xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC, idKey);
		ar << -1;
		ar << idxMandrake;
		ar << snSpot;
		ar << idOwner;
		ar << idPacket;
		ar << arOut;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
	else
	{
		// error
		XPacket ar((ID)xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC, idKey);
		ar << -1;
		ar << idxMandrake;
		ar << snSpot;
		ar << 0;		// idOwner
		ar << idPacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

/**
idAccount가 갖고 있던 idxMandrake의 소유정보를 바꾼다.
*/
void XGameSvrConnection::RecvChangeMandrakeOwn(XPacket& p, ID idKey)
{
	XArchive arParam;
	arParam.MoveFromArchive(p);
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbChangeOwnerMandrake,
		arParam);
}

void XGameSvrConnection::cbChangeOwnerMandrake(XDatabase *pDB, XArchive& arParam)
{
	int idxMandrake, win, reward, idxLegion, addOffWin;
	ID idAcc, idNewOwner, idEnemy;
	_tstring strName;

	arParam >> idxMandrake;
	arParam >> idAcc;			// 방어자
	arParam >> idNewOwner;		// 0이면 방어실패 idAcc와 같으면 방어성공
	arParam >> idEnemy;
	arParam >> strName;
	arParam >> win;
	arParam >> reward;
	arParam >> idxLegion;
	arParam >> addOffWin;
	// 월드데이타 읽어서 스팟 정산.
	if (idNewOwner == 0) {
		// 방어실패시 방어자의 스팟정보 읽어서 갱신
		XArchive arResult;	// DB에서 읽어온 데이타가 담길 아카이브
		pDB->GetpDB()->LoadResourceWorldByIdAcc(idAcc, arResult);
		XArchiveResourceWorld arObj(arResult);
		XWorld *pWorld = arObj.GetpWorld();
		// 현재 시점으로 정산.
		pWorld->OnAfterDeSerialize(NULL, idAcc);
		// 해당스팟의 지역창고자원 30% 손실시킴
		auto pSpot = pWorld->GetSpotMandrakeByIdx(idxMandrake);
		if (pSpot) {		// 프로퍼티에서 스팟이 삭제되면 널일수 있음.
			// 방어 실패시에는 방어자의 스팟정보를 클리어
			pSpot->LossLocalStorage( 0.3f, nullptr );
			pSpot->Initialize(nullptr);
//			pSpot->SetReward( reward );		// 클리어후에 다시 보상값을 써넣어 상대가 수동으로 보상을 갖도록함.
			pSpot->DoStackToLocal((float)reward, false);	// 보상은 지역창고에 쌓는다.(유황의 오프라인보상과 일관성을 유지하기 위해 그냥 지역창고에 쌓음)
		}
		// 다시 원래주인의 DB에 써넣음.
		// ChangeOwnerMandrakeMine로 아래에서 저장하므로 이부분은 필요없을듯 하다
		XArchive ar;
		arObj.Serialize(ar);
		pDB->GetpDB()->SaveResourceWorldByIdAcc(idAcc, ar);
	}

	// 스팟정보에 idOwner를 0으로 써넣어 ?(초기화) 상태로 만든다.
	BOOL bFound 
		= pDB->GetpDB()->ChangeOwnerMandrakeMine(idAcc,
																					idxMandrake,
																					idNewOwner,	// idOwner
																					idEnemy,	// idEnemy
																					strName,
																					win,	// win
																					reward,
																					idxLegion,
																					addOffWin);	// 오프라인동안의 승수더하기

}
/**
@brief 요청한 계정의 로그인에 락요청
*/
void XGameSvrConnection::RecvLockLoginForBattle(XPacket& p, ID idKey)
{
	ID idAttacker;
	ID idDefender;
	XArchive arPack;
	p >> idAttacker >> idDefender;
	p >> arPack;
	// idDefender의 DB에 LoginLockForBattle을 건다.
	// 락건 시간을 DB에 기록한다.
	// 만약 idDefender가 이미 로그인했으면 게임서버로 실패 패킷 보낸다.

	DBMNG->DoAsyncCall( GetThis(),
										&XGameSvrConnection::cbTargetAttack,
										idAttacker,
										idDefender,
										arPack,
										idKey);
}

void XGameSvrConnection::cbTargetAttack(XDatabase *pDB, ID idAttacker, ID idDefender, XArchive& arPack, ID idKey)
{
	ID battletargetid = 0;
	ID battletime = 0;
	ID connectserverid = 0;
	BOOL bFound = pDB->GetpDB()->SelectTargetbattlestatus(idDefender
																											, connectserverid
																											, battletargetid
																											, battletime);
	if (bFound) {
		if (connectserverid > 0) {
			XPacket ar((ID)xGDBPK_LOGIN_LOCK_FOR_BATTLE, idKey);
			ar << 1;
			ar << idAttacker;
			ar << idDefender;
			ar << 0;      // online
			ar << arPack;
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}	else {
			xSec secCurr = XTimer2::sGetTime();
			if (battletargetid > 0) {
				auto secMaxBattleSession = XGC->m_secMaxBattleSession;
				if (secCurr > (battletime + secMaxBattleSession)) {  //전투 종료 된거니까 Update 하고 다시...
					DBMNG->DoAsyncCall( GetThis(),
												&XGameSvrConnection::cbLockLoginForBattle,
												idKey,
												idAttacker,
												idDefender,
												secCurr,
												arPack);
				} else {//전투 중인 유저도 그냥 온라인이라고 표시 해준다.
					XPacket ar((ID)xGDBPK_LOGIN_LOCK_FOR_BATTLE, idKey);
					ar << 1;
					ar << idAttacker;
					ar << idDefender;
					ar << 0;      // online 
					ar << arPack;
					DBMNG->RegisterSendPacket( GetThis(), ar);
				}
			} else {
				DBMNG->DoAsyncCall( GetThis(),
													&XGameSvrConnection::cbLockLoginForBattle,
													idKey,
													idAttacker,
													idDefender,
													secCurr,
													arPack);
			}
		}
	}
}

void XGameSvrConnection::cbLockLoginForBattle(XDatabase *pDB,
																							ID idKey,
																							ID idAttacker,
																							ID idDefender,
																							DWORD secStart,
																							XArchive& arPack)
{
	// idDefender의DB에저장( secStart )
	// 락이 성공하면 게임서버로 다시 돌려준다.
	BOOL bFound = pDB->GetpDB()->AccountUpdateBattleInfo(idDefender
																										, idAttacker
																										, secStart);
	if (bFound)	{
		XPacket ar((ID)xGDBPK_LOGIN_LOCK_FOR_BATTLE, idKey);
		ar << 1;          // 0만 아니면 됨.
		ar << idAttacker;
		ar << idDefender;
		ar << secStart;      // success lock
		ar << arPack;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

/**
@brief idAcc유저가 현재 온라인상태냐
*/
bool XGameSvrConnection::IsOnlineUser(ID idAcc)
{
	bool bOnline = false;
	return bOnline;
}
/**
@brief idAttacker가 idDefender에 대한 공격이 끝남.
*/
void XGameSvrConnection::RecvUnlockLoginForBattle(XPacket& p, ID idKey)
{
	ID idDefender;
	ID idAttacker;
	p >> idDefender;
	p >> idAttacker;
	// idDefender의 로그인락을 해제한다. 현재 idDefender가 접속상태라면 그 커넥션아이디를 받아온다.
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbUnlockLoginForBattle,
		idDefender, idAttacker);
}

void XGameSvrConnection::cbUnlockLoginForBattle(XDatabase *pDB,
	ID idDefender,
	ID idAttacker)
{
	//#error("DB작업")
	// idDefender의로그인락을해제();
	BOOL bFound = pDB->GetpDB()->AccountUpdateBattleInfo(idDefender, 0, 0);
	if (bFound)
	{
		XPacket ar((ID)xGDBPK_LOGIN_UNLOCK_FOR_BATTLE);
		ar << 0;    // 공격자의 연결여부와 관계없이 하기위해
		ar << idDefender;
		ar << idAttacker;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
	// 방어자가 접속을 시도중일수 있으니 방어자에게 알림.

}

static XGuild *s_pGuild = nullptr;   // 가라생성

/**
@brief 길드레이드 캠페인 정보 요청
*/
void XGameSvrConnection::RecvReqCampaignByGuildRaid(XPacket& p, ID idKey)
{
	ID idGuild, idCamp, idAcc, idSpot;
	XArchive arCamp;
	int bCamp;
	p >> idAcc >> idGuild >> idCamp >> idSpot;
	p >> bCamp;		// 캠페인 아카이브가 왔으면.
	if (bCamp) {
		XBREAK(bCamp != 11);
		p >> arCamp;
	}
	// 1.idCamp로 캠페인을 검색해본다.
	// 2.있으면 그 정보를 꺼내온다.
	// 3.없으면 arCamp를 신규로 생성한다.
	DBMNG->DoAsyncCall( GetThis(), &XGameSvrConnection::cbCampaignByGuildRaid,
		idAcc, idGuild, idCamp, idSpot, idKey, arCamp);
}

void XGameSvrConnection::cbCampaignByGuildRaid( XDatabase *pDB,
																								ID idAcc,
																								ID idGuild,
																								ID idCamp,
																								ID idSpot,
																								ID idKey,
																								XArchive& arCampDefault)
{
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
#pragma message("pGuild는 thread safe하지 않으므로 길드개별적으로 락걸수 있게 해야함.")
	CampObjPtr spCampObj;
	if (XASSERT(pGuild)) {
		//		XLockObj locker(pGuild, __TFUNC__);
		spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (spCampObj == nullptr) {
			// 캠페인이 없으면 디폴트값으로 생성
			pGuild->DeSerializeUpdateGuildRaidCamp(arCampDefault, idCamp);
			spCampObj = pGuild->FindspRaidCampaign(idCamp);
			// db에 신규생성
			if (XASSERT(spCampObj)) {
				XLockObj locker( std::static_pointer_cast<XLock>( spCampObj ), __TFUNC__);
				auto result = pDB->GetpDB()->CreateCampaignByGuildRaid(idAcc, idGuild, idCamp, arCampDefault);
				if (result == XGAME::xGE_ERROR_ALREADY_EXIST_RAID) {
					// db에 이미 있다고 나오면 db에걸 읽어서 씀.
					XArchive arCamp;
					bool bFound = pDB->GetpDB()->LoadCampaignByGuildRaid(idAcc, idGuild, idCamp, &arCamp);
					if (XASSERT(bFound)) {
						XCampObj::sDeserializeUpdate(spCampObj, arCamp);
					}
				}
			}
		}
	} // guild
	do
	{
		if (XBREAK(spCampObj == nullptr))
			break;
		XLockObj locker(spCampObj, __TFUNC__);
		XArchive arCamp;
		bool bUpdate = false;
		// 오픈상태인가
		if (spCampObj->IsOpenCampaign()) {
			// 지금 누가 전투중이다.
			if (spCampObj->IsTryingUser()) {
				// 먼저하던 유저의 전투시간이 다되었는가.
				if (spCampObj->GetsecRemainTry() == 0) {
					// 강제로 닫음.
					auto err = XGAME::xGE_ERROR_UNKNOWN;
					spCampObj->DoFinishBattle();
					bUpdate = true;
					bool bResult
						= pDB->GetpDB()->LeaveGuildRaid(spCampObj->GetidAccTrying(),
						idGuild,
						idCamp,
						&err);
					if (XASSERT(bResult)) {
						XASSERT(err == XGAME::xGE_SUCCESS);
						XBREAK(err == XGAME::xGE_ERROR_IS_NOT_LOCKER);
					}
				}
			}
			else {
				// 아무도 전투중이지 않다.
				// 레이드 시간이 다되었는가
				if (spCampObj->GetsecRemainClose() == 0) {
					// 초기화상태로 전환
					spCampObj->DoCloseCampaign();	// 레이드닫음.
					bUpdate = true;
				}
			}
		}
		// 현재 상태를 묶음.
		XCampObj::sSerialize(spCampObj, arCamp);
		// 변경된내용을 db에 업데이트.
		if (bUpdate)
			pDB->GetpDB()->UpdateCampaignByGuildRaid(idAcc, idGuild, idCamp, arCamp);
		// 보내는 순서까지 컨트롤하기위해 lock블럭에 넣음.
		// 모든 게임서버로 업데이트
		{
			XPacket ar((ID)xGDBPK_GUILD_UPDATE_CAMPAIGN);
			ar << xNO_USER;
			ar << idGuild;
			ar << idCamp;
			ar << arCamp;
			// 게임서버용 소켓에 물린 모든 게임서버 커넥션들에게 전송
			MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>
				(&XGameSvrConnection::Send, ar);
		}
		// 
		{
			XPacket ar((ID)xGDBPK_GUILD_REQ_CAMPAIGN_RAID, idKey);
			ar << (int)xTO_USER;
			ar << idCamp;
			ar << idSpot;
			ar << 1;   // 찾음.
			ar << arCamp;
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	} while (0);

	//
	// 	XArchive arCamp;
	//  	bool bFound 
	//  		= pDB->GetpDB()->LoadCampaignByGuildRaid( idAcc, idGuild, idCamp, &arCamp );
	// 	if( !bFound ) {
	// 		// DB에 없다.
	// 		// 디폴트 캠페인정보로 생성한다. 만약 멀티스레드로 인해 그사이에 누군가가 이미 생성했다면 그 정보를 arCamp에 받아온다.
	// 		bFound = true;
	// 		auto result = pDB->GetpDB()->CreateCampaignByGuildRaid( idAcc, idGuild, idCamp, arCampDefault );
	// 		if( result == XGAME::xGE_ERROR_ALREADY_EXIST_RAID )
	// 			bFound = pDB->GetpDB()->LoadCampaignByGuildRaid( idAcc, idGuild, idCamp, &arCamp );
	// 		else {
	// 			XBREAK( result != XGAME::xGE_SUCCESS );		// db에러상황
	// 		}
	// 		// arCampDefault, &arCamp 정보가 다를 수 있음.(사용자가 있어서 내용을 바꾼 경우.. 바뀐놈이 arCamp 에 들어감..)
	// 	}
	// 	// lazy loading. 로딩했는데 메모리상엔 없을때 메모리에 넣어줌.
	// 	if( XASSERT(bFound) ) {
	// 	}
}

void XGameSvrConnection::RecvReqGuildRaidOpen(XPacket& p, ID idKey)
{
	ID idGuild, idCamp, idAcc, idSpot;
	XArchive arCampLegion;
	p >> idAcc >> idGuild >> idCamp >> idSpot;
	p >> arCampLegion;

	// 	만약 여기서 spCamp에 업데이트 시키고 save명령만 보냈다면.
	// 	다른스레드에서 에디팅이 있다고 해도 campLock으로 메모리상엔 최신버전으로 보호되고
	// 	세이브명령은 어느쪽이 먼저 실행되든 똑같은 값을 저장하게 된다.
	DBMNG->DoAsyncCall( GetThis(), &XGameSvrConnection::cbGuildRaidOpen,
		idAcc, idGuild, idCamp, idSpot, arCampLegion, idKey);
}

void XGameSvrConnection::cbGuildRaidOpen(XDatabase *pDB,
	ID idAcc, ID idGuild, ID idCamp,
	ID idSpot, XArchive& arCampLegion,
	ID idKey)
{
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		auto spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (XASSERT(spCampObj != nullptr)) {
			if (XASSERT(spCampObj->IsCloseCampaign())) {
				XLockObj lo(spCampObj.get(), __TFUNC__);
				spCampObj->DoOpenCampaign(0);
				XCampObj::sDeSerializeLegion(spCampObj, arCampLegion);
				// 바뀐정보를 db에 업데이트 한다.
				XArchive arCamp;
				XCampObj::sSerialize(spCampObj, arCamp);
				pDB->GetpDB()->UpdateCampaignByGuildRaid(idAcc, idGuild, idCamp, arCamp);
				// 모든 게임서버로 업데이트
				{
					XPacket ar((ID)xGDBPK_GUILD_UPDATE_CAMPAIGN);
					ar << xNO_USER;
					ar << idGuild;
					ar << idCamp;
					ar << arCamp;
					// 게임서버용 소켓에 물린 모든 게임서버 커넥션들에게 전송
					MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>
						(&XGameSvrConnection::Send, ar);
				}
				// 
				{
					XPacket ar((ID)xGDBPK_GUILD_OPEN_RAID, idKey);
					ar << (int)xTO_USER;
					ar << idCamp;
					ar << idSpot;
					ar << 1;
					ar << arCamp;
					DBMNG->RegisterSendPacket( GetThis(), ar);
				}
			}
		}
	}
}
/**
@brief 길드레이드 진입요청
*/
void XGameSvrConnection::RecvReqGuildEnterRaid(XPacket& p, ID idKey)
{
	ID idGuild, idCamp, idAcc, idSpot;
	XArchive arParam;
	p >> idAcc >> idGuild >> idCamp >> idSpot;
	p >> arParam;
	DBMNG->DoAsyncCall( GetThis(), &XGameSvrConnection::cbEnterGuildRaid,
		idAcc, idGuild, idCamp, idSpot, arParam, idKey);
}

void XGameSvrConnection::cbEnterGuildRaid(XDatabase *pDB,
	ID idAcc, ID idGuild,
	ID idCamp, ID idSpot,
	XArchive& arParam,
	ID idKey)
{
	_tstring strName;
	int level, power;
	arParam >> strName;
	arParam >> level >> power;
	auto err = XGAME::xGE_ERROR_UNKNOWN;
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		auto spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (XASSERT(spCampObj != nullptr)) {
			XLockObj locker(spCampObj, __TFUNC__);
			int idxStage = spCampObj->GetidxLastUnlock();
			do {
				// 캠페인이 이미 다 클리어되어있으면 취소시킴.
				if (spCampObj->IsClearCampaign()) {
					err = XGAME::xGE_ERROR_ALREADY_CLEAR_RAID;
					break;
				}
				if (spCampObj->GetsecRemainClose() == 0) {
					err = XGAME::xGE_ERROR_CLOSED_RAID;
					break;
				}
				ID idAccExist = 0;
				bool bResult = pDB->GetpDB()->EnterGuildRaid(idAcc, idGuild, idCamp, &err, &idAccExist);
				XBREAK(bResult == false);
				if (bResult) {
					if (err == XGAME::xGE_ERROR_STILL_TRYING_RAID) {
						// 이미 누가 하고있다.
						if (spCampObj->GetsecRemainTry() == 0) {
							XASSERT(idAcc != idAccExist);
							// 전투시작한지 3분이 넘었다. 강제로 해제시킴
							pDB->GetpDB()->LeaveGuildRaid(idAccExist, idGuild, idCamp, &err);
						}
					}
					if (err == XGAME::xGE_SUCCESS) {
						auto spStageObj = spCampObj->GetspStageLastUnlock();
						if (XASSERT(spStageObj)) {
							spStageObj->AddTryer(idAcc, strName, level, power);
						}
						// 스테이지 도전 시작
						spCampObj->DoStartBattle(idAcc, strName);
						spCampObj->DoEnterStage(idxStage);
					}
				}
			} while (0);
			XArchive arUpdate;
			XCampObj::sSerialize(spCampObj, arUpdate);
			if (err == XGAME::xGE_SUCCESS) {
				// 값이 바꼈을때만 db에 업데이트
				pDB->GetpDB()->UpdateCampaignByGuildRaid(idAcc, idGuild, idCamp, arUpdate);
			}
			//
			XPacket ar((ID)xGDBPK_GUILD_REQ_ENTER_RAID, idKey);
			ar << (int)xTO_USER;
			ar << idCamp;
			ar << idxStage;
			ar << idSpot;
			ar << (int)err;   // 결과
			ar << arUpdate;	// 최신 캠페인 정보
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}

}

/**
@brief 길드포인트(point)를 메일로 첨부해 모든 길드원에게 보낸다.
*/
void XGameSvrConnection::SendGuildPointByMail(ID idGuild, int point)
{
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		//XLockObj locker(pGuild, __TFUNC__);
		auto& listMember = pGuild->GetListMember();
		XArchive ar;
		ar << _T("길드 관리자");
		ar << _T("길드원");
		ar << _T("길드 레이드 보상");
		ar << _T("길드레이드를 성공적으로 완수하였으므로 포인트를 지급함.");
		ar << 1;
		XPostItem postItem( XGAME::xPOSTRES_GUILD_COIN, point );
		postItem.Serialize( ar );
		//
		for( auto pMember : listMember ) {
			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbPostAdd,
				pMember->s_idaccount,
				XE::GenerateID(),	// 임시
				XGAME::xPOSTTYPE_NORMAL,
				0,
				0,
				ar );
			ar.CurrMoveToStart();
		}
	}
}
/**
@brief 길드레이드 스테이지에서 빠져나옴
*/
void XGameSvrConnection::RecvReqGuildLeaveRaid(XPacket& p, ID idKey)
{
	ID idGuild, idCamp, idAcc, idSpot;
	BYTE b0;
	XArchive arLegionForRaid;
	p >> idAcc >> idGuild >> idCamp >> idSpot;
	p >> b0;	auto typeWin = (XGAME::xtWin)b0;
	p >> b0;	int lvAcc = b0;
	p >> b0 >> b0;
	p >> arLegionForRaid;
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		auto spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (XASSERT(spCampObj)) {
			XLockObj locker(spCampObj, __TFUNC__);
			spCampObj->DoFinishBattle();
			if (typeWin == XGAME::xWT_WIN) {
				//				spCampObj->ClearStage( spCampObj->GetidxLastUnlock() );
				spCampObj->ClearStage();
				if (spCampObj->IsClearCampaign()) {
					if (spCampObj->GetbSendReward() == false) {
						// 보상 메일 발송
						// 받게될 보상포인트.
						int point = spCampObj->GetRewardPointByClear(lvAcc);
						SendGuildPointByMail(idGuild, point);
						spCampObj->SetbSendReward(true);
					}
				}
			}
			else
				if (typeWin == XGAME::xWT_LOSE) {
				// 패배했을때만 군단정보 갱신하면 되므로
				auto spLegion = spCampObj->GetspLegionLastUnlockStage();
				if (XASSERT(spLegion)) {
					// 사망여부를 갱신한다.
					spLegion->DeserializeForGuildRaid(arLegionForRaid);
				}
				}
				else
					if (typeWin == XGAME::xWT_NONE) {
				// 전투캔슬
					}
					else {
						XBREAK(1);
					}
					DBMNG->DoAsyncCall( GetThis(), &XGameSvrConnection::cbLeaveGuildRaid,
						idAcc, idGuild, idCamp, idSpot, typeWin);
					// 모든 게임서버로 업데이트
					{
						XArchive arCamp;
						XCampObj::sSerialize(spCampObj, arCamp);
						//
						XPacket ar((ID)xGDBPK_GUILD_UPDATE_CAMPAIGN);
						ar << xNO_USER;
						ar << idGuild;
						ar << idCamp;
						ar << arCamp;
						// 게임서버용 소켓에 물린 모든 게임서버 커넥션들에게 전송
						MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>
							(&XGameSvrConnection::Send, ar);
					}
		}
	}
}

void XGameSvrConnection::cbLeaveGuildRaid(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, XGAME::xtWin typeWin)
{
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		auto spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (XASSERT(spCampObj != nullptr)) {
			XLockObj locker(spCampObj, __TFUNC__);
			auto err = XGAME::xGE_ERROR_UNKNOWN;
			bool bResult = pDB->GetpDB()->LeaveGuildRaid(idAcc, idGuild, idCamp, &err);
			if (XASSERT(bResult)) {
				if (XASSERT(err == XGAME::xGE_SUCCESS)) {
				}
				else
					if (XBREAK(err == XGAME::xGE_ERROR_IS_NOT_LOCKER)) {
					// 애초에 진입한거부터 잘못된거.
					}
			}
			// db에 캠페인데이타 업데이트.
			XArchive arCamp;
			XCampObj::sSerialize(spCampObj, arCamp);
			pDB->GetpDB()->UpdateCampaignByGuildRaid(idAcc, idGuild, idCamp, arCamp);
		}
	}
	// ㄴㅇㄹ	auto err = XGAME::xGE_ERROR_UNKNOWN;
	// 	bool bResult = pDB->GetpDB()->LeaveGuildRaid( idAcc, idGuild, idCamp, &err );
	// 	if( XASSERT( bResult ) ) {
	// 		if( XASSERT(err == XGAME::xGE_SUCCESS) ) {
	// 			auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	// 			if( XASSERT( pGuild ) ) {
	// 				auto spCampObj = pGuild->FindspRaidCampaign( idCamp );
	// 				if( XASSERT( spCampObj != nullptr ) ) {
	// 					spCampObj->DoFinishBattle();
	// 					if( bWin )
	// 						spCampObj->ClearStage( spCampObj->GetidxLastUnlock() );
	// 					// 바뀐정보를 db에 갱신
	// 					XArchive arCamp;
	// 					XCampObj::sSerialize( spCampObj, arCamp );
	// 					pDB->GetpDB()->UpdateCampaignByGuildRaid( idAcc, idGuild, idCamp, arCamp );
	// 					// 모든 게임서버로 업데이트
	// 					{
	// 						XPacket ar( (ID)xGDBPK_GUILD_UPDATE_CAMPAIGN );
	// 						ar << xNO_USER;
	// 						ar << idGuild;
	// 						ar << idCamp;
	// 						ar << arCamp;
	// 						// 게임서버용 소켓에 물린 모든 게임서버 커넥션들에게 전송
	// 						MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>
	// 							( &XGameSvrConnection::Send, ar );
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}

}

/**
@brief idCamp캠페인을 db에 업데이트 한다.
*/
void XGameSvrConnection::cbUpdateGuildRaidCampaign(XDatabase *pDB,
	ID idAcc,
	ID idGuild,
	ID idCamp,
	ID idSpot,
	ID idKey)
{
	auto pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
	if (XASSERT(pGuild)) {
		auto spCampObj = pGuild->FindspRaidCampaign(idCamp);
		if (XASSERT(spCampObj)) {
			XLockObj lo(spCampObj, __TFUNC__);
			XArchive arCamp;
			XCampObj::sSerialize(spCampObj, arCamp);
			auto err = pDB->GetpDB()->UpdateCampaignByGuildRaid(idAcc, idGuild, idCamp, arCamp);
			if (XASSERT(err == XGAME::xGE_SUCCESS)) {
				// 모든 게임서버로 업데이트
				XPacket ar((ID)xGDBPK_GUILD_UPDATE_CAMPAIGN);
				ar << xNO_USER;
				ar << idGuild;
				ar << idCamp;
				ar << arCamp;
				// 게임서버용 소켓에 물린 모든 게임서버 커넥션들에게 전송
				MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>
					(&XGameSvrConnection::Send, ar);
			}
		}
	}
}



// xuzhu end


void XGameSvrConnection::RecvPostItemsRemove(XPacket& p, ID idKey)
{
	ID idaccount, idpost, cbidpacket;

	p >> idaccount;
	p >> idpost;
	p >> cbidpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPostDestroy,
		idaccount,
		idpost,
		idKey,
		cbidpacket);
}
void XGameSvrConnection::RecvPostInfoDestroy(XPacket& p, ID idKey)
{
	ID idaccount, idpost, cbidpacket;

	p >> idaccount;
	p >> idpost;
	p >> cbidpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPostDestroy,
		idaccount,
		idpost,
		idKey,
		cbidpacket);
}
void XGameSvrConnection::cbPostItemsRemove(XDatabase *pDB, ID idAccount, ID idpost, ID idKey, ID cbidpacket)
{
	BOOL bFound = pDB->RemovePostItems(idAccount, idpost);

	XPacket ar((ID)xGDBPK_POST_ITEMS_REMOVE, idKey);
	ar << 1;
	ar << idAccount;
	ar << idpost;
	ar << (int)bFound;
	ar << cbidpacket;
	DBMNG->RegisterSendPacket( GetThis(), ar);
}
void XGameSvrConnection::cbPostDestroy(XDatabase *pDB, ID idAccount, ID idpost, ID idKey, ID cbidpacket)
{
	BOOL bFound = pDB->DeletePostInfo(idAccount, idpost);

	XPacket ar((ID)xGDBPK_POST_DESTROY, idKey);
	ar << 1;
	ar << idAccount;
	ar << idpost;
	ar << (int)bFound;
	ar << cbidpacket;
	DBMNG->RegisterSendPacket( GetThis(), ar);
}

void XGameSvrConnection::RecvPostInfoAdd(XPacket& p, ID idKey)
{
	_tstring strSenderName;
	_tstring strRecvName;
	_tstring strTitle;
	_tstring strMessage;

	ID PostType = 0;
	ID cbidpacket = 0;
	int nCount = 0;
	ID postsn = 0;
	ID idaccount = 0;
	ID rewardtableid = 0;
	BOOL bSuccess = FALSE;

	p >> idaccount;
	p >> postsn;
	p >> PostType;

	XArchive tmp;

	if ((XGAME::xtPostType)PostType == XGAME::xtPostType::xPOSTTYPE_NORMAL)
	{
		p >> strSenderName;
		p >> strRecvName;
		p >> strTitle;
		p >> strMessage;
		p >> nCount;

		tmp << strSenderName;
		tmp << strRecvName;
		tmp << strTitle;
		tmp << strMessage;
		tmp << nCount;

		for (int n = 0; n < nCount; n++)
		{
			XPostItem Item;
			Item.DeSerialize(p);
			Item.Serialize(tmp);
		}
		p >> cbidpacket;
		//		p >> idKey;
	}
	//	else if ((XGAME::xtPostType)PostType == XGAME::xtPostType::xPOSTTYPE_VECTOR)
	//	{
	//		p >> nCount;
	//
	//		tmp << nCount;
	//
	//		for (int n = 0; n < nCount; n++)
	//		{
	//			XPostItem Item;
	//			Item.DeSerialize(p);
	//			Item.Serialize(tmp);
	//		}
	//		p >> cbidpacket;
	////		p >> idKey;
	//
	//	}
	//	else if ((XGAME::xtPostType)PostType == XGAME::xtPostType::xPOSTTYPE_TABLE)
	//	{
	//		p >> rewardtableid;
	//		p >> cbidpacket;
	////		p >> idKey;
	//
	//		tmp << rewardtableid;
	//	}
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPostAdd,
		idaccount,
		postsn,
		PostType,
		idKey,
		cbidpacket,
		tmp);
}

void XGameSvrConnection::cbPostAdd(XDatabase *pDB, ID idaccount, ID postsn, ID PostType, ID idKey, ID cbidpacket, XArchive& tmp)
{
	BOOL bSuccess = FALSE;

	int nCount = 0;
	if (PostType == XGAME::xtPostType::xPOSTTYPE_NORMAL)
	{
		_tstring strSenderName;
		_tstring strRecvName;
		_tstring strTitle;
		_tstring strMessage;

		tmp >> strSenderName;
		tmp >> strRecvName;
		tmp >> strTitle;
		tmp >> strMessage;

		tmp >> nCount;

		XArchive itemlist1;
		XArchive itemlist2;

		for (int n = 0; n < nCount; n++)
		{
			XPostItem Item;
			Item.DeSerialize(tmp);
			Item.Serialize(itemlist1);
			Item.Serialize(itemlist2);
		}

		bSuccess = pDB->AddPostInfo(idaccount, postsn, PostType, strSenderName, strRecvName, strTitle, strMessage, nCount, itemlist1, 0);

		if (bSuccess && idKey )
		{
			XPacket ar((ID)xGDBPK_POST_ADD, idKey);
			ar << 1;
			ar << postsn;
			ar << PostType;
			ar << (int)bSuccess;
			ar << strSenderName;
			ar << strRecvName;
			ar << strTitle;
			ar << strMessage;
			ar << nCount;
			ar << cbidpacket;

			for (int n = 0; n < nCount; n++)
			{
				XPostItem Item;
				Item.DeSerialize(itemlist2);
				Item.Serialize(ar);
			}

			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}
	/*else if (PostType == XGAME::xtPostType::xPOSTTYPE_VECTOR)
	{
	tmp >> nCount;

	bSuccess = pDB->AddPostInfo(idaccount, postsn, PostType, NULL, NULL, NULL, NULL, nCount, tmp, 0);

	XPacket ar((ID)xGDBPK_POST_ADD, idKey);
	ar << idaccount;
	ar << postsn;
	ar << PostType;
	ar << (int)bSuccess;
	ar << nCount;
	ar << cbidpacket;
	ar << tmp;
	Send(ar);
	}
	else if (PostType == XGAME::xtPostType::xPOSTTYPE_TABLE)
	{
	ID rewardtableid = 0;
	tmp >> rewardtableid;
	bSuccess = pDB->AddPostInfo(idaccount, postsn, PostType, NULL, NULL, NULL, NULL, 0, tmp, rewardtableid);

	XPacket ar((ID)xGDBPK_POST_ADD, idKey);
	ar << idaccount;
	ar << postsn;
	ar << PostType;
	ar << (int)bSuccess;
	ar << rewardtableid;
	ar << cbidpacket;
	Send(ar);
	}*/

}

//void XGameSvrConnection::cbPostAdd(XDatabase *pDB, ID idaccount, ID postsn, ID PostType, ID idKey, ID cbidpacket, _tstring strSenderName, _tstring strRecvName, _tstring strTitle, _tstring strMessage, int nCount, XArchive& tmp, ID rewardtableid)
//{
//	BOOL bSuccess = pDB->AddPostInfo(idaccount, postsn, strSenderName, strRecvName, strTitle, strMessage, nCount, tmp, 0);
//
//	if (PostType == XGAME::xtPostType::xtPostType_Normal)
//	{
//		XPacket ar((ID)xGDBPK_POST_ADD, idKey);
//		ar << (int)bSuccess;
//		ar << idaccount;
//		ar << postsn;
//		ar << strSenderName;
//		ar << strRecvName;
//		ar << strTitle;
//		ar << strMessage;
//		ar << nCount;
//		ar << tmp;
//		ar << cbidpacket;
//
//		Send(ar);
//	}
//	else if (PostType == XGAME::xtPostType::xtPostType_Vector)
//	{
//
//		XPacket ar((ID)xGDBPK_POST_ADD, idKey);
//		ar << (int)bSuccess;
//		ar << idaccount;
//		ar << postsn;
//		ar << nCount;
//		ar << tmp;
//		ar << cbidpacket;
//		Send(ar);
//	}
//	else if (PostType == XGAME::xtPostType::xtPostType_Table)
//	{
//		XPacket ar((ID)xGDBPK_POST_ADD, idKey);
//		ar << (int)bSuccess;
//		ar << idaccount;
//		ar << postsn;
//		ar << rewardtableid;
//		ar << cbidpacket;
//		Send(ar);
//	}
//
//}

void XGameSvrConnection::RecvAccountPostInfo(XPacket& p, ID idKey)
{
	ID idAcc, idConnect;
	p >> idAcc;
	p >> idConnect;

	XArchive arParam;
	arParam << idAcc;
	arParam << idConnect;
	arParam << idKey;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPostInfo,
		arParam);
}

void XGameSvrConnection::RecvUpdateCashInfo(XPacket& p, ID idKey)
{
	ID idAcc;
	int remaincash;
	p >> idAcc;
	p >> remaincash;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbCashinfoUpdate,
		idAcc,
		remaincash);
}

void XGameSvrConnection::cbCashinfoUpdate(XDatabase *pDB, ID idAccount, int remaincash)
{
	pDB->UpdateAccountCashInfo(idAccount, remaincash);
}

void XGameSvrConnection::cbPostInfo(XDatabase *pDB, XArchive& arParam)
{
	ID idAcc, idConnect, idKey;
	XArchive arResult;

	arParam >> idAcc;
	arParam >> idConnect;
	arParam >> idKey;

	if (XBREAK(idAcc == 0))
		return;
	BOOL bFound = pDB->LoadPostInfoByIdAcc(idAcc, arResult);

	XPacket ar((ID)xGDBPK_POST_INFO, idKey);

	if (bFound)
	{
		ar << idConnect;
		//ar << 0;
		ar << arResult;
	}
	else
	{
		ar << idConnect;
		//ar << 0;
		ar << -1;	//우편물 없음.		
	}
	DBMNG->RegisterSendPacket( GetThis(), ar);
}

void XGameSvrConnection::RecvAccountShopInfoUpdate(XPacket& p, ID idKey)
{
	ID idAcc = 0;
	XArchive arparam;
	int req = 0;

	p >> idAcc;
	p >> arparam;


// 	XSPDBUAcc spAccOut = std::make_shared<XDBUAccount>();
	auto spAccOut = XSPDBUAcc( new XDBUAccount );
	spAccOut->DeSerializeShopList(arparam);

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbRecvAccountShopInfoUpdate,
		spAccOut,
		idAcc,
		idKey);
}

void XGameSvrConnection::cbRecvAccountShopInfoUpdate(XDatabase *pDB,
	XSPDBUAcc spAccOut,
	ID idAccount,
	ID idKey)
{
	pDB->SaveAccountShopInfo(idAccount, spAccOut);
	//SAFE_RELEASE_REF(spAccOut);
}

void XGameSvrConnection::RecvAccountPostInfoUpdate(XPacket& p, ID idKey)
{
	ID idAcc, idConnect, idReqPacket;
	XArchive arParam;
	p >> idAcc;
	p >> idConnect;
	p >> arParam;
	p >> idReqPacket;
	int req;
// 	auto spAcc = std::make_shared<XDBUAccount>();
	auto spAcc = XSPDBUAcc( new XDBUAccount );
	//spAcc
	spAcc->DeSerializePostUpdate(p);
	p >> req;	// 11이면 세이브 결과를 다시 알려줘야함.
	p >> idAcc;
	p >> idConnect;
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbUpdatePost,
		spAcc,
		idAcc,
		idConnect,
		(req) ? TRUE : FALSE,
		idReqPacket,
		idKey);
}

void XGameSvrConnection::cbUpdatePost(XDatabase *pDB,
																			XSPDBUAcc spAcc,
																			ID idAccount,
																			ID idConnect,
																			BOOL bReq,
																			ID idReqPacket,
																			ID idKey)
{
	BOOL bUpdate = pDB->UpdatePostInfo(spAcc);
	//
	if (bReq)
	{
		ID idPacket = (idReqPacket != 0) ? idReqPacket : xGDBPK_POST_INFO_UPDATE;
		XPacket ar(idReqPacket, idKey);
		ar << 0;
		ar << bUpdate;
		ar << idAccount;
		ar << idConnect;
		//		spAccOut->Serialize( ar );
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
	//SAFE_RELEASE_REF(spAcc);

}


void XGameSvrConnection::RecvRankingInfo(XPacket& p, ID idKey)
{
	XAUTO_LOCK2( RANKING_DB );
	if (RANKING_DB->GetRankingSize() > 0) {
		if (RANKING_DB->UpdateRank()) {
			DBMNG->DoAsyncCall( GetThis(),
													&XGameSvrConnection::cbRankingInfo,
													idKey);
			TRACE("랭킹정보 있지만 업데이트 해서 알려줄꺼임 db에 요청");
		} else {
			XPacket ar((ID)xGDBPK_RANK_INFO, idKey);
			ar << 0;
			RANKING_DB->Serialize(ar);
			Send( ar );
// 				DBMNG->RegisterSendPacket( GetThis(), ar);
			TRACE("랭킹정보 아는걸로 게임 서버에 보내줌.");
		}
	} else {
		DBMNG->DoAsyncCall( GetThis(),
												&XGameSvrConnection::cbRankingInfo,
												idKey);
		TRACE("랭킹정보 없으니 db에 요청");
	}
}

void XGameSvrConnection::cbRankingInfo(XDatabase *pDB, ID idKey)
{
	XArchive arResult;
	BOOL bFound = pDB->LoadRankingList(arResult);
	if (bFound)	{
			RANKING_DB->Lock(__TFUNC__);
			RANKING_DB->CreateDeSerialize( arResult );
			//
			XPacket ar( (ID)xGDBPK_RANK_INFO, 0 );
			ar << 0;
			RANKING_DB->Serialize( ar );
			RANKING_DB->Unlock();
			DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

int XGameSvrConnection::RecvGuildInfo(XPacket& p, ID idKey)
{
	if (XGuildMgr::sGet()->GetGuildSize() > 0) {
		XPacket ar((ID)xGDBPK_GUILD_INFO, idKey);
		ar << 0;
		XGuildMgr::sGet()->SerializeGuildAll(ar);
		Send( ar );
	} else {
		DBMNG->DoAsyncCall( GetThis(),
												&XGameSvrConnection::cbGuildInfo,
												idKey);
	}

	return 1;
}
void XGameSvrConnection::cbGuildInfo(XDatabase *pDB, ID idKey)
{
	if (pDB) {
		XArchive arResult;
		BOOL bFound = pDB->LoadGuildInfo(arResult);
		if (bFound) {
			XGuildMgr::sGet()->DeSerializeGuildAll(arResult);

			XPacket ar((ID)xGDBPK_GUILD_INFO);
			ar << (ID)0;
			XGuildMgr::sGet()->SerializeGuildAll(ar);
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}
}

int XGameSvrConnection::RecvGuildCreate(XPacket& p, ID idKey)
{
	ID idaccount;
	ID cbidPacket;
	_tstring strCreateGuildName;
	_tstring strGuildContext;
	_tstring strCreateUserName;
	XArchive arParam;

	int nCount = 0;
	p >> idaccount;
	p >> strCreateUserName;
	p >> strCreateGuildName;
	p >> strGuildContext;
	p >> cbidPacket;
	p >> nCount;

	arParam << idaccount;
	arParam << strCreateUserName;
	arParam << strCreateGuildName;
	arParam << strGuildContext;
	arParam << nCount;
	if( nCount > 0 ) {
		for( int i = 0; i < nCount; i++ ) {
			ID dw1;
			p >> dw1;
			arParam << dw1;
		}
	}

	//if (GUILD_MGR)
	{
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( strCreateGuildName );
		if( pGuild ) {
			XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
			ar << 1;
			ar << (ID)XGAME::xtGuildError::xGE_ERROR_NAME_DUPLICATE;
			ar << cbidPacket;
			//			DBMNG->RegisterSendPacket( GetThis(), ar);
			Send( ar );
		} else {
			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildNameDuplicateCheck,
				idaccount,
				strCreateGuildName,
				arParam,
				idKey,
				cbidPacket );
		}
	}
	return 1;
}
void XGameSvrConnection::cbGuildNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring strCreateGuildName, XArchive& arParam, ID idKey, ID cbidpacket)
{
	if (pDB) {
		BOOL bSuccess = pDB->GuildNameDuplicateCheck(strCreateGuildName);
		if( bSuccess ) {
			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildCreate,
				arParam,
				idKey,
				cbidpacket );
		} else {
			XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
			ar << 1;
			ar << (ID)XGAME::xtGuildError::xGE_ERROR_NAME_DUPLICATE;
			ar << cbidpacket;
			DBMNG->RegisterSendPacket( GetThis(), ar );
		}
	}
}
void XGameSvrConnection::cbGuildCreate(XDatabase *pDB, XArchive& arParam, ID idKey, ID cbidpacket)
{
	ID GuildIndex = 0;
	ID idaccount;
	_tstring strCreateGuildName;
	_tstring strUserName;
	_tstring strGuildContext;

	arParam >> idaccount;
	arParam >> strUserName;
	arParam >> strCreateGuildName;
	arParam >> strGuildContext;

	//	if (GUILD_MGR == NULL)
	// 	{
	// 		XPacket ar((ID)xGDBPK_GUILD_CREATE, idKey);
	// 		ar << 1;
	// 		ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_CREATE;
	// 		ar << strCreateGuildName;
	// 		ar << strUserName;
	// 		ar << 0;
	// 		ar << cbidpacket;
	// 		DBMNG->RegisterSendPacket( GetThis(), ar);
	// 		return;
	// 	}
	XArchive arOut;
	BOOL bSuccess = pDB->LoadAccountGuildJoinReq(idaccount, arOut);
	if( bSuccess == FALSE ) {
		XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
		ar << 1;
		ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_CREATE;
		ar << strCreateGuildName;
		ar << strUserName;
		ar << 0;
		ar << cbidpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar );
		return;
	}
	int nCount = 0;
	ID dw1;
	arOut >> nCount;
	if( nCount > 0 ) {//가입 신청 했던 모든 길드의 정보에서 나를 제거한다.
		for( int n = 0; n < nCount; n++ ) {
			arParam >> dw1;
			if( dw1 > 0 ) {
				XGuild* preGuild = XGuildMgr::sGet()->FindGuild( dw1 );
				if( preGuild ) {
					XArchive arUpdate;
					preGuild->RemoveGuildJoinMember( strUserName );
					preGuild->SerializeGuildReqMemberlist( arUpdate );
					DBMNG->DoAsyncCall( GetThis(),
						&XGameSvrConnection::cbGuildUpdate,
						dw1,
						(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ,
						arUpdate, 0, 0, 0 );
				}
			}
		}
	}

	bSuccess = pDB->CreateNewGuild(strCreateGuildName, strGuildContext, strUserName, 100, idaccount, GuildIndex);
	if( bSuccess ) {
		XArchive arNew;
		bSuccess = pDB->UpdateAccountGuildJoinReq( idaccount, GuildIndex, (ID)XGAME::xtGuildGrade::xGGL_LEVEL5, arNew );
		if( bSuccess ) {
			XGuild* pGuild = new XGuild;
			pGuild->SetGuildIndex( GuildIndex );
			pGuild->SetstrMasterName( strUserName.c_str() );
			pGuild->SetstrName( strCreateGuildName.c_str() );
			pGuild->SetstrGuildContext( strGuildContext.c_str() );
			pGuild->AddGuildMember( idaccount, strUserName, 1, XGAME::xGGL_LEVEL5 );
			if( XGuildMgr::sGet()->AddGuild( pGuild ) ) {
				XArchive arParam;
				pGuild->SerializeGuildMemberlist( arParam );
				// 모든 GameServer 에 Send 해줘야 하겠군요.			
				XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
				ar << 1;
				ar << (ID)XGAME::xtGuildError::xGE_SUCCESS;
				ar << pGuild->GetstrName();
				ar << pGuild->GetstrGuildContext();
				ar << pGuild->GetstrMasterName();
				ar << GuildIndex;
				ar << cbidpacket;
				pGuild->SerializeGuildMemberlist( ar );
				DBMNG->RegisterSendPacket( GetThis(), ar );
				XArchive ar1;
				pGuild->SerializeGuildMemberlist( ar1 );
				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbGuildUpdate,
					pGuild->GetGuildIndex(),
					(ID)XGAME::xtGuildUpdate::xGU_UPDATE_CREATE,
					ar1,
					idaccount,
					idKey,
					cbidpacket );
			} else {
				XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
				ar << 1;
				ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_CREATE;
				ar << strCreateGuildName;
				ar << strUserName;
				ar << 0;
				ar << cbidpacket;
				DBMNG->RegisterSendPacket( GetThis(), ar );
			}
		}
	} else {
		XPacket ar( (ID)xGDBPK_GUILD_CREATE, idKey );
		ar << 1;
		ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_CREATE;
		ar << strCreateGuildName;
		ar << strUserName;
		ar << 0;
		ar << cbidpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar );
	}
}
void XGameSvrConnection::cbAccountJoinProcessGuildInfo(XDatabase *pDB, ID acceptidaccount, ID targetidaccount, ID GuildIndex, ID guildacceptflag, ID idKey, ID idPacket)
{
	XArchive arJoinReqGuild;
	ID alreadyGuildindex = 0;
	int alreadyguildGrade = 0;
	BOOL bSuccess = pDB->AccountLoadGuildInfo(targetidaccount, alreadyGuildindex, alreadyguildGrade, arJoinReqGuild);
	if( bSuccess ) {
		XArchive arParam;
		//if (GUILD_MGR)	// 길드매니저가 없을수는 없으므로 이 처리는 하지말것.
		{
			XGuild* pGuild = XGuildMgr::sGet()->FindGuild( GuildIndex );
			if( pGuild ) {
				if( alreadyGuildindex != 0 ) {//이미 가입된 길드가 있음.
					// 대기열에서 삭제 해주고..
					if( pGuild->RemoveGuildJoinMember( targetidaccount ) ) {
						pGuild->SerializeGuildReqMemberlist( arParam );

						DBMNG->DoAsyncCall( GetThis(),
							&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
							GuildIndex,
							(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ,
							arParam,
							0,
							0,
							0 );

						XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
						ar << 1;
						ar << acceptidaccount;
						ar << GuildIndex;
						ar << guildacceptflag;
						ar << (ID)XGAME::xtGuildError::xGE_ERROR_ALREADY_HAVE_GUILD;
						ar << idPacket;
						DBMNG->RegisterSendPacket( GetThis(), ar );
						return;
					} else {
						XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
						ar << 1;
						ar << acceptidaccount;
						ar << GuildIndex;
						ar << guildacceptflag;
						ar << (ID)XGAME::xtGuildError::xGE_ERROR_ACCEPT_MEMBER;
						ar << idPacket;
						DBMNG->RegisterSendPacket( GetThis(), ar );
						return;
					}
				} else 
				if( alreadyGuildindex == 0 ) {
					XGuild::SGuildMember* pMember = pGuild->FindGuildMemberIdaccount( acceptidaccount );
					if( pMember == NULL ) {
						XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
						ar << 1;
						ar << acceptidaccount;
						ar << GuildIndex;
						ar << guildacceptflag;
						ar << (ID)XGAME::xtGuildError::xGE_ERROR_NO_AUTHORITY;
						ar << idPacket;
						DBMNG->RegisterSendPacket( GetThis(), ar );
						return;
					}
					if( pMember->s_Grade < XGAME::xtGuildGrade::xGGL_LEVEL4 ) {
						XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
						ar << 1;
						ar << acceptidaccount;
						ar << GuildIndex;
						ar << guildacceptflag;
						ar << (ID)XGAME::xtGuildError::xGE_ERROR_NO_AUTHORITY;
						ar << idPacket;
						DBMNG->RegisterSendPacket( GetThis(), ar );
						return;
					}

					XGuild::SGuildMember* ptargetMember = pGuild->FindGuildJoinReqMember( targetidaccount );
					if( ptargetMember == nullptr ) {
						XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
						ar << 1;
						ar << acceptidaccount;
						ar << GuildIndex;
						ar << guildacceptflag;
						ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
						ar << idPacket;
						DBMNG->RegisterSendPacket( GetThis(), ar );
						return;
					}
					_tstring moveusername = ptargetMember->s_strUserName;
					// 가입 승인
					if( guildacceptflag == xGAF_ACCEPT ) {
						XArchive arOut;
						BOOL bSuccess = pDB->LoadAccountGuildJoinReq( targetidaccount, arOut );

						int nCount = 0;
						ID dw1;
						arOut >> nCount;

						if( nCount > 0 ) {//가입 신청 했던 모든 길드의 정보에서 나를 제거한다.
							for( int n = 0; n < nCount; n++ ) {
//								arParam >> dw1;
								arOut >> dw1;		//
								if( dw1 > 0 ) {
									XGuild* preGuild = XGuildMgr::sGet()->FindGuild( dw1 );
									if( preGuild ) {
										XArchive arUpdate;
										preGuild->RemoveGuildJoinMember( moveusername );
										preGuild->SerializeGuildReqMemberlist( arUpdate );
										DBMNG->DoAsyncCall( GetThis(),
											&XGameSvrConnection::cbGuildUpdate,
											dw1,
											(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ,
											arUpdate, 0, 0, 0 );
									}
								}
							}
						}
						// 길드멤버로 가입시킨다.
						auto bOk = pGuild->AddGuildMember( targetidaccount
																							, moveusername
																							, ptargetMember->s_AccountLevel
																							, XGAME::xtGuildGrade::xGGL_LEVEL1 );
						if( bOk ) {
							// 가입신청 리스트에서 idAcc를 삭제
							auto bRemoved = pGuild->RemoveGuildJoinMember( targetidaccount );
							if( bRemoved ) {
								// 길드 멤버리스트와 가입신청리스트를 DB에 저장하고 모든 게임서버에 알려서 갱신하도록 한다.
								DBMNG->DoAsyncCall( GetThis()
																	, &XGameSvrConnection::cbGuildUpdateJoinAcceptAndBroadcast
																	, GuildIndex );

// 								XArchive arParam;
// 								pGuild->SerializeGuildMemberlist( arParam );
// 								pGuild->SerializeGuildReqMemberlist( arParam );
// 								DBMNG->DoAsyncCall( GetThis(),
// 									&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
// 									GuildIndex,
// 									(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_ACCEPT,
// 									arParam,
// 									acceptidaccount,
// 									idKey,
// 									idPacket );

								DBMNG->DoAsyncCall( GetThis(),
									&XGameSvrConnection::cbGuildAcceptProcess,
									acceptidaccount,
									targetidaccount,
									GuildIndex,
									(ID)XGAME::xtGuildGrade::xGGL_LEVEL1,
									(ID)XGAME::xtGuildAcceptFlag::xGAF_ACCEPT,
									idPacket );
							} else {
								XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
								ar << 1;
								ar << acceptidaccount;
								ar << GuildIndex;
								ar << guildacceptflag;
								ar << (ID)XGAME::xtGuildError::xGE_ERROR_ACCEPT_MEMBER;
								ar << idPacket;
								DBMNG->RegisterSendPacket( GetThis(), ar );
								return;
							}
						} else {
							XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
							ar << 1;
							ar << acceptidaccount;
							ar << GuildIndex;
							ar << guildacceptflag;
							ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
							ar << idPacket;
							DBMNG->RegisterSendPacket( GetThis(), ar );
							return;
						}
					} else 
					// 가입 거절
					if( guildacceptflag == xGAF_REFUSE ) {
						// 가입신청 리스트에서 삭제
						if( pGuild->RemoveGuildJoinMember( targetidaccount ) ) {
								// 길드 멤버리스트와 가입신청리스트를 DB에 저장하고 모든 게임서버에 알려서 갱신하도록 한다.
								DBMNG->DoAsyncCall( GetThis()
																	, &XGameSvrConnection::cbGuildUpdateJoinAcceptAndBroadcast
																	, GuildIndex );
// 							XArchive arParam;
// 							pGuild->SerializeGuildReqMemberlist( arParam );
// 							DBMNG->DoAsyncCall( GetThis(),
// 								&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
// 								GuildIndex,
// 								(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_ACCEPT,
// 								arParam,
// 								acceptidaccount,
// 								idKey,
// 								idPacket );
							DBMNG->DoAsyncCall( GetThis(),
								&XGameSvrConnection::cbGuildAcceptProcess,
								acceptidaccount,
								targetidaccount,
								GuildIndex,
								(ID)XGAME::xtGuildGrade::xGGL_NONE,
								(ID)XGAME::xtGuildAcceptFlag::xGAF_REFUSE,
								idPacket );
						} else {
							XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
							ar << 1;
							ar << acceptidaccount;
							ar << GuildIndex;
							ar << guildacceptflag;
							ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
							ar << idPacket;
							DBMNG->RegisterSendPacket( GetThis(), ar );
							return;
						}
					}
				}
			} else {
				XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
				ar << 1;
				ar << acceptidaccount;
				ar << GuildIndex;
				ar << guildacceptflag;
				ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_NOT_FOUND;
				ar << idPacket;
				DBMNG->RegisterSendPacket( GetThis(), ar );
				return;
			}
		}
		// 		else
		// 		{
		// 			XPacket ar((ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey);
		// 			ar << 1;
		// 			ar << acceptidaccount;
		// 			ar << GuildIndex;
		// 			ar << guildacceptflag;
		// 			ar << (ID)XGAME::xtGuildError::XGE_ERROR_GUILD_NOT_FOUND;
		// 			ar << idPacket;
		// 			DBMNG->RegisterSendPacket( GetThis(), ar);
		// 			return;
		// 		}
	} else {
		XPacket ar( (ID)xGDBPK_GUILD_JOIN_ACCEPT, idKey );
		ar << 1;
		ar << acceptidaccount;
		ar << GuildIndex;
		ar << guildacceptflag;
		ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
		ar << idPacket;
		DBMNG->RegisterSendPacket( GetThis(), ar );
	}

	return;
}

/**
 @brief 
*/
void XGameSvrConnection::ArchivingGuildJoinResult( XPacket* pOut 
																									, xtGuildError errCode
																									, ID idAccMaster
																									, ID idAccTarget
																									, ID idGuild
																									, xtGuildAcceptFlag acceptFlag )
{
	pOut->SetPacketHeader2( (ID)xGDBPK_GUILD_JOIN_RESULT );
	(*pOut) << 0;
	(*pOut) << idAccMaster;
	(*pOut) << idAccTarget;
	(*pOut) << idGuild;
	(*pOut) << acceptFlag;
	(*pOut) << (ID)errCode;
}

/**
 @brief 
 @param idAccMaster 가입승인해준 사람(길드 마스터)
 @param idAccTarget 가입된 유저
 @param gradeMember 가입된 유저의 등급
 @param acceptFlag 승인했는지 거절했는지.
*/
void XGameSvrConnection::cbGuildAcceptProcess(XDatabase *pDB
																						, ID idAccMaster
																						, ID idAccTarget
																						, ID idGuild
																						, xtGuildGrade gradeMember
																						, xtGuildAcceptFlag acceptFlag
																						/*, ID idPacket*/)
{
//	BOOL bSuccess = FALSE;
	xtGuildError errCode = xGE_SUCCESS;
	do {
		XArchive arJoinReqList;
		// idAccTarget유저가 가입신청한 길드리스트 로딩
		auto result = pDB->GetpDB()->LoadGuildJoinReqListByAcc( idAccTarget, &arJoinReqList );
		if( result != xDB_OK ) {
			errCode = xGE_ERROR_UNKNOWN;
			break;
		}
		int numList;
		arJoinReqList >> numList;
		// 길드 가입 승인
		if( acceptFlag == xGAF_ACCEPT ) {
			// 가입이 되었으므로 다른길드에 가입신청한것을 모두 클리어 해야함.
			for( int n = 0; n < numList; n++ ) {
				ID idGuildReq;
				arJoinReqList >> idGuildReq;		// 가입신청했던 길드아이디
				auto preGuild = XGuildMgr::sGet()->FindGuild( idGuildReq );
				if( preGuild ) {
					XArchive arRemove;
					// 해당 길드의 가입신청 목록에서 제거
					XAUTO_LOCK4( preGuild );
					preGuild->RemoveGuildJoinMember( idAccTarget );
					// 가입신청자 목록을 다시 저장.
					cbGuildUpdateJoinReqListAndBroadcast( pDB, idGuildReq );
				} // preGuild
			} // for
			// 
			XArchive arNew;
			XList4<ID> listEmpty;
			arNew << listEmpty;
			// 가입이 되었으니 기존에 가입신청했던 리스트는 날린다. 동시에 가입된 길드의 정보를 쓴다.
			auto bSuccess = pDB->GetpDB()->UpdateGuildJoinReqListByAcc( idAccTarget
																														, idGuild
																														, xGGL_LEVEL1
																														, arNew );
			if( !bSuccess ) {
				errCode = xGE_ERROR_UNKNOWN;
				break;
			}
		} else 
		// 길드 가입 거절
		if( acceptFlag == xGAF_REFUSE ) {
			const ID idGuildRefused = idGuild;		// 거절된 길드
			std::list<ID> listNewJoinReq;
			// 유저가 가입신청했던 길드목록에서 거절된 길드아이디를 제외하고 다시 저장.
			for( int n = 0; n < numList; n++ ) {
				ID idGuildEach;
				arJoinReqList >> idGuildEach;
				if( idGuildRefused != idGuildEach )
					listNewJoinReq.push_back( idGuildEach );
			}
			XArchive arNew;
			arNew << listNewJoinReq;
			auto bSuccess = pDB->GetpDB()->UpdateGuildJoinReqListByAcc( idAccTarget
																																, 0							// idGuild
																																, xGGL_NONE
																																, arNew );
			if( !bSuccess ) {
				errCode = xGE_ERROR_UNKNOWN;
				break;
			}
		}
	} while (0);
	// 가입거절된것을 모든 게임서버로 알려서 해당 유저가 접속중이면 정보를 갱신해야함.
	XPacket arPacket;
	ArchivingGuildJoinResult( &arPacket, errCode, idAccMaster, idAccTarget, idGuild, acceptFlag );
	MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::Send, arPacket );
}

void XGameSvrConnection::cbUserGuildInfoUpdate(XDatabase *pDB, ID idAct, ID idTarget, ID Guildindex, ID GuildGrade, XArchive& arParam, ID idKey, ID cbidpacket)
{
	if( idAct == idTarget ) //Out, Self Update
	{
		BOOL bSuccess = pDB->UpdateAccountGuildJoinReq( idTarget, Guildindex, GuildGrade, arParam );
		if( bSuccess ) {
			//가해자 한테도 알려준다.
			if( Guildindex == 0 ) //OUT
			{
				if( idKey > 0 && cbidpacket > 0 ) {
					XPacket ar( (ID)xGDBPK_GUILD_OUT, idKey );
					ar << 1;
					ar << idAct;
					ar << idTarget;
					ar << Guildindex;
					ar << (ID)XGAME::xtGuildError::xGE_SUCCESS;
					ar << cbidpacket;
					DBMNG->RegisterSendPacket( GetThis(), ar );
				}
			} else {//Update

			}
		}
	} else {//Kick
		BOOL bSuccess = pDB->UpdateAccountGuildJoinReq( idTarget, 0, 0, arParam );

		if( bSuccess ) {
			//target 한테 알려준다.
			XPacket ar( (ID)xGDBPK_GUILD_KICK, idKey );
			ar << 0;
			ar << idTarget;
			ar << (ID)XGAME::xtGuildError::xGE_SUCCESS;
			DBMNG->RegisterSendPacket( GetThis(), ar );
			//가해자 한테도 알려준다.
			if( idKey > 0 && cbidpacket > 0 ) {
				XPacket ar( (ID)xGDBPK_GUILD_KICK, idKey );
				ar << 1;
				ar << idAct;
				ar << idTarget;
				ar << Guildindex;
				ar << (ID)XGAME::xtGuildError::xGE_SUCCESS;
				ar << cbidpacket;
				DBMNG->RegisterSendPacket( GetThis(), ar );
			}

		}
	}
}

/**
 @brief 길드 멤버와 가입신청리스트를 저장하고 모든 게임서버에 보내 갱신하게 한다.
*/
void XGameSvrConnection::cbGuildUpdateJoinAcceptAndBroadcast( XDatabase* pDB, ID idGuild )
{
	XPacket ar( (ID)xGDBPK_GUILD_UPDATE );
	ar << 0;
	ar << idGuild;
	ar << (DWORD)xGU_UPDATE_JOIN_ACCEPT;
	//
	BOOL bSuccess = FALSE;
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if( XASSERT(pGuild) ) {
		XArchive arMember, arReqList;
		pGuild->SerializeGuildMemberlist( arMember );
		pGuild->SerializeGuildReqMemberlist( arReqList );
		bSuccess = pDB->UpdateGuildJoinAccept( idGuild, arMember, arReqList );
		if( bSuccess ) {
			ar << arMember;
			ar << arReqList;
			// xGDBPK_GUILD_UPDATE패킷을 모든 게임서버로 보내서 정보 갱신
			// Send()함수를 바로 사용하는데 스레드 안전한지 몰겄네.
			MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::Send, ar );
		}
	}
}

/**
 @brief idGuild의 가입신청목록을 DB에 저장하고 모든 게임서버로 브로드캐스트
 @param idGuild 해당길드의 가입신청자 목록을 저장.
 @param idAcc 값이 지정되면 idAcc유저의 가입신청목록도 함께 갱신
*/
void XGameSvrConnection::cbGuildUpdateJoinReqListAndBroadcast( XDatabase* pDB, ID idGuild )
{
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if( !pGuild )
		return;
	XArchive arJoinReqList;
	XAUTO_LOCK4( pGuild );
	pGuild->SerializeGuildReqMemberlist( arJoinReqList );
	// idGuild의 가입신청자 리스트 저장.
	pDB->GetpDB()->UpdateGuildJoinReqListByGuild( idGuild, arJoinReqList );
// 	if( idAcc )	이건 외부에서 따로 불러주자.
// 		cbRemoveidGuildInJoinReqListByAcc( pDB, idAcc, idGuild, gradeMember );
}

/**
 @brief idAcc유저의 가입신청목록에서 idGuild를 삭제하여 다시 저장.
 @param idGuildRemove 삭제될 길드
 @param gradeMember
 이런함수가 필요없을것 같은데? 길드에 가입하면 리스트에서 하나만 지우는게 아니라 리스트전체를 날려야 하기때문. 가입신청 취소기능이 있다면 필요해짐.
*/
// void XGameSvrConnection::cbRemoveidGuildInJoinReqListByAcc( XDatabase* pDB
// 																													, ID idAcc
// 																													, ID idGuildRemove
// 																													, xtGuildGrade gradeMember )
// {
// 	XArchive arJoinList;
// 	// idAcc유저의 가입신청한 길드리스트 로드
// 	auto errCode = pDB->GetpDB()->LoadGuildJoinReqListByAcc( idAcc, &arJoinList );
// 	if( errCode == xDB_OK ) {
// 		int num = 0;
// 		arJoinList >> num;
// 		XList4<ID> listJoinReq;
// 		for( int i = 0; i < num; ++i ) {
// 			DWORD idGuild;
// 			arJoinList >> idGuild;
// 			// 삭제하려는 길드아이디만 제외하고 리스트에 담음.
// 			if( idGuild != idGuildRemove )
// 				listJoinReq.Add( idGuild );
// 		}
// 		XArchive arNew;
// 		arNew << listJoinReq;
// 		// idAcc유저의 가입신청목록과 가입된 길드, 멤버등급을 저장한다.
// 		pDB->GetpDB()->UpdateGuildJoinReqListByAcc( idAcc, idGuildRemove, gradeMember, arNew );
// 	} // xDB_OK
// }

void XGameSvrConnection::cbGuildUpdate(XDatabase *pDB
																		, ID Guildindex
																		, ID GuildUpdateflag
																		, XArchive& arParam
																		, ID idAcc
																		, ID idKey
																		, ID cbidpacket)
{
	XPacket ar((ID)xGDBPK_GUILD_UPDATE);
	ar << 0;
	ar << Guildindex;
	ar << GuildUpdateflag;
	BOOL bSuccess = FALSE;
	ID cbGSidpacket = 0;
	//
	switch( ( XGAME::xtGuildUpdate )GuildUpdateflag ) {
	case XGAME::xtGuildUpdate::xGU_UPDATE_CREATE: {
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
		if( pGuild ) {
			pDB->UpdateGuildMember( Guildindex, arParam );
			bSuccess = TRUE;
			pGuild->Serialize( ar );
			if( MAIN ) {//모든 게임 서버에 알림.
				MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::Send, ar );
			}
			return;
		}
	} break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER: {
		ID MasterIdAcc = 0;
		ID idActMember = 0;
		ID idtgtMember = 0;
		int gradeActMember = 0;
		int gradetgtMember = 0;
		_tstring strMemberName;
		arParam >> idActMember;
		arParam >> gradeActMember;
		arParam >> idtgtMember;
		arParam >> gradetgtMember;
		arParam >> MasterIdAcc;
		arParam >> strMemberName;
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
		if( pGuild ) {
			XArchive ar1;
			pGuild->SerializeGuildMemberlist( ar1 );
			if( MasterIdAcc > 0 ) {
				bSuccess = pDB->UpdateGuildMemberwithMaster( Guildindex, MasterIdAcc, strMemberName, ar1 );
			} else {
				bSuccess = pDB->UpdateGuildMember( Guildindex, ar1 );
			}
			if( bSuccess ) {
				ar << idActMember;
				ar << gradeActMember;
				ar << idtgtMember;
				ar << gradetgtMember;
				ar << MasterIdAcc;
				ar << strMemberName;

				pGuild->SerializeGuildMemberlist( ar );
			}
		}
	} break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ: {
		asdfasedf
		XBREAK(1);		// 이코드는 더이상 사용안함.
		// arParam(길드멤버와 가입신청리스트) 저장?
// 		bSuccess = pDB->UpdateGuildJoinReq( Guildindex, arParam );
// 		if( bSuccess ) {
// 			XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
// 			if( pGuild ) {
// 				pGuild->DeSerializeGuildReqMemberlist( arParam );
// 				pGuild->SerializeGuildReqMemberlist( ar );
// 			}
// 			if( idAcc > 0 ) {
// 				XArchive arJoinList;
// 				// 길드 가입신청자 로드?
// 				auto errCode = pDB->GetpDB()->LoadAccountGuildJoinReq( idAcc, &arJoinList );
// 				if( errCode == xDB_OK ) {
// 					int num = 0;
// 					arJoinList >> num;
// 					if( num > 0 && num <= XGAME::MAX_GUILD_MEMBER_COUNT ) {
// 						std::list< ID > listJoinReq;
// 						for( int n = 0; n < num; ++n ) {
// 							ID idGuild = 0;
// 							arJoinList >> idGuild;
// 							if( idGuild == Guildindex ) {
// 								break;
// 							}
// 							listJoinReq.push_back( idGuild );
// 						}
// 						listJoinReq.push_back( Guildindex );
// 						XArchive arIn;
// 						arIn << (int)listJoinReq.size();
// 						for( auto idGuild : listJoinReq ) {
// 							arIn << idGuild;
// 						}
// 						pDB->GetpDB()->UpdateGuildJoinReq( idAcc, arIn );
// 					}
// 				} // xDB_OK
// 			}
// 		}
	} break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_ACCEPT: {
		XBREAK(1);		// cbGuildUpdateJoinAccept로 옮겨짐.
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
// 		if( pGuild ) {
// 			XArchive arMember, arReqList;
// 			pGuild->SerializeGuildMemberlist( arMember );
// 			pGuild->SerializeGuildReqMemberlist( arReqList );
// 			bSuccess = pDB->UpdateGuildJoinAccept( Guildindex, arMember, arReqList );
// 			if( bSuccess) {
// 				ar << arMember;
// 				ar << arReqList;
// 				// xGDBPK_GUILD_UPDATE패킷으로 모든 게임서버에 전송.
// 			}
// 		}
		// 모든 게임서버로 보냄.
// 		bSuccess = pDB->UpdateGuildJoinAccept( Guildindex, arParam );
// 		if( bSuccess ) {
// 			XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
// 			if( pGuild ) {
// 				pGuild->SerializeGuildMemberlist( ar );
// 				pGuild->SerializeGuildReqMemberlist( ar );
// 			}
// 		}
	} break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_CONTEXT: {
		_tstring strContext;
		arParam >> strContext;
		bSuccess = pDB->UpdateGuildContext( Guildindex, strContext );
		if( bSuccess ) {
			XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
			if( pGuild ) {
				pGuild->SetstrGuildContext( strContext.c_str() );
				ar << pGuild->GetstrGuildContext();
			}
		}
	} break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_OPTION: {
		bSuccess = pDB->UpdateGuildOption( Guildindex, arParam );
		if( bSuccess ) {
			XGuild* pGuild = XGuildMgr::sGet()->FindGuild( Guildindex );
			if( pGuild ) {
				pGuild->DeSerializeGuildOption( arParam );
				pGuild->SerializeGuildOption( ar );
			}
		}
	}break;
	case XGAME::xtGuildUpdate::xGU_UPDATE_DELETE:	{
		bSuccess = pDB->DeleteGuild( Guildindex );
		if( bSuccess ) {
			ar << Guildindex;
			XGuildMgr::sGet()->RemoveGuild( Guildindex );
		} 
	} break;
	}

	if( bSuccess ) {
		if( idKey != 0 && cbidpacket != 0 ) {
			XPacket p( (ID)xGDBPK_GUILD_UPDATE, idKey );
			p << 1;
			p << idAcc;
			p << Guildindex;
			p << GuildUpdateflag;
			p << (ID)XGAME::xtGuildError::xGE_SUCCESS;
			p << cbidpacket;
			DBMNG->RegisterSendPacket( GetThis(), p );
		}
		if( MAIN ) //모든 게임 서버에 알림.
		{
			MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::Send, ar );
		}
	}
}

void XGameSvrConnection::cbGuildDestroy(XDatabase *pDB, ID idaccount, ID GuildIndex, ID idKey, ID cbidpacket)
{

}

int XGameSvrConnection::RecvGuildUpdateOption(XPacket& p, ID idKey)
{
	ID idacc = 0;
	ID guildidx = 0;
	ID autoaccept = 0;
	ID blockaccept = 0;
	ID cbPacket = 0;

	p >> guildidx;
	p >> idacc;
	p >> autoaccept;
	p >> blockaccept;
	p >> cbPacket;


	//if (GUILD_MGR)
	{
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(guildidx);
		if (pGuild)
		{
			pGuild->SetGuildOption((BOOL)autoaccept, (BOOL)blockaccept);
			XArchive arParam;
			pGuild->SerializeGuildOption(arParam);

			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
				guildidx,
				(ID)XGAME::xtGuildUpdate::xGU_UPDATE_OPTION,
				arParam,
				idacc,
				idKey,
				cbPacket);
		}
	}
	return 1;
}
int XGameSvrConnection::RecvGuildUpdateMember(XPacket& p, ID idKey)
{
	ID idacc = 0;
	ID targetidacc = 0;
	ID guildidx = 0;
	int grade = 0;
	int accountlevel = 0;
	_tstring strMembername;
	ID cbPacket = 0;

	p >> guildidx;
	p >> idacc;

	p >> targetidacc;
	p >> strMembername;
	p >> grade;
	p >> accountlevel;

	p >> cbPacket;

	//if (GUILD_MGR)
	{
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( guildidx );

		if( pGuild ) {
			XGuild::SGuildMember* pMember1 = pGuild->FindGuildMemberIdaccount( idacc );
			XGuild::SGuildMember* pMember2 = pGuild->FindGuildMemberIdaccount( targetidacc );
			if( pMember1 == nullptr || pMember2 == nullptr ) {
				XPacket ar( (ID)xGDBPK_GUILD_UPDATE_MEMBER );
				ar << 1;
				ar << (ID)XGAME::XGE_ERROR_MEMBER_NOT_FOUND;
				Send( ar );
				//DBMNG->RegisterSendPacket( GetThis(), ar);
				return 1;
			}

			if( pMember1->s_Grade < XGAME::xGGL_LEVEL4 || pMember1->s_Grade <= pMember2->s_Grade ) //등급이 같거나 4등급 이하는 조작 불가.
			{
				XPacket ar( (ID)xGDBPK_GUILD_UPDATE_MEMBER );
				ar << 1;
				ar << (ID)XGAME::xGE_ERROR_NO_AUTHORITY;
				Send( ar );
				//				DBMNG->RegisterSendPacket( GetThis(), ar);
				return 1;
			}

			ID MasterChange = 0;
			XArchive arParam;

			if( grade == XGAME::xGGL_LEVEL5 ) {
				if( pMember1->s_Grade != XGAME::xGGL_LEVEL5 ) //길마를 넘기는거구만. 
				{
					XPacket ar( (ID)xGDBPK_GUILD_UPDATE_MEMBER );
					ar << 1;
					ar << (ID)XGAME::xGE_ERROR_NO_AUTHORITY;
					Send( ar );
					//					DBMNG->RegisterSendPacket( GetThis(), ar);
					return 1;
				}

				pMember1->s_Grade = XGAME::xGGL_LEVEL4;

				MasterChange = targetidacc;
				pGuild->SetstrMasterName( strMembername.c_str() );
				pGuild->SetGuildMasteridaccount( targetidacc );
			}
			pMember2->s_Grade = ( XGAME::xtGuildGrade ) grade;
			arParam << pMember1->s_idaccount;
			arParam << (int)pMember1->s_Grade;
			arParam << pMember2->s_idaccount;
			arParam << (int)pMember2->s_Grade;

			arParam << MasterChange;
			arParam << strMembername;

			pGuild->SerializeGuildMemberlist( arParam );

			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
				guildidx,
				(ID)XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER,
				arParam,
				idacc,
				idKey,
				cbPacket );
		} else {
			XPacket ar( (ID)xGDBPK_GUILD_UPDATE_MEMBER );
			ar << 1;
			ar << (ID)XGAME::XGE_ERROR_GUILD_NOT_FOUND;
			Send( ar );
			//		DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}

	return 1;
} // int XGameSvrConnection::RecvGuildUpdateMember(XPacket& p, ID idKey)

int XGameSvrConnection::RecvGuildUpdateContext(XPacket& p, ID idKey)
{
	ID idacc = 0;
	ID guildidx = 0;
	ID cbPacket = 0;
	_tstring strGuildContext;

	p >> guildidx;
	p >> idacc;
	p >> strGuildContext;
	p >> cbPacket;

	//if (GUILD_MGR)
	{
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(guildidx);
		if (pGuild)
		{
			XArchive arParam;
			arParam << strGuildContext;

			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
				guildidx,
				(ID)XGAME::xtGuildUpdate::xGU_UPDATE_CONTEXT,
				arParam,
				idacc,
				idKey,
				cbPacket);
		}
	}
	return 1;
}
void XGameSvrConnection::cbGuildUpdateOption(XPacket& p, ID idKey)
{

}
void XGameSvrConnection::cbGuildUpdateContext(XPacket& p, ID idKey)
{

}

int XGameSvrConnection::RecvGuildUpdate(XPacket& p, ID idKey)
{
	//ID idacc = 0;
	//ID guildidx = 0;
	//ID autoaccept = 0;
	//ID blockaccept = 0;
	//_tstring strGuildContext;

	//p >> guildidx;
	//p >> idacc;
	//p >> autoaccept;
	//p >> blockaccept;
	//p >> strGuildContext;

	//if (GUILD_MGR)
	//{
	//	XGuild* pGuild = XGuildMgr::sGet()->FindGuild(guildidx);
	//	if (pGuild)
	//	{
	//		pGuild->SetGuildOption((BOOL)autoaccept, (BOOL)blockaccept, strGuildContext);
	//		XArchive arParam;
	//		pGuild->SerializeGuildOption(arParam);

	//		DBMNG->DoAsyncCall( GetThis(),
	//			&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
	//			guildidx,
	//			(ID)XGAME::xtGuildUpdate::xGU_UPDATE_OPTION,
	//			arParam,
	//			idacc,
	//			0,
	//			0);
	//	}
	//}
	return 1;
}

/**
 @brief idAcc유저를 pGuild에 자동가입시킴
*/
void XGameSvrConnection::DoAutoJoinAccept( XGuild* pGuild, ID idAcc, const _tstring& strUsername, int lvAcc )
{
	if( !pGuild )
		return;
	const ID idGuild = pGuild->GetidGuild();
	// 길드 멤버에 추가. 이미 가입된 유저면 false리턴
	auto bOk = pGuild->AddGuildMember( idAcc, strUsername, lvAcc, xGGL_LEVEL1 );
	if( bOk ) {
		// 길드 멤버리스트와 가입신청리스트를 DB에 저장하고 모든 게임서버에 알려서 갱신하도록 한다.
		DBMNG->DoAsyncCall( GetThis()
											, &XGameSvrConnection::cbGuildUpdateJoinAcceptAndBroadcast
											, idGuild );
		// 자동가입승낙하고 관련 정보를 DB에 갱신
		DBMNG->DoAsyncCall( GetThis(),
												&XGameSvrConnection::cbGuildAcceptProcess,
												pGuild->GetGuildMasteridaccount(),
												idAcc,
												idGuild,
												xGGL_LEVEL1,
												xGAF_ACCEPT );
	} else {
		// 이미 가입된 멤버
		SendGuildJoinReq( idAcc, idGuild, xtGuildError::xGE_ERROR_ALREADY_MEMBER, idKey );
	}
}

int XGameSvrConnection::RecvGuildJoinReq(XPacket& p, ID idKey)
{
	ID idAcc;
	ID idGuild;
	ID idPacket;
	int lvAcc;
	_tstring strUsername;

	p >> idAcc;					// 가입신청자
	p >> strUsername;
	p >> idGuild;
	p >> lvAcc;
	p >> idPacket;
	do {
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
		if( pGuild == nullptr ) {
			// 요청한 길드 찾을 수 없음
			SendGuildJoinReq( idAcc, idGuild, XGE_ERROR_GUILD_NOT_FOUND, idKey );
			break;
		}
		const auto numMember = pGuild->GetGuildJoinMemberSize();
		if( numMember >= XGAME::MAX_GUILD_MEMBER_COUNT ) {
			// 길드 멤버가 꽉참.
			SendGuildJoinReq( idAcc, idGuild, xGE_ERROR_MAX_MEMBER_OVER, idKey );
			break;
		}
		// 자동 가입 옵션일 때.
		if( pGuild->GetGuildOption().s_bAutoAccept ) {
			DoAutoJoinAccept( pGuild, idAcc, strUsername, lvAcc );
		} else {
				









			여기 하다 맘
















			if( FindGuildMemberIdaccount( strUsername ) ) {// 이미 가입 되어 있는 유저.
				break;
			}

			if( pGuild->AddGuildJoinReqMember( idAcc, strUsername, 1 ) ) {
				XArchive arParam;
				pGuild->SerializeGuildReqMemberlist( arParam );

				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbGuildUpdate,
					idGuild,
					(ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ,
					arParam,
					idAcc,
					idKey,
					idPacket );

				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbGuildJoinReq,
					idAcc,
					idGuild,
					TRUE,
					0,
					0 );
			} else {
				SendGuildJoinReq( idAcc, idGuild, xtGuildError::xGE_ERROR_JOIN_MEMBER, idKey );
			}
		}
	} while(0);
	return 1;
}

/**
 @brief 길드 가입신청의 결과를 돌려준다.
 @param idAcc 가입 신청자
 @param idGuild 가입신청한 길드
 @param errCode 가입신청 결과
 @param idKey	전송키
 
*/
void XGameSvrConnection::SendGuildJoinReq( ID idAcc
																				, ID idGuild
																				, XGAME::xtGuildError errCode
																				, ID idKey )
{
	XPacket ar( (ID)xGDBPK_GUILD_JOIN_REQ, idKey );
	ar << 1;
	ar << idAcc;
	ar << idGuild;
	ar << (ID)errCode;
	Send( ar );
}

void XGameSvrConnection::cbGuildJoinReq(XDatabase *pDB, ID idAcc, ID Guildindex, BOOL bAdd, ID idKey, ID cbidpacket)
{
	XArchive arOut;
	BOOL bSuccess = pDB->LoadAccountGuildJoinReq(idAcc, arOut);
	if (bSuccess)
	{
		int nCount = 0;
		arOut >> nCount;

		XArchive arNew;
		ID guildindex = 0;
		std::list<ID> joinguildlist;

		for (int n = 0; n < nCount; n++)
		{
			arOut >> guildindex;
			if (guildindex != Guildindex)
				joinguildlist.push_back(guildindex);
		}

		if (bAdd)
		{
			joinguildlist.push_back(Guildindex);
		}

		std::list<ID>::iterator iter = joinguildlist.begin();
		int size = (int)joinguildlist.size();
		arNew << size;
		if (size > 0)
		{
			while (iter != joinguildlist.end())
			{
				arNew << (*iter);
				iter++;
			}
			bSuccess = pDB->UpdateAccountGuildJoinReq(idAcc, 0, 0, arNew);
		}
	}
}
int XGameSvrConnection::RecvGuildJoinAccept(XPacket& p, ID idKey)
{
	ID reqidaccount;
	ID guildid;
	ID flag;
	ID idPacket;
	ID idaccount;

	p >> idaccount;
	p >> reqidaccount;
	p >> guildid;
	p >> flag;
	p >> idPacket;


	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountJoinProcessGuildInfo,
		idaccount,
		reqidaccount,
		guildid,
		(ID)flag,
		idKey,
		idPacket);
	return 1;
}

int XGameSvrConnection::RecvGuildOut(XPacket& p, ID idKey)
{
	ID idaccount;
	ID guildid;
	ID idPacket;

	p >> idaccount;
	p >> guildid;
	p >> idPacket;

	XGuild* pGuild = XGuildMgr::sGet()->FindGuild( guildid );
	if( pGuild ) {
		XGuild::SGuildMember* pMember = pGuild->FindGuildMemberIdaccount( idaccount );
		if( pMember == nullptr ) {
			XPacket ar( (ID)xGDBPK_GUILD_OUT, idKey );
			ar << 1;
			ar << idaccount;
			ar << idaccount;
			ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
			ar << idPacket;
			Send( ar );
			return 1;
		}
		if( pMember->s_Grade == XGAME::xGGL_LEVEL5 ) {
			if( pGuild->GetGuildMemberSize() > 1 ) {
				XPacket ar( (ID)xGDBPK_GUILD_OUT, idKey );
				ar << 1;
				ar << idaccount;
				ar << idaccount;
				ar << (ID)XGAME::xtGuildError::xGE_ERROR_MASTER_OUT;
				ar << idPacket;
				Send( ar );
				return 1;
			}
			if( pGuild->GetGuildJoinMemberSize() > 0 ) {
				std::list<XGuild::SGuildMember*> pList = pGuild->GetListJoinReq();
				std::list<XGuild::SGuildMember*>::iterator itor = pList.begin();
				while( itor != pList.end() ) {
					XGuild::SGuildMember* pRequser = ( *itor );
					if( pRequser ) {
						ID idacc = pRequser->s_idaccount;

						DBMNG->DoAsyncCall( GetThis(),
							&XGameSvrConnection::cbGuildJoinReq,
							idacc,
							guildid,
							FALSE,
							0,
							0 );
					}
					itor++;
				}
			}
			XArchive arNone;
			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbUserGuildInfoUpdate,
				idaccount,
				idaccount,
				0,
				(ID)XGAME::xtGuildGrade::xGGL_NONE,
				arNone,
				idKey,
				idPacket );

			XArchive arParam;
			DBMNG->DoAsyncCall( GetThis(),
				&XGameSvrConnection::cbGuildUpdate,
				guildid,
				(ID)XGAME::xtGuildUpdate::xGU_UPDATE_DELETE,
				arParam,
				idaccount,
				0,
				0 );
		} else {
			if( pGuild->RemoveGuildMember( idaccount ) ) {
				XArchive arNone;
				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbUserGuildInfoUpdate,
					idaccount,
					idaccount,
					0,
					(ID)XGAME::xtGuildGrade::xGGL_NONE,
					arNone,
					idKey,
					idPacket );

				XArchive arParam;
				arParam << idaccount;
				arParam << 0;
				arParam << idaccount;
				arParam << 0;
				arParam << pGuild->GetGuildMasteridaccount();
				arParam << pGuild->GetstrMasterName();

				pGuild->SerializeGuildMemberlist( arParam );

				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbGuildUpdate, //(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam);
					guildid,
					(ID)XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER,
					arParam,
					idaccount,
					0,
					0 );
			} else {
				XPacket ar( (ID)xGDBPK_GUILD_OUT, idKey );
				ar << 1;
				ar << idaccount;
				ar << idaccount;
				ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
				ar << idPacket;
				Send( ar );
			}
		}
	}
	return 1;
}

int XGameSvrConnection::RecvGuildKick(XPacket& p, ID idKey)
{
	ID idaccount;
	ID reqidaccount;
	ID guildid;
	ID idPacket;

	p >> idaccount;		//가해자
	p >> reqidaccount;	//피해자
	p >> guildid;
	p >> idPacket;

	//if (GUILD_MGR)
	{
		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(guildid);
		if (pGuild)
		{
			XGuild::SGuildMember* pMember = pGuild->FindGuildMemberIdaccount(idaccount);
			if (pMember == nullptr)
			{
				XPacket ar((ID)xGDBPK_GUILD_KICK, idKey);
				ar << 1;
				ar << idaccount;
				ar << reqidaccount;
				ar << (ID)XGAME::xtGuildError::xGE_ERROR_NO_AUTHORITY;
				ar << idPacket;
				Send( ar );
//				DBMNG->RegisterSendPacket( GetThis(), ar);
				return 1;
			}
			if (pMember->s_Grade < XGAME::xtGuildGrade::xGGL_LEVEL4)
			{
				XPacket ar((ID)xGDBPK_GUILD_KICK, idKey);
				ar << 1;
				ar << idaccount;
				ar << reqidaccount;
				ar << (ID)XGAME::xtGuildError::xGE_ERROR_NO_AUTHORITY;
				ar << idPacket;
				Send( ar );
//				DBMNG->RegisterSendPacket( GetThis(), ar);
				return 1;
			}

			XGuild::SGuildMember* ptargetMember = pGuild->FindGuildMemberIdaccount(reqidaccount);
			if (ptargetMember == nullptr)
			{
				XPacket ar((ID)xGDBPK_GUILD_KICK, idKey);
				ar << 1;
				ar << idaccount;
				ar << reqidaccount;
				ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
				ar << idPacket;
				Send( ar );
//				DBMNG->RegisterSendPacket( GetThis(), ar);
				return 1;
			}

			if (pGuild->RemoveGuildMember(reqidaccount))
			{
				XArchive arNone;
				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbUserGuildInfoUpdate,
					idaccount,
					reqidaccount,
					0,
					(ID)XGAME::xtGuildGrade::xGGL_NONE,
					arNone,
					idKey,
					idPacket);

				XArchive arParam;
				arParam << pMember->s_idaccount;
				arParam << (int)pMember->s_Grade;
				arParam << reqidaccount;
				arParam << 0;
				arParam << pGuild->GetGuildMasteridaccount();
				arParam << pGuild->GetstrMasterName();

				pGuild->SerializeGuildMemberlist(arParam);

				DBMNG->DoAsyncCall( GetThis(),
					&XGameSvrConnection::cbGuildUpdate,
					guildid,
					(ID)XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER,
					arParam,
					idaccount,
					0,
					0);
			}
			else
			{
				XPacket ar((ID)xGDBPK_GUILD_KICK, idKey);
				ar << 1;
				ar << idaccount;
				ar << reqidaccount;
				ar << guildid;
				ar << (ID)XGAME::xtGuildError::XGE_ERROR_MEMBER_NOT_FOUND;
				ar << idPacket;
				Send( ar );
//				DBMNG->RegisterSendPacket( GetThis(), ar);
			}
		}
	}
	return 1;
}

void XGameSvrConnection::RecvAccountNickNameDuplicateCheck(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idaccount;
	ID idpacket;
	_tstring Buff;

	p >> idaccount;
	p >> Buff;
	p >> idpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountNickNameDuplicateCheck,
		idaccount,
		Buff.c_str(),
		idpacket,
		idKey);
}

void XGameSvrConnection::RecvAccountNameDuplicateCheck(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idaccount;
	ID idpacket;
	_tstring Buff;

	p >> idaccount;
	p >> Buff;
	p >> idpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountNameDuplicateCheck,
		idaccount,
		Buff.c_str(),
		idpacket,
		idKey);
}
void XGameSvrConnection::RecvAccountNameRegist(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idaccount;
	ID idpacket;
	_tstring strAccountName;
	_tstring strPass;

	p >> idaccount;
	p >> strAccountName;
	p >> strPass;
	p >> idpacket;
	BTRACE( "D:%s:id=%s", __TFUNC__, strAccountName.c_str() );

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountNameRegist,
		idaccount,
		strAccountName,
		strPass,
		idpacket,
		idKey);
}
void XGameSvrConnection::RecvAccountNameRegistCheck(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idaccount;
	ID idpacket;
	_tstring strAccountName;
	_tstring strPass;

	p >> idaccount;
	p >> strAccountName;
	p >> strPass;
	p >> idpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountNameRegistCheck,
		idaccount,
		strAccountName,
		strPass,
		idpacket,
		idKey);
}
void XGameSvrConnection::RecvAccountNickNameChange(XPacket& p, ID idKey)
{
	ID idaccount;
	ID idpacket;
	_tstring strAccountNickName;

	p >> idaccount;
	p >> strAccountNickName;
	p >> idpacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAccountNickNameChange,
		idaccount,
		strAccountNickName,
		idpacket,
		idKey);
}

void XGameSvrConnection::RecvTradeGemCall(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	int remaingem = 0;
	ID idClientConnection;
	ID idPacket;

	p >> idaccount;
	p >> remaingem;
	p >> idClientConnection;
	p >> idPacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbTradeGemCall,
		idaccount,
		remaingem,
		idClientConnection,
		idPacket,
		idKey);
}

void XGameSvrConnection::RecvShopGemCall(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	int remaingem = 0;
	ID idClientConnection;
	ID idPacket;

	p >> idaccount;
	p >> remaingem;
	p >> idClientConnection;
	p >> idPacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbTradeGemCall,
		idaccount,
		remaingem,
		idClientConnection,
		idPacket,
		idKey);
}

void XGameSvrConnection::cbAccountNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring  szAccountName, ID idpacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->AccountNameDuplicateCheck(szAccountName);
		XPacket ar((ID)xGDBPK_ACCOUNT_NAME_DUPLICATE, idKey);
		ar << 0;
		ar << idaccount;
		ar << (int)bFound;
		//ar.WriteString(szAccountName);
		ar << idpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}
void XGameSvrConnection::cbAccountNameRegistCheck(XDatabase *pDB, ID idaccount, _tstring strAccountName, _tstring strPassWord, ID idpacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->AccountNameRegistCheck(strAccountName, strPassWord);

		XPacket ar((ID)xGDBPK_ACCOUNT_NAME_REGIST_CHECK, idKey);
		ar << 0;
		ar << idaccount;
		ar << (int)bFound;
		ar << strAccountName;
		ar << strPassWord;
		ar << idpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}
void XGameSvrConnection::cbAccountNameRegist(XDatabase *pDB, ID idaccount, _tstring strAccountName, _tstring strPassWord, ID idpacket, ID idKey)
{
	BTRACE( "D0:%s:id=%s", __TFUNC__, strAccountName.c_str() );
	if (pDB) {
		BOOL bFound = pDB->AccountNameDuplicateCheck(strAccountName);
		if (bFound) {
			bFound = pDB->AccountNameRegist(idaccount, strAccountName, strPassWord);
			XBREAK( !bFound );
		}
		XPacket ar((ID)xGDBPK_ACCOUNT_NAME_REGIST, idKey);
		ar << 1;
		ar << idaccount;
		ar << (int)bFound;
		ar << strAccountName;
		ar << strPassWord;
		ar << idpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

void XGameSvrConnection::cbAccountNickNameChange(XDatabase *pDB, ID idaccount, _tstring  strNickName, ID idpacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->AccountNickNameDuplicateCheck(strNickName);
		if (bFound)
		{
			bFound = pDB->AccountNickNameRegist(idaccount, strNickName);
		}
		XPacket ar((ID)xGDBPK_ACCOUNT_NICKNAME_CHANGE, idKey);
		ar << 1;
		ar << idaccount;
		ar << (int)bFound;
		ar << strNickName;
		ar << idpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);

	}
}

void XGameSvrConnection::cbAccountNickNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring  szAccountNickName, ID idpacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->AccountNickNameDuplicateCheck(szAccountNickName);
		XPacket ar((ID)xGDBPK_ACCOUNT_NICKNAME_DUPLICATE, idKey);
		ar << 0;
		ar << idaccount;
		ar << (int)bFound;
		//ar.WriteString(szAccountNickName);			
		ar << idpacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

void XGameSvrConnection::cbTradeGemCall(XDatabase *pDB, ID idaccount, int remaingem, ID idClientConnection, ID idPacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->TradeGemCall(idaccount, remaingem);
		if (bFound)
		{
			XPacket ar(xGDBPK_TRADE_GEM_CALL, idKey);
			ar << 1;
			ar << idaccount;
			ar << remaingem;
			ar << idClientConnection;
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}
}


void XGameSvrConnection::cbShopGemCall(XDatabase *pDB, ID idaccount, int remaingem, ID idClientConnection, ID idPacket, ID idKey)
{
	if (pDB)
	{
		BOOL bFound = pDB->ShopGemCall(idaccount, remaingem);
		if (bFound)
		{
			XPacket ar(idPacket, idKey);
			ar << 0;
			ar << idaccount;
			ar << remaingem;
			ar << idClientConnection;
			ar << idPacket;
			ar << idKey;
			DBMNG->RegisterSendPacket( GetThis(), ar);
		}
	}
}


void XGameSvrConnection::SendSample(int param)
{
	// 	XPacket ar( (ID)xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC );
	// 	ar << 1;
	// 	Send( ar );
}

void XGameSvrConnection::SendNotify( const _tstring& strMessage )
{
	XPacket ar((ID)xGDBPK_GAME_NOTIFY);
	ar << 0;
	ar << strMessage;
	Send( ar );
//	DBMNG->RegisterSendPacket( GetThis(), ar);
}

void XGameSvrConnection::RecvUserLogAdd(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	int logtype = 0;
	_tstring strUserNickname;
	_tstring strLog;

	p >> idaccount;
	p >> logtype;
	p >> strUserNickname;
	p >> strLog;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbUserLogAdd,
		idaccount,
		logtype,
		strUserNickname,
		strLog);
}

void XGameSvrConnection::cbUserLogAdd(XDatabase *pDB, ID idaccount, int logtype, _tstring strNickName, _tstring strLog)
{
	if (pDB)
	{
		pDB->UserLogAdd(idaccount, logtype, strNickName, strLog);
	}
}

/**
 @brief 디바이스로부터 받은 GCM registration_id를 저장한다.
*/
void XGameSvrConnection::RecvReqGCMRegistId( XPacket& p, ID idKey )
{
	ID idAcc;
	_tstring strRegistId, strPlatform;
	p >> idAcc;
	p >> strRegistId;
	p >> strPlatform;
	DBMNG->DoAsyncCall( GetThis()
										, &XGameSvrConnection::cbGCMRegistId
										, idAcc
										, strRegistId
										, strPlatform
										, idKey );
}

void XGameSvrConnection::cbGCMRegistId(XDatabase *pDB
																			, const ID idAcc
																			, const _tstring& strRegistId
																			, const _tstring& strPlatform
																			, ID idKey )
{
	auto bOk = pDB->GetpDB()->RegistAccountDeviceID( idAcc, strRegistId, strPlatform );
	if( bOk ) {
		XPacket ar( xGDBPK_GAME_PUSH_GCM_REGIST_ID, idKey );
		ar << 1;
		ar << (DWORD)XGAME::xE_OK;
// 		ar << strRegistId;
// 		ar << strPlatform;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}
void XGameSvrConnection::RecvPushMsgRegist(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	int type1 = 0;
	int type2 = 0;
	int recvtime = 0;
	_tstring strPushMsg;

	p >> idaccount;
	p >> type1;
	p >> type2;
	p >> recvtime;
	p >> strPushMsg;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPushMsgRegist,
		idaccount,
		type1,
		type2,
		recvtime,
		strPushMsg);
}
void XGameSvrConnection::RecvPushMsgUnRegist(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	int type1 = 0;
	int type2 = 0;

	p >> idaccount;
	p >> type1;
	p >> type2;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbPushMsgUnRegist,
		idaccount,
		type1,
		type2);
}

void XGameSvrConnection::cbPushMsgRegist(XDatabase *pDB, ID idaccount, int type1, int type2, int recvtime, _tstring strMessage)
{
	if (pDB)
	{
		pDB->PushMsgRegist(idaccount, type1, type2, recvtime, strMessage);
	}
}
void XGameSvrConnection::cbPushMsgUnRegist(XDatabase *pDB, ID idaccount, int type1, int type2)
{
	if (pDB)
	{
		pDB->PushMsgUnRegist(idaccount, type1, type2);
	}
}
void XGameSvrConnection::RecvResourceTmpGet(XPacket& p, ID idKey)
{
	ID idSpot;
	ID idClientConnect;
	ID idPacket;
	ID idFinder;
	ID idTarget;

	p >> idFinder;
	p >> idTarget;
	p >> idSpot;
	p >> idClientConnect;
	p >> idPacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbResourceTmpGet,
		idFinder,
		idTarget,
		idSpot,
		idClientConnect,
		idKey,
		idPacket);
}
void XGameSvrConnection::cbResourceTmpGet(XDatabase *pDB, ID idFinder, ID idTarget, ID Spot, ID idClientConnect, ID idKey, ID cbidpacket)
{
	XArchive arParam;
	BOOL bFound = pDB->AccountLoadResourceTmp(idTarget, arParam);
	if (bFound)
	{
		int n = 0;
		// Todo..
	}
}

void XGameSvrConnection::RecvResourceTmpUpdate(XPacket& p, ID idKey)
{
	ID idSpot;
	ID idClientConnect;
	ID idPacket;
	ID idAcc;
	XArchive arparam;

	p >> idAcc;
	p >> idSpot;
	p >> arparam;
	p >> idClientConnect;
	p >> idPacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbResourceTmpUpdate,
		idAcc,
		idSpot,
		idClientConnect,
		arparam,
		idKey,
		idPacket);

}
void XGameSvrConnection::cbResourceTmpUpdate(XDatabase *pDB, ID idaccount, ID Spot, ID idClientConnect, XArchive& arParam, ID idKey, ID cbidpacket)
{
	BOOL bFound = pDB->AccountUpdateResourcetmp(idaccount, arParam);
	if (bFound)
	{
		int n = 0;
		// Todo..
	}
}

/**
 @brief 페북 fb_userid를 계정에 등록.
*/
void XGameSvrConnection::RecvReqRegisterFacebook( XPacket& p, ID idKey )
{
	ID idAcc;
	_tstring strFbUserId, strFbUsername;
	p >> idAcc;
	p >> strFbUserId;
	p >> strFbUsername;
	DBMNG->DoAsyncCall( GetThis()
										, &XGameSvrConnection::cbRegisterFacebook
										, idAcc
										, strFbUserId
										, strFbUsername
										, idKey );
}

void XGameSvrConnection::cbRegisterFacebook(XDatabase *pDB
																					, const ID idAcc
																					, const _tstring& strFbUserId
																					, const _tstring& strFbUsername
																					, ID idKey )
{
	XArchive arParam;

	bool bOk = pDB->GetpDB()->RegistFBAccount( idAcc, strFbUserId, strFbUsername );
	if( bOk ) {
		XPacket ar( xGDBPK_REGISTER_FACEBOOK, idKey );
		ar << 1;
		ar << idAcc;
		ar << strFbUserId;
		ar << strFbUsername;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

void XGameSvrConnection::RecvReqHelloMsgByidAcc( XPacket& p, ID idKey )
{
	ID idAcc, idSpot;
	p >> idAcc >> idSpot;
	DBMNG->DoAsyncCall( GetThis()
										, &XGameSvrConnection::cbReqHelloMsgByidAcc
										, idAcc
										, idSpot
										, idKey );
}

void XGameSvrConnection::cbReqHelloMsgByidAcc(XDatabase *pDB
																						, const ID idAcc
																						, const ID idSpot
																						, const ID idKey )
{
	_tstring strHello;
	bool bOk = pDB->GetpDB()->FindAccountHello( idAcc, &strHello );
	BTRACE( "D:%s: idAcc=%d", __TFUNC__, idAcc );
	XPacket ar( xGDBPK_REQ_HELLO_MSG_BY_IDACC, idKey );
	ar << 1;
	ar << (ID)((bOk)? idAcc : 0);
	ar << idSpot;
	ar << strHello;
	DBMNG->RegisterSendPacket( GetThis(), ar);
}

// #ifdef _INAPP_GOOGLE
// void XGameSvrConnection::RecvGoogleInappPayload(XPacket& p, ID idKey)
// {
// 	BYTE b0;
// 	ID idAccount, idClientConnect, idPacket;
// 	XGAME::xInApp inapp;
// 
// 	p >> idAccount;
// 	p >> inapp;
// 	p >> idClientConnect;
// 	p >> idPacket;
// 
// 	DBMNG->DoAsyncCall( GetThis(),
// 		&XGameSvrConnection::cbGoogleInappPayload,
// 		idAccount,
// 		inapp
// 		idClientConnect,
// 		idKey,
// 		idPacket);
// }
// 
// /*
// switch ((XGAME::xtInAppError)Result)
// {
// case XGAME::xIAPP_SUCCESS:						//성공.
// case XGAME::xIAPP_ERROR_UNKNOWN:				// 기타등등 에러
// case XGAME::xIAPP_ERROR_CRITICAL: 				// 있어서는 안되는 상황이 생겼을때.
// case XGAME::xIAPP_ERROR_VERIFY_SERVER_OFFLINE:	// 결제 검증 서버 오프라인
// case XGAME::xIAPP_ERROR_VERIFY_FAILED: 			// 검증 실패
// case XGAME::xIAPP_ERROR_RECEIPT_WRONG: 			// 결제 영수증 에러
// case XGAME::xIAPP_ERROR_NOT_FOUND_PRODUCT:		// 존재 하지 않는 상품
// case XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD:		// PAYLOAD 다름.
// case XGAME::xIAPP_ERROR_DUPLICATE_RECEIPT:		// 이미 사용된 영수증.
// }
// */
// void XGameSvrConnection::cbGoogleInappPayload(XDatabase *pDB
// 																						, ID idaccount
// 																						, const XGAME::xInApp& inapp
// 																						, ID idConnect
// 																						, ID idKey
// 																						, ID idPacket)
// {
// 	xtErrorIAP errCode = XGAME::xIAPP_ERROR_UNKNOWN;
// 	_tstring strPayload;
// 	// 영수증 검증
// 	do 
// 	{
// 		if( inapp.m_Platform == xPL_ANDROID ) {
// 			const auto& strcPublicKey = MAIN->GetstrPublicKey();
// 			XBREAK( strcPublicKey.empty() );
// 			auto resultVerify = XE::InappBillingVerify( 
// 															inapp.m_strcJsonReceipt
// 														, inapp.m_strcSignature
// 														, strcPublicKey ); //PubKey 설정 되면 넣어 줘야 한다.
// 			if( resultVerify != 1 ) {
// 				errCode = xIAPP_ERROR_RECEIPT_WRONG;
// 				break;
// 			}
// 		} else {
// 			XASSERT( platform == xPL_IOS );
// 			// 미구현
// 			XBREAK(1);
// 		}
// 		// DB
// 		errCode = pDB->CreateGooglePayload(idaccount
// 																		, inapp.m_typeProduct
// 																		, inapp.m_idsProduct
// 																		, &strPayload);
// 	} while (0);
// 
// 	XPacket ar((ID)xGDBPK_GAME_INAPP_PAYLOAD, idKey);
// 	ar << (DWORD)errCode;
// 	ar << idaccount;
// 	ar << (int)inapp.m_typeProduct;
// 	ar << inapp.m_idsProduct;
// 	ar << strPayload;
// 	ar << idConnect;
// 	ar << idPacket;
// 	DBMNG->RegisterSendPacket( GetThis(), ar);
// 
// }
#ifdef _INAPP_GOOGLE
void XGameSvrConnection::RecvGoogleInappPayload(XPacket& p, ID idKey)
{
	BYTE b0;
	ID idAccount, idClientConnect, idPacket;
	_tstring idsProduct;
	p >> idAccount;
	p >> b0;	auto platform = (XGAME::xtPlatform)b0;
	p >> b0;	auto typeProduct = (XGAME::xtCashType)b0;
	p >> b0 >> b0;
	p >> idsProduct;
	p >> idClientConnect;
	p >> idPacket;

	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbGoogleInappPayload,
		idAccount,
		platform,
		typeProduct,
		idsProduct,
		idClientConnect,
		idKey,
		idPacket);
}

/*
switch ((XGAME::xtInAppError)Result)
{
case XGAME::xIAPP_SUCCESS:						//성공.
case XGAME::xIAPP_ERROR_UNKNOWN:				// 기타등등 에러
case XGAME::xIAPP_ERROR_CRITICAL: 				// 있어서는 안되는 상황이 생겼을때.
case XGAME::xIAPP_ERROR_VERIFY_SERVER_OFFLINE:	// 결제 검증 서버 오프라인
case XGAME::xIAPP_ERROR_VERIFY_FAILED: 			// 검증 실패
case XGAME::xIAPP_ERROR_RECEIPT_WRONG: 			// 결제 영수증 에러
case XGAME::xIAPP_ERROR_NOT_FOUND_PRODUCT:		// 존재 하지 않는 상품
case XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD:		// PAYLOAD 다름.
case XGAME::xIAPP_ERROR_DUPLICATE_RECEIPT:		// 이미 사용된 영수증.
}
*/
void XGameSvrConnection::cbGoogleInappPayload(XDatabase *pDB
																						, ID idaccount
																						, XGAME::xtPlatform platform
																						, XGAME::xtCashType typeProduct
																						, const _tstring& idsProduct
																						, ID idConnect
																						, ID idKey
																						, ID idPacket)
{
	xtErrorIAP errCode = XGAME::xIAPP_ERROR_UNKNOWN;
	const _tstring strPayload = C2SZ( XE::GetGUID() );
// 	xtErrorIAP errCode = XGAME::xIAPP_ERROR_UNKNOWN;
// 	_tstring strPayload;
// 	// 영수증 검증
// 	do 
// 	{
// 		if( inapp.m_Platform == xPL_ANDROID ) {
// 			const auto& strcPublicKey = MAIN->GetstrPublicKey();
// 			XBREAK( strcPublicKey.empty() );
// 			auto resultVerify = XE::InappBillingVerify( 
// 															inapp.m_strcJsonReceipt
// 														, inapp.m_strcSignature
// 														, strcPublicKey ); //PubKey 설정 되면 넣어 줘야 한다.
// 			if( resultVerify != 1 ) {
// 				errCode = xIAPP_ERROR_RECEIPT_WRONG;
// 				break;
// 			}
// 		} else {
// 			XASSERT( platform == xPL_IOS );
// 			// 미구현
// 			XBREAK(1);
// 		}
	// payload를 DB에 저장
	do {
		// DB
		errCode = pDB->CreateGooglePayload(idaccount
																		, typeProduct
																		, idsProduct
																		, strPayload);
	} while (0);

	XPacket ar((ID)xGDBPK_GAME_INAPP_PAYLOAD, idKey);
	ar << idConnect;
	ar << (BYTE)errCode;
	ar << (BYTE)platform;
	ar << (BYTE)typeProduct;
	ar << (BYTE)0;
	ar << idaccount;
	ar << idsProduct;
	ar << strPayload;
	ar << idConnect;
	ar << idPacket;
	DBMNG->RegisterSendPacket( GetThis(), ar);

}
/**
 @brief 클라가 구매를 완료하고 서버에 승인을 요청함.
*/
void XGameSvrConnection::RecvGoogleInappBuyCash(XPacket& p, ID idKey)
{
	ID idAccount, idClientConnect, idPacket;
	XGAME::xInApp inapp;
	_tstring _strPublicKey;

	p >> idAccount;
	p >> inapp;
	p >> _strPublicKey;
	p >> idClientConnect;
	p >> idPacket;
	// C2SZ는 스레드안전하지 않아서 변환해서 보냄.
	XBREAK( inapp.m_strcPayload.empty() );
	const _tstring strPayload = C2SZ( inapp.m_strcPayload );
	const std::string strcPublicKey = SZ2C( _strPublicKey );

	//Google 은 횟수 제한이 있으니까....DB 먼저검색 해서 결과를 가지고 Google Verify 를 진행 하도록 하자.
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbGoogleInappBuyCash,
		idAccount,
		inapp,
		strPayload,
		strcPublicKey,
		idClientConnect,
		idKey,
		idPacket);

}


void XGameSvrConnection::cbGoogleInappBuyCash(XDatabase *pDB
																						, ID idaccount
																						, const XGAME::xInApp& inapp
																						, const _tstring& strPayload
																						, const std::string& strcPublicKey
																						, ID idconnect, ID idKey, ID idPacket)
{
	// payload검사
	// 영수증 verify검사
//	const std::string strcPublicKey = MAIN->GetstrPublicKey();
	if( inapp.m_Platform == XGAME::xPL_GOOGLE_STORE ) {
		XBREAK( strcPublicKey.empty() );
	}
	auto errCode = XGAME::xIAPP_ERROR_UNKNOWN;
	//
	do {
		// 발급한 페이로드가 맞는지 검사.
		_tstring idsProductInDB;
		errCode = pDB->GetpDB()->LoadGooglePayload(idaccount, strPayload, &idsProductInDB );
		//
		if(errCode == XGAME::xIAPP_SUCCESS) {
			if( inapp.m_idsProduct != idsProductInDB ) {
				errCode = XGAME::xIAPP_ERROR_DIFF_PAYLOAD_WITH_PRODUCT;
				break;
			}
			if( inapp.m_Platform == XGAME::xPL_GOOGLE_STORE ) {
				auto resultVerify = XE::InappBillingVerify(
					inapp.m_strcJsonReceipt
					, inapp.m_strcSignature
					, strcPublicKey ); //PubKey 설정 되면 넣어 줘야 한다.
				if( resultVerify != 1 ) {
					errCode = XGAME::xIAPP_ERROR_VERIFY_FAILED;
					break;
				}
				//Google 검증.
				// 		XHttpMng::sGetSingleton()->SendReqPOST(_T(ITMS_SANDBOX_VERIFY_RECEIPT_URL),
				// 			cBody,
				// 			this,		// delegate(this객체는 레퍼런스 카운트 객체로 할것)
				// 			xHTTP_MAKE_DELEGATE(this, &XGameSvrConnection::OnReqFromAppleVerify),
				// 			(void*)pPurchaseInfo);	// 응답이 돌아왔을때 비교에 필요한 데이타를 담아 넣는다.
			} // google
		}
	} while (0);
	XPacket ar( (ID)xGDBPK_GAME_INAPP_PAYLOAD, idKey );
	ar << idconnect;
	ar << (BYTE)errCode;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << idaccount;
	ar << inapp;
	ar << idconnect;
	ar << idPacket;
	DBMNG->RegisterSendPacket( GetThis(), ar );
}

/**
 @brief 페이로드 삭제 후 거래 마무리
*/
void XGameSvrConnection::RecvDeletePayload(XPacket& p, ID idKey)
{
	ID idAcc;
	_tstring idsProduct, strPayload;

	p >> idAcc;
	p >> idsProduct >> strPayload;

	//Google 은 횟수 제한이 있으니까....DB 먼저검색 해서 결과를 가지고 Google Verify 를 진행 하도록 하자.
	DBMNG->DoAsyncCall( GetThis(),
										&XGameSvrConnection::cbDeletePayload,
										idAcc,
										idsProduct,
										strPayload,
										idKey );

}


void XGameSvrConnection::cbDeletePayload(XDatabase *pDB
																				, ID idAcc
																				, const _tstring& idsProduct
																				, const _tstring& strPayload
																				, ID idKey )
{
	// 사용한 페이로드 삭제.
	auto errCode = pDB->GetpDB()->DeletePayload( idAcc, strPayload );
	//
	XPacket ar( (ID)xGDBPK_GAME_INAPP_PAYLOAD, idKey );
	ar << 1;
	ar << (BYTE)errCode;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << idAcc;
	ar << idsProduct;
	DBMNG->RegisterSendPacket( GetThis(), ar );
}


// void XGameSvrConnection::RecvGoogleInappBuyCash(XPacket& p, ID idKey)
// {
// 	ID idaccount;
// 	ID idClientConnection;
// 	ID idPacket;
// 	int producttype;
// 	_tstring strProductID;
// 	_tstring strReceipt;
// 
// 	p >> idaccount;
// 	p >> producttype;
// 	p >> strProductID;
// 	p >> strReceipt;
// 	p >> idClientConnection;
// 	p >> idPacket;
// 	//Google 은 횟수 제한이 있으니까....DB 먼저검색 해서 결과를 가지고 Google Verify 를 진행 하도록 하자.
// 
// 	DBMNG->DoAsyncCall( GetThis(),
// 		&XGameSvrConnection::cbGoogleInappBuyCash,
// 		idAccount,
// 		producttype,
// 		strProductID,
// 		strReceipt,
// 		idClientConnect,
// 		idKey,
// 		idPacket);
// 
// }
// 
// 
// void XGameSvrConnection::cbGoogleInappBuyCash(XDatabase *pDB, ID idaccount, _tstring strProductID, _tstring strPayload, _tstring strReceipt, ID idconnect, ID idKey, ID idPacket)
// {
// 	BYTE Result = pDB->InAppGoogleReceiptVerify(idaccount, strproductID, strPayload, strReceipt);
// 	if (Result == XGAME::xIAPP_SUCCESS)
// 	{
// 		//Google 검증.
// 		XHttpMng::sGetSingleton()->SendReqPOST(_T(ITMS_SANDBOX_VERIFY_RECEIPT_URL),
// 			cBody,
// 			this,		// delegate(this객체는 레퍼런스 카운트 객체로 할것)
// 			xHTTP_MAKE_DELEGATE(this, &XGameSvrConnection::OnReqFromAppleVerify),
// 			(void*)pPurchaseInfo);	// 응답이 돌아왔을때 비교에 필요한 데이타를 담아 넣는다.
// 	}
// 	else
// 	{
// 		XPacket ar((ID)xGDBPK_GAME_INAPP_PAYLOAD, idKey);
// 		ar << (ID)Result;
// 		ar << idaccount;
// 		ar << producttype;
// 		ar << strproductID;
// 		ar << strPayload;
// 		ar << idConnect;
// 		ar << idPacket;
// 		DBMNG->RegisterSendPacket( GetThis(), ar);
// 	}
// }
#endif
#ifdef _INAPP_APPLE
void XGameSvrConnection::RecvAppleInappBuyCash(XPacket& p, ID idKey)
{
	ID idaccount = 0;
	ID idPacket = 0;
	ID idClientConnection = 0;

	_tstring strSKU;
	_tstring strReceipt;
	_tstring strPurchaseInfo;

	p >> idaccount;
	p >> strSKU;
	p >> strReceipt;
	p >> strPurchaseInfo;
	p >> idClientConnection;
	p >> idPacket;

	char cPurchaseInfo[8192];
	size_t retLen = 0;
	strcpy_s(cPurchaseInfo, SZ2C(szPurchaseInfo));
	const char* decodedPurchaseInfo = (const char *)base64_decode(cPurchaseInfo, &retLen);
	memset(cPurchaseInfo, 0, sizeof(cPurchaseInfo));
	memcpy_s(cPurchaseInfo, sizeof(cPurchaseInfo), decodedPurchaseInfo, retLen);
	free((void*)decodedPurchaseInfo);
	rapidjson::Document jsonDoc;
	jsonDoc.Parse<0>(cPurchaseInfo);

	if (jsonDoc.HasParseError())
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_RECEIPT_WRONG;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		Send( ar );
		//DBMNG->RegisterSendPacket( GetThis(), ar);
		return;
	}

	char cBody[8192];
	sprintf_s(cBody, "{\"receipt-data\" : \"%s\", \"password\" : \"%s\"}",
		SZ2C(szReceipt),
		ITC_CONTENT_PROVIDER_SHARED_SECRET);
	xAPPLE_PURCHASE *pPurchaseInfo = new xAPPLE_PURCHASE;
	pPurchaseInfo->cstrPurchaseInfo = cPurchaseInfo;
	pPurchaseInfo->idAcc = idAcc;
	// 요청 결과는 델리게이터(콜백)로 온다.
	XHttpMng::sGet()->SendReqPOST(_T(ITMS_SANDBOX_VERIFY_RECEIPT_URL),
		cBody,
		this,		// delegate(this객체는 레퍼런스 카운트 객체로 할것)
		xHTTP_MAKE_DELEGATE(this, &XGameSvrConnection::OnReqFromAppleVerify),
		(void*)pPurchaseInfo);	// 응답이 돌아왔을때 비교에 필요한 데이타를 담아 넣는다.

}
void XGameSvrConnection::cbAppleInappBuyCash(XDatabase *pDB, ID idaccount, _tstring strSKU, _tstring strReceipt, _tstring strPurchaseInfo, ID idClientConnect, ID idKey, ID idPacket)
{
	BYTE Result = pDB->InappAppleReceiptVerify(idaccount, strSKU, strReceipt, strPurchaseInfo);
	if (bFound)
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)Result;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
	else
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_DUPLICATE_RECEIPT;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		DBMNG->RegisterSendPacket( GetThis(), ar);
	}
}

void XGameSvrConnection::OnReqFromAppleVerify(const char *cResponse, xHTTP::xREQ *pReq)
{
	CONSOLE("OnReqFromAppleVerify");
	// Http Post로 요청한 결과를 Json객체로 담음
	rapidjson::Document jsonResponse;
	jsonResponse.Parse<0>(cResponse);
	if (jsonResponse.IsObject() == false)
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_RECEIPT_WRONG;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		Send( ar );
		//DBMNG->RegisterSendPacket( GetThis(), ar);
		return;
	}
	if (jsonResponse.HasMember("status") == false)
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_RECEIPT_WRONG;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		Send( ar );
//		DBMNG->RegisterSendPacket( GetThis(), ar);
		return;
	}
	int status = jsonResponse["status"].GetInt();
	if (status != 0 && status != 21006)
	{
		CONSOLE("*********************************** Purchase Failed. error code = %d", (int)status);

		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_RECEIPT_WRONG;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		Send( ar );
//		DBMNG->RegisterSendPacket( GetThis(), ar);
		return;
	}
	const rapidjson::Value& valueRequest = jsonResponse["receipt"];
	// Http Request된 결과와 비교할 영수증 Json을 Json객체에 담음
	rapidjson::Document jsonPurchaseInfo;
	xAPPLE_PURCHASE *pPurchaseInfo = static_cast<xAPPLE_PURCHASE*>(pReq->pUserData);
	ID idAcc = pPurchaseInfo->idAcc;
	jsonPurchaseInfo.Parse<0>(pPurchaseInfo->cstrPurchaseInfo.c_str());
	if (jsonPurchaseInfo.HasParseError()
		|| (jsonPurchaseInfo.IsObject() == false)
		|| (IsEqualJsonValue(valueRequest, "transaction_id", jsonPurchaseInfo, "transaction-id", idAcc) == FALSE)
		|| (IsEqualJsonValue(valueRequest, "bid", jsonPurchaseInfo, "bid", idAcc) == FALSE)
		|| (IsEqualJsonValue(valueRequest, "product_id", jsonPurchaseInfo, "product-id", idAcc) == FALSE)
		|| (IsEqualJsonValue(valueRequest, "quantity", jsonPurchaseInfo, "quantity", idAcc) == FALSE)
		|| (IsEqualJsonValue(valueRequest, "item_id", jsonPurchaseInfo, "item-id", idAcc) == FALSE))
	{
		XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE, idKey);
		ar << (ID)XGAME::xIAPP_ERROR_RECEIPT_WRONG;
		ar << idaccount;
		ar << strSKU;
		ar << strReceipt;
		ar << strPurchaseInfo;
		ar << idConnect;
		ar << idPacket;
		Send( ar );
//		DBMNG->RegisterSendPacket( GetThis(), ar);

		SAFE_DELETE(pPurchaseInfo);
		return;
	}

#pragma message("DB 에서 유효성 검사 필요.")
	DBMNG->DoAsyncCall( GetThis(),
		&XGameSvrConnection::cbAppleInappBuyCash,
		idAccount,
		strSKU,
		strReceipt,
		strPurchaseInfo,
		idClientConnect,
		idKey,
		idPacket);
	SAFE_DELETE(pPurchaseInfo);
}
#endif

BOOL IsEqualJsonValue(const rapidjson::Value& jsonDoc1,
	const char *cName1,
	rapidjson::Document& jsonDoc2,
	const char *cName2,
	ID idAcc)
{
	if (jsonDoc1.HasMember(cName1) == false)
	{
		CONSOLE("******************** has not json1 member: %s, idAcc=%d", C2SZ(cName1), idAcc);
		return FALSE;
	}
	if (jsonDoc2.HasMember(cName2) == false)
	{
		CONSOLE("******************** has not json2 member: %s, idAcc=%d", C2SZ(cName2), idAcc);
		return FALSE;
	}
	std::string str1 = jsonDoc1[cName1].GetString();
	std::string str2 = jsonDoc2[cName2].GetString();
	if (str1 == str2)
		return TRUE;
	CONSOLE("******************* purchase failed. %s not equal.: idAcc=%d", C2SZ(cName1), idAcc);
	return FALSE;
}
// 
// void XGameSvrConnection::SendAccountDuplicateLogout()
// {
// 	XPacket ar( (ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT );
// 	Send( ar );		// 클라이언트로 데이타를 보냄
// }






