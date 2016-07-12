// OverlappedServer_1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 512

// 소켓 정보 저장을 위한 구조체
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

// 소켓 입출력 스레드 함수
DWORD WINAPI WorkerThread( LPVOID pParam );



// 소켓 관리 함수
BOOL AddSocketInfo( SOCKET clientSocket );
void RemoveSocketInfo( int nIndex );



// 오류 출력 함수
void err_quit( char* msg );
void err_display( char* msg );
void err_display( int errCode );



int _tmain( int argc, _TCHAR* argv[] )
{
	int retValue;
	InitializeCriticalSection( &g_CSection );



	// 윈속 초기화
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



	// 더미(dummy) 이벤트 객체 생성
	WSAEVENT hEvent = WSACreateEvent();
	if( hEvent == WSA_INVALID_EVENT )
		err_quit( "WSACreateEvent()" );

	g_aEventArray[ g_nTotalSockets++ ] = hEvent;



	// 스레드 생성
	DWORD dwThreadID;
	HANDLE hThread = CreateThread( NULL, 0, WorkerThread, NULL, 0, &dwThreadID );
	if( hThread == NULL )
		return -1;



	// 통신에 사용할 변수
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



		printf( "[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트번호 = %d\n",
			inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );



		// 접속 리스트 추가
		if( AddSocketInfo( clientSocket ) == FALSE )
		{
			closesocket( clientSocket );
			printf( "[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n",
				inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
			continue;
		}



		// 받기 상태로 설정
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



		// 이벤트 갯수 변동에 따른 대기상태 이벤트 배열 재설정 요청
		if( WSASetEvent( g_aEventArray[ 0 ] ) == FALSE )
		{
			err_display( "WSASetEvent()" );
			break;
		}
	}

	g_bThreadEnd = TRUE;
	WSASetEvent( g_aEventArray[ 0 ] );
	WaitForSingleObject( hThread, INFINITE );



	// 윈속 종료
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
		// 이벤트 대기상태
		index = WSAWaitForMultipleEvents( g_nTotalSockets, g_aEventArray, FALSE, WSA_INFINITE, FALSE );
		if( index == WSA_WAIT_FAILED )
		{
			err_display( "WSAWaitForMultipleEvents()" );
			continue;
		}

		index -= WSA_WAIT_EVENT_0;



		// 이벤트 재설정
		WSAResetEvent( g_aEventArray[ index ] );
		if( index == 0 ) // 만약 0번째 인덱스일 경우 이벤트 배열 재설정 요청을 한 것임
		{
			if( g_bThreadEnd == TRUE )
				break;

			continue;
		}



		// 클라이언트 정보 얻기
		SOCKET_INFO* pSocketInfo = g_aSocketInfoArray[ index ];
		nAddrLength = sizeof( clientAddr );
		getpeername( pSocketInfo->sock, (SOCKADDR*)&clientAddr, &nAddrLength );



		// 비동기 입출력 결과 확인
		retValue = WSAGetOverlappedResult( pSocketInfo->sock, &( pSocketInfo->overlapped ),
			&cbTransferred, FALSE, &dwFlags );
		if( retValue == FALSE || cbTransferred == 0 )
		{
			if( retValue == FALSE )
				err_display( "WSAGetOverlappedResult()" );

			RemoveSocketInfo( index );
			printf( "[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n",
				inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
			continue;
		}



		// 데이터 전송량 갱신
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



			// 데이터 받기
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
		printf( "[오류] 소켓 정보를 추가할 수 없습니다.\n" );
		return FALSE;
	}

	SOCKET_INFO* pSocketInfo = new SOCKET_INFO;
	if( pSocketInfo == NULL )
	{
		printf( "[오류] 메모리가 부족합니다.\n" );
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
	printf( "[오류] %s", (LPCTSTR)lpMsgBuf );
	LocalFree( lpMsgBuf );
}