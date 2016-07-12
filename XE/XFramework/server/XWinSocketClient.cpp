#include "stdafx.h"
// 서버에서만 쓰는 클라이언트용 소켓 객체
#ifdef _SERVER
#include "XWinSocketClient.h"
#include "etc/Debug.h"
#include "Network/XNetworkDelegate.h"
#include "Network/xenDef.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//int g_sizeQBuffer = 0;
namespace XENW {
	static WSADATA x_WSAData = {0,}; 
};

static unsigned long __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	XEWinSocketClient *This = static_cast<XEWinSocketClient*>( param );
	//
	This->WorkThread();
	return 1;
}

static unsigned long __stdcall _ConnectThread( void *param ) 
{
    XBREAK( param == 0 );
	XEWinSocketClient *This = static_cast<XEWinSocketClient*>( param );
	//
	This->ConnectThread();
	return 1;
}

XEWinSocketClient::XEWinSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param )
	: XENetworkConnection( pDelegate, sizeQueue )
{
	Init();
	//
	if( cIP == NULL )
	{
		m_bDummy = TRUE;	// 시뮬레이션 모드로
		return;
	}
	//
	if( XENW::x_WSAData.wVersion == 0 )
	{
		if( WSAStartup( MAKEWORD(1,1), &XENW::x_WSAData ) != 0 ) 
		{
			XALERT( "WSAStartup failed" );
			return;
		}
	}
	strcpy_s( m__cIP, cIP );
	_tcscpy_s( m_szIP, C2SZ( cIP ) );
	m_Port = port;
	m_dwConnectParam = param;
	//
	CreateSocket();
	//
}

BOOL XEWinSocketClient::CreateSocket( void )
{
	//
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
	if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) 
	{ 
		XALERT( "Network bind error" );
		Destroy();
		return FALSE; 
	} 
	return TRUE;
}

/**
 소켓을 새로열고 커넥션 스레드를 새로 열어 재접속 한다. 
 재접속했을때 서버주소나 포트가 바꼈을수 있으므로 반드시 인자로 받는걸로 
 재 세팅해야한다.
*/
// BOOL XEWinSocketClient::DoReconnect( float secRetry, const char *cIP, unsigned short port )
// {
// 	if( XE::IsHave( cIP ) )
// 	{
// 		strcpy_s( m__cIP, cIP );
// 		_tcscpy_s( m_szIP, C2SZ( cIP ) );
// 		m_Port = port;
// 	}
// 	//
// 	if( m_timerReconnect.IsOff() ||	m_timerReconnect.IsOver() )
// 	{
// 		XBREAK( IsDisconnected() == FALSE );		// 안끊어졌는데 왔으면 경고.
// 		// 소켓을 다시 생성함.
// 		if( CreateSocket() )
// 		{
// 			// 커넥션 스레드 다시 열어서 스레드내에서 계속 접속시도.
// 			m_bReadyServer = FALSE;
// 			SetbReconnected( TRUE );
// 			TryConnect();
// 			m_timerReconnect.Off();
// 		} else
// 		{
// 			// 소켓생성에 실패했다면 x초후에 다시 시도.
// 			m_timerReconnect.Set( secRetry );
// 			return FALSE;
// 		}
// 	}
// 	return TRUE;
// }

void XEWinSocketClient::WorkThread( void ) 
{
//	int sizePacket = 0;
//	int totalRead = 0;
	while(1)
	{
		// 큐에 받을 버퍼가 더이상 없으면 다 퍼갈때까지 걍 대기
		if( XBREAK( XENetworkConnection::IsqBufferFull() == TRUE ) )
			continue;
		// 소켓에서 패킷을 읽어 버퍼에 저장
		int readbytes = recv( m_Socket, (char *)m_Buffer, sizeof(m_Buffer), 0 );		// 버퍼크기만큼 패킷을 읽어온다
		if( readbytes <= 0 )
		{
			if( m_hWorkThread == 0 )
				return;
			// 접속이 끊어짐
			DoDisconnect();		// 즉시 소켓 제거하여 디스커넥상태로 인식하도록 만듬.
			return;
		}
		XBREAK( readbytes > (int)sizeof(m_Buffer) );
		// 버퍼의 내용을 큐로 보냄
		XENetworkConnection::PushBlock( m_Buffer, readbytes );		// 받은 데이터를 큐에 보냄
	}
}

void XEWinSocketClient::ConnectThread( void )
{
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(m_Port); 
	addr.sin_addr.S_un.S_addr = inet_addr(m__cIP); 
	while(1)
	{
		if( connect( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == 0 ) 
		{
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
BOOL XEWinSocketClient::TryConnect( void )
{
	XBREAK( m_Socket == NULL );
	// 커넥트 스레드 생성
	// 스레드를 추상화시킨걸로 쓸것.
	DWORD idThread;
	// _beginthreadex로 바꿀것.
	m_hConnectThread = CreateThread( NULL, 0, _ConnectThread, (LPVOID)this, 0, &idThread );
	return TRUE;
}

// 소켓에 연결이 성공했다
void XEWinSocketClient::CreateWorkThread( void )
{
	Lock();
	DWORD idThread;
	// recv 스레드 생성
	// _beginthreadex로 바꿀것.
	m_hWorkThread = CreateThread( NULL, 0, _WorkThread, (LPVOID)this, 0, &idThread );
	// 커넥트 스레드는 삭제해줌
	SAFE_CLOSE_HANDLE( m_hConnectThread );
	Unlock();
}

// 서버와의 연결을 끊는다.
void XEWinSocketClient::DoDisconnect()
{
	XLOCK_OBJ;
	// 스레드에서 끊긴게 아니라 인위적으로 끊었을때 스레드쪽에선 어떻게 반응하는지 확인해 볼것.
	if( m_Socket )
		closesocket( m_Socket );   
	m_Socket = 0;
#ifdef _XRECONNECT
	m_bReadyServer = FALSE;
	Set_bConnected( FALSE );
#endif 
	// 아직 연결도 안된상태에서 명령이 오면 플래그만 세팅함
//	if( GetbConnected() == FALSE )
//		SetbDisconnected( TRUE );	
}

BOOL XEWinSocketClient::ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	auto packet = (XE::xtPacketReserved) idPacket;
	switch( packet ) {
	case XE::xXEPK_REQUEST_HB: {		// 서버로부터 살아있는지 응답요청 받음.
			XPacket ar((ID)XE::xXEPK_RESPONSE_HB);
			Send( ar );
	} break;
	case XE::xXEPK_CONNECT_COMPLETE: {
#ifdef _XCRYPT_PACKET
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
#endif
			m_bReadyServer = TRUE;
			// 접속핸들러 호출
			OnConnect();		// virtual
			if( GetpDelegate() )
				GetpDelegate()->DelegateConnect( this, m_dwConnectParam );
		} break;
	default:
		// 유저 정의 패킷.
		return FALSE;
	}
	return TRUE;
}

// this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
BOOL XEWinSocketClient::Process( void )
{
	static XPacket p(true);
	// 접속끊어졌어도 클라에선 남아있는 패킷 다 처리해야함. 
	if( PumpPacket( &p ) ){
#ifdef _XCRYPT_PACKET
		// 암호화된 아카이브면 해독한다.
//		if( p.GetbEncryption() )
		if( GetbEncryption() )
			p.DoDecrypto( this );
#endif
		DWORD idPacket;
		p >> idPacket;
#ifdef _XPACKET2
		ID idKey;	// 패킷 고유 키값
		p >> idKey;
#endif

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
void XEWinSocketClient::SendData( const BYTE *pBuffer, int len )
{
	Lock();
	if( m_bDummy )
		return;
	if( m_Socket )
	{
		int sizeSend = send( m_Socket, (const char *)pBuffer, len, 0 );		// 커넥션 상대에게 데이타를 보냄
		if( sizeSend == SOCKET_ERROR )	// 끊어진 소켓일때 발생함.
		{
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
	Unlock();
}


#endif // server
