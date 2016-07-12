#include "stdafx.h"
#include "XCFSocketClient.h"
#include "Debug.h"

void _CFSockCallBack( CFSocketRef s,
                    CFSocketCallBackType callbackType,
                    CFDataRef address,
                    const void *data,
                    void *info )
{
    NSLog( @"callback!" );
    XCFSocketClient *This = (XCFSocketClient*)info;
    if( callbackType == kCFSocketDataCallBack )
    {
        NSLog( @"has data" );
        // 데이터 수신시 여기에 프로그래밍 하시오
        const UInt8 *d = CFDataGetBytePtr( (CFDataRef)data );
        int len = CFDataGetLength( (CFDataRef)data );
        if( len == 0 )
        {
            // 로긴서버 끊어진정보가 게임서버 콜백으로 날아오는건 게임서버 ip를 127.0.0.1로 해서 그랬다..
            NSLog( @"Disconnected" );
            This->OnDisconnect();
			This->DoDisconnect();
//            This->SetbDisconnected( TRUE );
        }
        else
            This->RecvCallBack( (unsigned char *)d, len );
//        for( int i = 0; i < len; ++i )
//            NSLog( @"%c", *(d+i) );
    } else
    if( callbackType == kCFSocketReadCallBack )
    {
        NSLog( @"to read" );
        // 소켓에서 읽을 수 있습니다. 
        char buf[100] = { 0, };
        int sock = CFSocketGetNative( s );
        NSLog( @"to read" );
        NSLog( @"read:%d", (int)recv(sock, &buf, 100, 0) );
        NSLog( @"%s", buf );
    } else
    if( callbackType == kCFSocketWriteCallBack )
    {
        NSLog( @"to write" );
        // 데이터 송신이 가능해졌습니다. 
//        char sendbuf[100] = { 0, };
//        strcpy( sendbuf, "hello" );
//        CFDataRef dt = CFDataCreate( NULL, (unsigned char*)sendbuf, 100 );
//        CFSocketSendData( s, NULL, dt, strlen(sendbuf) );
    } else
    if( callbackType == kCFSocketConnectCallBack )
    {
        NSLog( @"connected" );
        // 연결이 이루어 졌습니다
		This->Set_bConnected( TRUE );
    }
}
///////////////////////////////////////////////////////////////////////////////
XCFSocketClient::XCFSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param )
	: XESocketClient( cIP, port, pDelegate, sizeQueue, param )
//    : XNetworkConnection( pDelegate, sizeQueue )
{
	Init();
    NSLog( @"create CFSocket:0x%08lx", (DWORD)this );
    if( cIP == NULL )
        return;
    //
    CFSocketContext socketContext = { 0, this, NULL, NULL, NULL };
    CFSocketRef socket = NULL;
//    if( this == __LOGIN )
        socket = CFSocketCreate( kCFAllocatorDefault, PF_INET, SOCK_STREAM, 0,
                                     kCFSocketReadCallBack | kCFSocketDataCallBack | kCFSocketConnectCallBack | kCFSocketWriteCallBack,
                                     _CFSockCallBack, &socketContext );
    m_Socket = socket;
 
    struct sockaddr_in addr;
    memset( &addr, 0, sizeof(addr) );
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    addr.sin_addr.s_addr = inet_addr( cIP );
    CFDataRef addressData = CFDataCreate( NULL, (unsigned char *)&addr, sizeof(addr) );
    CFSocketError err = CFSocketConnectToAddress( socket, addressData, -1 );
 
    //
    CFRunLoopSourceRef frameRunLoopSource = CFSocketCreateRunLoopSource( NULL, socket, 0 );
    CFRunLoopAddSource( CFRunLoopGetCurrent(), frameRunLoopSource, kCFRunLoopCommonModes );
//    CFRunLoopAddSource( CFRunLoopGetMain(), frameRunLoopSource, kCFRunLoopDefaultMode );
    CFRelease( frameRunLoopSource );
    
	//
    m_pSocket = (void *)socket;
//	strcpy_s( m_cIP, sizeof(m_cIP), cIP );
//	m_Port = port;
//	m_bReadyServer = FALSE;
}

void XCFSocketClient::Destroy()
{
    NSLog( @"destroy CFSocket" );
    if( m_Socket )
    {
        CFSocketInvalidate( m_Socket );
        CFRelease( m_Socket );
        m_Socket = NULL;
    }
}

void XCFSocketClient::DoDisconnect( void )
{
	XLOCK_OBJ;
    if( m_Socket )
    {
		NSLog( @"destroy CFSocket" );
        CFSocketInvalidate( m_Socket );
        CFRelease( m_Socket );
        m_Socket = NULL;
    }
}

// 현재 끊긴상태인지 플랫폼별 방식으로 알아내어 알려주는걸 구현해야 한다.
BOOL XCFSocketClient::IsDisconnected( void )
{
	XLOCK_OBJ;
	return (m_Socket == NULL ) ? TRUE : FALSE;
}

void XCFSocketClient::RecvCallBack( unsigned char *buffer, int readbytes )
{
    XENetworkConnection::PushBlock( buffer, readbytes );		// 받은 데이터를 큐에 보냄
}

// 접속 시도
BOOL XCFSocketClient::TryConnect( void )
{
	// 커넥트 스레드 생성
	return TRUE;
}

// 소켓에 연결이 성공했다
void XCFSocketClient::OnConnect( void )
{
}
/*
// pConnect의 커넥션을 끊는다.
void XCFSocketClient::DoDisconnect()
{
    if( m_Socket )
    {
        CFSocketInvalidate( m_Socket );
        CFRelease( m_Socket );
        m_Socket = NULL;
    }
	XESocketClient::DoDisconnect();
	// 아직 연결도 안된상태에서 명령이 오면 플래그만 세팅함
//	if( GetbConnected() == FALSE )
//		SetbDisconnected( TRUE );	
}
*/
/*
// this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
BOOL XCFSocketClient::Process( void )
{
	XPacket p;
	while( PumpPacket( &p ) )
	{
		if( ProcessPacket( p ) == FALSE )		// virtual call
		{
			// 패킷 처리에 실패함. 접속 끊어야 함.
			return FALSE;
		}
		p.Clear();
	}
	return TRUE;
}
*/
/*
BOOL XCFSocketClient::PumpPacket( XPacket *pOutPacket )
{
	//
	BOOL bRet = FALSE;
	if( m_bReadyServer  )
	{
		//
		bRet = XNetworkConnection::PumpPacket( pOutPacket );
	} else
	{
		// 서버에서 아직 패킷받을 준비가 되었다는 메시지가 안왔다면
		bRet = XNetworkConnection::PumpPacket( pOutPacket );
		if( bRet )
		{
			// 완성된 패킷이 도착했다.
			ID idPacket;
			*pOutPacket >> idPacket;
			XBREAK( idPacket != (DWORD)0x80000001 );
			m_bReadyServer = TRUE;
			// 접속핸들러 호출
			OnConnect();		// virtual
			if( GetpDelegate() )
				GetpDelegate()->DelegateConnect( this );
			bRet = FALSE;
		}
	}
	//
	return bRet;
}
*/
void XCFSocketClient::SendData( const BYTE *pBuffer, int len )
{
    if( GetbDummy() )
        return;
    CFSocketRef socket = (CFSocketRef)m_pSocket;
    CFDataRef data = CFDataCreate( NULL, pBuffer, len );
    CFSocketSendData( socket, NULL, data, -1 );
    CFRelease( data );

    
}
