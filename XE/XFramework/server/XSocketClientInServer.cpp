#include "stdafx.h"
// 서버에서만 쓰는 클라이언트용 소켓 객체
#include "XSocketClientInServer.h"
#include "etc/Debug.h"
#include "Network/XNetworkDelegate.h"
#include "Network/xenDef.h"
#include "XFramework/XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifndef _SERVER
#error("only use in server")
#endif // not server

//int g_sizeQBuffer = 0;
// namespace XENW {
// 	static WSADATA x_WSAData = {0,}; 
// };

static unsigned int __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	XESocketClientInServer *This = static_cast<XESocketClientInServer*>( param );
	//
	This->WorkThread();
//	This->CloseWorkThread();
	return 0;
}

static unsigned int __stdcall _ConnectThread( void *param ) 
{
    XBREAK( param == 0 );
	auto This = static_cast<XESocketClientInServer*>( param );
	//
	This->ConnectThread();
//	This->CloseConnectThread();
	return 1;
}

// XESocketClientInServer::XESocketClientInServer( XNetworkDelegate *pDelegate, int sizeQueue, DWORD param )
// 	: XENetworkConnection( pDelegate, sizeQueue )
// {
// 	Init();
// 	//
// // 	if( cIP == NULL )
// // 	{
// // 		m_bDummy = TRUE;	// 시뮬레이션 모드로
// // 		return;
// // 	}
// 	//
// // 	if( XWinNetwork::s_WSAData.wVersion == 0 ) {
// // 		if( WSAStartup( MAKEWORD(1,1), &XWinNetwork ) != 0 ) {
// // 			XALERT( "WSAStartup failed" );
// // 			return;
// // 		}
// // 	}
// // 	strcpy_s( m__cIP, cIP );
// // 	_tcscpy_s( m_szIP, C2SZ( cIP ) );
// // 	m_Port = port;
// 	m_dwConnectParam = param;
// }
// 클라이언트역할 커넥션같은 경우 싱글톤으로 할때가 많아서 생성자에 파라메터를 최소한으로 줄임.
XESocketClientInServer::XESocketClientInServer( int sizeQueue )
	: XENetworkConnection( nullptr, sizeQueue )		// delegate는 OnCreate()에서 지정하는걸로 바뀜
{
	Init();
	m_spLock = std::make_shared<XLock>();
	//
	// 	if( cIP == NULL )
	// 	{
	// 		m_bDummy = TRUE;	// 시뮬레이션 모드로
	// 		return;
	// 	}
	//
	// 	if( XWinNetwork::s_WSAData.wVersion == 0 ) {
	// 		if( WSAStartup( MAKEWORD(1,1), &XWinNetwork ) != 0 ) {
	// 			XALERT( "WSAStartup failed" );
	// 			return;
	// 		}
	// 	}
	// 	strcpy_s( m__cIP, cIP );
	// 	_tcscpy_s( m_szIP, C2SZ( cIP ) );
	// 	m_Port = port;
//	m_dwConnectParam = param;
}

void XESocketClientInServer::Destroy() 
{
	CloseAllThread();
	if( m_Socket ) {
		closesocket( m_Socket );
	}
	m_Socket = 0;
	Sleep( 100 );		// 소켓 닫히고 this가 파괴되기전에 Work스레드에서 빠져나올시간을 조금 줬음.
	//		DeleteCriticalSection( &m_cs );
}

/**
 @brief 객체가 생성된 후 가장먼저 불려짐.
*/
bool XESocketClientInServer::OnCreate( const std::string& strcIP
																		, WORD port
																		, XNetworkDelegate* pDelegate
																		, DWORD param/* = 0*/ )
{
	if( XBREAK(strcIP.empty() || port == 0) ) {
		return false;
	}
	m_strcIP = strcIP;
	m_strtIP = C2SZ(strcIP);
	m_Port = port;
	m_dwConnectParam = param;
	SetpDelegate( pDelegate );
	if( !XWinNetwork::sStartUp() ) {
		XALERT( "WSAStartup failed" );
		return false;
	}
	return CreateSocket() != FALSE;
}

/**
 소켓을 새로열고 커넥션 스레드를 새로 열어 재접속 한다. 
 재접속했을때 서버주소나 포트가 바꼈을수 있으므로 반드시 인자로 받는걸로 
 재 세팅해야한다.
*/
bool XESocketClientInServer::DoReconnectTry()
{
	// 소켓을 다시 생성함.
	if( CreateSocket() ) {
		// 커넥션 스레드 다시 열어서 스레드내에서 계속 접속시도.
		m_bReadyServer = FALSE;
		SetbReconnected( TRUE );
		TryConnect();
		m_timerReconnect.Off();
	} else {
		// 소켓생성에 실패했다면 x초후에 다시 시도.
		return false;
	}
	return true;
}


BOOL XESocketClientInServer::IsDisconnected()	const
{
	if( Get_bConnected() == FALSE )
		return TRUE;
	return ( m_Socket == 0 ) ? TRUE : FALSE;
}

void XESocketClientInServer::CloseAllThread() 
{
	CloseConnectThread();
	CloseWorkThread();
// 	SAFE_CLOSE_HANDLE( m_thConnect.m_hHandle );
// 	XDetectDeadLock::sGet()->DelThread( m_thConnect.m_idThread );
// 	SAFE_CLOSE_HANDLE( m_thWork.m_hHandle );
// 	XDetectDeadLock::sGet()->DelThread( m_thWork.m_idThread );
}

void XESocketClientInServer::CloseConnectThread()
{
	Lock(__TFUNC__);
	SAFE_CLOSE_HANDLE( m_thConnect.m_hHandle );
	Unlock();
	if( m_thConnect.m_idThread )
		XDetectDeadLock::sGet()->DelThread( m_thConnect.m_idThread );
	m_thConnect.m_idThread = 0;
}
void XESocketClientInServer::CloseWorkThread()
{
	Lock( __TFUNC__ );
	SAFE_CLOSE_HANDLE( m_thWork.m_hHandle );
	Unlock();
	if( m_thWork.m_idThread )
		XDetectDeadLock::sGet()->DelThread( m_thWork.m_idThread );
	m_thWork.m_idThread = 0;
}


BOOL XESocketClientInServer::CreateSocket()
{
	XAUTO_LOCK;
	m_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
	if( m_Socket == INVALID_SOCKET ) 
	{
		XALERT( "create socket failed" );
		return FALSE; 
	}
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(0); 
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) { 
		XBREAK( 1 );
		XALERT( "Network bind error" );
		Destroy();
		return FALSE; 
	} 
	return TRUE;
}

void XESocketClientInServer::WorkThread() 
{
//	int sizePacket = 0;
//	int totalRead = 0;
	while(1) {
		// 큐에 받을 버퍼가 더이상 없으면 다 퍼갈때까지 걍 대기
// 		if( XBREAK( XENetworkConnection::IsqBufferFull() == TRUE ) )
// 			continue;
		// 소켓에서 패킷을 읽어 버퍼에 저장
		int readbytes = recv( m_Socket, (char *)m_Buffer, sizeof(m_Buffer), 0 );		// 버퍼크기만큼 패킷을 읽어온다
		{
			XAUTO_LOCK;
			if( readbytes <= 0 ) {
				// 접속이 끊어짐
				Set_bConnected( FALSE );
				XTRACE("외부요인에 의해 연결 끊김.");
//				ClearConnection();		// 패킷 버퍼 마저다 처리하려고 큐는 클리어 안함.
				return;
			}
			XBREAK( readbytes > ( int )sizeof( m_Buffer ) );
			// 버퍼의 내용을 큐로 보냄
			XENetworkConnection::PushBlock( m_Buffer, readbytes );		// 받은 데이터를 큐에 보냄
#if _DEV_LEVEL <= DLV_OPEN_BETA
			const int sizeQ = GetSizeQueue();
			if( sizeQ > GetsizeMaxQ() )
				SetsizeMaxQ( sizeQ );
#endif
		}
	}
}

void XESocketClientInServer::ConnectThread()
{
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(m_Port); 
	addr.sin_addr.S_un.S_addr = inet_addr(m_strcIP.c_str()); 
	{
		// 루프진입하기전에 대기(TryConnect()에서 스레드 생성하자마자 진입해서 한번 막음.
		 XAUTO_LOCK;
	}
	while(1)	{
		if( connect( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == 0 ) {
			XAUTO_LOCK;
			CreateWorkThread();
			Set_bConnected( TRUE );		// 연결상태로 만든다.
			return;			// 스레드 종료
		} 
		// 커넥트에 실패했으면
		if( m_Socket == 0 )
			return;
		Sleep( 1000 );		// 1초쉬고 다시 시도
	}
}

// 접속 시도
BOOL XESocketClientInServer::TryConnect()
{
	XAUTO_LOCK;
	XBREAK( m_Socket == NULL );
	// 커넥트 스레드 생성
	// 스레드를 추상화시킨걸로 쓸것.
// 	DWORD idThread;
	// _beginthreadex로 바꿀것.
// 	m_hConnectThread = (HANDLE)_beginthreadex( NULL, 0, _ConnectThread, (LPVOID)this, 0, (unsigned*)&idThread );
	m_thConnect.m_hHandle = (HANDLE)_beginthreadex( NULL
																								, 0
																								, _ConnectThread
																								, ( LPVOID )this
																								, 0
																								, (unsigned*)&m_thConnect.m_idThread );
	XDetectDeadLock::sGet()->AddThread( m_thConnect, _T("WinSockClient") );
// 	m_bRunConnectThread = true;
	return TRUE;
}

// 소켓에 연결이 성공했다
void XESocketClientInServer::CreateWorkThread()
{
// 	Lock(__TFUNC__);
//	DWORD idThread;
	// recv 스레드 생성
	// _beginthreadex로 바꿀것.
//	m_hWorkThread = (HANDLE)_beginthreadex( NULL, 0, _WorkThread, (LPVOID)this, 0, (unsigned*)&idThread );
	m_thWork.m_hHandle = (HANDLE)_beginthreadex( NULL
																						, 0
																						, _WorkThread
																						, ( LPVOID )this
																						, 0
																						, (unsigned*)&m_thWork.m_idThread );
	XDetectDeadLock::sGet()->AddThread( m_thWork, _T("WinSockClient") );
// 	// 커넥트 스레드는 삭제해줌
// 	SAFE_CLOSE_HANDLE( m_thConnect.m_hHandle );
// 	Unlock();
}

// 서버와의 연결을 끊는다.
void XESocketClientInServer::DoDisconnect()
{
	XAUTO_LOCK;
	// 스레드에서 끊긴게 아니라 인위적으로 끊었을때 스레드쪽에선 어떻게 반응하는지 확인해 볼것.
	if( m_Socket )
		closesocket( m_Socket );   
	m_Socket = 0;
	m_bReadyServer = FALSE;
	Set_bConnected( FALSE );
	// 아직 연결도 안된상태에서 명령이 오면 플래그만 세팅함
//	if( GetbConnected() == FALSE )
//		SetbDisconnected( TRUE );	
}

BOOL XESocketClientInServer::ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	auto packet = (XE::xtPacketReserved) idPacket;
	switch( packet ) {
	case XE::xXEPK_REQUEST_HB: {		// 서버로부터 살아있는지 응답요청 받음.
			XPacket ar((ID)XE::xXEPK_RESPONSE_HB);
			Send( ar );
	} break;
	case XE::xXEPK_CONNECT_COMPLETE: {
			// 서버로부터 암호화 키를 받음 0을 받으면 암호화 기능 끔
			BYTE b1;
			p >> b1;	BYTE keyCrypto = b1;
			SetkeyPrivate( keyCrypto );
			p >> b1 >> b1 >> b1;
			// 서버로부터 받은 암호화 모드에 따라 초기화를 함.
			if( keyCrypto ) {
				DeSerializeKeyTable( p );
			} else
				SetEncrypt( FALSE );
			m_bReadyServer = TRUE;
			// 접속핸들러 호출
			if( GetpDelegate() )
				GetpDelegate()->DelegateConnect( this, m_dwConnectParam );
			XTRACE("recv XEPK_CONNECT_COMPLETE");
			if( m_bReconnected ) {
				XBREAK( m_stateConnect != 2 );
			}
			m_stateConnect = 0;
			m_timerDelay.Off();
			OnLogined();		// virtual
		} break;
	default:
		// 유저 정의 패킷.
		return FALSE;
	}
	return TRUE;
}

// this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
BOOL XESocketClientInServer::Process()
{
	XAUTO_LOCK;
	// 연결 끊김을 감지
	ProcessConnect();
	// 패킷처리
	static XPacket p(true);		// heap영역을 쓰려고 이렇게 한듯?
	// 접속끊어졌어도 클라에선 남아있는 패킷 다 처리해야함. 
	if( PumpPacket( &p ) ){
		// 암호화된 아카이브면 해독한다.
		if( GetbEncryption() )
			p.DoDecrypto( this );
		DWORD idPacket;
		p >> idPacket;
		ID idKey;	// 패킷 고유 키값
		p >> idKey;
		if( ProcessReservedPacket( idPacket, idKey, p ) == FALSE ) {
			p._SetidPacket( idPacket );
			if( ProcessPacket( idPacket, idKey, p ) == FALSE ) {		// virtual call
				// 패킷 처리에 실패함. 접속 끊어야 함.
				return FALSE;
			}
		}
		p.Clear();
	}
	return TRUE;
}

void XESocketClientInServer::ProcessConnect()
{
	// online
	if( m_stateConnect == 0 ) {
		if( IsDisconnected() ) {
			OnDisconnect();  // virtual
			m_timerDelay.Set( 3.f );
			CloseAllThread();
			m_stateConnect = 1;
		}
	} else
	// delay & create socket & tryconnect
	if( m_stateConnect == 1 ) {
		XBREAK( m_timerDelay.IsOff() );
		if( m_timerDelay.IsOver() ) {
			// 연결이 끊기면 일정시간 기다렸다가 다시 소켓생성하고 접속시도.
			m_timerDelay.Off();
			if( DoReconnectTry() == false ) {
				// 소켓생성 실패하면 다시 기다림.
				m_timerDelay.Set( 3.f );
			} else {
				m_stateConnect = 2;
			}
		}
	} else 
	// logined()를 기다림
	if( m_stateConnect == 2 ) {
		// OnLogined()에서 해제됨.
	}
}

/**
 @brief 서버와 연결되어 CONNECT_COMPLETE까지 받음.
*/
// void XESocketClientInServer::OnLogined()
// {
// 	XBREAK( m_stateConnect != 2 );
// 	m_stateConnect = 0;
// 	m_timerDelay.Off();
// }


void XESocketClientInServer::SendData( const BYTE *pBuffer, int len )
{
	XAUTO_LOCK;
	if( IsDisconnected() )
		return;
	if( m_Socket ) {
		int sizeSend = send( m_Socket, (const char *)pBuffer, len, 0 );		// 커넥션 상대에게 데이타를 보냄
		if( sizeSend == SOCKET_ERROR ) {	// 끊어진 소켓일때 발생함.
#ifdef _DEV
			int numError = WSAGetLastError();
			CONSOLE( "WSAGetLastError() == 0x%x", numError );
#endif
			DoDisconnect();
		}
		// 전부다 못보낸 경우도 처리해야 할듯.
#pragma message( "check this" )
#ifdef _DEV
		XBREAK( sizeSend != len );
#endif
	}
}

// struct xSend {
// 	BYTE* m_pBuff;
// 	int m_size;
// 	xSend( CHAR* pBuff, int size ) : m_pBuff( pBuff ), m_size( size ) {}
// };

// Lock m_lock;
// XQueueArray<xSend> m_qBuffer;
// m_thread = CreateThread();
// 
// 
// SendThread()
// {
// 	while(1) {
// 		AUTOLOCK();
// 		while( m_qBuffer.size() == 0 );
// 		auto send = m_qBuffer.Front();
// 		m_qBuffer.pop();
// 		b.buf = send.m_pBuff;
// 		b.len = send.m_size;
// 		WSASend( m_Socket, b, 1 );
// 		++m_cntRef;
// 		Sleep(1);
// 	}
// }
// 
// WorkThread()
// {
// 	while(1) {
// 		GetQueuedCompletionStatus();
// 		if( type == SEND ) {
// 			--m_cntRef;
// 			continue;
// 		} else {
// 			// recv
// 		}
// 	}
// }
// 
// 
// 
// SendData( BYTE* pBuffer, int size )
// {
// 	AUTOLOCK();
// 	xSend send;
// 	send.m_pBuff = new BYTE[ size ];
// 	send.m_size = size;
// 	memcpy_s( send.m_pBuff, size, pBuffer, size );
// 	m_qBuffer.push_back( send );
// }