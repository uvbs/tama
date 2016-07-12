#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"
#include "XPool.h"
#include "XDBUAccount.h"
#include "XPool.h"


enum xtPacket;
class XAccount;
class XSAccount;
class XGameUser;

const float reconnect_time = 1.f;
// 월드서버에서 클라이언트들과 연결되는 커넥션 객체. XUser들이 하나씩 보유한다.
class XClientConnection : public XEWinConnectionInServer
												, public XMemPool<XClientConnection>
{	
public:
	static int s_numUsed;		//생성된 객체수
private:
// 	XSPLock m_spLock;
	void Init() {
	}
	void Destroy() {
		::InterlockedDecrement( (LONG*)( &s_numUsed ) );
	}
public:
	XClientConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XClientConnection() { Destroy(); }
	//
// 	GET_ACCESSOR( XSPLock&, spLock );
	void CreateUserObj( ID idAccount, XSAccount *spAccount, BOOL bReconnect );
	void OnAsyncVerify( XGameUser *pUser );
	void CreateFakeAccount( XSAccount *spAccount );
	void OnInvalidVerify( void );

	// cheat
#ifdef _CHEAT
#endif
	//Account 관련 
	void RecvRequestNewID( XPacket& p, ID idKey );
	void RecvRequesLoginID( XPacket& p, ID idKey );
	void RecvRequestLoginUUID( XPacket& p, ID idKey );
	void RecvRequestSESSIONID( XPacket& p, ID idKey );
	void RecvRequestSESSIONUUID(XPacket& p, ID idKey);
	void RecvRequesLoginByFacebook( XPacket& p, ID idKey );
	
	//PVP	
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );
	//
	//void RecvLoginFromUUID( XPacket& p, ID idKey );

	//CallBack
//	void cbCreateNewAccount( XDatabase *pDB, XSPDBUAcc sspAccOut, const _tstring& strUUID, const _tstring strName, const _tstring strDeviceID, const _tstring strAppstore, ID idKey );
	void cbCreateNewAccount2( XDatabase *pDB,
														const _tstring& strUUID,
														const _tstring& strNickName,
														const _tstring& strDeviceID,
														const _tstring& strAppstore,
														const _tstring& strFbUserId,
														const XGAME::xtConnectParam typeLogin,
														ID idKey );
	void cbFindAccount( XDatabase *pDB
										, XGAME::xtConnectParam typeLogin
										, const _tstring& strID
										, const _tstring& strPW
										, const _tstring& strDeviceID
										, const _tstring& strAppstore
										, ID idKey );
	void OnResultFindAccount( ID idKey
													, XArchive& arAcc
													, const XGAME::xtConnectParam typeLogin
													, const _tstring& strID
													, const _tstring& strPW );
	void OnResultCreateNewAccount( ID idKey
																, ID idAcc
																, const _tstring& strUUID
																, const _tstring& strNickName
																, const _tstring& strFbUserId
																, const XGAME::xtConnectParam typeLogin
																, bool bDuplicate );
	void cbFindAccountByUUID(	XDatabase *pDB,
								const _tstring& strUUID,
								const _tstring& strDeviceID,
								const _tstring& strAppstore,
//								_tstring& strNickName,								
								ID idKey );
	void OnResultFindAccountByUUID(ID idKey,
									ID idAcc,
									XArchive& arAcc,
									const _tstring& strUUID,
									const _tstring& strDeviceID,
									const _tstring& strAppstore);
// 	void cbFindAccountByFacebook(XDatabase *pDB
// 															, const _tstring& strtFbUserId
// 															, const _tstring& strDeviceID
// 															, const _tstring& strAppstore
// 															, ID idKey);
// 	void OnResultFindAccountByFacebook( ID idKey
// 																		,	const ID idAcc
// 																		, XArchive& arAcc
// 																		, const _tstring& strFbUserId );
	void OnResultFindAccountByUUIDSessionKey(ID idKey,
									BOOL bFound,
									XSPDBUAcc spAccount,
									_tstring strUUID,
									_tstring strSessionKey);
// 	void cbFindAccountByID( XDatabase *pDB,
// // 							XSPDBUAcc sspAccOut,
// 							const _tstring& strID,
// 							const _tstring& strPW,
// 							const _tstring& strDeviceID,
// 							const _tstring& strAppstore,
// 							ID idKey );
// 	void OnResultFindAccountByID( ID idKey,
// 								const ID idAcc,
// 								XArchive& arAcc,
// 								const _tstring& strID,
// 								const _tstring& strPW);

	void OnResultFindAccountByIDSessionKey(	ID idKey,
											BOOL bFound,
											XSPDBUAcc spAccount,
											_tstring strID,
											_tstring strPW,
											_tstring strSessionKey);

	void cbFindAccountByIDSession(	XDatabase *pDB,
									XSPDBUAcc sspAccOut,
									_tstring strID,
									_tstring strPW,
									_tstring strSessionKey,
									ID idKey );
	void cbFindAccountByUUIDSessionKey(XDatabase *pDB,
									XSPDBUAcc sspAccOut,
									_tstring strUUID,
									_tstring strSessionKey,
									ID idKey );
	void cbSendUserLog(XDatabase *pDB, ID idaccount, int LogType, _tstring strNickName, _tstring strLog);
	void SendNoAccount( XGAME::xtConnectParam param );
	void cbUnlockLoginForBattle();
	void SendUnlockLoginForBattle();
	void OnResultUserLoginSave(ID idacc, int svridx, _tstring strConnectip, _tstring strUserName);
	void cbUserLoginUpdate(XDatabase *pDB, ID idacc, ID idSvr, const _tstring& strConnectip, const _tstring& strUserName);
	void RecvCallStack( XPacket& p, ID idKey );
	void SendAccountNicknameDuplicate( const _tstring& strNickName
																		, const XGAME::xtConnectParam typeLogin );
	void SendClosedServer();
	void SendWrongPassword();
	void SendAccountReconnectTry();
	void SendAccountReqLoginInfo( ID idAcc, const char *cIP, WORD port, const _tstring& strUUID );
private:
	void RecvCreateNewAccount( XPacket& p, ID idKey );
	bool SendToGameSvrLoginAccInfo( ID idAcc, const _tstring& strNick, XGAME::xtConnectParam param, bool bSessionKeyLogin );
	void SendLoginLockForBattle( int secPass );
	bool IsInvalidPacketVer( DWORD verCLPK, DWORD verCGPK, DWORD verCPPK );
	void SendInvalidPacketVer();
	XSPXClientConnection GetThis() {
		return std::static_pointer_cast<XClientConnection>( XEWinConnectionInServer::GetThis() );
	}
private:
}; // class XClientConnection
