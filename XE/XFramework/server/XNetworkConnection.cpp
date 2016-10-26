#include "stdafx.h"
#ifdef _SERVER
#include "XNetworkConnection.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// float XENetworkConnection::s_maxQueueRatio = 0;			// 모든커넥션들 중에서 최고로 많이 쌓였던 큐 크기
// float XENetworkConnection::s_maxQueueSize = 0;			// 모든커넥션들 중에서 최고로 많이 쌓였던 큐 크기

XENetworkConnection::XENetworkConnection( XNetworkDelegate *pDelegate, int sizeQueue ) 
{
	m_idConnect = XE::GenerateID();
	m_pDelegate = pDelegate;		// null이 올수도 있음.
	m_sizePacket = 0;
	m__bConnected = FALSE;
	//		m_bDisconnected = FALSE;
	m_qBuffer.Create( sizeQueue );	// 이제 모두 동적 할당큐로 바뀜
}

// 받은 데이터를 큐에 밀어넣음.
int XENetworkConnection::PushBlock( BYTE *buffer, DWORD readbytes ) 
{
	for( int i = 0; i < (int)readbytes; ++i ) {
		m_qBuffer.push( buffer[i] );
	}
// #if _DEV_LEVEL <= DLV_OPEN_BETA
// 	// 가장 많이 쌓인 커넥션의 큐 크기를 전역으로 담음.
// 	const int sizeQ = m_qBuffer.size();
// 	if( sizeQ > s_maxQueueSize )
// 		s_maxQueueSize = sizeQ;
// #endif
	return readbytes;
// 	int bytePushed = m_qBuffer.PushBlock( buffer, readbytes );	// 최적화를 위해 만듬. 큐에 하나씩 보내지않고 뭉탱이로 보냄
// 	float maxRatio = (float)m_qBuffer.size() / m_qBuffer.GetMax();
// 	if( maxRatio > s_maxQueueRatio )
// 		s_maxQueueRatio = maxRatio;
//	return bytePushed;
}

/**
 @brief 큐 버퍼에서 새로운 패킷을 뽑아낸다.. 패킷이 도착하지 않았으면 FALSE를 리턴한다
*/
BOOL XENetworkConnection::PumpPacket( XPacket *pOutPacket )
{
	// 패킷펌핑하고 있을때 다른 스레드가 이 커넥션을 사용하지 못하게 막는다.
	//
	BOOL bRet = FALSE;
	{
		// 최초 읽은 4바이트는 패킷의 총 길이다		
		if( m_qBuffer.size() >= sizeof(DWORD) ) {		// 큐버퍼에 최소4바이트가 모여 패킷길이를 알수 있게 되었다
			if( m_sizePacket == 0 ) {
				m_sizePacket = m_qBuffer.PopDWORD();	// 큐에서 4바이트를 꺼낸다.
				if( XBREAK( m_sizePacket > 0x1fff /*|| m_sizePacket <= 4*/) ) {
					// 정상적이지 않은 패킷이 날아왔음. 접속끊어야 함.
					m_qBuffer.clear();
					DoDisconnect();		// 즉시 접속을 끊어라.
					// 에러 핸들러 호출
					OnError( xERR_PACKET_SIZE_TO_LARGE, m_sizePacket );
					return FALSE;
				}
			}
		}
		XBREAK( m_sizePacket > 0x1fff );		// 뭔가 잘못된 경우다
		// 패킷 하나가 완전히 도착하면 패킷 아카이브를 만든다
		if( m_sizePacket > 0 && (DWORD)m_qBuffer.Size() >= m_sizePacket )	// 필요한 패킷양 이상이 큐에 쌓였다
		{
			if( pOutPacket->IsOverBuffer( m_sizePacket ) ) {
				pOutPacket->ReAllocBuffer( m_sizePacket + 16 );	// 안전을 위해 16바이트 더 늘임.
			}
			int popSize = m_qBuffer.PopBlock( pOutPacket->GetBufferPacket()
																			, pOutPacket->GetMaxBufferSize()
																			, m_sizePacket );	// 큐에서 sizePacket만큼 pop시킨다
			if( XBREAK( popSize == 0 ) )
			{
				// 패킷에 담는데 에러가 남 접속끊어야 함.
				m_qBuffer.clear();
				//				SetbDestroy( TRUE );
				DoDisconnect();
				OnError( xERR_PACKET_SIZE_TO_LARGE, m_sizePacket );
				return FALSE;
			}
			pOutPacket->SetPacketLength( m_sizePacket );	// 완성된 패킷이 준비되었다
#ifdef _XCRYPT_PACKET
			pOutPacket->IsCryptThenSetFlag();	// 암호화 된 메모리인지 플래그를 세팅한다.
#endif
			m_sizePacket = 0;
			bRet = TRUE;
		}
	}
	//
	return bRet;
}

#endif // server