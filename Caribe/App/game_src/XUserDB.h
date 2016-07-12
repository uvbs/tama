#pragma once

#include "XDatabaseODBC.h"
#include "XDBUAccount.h"
#include "XPacketGDB.h"
#include "XStruct.h"


class CDataBaseODBC;
class XPostInfo;
struct ArchiveSplit;
#define LEN_LOGIN_ID		40
#define LEN_POST_TITLE		100
#define LEN_POST_MSG		255
#define LEN_POST_NAME		100
#define LEN_PAYLOAD			100


class CUserDB : public CDataBaseODBC
{
public:
	CUserDB(void);
	~CUserDB(void);

	virtual void OutputTextFormat( const TCHAR* szFormat, ... );
	
	//구현된거
	ID			CreateNewAccount( const _tstring& strNickName, const _tstring& strUUID );		//CREATE
	xtDB		AccountLoadIDAccount ( XSPDBUAcc spAcc, ID idAccount);			//SELECT	
	BOOL		AccountUpdateAll( XSPDBUAcc spAcc );								//UPDATE	
	
	BOOL		UpdateAccountLogin(ID idAccount, ID gameserverid, _tstring strConnectIP);
	BOOL		UpdateAccountLogOut(ID idAccount);

	bool		AccountNickNameDuplicateCheck( const _tstring& strAccountNickName);
	BOOL		AccountNickNameRegist(ID idaccount, _tstring NickName);

	BOOL		AccountNameDuplicateCheck(_tstring strAccountName);
	BOOL		AccountNameDuplicate(_tstring strAccountName);
	BOOL		AccountNameRegist(ID idAccount, _tstring strAccountName, _tstring strPassword);
	BOOL		AccountNameRegistCheck(_tstring strAccountName, _tstring strPassword);
	
// 	BOOL		AccountLoadUUID(LPCTSTR tUUI, XSPDBUAcc pOutAcc );
	ID			FindAccountByUUID( LPCTSTR tUUID, XArchive *pOutAr );
// 	BOOL		AccountLoadIDName(XSPDBUAcc spAcc, _tstring strID);
	ID			FindAccountByID( const _tstring& strID, XArchive *pOutAr );
	ID			FindAccountByFacebook( const _tstring& strFbUserId, XArchive *pOutAr );

	BOOL		AccountLoadSessionUUID(XSPDBUAcc spAcc, _tstring strUUID, _tstring strSessionKey);
	BOOL		AccountLoadSessionIDName(XSPDBUAcc spAcc, _tstring strIDName, _tstring strSessionKey);


	BOOL		FindAccountPowerValue( XDBUAccount* pAccOut, int MinValue, int MaxValue, ID Finderidaccount, XArchive& ar );
	BOOL		LegionLoadIDAccount ( ID idAccount, int idxLegion, XArchive& arOut);					//SELECT
	BOOL		LoadResourceWorldByIdAcc( ID idAcc, XArchive& arOut );
	BOOL		SaveResourceWorldByIdAcc( ID idAcc, XArchive& ar );
	
	BOOL		FindAccountMineEnemyZero ( XSPDBUAcc spAcc, ID idAccount, int num, int MinValue, int MaxValue, XArchive& ar);
	bool		FindAccountMineEnemyZero2(ID idFinder
																	, int idxMine
																	, int powerMin
																	, int powerMax
																	, bool bSimul
																	, XGAME::xJewelMatchEnemy *pOut );
	BOOL		FindJewelInfoByIdAcc ( ID idAccount, int jewelminenum,  XArchive& ar);
	BOOL		ChangeOwnerJewelMine ( ID idAccount, int jewelminenum, int newOwner, _tstring& strName, int idenemy, int minelevel, int minedefense, int seclastevent);
	BOOL		ChangeOwnerJewelMine2(ID idDBAcc, XGAME::xJewelInfo& info, int secUpdate );
	BOOL		ChangeOwnerMandrakeMine( ID idAccount, int idxMandrake, ID idNewOwner, ID idEnemy, _tstring& strName, int win, int reward, int idxLegion, int addOffWin );
	BOOL		FindAccountLegionByPower( int scoreMin, int scoreMax, int idxLegion, ID idFinder, XArchive& arOut );
	BOOL		FindMandrakeInfoByIdAcc( ID idAccount, int idxMandrake, XArchive& arOut );
	BOOL		FindMatchMandrake( ID idFinder, int idxMandrake, int scoreMin, int scoreMax, XArchive& arOut );
	

//	BOOL		AddPostInfo(ID idaccount, ID postsn, ID PostType, _tstring SenderName, _tstring RecvName, _tstring Title, _tstring message, int nCount, XArchive& ariteminfo, ID rewardtableid);
	// 발신ID, 발송자이름, 수신자Name, 메일 타입, 
	BOOL		LoadPostInfoByIdAcc(ID idaccount, XArchive& arOut);
	BOOL		LoadPostItemsInfo(ID postindex, ID idaccount, XArchive& arOut);
// 	BOOL		UpdatePost(XSPDBUAcc spAcc);
//	BOOL		UpdatePost(ID idaccount, ID index, short status);

	BOOL		DeletePostInfo(ID idaccount, ID postsn);
	BOOL		RemovePostItems(ID idaccount, ID postsn);

//	BOOL		UpdatePostItems(ID idaccount, ID index, int itemid, short itempos, short itemstatus);
		
	BOOL		SaveShopInfo(ID idaccount, XSPDBUAcc spAcc);
	BOOL		TradeGemCall(ID idaccount, int remaingem);
	BOOL		ShopGemCall(ID idaccount, int remaingem);

	BOOL		LoadServerRanking( XArchive& arOut);
	XGAME::xtGuildError DeleteGuild(ID idGuild);
	BOOL		LoadGuildInfo(XArchive& arOut);
	BOOL		CreateNewGuild(_tstring strGuildName, _tstring strGuildContext, _tstring strUserName, int Ver, ID idAccount, ID& GuildIndex);		//CREATE Guild
	BOOL CreateNewGuild( const _tstring& strName
										, const _tstring& strContext
										, const _tstring& strUserName
										, int Ver
										, ID idAcc
										, ID* pOutidGuild );
	BOOL		GuildNameDuplicateCheck(_tstring strGuildName);
//	BOOL		UpdateAccountGuildJoinReq(ID idaccount, ID GuildIndex, ID GuildGrade, const XArchive& arResult);
	XGAME::xtGuildError UpdateGuildJoinReqListByAcc(ID idAcc
																	, ID idGuild
																	, XGAME::xtGuildGrade _gradeMember
																	, const XArchive& arResult);
	XGAME::xtGuildError LoadGuildJoinReqListByAcc(ID idaccount, XArchive* pOut );
	BOOL		UpdateAccountCashInfo(ID idaccount, int remaincash);
	//BOOL		AccountLoadGuildInfo(ID idaccount, ID& GuildIndex);
	XGAME::xtGuildError LoadGuildInfoByAcc( ID idAcc, ID* pOutidGuild, XGAME::xtGuildGrade* pOutGrade, XArchive* pOutReqList );

	BOOL		CreateGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion);
	BOOL		SelectGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion);
	BOOL		SelectGuildRaidEmptyInfo(ID GuildID);
	BOOL		DeleteGuildRaidInfo(ID GuildID, int raidindex, int spotid);
	BOOL		DeleteGuildRaidAll(ID GuildID);
	BOOL		UpdateGuildRaid(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);
	BOOL		UpdateGuildRaidLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);
	BOOL		UpdateGuildRaidUnLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar);

	BOOL		RegistAccountDeviceID(ID idaccount, _tstring strDeviceID, _tstring strAppstore);
	
	void		PushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff);
	void		PushMsgUnRegist(ID idacc, int type1, ID type2);

	BOOL		SelectTargetbattlestatus(ID idDefender, ID& connectserverid, ID& battletargetid, ID& battletime);
	BOOL		AccountUpdateBattleInfo(ID idDefender, ID idAttacker, int );
	//구현 안된거
	

	BOOL		CreateNewAccountIDPW( TCHAR* szID,TCHAR* szPW,  int& idaccount );	
	BOOL		CreateNewAccountFacebook( TCHAR* szuserid, TCHAR* szusername,  TCHAR* sznickname,  int& idaccount );
	
//	BOOL		RegistFBAccount( ID idAccount, TCHAR* pFBuserID, TCHAR* pFBUserName, TCHAR* szNickname );
	bool		RegistFBAccount(ID idAcc, const _tstring& strFbUserId, const _tstring& strFbUsername );
	BOOL		AccountLoadFBUserID( XSPDBUAcc spAcc, LPCTSTR FBUserID);

	BOOL		AccountLoadDataExist	( XSPDBUAcc spAcc );
//	BOOL		AccountLoadDataAll	( XSPDBUAcc spAcc );		//순서대로 읽기
	BOOL		AccountUpdateData		( XSPDBUAcc spAcc );		//순서대로 저장
//	void		AccountUpdateGoldData( ID idaccount, DWORD Gold, DWORD rewardgold );
	//BOOL		AccountExistFind	( TCHAR* pIDName );				// 계정 ID있는지 확인.
	
	void		AddDuplicateVerify( ID idaccount, TCHAR* szJsonReceiptFromClient);
	BOOL		FindDuplicateVerify( ID idaccount, TCHAR* szJsonReceiptFromClient);

	BOOL		AccountSessionInfo( ID idaccount, TCHAR* SessionMemberID, TCHAR* SessionToken );
	BOOL		LoadDefenseLogByIdAcc( ID idAcc, XArchive& arOut );
	BOOL		SaveDefenseLogByIdAcc( ID idAcc, const XArchive& ar );
		
	XGAME::xtGuildError UpdateGuildMembers( XGuild* pGuild );
	XGAME::xtGuildError UpdateGuildMemberWithMaster( XGuild* pGuild
																								, ID idAccNewMaster
																								, const _tstring& strNewNameByMaster );
	BOOL		UpdateGuildJoinReqListByGuild( ID Guildindex, const XArchive& ar );
	XGAME::xtGuildError UpdateGuildJoinAccept( ID idGuild, const XArchive& arMember, const XArchive& arReqList );
// 	BOOL		UpdateGuildJoinAccept(ID Guildindex, XArchive& ar);
	BOOL		UpdateGuildContext(ID Guildindex, _tstring strContext);
	XGAME::xtGuildError UpdateGuildOption( XGuild* pGuild );
	//	BOOL		UpdateGuildOption(ID Guildindex, const XArchive& ar);

	BOOL		AccountLoadResourceTmp(ID idAccount, XArchive& arPacket);
	BOOL		AccountUpdateResourcetmp(ID idAccount, const XArchive& arPacket);
	

	bool LoadCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive* pOut );
	XGAME::xtGuildError CreateCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive& arCamp );
	XGAME::xtGuildError UpdateCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive& arCamp );
	bool EnterGuildRaid( ID idAcc, ID idGuild, ID idCamp, XGAME::xtGuildError *pOutErr, ID *pOutidAccExist );
	bool LeaveGuildRaid( ID idAcc, ID idGuild, ID idCamp, XGAME::xtGuildError *pOutErr );

	BOOL	AddUserLog( ID idAccount, int type, _tstring strNickName, _tstring pLog );
	int CombineSplitData( XArchive& arOut, ArchiveSplit *pAry, const int numAry, int verAcc ) const;
	template<size_t _Size>
	int CombineSplitData( XArchive& arOut, ArchiveSplit (&pAry)[_Size], int verAcc ) const {
		return CombineSplitData( arOut, pAry, _Size, verAcc );
	}
	void SplitArchive( ArchiveSplit *pAry, int numAry, XArchive& ar ) const;
	template<size_t _Size>
	void SplitArchive( ArchiveSplit (&pAry)[_Size], XArchive& ar ) const {
		SplitArchive( pAry, _Size, ar );
	}
	void SplitBuffWithCompress( ArchiveSplit *pAry
								, int numAry
								, BYTE* pSrcCompressed
								, int bytesSrcCompressed
								, int bytesSrcUnCompressed ) const;
	bool WorldToSplit( XSPAcc pAcc, ArchiveSplit *pOut, int numAry );
	bool WorldToSplit( XArchive& arWorld, ArchiveSplit *pOut, int numAry );
	void ItemsToSplit( XSPAcc pAcc, ArchiveSplit *pOut, int numAry );
#ifdef _INAPP_GOOGLE
// 	BYTE CreateGooglePayload(ID idacc, int priductType, _tstring strProductID, _tstring& strPayload);
	XGAME::xtErrorIAP CreateGooglePayload( ID idacc, XGAME::xtCashType typeProduct, const _tstring& strProductID, const _tstring& strPayload );
//	BYTE InAppGoogleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt);
	XGAME::xtErrorIAP LoadGooglePayload(ID idacc, const _tstring& strPayload, _tstring* pOutidsProduct );
	XGAME::xtErrorIAP DeletePayload( ID idAcc, const _tstring& strPayload );
	
#endif
#ifdef _INAPP_APPLE	
	BYTE InappAppleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt);
#endif
	bool UpdateAccountHello( ID idacc, const _tstring& strHello ); //Hello 2016.01.20
	bool FindAccountHello( ID idAcc, _tstring *pOutStr );
	bool AddPostInfo( ID idAcc, XGAME::xtPostType type, const XPostInfo* pPostInfo );
};
