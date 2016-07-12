#pragma once
#ifdef _SERVER
#include "etc/ContainerSTL.h"
#include "Network/XPacket.h"
#include "Network/XNetworkDelegate.h"
#include "XLock.h"
#include "etc/xUtil.h"
#include "XRefObj.h"

class XWnd;
class XEWinSocketSvr;
// 네트워크끼리의 한 연결을 의미하는 가상 객체
// #ifdef _VIRTUAL_REFOBJ
// class XENetworkConnection : virtual public XRefObj, public XLock//, private XDestroyObj // 소켓서버만 죽일수 있게 모두 프라이빗으로 숨김?
// #else
// class XENetworkConnection : public XRefObj, public XLock//, private XDestroyObj // 소켓서버만 죽일수 있게 모두 프라이빗으로 숨김?
// #endif
class XENetworkConnection
{
public:
	enum xtError { xERR_NOERROR, 
						xERR_PACKET_SIZE_TO_LARGE,		// 패킷 크기가 너무 크다.
						xERR_Q_IS_FULL,			// 패킷큐가 꽉찼다.
	};
// 	static float sGetmaxQueueRatio() {
// 		return s_maxQueueRatio;
// 	}
// 	static void sSetmaxQueue( float maxQueueRatio ) {
// 		s_maxQueueRatio = maxQueueRatio;
// 	}
// 	static float sGetmaxQueueSize() {
// 		return s_maxQueueSize;
// 	}
// 	static void sSetmaxQueueSize( float maxQueueSize ) {
// 		s_maxQueueSize = maxQueueSize;
// 	}

//////////////////////////////////////////////////////////////////////////
private:
//	static int s_maxQueue;			// 모든커넥션들 중에서 최고로 많이 쌓였던 큐 크기
// 	static float s_maxQueueRatio;	// 모든커넥션들 중에서 최고로 많이 쌓였던 큐크기의 비율
// 	static int s_maxQueueSize;		// 모든 커넥션들 중에서 최고로 많이 쌓였던 큐크기
	//
//	XQueue<BYTE> m_qBuffer;		// 동적배열을 이용한 큐구현
	XQueueArray<BYTE> m_qBuffer;
	int m_sizeMaxQ = 0;				// x초 동안 가장 많이 쌓인 크기
	DWORD m_sizePacket = 0;
	BOOL m__bConnected = FALSE;		// 연결됨(connect()까지 되면 TRUE가 된다.
	bool m_bDestroy = false;		// true로 죽음을 명령받음.
//////////////////////////////////////////////////////////////////////////
protected:
	ID m_idConnect = 0;		// 커넥션 아이디
	XNetworkDelegate *m_pDelegate = nullptr;
	GET_SET_ACCESSOR( XNetworkDelegate*, pDelegate );
//////////////////////////////////////////////////////////////////////////
public:
	XENetworkConnection( XNetworkDelegate *pDelegate, int sizeQueue );
	virtual ~XENetworkConnection() {}
	//////////////////////////////////////////////////////////////////////////
	GET_READONLY_ACCESSOR( ID, idConnect );
	ID getid() const {
		return m_idConnect;
	}
//   GET_READONLY_ACCESSOR( XNetworkDelegate*, pDelegate );
	GET_SET_ACCESSOR_CONST( BOOL, _bConnected );
	GET_SET_BOOL_ACCESSOR( bDestroy );
	GET_SET_ACCESSOR_CONST( int, sizeMaxQ );
	void ClearSizeMaxQ() {
		m_sizeMaxQ = 0;
	}
	void ClearConnection() {
		m_sizePacket = 0;
		m__bConnected = FALSE;
		m_bDestroy = false;
		m_qBuffer.clear();
//		std::queue<BYTE> empty;
//		std::swap( m_qBuffer, empty );
	}
	int GetSizeQueue() const {
		return m_qBuffer.size();
	}
// 	int GetmaxQueue() const {
// 		return m_qBuffer.GetMax();
// 	}
	// 받은 데이터를 큐에 밀어넣음.
	int PushBlock( BYTE *buffer, DWORD readbytes );
	BOOL PumpPacket( XPacket *pOutPacket );
// 	BOOL IsqBufferFull() { 
// 		return m_qBuffer.IsFull(); 
// 	}
	//
	// 접속을 즉시 끊을 것을 명령한다.(삭제가 아님)
	virtual void DoDisconnect() = 0;
	// 현재 끊긴상태인지 플랫폼별 방식으로 알아내어 알려주는걸 구현해야 한다.
	virtual BOOL IsDisconnected() const = 0;		// 모든 커넥션 상속자들은 자신이 현재 끊긴상태인지를 파악해서 알려줄 의무가 있다.
	virtual void OnError( xtError error, DWORD param1=0, DWORD param2=0 ) {}
//friend class XWinSocketSvr;
};

#endif // server
