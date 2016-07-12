#include "stdafx.h"
#include "XSocketForGameSvr.h"
#include "XGameSvrConnection.h"
#include "Network/XPacket.h"
#include "XGameUser.h"
#include "XMain.h"
#include "DBAgentServerView.h"
#include "XGame.h"
#include "XGameUserMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//XSocketForGameSvr *SOCKET_GAMESVR = NULL;

std::shared_ptr<XSocketForGameSvr> XSocketForGameSvr::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XSocketForGameSvr>& XSocketForGameSvr::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::make_shared<XSocketForGameSvr>();
	return s_spInstance;
}
void XSocketForGameSvr::sDestroyInstance() {
	s_spInstance.reset();
}

XSocketForGameSvr::XSocketForGameSvr() 
	: XEWinSocketSvr( _T("Socket for Gameserver")/*, port*/, MAX_GAMESVR_CONNECT, MAX_GAMESVR_CONNECT, FALSE ) 
{
	XBREAK( s_spInstance != nullptr );
// 	SOCKET_GAMESVR = this;
	
	Init();

//	DoLoadData( );
}

// virtual, 커넥션 객체를 생성함. 
XSPWinConnInServer XSocketForGameSvr::tCreateConnectionObj( SOCKET socket, LPCTSTR szIP )
{
	// 커넥션 객체 생성
// 	auto pConnect = new XGameSvrConnection( socket, szIP );
	auto spConnect = std::make_shared<XGameSvrConnection>( socket, szIP );
// 	// 개발중엔 브레이크 잡고 디버깅을 해야하므로 자동으로 안끊기게 함.
#if _DEV_LEVEL <= DLV_LOCAL
	spConnect->SetbAutoDisconnect( false );	
#endif
	//
	return spConnect;
}

/**
 부모클래스의 Create()가 호출되는중에 발생하는 이벤트. 
 여기서만 따로 생성해야 하는게 있다면 이곳에 코딩한다.
*/
void XSocketForGameSvr::OnCreate()
{
}


XEUserMng* XSocketForGameSvr::CreateUserMng( int maxConnect )
{
	m_pUserMng = new XGameUserMng( maxConnect );
	return m_pUserMng;
}

// 클라이언트로부터 접속이 성공하면 유저객체를 만들어 유저관리자에 등록한다.
void XSocketForGameSvr::OnLoginedFromClient( XSPWinConnInServer spConnect )
{
	XEWinSocketSvr::OnLoginedFromClient( spConnect );
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

XSPUserBase XSocketForGameSvr::CreateUser( XSPWinConnInServer spConnect )
{
// 	auto spGameUser = std::make_shared<XGameUser>( m_pUserMng, spConnect );
	auto spGameUser = XSPGameUser( new XGameUser( m_pUserMng, spConnect ) );
	return std::static_pointer_cast<XEUser>( spGameUser );
//	XGameSvrConnection *pCConnect = SafeCast<XGameSvrConnection*, XEWinConnectionInServer*>( pConnect );
// 	return new XGameUser( m_pUserMng, spConnect );
}

void XSocketForGameSvr::ProcessTimer()
{	
}


void XSocketForGameSvr::DrawConnections( _tstring* pOutStr )
{
	XAUTO_LOCK;
	{
// 		auto plist = &GetlistConnected().GetSharedObj();
// 		for( auto spConnect : *plist ) {
// 			(*pOutStr) += XE::Format( _T("    connected GameServer: sizeQ:%d\n")
// 																, spConnect->GetSizeQueue() );
// 			// 출력하고 지움.
// 			spConnect->ClearSizeMaxQ();
// 		}// END_LOOP;
// 		GetlistConnected().ReleaseSharedObj();
	}
	{
		auto plist = &GetlistLogined().GetSharedObj();
		for( auto spConnect : *plist ) {
			( *pOutStr ) += XE::Format( _T( "    GameServer: sizeQ:%d\n" )
																	, spConnect->GetSizeQueue() );
			// 출력하고 지움.
			spConnect->ClearSizeMaxQ();
		}// END_LOOP;
		GetlistLogined().ReleaseSharedObj();
	}

}