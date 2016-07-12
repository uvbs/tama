#include "stdafx.h"
#include "XWinConnection.h"
#include <stdio.h>
#include "XEUser.h"
#include "XWinSocketSvr.h"
#include "Network/xenDef.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define _USING_WSA

XSharedObj<int> XEWinConnectionInServer::m_shoNumCreateObj;	// 생성된 커넥션 수-메모리 릭 조사용

XEWinConnectionInServer::XEWinConnectionInServer( LPCTSTR szTag
																								, XEWinSocketSvr *pSockServer
																								, SOCKET socket
																								, LPCTSTR szIP
																								, int sizeQueue )
	: XENetworkConnection( NULL, sizeQueue )
{
	Init();
	if( szTag ) {
		TCHAR szBuff[256];
		_stprintf_s( szBuff, _T("%s-0x%08x"), szTag, GetidConnect() );
		m_strtTag = szBuff;
	}

	m_Socket = socket;
	_tcscpy_s( m_szIP, szIP );
	m_pSocketSvr = pSockServer;
	//
	++m_shoNumCreateObj.GetSharedObj();
	m_shoNumCreateObj.ReleaseSharedObj();
}

void XEWinConnectionInServer::Destroy() 
{
	XLOCK_OBJ;
	if( m_Socket )
		closesocket( m_Socket );
	m_Socket = 0;
	SAFE_RELEASE_REF( m_pUser );
	--m_shoNumCreateObj.GetSharedObj();
	m_shoNumCreateObj.ReleaseSharedObj();
}

int XEWinConnectionInServer::Release() 
{
	XLOCK_OBJ;
	int cntRef = XRefObj::Release();
	return cntRef;
}

BOOL XEWinConnectionInServer::IsDisconnected() 
{
	XLOCK_OBJ;
	BOOL bRet = ( m_Socket == 0 ) ? TRUE : FALSE;
	return bRet;
}

void XEWinConnectionInServer::DoDisconnect() 
{
	XLOCK_OBJ;
	if( m_Socket ) {
		XTRACE( "DoDisconnect: idConnect=0x%08x", m_idConnect );
		closesocket( m_Socket );
		m_Socket = 0;
	}
}

/**
 비동기로 recv를 시킴
 다수의 워커스레드로부터 호출됨.
*/
void XEWinConnectionInServer::tWSARecv( void )
{
	XLOCK_OBJ;
	//
	do 	{
		if( IsDisconnected() )
			break;;		// 
		if( IsDestroy() )
			break;
	//	BOOL bResult = TRUE;
		unsigned long readbytes; 
		// 비동기로 전송처리, 큐에 올라감. 이걸해야 스레드에서 데이타가 받아짐.
	#ifdef _USING_WSA 
		WSABUF b[1]; 
		unsigned long flags = 0; 
		b[0].buf = (CHAR *)m_Buffer; 
		b[0].len = sizeof(m_Buffer); 
		if( WSARecv( m_Socket, b, 1, &readbytes, &flags, &m_op, NULL ) == SOCKET_ERROR )
	#else
		// 최초 accept시에는 아래처림 == FALSE로 검사하면 안됨. 원래 0나옴
		if( ReadFile( (HANDLE)m_Socket, m_Buffer, sizeof(m_Buffer), &readbytes, &m_op ) == FALSE )
	#endif
		{
			TCHAR szBuff[ 1024 ];
			_stprintf_s( szBuff, _T("WSARecv error: socket=%x, error=%d"), m_Socket, GetLastError() );
			if( GetLastError() != ERROR_IO_PENDING )
			{
				DoDisconnect();
#pragma message("접속끊긴 에러(10054)에러 외에것은 로그 남기자.")
//				XTRACE( "GetLastError() != ERROR_IO_PENDING" );
//				AfxDebugBreak();
			}
//			XBREAKF( 1, "WSARecv error: socket=%x, error=%d", m_Socket, GetLastError() );
//			XBREAK( GetLastError() != ERROR_IO_PENDING );
			// 어떤경우에 이런 에러가 나오는가. 이런에러가 실제로 발생하면 자세하게 조사해볼것.
			// 접속이 끊길때 WSARecv가 실패하는일이 있던가 그랬던듯. 그래서 error_io_pending으로 걸렀던거 같음.
	//		bResult = FALSE;
		}
	} while(0);
	//
//	Unlock();
	return;
}

// 다수의 워커스레드에서 해당 커넥션(소켓)으로부터 데이타를 받으면 호출됨
void XEWinConnectionInServer::tRecvData( DWORD readbytes )
{
	XLOCK_OBJ;
	//
	do {
		if( IsDisconnected() == TRUE )		// 이런경우가 있음(언젠진 모름)
			break;
		if( IsDestroy() )
			break;
		// 이전에 m_Buffer에 받았던데이타가 아직 큐에 밀어넣지 못한채로 또 불렸다.
	#ifdef _BOT
		XBREAK( m_bFlush == TRUE );
	#endif
		// 받은 데이터를 큐에 밀어넣음.
		int bytePushed = XENetworkConnection::PushBlock( m_Buffer, readbytes );	// 최적화를 위해 만듬. 큐에 하나씩 보내지않고 뭉탱이로 보냄
		// 만약 큐가 꽉차서 더이상 못밀어 넣었다면?
		// 큐는 넉넉하게 잡아놔야겠지만 그것마저 꽉 채울정도라면 비정상으로 보고 끊어야 하지 않을까 싶다.
		// 일단은 로그라도 남겨야 한다.
		// 밀어넣은데이타와 실제 푸쉬된 데이타의 크기가 다르면 에러.
		XBREAKF( bytePushed != readbytes, "bytePushed(%d) != readBytes(%d): ip=%s", 
														bytePushed, readbytes, m_szIP );
		if( bytePushed != readbytes )	{
			OnError( XENetworkConnection::xERR_Q_IS_FULL, GetSizeQueue() );
			DoDisconnect();
		}
		m_bFlush = TRUE;
	} while(0);

}

void XEWinConnectionInServer::Send( XPacket& ar ) 
{ 
	if( IsDestroy() )
		return;
#ifdef _XCRYPT_PACKET
	if( m_pSocketSvr->GetbEncryption() && ar.GetbEncryption() == FALSE )
		ar.DoCrypto( m_pSocketSvr );
#endif
	SendData( ar.GetBuffer(), ar.size() ); 
}

void XEWinConnectionInServer::SendData( const BYTE *pBuffer, int len )
{
	XLOCK_OBJ;
//	Lock();	// <<-여기서 락잡는건 변수값 보호는 되지만 커넥션삭제땐 보호되지 않음. 이미 들어온채로 블로킹되어 삭제될수 있음.
	//
	if( IsDestroy() )
		return;
	if( m_Socket ) {	// 커넥션이 끊어진 이후에 올수도 있다.
		int sizeSend = send( m_Socket, (const char *)pBuffer, len, 0 );		// 커넥션 상대에게 데이타를 보냄
		if( sizeSend == SOCKET_ERROR )	{
			if( GetLastError() != ERROR_IO_PENDING ) {
				int numError = WSAGetLastError();
				CONSOLE( "WSAGetLastError() == 0x%x socket=%d this=0x%0x idConnect=0x%0x", 
					numError, (DWORD)m_Socket, (DWORD)this, m_idConnect );
				// 끊어진 소켓에 send를 시도한 경우.
				DoDisconnect();
			}
		}
		// 전부다 못보낸 경우도 처리해야 할듯.
#pragma message( "check this" )
//		XBREAK( sizeSend != len );
//		XTRACE( "SendData:%d bytes", sizeSend );
	}
	//
//	Unlock();
}

// 큐 버퍼에서 새로운 패킷을 뽑아낸다.. 패킷이 도착하지 않았으면 FALSE를 리턴한다
BOOL XEWinConnectionInServer::PumpPacket( XPacket *pOutPacket )
{
	XLOCK_OBJ;
	if( IsDestroy() )
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
	XLOCK_OBJ;
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
	XLOCK_OBJ;
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
	#ifdef _DEV
			// 응답이 없으므로 이 커넥션은 끊는다.
			if( m_pUser )
				CONSOLE("응답이 없는 소켓이 발견되어 연결을 끊습니다: ip=%s, idAcc=%d", m_szIP, m_pUser->GetidUser() );
			else
				CONSOLE("응답이 없는 소켓이 발견되어 연결을 끊습니다: %s", m_szIP );
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
	XLOCK_OBJ;
	// 소켓이 끊어졌으면 더이상 돌필요 없다.
	if( m_Socket == 0 )
		return;
	// 쌓인 패킷 다 뽑아낼때까지 락걸어두자 패킷 계속오면 계속 못빠져나올수도...
	if( !GetbDestroy() ) {
		// 만약 커넥션 프로세스도 멀티스레드에서 돌아가면 이거 전체를 락걸어야 한다.
		// 허트비트 처리
		if( GetbAutoDisconnect() ) {
			ProcessHeartBeat();
		}
		//////////////////////////////////////////////////////////////////////////
		//
		XPacket p(true);
		while( PumpPacket( &p ) )	{
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
			} else {
				p._SetidPacket( idPacket );
				BOOL result = ProcessPacket( idPacket, idKey, p );
				if( result == FALSE ) {		// virtual call
					// 패킷 처리에 실패함. 접속 끊어야 함.
					// 일단 커넥션만 끊음.
					DoDisconnect();
					break;
				}
			}
			p.Clear();
		} // while( PumpPacket( &p ) )	{
	} // bDestroy
}

void XEWinConnectionInServer::ProcesssAsyncDisconnect()
{
	XLOCK_OBJ;
	// 비동기 커넥션 파괴(혹시 패킷 처리하고 파괴해야할일이 있을지몰라 밑에다 넣음.
	if( m_timerAsyncDestroy.IsOver() ) {
		m_timerAsyncDestroy.Off();
		SetbDestroy( true );
//		CONSOLE( "Async disconnect" );
		return;
	}
}

// pAccount로 유저객체를 만들어 this커넥션에 붙인다.
XEUser* XEWinConnectionInServer::CreateAddUser( XDBAccount *pAccount )
{
	XLOCK_OBJ;
	return m_pSocketSvr->CreateAddUser( this, pAccount );
}

