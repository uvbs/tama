#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "server/XWinConnection.h"
#include "XPool.h"
//#include "XDBMng.h"


enum xtPacket;
class XAccount;
class XSAccount;
class XGameUser;

const float reconnect_time = 1.f;
// 월드서버에서 클라이언트들과 연결되는 커넥션 객체. XUser들이 하나씩 보유한다.
class XClientConnection : public XEWinConnectionInServer
{	
	void Init() {
	}
	void Destroy() {
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
	//PVP	

	virtual BOOL ProcessPacket( DWORD idPacket, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );

};



