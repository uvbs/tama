#include "stdafx.h"
#include "XFSMConnector.h"
#include "XSockLoginSvrBot.h"
#include "XSockGameSvrBot.h"
#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xConnector;

#define VALID_SOCK( L, G ) \
	auto L = m_pOwner->GetpSockLoginSvr(); \
	auto G = m_pOwner->GetpSockGameSvr(); \
	XBREAK( L == nullptr ); \
	XBREAK( G == nullptr ); \
	XLockObj lockObjL( L, __TFUNC__ ); \
	XLockObj lockObjG( G, __TFUNC__ );

// #define END_VALID_SOCKET \
// 	G->Unlock();\
// 	L->Unlock(); 


LPCTSTR xConnector::GetenumFSMId( xtFSMId idFSM ) {
	switch( idFSM )	{
	case xFID_NONE:	return _T("xFID_NONE");
	case xFID_OFFLINE: return _T("xFID_OFFLINE");
	case xFID_CONNECTING_LOGINSVR: return _T("xFID_CONNECTING_LOGINSVR");
	case xFID_LOGINING_LOGINSVR: return _T("xFID_LOGINING_LOGINSVR");
	case xFID_CONNECTING_GAMESVR: return _T("xFID_CONNECTING_GAMESVR");
	case xFID_LOGINING_GAMESVR: return _T("xFID_LOGINING_GAMESVR");
	case xFID_MAKE_NEW_ACCOUNT: return _T("xFID_MAKE_NEW_ACCOUNT");
	case xFID_ONLINE: return _T("xFID_ONLINE");
	case xFID_DELAY_AFTER_CONNECTING_LOGINSVR: return _T("xFID_DELAY_AFTER_CONNECTING_LOGINSVR");
	case xFID_DELAY_AFTER_CONNECTING_GAMESVR: return _T( "xFID_DELAY_AFTER_CONNECTING_GAMESVR" );
	default:
		XBREAK(1);
		break;
	}
	return _T("");
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief fsm 전환 이벤트를 등록한다.
*/
void XFSMBaseConnector::AddFSMEvent( xConnector::xtFSMEvent event, XFSMBaseConnector* pfsmObj )
{
	xChangeFSM fsm( event, pfsmObj );
	m_aryChangeFSM.Add( fsm );
}

/**
 @brief fsm전환을 위한 이벤트에 해당하는 fsm을 가지고 있는지 검색해서 돌려줌.
*/
XFSMBaseConnector* XFSMBaseConnector::GetpfsmByEvent( xConnector::xtFSMEvent event )
{
	for( const auto& fsmChange : m_aryChangeFSM ) {
		if( fsmChange.m_Event == event )
			return fsmChange.m_fsmObj;
	}
	return nullptr;
}

void XFSMBaseConnector::Process() 
{
	// 타임아웃 타이머가 준비되었을때 응답없음 이벤트를 처리함.
	if( m_timerTimeout.IsOn() && m_timerTimeout.IsOver() ) {
		m_timerTimeout.Off();
		XTRACE("xFE_NO_RESPONE");
// 		GetpMng()->DispatchFSMEvent( xFE_NO_RESPONSE );
	}
}

//////////////////////////////////////////////////////////////////////////
void XFSMOffline::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	// 일정시간 대기후 접속을 시도함.
	const float sec = xRandomF( 3.f, 10.f );
	m_timerDelay.Set( sec );
}
void XFSMOffline::Process()
{
	// 일정시간을 기다리고 접속을 시도한다.
	if( m_timerDelay.IsOver() ) {
		if( XMain::sGet()->GetbReadyOffline() ) {
			// fsm스탑상태면 다음 단계로 넘어가지 않고 타이머를 리셋한 후 대기
			m_timerDelay.Reset();
		} else {
			m_timerDelay.Off();
			m_pOwner->DispatchFSMEvent( xFE_WILL_CONNECT );
		}
	}
}

////////////////////////////////////////////////////////////////
XFSMConnectingL::XFSMConnectingL( XBotObj* pOwner )
	: XFSMBaseConnector( xFID_CONNECTING_LOGINSVR, pOwner )
{
	Init();
}

void XFSMConnectingL::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
	pSockLogin->DoDisconnect();
	pSockLogin->ClearConnection();
	pSockGame->DoDisconnect();
	pSockGame->ClearConnection();
	if( XASSERT( pSockLogin->CreateSocket()) ) {
		pSockLogin->TryConnect();
// 		m_stateConnect = xST_CONNECTING_L;
	}
}
////////////////////////////////////////////////////////////////
int XFSMLoginingL::s_cntSample = 0;		// 평균로그인시간 측정용 카운터
double XFSMLoginingL::s_secSum = 0;			// 누적
XFSMLoginingL::XFSMLoginingL( XBotObj* pOwner )
	: XFSMBaseConnector( xFID_LOGINING_LOGINSVR, pOwner )
{
	Init();
}

void XFSMLoginingL::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	// 아이디 비번으로 로그인시도
//	const std::string strcID = m_pOwner->GetstrcIDGenerate();
// 	pSockLogin->SendLoginFromIDPW( strcID, "1234" );
	m_timerLogin.Set(0, TRUE);
	const std::string strcUUID = m_pOwner->GetstrcUUIDGenerate();
	pSockLogin->SendLoginByUUID( strcUUID );

}

void XFSMLoginingL::OnFinish() 
{
	const auto secPass = m_timerLogin.GetPassSec();
	m_timerLogin.Off();
	s_secSum += secPass;
	++s_cntSample;
}

//////////////////////////////////////////////////////////////////////////
// 게임서버 연결상태 시작
void XFSMConnectingG::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
	pSockLogin->DoDisconnect();
	pSockLogin->ClearConnection();
	pSockGame->DoDisconnect();
	pSockGame->ClearConnection();
	// 게임서버 소켓 생성
	if( pSockGame->CreateSocket() ) {
		pSockGame->TryConnect();
		// 		m_stateConnect = xST_CONNECTING_G;
	}
}
//////////////////////////////////////////////////////////////////////////
void XFSMLoginingG::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
	pSockGame->SendReqLogin( m_pOwner->GetidAcc() );
}
//////////////////////////////////////////////////////////////////////////
void XFSMOnline::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
}
void XFSMOnline::Process()
{
	// 아이디 등록이 끝나기 전까진 온라인 해제시키면 안됨.
	// 시간 지나면 접속을 끊음.
	const int secStayOnline = XMain::sGet()->GetOption().GetsecOnline();
	if( secStayOnline > 0 ) {
		if( m_timerDelay.IsOff() ) {
	//		const float sec = xRandomF( 3.f, 10.f );
//			const float sec = xRandomF( 100.f, 200.f );
			const float sec = (float)secStayOnline + xRandomF( secStayOnline );	// 100%범위 랜덤.
			m_timerDelay.Set( sec );
		}

		if( m_timerDelay.IsOver() ) {
			m_timerDelay.Off();
			XLOGB( "%d:will_disconnect", m_pOwner->GetidObj() );
			m_pOwner->DispatchFSMEvent( xFE_WILL_DISCONNECT );
		}
	} // secStayOnline
}

void XFSMOnline::OnFinish()
{
	VALID_SOCK( pSockLogin, pSockGame );
	pSockLogin->DoDisconnect();
	pSockLogin->ClearConnection();
	pSockGame->DoDisconnect();
	pSockGame->ClearConnection();
	XBREAK( m_timerDelay.IsOn() );
}

//////////////////////////////////////////////////////////////////////////
void XFSMMakeNewAccount::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
	const _tstring strtNick = m_pOwner->GetstrtNickGenerate();
	XBREAK( strtNick.empty() );
	XBREAK( !m_pOwner->GetstrName().empty() );
	m_pOwner->SetstrName( strtNick );
	pSockLogin->SendReqCreateAccount( strtNick );

}
//////////////////////////////////////////////////////////////////////////
void XFSMDelayAfterConnectingL::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
//	XTimeoutMng::sGet()->Clear();
	pSockLogin->DoDisconnect();
	pSockLogin->ClearConnection();
	pSockGame->DoDisconnect();
	pSockGame->ClearConnection();
 	auto sec = GetParam().GetFloat( "sec" );
 	if( sec <= 0 )
 		sec = 1.f;
//	const float sec = xRandomF( 3.f, 10.f );
	m_timerDelay.Set( sec );
}

void XFSMDelayAfterConnectingL::Process()
{
	XBREAK( m_timerDelay.IsOff() );
	if( m_timerDelay.IsOver() ) {
		XLOGB( "%d:will_connect ============================", m_pOwner->GetidObj() );
		m_pOwner->SetidNextFSM( xFID_CONNECTING_LOGINSVR );
		m_timerDelay.Off();
		return;
	}
		
}

//////////////////////////////////////////////////////////////////////////
void XFSMDelayAfterConnectingG::OnStart()
{
	VALID_SOCK( pSockLogin, pSockGame );
	//
	pSockLogin->DoDisconnect();
	pSockGame->DoDisconnect();
	auto sec = GetParam().GetFloat( "sec" );
	if( sec <= 0 )
		sec = 1.f;
	m_timerDelay.Set( sec );
}

void XFSMDelayAfterConnectingG::Process()
{
	XBREAK( m_timerDelay.IsOff() );
	if( m_timerDelay.IsOver() ) {
		XLOGB( "%d:will_login_G ============================", m_pOwner->GetidObj() );
		m_pOwner->SetidNextFSM( xFID_CONNECTING_GAMESVR );
		m_timerDelay.Off();
		return;
	}
		
}
