#pragma once
#include "XDBUAccount.h"

namespace XGAME {
	struct xInApp;
};

#define VER_DB		2

class XDBUAccount;
class XPacket;

#ifdef _xCONTENTS_SERVER_EVENT
class XServerEventMng;
#endif

class CUserDB;

class XDatabase
{
	void Init() { 
		m_pDB = NULL;
	}
	void Destroy();
public:
	XDatabase();
	virtual ~XDatabase() { Destroy(); }
	//
	GET_ACCESSOR( CUserDB*, pDB );
	//
//	BOOL FindAccountByUUID( XSPDBUAcc spAcc, LPCTSTR szUUID );
//	BOOL CreateNewAccount(XSPDBUAcc spAcc, const _tstring& strNickname, const _tstring& strUUID);

	BOOL AccountNickNameRegist(ID idaccount, _tstring strAccountNickName);
	bool AccountNickNameDuplicateCheck( const _tstring& strAccountNickName);
	
	BOOL AccountNameDuplicateCheck(_tstring strAccountName);
	BOOL AccountNameRegist(ID idAccount, _tstring strAccountName, _tstring strPassword);
	BOOL AccountNameRegistCheck(_tstring strAccountName, _tstring strPassword);

	BOOL SaveAccount( ID idAccount, XSPDBUAcc spAcc );
	BOOL UpdateAccountCashInfo(ID idaccount, int remaincash);
//	BOOL SaveAccountLoginSave(ID idAccount, ID gameserverid, _tstring strConnectIP);
	BOOL SaveAccountLogOut(ID idAccount);
	void SaveAccountShopInfo(ID idaccount, XSPDBUAcc spAcc);
//	BOOL UpdatePostInfo(XSPDBUAcc spAcc);
//	BOOL CreateNewAccountIDPW( XSPDBUAcc spAcc, LPCTSTR szID , LPCTSTR szPW);
//	BOOL CreateNewAccountLyto (  XSPDBUAcc spAcc, LPCTSTR szDeviceID , LPCTSTR szSessionToken, int SessionMemberID );
	BOOL CreateNewAccountFaceBook ( XSPDBUAcc spAcc, LPCTSTR szUserid , LPCTSTR szUsername, LPCTSTR szNickname);	
//	BOOL FindAccountIDName(XSPDBUAcc spAcc, _tstring strID);
	BOOL FindAccountSessionID(XSPDBUAcc spAcc, _tstring strID, _tstring strSessionKey);
	BOOL FindAccountSessionUUID(XSPDBUAcc spAcc, _tstring strID, _tstring strSessionKey);
//	BOOL FindAccountFromIdAccount( XSPDBUAcc spAcc, ID idAccount);


	BOOL FindDuflicateVerify( ID idaccount, LPCTSTR szJsonReceiptFromClient);
	BOOL FindAccountFromFaceBook ( XSPDBUAcc spAcc, LPCTSTR szUserID, LPCTSTR szUserName);
//	BOOL FindUserByPower( XSPDBUAcc pAccOut, int scoreMin, int scoreMax, ID idExclude, XArchive& ar );
	BOOL GetLegionByIdAcc( int idxLegion, ID idAcc, XArchive& arOut );	
	
//	BOOL RegistFBAccount( ID idAccount, LPCTSTR szFBuserid, LPCTSTR szFBusername, LPCTSTR szNickname );

	BOOL LoadPostInfoByIdAcc(ID idaccount, XArchive& ar);
//	BOOL AddPostInfo(ID idaccount, ID postsn, ID PostType, _tstring SenderName, _tstring RecvName, _tstring Title, _tstring Message, int nCount, XArchive& ar, ID rewardtableid);
	BOOL DeletePostInfo(ID idaccount, ID postsn);
	BOOL RemovePostItems(ID idaccount, ID postsn);
	

	void SaveAccountGold( ID idaccount, DWORD Gold, DWORD RewardGold );
	void AddUserLog( ID idAccount, int type, _tstring  idname , _tstring Log );
	BOOL FindJewelMatch( ID idFinder, int idxMine, int scoreMin, int scoreMax, XArchive& ar );
	BOOL FindJewelInfoByIdAcc( ID idAccount, int idxMine, XArchive& arPacket );
	BOOL ChangeOwnerJewelMine( int idxMine, ID idOldOwner, ID idNewOwner, _tstring& strName, int levelMine, int defense, DWORD secLastEvent );

	BOOL AccountLoadResourceTmp(ID idAccount, XArchive& arPacket);
	BOOL AccountUpdateResourcetmp(ID idAccount, XArchive& arPacket);

	BOOL LoadRankingList(XArchive& arResult);
	BOOL LoadGuildInfo(XArchive& arResult);

	BOOL TradeGemCall(ID idaccount, int remaingem);
	BOOL ShopGemCall(ID idaccount, int remaingem);

	BOOL GuildNameDuplicateCheck(_tstring strGuildName);
	BOOL CreateNewGuild(_tstring strGuildName, _tstring strGuildContext, _tstring strUserName, int Ver, ID idAccount, ID& GuildIndex);		//CREATE Guild

//	BOOL DeleteGuild(ID Guildindex);
//	BOOL UpdateGuildMembers(ID Guildindex, const XArchive& ar);
//	BOOL UpdateGuildMemberwithMaster(ID Guildindex, ID newmasteridacc, _tstring newmasterName, const XArchive& ar);
	BOOL UpdateGuildJoinReq(ID Guildindex, const XArchive& ar);
	BOOL UpdateGuildJoinAccept(ID Guildindex, XArchive& ar);
	BOOL UpdateGuildContext(ID Guildindex, _tstring strContext);
	BOOL UpdateGuildOption(ID Guildindex, const XArchive& ar);
//	BOOL UpdateGuildJoinReqListByAcc(ID idaccount, ID GuildIndex, ID GuildGrade, const XArchive& arResult);
//	BOOL LoadGuildJoinReqListByAcc(ID idaccount, XArchive* pOut);	
//	BOOL AccountLoadGuildInfo(ID idaccount, ID& GuildIndex, int& guildgrade, XArchive& arOut);

	BOOL CreateGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion);
	BOOL SelectGuildRaidInfo( ID idaccount, ID GuildID, int raidindex, int spotid, int vesion );
	BOOL SelectGuildRaidEmptyInfo(ID GuildID);
	BOOL DeleteGuildRaidInfo(ID GuildID, int raidindex, int spotid);
	BOOL DeleteGuildRaidAll(ID GuildID);
	BOOL UpdateGuildRaid(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);
	BOOL UpdateGuildRaidLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);
	BOOL UpdateGuildRaidUnLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);

	BOOL RegistAccountDeviceID(ID idacc, _tstring strDeviceID, _tstring strAppstore);

	void PushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff);
	void PushMsgUnRegist(ID idacc, int type1, ID type2);

#ifdef _INAPP_GOOGLE
// 	BYTE CreateGooglePayload(ID idacc, int priductType, _tstring strProductID, _tstring& strPayload);
//  XGAME::xtErrorIAP CreateGooglePayload(ID idacc, XGAME::xtCashType typeProduct, const _tstring& strProductID, _tstring* pOutstrPayload);
	XGAME::xtErrorIAP CreateGooglePayload(ID idacc, XGAME::xtCashType typeProduct, const _tstring& strProductID, const _tstring& strPayload);
// 	BYTE InAppGoogleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt);
//	XGAME::xtErrorIAP LoadGooglePayload( ID idacc, const _tstring& strPayload );
#endif
#ifdef _INAPP_APPLE	
	BYTE InappAppleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt);
#endif

	BOOL UserLogAdd(ID idaccount, int logtype, _tstring strNickName, _tstring strLog);
private:
	CUserDB*		m_pDB;
};