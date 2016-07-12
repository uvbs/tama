#include "stdafx.h"
#if defined(_CLIENT) || defined(_CONSOLE)
#include "XWinSocketClient.h"
#include <process.h>
#include "etc/Debug.h"
#include "Network/XNetworkDelegate.h"
#include "Network/xenDef.h"
#include "XLock.h"
//#include <WS2tcpip.h>


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

static unsigned int __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	auto This = static_cast<XEWinSocketClient*>( param );
	//
	This->WorkThread();
	return 0;
}

static unsigned int __stdcall _ConnectThread( void *param ) 
{
    XBREAK( param == 0 );
	XEWinSocketClient *This = static_cast<XEWinSocketClient*>( param );
	//
	This->ConnectThread();
	return 1;
}

//XEWinSocketClient::XEWinSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param )
XEWinSocketClient::XEWinSocketClient( XNetworkDelegate *pDelegate, int sizeQueue, DWORD param )
	: XESocketClient( pDelegate, sizeQueue, param )
{
	Init();
	//
	if( XENW::x_WSAData.wVersion == 0 )	{
		if( WSAStartup( MAKEWORD(2,2), &XENW::x_WSAData ) != 0 ) {
			XALERT( "WSAStartup failed" );
			return;
		}
	}
// 	strcpy_s( m_cIP, cIP );
// 	//
// 	CreateSocket();
	//
}

BOOL XEWinSocketClient::CreateSocket( void )
{
	ClearConnection();
	//
	m_Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED ); 
//	m_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
	if( m_Socket == INVALID_SOCKET ) 
	{
		if( GetpDelegate() )
			GetpDelegate()->DelegateNetworkError( this, XE::xN_FAILED_CREATE_SOCKET );
		m_Socket = 0;
		return FALSE; 
	}
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(0); 
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	if( ::bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) { 
		if( GetpDelegate() )
			GetpDelegate()->DelegateNetworkError( this, XE::xN_FAILED_BIND_SOCKET );
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
		// 워크스레드가 하나뿐이라 XSocketClient::m_Buffer[]로 recv할때 특별히 락을 걸지 않은 모양.
		// 만약 멀티스레드라면 recv받을때 락 걸어야 함.
		int sizeBuff = 0;
		BYTE* pBuffer = GetRecvBuff( &sizeBuff );
		XBREAK( pBuffer == NULL || sizeBuff == 0 );
		int readbytes = recv( m_Socket, (char*)pBuffer, sizeBuff, 0 );		// 버퍼크기만큼 패킷을 읽어온다
		if( readbytes <= 0 )	{
			Lock(__TFUNC__);
			if( m_hWorkThread == 0 )
				return;
			XTRACE("disconnect by external");
			// 접속이 끊어짐
			XBREAK( GetbDoDisconnect() == true );		// DoDisconnect()로 종료시킬땐 여기에 안들어와야 정상임.
			if( !GetbDoDisconnect() ) {
				XTRACE("cbOnDisconnectByExternal()");
				cbOnDisconnectByExternal();		// virtual callback
			}
			CloseSocket();
			XESocketClient::ClearConnection();
//			DoDisconnect();		// 즉시 소켓 제거하여 디스커넥상태로 인식하도록 만듬.
			Unlock();
			return;
		}
		XBREAK( readbytes > sizeBuff );
		// 버퍼의 내용을 큐로 보냄
		Lock(__TFUNC__);
		XENetworkConnection::PushBlock( pBuffer, readbytes );		// 받은 데이터를 큐에 보냄
		Unlock();
	}
	
}


void XEWinSocketClient::ConnectThread( void )
{
	XBREAK( GetPort() == 0 );
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(GetPort()); 

	if(m_cURL.empty()) {
// 		DWORD ipv4_addr;
// 		addr.sin_addr.S_un.S_addr = inet_pton( AF_INET, m_cIP, (void*)&ipv4_addr );
		XBREAK( GetstrcIP().empty() );
 		addr.sin_addr.S_un.S_addr = inet_addr( GetstrcIP().c_str() ); 
	} else {
// 		struct addrinfo hints;
// 		struct addrinfo *result;
// 		if( getaddrinfo( nullptr, m_cURL.c_str(), &hints, &result ) != 0 ) {
// 			addr.sin_addr.s_addr = *(long int *)*ent->h_addr_list;
// 			printf( "%s\n", inet_ntoa( addr.sin_addr ) );
// 		}
		hostent* ent = gethostbyname(m_cURL.c_str());
		if(ent == NULL)
			return;		// 에러
		addr.sin_addr.s_addr = *(long int *)*ent->h_addr_list;
		printf("%s\n", inet_ntoa(addr.sin_addr));
	}
	while(1)	{
		if( connect( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == 0 ) 	{
			CreateWorkThread();
//			Set_bConnected( TRUE );		// 연결상태로 만든다.
			// 봇에서는 이 스레드를 쓰지 않기 때문에 이 플래그를 세팅하려면 따로 해야한다. 일관성을 유지하기 위해 XSocketClient::ProcessReservedPacket에서 일괄적으로 하게 바꿈.
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
	XBREAK( m_hConnectThread != 0 );
	// _beginthreadex로 바꿀것.
	m_hConnectThread = (HANDLE)_beginthreadex( NULL, 0, _ConnectThread, (LPVOID)this, 0, (unsigned*)&idThread );
	return XESocketClient::TryConnect();
}

// 소켓에 연결이 성공했다
void XEWinSocketClient::CreateWorkThread( void )
{
	XLOCK_OBJ;
	DWORD idThread;
	// recv 스레드 생성
	XBREAK( m_hWorkThread != 0 );
	// 워크스레드 여러개 만들거라면 m_Buffer에 대한 락처리 해줄것.
	// _beginthreadex로 바꿀것.
	m_hWorkThread = (HANDLE)_beginthreadex( NULL, 0, _WorkThread, (LPVOID)this, 0, (unsigned*)&idThread );
}

// 서버와의 연결을 끊는다.
void XEWinSocketClient::DoDisconnect()
{
	XLOCK_OBJ;
	XTRACE("DoDisconnect:%s", C2SZ(GetstrcIP()) );
	// 스레드 종료(cbOndisconnectedByExternal이 발생하지 않아야함).
	SetbDoDisconnect( true );	// 검사용
	// 스레드를 모두 닫고 종료될때까지 대기
	CloseAllThread();
	// 소켓 닫음.
	CloseSocket();
	// 관련 변수 클리어
	XESocketClient::DoDisconnect();
}

void XEWinSocketClient::CloseConnectThread()
{
	if( m_hConnectThread ) {
		XTRACE("::TerminateThread: tConnect");
		::TerminateThread( m_hConnectThread, 1 );
		XTRACE("::WaitForSingleObject: tConnect");
		::WaitForSingleObject( m_hConnectThread, INFINITE );
		XTRACE("::CloseHandle: tConnect");
		::CloseHandle( m_hConnectThread );
	}
	m_hConnectThread = 0;
	XTRACE( "finish CloseConnectThread" );
}

void XEWinSocketClient::CloseWorkThread()
{
	if( m_hWorkThread ) {
		XTRACE("::TerminateThread: tWork");
		::TerminateThread( m_hWorkThread, 1 );
		XTRACE("::WaitForSingleObject: tWork");
		::WaitForSingleObject( m_hWorkThread, INFINITE );
		XTRACE("::CloseHandle: tWork");
		::CloseHandle( m_hWorkThread );
	}
	m_hWorkThread = 0;
	XTRACE( "finish CloseWorkThread" );
}

void XEWinSocketClient::CloseAllThread() 
{
	CloseConnectThread();
	CloseWorkThread();
}
/**
 @brief 단지 소켓만 닫는다. 패킷큐는 클리어하지 않는다.
*/
void XEWinSocketClient::CloseSocket()
{
	XLOCK_OBJ;
	if( m_Socket )
		closesocket( m_Socket );   
	m_Socket = 0;
//	XESocketClient::ClearConnection();
}

BOOL XEWinSocketClient::IsDisconnected()
{
	Lock();
	BOOL bRet = (m_Socket == 0 ) ? TRUE : FALSE;
	Unlock();
	return bRet;
}

void XEWinSocketClient::SendData( const BYTE *pBuffer, int len )
{
	XLOCK_OBJ;
	if( GetbDummy() )
		return;
	if( m_Socket ) {
		auto bConnected = Get_bConnected();
		XBREAK( bConnected == FALSE );	// 아직 connect()는 안된상태
		int sizeSend = send( m_Socket, (const char *)pBuffer, len, 0 );		// 커넥션 상대에게 데이타를 보냄
		if( XBREAK( sizeSend == SOCKET_ERROR ) )
		{
			int numError = WSAGetLastError();
			CONSOLE( "WSAGetLastError() == 0x%x", numError );
			DoDisconnect();
		}
		// 전부다 못보낸 경우도 처리해야 할듯.
#pragma message( "check this" )
		XBREAK( sizeSend != len );
	}
}

// http Post 관련

void XEWinSocketClient::SetHttpData( const char* baseURL )
{
	m_cURL = baseURL;
}

void XEWinSocketClient::ProcessHttp( const char* host, const char* page, const char* postStr, char *response )
{
/*
	char sendline[4000+1], recvline[4000+1];
	size_t n;

	sprintf_s(sendline, 4000,
		"POST %s HTTP/1.0\r\n"
		"Host: %s\r\n"
		"Content-type: application/x-www-form-urlencoded\r\n"
		"Content-length: %d\r\n\r\n"
		"%s", page / * "/lyto/api/dev/0.1/login.php" * /, host / * "vps.ritchienathaniel.com" * / , (int)strlen(postStr), postStr / * "device_id=355666050014363&app_id=gaME_DeV&app_secret=vMAj4MkVnHVWoTSOs7YRnRcCj4PlFU" * /);

	int iResult = send(m_Socket, sendline, strlen(sendline), 0);

	if ( iResult == SOCKET_ERROR) {
		return;
	}

	int offset = 0;
	while ((n = recv(m_Socket, recvline, 4000, 0)) > 0) {
		recvline[n] = '\0';
		//printf("%s", recvline);
		strcpy( response+offset, recvline );
		offset += n;

		if (offset < 0) break;
	}
*/
}

#endif // client
