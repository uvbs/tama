#include "StdAfx.h"
#include "XUserDB.h"
#include "XHero.h"
#include "XWorld.h"
#include "XPostInfo.h"
#include "XRanking.h"
#include "XGuild.h"
#include "XSpots.h"
#include "XFramework/XGoogleVerify.h"
//#include "Timer.h"

using namespace XGAME;

#define BINARY_BUFF_LEN		8000
#define BINARY_BUFF_LEN2		1000
#define BREAK_AND_LOG()		XBREAK(1)//CONSOLE( "error: %s", __TFUNC__ )
//#define BREAK_AND_LOG()	XBREAKF( 1, "error: %s", __TFUNC__ )
//#define BREAK_AND_LOG()	(0)

struct xJEWEL {
	ID idOwner;
	_tstring OwnerName;
	ID idEnemy;
	int levelMine;
	int defense;
	int idxLegion;
	float secLastEvent;	// 매치가되었거나 마지막으로 전투가일어난 시간
	xJEWEL() {
		idOwner = 0;
		idEnemy = 0;
		levelMine = 0;
		defense = 0;
		idxLegion = 0;
		secLastEvent = 0;
	}
};

struct xMandrakeDB {
	ID idOwner;
	ID idEnemy;
	int win;
	int reward;
	int idxLegion;
	int offwin;
	TCHAR szName[256];
	xMandrakeDB()	{
		idOwner = 0;
		idEnemy = 0;
		win = 0;
		reward = 0;
		idxLegion = 0;
		offwin = 0;
		XCLEAR_ARRAY(szName);
	}
};

struct ArchiveSplit {
	SQLLEN sqlLen;
	BYTE buff[BINARY_BUFF_LEN];
	int size() const {
		return sizeof(buff);
	}
	ArchiveSplit() {
		sqlLen = 0;
		XCLEAR_ARRAY(buff);
	}
};

CUserDB::CUserDB(void)
{
}
CUserDB::~CUserDB(void)
{
}
void CUserDB::OutputTextFormat(const TCHAR* szFormat, ...)
{
	const int bufSize = 2048;
	TCHAR textBuf[bufSize];
	va_list args;
	va_start(args, szFormat);
	_vsnwprintf_s(textBuf, bufSize, _TRUNCATE, szFormat, args);
	va_end(args);
	_tprintf(textBuf);
	TRACE("%s", textBuf);
}
//
BOOL CUserDB::AccountUpdateData(XSPDBUAcc spAcc)
{
	return FALSE;
	//	AccountUpdateAll( spAcc );		//계정정보 몽땅...
	//	
	//	HaveThemeUpdate( spAcc );
	//	DailyQuestUpdate( spAcc );
	//	NormalQuestUpdate( spAcc );
	//	LastScenarioUpdate( spAcc );
	//	TempScenarioUpdate( spAcc );
	//
	//	return TRUE;
}
//
//
BOOL CUserDB::AccountLoadDataExist(XSPDBUAcc spAcc)
{
	//AccountDataLoadAll( spAcc );		//LOAD
	//DBTeamCardLoad( spAcc );
	//DBCardInvenLoad( spAcc );
	//DBItemInvenLoad( spAcc );
	//DBUseItemInvenLoad( spAcc );	//카드에 달려있는 아이템도 모두 읽자.

	//HaveThemeLoad( spAcc );
	//DailyQuestLoad( spAcc );
	//NormalQuestLoad( spAcc );
	//LastScenarioLoad( spAcc );
	//TempScenarioLoad( spAcc );	

	return TRUE;
}

//BOOL CUserDB::AccountLoadDataAll		( XSPDBUAcc spAcc )		//순서대로 읽기
//{
//AccountDataLoadAll( spAcc );		//LOAD

////기존에 있던 데이터가 있으면 일단 읽어 주고	
//DBTeamCardLoad( spAcc );
//DBCardInvenLoad( spAcc );
//DBItemInvenLoad( spAcc );
//DBUseItemInvenLoad( spAcc );	//카드에 달려있는 아이템도 모두 읽자.

//HaveThemeLoad( spAcc );
//DailyQuestLoad( spAcc );
//NormalQuestLoad( spAcc );
//LastScenarioLoad( spAcc );
//TempScenarioLoad( spAcc );
//}

//void CUserDB::AccountUpdateFreeGem( ID idaccount, int  timestamp)
//{
//	ReAllocStmt();
//	SQLUSMALLINT i = 1;
//	SQLLEN		retLen = 0;	
//
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &idaccount,				0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &timestamp,				0, &retLen, i++);	
//	
//	SQLRETURN ret;
//
//	if ( (ret = Excute (_T("{ call dbo.AccountUpdateFreeGem( ?,?) }"))) != SQL_SUCCESS )
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountUpdateFreeGem Error : %s"), err);
//		BREAK_AND_LOG();		
//	}	
//}
//
//void CUserDB::AccountUpdateFirstRefill( ID idaccount, int timestamp)
//{
//	ReAllocStmt();
//	SQLUSMALLINT i = 1;
//	SQLLEN		retLen = 0;	
//
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &idaccount,				0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &timestamp,				0, &retLen, i++);	
//	
//	SQLRETURN ret;
//
//	if ( (ret = Excute (_T("{ call dbo.AccountUpdateFirstRefill( ?,?) }"))) != SQL_SUCCESS )
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountUpdateFirstRefill Error : %s"), err);
//		BREAK_AND_LOG();		
//	}	
//}
//
//void CUserDB::AccountUpdateFreeStoneMoney( ID idaccount, int timestamp)
//{
//	ReAllocStmt();
//	SQLUSMALLINT i = 1;
//	SQLLEN		retLen = 0;	
//
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &idaccount,				0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &timestamp,				0, &retLen, i++);	
//	
//	SQLRETURN ret;
//
//	if ( (ret = Excute (_T("{ call dbo.AccountUpdateFreeStoneMoney( ?,?) }"))) != SQL_SUCCESS )
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountUpdateFreeStoneMoney Error : %s"), err);
//		BREAK_AND_LOG();		
//	}	
//}
//
//void CUserDB::AccountUpdateGoldData( ID idaccount, DWORD Gold, DWORD rewardgold )
//{
//	ReAllocStmt();
//	SQLUSMALLINT i = 1;
//	SQLLEN		retLen = 0;	
//
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &idaccount,				0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &Gold,						0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &rewardgold,				0, &retLen, i++);	
//	
//	SQLRETURN ret;
//
//	if ( (ret = Excute (_T("{ call dbo.AccountUpdateGold( ?,?,?) }"))) != SQL_SUCCESS )
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountUpdateGold Error : %s"), err);
//		BREAK_AND_LOG();		
//	}	
//}
//
// BOOL CUserDB::CreateNewAccount(_tstring strNickname, _tstring tUUID, ID& idaccount)
// {
// 	ReAllocStmt();
// 
// 	SQLRETURN ret;
// 	int retValue = 0;
// 
// 	SQLLEN	retLen = 0, NcknameIDLen = SQL_NTS, loginIDLen = SQL_NTS;
// 
// 	int nicknamelen = _tcslen(strNickname.c_str());
// 	int uuidlen = _tcslen(tUUID.c_str());
// 	XBREAK(uuidlen == 0);
// 
// 	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &retValue, 0, &retLen, 1);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicknamelen, 0, (TCHAR*)strNickname.c_str(), 0, &loginIDLen, 2);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)uuidlen, 0, (TCHAR*)tUUID.c_str(), 0, &loginIDLen, 3);
// 
// 	if ((ret = Excute(_T("{ ? = call dbo.AccountAddUUID( ?,? ) }"))) != SQL_SUCCESS)
// 	{
// 		TCHAR err[1024];
// 		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
// 		OutputTextFormat(_T("\n AccountUUIDAdd Error : %s"), err);
// 		BREAK_AND_LOG();
// 		return FALSE;
// 	}
// 
// 	if (retValue > 0)
// 	{
// 		idaccount = retValue;
// 		return TRUE;
// 	}
// 	return FALSE;
// }
/**
 @brief 새 계정을 생성한다.
 @param strNickname 새계정의 닉네임
 @param tUUID 새 계정의 uuid
 @return 만들어진 계정의 idAcc를 리턴한다.
*/
ID CUserDB::CreateNewAccount( const _tstring& strNickname, const _tstring& tUUID )
{
	ReAllocStmt();
	SQLRETURN ret;
	int retValue = 0;
	SQLLEN	retLen = 0, NcknameIDLen = SQL_NTS, loginIDLen = SQL_NTS;
	int nicknamelen = _tcslen(strNickname.c_str());
	int uuidlen = _tcslen(tUUID.c_str());
	XBREAK(uuidlen == 0);
	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &retValue, 0, &retLen, 1);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicknamelen, 0, (TCHAR*)strNickname.c_str(), 0, &loginIDLen, 2);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)uuidlen, 0, (TCHAR*)tUUID.c_str(), 0, &loginIDLen, 3);
	if ((ret = Excute(_T("{ ? = call dbo.AccountAddUUID( ?,? ) }"))) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUUIDAdd Error : %s"), err);
		BREAK_AND_LOG();
		return 0;
	}
	if( XBREAK(retValue < 0) )	// <<- 이런경우가 있나?
		retValue = 0;
	return (ID)retValue;
}

BOOL CUserDB::SelectTargetbattlestatus(ID idDefender
																		, ID& connectserverid
																		, ID& battletargetid
																		, ID& battletime)
{
	ReAllocStmt();

	ID DB_battletarget = 0;
	ID DB_battletime = 0;
	ID DB_connectsvr = 0;
	connectserverid = 0;
	battletargetid = 0;
	battletime = 0;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadBattleStatus( '%d' ) }"), idDefender);

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadBattleStatus Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	int idaccount = 0;
	SQLUSMALLINT i = 1;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;

	ret = BindCol(SQL_C_LONG, &DB_connectsvr, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &DB_battletarget, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &DB_battletime, sizeof(int), &nLen, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		if (DB_connectsvr > 0) {
			connectserverid = DB_connectsvr;
		} else {
			if (DB_battletarget > 0) {
				battletargetid = DB_battletarget;
				battletime = DB_battletime;
			}
		}		
	}
	return TRUE;
}
BOOL CUserDB::AccountUpdateBattleInfo(ID idDefender, ID idAttacker, int sectime)
{
	ReAllocStmt();

	SQLRETURN ret;
	int retValue = 0;

	SQLUSMALLINT i = 1;
	SQLLEN	retLen = 0;

	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idDefender, 0, &retLen, i++);
	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAttacker, 0, &retLen, i++);
	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &sectime, 0, &retLen, i++);
	
	if ((ret = Excute(_T("{ call dbo.AccountUpdateBattleInfo( ?,?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUpdateBattleInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;  
}
BOOL CUserDB::CreateNewAccountIDPW(TCHAR* szID, TCHAR* szPW, int& idaccount)
{
	return FALSE;
	//	ReAllocStmt();
	//
	//	SQLRETURN ret;
	//	int retValue = 0;
	//
	//	SQLLEN	retLen = 0, loginIDLen = SQL_NTS;
	//
	//	int idlen = _tcslen( szID );
	//	int pwlen = _tcslen( szPW );
	//
	//	ret = BindParameter(SQL_PARAM_OUTPUT,	SQL_C_LONG,	SQL_INTEGER,	(SQLUINTEGER)sizeof(int),		0, &retValue,	0, &retLen,		1);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)idlen,			0, szID,		0, &loginIDLen,	2);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)pwlen,			0, szPW,		0, &loginIDLen,	3);
	//
	//	if ( (ret = Excute (_T("{ ? = call dbo.AccountAddFacebook( ?, ? ) }"))) != SQL_SUCCESS ) 
	//	{
	//		TCHAR err[1024];
	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
	//		OutputTextFormat(_T("\n AccountAddFacebook Error : %s"), err);
	//		BREAK_AND_LOG();
	//		return FALSE; 
	//	}
	//
	//	if( retValue > 0 ) 
	//	{
	//		idaccount = retValue;
	//		return TRUE;
	//	}
	//
	//	return FALSE;
}
BOOL	CUserDB::CreateNewAccountFacebook(TCHAR* szuserid, TCHAR* szusername, TCHAR* szNickName, int& idaccount)
{
	return FALSE;
	//	ReAllocStmt();
	//
	//	SQLRETURN ret;
	//	int retValue = 0;
	//
	//	SQLLEN	retLen = 0, loginIDLen = SQL_NTS;
	//
	//	int useridlen = _tcslen( szuserid );
	//	int usernamelen = _tcslen( szusername );
	//	int nicknamelen = _tcslen( szNickName );
	//
	//	ret = BindParameter(SQL_PARAM_OUTPUT,	SQL_C_LONG,	SQL_INTEGER,	(SQLUINTEGER)sizeof(int),		0, &retValue,		0, &retLen,		1);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)useridlen,		0, szuserid,		0, &loginIDLen,	2);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)usernamelen,	0, szusername,	0, &loginIDLen,	3);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)nicknamelen,	0, szNickName,	0, &loginIDLen,	4);
	//
	//	if ( (ret = Excute (_T("{ ? = call dbo.AccountAddFacebook( ?, ?, ? ) }"))) != SQL_SUCCESS ) 
	//	{
	//		TCHAR err[1024];
	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
	//		OutputTextFormat(_T("\n AccountAdd Error : %s"), err);
	//		BREAK_AND_LOG();
	//		return FALSE; 
	//	}
	//
	//	if( retValue > 0 ) 
	//	{
	//		idaccount = retValue;
	//		return TRUE;
	//	}
	//
	//	return FALSE;
}
BOOL CUserDB::FindDuplicateVerify(ID idaccount, TCHAR* szJsonReceiptFromClient)
{
	return TRUE;
	//	ReAllocStmt();
	//
	//	SQLRETURN ret;
	//	int retValue = 0;
	//
	//	SQLLEN	retLen = 0, JsonReceiptLen = SQL_NTS;
	//
	//	int len = _tcslen( szJsonReceiptFromClient );
	//
	//	ret = BindParameter(SQL_PARAM_OUTPUT,	SQL_C_LONG,		SQL_INTEGER,	(SQLUINTEGER)sizeof(int),	0, &retValue,						0, &retLen,			1);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_LONG,		SQL_INTEGER,	(SQLUINTEGER)sizeof(int),	0, &idaccount,					0, &retLen,			2);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,		SQL_WCHAR,		(SQLUINTEGER)len,			0, szJsonReceiptFromClient,	0, &JsonReceiptLen,3);
	//
	//	if ( (ret = Excute (_T("{ ? = call dbo.FindDuplicateVerify( ?, ? ) }"))) != SQL_SUCCESS ) 
	//	{
	//		CONSOLE("AddDuplicateVerify Error %d, %d", idaccount, ret);
	//	}
	//
	//	if( retValue > 0 ) 
	//	{		
	//		return FALSE;
	//	}
	//	else
	//	{
	//		CONSOLE("AddDuplicateVerify %d, %s", idaccount, szJsonReceiptFromClient);
	//		AddDuplicateVerify( idaccount, szJsonReceiptFromClient);
	//	}
	//	return TRUE;
}
void CUserDB::AddDuplicateVerify(ID idaccount, TCHAR* szJsonReceiptFromClient)
{
	//	ReAllocStmt();
	//
	//	SQLRETURN ret;
	//	int retValue = 0;
	//
	//	SQLUSMALLINT i = 1;
	//	SQLLEN	retLen = 0, nicknameLen = SQL_NTS, idnameLen = SQL_NTS, pwLen = SQL_NTS ;
	//	
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_LONG,	SQL_INTEGER,	sizeof(int),													0, &idaccount,					0, &retLen,		i++);
	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)wcslen(szJsonReceiptFromClient),		0, szJsonReceiptFromClient,	0, &nicknameLen,i++);
	//	
	//	if ( (ret = Excute (_T("{ call dbo.AddDuplicateVerify( ?,?) }"))) != SQL_SUCCESS ) 
	//	{
	//		TCHAR err[1024];
	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
	//		OutputTextFormat(_T("\n AddDuplicateVerify Error : %s"), err);
	//		BREAK_AND_LOG();	
	//	}	
}
//
BOOL CUserDB::AccountNameDuplicateCheck(_tstring strAccountName)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountNameDuplicate( '%s' ) }"), strAccountName.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountAlreadyExist Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	int idaccount = 0;
	SQLUSMALLINT i = 1;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;

	ret = BindCol(SQL_C_LONG, &idaccount, sizeof(int), &nLen, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if (idaccount > 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CUserDB::AccountNameRegist(ID idAccount, _tstring  szAccountName, _tstring  szPassword)
{
	ReAllocStmt();

	SQLRETURN ret;
	int retValue = 0;

	SQLUSMALLINT i = 1;
	SQLLEN	retLen = 0, nicknameLen = SQL_NTS, idnameLen = SQL_NTS, pwLen = SQL_NTS;

	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)wcslen(szAccountName.c_str()), 0, (TCHAR*)szAccountName.c_str(), 0, &nicknameLen, i++);
	ret = BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)wcslen(szPassword.c_str()), 0, (TCHAR*)szPassword.c_str(), 0, &pwLen, i++);

	if ((ret = Excute(_T("{ call dbo.AccountNameRegist( ?,?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountRegist Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

BOOL CUserDB::AccountNameRegistCheck(_tstring  strAccountName, _tstring  strPassword)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountNameRegistCheck( '%s', '%s' ) }"), strAccountName.c_str(), strPassword.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountAlreadyExist Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	int idaccount = 0;
	SQLUSMALLINT i = 1;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;

	ret = BindCol(SQL_C_LONG, &idaccount, sizeof(int), &nLen, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if (idaccount > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//
// BOOL CUserDB::RegistFBAccount(ID idAccount, TCHAR* pFBuserID, TCHAR* pFBUserName, TCHAR* szNickname)
// {
// 	return FALSE;
// 	//	ReAllocStmt();
// 	//
// 	//	SQLRETURN ret;
// 	//	int retValue = 0;
// 	//
// 	//	SQLUSMALLINT i = 1;
// 	//	SQLLEN	retLen = 0, nicknameLen = SQL_NTS, idnameLen = SQL_NTS, pwLen = SQL_NTS ;
// 	//
// 	//	ret = BindParameter(SQL_PARAM_OUTPUT,	SQL_C_LONG,	SQL_INTEGER,	(SQLUINTEGER)sizeof(int),					0, &retValue,				0, &retLen,			i++);
// 	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_LONG,	SQL_INTEGER,	sizeof(int),									0, &idAccount,			0, &retLen,			i++);
// 	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)wcslen(pFBuserID),		0, pFBuserID,				0, &nicknameLen,	i++);
// 	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)wcslen(pFBUserName),	0, pFBUserName,		0, &idnameLen,		i++);	
// 	//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)wcslen(szNickname),	0, szNickname,		0, &nicknameLen,		i++);	
// 	//
// 	//	if ( (ret = Excute (_T("{ ? = call dbo.AccountFBRegist( ?,?,?,? ) }"))) != SQL_SUCCESS ) 
// 	//	{
// 	//		TCHAR err[1024];
// 	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
// 	//		OutputTextFormat(_T("\n AccountAdd Error : %s"), err);
// 	//		BREAK_AND_LOG();
// 	//		return FALSE; 
// 	//	}
// 	//
// 	//	if( retValue > 0 ) 
// 	//	{
// 	//		return TRUE;
// 	//	}
// 	//
// 	//	return FALSE;
// }
bool CUserDB::RegistFBAccount(ID idAcc
														, const _tstring& strFbUserId
														, const _tstring& strFbUsername)
{
	SQLLEN	retLen = 0, deviceidLen = SQL_NTS;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	BindParameter( SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof( int ), 0, &idAcc, 0, &retLen, i++ );
	SQLRETURN ret;
	TCHAR szQuery[ 1024 ];
	_stprintf_s( szQuery, _T( "{ call dbo.RegistFbInfo( ?,  '%s', '%s') }" )
																		, strFbUserId.c_str(), strFbUsername.c_str() );
	if( ( ret = Excute( szQuery ) ) != SQL_SUCCESS ) {
		TCHAR err[ 1024 ];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Error : %s" ), __TFUNC__, err );
		BREAK_AND_LOG();
		return false;
	}
	return true;
}

//

BOOL CUserDB::AddUserLog(ID idAccount, int type, _tstring strNickName, _tstring strLog)
{
	ReAllocStmt();

	int retValue = 0;
	SQLRETURN	ret;
	SQLUSMALLINT i = 1;
	SQLLEN	retLen = 0, Loglen = SQL_NTS, idnameLen = SQL_NTS, pwLen = SQL_NTS;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &type, 0, &retLen, i++);

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.UserLogAdd( ?, ?, '%s', '%s' ) }"), strNickName.c_str(), strLog.c_str());	
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UserLogAdd Error : %s"), err);
		return FALSE;
	}
	return TRUE;
}
//#endif //_xCONTENTS_USERLOG	
//BOOL	CUserDB::CreateNewAccountLyto( TCHAR* szDeviceid, int sessionmemberid, TCHAR* szSessiontoken,  int& idaccount )
//{
//	ReAllocStmt();
//
//	SQLRETURN ret;
//	int retValue = 0;
//
//	SQLLEN	retLen = 0, loginIDLen = SQL_NTS;
//
//	int deviceidlen = _tcslen( szDeviceid );
//	int sessionlen = _tcslen( szSessiontoken );
//	SQLUSMALLINT i = 1;
//	ret = BindParameter(SQL_PARAM_OUTPUT,	SQL_C_LONG,	SQL_INTEGER,	(SQLUINTEGER)sizeof(int),		0, &retValue,				0,	&retLen,		i++	);
//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)deviceidlen,	0, szDeviceid,				0,	&loginIDLen,	i++	);
//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_WCHAR,	SQL_WCHAR,		(SQLUINTEGER)sessionlen,		0, szSessiontoken,		0,	&loginIDLen,	i++	);
//	ret = BindParameter(SQL_PARAM_INPUT,	SQL_C_LONG,	SQL_INTEGER,	sizeof(int),						0, &sessionmemberid,	0,	&retLen,		i++	);
//
//	if ( (ret = Excute (_T("{ ? = call dbo.AccountAddLyto( ?, ?, ? ) }"))) != SQL_SUCCESS ) 
//	{
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountAdd Error : %s"), err);
//		BREAK_AND_LOG();
//		return FALSE; 
//	}
//
//	if( retValue > 0 ) 
//	{
//		idaccount = retValue;
//		return TRUE;
//	}
//
//	return FALSE;
//}
//#ifdef _LOGIN_SVR_IDN
//BOOL	CUserDB::AccountLoadLytoAccount ( XSPDBUAcc spAcc, LPCTSTR szDeviceID, LPCTSTR szSessionToken, ID SessionMemberID )
//{
//	ReAllocStmt();
//
//	TCHAR szQuery[1024];
//	_stprintf_s( szQuery, 1024, _T("{ call dbo.AccountLoadLyto( '%s, %d, %s' ) }"), szDeviceID, SessionMemberID, szSessionToken);
//
//	SQLRETURN	ret;
//	if ( (ret = Excute( szQuery)) != SQL_SUCCESS ) 
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountLoadLyto Execute Error (err:%d) : %s"), ret, err);
//		BREAK_AND_LOG();
//		return FALSE; 
//	}	
//
//	BYTE m_idCurrTheme = 0;	
//	BYTE m_idPlayTheme = 0;
//	WORD m_maxCardInven = 0;
//	WORD m_maxItemInven = 0;
//	BYTE m_idxCurrScenario = 0;	
//	short m_Gem = 0;
//	short m_Level = 0;
//	short m_Stamina = 0;
//	short m_idThemeCurr = 0;
//	short m_idxScenarioCurr = 0;
//	int m_EE = 0;
//	int m_Ver = 0;
//	int m_Exp = 0;
//	int m_Gold = 0;
//	int m_idAccount = 0;
//	int m_pointSummon = 0;	
//	int m_timePassStamina = 0;
//	int SnNum = 0;
//
//	BYTE Buff[512];	
//	TCHAR tstrPass[40];
//	TCHAR tstrNick[40];
//	
//	SQLUSMALLINT i = 1;
//	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;
//
//	ret = BindCol( SQL_C_LONG,		&m_idAccount,				sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_WCHAR,	tstrPass,						LEN_LOGIN_ID,	&pwLen,		i++);
//	ret = BindCol( SQL_C_WCHAR,	tstrNick,						LEN_LOGIN_ID,	&nickLen,	i++);
//	ret = BindCol( SQL_C_LONG,		&m_Ver,						sizeof(int),	&nLen, 		i++);	
//	ret = BindCol( SQL_C_USHORT,	&m_maxCardInven,			sizeof(WORD),&nLen, 		i++);
//	ret = BindCol( SQL_C_USHORT,	&m_maxItemInven,			sizeof(WORD),&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_Gold,						sizeof(int ),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_EE,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Gem,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Level,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Stamina,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idCurrTheme,			sizeof(BYTE),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idxCurrScenario,		sizeof(char),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_idThemeCurr,			sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_idxScenarioCurr,		sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idPlayTheme,			sizeof(BYTE),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_Exp,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_pointSummon,			sizeof(int),	&nLen, 		i++);	
//	ret = BindCol( SQL_C_LONG,		&m_timePassStamina,		sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&SnNum,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_BINARY,	&Buff,							512,			&dataLen, 	i++);
//
//	ret= Fetch();
//	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )//!= SQL_NO_DATA)
//	{
//		XArchive pTutorial; //TUTORIAL
//		pTutorial.SetBufferMem(Buff, dataLen);
//
//		spAcc->m_strName				= tstrNick;
//		spAcc->m_strPassword		= tstrPass;
//		spAcc->m_idAccount			= (DWORD)m_idAccount;
//		spAcc->m_maxCardInven		= (WORD)m_maxCardInven,	
//		spAcc->m_maxItemInven		= (WORD)m_maxItemInven;	
//		spAcc->m_Gold					= (DWORD)m_Gold;			
//		spAcc->m_EE					= (DWORD)m_EE;
//		spAcc->m_Gem					= (WORD)m_Gem;
//		spAcc->m_Level					= (WORD)m_Level;
//		spAcc->m_Stamina				= (WORD) m_Stamina;
//		spAcc->m_idCurrTheme		= (BYTE)m_idCurrTheme;
//		spAcc->m_idxCurrScenario	= (char)m_idxCurrScenario;
//		spAcc->m_idThemeCurr		= (WORD)m_idThemeCurr;
//		spAcc->m_idxScenarioCurr	= m_idxScenarioCurr;
//		spAcc->m_idPlayTheme		= (BYTE )m_idPlayTheme;
//		spAcc->m_Exp					= (DWORD)m_Exp;
//		spAcc->m_pointSummon		= (DWORD)m_pointSummon;		
//		spAcc->m_timePassStamina	= (DWORD)m_timePassStamina;
//		spAcc->m_CurrSN				= (DWORD)SnNum;
//		spAcc->m_Tutorial.RestorePacket( pTutorial );
//
//		return TRUE;
//	}
//	else
//	{
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountLoadLyto Data Error  (err:%d) : %s"), ret, err);
//	}
//
//	return FALSE;
//}
//
//
//#elif _LOGIN_SVR_THAILAND
//BOOL	CUserDB::AccountLoadIni3Account ( XSPDBUAcc spAcc, LPCTSTR szDeviceID, LPCTSTR szSessionToken, ID SessionMemberID )
//{
//	ReAllocStmt();
//
//	TCHAR szQuery[1024];
//	_stprintf_s( szQuery, 1024, _T("{ call dbo.AccountLoadIni3( '%s, %d, %s' ) }"), szDeviceID, SessionMemberID, szSessionToken);
//
//	SQLRETURN	ret;
//	if ( (ret = Excute( szQuery)) != SQL_SUCCESS ) 
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountLoadIni3 Execute Error (err:%d) : %s"), ret, err);
//		BREAK_AND_LOG();
//		return FALSE; 
//	}	
//
//	BYTE m_idCurrTheme = 0;	
//	BYTE m_idPlayTheme = 0;
//	WORD m_maxCardInven = 0;
//	WORD m_maxItemInven = 0;
//	BYTE m_idxCurrScenario = 0;	
//	short m_Gem = 0;
//	short m_Level = 0;
//	short m_Stamina = 0;
//	short m_idThemeCurr = 0;
//	short m_idxScenarioCurr = 0;
//	int m_EE = 0;
//	int m_Ver = 0;
//	int m_Exp = 0;
//	int m_Gold = 0;
//	int m_idAccount = 0;
//	int m_pointSummon = 0;	
//	int m_timePassStamina = 0;
//	int SnNum = 0;
//
//	BYTE Buff[512];	
//	TCHAR tstrPass[40];
//	TCHAR tstrNick[40];
//	
//	SQLUSMALLINT i = 1;
//	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;
//
//	ret = BindCol( SQL_C_LONG,		&m_idAccount,				sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_WCHAR,	tstrPass,						LEN_LOGIN_ID,	&pwLen,		i++);
//	ret = BindCol( SQL_C_WCHAR,	tstrNick,						LEN_LOGIN_ID,	&nickLen,	i++);
//	ret = BindCol( SQL_C_LONG,		&m_Ver,						sizeof(int),	&nLen, 		i++);	
//	ret = BindCol( SQL_C_USHORT,	&m_maxCardInven,			sizeof(WORD),&nLen, 		i++);
//	ret = BindCol( SQL_C_USHORT,	&m_maxItemInven,			sizeof(WORD),&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_Gold,						sizeof(int ),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_EE,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Gem,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Level,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_Stamina,					sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idCurrTheme,			sizeof(BYTE),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idxCurrScenario,		sizeof(char),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_idThemeCurr,			sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_SHORT,		&m_idxScenarioCurr,		sizeof(short),	&nLen, 		i++);
//	ret = BindCol( SQL_C_TINYINT,	&m_idPlayTheme,			sizeof(BYTE),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_Exp,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&m_pointSummon,			sizeof(int),	&nLen, 		i++);	
//	ret = BindCol( SQL_C_LONG,		&m_timePassStamina,		sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&SnNum,						sizeof(int),	&nLen, 		i++);
//	ret = BindCol( SQL_C_BINARY,	&Buff,							512,			&dataLen, 	i++);
//
//	ret= Fetch();
//	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )//!= SQL_NO_DATA)
//	{
//		XArchive pTutorial; //TUTORIAL
//		pTutorial.SetBufferMem(Buff, dataLen);
//
//		spAcc->m_strName				= tstrNick;
//		spAcc->m_strPassword		= tstrPass;
//		spAcc->m_idAccount			= (DWORD)m_idAccount;
//		spAcc->m_maxCardInven		= (WORD)m_maxCardInven,	
//		spAcc->m_maxItemInven		= (WORD)m_maxItemInven;	
//		spAcc->m_Gold					= (DWORD)m_Gold;			
//		spAcc->m_EE					= (DWORD)m_EE;
//		spAcc->m_Gem					= (WORD)m_Gem;
//		spAcc->m_Level					= (WORD)m_Level;
//		spAcc->m_Stamina				= (WORD) m_Stamina;
//		spAcc->m_idCurrTheme		= (BYTE)m_idCurrTheme;
//		spAcc->m_idxCurrScenario	= (char)m_idxCurrScenario;
//		spAcc->m_idThemeCurr		= (WORD)m_idThemeCurr;
//		spAcc->m_idxScenarioCurr	= m_idxScenarioCurr;
//		spAcc->m_idPlayTheme		= (BYTE )m_idPlayTheme;
//		spAcc->m_Exp					= (DWORD)m_Exp;
//		spAcc->m_pointSummon		= (DWORD)m_pointSummon;		
//		spAcc->m_timePassStamina	= (DWORD)m_timePassStamina;
//		spAcc->m_CurrSN				= (DWORD)SnNum;
//		spAcc->m_Tutorial.RestorePacket( pTutorial );
//
//		return TRUE;
//	}
//	else
//	{
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountLoadIni3 Data Error  (err:%d) : %s"), ret, err);
//	}
//
//	return FALSE;
//}
//#endif
// BOOL CUserDB::AccountLoadIDName(XSPDBUAcc spAcc, _tstring strID)
// {
// 	ReAllocStmt();
// 
// 	TCHAR szQuery[1024];
// 	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadIDName( '%s' ) }"), strID.c_str());
// 
// 	SQLRETURN	ret;
// 	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
// 	{
// 		TCHAR err[1024];
// 		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
// 		OutputTextFormat(_T("\n AccountLoadIDName Execute Error (err:%d) : %s"), ret, err);
// 		BREAK_AND_LOG();
// 		return FALSE;
// 	}
// 
// 	SQLUSMALLINT i = 1;
// 	SQLLEN nLen = 0, pwLen = SQL_NTS, nickLen = SQL_NTS;;
// 	int			tmpidAccount = 0;
// 	int			Ver = 0;
// 	int			lastconnectsvrid = 0;
// 	int			GMLevel = 0;
// 	TCHAR		Buff[1024];
// 	TCHAR		szNickName[LEN_LOGIN_ID];
// 	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
// 	ret = BindCol(SQL_C_WCHAR, Buff, LEN_LOGIN_ID * 2, &pwLen, i++);
// 	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
// 	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
// 	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);
// 	ret = BindCol(SQL_C_WCHAR, szNickName, LEN_LOGIN_ID, &nickLen, i++);
// 
// 	ret = Fetch();
// 
// 	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
// 	{
// 		if ((DWORD)tmpidAccount > 0)  spAcc->m_idAccount = (DWORD)tmpidAccount;
// 		if (Ver > 0) spAcc->m_Ver = Ver;
// 		if (GMLevel  > 0) spAcc->m_GMLevel = GMLevel;
// 		if (lastconnectsvrid > 0) spAcc->m_lastconnectsvrid = lastconnectsvrid; //서버 중복 접속 허용 막을때 여기 주석 풀어주자.
// 		spAcc->SetstrName(szNickName);
// 		int pwlen = wcslen(Buff);
// 		if (pwlen > 0)
// 		{
// 			spAcc->m_strPassword = Buff;
// 			return TRUE;
// 		}
// 	}
// 	else
// 	{
// 		TCHAR err[1024];
// 		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
// 		OutputTextFormat(_T("\n AccountLoadIDName Data Error  (err:%d) : %s"), ret, err);
// 		//		BREAK_AND_LOG();
// 	}
// 
// 	return FALSE;
// }



BOOL	CUserDB::AccountLoadFBUserID(XSPDBUAcc spAcc, LPCTSTR FBUserID)
{
	return TRUE;
	//	XPROF_OBJ_AUTO();
	//	ReAllocStmt();
	//
	//	TCHAR szQuery[1024];
	//	_stprintf_s( szQuery, 1024, _T("{ call dbo.AccountLoadFBUserID( '%s') }"), FBUserID);
	//
	//	SQLRETURN	ret;
	//	if ( (ret = Excute( szQuery)) != SQL_SUCCESS ) 
	//	{		
	//		TCHAR err[1024];
	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
	//		OutputTextFormat(_T("\n AccountLoadFBUserID Execute Error (err:%d) : %s"), ret, err);
	//		BREAK_AND_LOG();
	//		return FALSE; 
	//	}	
	//
	//	BYTE m_idCurrTheme = 0;	
	//	BYTE m_idPlayTheme = 0;
	//	WORD m_maxCardInven = 0;
	//	WORD m_maxItemInven = 0;
	//	BYTE m_idxCurrScenario = 0;	
	//	short m_Gem = 0;
	//	short m_Level = 0;
	//	short m_Stamina = 0;
	//	short m_idThemeCurr = 0;
	//	short m_idxScenarioCurr = 0;
	//	int m_EE = 0;
	//	int m_Ver = 0;
	//	int m_Exp = 0;
	//	int m_Gold = 0;
	//	int m_idAccount = 0;
	//	int m_pointSummon = 0;	
	//	int m_timePassStamina = 0;
	//	int SnNum = 0;
	//
	//	BYTE Buff[512];	
	////	TCHAR tstrPass[40];
	//	TCHAR tstrNick[40];
	//	TCHAR FBUserName[40];
	//	
	//	
	//	SQLUSMALLINT i = 1;
	//	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;
	//
	//	ret = BindCol( SQL_C_LONG,		&m_idAccount,				sizeof(int),	&nLen, 		i++);
	//	//ret = BindCol( SQL_C_WCHAR,	tstrPass,						LEN_LOGIN_ID,	&pwLen,		i++);
	//	ret = BindCol( SQL_C_WCHAR,		tstrNick,						LEN_LOGIN_ID*2,	&nickLen,	i++);
	//	ret = BindCol( SQL_C_WCHAR,		FBUserName,						LEN_LOGIN_ID*2,	&nickLen,	i++);
	//	ret = BindCol( SQL_C_LONG,		&m_Ver,						sizeof(int),	&nLen, 		i++);	
	//	ret = BindCol( SQL_C_USHORT,	&m_maxCardInven,			sizeof(WORD),&nLen, 		i++);
	//	ret = BindCol( SQL_C_USHORT,	&m_maxItemInven,			sizeof(WORD),&nLen, 		i++);
	//	ret = BindCol( SQL_C_LONG,		&m_Gold,						sizeof(int ),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_LONG,		&m_EE,						sizeof(int),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_SHORT,		&m_Gem,					sizeof(short),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_SHORT,		&m_Level,					sizeof(short),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_SHORT,		&m_Stamina,					sizeof(short),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_TINYINT,	&m_idCurrTheme,			sizeof(BYTE),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_TINYINT,	&m_idxCurrScenario,		sizeof(char),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_SHORT,		&m_idThemeCurr,			sizeof(short),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_SHORT,		&m_idxScenarioCurr,		sizeof(short),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_TINYINT,	&m_idPlayTheme,			sizeof(BYTE),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_LONG,		&m_Exp,						sizeof(int),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_LONG,		&m_pointSummon,			sizeof(int),	&nLen, 		i++);	
	//	ret = BindCol( SQL_C_LONG,		&m_timePassStamina,		sizeof(int),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_LONG,		&SnNum,						sizeof(int),	&nLen, 		i++);
	//	ret = BindCol( SQL_C_BINARY,	&Buff,							512,			&dataLen, 	i++);
	//
	//	ret= Fetch();
	//	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )//!= SQL_NO_DATA)
	//	{
	//		XArchive pTutorial; //TUTORIAL
	////		XBREAK( dataLen <= 0 );
	//		if( dataLen > 0 )
	//			pTutorial.SetBufferMem(Buff, dataLen);
	//
	//		spAcc->m_strName				= tstrNick;
	//		//spAcc->m_strPassword		= tstrPass;
	//		spAcc->m_idAccount			= (DWORD)m_idAccount;
	//		spAcc->m_maxCardInven		= (WORD)m_maxCardInven,	
	//		spAcc->m_maxItemInven		= (WORD)m_maxItemInven;	
	//		spAcc->m_Gold					= (DWORD)m_Gold;			
	//		spAcc->m_EE					= (DWORD)m_EE;
	//		spAcc->m_Gem					= (WORD)m_Gem;
	//		spAcc->m_Level					= (WORD)m_Level;
	//		spAcc->m_Stamina				= (WORD) m_Stamina;
	//		spAcc->m_idCurrTheme		= (BYTE)m_idCurrTheme;
	//		spAcc->m_idxCurrScenario	= (char)m_idxCurrScenario;
	//		spAcc->m_idThemeCurr		= (WORD)m_idThemeCurr;
	//		spAcc->m_idxScenarioCurr	= m_idxScenarioCurr;
	//		spAcc->m_idPlayTheme		= (BYTE )m_idPlayTheme;
	//		spAcc->m_Exp					= (DWORD)m_Exp;
	//		spAcc->m_pointSummon		= (DWORD)m_pointSummon;		
	//		spAcc->m_timePassStamina	= (DWORD)m_timePassStamina;
	//		spAcc->m_CurrSN				= (DWORD)SnNum;
	//		if( dataLen > 0 )
	//			spAcc->m_Tutorial.RestorePacket( pTutorial );
	//
	//		wcscpy_s( spAcc->m_szFaceBookUserID , FBUserID );
	//		wcscpy_s( spAcc->m_szFaceBookUserName , FBUserName);
	//
	//		return TRUE;
	//	}
	//	else
	//	{
	//		TCHAR err[1024];
	//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
	//		OutputTextFormat(_T("\n AccountLoadFBUserID Data Error  (err:%d) : %s"), ret, err);
	//	}
	//
	//	return FALSE;
}
//
//#ifdef _xCONTENTS_MOSTER_DIC
//BOOL CUserDB::SaveMonsterDic( XSPDBUAcc spAcc)
//{
//	if( spAcc == NULL )
//		return FALSE;
//
//	ReAllocStmt();
//
//	DWORD IDACCOUNT = spAcc->m_idAccount;	
//	
//	XArchive MonsterDic;
//	SQLUSMALLINT i = 1;
//
//	spAcc->MakeMonsterDicPacket( MonsterDic );
//	SQLLEN		retLen = 0, dataLen = MonsterDic.size(), idnameLen = SQL_NTS;
//
//	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG,		SQL_INTEGER,		sizeof(int),		0, &IDACCOUNT,							0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY,	SQL_VARBINARY,	4000,				0, (SQLPOINTER*)MonsterDic.GetBuffer(),0, &dataLen,i++);		
//	
//	SQLRETURN ret;
//	
//	if ( (ret = Excute (_T("{ call dbo.AccountMonsterDicUpdate( ?,?) }"))) != SQL_SUCCESS ) 		
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n AccountMonsterDicUpdate Error : %s"), err);
//		BREAK_AND_LOG();
//		return FALSE; 
//	}
//	return TRUE;
//}
//#endif
//
//
//
//#ifdef _xCONTENTS_SERVER_EVENT
//BOOL CUserDB::LoadSvrEvt( XServerEventMng* pOut )
//{
//	ReAllocStmt();
//
//	TCHAR szQuery[1024];
//	_stprintf_s( szQuery, 1024, _T("{ call dbo.SvrEvtLoad() }"));
//
//	SQLRETURN	ret = Excute( szQuery);
//	if ( (ret) != SQL_SUCCESS && (ret) != SQL_NULL_DATA ) 
//	{		
//		TCHAR err[1024];
//		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
//		OutputTextFormat(_T("\n SvrEvtLoad Execute Error (err:%d) : %s"), ret, err);
//		BREAK_AND_LOG();
//		return FALSE; 
//	}
//
//	ID index = 0;
//	int eventtype = 0;
//	int eventrate = 0;
//
//	TCHAR startdate[255];
//	TCHAR enddate[255];
//	TCHAR notice[1024];
//	char _dt_str[64];
//	char _et_str[64];
//
//	WORD startyear, startmonth, startday, endyear, endmonth, endday;
//	
//	::memset( &startdate, 0x0, 255);
//	::memset( &enddate, 0x0, 255);
//	::memset( &notice, 0x0, 1024);
//	SQLUSMALLINT i = 1;
//	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;
//
//	ret = BindCol( SQL_C_LONG,		&index,				sizeof(int),		&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&eventtype,			sizeof(int ),		&nLen, 		i++);
//	ret = BindCol( SQL_C_LONG,		&eventrate,			sizeof(int),		&nLen, 		i++);	
//	ret = BindCol( SQL_C_WCHAR,	startdate,				LEN_LOGIN_ID,	&pwLen,		i++);
//	ret = BindCol( SQL_C_WCHAR,	enddate,				LEN_LOGIN_ID,	&nickLen,	i++);
//	ret = BindCol( SQL_C_WCHAR,	notice,					2048,				&pwLen,		i++);	
//
//	while( (ret = Fetch()) == SQL_SUCCESS || (ret = Fetch()) == SQL_SUCCESS_WITH_INFO )//!= SQL_NO_DATA)
//	{
//		int len = WideCharToMultiByte(CP_ACP,0,startdate,-1,NULL,NULL,NULL,NULL);	
//		WideCharToMultiByte(CP_ACP,0,startdate,-1,_dt_str,len,NULL,NULL);	
//		len = WideCharToMultiByte(CP_ACP,0,enddate,-1,NULL,NULL,NULL,NULL);	
//		WideCharToMultiByte(CP_ACP,0,enddate,-1,_et_str,len,NULL,NULL);	
//		_dt_str[4] = _dt_str[7] = '\0';
//		_et_str[4] = _et_str[7] = '\0';
//		 
//		startyear		= (WORD)atoi(&_dt_str[0]);
//		startmonth	= (WORD)atoi(&_dt_str[5]);
//		startday		= (WORD)atoi(&_dt_str[8]);		
//		endyear		= (WORD)atoi(&_et_str[0]);
//		endmonth	= (WORD)atoi(&_et_str[5]);
//		endday		= (WORD)atoi(&_et_str[8]);		
//		SYSTEMTIME now;
//		::GetLocalTime(&now);
//		if( eventtype != 1 )
//		{
//			if( endyear < now.wYear ) 
//				continue;
//			if(( endyear == now.wYear )&& (endmonth < now.wMonth )) 
//				continue;
//			if(( endyear == now.wYear )&& (endmonth == now.wMonth ) && ( endday < now.wDay )) 
//				continue;
//		}
//
//		XServerEventMng::xEventValue* pData = new XServerEventMng::xEventValue;
//		pData->idevent = index;
//		pData->evttype = eventtype;
//		pData->evtrate = eventrate;
//
//		pData->st.wYear = startyear;
//		pData->st.wMonth	= startmonth;
//		pData->st.wDay = startday;
//		pData->et.wYear = endyear;
//		pData->et.wMonth	= endmonth;
//		pData->et.wDay = endday;
//		if( eventtype == 1 )
//			wcscpy_s(pData->Notice , notice );
//
//		if( pOut->AddserverEvent( pData ) == FALSE )
//		{
//			delete pData;
//			pData = NULL;
//		}
//	}
//	return TRUE;
//}
//#endif 
//

/**
xLoad
*/
xtDB CUserDB::AccountLoadIDAccount(XSPDBUAcc spAcc, ID idAccount)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadIDAccount( '%d' ) }"), idAccount);
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return xDB_FAIL;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, freegem = SQL_NTS, freegemstone = SQL_NTS;
	SQLLEN sqllenHello = SQL_NTS;
	SQLLEN sqllenFbUserid = SQL_NTS;
	int			tmpidAccount = 0;
	int			Ver = 0;
	TCHAR	szID[256] = { 0, };
	TCHAR	szName[256] = { 0, };
	TCHAR	szUUID[256] = { 0, };
	TCHAR	szSessionKey[256] = { 0, };
	TCHAR	szFbuserid[256] = {0, };
	TCHAR	szHello[256] = {0, };
	int			CurrSN = 0;
	int			Gold = 0;			// 게임머니
	int			Cashtem = 0;		// 캐쉬 아이템(보석같은..)
	int			AP = 0;
	int			HeroCount = 0;
	short		maxitems = 0;
	int			gmlevel = 0;
	int			lastconnectsvrid = 0;
	int			powerIncludeEmpty = 0;
  int     ladder = 0;

	//	int			nSecPlay1 = 0;
	//	int			nSecPlay2 = 0;

// 	int nTradesec = 0;
// 	int nshopsec = 0;
	int secPassByTrader = 0;
	int secPassByShop = 0;

	int secsubscribe = 0;
	int secsubscribeend = 0;

	SQLLEN	LeveldataLen = 0;
	SQLLEN	herodataLen = 0;
	//	SQLLEN	WorlddataLen = 0;
	SQLLEN	ResourcedataLen = 0;
	SQLLEN	Abilitydatalen = 0;
	SQLLEN	Questdatalen = 0;
	SQLLEN	secPlayLen = 0;
	SQLLEN	shoplistlen = 0;
	SQLLEN	sqllenAttackLog = 0;
	SQLLEN	sqllenDefenseLog = 0;
	SQLLEN	sqllenJoinreqGuild = 0;

	BYTE		LevelBuff[BINARY_BUFF_LEN];
	BYTE		HeroBuff[BINARY_BUFF_LEN];
	//	BYTE		WorldBuff[BINARY_BUFF_LEN];
	BYTE		ResourceBuff[BINARY_BUFF_LEN2];
	BYTE		ShoplistBuff[BINARY_BUFF_LEN2];
	ArchiveSplit items[2];		// 아이템 바이너리 버퍼
	ArchiveSplit worlds[2];		// 월드 바이너리
	BYTE		AbilityBuff[BINARY_BUFF_LEN];
	BYTE		QuestBuff[BINARY_BUFF_LEN];
	BYTE		buffAttackLog[BINARY_BUFF_LEN];
	BYTE		buffDefenseLog[BINARY_BUFF_LEN];
	BYTE		buffJoinreqGuild[BINARY_BUFF_LEN];
	::memset(&LevelBuff, 0x0, BINARY_BUFF_LEN);
	::memset(&HeroBuff, 0x0, BINARY_BUFF_LEN);
	::memset(&ResourceBuff, 0x0, BINARY_BUFF_LEN2);
	::memset(&ShoplistBuff, 0x0, BINARY_BUFF_LEN2);
	XCLEAR_ARRAY(AbilityBuff);
	XCLEAR_ARRAY(QuestBuff);
	XCLEAR_ARRAY(buffAttackLog);
	XCLEAR_ARRAY(buffDefenseLog);

	BYTE		legionBuff[XGAME::MAX_LEGION][BINARY_BUFF_LEN];
	SQLLEN	regiondataLen[XGAME::MAX_LEGION];
	int			LastLogouttick = 0;
	BYTE		tempdata[BINARY_BUFF_LEN];
	SQLLEN	tempdataLen = 0;
	xJEWEL jewelMine[2];	// 광산데이타 정보
	xMandrakeDB dbMandrake[2];
	XINT64 secPlay = 0;		// 총 플레이시간(초)

	TCHAR szNameOwner[2][256] = { 0, };

	int timestamp1 = 0;
	int timestamp2 = 0;

	int guildindex = 0;
	int guildgrade = 0;
	//	ret = BindCol( SQL_C_LONG,		&tmpidAccount,			sizeof(int),					&nLen, 				i++);
	//1
	ret = BindCol(SQL_C_WCHAR, szID, LEN_LOGIN_ID * 2, &pwLen, i++);
	ret = BindCol(SQL_C_WCHAR, szFbuserid, LEN_LOGIN_ID * 2, &sqllenFbUserid, i++);
	//	ret = BindCol( SQL_C_WCHAR,	szPassword,				LEN_LOGIN_ID*2,			&nickLen,				i++);
	ret = BindCol(SQL_C_WCHAR, szUUID, 256 * sizeof(TCHAR), &nickLen, i++);
// 	ret = BindCol(SQL_C_WCHAR, szUUID, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szSessionKey, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &sqllenHello, i++); //Hello 2016.01.20

	//2
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &CurrSN, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Gold, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Cashtem, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &AP, sizeof(int), &nLen, i++);

	//3
	ret = BindCol(SQL_C_LONG, &HeroCount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &gmlevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &LastLogouttick, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &powerIncludeEmpty, sizeof(int), &nLen, i++);
	
	ret = BindCol(SQL_C_LONG, &ladder, sizeof(int), &nLen, i++);

	//4
	ret = BindCol(SQL_C_LONG, &jewelMine[0].idOwner, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, &szNameOwner[0], LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[0].idEnemy, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[0].levelMine, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[0].defense, sizeof(int), &nLen, i++);

	//5
	ret = BindCol(SQL_C_LONG, &jewelMine[0].idxLegion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &timestamp1, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[1].idOwner, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, &szNameOwner[1], LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[1].idEnemy, sizeof(int), &nLen, i++);

	//6
	ret = BindCol(SQL_C_LONG, &jewelMine[1].levelMine, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[1].defense, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewelMine[1].idxLegion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &timestamp2, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].idOwner, sizeof(int), &nLen, i++);

	//7
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].idEnemy, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, &dbMandrake[0].szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].win, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].offwin, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].reward, sizeof(int), &nLen, i++);

	//8
	ret = BindCol(SQL_C_LONG, &dbMandrake[0].idxLegion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].idOwner, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].idEnemy, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, &dbMandrake[1].szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].win, sizeof(int), &nLen, i++);

	//9
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].offwin, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].reward, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &dbMandrake[1].idxLegion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &LevelBuff, BINARY_BUFF_LEN, &LeveldataLen, i++);
	ret = BindCol(SQL_C_BINARY, &HeroBuff, BINARY_BUFF_LEN, &herodataLen, i++);

	//10
	//	ret = BindCol( SQL_C_BINARY,	&WorldBuff,							BINARY_BUFF_LEN,			&WorlddataLen, 	i++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			ret = BindCol(SQL_C_BINARY, &worlds[idx].buff, BINARY_BUFF_LEN, &worlds[idx].sqlLen, i++);
	}
	ret = BindCol(SQL_C_BINARY, &ResourceBuff, BINARY_BUFF_LEN2, &ResourcedataLen, i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[0], BINARY_BUFF_LEN, &regiondataLen[0], i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[1], BINARY_BUFF_LEN, &regiondataLen[1], i++);

	//11
	ret = BindCol(SQL_C_BINARY, &legionBuff[2], BINARY_BUFF_LEN, &regiondataLen[2], i++);
	ret = BindCol(SQL_C_BINARY, &tempdata, BINARY_BUFF_LEN, &tempdataLen, i++);
	// 아이템
	{
		int num = XNUM_ARRAY(items);
		for (int idx = 0; idx < num; ++idx)
			ret = BindCol(SQL_C_BINARY, &items[idx].buff, BINARY_BUFF_LEN, &items[idx].sqlLen, i++);
	}
	ret = BindCol(SQL_C_BINARY, &AbilityBuff, BINARY_BUFF_LEN, &Abilitydatalen, i++);

	//12
	ret = BindCol(SQL_C_BINARY, &QuestBuff, BINARY_BUFF_LEN, &Questdatalen, i++);
	ret = BindCol(SQL_C_SBIGINT, &secPlay, sizeof(XINT64), &secPlayLen, i++);	// 32비트로 바꿔도 됨.
	ret = BindCol(SQL_C_SHORT, &maxitems, sizeof(short), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &secPassByTrader, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &secPassByShop, sizeof(int), &nLen, i++);

	ret = BindCol(SQL_C_BINARY, &ShoplistBuff, BINARY_BUFF_LEN2, &shoplistlen, i++);
	ret = BindCol(SQL_C_LONG, &guildindex, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &guildgrade, sizeof(int), &nLen, i++);

	// xblog
	ret = BindCol(SQL_C_BINARY, &buffAttackLog, BINARY_BUFF_LEN, &sqllenAttackLog, i++);
	ret = BindCol(SQL_C_BINARY, &buffDefenseLog, BINARY_BUFF_LEN, &sqllenDefenseLog, i++);
	ret = BindCol(SQL_C_BINARY, &buffJoinreqGuild, BINARY_BUFF_LEN, &sqllenJoinreqGuild, i++);

	ret = BindCol(SQL_C_LONG, &secsubscribe, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &secsubscribeend, sizeof(int), &nLen, i++);

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if ((DWORD)idAccount > 0)  spAcc->m_idAccount = (DWORD)idAccount;
		if (Ver > 0) spAcc->m_Ver = Ver;
		int size = (int)::wcslen(szID);
		if (size > 0) spAcc->m_strID = szID;
		//		size = ( int )::wcslen( szPassword );
		//		if( size > 0 ) spAcc->m_strPassword = szPassword;
		spAcc->m_strFbUserId = szFbuserid;
		spAcc->m_strHello = szHello;
		size = (int)::wcslen(szName);
		if (size > 0) spAcc->m_strName = szName;
		size = (int)::wcslen(szUUID);
		if (maxitems > 0) spAcc->m_maxItems = maxitems;
		if (size > 0) spAcc->m_strUUID = szUUID;
		size = (int)::wcslen(szSessionKey);
		if (size > 0) spAcc->m_StrSessionKey = szSessionKey;
		if (CurrSN > 0) spAcc->m_CurrSN = CurrSN;
		if (Gold > 0) spAcc->m_Gold = Gold;
		if (Cashtem > 0) spAcc->m_Cashtem = Cashtem;
		//		if( AP > 0 ) spAcc->m_AP = AP;
		if (gmlevel > 0) spAcc->m_GMLevel = gmlevel;
		if (guildindex > 0) spAcc->SetGuildIndex(guildindex);
		if (guildgrade > 0) spAcc->SetGuildgrade((XGAME::xtGuildGrade)guildgrade);
//		spAcc->m_secSaved = (xSec)LastLogouttick;
		spAcc->m_PowerIncludeEmpty = powerIncludeEmpty;
		spAcc->m_PowerExcludeEmpty = 0;	// 재계산하게 하기위해 0으로 넣음.
		spAcc->m_Ladder = ladder;
		//		if( lastconnectsvrid > 0 ) spAcc->m_lastconnectsvrid = lastconnectsvrid ;
		{
			XArchive arAcc;
			if (tempdataLen < 0)	// 이경우는 보통 DB에 계정은 만들었으나 디폴트 데이타를 만드는데 실패한경우.
				return xDB_INVALID_ACCOUNT;
			if (XASSERT(tempdataLen)) {
				arAcc.SetBufferMem(tempdata, tempdataLen);
				spAcc->DeSerializeEtcData(arAcc);
			}
		} {
		// XArchive이 스택을 많이 써서 스택 오버플로우 날까봐 따로따로 블럭화 시킴.-xuzhu-
			XArchive pBuff; //Leveldata
			if (LeveldataLen < 0)	// 이경우는 보통 DB에 계정은 만들었으나 디폴트 데이타를 만드는데 실패한경우.
				return xDB_INVALID_ACCOUNT;
			if (XASSERT(LeveldataLen > 0)) {
				pBuff.SetBufferMem(LevelBuff, LeveldataLen);
				//				spAcc->GetLevel().DeSerialize( pBuff );
				spAcc->DeSerializeLevel(pBuff);
			}
		} {
			XArchive arBuff(0x10000);		// 아카이브 크기가 커서 동적할당 받음.
			int offset = 0;
			int num = XNUM_ARRAY(items);
			for (int i = 0; i < num; ++i) {
				if (items[i].sqlLen > 0) {
					arBuff.CopyFromBinary(offset, items[i].buff, items[i].sqlLen);
					offset += items[i].sqlLen;
				}
			}
			if (offset > 0)
				spAcc->DeSerializeItems(arBuff);
		}{
			XArchive pBuff2; //HeroBuff
			if (XASSERT(herodataLen > 0)) {
				pBuff2.SetBufferMem(HeroBuff, herodataLen);
				if (HeroCount > 0) {
					spAcc->RestoreArchiveHeros(pBuff2);
				}
			}
		}{
			XArchive arWorld(0x10000); //Worlddata. 아카이브 크기가 커서 동적할당 받음.
			arWorld.SetbForDB(true);
			int offset = CombineSplitData(arWorld, worlds, Ver );
			if (offset > 0) {
				SAFE_DELETE(spAcc->m_pWorld);
				spAcc->m_pWorld = new XWorld;
				if( spAcc->m_pWorld->DeSerialize(arWorld) == 0 )
					return xDB_FAIL;
				// 로딩한 보석광산 정보 입력.
				jewelMine[0].secLastEvent = (float)timestamp1;
				jewelMine[1].secLastEvent = (float)timestamp2;
				for (int i = 0; i < 2; ++i) {
					auto pJewel = spAcc->GetpWorld()->GetSpotJewelByIdx(i + 1);
					if (pJewel) {
						pJewel->m_strName = szNameOwner[i];
						pJewel->m_idOwner = (ID)jewelMine[i].idOwner;
						pJewel->m_idMatchEnemy = (ID)jewelMine[i].idEnemy;
						pJewel->m_levelMine = jewelMine[i].levelMine;
						pJewel->m_Defense = jewelMine[i].defense;
						pJewel->m_idxLegion = jewelMine[i].idxLegion;
						pJewel->m_secLastEvent = jewelMine[i].secLastEvent;
						if( pJewel->m_idMatchEnemy && pJewel->m_strName.empty() )
							pJewel->m_strName = _T("누군가");
					}
					// 스팟에 만드레이크 스팟 정보 입력
					auto pMandrake = spAcc->GetpWorld()->GetSpotMandrakeByIdx(i + 1);
					if (pMandrake) {
						pMandrake->m_idOwner = dbMandrake[i].idOwner;
						pMandrake->m_idMatchEnemy = dbMandrake[i].idEnemy;
						pMandrake->m_Win = dbMandrake[i].win;
						pMandrake->m_Reward = dbMandrake[i].reward;
						pMandrake->m_idxLegion = dbMandrake[i].idxLegion;
						pMandrake->m_OffWin = dbMandrake[i].offwin;
						pMandrake->m_strName = dbMandrake[i].szName;
					}
				}
			}
		} {
			XArchive pBuff4; //Resourcedata
			if (XASSERT(ResourcedataLen > 0)) {
				pBuff4.SetBufferMem(ResourceBuff, ResourcedataLen);
				spAcc->RestoreResourcePacket(pBuff4);
			}
		} {
			// 군단정보 바이너리 읽기
			XArchive packetLegion[XGAME::MAX_LEGION];
			for (int i = 0; i < XGAME::MAX_LEGION; ++i) {
				if (regiondataLen[i] > 0) {
					packetLegion[i].SetBufferMem(legionBuff[i], regiondataLen[i]);
					spAcc->RestoreLegionPacketFullToLink(packetLegion[i], i);
				}
			}
		} {
			XArchive arAbility;
#ifdef _DEV_SERVER
			{
#else
			if (XASSERT(Abilitydatalen > 0)) {
#endif
				arAbility.SetBufferMem(AbilityBuff, Abilitydatalen);
				XTRACE( "%s", __TFUNC__ );
				spAcc->DeserializeAbil(arAbility);
			}
		} {
			XArchive arQuest;
			if (Questdatalen > 0) {
				arQuest.SetBufferMem(QuestBuff, Questdatalen);
				spAcc->DeSerializeQuest(arQuest);
			}

		} {
			XArchive arAttackLog;
			if (sqllenAttackLog > 0) {
				arAttackLog.SetBufferMem(buffAttackLog, sqllenAttackLog);
				spAcc->DeSerializeAttackLog(arAttackLog);
			}
		} {
			XArchive arDefenseLog;
			if (sqllenDefenseLog > 0) {
				arDefenseLog.SetBufferMem(buffDefenseLog, sqllenDefenseLog);
				XArchive arDefense;
				XArchive arEncounter;
// 				bool bFlag = true;
// 				if( bFlag ) {
					arDefenseLog >> arDefense;
					arDefenseLog >> arEncounter;
					spAcc->DeSerializeDefenseLog( arDefense );
					spAcc->DeSerializeEncounter( arEncounter );
	// 				spAcc->DeSerializeDefenseLog(arDefenseLog);
//				}
			}
		}
		if (spAcc->GetGuildIndex() == 0)	{
			XArchive arJoinreqGuild;
			if (sqllenJoinreqGuild > 0) {
				arJoinreqGuild.SetBufferMem(buffJoinreqGuild, sqllenJoinreqGuild);
				if( arJoinreqGuild.IsHave() )
					spAcc->DeSerializeJoinReqGuild(arJoinreqGuild);
			}
		}			
		// 총 플레이 시간 로딩.
		spAcc->UpdatePlayTimer((xSec)secPlay);

		if( spAcc->m_Ver <= 2 ) {
			spAcc->OffTimerByTrader();
			spAcc->m_timerShop.Off();
		} else {
			const xSec secSaved = LastLogouttick;
			spAcc->m_timerTrader.RestoreTimer( secSaved, secPassByTrader );
			spAcc->m_timerShop.RestoreTimer( secSaved, secPassByShop );
			if( shoplistlen > 0 ) {
				XArchive arShop;
				arShop.SetBufferMem( ShoplistBuff, shoplistlen );
				spAcc->DeSerializeShopList( arShop );

			}

		}
// 		if (nshopsec > 0) {
// 			if (nowtime < (DWORD)nshopsec) {
// 				nshopsec -= nowtime;
// 				spAcc->SetsecShop(nshopsec);
// 				spAcc->GetShopTimer()->Set((float)nshopsec);
// 				XArchive arShop;
// 				if (shoplistlen > 0) {
// 					arShop.SetBufferMem(ShoplistBuff, shoplistlen);
// 					spAcc->DeSerializeShopList(arShop);
// 				}
// 			} else {
// 				spAcc->SetsecShop(1);
// 				spAcc->GetShopTimer()->Set((float)1);
// 				/*spAcc->SetsecShop(XGC->m_shopCallInerval);
// 				spAcc->GetShopTimer()->Set((float)(XGC->m_shopCallInerval));
// 				spAcc->ChangeShopItemList();*/
// 			}
// 		}	else {
// 			spAcc->SetsecShop(1);
// 			spAcc->GetShopTimer()->Set((float)1);
// 		}

// 		if (nTradesec > 0) {
// 			if (nowtime < (DWORD)nTradesec) {
// 				nTradesec -= nowtime;
//  				spAcc->SetsecTrade(nTradesec);
//  				spAcc->GetTradeTimer()->Set((float)nTradesec);
// 			}
// 		}
		DWORD nowtime = (DWORD)( timeGetTime() / TICKS_PER_SEC );
		int nCount = 0;
		DWORD PassTime = 0;	
		//접종 중 구독 상품 줘야 한다면 여기서 계산 할것.
		if (secsubscribeend > 0) {
			int nInterval = XGC->m_subscribeGemInterval;
			if (nInterval == 0)
				nInterval = XGAME::SUBSCRIBE_INTERVAL;
			if (nowtime > (DWORD)secsubscribeend) {
				//만료됨
				PassTime = secsubscribeend - secsubscribe;				
				nCount = int(PassTime / nInterval);
			} else { 
				//만료 되지 않음.
				if (secsubscribe == 0)
					secsubscribe = nInterval;

				if (nowtime > (DWORD)secsubscribe) {
					PassTime = nowtime - secsubscribe;
					nCount = int(PassTime / nInterval ) + 1;					
				}
			}

			if (nCount > 0) spAcc->AddSubscribeOffline(nCount);
			if (nowtime < (DWORD)secsubscribeend) { //만료 되지 않았음.
				secsubscribeend -= nowtime;
				spAcc->SetsecSubscribeEnd(secsubscribeend);
				spAcc->GetSubscribeEndTimer()->Set((float)secsubscribeend);
				if (secsubscribe > 0) {
					if ((DWORD)secsubscribe > nowtime ) {
						secsubscribe -= nowtime;				
					} else {
						PassTime = nowtime - secsubscribe;
						if (PassTime > DWORD( nInterval))
							secsubscribe = PassTime % nInterval;
						else
							secsubscribe = PassTime;
					}
				} else {
					secsubscribe = nInterval;
				}
				spAcc->SetsecSubscribe(secsubscribe);
				spAcc->GetSubscribeTimer()->Set((float)secsubscribe);
			}			
		}
		
// 		if (nshopsec > 0) {
// 			if (nowtime < (DWORD)nshopsec) {
// 				nshopsec -= nowtime;
// 				spAcc->SetsecShop(nshopsec);
// 				spAcc->GetShopTimer()->Set((float)nshopsec);
// 				XArchive arShop;
// 				if (shoplistlen > 0) {
// 					arShop.SetBufferMem(ShoplistBuff, shoplistlen);
// 					spAcc->DeSerializeShopList(arShop);
// 				}
// 			} else {
// 				spAcc->SetsecShop(1);
// 				spAcc->GetShopTimer()->Set((float)1);
// 				/*spAcc->SetsecShop(XGC->m_shopCallInerval);
// 				spAcc->GetShopTimer()->Set((float)(XGC->m_shopCallInerval));
// 				spAcc->ChangeShopItemList();*/
// 			}
// 		}	else {
// 			spAcc->SetsecShop(1);
// 			spAcc->GetShopTimer()->Set((float)1);
// 		}
		return xDB_OK;
	} else
	if( ret == SQL_NO_DATA ) {
		return xDB_NOT_FOUND;
	} else {
		// 치명적 에러.
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}

	return xDB_FAIL;
}

/**
@brief pAry에 분할되어 저장된 바이너리를 합쳐서 arOut에 넣는다.
*/
// int CUserDB::CombineSplitData(XArchive& arOut, ArchiveSplit *pAry, int numAry) const
// {
// 	int offset = 0;
// 	int num = numAry;
// 	for (int i = 0; i < num; ++i)
// 	{
// 		if (pAry[i].sqlLen > 0)	{
// 			arOut.CopyFromBinary(offset, pAry[i].buff, pAry[i].sqlLen);
// 			offset += pAry[i].sqlLen;
// 		}
// 	}
// 	return offset;
// }
/**
 @brief 쪼개져있는 데이타 pAry를 하나로 합쳐서 arOut에 넣어준다.
 @return 합친 데이터의 바이트 크기
*/
int CUserDB::CombineSplitData(XArchive& arOut
														, ArchiveSplit *pAry
														, const int numAry
														, int verAcc ) const
{
	XBREAK( verAcc <= 0 );
	const DWORD dwIdentifier = 0x12345678;
	const auto sizeHeader = 4 + 4;
	int offset = 0;
	DWORD* pIdentifier = (DWORD*)pAry[0].buff;
	const bool bCompress = (*pIdentifier == dwIdentifier);
	const int sizeUncompressed = (bCompress)? pIdentifier[1] : 0;		// 압축되기전 원본 크기
//	XBREAK( bCompress && sizeUncompressed <= 32 );		// 숫자가 너무 작음. 뭔가 이상함.(32바이트 이하짜리를 압축했다면 몰라도)
	//
	if( bCompress ) {
		// 압축되어 쪼개진 데이터처리
		BYTE* pBuffUnCompressed = nullptr;
		{
			// 병합하고 압축해제
			int ofsDst = 0;		// buffCombined의 오프셋
			BYTE buffCombined[ 0x10000 ];		// 쪼개져있던 메모리를 하나로 합친것(압축된데이터). 현재 최대 64k까지만 지원.
			//
			int sizeCompressed = 0;
			for( int i = 0; i < numAry; ++i )	{
				XASSERT( bCompress == true );
				const int sizeSrcCopy = ( i == 0 ) ?		// 카피해야할 바이트수
																	pAry[ i ].sqlLen - sizeHeader
																	: pAry[ i ].sqlLen;
				if( sizeSrcCopy > 0 )	{
					XBREAK( i > 0 );		// 압축데이터가 두개이상으로 나눠진건 테스트해본적없으니 테스트할것.
					BYTE* pSrc = pAry[ i ].buff;
					if( i == 0 ) {
						// 첫번째 조각은 헤더가 있으므로 건너뜀
						pSrc += sizeHeader;
					}
					XBREAK( sizeSrcCopy > sizeof(buffCombined) );
					memcpy_s( buffCombined + ofsDst, sizeof( buffCombined ), pSrc, sizeSrcCopy );
					sizeCompressed += sizeSrcCopy;
					ofsDst += sizeSrcCopy;
				}
			} // for
			XASSERT( sizeCompressed > 0 );
			XASSERT( ofsDst == sizeCompressed );
			// 합쳐진 데이터를 압축해제
			XSYSTEM::UnCompressMem( &pBuffUnCompressed
														, sizeUncompressed
														, buffCombined
														, sizeCompressed );
		}
		// 압축푼것을 아카이브로 카피
		arOut.CopyFromBinary( 0, pBuffUnCompressed, sizeUncompressed );
		offset = sizeUncompressed;
		SAFE_DELETE_ARRAY( pBuffUnCompressed );
	} else {
		// 압축안된 쪼개진 메모리
		for (int i = 0; i < numAry; ++i)	{
			const int sizeSrc = pAry[ i ].sqlLen;
			if (sizeSrc > 0)	{
				BYTE* pSrc = pAry[i].buff;
				arOut.CopyFromBinary(offset, pSrc, sizeSrc );
				offset += sizeSrc;
			}
		}
	}
	return offset;
}

/**
@brief 아카이브ar을 8000바이트씩 쪼개서 pAry에 담아준다.
*/
void CUserDB::SplitArchive( ArchiveSplit *pAry, int numAry, XArchive& ar ) const
{
	{
		int offset = 0;
		for( int i = 0; i < numAry; ++i ) {
			if( false == ar.CopyToBinary( pAry[ i ].buff, offset, 8000 ) )
				break;
			offset += 8000;
		}
	}
	const int size = ar.size();
	int div = size / 8000;
	int mod = size % 8000;
	int i;
	XBREAK(div > numAry);
	for (i = 0; i < div; ++i)
		pAry[i].sqlLen = 8000;
	if (mod > 0)
		pAry[i].sqlLen = mod;
}

/**
 @brief 압축된 버퍼를 8000바이트단위로 잘라서 pAry에 담는다.
 저장 포맷이 변하면 VER_SERIALIZE를 수정해서 할것.
*/
void CUserDB::SplitBuffWithCompress( ArchiveSplit *pAry
																	, int numAry
																	, BYTE* pSrcCompressed
																	, int bytesSrcCompressed
																	, int bytesSrcUnCompressed ) const
{
	XBREAK( bytesSrcCompressed > 8000 );	// 압축된 데이터가 두개이상으로 쪼개져 저장되ㅑ는건 테스트 안해봤으니 해볼것.
	const int _sizeHeader = 4 + 4;
	int ofsSrc = 0;
	for( int i = 0; i < numAry; ++i ) {
		int ofsDst = 0;
		int bytesCopy = 8000;
		if( i == 0 ) {
			//저장 포맷이 변하면 VER_SERIALIZE를 수정해서 할것.
			// 압축포맷으로 저장해야하면 첫번째 버퍼에 헤더를 저장.
			const DWORD dwIdentifier = 0x12345678;		// 압축 identifier
			BYTE* pBuff = pAry[ 0 ].buff;
			*((DWORD*)pBuff) = dwIdentifier;
			ofsDst += 4;
			*((DWORD*)(pBuff + ofsDst)) = bytesSrcUnCompressed;		// 압축되기전 크기
			ofsDst += 4;
			bytesCopy -= _sizeHeader;		// 첫번째 버퍼는 헤더길이 빼고 카피한다.
		}
		// 8000바이트씩 pAry.buff에 카피한다. 더이상 용량이 없으면 짜투리 카피하고 빠져나간다.
		int bytesRemainSrc = bytesSrcCompressed - ofsSrc;
		if( bytesRemainSrc < bytesCopy )
			bytesCopy = bytesRemainSrc;
		XBREAK( ofsDst + bytesCopy > pAry[i].size() );		// dst버퍼 오버플로우됨.
		memcpy_s( pAry[i].buff + ofsDst
						, pAry[i].size() - ofsDst
						, pSrcCompressed + ofsSrc
						, bytesCopy );
		// 전체 아카이브크기는 헤더 포함한 크기여야 한다.
		pAry[i].sqlLen = bytesCopy + ((i == 0)? _sizeHeader : 0);
		ofsSrc += bytesCopy;
		if( bytesCopy < 8000 )		// 짜투리 카피했었으면 끝난것이므로 빠져나감.
			break;
	}
	XASSERT( ofsSrc == bytesSrcCompressed );			// 검증.
}

/**
 @brief uuid로 계정을 찾아서 아카이브에 담는다.
*/
ID CUserDB::FindAccountByUUID( LPCTSTR tUUID, XArchive *pOutAr )
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadUUID( '%s' ) }"), tUUID);

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadUUID Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return 0;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0;
	SQLLEN nickLen = SQL_NTS;

	int			tmpidAccount = 0;
	int			Ver = 0;
	int			lastconnectsvrid = 0;
	ID			BattleTargetID = 0;
	int			BattleTime = 0;
	int			GMLevel = 0;
	TCHAR		szNickName[LEN_LOGIN_ID];
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szNickName, LEN_LOGIN_ID, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &BattleTargetID, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &BattleTime, sizeof(int), &nLen, i++);	
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		if( tmpidAccount <= 0 )
			return 0;
		(*pOutAr) << (ID)tmpidAccount;
		(*pOutAr) << (WORD)Ver;
		(*pOutAr) << (WORD)lastconnectsvrid;
		(*pOutAr) << GMLevel;
		(*pOutAr) << szNickName;
		(*pOutAr) << BattleTargetID;
		(*pOutAr) << BattleTime;
		return (ID)tmpidAccount;
	} else
	if( ret == SQL_NO_DATA ) {
		(*pOutAr) << 0;
		return 0;
	}	else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}

	return 0;
}

/**
 @brief ID로 계정을 검색하여 아카이브에 담는다.
*/
ID CUserDB::FindAccountByID( const _tstring& strID, XArchive *pOutAr)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadIDName( '%s' ) }"), strID.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadIDName Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return 0;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, pwLen = SQL_NTS, nickLen = SQL_NTS;;
	int			tmpidAccount = 0;
	int			Ver = 0;
	int			lastconnectsvrid = 0;
	int			GMLevel = 0;
	TCHAR		szPW[1024];
	TCHAR		szNickName[LEN_LOGIN_ID];
	int			battleidaccount = 0;
	int			battleendtime = 0;
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szPW, LEN_LOGIN_ID * 2, &pwLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);
	ret = BindCol( SQL_C_LONG, &battleidaccount, sizeof( int ), &nLen, i++ );
	ret = BindCol( SQL_C_LONG, &battleendtime, sizeof( int ), &nLen, i++ );
	ret = BindCol(SQL_C_WCHAR, szNickName, LEN_LOGIN_ID, &nickLen, i++);
#ifdef _XBOT
	XBREAK( XE::IsEmpty(szNickName) );
#endif // _XBOT

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		if( tmpidAccount <= 0 )
			return 0;
		(*pOutAr) << (ID)tmpidAccount;
		(*pOutAr) << (WORD)Ver;
		(*pOutAr) << ((lastconnectsvrid > 0)? (WORD)lastconnectsvrid : (WORD)0);
		(*pOutAr) << GMLevel;
		(*pOutAr) << szNickName;
		(*pOutAr) << szPW;
 		(*pOutAr) << battleidaccount;
 		(*pOutAr) << battleendtime;
		return (ID)tmpidAccount;
	} else
	if( ret == SQL_NO_DATA ) {
		(*pOutAr) << 0;	// 추가
		return 0;
	}	else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}
	return 0;
} // ID CUserDB::FindAccountByID( const _tstring& strID, XArchive *pOutAr)

/**
 @brief Facebook userid로 계정을 검색하여 아카이브에 담는다.
 @param strFbUserId 페이스북계정의 fb_userid. 포맷예시=>"100000786878603"
 @param pOutAr 결과값을 담을 아카이브
 @return 검색된 계정의 idAcc
*/
ID CUserDB::FindAccountByFacebook( const _tstring& strFbUserId, XArchive *pOutAr )
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ call dbo.AccountLoadByFbUserId( '%s' ) }"), strFbUserId.c_str());
	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n query:[%s] Execute Error (err:%d) : %s"), szQuery, ret, err);
		BREAK_AND_LOG();
		return 0;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, pwLen = SQL_NTS, nickLen = SQL_NTS;;
	int			tmpidAccount = 0;
	int			Ver = 0;
	int			lastconnectsvrid = 0;
	int			GMLevel = 0;
	TCHAR		szNickName[LEN_LOGIN_ID];
	int			battleidaccount = 0;
	int			battleendtime = 0;
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);
	ret = BindCol( SQL_C_LONG, &battleidaccount, sizeof( int ), &nLen, i++ );
	ret = BindCol( SQL_C_LONG, &battleendtime, sizeof( int ), &nLen, i++ );
	ret = BindCol(SQL_C_WCHAR, szNickName, LEN_LOGIN_ID, &nickLen, i++);
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		if( tmpidAccount <= 0 )
			return 0;
		(*pOutAr) << (ID)tmpidAccount;
		(*pOutAr) << (WORD)Ver;
		(*pOutAr) << ((lastconnectsvrid > 0)? (WORD)lastconnectsvrid : (WORD)0);
		(*pOutAr) << GMLevel;
		(*pOutAr) << szNickName;
		(*pOutAr) << battleidaccount;
		(*pOutAr) << battleendtime;
		return (ID)tmpidAccount;
	} else
	if( ret == SQL_NO_DATA ) {
		(*pOutAr) << 0;
		return 0;
	}	else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}
	return 0;
}

bool CUserDB::AccountNickNameDuplicateCheck( const _tstring& strNickName)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLRETURN ret;

	int nicklen = strNickName.length();
	int idaccount = 0;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;

	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &idaccount, 0, &nLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicklen, 0, (SQLPOINTER*)strNickName.c_str(), 0, &nickLen, i++);

	ret = Excute(_T("{ ?= call dbo.AccountNickNameDuplicate( ? ) }"));

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)	
		if (idaccount > 0) {
			return false;
		}
		return true;
	}	else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountNickNameDuplicate Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
	}
	return false;

}
BOOL CUserDB::AccountNickNameRegist(ID idaccount, _tstring  strNickName)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	int nicklen = strNickName.length();
	SQLLEN NicknameLen = SQL_NTS;
	SQLLEN nLen = 0;
	//1
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &nLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicklen, 0, (SQLPOINTER*)strNickName.c_str(), 0, &NicknameLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.AccountNickNameChange( ?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountNickNameChange Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

BOOL CUserDB::AccountLoadSessionUUID(XSPDBUAcc spAcc, _tstring strUUID, _tstring strSessionKey )
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadSessionUUID( '%s', '%s' ) }"), strUUID.c_str(), strSessionKey.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadSessionUUID Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0;
	int	tmpidAccount = 0;
	int	Ver = 0;
	int	lastconnectsvrid = 0;
	int	GMLevel = 0;
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if ((DWORD)tmpidAccount > 0)  spAcc->m_idAccount = (DWORD)tmpidAccount;
		if (Ver > 0) spAcc->m_Ver = Ver;
		if (GMLevel  > 0) spAcc->m_GMLevel = GMLevel;
		if (lastconnectsvrid > 0) spAcc->m_idLastConnectSvr = lastconnectsvrid; //서버 중복 접속 허용 막을때 여기 주석 풀어주자.
		return TRUE;
	}
	else
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadSessionUUID Data Error  (err:%d) : %s"), ret, err);
		//		BREAK_AND_LOG();
	}

	return FALSE;
}

BOOL CUserDB::AccountLoadSessionIDName(XSPDBUAcc spAcc, _tstring strIDName, _tstring strSessionKey)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadSessionIDName( '%s', '%s' ) }"), strIDName.c_str(), strSessionKey.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadSessionIDName Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0;
	int	tmpidAccount = 0;
	int	Ver = 0;
	int	lastconnectsvrid = 0;
	int	GMLevel = 0;
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &GMLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &lastconnectsvrid, sizeof(int), &nLen, i++);

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if ((DWORD)tmpidAccount > 0)  spAcc->m_idAccount = (DWORD)tmpidAccount;
		if (Ver > 0) spAcc->m_Ver = Ver;
		if (GMLevel  > 0) spAcc->m_GMLevel = GMLevel;
		if (lastconnectsvrid > 0) spAcc->m_idLastConnectSvr = lastconnectsvrid; //서버 중복 접속 허용 막을때 여기 주석 풀어주자.
		return TRUE;
	}
	else
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountLoadSessionIDName Data Error  (err:%d) : %s"), ret, err);
		//		BREAK_AND_LOG();
	}

	return FALSE;
}

/**
 @brief 접속한 게임서버의 아이디를 DB에 기록한다.
*/
BOOL CUserDB::UpdateAccountLogin(ID idAccount, ID gameserverid, _tstring strConnectIP)
{
	if (idAccount == 0) return FALSE;
	if (gameserverid == 0) return FALSE;
	if (strConnectIP.length() == 0) strConnectIP = _T("Unknown IP");

	ReAllocStmt();

	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &gameserverid, 0, &retLen, i++);
	
	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountUpdateLogin(?, ?, '%s') }"), strConnectIP.c_str());	
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUpdateLogin Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
 @brief 접속종료시간을 기록하고 connectsvrid를 0으로 저장한다.
*/
BOOL CUserDB::UpdateAccountLogOut(ID idAccount)
{
	if (idAccount == 0) return FALSE;

	ReAllocStmt();

	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);


	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.AccountUpdateLogOut(?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUpdateLogOut Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
@brief 아이템 아카이브를 8000바이트로 쪼개줌
스택이 오버되서 별도의 함수로 분리시킴
*/
void CUserDB::ItemsToSplit(XSPAcc pAcc, ArchiveSplit *pOut, int numAry)
{
	XArchive arItems;
	pAcc->SerializeItems(arItems);
	SplitArchive(pOut, numAry, arItems);
}

/**
@brief 월드 아카이브를 8000바이트로 쪼개줌
스택이 오버되서 별도의 함수로 분리시킴
*/
bool CUserDB::WorldToSplit(XSPAcc pAcc, ArchiveSplit *pOut, int numAry)
{
	XArchive arWorld( 0x10000 );	//모든 지역을 다 열면 용량이 꽤 나가기땜에 최악의 상황을 고려해서 크게 잡음.
	arWorld.SetbForDB(true);			// DB저장용 아카이브인것을 알림.
	pAcc->m_pWorld->Serialize(arWorld);
	const int sizeAr = arWorld.size();
	if( XBREAK(sizeAr >= pOut->size() * numAry * 2 ) )	// 압축된후의 크기가 저장될때 크기이므로 압축전 크기는 그 두배로 잡음.
		return false;
	WorldToSplit( arWorld, pOut, numAry );
	int sizeArComp = 0;
	for( int i = 0; i < numAry; ++i )
		sizeArComp += pOut[i].sqlLen;
	XTRACE( "save: world size=%d/%d"
												, sizeArComp
												,sizeAr );
	return true;
}

bool CUserDB::WorldToSplit( XArchive& arWorld, ArchiveSplit *pOut, int numAry )
{
	//	SplitArchive( pOut, numAry, arWorld );
	const auto sizeAr = arWorld.size();
	BYTE *pBuff = nullptr;
	DWORD sizeBuff = 0;
	XSYSTEM::CreateCompressMem( &pBuff
														, &sizeBuff
														, arWorld.GetBuffer()
														, sizeAr );
	if( XASSERT( pBuff ) ) {
		if( XBREAK( sizeBuff > 16000 ) )	// 압축된 후 사이즈가 16k를 넘어가면 저장하지 않음.
			return false;
		// 압축된 pBuff의 바이너리를 8000바이트씩 쪼개서 ArchiveSplit에 담아준다.
		const bool bCompress = true;
		SplitBuffWithCompress( pOut, numAry, pBuff, (int)sizeBuff, sizeAr );
	}
	return true;
}
/**
xSave
*/
BOOL CUserDB::AccountUpdateAll(XSPDBUAcc spAcc) //모든 정보를 한번에 저장 한다.
{
	if (spAcc == NULL) return FALSE;

	ReAllocStmt();

	DWORD IDACCOUNT = spAcc->m_idAccount;
	SQLUSMALLINT sqlcount = 1;
	XArchive arQuest;
	spAcc->SerializeQuest(arQuest);
	SQLLEN sqllenQuest = arQuest.size();

	XArchive arAbility;
	XTRACE( "%s", __TFUNC__ );
	spAcc->SerializeAbil(arAbility);
	SQLLEN sqllenAbilityBin = arAbility.size();

	XArchive ResourcePacket;
	spAcc->MakeResourcePacket(ResourcePacket);
	SQLLEN		retLen = 0, ResourceLen = ResourcePacket.size(), idnameLen = SQL_NTS;

	XArchive LevelInfoPacket;
	spAcc->m_Level.Serialize(LevelInfoPacket);

	XArchive arShopListInfo;
	spAcc->SerializeShopList(arShopListInfo);
	SQLLEN sqllenshoplist = arShopListInfo.size();

	XArchive arBattleLogAttack;
	spAcc->SerializeAttackLog(arBattleLogAttack);
	SQLLEN sqllenAttackLog = arBattleLogAttack.size();
	XArchive arBattleLogDefense;
	{
		XArchive arDefense;
		XArchive arEncounter;		// encounter정보는 DB에 저장하지 않음.
//		spAcc->SerializeDefenseLog( arBattleLogDefense );
		spAcc->SerializeDefenseLog( arDefense );
		spAcc->ClearEncounter();		// 저장할땐 인카운터 정보 저장안함.
		spAcc->SerializeEncounter( arEncounter );
		arBattleLogDefense << arDefense;
		arBattleLogDefense << arEncounter;
	}
	SQLLEN sqllenDefenseLog = arBattleLogDefense.size();
	const _tstring strHello = spAcc->m_strHello;

	XArchive arJoinreq;
	spAcc->SerializeJoinReqGuild(arJoinreq);
	SQLLEN sqllenJoinreq = arJoinreq.size();

	// 아이템 아카이브를 8000바이트씩 잘라서 저장한다.
	ArchiveSplit items[2];
	//	SplitArchive( items, arItems );
	ItemsToSplit(spAcc, items, XNUM_ARRAY(items));

	int	Herocount = spAcc->m_listHero.size();
	short MaxItemCount = (short)spAcc->m_maxItems;
	XArchive HeroInfoPacket;
	spAcc->MakeArchiveHeros(HeroInfoPacket);

	int secSaved = (int)XTimer2::sGetTime();		// 저장당시의 시간을 저장
//	int secSaved = (int)secSaved;
	// 	XArchive WorldPacket;
	// 	spAcc->m_pWorld->Serialize( WorldPacket );
	ArchiveSplit worlds[2];
	//	SplitArchive( worlds, WorldPacket );
	auto bOk = WorldToSplit(spAcc, worlds, XNUM_ARRAY(worlds));
	if( !bOk )
		return FALSE;
	int powerIncludeEmpty = spAcc->GetPowerIncludeEmpty();
	int ladder = spAcc->GetLadder();

	// 군단정보를 풀버전으로 따로 저장. 이것은 타유저가 DB에서 군단정보를 필요로 할때 빼서 쓰는 용도.
	int maxLegion = spAcc->GetMaxLegions();
	XArchive packetLegion[XGAME::MAX_LEGION];
	SQLLEN		packetLegionLen[XGAME::MAX_LEGION];
	int AccountLevel = spAcc->GetLevel();

	XBREAK(maxLegion != XGAME::MAX_LEGION);
	for (int i = 0; i < maxLegion; ++i)
	{
		spAcc->MakeLegionPacketFull(packetLegion[i], i);
		packetLegionLen[i] = packetLegion[i].size();
		TRACE("size archive idx=%d: size=%d\n", i, packetLegion[i].size());
	}

	XArchive		packetdata1;
	SQLLEN		packetdata1Len;
	// 계정의 잡데이터를 바이너리에 밀어넣어 한꺼번에 저장한다
	spAcc->SerializeEtcData(packetdata1);
	packetdata1Len = packetdata1.size();

	//	XSpotJewel jewel[2];	// DB에 저장해야할 보석광산 데이타가 들어있다.

	ID mine1_idOwner = 0;
	ID mine1_idEnemy = 0;
	int mine1_levelMine = 0;
	int mine1_defense = 0;
	int mine1_idxLegion = 0;
	int mine1_secLastEvent = 0;	// 매치가되었거나 마지막으로 전투가일어난 시간

	ID mine2_idOwner = 0;
	ID mine2_idEnemy = 0;
	int mine2_levelMine = 0;
	int mine2_defense = 0;
	int mine2_idxLegion = 0;
	int mine2_secLastEvent = 0;	// 매치가되었거나 마지막으로 전투가일어난 시간
	_tstring strOwnerName[2];

	{
		auto pJewel = spAcc->GetpWorld()->GetSpotJewelByIdx(1);
		if (pJewel) {
			mine1_idOwner = pJewel->GetidOwner();
			strOwnerName[0] = pJewel->GetszName();
// 			if( !strOwnerName[0].empty() ) {
// 				TCHAR tc = strOwnerName[ 0 ].front();
// 				if( XE::IsHangul( tc ) )
// 					strOwnerName[ 0 ] += _T( "끗" );		// 끝에 한글자씩 잘려 저장되어서 더미글자를 넣음.
// 			}
			mine1_idEnemy = pJewel->GetidMatchEnemy();
			mine1_levelMine = pJewel->GetlevelMine();
			mine1_defense = pJewel->GetDefense();
			mine1_idxLegion = pJewel->GetidxLegion();
			mine1_secLastEvent = (int)pJewel->GetsecLastEvent(); // 소수점 이하 절삭
		}
		pJewel = spAcc->GetpWorld()->GetSpotJewelByIdx(2);
		if (pJewel) {
			mine2_idOwner = pJewel->GetidOwner();
			strOwnerName[1] = pJewel->GetszName();
// 			if( !strOwnerName[ 1 ].empty() ) {
// 				TCHAR tc = strOwnerName[ 1 ].front();
// 				if( XE::IsHangul( tc ) )
// 					strOwnerName[ 1 ] += _T( "깟" );		// 끝에 한글자씩 잘려 저장되어서 더미글자를 넣음.
// 			}
			mine2_idEnemy = pJewel->GetidMatchEnemy();
			mine2_levelMine = pJewel->GetlevelMine();
			mine2_defense = pJewel->GetDefense();
			mine2_idxLegion = pJewel->GetidxLegion();
			mine2_secLastEvent = (int)pJewel->GetsecLastEvent(); // 소수점 이하 절삭
		}
	}
	// 만드레이트 스팟 데이타.
	xMandrakeDB dbMandrake[2];
	for (int i = 0; i < 2; ++i) {
		auto pSpot = spAcc->GetpWorld()->GetSpotMandrakeByIdx(i + 1);
		if (pSpot) {
			dbMandrake[i].idOwner = pSpot->GetidOwner();
			dbMandrake[i].idEnemy = pSpot->m_idMatchEnemy;
			dbMandrake[i].win = pSpot->GetWin();
			dbMandrake[i].reward = pSpot->GetReward();
			dbMandrake[i].offwin = 0;		// 저장할땐 항상 0으로 저장됨
			_tcscpy_s(dbMandrake[i].szName, pSpot->GetszName());
		}
	}
	// 저장하는 시점에 플레이타이머를 업데이트 한다.
	spAcc->UpdatePlayTimer();
	XINT64 secPlay = (XINT64)spAcc->GetsecPlay();
// 	DWORD nsecTrade = spAcc->GetsecTrade();
// 	DWORD nsecShop = spAcc->GetsecShop();
	DWORD secPassByTrader = spAcc->GettimerTrader().GetsecPass();	// 지나간 시간을 저장함.
	DWORD secPassByShop = spAcc->GettimerShop().GetsecPass();
//	DWORD longest = timeGetTime();
	DWORD nowtime = (DWORD)(timeGetTime() / TICKS_PER_SEC);
// 	if (nsecTrade > 0) {
// 		nsecTrade += nowtime;
// 	}
// 	if (nsecShop > 0) {
// 		nsecShop += nowtime;
// 	}	else {
// 		nsecShop = XGC->m_shopCallInerval + nowtime;
// 	}
	DWORD nsecSubscribe = spAcc->GetsecSubscribe();
	DWORD nsecSubscribeEnd = spAcc->GetsecSubscribeEnd();
	if (nsecSubscribe > 0) {
		nsecSubscribe += nowtime;
	}
	if (nsecSubscribeEnd > 0) {
		nsecSubscribeEnd += nowtime;
		if( nsecSubscribe == 0 )
			nsecSubscribe = nowtime;
	}
	SQLLEN		LevelPacketLen = LevelInfoPacket.size();
	SQLLEN		HeroPacketLen = HeroInfoPacket.size();
	SQLLEN loginIDLen1 = SQL_NTS;
	SQLLEN loginIDLen2 = SQL_NTS;
	SQLLEN loginIDLen3 = SQL_NTS;
	SQLLEN loginIDLen4 = SQL_NTS;

	SQLLEN	retlen64 = 0;
	//spAcc->GetstrName();
	_tstring strMandOwner1 = dbMandrake[0].szName;
	_tstring strMandOwner2 = dbMandrake[1].szName;
	//strMandOwner1 = _T("test1");

	TCHAR szjewelOwnerName1[256];
	TCHAR szjewelOwnerName2[256];
	TCHAR szMandOwnerName1[256];
	TCHAR szMandOwnerName2[256];
	_tcscpy_s(szjewelOwnerName1, strOwnerName[0].c_str());
	_tcscpy_s(szjewelOwnerName2, strOwnerName[1].c_str());
	_tcscpy_s(szMandOwnerName1, dbMandrake[0].szName);
	_tcscpy_s(szMandOwnerName2, dbMandrake[1].szName);
	int nicklen = _tcslen(spAcc->GetstrName());
// 	int mineOwnerNamelen1 = _tcslen(strOwnerName[0].c_str());
// 	int mineOwnerNamelen2 = _tcslen(strOwnerName[1].c_str());
	int mandrakenamelen1 = _tcslen(dbMandrake[0].szName);
	int mandrakenamelen2 = _tcslen(dbMandrake[1].szName);
	int guildindex = spAcc->GetGuildIndex();
	int guildgrade = (int)spAcc->GetGuildgrade();

	int dummy = 0;
	// xSave
	//1
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_idAccount, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_Ver, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_CurrSN, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_Gold, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_Cashtem, 0, &retLen, sqlcount++);

	//2
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dummy, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &Herocount, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &spAcc->m_idLastConnectSvr, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &secSaved, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &powerIncludeEmpty, 0, &retLen, sqlcount++);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &ladder, 0, &retLen, sqlcount++);

	//3
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_idOwner, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_idEnemy, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_levelMine, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_defense, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_idxLegion, 0, &retLen, sqlcount++);

	//4
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine1_secLastEvent, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_idOwner, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_idEnemy, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_levelMine, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_defense, 0, &retLen, sqlcount++);

	//5
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_idxLegion, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &mine2_secLastEvent, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].idOwner, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].idEnemy, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].win, 0, &retLen, sqlcount++);

	//6
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].offwin, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].reward, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[0].idxLegion, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].idOwner, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].idEnemy, 0, &retLen, sqlcount++);

	//7
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].win, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].offwin, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].reward, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &dbMandrake[1].idxLegion, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &AccountLevel, 0, &retLen, sqlcount++);


	//8
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)LevelInfoPacket.GetBuffer(), 0, &LevelPacketLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)HeroInfoPacket.GetBuffer(), 0, &HeroPacketLen, sqlcount++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)worlds[idx].buff, 0, &worlds[idx].sqlLen, sqlcount++);
	}
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)ResourcePacket.GetBuffer(), 0, &ResourceLen, sqlcount++);

	//9
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)packetLegion[0].GetBuffer(), 0, &packetLegionLen[0], sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)packetLegion[1].GetBuffer(), 0, &packetLegionLen[1], sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)packetLegion[2].GetBuffer(), 0, &packetLegionLen[2], sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)packetdata1.GetBuffer(), 0, &packetdata1Len, sqlcount++);
	//10
	// 아이템
	{
		int num = XNUM_ARRAY(items);
		for (int idx = 0; idx < num; ++idx)
			BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)items[idx].buff, 0, &items[idx].sqlLen, sqlcount++);
	}
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arAbility.GetBuffer(), 0, &sqllenAbilityBin, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arQuest.GetBuffer(), 0, &sqllenQuest, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(XINT64), 0, &secPlay, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &MaxItemCount, 0, &retLen, sqlcount++);

	//11
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nsecTrade, 0, &retLen, sqlcount++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nsecShop, 0, &retLen, sqlcount++);
	BindParameter( SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof( int ), 0, &secPassByTrader, 0, &retLen, sqlcount++ );
	BindParameter( SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof( int ), 0, &secPassByShop, 0, &retLen, sqlcount++ );
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)arShopListInfo.GetBuffer(), 0, &sqllenshoplist, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &guildindex, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &guildgrade, 0, &retLen, sqlcount++);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nsecSubscribe, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nsecSubscribeEnd, 0, &retLen, sqlcount++);

	// xblog
	//12
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arBattleLogAttack.GetBuffer(), 0, &sqllenAttackLog, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arBattleLogDefense.GetBuffer(), 0, &sqllenDefenseLog, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arJoinreq.GetBuffer(), 0, &sqllenJoinreq, sqlcount++);


	//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)mineOwnerNamelen1, 0, szjewelOwnerName1, 0, &loginIDLen1, sqlcount++);
	//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)mineOwnerNamelen2, 0, szjewelOwnerName2, 0, &loginIDLen2, sqlcount++);
	//13
	//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)mandrakenamelen1, 0, szMandOwnerName1, 0, &loginIDLen3, sqlcount++);
	//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)mandrakenamelen2, 0, szMandOwnerName2, 0, &loginIDLen4, sqlcount++);


	SQLRETURN ret;
	TCHAR szQuery[1024];
	//_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountUpdateAll( ?, ?,?,?,?, ?,?,?,?,? ,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, '%s', '%s', '%s', '%s') }"), szjewelOwnerName1, szjewelOwnerName2, szMandOwnerName1, szMandOwnerName2);
//	_stprintf_s(szQuery, _T("{ call dbo.AccountUpdateAll( ?, ?,?,?,?, ?,?,?,?,? ,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, '%s', '%s', '%s', '%s') }")
// 																											, strOwnerName[0].c_str()
// 																											, strOwnerName[1].c_str()
// 																											, dbMandrake[0].szName
// 																											, dbMandrake[1].szName);
 _stprintf_s(szQuery, 1024, _T("{ call dbo.AccountUpdateAll( ?, ?,?,?,?, ?,?,?,?,? ,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, '%s', '%s', '%s', '%s', '%s') }")
																	, strOwnerName[0].c_str()
																	, strOwnerName[1].c_str()
																	, dbMandrake[0].szName
																	, dbMandrake[1].szName
																	, strHello.c_str()); //Hello 2016.01.20

	//	if ((ret = Excute(_T("{ call dbo.AccountUpdateAll( ?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,?, ?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,?,? ,?,?,?,'?','?', '?','?') }"))) != SQL_SUCCESS)
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUpdateAll Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}


	return TRUE;
}
BOOL CUserDB::AccountLoadResourceTmp(ID idAccount, XArchive& arOut)
{
	ReAllocStmt();

	BOOL bRet = FALSE;
	SQLLEN	sqlen = 0;
	BYTE buff[BINARY_BUFF_LEN];
	XCLEAR_ARRAY(buff);
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.AccountLoadResourcetmp( '%d' ) }"), idAccount);
	SQLSMALLINT i = 1;

	SQLRETURN ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return xDB_FAIL;
	}

	ret = BindCol(SQL_C_BINARY, &buff, BINARY_BUFF_LEN, &sqlen, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{		//
		if (sqlen > 0)
			arOut.SetBufferMem(buff, sqlen);
		bRet = TRUE;
	}
	else
	{
		if (ret == SQL_NO_DATA)
			bRet = FALSE;
		else
		{
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
			BREAK_AND_LOG();
			bRet = FALSE;
		}
	}

	return bRet;
}
BOOL CUserDB::AccountUpdateResourcetmp(ID idAccount, const XArchive& arParam)
{
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = arParam.size();
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arParam.GetBuffer(), 0, &sqlen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.AccountUpdateResourcetmp( ?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountUpdateResourcetmp Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}
BOOL CUserDB::ChangeOwnerJewelMine(ID idAccount, int jewelminenum, int newOwner, _tstring& _strName, int idenemy, int minelevel, int minedefense, int seclastevent)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN NickNameLen = SQL_NTS;
	SQLLEN retLen = 0;
	TCHAR szBuff[64]={0,};
	auto strName = _strName;
// 	if( !strName.empty() ) {
// 		TCHAR tc = strName.front();
// 		if( XE::IsHangul( tc ) )
// 			strName += _T( "끝" );		// 끝에 한글자씩 잘려 저장되어서 더미글자를 넣음.
// 	}
	_tcscpy_s( szBuff, strName.c_str() );
//	int nicklen = _tcslen(strName.c_str());
	int nicklen = _tcslen( szBuff ) * sizeof(TCHAR);
	//	TCHAR szName[256];
	//	_tcscpy_s( szName, strName.c_str() );
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &jewelminenum, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &newOwner, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idenemy, 0, &retLen, i++);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &minelevel, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &minedefense, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &seclastevent, 0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicklen, 0, (SQLPOINTER*)strName.c_str(), 0, &NickNameLen, i++);
	BindParameter( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)nicklen, 0, szBuff, 0, &NickNameLen, i++ );

	SQLRETURN ret;
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.UpdateOwnerJewleMine( ?,?,?,?,?, ?,?, '%s') }"), szBuff );

	//if ( (ret = Excute (_T("{ call dbo.UpdateOwnerJewleMine( ?,?,?,?,?, ?,?,'?') }"))) != SQL_SUCCESS )
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateOwnerJewleMine Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}
/**
 @brief idDBAcc의 DB정보에 광산정보를 업데이트 한다.
*/
BOOL CUserDB::ChangeOwnerJewelMine2(ID idDBAcc
																	, XGAME::xJewelInfo& info
																	, int secUpdate )
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN sqllenName = SQL_NTS;
	SQLLEN retLen = 0;
	TCHAR szBuff[64]={0,};
	auto strName = info.m_strName;
// 	if( !strName.empty() ) {
// 		TCHAR tc = strName.front();
// 		if( XE::IsHangul( tc ) )
// 			strName += _T( "끝" );		// 끝에 한글자씩 잘려 저장되어서 더미글자를 넣음.
// 	}
	_tcscpy_s( szBuff, strName.c_str() );	// std::string의 c_str()의 메모리가 연속적인지 확신할수 없어서 한번 옮겨서 저장함.
	int lenName = _tcslen( szBuff ) * sizeof(TCHAR);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idDBAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &info.m_idxJewel, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &info.m_idOwner, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &info.m_idMatchEnemy, 0, &retLen, i++);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &info.m_LevelMine, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &info.m_Defense, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &secUpdate, 0, &retLen, i++);
	BindParameter( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)lenName, 0, szBuff, 0, &sqllenName, i++ );

	SQLRETURN ret;
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.UpdateOwnerJewleMine( ?,?,?,?,?, ?,?, '%s') }"), szBuff );
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateOwnerJewleMine2 Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

BOOL CUserDB::FindAccountPowerValue(XDBUAccount* pAccOut,
	                                  int MinValue, int MaxValue,
	                                  ID Finderidaccount,
                                  	XArchive& ar)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.FindAccountPowerValue( '%d',  '%d',  '%d' ) }"), Finderidaccount, MinValue, MaxValue);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindAccountPowerValue Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS, fbnickLen = SQL_NTS;
	SQLLEN sqllenHello = SQL_NTS;

	int			tmpidAccount = 0;
	int			Ver = 0;
	int			powerIncludeEmpty = 0;
	int			ladder = 0;
	int			accountLevel = 0;
	DWORD		secLastCalc = 0;		// <<<<<
	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szfbName[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szHello[ 80 ] = {0, };
	SQLLEN	sqLenResourceData = 0;
	BYTE	pResourceBuff[BINARY_BUFF_LEN2];	// <<<<<
	XCLEAR_ARRAY(pResourceBuff);
	ArchiveSplit worlds[2];

	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szfbName, LEN_LOGIN_ID * 2, &fbnickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &sqllenHello, i++);		//Hello 2016.01.20
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &powerIncludeEmpty, sizeof(int), &nLen, i++);

	ret = BindCol( SQL_C_LONG, &ladder, sizeof( int ), &nLen, i++ );
	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &secLastCalc, sizeof(int), &nLen, i++);
	// 	ret = BindCol( SQL_C_BINARY,	&pWorldBuff1,			BINARY_BUFF_LEN,			&sqLenWorldData1,		i++);
	// 	ret = BindCol(SQL_C_BINARY, &pWorldBuff2, BINARY_BUFF_LEN, &sqLenWorldData2, i++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			ret = BindCol(SQL_C_BINARY, &worlds[idx].buff, BINARY_BUFF_LEN, &worlds[idx].sqlLen, i++);
	}
	ret = BindCol(SQL_C_BINARY, &pResourceBuff, BINARY_BUFF_LEN2, &sqLenResourceData, i++);
	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		if ((DWORD)tmpidAccount > 0)  pAccOut->m_idAccount = (DWORD)tmpidAccount;
		if (Ver > 0) pAccOut->m_Ver = Ver;
		int size = _tcslen(szName);
		if (size > 0) pAccOut->m_strName = szName;
		pAccOut->m_strFbUserId = szfbName;
		pAccOut->m_strHello = szHello;
		pAccOut->m_PowerExcludeEmpty = powerIncludeEmpty;		// 적유저의 경우 빈슬롯뺀값이 필요있을까 해서 포함된값을 넣음.
		pAccOut->m_PowerIncludeEmpty = powerIncludeEmpty;		// 
		pAccOut->SetLadder( ladder );
		pAccOut->SetLevel(accountLevel);
		// 월드 바이너리 데이타를 DeSerialize한다.		
		XArchive arWorld(0xffff); //Worlddata
		int offset = CombineSplitData(arWorld, worlds, Ver );
		if (offset > 0) {
			SAFE_DELETE(pAccOut->m_pWorld);
			pAccOut->m_pWorld = new XWorld;
			pAccOut->m_pWorld->DeSerialize(arWorld);
		} else
			return FALSE;
		XArchive arResource; //Resourcedata
		if (sqLenResourceData > 0) {
			arResource.SetBufferMem(pResourceBuff, sqLenResourceData);
			pAccOut->RestoreResourcePacket(arResource);
		}
		return TRUE;
	} if( ret == SQL_NO_DATA ) {
		return FALSE;
	} else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindAccountPowerValue Data Error  (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
	}

	return FALSE;
}

BOOL	CUserDB::FindJewelInfoByIdAcc(ID idAccount, int jewelminenum, XArchive& ar)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.FindJewelInfoByIdAcc( '%d',  '%d') }"), idAccount, jewelminenum);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindJewelInfoByIdAcc Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS;

	int			ver = 0;
	int			accountLevel = 0;
	int			jewel_level = 0;
	int			jewel_defense = 0;
	int			jewel_legion = 0;
	int     power = 0;
	int     ladder = 0;
	DWORD		secLastCalc = 0;
	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };	//secPlay
	BYTE	legionBuff[BINARY_BUFF_LEN];
	SQLLEN	legiondataLen;
	ArchiveSplit worlds[2];
	BYTE	buffAbil[ BINARY_BUFF_LEN ];
	SQLLEN sqllenAbil;
	XCLEAR_ARRAY( buffAbil );

	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &jewel_level, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewel_defense, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[0], BINARY_BUFF_LEN, &legiondataLen, i++);
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &ladder, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &buffAbil, BINARY_BUFF_LEN, &sqllenAbil, i++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			ret = BindCol(SQL_C_BINARY, &worlds[idx].buff, BINARY_BUFF_LEN, &worlds[idx].sqlLen, i++);
	}
	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		ar << ver;
		ar << accountLevel;
		ar << szName;
		ar << jewel_level;
		ar << jewel_defense;
		ar << power;
		ar << ladder;
		{
			XArchive arLegion;
			arLegion.SetBufferMem(legionBuff, legiondataLen);
			ar << arLegion;
		}
    if( XBREAK( sqllenAbil <= 0 ) )
      return FALSE;
    {
      XArchive arAbil;
      arAbil.SetBufferMem( buffAbil, sqllenAbil );
      ar << arAbil;
    }
		{
			XArchive arWorld(0xffff); //Worlddata
			int offset = CombineSplitData(arWorld, worlds, ver);
			if (offset > 0)
				ar << arWorld;
		}
		return TRUE;
	} else 
	if( ret == SQL_NO_DATA ) {
		return FALSE;
	} else
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindJewelInfoByIdAcc Data Error  (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
	}

	return FALSE;
}

/**
min~max스코어 사이의 유저중 idEnemy가 0인 유저를 찾아 그 정보를 돌려준다.
*/
BOOL CUserDB::FindAccountMineEnemyZero(XSPDBUAcc
																		, ID idAccount
																		, int idxMine
																		, int MinValue, int MaxValue
																		, XArchive& ar)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, 
		_T("{ call dbo.FindAccountMineEnemyZero( '%d',  '%d',  '%d', '%d' ) }")
						, idAccount, idxMine, MinValue, MaxValue);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindAccountMineEnemyZero Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS, fbnickLen = SQL_NTS;
	SQLLEN sqllenHello = SQL_NTS;
	SQLLEN sqllenFbUserId = SQL_NTS;

	int			tmpidAccount = 0;
	int			Ver = 0;
	int			power = 0;
	int     ladder = 0;
	int			accountLevel = 0;
	int			jewel_level = 0;
	int			jewel_defense = 0;
	int			idxJewel_legion = 0;

	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szfbName[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szHello[ 80 ] = {0, };

	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szfbName, LEN_LOGIN_ID * 2, &fbnickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &sqllenHello, i++); //hello 2016.01.20
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &ladder, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewel_level, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &jewel_defense, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &idxJewel_legion, sizeof(int), &nLen, i++);

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if (tmpidAccount == idAccount) return FALSE;
		ar << (ID)tmpidAccount;
		ar << (DWORD)Ver;
		ar << szName;
		ar << accountLevel;
		ar << jewel_level;
		ar << jewel_defense;
		ar << idxJewel_legion;
		ar << power;
		ar << ladder;
		ar << szfbName;
		ar << szHello;
		return TRUE;
	} else
	if (ret == SQL_NO_DATA) {
		return FALSE;
	} else {
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n FindAccountMineEnemyZero Data Error  (err:%d) : %s"), ret, err);
			BREAK_AND_LOG();
	}
	return FALSE;
}

bool CUserDB::FindAccountMineEnemyZero2(ID idFinder
																			, int idxMine
																			, int powerMin
																			, int powerMax
																			, XGAME::xJewelMatchEnemy *pOut )
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	SQLRETURN	ret;
	if( pOut == nullptr )
		return false;
	_stprintf_s(szQuery, 
		_T("{ call dbo.FindAccountMineEnemyZero( '%d',  '%d',  '%d', '%d' ) }")
						, idFinder, idxMine, powerMin, powerMax);
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n FindAccountMineEnemyZero Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return false;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS, fbnickLen = SQL_NTS;
	SQLLEN sqllenFbUserId = SQL_NTS;
	SQLLEN sqllenHello = SQL_NTS;
	XGAME::xJewelMatchEnemy info;
	int			Ver = 0;
	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szfbName[ LEN_LOGIN_ID * 2 ] = {0, };
	TCHAR	szHello[ 80 ] = {0, };
	ret = BindCol(SQL_C_LONG, &info.m_idAcc, sizeof(int), &nLen, i++);
	ret = BindCol( SQL_C_WCHAR, szfbName, LEN_LOGIN_ID * 2, &fbnickLen, i++ );
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &sqllenHello, i++); //hello 2016.01.20
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_Power, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_Ladder, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_lvAcc, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_LvMine, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_Defense, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &info.m_idxLegion, sizeof(int), &nLen, i++);
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		XBREAK( pOut == nullptr );
		if (info.m_idAcc == idFinder)
			return false;		// 상대를 못찾음.
		info.m_strName = szName;
		info.m_strFbUserId = szfbName;
		info.m_strHello = szHello;
		*pOut = info;
		return true;
	} else 
	if (ret == SQL_NO_DATA) {
		return false;
	} else {
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n FindAccountMineEnemyZero2 Data Error  (err:%d) : %s"), ret, err);
			BREAK_AND_LOG();
		}

	return false;
}

BOOL CUserDB::LegionLoadIDAccount(ID idAccount, int idxLegion, XArchive& arOut)					//SELECT
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.LegionLoadIDAccount( '%d' ) }"), idAccount);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n LegionLoadIDAccount Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, pwLen = SQL_NTS, nickLen = SQL_NTS, freegem = SQL_NTS, freegemstone = SQL_NTS;

	int			tmpidAccount = 0;
	int			Ver = 0;
	BYTE		legionBuff[XGAME::MAX_LEGION][BINARY_BUFF_LEN];
	SQLLEN	regiondataLen[XGAME::MAX_LEGION];
	BYTE	buffAbil[BINARY_BUFF_LEN];
	SQLLEN sqllenAbil;


	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[0], BINARY_BUFF_LEN, &regiondataLen[0], i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[1], BINARY_BUFF_LEN, &regiondataLen[1], i++);
	ret = BindCol(SQL_C_BINARY, &legionBuff[2], BINARY_BUFF_LEN, &regiondataLen[2], i++);
	ret = BindCol(SQL_C_BINARY, &buffAbil, BINARY_BUFF_LEN, &sqllenAbil, i++);

	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		{
			// 군단정보 바이너리 읽기
			XBREAK(regiondataLen[idxLegion] <= 0);
			XArchive arLegion;
			arLegion.SetBufferMem(legionBuff[idxLegion], regiondataLen[idxLegion]);
			//			arOut << Ver;
			arOut << arLegion;
			XArchive arAbil;
			arAbil.SetBufferMem( buffAbil, sqllenAbil );
			arOut << arAbil;
		}
		return TRUE;
	}
	else
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n LegionLoadIDAccount Data Error  (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
	}

	return FALSE;
}

/**
idAcc유저를 찾아 자원양와 월드정보등을 읽어온다.
arOut에 담길 정보
XAccount::m_Ver:
XAccount::m_secLastCalc:
XAccount::Score
XAccount::m_aryResource
XAccount::m_pWorld
*/
BOOL CUserDB::LoadResourceWorldByIdAcc(ID idAcc, XArchive& arOut)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;
	int			tmpidAccount = 0;
	int			Ver = 0;				// <<<<<
	DWORD		secLastCalc = 0;		// <<<<<
	int			power = 0;				// <<<<<

	SQLLEN	sqLenResourceData = 0;
	BYTE		pResourceBuff[BINARY_BUFF_LEN2];	// <<<<<
	XCLEAR_ARRAY(pResourceBuff);
	ArchiveSplit worlds[2];

	_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadResourceWorldByIdAcc( '%d' ) }"), idAcc);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0;

	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &secLastCalc, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	// 	ret = BindCol( SQL_C_BINARY,		&pWorldBuff1,			BINARY_BUFF_LEN,			&sqLenWorldData1,		i++);
	// 	ret = BindCol(SQL_C_BINARY, &pWorldBuff2, BINARY_BUFF_LEN, &sqLenWorldData2, i++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			ret = BindCol(SQL_C_BINARY, &worlds[idx].buff, BINARY_BUFF_LEN, &worlds[idx].sqlLen, i++);
	}
	ret = BindCol(SQL_C_BINARY, &pResourceBuff, BINARY_BUFF_LEN2, &sqLenResourceData, i++);

	ret = Fetch();

	// DB에서 읽은 후 arOut에 밀어넣음.
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		arOut << Ver;
		arOut << secLastCalc;
		arOut << power;
		//
		XArchive arResource; //Resourcedata
		if (XASSERT(sqLenResourceData > 0))
		{
			arResource.SetBufferMem(pResourceBuff, sqLenResourceData);
			arOut << arResource;
		}
		// 월드 바이너리 데이타를 DeSerialize한다.		
		XArchive arWorld(0x10000); //Worlddata
		arWorld.SetbForDB( true );
		int offset = CombineSplitData(arWorld, worlds, Ver );
		if (offset) {
			arOut << arWorld;
		}
		return TRUE;
	}	else {
	if (ret == SQL_NO_DATA) {
		return FALSE;
	} else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}
	}

	return FALSE;
}

/**
ar에 들어있는 자원수량과 월드데이타등을 DB에 저장한다.
ar내용:
XArchive arResource;
XArchive arWorld;
int Score;
*/
BOOL CUserDB::SaveResourceWorldByIdAcc(ID idAcc, XArchive& ar)
{
	XArchive arResource;
	XArchive arWorld(0x10000);
	int power;
	ar >> arResource;
	ar >> arWorld;
	ar >> power;

	SQLLEN sqlenResourceBin = arResource.size();
	SQLLEN		retLen = 0;
	ArchiveSplit worlds[2];
	//SplitArchive(worlds, arWorld);
	arWorld.SetbForDB( true );
	// arWorld아카이브를 압축시킨다.
	WorldToSplit( arWorld, worlds, XNUM_ARRAY(worlds) );
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &power, 0, &retLen, i++);
	{
		int num = XNUM_ARRAY(worlds);
		for (int idx = 0; idx < num; ++idx)
			BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)worlds[idx].buff, 0, &worlds[idx].sqlLen, i++);
	}
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arResource.GetBuffer(), 0, &sqlenResourceBin, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.SaveResourceWorldByIdAcc( ?,?,?,?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n SaveResourceWorldByIdAcc Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
idAcc유저를 찾아 디펜스로그 데이타를 읽어 arOut에 담는다.

*/
BOOL CUserDB::LoadDefenseLogByIdAcc(ID idAcc, XArchive& arOut)
{
	ReAllocStmt();

	BOOL bRet = FALSE;
	SQLLEN	sqlenDefenseLog = 0;
	BYTE buffDefenseLog[BINARY_BUFF_LEN];
	XCLEAR_ARRAY(buffDefenseLog);
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadDefenseLogByIdAcc( '%d' ) }"), idAcc);
	SQLSMALLINT i = 1;
	
	SQLRETURN ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return xDB_FAIL;
	}

	ret = BindCol(SQL_C_BINARY, &buffDefenseLog, BINARY_BUFF_LEN, &sqlenDefenseLog, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		if (sqlenDefenseLog > 0)
			arOut.SetBufferMem(buffDefenseLog, sqlenDefenseLog);
		bRet = TRUE;
	} else 
	if (ret == SQL_NO_DATA) {
			bRet = FALSE;
	} else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		bRet = FALSE;
	}
	return bRet;
}
/**
ar에 들어있는 방어로그 데이타를 저장한다.
ar내용:
*/
BOOL CUserDB::SaveDefenseLogByIdAcc(ID idAcc, const XArchive& ar)
{
	SQLLEN	retLen = 0;
	SQLLEN	sqlenDefenseLog = ar.size();
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)ar.GetBuffer(), 0, &sqlenDefenseLog, i++);
	SQLRETURN ret;
	if ((ret = Excute(_T("{ call dbo.SaveDefenseLogByIdAcc( ?,?) }"))) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n SaveDefenseLogByIdAcc Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}


/**
 @brief DB로부터 길드의 레이드캠페인 정보를 꺼내온다.
*/
bool CUserDB::LoadCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive* pOut )
{
	XArchive& arOut = *pOut;
	SQLLEN	sqllenCampDB = 0;
	BYTE		buffCampDB[ BINARY_BUFF_LEN ];
	XCLEAR_ARRAY( buffCampDB );
	bool bRet = true;
	ReAllocStmt();
	
	int ver = 100;
	//강제로 버전 정보를 넣었음.

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.guildraidselect( '%d','%d','%d','%d' ) }"), idGuild, idCamp, ver, idAcc);
	SQLSMALLINT i = 1;

	SQLRETURN ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return false;
	}

	ret = BindCol(SQL_C_BINARY, &buffCampDB, BINARY_BUFF_LEN, &sqllenCampDB, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{		//
		if (sqllenCampDB > 0)
			arOut.SetBufferMem(buffCampDB, sqllenCampDB);		
	}
	else
	{
		if (ret != SQL_NO_DATA)
		{
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
			BREAK_AND_LOG();
		}
		bRet = false;		
	}
	return bRet;
}

/**
 @brief 캠페인정보 arCamp를 생성한다
 만약 이미 idCamp로 데이타가 있었다면 실패한다.
*/
XGAME::xtGuildError CUserDB::CreateCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive& arCamp )
{
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = arCamp.size();
	
	int retValue = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	int ver = 100;	
	
	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &retValue, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idCamp, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &ver, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arCamp.GetBuffer(), 0, &sqlen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ ? = call dbo.CreateCampaignByGuildRaid( ?,?,?,?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n CreateCampaignByGuildRaid Error : %s"), err);
		BREAK_AND_LOG();
		return XGAME::xGE_ERROR_CRITICAL;
	}

	if (retValue > 0)
		return XGAME::xGE_ERROR_ALREADY_EXIST_RAID;
	return XGAME::xGE_SUCCESS;
}

/**
 @brief 캠페인정보 arCamp를 업데이트 한다.
 DB에 문제가 있거나 아직 생성되지 않은 캠페인이면 실패한다.
*/
XGAME::xtGuildError CUserDB::UpdateCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, XArchive& arCamp )
{
//#pragma message("구현해야함")
	//UPDATE 가 성공 하는 경우는 guildid, idcamp가 제시한 내용의 idaccount 가 0이거나 idaccount == idacc 인 경우만 성공.
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = arCamp.size();

	int retValue = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	int ver = 100;

	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &retValue, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idCamp, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &ver, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arCamp.GetBuffer(), 0, &sqlen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ ? = call dbo.UpdateCampaignByGuildRaid( ?,?,?,?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateCampaignByGuildRaid Error : %s"), err);
		BREAK_AND_LOG();
		return XGAME::xGE_ERROR_CRITICAL;
	}

	if (retValue == 0)
		return XGAME::xGE_ERROR_NOT_FOUND_RAID;
	return XGAME::xGE_SUCCESS;
}

/**
 @brief 길드레이드 진입
*/
bool CUserDB::EnterGuildRaid( ID idAcc, ID idGuild, ID idCamp, XGAME::xtGuildError *pOutErr, ID *pOutidAccExist )
{
	ID playidacc = 0;
	SQLLEN	retLen = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	int ver = 100;

	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &playidacc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idCamp, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &ver, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ ? = call dbo.EnterGuildRaid( ?,?,?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n EnterGuildRaid Error : %s"), err);
		BREAK_AND_LOG();
		return false;
	}
	*pOutidAccExist = playidacc;
	if (playidacc == idAcc)	{
		*pOutErr = XGAME::xGE_SUCCESS;
	} else if (playidacc == 0) {
		*pOutErr = XGAME::xGE_ERROR_NOT_FOUND_RAID;
		return false;
	} else	{
		*pOutErr = XGAME::xGE_ERROR_STILL_TRYING_RAID;
	}
	return true;
}
///
bool CUserDB::LeaveGuildRaid( ID idAcc, ID idGuild, ID idCamp, XGAME::xtGuildError *pOutErr )
{
	ID playidacc = 0;
	SQLLEN	retLen = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	int ver = 100;

	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &playidacc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idCamp, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &ver, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ ? = call dbo.LeaveGuildRaid( ?,?,?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n EnterGuildRaid Error : %s"), err);
		BREAK_AND_LOG();
		return false;
	}

	if (playidacc == idAcc)	{
		*pOutErr = XGAME::xGE_SUCCESS;
	} else if (playidacc == 0) {
		*pOutErr = XGAME::xGE_ERROR_IS_NOT_LOCKER;
	}
	return true;
}

// xuzhu end

/**
 @brief 길드 마스터이름과 길드 멤버목록을 업데이트 한다.
*/
xtGuildError CUserDB::UpdateGuildMemberWithMaster( XGuild* pGuild
																									 , ID idAccNewMaster
																									 , const _tstring& strNewNameByMaster )
{
	XAUTO_LOCK4( pGuild );
	ID idGuild = pGuild->GetidGuild();
	XArchive arMembers;
	pGuild->SerializeGuildMemberlist( arMembers );
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = arMembers.size();
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccNewMaster, 0, &retLen, i++);	
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arMembers.GetBuffer(), 0, &sqlen, i++);
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{call dbo.UpdateGuildmemberlistwithMaster( ?,?,?,'%s') }")
							 , strNewNameByMaster.c_str());

	SQLRETURN ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateGuildmemberlistwithMaster Error : %s"), err);
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}
	return xGE_SUCCESS;
}

/**
 @brief idGuild의 멤버리스트 목록(arMembers)을 갱신한다.
*/
xtGuildError CUserDB::UpdateGuildMembers( XGuild* pGuild )
{
	XAUTO_LOCK4( pGuild );
	XArchive arMembers;
	pGuild->SerializeGuildMemberlist( arMembers );
	ID idGuild = pGuild->GetidGuild();
	//
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = arMembers.size();
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arMembers.GetBuffer(), 0, &sqlen, i++);
	SQLRETURN ret = Excute( _T( "{ call dbo.UpdateGuildmemberlist( ?,?) }" ) );
	if( ret != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n UpdateGuildmemberlist Error : %s" ), err );
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}
	return xGE_SUCCESS;
}

/**
 @brief idGuild의 가입신청자 리스트를 저장.
 @param ar XGuild::SerializeGuildReqMemberList()
*/
BOOL CUserDB::UpdateGuildJoinReqListByGuild(ID idGuild, const XArchive& ar)
{
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = ar.size();
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)ar.GetBuffer(), 0, &sqlen, i++);
	SQLRETURN ret;
	if( ( ret = Excute( _T( "{ call dbo.UpdateGuildjoinreqmemberlist( ?,?) }" ) ) ) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n UpdateGuildjoinreqmemberlist Error : %s" ), err );
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
 @brief 길드 멤버리스트와 가입신청자리스트 저장?
*/
xtGuildError CUserDB::UpdateGuildJoinAccept( ID idGuild, const XArchive& arMember, const XArchive& arReqList )
{
	SQLLEN	retLen = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;
// 	XArchive arParam1;
// 	XArchive arParam2;
// 
// 	XGuild* data = new XGuild;
// 	data->DeSerializeGuildMemberlist( ar );
// 	data->DeSerializeGuildReqMemberlist( ar );
// 	data->SerializeGuildMemberlist( arParam1 );
// 	data->DeSerializeGuildReqMemberlist( arParam2 );		// ???? 응?
// 
// 	SQLLEN	sqlen1 = arParam1.size();
// 	SQLLEN	sqlen2 = arParam2.size();
	SQLLEN	sqlen1 = arMember.size();
	SQLLEN	sqlen2 = arReqList.size();
	BindParameter( SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof( int ), 0, &idGuild, 0, &retLen, i++ );
	BindParameter( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arMember.GetBuffer(), 0, &sqlen1, i++ );
	BindParameter( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arReqList.GetBuffer(), 0, &sqlen2, i++ );
// 	BindParameter( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arParam1.GetBuffer(), 0, &sqlen1, i++ );
// 	BindParameter( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arParam2.GetBuffer(), 0, &sqlen2, i++ );
	SQLRETURN ret;
	if( ( ret = Excute( _T( "{ call dbo.UpdateGuildjoinAcceptmemberlist( ?,?,?) }" ) ) ) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n UpdateGuildjoinreqmemberlist Error : %s" ), err );
		BREAK_AND_LOG();
// 		if( data ) {
// 			delete data;
// 			data = nullptr;
// 		}
		return xGE_ERROR_DB;
	}
// 	if( data ) {
// 		delete data;
// 		data = nullptr;
// 	}
	return xGE_SUCCESS;
}
// BOOL CUserDB::UpdateGuildJoinAccept(ID Guildindex, XArchive& ar)
// {
// 	SQLLEN	retLen = 0;
// 
// 	ReAllocStmt();
// 	SQLUSMALLINT i = 1;
// 
// 	XArchive arParam1;
// 	XArchive arParam2;
// 
// 	XGuild* data = new XGuild;
// 	data->DeSerializeGuildMemberlist(ar);
// 	data->DeSerializeGuildReqMemberlist(ar);
// 	data->SerializeGuildMemberlist(arParam1);	
// 	data->DeSerializeGuildReqMemberlist(arParam2);		// ???? 응?
// 
// 	SQLLEN	sqlen1 = arParam1.size();
// 	SQLLEN	sqlen2 = arParam2.size();
// 
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &Guildindex, 0, &retLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arParam1.GetBuffer(), 0, &sqlen1, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arParam2.GetBuffer(), 0, &sqlen2, i++);
// 
// 	SQLRETURN ret;
// 	if( ( ret = Excute( _T( "{ call dbo.UpdateGuildjoinAcceptmemberlist( ?,?,?) }" ) ) ) != SQL_SUCCESS ) {
// 		TCHAR err[1024];
// 		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
// 		OutputTextFormat( _T( "\n UpdateGuildjoinreqmemberlist Error : %s" ), err );
// 		BREAK_AND_LOG();
// 
// 		if( data ) {
// 			delete data;
// 			data = nullptr;
// 		}
// 		return FALSE;
// 	}
// 	if( data ) {
// 		delete data;
// 		data = nullptr;
// 	}
// 	return TRUE;
// }
BOOL CUserDB::UpdateGuildContext(ID Guildindex, _tstring strContext)
{
	SQLLEN	retLen = 0, contextLen = SQL_NTS;
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &Guildindex, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)wcslen(strContext.c_str()), 0, (TCHAR*)strContext.c_str(), 0, &contextLen, i++);
	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.UpdateGuildContext( ?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateGuildContext Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
 @brief idAcc유저가 가입신청한 길드리스트의 저장
 @param idGuild 리스트의 저장과 동시에 소속길드아이디를 저장
 @param gradeMember 가입된 길드에서의 등급도 함께 저장
*/
xtGuildError CUserDB::UpdateGuildJoinReqListByAcc(ID idAcc
																				, ID idGuild
																				, xtGuildGrade _gradeMember
																				, const XArchive& arJoinReqList)
{	
	ReAllocStmt();
	SQLLEN	retLen = 0;
	SQLUSMALLINT sqlcount = 1;
	DWORD gradeMember = (DWORD)_gradeMember;
	SQLLEN sqllenJoinreq = arJoinReqList.size();

	//1
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &gradeMember, 0, &retLen, sqlcount++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN, 0, (SQLPOINTER*)arJoinReqList.GetBuffer(), 0, &sqllenJoinreq, sqlcount++);

	const SQLRETURN ret 
		= Excute(_T("{ call dbo.UpdateAccountGuildJoinReq( ?, ?, ?, ?) }"));	
	if(ret != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateAccountGuildJoinReq Error : %s"), err);
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}

	return xGE_SUCCESS;
}

/**
 @brief idAcc유저가 가입신청한 길드 리스트 로딩
*/
xtGuildError CUserDB::LoadGuildJoinReqListByAcc(ID idAcc, XArchive* pOut )
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;
	_stprintf_s( szQuery, _T( "{ call dbo.LoadAccountGuildJoinReq( '%d' ) }" )
												, idAcc );
	if( ( ret = Excute( szQuery ) ) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0;
	SQLLEN	sqllenJoinreqGuild = 0;

	BYTE		buffJoinreqGuild[BINARY_BUFF_LEN];
	XCLEAR_ARRAY( buffJoinreqGuild );
	ret = BindCol( SQL_C_BINARY, &buffJoinreqGuild, BINARY_BUFF_LEN, &sqllenJoinreqGuild, i++ );
	ret = Fetch();
	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) {//!= SQL_NO_DATA)
//		XArchive arJoinList;
		if( sqllenJoinreqGuild > 0 ) {
			pOut->SetBufferMem( buffJoinreqGuild, sqllenJoinreqGuild );
//			arResult.SetBufferMem( buffJoinreqGuild, sqllenJoinreqGuild );
		}
		if( sqllenJoinreqGuild <= 4 )
			(*pOut) << 0;		// nCount값을 넣었음.
// 		(*pOut) << arJoinList;
// 		if( sqllenJoinreqGuild <= 4 )
// 			arResult << 0;		// nCount값을 넣었음.
// 		(*pOut).CurrMoveToStart();
		return xGE_SUCCESS;
	} else
	if( ret == SQL_NO_DATA ) {
		//
		return xGE_ERROR_NOT_FOUND_USER;
	} else {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Data Error  (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
	}

	return xGE_ERROR_DB;
}

xtGuildError CUserDB::UpdateGuildOption( XGuild* pGuild )
{
	XArchive ar;
	pGuild->SerializeGuildOption( ar );
	//
	SQLLEN	retLen = 0;
	SQLLEN	sqlen = ar.size();
	ID idGuild = pGuild->GetidGuild();
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idGuild, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)ar.GetBuffer(), 0, &sqlen, i++);

	SQLRETURN ret;
	if ((ret = Excute(_T("{ call dbo.UpdateGuildOption( ?,?) }"))) != SQL_SUCCESS) {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdateGuildOptionError : %s"), err);
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}
	return xGE_SUCCESS;
}

BOOL CUserDB::TradeGemCall(ID idaccount, int remaingem)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &remaingem, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.AccountTradecallgem( ?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountTradecallgem Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

BOOL CUserDB::ShopGemCall(ID idaccount, int remaingem)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &remaingem, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.AccountShopcallgem( ?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AccountShopcallgem Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/**
 @brief idAcc유저의 길드가입정보를 꺼낸다.
*/
xtGuildError CUserDB::LoadGuildInfoByAcc(ID idAcc
																					, ID* pOutidGuild
																					, xtGuildGrade* pOutGrade
																					, XArchive* pOutReqList)
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	SQLRETURN	ret;
	_stprintf_s(szQuery, _T("{ call dbo.AccountLoadGuildInfo( '%d' ) }"), idAcc);
	if( (ret = Excute( szQuery )) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0;
	SQLLEN njoinguildLen = 0;
	BYTE		buffReqList[BINARY_BUFF_LEN];
	XCLEAR_ARRAY(buffReqList);
	int gradeMember = 0;
	ID idGuild = 0;
	ret = BindCol(SQL_C_LONG, &idGuild, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &gradeMember, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &buffReqList, BINARY_BUFF_LEN, &njoinguildLen, i++);
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
		if( njoinguildLen > 0 ) {
			pOutReqList->SetBufferMem(buffReqList, njoinguildLen);
		}
		return xGE_SUCCESS;
	} else
	if( ret == SQL_NO_DATA ) {
		return xGE_ERROR_NOT_FOUND_USER;
	} else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}
	return xGE_ERROR_DB;
}
BOOL CUserDB::CreateGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion)
{
	return TRUE;
}
BOOL CUserDB::SelectGuildRaidInfo(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion)
{
	return TRUE;
}
BOOL CUserDB::SelectGuildRaidEmptyInfo(ID GuildID)
{
	return TRUE;
}
BOOL CUserDB::DeleteGuildRaidInfo(ID GuildID, int raidindex, int spotid)
{
	return TRUE;
}
BOOL CUserDB::DeleteGuildRaidAll(ID GuildID)
{
	return TRUE;
}
BOOL CUserDB::UpdateGuildRaid(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return TRUE;
}
BOOL CUserDB::UpdateGuildRaidLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return TRUE;
}
BOOL CUserDB::UpdateGuildRaidUnLock(ID idaccount, ID GuildID, int raidindex, int spotid, int vesion, const XArchive& ar)
{
	return TRUE;
}
BOOL CUserDB::UpdateAccountCashInfo(ID idaccount, int remaincash)
{
	SQLLEN		retLen = 0;
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &remaincash, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.UpdateAccountCahsInfo( ?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n dbo.UpdateAccountCahsInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}
void CUserDB::PushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff)
{
	SQLLEN	retLen = 0, deviceidLen = SQL_NTS;
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idacc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &type1, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &type2, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &recvtime, 0, &retLen, i++);

	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.PushMsgRegist( ?, ?, ?, ?, '%s') }"), strBuff.c_str());

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n PushMsgRegist Error : %s"), err);
		BREAK_AND_LOG();		
	}
	return;
}
void CUserDB::PushMsgUnRegist(ID idacc, int type1, ID type2)
{
	SQLLEN	retLen = 0, deviceidLen = SQL_NTS;
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idacc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &type1, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &type2, 0, &retLen, i++);

	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.PushMsgUnRegist( ?, ?, ?) }"));

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n PushMsgUnRegist Error : %s"), err);
		BREAK_AND_LOG();
	}
	return;
}

BOOL CUserDB::RegistAccountDeviceID(ID idaccount, _tstring strDeviceID, _tstring strAppstore)
{
	SQLLEN	retLen = 0, deviceidLen = SQL_NTS;
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	
	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.RegistDeviceID( ?,  '%s', '%s') }"), strDeviceID.c_str(), strAppstore.c_str());

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n RegistDeviceID Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}
BOOL CUserDB::SaveShopInfo(ID idaccount, XSPDBUAcc spAcc)
{
	XArchive arShopListInfo;

	spAcc->DeSerializeShopList(arShopListInfo);
	SQLLEN sqlenShopListlen = arShopListInfo.size();
	SQLLEN		retLen = 0;

	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)arShopListInfo.GetBuffer(), 0, &sqlenShopListlen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.SaveShopListInfo( ?,? ) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n SaveShopListInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}
/**
min~max사이의 유저를 찾아 idxLegion인덱스의 군단 바이너리 데이타를 찾는다.
*/
BOOL CUserDB::FindAccountLegionByPower(int powerMin, int powerMax,
	int idxLegion,
	ID idFinder,
	XArchive& arOut)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.FindAccountLegionPowerValue( '%d',  '%d',  '%d',  '%d' ) }"), idFinder, powerMin, powerMax, idxLegion);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS;

	int			tmpidAccount = 0;
	int			power = 0;
	int			ladder  = 0;
	int			accountLevel = 0;

	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };
	SQLLEN	sqlenLegionData = 0;
	BYTE	pLegionBuff[BINARY_BUFF_LEN];
	XCLEAR_ARRAY(pLegionBuff);
	BYTE	buffAbil[ BINARY_BUFF_LEN ];
	SQLLEN sqllenAbil;
	XCLEAR_ARRAY( buffAbil );

	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	ret = BindCol( SQL_C_LONG, &ladder, sizeof( int ), &nLen, i++ );
	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &pLegionBuff, BINARY_BUFF_LEN, &sqlenLegionData, i++);
	ret = BindCol( SQL_C_BINARY, &buffAbil, BINARY_BUFF_LEN, &sqllenAbil, i++ );
	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		arOut << (ID)tmpidAccount;
		arOut << _tstring(szName);
		arOut << power;
	    arOut << ladder;
		arOut << accountLevel;

		if (XASSERT(sqlenLegionData > 0))
		{
			XArchive arLegion;
			arLegion.SetBufferMem(pLegionBuff, sqlenLegionData);
			arOut << arLegion;
		}
		else
      return FALSE;
    if( XBREAK(sqllenAbil <= 0) )
      return FALSE;
    {
      XArchive arAbil;
      arAbil.SetBufferMem( buffAbil, sqllenAbil );
      arOut << arAbil;
    }
		return TRUE;
	}
	else
		if (ret == SQL_NO_DATA)
		{
		// 적절한 대상을 못찾은 경우
		return FALSE;
		}
		else
		{
			// 이건 그냥 에러.
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
			BREAK_AND_LOG();
		}

	return FALSE;
}

/**

mandrake1_idOwner
mandrake1_idEnemy
mandrake1_szName
mandrake1_win
mandrake1_reward
mandrake1_idxLegion
mandrake1_offwin
...
mandrake2_idOwner
...


min~max스코어 사이의 유저중 idOwner가 0이아니고 idEnemy가 0인 유저를 찾아 그 정보를 돌려준다.
*/
BOOL CUserDB::FindMatchMandrake(ID idFinder, int idxMandrake, int powerMin, int powerMax, XArchive& arOut)
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	SQLRETURN	ret;
	_stprintf_s(szQuery, 1024, _T("{ call dbo.FindMatchMandrake( '%d',  '%d',  '%d', '%d' ) }"), idFinder, idxMandrake, powerMin, powerMax);
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS, fbnickLen = SQL_NTS;
	SQLLEN sqllenHello = SQL_NTS;
	int			tmpidAccount = 0;
	int			power = 0;
	int			ladder = 0;
	int			accountLevel = 0;
	int			mandrake_win = 0;
	int			mandrake_reward = 0;
	int			mandrake_idxLegion = 0;
	
	BYTE		buffAbil[BINARY_BUFF_LEN];
	SQLLEN		sqllenAbil;
	XCLEAR_ARRAY(buffAbil);

	TCHAR	szName[ LEN_LOGIN_ID * 2 ] = {0, };
	TCHAR	szfbName[ LEN_LOGIN_ID * 2 ] = {0, };
	TCHAR	szHello[ 80 ] = {0, };
//	const int numBuffHello = XNUM_ARRAY(szHello);
	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szfbName, LEN_LOGIN_ID * 2, &fbnickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &sqllenHello, i++); //hello 2016.01.20
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &ladder, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &mandrake_win, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &mandrake_reward, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &mandrake_idxLegion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &buffAbil, BINARY_BUFF_LEN, &sqllenAbil, i++);
	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {//!= SQL_NO_DATA)
#ifdef _XUZHU
// #pragma message("==================코드삭제======================================")
// 		tmpidAccount = 213505001;
// 		_tcscpy_s(szName, _T("성덕선"));
// 		accountLevel = 13;
// 		mandrake_reward = 2000;
// 		mandrake_idxLegion = 0;
// 		mandrake_win = 2;
#endif // _XUZHU
		arOut << (ID)tmpidAccount;
		arOut << szName;
		arOut << accountLevel;
		arOut << mandrake_win;
		arOut << mandrake_reward;
		arOut << mandrake_idxLegion;
		arOut << power;
		arOut << ladder;
		arOut << szfbName;
		arOut << szHello;
    if( XBREAK( sqllenAbil <= 0 ) )
      return FALSE;
    {
      XArchive arAbil;
      arAbil.SetBufferMem( buffAbil, sqllenAbil );
      arOut << arAbil;
    }
		return TRUE;
	} else {
		if (ret == SQL_NO_DATA) {
			return FALSE;
		} else {
			TCHAR err[1024];
			ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
			OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
			BREAK_AND_LOG();
		}
	}
	return FALSE;
}

/**
idAccount유저의 idxMandrake번호의 만드레이크 스팟의 군단정보를 비롯한 기타정보를 읽어온다.
*/
BOOL	CUserDB::FindMandrakeInfoByIdAcc(ID idAccount, int idxMandrake, XArchive& arOut)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.FindMandrakeInfoByIdAcc( '%d',  '%d') }"), idAccount, idxMandrake);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, nickLen = SQL_NTS;

	int			accountLevel = 0;
	int			Mandrake_win = 0;
	int			Mandrake_reward = 0;
	int			Mandrake_legion = 0;
	int			power = 0;
	int			ladder = 0;

	TCHAR	szName[LEN_LOGIN_ID * 2] = { 0, };
	BYTE		pLegionBuff[BINARY_BUFF_LEN];
	SQLLEN	sqlenLegionData = 0;
	BYTE	buffAbil[ BINARY_BUFF_LEN ];
	SQLLEN sqllenAbil;
	XCLEAR_ARRAY( buffAbil );
	XCLEAR_ARRAY(pLegionBuff);

	ret = BindCol(SQL_C_LONG, &accountLevel, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szName, LEN_LOGIN_ID * 2, &nickLen, i++);
	ret = BindCol(SQL_C_LONG, &Mandrake_win, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Mandrake_reward, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &Mandrake_legion, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &pLegionBuff[0], BINARY_BUFF_LEN, &sqlenLegionData, i++);
	ret = BindCol(SQL_C_LONG, &power, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &ladder, sizeof(int), &nLen, i++);
    ret = BindCol( SQL_C_BINARY, &buffAbil, BINARY_BUFF_LEN, &sqllenAbil, i++ );
	ret = Fetch();

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		arOut << accountLevel;
		arOut << szName;
		arOut << Mandrake_win;
		arOut << Mandrake_reward;
		arOut << Mandrake_legion;
		{
			XArchive arLegion;
			if (XASSERT(sqlenLegionData > 0))
			{
				arLegion.SetBufferMem(pLegionBuff, sqlenLegionData);
				arOut << arLegion;
			}
		}
    if( XBREAK( sqllenAbil <= 0 ) )
      return FALSE;
    {
      XArchive arAbil;
      arAbil.SetBufferMem( buffAbil, sqllenAbil );
      arOut << arAbil;
    }

		return TRUE;
	}
	else
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Data Error  (err:%d) : %s"), __TFUNC__, ret, err);
		//	BREAK_AND_LOG();
	}

	return FALSE;
}

/**
idAcc계정의 만드레이크 스팟정보를 써넣는다.
addOffWin값은 db.offWin값에 더해넣는다.
*/
BOOL CUserDB::ChangeOwnerMandrakeMine(ID idAccount, int idxMandrake, ID idNewOwner, ID idEnemy, _tstring& strName, int win, int reward, int idxLegion, int addOffWin)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN sqlenName = SQL_NTS;
	SQLLEN retLen = 0;

	_tstring strtargetName = strName;
	int Namelen = strtargetName.length();

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idxMandrake, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idNewOwner, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idEnemy, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &win, 0, &retLen, i++);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &reward, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idxLegion, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &addOffWin, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)Namelen, 0, (SQLPOINTER*)strtargetName.c_str(), 0, &sqlenName, i++);

	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.UpdateOwnerMandrake( ?,?,?,?,?, ?,?,?, '%s') }"), strtargetName.c_str());

	//if ((ret = Excute(_T("{ call dbo.UpdateOwnerMandrake( ?,?,?,?,?, ?,?,?,'?') }"))) != SQL_SUCCESS)
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Error : %s"), __TFUNC__, err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

/*
.DB함수 작성
.보상 수거
.만드레이크 상대에게 데이타 잘 써졌는지 테스트
*/


BOOL CUserDB::LoadServerRanking( XArchive& arOut)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadServerRanking }"));

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, usernameLen = SQL_NTS;

	int			tmpidAccount = 0;
	TCHAR		szUserName[LEN_POST_NAME] = { 0, };
	int			ladderpoint = 0;

	ret = BindCol(SQL_C_LONG, &tmpidAccount, sizeof(ID), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szUserName, LEN_LOGIN_ID * 2, &usernameLen, i++);
	ret = BindCol(SQL_C_LONG, &ladderpoint, sizeof(int), &nLen, i++);

	XRanking::s_rank info;
	std::list<XRanking::s_rank> rank_list;
	while ((ret = Fetch()) == SQL_SUCCESS || (ret = Fetch()) == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		info.s_idaccount = tmpidAccount;
		info.s_username = szUserName; //NickName
		info.s_ladderpoint = ladderpoint;

		rank_list.push_back(info);
	}

	if (rank_list.size() > 0)
	{
		arOut << (int)rank_list.size();

		std::list<XRanking::s_rank>::iterator begin = rank_list.begin();
		for (; begin != rank_list.end(); begin++)
		{
			XRanking::s_rank Data = (XRanking::s_rank)*begin;
			arOut << Data.s_idaccount;
			arOut.WriteString(Data.s_username.c_str());
			arOut << Data.s_ladderpoint;
		}
	}
	else
		arOut << 0;
	return TRUE;
}

BOOL CUserDB::CreateNewGuild( const _tstring& strName
														, const _tstring& strContext
														, const _tstring& strUserName
														, int Ver
														, ID idAcc
														, ID* pOutidGuild)		//CREATE Guild
{
	if( XBREAK( strName.length() > 32 ) )
		return FALSE;
	if( XBREAK( strContext.length() > 256 ) )
		return FALSE;
	ReAllocStmt();
	SQLRETURN ret;
	int retValue = 0;
	SQLLEN	retLen = 0; //, GuildNameLen = SQL_NTS, UserNameLen = SQL_NTS, GuildContextLen = SQL_NTS;
	SQLUSMALLINT i = 1;
// 	int guildnamelen = _tcslen(strGuildName.c_str());
// 	int usernamelen = _tcslen(strUserName.c_str());
// 	int GuildContextlen = _tcslen(strGuildContext.c_str());
// 	XBREAK(guildnamelen == 0);
// 	XBREAK(usernamelen == 0);
	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &retValue, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &Ver, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)guildnamelen, 0, (TCHAR*)strGuildName.c_str(), 0, &GuildNameLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)usernamelen, 0, (TCHAR*)strUserName.c_str(), 0, &UserNameLen, i++);
//	BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)GuildContextlen, 0, (TCHAR*)strGuildContext.c_str(), 0, &UserNameLen, i++);

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ ? = call dbo.CreateGuildInfo( ?,?, '%s', '%s', '%s') }")
											, strName.c_str()
											, strUserName.c_str()
											, strContext.c_str());
	if( (ret = Excute( szQuery )) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n CreateGuildInfo Error : %s" ), err );
		BREAK_AND_LOG();
		return FALSE;
	}
	if( retValue > 0 ) {
		(*pOutidGuild) = retValue;
		return TRUE;
	}
	return FALSE;
}

/**
 @brief 길드를 삭제한다.
*/
xtGuildError CUserDB::DeleteGuild(ID idGuild)
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ call dbo.DeleteGuild( %d ) }"), idGuild);
	SQLRETURN	ret = Excute( szQuery );
	if( ret != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n DeleteGuild Execute Error (err:%d) : %s" ), ret, err );
		BREAK_AND_LOG();
		return xGE_ERROR_DB;
	}
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		// 삭제 성공
	} else
	if( ret == SQL_NO_DATA ) {
		return XGE_ERROR_GUILD_NOT_FOUND;
	} else {
		// 현재 이 에러나서 막아둠.(잘못된 커서 상태에러)
// 		TCHAR err[1024];
// 		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
// 		OutputTextFormat( _T( "\n %s Data Error  (err:%d) : %s" ), __TFUNC__, ret, err );
// 		BREAK_AND_LOG();
// 		return xGE_ERROR_DB;
	}
	return xGE_SUCCESS;
}

BOOL CUserDB::GuildNameDuplicateCheck(_tstring strGuildName)
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 1024, _T("{ call dbo.GuildNameDuplicate( '%s' ) }"), strGuildName.c_str());

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n GuildNameDuplicate Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	int GuildIndex = 0;
	SQLUSMALLINT i = 1;
	SQLLEN pwLen = SQL_NTS, nickLen = SQL_NTS, dataLen = 0, nLen = 0;

	ret = BindCol(SQL_C_LONG, &GuildIndex, sizeof(int), &nLen, i++);

	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if (GuildIndex > 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CUserDB::LoadGuildInfo(XArchive& arOut)
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	SQLRETURN	ret;
	_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadGuildInfo }"));
	if( ( ret = Excute( szQuery ) ) != SQL_SUCCESS ) {
		TCHAR err[1024];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
		return FALSE;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, usernameLen = SQL_NTS;
	int		tmpguildindex = 0;
	TCHAR	szguildname[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szmastername[LEN_LOGIN_ID * 2] = { 0, };
	TCHAR	szcontext[256 * 2] = { 0, };
	int		ver = 0;
	int		curr_sn = 0;
	int		masteridacc = 0;
	int		level = 0;
	int		maxmembercount;
	BYTE	memberlistbuff1[BINARY_BUFF_LEN];
	BYTE	memberlistbuff2[BINARY_BUFF_LEN];
	BYTE	Optionbuff[BINARY_BUFF_LEN2];

	SQLLEN		binary1Len = 0;
	SQLLEN		binary2Len = 0;
	SQLLEN		binary3Len = 0;

	ret = BindCol(SQL_C_LONG, &tmpguildindex, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szguildname, LEN_LOGIN_ID * 2, &usernameLen, i++);
	ret = BindCol(SQL_C_LONG, &ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &curr_sn, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &masteridacc, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szmastername, LEN_LOGIN_ID * 2, &usernameLen, i++);
	ret = BindCol(SQL_C_WCHAR, szcontext, 256 * 2, &usernameLen, i++);
	ret = BindCol(SQL_C_LONG, &maxmembercount, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_LONG, &level, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &memberlistbuff1, BINARY_BUFF_LEN, &binary1Len, i++);
	ret = BindCol(SQL_C_BINARY, &memberlistbuff2, BINARY_BUFF_LEN, &binary2Len, i++);
	ret = BindCol(SQL_C_BINARY, &Optionbuff, BINARY_BUFF_LEN2, &binary3Len, i++);

	std::list<XGuild*> guild_list;
	while( ( ret = Fetch() ) == SQL_SUCCESS || ( ret = Fetch() ) == SQL_SUCCESS_WITH_INFO ) {//!= SQL_NO_DATA)
		//#pragma message("DeSerializeGuildMemberlist에서 에러나서 막아둠.")
		auto pGuild = new XGuild;
		pGuild->SetGuildIndex( tmpguildindex );
		pGuild->SetstrName( szguildname );
		pGuild->SetidAccMaster( masteridacc );
		pGuild->SetstrMasterName( szmastername );
		pGuild->SetMaxMemberCount( maxmembercount );
		pGuild->SetstrGuildContext( szcontext );
		pGuild->SetVer( ver );
		pGuild->SetCurrSN( curr_sn );
		// 길드원 정보 읽기
		if( binary1Len > 0 ) {
			XArchive memberlist;
			memberlist.SetBufferMem( memberlistbuff1, binary1Len );
			pGuild->DeSerializeGuildMemberlist( memberlist );
		}
		if( binary2Len > 0 ) {
			// 길드 요청 유저 정보 읽기
			XArchive joinreqmemberlist;
			joinreqmemberlist.SetBufferMem( memberlistbuff2, binary2Len );
			pGuild->DeSerializeGuildReqMemberlist( joinreqmemberlist );
		}
		if( binary3Len > 0 ) {
			// 길드 옵션 정보 읽기
			XArchive guildoption;
			guildoption.SetBufferMem( Optionbuff, binary3Len );
			pGuild->DeSerializeGuildOption( guildoption );
		}
		guild_list.push_back( pGuild );
	}

	if( guild_list.size() > 0 ) {
		arOut << (int)guild_list.size();
		std::list<XGuild*>::iterator begin = guild_list.begin();
		while( begin != guild_list.end() ) {
			XGuild* pinfo = *begin;
			if( pinfo ) {
				pinfo->Serialize( arOut );
//				pinfo->ClearGuild();
				guild_list.erase( begin++ );
				SAFE_DELETE( pinfo );
			} else
				begin++;
		}
	} else {
		arOut << 0;
	}

	return TRUE;
}

//POST
BOOL CUserDB::LoadPostInfoByIdAcc(ID idaccount, XArchive& arOut)
{
	//발신일, 만료일 계산 해서 읽어 들일때 삭제 해야 할 메일은 삭제.
	ReAllocStmt();

	TCHAR szQuery[1024];
	SQLRETURN	ret;

	_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadPostInfoByIdAcc( '%d' ) }"), idaccount);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return FALSE;
	}

	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, dataLen = 0, sendernameLen = SQL_NTS, titleLen = SQL_NTS, msglen = SQL_NTS, sendtimelen = SQL_NTS, expiretimelen = SQL_NTS;

	int			tmpidAccount = 0;
	int			Ver = 0;
	TCHAR		szSenderName[LEN_POST_NAME] = { 0, };
	short		posttype = 0;
	short		poststatus = 0;
	TCHAR		szTitle[LEN_POST_TITLE] = { 0, };
	TCHAR		szMessage[LEN_POST_MSG] = { 0, };
	TCHAR		szSendTime[LEN_POST_TITLE] = { 0, };
	TCHAR		szIdentyfy[LEN_POST_TITLE] = { 0, };
	BYTE		withItem = 0;
	ID			RecvIndex = 0;

	SQLLEN		WithItemLen = 0;
	BYTE		withItemBuff[BINARY_BUFF_LEN2];
	::memset(&withItemBuff, 0x0, BINARY_BUFF_LEN2);

	ret = BindCol(SQL_C_LONG, &RecvIndex, sizeof(ID), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szSenderName, LEN_LOGIN_ID * 2, &sendernameLen, i++);
	ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_SHORT, &posttype, sizeof(short), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szTitle, LEN_POST_TITLE, &titleLen, i++);
	ret = BindCol(SQL_C_WCHAR, szMessage, LEN_POST_MSG, &msglen, i++);
	ret = BindCol(SQL_C_TINYINT, &withItem, sizeof(BYTE), &nLen, i++);
	ret = BindCol(SQL_C_BINARY, &withItemBuff, BINARY_BUFF_LEN2, &WithItemLen, i++);
		
	XArchive arloadpost;
	int nCount = 0;
	while ((ret = Fetch()) == SQL_SUCCESS || (ret = Fetch()) == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		arloadpost << RecvIndex;		
		_tstring strBuff = szTitle;		
		arloadpost << strBuff;
		strBuff = szMessage;
		arloadpost << strBuff;
		strBuff = szSenderName;
		arloadpost << strBuff;
		arloadpost << (ID)withItem;
		if( withItem > 0 ) {
			XArchive arItemBuff;
			arItemBuff.SetBufferMem( withItemBuff, WithItemLen );

			for( int n = 0; n < withItem; n++ ) {
				DWORD dw1, dw2;
				arItemBuff >> dw1;	auto typePost = (xtPostType)dw1;
				arloadpost << (DWORD)typePost;
				if( typePost == XGAME::xtPOSTResource::xPOSTRES_GOLD ) {
					arItemBuff >> dw2;
					arloadpost << dw2;
				} else if( typePost == XGAME::xtPOSTResource::xPOSTRES_GEM ) {
					arItemBuff >> dw2;
					arloadpost << dw2;
				} else if( typePost == XGAME::xtPOSTResource::xPOSTRES_ITEMS ) {
					arItemBuff >> dw2;
					arloadpost << dw2;
					arItemBuff >> dw2;
					arloadpost << dw2;
				} else if( typePost == XGAME::xtPOSTResource::xPOSTRES_RESOURCE ) {
					arItemBuff >> dw2;
					arloadpost << dw2;
					arItemBuff >> dw2;
					arloadpost << dw2;
				} else if( typePost == XGAME::xtPOSTResource::xPOSTRES_GUILD_COIN ) {
					arItemBuff >> dw2;
					arloadpost << dw2;
				}
			}
		}
		nCount++;
	}
	arOut << (ID)idaccount;
	if( nCount > 0 ) {
		arOut << nCount;
		for( int n = 0; n < nCount; n++ ) {
			DWORD dw1;
			_tstring strBuff;
			arloadpost >> dw1;		arOut << dw1;
			arloadpost >> strBuff;	arOut << strBuff;
			arloadpost >> strBuff;	arOut << strBuff;
			arloadpost >> strBuff;	arOut << strBuff;
			arloadpost >> dw1;		arOut << dw1;

			if( dw1 > 0 && dw1 <= 5 ) {
				for( int i = 0; i < (int)dw1; i++ ) {
					DWORD dw2;
					//DWORD dw3;
					arloadpost >> dw2;
					if( dw2 == XGAME::xtPOSTResource::xPOSTRES_GOLD ) {
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
					} else if( dw2 == XGAME::xtPOSTResource::xPOSTRES_GEM ) {
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
					} else if( dw2 == XGAME::xtPOSTResource::xPOSTRES_ITEMS ) {
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
					} else if( dw2 == XGAME::xtPOSTResource::xPOSTRES_RESOURCE ) {
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
					} else if( dw2 == XGAME::xtPOSTResource::xPOSTRES_GUILD_COIN ) {
						arOut << dw2;
						arloadpost >> dw2;
						arOut << dw2;
					}
				}
			}
		}
	} else {
		arOut << 0;
	}
	return TRUE;
}
//
/*
BOOL CUserDB::LoadPostItemsInfo(ID postindex, ID idaccount, XArchive& arOut)
{
//발신일, 만료일 계산 해서 읽어 들일때 삭제 해야 할 메일은 삭제.
ReAllocStmt();

TCHAR szQuery[1024];
SQLRETURN	ret;

_stprintf_s(szQuery, 1024, _T("{ call dbo.LoadPostItemsInfo( '%d' ) }"), postindex);

if ((ret = Excute(szQuery)) != SQL_SUCCESS)
{
TCHAR err[1024];
ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
BREAK_AND_LOG();
return FALSE;
}

SQLUSMALLINT i = 1;
SQLLEN nLen = 0;

int			Ver = 0;
int			ItemPropnum = 0;
int			nCount = 0;
short		Status = 0;
int			pos = 0;

ret = BindCol(SQL_C_LONG, &Ver, sizeof(int), &nLen, i++);
ret = BindCol(SQL_C_LONG, &ItemPropnum, sizeof(int), &nLen, i++);
ret = BindCol(SQL_C_LONG, &nCount, sizeof(int), &nLen, i++);
ret = BindCol(SQL_C_SHORT, &Status, sizeof(short), &nLen, i++);
ret = BindCol(SQL_C_LONG, &pos, sizeof(int), &nLen, i++);

std::list<XPostItem> postitem_list;
XPostItem e;
while ((ret = Fetch()) == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
{
e.SetItemID(ItemPropnum);
e.SetnCount(nCount);
e.SetStatus(Status);
e.SetIndex(pos);
postitem_list.push_back(e);
}

if (postitem_list.size() > 0)
{
arOut << (ID) postitem_list.size();

std::list<XPostItem>::iterator begin = postitem_list.begin();
std::list<XPostItem>::iterator end = postitem_list.end();
for (; begin != end; begin++)
{
XPostItem Data = (XPostItem)*begin;
Data.Serialize(arOut);
}
}
return TRUE;
}
*/
// BOOL CUserDB::AddPostInfo(ID idaccount
// 												, ID postsn
// 												, ID PostType
// 												, _tstring SenderName
// 												, _tstring RecvName
// 												, _tstring Title
// 												, _tstring message
// 												, int nCount
// 												, XArchive& ariteminfo
// 												, ID rewardtableid)
// {
// 	ReAllocStmt();
// 	SQLUSMALLINT i = 1;
// 	SQLLEN retLen = 0;
// 	SQLLEN SenderNameLen = SQL_NTS;
// 	SQLLEN ReceiveNameLen = SQL_NTS;
// 	SQLLEN TitleLen = SQL_NTS;
// 	SQLLEN MessageLen = SQL_NTS;
// 	SQLLEN identifyLen = SQL_NTS;
// 
// 	int sendernamelen = wcslen(SenderName.c_str());
// 	int receivenamelen = wcslen(RecvName.c_str());
// 	int titlelen = wcslen(Title.c_str());
// 	int messagelen = wcslen(message.c_str());
// 
// 	short sPostType = (short)PostType;
// 
// 	SQLLEN sqlenItemlen = ariteminfo.size();
// 
// 
// 	//ID PostIndex = 0;		
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &postsn, 0, &retLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &sPostType, 0, &retLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &rewardtableid, 0, &retLen, i++);
// 	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)sendernamelen, 0, (SQLPOINTER*)SenderName.c_str(), 0, &SenderNameLen, i++);
// 	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)titlelen, 0, (SQLPOINTER*)Title.c_str(), 0, &TitleLen, i++);
// 	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)messagelen, 0, (SQLPOINTER*)message.c_str(), 0, &MessageLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nCount, 0, &retLen, i++);
// 	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)ariteminfo.GetBuffer(), 0, &sqlenItemlen, i++);
// 
// 	SQLRETURN ret;
// 
// 	TCHAR szQuery[1024];
// 	_stprintf_s(szQuery, 1024, _T("{ call dbo.AddPostInfo( ?,?,?,?,'%s', '%s','%s',?,?) }"), SenderName.c_str(), Title.c_str(), message.c_str());
// 	//if ((ret = Excute(_T("{ call dbo.AddPostInfo( ?,?,?,?,?, ?,?,?,?) }"))) != SQL_SUCCESS)	
// 	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	
// 	{
// 		TCHAR err[1024];
// 		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
// 		OutputTextFormat(_T("\n AddPostInfo Error : %s"), err);
// 		BREAK_AND_LOG();
// 		return FALSE;
// 	}
// 
// 	return TRUE;
// }
bool CUserDB::AddPostInfo(ID idAcc
												, xtPostType type
												, const XPostInfo* pPostInfo )
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN retLen = 0;
//	SQLLEN SenderNameLen = SQL_NTS;
//	SQLLEN ReceiveNameLen = SQL_NTS;
//	SQLLEN TitleLen = SQL_NTS;
//	SQLLEN MessageLen = SQL_NTS;
//	SQLLEN identifyLen = SQL_NTS;

	ID snPost = pPostInfo->GetsnPost();
//	int sendernamelen = wcslen(SenderName.c_str());
//	int receivenamelen = wcslen(RecvName.c_str());
//	int titlelen = wcslen(Title.c_str());
//	int messagelen = wcslen(message.c_str());

	short sPostType = (short)type;
	ID idRewardTable = pPostInfo->GetRewardTableid();
	int nCount = pPostInfo->GetnumItems();
	XArchive arItems;
	pPostInfo->SerializeItems( arItems );
	SQLLEN sqlenItemlen = arItems.size();
	LPCTSTR szSender = pPostInfo->GetstrSenderName();
	LPCTSTR szTitle = pPostInfo->GetstrTitle();
	LPCTSTR szMsg = pPostInfo->GetstrMessage();

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idAcc, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &snPost, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &sPostType, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idRewardTable, 0, &retLen, i++);
	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)sendernamelen, 0, (SQLPOINTER*)SenderName.c_str(), 0, &SenderNameLen, i++);
	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)titlelen, 0, (SQLPOINTER*)Title.c_str(), 0, &TitleLen, i++);
	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)messagelen, 0, (SQLPOINTER*)message.c_str(), 0, &MessageLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &nCount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, BINARY_BUFF_LEN2, 0, (SQLPOINTER*)arItems.GetBuffer(), 0, &sqlenItemlen, i++);

	SQLRETURN ret;

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ call dbo.AddPostInfo( ?,?,?,?,'%s', '%s','%s',?,?) }")
													, szSender, szTitle, szMsg);
	//if ((ret = Excute(_T("{ call dbo.AddPostInfo( ?,?,?,?,?, ?,?,?,?) }"))) != SQL_SUCCESS)	
	auto err = ret = Excute(szQuery);
	if (err == SQL_SUCCESS )	{
		return true;
	} else
	if( err == SQL_NO_DATA ) {
		return false;
	} else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n AddPostInfo Error : %s"), err);
		BREAK_AND_LOG();
		return false;
	}
	return true;
}

BOOL CUserDB::UpdatePost(XSPDBUAcc spAcc)
{
	if (spAcc == NULL) return FALSE;
	int nCount = spAcc->GetNumPostInfo();
	if (nCount <= 0) return FALSE;

	ID idaccount = spAcc->GetidAccount();

	//Post 저장 하고.	

	XList<XPostInfo*>::Itor itor = spAcc->GetPostInfo()->GetNextClear2();
	for (int n = 0; n < nCount; ++n)
	{
		XPostInfo* E = spAcc->GetPostInfo()->GetNext2(itor);
		if (E)
		{
			int i = 'casr';
			//UpdatePost(idaccount, E->GetIndex(), E->GetStatus());
			//if (E->GetnumItems() > 0) //PostItem 저장 하고..
			//{
			//	int _max = E->GetlistPostItems()->size();
			//	XList<XPostItem*>::Itor _itor = E->GetlistPostItems()->GetNextClear2();
			//	for (int i = 0; i < _max; ++i)
			//	{					
			//		XPostItem* e = E->GetlistPostItems()->GetNext2(_itor);
			//		if (e)
			//		{
			//			UpdatePostItems(idaccount, E->GetIndex(), e->GetItemID(), e->GetIndex(), e->GetStatus());
			//		}
			//	}
			//}
		}
	}
	return TRUE;
}
BOOL CUserDB::RemovePostItems(ID idaccount, ID postsn)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;
	SQLLEN		identifyLen = SQL_NTS;
	//	int Postidentilen = _tcslen(szPostidentify);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &postsn, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.RemovePostInfoItems( ?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n RemovePostInfoItems Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}

	return TRUE;
}
BOOL CUserDB::DeletePostInfo(ID idaccount, ID postsn)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;
	SQLLEN		identifyLen = SQL_NTS;
	//	int Postidentilen = _tcslen(szPostidentify);

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &postsn, 0, &retLen, i++);
	//BindParameter(SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)Postidentilen, 0, szPostidentify, 0, &identifyLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.DeletePostInfo( ?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n DeletePostInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}

	return TRUE;
}

BOOL CUserDB::UpdatePost(ID idaccount, ID index, short status)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &index, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &status, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.UpdatePostInfo( ?,?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdatePostInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}

	return TRUE;
}
BOOL CUserDB::UpdatePostItems(ID idaccount, ID index, int itemid, short itempos, short itemstatus)
{
	ReAllocStmt();
	SQLUSMALLINT i = 1;
	SQLLEN		retLen = 0;

	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &idaccount, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &index, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, &itemid, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &itempos, 0, &retLen, i++);
	BindParameter(SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(short), 0, &itemstatus, 0, &retLen, i++);

	SQLRETURN ret;

	if ((ret = Excute(_T("{ call dbo.UpdatePostItemsInfo( ?,?,?,?,?) }"))) != SQL_SUCCESS)
	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n UpdatePostItemsInfo Error : %s"), err);
		BREAK_AND_LOG();
		return FALSE;
	}
	return TRUE;
}

#ifdef _INAPP_GOOGLE
//BYTE CUserDB::CreateGooglePayload(ID idacc, int priductType, _tstring strProductID, _tstring& strpayload)
XGAME::xtErrorIAP CUserDB::CreateGooglePayload( ID idacc
																							, XGAME::xtCashType typeProduct
																							, const _tstring& strProductID
																							, const _tstring& strPayload )
{
#ifdef _FAKE_PAYLOAD
	GUID guid;
	auto hr = CoCreateGuid( &guid );
	if( hr != S_OK )
		return XGAME::xIAPP_ERROR_UNKNOWN;
	*pOutstrPayload = XFORMAT("%08X%04X%04X"
															, guid.Data1
															, guid.Data2
															, guid.Data3 );
	for( int i = 0; i < XNUM_ARRAY(guid.Data4); ++i ) {
		*pOutstrPayload += XFORMAT("%02X", guid.Data4[i] );
	}
// 	const std::string strcPayload = XE::GetUUID();
// 	*pOutstrPayload = C2SZ(strcPayload);
	return XGAME::xIAPP_SUCCESS;
#else
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s(szQuery, 
		_T("{ ? = call dbo.CreateGooglePayload( %d, %d, '%s', '%s') }")
					, idacc
					, (int)typeProduct
					, strProductID.c_str()
					, strPayload.c_str() );
	SQLRETURN	ret;
	int resultIdAcc = 0;
	SQLLEN payloadLen = SQL_NTS, nLen = SQL_NTS;
	
	BindParameter(SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, (SQLUINTEGER)sizeof(int), 0, &resultIdAcc, 0, &nLen, 1);

	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n CreateGooglePayload Execute Error (err:%d) : %s"), ret, err);
		BREAK_AND_LOG();
		return XGAME::xIAPP_ERROR_CRITICAL;
	}	
	


	auto errIap = XGAME::xIAPP_ERROR_UNKNOWN;
	
	if ((DWORD)resultIdAcc == idacc) errIap = XGAME::xtErrorIAP::xIAPP_SUCCESS;
	else errIap = XGAME::xtErrorIAP::xIAPP_ERROR_DUPLICATE_RECEIPT;
	
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)//!= SQL_NO_DATA)
	{
		if (errIap == XGAME::xIAPP_SUCCESS ) {
			//*pOutstrPayload = szBuff;
		}		
	}
	return errIap;
#endif // not _FAKE_PAYLOAD
}
/**
 @brief DB에서 strPayload를 검색한다.
*/
XGAME::xtErrorIAP 
CUserDB::LoadGooglePayload(ID idacc, const _tstring& strPayload, _tstring* pOutidsProduct )
{
	auto errCode = XGAME::xIAPP_ERROR_UNKNOWN;

	ReAllocStmt();

	// payload로 검색
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ call dbo.LoadGooglePayload( '%d', '%s') }")
													, idacc
													, strPayload.c_str() );

	SQLRETURN	ret;
	if ((ret = Excute(szQuery)) != SQL_SUCCESS)	{
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Execute Error (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
		return XGAME::xIAPP_ERROR_CRITICAL;
	}
	SQLUSMALLINT i = 1;
	int result = (int)XGAME::xIAPP_ERROR_UNKNOWN;
	SQLLEN payloadLen = SQL_NTS, nLen = SQL_NTS;
	TCHAR szPayloadBuff[LEN_PAYLOAD] = {0,};
	int typeProduct = 0;
	TCHAR szProductID[80] = {0,};
	
	ret = BindCol(SQL_C_WCHAR, szPayloadBuff, LEN_PAYLOAD, &payloadLen, i++);
	ret = BindCol(SQL_C_LONG, &typeProduct, sizeof(int), &nLen, i++);
	ret = BindCol(SQL_C_WCHAR, szProductID, LEN_PAYLOAD, &payloadLen, i++);

	ret = Fetch();

//	auto errIap = XGAME::xIAPP_ERROR_UNKNOWN;
	if( ret == SQL_NO_DATA ) {
		errCode = XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD;
	} else
	if( strPayload == szPayloadBuff ) {
		errCode = XGAME::xIAPP_SUCCESS;
		*pOutidsProduct = szProductID;
	}
	return errCode;
}

XGAME::xtErrorIAP 
CUserDB::DeletePayload( ID idAcc, const _tstring& strPayload )
{
	ReAllocStmt();

	TCHAR szQuery[1024];
	_stprintf_s( szQuery, _T( "{ call dbo.DeletePayload( '%d', '%s' ) }" )
														, idAcc
														, strPayload.c_str() );

	SQLRETURN	ret;
	if( ( ret = Excute( szQuery ) ) != SQL_SUCCESS ) {
		TCHAR err[ 1024 ];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
		return XGAME::xIAPP_ERROR_CRITICAL;
	}
	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) {//!= SQL_NO_DATA)
		return XGAME::xIAPP_SUCCESS;
	} else {
		TCHAR err[ 1024 ];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
	}
	return XGAME::xIAPP_ERROR_NOT_FOUND_PAYLOAD;
}
#endif // INAPP_GOOGLE
#ifdef _INAPP_APPLE	
BYTE CUserDB::InappAppleReceiptVerify(ID idacc, int priductType, _tstring strProductID, _tstring strReceipt)
{
}
#endif

/**
 @brief idAcc계정의 strHello를 저장한다.
*/
bool CUserDB::UpdateAccountHello( ID idacc, const _tstring& strHello ) //Hello 2016.01.20
{
	SQLLEN	retLen = 0, strHelloLen = SQL_NTS;
	//
	ReAllocStmt();
	SQLUSMALLINT i = 1;

	BindParameter( SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof( int ), 0, &idacc, 0, &retLen, i++ );
	BindParameter( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, (SQLUINTEGER)wcslen( strHello.c_str() ), 0, (TCHAR*)strHello.c_str(), 0, &strHelloLen, i++ );
	SQLRETURN ret = Excute( _T( "{ call dbo.UpdateAccountHello( ?,?) }" ) );
	if( ret == SQL_SUCCESS ) {
		return true;
	} else
	if( ret == SQL_NO_DATA ) {
		return false;
	} else {
		TCHAR err[ 1024 ];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n UpdateAccountHello Error : %s" ), err );
		BREAK_AND_LOG();
		return false;
	}
	return true;
}

/**
 @brief idAcc유저의 인사말 메시지를 얻는다.
*/
bool CUserDB::FindAccountHello( ID idAcc, _tstring *pOutStr )
{
	ReAllocStmt();
	TCHAR szQuery[1024];
	_stprintf_s(szQuery, _T("{ call dbo.FindAccountHello( '%d') }"), idAcc );
	SQLRETURN	ret = Excute( szQuery );
	if( ret != SQL_SUCCESS ) {
		TCHAR err[ 1024 ];
		ShowErrorInfo( SQL_HANDLE_STMT, err, 1024 );
		OutputTextFormat( _T( "\n %s Execute Error (err:%d) : %s" ), __TFUNC__, ret, err );
		BREAK_AND_LOG();
		return false;
	}
	SQLUSMALLINT i = 1;
	SQLLEN nLen = 0, helloLen = SQL_NTS;
	TCHAR	szHello[80] = { 0, };
	ret = BindCol(SQL_C_WCHAR, szHello, XNUM_ARRAY(szHello), &helloLen, i++);		//Hello 2016.01.20
	ret = Fetch();
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		(*pOutStr) = szHello;
		return true;
	} else
	if( ret == SQL_NO_DATA ) {
		return false;
	}	else {
		TCHAR err[1024];
		ShowErrorInfo(SQL_HANDLE_STMT, err, 1024);
		OutputTextFormat(_T("\n %s Error  (err:%d) : %s"), __TFUNC__, ret, err);
		BREAK_AND_LOG();
	}

	return false;
}
