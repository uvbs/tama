#include "stdafx.h"
#include "stdafx.h"
#include "XConnector.h"
#include "XSockLoginSvr.h"
#include "XSockGameSvr.h"
#include "XLoginInfo.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 홈으로 나갔을때 FSMOnline상태가 풀리고 초기화상태가 되어야 하는데 여전히 온라인상태.
// 이때문에 다시 홈에서돌아왔을때 프로세스를 돌면서 Online상태 프로세스를 돌면서 끊긴걸로 판단하고
// 딜레이 커넥트 로긴서버를 실행시켜버리는게 문제

std::shared_ptr<XConnector> XConnector::s_spInstance;
std::shared_ptr<XConnector>& XConnector::sGet() 
{
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XConnector>( new XConnector );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XConnector::XConnector()
{
	Init();
}

void XConnector::Destroy()
{
}

void XConnector::OnCreate()
{
//	ChangeFSMConnect( xFS_CREATE_SOCKET_OBJ );
	auto pSocketLogin = CreateLoginSocketObj(0);
	if( pSocketLogin->IsDisconnected() )	// 소켓생성 실패를 의미함.
	{
		// 소켓생성 실패하면 에러창 띄우고 앱 종료
		GAME->OnCreateFailedSocket();
		ChangeFSMConnect( xFS_NONE );
	}
}

XSockLoginSvr* XConnector::CreateLoginSocketObj( DWORD param )
{
//	XBREAK( LOGINSVR_SOCKET != nullptr );
  SAFE_RELEASE_REF( LOGINSVR_SOCKET );
	// 최초 소켓 생성상황
	_tstring strIP = C2SZ( CONNECT_INI.m_cIP );
	XTRACE( "create loginsvr connection:%s:%d", strIP.c_str(), CONNECT_INI.m_Port );
	// 객체 생성과 소켓생성을 분리해야 함. ip/port를 지금 넘겨줄필요는 없음.
	LOGINSVR_SOCKET = new XSockLoginSvr( CONNECT_INI.m_cIP,
										CONNECT_INI.m_Port,
										this,
										param );
	// 소켓 생성에 실패했을수도 있음.
	return LOGINSVR_SOCKET;
}



/**
 @brief 로그인서버 접속 소켓 생성 및 접속시도
 결과는 XConnector::DelegateConnect가 호출된다. 
 콜백이 필요없다면 LOGINSVR_SOCKET::Get_bConnected()값이 1이 되었는지를 검사하면된다.
*/
XSockLoginSvr* XConnector::TryConnectLoginSvr( bool bReconnect )
{
	if( bReconnect == false ) {
		if( LOGINSVR_SOCKET->IsDisconnected() ) {
			auto param = LOGINSVR_SOCKET->GetdwConnectParam();
			ReconnectLoginSvr( 1.f );
			return nullptr;
		}
		LOGINSVR_SOCKET->TryConnect();
		ChangeFSMConnect( xFS_CONNECT_LOGINSVR );
	} else {
		// 재접속 상황
		XBREAK(1);	// 아직 용도가 없음.
// 		ChangeFSMConnect( xFS_RECONNECT_LOGINSVR );
	}
	return LOGINSVR_SOCKET;
}

/**
 @brief 게임서버용 소켓
*/
XSockGameSvr* XConnector::CreateGameSvrSocketObj( std::string strIP,
												WORD port,
												DWORD param )
{
	XBREAK( GAMESVR_SOCKET != nullptr );
	// 최초 소켓 생성상황
	XTRACE( "create gamesvr socket" );
	//
	GAMESVR_SOCKET = new XSockGameSvr( strIP.c_str(), port, this, param );
	return GAMESVR_SOCKET;
}

/**
 @brief 
*/
XSockGameSvr* XConnector::TryConnectGameSvr( bool bReconnect )
{
	if( bReconnect == false ) {
		// 최초 소켓 생성상황
		XTRACE( "try gamesvr connect" );
		//
		GAMESVR_SOCKET->TryConnect();
		ChangeFSMConnect( xFS_CONNECT_GAMESVR );
	} else {
		// 재접속 상황
		XBREAK( 1 );	// 아직 용도가 없음.
// 		ChangeFSMConnect( xFS_RECONNECT_GAMESVR );
	}
	return GAMESVR_SOCKET;
}

/**
 @brief 소켓 연결후 호출
*/
void XConnector::DelegateConnect( XENetworkConnection *pConnect, DWORD param )
{
	if( LOGINSVR_SOCKET && pConnect->GetidConnect() == LOGINSVR_SOCKET->GetidConnect() ) {
//		auto connectParam = ( XGAME::xtConnectParam ) param;
//		ChangeFSMConnect( xFS_CONNECTED_LOGINSVR );
		if( LOGINSVR_SOCKET->GetbReconnected() == FALSE ) {
			// 최초 접속
			CONSOLE( "first connected success" );
		} else {
			// 재접속
			CONSOLE( "reconnect ok:" );
		}
#ifdef _XLOGIN_FACEBOOK
		//////////////////////////////////////////////////////////////////////////
		// 		if( connectParam == XGAME::xCP_LOGIN_FACEBOOK ) {
		// 			XBREAK( LOGIN_INFO.GetstrFbUserId().empty() == true );
		// 			if( LOGIN_INFO.GetstrFbUserId().empty() == false )
		// 			{
		// 				CONSOLE( "DelegateConnect: request login with facebook" );
		// 				// 페이스북 userid와 username을 서버로 보낸다.
		// 				LOGINSVR_SOCKET->SendRequestLoginFromFacebook( 
		// 					LOGIN_INFO.GetstrFbUserId().c_str(),
		// 					LOGIN_INFO.GetstrFbUsername().c_str(),
		// 					m_strNickname.c_str() );
		// 			}
		// 		} else
#endif // login_facebook
		//////////////////////////////////////////////////////////////////////////
// 		LOGINSVR_SOCKET->SendLoginFromUUID( GAME
// 																			, LOGIN_INFO.GetstrUUID().c_str() );
// 		if( connectParam == XGAME::xCP_LOGIN_UUID ) {
// 			if( LOGIN_INFO.IsLastLoginUUID()
// 				&& LOGINSVR_SOCKET->SendLoginFromUUID( GAME, LOGIN_INFO.GetstrUUID().c_str(), m_strNickname ) )	{
// 			}
// 			else
// 				// 전송에 실패하면 소켓 해제시킴
// 				LOGINSVR_SOCKET->DoDisconnect();
// 		}
	//////////////////////////////////////////////////////////////////////////
	// 		if (connectParam == XGAME::xCP_LOGIN_IDPW) {
	// 			XBREAK( LOGIN_INFO.IsLastLoginIDPW() == FALSE );
	// 			if ( LOGIN_INFO.IsLastLoginIDPW() && 
	// 				LOGINSVR_SOCKET->SendLoginFromIDPW(GAME, 
	// 													LOGIN_INFO.GetstrID().c_str(), 
	// 													LOGIN_INFO.GetstrPassword().c_str(),
	// 													m_strNickname)) {
	// 			}
	// 			else
	// 				// 전송에 실패하면 소켓 해제시킴
	// 				LOGINSVR_SOCKET->DoDisconnect();
	// 		}
	// 로그인서버
	//////////////////////////////////////////////////////////////////////////
	} else
	if( GAMESVR_SOCKET && pConnect->GetidConnect() == GAMESVR_SOCKET->GetidConnect() ) {
		ChangeFSMConnect( xFS_CONNECTED_GAMESVR );
		if( GAMESVR_SOCKET->SendReqLogin( GAME, m_LoginInfo.idAccount ) ) {
		} else {
			// 전송에 실패하면 소켓 해제시킴
			GAMESVR_SOCKET->DoDisconnect();
		}
	}
}

/**
 @brief 
*/
void XConnector::DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error )
{
	switch( error )
	{
	case XE::xN_TIMEOUT:
		XWND_ALERT( "connect failed. timeout!" );
		/*
		타임아웃 처리.
		최초 게임서버에 접속을 시도한다
		  .소켓생성 시도-실패시 1초후 다시 시도
		  .바인드 시도-실패시 1초후 다시 시도(혹은 소켓삭제후 다시 시도)
		  .connect()시도 일정시간 실패시 처음부터 다시 시도.
		  .이과정을 지정된 타임아웃 시간동안 한다.
		  ** 서버가 완전히 꺼졌을때/서버가 멈춰있을때 인터넷연결이 끊겨있을때등으로 다양하게 실험해볼것.
		*/
		break;
	}
	XWND_ALERT( "connect failed." );
}

/**
 로그인서버에 접속이 성공했다.
*/
void XConnector::OnLoginedLoginSvr( ID idAccount, _tstring& strIP, WORD port )
{
// 	ChangeFSMConnect(xFS_SUCCESS_LOGIN_LOGINSVR);
	LOGINSVR_SOCKET->DoDisconnect();
	m_LoginInfo.idAccount = idAccount;
	m_LoginInfo.strIP = strIP;
	m_LoginInfo.port = port;
	// 게임서버 소켓을 생성하고 접속을 시도한다.
//	CreateGameSvrSocketAndTryConnect( FALSE, SZ2C(strIP.c_str()), port, 0 );
	if( GAMESVR_SOCKET == nullptr )	{
		ChangeFSMConnect( xFS_CONNECT_GAMESVR );
		CreateGameSvrSocketObj( SZ2C(strIP.c_str()), port, 0 );
	}	else {
		GAMESVR_SOCKET->SetIpAndPort( strIP, port );	// 게임서버의 ip가 바뀌었을수도 있다.
		RecreateSocketForGameSvr( 0 );
	}
	if( !GAMESVR_SOCKET->IsDisconnected() )
		TryConnectGameSvr( false );

}

/**
 @brief 
*/
void XConnector::Process( float dt )
{
	DoFSMConnect( m_fsmStateConnect, xFE_PROCESS, 0 );
}

//////////////////////////////////////////////////////////////////////////
// xFSM start
void XConnector::ChangeFSMConnect( xtFSMConnectState stateNew, DWORD param /*= 0*/)
{
	// 같은상태를 다시 바꾸려 했다면 그냥 리턴함.
	if( m_fsmStateConnect == stateNew )
		return;
	// 구 상태 해제 처리
	DoFSMConnect( m_fsmStateConnect, XConnector::xFE_UNINIT, param );
	//
	CONSOLE("change FSM state: oldstate=%s, newState=%s", 
										GetStringStateConnect( m_fsmStateConnect ), 
										GetStringStateConnect( stateNew ) );
	// 새상태 초기화
	m_fsmStateConnect = stateNew;
	//
	DoFSMConnect( m_fsmStateConnect, XConnector::xFE_INIT, param );
}

/**
 FSM실행
*/
int XConnector::DoFSMConnect( xtFSMConnectState state, xtFSMEvent event, DWORD param )
{
	int ret = 0;
	switch( state )	{
// 	case xFS_CREATE_SOCKET_OBJ:
// 		ret = FSMCreateSocketObj( event, param );
// 		break;
	case xFS_CONNECT_LOGINSVR:
		ret = FSMDoConnectLoginSvr( event, param );
		break;
	case xFS_CONNECTED_LOGINSVR:
		ret = FSMConnectedLoginSvr( event, param );
		break;
	case xFS_RECONNECT_TO_LOGINSVR:
		ret = FSMReconnectToLoginSvr( event, param );
		break;
	case xFS_CONNECT_GAMESVR:
		ret = FSMDoConnectGameSvr( event, param );
		break;
	case xFS_CONNECTED_GAMESVR:
		ret = FSMConnectedGameSvr( event, param );
		break;
	case xFS_ONLINE:
		ret = FSMOnline( event, param );
		break;
	case xFS_DELAY_AFTER_CONNECT_LOGINSVR:
		ret = FSMDelayAfterConnectLoginSvr( event, param );
		break;
	}
	return ret;
}

/**
 특정 FSM상태에 이벤트 전달
*/
int XConnector::SendFSMEvent( xtFSMConnectState state, xtFSMEvent event, DWORD param )
{
	// 반드시 현재상태에만 이벤트를 보낼 수 있다.
	XBREAK( m_fsmStateConnect != state );
	//
	return DoFSMConnect( state, event, param );
}

/**
 각 FSM상태를 문자열로 만들어 준다. 
*/
LPCTSTR XConnector::GetStringStateConnect( xtFSMConnectState state )
{
	switch( state )
	{
	case xFS_NONE:	return _T("xFS_NONE");;
// 	case xFS_CREATE_SOCKET_OBJ: return _T( "xFS_CREATE_SOCKET_OBJ" );
	case xFS_CONNECT_LOGINSVR: return _T("xFS_CONNECT_LOGINSVR");
	case xFS_CONNECT_GAMESVR: return _T( "xFS_CONNECT_GAMESVR" );
	case xFS_RECONNECT_TO_LOGINSVR:	return _T( "xFS_RECONNECT_TO_LOGINSVR" );;
	case xFS_CONNECTED_LOGINSVR:	return _T( "xFS_CONNECTED_LOGINSVR" );;
	case xFS_SUCCESS_LOGIN_LOGINSVR:	return _T( "xFS_SUCCESS_LOGIN_LOGINSVR" );;
	case xFS_CONNECTED_GAMESVR:	return _T( "xFS_CONNECTED_GAMESVR" );;
	case xFS_ONLINE:	return _T( "xFS_ONLINE" );;
	case xFS_DISCONNECT_GAMESVR:	return _T( "xFS_DISCONNECT_GAMESVR" );;
	case xFS_GOTO_START:	return _T( "xFS_GOTO_START" );;
	case xFS_DELAY_AFTER_CONNECT_LOGINSVR:	return _T("xFS_DELAY_AFTER_CONNECT_LOGINSVR");
	}
	return _T("unknown");
}

/**
 각 FSM동작
*/
/**
 @brief 로긴서버소켓을 재생성하고 재연결 시도를 한다.
*/
bool XConnector::RecreateSocketForLoginSvr( DWORD param )
{
	XTimeoutMng::sGet()->Clear();
	ChangeFSMConnect( xFS_CONNECT_LOGINSVR );
	static int s_cnt = 0;
	CONSOLE("소켓 재성성후 재연결 시도중.:%d", ++s_cnt);
	if( !LOGINSVR_SOCKET->DoReconnect2( param ) ) {
		// 소켓생성 실패
		GAME->OnCreateFailedSocket();
		ChangeFSMConnect( xFS_NONE );
		return false;
	}
	return true;
}

/**
 @brief 게임서버 소켓을 재생성하고 재연결을 시도한다.
*/
bool XConnector::RecreateSocketForGameSvr( DWORD param )
{
	XTimeoutMng::sGet()->Clear();
	// 접속했었던 게임서버에 재접속 시도.
	if( ++m_numRetryForGameSvr < 5 || 1 )	{// 아직 세션키가 없으므로 로긴서버부터 다시 접속하도록.
		ChangeFSMConnect( xFS_CONNECT_GAMESVR );
		m_numRetryForGameSvr = 0;
	}	else {
		// 5회 시도했지만 실패했으면 로긴서버부터 다시 연결해서 다른 게임서버주소를 받아옴.
		GAMESVR_SOCKET->DoDisconnect();
// 		if (LOGIN_INFO.IsLastLoginIDPW())
// 			RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 		else if (LOGIN_INFO.IsLastLoginUUID())
			RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
		return false;
	}
	if( !GAMESVR_SOCKET->DoReconnect2( param ) ) {
		// 소켓생성 실패
		GAME->OnCreateFailedSocket();
		ChangeFSMConnect( xFS_NONE );
		return false;
	}
	return true;
}
/**
 @brief 로긴서버에 connect()를 시도
*/
int XConnector::FSMDoConnectLoginSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:	
		m_timerTimeout.Off();
		m_numRetryForGameSvr = 0;
		break;
	case XConnector::xFE_PROCESS:
#ifdef AUTO_CONNECT
		if( m_timerTimeout.IsOff() || m_timerTimeout.GetWaitSec() == 0 )
			m_timerTimeout.Set( 5.f );
#endif // AUTO_CONNECT
		// 접속이 이미 끊긴상태면 딜레이후 다시 접속한다.
		if( LOGINSVR_SOCKET->IsDisconnected() ) {
			ChangeFSMConnect( xFS_DELAY_AFTER_CONNECT_LOGINSVR );
			break;
		}
		// 연결이 안되면 끊고 다시 연결시도한다.
		if (m_timerTimeout.IsOver()) {
// 			if (LOGIN_INFO.IsLastLoginIDPW()) {
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 				m_timerTimeout.Reset();
// 			} else 
			if (XASSERT(LOGIN_INFO.IsLastLoginUUID())) {
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
				m_timerTimeout.Reset();
			} else
				m_timerTimeout.Off();
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}

/**
 @brief 로긴서버에 connect()됨. 로긴서버로부터 응답을 기다리고 있음.
*/
int XConnector::FSMConnectedLoginSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:
		m_timerTimeout.Off();
		break;
	case XConnector::xFE_PROCESS:
#ifdef AUTO_CONNECT
		if( m_timerTimeout.IsOff() )
			m_timerTimeout.Set( 5.f );
#endif // AUTO_CONNECT
		// 접속이 이미 끊긴상태면 딜레이후 다시 접속한다.
		if( LOGINSVR_SOCKET->IsDisconnected() )	{
			ChangeFSMConnect( xFS_DELAY_AFTER_CONNECT_LOGINSVR );
			break;
		}
		// 서버로부터 응답이 안오고 있음.(서버 로직에 문제가 생겨 응답을 못보내고 있음)
		if( m_timerTimeout.IsOver() ) {
			// 소켓을 해제하고 재생성후 다시 접속시도.
// 			if (LOGIN_INFO.IsLastLoginIDPW()) {
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 				m_timerTimeout.Reset();
// 			} else 
			if (LOGIN_INFO.IsLastLoginUUID()) {
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
				m_timerTimeout.Reset();
			} else
				m_timerTimeout.Off();
		} break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}

/**
 @brief 1초 딜레이 후 로그인서버 재연결 시도
*/
int XConnector::FSMReconnectToLoginSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:
		m_timerDelay.Set( 1.f );
		break;
	case XConnector::xFE_PROCESS:
		if( m_timerDelay.IsOff() )
			m_timerDelay.Set( 1.f );
		if( m_timerDelay.IsOver() ) {
// 			if (LOGIN_INFO.IsLastLoginIDPW())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 			else 
			if (LOGIN_INFO.IsLastLoginUUID())
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
			m_timerDelay.Off();
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}
/**
 @brief 게임서버에 connect()시도중
*/
int XConnector::FSMDoConnectGameSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:	
		m_timerTimeout.Off();
		break;
	case XConnector::xFE_PROCESS:
#ifdef AUTO_CONNECT
		if( m_timerTimeout.IsOff() )
			m_timerTimeout.Set( 10.f );
#endif // AUTO_CONNECT
		// 연결이 안되면 끊고 다시 연결시도한다.
		if( m_timerTimeout.IsOver() ) {
			// 세션키 생길때까지 로긴서버부터 다시 들어가는걸로.
			GAMESVR_SOCKET->DoDisconnect();
// 			if (LOGIN_INFO.IsLastLoginIDPW())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 			else 
			if (LOGIN_INFO.IsLastLoginUUID())
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
// 			if( RecreateSocketForGameSvr( 0 ) )
// 				m_timerTimeout.Reset();
// 			else
// 				m_timerTimeout.Off();
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}

/**
 @brief 로긴서버에 connect()됨
*/
int XConnector::FSMConnectedGameSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:
		m_timerTimeout.Off();
		break;
	case XConnector::xFE_PROCESS:
#ifdef AUTO_CONNECT
		if( m_timerTimeout.IsOff() )
			m_timerTimeout.Set( 5.f );
#endif // AUTO_CONNECT
		// 계정정보가 안오면 끊고 연결부터 다시 한다.
		if( m_timerTimeout.IsOver() ) {
			// 세션키 생길때까지 로긴서버부터 다시 들어가는걸로.
// 			if (LOGIN_INFO.IsLastLoginIDPW())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 			else 
			if (LOGIN_INFO.IsLastLoginUUID())
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
// 			if( RecreateSocketForGameSvr( 0 ) )
// 				m_timerTimeout.Reset();
// 			else
// 				m_timerTimeout.Off();
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}

/**
 온라인 상태
*/
int XConnector::FSMOnline( xtFSMEvent event, DWORD param )
{
	switch( event )
	{
	case XConnector::xFE_INIT:	
		m_timerTimeout.Off();
		GAME->OnOnline();
		break;
	case XConnector::xFE_PROCESS:
		// 접속이 끊기는지 감시
		if( GAMESVR_SOCKET->IsDisconnected() ) {
      // 1초 정도 기다렸다가 재접.
      ChangeFSMConnect( xFS_DELAY_AFTER_CONNECT_LOGINSVR );
			// 아직 세션처리가 안되어있으니 로긴서버부터 다시 접속하는걸로.
// 			if (LOGIN_INFO.IsLastLoginIDPW())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 			else if (LOGIN_INFO.IsLastLoginUUID())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}

int XConnector::FSMDelayAfterConnectLoginSvr( xtFSMEvent event, DWORD param )
{
	switch( event ) {
	case XConnector::xFE_INIT:
		m_timerDelay.Set( 1.f );
		break;
	case XConnector::xFE_PROCESS:
		if( m_timerDelay.IsOff() )
			m_timerDelay.Set( 1.f );
		if( m_timerDelay.IsOver() ) {
// 			if(LOGIN_INFO.IsLastLoginIDPW())
// 				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 			else 
			if (LOGIN_INFO.IsLastLoginUUID())
				RecreateSocketForLoginSvr( XGAME::xCP_LOGIN_UUID );
			m_timerDelay.Off();
		}
		break;
	case XConnector::xFE_UNINIT:
		break;
	}
	return 1;
}


// FSM
//////////////////////////////////////////////////////////////////////////

/**
 @brief 현재 소켓을 끊고 로그인서버에 재접속 한다.
*/
void XConnector::ReconnectLoginSvr( float secDelay )
{
	if( GAMESVR_SOCKET )
		GAMESVR_SOCKET->DoDisconnect();
	XBREAK( LOGINSVR_SOCKET == nullptr );
	LOGINSVR_SOCKET->DoDisconnect();
	if( secDelay > 0 )
		ChangeFSMConnect( xFS_DELAY_AFTER_CONNECT_LOGINSVR );
	else {
// 		if (LOGIN_INFO.IsLastLoginIDPW())
// 			RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_IDPW);
// 		else 
			if (LOGIN_INFO.IsLastLoginUUID())
				RecreateSocketForLoginSvr(XGAME::xCP_LOGIN_UUID);
	}
}

void XConnector::Draw()
{
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		// 현재 커넥션 상태를 찍음.
		_tstring strMsg = GetStringStateConnect();
		if( m_timerTimeout.IsOff() )
			strMsg += _T("(timer off)");
		else
			strMsg += XFORMAT("(%d)", (int)m_timerTimeout.GetPassSec() );
		if( !strMsg.empty() )
			PUT_STRING_STYLE( 419, 2, XCOLOR_WHITE, xFONT::xSTYLE_STROKE, strMsg.c_str() );
		if( GAMESVR_SOCKET && GAMESVR_SOCKET->IsDisconnected() )
			PUT_STRING_STYLE( 419, 12, XCOLOR_WHITE, xFONT::xSTYLE_STROKE, _T("GS=disconnect") );

	}
#endif
}
/**
 @brief 로그인서버에 connect하는 매크로
*/
void XConnector::DoConnectLoginSvrByUUID()
{
//	LOGINSVR_SOCKET->SetdwConnectParam( XGAME::xCP_LOGIN_UUID );
	TryConnectLoginSvr( false );
}

// void XConnector::DoLoginIDPW()
// {
// 	LOGINSVR_SOCKET->SetdwConnectParam( XGAME::xCP_LOGIN_IDPW );
// 	TryConnectLoginSvr( false );
// }

void XConnector::DoDisconnectLoginSvr()
{
  if( LOGINSVR_SOCKET )
    LOGINSVR_SOCKET->DoDisconnect();
}
void XConnector::DoDisconnectGameSvr()
{
  if( GAMESVR_SOCKET )
    GAMESVR_SOCKET->DoDisconnect();
}
