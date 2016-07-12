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
const int MAX_VALID_CONNECT = 10; //���� ���� �ϴ� 10�� ���� �ٴ´ٰ� ���� �ϰ� ����.
XSocketForGameSvr::XSocketForGameSvr() 
	: XEWinSocketSvr( _T("Socket for GameServer")
									, MAIN->GetmaxGSvrConnect() 
									, MAIN->GetmaxGSvrConnect() 
									, FALSE ) 
{
//	m_spLock = std::make_shared<XLock>();
	CONSOLE( "create socket for Gameserver:" );
}

// virtual, Ŀ�ؼ� ��ü�� ������. 
XSPWinConnInServer XSocketForGameSvr::tCreateConnectionObj( SOCKET socket, LPCTSTR szIP )
{
	// Ŀ�ؼ� ��ü ����
	auto spConnect = std::make_shared<XGameSvrConnection>( socket, szIP );
	return spConnect;
// 	auto pConnect = new XGameSvrConnection( socket, szIP );
// #ifdef _DEV
// 	// �����߿� �극��ũ ��� ������� �ؾ��ϹǷ� �ڵ����� �Ȳ���� ��.
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
 @brief ���Ӽ����� ������ �Ǹ� ���Ӽ�������Ʈ�� �ִ´�.
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
// ���Ӽ����� ������ ��������.
void XSocketForGameSvr::OnDestroyConnection( XSPWinConnInServer spConnect )
{
	XAUTO_LOCK;
	XEWinSocketSvr::OnDestroyConnection( spConnect );
	auto spC = std::static_pointer_cast<XGameSvrConnection>( spConnect );
	if( XASSERT(spC) ) {
		XConsole(_T("���Ӽ���(idSvr=%d)�� ������ ������."), spC->GetGSvrID() );
		m_listGameSvr.DelByID( spC->getid() );
	}
}
/**
 ���Ӽ��� Ŀ�ؼ��� ��´�. idGSvr�� ���Ӽ��� ���̵�
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
 ���� �Ѱ��� ���Ӽ����� Ŀ�ؼ��� ��´�.
*/
XSPGameSvrConnect XSocketForGameSvr::GetFreeConnection()
{
	XAUTO_LOCK;
	if( m_listGameSvr.size() == 0 )
		return nullptr;
	int minClients = 0x7fffffff;
//	XSPGameSvrConnect spMinGameSvr;
	XVector<XSPGameSvrConnect> aryMinConns;
	// ���� �����ڰ� ���� ���Ӽ����� ã�´�.
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
//	return m_listGameSvr.GetByIndexNonPtr(0);	// �ϴ��� ���� ù��° �ɷ� ����
	return nullptr;
}

//void XGameSvrSocket::Create()
//{
	//m_pSvrMng  = CreateUserMng( m_maxConnect );
	//XBREAK( m_pUserMng == NULL );
	//// ���� Ŀ���� create
	//OnCreate(); // virtual
	//// IOCP ��ü ����
	//m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ); 
	//// ��Ŀ ������ ����
	//for( int i = 0; i <MAX_THREAD; ++i ) 
	//	m_hThread[i] = CreateWorkThread();
	//// accept( Ŭ���̾�Ʈ�κ����� ���Ӵ�� ) ������ ����
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
		// draw�� ����.
		spConnect->ClearSizeMaxQ();
	}// END_LOOP;

}