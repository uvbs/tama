#pragma once
#include "ContainerSTL.h"

#include <winsock.h>
#include <stdio.h>
#include "XBaseSession.h"
#include "XPacket.h"

class XSessionWinsock : public XSession
{
	unsigned short m_Port;
	char m_cIP[16];
	char m_cIPClient[16];
	SOCKET m_Socket;
	SOCKET m_Client;		// server모드 일때만
	WSADATA m_WSAData;
	BYTE m_Buffer[ 1024 ];
	HANDLE m_hRecvThread; 
	HANDLE m_hConnectThread; 
	HANDLE m_hAcceptThread;
	CRITICAL_SECTION m_cs;
	void Init() {
		m_Port = 0;
		XCLEAR_ARRAY( m_cIP );
		XCLEAR_ARRAY( m_cIPClient );
		m_Socket = 0;
		m_hRecvThread = 0;
		m_hConnectThread = 0;
		m_hAcceptThread = 0;
		XCLEAR_ARRAY( &m_WSAData );
	}
	void Destroy() {
		// 쓰레드 핸들 해제 넣을것
		CloseAllThread();
		closesocket( m_Socket ); 
		DeleteCriticalSection( &m_cs );
	}
	void CloseAllThread( void ) {
		if( m_hRecvThread )
			CloseHandle( m_hRecvThread );
		if( m_hConnectThread )
			CloseHandle( m_hConnectThread );
		if( m_hAcceptThread )
			CloseHandle( m_hAcceptThread );
	}
public:
	XSessionWinsock( XSessionDelegate *pDelegate, const char *cIP, unsigned short port );
	XSessionWinsock( XSessionDelegate *pDelegate, unsigned short port );
	virtual ~XSessionWinsock() { Destroy(); }
	//
	BOOL IsConnected( void ) {
		return (m_Socket > 0)? TRUE : FALSE;
	}
	// thread
	void ConnectThread( void );
	void RecvThread( void );
	void AcceptThread( void );
	//
	BOOL TryConnect( void );
	void Send( const BYTE *pBuffer, int len );
	void Disconnect( void ) {
		closesocket( m_Socket ); 
		m_Socket = 0;
		CloseAllThread();
	}
	BOOL PumpPacket( XPacket *pOutPacket );
	// handler
	void OnConnect( void );
	void OnAccept( void );
	//

};



