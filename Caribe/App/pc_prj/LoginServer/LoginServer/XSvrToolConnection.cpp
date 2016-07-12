#include "StdAfx.h"
#if 0
#include "XGameSvrSocket.h"
#include "XSvrToolConnection.h"
//#include "XClientConnection.h"
#include "XSocketSvrForClient.h"
#include "XPacketLT.h"
#include "XMain.h"
#include "server/XSAccount.h"

using namespace std;

XSvrToolConnection::XSvrToolConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( _T("svrToolConn"), MAIN->GetpSocketForGameSvr(), socket, szIP, 1024 * 1024 * 2 )		
{
	Init();
	m_numConnectedClient = 0;
	m_GameSvrPort = 0;
	m_cIPExternal[0] = 0;
}

void XSvrToolConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
}

BOOL XSvrToolConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	switch( idPacket )
	{
	case xLS2ST_SERVER_INFO:		break;		// 게임 서버 정보등을 요청받음.
	case xLS2ST_SERVER_EVENT:		break;		// 진행 중인 이벤트등 게임 설정 내용 요청 받음
	case xLS2ST_SERVER_IAPINFO:		break;		// 결제
	
	default:		
		XBREAKF( 1, "unknown packet 0x%x", idPacket );
		{
			return FALSE;
		}
		break;
	}
	return TRUE;
}

void XSvrToolConnection::RecvGameSvrInfo( XPacket& p )
{
	int n = 0;
}

void XSvrToolConnection::RecvDidFinishLoad( XPacket& p )
{
	TCHAR szIPExternal[32];
	WORD port, w1;
	p >> port;	m_GameSvrPort = port;
	p >> w1;	SetGSvrID( w1 );
	p.ReadString( szIPExternal );
	strcpy_s( m_cIPExternal, SZ2C( szIPExternal ) );
	CONSOLE("ServerTool Sync Finished: %s:%d", szIPExternal, port );	
}
#endif // 0