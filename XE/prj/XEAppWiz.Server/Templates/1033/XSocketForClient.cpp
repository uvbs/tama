#include "stdafx.h"
#include "XSocketForClient.h"
#include "XClientConnection.h"
#include "Network/XPacket.h"
#include "XGameUser.h"
#include "XMain.h"
#include "[!output PROJECT_NAME]View.h"
#include "XGame.h"
#include "XGameUserMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


XSocketForClient *SOCKET_CLIENT = NULL;

XSocketForClient::XSocketForClient( unsigned short port ) 
	: XEWinSocketSvr( _T("[!output PROJECT_NAME]"), port, MAX_READY_CONNECT, MAX_CONNECT, TRUE ) 
{
	SOCKET_CLIENT = this;
	
	Init();

//	DoLoadData( );
}

// virtual, 커넥션 객체를 생성함. 
XEWinConnectionInServer* XSocketForClient::tCreateConnectionObj( SOCKET socket, LPCTSTR szIP )
{
	// 커넥션 객체 생성
	XClientConnection *pConnect = new XClientConnection( socket, szIP );
	//
	return pConnect;
}

/**
 부모클래스의 Create()가 호출되는중에 발생하는 이벤트. 
 여기서만 따로 생성해야 하는게 있다면 이곳에 코딩한다.
*/
void XSocketForClient::OnCreate( void )
{
}


XEUserMng* XSocketForClient::CreateUserMng( int maxConnect )
{
	m_pUserMng = new XGameUserMng( maxConnect );
	return m_pUserMng;
}

// 클라이언트로부터 접속이 성공하면 유저객체를 만들어 유저관리자에 등록한다.
void XSocketForClient::OnConnectFromClient( XEWinConnectionInServer *pConnect )
{
	XEWinSocketSvr::OnConnectFromClient( pConnect );
//#ifndef _BOT
//#if !defined(_BOT) && !defined(_XBOT2)
#ifdef _DEV
	if( GetNumConnection() < 10 )
	{
		TCHAR szIP[ 64 ];
		TCHAR szTime[ 1024 ];
		_tcscpy_s( szIP, pConnect->GetszIP() );
		XE::GetTimeString( szTime, 1024 );		
	//	CONSOLE( "connected: %s, %s", szIP, szTime );
		CONSOLE( "connected: %s", szIP );
	}
#endif
//#endif
	
}

XEUser* XSocketForClient::CreateUser( XEWinConnectionInServer *pConnect )
{
	XClientConnection *pCConnect = SafeCast<XClientConnection*, XEWinConnectionInServer*>( pConnect );
	return new XGameUser( m_pUserMng, pCConnect );
}

void XSocketForClient::ProcessTimer()
{	
}