#include "stdafx.h"
#include "XSocketClient.h"
#include "Network/xenDef.h"

#ifdef _XCRYPT_PACKET
#pragma message("define _XCRYPT_PACKET")
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
// XESocketClient::XESocketClient( const char *cIP, 
// 							unsigned short port, 
// 							XNetworkDelegate *pDelegate, 
// 							int sizeQueue,
// 							DWORD dwParam )
// 	: XENetworkConnection( pDelegate, sizeQueue )
// {
// 	Init();
// 	//
// 	if( cIP == NULL )
// 	{
// 		m_bDummy = TRUE;	// 시뮬레이션 모드로
// 		return;
// 	}
// 	_tcscpy_s( m_szIP, C2SZ( cIP ) );
// 	m_Port = port;
// 	m_dwConnectParam = dwParam;
// 
// }
XESocketClient::XESocketClient( XNetworkDelegate *pDelegate
															, int sizeQueue
															, DWORD dwParam )
	: XENetworkConnection( pDelegate, sizeQueue )
{
	Init();
	//
// 	if( cIP == NULL )	{
// 		m_bDummy = TRUE;	// 시뮬레이션 모드로
// 		return;
// 	}
// 	_tcscpy_s( m_szIP, C2SZ( cIP ) );
// 	m_Port = port;
//	m_dwConnectParam = dwParam;

}

void XESocketClient::Destroy()
{
}

void XESocketClient::SetIpAndPort( const char *cIP, WORD port ) 
{
	XLOCK_OBJ;
	XBREAK( XE::IsEmpty(cIP) );		// 이제 ip를 클리어하려면 ClearIPAndPort()를 쓸것.
	XBREAK( port == 0 );
	m_strcIP = cIP;
	m_Port = port;
}

void XESocketClient::ClearIPAndPort()
{
	XLOCK_OBJ;
	m_strcIP.clear();
	m_Port = 0;
}

BOOL XESocketClient::Send( XPacket& ar ) 
{ 
	XLOCK_OBJ;
//	if( XBREAK( GetpDelegate() == NULL ) )
//		return FALSE;
	if( GetbEncryption() )
		ar.DoCrypto( this, ar.GetidPacket() );
	try	{
		SendData( ar.GetBuffer(), ar.size() ); 
	}	catch (std::exception& e)	{
		XTRACE( "exception: %s ", e.what() );
	}
	return TRUE;
}

BOOL XESocketClient::TryConnect( void )
{
	return TRUE;
}

/**
 @brief 앞으로 DoDisconnect는 클라측에서 능동적으로 끊은경우만 해당하도록 바꿈.
 서버에서 끊었거나 인터넷불량등으로 끊겼을경우는 ClearConnection()만 하는걸로 한다.
*/
void XESocketClient::DoDisconnect()
{
	XLOCK_OBJ;
	m_bReadyServer = FALSE;
	Set_bConnected( FALSE );
	m_bDoDisconnect = false;
	// 온 패킷은 다 처리하기 위해 패킷큐는 클리어 하지 않음.
// 	m_bReadyServer = FALSE;
////	InitConnection();   // 온패킷은 다 처리고 하고 클리어 해야해서 위치 옮김.
////	Set_bConnected( FALSE );
}

/**
 @brief 연결정보를 초기화 한다.
 연결끊김을 외부요인과 능동적종료로 구분하기 위해 새로 만듬
 연결에 필요한 모든 정보를 초기화상태로 만든다.
*/
void XESocketClient::ClearConnection()
{
	XLOCK_OBJ;
	m_bReadyServer = FALSE;
	Set_bConnected( FALSE );
	m_bDoDisconnect = false;
	ClearPacketQ();
}

BOOL XESocketClient::ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	XE::xtPacketReserved packet = (XE::xtPacketReserved) idPacket;
#ifdef _CLIENT
//	XLOGXN("reserved packet: 0x%x", idPacket );
#endif
	switch( packet )
	{
	case XE::xXEPK_REQUEST_HB: {		// 서버로부터 살아있는지 응답요청 받음.
			XPacket ar((ID)XE::xXEPK_RESPONSE_HB);
			if( Get_bConnected() )
				Send( ar );
	} break;
	case XE::xXEPK_CONNECT_COMPLETE: {
#ifdef _XCRYPT_PACKET
			// 서버로부터 암호화 키를 받음 0을 받으면 암호화 기능 끔
			BYTE keyCrypto = 0, b1;
			p >> keyCrypto;
			SetkeyPrivate( keyCrypto );
			p >> b1 >> b1 >> b1;
			// 서버로부터 받은 암호화 모드에 따라 초기화를 함.
			if( keyCrypto )
				DeSerializeKeyTable( p );
			else
				SetEncrypt( FALSE );
#endif
			m_bReadyServer = TRUE;
			Set_bConnected( TRUE );
			// 접속핸들러 호출
#ifdef _CLIENT
			CONSOLE("xXEPK_CONNECT_COMPLETE:idConnect=0x%08x", GetidConnect() );
#endif
			// connect()에 성공했음을 의미함. login성공이 아님에 주의
			OnConnect();		// virtual
			if( GetpDelegate() )
				GetpDelegate()->DelegateConnect( this, /*m_dwConnectParam*/0 );
		}
		break;
	default:
		// 유저 정의 패킷.
		return FALSE;
	}
	return TRUE;
}

// this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
BOOL XESocketClient::Process( void )
{
	XLOCK_OBJ;
	XPacket p(true);

	// 접속끊어졌어도 클라에선 남아있는 패킷 다 처리해야함. 
//	while( PumpPacket( &p ) ) {
	if( PumpPacket( &p ) ) {
		if( GetbEncryption() )
			p.DoDecrypto( this );
		DWORD idPacket;
		p >> idPacket;
		ID idKey;	// 패킷 고유 키값
		p >> idKey;
		// 예약된 시스템 패킷부터 처리함.
		if( ProcessReservedPacket( idPacket, idKey, p ) == FALSE )	
		{
			// 유저정의 패킷을 처리함
			if( ProcessPacket( idPacket, idKey, p ) == FALSE )		// virtual call
			{
				// 패킷 처리에 실패함. 접속 끊어야 함.
				return FALSE;
			}
		}
		p.Clear();
	}
  // 온 패킷은일단 다 처리하고 끊어졌으면 초기화 시킨다
  if( IsDisconnected() )
    InitConnection();
	//
	return TRUE;
}

// IOCP를 사용시(ex:봇)
// 다수의 워커스레드에서 IOCP를 통해 해당 소켓에서 데이타가 들어오면 큐에 밀어넣음.
void XESocketClient::tRecvData( DWORD readbytes )
{
	Lock();
	//
	do 	{
//		BTRACE( "recv: %d bytes", readbytes );
		// 봇들중에서 x초간 가장 큐가 많이 쌓인 크기를 저장.
		const int sizeQ = GetSizeQueue();
		if( sizeQ > XENetworkConnection::sGetmaxQueue() )
			XENetworkConnection::sSetmaxQueue( sizeQ );
		
		// 받은 데이터를 큐에 밀어넣음.
		int bytePushed = XENetworkConnection::PushBlock( m_Buffer, readbytes );	// 최적화를 위해 만듬. 큐에 하나씩 보내지않고 뭉탱이로 보냄
		XBREAKF( bytePushed != readbytes
					, "bytePushed(%d) != readBytes(%d): ip=%s"
					, bytePushed, readbytes, C2SZ(m_strcIP) );
	} while(0);
	//
	Unlock();

}
