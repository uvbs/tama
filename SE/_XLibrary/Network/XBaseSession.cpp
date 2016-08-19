#include "stdafx.h"
#include "XBaseSession.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 큐 버퍼에서 새로운 패킷을 뽑아낸다.. 패킷이 도착하지 않았으면 FALSE를 리턴한다
BOOL XSession::PumpPacket( XPacket *pOutPacket )
{
    BOOL bRet = FALSE;
//	if( m_Packet.GetPacketLength() == 0 )	// 패킷이 비어 있으면
	{
		// 최초 읽은 4바이트는 패킷의 총 길이다		
		if( m_qBuffer.Size() >= sizeof(long) )		// 큐버퍼에 최소4바이트가 모여 패킷길이를 알수 있게 되었다
			if( m_sizePacket == 0 )
				m_sizePacket = m_qBuffer.PopDWORD();	// 큐에서 4바이트를 꺼낸다.
		XBREAK( m_sizePacket < 0 || m_sizePacket > 0xffff );		// 뭔가 잘못된 경우다
		// 패킷 하나가 완전히 도착하면 패킷 아카이브를 만든다
		if( m_sizePacket > 0 && m_qBuffer.Size() >= m_sizePacket )	// 필요한 패킷양 이상이 큐에 쌓였다
		{
			m_qBuffer.PopBlock( pOutPacket->GetBufferPacket(), pOutPacket->GetMaxBufferSize(), m_sizePacket );	// 큐에서 sizePacket만큼 pop시킨다
			pOutPacket->SetPacketLength( m_sizePacket );	// 완성된 패킷이 준비되었다
#ifdef WIN32
#ifdef _DEBUG
/*			int size = m_sizePacket;
			CString str;
			for( int i = 0; i < size; ++i )
			{
				str.Format( _T("0x%02x,"), m_Packet.GetBufferPacket()[i] );
				::OutputDebugString( str );		// TRACE로 한글출력이 안되서 이걸로 바꿈
			}
			::OutputDebugString( _T("\n") );
			*/
#endif
#endif
			m_sizePacket = 0;
            bRet = TRUE;
		}
	}
	return bRet;
}

void XSession::Process( void )
{
	if( m_pDelegate )
	{
		if( m_bDisconnect )		// 세션이 끊겼다.
		{
			m_pDelegate->DelegateDisconnect( this );
			m_bDisconnect = FALSE;
		}
	}
}
// 세션에 쌓인 패킷들을 분석해서 각자의 핸들러를 불러준다
/*void XSession::Process( void )
{
    // 큐버퍼에서 패킷을 뽑아낸다
    PumpPacket();
    // virtual로 패킷을 처리한다
	while( m_Packet.GetPacketLength() > 0 )		// 패킷이 준비되었다
	{
		if( ProcessPacket( m_Packet ) == FALSE )			// virtual. 하위클래스에 이 패킷을 사용하라고 던져줌
			Disconnect();			// 패킷분석에 실패하여 연결을 끊음
		m_Packet.Clear();						// 패킷 클리어
		if( IsConnected() )
		{
			PumpPacket();		// 버퍼에서 새로운 패킷을 뽑아낸다
		}
	} 
} */

