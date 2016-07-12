#include "stdafx.h"
#ifndef WIN32
#include "XBSDSocketClient.h"
#include <pthread.h>
#include <stdio.h>
#include "etc/Debug.h"
//#include "XWindow.h"
#include "Network/XNetworkDelegate.h"
#include "Network/xenDef.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
//#define BTRACE(F, ...)					(0)

void *_WorkThread( void *data )
{
    XBREAK( data == NULL );
    XBSDSocketClient *This = static_cast<XBSDSocketClient*>(data);
    //
    This->WorkThread();
    return NULL;
}

void *_ConnectThread( void *data )
{
    XBREAK( data == NULL );
    XBSDSocketClient *This = static_cast<XBSDSocketClient*>(data);
    //
    This->ConnectThread();
    return NULL;
}


// XBSDSocketClient::XBSDSocketClient( const char *cIP, 
// 									unsigned short port, 
// 									XNetworkDelegate *pDelegate, 
// 									int sizeQueue,
// 									DWORD param )
// 	: XESocketClient( cIP, port, pDelegate, sizeQueue, param )
// {
// 	Init();
//    
// //	strcpy_s( m_cIP, sizeof(m_cIP), cIP );
// 	//
// //	CreateSocket();
// 	//
// }

XBSDSocketClient::XBSDSocketClient( XNetworkDelegate *pDelegate, 
																		int sizeQueue,
																		DWORD param )
	: XESocketClient( pDelegate, sizeQueue, param )
{
	Init();
}

BOOL XBSDSocketClient::CreateSocket( void )
{
	ClearConnection();
	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( m_Socket == -1 )	{
		if( GetpDelegate() )
			GetpDelegate()->DelegateNetworkError( this, XE::xN_FAILED_CREATE_SOCKET );
		m_Socket = 0;
		return FALSE;
	}
	return TRUE;
}

// BOOL XBSDSocketClient::DoReconnect( float secRetry, const char *cIP, unsigned short port )
// {
// 	if( XE::IsHave( cIP ) )
// 		strcpy_s( m_cIP, cIP );
// 	return XESocketClient::DoReconnect( secRetry, cIP, port );
// }

void XBSDSocketClient::WorkThread( void ) 
{
	while( 1 ) {
		// 큐에 받을 버퍼가 더이상 없으면 걍 리턴
		if( XENetworkConnection::IsqBufferFull() )
			continue;
		int sizeBuff = 0;
		BYTE* pBuffer = GetRecvBuff( &sizeBuff );
		XBREAK( pBuffer == NULL || sizeBuff == 0 );
		// 소켓에서 패킷을 읽어 버퍼에 저장
		int readbytes = recv( m_Socket, (char*)pBuffer, sizeBuff, 0 );		// 버퍼크기만큼 패킷을 읽어온다
		if( readbytes <= 0 )		{
			BTRACE( "disconnected by external." );
			XLOCK_OBJ;
			BTRACE( "disconnected lock" );
			if( !m_Socket ) {
				BTRACE( "exit connect thread" );
				return;
			}
			// 접속이 끊어짐
//			XBREAK( GetbDoDisconnect() == true );		// DoDisconnect()로 종료시킬땐 여기에 안들어와야 정상임.
			if( !GetbDoDisconnect() ) {
				BTRACE("cbOnDisconnectByExternal()");
				cbOnDisconnectByExternal();			// virtual callback
			} else {
				BTRACE("GetbDoDisconnect() == true");
			}
			BTRACE( "close bsd socket" );
			CloseSocket();
			BTRACE( "clear connection" );
			XESocketClient::ClearConnection();
//			DoDisconnect();
			BTRACE( "thread exit" );
			return;
		}
		XBREAK( readbytes > sizeBuff );
		// 버퍼의 내용을 큐로 보냄
		Lock();
#ifdef _VER_ANDROID
//  XLOGXN("recv:%d bytes", readbytes );
#endif
		XENetworkConnection::PushBlock( pBuffer, readbytes );		// 받은 데이터를 큐에 보냄
		Unlock();
	}
}

void XBSDSocketClient::ConnectThread( void )
{
	struct sockaddr_in addr;
	bzero( &addr, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( GetPort() );

	if( m_cURL.empty() ) {
		XBREAK( GetstrcIP().empty() );
		XBREAK( GetPort() == 0 );
		addr.sin_addr.s_addr = inet_addr( GetstrcIP().c_str() );  //"59.5.5.245"
		XLOGXN( "Try connect...%s:%d", GetstrcIP().c_str(), GetPort() );
	}	else {
		hostent* ent = gethostbyname( m_cURL.c_str() );
		if( XBREAK( ent == NULL ) )
			return;
		addr.sin_addr.s_addr = *(long int *)*ent->h_addr_list;
		XLOGXNA( "Try connect...:%s", inet_ntoa( addr.sin_addr ) );
	}

	int numTry = 0;
	while( 1 )	{
		int errCode = connect( m_Socket, ( struct sockaddr * )&addr, sizeof( addr ) );
		if( errCode == 0 ) {
			XLOGXN( "connect() success. %s:%d", GetstrcIP().c_str(), GetPort() );
			CreateWorkThread();
			Set_bConnected( TRUE );
			return;	// 연결되면 스레드 바로 종료.
		}
		XLOGXN( "connect failed(%d). %s:%d, err=%d", numTry, GetstrcIP().c_str(), GetPort(), errCode );
		// 커넥트에 실패했으면
		if( m_Socket == 0 ) {
			BTRACE("exit connect thread");
			return;
		}
		sleep( 1 );		// 1초쉬고 다시 시도
		// 10번 시도해보고 안되면 소켓생성부터 다시 해보자.
		if( ++numTry >= 10 ) {
			XLOGXN( "give up connect. retry create socket" );
			DoDisconnect();
			return;
		}
		// 타임아웃카운터 올리고 타임아웃시 특정 핸들러 호출 구현할것
	}
}


BOOL XBSDSocketClient::TryConnect( void )
{
	if( GetbDummy() )
		return TRUE;
  XBREAK( m_Socket == 0 );
//  m_bConnectThread = TRUE;
  int idThr = pthread_create( &m_hConnectThread, NULL, _ConnectThread, (void *)this);
  if( XBREAK( idThr < 0 ) )
		return FALSE;
	return XESocketClient::TryConnect();
}

void XBSDSocketClient::CreateWorkThread( void )
{
	XLOCK_OBJ;
//	m_bWorkThread = TRUE;
	int idThr = pthread_create( &m_hWorkThread, NULL, _WorkThread, ( void * )this );
	XBREAK( idThr < 0 );
	// 커넥트 스레드안에서 리턴하면서 자동으로 종료.
	// 	if( m_hConnectThread )
	// 		pthread_detach( m_hConnectThread );		// 140110추가
	// 	m_bConnectThread = FALSE;
}

void XBSDSocketClient::CloseConnectThread() 
{
	if( m_hConnectThread ) {
		void* tRet = nullptr;
		CloseSocket();
//		pthread_cancel( m_hConnectThread );				// 스레드 종료 요청.(ndk에서 pthread_cancel을 지원안함. 젠장)
//		pthread_join( m_hConnectThread, &tRet );		// 스레드가 종료될때까지 대기
	}
	m_hConnectThread = 0;				// win32 pthread는 이게 구조체이고 gcc에선 그냥 long형인듯.
//	m_bConnectThread = FALSE;
}

void XBSDSocketClient::CloseWorkThread()
{
	if( m_hWorkThread ) {
		void* tRet = nullptr;
		CloseSocket();
//		pthread_cancel( m_hWorkThread );				// 스레드 종료 요청.(ndk에서 pthread_cancel을 지원안함. 젠장)
//		pthread_join( m_hWorkThread, &tRet );		// 스레드가 종료될때까지 대기
	}
	m_hWorkThread = 0;
//	m_bWorkThread = FALSE;
}

void XBSDSocketClient::CloseAllThread() 
{
	CloseConnectThread();
	CloseWorkThread();
}

void XBSDSocketClient::DoDisconnect()
{
	XLOCK_OBJ;
	BTRACE("DoDisconnect:[%s]", C2SZ(GetstrcIP()) );
	// 스레드 종료(cbOndisconnectedByExternal이 발생하지 않아야함).
	SetbDoDisconnect( true );	// 검사용
	// 스레드를 닫고 스레드가 종료될때까지 대기
	BTRACE( "close all thread" );
	CloseAllThread();
	// 소켓 close 
	BTRACE( "close socket" );
	CloseSocket();
	// 관련 변수 클리어
	BTRACE( "XESocketClient::DoDisconnect();" );
	XESocketClient::DoDisconnect();
	BTRACE("finish do disconnect");
}

/**
 @brief 단지 소켓만 닫는다. 패킷큐는 클리어하지 않는다.
*/
void XBSDSocketClient::CloseSocket()
{
	XLOCK_OBJ;
	if( m_Socket ) {
		shutdown( m_Socket, SHUT_RDWR );
		close( m_Socket );
	}
	m_Socket = 0;
//	XESocketClient::ClearConnection();
}

BOOL XBSDSocketClient::IsDisconnected( void )
{
	XLOCK_OBJ;
	return (m_Socket == 0)? TRUE : FALSE;
}

void XBSDSocketClient::SendData( const BYTE *pBuffer, int len )
{
	XLOCK_OBJ;
	if( GetbDummy() )
		return;
	if( m_Socket )	{
		XBREAK( Get_bConnected() == FALSE );	// 아직 connect()는 안된상태
		int sendBytes = send( m_Socket, (const char *)pBuffer, len, 0 );		// 커넥션 상대에게 데이타를 보냄
		if( sendBytes == -1 )		{
			XLOGXN("try send %d byte..... failed", len );
			DoDisconnect();
		}
	}
}

// http Post 관련

void XBSDSocketClient::SetHttpData( const char* baseURL )
{
	m_cURL = baseURL;
}

//#undef sprintf_s	sprintf 

void XBSDSocketClient::ProcessHttp( const char* host, const char* page, const char* postStr, char *response )
{
	char sendline[4000+1], recvline[4000+1];
	size_t n;

#ifdef WIN32
	sprintf_s(sendline, 4000,
		"POST %s HTTP/1.0\r\n"
		"Host: %s\r\n"
		"Content-type: application/x-www-form-urlencoded\r\n"
		"Content-length: %d\r\n\r\n"
		"%s", page /* "/lyto/api/dev/0.1/login.php" */, host /* "vps.ritchienathaniel.com" */ , (int)strlen(postStr), postStr /* "device_id=355666050014363&app_id=gaME_DeV&app_secret=vMAj4MkVnHVWoTSOs7YRnRcCj4PlFU" */);
#else	
	
	sprintf_s(sendline,
		"POST %s HTTP/1.0\r\n"
		"Host: %s\r\n"
		"Content-type: application/x-www-form-urlencoded\r\n"
		"Content-length: %d\r\n\r\n"
		"%s", page /* "/lyto/api/dev/0.1/login.php" */, host /* "vps.ritchienathaniel.com" */ , (int)strlen(postStr), postStr /* "device_id=355666050014363&app_id=gaME_DeV&app_secret=vMAj4MkVnHVWoTSOs7YRnRcCj4PlFU" */);
#endif
	
	int iResult = send(m_Socket, sendline, strlen(sendline), 0);

	if ( iResult == -1) {
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
}

#define sprintf_s	sprintf 

#endif // not WIN32