#pragma once
// 서버에서만 쓰는 클라이언트용 소켓 객체
#include "Network/XWinNetwork.h"
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "XFramework/server/XNetworkConnection.h"
#include "Network/XPacket.h"
#include "XCrypto.h"

class XWnd;
class XNetworkDelegate;
/**
 @brief 
*/ 
class XESocketClientInServer : public XENetworkConnection
														 , public XCrypto
{
	/**
	this객체를 삭제할때 락객체를 복사해두고 삭제후 unlock을 시키려고 이렇게 함. 제대로 될진 몰겠음.
	데드락 감지가가 Lock/Unlock이 쌍으로 이뤄져야 해서.
	auto spLock = m_pSocketClient->GetspLock();
	spLock->Lock();
	SAFE_DELETE( m_pSocketClient );
	spLock->Unlock();
	*/
	std::shared_ptr<XLock> m_spLock;
// 	HANDLE m_hWorkThread;			// work thread
// 	HANDLE m_hConnectThread;		// connect thread
	XE::xThread m_thWork;
	XE::xThread m_thConnect;
//	WSADATA m_WSAData; 
// 	TCHAR m_szIP[16];		// ip같은걸 EUCKR같은걸로 갖고 있었더니 콘솔같은데 쓸때 일일히 변환해야하고 스레드 내부에서 인코딩변환하는것도 부담스럽고 해서 tchar로 갖고 있게 바꿈.
// 	char m__cIP[16];			// 편의상 euckr버전도 가지고 있는다
	_tstring m_strtIP;
	std::string m_strcIP;
	unsigned short m_Port;
	WORD m_reserved;	// 4바이트 정렬
	BYTE m_Buffer[ 1024 ];
//	BOOL m_bDummy;			// 직접 접속하는게 아니라 가상으로 시뮬레이션으로 쓸때
	BOOL m_bReadyServer;	// 서버가 패킷받을 준비가 끝남
	BOOL m_bDestroy;			// 디스커넥이 아니라 즉각삭제시사용
	BOOL m_bReconnected;		// 끊어졌다 다시 붙은 커넥션이다.
	CTimer m_timerReconnect;		// 재시도 하고 지난시간.
	DWORD m_dwConnectParam;	// 커넥트 델리게이트때 전달할 파라메터
	CTimer m_timerDelay;			// 서버로 연결되는 커넥션이 끊겼을때 딜레이 타이머
	int m_stateConnect = 0;
//	int m_maxSizeQ = 0;
//	volatile bool m_bRunConnectThread = false;
//
	void Init() {
		m_Socket = 0;
// 		m_hWorkThread = 0;
// 		m_hConnectThread = 0;
		m_Port = 0;
// 		m_szIP[0] = 0;
// 		m__cIP[0] = 0;
		m_Buffer[0] = 0;
//		m_bDummy = FALSE;
		m_bReadyServer = FALSE;
		m_bDestroy = FALSE;
		m_bReconnected = NULL;
		m_dwConnectParam = 0;
	}
protected:
	SOCKET m_Socket;
	SET_ACCESSOR( BOOL, bReconnected );
	GET_ACCESSOR_CONST( BOOL, bReadyServer );
public:
// 	XESocketClientInServer( const char *cIP
// 												, unsigned short port
// 												, XNetworkDelegate *pDelegate
// 												, int sizeQueue
// 												, DWORD param );
	XESocketClientInServer( int sizeQueue );
	virtual ~XESocketClientInServer() { Destroy(); }
	//
	virtual bool OnCreate( const std::string& strcIP
											, WORD port
											, XNetworkDelegate *pDelegate
											, DWORD param = 0 );
//	GET_SET_ACCESSOR( BOOL, bDummy );
	GET_SET_ACCESSOR( BOOL, bDestroy );
// 	GET_READONLY_ACCESSOR( LPCTSTR, szIP );
	GET_READONLY_ACCESSOR( const _tstring&, strtIP );
	GET_READONLY_ACCESSOR( const std::string&, strcIP );
	GET_READONLY_ACCESSOR( WORD, Port );
	GET_ACCESSOR_CONST( const CTimer&, timerReconnect );
	GET_ACCESSOR_CONST( BOOL, bReconnected );
	GET_ACCESSOR( std::shared_ptr<XLock>&, spLock );
//	GET_ACCESSOR_CONST( int, maxSizeQ );
	//
	void WorkThread();
	void ConnectThread();
	//
	BOOL TryConnect();
	BOOL Process();
	BOOL CreateSocket( );
	void Lock( LPCTSTR szFunc ) {
		m_spLock->Lock( szFunc );
	}
	void Unlock() {
		m_spLock->Unlock();
	}
//	void DoDisconnectConnection();
//	BOOL PumpPacket( XPacket *pOutPacket );
	void SendData( const BYTE *pBuffer, int len );
	virtual BOOL Send( XPacket& ar ) { 
		SendData( ar.GetBuffer(), ar.size() ); 
		return TRUE;
	}
	void CreateWorkThread();
	//
	virtual void DoDisconnect() override;
	virtual BOOL IsDisconnected() const override;
	virtual void OnLogined() {}
	virtual void OnDisconnect() {}
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) = 0;
	void CloseConnectThread();
	void CloseWorkThread();
	//////////////////////////////////////////////////////////////////////////
private:
	void Destroy();
	BOOL ProcessReservedPacket( DWORD idPacket, ID idKey, XPacket& p );
	void ProcessConnect();
	bool DoReconnectTry();
	void CloseAllThread();
};

