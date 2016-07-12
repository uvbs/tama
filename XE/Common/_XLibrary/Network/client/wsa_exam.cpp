// OverlappedServer_1.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 512

// ���� ���� ������ ���� ����ü
typedef struct
{
	WSAOVERLAPPED overlapped;
	SOCKET        sock;
	char        recvBuffer[ MAX_BUFFER_SIZE + 1 ];
	int                recvBytes;
	int                sendBytes;
	WSABUF        wsabuf;
} SOCKET_INFO;

int g_nTotalSockets = 0;

SOCKET_INFO*                g_aSocketInfoArray[ WSA_MAXIMUM_WAIT_EVENTS ];
WSAEVENT                        g_aEventArray[ WSA_MAXIMUM_WAIT_EVENTS ];
CRITICAL_SECTION        g_CSection;
BOOL                                g_bThreadEnd = FALSE;

// ���� ����� ������ �Լ�
DWORD WINAPI WorkerThread( LPVOID pParam );



// ���� ���� �Լ�
BOOL AddSocketInfo( SOCKET clientSocket );
void RemoveSocketInfo( int nIndex );



// ���� ��� �Լ�
void err_quit( char* msg );
void err_display( char* msg );
void err_display( int errCode );



int _tmain( int argc, _TCHAR* argv[] )
{
	int retValue;
	InitializeCriticalSection( &g_CSection );



	// ���� �ʱ�ȭ
	WSADATA wsa;
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsa ) != 0 )
		return -1;

	// socket()
	SOCKET listenSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( listenSocket == INVALID_SOCKET )
		err_quit( "socket()" );



	// bind()
	SOCKADDR_IN serverAddr;
	memset( &serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( 5001 );
	serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	retValue = bind( listenSocket, (SOCKADDR*)&serverAddr, sizeof( serverAddr ) );
	if( retValue == SOCKET_ERROR )
		err_quit( "bind()" );



	//listen()
	retValue = listen( listenSocket, SOMAXCONN );
	if( retValue == SOCKET_ERROR )
		err_quit( "listen()" );



	// ����(dummy) �̺�Ʈ ��ü ����
	WSAEVENT hEvent = WSACreateEvent();
	if( hEvent == WSA_INVALID_EVENT )
		err_quit( "WSACreateEvent()" );

	g_aEventArray[ g_nTotalSockets++ ] = hEvent;



	// ������ ����
	DWORD dwThreadID;
	HANDLE hThread = CreateThread( NULL, 0, WorkerThread, NULL, 0, &dwThreadID );
	if( hThread == NULL )
		return -1;



	// ��ſ� ����� ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int nAddrLength;
	DWORD dwRecvBytes;
	DWORD dwFlags;

	while( 1 )
	{
		// accept()
		nAddrLength = sizeof( clientAddr );
		clientSocket = accept( listenSocket, (SOCKADDR*)&clientAddr, &nAddrLength );
		if( clientSocket == INVALID_SOCKET )
		{
			err_display( "connect()" );
			continue;
		}



		printf( "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
			inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );



		// ���� ����Ʈ �߰�
		if( AddSocketInfo( clientSocket ) == FALSE )
		{
			closesocket( clientSocket );
			printf( "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
				inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
			continue;
		}



		// �ޱ� ���·� ����
		SOCKET_INFO* pSocketInfo = g_aSocketInfoArray[ g_nTotalSockets - 1 ];
		dwFlags = 0;
		int retValue = WSARecv( pSocketInfo->sock, &( pSocketInfo->wsabuf ), 1, &dwRecvBytes,
			&dwFlags, &( pSocketInfo->overlapped ), NULL );
		if( retValue == SOCKET_ERROR )
		{
			if( WSAGetLastError() != WSA_IO_PENDING )
			{
				err_display( "WSARecv()" );
				RemoveSocketInfo( g_nTotalSockets - 1 );
				continue;
			}
		}



		// �̺�Ʈ ���� ������ ���� ������ �̺�Ʈ �迭 �缳�� ��û
		if( WSASetEvent( g_aEventArray[ 0 ] ) == FALSE )
		{
			err_display( "WSASetEvent()" );
			break;
		}
	}

	g_bThreadEnd = TRUE;
	WSASetEvent( g_aEventArray[ 0 ] );
	WaitForSingleObject( hThread, INFINITE );



	// ���� ����
	WSACleanup();
	DeleteCriticalSection( &g_CSection );
	return 0;
}

DWORD WINAPI WorkerThread( LPVOID pParam )
{
	int retValue;
	DWORD index;
	SOCKADDR_IN clientAddr;
	int nAddrLength;
	DWORD cbTransferred, dwFlags, dwSendBytes, dwRecvBytes;

	while( 1 )
	{
		// �̺�Ʈ ������
		index = WSAWaitForMultipleEvents( g_nTotalSockets, g_aEventArray, FALSE, WSA_INFINITE, FALSE );
		if( index == WSA_WAIT_FAILED )
		{
			err_display( "WSAWaitForMultipleEvents()" );
			continue;
		}

		index -= WSA_WAIT_EVENT_0;



		// �̺�Ʈ �缳��
		WSAResetEvent( g_aEventArray[ index ] );
		if( index == 0 ) // ���� 0��° �ε����� ��� �̺�Ʈ �迭 �缳�� ��û�� �� ����
		{
			if( g_bThreadEnd == TRUE )
				break;

			continue;
		}



		// Ŭ���̾�Ʈ ���� ���
		SOCKET_INFO* pSocketInfo = g_aSocketInfoArray[ index ];
		nAddrLength = sizeof( clientAddr );
		getpeername( pSocketInfo->sock, (SOCKADDR*)&clientAddr, &nAddrLength );



		// �񵿱� ����� ��� Ȯ��
		retValue = WSAGetOverlappedResult( pSocketInfo->sock, &( pSocketInfo->overlapped ),
			&cbTransferred, FALSE, &dwFlags );
		if( retValue == FALSE || cbTransferred == 0 )
		{
			if( retValue == FALSE )
				err_display( "WSAGetOverlappedResult()" );

			RemoveSocketInfo( index );
			printf( "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n",
				inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
			continue;
		}



		// ������ ���۷� ����
		if( pSocketInfo->recvBytes == 0 )
		{
			pSocketInfo->recvBytes = cbTransferred;
			pSocketInfo->sendBytes = 0;
			pSocketInfo->recvBuffer[ cbTransferred ] = '\0';
			printf( "[TCP/%s:%d] %s\n", inet_ntoa( clientAddr.sin_addr ),
				ntohs( clientAddr.sin_port ), pSocketInfo->recvBuffer );
		}
		else
		{
			pSocketInfo->sendBytes += cbTransferred;
		}

		if( pSocketInfo->recvBytes > pSocketInfo->sendBytes )
		{
			memset( &( pSocketInfo->overlapped ), 0, sizeof( pSocketInfo->overlapped ) );
			pSocketInfo->overlapped.hEvent = g_aEventArray[ index ];
			pSocketInfo->wsabuf.buf = pSocketInfo->recvBuffer + pSocketInfo->sendBytes;
			pSocketInfo->wsabuf.len = pSocketInfo->recvBytes - pSocketInfo->sendBytes;

			retValue = WSASend( pSocketInfo->sock, &( pSocketInfo->wsabuf ), 1, &dwSendBytes,
				0, &( pSocketInfo->overlapped ), NULL );

			if( retValue == SOCKET_ERROR )
			{
				if( WSAGetLastError() != WSA_IO_PENDING )
					err_display( "WSASend()" );
				continue;
			}
		}
		else
		{
			pSocketInfo->recvBytes = 0;



			// ������ �ޱ�
			memset( &( pSocketInfo->overlapped ), 0, sizeof( pSocketInfo->overlapped ) );
			pSocketInfo->overlapped.hEvent = g_aEventArray[ index ];
			pSocketInfo->wsabuf.buf = pSocketInfo->recvBuffer;
			pSocketInfo->wsabuf.len = MAX_BUFFER_SIZE;

			dwFlags = 0;
			retValue = WSARecv( pSocketInfo->sock, &( pSocketInfo->wsabuf ), 1, &dwRecvBytes,
				&dwFlags, &( pSocketInfo->overlapped ), NULL );
			if( retValue == SOCKET_ERROR )
			{
				if( WSAGetLastError() != WSA_IO_PENDING )
					err_display( "WSARecv()" );
				continue;
			}
		}
	}

	return 0;
}

BOOL AddSocketInfo( SOCKET clientSocket )
{
	EnterCriticalSection( &g_CSection );



	if( g_nTotalSockets >= WSA_MAXIMUM_WAIT_EVENTS )
	{
		printf( "[����] ���� ������ �߰��� �� �����ϴ�.\n" );
		return FALSE;
	}

	SOCKET_INFO* pSocketInfo = new SOCKET_INFO;
	if( pSocketInfo == NULL )
	{
		printf( "[����] �޸𸮰� �����մϴ�.\n" );
		return FALSE;
	}

	WSAEVENT hEvent = WSACreateEvent();
	if( hEvent == WSA_INVALID_EVENT )
	{
		err_display( "WSACreateEvent()" );
		return FALSE;
	}

	memset( &pSocketInfo->overlapped, 0, sizeof( pSocketInfo->overlapped ) );
	pSocketInfo->overlapped.hEvent = hEvent;
	pSocketInfo->sock = clientSocket;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;
	pSocketInfo->wsabuf.buf = pSocketInfo->recvBuffer;
	pSocketInfo->wsabuf.len = MAX_BUFFER_SIZE;

	g_aSocketInfoArray[ g_nTotalSockets ] = pSocketInfo;
	g_aEventArray[ g_nTotalSockets ] = hEvent;
	g_nTotalSockets++;

	LeaveCriticalSection( &g_CSection );
	return TRUE;
}

void RemoveSocketInfo( int nIndex )
{
	EnterCriticalSection( &g_CSection );

	SOCKET_INFO* pInfo = g_aSocketInfoArray[ nIndex ];

	closesocket( pInfo->sock );
	delete pInfo;
	WSACloseEvent( g_aEventArray[ nIndex ] );

	for( int i = nIndex; i < g_nTotalSockets; ++i )
	{
		g_aSocketInfoArray[ i ] = g_aSocketInfoArray[ i + 1 ];
		g_aEventArray[ i ] = g_aEventArray[ i + 1 ];
	}

	g_nTotalSockets--;

	LeaveCriticalSection( &g_CSection );
}

void err_quit( char* msg )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
	exit( -1 );
}

void err_display( char* msg )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf( "[%s] %s\n", msg, (LPCTSTR)lpMsgBuf );
	LocalFree( lpMsgBuf );
}

void err_display( int errCode )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errCode,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf( "[����] %s", (LPCTSTR)lpMsgBuf );
	LocalFree( lpMsgBuf );
}