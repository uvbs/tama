#include "stdafx.h"
#include "stdafx.h"
#include "XConnector.h"
#include "XSockLoginSvr.h"
#include "XSockGameSvr.h"
#include "XLoginInfo.h"
#include "XGame.h"
#include "XFSMConnector.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif // _CHEAT

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xConnector;
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
	XVECTOR_DESTROY( m_aryFSMObj );
}

void XConnector::OnCreate()
{
	// 소켓객체 싱글톤을 생성한다.
	CreateLoginSocketObj();
	CreateGameSvrSocketObj();
	//
	CreateFSMs();
//	ChangeFSMConnect( xFS_CREATE_SOCKET_OBJ );
// 	auto pSocketLogin = CreateLoginSocketObj(0);
// 	if( pSocketLogin->IsDisconnected() ) {	// 소켓생성 실패를 의미함.
// 		// 소켓생성 실패하면 에러창 띄우고 앱 종료
// 		GAME->OnCreateFailedSocket();
// 		ChangeFSMConnect( xFS_NONE );
// 	}
}

/**
 @brief 소켓객체를 생성한다. 아직 소켓을 만들진 않는다.
*/
XSockLoginSvr* XConnector::CreateLoginSocketObj()
{
	// 싱글톤처럼 동작해야 하므로 최초 한번만 호출되어야 한다.
	XBREAK( LOGINSVR_SOCKET != nullptr );
// 	_tstring strIP = C2SZ( CONNECT_INI.m_cIP );
// 	XTRACE( "create loginsvr connection:%s:%d", strIP.c_str(), CONNECT_INI.m_Port );
	// 객체 생성과 소켓생성을 분리해야 함. ip/port를 지금 넘겨줄필요는 없음.
	LOGINSVR_SOCKET = new XSockLoginSvr( this );
// 	LOGINSVR_SOCKET->SetIpAndPort( strIP, CONNECT_INI.m_Port );
	return LOGINSVR_SOCKET;
}
/**
 @brief 게임서버용 소켓
*/
// XSockGameSvr* XConnector::CreateGameSvrSocketObj( std::string strIP,
// 												WORD port,
// 												DWORD param )
// {
// 	XBREAK( GAMESVR_SOCKET != nullptr );
// 	// 최초 소켓 생성상황
// 	XTRACE( "create gamesvr socket" );
// 	//
// 	GAMESVR_SOCKET = new XSockGameSvr( strIP.c_str(), port, this, param );
// 	return GAMESVR_SOCKET;
// }
/**
 @brief 소켓객체를 생성한다. 아직 소켓을 만들진 않는다.
*/
XSockGameSvr* XConnector::CreateGameSvrSocketObj()
{
	XBREAK( GAMESVR_SOCKET != nullptr );
	XTRACE( "create gamesvr socket" );
	GAMESVR_SOCKET = new XSockGameSvr( this );
	return GAMESVR_SOCKET;
}

XFSMBaseConnector* XConnector::FindFSMObj( xtFSMId idFSM )
{
	for( auto pObj : m_aryFSMObj ) {
		if( pObj->GetidFSM() == idFSM )
			return pObj;
	}
	return nullptr;
}

void XConnector::SetParam( xtFSMId idFSM, const XParamObj& paramObj )
{
	auto fsm = FindFSMObj( idFSM );
	if( XASSERT(fsm) )
		fsm->SetParam( paramObj );
}

// void XConnector::CreateFSMs( const std::string& strcUUID )
// {
// 	m_aryFSMObj.Add( new XFSMConnectingL( XGAME::xCP_LOGIN_UUID, strcUUID.c_str() ) );
// 	m_aryFSMObj.Add( new XFSMLoginingL( XGAME::xCP_LOGIN_UUID, strcUUID.c_str() ) );
// 	CreateFSMsEtc();
// }
// void XConnector::CreateFSMs( const std::string& strcID, const std::string& strcPassword )
// {
// 	auto fsmConnectingL = new XFSMConnectingL( XGAME::xCP_LOGIN_IDPW, strcID.c_str(), strcPassword.c_str() );
// 	auto fsmLoginingL = new XFSMLoginingL( XGAME::xCP_LOGIN_IDPW, strcID.c_str(), strcPassword.c_str() );
// 	CreateFSMsEtc();
// }
// 
// void XConnector::CreateFSMs( const _tstring& strNick )
// {
// 	auto fsmConnectingL = new XFSMConnectingL( XGAME::xCP_LOGIN_NEW_ACC, strNick );
// 	auto fsmLoginingL = new XFSMLoginingL( XGAME::xCP_LOGIN_NEW_ACC, strNick );
// 	CreateFSMsEtc();
// }

void XConnector::CreateFSMs()
{
	m_aryFSMObj.Add( new XFSMOffline() );
	m_aryFSMObj.Add( new XFSMConnectingL() );
	m_aryFSMObj.Add( new XFSMLoginingL() );
	m_aryFSMObj.Add( new XFSMConnectingG() );
	m_aryFSMObj.Add( new XFSMLoginingG() );
	m_aryFSMObj.Add( new XFSMOnline() );
	m_aryFSMObj.Add( new XFSMDelayAfterConnectingL() );
	m_aryFSMObj.Add( new XFSMDelayAfterConnectingG() );
	//
	XBREAK( m_aryFSMObj.size() != xFID_MAX - 1);
	auto fsmOffline = FindFSMObj( xFID_OFFLINE );
	auto fsmConnectingL = FindFSMObj( xFID_CONNECTING_LOGINSVR );
	auto fsmLoginingL = FindFSMObj( xFID_LOGINING_LOGINSVR );
	auto fsmConnectingG = FindFSMObj( xFID_CONNECTING_GAMESVR );
	auto fsmLoginingG = FindFSMObj( xFID_LOGINING_GAMESVR );
	auto fsmOnline = FindFSMObj( xFID_ONLINE );
	auto fsmDelayAfterConnectingL = FindFSMObj( xFID_DELAY_AFTER_CONNECTING_LOGINSVR );
	auto fsmDelayAfterConnectingG = FindFSMObj( xFID_DELAY_AFTER_CONNECTING_GAMESVR );
	//
	fsmConnectingL->AddFSMEvent( xFE_CONNECTED_LOGINSVR, fsmLoginingL );
	fsmConnectingL->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmConnectingL->AddFSMEvent( xFE_FAILED_CREATE_SOCKET, fsmDelayAfterConnectingL );
	//
	fsmLoginingL->AddFSMEvent( xFE_LOGINED_LOGINSVR, fsmConnectingG );
	fsmLoginingL->AddFSMEvent( xFE_DISCONNECTED_LOGINSVR, fsmDelayAfterConnectingL );
	fsmLoginingL->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmLoginingL->AddFSMEvent( xFE_RECONNECT_TRY_LOGINSVR, fsmDelayAfterConnectingL );

	fsmConnectingG->AddFSMEvent( xFE_CONNECTED_GAMESVR, fsmLoginingG );
	fsmConnectingG->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmConnectingG->AddFSMEvent( xFE_FAILED_CREATE_SOCKET, fsmDelayAfterConnectingL );
	//
	fsmLoginingG->AddFSMEvent( xFE_LOGINED_GAMESVR, fsmOnline );
	fsmLoginingG->AddFSMEvent( xFE_DISCONNECTED_GAMESVR, fsmDelayAfterConnectingL );
	fsmLoginingG->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmLoginingG->AddFSMEvent( xFE_RECONNECT_TRY_GAMESVR, fsmDelayAfterConnectingG );
	fsmLoginingG->AddFSMEvent( xFE_RECONNECT_TRY_LOGINSVR, fsmDelayAfterConnectingL );
	//
	fsmDelayAfterConnectingL->AddFSMEvent( xFE_DELAY_OVER, fsmConnectingL );
	//
	fsmDelayAfterConnectingG->AddFSMEvent( xFE_DELAY_OVER, fsmLoginingG );
	//
	fsmOnline->AddFSMEvent( xFE_DISCONNECTED_GAMESVR, fsmDelayAfterConnectingL );
}

void XConnector::SetConnectParamByUUID( const std::string& strcUUID )
{
	{
		auto fsm = SafeCast2<XFSMConnectingL*>( FindFSMObj( xFID_CONNECTING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByUUID( strcUUID );
		}
	}
	{
		auto fsm = SafeCast2<XFSMLoginingL*>( FindFSMObj( xFID_LOGINING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByUUID( strcUUID );
		}
	}
}
void XConnector::SetConnectParamByIDPW( const std::string& strcID, const std::string& strcPassword )
{
	{
		auto fsm = SafeCast2<XFSMConnectingL*>( FindFSMObj( xFID_CONNECTING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByIDPW( strcID, strcPassword );
		}
	}
	{
		auto fsm = SafeCast2<XFSMLoginingL*>( FindFSMObj( xFID_LOGINING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByIDPW( strcID, strcPassword );
		}
	}
}
void XConnector::SetConnectParamByNewAcc( const _tstring& strNick
																				, XGAME::xtConnectParam typeLogin
																				, const std::string& strFbUserId )
{
	{
		auto fsm = SafeCast2<XFSMConnectingL*>( FindFSMObj( xFID_CONNECTING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByNewAcc( strNick );
		}
	}
	{
		auto fsm = SafeCast2<XFSMLoginingL*>( FindFSMObj( xFID_LOGINING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			if( typeLogin == XGAME::xCP_LOGIN_FACEBOOK ) {
				XBREAK( strFbUserId.empty() );
				fsm->GetparamConnectMutable().SetConnectParamByNewAcc( strNick, strFbUserId );
			} else
				fsm->GetparamConnectMutable().SetConnectParamByNewAcc( strNick );
		}
	}
}
void XConnector::SetConnectParamByFacebook( const std::string& strFbUserid )
{
// 	{
// 		auto fsm = SafeCast2<XFSMConnectingL*>( FindFSMObj( xFID_CONNECTING_LOGINSVR ) );
// 		if( XASSERT( fsm ) ) {
// 			fsm->GetparamConnectMutable().SetConnectParamByFacebook( strFbUserid );
// 		}
// 	}
	{
		auto fsm = SafeCast2<XFSMLoginingL*>( FindFSMObj( xFID_LOGINING_LOGINSVR ) );
		if( XASSERT( fsm ) ) {
			fsm->GetparamConnectMutable().SetConnectParamByFacebook( strFbUserid );
		}
	}
}


/**
 @brief 로그인서버 접속 소켓 생성 및 접속시도
 결과는 XConnector :: DelegateConnect가 호출된다. 
 콜백이 필요없다면 LOGINSVR_SOCKET::Get_bConnected()값이 1이 되었는지를 검사하면된다.
*/
// XSockLoginSvr* XConnector::TryConnectLoginSvr( bool bReconnect )
// {
// 	if( bReconnect == false ) {
// 		if( LOGINSVR_SOCKET->IsDisconnected() ) {
// 			auto param = LOGINSVR_SOCKET->GetdwConnectParam();
// 			ReconnectLoginSvr( 1.f );
// 			return nullptr;
// 		}
// 		LOGINSVR_SOCKET->TryConnect();
// 		ChangeFSMConnect( xFS_CONNECT_LOGINSVR );
// 	} else {
// 		// 재접속 상황
// 		XBREAK(1);	// 아직 용도가 없음.
// // 		ChangeFSMConnect( xFS_RECONNECT_LOGINSVR );
// 	}
// 	return LOGINSVR_SOCKET;
// }

/**
 @brief 소켓 연결후 호출
*/
void XConnector::DelegateConnect( XENetworkConnection *pConnect, DWORD param )
{
	if( pConnect->GetidConnect() == LOGINSVR_SOCKET->GetidConnect() ) {
		DispatchFSMEvent( xFE_CONNECTED_LOGINSVR );
	// 로그인서버
	//////////////////////////////////////////////////////////////////////////
	} else
	if( pConnect->GetidConnect() == GAMESVR_SOCKET->GetidConnect() ) {
		DispatchFSMEvent( xFE_CONNECTED_GAMESVR );
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
 @brief 현재 fsm에 대한 상태전환 이벤트를 처리한다.
*/
void XConnector::DispatchFSMEvent( xtFSMEvent event, const XParamObj& paramObj )
{
	XLockObj lockObj( &m_Lock );
#ifndef _DEBUG
	// 디버그 모드에선 편의상 응답없음에 대한 처리를 무시한다.
	if( event == xFE_NO_RESPONSE )
		return;
#endif // not _DEBUG
	if( m_pCurrFSM || m_pNextFSM ) {
		// 현재 상태가 "event"이벤트로 전환될수 있는지 검색.
		auto pfsmNext = ( m_pCurrFSM ) ? m_pCurrFSM->GetpfsmByEvent( event ) : m_pNextFSM;
// 		auto pfsmNext = m_pCurrFSM->GetpfsmByEvent( event );
		if( XASSERT(pfsmNext) ) {
			pfsmNext->SetParam( paramObj );
			if( m_pCurrFSM )		// 홈으로 나가있을땐 널이된다.
				m_pCurrFSM->OnFinish();		// 현재 fsm종료 핸들러
			m_pNextFSM = pfsmNext;		// 다음 fsm으로 등록.
		}
	} else
	if( event == xFE_START_CONNECTING ) {
		XASSERT( LOGIN_INFO.GetstrUUID().empty() == false );
		XConnector::sGet()->SetConnectParamByUUID( LOGIN_INFO.GetstrUUID() );
		XConnector::sGet()->SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
	}
}

void XConnector::DispatchFSMEvent( xtFSMEvent event )
{
	XParamObj paramObj;
	DispatchFSMEvent( event, paramObj );
}

/**
 @brief 
*/
void XConnector::Process( float dt )
{
	XLockObj lockObj( &m_Lock );
//	DoFSMConnect( m_fsmStateConnect, xFE_PROCESS, 0 );
	// next로 등록된 fsm을 현재  fsm으로 올린다.
	if( m_pNextFSM ) {
		m_pCurrFSM = m_pNextFSM;
		if( m_pCurrFSM )
			m_pCurrFSM->OnStart();
		m_pNextFSM = nullptr;
	}
	if( m_pCurrFSM )
		m_pCurrFSM->Process();
}

bool XConnector::IsOnline()
{
	if( m_pCurrFSM )
		return m_pCurrFSM->GetidFSM() == xFID_ONLINE;
	return false;
}

void XConnector::Draw()
{
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		_tstring strId;
		if( m_pCurrFSM ) {
			strId = xConnector::GetenumFSMId( m_pCurrFSM->GetidFSM() );
		} else {
			strId = _T( "null" );
		}
		PUT_STRINGF_STROKE( 452, 40, XCOLOR_YELLOW, "FSMc:%s", strId.c_str() );
	}
#endif // _CHEAT
}

