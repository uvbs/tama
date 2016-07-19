#include "stdafx.h"
#include "XMain.h"
#include "XSocketForClient.h"
#include "XClientConnection.h"
#include "XDBASvrConnection.h"
#include "XLoginConnection.h"
#include "XGame.h"
#include "MainFrm.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XMain *MAIN = NULL;

//////////////////////////////////////////////////////////////////////////
// static 
void XMain::sCreateMemoryPool()
{
	// 엔진이 초기화 되기 이전에 호출됨.
	// 메모리 풀 할당.
/*
	XClientConnection::s_pPool = new XPool<XClientConnection>( MAIN->GetmaxClient() );
	XBREAK( XClientConnection::s_pPool == NULL );
	CONSOLE( "XClientConnection pool:%d KB", (sizeof( XClientConnection ) * MAIN->GetmaxClient()) / 1000 );
*/
}

void XMain::sDestroyMemoryPool()
{
//	SAFE_DELETE( XClientConnection::s_pPool );	// 2.
}
// static 
//////////////////////////////////////////////////////////////////////////

void XMain::Destroy() 
{
	XDBASvrConnection::sDestroySingleton();
}


// 생성자 다음에 최초 호출
void XMain::Create( LPCTSTR szINI )
{
	// ini 로딩
	XEServerMain::LoadINI( szINI );	// CreateMemoryPool에서 ini값을 쓰기때문에 먼저 불려야 함.
	// 메모리풀 생성
	XMain::sCreateMemoryPool();
	// 컨텐츠 객체 생성, DidFinishCreate()호출 
	XEServerMain::Create( szINI );
}
/**
 
*/
BOOL XMain::LoadINI( CToken &token )
{	
	m_GSvrID = (WORD)token.GetNumber();
	m_maxConnect = token.GetNumber();
	XBREAK( m_maxConnect == 0 );
	m_PortLoginSvr = (WORD)token.GetNumber();
	XBREAK( m_PortLoginSvr == 0 );
	m_strIPLoginSvr = SZ2C( token.GetToken() );
	XBREAK( m_strIPLoginSvr.empty() );
	m_PortForClient = token.GetNumber();
	XBREAK( m_PortForClient == 0 );
	LPCTSTR szIPExternal = token.GetToken();
	m_strIPExternal = SZ2C( szIPExternal );
	XBREAK( m_strIPExternal.empty() );	// win32폴더의 해당서버 ini에 외부접속 ip를 넣으시오.
	m_PortDBASvr = token.GetNumber();
	XBREAK( m_PortDBASvr == 0 );
	m_strIPDBASvr = SZ2C( token.GetToken() );
	XBREAK( m_strIPDBASvr.empty() );
	LPCTSTR szKey = token.GetToken();
	if( szKey )
		m_strPublicKey = SZ2C( szKey );
	TRACE( "publicKey=%s\n", m_strPublicKey.c_str() );

	// 로긴시뮬에선 게임서버 아이피가 있었지만
	// 실제서버에선 접속하는 게임서버의 아이피를 따로 받아둬야 한다.
	CONSOLE( "max connect=%d", m_maxConnect );
	return TRUE;
}

void XMain::DidFinishCreate()
{
	DoConnectLoginServer();
	DoConnectDBAServer();
	UpdateTitle();
}

/**
 로그인서버(소켓)로 접속하는 커넥션 생성하고 접속시도
 접속이 성공하면 메인스레드에서 DelegateConnect()가 발생한다.
*/
void XMain::DoConnectLoginServer( void )
{
	XLoginConnection::sCreateSingleton( m_strIPLoginSvr, m_PortLoginSvr, this );
	XLoginConnection::sGet()->TryConnect();
	// 타임아웃 처리
}

void XMain::DoConnectDBAServer()
{
	XDBASvrConnection::sCreateSingleton( m_strIPDBASvr.c_str(), m_PortDBASvr, this );
	XDBASvrConnection::sGet()->TryConnect();
}

/**
 접속을 시도한 서버에 connect()가 이루어 졌다.
*/
void XMain::DelegateConnect( XENetworkConnection *pConnect, DWORD param )
{
	if( pConnect->GetidConnect() == XLoginConnection::sGet()->GetidConnect() ) {
		// LOGIN_SVR 소켓에 connect가 됨
		// 클라로부터 접속을 받기시작할 소켓객체 생성
		if( m_pSocketForClient == nullptr ) {
			XSocketForClient *pSvr = new XSocketForClient( m_PortForClient );
			m_pSocketForClient = pSvr;
			pSvr->Create();
			AddSocketServer( pSvr );
			auto pMainFrm = static_cast<CMainFrame*>( AfxGetMainWnd() );
			pMainFrm->XEServerMainFrame::SetTitle();
		} else {
			// 로긴서버 끊겨서 재접속한 상황.
		}
		// 게임서버의 모든 로딩이 끝난후 로그인서버로부터 데이타를 받을 준비가 되면 로그인서버로 패킷 날림
		XLoginConnection::sGet()->SendDidFinishLoad();
		if( XLoginConnection::sGet()->GetbReconnected() ) {
	#ifdef _XPACKET_BUFFER
			// 끊어진동안 쌓였던 패킷들 모두 보냄.
			XLoginConnection::sGet()->FlushSendPacket();
	#endif
		}
		//
		CONSOLE("로그인서버에 연결되었습니다.");
	} else
	if( pConnect->GetidConnect() == XDBASvrConnection::sGet()->GetidConnect() ) {
		// DBA_SVR 소켓에 connect가 됨
		CONSOLE("DBAgent서버에 연결되었습니다.");
	}
}

// 게임 객체 생성
XEContents* XMain::CreateContents()
{
	//	XE::g_Lang = XE::xLANG_KOREAN;
	// 게임 객체 생성
	XGame::sCreateInstance();
	XGame::sGet()->Create();
	return XGame::sGet();
}

void XMain::Process()
{
	XLoginConnection::sGet()->Process();
	XDBASvrConnection::sGet()->Process();
	//
	XEServerMain::Process();
}

void XMain::ConsoleMessage( LPCTSTR szMsg )
{
	if( GetDlgConsole() )
		GetDlgConsole()->MessageString( szMsg );
}

