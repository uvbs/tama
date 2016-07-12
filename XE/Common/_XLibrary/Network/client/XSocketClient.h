
#pragma once
#include "XNetworkConnection.h"
#include "../XPacket.h"
#include "XCrypto.h"

////////////////////////////////////////////////////////////////
class XESocketClient : public XENetworkConnection, public XCrypto
{
// 	TCHAR m_szIP[16];		// ip같은걸 EUCKR같은걸로 갖고 있었더니 콘솔같은데 쓸때 일일히 변환해야하고 스레드 내부에서 인코딩변환하는것도 부담스럽고 해서 tchar로 갖고 있게 바꿈.
	std::string m_strcIP;
	BYTE m_Buffer[ 1024 ];
	BOOL m_bDummy;			// 직접 접속하는게 아니라 가상으로 시뮬레이션으로 쓸때
	BOOL m_bReadyServer;	// 서버가 패킷받을 준비가 끝남
	BOOL m_bDestroy;			// 디스커넥이 아니라 즉각삭제시사용
	BOOL m_bReconnected;		// 끊어졌다 다시 붙은 커넥션이다.
	CTimer m_timerReconnect;		// 재시도 하고 지난시간.
	unsigned short m_Port;
	BYTE m_reserved[2];		// 4바이트 정렬
//	DWORD m_dwConnectParam;	// 커넥트 델리게이트때 전달할 파라메터
	bool m_bDoDisconnect = false;		// DoDisconnect()로 종료시킬땐 이플래그를 켜놓고 cbOnDisconnectByExternal이 호출되지 않도록 한다.
	void Init() {
		m_Port = 0;
//		m_szIP[0] = 0;
		m_Buffer[0] = 0;
		m_bDummy = FALSE;
		m_bReadyServer = FALSE;
		m_bDestroy = FALSE;
		m_bReconnected = FALSE;
//		m_dwConnectParam = 0;
	}
	void Destroy();
protected:
	std::string m_cURL;
public:
// 	XESocketClient( const char *cIP, 
// 					unsigned short port, 
// 					XNetworkDelegate *pDelegate, 
// 					int sizeQueue,
// 					DWORD dwParam );	// 접속 델리게이트때 전달할 파라메터
	XESocketClient( XNetworkDelegate *pDelegate
								, int sizeQueue
								, DWORD dwParam );	// 접속 델리게이트때 전달할 파라메터
	virtual ~XESocketClient() { Destroy(); }
	//
	SET_LOCK_ACCESSOR( BOOL, bReconnected );
	GET_LOCK_ACCESSOR( BOOL, bReadyServer );
	GET_SET_ACCESSOR( BOOL, bDummy );
	GET_SET_LOCK_ACCESSOR( BOOL, bDestroy );
//	GET_LOCK_ACCESSOR( LPCTSTR, szIP );
	GET_LOCK_ACCESSOR( const std::string&, strcIP );
	GET_LOCK_ACCESSOR( WORD, Port );
	GET_LOCK_ACCESSOR( CTimer&, timerReconnect );
	GET_LOCK_ACCESSOR( BOOL, bReconnected );
//	GET_SET_ACCESSOR( DWORD, dwConnectParam );
	// recv()로 받을 버퍼의 포인터를 리턴받고 그 사이즈를 pOutSize에 담아준다.
	BYTE* GetRecvBuff( int *pOutSize ) {
		XBREAK( pOutSize == NULL );
		*pOutSize = sizeof( m_Buffer );
		return m_Buffer;
	}
	// 다수의 워커스레드 사용시
	void tRecvData( DWORD readbytes );
	//
	virtual BOOL CreateSocket( void ) = 0;
	virtual BOOL TryConnect( void );
//	virtual BOOL DoReconnect( float secRetry, const char *cIP=NULL, unsigned short port=0 );
	void SetIpAndPort( const char *cIP, WORD port );
	void SetIpAndPort( const std::string& strIP, WORD port ) {
		SetIpAndPort( strIP.c_str(), port );
	}
	void ClearIPAndPort();
#ifdef WIN32
// 	void SetIpAndPort( LPCTSTR szIP, WORD port ) {
// 		XLOCK_OBJ;
// 		if( XE::IsHave( szIP ) ) {
// 			_tcscpy_s( m_szIP, szIP );
// 			XBREAK( port == 0 );
// 			m_Port = port;
// 		} else {
// 			m_szIP[ 0 ] = 0;
// 			m_Port = 0;
// 		}
// 	}
// 	void SetIpAndPort( const _tstring& strIP, WORD port ) {
// 		SetIpAndPort( strIP.c_str(), port );
// 	}
#endif // WIN32
// 	BOOL DoReconnect2( const char *cIP, unsigned short port, DWORD param );
// 	BOOL DoReconnect2( DWORD param );
	virtual void DoDisconnect() = 0;
	BOOL Process( void );
	BOOL ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p );
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) = 0;
	virtual void SendData( const BYTE *pBuffer, int len ) = 0;
	virtual BOOL Send( XPacket& ar );
	virtual void OnConnect( void ) {}
	// 외부요인에 의해 접속이 끊김. 스레드에서 호출됨.
	virtual void cbOnDisconnectByExternal() {}
	void ClearConnection();
protected:
	GET_SET_ACCESSOR( bool, bDoDisconnect );
};
