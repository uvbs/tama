#include "stdafx.h"
#include "XSessionWinsock.h"
#include "XUserMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

unsigned long __stdcall _ConnectThread( void *param );
unsigned long __stdcall _RecvThread( void *param );
unsigned long __stdcall _AcceptThread( void *param );

// ������
// client mode
XSessionWinsock::XSessionWinsock( XSessionDelegate *pDelegate, const char *cIP, unsigned short port ) 
: XSession( pDelegate )
{ 
	Init(); 
	//
	if( WSAStartup( MAKEWORD(1,1), &m_WSAData ) != 0 ) 
	{
		XALERT( "WSAStartup failed" );
		return;
	}
	//
	m_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 
	if( m_Socket == INVALID_SOCKET ) 
	{
		XALERT( "create socket failed" );
		return;
	}
	//
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(0); 
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) 
	{ 
		XALERT( "Network bind error" );
		Destroy();
		return; 
	} 
	strcpy_s( m_cIP, sizeof(m_cIP), cIP );
	m_Port = port;
	m_Mode = xCLIENT;
	//
	InitializeCriticalSection( &m_cs );
}

// server mode
XSessionWinsock::XSessionWinsock( XSessionDelegate *pDelegate, unsigned short port ) 
: XSession( pDelegate )
{
	Init(); 
	//
	if( WSAStartup( MAKEWORD(1,1), &m_WSAData ) != 0 ) 
	{
		XALERT( "WSAStartup failed" );
		return;
	}
	//
	m_Socket = socket( AF_INET, SOCK_STREAM, 0 ); 
	if( m_Socket == INVALID_SOCKET ) 
	{
		XALERT( "create socket failed" );
		return;
	}
	//
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) 
	{ 
		XALERT( "Network bind error" );
		Destroy();
		return; 
	} 
	//
	if( listen( m_Socket, SOMAXCONN ) == SOCKET_ERROR ) 
	{
		XALERT( "Network::listen error" );
		Destroy();
		return; 
	}
	//
//	m_pDelegate = pDelegate;
//	strcpy_s( m_cIP, strlen(cIP), cIP );
	m_Port = port;
	m_Mode = xSERVER;
	// �����Ϸ�, ���Ʈ �����
	if( m_pDelegate )
		m_pDelegate->DelegateFinishListen( this );
	//
	InitializeCriticalSection( &m_cs );

	DWORD idThread;
	m_hAcceptThread = CreateThread( NULL, 0, _AcceptThread, (LPVOID)this, 0, &idThread); 

}

unsigned long __stdcall _AcceptThread( void *param ) 
{
	XBREAK( param == 0 );
	XSessionWinsock *This = static_cast<XSessionWinsock*>( param );
	//
	This->AcceptThread();
	return 1;
}

void XSessionWinsock::AcceptThread( void )
{
	SOCKADDR_IN addr; 
	int addrlen; 

	while(1) { 
		addrlen = sizeof(addr); 
		m_Client = accept( m_Socket, (struct sockaddr*)&addr, &addrlen); 

		if( m_Client != INVALID_SOCKET ) 
		{ 
			sprintf_s( m_cIPClient, "%d.%d.%d.%d", 
				addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2, 
                addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4 );
			//m_PortClient = ntohs(addr.sin_port);
			// call handler
			OnAccept();
			if( m_pDelegate )
				m_pDelegate->DelegateAccept( this );
//			CallEventHandler( xON_ACCEPT );
			// �ϴ��� Ŭ�� �ϳ��ۿ� �������ϱ� �� ����
			return;	
		} 
	}
}

unsigned long __stdcall _ConnectThread( void *param ) 
{
	XBREAK( param == 0 );
	XSessionWinsock *This = static_cast<XSessionWinsock*>( param );
	//
	This->ConnectThread();
	return 1;
}

void XSessionWinsock::ConnectThread( void )
{
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(m_Port); 
	addr.sin_addr.S_un.S_addr = inet_addr(m_cIP); 
	while(1)
	{
		if( connect( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == 0 ) 
		{
			OnConnect();
			if( m_pDelegate )
				m_pDelegate->DelegateConnect( this );
//			CallEventHandler( xON_CONNECT );		// virtual �ڵ鷯 ȣ��
			return;			// ������ ����
		}
		// Ŀ��Ʈ�� ����������
		Sleep( 1000 );		// 1�ʽ��� �ٽ� �õ�
		// Ÿ�Ӿƿ�ī���� �ø��� Ÿ�Ӿƿ��� Ư�� �ڵ鷯 ȣ�� �����Ұ�
	}
}

BOOL XSessionWinsock::TryConnect( void )
{
	// Ŀ��Ʈ ������ ����
	DWORD idThread;
	m_hConnectThread = CreateThread( NULL, 0, _ConnectThread, (LPVOID)this, 0, &idThread );
	return TRUE;
}

void XSessionWinsock::Send( const BYTE *pBuffer, int len )
{
	if( m_Mode == xCLIENT )
		send( m_Socket, (const char *)pBuffer, len, 0 );
	else
		send( m_Client, (const char *)pBuffer, len, 0 );		// ������忡�� Ŭ�󿡰� ������
}

unsigned long __stdcall _RecvThread( void *param ) 
{
	XBREAK( param == 0 );
	XSessionWinsock *This = static_cast<XSessionWinsock*>( param );
	//
	This->RecvThread();
	return 1;
}

extern int g_sizeQBuffer;
void XSessionWinsock::RecvThread( void ) 
{
//	int sizePacket = 0;
//	int totalRead = 0;
	while(1)
	{
		// ť�� ���� ���۰� ���̻� ������ �� ����
		if( m_qBuffer.IsFull() )		
			continue;
		// ���Ͽ��� ��Ŷ�� �о� ���ۿ� ����
		int readbytes; 
		if( m_Mode == xCLIENT )
			readbytes = recv( m_Socket, (char *)m_Buffer, sizeof(m_Buffer), 0 );		// ����ũ�⸸ŭ ��Ŷ�� �о�´�
		else
			readbytes = recv( m_Client, (char *)m_Buffer, sizeof(m_Buffer), 0 );		// ����ũ�⸸ŭ ��Ŷ�� �о�´�
		XBREAK( readbytes > (int)sizeof(m_Buffer) );
		if( readbytes <= 0 )
		{
			CONSOLE( "network error(%d)", WSAGetLastError() );
			m_bDisconnect = TRUE;
			return;
		}
		// ������ ������ ť�� ����
		EnterCriticalSection( &m_cs );
		m_qBuffer.PushBlock( m_Buffer, readbytes );	// ����ȭ�� ���� ����. ť�� �ϳ��� �������ʰ� �����̷� ����
		g_sizeQBuffer = m_qBuffer.Size();
		LeaveCriticalSection( &m_cs );
	}
}

// ť ���ۿ��� ���ο� ��Ŷ�� �̾Ƴ���.. ��Ŷ�� �������� �ʾ����� FALSE�� �����Ѵ�
BOOL XSessionWinsock::PumpPacket( XPacket *pOutPacket )
{
	EnterCriticalSection( &m_cs );
	BOOL bRet = XSession::PumpPacket( pOutPacket );
	LeaveCriticalSection( &m_cs );
	return bRet;
}

// ���Ͽ� ������ �����ߴ�
void XSessionWinsock::OnConnect( void )
{
	DWORD idThread;
	// recv ������ ����
	m_hRecvThread = CreateThread( NULL, 0, _RecvThread, (LPVOID)this, 0, &idThread );
	// ���⼭ Ŀ��Ʈ ������� �����ؾ� �ϴ°� �ƴѰ�?
}

// Ŭ���̾�Ʈ�� �����ؿԴ�
void XSessionWinsock::OnAccept( void )
{
	DWORD idThread;
	// recv ������ ����
	m_hRecvThread = CreateThread( NULL, 0, _RecvThread, (LPVOID)this, 0, &idThread );
}





/*
pUser->Send��������¼��();

XUser::Send��¼����¼��()
{
	ar << ����Ÿ;
	m_pSession->Send( ar );
}

USERMNG->Process()

XUser::Process()
{
	while( m_pSession->PumpPacket( &packet ) )
	{
		ProcessPacket( packet );
	}
}





*/