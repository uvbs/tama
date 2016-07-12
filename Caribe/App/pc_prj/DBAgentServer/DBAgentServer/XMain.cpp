#include "stdafx.h"
#include "XMain.h"
#include "XSocketForGameSvr.h"
#include "XGameSvrConnection.h"
#include "XGame.h"
#include "XDBUAccount.h"
#include "XFramework/XDBMng2.h"
#include "MainFrm.h"
#include "XFramework/XHttpMng.h"
#include "XFramework/MFC/XEServerApp.h"

#include "XRanking.h"
#include "server/XGuildMgr.h"

#ifdef _XMNGTOOL
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XMain *MAIN = NULL;

extern XRanking* RANKING_DB;
//extern XGuildMgr* GUILD_MGR;
//////////////////////////////////////////////////////////////////////////
// static 
void XMain::sCreateMemoryPool( void )
{
	// 엔진이 초기화 되기 이전에 호출됨.
	// 메모리 풀 할당.
/*
	XClientConnection::s_pPool = new XPool<XClientConnection>( MAIN->GetmaxClient() );
	XBREAK( XClientConnection::s_pPool == NULL );
	CONSOLE( "XClientConnection pool:%d KB", (sizeof( XClientConnection ) * MAIN->GetmaxClient()) / 1000 );
*/
#pragma message("시스템 메모리 확인해서 남은메모리에 따라 메모리풀 크기를 다르게 해야함.")
	int maxPool = MAIN->GetmaxPool();
#ifdef _XBOT
	if( maxPool < 10000 )
		maxPool = 10000;
#endif // _XBOT
	XDBUAccount::s_pPool = new XPool<XDBUAccount>( maxPool );
	XBREAK( XDBUAccount::s_pPool == NULL );
	CONSOLE( "XDBUAccount pool:%d KB", (sizeof( XDBUAccount ) * maxPool) / 1000 );
}

void XMain::sDestroyMemoryPool( void )
{
 	SAFE_DELETE( XDBUAccount::s_pPool );
//	SAFE_DELETE( XClientConnection::s_pPool );	// 2.
}
// static 
//////////////////////////////////////////////////////////////////////////

void XMain::Destroy() 
{
}
// This is DBAServer
// 생성자 다음에 최초 호출
void XMain::Create( LPCTSTR szINI )
{
	// ini 로딩
//	XEServerMain::LoadINI( szINI );	// CreateMemoryPool에서 ini값을 쓰기때문에 먼저 불려야 함.
	m_Env.Load2( szINI );
	const int numDBThread = m_Env.GetParam().GetInt( "num_db_thread" );
	XDBMng2<XGameSvrConnection>::sGet()->Create( numDBThread );
	UpdateSleepMain( &m_Env );
	UpdateSleepDBMng( &m_Env );
	// 메모리풀 생성
	XMain::sCreateMemoryPool();
	// 컨텐츠 객체 생성, DidFinishCreate()호출 
	XEServerMain::Create( szINI );
	RANKING_DB = XRanking::sCreateInstance();
	auto pMgr = XGuildMgr::sGet();		// 안전한시점에 생성시키려는 의도.
}

XSocketForGameSvr* XMain::GetpSocketForGameSvr() const
{
	return XSocketForGameSvr::sGet().get();
}

void XMain::ReloadIni()
{
	m_Env.Reload2();
	UpdateSleepMain( &m_Env );
	UpdateSleepDBMng( &m_Env );
	UpdateSleepServer( &m_Env, GetpSocketForGameSvr() );
}

void XMain::UpdateSleepDBMng( const XEOption* pEnv )
{
	const auto msec1 = pEnv->GetmsecSleep( KEY_SLEEP_DBMNG_READY );
	const auto msec2 = pEnv->GetmsecSleep( KEY_SLEEP_DBMNG_PUMP );
	XDBMng2<XGameSvrConnection>::sGet()->SetmsecSleepReady( msec1 );		// 로긴서버에선 lock간섭을 줄이려고 좀 늦춤.
	XDBMng2<XGameSvrConnection>::sGet()->SetmsecSleepPump( msec2 );
}

void XMain::UpdateSleepServer( const XEOption* pEnv, XEWinSocketSvr* pSocketSvr )
{
	if( XASSERT(pSocketSvr) ) {
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
// 	m_Port = token.GetNumber();
// 	m_strIP = SZ2C(token.GetToken());
// //	m_ServerID = token.GetNumber();
// 	m_maxPool = token.GetNumber();
// 	if( m_maxPool == 0 )
// 		m_maxPool = 100;
// 	return TRUE;
// }

bool XMain::DelegateDidFinishLoad( const XParamObj2& param )
{
	//	m_maxGSvrConnect = param.GetWord( "max_gamesvr" );
	m_Port = param.GetWord( "port_for_gamesvr" );
	XBREAK( m_Port == 0 );
//	m_strIP = param.GetStrc( "ip_")
	m_maxPool = param.GetInt( "max_acc_pool" );
	if( m_maxPool == 0 )
		m_maxPool = 0;
	CONSOLE( "max acc pool=%d", m_maxPool );
	return true;
}

void XMain::DidFinishCreate( void )
{
	XBREAK( m_Port == 0 );
//	Http관리자
//	XHttpMng::sInit( 4 );		// 커넥션 스레드 4개로 생성
	//
	// 게임서버들이 접속해오는 소켓 생성
//	auto pSocketForClient = new XSocketForGameSvr();
	auto pSocketForGameSvr = XSocketForGameSvr::sGet().get();
//	m_pSocketForGameSvr = pSocketForClient;
	const int numThread = m_Env.GetParam().GetInt( "num_recv_thread" );
	pSocketForGameSvr->Create( m_Port, numThread );
	AddSocketServer( pSocketForGameSvr );	
	UpdateSleepServer( &m_Env, pSocketForGameSvr );
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
	XDBMng2<XGameSvrConnection>::sGet()->Process();
	XEServerMain::Process();
}

void XMain::ConsoleMessage( LPCTSTR szMsg )
{
#ifdef _XBOT
	XTRACE("%s", szMsg );
	return;
#endif // _BOT
	// MFC쪽이 멀티스레드가 아직 안되서 막아둠.
	if( GetDlgConsole() )
		GetDlgConsole()->MessageString( szMsg );
}

