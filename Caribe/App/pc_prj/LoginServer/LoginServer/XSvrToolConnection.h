#pragma once
#if 0
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XWinConnection.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'xxxx' : 우위에 따라 'xxxxx'을(를) 상속합니다.

class XLAccount;
class XUser;


class XSvrToolConnection :	public XEWinConnectionInServer
{
	int m_numConnectedClient;		// 이 게임서버에 물려있는 접속자 수.
	WORD m_GameSvrPort;			// 이 커넥션에 연결되어있는 게임서버의 클라이언트 소켓 포트.
	WORD wReserved;
//	WORD m_GSvrID;				// 게임서버 아이디-XEWinConnectionInServer::m_idConnectUserDef 로 대체함.
	char m_cIPExternal[ 16 ];			// 클라이언트에 알려질 this게임서버의 외부 아이피`

	void Init() {	
		m_numConnectedClient = 0;
		m_GameSvrPort = 0;
		m_cIPExternal[0] = 0;
//		m_GSvrID = 0;
	}
	void Destroy() {	}

	GET_SET_ACCESSOR( int, numConnectedClient );
	SET_ACCESSOR( WORD, GameSvrPort );
public:
	XSvrToolConnection( SOCKET socket, LPCTSTR szIP );
	virtual ~XSvrToolConnection() { Destroy(); }
	//
	GET_ACCESSOR( const char*, cIPExternal );
	GET_ACCESSOR( WORD, GameSvrPort );
	WORD GetGSvrID( void ) {
		return (WORD)GetidConnectUserDef();
	}
	void SetGSvrID( WORD widGSvr ) {
		SetidConnectUserDef( (DWORD)widGSvr );
	}
	//	
	void RecvGameSvrInfo( XPacket& p );
	void RecvDidFinishLoad( XPacket& p );	
	//
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual void OnError( xtError error, DWORD param1, DWORD param2 );
	// Delegate
};

#pragma warning ( default : 4250 )

#endif // 0