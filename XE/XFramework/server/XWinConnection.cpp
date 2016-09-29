#include "stdafx.h"
#include "XWinConnection.h"
#include <stdio.h>
#include "XEUser.h"
#include "XWinSocketSvr.h"
#include "Network/xenDef.h"
#include "XFramework/XEProfile.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define _USING_WSA

//XSharedObj<int> XEWinConnectionInServer::m_shoNumCreateObj;	// 생성된 커넥션 수-메모리 릭 조사용
int XEWinConnectionInServer::s_numCreateObj = 0;		// 생성된 커넥션 수-메모리 릭 조사용

#define CONSOLE_THIS( TAG, F, ... ) \
	__xLogfTag( TAG, XLOGTYPE_LOG, XTSTR("%s:-", F), __TFUNC__, ##__VA_ARGS__);

//////////////////////////////////////////////////////////////////////////
XEWinConnectionInServer::XEWinConnectionInServer( LPCTSTR szTag
																								, XEWinSocketSvr *pSockServer
																								, SOCKET socket
																								, LPCTSTR szIP
																								, int sizeQueue )
	: XENetworkConnection( NULL, sizeQueue )
{
	Init();
	m_spLock = std::make_shared<XLock>();
	if( szTag ) {
		TCHAR szBuff[256];
		_stprintf_s( szBuff, _T("%s-0x%08x"), szTag, GetidConnect() );
		m_strtTag = szBuff;
	}

	m_Socket = socket;
	_tcscpy_s( m_szIP, szIP );
	m_pSocketSvr = pSockServer;
	//
	::InterlockedIncrement( (LONG*)(&s_numCreateObj) );
// 	++m_shoNumCreateObj.GetSharedObj();
// 	m_shoNumCreateObj.ReleaseSharedObj();
}

void XEWinConnectionInServer::Destroy() 
{
	if( m_Socket )
		closesocket( m_Socket );
	m_Socket = 0;
	m_spUser.reset();
//	SAFE_RELEASE_REF( m_pUser );
	::InterlockedDecrement( (LONG*)(&s_numCreateObj) );
// 	--m_shoNumCreateObj.GetSharedObj();
// 	m_shoNumCreateObj.ReleaseSharedObj();
}

// int XEWinConnectionInServer::Release() 
// {
// 	XLOCK_OBJ;
// 	int cntRef = XRefObj::Release();
// 	return cntRef;
// }

BOOL XEWinConnectionInServer::IsDisconnected() const
{
	if( !Get_bConnected() )
		return TRUE;
// 	XLOCK_OBJ;
	BOOL bRet = ( m_Socket == 0 ) ? TRUE : FALSE;
	return bRet;
}

/**
 @brief 능동적으로 연결을 끊을때만 사용하도록 성격이 바뀜
*/
void XEWinConnectionInServer::DoDisconnect() 
{
// 	XLOCK_OBJ;
	if( m_Socket ) {
		XTRACE( "DoDisconnect: idConnect=0x%08x", m_idConnect );
		if( m_Socket )
			closesocket( m_Socket );
		m_Socket = 0;
	}
}

/**
 @brief 타의. 즉 클라측의 연결해제로 끊어질때 사용하는걸로 컨셉을 잡음.
 DoDisconnect()와 구별해서 사용할것.
*/
void XEWinConnectionInServer::ClearConnection()
{
	XENetworkConnection::ClearConnection();
	if( m_Socket )
		closesocket( m_Socket );
	m_Socket = 0;
}

/**
 비동기로 recv를 시킴
 다수의 워커스레드로부터 호출됨.
*/
void XEWinConnectionInServer::tWSARecv( void )
{
// 	XLOCK_OBJ;
	//
	do 	{
// 		if( IsDisconnected() )
// 			break;;		// 
// 		if( IsDestroy() )
// 			break;
	//	BOOL bResult = TRUE;
		unsigned long readbytes; 
		// 비동기로 전송처리, 큐에 올라감. 이걸해야 스레드에서 데이타가 받아짐.
	#ifdef _USING_WSA 
		WSABUF b[1]; 
		unsigned long flags = 0; 
// 		XE::xOVERLAPPED overlapped;
 		memset( &m_opRecv, 0, sizeof(m_opRecv) );
// 		overlapped.typeEvent = 2;
		m_opRecv.typeEvent = 2;
		//WSAOVERLAPPED overlapped;
//		OVERLAPPED overlapped;
		b[0].buf = (CHAR *)m_Buffer; 
		b[0].len = sizeof(m_Buffer); 
		if( WSARecv( m_Socket, b, 1, &readbytes, &flags, &m_opRecv, NULL ) == SOCKET_ERROR )
//		if( WSARecv( m_Socket, b, 1, &readbytes, &flags, (LPWSAOVERLAPPED)&overlapped, NULL ) == SOCKET_ERROR )
	#else
		// 최초 accept시에는 아래처림 == FALSE로 검사하면 안됨. 원래 0나옴
		if( ReadFile( (HANDLE)m_Socket, m_Buffer, sizeof(m_Buffer), &readbytes, &m_op ) == FALSE )
	#endif
		{
			const auto lastErr = GetLastError();
			XBREAKF( lastErr != WSAECONNRESET	&& lastErr != ERROR_IO_PENDING
						, "WSARecv error:err=%d", lastErr );
			if( lastErr != WSAECONNRESET		// 접속끊김
				&& lastErr != ERROR_IO_PENDING ) {	// IO처리중
				CONSOLE( "WSARecv error: socket=%x, error=%d", m_Socket, lastErr );
				ClearConnection();
			}
		}
	} while(0);
	//
	return;
}

// 다수의 워커스레드에서 해당 커넥션(소켓)으로부터 데이타를 받으면 호출됨
bool XEWinConnectionInServer::tRecvData( DWORD readbytes )
{
	bool bOk = true;
	do {
// 		if( IsDisconnected() == TRUE )		// 이런경우가 있음(언젠진 모름)
// 			break;
// 		if( IsDestroy() )
// 			break;
		// 이전에 m_Buffer에 받았던데이타가 아직 큐에 밀어넣지 못한채로 또 불렸다.
		// 받은 데이터를 큐에 밀어넣음.
		int bytePushed = XENetworkConnection::PushBlock( m_Buffer, readbytes );	// 최적화를 위해 만듬. 큐에 하나씩 보내지않고 뭉탱이로 보냄
		const int sizeQ = GetSizeQueue();
		if( sizeQ > GetsizeMaxQ() )
			SetsizeMaxQ( sizeQ );
		// 만약 큐가 꽉차서 더이상 못밀어 넣었다면?
		// 큐는 넉넉하게 잡아놔야겠지만 그것마저 꽉 채울정도라면 비정상으로 보고 끊어야 하지 않을까 싶다.
		// 일단은 로그라도 남겨야 한다.
		// 밀어넣은데이타와 실제 푸쉬된 데이타의 크기가 다르면 에러.
// 		XBREAKF( bytePushed != readbytes, "bytePushed(%d) != readBytes(%d): ip=%s", 
// 														bytePushed, readbytes, m_szIP );
		if( bytePushed != readbytes )	{
			OnError( XENetworkConnection::xERR_Q_IS_FULL, sizeQ );
			DoDisconnect();
			bOk = false;
		}
// 		m_bFlush = TRUE;
	} while(0);
	return bOk;
}

void XEWinConnectionInServer::Send( XPacket& ar ) 
{ 
//	XPROF_OBJ_AUTO();
	XAUTO_LOCK3( this );
	if( IsbDestroy() )
		return;
	const ID idPacket = ar.GetidPacket();
	if( m_pSocketSvr->GetbEncryption() && ar.GetbEncryption() == FALSE )
		ar.DoCrypto( m_pSocketSvr, idPacket );
	auto pBuff = ar.GetBuffer();
	const int size = ar.size();
	SendData( pBuff, size ); 
}

void XEWinConnectionInServer::SendData( const BYTE *pBuffer, int len )
{
// 	XLOCK_OBJ;
	if( IsDisconnected() )
		return;
	if( m_Socket ) {	// 커넥션이 끊어진 이후에 올수도 있다.
 		memset( &m_opSend, 0, sizeof( m_opSend ) );
		m_opSend.typeEvent = 1;
		DWORD sendBytes = 0;
		WSABUF b[ 1 ];
		b[0].buf = (CHAR*)pBuffer;
		b[0].len = len;
		// WSARecv와 마찬가지로 미리 실행시키고 완료되면 워커스레드의 IOCP에서 통보받는다.
		auto result = WSASend( m_Socket, b, 1, &sendBytes, 0, (LPWSAOVERLAPPED)&m_opSend, nullptr );
		if( result == SOCKET_ERROR ) {
			const auto lastErr = GetLastError();
			if( XBREAK(lastErr != WSAECONNRESET 
							&& lastErr != ERROR_IO_PENDING)) {
				int numError = WSAGetLastError();
				CONSOLE_THIS( "connect", "WSAGetLastError() == 0x%x socket=%d this=0x%0x idConnect=0x%0x",
					numError, (DWORD)m_Socket, ( DWORD )this, m_idConnect );
				ClearConnection();
			} else {
				XBREAK( sendBytes != 0 && (DWORD)len != sendBytes );	// 어떤경우에 생기는지 확인하려고
			}
		} else {
			XBREAK( sendBytes != 0 && (DWORD)len != sendBytes );	// 어떤경우에 생기는지 확인하려고
		}
		// 전부다 못보낸 경우도 처리해야 할듯.
#pragma message( "check this" )
	} // Socket
}

// 큐 버퍼에서 새로운 패킷을 뽑아낸다.. 패킷이 도착하지 않았으면 FALSE를 리턴한다
BOOL XEWinConnectionInServer::PumpPacket( XPacket *pOutPacket )
{
// 	XLOCK_OBJ;
	if( IsbDestroy() )
		return FALSE;
	BOOL bRet = XENetworkConnection::PumpPacket( pOutPacket );
	return bRet;
}

/**
 @brief xHB_READY상태에서 다음 HB를 보내기까지 기다리는 타이머를 리셋시킨다.
 @param secTimeout "클라: 나 잠시 딴거할테니 secTimeout시간동안 내가 안오면 다시 HB를 보내도 좋다"
*/
void XEWinConnectionInServer::ResetHeartBeatTimer( int secTimeout )
{
// 	XLOCK_OBJ;
	if( m_modeHeartBeat == xHB_READY ) {
		XBREAK( secTimeout <= 0 );
		m_timerHeartBeat.Set( secTimeout );
	} else
	if( m_modeHeartBeat == xHB_REQUEST ) {
		// HB보내고 기다리던중이었으면 다시 대기상태로 전환시킴.
		m_modeHeartBeat = xHB_READY;
		m_timerHeartBeat.Set( secTimeout );
	} else {
		XBREAK(1);
	}
}

/**
 @brief 
*/
void XEWinConnectionInServer::ProcessHeartBeat()
{
// 	XLOCK_OBJ;
	//////////////////////////////////////////////////////////////////////////
	if( m_modeHeartBeat == xHB_READY ) {
		if( m_timerHeartBeat.IsOff() )
			m_timerHeartBeat.Set( 10.f );
		// x초 기다리고 요청패킷을 보내본다.
		if( m_timerHeartBeat.IsOver() ) {
			// 확인패킷 보내고 10초짜리 타이머 돌린다. 10초안에 응답이 안오면 이 커넥트는 끊는다.
			// 아이폰에서 테스트할때 와이파이에서 3G로 바꿔서 연결끊어졌는데 소켓으로 데이타는 계속 날아가는
			// 현상 있었음. 클라가보내는 응답도 받아야 할듯?
			XPacket ar((ID)XE::xXEPK_REQUEST_HB);
			Send( ar );		
			if( IsDisconnected() == FALSE ) {
				m_modeHeartBeat = xHB_REQUEST;
				m_timerHeartBeat.Set( 30.f );
			}
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( m_modeHeartBeat == xHB_REQUEST ) {
		// 기다리는중. 
		if( m_timerHeartBeat.IsOver() ) {
#if defined(_DEBUG) && !defined(_XNOT_BREAK)
			m_modeHeartBeat = xHB_READY;
			m_timerHeartBeat.Off();
//			DoDisconnect();		// 디버그 모드에서 끊김 테스트를 위해..
#else
			// 응답이 없으므로 이 커넥션은 끊는다.
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
			auto spUser = GetspUser();
			if( spUser ) {
				CONSOLE("응답이 없는 소켓이 발견되어 연결을 끊습니다: ip=%s, idAcc=%d", m_szIP, spUser->GetidUser() );
			} else {
				CONSOLE("응답이 없는 소켓이 발견되어 연결을 끊습니다: %s", m_szIP );
			}
#endif
      OnFireConnectBefore();    // 서버에서 자르기 전에 이벤트 핸들러.
			DoDisconnect();
			m_timerHeartBeat.Off();
#endif // not debug
		} 
	}// else
}
/**
 @brief 
*/
void XEWinConnectionInServer::Process( void )
{
	XPROF_OBJ_AUTO();
	// XWinSocketSvr::Process()에서 커넥션별로 락걸어서 들어오고 있음.
// 	XLOCK_OBJ;
	// 소켓이 끊어졌으면 더이상 돌필요 없다.
// 	if( m_Socket == 0 )		// 
// 		return;
	// 쌓인 패킷 다 뽑아낼때까지 락걸어두자 패킷 계속오면 계속 못빠져나올수도...
	if( !GetbDestroy() ) {
		// 만약 커넥션 프로세스도 멀티스레드에서 돌아가면 이거 전체를 락걸어야 한다.
		// 자동연결해제 옵션이 있으면 허트비트 처리
		if( GetbAutoDisconnect() ) {
			XPROF_OBJ("ProcessHeartBeat");
			ProcessHeartBeat();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			XPROF_OBJ( "PumpPacket" );
			XPacket p( true );
			// 		while( PumpPacket( &p ) )	{
			// 패킷 한번만 뽑고 넘어감.
			auto bPump = PumpPacket( &p );
			if( bPump ) {
				XPROF_OBJ( "ProcessPacket" );
#ifdef _XCRYPT_PACKET
				BOOL bOk = FALSE;
				if( m_pSocketSvr->GetbEncryption() ) {
					bOk = p.DoDecrypto( m_pSocketSvr );
				}
#endif
				DWORD idPacket;
				p >> idPacket;
				ID idKey;
				p >> idKey;
				// 예약된 패킷 처리
				if( idPacket == XE::xXEPK_RESPONSE_HB )	{
#ifdef _DEBUG
					//			CONSOLE("recv heart beat");
#endif
					// 응답이 왔으므로 다시 대기상태로...
					m_timerHeartBeat.Off();
					m_modeHeartBeat = xHB_READY;
				}
				else {
					p._SetidPacket( idPacket );
					BOOL result = ProcessPacket( idPacket, idKey, p );
					if( result == FALSE ) {		// virtual call
						// 패킷 처리에 실패함. 접속 끊어야 함.
						// 일단 커넥션만 끊음.
						DoDisconnect();
						//					break;
					}
				}
				p.Clear();
			} // while( PumpPacket( &p ) )	{
		}
	} // bDestroy
}

void XEWinConnectionInServer::ProcesssAsyncDisconnect()
{
// 	XLOCK_OBJ;
	// 비동기 커넥션 파괴(혹시 패킷 처리하고 파괴해야할일이 있을지몰라 밑에다 넣음.
	if( m_timerAsyncDestroy.IsOver() ) {
		m_timerAsyncDestroy.Off();
		SetbDestroy( true );
//		CONSOLE( "Async disconnect" );
		return;
	}
}

// spAcc로 유저객체를 만들어 this커넥션에 붙인다.
XSPUserBase XEWinConnectionInServer::CreateAddUser( XSPDBAcc spAcc )
{
// 	XLOCK_OBJ;
	return m_pSocketSvr->CreateAddToUserMng( GetThis(), spAcc );
}

