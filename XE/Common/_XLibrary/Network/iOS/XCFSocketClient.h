#pragma once

#import <sys/types.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <netdb.h>
#import <arpa/inet.h>
//#include "XList.h"
//#include "ContainerSTL.h"
#include "Network/client/XSocketClient.h"
#include "../XPacket.h"
#import <CoreFoundation/CoreFoundation.h>

// iOS CFSocket 소켓
class XCFSocketClient : public XESocketClient
{
    CFSocketRef m_Socket;
    void *m_pSocket;
//	char m_cIP[16];
//	unsigned short m_Port;
//	BOOL m_bReadyServer;
//	BOOL m_bDummy;
	//
	void Init() {
        m_pSocket = NULL;
        m_Socket = NULL;
//		m_Port = 0;
//		m_cIP[0] = 0;
//		m_bReadyServer = FALSE;
//		m_bDummy = FALSE;
	}
	void Destroy();
protected:
public:
	XCFSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	virtual ~XCFSocketClient() { Destroy(); }
    //
//	GET_SET_ACCESSOR(BOOL, bReadyServer );
	//
    void RecvCallBack( unsigned char *buffer, int readbytes );
	//
	BOOL TryConnect( void );
	BOOL Process( void );
	void DoDisconnectConnection( void );
	BOOL PumpPacket( XPacket *pOutPacket );
//	void DoDisconnect();
	void SendData( const BYTE *pBuffer, int len );
//	void Send( XPacket& ar ) { SendData( ar.GetBufferMem(), ar.size() ); }
	//
	virtual void OnConnect( void );
	virtual void OnDisconnect( void ) {}
	virtual BOOL ProcessPacket( XPacket& p ) = 0;
	// 접속을 즉시 끊을 것을 명령한다.(삭제가 아님)
	virtual void DoDisconnect( void );
	// 현재 끊긴상태인지 플랫폼별 방식으로 알아내어 알려주는걸 구현해야 한다.
	virtual BOOL IsDisconnected( void );		// 모든 커넥션 상속자들은 자신이 현재 끊긴상태인지를 파악해서 알려줄 의무가 있다.
};

