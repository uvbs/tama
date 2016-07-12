#include "stdafx.h"
#include "XNetworkConnection.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _SERVER
#error "클라전용입니다."
#endif

int XENetworkConnection::s_maxQueue = 0;			// 모든커넥션들 중에서 최고로 많이 쌓였던 큐 크기

// 큐 버퍼에서 새로운 패킷을 뽑아낸다.. 패킷이 도착하지 않았으면 FALSE를 리턴한다
BOOL XENetworkConnection::PumpPacket( XPacket *pOutPacket )
{
	// 패킷펌핑하고 있을때 다른 스레드가 이 커넥션을 사용하지 못하게 막는다.
	XLOCK_OBJ;
	//
    BOOL bRet = FALSE;
//	if( m_Packet.GetPacketLength() == 0 )	// 패킷이 비어 있으면
	{
		// 최초 읽은 4바이트는 패킷의 총 길이다		
		if( m_qBuffer.Size() >= sizeof(long) ) {		// 큐버퍼에 최소4바이트가 모여 패킷길이를 알수 있게 되었다
			if( m_sizePacket == 0 ) {
				m_sizePacket = m_qBuffer.PopDWORD();	// 큐에서 4바이트를 꺼낸다.
				if( XBREAK( m_sizePacket > 0xffff ) ) {
					// 정상적이지 않은 패킷이 날아왔음. 접속끊어야 함.
					m_qBuffer.clear();
//					SetbDestroy( TRUE );
					DoDisconnect();		// 즉시 접속을 끊어라.
// 					m_sizePacket = 0;
					// 에러 핸들러 호출
					OnError( xERR_PACKET_SIZE_TO_LARGE, m_sizePacket );
					return FALSE;
				}
			}
		}
		XBREAK( m_sizePacket > 0xffff );		// 뭔가 잘못된 경우다
		// 패킷 하나가 완전히 도착하면 패킷 아카이브를 만든다
		if( m_sizePacket > 0 && (DWORD)m_qBuffer.Size() >= m_sizePacket )	// 필요한 패킷양 이상이 큐에 쌓였다
		{
			if( pOutPacket->IsOverBuffer( m_sizePacket ) ) {
				pOutPacket->ReAllocBuffer( m_sizePacket + 32 );	// 안전을 위해 16바이트 더 늘임.
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
			m_sizePacket = 0;
            bRet = TRUE;
		}
	}
	//
	return bRet;
}

