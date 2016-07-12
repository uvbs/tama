#pragma once
#include "etc/ContainerSTL.h"
#include "Network/XPacket.h"
#include "Network/XNetworkDelegate.h"
#include "XLock.h"
#include "etc/xUtil.h"
#include "XRefObj.h"

#ifdef _SERVER
#error "클라전용입니다."
#endif

class XWnd;
class XWinSocketSvr;
// 네트워크끼리의 한 연결을 의미하는 가상 객체
class XENetworkConnection : public XRefObj, public XLock//, private XDestroyObj // 소켓서버만 죽일수 있게 모두 프라이빗으로 숨김?
{
public:
	enum xtError { xERR_NOERROR, 
						xERR_PACKET_SIZE_TO_LARGE,		// 패킷 크기가 너무 크다.
						xERR_Q_IS_FULL,			// 패킷큐가 꽉찼다.
	};
	static int sGetmaxQueue( void ) {
		return s_maxQueue;
	}
	static void sSetmaxQueue( int maxQueue ) {
		s_maxQueue = maxQueue;
	}
	static void sClearmaxQueue() {
		s_maxQueue = 0;
	}

//////////////////////////////////////////////////////////////////////////
private:
	static int s_maxQueue;			// 모든커넥션들 중에서 최고로 많이 쌓였던 큐 크기
	//
	XQueueArray<BYTE> m_qBuffer;		// 동적배열을 이용한 큐구현
	DWORD m_sizePacket;
	BOOL m__bConnected;		// 연결됨(connect()까지 되면 TRUE가 된다.
	bool m_bDestroy = false;		// true로 죽음을 명령받음.
//	BOOL m_bDisconnected;	// 연결끊김
//////////////////////////////////////////////////////////////////////////
protected:
	ID m_idConnect;		// 커넥션 아이디
	XNetworkDelegate *m_pDelegate;
	#ifdef _VER_IOS
public:
	//이제 디스커넥 명령을 받으면 iOS쪽에서 직접 소켓을 닫고 플래그는 이용하지 않는다.
//		SET_ACCESSOR( BOOL, bDisconnected );    // iOS의 경우 콜백에서 호출해야해서....
protected:
	#else
	//	SET_ACCESSOR( BOOL, bDisconnected );
	#endif
//////////////////////////////////////////////////////////////////////////
public:
	XENetworkConnection( XNetworkDelegate *pDelegate, int sizeQueue ) {
		m_idConnect = XE::GenerateID();
		m_pDelegate = pDelegate;
		m_sizePacket = 0;
		m__bConnected = FALSE;
//		m_bDisconnected = FALSE;
		m_qBuffer.Create( sizeQueue );	// 이제 모두 동적 할당큐로 바뀜
	}
	virtual ~XENetworkConnection() {}
	//////////////////////////////////////////////////////////////////////////
	GET_READONLY_ACCESSOR( ID, idConnect );
	GET_SET_LOCK_ACCESSOR( bool, bDestroy  );
#ifdef _CLIENT
	GET_SET_ACCESSOR( XNetworkDelegate*, pDelegate );
#else
	GET_READONLY_ACCESSOR( XNetworkDelegate*, pDelegate );
#endif
	GET_SET_LOCK_ACCESSOR( BOOL, _bConnected );
//	GET_ACCESSOR( BOOL, bDisconnected );
	void InitConnection() {
		XLOCK_OBJ;
		m__bConnected = FALSE;
		ClearPacketQ();
	}
	void ClearPacketQ() {
		XLOCK_OBJ;
		m_sizePacket = 0;
		m_qBuffer.clear();
	}
	int GetSizeQueue( void ) {
		XLOCK_OBJ;
		return m_qBuffer.size();
	}
	int GetmaxQueue( void ) {
		XLOCK_OBJ;
		return m_qBuffer.GetMax();
	}
	float GetmaxQueueRate() {
		XLOCK_OBJ;
		return (float)s_maxQueue / m_qBuffer.GetMax();
	}
	// 받은 데이터를 큐에 밀어넣음.
	int PushBlock( BYTE *buffer, DWORD readbytes ) {
		XLOCK_OBJ;
		int bytePushed = m_qBuffer.PushBlock( buffer, readbytes );	// 최적화를 위해 만듬. 큐에 하나씩 보내지않고 뭉탱이로 보냄
		if( m_qBuffer.size() > s_maxQueue )
			s_maxQueue = m_qBuffer.size();
		return bytePushed;
	}
	BOOL PumpPacket( XPacket *pOutPacket );
	BOOL IsqBufferFull( void ) { 
		XLOCK_OBJ;
		return m_qBuffer.IsFull(); 
	}
	//
	// 접속을 즉시 끊을 것을 명령한다.(삭제가 아님)
	virtual void DoDisconnect( void ) = 0;
	BOOL IsConnected( void ) {
		return ! IsDisconnected();
	}
	// 현재 끊긴상태인지 플랫폼별 방식으로 알아내어 알려주는걸 구현해야 한다.
	virtual BOOL IsDisconnected( void ) = 0;		// 모든 커넥션 상속자들은 자신이 현재 끊긴상태인지를 파악해서 알려줄 의무가 있다.
	virtual void OnError( xtError error, DWORD param1=0, DWORD param2=0 ) {}
//friend class XWinSocketSvr;
};

