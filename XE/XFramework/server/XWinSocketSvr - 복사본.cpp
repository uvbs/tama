#include "stdafx.h"
#include "XWinSocketSvr.h"
#include "XWinConnection.h"
#include "etc/Debug.h"
//#include "MFC\XServerView.h"
#include "XEUser.h"
#include "XEUserMng.h"
#include "XServerMain.h"
#include "Network/xenDef.h"
#include "XPool.h"
#include "XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)

WSADATA XEWinSocketSvr::s_WSAData = {0,};

//int g_sizeQBuffer = 0;
static unsigned int __stdcall _WorkThread(void *param)
{
//	XBREAK(param == 0);
	auto This = static_cast<XEWinSocketSvr*>(param);
	//
	This->WorkThread();
	
	_endthreadex(0);
	return 1;
}

static unsigned int __stdcall _AcceptThread(void *param)
{
//	XBREAK(param == 0);
	auto This = static_cast<XEWinSocketSvr*>(param);
	//
	This->AcceptThread();
	
	_endthreadex(0);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// numReadyConnect : 접속대기열 버퍼 크기
// maxConnect : 최대 동접자 수
XEWinSocketSvr::XEWinSocketSvr( LPCTSTR szName
															, unsigned short port
															, int numReadyConnect
															, int maxConnect
															, BOOL bCryptPacket )
{
	Init();
	XBREAK( port == 0 );
	XBREAK( maxConnect == 0 );
	m_strName = szName;
	m_maxConnect = maxConnect;
#ifdef _XCRYPT_PACKET
	// 암호화된 패킷을 사용 할 것인가
	XCrypto::SetEncrypt( bCryptPacket );
#endif
	//
	XList4<XE::xWinConn>* plist;
	//
// 	plist = m_shoConnectReady.GetpSharedObj();
// 	plist->Create( numReadyConnect );
// 	m_shoConnectReady.ReleaseSharedObj();
	//
// 	plist = &m_shoConnectionList.GetSharedObj();
// 	plist->Create( maxConnect );
// 	m_shoConnectionList.ReleaseSharedObj();
	//
// 	m_listDestroy.Create( numReadyConnect );
	//
	if( s_WSAData.wVersion == 0 ) {
		if( WSAStartup( MAKEWORD( 1, 1 ), &s_WSAData ) != 0 )  {
			XALERT( "WSAStartup failed" );
			return;
		}
	}
	//
	m_Socket = socket( AF_INET, SOCK_STREAM, 0 ); 
	if( m_Socket == INVALID_SOCKET ) {
		XALERT( "create socket failed" );
		return;
	}
	SOCKADDR_IN addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr) ) == SOCKET_ERROR ) { 
		XALERT( "Network bind error" );
		Destroy();
		return; 
	} 
	//
	m_Port = port;
	// 클라로부터 접속을 받을 준비
 	if( listen( m_Socket, SOMAXCONN ) == SOCKET_ERROR ) {
		XALERT( "Network::listen error" );
		Destroy();
		return; 
	}
	//
}

void XEWinSocketSvr::Destroy() 
{
	XTRACE( "destroy %s", m_strName.c_str() );
	XTRACE( "save user" );
	m_pUserMng->Save();
	
//	m_pUserMng->DestroyAll();
	XTRACE( "destroy usermng" );
	SAFE_DELETE( m_pUserMng );		// 여기서 커넥션도 실제 삭제함.
	XTRACE( "close thread" );
	CloseAllThread();
	XTRACE( "close socket" );
	if( m_Socket )
		closesocket( m_Socket );
	m_Socket = 0;
}

void XEWinSocketSvr::CloseAllThread( void ) 
{
	if (m_thAccept.m_hHandle != INVALID_HANDLE_VALUE && m_thAccept.m_hHandle != nullptr )	{
		::WaitForSingleObject(m_thAccept.m_hHandle, INFINITE);
//		CloseHandle(m_thAccept.m_hHandle);
		SAFE_CLOSE_HANDLE(m_thAccept.m_hHandle);
		m_thAccept.m_hHandle = INVALID_HANDLE_VALUE;
		XDetectDeadLock::sGet()->DelThread( m_thAccept.m_idThread );
	}
	for (int i = 0; i < MAX_THREAD; ++i)	{
		::WaitForSingleObject( m_aryThreadWork[i].m_hHandle, INFINITE);
		SAFE_CLOSE_HANDLE(m_aryThreadWork[i].m_hHandle);
//		CloseHandle(m_aryThreadWork[i].m_hHandle);
		XDetectDeadLock::sGet()->DelThread( m_aryThreadWork[i].m_idThread );
		m_aryThreadWork[i].m_hHandle = INVALID_HANDLE_VALUE;
	}
}

void XEWinSocketSvr::Create( void )
{
	m_pUserMng = CreateUserMng( m_maxConnect );
	XBREAK( m_pUserMng == NULL );
	// 유저 커스텀 create
	OnCreate(); // virtual
	// IOCP 객체 생성
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ); 
	// 워커 스레드 생성
	for( int i = 0; i <MAX_THREAD; ++i ) {
//		m_hThread[i] = CreateWorkThread();
		m_aryThreadWork.push_back( CreateWorkThread() );
	}
	// accept( 클라이언트로부터의 접속대기 ) 스레드 생성
	m_thAccept = CreateAcceptThread();
// 	m_hAcceptThread = CreateAcceptThread();
}


XEUserMng* XEWinSocketSvr::CreateUserMng( int maxConnect )
{
	XBREAK( m_pUserMng != NULL );
	return new XEUserMng( this, maxConnect );
}

XE::xThread XEWinSocketSvr::CreateWorkThread()
{
	XE::xThread th;
	th.m_hHandle = (HANDLE)_beginthreadex(NULL
																			, 0
																			, _WorkThread
																			, (LPVOID)this
																			, 0
																			, (unsigned*)&th.m_idThread);
	TCHAR szBuff[256];
	_stprintf_s( szBuff, _T("%s-work"), m_strName.c_str() );
	XDetectDeadLock::sGet()->AddThread( th, szBuff );
//	return CreateThread(NULL, 0, _WorkThread, (LPVOID)this, 0, &idThread);
	return th;
}
// HANDLE XEWinSocketSvr::CreateAcceptThread( void )
// {
// 	DWORD idThread; 
// 	auto hHandle = (HANDLE)_beginthreadex(NULL, 0, _AcceptThread, (LPVOID)this, 0, (unsigned*)&idThread);
// 	XDetectDeadLock::sGet()->Add( idThread, hHandle );
// //	return CreateThread( NULL, 0, _AcceptThread, (LPVOID)this, 0, &idThread ); 
// 	return hHandle;
// }
XE::xThread XEWinSocketSvr::CreateAcceptThread()
{
	XE::xThread th;
// 	DWORD idThread;
	th.m_hHandle = (HANDLE)_beginthreadex( NULL
																				, 0
																				, _AcceptThread
																				, ( LPVOID )this
																				, 0
																				, (unsigned*)&th.m_idThread );
	TCHAR szBuff[ 256 ];
	_stprintf_s( szBuff, _T( "%s-accept" ), m_strName.c_str() );
	XDetectDeadLock::sGet()->AddThread( th, szBuff );
	//	return CreateThread( NULL, 0, _AcceptThread, (LPVOID)this, 0, &idThread ); 
	return th;
}

void XEWinSocketSvr::WorkThread( void )
{
//	DWORD idThread = GetCurrentThreadId();
   unsigned long readbytes; 
   unsigned long dwCompKey; 
   OVERLAPPED * pOverlap; 
   while( 1 )
   {
		// 클라이언트로부터 데이타가 오길 기다림
		GetQueuedCompletionStatus( m_hIOCP, &readbytes, &dwCompKey, (LPOVERLAPPED *)&pOverlap, INFINITE ); 
		// 어떤 커넥션으로부터 데이타가 도착함.
		// 어떤 클라이언트 연결로부터 왔는지 알아냄
		ID idConnect = (ID) dwCompKey;
//		XEWinConnectionInServer *pConnect = NULL;
//			auto plistConnection = &m_shoConnectionList.GetSharedObj();
		auto plistReady = &m_shoConnectReady.GetSharedObj();
		auto pmap = &m_shoMapConnection.GetSharedObj();
// 		{
			// Lock: 커넥션 리스트와 대기열 자원 점유시작.
			auto pConnect = FindConnection( idConnect );
			if( pConnect ) {
				pConnect->Lock(__TFUNC__);
			//
// 		} // listConnection, listReady자원 점유구간.
			
//		if( pConnect )	{
			if( readbytes == 0 )	{	// 접속 끊김
				pConnect->DoDisconnect();
				BTRACE( "disconnected:%s", GetszName() );
				// 커넥션 자원점유 풀어줌. 이제 삭제해도 됨.
				pConnect->Unlock();
				// Unlock
				m_shoMapConnection.ReleaseSharedObj();
				m_shoConnectReady.ReleaseSharedObj();
//			m_shoConnectionList.ReleaseSharedObj(); 
				continue;		// 끊긴 커넥션이므로 더이상 처리안해도 된다.
			} else 	{
#ifdef _DEV
				if( pConnect->IsDisconnected() )
					CONSOLE( "삭제된 소켓에서 IOCP발생" );
#endif
				// 받은 데이터를 큐에 밀어넣음
				// 접속되고 곧바로 recv가 온다면 커넥션객체에 채 XUser가 붙기도 전에 이게 호출될수도 있음.
				// 그러므로 그런패킷은 일단 커넥션 큐안에 쌓아둬야 한다.
				pConnect->tRecvData( readbytes );
			}
			// 비동기 recv를 큐에 올림
			pConnect->tWSARecv();
			// 커넥션 자원점유 풀어줌. 이제 삭제해도 됨.
			pConnect->Unlock();
		} else {
#ifdef _DEV
			CONSOLE( "이미 삭제되거나 없는 연결에서 데이타가 도착했습니다." );
#endif
		}
		// Unlock
		m_shoMapConnection.ReleaseSharedObj();
		m_shoConnectReady.ReleaseSharedObj();
//	m_shoConnectionList.ReleaseSharedObj(); 
   } // while
}                           
// 접속 대기 스레드
void XEWinSocketSvr::AcceptThread( void ) 
{
	SOCKADDR_IN addr;
	int addrlen;
	SOCKET client;
	HANDLE hIOCP;
	while( m_bLoopAccept ) {
		addrlen = sizeof( addr );
		client = accept( m_Socket, ( struct sockaddr* )&addr, &addrlen );
		// 새로운 클라이언트의 접속
		//	   XBREAK( client == INVALID_SOCKET );
		//	   DWORD idThread = GetCurrentThreadId();
		if( client != INVALID_SOCKET ) {
			TCHAR	szIPClient[ 32 ];
			_stprintf_s( szIPClient, _T( "%d.%d.%d.%d" ),
				addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
				addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4 );
			// 이 서버로 접속되는 접속객체 하나 생성. 
			// 만약 이 서버가 월드서버라면 클라이언트 커넥션을 받고 만약 다른 서버와도 커넥션이 있어야 한다면 월드서버쪽에서 그 서버로 접속하여 별도의 커넥션을 유지시킬것
			auto pConnect = tCreateConnectionObj( client, szIPClient );
			if( pConnect ) {
				//				BTRACE( "accept -" );
				// Lock: 대기열 리스트의 자원을 점유한다.
				auto plistReady = &m_shoConnectReady.GetSharedObj();
				//
				if( plistReady->size() >= plistReady->GetnMax() )
					XBREAKF( 1, "plistReady is Full: %d/%d", plistReady->size(), plistReady->GetnMax() );
				plistReady->Add( pConnect );	// 일단 대기열에 올림.
				pConnect->Lock(__TFUNC__);		// 대기열에 올라간순간부터 락.
				// IOCP 객체에 클라이언트 소켓을 추가
				hIOCP = CreateIoCompletionPort( (HANDLE)client, m_hIOCP, (DWORD)pConnect->GetidConnect(), 0 );
				XBREAK( hIOCP != m_hIOCP );			// 이런 경우가 있나?
				XTRACE( "connected:%s", GetszName() );
				// 비동기 recv
				pConnect->tWSARecv();
				// 패킷받을 준비가 된걸 알림
				SendConnectComplete( pConnect );
				pConnect->Unlock();
				// Unlock: 대기열 자원점유를 해제한다.
				m_shoConnectReady.ReleaseSharedObj();
			} else {
				XBREAKF( 1, "create failed memory pool: CreateConnectionObj" );
				// 메모리풀 할당에 실패한경우.
				closesocket( client );
			}
		} else
			m_bLoopAccept = FALSE;
	}
	m_bExitAccept = TRUE;		// 스레드루프에서 빠져나왔음.
}

void XEWinSocketSvr::SendConnectComplete( XEWinConnectionInServer *pConnect )
{
	XPacket ar( (ID)XE::xXEPK_CONNECT_COMPLETE );
	if( XCrypto::GetbEncryption() )	{
		// 암호화 키
		XBREAK( GetkeyPrivate() == 0 );
		ar << (BYTE)GetkeyPrivate();
		ar << (BYTE)random(255);	// FAKE
		ar << (BYTE)random(255);	// fake
		ar << (BYTE)random(255);	// fake
		XCrypto::SerializeKeyTable( ar );
	} else
		ar << (DWORD)0;
	// 처음엔 암호화 안된 데이타를 보내줌	
	pConnect->SendData( ar.GetBuffer(), ar.size() ); 
}
// 새 유저객체를 만들어 유저매니저에 넣는다.
XEUser* XEWinSocketSvr::CreateAddUserMng( XEWinConnectionInServer *pConnect, XDBAccount *pAccount )
{
	// 커넥션이 끊긴채로 왔으면 생성하지 않는다. 
	// 커넥션이 생성되고 메인스레드의 Process에서 XUser가 연결되기까지 
	// 아주 잠깐동안의 시간이 있는데 이시간동안 만약 접속이 끊기면 유저 등록을 취소해야한다.
	if( pConnect->IsDisconnected() )
	{
		// 드문경우라서 로그를 한번 남겨봄. 너무 자주나오면 없앰.
		CONSOLE( "pConnect is disconnected. ip=%s", pConnect->GetszIP() );
		return NULL;
	}
	XBREAK( m_pUserMng == NULL );
	// 유저 생성
	XEUser *pNewUser = CreateUser( pConnect );		// virtual
	// 아마도 메모리풀 꽉참? 혹은 CreateUser미구현?
	if( XBREAK( pNewUser == NULL ) )
		return NULL;
	pNewUser->SetpDBAccount( pAccount );
	// 내부에서 계정객체를 virtual로 생성한다.
//	pNewUser->Create();
	// 유저매니저에 넣는다.
	//계정정보(계정아이디)가 아직 안올라와서 검색용 맵을 만들수 없다.
	//지금 시점에 유저매니저에 유저를 넣어야 하는 이유는?
	BOOL bSuccess = m_pUserMng->Add( pNewUser );
	//
	if( XBREAK( bSuccess == FALSE ) )
		SAFE_DELETE( pNewUser );
	return pNewUser;
}

// pConnect쪽에서 pAccount를 주며 유저객체 생성을 요청함.
XEUser* XEWinSocketSvr::CreateAddUser( XEWinConnectionInServer *pConnect, XDBAccount *pAccount )
{
	XEUser *pNewUser = CreateAddUserMng( pConnect, pAccount );
	if( pNewUser )
	{
		pConnect->SetpUser( pNewUser );	// 커넥션에 유저를 붙인다.
		pNewUser->OnConnect();				// 유저객체로 핸들러를 날려준다.
	} else
	{
		/**
		유저를 생성하는데 문제발생
		원인1: 커넥션이 접속이 끊김.
		원인2: 메모리풀 부족
		원인3: 유저매니저 내의 리스트공간 부족.
		*/
		// 
		// 유저와 붙는데 실패하면 커넥션 삭제
		AddDestroyConnectionForSelf( NULL, pConnect );
//		pConnect->DoDisconnect();   // 중복계정 메시지 보내야 해서 여기서 끊어도 안됨.
	}
	return pNewUser;
}

/**
 this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
*/
void XEWinSocketSvr::Process( float dt )
{
	//////////////////////////////////////////////////////////////////////////
	// 접속대기열의 커넥션들을 리스트에 등록시키고 유저객체를 만든다.
	{
		// lock: 순서 바꾸지 말것. 순서바꾸면 데드락 가능성 있음.(묶음 락도 있어야 겠네),
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		auto plistReady = &m_shoConnectReady.GetSharedObj();
		auto pmap = &m_shoMapConnection.GetSharedObj();
		// 이게 제일먼저 한락인지 저번 턴에서 release빼먹은건 아닌지 검사.
		XBREAK( m_shoConnectionList.IsFirstLock() == FALSE );
		XBREAK( m_shoConnectReady.IsFirstLock() == FALSE );
		int num = plistReady->size();
		if( num > 0 )	{	// xlist2루프 최적화가 안좋아 임시로 이렇게 함.
			XLIST2_LOOP( *plistReady, XEWinConnectionInServer*, pConnect ) {
				// 이미 커넥션리스트에 대기열 객체가 있으면 안된다.
				if( XBREAK( _FindConnectionInAdded( plistConnection, pConnect->GetidConnect() ) != NULL ) )
					continue;
				//
				if( pConnect->IsDisconnected() == FALSE ) {
					// 커넥션리스트에 등록.
					if( plistConnection->size() >= plistConnection->GetnMax() )
						XBREAKF( 1, "listAdded full: %d/%d", plistConnection->size(), plistConnection->GetnMax() );
					plistConnection->Add( pConnect );
					(*pmap)[ pConnect->GetidConnect() ] = pConnect;
					// 클라로부터 커넥션이 연결을때 호출된다. (로그인했을때가 아님)
					OnConnectFromClient( pConnect );
				} else {
					// 그새 접속이 끊겼으면 리스트에 추가하던거 취소하고 커넥션 바로 삭제.
					pConnect->Lock(__TFUNC__);				// 워커스레드쪽에서 사용중일수도 있으니 대기
//					SAFE_DELETE( pConnect );
					SAFE_RELEASE_REF( pConnect );
				}
			} END_LOOP;
		}
		// 대기열 지움.
		if( num )
			plistReady->Clear();
		// unlock
		m_shoMapConnection.ReleaseSharedObj();
		m_shoConnectReady.ReleaseSharedObj();
		m_shoConnectionList.ReleaseSharedObj();
	} // listConnectionReady

	//////////////////////////////////////////////////////////////////////////
	// Add 된 모든 커넥션들의 리스트를 루프돌며 패킷 펌핑.
	{
		//////////////////////////////////////////////////////////////////////////
		// 커넥션 프로세스 부활
		// Lock: 커넥션 리스트. 위에서 닫고 다시 락을 건이유는 이 프로세스에서 너무 오래 락잡고 있으면 안좋을거 같아서.
		// lock: 순서 바꾸지 말것. 순서바꾸면 데드락 가능성 있음.(묶음 락도 있어야 겠네),
		XList2<XEWinConnectionInServer> *plistConnection = &m_shoConnectionList.GetSharedObj();
//		XList2<XWinConnectionInServer> *plistReady = &m_shoConnectReady.GetSharedObj();
		// 동접이 많으면 이 많은 커넥션들 다 패킷펌핑하기전까진 워커스레드 접속스레드 멈춰야 되는데... 이래선 싱글스레드와 다를바 없지 않은가.
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			// 커넥션에 쌓인 패킷을 펌핑하고 처리함
			if( !pConnect->GetbDestroy() )
				pConnect->Process();			// 클라가 죽기전 중요한 패킷을 보내고 죽었을수도 있으니 이번턴에 온 패킷은 다 처리하고 죽음.
			pConnect->ProcesssAsyncDisconnect();
			// 접속끊긴 커넥션은 파괴 리스트에 넣는다.
			if( pConnect->IsDisconnected() || pConnect->GetbDestroy()) {
				// 커넥션 파괴 목록에 넣는다.
				AddDestroyConnectionForSelf( plistConnection, pConnect );
			}	else {
			}
		} END_LOOP;
		// 커넥션 프로세스
		//////////////////////////////////////////////////////////////////////////
		// unlock
//		m_shoConnectReady.ReleaseSharedObj();
		m_shoConnectionList.ReleaseSharedObj();
	}
	//////////////////////////////////////////////////////////////////////////
	m_pUserMng->Process( dt );		
	
	//////////////////////////////////////////////////////////////////////////
	// 죽음 명령받은 커넥션들 삭제
	int size = m_listDestroy.size();		// <- 이건 다른 스레드에서 사용하지 않음.
	if( size > 0 )	
	{
		// Lock: 커넥션 리스트/대기열리스트의 자원 점유시작
		XList2<XEWinConnectionInServer> *plistConnection = &m_shoConnectionList.GetSharedObj();
		XList2<XEWinConnectionInServer> *plistReady = &m_shoConnectReady.GetSharedObj();
		std::map<ID, XEWinConnectionInServer*> *pmap = &m_shoMapConnection.GetSharedObj();
		BOOL bLog = FALSE;
		int idx = 0;
		XLIST2_LOOP( m_listDestroy, XEWinConnectionInServer*, pConnect )
		{
			// 연결리스트에서 제거
			plistConnection->Del( pConnect );
			pmap->erase( pConnect->GetidConnect() );	// 맵에서 삭제
			XBREAK( plistReady->Find( pConnect ) == -1 );
			// 워커스레드에서 뽑아서 쓰고 있을수 있으므로 대기
			pConnect->Lock(__TFUNC__);
			// 유저 삭제
			XEUser *pUser = pConnect->GetpUser();
			if( pUser )
			{
#if !defined(_XBOT2)
#ifdef _DEV
				// 사람많으면 느려지니까 개발중에만 나오도록 바꿈.
				TCHAR szLog[ 1024 ];
				_tcscpy_s( szLog, XFORMAT( "usermng del user: %s, idAcc=%d", pConnect->GetszIP(), pUser->GetidUser() ) );
				SERVER_MAIN->DoLog( XE::xSLOG_DESTROY_USER, szLog, (DWORD) this );
#endif // 
#endif // not bot
				// 유저매니저에서 커넥션을 가진 유저를 뺌(즉시).
				m_pUserMng->DelUser( pUser );
				// 커넥션이 가진 유저객체 삭제
				// 커넥션내부에서 유저를 삭제할수도 있는데 유저객체의 생성과 소멸을 모두 윈소켓서버에서 하고 싶어서 밖으로 뺌
				// 커넥션이 잡고있던 유저를 릴리즈 시키고 내부 유저포인터를 클리어 시킴.
			}
#if !defined(_XBOT2)
#ifdef _DEV
			XLOGXN("release connect: id=0x%08x, refCnt=%d, ip=%s", 
														pConnect->GetidConnect(),
														pConnect->GetcntRef(),
														pConnect->GetszIP() );
#endif
#endif
			SAFE_RELEASE_REF( pConnect );
			++idx;
		} END_LOOP;		// listDestroy
		m_listDestroy.Clear();
		// 커넥션수 갱신
		int num = plistConnection->size();
//		XServerView::GetView()->SetnumConnect( num );
		m_numConnect = num;
		// Unlock: 대기열/커넥션 리스트 자원점유를 반납.
		m_shoMapConnection.ReleaseSharedObj();
		m_shoConnectReady.ReleaseSharedObj();
		m_shoConnectionList.ReleaseSharedObj();
	} // 커넥션 삭제 블럭

	++m_Count;
} // process

// 유저매니저로부터 유저가 가진 커넥션을 삭제해도 좋다는 통보를 받음.
/*
BOOL XWinSocketSvr::AddDestroyConnectionFromUserMng( XWinConnectionInServer *pConnect )
{
	return AddDestroyConnectionForSelf( pConnect );
}
*/
// this만이 사용하는 커넥션 파괴 등록 함수.
BOOL XEWinSocketSvr::AddDestroyConnectionForSelf( XList2<XEWinConnectionInServer>* /*plistAdded*/, XEWinConnectionInServer *pConnect )
{
	BOOL bRet = TRUE;
	// plistAdded는 락이 걸려있어야 한다.
	// Lock: 커넥션 리스트(m_listDestroy.Add();보다 아래로 내려가도 되지만 안전을 위해서 올려놓음)
	do 
	{
		// 이럴리는 없겠지만 이미 같은 커넥션이 등록되어 있으면 그냥 나감.
		// 두번호출되는지 확인
		if( XBREAK( m_listDestroy.Find( pConnect ) == TRUE ) )		
		{
			// 그렇다면 혹시 리스트에도 있는지 확인.
/*			if( XBREAK( plistAdded->Find( pConnect ) == TRUE ) )
			{
				int idxDel = plistAdded->Del( pConnect );
				// 정상적으로 지워지는지도 확인.
				XBREAK( idxDel == -1 );
			} */
			bRet = FALSE;
			break;
		}
		// 삭제목록에 올림
		m_listDestroy.Add( pConnect );
		// 하위 상속자에게 pConnect가 삭제될거란걸 알림
		OnDestroyConnection( pConnect );
		// 커넥션의 파괴 핸들러 호출
		pConnect->Lock(__TFUNC__);
		pConnect->OnDestroy();
		pConnect->Unlock();
		++m_numDestroyAdd;
	} while (0);

	return bRet;
}


// 클라이언트로부터 접속이 성공하면 유저객체를 만들어 유저관리자에 등록한다.
void XEWinSocketSvr::OnConnectFromClient( XEWinConnectionInServer *pConnect )
{
	int num = GetNumConnection();
//	XServerView::GetView()->SetnumConnect( num );
	m_numConnect = num;
}

XEUser* XEWinSocketSvr::CreateUser( XEWinConnectionInServer *pConnect )
{
	return new XEUser( m_pUserMng, pConnect );
}


// 커넥션 아이디로 커넥션 객체를 찾는다. listConnection과 대기열 모두에서 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::FindConnection( 
									XList2<XEWinConnectionInServer> *plistReady, 
									XList2<XEWinConnectionInServer> *plistAdded, 
									ID idConnect )
{
	XEWinConnectionInServer *pConnect = _FindConnectionInReady( plistReady, idConnect );
	if( pConnect )
		return pConnect;
	pConnect = _FindConnectionInAdded( plistAdded, idConnect );
	return pConnect;
}

// 커넥션 아이디로 커넥션 객체를 찾는다. 대기열에서만 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInReady( XList2<XEWinConnectionInServer> *plist, ID idConnect )
{
	XLIST2_LOOP( *plist, XEWinConnectionInServer*, pConnect )
	{
		XBREAK( pConnect->GetidConnect() == 0 );
		if( pConnect->GetidConnect() == idConnect )
			return pConnect;
	} END_LOOP; 
	return NULL;
}

// 커넥션 아이디로 커넥션 객체를 찾는다. add된. 그러니까 listConnection에서만 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInAdded( XList2<XEWinConnectionInServer> *plist, ID idConnect )
{
	XLIST2_LOOP( *plist, XEWinConnectionInServer*, pConnect )
	{
		XBREAK( pConnect->GetidConnect() == 0 );
		if( pConnect->GetidConnect() == idConnect )
			return pConnect;
	} END_LOOP;
	return NULL;
}

// 커넥션 아이디로 커넥션 객체를 찾는다. listConnection과 대기열 모두에서 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::FindConnection( ID idConnect )
{
	XEWinConnectionInServer *pConnect = _FindConnectionInReady( idConnect );
	if( pConnect )
		return pConnect;
	pConnect = _FindConnectionInAdded( idConnect );
	return pConnect;
}

// 커넥션 아이디로 커넥션 객체를 찾는다. 대기열에서만 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInReady( ID idConnect )
{
	XList2<XEWinConnectionInServer> *plist = &m_shoConnectReady.GetSharedObj();
	XEWinConnectionInServer *pFind = NULL;
	XLIST2_LOOP( *plist, XEWinConnectionInServer*, pConnect )
	{
		XBREAK( pConnect->GetidConnect() == 0 );
		if( pConnect->GetidConnect() == idConnect )
		{
			pFind = pConnect;
			break;
		}
	} END_LOOP; 
	m_shoConnectReady.ReleaseSharedObj();
	return pFind;
}

// 커넥션 아이디로 커넥션 객체를 찾는다. 맵에서 찾는다.
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInAdded( ID idConnect )
{
//	std::map<ID, XEWinConnectionInServer*>::iterator itor;
	XEWinConnectionInServer* pConnect = NULL;
	// Lock
	auto pmap = &m_shoMapConnection.GetSharedObj();
	do
	{
		auto itor = pmap->find( idConnect );
		if( itor == pmap->end() )
		{
			pConnect = NULL;
			break;
		}
		pConnect = (*itor).second;
		XBREAK( pConnect == NULL );
		if( XBREAK( XE::IsValidPtr( pConnect ) == FALSE ) )
		{
			pmap->erase( idConnect );
			pConnect = NULL;
		}
	} while(0);
	// Unlock
	m_shoMapConnection.ReleaseSharedObj();
	return pConnect;
}

/**
 @brief idAcc로 커넥션을 찾는다.
*/
XEWinConnectionInServer* XEWinSocketSvr::GetConnectionByIdAcc( ID idAcc )
{
  auto pConnect = _FindConnectionInReadyByidAcc( idAcc );
  if( pConnect )
    return pConnect;
  pConnect = _FindConnectionInAddedByidAcc( idAcc );
  return pConnect;
}

/**
 @brief 대기열에서 idAcc로 커넥션을 찾는다.
*/
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInReadyByidAcc( ID idAcc )
{
  auto plist = &m_shoConnectReady.GetSharedObj();
  XEWinConnectionInServer *pFind = NULL;
  XLIST2_LOOP( *plist, XEWinConnectionInServer*, pConnect )
  {
    XBREAK( pConnect->GetidConnect() == 0 );
    if( pConnect->GetidAccount() == idAcc )
    {
      pFind = pConnect;
      break;
    }
  } END_LOOP;
  m_shoConnectReady.ReleaseSharedObj();
  return pFind;
}

/**
 @brief idAcc로 커넥션을 찾는 버전
 접속이 완전히 이루어지지 않아서 User객체가 없을때 사용하는 버전
*/
XEWinConnectionInServer* XEWinSocketSvr::_FindConnectionInAddedByidAcc( ID idAcc )
{
  //	std::map<ID, XEWinConnectionInServer*>::iterator itor;
  XEWinConnectionInServer* pConnect = nullptr;
  // Lock
  auto pmap = &m_shoMapConnection.GetSharedObj();
  do {
// 		auto itor = pmap->find( idAcc );
// 		if( itor != pmap->end() ) {
// 			auto _pC = itor->second;
// 			if( XASSERT( XE::IsValidPtr( _pC ) ) ) {
// 				pConnect = _pC;
// 				break;
// 			}
// 		}
//		pConnect = nullptr;
    for( auto& itor : *pmap ) {
      auto pC = itor.second;
      if( pC ) {
        if( XASSERT( XE::IsValidPtr( pC ) ) ) {
          if( pC->GetidAccount() == idAcc ) {
						pConnect = pC;
            break;
					}
        }
      }
    }
  } while( 0 );
  // Unlock
  m_shoMapConnection.ReleaseSharedObj();
  return pConnect;
}

/*
{
	m_list.Lock();
	pObj = m_list.Find();
	if( pObj ) {
		pObj->Lock();
		pObj->Add();
		pObj->Unlock();
	}
	m_list.Unlock();

	m_pObjcopy->Lock();
	m_pObjCopy->Add();
	m_pObjCopy->Unlock();
}

// 두 리스트가 한 객체를 참조하는 경우
m_list2.Lock();
for( pObj : m_list2 ) {
	pObj->Do2();
}
m_list2.Unlock();

// 객체에락까지 거는 경우
m_list2.Lock();
for( pObj : m_list2 ) {
	pObj->Lock();
	pObj->Do2();
	pObj->Unlock();
}
m_list2.Unlock();
//
m_list2.Lock();
for( pObj : m_list2 ) {
pObj->Lock();
SAFE_DELETE( pObj );		=> 안됨
}
m_list2.Unlock();
//

m_list2.Lock();
m_lockDestroy.Lock();	or m_list.Lock();	=> 삭제할때는 별도의 락을 걸고 다른 스레드에서 항상 이걸 락걸게 하면 삭제할때 외에는 객체별 락을 걸어도 쓸수 있을듯.
하지만 이러면 m_list에 대한 락 하나만 쓰는것과 다를바가 없음.
for( pObj : m_list2 ) {
	SAFE_DELETE( pObj );
}
m_lockDestroy.UnLock();
m_list2.Unlock();

m_list보관용 <- refCnt를 최소1로 보존하는 역할.
m_list.Lock();
for( spObj : m_list ) {
	spObj->Lock();
	m_list.erase( spObj );
	spObj->UnLock();		=> 보관용리스트로인해 삭제가 안되므로 가능. 그러나 보관용리스트는 언제까지 보관해야함?
}
m_list.Unlock();
//
m_list.Lock();
for( spObj : m_list ) {
	spObj->Lock();
	m_listDestroy().add( spObj );
	spObj->SetDestroy(1);
	m_list.erase( spObj );
	spObj->UnLock();		=> 삭제리스트에 들어갔으므로 아직 파괴는 안됨
}
m_list.Unlock();

m_listDestroy.Lock();
m_listDestroy.clear();		// 객체 파괴. 이때는 다른어떤곳에서도 이 객체를 참조하고 있으면 안됨.

for( spObj : m_listDestroy ) {
	spObj->Lock();
	m_listDestroy.erase( spObj );
}
m_listDestroy.Unlock();

*/
