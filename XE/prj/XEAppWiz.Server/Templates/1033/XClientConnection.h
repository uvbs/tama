#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "server/XWinConnection.h"
#include "XPool.h"
#include "XGameUser.h"


enum xtPacket;
class XAccount;
class XSAccount;
class XGameUser;

const float reconnect_time = 1.f;
// 월드서버에서 클라이언트들과 연결되는 커넥션 객체. XUser들이 하나씩 보유한다.
class XClientConnection : public XEWinConnectionInServer, public XMemPool<XClientConnection>
{	
	friend void XGameUser::Send( XPacket& ar );
	void Init() {}
	void Destroy() {}
public:
	XClientConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XClientConnection() { Destroy(); }
	//
	void OnInvalidVerify();
	// cheat
#ifdef _CHEAT
#endif
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	virtual void OnError( xtError error, DWORD param1, DWORD param2 ) override;
public:
	void SendAccountDuplicateLogout();
private:
	void RecvLogin( XPacket& p );
	XGameUser* CreateUserObj( ID idAccount, XSAccount *pAccount, BOOL bReconnect );
	void OnAccountInfoNotYetFromLoginSvr( ID idAccount );
	void SendLoginFailedGameSvr( DWORD idMsg );
	void RecvDisconnectMe( XPacket& ar );
	void SendInvalidVerify();
};



