#include "stdafx.h"
#include "XMain.h"
#include "XSocketSvrForClient.h"
#include "XGameSvrSocket.h"
#include "XClientConnection.h"
#include "XGameSvrConnection.h"
#include "XGame.h"
#include "XFramework/XDBMng2.h"
#include "MainFrm.h"
#include "XFramework/XHttpMng.h"
#include "XFramework/MFC/XEServerApp.h"
#ifdef _DUMMY_GENERATOR
  #include "XGameSvrConnection.h"
  #include "XGameSvrSocket.h"
  #include "XPacketLG.h"
	#include "XUserDB.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<XMain> MAIN;

//////////////////////////////////////////////////////////////////////////
// static 
//std::shared_ptr<XMain> XMain::s_spMain;
void XMain::sCreateMemoryPool( void )
{
	// 엔진이 초기화 되기 이전에 호출됨.
	// 메모리 풀 할당.
	int maxConnect = MAIN->GetmaxConnect();
#ifdef _XBOT
	if( maxConnect < 10000 )
		maxConnect = 10000;
#endif // _XBOT
	XClientConnection::s_pPool = new XPool<XClientConnection>( maxConnect );
	XBREAK( XClientConnection::s_pPool == NULL );
	CONSOLE( "XClientConnection pool:%d KB", (sizeof(XClientConnection) * maxConnect) / 1000 );
	XDBUAccount::s_pPool = new XPool<XDBUAccount>( maxConnect );
	XBREAK( XDBUAccount::s_pPool == NULL );
	CONSOLE( "XDBUAccount pool:%d KB", (sizeof( XDBUAccount ) * maxConnect) / 1000 );
}

void XMain::sDestroyMemoryPool( void )
{
	SAFE_DELETE( XDBUAccount::s_pPool );	// 2.
	SAFE_DELETE( XClientConnection::s_pPool );	// 2.
}
// static 
//////////////////////////////////////////////////////////////////////////

void XMain::Destroy() 
{
}

// This is LoginServer
// 생성자 다음에 최초 호출
void XMain::Create( LPCTSTR szINI )
{
	// ini 로딩
//	XEServerMain::LoadINI( szINI );	// CreateMemoryPool에서 ini값을 쓰기때문에 먼저 불려야 함.
	m_Env.Load2( szINI );
	const int numDBThread = m_Env.GetParam().GetInt( "num_db_thread");
	XDBMng2<XGameSvrConnection>::sGet()->Create( numDBThread );
	XDBMng2<XClientConnection>::sGet()->Create( numDBThread );
	XDBMng2<XMain>::sGet()->Create( numDBThread );
	UpdateSleepMain( &m_Env );
	UpdateSleepDBMng( &m_Env );
	// 메모리풀 생성
	XMain::sCreateMemoryPool();
	// 컨텐츠 객체 생성, DidFinishCreate()호출 
	XEServerMain::Create( nullptr );
}

void XMain::ReloadIni()
{
	m_Env.Reload2();
	UpdateSleepMain( &m_Env );
	UpdateSleepDBMng( &m_Env );
	UpdateSleepServer( &m_Env, m_pSocketForGameSvr );
	UpdateSleepServer( &m_Env, m_pSocketForClient );
}

void XMain::UpdateSleepDBMng( const XEOption* pEnv )
{
	const auto msec1 = pEnv->GetmsecSleep( KEY_SLEEP_DBMNG_READY );
	const auto msec2 = pEnv->GetmsecSleep( KEY_SLEEP_DBMNG_PUMP );
	XDBMng2<XGameSvrConnection>::sGet()->SetmsecSleepReady( msec1 );		// 로긴서버에선 lock간섭을 줄이려고 좀 늦춤.
	XDBMng2<XGameSvrConnection>::sGet()->SetmsecSleepPump( msec2 );
	XDBMng2<XClientConnection>::sGet()->SetmsecSleepReady( msec1 );		// 로긴서버에선 lock간섭을 줄이려고 좀 늦춤.
	XDBMng2<XClientConnection>::sGet()->SetmsecSleepPump( msec2 );
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
// 	m_maxGSvrConnect = token.GetNumber();
// 	m_maxConnect = token.GetNumber();
// #ifdef _DUMMY_GENERATOR
// 	m_maxConnect = 5000;
// #endif
// 	m_portGameSvr = token.GetNumber();
// 	m_Port = token.GetNumber();	
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

bool XMain::DelegateDidFinishLoad( const XParamObj2& param )
{
//	m_maxGSvrConnect = param.GetWord( "max_gamesvr" );
	m_maxGSvrConnect = 10;		// 게임서버 리스트를 vector로 바꿔서 이값이 필요없도록 하자
	m_maxConnect = param.GetInt( "max_connect" );
	if( m_maxConnect == 0 )
		m_maxConnect = 100;
#ifdef _DUMMY_GENERATOR
	m_maxConnect = 5000;		// 더미생성기일땐 옵션과 상관없이 최대 크기로 잡는다.,
#endif
	m_portGameSvr = param.GetWord( "port_for_gamesvr" );
	XBREAK( m_portGameSvr == 0 );
	m_Port = param.GetWord( "port_for_client" );
	XBREAK( m_Port == 0 );
	m_strPublicKey = param.GetStrc( "google_public_key" );
	TRACE( "publicKey=%s\n", m_strPublicKey.c_str() );
	// 로긴시뮬에선 게임서버 아이피가 있었지만
	// 실제서버에선 접속하는 게임서버의 아이피를 따로 받아둬야 한다.
	CONSOLE( "max connect=%d", m_maxConnect );
	return true;
}

void XMain::DidFinishCreate( void )
{
	XBREAK( m_Port == 0 );
	// Http관리자
//	XHttpMng::sInit( 4 );		// 커넥션 스레드 4개로 생성
	// 게임서버들이 접속해오는 소켓 생성
//	XSocketForGameSvr::sGet()->Create( m_portGameSvr );
	auto pSocketForGameSvr = new XSocketForGameSvr();
	m_pSocketForGameSvr = pSocketForGameSvr;
	UpdateSleepServer( &m_Env, pSocketForGameSvr );
	const int numThread = m_Env.GetParam().GetInt( "num_recv_thread" );
	m_pSocketForGameSvr->Create( m_portGameSvr, numThread );
	AddSocketServer( pSocketForGameSvr );	
	UpdateTitle();
	//
// 	XEProfile::sGet()->AutoStart( 30 );		// x초를 기다린후 자동 시작
// 	XEProfile::sGet()->AutoFinish( 60 );	// x초를 수행한 후 자동 종료
}

/**
 게임 클라이언트들이 접속해오는 소켓 생성
*/
void XMain::CreateSocketForClient( void )
{
	if( m_pSocketForClient == nullptr ) {
		// 클라이언트가 접속해오는 소켓 생성(오직 하나만 생성)
		auto pSocketForClient = new XSocketSvrForClient();
		m_pSocketForClient = pSocketForClient;
		UpdateSleepServer( &m_Env, pSocketForClient );
		const int numThread = m_Env.GetParam().GetInt( "num_recv_thread" );
		m_pSocketForClient->Create( m_Port, numThread );
		AddSocketServer( pSocketForClient );	
		CONSOLE("create XSocketforClient:%d", (int)m_Port);
	}
}

// 게임 객체 생성
XEContents* XMain::CreateContents( void )
{
	//	XE::g_Lang = XE::xLANG_KOREAN;
	// 게임 객체 생성
	_GAME = new XGame;
	_GAME->Create();

#ifdef _DUMMY_GENERATOR
	LoadDummyNames();
#endif // _DUMMY_GENERATOR

	return _GAME;
}

#ifdef _DUMMY_GENERATOR
//
#define DBMNG	XDBMng2<XMain>::sGet()
//
void XMain::LoadDummyNames()
{
	CToken token;
	if( XASSERT( token.LoadFile( PATH_PROP("names.txt"), XE::TXT_UTF16 ) ) ) {
		while( token.GetToken() ) {
			if( XE::IsHave(token.m_Token) ) {
				m_aryNames.Add( _tstring(token.m_Token) );
			}
		}
	}
}

#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
static std::vector<std::string> s_aryFbid = {
	"557055802"
	, "100000786878603"
	, "100000786878203"
	, "100000786878605"
	, "422888354515288"
	, "100004602252270"
	, "1917811051777352"
	, "100010463058461"
	, "100000686899365"
	, "100002882537209"
	, "100000274211603"
	, "1377662711"
	, "171018822936702"
	, "171019052936679"
	, "1379861695612685"
	, "664936773579417"
	, "494164730653236"
	, "278203089013244"
	, "375691212504985"
	, "405424532872449"
	, "1489534137948675"
	, "2303430311544"
	, "10203589094112277"
	, "10206843052747901"
	, "10200388672078845"
	, "10153241046122776"
	, "304785792997554"
	, "10104882054605370"};
#endif
static std::map<_tstring, int> s_mapNumName;
#endif // _DUMMY_GENERATOR



void XMain::Process( void )
{
	static XUINT64 lldbmng = 0;
	static int cntDbMng = 0;
	auto lldbmng1 = GET_FREQ_TIME();
	XDBMng2<XClientConnection>::sGet()->Process();
	lldbmng += GET_FREQ_TIME() - lldbmng1;
	++cntDbMng;
	DWORD avgdbmng = (DWORD)(lldbmng / cntDbMng);
#ifdef _DUMMY_GENERATOR
	if( m_cntCreateAcc > 0 && m_cntSave - m_cntCreateAcc < 100 )	{
// 		XDBUAccount *pAccountBuff = new XDBUAccount;
		const std::string strUUID = XE::GetUUID();
		const _tstring tstrUUID = C2SZ( strUUID.c_str() );
// #if _DEV_LEVEL <= DLV_DEV_EXTERNAL
// 		const _tstring strFbId = C2SZ( s_aryFbid[ xRandom(s_aryFbid.size()) ] );
// #else
		_tstring strFbId;
// #endif
		_tstring strName = m_aryNames.GetFromRandom();
		//
		{
			int num = 50;
			auto itor = s_mapNumName.find( strName );
			if( itor != s_mapNumName.end() ) {
				num = itor->second;
				strName += XFORMAT("%d", num);
				++num;
				itor->second = num;
			} else {
				s_mapNumName[ strName ] = num;
				strName += XFORMAT( "%d", num );
			}
		}
		//
		DBMNG->DoAsyncCall( GetThis(),
							&XMain::cbCreateNewAccount,
							tstrUUID,
							strName,
							strFbId,
							m_cntCreateAcc );
// 		DBMNG->DoAsyncCall( this,
// 							&XMain::cbCreateNewAccount,
// 							pAccountBuff,
// 							tstrUUID,
// 							strName, //DUMMY_NAME,
// 							m_cntCreateAcc );
//		CONSOLE( "creae dummy account:%d........", m_cntCreateAcc );

		--m_cntCreateAcc;
 		if( m_cntCreateAcc == 0 )  {
 			CONSOLE( "send create dummy account:............please wait.");
 		}

	}
	XDBMng2<XMain>::sGet()->Process();
#endif // _DUMMY_GENERATOR

	static XUINT64 llproc = 0;
	static int cntproc = 0;
	auto llproc1 = GET_FREQ_TIME();
	XEServerMain::Process();
	llproc += GET_FREQ_TIME() - llproc1;
	++cntproc;
	DWORD avgproc = (DWORD)( llproc / cntproc );
	llproc = 0;
}

#ifdef _DUMMY_GENERATOR
void XMain::cbCreateNewAccount( XDatabase *pDB,
								const _tstring& strUUID,
								const _tstring& strName,
								const _tstring& strFbId,
								int cntCreateAcc )
{
	ID idAcc = pDB->GetpDB()->CreateNewAccount( strName, strUUID );
	if( XASSERT(idAcc) ) {
		if( !strFbId.empty() ) {
			bool bOk = pDB->GetpDB()->RegistFBAccount( idAcc, strFbId, _T("") );
			XBREAK( bOk == false );
		}
	}

	DBMNG->RegisterSendCallback( GetThis(),
								&XMain::OnResultCreateNewAccount,
								idAcc
								, strName
								, cntCreateAcc );

}

void XMain::OnResultCreateNewAccount( ID idAcc, const _tstring& strName, int cntCreateAcc )
{
	do {
		if( idAcc ) {
			//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
			auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetFreeConnection();
			if( pGameSvrConnection ) {
				pGameSvrConnection->SendNewDummyAccount( idAcc, strName, cntCreateAcc );
				++m_cntSave;
			}
		}
	} while( 0 );
}
#endif // _DUMMY_GENERATOR

void XMain::OnArriveSaveDummy()
{
#ifdef _DUMMY_GENERATOR
	--m_cntSave;
	XBREAK( m_cntSave < 0 );
//	CONSOLE( "creating dummy account:%d........", m_cntSave );
	if( m_cntSave == 0 )
		CONSOLE( "finish create dummy account:---------------------------------------------------" );
#endif // _DUMMY_GENERATOR
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

#ifdef _XPROFILE
#include "XResObj.h"

void XMain::OnFinishProfile( XEProfile* pProfile )
{
#ifdef _XPROFILE
	TCHAR szSpace[ 256 ] = {_T( ' ' ), };
	char cFile[ 256 ];
	XE::MakeFilenameFromTime( cFile, 256, "pf" );
	_tstring strFile = C2SZ( cFile );
	XResFile file;
	if( file.OpenForWrite( strFile.c_str() ) ) {
		XARRAYLINEAR_LOOP_AUTO( XEProfile::sGetResult(), &result ) {
			memset( szSpace, ' ', sizeof( TCHAR ) * 256 );
			szSpace[ result.depth * 2 ] = 0;
			file.Printf( _T( "%s%s  %.3f  %d  %d  %d\r\n" ), szSpace, result.strFunc.c_str()
				, result.ratioShare
				, result.mcsExecAvg
				, result.mcsExecMin
				, result.mcsExecMax );
		} END_LOOP;
		CONSOLE( "프로파일 결과 저장완료:%s", strFile.c_str() );
	}
#endif // _XPROFILE
}
#endif // _XPROFILE

