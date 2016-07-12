#include "stdafx.h"
#include "XWinSocketBot.h"
#include <WS2tcpip.h>

BOOL XWinSocketBot::TryConnect()
{
// 	if( Get_bConnected() )
// 		return TRUE;
//	XBREAK( IOCP_MNG == NULL );
	int size;
	xConn conn;
	conn.m_dwKey = (DWORD)this;
	conn.m_pBuffer = (char*)GetRecvBuff( &size );
	conn.m_pOp = &m_op;
	conn.m_port = GetPort();
	conn.m_Socket = GetSocket();
	conn.m_strIP = GetstrcIP();
	XIOCPMng::sGet()->PushConnectQ( conn );
// 	XBREAK( GetstrcIP().empty() );
// 	XBREAK( GetPort() == 0 );
// 	SOCKADDR_IN addr; 
// 	memset( &addr, 0, sizeof(addr) );
// 	addr.sin_family = AF_INET; 
// 	addr.sin_port = htons(GetPort()); 
// //	addr.sin_addr.S_un.S_addr = inet_addr(GetstrcIP().c_str()); 
// 	inet_pton( AF_INET, GetstrcIP().c_str(), &addr.sin_addr );
// 	SOCKET socket = GetSocket();
// 	while( 1 ) {
// 		const auto result = WSAConnect( socket, ( struct sockaddr * )&addr, sizeof( addr ), nullptr, nullptr, nullptr, nullptr );
// 		if( result != SOCKET_ERROR ) {
// 			XIOCPMng::sGet()->RegisterSocket( this );
// 			// 서버와 연결이 되면(complete패킷) DelegateConnect()가 발생한다.
// 			return TRUE;
// 		} else {
// 			//			XBREAK( 1 );
// 		}
// 		Sleep( 1000 );
// 	}
// 	while(1) {
// 		if( connect( socket, ( struct sockaddr * )&addr, sizeof( addr ) ) == 0 ) {
// 			XIOCPMng::sGet()->RegisterSocket( this );
// 			// 서버와 연결이 되면(complete패킷) DelegateConnect()가 발생한다.
// 			return TRUE;
// 		}
// 		else {
// //			XBREAK( 1 );
// 		}
// 		Sleep(1000);
// 	}

	return FALSE;
}

/**
 비동기로 recv를 시킴
 다수의 워커스레드로부터 호출됨.
*/
void XWinSocketBot::tWSARecv()
{
	XLOCK_OBJ;
	//
	SOCKET socket = GetSocket();
	do  {
		if( IsDisconnected() == TRUE )
			break;;		// 
		unsigned long readbytes; 
		// 비동기로 전송처리, 큐에 올라감. 이걸해야 스레드에서 데이타가 받아짐.
	#ifdef _USING_WSA 
		WSABUF b[1]; 
		unsigned long flags = 0; 
		int size = 0;
		b[0].buf = (CHAR *)GetRecvBuff( &size ); 
		b[0].len = size; 
		if( WSARecv( socket, b, 1, &readbytes, &flags, &m_op, NULL ) == SOCKET_ERROR )
	#else
		// 최초 accept시에는 아래처림 == FALSE로 검사하면 안됨. 원래 0나옴
		if( ReadFile( (HANDLE)socket, m_Buffer, sizeof(m_Buffer), &readbytes, &m_op ) == FALSE )
	#endif
		{
			if( GetLastError() != ERROR_IO_PENDING ) {
				TCHAR szBuff[ 1024 ];
				_stprintf_s( szBuff, _T( "WSARecv error: socket=%x, error=%d" ), socket, GetLastError() );
				TRACE( "GetLastError() != ERROR_IO_PENDING" );
			}
		}
	} while(0);
	//
	return;
}

//////////////////////////////////////////////////////////////////////////
std::shared_ptr<XIOCPMng> XIOCPMng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XIOCPMng>& XIOCPMng::sGet() {	if( s_spInstance == nullptr ) {		s_spInstance = std::shared_ptr<XIOCPMng>( new XIOCPMng );		s_spInstance->Create();	}	return s_spInstance; }
void XIOCPMng::sDestroyInstance() {
	s_spInstance.reset();
}
//
static unsigned long __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	XIOCPMng *This = static_cast<XIOCPMng*>( param );
	//
	This->WorkThread();
	return 1;
}

static unsigned long __stdcall _ConnectThread( void *param )
{
	XBREAK( param == 0 );
	XIOCPMng *This = static_cast<XIOCPMng*>( param );
	//
	This->ConnectThread();
	return 1;
}

////////////////////////////////////////////////////////////////
void XIOCPMng::Destroy() 
{
	for( int i = 0; i < MAX_THREAD_WORK; ++i )	{
		TerminateThread( m_hThread[i], 0 );
		CloseHandle( m_hThread[i] );
	}
	CloseHandle( m_hIOCP );
}

BOOL XIOCPMng::Create()
{
	// IOCP 객체 생성
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ); 
	// 접속스레드 생성
	for( int i = 0; i < MAX_THREAD_WORK; ++i ) {
		DWORD idThread;
		m_hThreadConn[i] = CreateThread( NULL, 0, _ConnectThread, ( LPVOID )this, 0, &idThread );
	}
	// 워커 스레드 생성
	for( int i = 0; i <MAX_THREAD_WORK; ++i ) 
		m_hThread[i] = CreateWorkThread();
	return TRUE;
}

HANDLE XIOCPMng::CreateWorkThread()
{
	DWORD idThread; 
	return CreateThread( NULL, 0, _WorkThread, (LPVOID)this, 0, &idThread ); 
}

void XIOCPMng::ConnectThread()
{
	SOCKADDR_IN addr;
	while(1) {
		std::queue<xConn>* pQueue = nullptr;
		while(1) {
			pQueue = &m_shoQConn.GetSharedObj();
			if( pQueue->size() > 0 )
				break;
			m_shoQConn.ReleaseSharedObj();
			Sleep(1);
		}
		auto conn = pQueue->front();
		pQueue->pop();
		m_shoQConn.ReleaseSharedObj();
		memset( &addr, 0, sizeof( addr ) );
		addr.sin_family = AF_INET;
		addr.sin_port = htons( conn.m_port );
		inet_pton( AF_INET, conn.m_strIP.c_str(), &addr.sin_addr );
		const auto result = WSAConnect( conn.m_Socket
																	, ( struct sockaddr * )&addr
																	, sizeof( addr )
																	, NULL, NULL, NULL, NULL );
//		const auto result = connect( conn.m_Socket, ( struct sockaddr * )&addr, sizeof( addr ) );
		if( result != SOCKET_ERROR ) {
			XIOCPMng::sGet()->RegisterSocket( conn );
		} else {
			// 접속에 실패하면 다시 뒤에 넣음.
			pQueue = &m_shoQConn.GetSharedObj();
			pQueue->push( conn );
			m_shoQConn.ReleaseSharedObj();
		}
	}
}

void XIOCPMng::WorkThread()
{
   unsigned long readbytes; 
   unsigned long dwCompKey; 
   OVERLAPPED * pOverlap; 
   while( 1 ) {
		// 
		GetQueuedCompletionStatus( m_hIOCP, &readbytes, &dwCompKey, (LPOVERLAPPED *)&pOverlap, INFINITE ); 
		// 어떤 소켓에 데이타가 도착함.
		XWinSocketBot *pSocketClient = (XWinSocketBot*) dwCompKey;
		XLOCK_OBJ2( pSocketClient );
		if( readbytes == 0 ) {		// 접속 끊김
			TRACE( "disconnect:");
			pSocketClient->DoDisconnect();
//			pSocketClient->Set_bConnected( FALSE );
//			pSocketClient->Unlock();
			continue;
		} else {
			// 받은 데이터를 큐에 밀어넣음
			pSocketClient->tRecvData( readbytes );
		}
		// 비동기 recv를 큐에 올림
		pSocketClient->tWSARecv();
		// WSARecv안했는데도 같은소켓에서 GetQueuedCompletionStatus 작동해서 리턴될수 있나?
   } // while
}                           

// 소켓을 IOCP에 등록한다.
BOOL XIOCPMng::RegisterSocket( XWinSocketBot *pSocketClient )
{
	// IOCP에 소켓을 등록
	pSocketClient->Lock();
	HANDLE hIOCP = CreateIoCompletionPort( (HANDLE)pSocketClient->GetSocket()
																				, m_hIOCP
																				, (DWORD)pSocketClient
																				, 0 ); 

	// 비동기 Recv를 선 실행
	pSocketClient->tWSARecv();
	pSocketClient->Unlock();
	return ( hIOCP == m_hIOCP );
}

BOOL XIOCPMng::RegisterSocket( const xConn& conn )
{
	XAUTO_LOCK2( &m_lockIOCP );
	// IOCP에 소켓을 등록
	HANDLE hIOCP = CreateIoCompletionPort( (HANDLE)conn.m_Socket
																				, m_hIOCP
																				, conn.m_dwKey
																				, 0 );

	// 비동기 Recv를 선 실행
	tWSARecv( conn );
	return ( hIOCP == m_hIOCP );
}


void XIOCPMng::tWSARecv( const xConn& conn )
{
	WSABUF b[1];
	DWORD readbytes;
	unsigned long flags = 0;
	int size = 0;
	b[0].buf = (CHAR *)conn.m_pBuffer;
	b[0].len = 1024;
	if( WSARecv( conn.m_Socket, b, 1, &readbytes, &flags, conn.m_pOp, NULL ) == SOCKET_ERROR ) {
		const auto err = GetLastError();
		if( err != ERROR_IO_PENDING ) {
			XBREAKF(1, "err=%d", (int)err );
		}
	}
}

void XIOCPMng::PushConnectQ( const xConn& conn )
{
	auto pQueue = &m_shoQConn.GetSharedObj();
	pQueue->push( conn );
	m_shoQConn.ReleaseSharedObj();
}
