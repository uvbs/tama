#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"
#include "XPool.h"
#include "XCrypto.h"
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
	XCrypto m_CryptObj;
	//
	void Init() {	}
	void Destroy() {	}
public:
	XClientConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XClientConnection() { Destroy(); }
	//
	GET_ACCESSOR( XCrypto&, CryptObj );
	//
	XSPGameUser CreateUserObj( ID idAccount, XSPSAcc spAccount, BOOL bReconnect );
	void OnAsyncVerify( XGameUser *pUser );
	void CreateFakeAccount( XSPSAcc spAccount );
	void OnInvalidVerify( void );
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );

	// cheat
#ifdef _CHEAT
#endif

	//Account 관련 
	void RecvLogin( XPacket& p );
	void RecvDisconnectMe( XPacket& ar );	

	void OnAccountInfoNotYetFromLoginSvr( ID idAccount, bool bToLogin );
	void SendLoginFailedGameSvr( DWORD idMsg );	
	// Lobby

	void SendNotify(_tstring strMessage);
	void SendGuildInfo(ID GuildIndex, ID GuildUpdateflag, XPacket& ar);
	void SendGuildInfoUpdate(ID idaccount, XPacket& ar);
	void SendInvalidVerify();
	void SendAccountDuplicateLogout();
//	void SendPostInfo( XSPSAcc pAcc );
};



