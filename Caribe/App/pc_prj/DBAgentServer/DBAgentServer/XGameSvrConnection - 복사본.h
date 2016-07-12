#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"
#ifdef _INAPP_APPLE
#include "XFramework/XHttpMng.h"
#endif
#include "XDBUAccount.h"
namespace XGAME {
	struct xInApp;
};

#pragma warning( disable : 4250 )	//  warning C4250: 'xxxx' : 우위에 따라 'xxxxx'을(를) 상속합니다.

class XDBUAccount;
class XUser;
class XDBMng;
class XDatabase;
class XWorld;
class XPostInfo;

class XGameSvrConnection : public XEWinConnectionInServer
{
//	XSPLock m_spLock;
	int m_numConnectedClient;		// 이 게임서버에 물려있는 접속자 수.
	WORD m_GameSvrPort;			// 이 커넥션에 연결되어있는 게임서버의 클라이언트 소켓 포트.
	WORD wReserved;
	//	WORD m_GSvrID;				// 게임서버 아이디-XEWinConnectionInServer::m_idConnectUserDef 로 대체함.
	char m_cIPExternal[16];			// 클라이언트에 알려질 this게임서버의 외부 아이피

	void Init() {
		m_numConnectedClient = 0;
		m_GameSvrPort = 0;
		m_cIPExternal[0] = 0;
		//		m_GSvrID = 0;
	}
	void Destroy() {
	}

	GET_SET_ACCESSOR_CONST(int, numConnectedClient);
	SET_ACCESSOR(WORD, GameSvrPort);
public:
	XGameSvrConnection(SOCKET socket, LPCTSTR szIP);
	virtual ~XGameSvrConnection() { Destroy(); }
	//
	GET_ACCESSOR_CONST(const char*, cIPExternal);
	GET_ACCESSOR_CONST(WORD, GameSvrPort);
//	GET_ACCESSOR( XSPLock&, spLock );

	WORD GetGSvrID(void) {
		return (WORD)GetidConnectUserDef();
	}
	void SetGSvrID(WORD widGSvr) {
		SetidConnectUserDef((DWORD)widGSvr);
	}
	//
	virtual BOOL ProcessPacket(DWORD idPacket, ID idKey, XPacket& p);
	virtual void OnError(xtError error, DWORD param1, DWORD param2);
	//
	void UpdateCalcWorld(ID idAcc, XWorld *pWorld) const;
	//	void GetLootingAmount(XWorld *pWorld, int *pOutWood, int *pOutIron);
	//	
	void RecvGameSvrInfo(XPacket& p, ID idKey);
	void RecvDidFinishLoad(XPacket& p, ID idKey);

	void RecvAccountNickNameDuplicateCheck(XPacket& p, ID idKey);
	void RecvAccountNameDuplicateCheck(XPacket& p, ID idKey);
	void RecvAccountNameRegist(XPacket& p, ID idKey);
	void RecvAccountNameRegistCheck(XPacket& p, ID idKey);
	void RecvAccountNickNameChange(XPacket& p, ID idKey);

	void RecvAccountInfoLoad(XPacket& p, ID idKey);
	void RecvAccountInfoSave(XPacket& p, ID idKey);
	void RecvAccountLoingSave(XPacket& p, ID idKey);
	void RecvAccountLogOut(XPacket& p, ID idKey);
	void RecvGetUserLegionByPower(XPacket& p, ID idKey);
	void cbFindAccountLegionByPower(XDatabase *pDB, XArchive& arParam);
	void RecvGetUserLegionByIdAcc(XPacket& p, ID idKey);
	//	void RecvDoMatchingByScore( XPacket& p, ID idKey );
	//	void RecvGetWoodIron( XPacket& p, ID idKey );
	//	void RecvAddWoodIron( XPacket& p, ID idKey );
	void RecvGetUserByPower(XPacket& p, ID idKey);
	void RecvReqMatchJewelMine(XPacket& p, ID idKey);
	void RecvGetJewelLegionByIdAcc(XPacket& p, ID idKey);
	void RecvChangeJewelOwn(XPacket& p, ID idKey);
	void RecvAddDefenseByJewel(XPacket& p, ID idKey);
	void RecvUpdateJewelMineInfo( XPacket& p, ID idKey );
	void cbUpdateJewelMine(XDatabase *pDB, XArchive& arJewel, ID idKey);
	void cbUpdateJewelMine2( XDatabase *pDB, ID idDBAcc, XArchive& arInfo, xSec secUpdate, ID idKey );
	void RecvDefeatInfo(XPacket& p, ID idKey);
	void RecvEncounterWin(XPacket& p, ID idKey);
	void RecvLockLoginForBattle(XPacket& p, ID idKey);
	void RecvUnlockLoginForBattle(XPacket& p, ID idKey);

	void cbTargetAttack(XDatabase *pDB, ID idAttacker, ID idDefender, XArchive& arPack, ID idKey);
	bool IsOnlineUser(ID idAcc);
	void RecvReqCampaignByGuildRaid(XPacket& p, ID idKey);
	void cbCampaignByGuildRaid(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, ID idKey, XArchive& arCampDefault);
	void RecvReqGuildEnterRaid(XPacket& p, ID idKey);
	void cbEnterGuildRaid(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, XArchive& arParam, ID idKey);
	void RecvReqGuildLeaveRaid(XPacket& p, ID idKey);
	void cbLeaveGuildRaid(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, XGAME::xtWin typeWin);
	void RecvReqGuildRaidOpen(XPacket& p, ID idKey);
	void cbGuildRaidOpen(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, XArchive& arCamp, ID idKey);
	void cbUpdateGuildRaidCampaign(XDatabase *pDB, ID idAcc, ID idGuild, ID idCamp, ID idSpot, ID idKey);
	void SendGuildPointByMail(ID idGuild, int point);
	// xuzhu end
	void RecvAccountPostInfo(XPacket& p, ID idKey);
	void RecvAccountPostInfoUpdate(XPacket& p, ID idkey);
	void RecvPostInfoAdd(XPacket& p, ID idKey);
	void RecvPostInfoDestroy(XPacket& p, ID idKey);
	void RecvPostItemsRemove(XPacket& p, ID idKey);

	void RecvUpdateCashInfo(XPacket& p, ID idKey);
	void cbCashinfoUpdate(XDatabase *pDB, ID idAccount, int remaincash);

	void RecvAccountShopInfoUpdate(XPacket& p, ID idKey);
	void cbRecvAccountShopInfoUpdate(XDatabase *pDB, XSPDBUAcc spAccOut, ID idAccount, ID idKey);
	// Delegate

	//CallBack
	/*
	void cbFindUserByScore( XDatabase *pDB,
	XSPDBUAcc spAccOut,
	DWORD score_dw,
	float secAdjustOffline,
	ID idFinder,
	ID idSpot, ID idClientConnect,
	ID idPacket );
	*/
	void cbFindUserByPower(XDatabase *pDB,
		//		XSPDBUAcc spAccOut,
		XArchive& arParam);
	void cbFindAccountByIdAcc(XDatabase *pDB,
		XSPDBUAcc spAccOut,
		ID idAccount,
		ID idConnect,
		bool bSessionKeyLogin,
		ID idKey);
	void OnFoundUserByScore2(BOOL bFound,
		XDBUAccount* pAccount,
		XPacket& pk);
	void cbAccountLogOut(XDatabase *pDB, ID newidConnect/*, ID alreadyuseridConnect*/, ID idAccount);
	void cbAccountLoginSave(XDatabase *pDB, ID idAccount, ID gameserverid, _tstring strConnectIP, ID idKey);
	void cbSaveAccount(XDatabase *pDB,
		XSPDBUAcc spAccOut,
		ID idAccount,
		ID idConnect,
		BOOL bReq,
		ID idReqPacket,
		ID idKey);
	void cbGetLegionByIdAcc(XDatabase *pDB,
		ID idAcc,
		ID idSpot, ID idClientConnect,
		ID idPacket,
		ID idKey);
	void OnResultGetLegionByIdAcc(BOOL bFound,
		ID idSpot,
		ID idClientConnect,
		ID idPacket,
		ID idAcc,
		XArchive& arResult);
		void cbChangeOwnerJewelMine( XDatabase *pDB, ID idDBAcc, XArchive& arJewel, xSec secUpdate );
// 	void cbChangeOwnerJewelMine(XDatabase *pDB, XArchive& arParam);
	void cbFindJewelInfoByIdAcc(XDatabase *pDB,
		ID idAccount,
		ID idClientConnect,
		ID idPacket,
		int idxMine,
		ID idKey);
	void cbFindJewelMatch( XDatabase *pDB, XArchive& arParam, ID idClientConnect, ID idKey );
	void cbLockLoginForBattle(XDatabase *pDB,
		ID idKey,
		ID idAttacker,
		ID idDefender,
		DWORD secStart,
		XArchive& arPack);
	void cbUnlockLoginForBattle(XDatabase *pDB,
		ID idDefender, ID idAttacker);
	// callback result
	void OnResultUnlockLoginForBattle(ID idDefender);
	void cbUpdatePost(XDatabase *pDB,
		XSPDBUAcc spAccOut,
		ID idAccount,
		ID idConnect,
		BOOL bReq,
		ID idReqPacket,
		ID idKey);

	void cbPostDestroy(XDatabase *pDB, ID idAccount, ID idpost, ID idKey, ID cbidpacket);
	void cbPostItemsRemove(XDatabase *pDB, ID idAccount, ID idpost, ID idKey, ID cbidpacket);
	void cbDefeatInfo(XDatabase *pDB, XArchive& arParam, XGAME::xtSpot typeSpot, int powerGrade, ID idKey);
	void cbEncounterWin(XDatabase *pDB, XArchive& arParam);
	void RecvReqMatchMandrake(XPacket& p, ID idKey);
	void cbReqMatchMandrake(XDatabase *pDB, XArchive& arParam);
	void RecvGetMandrakeLegionByIdAcc(XPacket& p, ID idKey);
	void cbFindMandrakeInfoByIdAcc(XDatabase *pDB, XArchive& arParam);
	void RecvChangeMandrakeOwn(XPacket& p, ID idKey);
	void cbChangeOwnerMandrake(XDatabase *pDB, XArchive& arParam);
	void cbPostInfo(XDatabase *pDB, XArchive& arParam);
	void cbPostAdd(XDatabase *pDB, ID idaccount, ID postsn, ID PostType, ID idKey, ID cbidpacket, XArchive& arParam);

	void RecvRankingInfo(XPacket& p, ID idKey);
	void cbRankingInfo(XDatabase *pDB, ID idKey);

	int RecvGuildInfo(XPacket& p, ID idKey);
	void cbGuildInfo(XDatabase *pDB, ID idKey);

	int RecvGuildCreate(XPacket& p, ID idKey);
	void cbGuildNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring strCreateGuildName, XArchive& arParam, ID idKey, ID cbidPacket);
	void cbGuildCreate(XDatabase *pDB, XArchive& arParam, ID idKey, ID cbidpacket);
	void cbGuildDestroy(XDatabase *pDB, ID idaccount, ID GuildIndex, ID idKey, ID cbidpacket);
	void cbGuildUpdate(XDatabase *pDB, ID Guildindex, ID GuildUpdateflag, XArchive& arParam, ID idAcc, ID idKey, ID cbidpacket);
	void cbUserGuildInfoUpdate(XDatabase *pDB, ID idAct, ID idTarget, ID Guildindex, ID GuildGrade, XArchive& arParam, ID idKey, ID cbidpacket);
//	void cbGuildAcceptProcess(XDatabase *pDB, ID acceptidaccount, ID targetidaccount, ID GuildIndex, ID Guildgrade, ID guildacceptflag, ID idPacket);
	void cbAccountJoinProcessGuildInfo(XDatabase *pDB, ID acceptidaccount, ID targetidaccount, ID GuildIndex, ID guildacceptflag, ID idKey, ID idPacket);
	int RecvGuildUpdate(XPacket& p, ID idKey);
	int RecvGuildUpdateOption(XPacket& p, ID idKey);
	int RecvGuildUpdateContext(XPacket& p, ID idKey);
	int RecvGuildUpdateMember(XPacket& p, ID idKey);
	void cbGuildUpdateOption(XPacket& p, ID idKey);
	void cbGuildUpdateContext(XPacket& p, ID idKey);
	void cbGuildUpdateMember(XPacket& p, ID idKey);
	int RecvGuildJoinReq(XPacket& p, ID idKey);
	void cbGuildJoinReq(XDatabase *pDB, ID idAcc, ID Guildindex, BOOL bAdd, ID idKey, ID cbidpacket);
	int RecvGuildJoinAccept(XPacket& p, ID idKey);
	void cbGuildJoinAccept(XDatabase *pDB, ID Guildindex, XArchive& arParam, ID idAcc, ID idKey, ID cbidpacket);
	int RecvGuildOut(XPacket& p, ID idKey);
	void cbGuildOut(XDatabase *pDB, ID Guildindex, XArchive& arParam, ID idAcc, ID idKey, ID cbidpacket);
	int RecvGuildKick(XPacket& p, ID idKey);
	void cbGuildKick(XDatabase *pDB, ID Guildindex, XArchive& arParam, ID idAcc, ID idKey, ID cbidpacket);
	//Recv
	void RecvTradeGemCall(XPacket& p, ID idKey);
	void RecvShopGemCall(XPacket& p, ID idKey);

	void RecvResourceTmpGet(XPacket& p, ID idKey);
	void RecvResourceTmpUpdate(XPacket& p, ID idKey);
	void cbResourceTmpGet(XDatabase *pDB, ID idFinder, ID idTarget, ID Spot, ID idClientConnect, ID idKey, ID cbidpacket);
	void cbResourceTmpUpdate(XDatabase *pDB, ID idaccount, ID Spot, ID idClientConnect, XArchive& arParam, ID idKey, ID cbidpacket);


	void RecvUserLogAdd(XPacket& p, ID idKey);


	//CallBack		 	
	void cbUserLogAdd(XDatabase *pDB, ID idaccount, int logtype, _tstring strNickName, _tstring strLog);

	void cbAccountNickNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring strNickName, ID idpacket, ID idKey);
	void cbAccountNickNameChange(XDatabase *pDB, ID idaccount, _tstring strNickName, ID idpacket, ID idKey);
	void cbAccountNameDuplicateCheck(XDatabase *pDB, ID idaccount, _tstring strAccountName, ID idpacket, ID idKey);
	void cbAccountNameRegist(XDatabase *pDB, ID idaccount, _tstring strAccountName, _tstring strPassWord, ID idpacket, ID idKey);
	void cbAccountNameRegistCheck(XDatabase *pDB, ID idaccount, _tstring strAccountName, _tstring strPassWord, ID idpacket, ID idKey);


	void cbTradeGemCall(XDatabase *pDB, ID idaccount, int remaingem, ID idClientConnection, ID idPacket, ID idKey);
	void cbShopGemCall(XDatabase *pDB, ID idaccount, int remaingem, ID idClientConnection, ID idPacket, ID idKey);

	void SendSample(int param);
	void SendNotify( const _tstring& strMessage );
	void RecvDefenseInfo(XPacket& p, ID idKey);
	void cbDefenseInfo(XDatabase *pDB, XArchive& arParam);

	void RecvPushMsgRegist(XPacket& p, ID idKey);
	void RecvPushMsgUnRegist(XPacket& p, ID idKey);

	void cbPushMsgRegist(XDatabase *pDB, ID idaccount, int type1, int type2, int recvtime, _tstring strMessage);
	void cbPushMsgUnRegist(XDatabase *pDB, ID idaccount, int type1, int type2);
// 	void SendAccountDuplicateLogout();

#ifdef _INAPP_GOOGLE
	void RecvGoogleInappPayload(XPacket& p, ID idKey);
	void RecvGoogleInappBuyCash(XPacket& p, ID idKey);

	void cbGoogleInappPayload( XDatabase *pDB
													, ID idaccount
													, XGAME::xtPlatform platform
													, XGAME::xtCashType typeProduct
													, const _tstring& idsProduct
													, ID idConnect
													, ID idKey
													, ID idPacket);
// 	void cbGoogleInappBuyCash(XDatabase *pDB, ID idaccount, _tstring strProductID, _tstring strPayload, _tstring strReceipt, ID idconnect, ID idKey, ID idPacket);
	void cbGoogleInappBuyCash(XDatabase *pDB
													, ID idaccount
													, const XGAME::xInApp& inapp
													, const _tstring& strPayload
													, const std::string& strcPublicKey
													, ID idconnect, ID idKey, ID idPacket);
		void RecvDeletePayload( XPacket& p, ID idKey );
		void cbDeletePayload(XDatabase *pDB
											, ID idAcc
											, const _tstring& idsProduct
											, const _tstring& strPayload
											, ID idKey );
#endif // INAPP_GOOGLE
#ifdef _INAPP_APPLE
	void RecvAppleInappBuyCash(XPacket& p, ID idKey);
	void cbAppleInappBuyCash(XDatabase *pDB, ID idaccount, _tstring strSKU, _tstring strReceipt, _tstring strPurchaseInfo);
	void OnReqFromAppleVerify(const char *cResponse, xHTTP::xREQ *pReq);
#endif

private:
	void RecvReqRegisterFacebook( XPacket& p, ID idKey );
	void cbRegisterFacebook(XDatabase *pDB
												, const ID idAcc
												, const _tstring& strFbUserId
												, const _tstring& strFbUsername
												, ID idKey );
	void RecvReqHelloMsgByidAcc( XPacket& p, ID idKey );
	void cbReqHelloMsgByidAcc(XDatabase *pDB
													, const ID idAcc
													, const ID idSpot
													, const ID idKey );
	void RecvReqGCMRegistId( XPacket& p, ID idKey );
	void cbGCMRegistId(XDatabase *pDB
										, const ID idAcc
										, const _tstring& strRegistId
										, const _tstring& strPlatform
										, ID idKey );
	void SendGuildJoinReq( ID idAcc
												, ID idGuild
												, XGAME::xtGuildError errCode
												, ID idKey );
	std::shared_ptr<XGameSvrConnection> GetThis() {
		return std::static_pointer_cast<XGameSvrConnection>( XEWinConnectionInServer::GetThis() );
	}
	void cbGuildUpdateJoinAcceptAndBroadcast( XDatabase* pDB, ID idGuild );
	void cbGuildUpdateJoinReqListAndBroadcast( XDatabase* pDB, ID idGuild );
	void cbGuildAcceptProcess( XDatabase *pDB
													, ID idAccMaster
													, ID idAccTarget
													, ID idGuild
													, xtGuildGrade gradeMember
													, xtGuildAcceptFlag acceptFlag );
// 	void cbRemoveidGuildInJoinReqListByAcc( XDatabase* pDB
// 																				, ID idAcc
// 																				, ID idGuildRemove
// 																				, XGAME::xtGuildGrade gradeMember );
};

#pragma warning ( default : 4250 )

