#pragma once
#if defined(_CLIENT) || defined(_CONSOLE)

#include <WinSock2.h>
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "../client/XSocketClient.h"

#define _USING_WSA
namespace XENW {
};
class XWnd;
//class XWinConnection;
class XNetworkDelegate;
class XEWinSocketClient : public XESocketClient
{
	SOCKET m_Socket;
	HANDLE m_hWorkThread;			// work thread
	HANDLE m_hConnectThread;		// connect thread
	WSADATA m_WSAData; 
//	char m_cIP[16];			// 편의상 euckr버전도 가지고 있는다
	//
	void Init() {
		m_Socket = 0;
		m_hWorkThread = 0;
		m_hConnectThread = 0;
//		m_cIP[0] = 0;
	}
	void Destroy() {
		CloseAllThread();
		if( m_Socket )
			closesocket( m_Socket ); 
		m_Socket = 0;
		Sleep( 100 );		// 소켓 닫히고 this가 파괴되기전에 Work스레드에서 빠져나올시간을 조금 줬음.
	}
	void CloseAllThread( void );
protected:
	GET_ACCESSOR_CONST( SOCKET, Socket );
public:
//	XEWinSocketClient( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	XEWinSocketClient( XNetworkDelegate *pDelegate, int sizeQueue, DWORD param );
	
	virtual ~XEWinSocketClient() { Destroy(); }
	//
//	GET_ACCESSOR_CONST( const char*, cIP );
	//
	void WorkThread( void );
	void ConnectThread( void );
	//
	virtual BOOL TryConnect( void );
	BOOL CreateSocket( void );
	void SendData( const BYTE *pBuffer, int len );
	void CreateWorkThread( void );
	//
	void DoDisconnect() override;
	BOOL IsDisconnected()	override;
	void OnConnect( void ) override {
		// 핸들 안닫아주면 메모리 leak생김
		CloseConnectThread();
	}
	void CloseSocket();
//	void OnDisconnect( void ) override {}
	//////////////////////////////////////////////////////////////////////////
	// http Post 관련
	void SetHttpData(const char* baseURL);
	void ProcessHttp( const char* host, const char* page, const char* postStr, char *response );
protected:
private:
	void CloseConnectThread();
	void CloseWorkThread();
friend class XIOCPMng;
};


#endif // CLIENT
