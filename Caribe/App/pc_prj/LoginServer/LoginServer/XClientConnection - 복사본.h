#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "server/XWinConnection.h"
#include "XPool.h"
#include "XDBUAccount.h"


enum xtPacket;
class XAccount;
class XSAccount;
class XGameUser;

const float reconnect_time = 1.f;
// 월드서버에서 클라이언트들과 연결되는 커넥션 객체. XUser들이 하나씩 보유한다.
class XClientConnection : public XEWinConnectionInServer, public XMemPool<XClientConnection>
{	
	void Init() {
	}
	void Destroy() {
		int n = 0;
	}
public:
	XClientConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XClientConnection() { Destroy(); }
	//
	void CreateUserObj( ID idAccount, XSAccount *pAccount, BOOL bReconnect );
	void OnAsyncVerify( XGameUser *pUser );
	void CreateFakeAccount( XSAccount *pAccount );
	void OnInvalidVerify( void );

	// cheat
#ifdef _CHEAT
#endif
	//Account 관련 
	void RecvRequestNewID( XPacket& p, ID idKey );
	void RecvRequesLogintID( XPacket& p, ID idKey );
	void RecvRequestLoginUUID( XPacket& p, ID idKey );
	void RecvRequestSESSIONID( XPacket& p, ID idKey );
	void RecvRequestSESSIONUUID(XPacket& p, ID idKey);
	
	//PVP	
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );
	//
	//void RecvLoginFromUUID( XPacket& p, ID idKey );

	//CallBack
	void cbCreateNewAccount( XDatabase *pDB,
							XDBUAccount *pAccOut, 
							_tstring& strUUID,
							_tstring strName,
							_tstring strDeviceID,
							_tstring strAppstore,
							ID idKey );
	void OnResultCreateNewAccount(	ID idKey,
									BOOL bFound, 
									XDBUAccount* pAccount,
									_tstring strNickName );
	void cbFindAccountByUUID(	XDatabase *pDB,
								XDBUAccount *pAccOut,
								_tstring& strUUID,
								_tstring strDeviceID,
								_tstring strAppstore,
								_tstring& strNickName,								
								ID idKey );
	void OnResultFindAccountByUUID(ID idKey,
									BOOL bFound,
									XDBUAccount* pAccount,
									_tstring& strUUID,
									_tstring& strNickName,
									_tstring strDeviceID,
									_tstring strAppstore);

	void OnResultFindAccountByUUIDSessionKey(ID idKey,
									BOOL bFound,
									XDBUAccount* pAccount,
									_tstring strUUID,
									_tstring strSessionKey);
	void cbFindAccountByID( XDatabase *pDB,
							XDBUAccount *pAccOut,
							_tstring strID,
							_tstring strPW,
							_tstring strDeviceID,
							_tstring strAppstore,
							ID idKey );
	void OnResultFindAccountByID( ID idKey,
								BOOL bFound,
								XDBUAccount* pAccount,
								_tstring strID,
								_tstring strPW);

	void OnResultFindAccountByIDSessionKey(	ID idKey,
											BOOL bFound,
											XDBUAccount* pAccount,
											_tstring strID,
											_tstring strPW,
											_tstring strSessionKey);

	void cbFindAccountByIDSession(	XDatabase *pDB,
									XDBUAccount *pAccOut,
									_tstring strID,
									_tstring strPW,
									_tstring strSessionKey,
									ID idKey );
	void cbFindAccountByUUIDSessionKey(XDatabase *pDB,
									XDBUAccount *pAccOut,
									_tstring strUUID,
									_tstring strSessionKey,
									ID idKey );
	void cbSendUserLog(XDatabase *pDB, ID idaccount, int LogType, _tstring strNickName, _tstring strLog);
	void SendNoAccount( XGAME::xtConnectParam param );
	void cbUnlockLoginForBattle();
	void SendUnlockLoginForBattle();
	void OnResultUserLoginSave(ID idacc, int svridx, _tstring strConnectip, _tstring strUserName);
	void cbUserLoginUpdate(XDatabase *pDB, ID idacc, int svridx, _tstring strConnectip, _tstring strUserName);
	void RecvCallStack( XPacket& p, ID idKey );
	void SendAccountNicknameDuplicate( BOOL bFound, const _tstring& strNickName );
	void SendClosedServer();
	void SendWrongPassword();
	void SendAccountReconnectTry();
	void SendAccountReqLoginInfo( ID idAcc, const char *cIP, WORD port );
};
