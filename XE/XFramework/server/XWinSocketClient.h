#pragma once
// 서버에서만 쓰는 클라이언트용 소켓 객체
#ifdef _SERVER
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "server/XNetworkConnection.h"
#include "Network/XPacket.h"
#include "XCrypto.h"

#define _USING_WSA
namespace XENW {
};
class XWnd;
//class XWinConnection;
class XNetworkDelegate;
// 윈도우 소켓 서버용
class XEWinSocketClient : public XENetworkConnection, public XCrypto
{
	HANDLE m_hWorkThread;			// work thread
	HANDLE m_hConnectThread;		// connect thread
	WSADATA m_WSAData; 
	TCHAR m_szIP[16];		// ip같은걸 EUCKR같은걸로 갖고 있었더니 콘솔같은데 쓸때 일일히 변환해야하고 스레드 내부에서 인코딩변환하는것도 부담스럽고 해서 tchar로 갖고 있게 바꿈.
	char m__cIP[16];			// 편의상 euckr버전도 가지고 있는다
	unsigned short m_Port;
	WORD m_reserved;	// 4바이트 정렬
	BYTE m_Buffer[ 1024 ];
	BOOL m_bDummy;			// 직접 접속하는게 아니라 가상으로 시뮬레이션으로 쓸때
	BOOL m_bReadyServer;	// 서버가 패킷받을 준비가 끝남
	BOOL m_bDestroy;			// 디스커넥이 아니라 즉각삭제시사용
	BOOL m_bReconnected;		// 끊어졌다 다시 붙은 커넥션이다.
	CTimer m_timerReconnect;		// 재시도 하고 지난시간.
	DWORD m_dwConnectParam;	// 커넥트 델리게이트때 전달할 파라메터
//
	void Init() {
		m_Socket = 0;
		m_hWorkThread = 0;
		m_hConnectThread = 0;
		m_Port = 0;
		m_szIP[0] = 0;
		m__cIP[0] = 0;
		m_Buffer[0] = 0;
		m_bDummy = FALSE;
		m_bReadyServer = FALSE;
		m_bDestroy = FALSE;
		m_bReconnected = NULL;
		m_dwConnectParam = 0;
	}
	void Destroy() {
		CloseAllThread();
		if( m_Socket )
			closesocket( m_Socket ); 
		m_Socket = 0;
		Sleep( 100 );		// 소켓 닫히고 this가 파괴되기전에 Work스레드에서 빠져나올시간을 조금 줬음.
//		DeleteCriticalSection( &m_cs );
	}
	void CloseAllThread( void ) {
		SAFE_CLOSE_HANDLE( m_hConnectThread );
		SAFE_CLOSE_HANDLE( m_hWorkThread );
	}
protected:
	SOCKET m_Socket;
	SET_LOCK_ACCESSOR( BOOL, bReconnected );
	GET_LOCK_ACCESSOR( BOOL, bReadyServer );
public:
	XEWinSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	virtual ~XEWinSocketClient() { Destroy(); }
	//
	GET_SET_ACCESSOR( BOOL, bDummy );
	GET_SET_LOCK_ACCESSOR( BOOL, bDestroy );
	GET_READONLY_ACCESSOR( LPCTSTR, szIP );
	GET_READONLY_ACCESSOR( WORD, Port );
	GET_LOCK_ACCESSOR( CTimer&, timerReconnect );
	GET_LOCK_ACCESSOR( BOOL, bReconnected );
	//
	void WorkThread( void );
	void ConnectThread( void );
	//
	BOOL TryConnect( void );
	BOOL Process( void );
	BOOL CreateSocket( );
//	BOOL DoReconnect( float secRetry, const char *cIP=NULL, unsigned short port=0 );
//	void DoDisconnectConnection( void );
//	BOOL PumpPacket( XPacket *pOutPacket );
	void SendData( const BYTE *pBuffer, int len );
	virtual BOOL Send( XPacket& ar ) { 
		SendData( ar.GetBuffer(), ar.size() ); 
		return TRUE;
	}
	void CreateWorkThread( void );
	//
	virtual void DoDisconnect();
	virtual BOOL IsDisconnected( void )	{
		Lock();
		BOOL bRet = (m_Socket == 0 ) ? TRUE : FALSE;
		Unlock();
		return bRet;
	}
	virtual void OnConnect( void ) {}
	virtual void OnDisconnect( void ) {}
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) = 0;
	BOOL ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p );
	//////////////////////////////////////////////////////////////////////////
};

#endif // server