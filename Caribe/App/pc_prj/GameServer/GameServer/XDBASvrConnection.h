#pragma once
#include "XFramework/server/XSocketClientInServer.h"
// #pragma warning( disable : 4005 )	//  warning C4005: 'UINT8_MAX' : 매크로 재정의
// #include <boost/bind.hpp>
// #include <boost/function.hpp>
// #pragma warning( default : 4005 )	//  warning C4005: 'UINT8_MAX' : 매크로 재정의
#include "XGameUser.h"

class XRanking;
class XAhgoUser;
class XSAccount;
class XEWinConnectionInServer;
class XDBASvrConnection;

/**
 @brief 게임서버<->DBA서버용 클라이언트소켓
*/
class XDBASvrConnection : public XESocketClientInServer
{
public:
	static std::shared_ptr<XDBASvrConnection>& sGet();
	static void sDestroyInstance();
	static XLockCS& sGetLock() { return s_Lock; }
private:
	struct xCALLBACK {
		ID idClientConnect;
		std::function<void(XGameUser*, XPacket&)> callbackFunc;
		xCALLBACK() {
			idClientConnect = 0;
		}
	};
	// snKey, callback
	std::map<ID,xCALLBACK> m_mapCallback;
// 	template<typename F>
// 	void AddRecv( ID idPacket, F func ) {
// 	}
	BOOL CallRecvCallback( ID idKey, XPacket& p );
private:
	static std::shared_ptr<XDBASvrConnection> s_spInstance;
	static XLockCS s_Lock;
	XList<XPacketMem*> m_listPacketToLogin;	// 로그인서버가 죽어서 못보내고있는 패킷뭉치들.
	void Init() {}
	void Destroy() {}
public:
// 	XDBASvrConnection( const char *cIP
// 									, unsigned short port
// 									, XNetworkDelegate *pDelegate
// 									, DWORD param=0 );
	XDBASvrConnection();
	virtual ~XDBASvrConnection() { Destroy(); }
	//
//	void CreateRanking();
	bool OnCreate( const std::string& strcIP
							, WORD port
							, XNetworkDelegate *pDelegate
							, DWORD param = 0 );
	template<typename F>
	ID AddResponse( ID idKey, /*ID idPacket, */XGameUser *pOwner, F func ) {
		std::map<ID,xCALLBACK>::iterator itor;
		itor = m_mapCallback.find( idKey );
		if( XASSERT(itor == m_mapCallback.end()) ) {
			xCALLBACK callback;
			callback.idClientConnect = pOwner->GetidConnect();
			callback.callbackFunc = std::bind( func, std::placeholders::_1, std::placeholders::_2 );
			m_mapCallback[ idKey ] = callback;
		}
		return idKey;
	}
	void FlushSendPacket( void );
	BOOL GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes );
	//
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnLogined( void );
	virtual void OnDisconnect( void );
private:
	// private으로 바뀜. 패킷전송루틴은 반드시 이 클래스 내에서 만들것.
	virtual BOOL Send( XPacket& ar );
public:
	//Send
	void SendDidFinishLoad( void );	
	void SendUserLog(ID idAccount, _tstring strUserName, int Logtype, _tstring strLog);
	
	void SendPushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff);
	void SendPushMsgUnRegist(ID idacc, int type1, ID type2);

	//Recv
	void RecvSuccessRegisterAccount( XPacket& p );
	void RecvFailedRegisterAccount( XPacket& p );
	void RecvLoginAccountInfo( XPacket& p );
	
	void RecvLoginAccountInfoLoad ( XPacket& p );	
	void RecvNewAccountForUUID( XPacket& p );	
	ID SendUpdateAccountLogout( ID idConnectNew/*, ID idConnectAlready*/, ID idAcc );
	void RecvAccountDuplicateProcess(XPacket& p);
#ifdef _xCONNECT_DUPLICATE
	void RecvAlreadyConnectUser ( XPacket& p);
#endif
	void SendAppleVerify( ID idAcc, LPCTSTR szSku, LPCTSTR szReceipt, LPCTSTR szPurchaseInfo );
	ID SendGetUserLegionByidAccount( ID idSpot, ID idAccount, ID idClientConnection, ID idPacket );
	ID SendGetUserLegionByScore( int scoreMin, int scoreMax, int idxLegion, ID idFinder, XArchive& arRequest, ID idPacket );
//	void RecvGetUserLegionByScore( XPacket& p );
	void RecvGetUserLegionByIdAcc( XPacket& p );
//	void SendReqDoMatchingByScore( ID idSpot, int scoreMin, int scoreMax, ID idClientConnection );
//	void RecvDoMatchingByScore( XPacket& p );
	void SendReqGetWoodIron( ID idSpot, ID idOwner, ID idConnect, ID idPacket );
	void SendAddWoodIron( ID idAccount, int addWood, int addIron );
	void RecvGetWoodIron( XPacket& p );
	ID SendReqGetUserByPower( ID idFinder, ID idSpot, int scoreMin, int scoreMax, xSec secAdjustOffline, ID idClientConnection,ID idPacket );
	void RecvGetUserByScore( XPacket& p );
	ID SendReqMatchJewelMine( int idxMine, ID idFinder, int scoreMin, int scoreMax, ID idExclude, bool bSimul, bool bSimulWin, ID idClientConnect );
	void RecvReqMatchJewelMine( XPacket& p );
	ID SendGetJewelLegionByidAcc( int idxMine, ID idSpot, ID idAccount, ID idClientConnection, ID idPacket );
	void RecvGetJewelLegionByIdAcc( XPacket& p );
//	void SendChangeJewelOwn( int idxMine, ID idAccount, ID idNewOwner, LPCTSTR szName, int levelMine, int defense, DWORD secLastEvent );
	void SendChangeJewelOwn( ID idDBAcc, const XGAME::xJewelInfo& info );
//	void SendAddDefenseByJewel( int idxMine, ID idAcc, int def );
	void SendAddDefenseByJewel( XSpotJewel *pSpot );
#ifdef _DEV
	void SendUpdateJewelMineInfo( ID idDBAcc, ID idSender, XSpotJewel *pSpot );
#endif // _DEV
	ID SendReqDefeatInfo( ID idDefender, ID idAttacker, XGAME::xtSpot type, int powerGrade );
	ID SendReqLoginLockForBattle( ID idAttacker, ID idDefender, ID idSpot, ID snSpot, XGAME::xtSpot type, DWORD param1 = 0, DWORD param2 = 0 );
	void SendUnlockLoginForBattle( ID idDefender, ID idAttacker );
	void RecvLoginLockForBattle( XPacket& p );
	void RecvUnlockLoginForBattle( XPacket& p );
	void SendLoginAccountInfoLoad( bool bSessionKeyLogin, ID idConnect, ID idAcc );
	ID SendReqCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, ID idSpot, xCampaign::CampObjPtr spCampObj );
	ID SendReqEnterGuildRaid( ID idAcc, ID idGuild, ID idCamp, ID idSpot, LPCTSTR szName, int level, int power );
	void SendReqLeaveGuildRaid( ID idAcc, ID idGuild, int lvAcc, ID idCamp, ID idSpot, XGAME::xtWin typeWin, const XArchive& arLegionSimple );
	ID SendReqGuildRaidOpen( ID idAcc, ID idGuild, ID idCamp, ID idSpot, XArchive& arCamp );
  // xuzhu end
	void SendSaveAccountInfo( XSPSAcc spAcc, BOOL bReq, 
		ID idConnectClient,	// 로긴서버로부터온 클라이언트 커넥션 아이디
		ID idReqPacket=0, int cnt = 0 );
	void RecvSaveAccountInfo( XPacket& p );
	void RecvRankInfo(XPacket& p );
	void RecvGuildInfoAll(XPacket& p);
	void RecvGuildInfoUpdate(XPacket& p);
	void RecvGuildUpdateCampaign( XPacket& p );
//	void RecvGuildJoinAccept(XPacket& p);
	void RecvGuildAcceptResult(XPacket& p);
	void RecvGuildEvent( XPacket& p );
	//void RecvGuildResult(XPacket& p);
//	void RecvGuildKickResult(XPacket& p);
	void RecvNotify(XPacket& p);
	
	ID SendGuildUpdateMember( XSPSAcc spAcc, ID idAccTarget, XGAME::xtGuildGrade grade );
	ID SendUpdateLogin(ID idaccount, ID GameServerID, const _tstring& lastconnectip );

	ID SendTradeGemCall(ID idaccount, int remaingem, ID idClientConnection, ID idPacket);
	ID SendShopGemCall(ID idaccount, int remaingem, ID idClientConnection, ID idPacket);

	void RecvGuildUserEvent( XPacket& p );
	ID SendAccountNickNameDuplicateCheck( ID idaccount, _tstring strAccountNickName, ID idPacket );
	ID SendAccountNameDuplicateCheck(ID idaccount, _tstring strAccountName, ID idPacket);
	ID SendAccountNameRegist(ID idaccount, _tstring strAccountName, _tstring strPassword, ID idPacket);
	ID SendAccountNameRegistCheck(ID idaccount, _tstring strAccountName, _tstring strPassword, ID idPacket);
	ID SendAccountNickNameChange(ID idaccount, _tstring strAccountNickName, ID idPacket);

	ID SendGuildCreate(XSPSAcc spAcc, const _tstring& strCreateGuildName, const _tstring& strGuildContext, ID idPacket);
	ID SendGuildJoinReq(XSPSAcc spAcc, ID guildid, ID idPacket);	
	ID SendGuildJoinAccept(ID idaccount, ID reqidaccount, int flag, ID guildid, ID idPacket);
	ID SendGuildJoinAutoAccept(ID reqidaccount, ID guildid, ID idPacket);
	ID SendGuildOut(ID idaccount, ID guildid, ID idPacket);
	ID SendGuildKick(ID idaccount, ID reqidaccount, ID guildid, ID idPacket);
	
	ID SendGuildUpdateContext(XSPSAcc spAcc, _tstring strguildcontext, ID idPacket);
	ID SendGuildUpdateOption(XSPSAcc spAcc, ID bAutoAccept, ID bBlockReq, ID idPacket);
	void GuildUpdateContext(XPacket& p);
	void GuildUpdateOption(XPacket& p);

	void SendCashInfoUpdate(ID idAccount, int remaincash);
	void SendShopList(XSPSAcc spAcc );
		
	ID SendPostInfoRequest( XSPSAcc spAcc );
	void RecvPostInfo(XPacket& p);
	
//	void SendPOSTInfoUpdate(XSPSAcc spAcc);	
//	ID SendPOSTInfoOneUpdate(XSPSAcc spAcc, ID Postsn, ID cbIdPacket);

	void SendPOSTInfoAddidaccount(ID Receiveuseridaccount, XPostInfo* pData);
	void SendPOSTInfoAddusername(_tstring strRecvName, XPostInfo* pData);
//	void SendPOSTInfoAdd(XPostInfo* pData);
//	ID SendPOSTInfoAdd(ID idaccount, ID postsn, ID posttype, XArchive& p, ID cbidpacket);
	//void SendPostDestroy(ID idaccount, ID postindex);
//	ID  SendPostRemoveItems(ID idaccount, ID idPost, ID idPacket);
	ID  SendPostDestroy(ID idaccount, ID snPost/*, ID idPacket*/);
	void RecvSaveDummyAccount( XPacket& p );
	ID SendDefenseInfo( ID idAccount, XGAME::xBattleLog& log );

	void SendRequestRankInfo();
	//
//	ID SendGooglePayload(ID idaccount, XGAME::xtPlatform platform, XGAME::xtCashType typeProduct, const _tstring& strProductID, const _tstring& strSignature, const _tstring& strJsonReceipt, ID idClientConnection, ID idPacket);
	ID SendGooglePayload(ID idaccount, XGAME::xtPlatform platform, XGAME::xtCashType typeProduct, const _tstring& idsProduct, ID idClientConnection, ID idPacket);
//	ID SendGoogleBuyCashVerify(ID idaccount, int producttype, _tstring strProductID, _tstring strReceipt, ID idClientConnection, ID idPacket);	
	ID SendGoogleBuyCashVerify(ID idaccount, const XGAME::xInApp& inapp, ID idClientConnection, ID idPacket);
	ID SendAppleBuyCashVerify(ID idaccount, _tstring strProductID, _tstring strReceipt, _tstring strPurchaseInfo, ID idClientConnection, ID idPacket);
	ID SendDeletePayload( ID idAcc
											, const _tstring& idsProduct
											, const _tstring& strPayload );
	//
	ID SendReqGetMandrakeLegionByIdAcc( int idxMandrake
																		, ID snSpot
																		, ID idOwner
																		, ID idPacketWith );
	ID SendEncounterWin( ID idPlayer, ID idEncounterUser, LPCTSTR szEncounter, int numSulfur, ID idSpot );
	void SendChangeMandrakeOwn( int idxMandrake, ID idAcc, ID idNewOwner, ID idEnemy, LPCTSTR szName, int win, int reward, int idxLegion, int addOffWin );
	ID SendReqMatchMandrake( ID idAcc, int idxMandrake, int powerMin, int powerMax, ID snSpot );
	ID SendReqRegisterFacebook( ID idAcc, const _tstring& strFbUserId, const _tstring& strFbUsername );
	ID SendReqHelloMsgByidAcc( ID idAcc, ID idSpot );
	ID SendReqGCMRegistId( ID idAcc, const _tstring& strRegistId, const _tstring& strPlatform );
	ID SendPOSTInfoAdd( ID idAcc, XGAME::xtPostType postType, const XPostInfo& postInfo );
	ID SendRemoveItemsOnlyAtPost( ID idAcc, ID snPost );
	void SendReload( const std::string& tag );
	//
};

