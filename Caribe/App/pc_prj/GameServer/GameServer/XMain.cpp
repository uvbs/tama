#include "stdafx.h"
#include "XMain.h"
#include "XSocketSvrForClient.h"
#include "XClientConnection.h"
#include "XGame.h"
#include "MainFrm.h"
#include "XPool.h"
#include "XGameUser.h"
#include "server/XSAccount.h"
#include "XExpTableHero.h"
#include "XRanking.h"
#include "server/XGuildMgr.h"
#include "XCrypto.h"
#include "XLoginConnection.h"
#include "XDBASvrConnection.h"
#include "XFramework/MFC/XEServerApp.h"
#include "XGameUserMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _XNOT_BREAK
#pragma message("defined _XNOT_BREAK..........................................")
#endif

XMain* XMain::s_pInstance = nullptr;
XMain						*MAIN = NULL;
XLoginConnection		*LOGIN_SVR = NULL;
//XDBASvrConnection	*DBA_SVR = NULL;



extern XRanking* RANKING_DB;
//extern XGuildMgr* GUILD_MGR;
//////////////////////////////////////////////////////////////////////////
// static 
void XMain::sCreateMemoryPool( void )
{
	// 엔진이 초기화 되기 이전에 호출됨.
	// 메모리 풀 할당.
	int maxConnect = XEnv::sGet()->GetmaxConnect();
#ifdef _XBOT
	if( maxConnect < 10000 )
		maxConnect = 10000;
#endif // _XBOT
	XClientConnection::s_pPool = new XPool<XClientConnection>( maxConnect );
	XBREAK( XClientConnection::s_pPool == NULL );
	CONSOLE( "XClientConnection pool:%d KB", (sizeof( XClientConnection ) * maxConnect) / 1000 );
	//
	XGameUser::s_pPool = new XPool<XGameUser>( maxConnect );
	XBREAK( XGameUser::s_pPool == NULL );
	CONSOLE( "XUser pool:%d KB", (sizeof( XGameUser ) * maxConnect) / 1000 );

	XSAccount::s_pPool = new XPool<XSAccount>( maxConnect );
	XBREAK( XSAccount::s_pPool == NULL );
	CONSOLE( "XAccount pool:%d KB", (sizeof( XSAccount ) * maxConnect) / 1000 );
}

void XMain::sDestroyMemoryPool( void )
{
	SAFE_DELETE( XGameUser::s_pPool );		// 1.
	SAFE_DELETE( XClientConnection::s_pPool );	// 2.
	SAFE_DELETE( XSAccount::s_pPool );	// 5
}
// static 
//////////////////////////////////////////////////////////////////////////

void XMain::Destroy() 
{
// UserMng 생기면 이 주석 풀어 줘야 한다.
//	if( GetpSocketForClient())
//		GetpSocketForClient()->GetpUserMng()->Save();
  SAFE_DELETE( RANKING_DB );
  XGuildMgr::sDestroy();
// 	SAFE_RELEASE_REF( LOGIN_SVR );
//	SAFE_RELEASE_REF( DBA_SVR );
}

// This is GameServer
// 생성자 다음에 최초 호출
void XMain::Create( LPCTSTR szINI )
{
	// ini 로딩
//	XEServerMain::LoadINI( szINI );	// CreateMemoryPool에서 ini값을 쓰기때문에 먼저 불려야 함.
	XEnv::sGet()->Load2( szINI );
	XAccount::s_secOfflineForSimul = XEnv::sGet()->GetParam().GetInt( "sec_simul" );
// 	if( XAccount::s_secOfflineForSimul == 0 )
// 		XAccount::s_secOfflineForSimul = 3600;
	UpdateSleepMain( XEnv::sGet().get() );
	XEUserMng::sResetSecAutoSave( XEnv::sGet()->GetParam().GetFloat("sec_auto_save") );
	CONSOLE("set auto save: %.1f sec", XEUserMng::sGetsecAutoSave() );
	// 메모리풀 생성
	XMain::sCreateMemoryPool();
	// 컨텐츠 객체 생성, DidFinishCreate()호출 
	XEServerMain::Create( szINI );

	RANKING_DB = XRanking::sCreateInstance();	
	{
		auto spMgr = XGuildMgr::sGet();		// 안전한 시점에 생성시키려는 의도. 안해도 될듯.
	}
#ifdef _DUMMY_GENERATOR
	LoadDummyNames();
#endif // _DUMMY_GENERATOR
}

#ifdef _DUMMY_GENERATOR
void XMain::LoadDummyNames()
{
	CToken token;
	if( token.LoadFile( PATH_PROP("names.txt"), XE::TXT_UTF16 ) ) {
		while( token.GetToken() ) {
			if( XE::IsHave(token.m_Token) ) {
				m_aryNames.Add( _tstring(token.m_Token) );
			}
		}
	}
}
#endif // _DUMMY_GENERATOR

void XMain::ReloadIni()
{
	XEnv::sGet()->Reload2();
	UpdateSleepMain( XEnv::sGet().get() );
	UpdateSleepServer( XEnv::sGet().get(), m_pSocketForClient );
	XEUserMng::sResetSecAutoSave( XEnv::sGet()->GetParam().GetFloat( "sec_auto_save" ) );
	CONSOLE( "set auto save: %.1f sec", XEUserMng::sGetsecAutoSave() );
}

void XMain::UpdateSleepServer( const XEOption* pEnv, XEWinSocketSvr* pSocketSvr )
{
	if( XASSERT( pSocketSvr ) ) {
		const auto msec1 = pEnv->GetmsecSleep( KEY_SLEEP_SVR_CONN );
		const auto msec2 = pEnv->GetmsecSleep( KEY_SLEEP_SVR_RECV );
		pSocketSvr->SetmsecSleepConn( msec1 );		// 로긴서버에선 lock간섭을 줄이려고 좀 늦춤.
		pSocketSvr->SetmsecSleepRecv( msec2 );
	}
}

void XMain::UpdateSleepMain( const XEOption* pEnv )
{
	const auto msec = pEnv->GetParam().GetInt( KEY_SLEEP_MAIN );
	APP->SetmsecSleepMain( msec );
}

/**
 
*/
// BOOL XMain::LoadINI( CToken &token )
// {	
// 	m_GSvrID = (WORD)token.GetNumber();
// 	XBREAK( m_GSvrID == 0 );
// 	XBREAK( m_GSvrID > 1000 );
// 	m_maxConnect = token.GetNumber();
// 	m_PortLoginSvr = (WORD)token.GetNumber();
// 	m_strIPLoginSvr = SZ2C( token.GetToken() );
// 	XBREAK( m_strIPLoginSvr.empty() );
// 	m_PortForClient = token.GetNumber();
// 	LPCTSTR szIPExternal = token.GetToken();
// 	XBREAK( XE::IsEmpty( szIPExternal ) == TRUE );
// 	m_strIPExternal = SZ2C( szIPExternal );
// 	m_PortDBASvr = token.GetNumber();
// 	m_strIPDBASvr = SZ2C( token.GetToken() );
// 	XBREAK( m_strIPDBASvr.empty() );
// 	LPCTSTR szKey = token.GetToken();
// 	if( szKey )
// 		m_strPublicKey = SZ2C(szKey);
// 	TRACE("publicKey=%s\n", m_strPublicKey.c_str() );
// 
// 	// 로긴시뮬에선 게임서버 아이피가 있었지만
// 	// 실제서버에선 접속하는 게임서버의 아이피를 따로 받아둬야 한다.
// 	CONSOLE( "max connect=%d", m_maxConnect );
// 	return TRUE;
// }

/**
 로그인서버(소켓)로 접속하는 커넥션 생성하고 접속시도
 접속이 성공하면 메인스레드에서 DelegateConnect()가 발생한다.
*/
void XMain::DoConnectLoginServer( void )
{
	const _tstring strIPLoginSvr = C2SZ( XEnv::sGet()->GetstrIPLoginSvr() );
	CONSOLE( "try connecting to login server: %s:%d"
				, strIPLoginSvr.c_str()
				, XEnv::sGet()->GetPortLoginSvr() );
	LOGIN_SVR = new XLoginConnection();
	LOGIN_SVR->OnCreate( XEnv::sGet()->GetstrIPLoginSvr(), XEnv::sGet()->GetPortLoginSvr(), this );
	LOGIN_SVR->TryConnect();
	// 타임아웃 처리
}

void XMain::DoConnectDBAServer( void )
{
#ifndef _XRECONNECT
	XBREAK( DBA_SVR != NULL );	// 값이 있는채로 들어오면 안된다. release를 하기땜에.
	SAFE_RELEASE_REF( DBA_SVR );
#endif // xreconnect
	const _tstring strIPDBASvr = C2SZ( XEnv::sGet()->GetstrIPDBASvr() );
	const WORD portDBASvr = XEnv::sGet()->GetPortDBASvr();
	CONSOLE( "try connecting to Db Agent server: %s:%d", strIPDBASvr.c_str(), portDBASvr );
	//
	XDBASvrConnection::sGet()->OnCreate( XEnv::sGet()->GetstrIPDBASvr(), portDBASvr, this );
	// 별도 스레드에서 connect시도 시작
	XDBASvrConnection::sGet()->TryConnect();
// 	DBA_SVR = new XDBASvrConnection( m_strIPDBASvr.c_str()
// 																, m_PortDBASvr
// 																, this );
// 	DBA_SVR->TryConnect();
	// 타임아웃 처리
}

/**
 접속을 시도한 서버에 연결이 되었고 CONNECT_COMPLETE패킷까지 받음.
*/
void XMain::DelegateConnect( XENetworkConnection *pConnect, DWORD param )
{
	auto pEnv = XEnv::sGet().get();
	if( pConnect->GetidConnect() == LOGIN_SVR->GetidConnect() )	{
		// LOGIN_SVR 소켓에 connect가 됨
		// 클라로부터 접속을 받기시작할 소켓객체 생성
		if( m_pSocketForClient == NULL )	{
			auto pSvr = new XSocketSvrForClient();
			m_pSocketForClient = pSvr;
			UpdateSleepServer( pEnv, pSvr );
			// 클라이언트가 접속해야할 게임서버의 포트는 8410 + idSvr가 된다.
			WORD port = pEnv->GetPortForClient() + (pEnv->GetGSvrID() - 1);
			const int numThread = pEnv->GetParam().GetInt( "num_recv_thread" );
			pSvr->Create( port, numThread );
			AddSocketServer( pSvr );
			CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
			pMainFrm->XEServerMainFrame::SetTitle();
		} else {
			// 로긴서버 끊겨서 재접속한 상황.
		}
		// 게임서버의 모든 로딩이 끝난후 로그인서버로부터 데이타를 받을 준비가 되면 로그인서버로 패킷 날림
		LOGIN_SVR->SendDidFinishLoad();
		if( LOGIN_SVR->GetbReconnected() ) {
	#ifdef _XPACKET_BUFFER
			// 끊어진동안 쌓였던 패킷들 모두 보냄.
			LOGIN_SVR->FlushSendPacket();
	#endif
		}
		//
		CONSOLE("로그인서버에 연결되었습니다.");
	} else
	if( pConnect->GetidConnect() == DBA_SVR->GetidConnect() )
	{
		// DBA_SVR 소켓에 connect가 됨
		CONSOLE("DBAgent서버에 연결되었습니다.");
	}
}

void XMain::DidFinishCreate( void )
{
	XBREAK( XEnv::sGet()->GetPortForClient() == 0 );
	DoConnectLoginServer();
	DoConnectDBAServer();
	UpdateTitle();
}


// 게임 객체 생성
XEContents* XMain::CreateContents( void )
{
	//	XE::g_Lang = XE::xLANG_KOREAN;
	// 게임 객체 생성
	XGame::sCreateInstance();
	XGame::sGet()->Create();
	return XGame::sGet();
}

void XMain::Process( void )
{
	if( LOGIN_SVR ) {
		LOGIN_SVR->Process();
	}
	XDBASvrConnection::sGet()->Process();
	XEServerMain::Process();
	if (m_timerRank.IsOff()) {
		m_timerRank.Set(10);
	}
	if (m_timerRank.IsOver()) {
		// 여기다 코딩			
		XDBASvrConnection::sGet()->SendRequestRankInfo();
//			CONSOLE("랭킹 정보 요청함");
		m_timerRank.Reset();
	}
}

void XMain::ConsoleMessage( LPCTSTR szMsg )
{
#ifdef _XBOT
	XTRACE( "%s", szMsg );
	return;
#endif // _BOT
	// MFC쪽이 멀티스레드가 아직 안되서 막아둠.
	if( GetDlgConsole() )
		GetDlgConsole()->MessageString( szMsg );
}

/**
 @brief 암호화된 퍼블릭키를 만들어 pOut에 담는다.
 @param sizeMax pOut의 버퍼 최대 크기
 @return pOut버퍼의 크기를 리턴한다.
*/
int XMain::GetPublicKeyWithEncrypted( XCrypto* pCryptObj, BYTE* pOut, const int sizeMax )
{
	const auto strPublicKey = XEnv::sGet()->GetstrPublicKey();
	XBREAK( strPublicKey.empty() );
	char cStr[ 4096 ] = {0,};
	const int sizeKey = strPublicKey.length();
	strcpy_s( cStr, strPublicKey.c_str() );

	const int sizeEncrypt = pCryptObj->EncryptMem( pOut, sizeMax
																							, (BYTE*)cStr, sizeKey, 0 );
	return sizeEncrypt;

}