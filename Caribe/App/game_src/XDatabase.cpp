#include "stdafx.h"
#include "XFramework/XEProfile.h"
#include "XDBUAccount.h"
#include "XResObj.h"
//<<<<<<< HEAD
#include "XFramework/XEProfile.h"
#include "XUserDB.h"
#include "XDatabase.h"
// =======
// #include "XUserDB.h"
// #include "XFramework/XEProfile.h"
// >>>>>>> private_raid

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)

XDatabase::XDatabase() 
{ 
	Init(); 
	// 이곳에 초기화 코드를 넣습니다.

	m_pDB = new CUserDB;

	BOOL bConnect = m_pDB->ConnectDB(L"Caribe", L"Caribe", L"Mobile099!");

	//DB 못 붙으면 안켜지게 수정 해야 할꺼 같아요

	if( bConnect ) 
		XLOGXN("DB CONNECT SUCCESS");
	else
		XALERT("DB CONNECT FAILED ");		
}

void XDatabase::Destroy()
{
	// 이곳에 파괴 코드를 넣습니다.
	SAFE_DELETE( m_pDB );
}

// DB에서 cUUID를 가진 계정을 찾아서 그 계정데이타를 spAcc에 담아준다.
// BOOL XDatabase::FindAccountByUUID( XSPDBUAcc spAcc, LPCTSTR szUUID )
// {	
// 	return m_pDB->AccountLoadUUID( szUUID, spAcc );
// }

// cUUID로 새 계정을 만들고 그 정보를 spAcc에 담아준다. 
// BOOL XDatabase::CreateNewAccount( XSPDBUAcc spAcc
// 																, const _tstring& strNickname
// 																, _tstring strUUID )
// {
// 	// 새 계정을 만들어 DB에 넣는다.
// 	ID idaccount = 0;
// //#pragma message("spAcc->m_strName에 값이 있다면 계정생성할때 닉네임으로 넣어주세요. 없으면 원래대로 HunterXXX로...")
// 	if (m_pDB->CreateNewAccount(strNickname, strUUID, &idaccount)) {
// 		spAcc->m_idAccount = idaccount;
// 		spAcc->m_strUUID = strUUID;
// 		spAcc->m_strName = strNickname;
// 		return TRUE;
// 	}
// 	return FALSE;
// 	
// }

//BOOL XDatabase::CreateNewAccountIDPW(XSPDBUAcc spAcc, LPCTSTR szID , LPCTSTR szPW)
//{
//	int idaccount = 0;
//	if( m_pDB->CreateNewAccountIDPW( (TCHAR*)szID, (TCHAR*)szPW, idaccount) == TRUE )
//	{
//		spAcc->m_idAccount = idaccount;
//		spAcc->m_strID = szID;
//		return TRUE;
//	}
//	return FALSE;
//}

BOOL XDatabase::CreateNewAccountFaceBook ( XSPDBUAcc spAcc, LPCTSTR szUserid , LPCTSTR szUsername, LPCTSTR szNickname)
{
	int idaccount = 0;
	if( m_pDB->CreateNewAccountFacebook( (TCHAR*)szUserid, (TCHAR*)szUsername, (TCHAR*)szNickname, idaccount) == TRUE )
	{
		spAcc->m_idAccount = idaccount;
		spAcc->m_strID = szUserid;		
		return TRUE;
	}
	return FALSE;
}

// 영문 아이디 cID로 DB에서 계정을 찾아 그 정보를 spAcc에 담아준다. 
// BOOL XDatabase::FindAccountIDName(XSPDBUAcc spAcc, _tstring strID)
// {
// 	return m_pDB->AccountLoadIDName(spAcc, strID);
// }

BOOL XDatabase::FindAccountSessionID(XSPDBUAcc spAcc, _tstring strID, _tstring strSessionKey)
{
	return m_pDB->AccountLoadSessionIDName(spAcc, strID, strSessionKey);
}
BOOL XDatabase::FindAccountSessionUUID(XSPDBUAcc spAcc, _tstring strUUID, _tstring strSessionKey)
{
	return m_pDB->AccountLoadSessionUUID(spAcc, strUUID, strSessionKey);
}


/*
BOOL XDatabase::FindAccountFromIdAccount( XSPDBUAcc spAcc, ID idAccount) //계정 인증 통과 했을때 실제 DB를 읽어 온다.
{
	XPROF_OBJ_AUTO();
	return m_pDB->AccountLoadIDAccount( spAcc, idAccount);
}
*/


BOOL XDatabase::FindDuflicateVerify( ID idaccount, LPCTSTR szJsonReceiptFromClient )
{
	return m_pDB->FindDuplicateVerify( idaccount, (TCHAR*)szJsonReceiptFromClient);
}


BOOL XDatabase::FindAccountFromFaceBook ( XSPDBUAcc spAcc, LPCTSTR szUserID, LPCTSTR szUserName)
{
	XPROF_OBJ_AUTO();
	BOOL bRet = m_pDB->AccountLoadFBUserID( spAcc, szUserID);

	if( bRet )
	{

		bRet = m_pDB->AccountLoadDataExist(spAcc);
	}	
	return bRet;
}

/**
 scoreMin ~ Max범위의 유저를 찾아 기본정보를 pAccout에 담아돌려준다.
 idExclude는 검색시 제외시켜야할 아이디(보통 찾는자)
*/
// BOOL XDatabase::FindUserByPower( XSPDBUAcc pAccOut, int scoreMin, int scoreMax, ID idExclude, XArchive& ar )
// {
// 	BOOL bRet = m_pDB->FindAccountPowerValue( pAccOut, scoreMin, scoreMax, idExclude, ar );
// 	return bRet;
// }

BOOL XDatabase::GetLegionByIdAcc( int idxLegion, ID idAcc, XArchive& arOut )
{
	BOOL bRet = m_pDB->LegionLoadIDAccount( idAcc, idxLegion, arOut );
	return bRet;
}
// 게임서버로부터 idAccount계정을 DB에 저장하라고 보냈다. 계정데이타는 p에 있다.
// 아래에 XDBUAccount*파라메터 방식으로 바뀜
/*BOOL XDatabase::SaveAccount( ID idAccount, XPacket& p )
{
#ifdef _FILE_DB
	XSPDBUAcc pdbAccount = _FindAccountFromIDAccount( idAccount );
	XBREAK( pdbAccount == NULL );
	if( pdbAccount )
	{
		pdbAccount->RestorePacket( p );
		_SaveFileDB();
	}
#else
	XDBUAccount saveAccount;
	saveAccount.RestorePacket( p );

	m_pDB->AccountUpdateData( &saveAccount );	
#endif 
	return TRUE;
}*/

// 게임서버로부터 idAccount계정을 DB에 저장하라고 보냈다. 계정데이타는 spAcc에 있다.
BOOL XDatabase::SaveAccount( ID idAccount, XSPDBUAcc spAcc )
{
	XPROF_OBJ_AUTO();
	return m_pDB->AccountUpdateAll( spAcc );
}

BOOL XDatabase::UpdateAccountCashInfo(ID idaccount, int remaincash)
{
	return m_pDB->UpdateAccountCashInfo(idaccount, remaincash);
}
// BOOL XDatabase::SaveAccountLoginSave(ID idAccount, ID gameserverid, _tstring strConnectIP)
// {
// 	return m_pDB->UpdateAccountLogin(idAccount, gameserverid, strConnectIP);
// }

BOOL XDatabase::SaveAccountLogOut(ID idAccount)
{
	return m_pDB->UpdateAccountLogOut(idAccount);
}

BOOL XDatabase::AccountNickNameRegist(ID idaccount, _tstring  szAccountNickName)
{
	return m_pDB->AccountNickNameRegist(idaccount, (TCHAR*)szAccountNickName.c_str());
}

BOOL XDatabase::AccountNameDuplicateCheck(_tstring strAccountNickName)
{
	return m_pDB->AccountNameDuplicateCheck(strAccountNickName);
}


bool XDatabase::AccountNickNameDuplicateCheck( const _tstring& strAccountNickName)
{
	return m_pDB->AccountNickNameDuplicateCheck(strAccountNickName);
}

void XDatabase::SaveAccountShopInfo(ID idaccount, XSPDBUAcc spAcc)
{
	m_pDB->SaveShopInfo(idaccount, spAcc);
}

BOOL XDatabase::AccountNameRegist(ID idAccount, _tstring strAccountName, _tstring strPassword)
{	
	return m_pDB->AccountNameRegist(idAccount, strAccountName, strPassword);
}

BOOL XDatabase::AccountNameRegistCheck(_tstring strAccountName, _tstring strPassword)
{
	return m_pDB->AccountNameRegistCheck(strAccountName, strPassword);
}

// BOOL XDatabase::RegistFBAccount( ID idAccount, LPCTSTR szFBuserid, LPCTSTR szFBusername, LPCTSTR szNickname )
// {
// 	return m_pDB->RegistFBAccount( idAccount, (TCHAR*)szFBuserid, (TCHAR*)szFBusername, (TCHAR*)szNickname );
// }


void XDatabase::AddUserLog( ID idAccount, int type, _tstring  idname, _tstring Log )
{
	m_pDB->AddUserLog( idAccount, type, idname , Log );
}

BOOL XDatabase::FindJewelMatch( ID idFinder, int idxMine, int scoreMin, int scoreMax, XArchive& ar )
{
	return m_pDB->FindAccountMineEnemyZero( NULL, idFinder, idxMine, scoreMin, scoreMax, ar );
}

/**
	arPacket순서
	level/strName/levelMine/defense/legion

*/
BOOL XDatabase::FindJewelInfoByIdAcc( ID idAccount, int idxMine, XArchive& arPacket )
{
	return m_pDB->FindJewelInfoByIdAcc( idAccount, idxMine, arPacket );
}

BOOL XDatabase::ChangeOwnerJewelMine( int idxMine, ID idOldOwner, ID idNewOwner, _tstring& strName, int levelMine, int defense, DWORD secLastEvent )
{
	return m_pDB->ChangeOwnerJewelMine( idOldOwner, idxMine, idNewOwner, strName, idOldOwner, levelMine, defense, (int)secLastEvent );
}

// BOOL XDatabase::AddPostInfo(ID idaccount, ID postsn, ID PostType, _tstring SenderName, _tstring RecvName, _tstring Title, _tstring Message, int nCount, XArchive& ar, ID rewardtableid)
// {
// 	return m_pDB->AddPostInfo(idaccount, postsn, PostType, SenderName, RecvName, Title, Message, nCount, ar, rewardtableid);
// }

BOOL XDatabase::LoadPostInfoByIdAcc(ID idaccount, XArchive& ar)
{
	return m_pDB->LoadPostInfoByIdAcc(idaccount, ar);
}

// BOOL XDatabase::UpdatePostInfo(XSPDBUAcc spAcc)
// {
// 	return m_pDB->UpdatePost(spAcc);
// }
BOOL XDatabase::RemovePostItems(ID idaccount, ID postsn)
{
	return m_pDB->RemovePostItems(idaccount, postsn);
}
BOOL XDatabase::DeletePostInfo(ID idaccount, ID postsn)
{
	return m_pDB->DeletePostInfo(idaccount, postsn);
}

BOOL XDatabase::LoadRankingList(XArchive& arOut)
{
	return m_pDB->LoadServerRanking(arOut);
}
// BOOL XDatabase::DeleteGuild(ID Guildindex)
// {
// 	return m_pDB->DeleteGuild(Guildindex);
// }
BOOL XDatabase::GuildNameDuplicateCheck(_tstring strGuildName)
{
	return m_pDB->GuildNameDuplicateCheck(strGuildName);
}
// BOOL XDatabase::CreateNewGuild(_tstring strGuildName, _tstring strGuildContext, _tstring strUserName, int Ver, ID idAccount, ID& GuildIndex)		//CREATE Guild
// {
// 	return m_pDB->CreateNewGuild(strGuildName, strGuildContext, strUserName, Ver, idAccount, GuildIndex);
// }
// BOOL XDatabase::UpdateGuildMembers(ID Guildindex, const XArchive& ar)
// {
// 	return m_pDB->UpdateGuildMembers(Guildindex, ar);
// }
// BOOL XDatabase::UpdateGuildMemberwithMaster(ID Guildindex, ID newmasteridacc, _tstring newmasterName, const XArchive& ar)
// {
// 	return m_pDB->UpdateGuildMemberwithMaster(Guildindex, newmasteridacc, newmasterName, ar);
// }
BOOL XDatabase::UpdateGuildJoinReq(ID Guildindex, const XArchive& ar)
{
	return m_pDB->UpdateGuildJoinReqListByGuild(Guildindex, ar);
}
// BOOL XDatabase::UpdateGuildJoinAccept(ID Guildindex, XArchive& ar)
// {
// 	return m_pDB->UpdateGuildJoinAccept(Guildindex, ar);
// }
BOOL XDatabase::UpdateGuildContext(ID Guildindex, _tstring strContext)
{
	return m_pDB->UpdateGuildContext(Guildindex, strContext);
}
// BOOL XDatabase::UpdateGuildOption(ID Guildindex, const XArchive& ar)
// {
// 	return m_pDB->UpdateGuildOption(Guildindex, ar);
// }
BOOL XDatabase::LoadGuildInfo(XArchive& arResult)
{
	return m_pDB->LoadGuildInfo(arResult);
}
// BOOL XDatabase::UpdateGuildJoinReqListByAcc(ID idaccount, ID GuildIndex, ID GuildGrade, const XArchive& arResult)
// {
// 	return m_pDB->UpdateGuildJoinReqListByAcc(idaccount, GuildIndex, GuildGrade, arResult);
// }
// BOOL XDatabase::AccountLoadGuildInfo(ID idaccount, ID& GuildIndex, int& guildgrade, XArchive& arOut)
// {
// 	return m_pDB->LoadGuildInfoByAcc(idaccount, GuildIndex, guildgrade, arOut);
// }
BOOL XDatabase::CreateGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion)
{
	return m_pDB->CreateGuildRaidInfo(idaccount, GuildID, raidindex, spotid, vesion);
}
BOOL XDatabase::SelectGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion)
{
	return m_pDB->SelectGuildRaidInfo(idaccount, GuildID, raidindex, spotid, vesion);
}
BOOL XDatabase::SelectGuildRaidEmptyInfo(ID GuildID)
{
	return m_pDB->SelectGuildRaidEmptyInfo(GuildID);
}
BOOL XDatabase::DeleteGuildRaidInfo(ID GuildID, int raidindex, int spotid)
{
	return m_pDB->DeleteGuildRaidInfo(GuildID, raidindex, spotid);
}
BOOL XDatabase::DeleteGuildRaidAll(ID GuildID)
{
	return m_pDB->DeleteGuildRaidAll(GuildID);
}
BOOL XDatabase::UpdateGuildRaid(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return m_pDB->UpdateGuildRaid(idaccount, GuildID, raidindex, spotid, vesion, ar);
}
BOOL XDatabase::UpdateGuildRaidLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return m_pDB->UpdateGuildRaidLock(idaccount, GuildID, raidindex, spotid, vesion, ar);
}
BOOL XDatabase::UpdateGuildRaidUnLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return m_pDB->UpdateGuildRaidUnLock(idaccount, GuildID, raidindex, spotid, vesion, ar);
}
// BOOL XDatabase::LoadGuildJoinReqListByAcc(ID idaccount, XArchive* pOut)
// {
// 	return m_pDB->LoadGuildJoinReqListByAcc(idaccount, pOut);
// }
BOOL XDatabase::TradeGemCall(ID idaccount, int remaingem)
{
	return m_pDB->TradeGemCall(idaccount, remaingem);
}
BOOL XDatabase::ShopGemCall(ID idaccount, int remaingem)
{
	return m_pDB->ShopGemCall(idaccount, remaingem);
}
BOOL XDatabase::RegistAccountDeviceID(ID idacc, _tstring strDeviceID, _tstring strAppstore)
{
	return m_pDB->RegistAccountDeviceID(idacc, strDeviceID, strAppstore);
}
void XDatabase::PushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff)
{
	m_pDB->PushMsgRegist(idacc, type1, type2, recvtime, strBuff);
}
void XDatabase::PushMsgUnRegist(ID idacc, int type1, ID type2)
{
	m_pDB->PushMsgUnRegist(idacc, type1, type2);
}
#ifdef _INAPP_GOOGLE
XGAME::xtErrorIAP XDatabase::CreateGooglePayload(ID idacc, XGAME::xtCashType typeProduct, const _tstring& strProductID, const _tstring& strPayload)
{
	return m_pDB->CreateGooglePayload(idacc, typeProduct, strProductID, strPayload);
}
// XGAME::xtErrorIAP XDatabase::LoadGooglePayload( ID idacc, const _tstring& strPayload )
// {
// 	return m_pDB->LoadGooglePayload( idacc, strPayload );
// }
// XGAME::xtErrorIAP XDatabase::InAppGoogleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt)
// {
// 	return m_pDB->InAppGoogleReceiptVerify(idacc, priductType, strProductID, strReceipt);
// }
#endif
#ifdef _INAPP_APPLE
BYTE XDatabase::InappAppleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt)
{
	return m_pDB->InappAppleReceiptVerify(idacc, priductType, strProductID, strReceipt);
}
#endif
BOOL XDatabase::UserLogAdd(ID idaccount, int logtype, _tstring strNickName, _tstring strLog)
{
	return m_pDB->AddUserLog(idaccount, logtype, strNickName, strLog);
}

BOOL XDatabase::AccountLoadResourceTmp(ID idAccount, XArchive& arPacket)
{
	return m_pDB->AccountLoadResourceTmp(idAccount, arPacket);
}
BOOL XDatabase::AccountUpdateResourcetmp(ID idAccount, XArchive& arPacket)
{
	return m_pDB->AccountUpdateResourcetmp(idAccount, arPacket);
}