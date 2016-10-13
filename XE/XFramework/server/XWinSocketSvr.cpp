#include "stdafx.h"
#include "XFramework/XEProfile.h"
#include "XWinSocketSvr.h"
#include "XWinConnection.h"
#include "etc/Debug.h"
//#include "MFC\XServerView.h"
#include "XEUser.h"
#include "XEUserMng.h"
#include "XServerMain.h"
#include "Network/xenDef.h"
#include "XPool.h"
#include "XFramework/XDetectDeadLock.h"
#include "Network/XWinNetwork.h"
#include "XFramework/XEProfile.h"

/********************************************************************
	@date:	2016/03/15 12:44
	@file: 	C:\xuzhu_work\Project\iPhone_zero\XE\XFramework\server\XWinSocketSvr.cpp
	@author:	xuzhu
	
	@brief:	서버소켓의 기본형
	멤버중에서 XSPWinConnInServer spConnect 형태로 커넥션 파라메터만 받는 멤버가 있는데 이것들은 보통 외부에서 락이 걸려있는걸 보장한다는 뜻이다.
	*********************************************************************/
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)
//#define GET_FREQ_TIME()				XE::GetFreqTime()
//#define GET_FREQ_TIME()				(0)
// WSADATA XEWinSocketSvr::s_WSAData = {0,};

// if( CONSOLE_MAIN->IsLogidAcc( GetidAcc() ) ) {
// 	\

#define CONSOLE_THIS( TAG, F, ... ) \
	__xLogfTag( TAG, XLOGTYPE_LOG, XTSTR("%s:-", F), __TFUNC__, ##__VA_ARGS__);

//int g_sizeQBuffer = 0;
static unsigned int __stdcall _WorkThread(void *param)
{
//	XBREAK(param == 0);
	auto This = static_cast<XEWinSocketSvr*>(param);
	//
	This->WorkThread();
//	_endthreadex(0);		// 리턴하면 시스템내부에서 호출하므로 여기서 안해도 됨
	return 0;		// 0을 리턴해야함.
}

static unsigned int __stdcall _AcceptThread(void *param)
{
//	XBREAK(param == 0);
	auto This = static_cast<XEWinSocketSvr*>(param);
	//
	This->AcceptThread();
	
//	_endthreadex(0);
	return 1;
}

// numReadyConnect : 접속대기열 버퍼 크기
// maxConnect : 최대 동접자 수
XEWinSocketSvr::XEWinSocketSvr( LPCTSTR szName
															, int numReadyConnect
															, int maxConnect
															, BOOL bCryptPacket )
{
	Init();
	m_spLock = std::make_shared<XLock>();
	XBREAK( maxConnect == 0 );
	m_strName = szName;
	m_maxConnect = maxConnect;
	// 암호화된 패킷을 사용 할 것인가
	XCrypto::SetEncrypt( bCryptPacket );
	//
}

void XEWinSocketSvr::Destroy() 
{
	BTRACE( "destroy %s", m_strName.c_str() );
	BTRACE( "destroy usermng" );
	SAFE_DELETE( m_pUserMng );		// 여기서 커넥션도 실제 삭제함.
	BTRACE( "close thread" );
	auto plistLogined = &m_Logined.m_shoList.GetSharedObj();
	for( auto spConnect : *plistLogined ) {
//		OVERLAPPED lpOverlapped;
//		BOOL bOk = PostQueuedCompletionStatus( m_hIOCP, 0, spConnect->GetidConnect(), nullptr );
		spConnect->DoDisconnect();
	}
	if( m_Socket )
		closesocket( m_Socket );
	m_Socket = 0;
	m_Logined.m_shoList.ReleaseSharedObj();
	CloseAllThread();
	CloseHandle( m_hIOCP );
//	CloseHandle( m_hIOCP );
	BTRACE( "close socket" );
}

void XEWinSocketSvr::OnDestroy()
{
	BTRACE( "save user" );
	if( XASSERT(m_pUserMng) ) {
		m_pUserMng->Save();
	}
}

/**
 @brief 
 @param numWorkThread 0이면 시스템 코어에 맞춰 자동
*/
void XEWinSocketSvr::Create( WORD port, int numWorkThread )
{
	XBREAK( port == 0 );
	if( XWinNetwork::sStartUp() == false ) {
		XALERT( "WSAStartup failed" );
		return;
	}
	//
//	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	m_Socket = WSASocket( PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_Socket == INVALID_SOCKET ) {
		XALERT( "create socket failed" );
		return;
	}
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
	if( bind( m_Socket, ( struct sockaddr * )&addr, sizeof( addr ) ) == SOCKET_ERROR ) {
		XBREAK(1);
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
	m_pUserMng = CreateUserMng( m_maxConnect );
	XBREAK( m_pUserMng == NULL );
	// 유저 커스텀 create
	OnCreate(); // virtual
	// IOCP 객체 생성
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ); 
	// 워커 스레드 생성
	SYSTEM_INFO si;
	memset( &si, 0, sizeof(si));
	GetSystemInfo( &si );
#if defined(_DEBUG) && !defined(_XBOT)
	const int numThread = 4;		// 개발중엔 편의상 스레드 적게쓴다.
#else
	const int numThread = (numWorkThread == 0)? 
														(int)(si.dwNumberOfProcessors * 2)
													: numWorkThread;
#endif
	XBREAK( numThread <= 0 || numThread > 32 );
	for( int i = 0; i < numThread; ++i ) {
		m_aryThreadWork.push_back( CreateWorkThread() );
	}
	// accept( 클라이언트로부터의 접속대기 ) 스레드 생성
	m_thAccept = CreateAcceptThread();
	//
#if _DEV_LEVEL <= DLV_OPEN_BETA
	m_timerSec.Set( 1.f );
#endif
}

void XEWinSocketSvr::CloseAllThread() 
{
	if (m_thAccept.m_hHandle != INVALID_HANDLE_VALUE && m_thAccept.m_hHandle != nullptr )	{
		::WaitForSingleObject(m_thAccept.m_hHandle, INFINITE);
		CloseHandle( m_thAccept.m_hHandle );
//		SAFE_CLOSE_HANDLE(m_thAccept.m_hHandle);
		m_thAccept.m_hHandle = INVALID_HANDLE_VALUE;
		XDetectDeadLock::sGet()->DelThread( m_thAccept.m_idThread );
	}
	for( auto& th : m_aryThreadWork ) {
		BOOL bOk = PostQueuedCompletionStatus( m_hIOCP, 0, 0, nullptr );
	}
	for( auto& th : m_aryThreadWork ) {
		::WaitForSingleObject( th.m_hHandle, INFINITE );
		CloseHandle( th.m_hHandle );
		XDetectDeadLock::sGet()->DelThread( th.m_idThread );
		th.m_hHandle = INVALID_HANDLE_VALUE;
	}
	m_aryThreadWork.clear();
// 	for (int i = 0; i < MAX_THREAD; ++i)	{
// 		::WaitForSingleObject( m_aryThreadWork[i].m_hHandle, INFINITE);
// 		CloseHandle( m_aryThreadWork[ i ].m_hHandle );
// //		SAFE_CLOSE_HANDLE(m_aryThreadWork[i].m_hHandle);
// 		XDetectDeadLock::sGet()->DelThread( m_aryThreadWork[i].m_idThread );
// 		m_aryThreadWork[i].m_hHandle = INVALID_HANDLE_VALUE;
// 	}
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
	return th;
}
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
	return th;
}

void XEWinSocketSvr::WorkThread()
{
	unsigned long readbytes;
	unsigned long dwCompKey;
//	OVERLAPPED * pOverlap;
	XE::xOVERLAPPED* pOverlap = nullptr;
	while( 1 ) {
		// 클라이언트로부터 데이타가 오길 기다림
		BOOL bRet = GetQueuedCompletionStatus( m_hIOCP, &readbytes, &dwCompKey, (LPOVERLAPPED*)&pOverlap, INFINITE );
		// TODO:
#pragma message("리턴값처리 제대로 할것. ")
		if( bRet == FALSE ) {
			auto err = GetLastError();
			TCHAR* szMsg = nullptr;
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER
									, nullptr
									, err
									, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT)
									, (TCHAR*)&szMsg
									, 0
									, nullptr );
			if( pOverlap == nullptr )	{
				return;
			} else {
				if( readbytes ) {
					// 패킷을 dequeue했지만 실패한I/O에 대한 dequeue였다.
				} else {
					// 클라이언트 소켓 close
				}
			}
		} else {
			if( pOverlap == nullptr )
				return;
		}
		const auto typeEvent = pOverlap->typeEvent;
		if( bRet == FALSE && pOverlap == nullptr ) {
			return;
		}
		// 어떤 커넥션으로부터 데이타가 도착함.
		// 어떤 클라이언트 연결로부터 왔는지 알아냄
		ID idConnect = (ID)dwCompKey;
		// 해당 커넥션객체를 찾음.
		auto spConnect = FindspConnect( idConnect );	// 스레드안전
		//
		if( spConnect ) {
			XAUTO_LOCK3( spConnect );
			//
			if( readbytes != 0 ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
				if( spConnect->IsDisconnected() ) {
					CONSOLE( "connect", "끊어진 소켓에서 IOCP발생" );
				}
#endif
				// 받은 데이터를 큐에 밀어넣음
				// 접속되고 곧바로 recv가 온다면 커넥션객체에 채 XUser가 붙기도 전에 이게 호출될수도 있음.
				// 그러므로 그런패킷은 일단 커넥션 큐안에 쌓아둬야 한다.
				if( !spConnect->IsDisconnected() && !spConnect->IsbDestroy() ) {
					if( typeEvent == 1 ) {		// send의 완료는 따로 처리할것이 없음.
						// send
						continue;
// 						spConnect->SendData( )
					}
					else {
						// recv
						spConnect->tRecvData( readbytes );
#if _DEV_LEVEL <= DLV_OPEN_BETA
						const int sizeQ = spConnect->GetSizeQueue();
						// 최대 큐크기 표시용
						if( sizeQ > m_sizeMaxQ )
							m_sizeMaxQ = sizeQ;
#endif
						// 비동기 recv를 큐에 올림
						spConnect->tWSARecv();
					}
				}
			} else 	{
				// 접속 끊김
				// 서버의 다른데서 소켓닫으면 여기로 들어오나?
				spConnect->Set_bConnected( FALSE );		// 연결해제 플래그만 켜고 실제 삭제처리는 메인스레드에서.
				if( bRet && pOverlap == nullptr ) {
					spConnect->DoDisconnect();
					return;
				}
				BTRACE( "disconnected:%s", GetszName() );
				continue;		// 끊긴 커넥션이므로 더이상 처리안해도 된다.
			} // if( readbytes == 0 )	{	// 접속 끊김
		} else {
#if (_DEV_LEVEL <= DLV_DEV_EXTERNAL) && !defined(_XBOT)
			CONSOLE( "이미 삭제되거나 없는 연결에서 데이타가 도착했습니다." );
#endif
		} // if( pConn ) {
		Sleep( m_msecSleepRecv );
	} // while(1)
} // WorkThread()

/**
 @brief 접속 대기 스레드
*/
void XEWinSocketSvr::AcceptThread() 
{
	SOCKADDR_IN addr;
	int addrlen;
	SOCKET client;
	HANDLE hIOCP;
	while( m_bLoopAccept ) {
		addrlen = sizeof( addr );
		//
		client = WSAAccept( m_Socket, ( struct sockaddr* )&addr, &addrlen, 0, 0 );
// 		client = accept( m_Socket, ( struct sockaddr* )&addr, &addrlen );
		// 새로운 클라이언트의 접속
		//	   XBREAK( client == INVALID_SOCKET );
		//	   DWORD idThread = GetCurrentThreadId();
		int sizeList = 0;
		if( client != INVALID_SOCKET ) {
			TCHAR	szIPClient[ 64 ];
			_stprintf_s( szIPClient, _T( "%d.%d.%d.%d" ),
																addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
																addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4 );
			CONSOLE_TAG( "connect", "%s:ip=%s", __TFUNC__, szIPClient );
			// 이 서버로 접속되는 접속객체 하나 생성. 
			// 만약 이 서버가 월드서버라면 클라이언트 커넥션을 받고 만약 다른 서버와도 커넥션이 있어야 한다면 월드서버쪽에서 그 서버로 접속하여 별도의 커넥션을 유지시킬것
			auto spConnect = tCreateConnectionObj( client, szIPClient );
			if( spConnect ) {
//				auto spConnect = std::make_shared<XE::xWinConn>( spConnect );
				// Lock: 대기열 리스트의 자원을 점유한다.
				XAutoSharedObj<XList_Connection> shaListConnected( m_Connected.m_shoList );
				XAutoSharedObj<XMap_Connection> shaMapConnected( m_Connected.m_shoMap );
				auto plistConnected = shaListConnected.Get();
				auto pMapConnected = shaMapConnected.Get();
				sizeList = plistConnected->size();
				//
				const ID idConnect = spConnect->GetidConnect();
				// 대기열에 올라간순간부터 락이 되어야 하기때문에 미리 락을 걸음.
				{
					XAUTO_LOCK3( spConnect );			// 락부터 걸고
#ifdef _DEBUG
					// 미리 한번 검사해본다.
// 					auto spConnExist = _FindspConnInList( *plistConnected, idConnect );
// 					auto spConnExistInMap = _FindspConnInMap( *pMapConnected, idConnect );
// 					if( XASSERT(spConnExist == nullptr && spConnExistInMap == nullptr) ) 
#endif // _DEBUG
					{
						plistConnected->Add( spConnect );	// 대기열에 올림.
						auto itor = pMapConnected->find( idConnect );
						if( itor != pMapConnected->end() ) {
							XBREAK(1);
						}
						(*pMapConnected)[ idConnect ] = spConnect;
						// IOCP 객체에 클라이언트 소켓을 추가
						hIOCP = ::CreateIoCompletionPort( (HANDLE)client
																						, m_hIOCP
																						, (DWORD)spConnect->GetidConnect()
																						, 0 );
						XBREAK( hIOCP != m_hIOCP );			// 이런 경우가 있나?
						BTRACE( "connected:%s", GetszName() );
						spConnect->Set_bConnected( TRUE );
						// 비동기 recv
						spConnect->tWSARecv();
						// 패킷받을 준비가 된걸 알림
						const int cntUse = spConnect.use_count();
						spConnect->SetbConnectedFlag( true );

//						m_bConnectedFlag = true;		// SendConnectComplete실행하도록 한다.
// 						SendConnectComplete( spConnect );
						// 여기서 패킷 Crypt하면서 m_pSocketSvr를 참조하게 된다. 스레드안전 주의.
					}
				}
			} else {
				XBREAKF( 1, "create failed memory pool: CreateConnectionObj" );
				// 메모리풀 할당에 실패한경우.
				closesocket( client );
			}
		} else {
			m_bLoopAccept = FALSE;
		}
		// 갑자기 접속이 몰리면 메인스레드에서 다 소화할때까지 잠시 대기시킨다.
// 		while(sizeList > 100) {
// 			XAutoSharedObj<XList_Connection> shaListConnected( m_Connected.m_shoList );
// 			auto pList = shaListConnected.Get();
// 			if( pList->size() < 100 )
// 				break;
// 			Sleep(1);
// 		}
		Sleep( m_msecSleepConn );
	}
	m_bExitAccept = TRUE;		// 스레드루프에서 빠져나왔음.
} // AcceptThread() 

XSPUserBase XEWinSocketSvr::CreateUser( XSPWinConnInServer spConnect )
{
	return std::make_shared<XEUser>( m_pUserMng, spConnect );
}


void XEWinSocketSvr::SendConnectComplete( XSPWinConnInServer spConnect )
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
	} else {
		ar << (DWORD)0;
	}
	// 처음엔 암호화 안된 데이타를 보내줌	
	spConnect->SendData( ar.GetBuffer(), ar.size() ); 
}
// 새 유저객체를 만들어 유저매니저에 넣는다.
XSPUserBase XEWinSocketSvr::CreateAddToUserMng( XSPWinConnInServer spConnect, XSPDBAcc spAccount )
{
	// 커넥션이 끊긴채로 왔으면 생성하지 않는다. 
	// 커넥션이 생성되고 메인스레드의 Process에서 XUser가 연결되기까지 
	// 아주 잠깐동안의 시간이 있는데 이시간동안 만약 접속이 끊기면 유저 등록을 취소해야한다.
	if( spConnect->IsDisconnected() ) {
		// 드문경우라서 로그를 한번 남겨봄. 너무 자주나오면 없앰.
//#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		CONSOLE_THIS( "connect", "pConnect is disconnected. ip=%s", spConnect->GetszIP() );
//#endif		
		return nullptr;
	}
	XBREAK( m_pUserMng == nullptr );
	// 유저 생성
	auto spNewUser = CreateUser( spConnect );		// virtual
	// 아마도 메모리풀 꽉참? 혹은 CreateUser미구현?
	if( XBREAK( spNewUser == nullptr ) )
		return nullptr;
	spNewUser->SetspDBAccount( spAccount );
	// 유저매니저에 넣는다.
	// 넣기전 중복체크
	const ID idUser = spNewUser->GetidUser();
	{
		auto spExist = m_pUserMng->GetspUserFromidAcc( idUser );
		if( spExist ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
			CONSOLE( "중복접속 계정: idAcc=%d", spExist->GetidUser() );
#endif			
			spExist->DoDisconnect();		// 일단 먼저 끊고
			spExist->DoDestroy();			// 커넥션 삭제되도록 파괴 메시지
			m_pUserMng->DelUser( spExist );		// 기존유저에게 이벤트핸들링 호출하고 리스트에서 삭제
			spExist.reset();
		}
	}
	// 새 유저객체 등록
	auto bSuccess = m_pUserMng->Add( spNewUser );
	//
// 	if( XBREAK( bSuccess == false ) ) {
// 		spNewUser = nullptr;
// 	}
	if( XASSERT( bSuccess ) ) {
		spConnect->SetspUser( spNewUser );	// 커넥션에 유저를 붙인다.
		// idAcc로 커넥션 찾는 맵에 등록.
		auto pMap = &m_Logined.m_shoMapByidAcc.GetSharedObj();
		(*pMap)[ spAccount->GetidAccount() ] = spConnect;
		m_Logined.m_shoMapByidAcc.ReleaseSharedObj();
		spNewUser->OnLogined();				// 유저객체로 핸들러를 날려준다.
	} else {
		spNewUser = nullptr;
	}
	return spNewUser;
}

// 
/**
 @brief 유저객체 생성을 요청함.
 로그인 인증이 성공하면 계정정보와 함께 커넥션측에서 유저객체 생성을 요청한다.
*/
// XSPUserBase XEWinSocketSvr::CreateAddUser( XSPWinConnInServer spConnect, XSPDBAcc spAccount )
// {
// 	auto spNewUser = CreateAddUserMng( spConnect, spAccount );
// 	if( spNewUser ) {
// 		spConnect->SetspUser( spNewUser );	// 커넥션에 유저를 붙인다.
// 		spNewUser->OnConnect();				// 유저객체로 핸들러를 날려준다.
// 	} else {
// 		/**
// 		유저를 생성하는데 문제발생
// 		원인1: 커넥션이 접속이 끊김.
// 		원인2: 메모리풀 부족
// 		원인3: 유저매니저 내의 리스트공간 부족.
// 		*/
// 		// 
// 		// 유저와 붙는데 실패하면 커넥션 삭제
// //		AddDestroyConnectionForSelf( nullptr, spConnect );
// 	}
// 	return spNewUser;
// }

/**
 this에 연결된 모든 클라이언트 커넥션들의 패킷을 process()한다.
*/
void XEWinSocketSvr::Process( float dt )
{
	m_aryTime.clear();
	XPROF_OBJ_AUTO();
	//////////////////////////////////////////////////////////////////////////
	{
		XPROF_OBJ("m_pUserMng->Process");
		m_pUserMng->Process( dt );
	}
	//////////////////////////////////////////////////////////////////////////
	// 접속대기열의 커넥션들을 리스트에 등록시키고 유저객체를 만든다.
	ProcessConnectedList();
	//////////////////////////////////////////////////////////////////////////
	// Logined된 모든 커넥션들의 리스트를 루프돌며 패킷 펌핑.
	m_aryTime.clear();
	ProcessLoginedList();

	OnProcess( dt );		// virtual
	++m_Count;
} // process

void XEWinSocketSvr::ProcessConnectedList()
{
	XPROF_OBJ_AUTO();
// 	auto llMilli1 = GET_FREQ_TIME();
	// lock: 순서 바꾸지 말것. 순서바꾸면 데드락 가능성 있음.(묶음 락도 있어야 겠네),
	auto plistConnected = &m_Connected.m_shoList.GetSharedObj();
	auto plistLogined = &m_Logined.m_shoList.GetSharedObj();
	auto pmapConnected = &m_Connected.m_shoMap.GetSharedObj();
	auto pmapLogined = &m_Logined.m_shoMap.GetSharedObj();
	// 이게 제일먼저 한락인지 저번 턴에서 release빼먹은건 아닌지 검사.
	XBREAK( m_Logined.m_shoList.IsFirstLock() == FALSE );
	XBREAK( m_Connected.m_shoList.IsFirstLock() == FALSE );
	//
	XINT64 llStart = XE::GetFreqTime();;
	for( auto spConnect : *plistConnected ) {
		XINT64 llloop = XE::GetFreqTime();;
		if( llloop - llStart > 100000 )		// 처리속도가 너무 올래걸릴거 같으면 일단 그냥 루프 빠져나감.
			break;
		XAUTO_LOCK3( spConnect );
		const int cntUse = spConnect.use_count();
		const ID idConnect = spConnect->GetidConnect();
		XBREAK( idConnect == 0 );
		XBREAK( spConnect == nullptr );
#ifdef _DEBUG
		{ // connect만 된 객체 리스트에 logined된 객체가 있으면 안된다.
			// 				const auto spConnExist = _FindspConnInList( *plistLogined, idConnect );
			// 				if( XBREAK( spConnExist != nullptr ) ){
			// 					spConnExist->SetbDestroy( true );
			// 				}
		}
#endif // _DEBUG
		//
		if( spConnect->IsDisconnected() == FALSE ) {
			if( spConnect->GetbConnectedFlag() ) {
				SendConnectComplete( spConnect );		// 메인스레드에서 보내려고 이리로 옮김.
				spConnect->SetbConnectedFlag( false );;
			}
			// logined 리스트로 옮김
			plistLogined->Add( spConnect );
			// 맵에 등록
			auto itor = pmapLogined->find( idConnect );
			if( itor != pmapLogined->end() ) {
				XBREAK( 1 );
			}
			( *pmapLogined )[ idConnect ] = spConnect;
			// 대기열 검색맵에서 삭제
			const int sizeOld = pmapConnected->size();
			ReleaseConnectInMap( idConnect, *pmapConnected );
			const int sizeCurr = pmapConnected->size();
			//				XBREAK( sizeOld == pmapConnected->size() );
			// 클라로부터 커넥션이 연결을때 호출된다. (로그인했을때가 아님)
			OnLoginedFromClient( spConnect );
		}
		else {
			// 그새 접속이 끊겼으면 리스트에 추가하던거 취소하고 커넥션 바로 삭제.
			spConnect = nullptr;			// 안해도 아래 리스트 clear에서 자동으로 되지만 코드 읽기좋게 직접 명시함
		}
	} // for
	// 대기열 지움.
	plistConnected->clear();
	// unlock
	m_Logined.m_shoMap.ReleaseSharedObj();
	m_Connected.m_shoMap.ReleaseSharedObj();
	m_Logined.m_shoList.ReleaseSharedObj();
	m_Connected.m_shoList.ReleaseSharedObj();
// 	auto llPass = GET_FREQ_TIME() - llMilli1;
// 	m_aryTime.Add( (int)( llPass / 1000 ) );
} // listConnectionReady

void XEWinSocketSvr::ProcessLoginedList()
{
	XVector<XSPWinConnInServer> aryDestroy;
	auto llMilli1 = GET_FREQ_TIME();
	XPROF_OBJ( "listLogined" );
	auto plistLogined = &m_Logined.m_shoList.GetSharedObj();
	// 동접이 많으면 이 많은 커넥션들 다 패킷펌핑하기전까진 워커스레드 접속스레드 멈춰야 되는데... 이래선 싱글스레드와 다를바 없지 않은가.
	// 로그인된 커넥션들 프로세스
	XINT64 llProcessTotal = 0;
	XINT64 lllock = 0;
//	for( auto spConnect : *plistLogined ) {
	
	XINT64 llStart = XE::GetFreqTime();;
	for( auto itor = (*plistLogined).begin(); itor != (*plistLogined).end(); ) {
		auto spConnect = (*itor);
		auto pConnect = spConnect.get();
		XINT64 lllock1 = XE::GetFreqTime();;
		if( lllock1 - llStart > 100000 )		// 처리속도가 너무 올래걸릴거 같으면 일단 그냥 루프 빠져나감.
			break;
		pConnect->GetspLock()->Lock( __TFUNC__ );
		lllock += GET_FREQ_TIME() - lllock1;
		const int cntUse = spConnect.use_count();
		if( XASSERT( pConnect ) ) {
			if( pConnect->IsDisconnected() ) {
				OnDisconnectConnection( spConnect );
				pConnect->OnDisconnect();
			}
			if( !pConnect->GetbDestroy() ) {
				auto llProcess = GET_FREQ_TIME();
				pConnect->Process();
				llProcessTotal += (GET_FREQ_TIME() - llProcess);
				//
				if( pConnect->IsDisconnected() ) {	// 연결은 끊어졌어도 클라측에서 중요한 패킷을 보냈을수도 있으므로 패킷펌핑은 다 끝내고 삭제하도록 바뀜.
					pConnect->SetbDestroy( true );
				}
				// 커넥션이 비동기 파괴명령을 처리
				pConnect->ProcesssAsyncDisconnect();
			}
			if( pConnect->GetbDestroy() ) {
				++m_numDestroyAdd;
				aryDestroy.Add( spConnect );
				plistLogined->erase( itor++ );
			} else
				++itor;
		}
		pConnect->GetspLock()->Unlock();
	}

	auto llPass = GET_FREQ_TIME() - llMilli1;
	m_aryTime.Add( xProfile(_T("process list"), llPass ) );
	m_aryTime.Add( xProfile(_T("process"), llProcessTotal ) );
	m_aryTime.Add( xProfile( _T( "lock" ), lllock ) );
	m_aryTime.Add( xProfile( _T( "num process" ), plistLogined->size() ) );
	// Logined 프로세스 & destroy
	//////////////////////////////////////////////////////////////////////////
	auto pListConnected = &m_Connected.m_shoList.GetSharedObj();
	m_numConnected = pListConnected->size();		// 멀티스레드이므로 여기에 값이 있을 수 있음.
	if( m_numConnected > m_maxConnected )
		m_maxConnected = m_numConnected;
	m_Connected.m_shoList.ReleaseSharedObj();
	m_numLogined = plistLogined->size();
	if( m_numLogined > m_maxLogined )
		m_maxLogined = m_numLogined;
	// 순간적으로 numConnect수보다 커넥션객체의 생성수가 훨씬 많을때가 있다. 
	// leak으로 보이지만 봇 클라이언트를 종료시키는 순간 모두 사라지는걸로 보아 순간적으로 접속이 몰릴때 메인스레드 프로세스에서
	// 다 처리를 못해서 그런듯 하다.
	// unlock
	m_Logined.m_shoList.ReleaseSharedObj();
	// 삭제예정된 커넥션들 참조해제
	ProcessDestroyList( aryDestroy );
	aryDestroy.clear();
} // process & destroy

void XEWinSocketSvr::ProcessDestroyList( XVector<XSPWinConnInServer>& aryDestroy )
{
	m_cntDeleted = 0;
	m_cntDestroyed = 0;
	auto llMilli1 = GET_FREQ_TIME();
	XPROF_OBJ( "destroy conn" );
//	const int sizeList = plistLogined->size();
	//
	XINT64 llDestroy = 0;
	XINT64 llErase = 0;
	XINT64 llassign = 0;
	XINT64 llloop = 0;
	XINT64 lllock = 0;
	XINT64 llunlock = 0;
	XINT64 lldestroy2 = 0;
	XINT64 llloop1 = GET_FREQ_TIME();
//	for( auto itor = ( *plistLogined ).begin(); itor != ( *plistLogined ).end(); ) {
	for( auto spConnect : aryDestroy ) {
		llloop += ( GET_FREQ_TIME() - llloop1 );
// 		XINT64 llassign1 = GET_FREQ_TIME();
// 		auto spConnect = ( *itor );
// 		llassign += ( GET_FREQ_TIME() - llassign1 );
		const int cntUse = spConnect.use_count();
		bool bDestroy = false;
		// 객체 깨지기전에 미리 받아둠.
		XINT64 lllock1 = GET_FREQ_TIME();;
		spConnect->GetspLock()->Lock( __TFUNC__ );
		lllock += ( GET_FREQ_TIME() - lllock1 );
		const ID idConnect = spConnect->GetidConnect();
		// 연결끊기 & 커넥션의 삭제이벤트발생 & 유저객체 삭제 & 검색맵에서 삭제
		XINT64 lld = GET_FREQ_TIME();
		DestroyConnectProcess( spConnect );
		llDestroy += (GET_FREQ_TIME() - lld);
		if( cntUse == 2 )
			++m_cntDestroyed;
//				XINT64 lle = GET_FREQ_TIME();
//				plistLogined->erase( itor++ );	// 참조하는곳이 없다면 xWinConn이 파괴되면서 DeleteCriticalSection() 자동으로 이루어진다.
//				llErase += ( GET_FREQ_TIME() - lle );
		++m_cntDeleted;
		XINT64 llunlock1 = GET_FREQ_TIME();;
		spConnect->GetspLock()->Unlock();		// spConnect를 refCnt올리고 받았으므로 spConnect가 파괴되진 않는다.
		llunlock += ( GET_FREQ_TIME() - llunlock1 );
		XINT64 lldestroy2 = GET_FREQ_TIME();;
		spConnect.reset();
		lldestroy2 += ( GET_FREQ_TIME() - lldestroy2 );
		llloop1 = GET_FREQ_TIME();
		// spConnect 파괴
	} // for
	auto llPass = GET_FREQ_TIME() - llMilli1;
	m_aryTime.Add( xProfile(_T("destroy list"), llPass ) );
	m_aryTime.Add( xProfile(_T("destroy"), llDestroy ) );
	m_aryTime.Add( xProfile(_T("erase"), llErase ) );
	m_aryTime.Add( xProfile( _T("assign"), llassign ) );
	m_aryTime.Add( xProfile( _T( "loop" ), llloop ) );
	m_aryTime.Add( xProfile( _T( "lock" ), lllock ) );
	m_aryTime.Add( xProfile( _T( "unlock" ), llunlock ) );
	m_aryTime.Add( xProfile( _T( "destroy2" ), lldestroy2 ) );
	m_aryTime.Add( xProfile( _T( "num erase" ), m_cntDeleted ) );
	m_aryTime.Add( xProfile( _T( "num destroy" ), m_cntDestroyed ) );
}

/**
 @brief 커넥션 삭제 프로세스
*/
void XEWinSocketSvr::DestroyConnectProcess( XSPWinConnInServer spConnect )
{
//	auto spConnect = spConnect;
	const ID idConnect = spConnect->GetidConnect();
	spConnect->OnDestroy();
	OnDestroyConnection( spConnect );
	if( spConnect->GetbDestroy() ) {
		spConnect->DoDisconnect();		// 딴데서 참조하고 있을수 있으니 연결은 일단 끊는다.
	} else
	if( spConnect->IsDisconnected() ) {
		spConnect->ClearConnection();
	}
	// 맵에서 삭제
	ReleaseConnectInMap( idConnect, m_Logined.m_shoMap );
	// 유저객체 삭제
	auto spUser = spConnect->GetspUser();
	if( spUser ) {
		const ID idAcc = spUser->GetspDBAccountConst()->GetidAccount();
		ReleaseConnectInMap( idAcc, m_Logined.m_shoMapByidAcc );
		DelUserProcess( spUser );
	}
	// 참조해제
	//					conn.m_spConnect = nullptr;
//#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	CONSOLE_TAG( "connect", "%s:release connect: id=0x%08x, refCnt=%d, ip=%s",
												__TFUNC__,
												spConnect->GetidConnect(),
												spConnect.use_count(),
												spConnect->GetszIP() );
//#endif				
}

void XEWinSocketSvr::ReleaseConnectInMap( ID idKey, XMap_Connection& mapConnects )
{
	mapConnects.erase( idKey );
// 	auto itor = mapConnects.find( idConnect );
// 	if( XASSERT(itor != mapConnects.end()) ) {
// 		mapConnects.erase( itor );
// 	}
}
void XEWinSocketSvr::ReleaseConnectInMap( ID idKey, XSharedObj<XMap_Connection>& shoMapConnects )
{
	auto pMap = &shoMapConnects.GetSharedObj();
	if( XASSERT(pMap) ) {
		ReleaseConnectInMap( idKey, *pMap );
	}
	shoMapConnects.ReleaseSharedObj();
}

//
void XEWinSocketSvr::DelUserProcess( XSPUserBase spUser )
{
	if( XBREAK(spUser == nullptr) )
		return;
	auto spConnect = spUser->GetspConnect();
	// spUser에게 삭제 이벤트를 보내고 리스트에서 삭제
	const ID idUser = spUser->GetidUser();
	m_pUserMng->DelUser( spUser );
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	// 사람많으면 느려지니까 개발중에만 나오도록 바꿈.
//	auto spConnect = spUser->GetspConnect();
	if( XASSERT(spConnect) ) {
		TCHAR szLog[ 1024 ];
		_tcscpy_s( szLog, XFORMAT( "usermng del user: %s, idAcc=%d", spConnect->GetszIP(), idUser ) );
		XEServerMain::sGet()->DoLog( XE::xSLOG_DESTROY_USER, szLog, ( DWORD ) this );
	}
#endif					
}

/**
 @brief 클라로부터 로그인이 성공
*/
void XEWinSocketSvr::OnLoginedFromClient( XSPWinConnInServer spConnect )
{
}

// 커넥션 아이디로 커넥션 객체를 찾는다. listConnection과 대기열 모두에서 찾는다.
XSPWinConnInServer XEWinSocketSvr::FindspConnect( ID idConnect )
{
	{
		XAutoSharedObj<XMap_Connection> shaAuto( m_Connected.m_shoMap );
		auto pMap = shaAuto.Get();
		auto spConnect = _FindspConnInMap( *pMap, idConnect );
		if( spConnect )
			return spConnect;
	}
	XAutoSharedObj<XMap_Connection> shaAuto( m_Logined.m_shoMap );
	auto pMap = shaAuto.Get();
	return _FindspConnInMap( *pMap, idConnect );
}

// 커넥션 아이디로 커넥션 객체를 찾는다.(범용 대기열에서만 찾는다.
XSPWinConnInServer XEWinSocketSvr::_FindspConnInList( const XList4<XSPWinConnInServer>& listConn, ID idConnectFind )
{
	for( const auto& spConnect : listConn ) {
		const ID idConnect = spConnect->GetidConnect();
		XBREAK( idConnect == 0 );
		if( idConnect == idConnectFind )
			return spConnect;
	}
	return nullptr;
}

XSPWinConnInServer XEWinSocketSvr::_FindspConnInMap( const XMap_Connection& mapConn, ID idConnectFind )
{
	auto itor = mapConn.find( idConnectFind );
	if( itor != mapConn.end() ) {
		return itor->second;
	}
	return nullptr;
}
/**
 @brief idAcc로 커넥션을 찾는다.
*/
XSPWinConnInServer XEWinSocketSvr::GetspConnByIdAcc( ID idAcc )
{
  auto pConn = _FindspConnInConnectedByidAcc( idAcc );
  if( pConn )
    return pConn;
  pConn = _FindspConnInLoginedByidAcc( idAcc );
  return pConn;
}

/**
 @brief 대기열에서 idAcc로 커넥션을 찾는다.
*/
XSPWinConnInServer XEWinSocketSvr::_FindspConnInConnectedByidAcc( ID idAcc )
{
	XAutoSharedObj<XMap_Connection> autoShared( m_Connected.m_shoMapByidAcc );
  auto pMap = autoShared.Get();
	auto itor = pMap->find( idAcc );
	if( itor != pMap->end() ) {
		return itor->second; 
	}
	return nullptr;
}

/**
 @brief idAcc로 커넥션을 찾는 버전
 접속이 완전히 이루어지지 않아서 User객체가 없을때 사용하는 버전
*/
XSPWinConnInServer  XEWinSocketSvr::_FindspConnInLoginedByidAcc( ID idAcc )
{
	XAutoSharedObj<XMap_Connection> autoShared( m_Logined.m_shoMapByidAcc );
	auto pMap = autoShared.Get();
	auto itor = pMap->find( idAcc );
	if( itor != pMap->end() ) {
		return itor->second;
// 		if( XASSERT( XE::IsValidPtr( spConnect.get() ) ) ) {
// 			return spConnect;
// 		}
	}
	return nullptr;
}

int XEWinSocketSvr::GetNumConnection()
{ 
	XAutoSharedObj<XList_Connection> autoObj( m_Logined.m_shoList );
	return autoObj.Get()->size();
}

// void XEWinSocketSvr::DrawConnections( _tstring* pOutStr )
// {
// 	{
// 		auto plist = &m_Connected.m_shoList.GetSharedObj();
// 		for( auto& spConnect : *plist ) {
// 			( *pOutStr ) += XE::Format( _T( "    ID:%d connect, sizeQ:%d\n" )
// 				, spConnect->GetGSvrID()
// 				, spConnect->GetSizeQueue() );
// 			// 출력하고 지움.
// 			spConnect->ClearSizeMaxQ();
// 		}// END_LOOP;
// 		m_Connected.m_shoList.ReleaseSharedObj();
// 	}
// 
// }