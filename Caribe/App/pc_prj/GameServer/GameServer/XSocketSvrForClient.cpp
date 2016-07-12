#include "stdafx.h"
#include "XSocketSvrForClient.h"
#include "XClientConnection.h"
#include "Network/XPacket.h"
#include "XGameUser.h"
#include "XMain.h"
#include "GameServerView.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XLoginConnection.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


XSocketSvrForClient *SOCKET_CLIENT = NULL;

XSocketSvrForClient::XSocketSvrForClient() 
	: XEWinSocketSvr( _T("GameServer")
									, MAX_READY_CONNECT
									, XEnv::sGet()->GetmaxConnect()
									, TRUE )	// encrypt
{
	SOCKET_CLIENT = this;
	
	Init();
//	DoLoadData( );
}

// virtual, 커넥션 객체를 생성함. 
XSPWinConnInServer XSocketSvrForClient::tCreateConnectionObj( SOCKET socket, LPCTSTR szIP )
{
	// 커넥션 객체 생성
// 	return std::make_shared<XClientConnection>( socket, szIP );
	return std::shared_ptr<XClientConnection>( new XClientConnection(socket, szIP) );
// 	auto pConnect = new XClientConnection( socket, szIP );
// 	//
// 	return pConnect;
}

/**
 부모클래스의 Create()가 호출되는중에 발생하는 이벤트. 
 여기서만 따로 생성해야 하는게 있다면 이곳에 코딩한다.
*/
void XSocketSvrForClient::OnCreate( void )
{
}


XEUserMng* XSocketSvrForClient::CreateUserMng( int maxConnect )
{
	m_pUserMng = new XGameUserMng( maxConnect );
	return m_pUserMng;
}

// 클라이언트로부터 접속이 성공하면 유저객체를 만들어 유저관리자에 등록한다.
void XSocketSvrForClient::OnLoginedFromClient( XSPWinConnInServer spConnect )
{
	XEWinSocketSvr::OnLoginedFromClient( spConnect );
	if( m_timerSec.IsOff() ) {
		m_timerSec.Set( 1.f );
	}
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	if( GetNumConnection() < 10 ) {
		TCHAR szIP[ 64 ];
		TCHAR szTime[ 1024 ];
		_tcscpy_s( szIP, spConnect->GetszIP() );
		XE::GetTimeString( szTime, 1024 );		
	//	CONSOLE( "connected: %s, %s", szIP, szTime );
		CONSOLE_TAG( "connect", "connected: %s", szIP );
	}
#endif
	
}

XSPUserBase XSocketSvrForClient::CreateUser( XSPWinConnInServer spConnect )
{
//	auto pCConnect = SafeCast<XClientConnection*>( spConnect.get() );
// 	return std::make_shared<XGameUser>( m_pUserMng, spConnect );
	return std::shared_ptr<XGameUser>( new XGameUser( m_pUserMng, spConnect ) );
// 	return std::make_shared<XGameUser>() new XGameUser( m_pUserMng, pCConnect );
}

void XSocketSvrForClient::OnProcess( float dt )
{	
	if( m_timerSec.IsOver() ) {
		m_timerSec.Reset();
		LOGIN_SVR->SendGameSvrInfo( GetnumConnected() + GetnumLogined() );
	}
}

