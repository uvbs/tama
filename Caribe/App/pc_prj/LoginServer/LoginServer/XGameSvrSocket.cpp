#include "stdafx.h"
#include "XGameSvrSocket.h"
#include "XGameSvrConnection.h"
//#include "Network/XPacket.h"
//#include "XAhgoUser.h"
#include "XMain.h"
//#include "WorldServerView.h"
//#include "XAhgo.h"
//#include "XGUserMng.h"
//#include "XGUser.h"
//#include "XGameUserMng.h"
#include "XGameSvrMng.h"
#include "Network\XLoginGameSvrPacket.h"


//XGameSvrSocket *AHGO_SERVER = NULL;
const int MAX_VALID_CONNECT = 10; //게임 서버 일단 10개 정도 붙는다고 가정 하고 생성.
XSocketForGameSvr::XSocketForGameSvr() 
	: XEWinSocketSvr( _T("Socket for GameServer")
									, MAIN->GetmaxGSvrConnect() 
									, MAIN->GetmaxGSvrConnect() 
									, FALSE ) 
{
//	m_spLock = std::make_shared<XLock>();
	CONSOLE( "create socket for Gameserver:" );
}

// virtual, 커넥션 객체를 생성함. 
XSPWinConnInServer XSocketForGameSvr::tCreateConnectionObj( SOCKET socket, LPCTSTR szIP )
{
	// 커넥션 객체 생성
	auto spConnect = std::make_shared<XGameSvrConnection>( socket, szIP );
	return spConnect;
// 	auto pConnect = new XGameSvrConnection( socket, szIP );
// #ifdef _DEV
// 	// 개발중엔 브레이크 잡고 디버깅을 해야하므로 자동으로 안끊기게 함.
// 	pConnect->SetbAutoDisconnect( false );	
// #endif
// 	//
// 	return pConnect;
}

// XEUserMng* XSocketForGameSvr::CreateUserMng( int maxConnect )
// {
//  	auto pSvrMng = new XGameSvrMng( maxConnect );
// 	XBREAK( pSvrMng == NULL );
// 	return (XEUserMng*)pSvrMng;
// }

/**
 @brief 게임서버가 연결이 되면 게임서버리스트에 넣는다.
*/
void XSocketForGameSvr::OnLoginedFromClient( XSPWinConnInServer spConnect )
{
	XAUTO_LOCK;
	XEWinSocketSvr::OnLoginedFromClient( spConnect );
	//
	auto spC = std::static_pointer_cast<XGameSvrConnection>( spConnect );
//	m_listGameSvr.Add( spC );
	TCHAR szIP[ 64 ];
	_tcscpy_s( szIP, spConnect->GetszIP() );
	CONSOLE( "game server connected: %s", szIP );
}

void XSocketForGameSvr::OnLoginedGameSvr( XSPGameSvrConnect spConnGameSvr
																				, WORD widSvr
																				, const std::string& strcIPExternal
																				, WORD portExternal )
{
	if( XASSERT(spConnGameSvr) ) {
		m_listGameSvr.Add( spConnGameSvr );
	}
}
// 게임서버의 접속이 끊어졌다.
void XSocketForGameSvr::OnDestroyConnection( XSPWinConnInServer spConnect )
{
	XAUTO_LOCK;
	XEWinSocketSvr::OnDestroyConnection( spConnect );
	auto spC = std::static_pointer_cast<XGameSvrConnection>( spConnect );
	if( XASSERT(spC) ) {
		XConsole(_T("게임서버(idSvr=%d)의 연결이 끊어짐."), spC->GetGSvrID() );
		m_listGameSvr.DelByID( spC->getid() );
	}
}
/**
 게임서버 커넥션을 얻는다. idGSvr은 게임서버 아이디
*/
XSPGameSvrConnect XSocketForGameSvr::GetGameSvrConnection( WORD widGSvr )
{
	XAUTO_LOCK;
	if( m_listGameSvr.size() == 0 )
		return nullptr;
	for( auto spConnect : m_listGameSvr ) {
		if( spConnect->GetGSvrID() == widGSvr )
			return spConnect;
	}
	return nullptr;
}

bool XSocketForGameSvr::IsExist( WORD widGSvr ) const
{
	if( m_listGameSvr.size() == 0 )
		return false;
	for( auto spConnect : m_listGameSvr ) {
		if( spConnect->GetGSvrID() == widGSvr )
			return true;
	}
	return false;
}

/**
 가장 한가한 게임서버의 커넥션을 얻는다.
*/
XSPGameSvrConnect XSocketForGameSvr::GetFreeConnection()
{
	XAUTO_LOCK;
	if( m_listGameSvr.size() == 0 )
		return nullptr;
	int minClients = 0x7fffffff;
//	XSPGameSvrConnect spMinGameSvr;
	XVector<XSPGameSvrConnect> aryMinConns;
	// 가장 동접자가 적은 게임서버를 찾는다.
	for( auto spConnGameSvr : m_listGameSvr ) {
		const int numClients = spConnGameSvr->GetnumConnectedClient();
		if( numClients < minClients ) {
			minClients = numClients;
			aryMinConns.clear();
			aryMinConns.Add( spConnGameSvr );
		} else 
		if( numClients == minClients ) {
			aryMinConns.Add( spConnGameSvr );
		}
	}
	if( aryMinConns.size() > 0 ) {
		return aryMinConns.GetFromRandom();
	}
	XBREAK(1);
//	return m_listGameSvr.GetByIndexNonPtr(0);	// 일단은 가장 첫번째 걸로 리턴
	return nullptr;
}

//void XGameSvrSocket::Create()
//{
	//m_pSvrMng  = CreateUserMng( m_maxConnect );
	//XBREAK( m_pUserMng == NULL );
	//// 유저 커스텀 create
	//OnCreate(); // virtual
	//// IOCP 객체 생성
	//m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ); 
	//// 워커 스레드 생성
	//for( int i = 0; i <MAX_THREAD; ++i ) 
	//	m_hThread[i] = CreateWorkThread();
	//// accept( 클라이언트로부터의 접속대기 ) 스레드 생성
	//m_hAcceptThread = CreateAcceptThread();
//}

void XSocketForGameSvr::DrawConnections( _tstring* pOutStr )
{
	XAUTO_LOCK;
// 	XLIST_LOOP( m_listGameSvr, XGameSvrConnection*, pConnect )
	for( auto& spConnect : m_listGameSvr ) {
		const _tstring strIP = C2SZ( spConnect->GetstrcIPExternal().c_str() );
		(*pOutStr) += XE::Format( _T("    ID:%d GameServer(user:%d), sizeQ:%d, externalIP:%s:%d\n")
															, spConnect->GetGSvrID()
															, spConnect->GetnumConnectedClient()
															, spConnect->GetSizeQueue()
															, strIP.c_str()
															, (int)spConnect->GetGameSvrPort());
		// draw후 지움.
		spConnect->ClearSizeMaxQ();
	}// END_LOOP;

}