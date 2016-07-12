#pragma once
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "Network/client/XNetworkConnection.h"
#include "Network/XPacket.h"
#include "XLock.h"
#include "../client/XSocketClient.h"

class XWnd;
// ¿©µµøÏ º“ƒœ º≠πˆøÎ
class XBSDSocketClient : public XESocketClient
{
	int m_Socket;
	pthread_t m_hWorkThread;
	pthread_t m_hConnectThread;
//	BOOL m_bConnectThread;
//	BOOL m_bWorkThread;
//	char m_cIP[ 16 ];
	//
	void Init() {
		m_Socket = 0;
//		m_cIP[ 0 ] = 0;
// 		m_bConnectThread = FALSE;
// 		m_bWorkThread = FALSE;
	}
	void Destroy() {
		CloseAllThread();
		CloseSocket();
		sleep(1);
	}
	void CloseAllThread( void );
	void CloseSocket();
protected:
public:
//	XBSDSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	XBSDSocketClient( XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	virtual ~XBSDSocketClient() { Destroy(); }
	//
//	LPCTSTR GetszIP( void ) {
//		return m_cIP;
//	}
	//
	void WorkThread( void );
	void ConnectThread( void );
	//
	BOOL CreateSocket( void );
//	BOOL DoReconnect( float secRetry, const char *cIP=NULL, unsigned short port=0 );
	BOOL TryConnect( void );
//	BOOL Process( void );
	void DoDisconnectConnection( void );
//	BOOL PumpPacket( XPacket *pOutPacket );
//	void DoDisconnect();
	void SendData( const BYTE *pBuffer, int len );
	void CreateWorkThread( void );
	//
	void CloseConnectThread();
	void CloseWorkThread();
	void OnConnect() override {
		// pthread는 핸들형태가 아니라 닫을필요 없음(닫을수도 없음)
	}
//	virtual BOOL ProcessPacket( DWORD idPacket, XPacket& p ) {return 1;}
	virtual void DoDisconnect( void );
	virtual BOOL IsDisconnected( void );

	// http Post 관련
	void SetHttpData(const char* baseURL);
	void ProcessHttp( const char* host, const char* page, const char* postStr, char *response );
protected:
	std::string m_cURL;
};

#endif // not WIN32