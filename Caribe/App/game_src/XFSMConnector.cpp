#include "stdafx.h"
#include "XFSMConnector.h"
#include "XConnector.h"
#include "XGame.h"
#include "XSockLoginSvr.h"
#include "XSockGameSvr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xConnector;

LPCTSTR xConnector::GetenumFSMId( xtFSMId idFSM ) {
	switch( idFSM )	{
	case xFID_NONE:	return _T("xFID_NONE");
	case xFID_OFFLINE: return _T("xFID_OFFLINE");
	case xFID_CONNECTING_LOGINSVR: return _T("xFID_CONNECTING_LOGINSVR");
	case xFID_LOGINING_LOGINSVR: return _T("xFID_LOGINING_LOGINSVR");
	case xFID_CONNECTING_GAMESVR: return _T("xFID_CONNECTING_GAMESVR");
	case xFID_LOGINING_GAMESVR: return _T("xFID_LOGINING_GAMESVR");
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
XConnector* XFSMBaseConnector::GetpMng()
{
	return XConnector::sGet().get();
}
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
		GetpMng()->DispatchFSMEvent( xFE_NO_RESPONSE );
	}
}
//////////////////////////////////////////////////////////////////////////
void XFSMOffline::OnStart()
{
// 	VALID_SOCK( pSockLogin, pSockGame );
// 	// 일정시간 대기후 접속을 시도함.
// 	const float sec = xRandomF( 3.f, 10.f );
// 	m_timerDelay.Set( sec );
}
void XFSMOffline::Process()
{
// 	// 일정시간을 기다리고 접속을 시도한다.
// 	if( m_timerDelay.IsOver() ) {
// 		if( XMain::sGet()->GetbReadyOffline() ) {
// 			// fsm스탑상태면 다음 단계로 넘어가지 않고 타이머를 리셋한 후 대기
// 			m_timerDelay.Reset();
// 		}
// 		else {
// 			m_timerDelay.Off();
// 			m_pOwner->DispatchFSMEvent( xFE_WILL_CONNECT );
// 		}
// 	}
}

////////////////////////////////////////////////////////////////
XFSMConnectingL::XFSMConnectingL()
	: XFSMBaseConnector( xFID_CONNECTING_LOGINSVR )
{
	Init();
}

void XFSMConnectingL::OnStart()
{
	// 연결을 완전히 끊고 다시 시작함.연결관련 변수 모두 초기화 & 패킷큐 초기화.
	{
		const _tstring strIp = C2SZ( LOGINSVR_SOCKET->GetstrcIP() );
		XTRACE( "ip/port:%s(%d)", strIp.c_str(), LOGINSVR_SOCKET->GetPort() );
	}
	LOGINSVR_SOCKET->DoDisconnect();
	LOGINSVR_SOCKET->ClearConnection();
	GAMESVR_SOCKET->DoDisconnect();
	GAMESVR_SOCKET->ClearConnection();
	// 소켓을 다시 생성한다.
	if( LOGINSVR_SOCKET->CreateSocket() ) {
		// 성공하면 연결스레드 생성하여 연결시도한다.
		const _tstring strIp = C2SZ(LOGINSVR_SOCKET->GetstrcIP());
		XTRACE("ip/port:%s(%d)", strIp.c_str(), LOGINSVR_SOCKET->GetPort() );
		LOGINSVR_SOCKET->TryConnect();
		SetTimeout( 10 );
	} else {
		// 소켓 생성 실패
		GAME->OnCreateFailedSocket();
		XParamObj paramObj;
		paramObj.Set( "sec", 1 );
// 		GetpMng()->DispatchFSMEvent( xFE_FAILED_CREATE_SOCKET, paramObj );				// 만약 툴을 만든다면 이런식으로.
		GetpMng()->SetidNextFSM( xFID_DELAY_AFTER_CONNECTING_LOGINSVR, paramObj );	// 코딩으로 할때는 이편이 더 직관적이어서 일케 함.
	}
}

// void XFSMConnectingL::Process()
// {
// 
// }
// void XFSMConnectingL::AdditionalFSMEvent(	xtFSMEvent event )
// {
// 	if( event == xFE_NO_RESPONSE ) {
// 		// cbOnDisconnectedByExternal이 발생하지 않도록 할것.
// 		LOGINSVR_SOCKET->DoDisconnect();
// 	}
// }
////////////////////////////////////////////////////////////////
XFSMLoginingL::XFSMLoginingL()
	: XFSMBaseConnector( xFID_LOGINING_LOGINSVR )
{
	Init();
}

void XFSMLoginingL::OnStart()
{
	const auto param = m_paramConnect.GetparamConnect();
	BOOL bOk = FALSE;
	switch( param )
	{
	case XGAME::xCP_LOGIN_UUID: {
		XBREAK( m_paramConnect.GetstrcUUID().empty() );
		bOk = LOGINSVR_SOCKET->SendLoginFromUUID( GAME, m_paramConnect.GetstrcUUID(), GAME->GetGCMRegId() );
	} break;
	case XGAME::xCP_LOGIN_IDPW: {
		XBREAK( m_paramConnect.GetstrcID().empty() );
		XBREAK( m_paramConnect.GetstrcPassword().empty() );
		bOk = LOGINSVR_SOCKET->SendLoginFromIDPW( GAME
																						, m_paramConnect.GetstrcID()
																						, m_paramConnect.GetstrcPassword()
																						, GAME->GetGCMRegId() );
} break;
	case XGAME::xCP_LOGIN_FACEBOOK: {
		bOk = LOGINSVR_SOCKET->SendLoginFromFacebook( GAME, m_paramConnect.GetstrcFbUserid(), GAME->GetGCMRegId() );
	} break;
	case XGAME::xCP_LOGIN_NEW_ACC: {
		const std::string strcFbUserId = "";
		bOk = LOGINSVR_SOCKET->SendReqCreateAccount( GAME, m_paramConnect.GetstrtNick(), strcFbUserId, GAME->GetGCMRegId() );
	} break;
	case XGAME::xCP_LOGIN_NEW_ACC_BY_FACEBOOK: {
		bOk = LOGINSVR_SOCKET->SendReqCreateAccount( GAME, m_paramConnect.GetstrtNick()
																										, m_paramConnect.GetstrcFbUserid()
																										, GAME->GetGCMRegId() );
	} break;
	default:
		XBREAK(1);
		break;
	}
	if( !bOk ) {
		GetpMng()->SetidNextFSM( xFID_DELAY_AFTER_CONNECTING_LOGINSVR );
	}
}
//////////////////////////////////////////////////////////////////////////
void XFSMConnectingG::OnStart()
{
	LOGINSVR_SOCKET->DoDisconnect();
	LOGINSVR_SOCKET->ClearConnection();
	// 혹시 되어있을지 모를 연결을 클리어 시키고 다시 생성
	GAMESVR_SOCKET->DoDisconnect();
//	GAMESVR_SOCKET->ClearConnection();
	// 소켓을 다시 생성한다.
	if( GAMESVR_SOCKET->CreateSocket() ) {
		// 성공하면 연결스레드 생성하여 연결시도한다.
// 		std::string strIP = m_Param.GetStrc( "ip" );
// 		WORD port = m_Param.GetWord( "port" );
// 		GAMESVR_SOCKET->SetIpAndPort( strIP, port );
		GAMESVR_SOCKET->TryConnect();
		SetTimeout( 10 );		// 응답없음을 기다리는 시간.
	}	else {
		GAME->OnCreateFailedSocket();
		// 소켓 생성실패
		GetpMng()->SetidNextFSM( xFID_DELAY_AFTER_CONNECTING_LOGINSVR );	// 코딩으로 할때는 이편이 더 직관적이어서 일케 함.
	}
}
//////////////////////////////////////////////////////////////////////////
void XFSMLoginingG::OnStart()
{
	ID idAcc = GetParam().GetDword( "id_acc" );
	auto bOk = GAMESVR_SOCKET->SendReqLogin( GAME, idAcc );
	if( !bOk ) {
		// 전송실패
		GetpMng()->SetidNextFSM( xFID_DELAY_AFTER_CONNECTING_LOGINSVR );	// 코딩으로 할때는 이편이 더 직관적이어서 일케 함.
	}
}
//////////////////////////////////////////////////////////////////////////
void XFSMDelayAfterConnectingL::OnStart()
{
	XTimeoutMng::sGet()->Clear();
	LOGINSVR_SOCKET->DoDisconnect();
	LOGINSVR_SOCKET->ClearConnection();
	GAMESVR_SOCKET->DoDisconnect();
	GAMESVR_SOCKET->ClearConnection();
	auto sec = GetParam().GetFloat( "sec" );
	if( sec <= 0 )
		sec = 1.f;
	m_timerDelay.Set( sec );
}

void XFSMDelayAfterConnectingL::Process()
{
	XBREAK( m_timerDelay.IsOff() );
	if( m_timerDelay.IsOver() ) {
		GetpMng()->SetidNextFSM( xFID_CONNECTING_LOGINSVR );
		m_timerDelay.Off();
		return;
	}
		
}
//////////////////////////////////////////////////////////////////////////
void XFSMDelayAfterConnectingG::OnStart()
{
	XTimeoutMng::sGet()->Clear();
	GAMESVR_SOCKET->DoDisconnect();
	GAMESVR_SOCKET->ClearConnection();
	auto sec = GetParam().GetFloat( "sec" );
	if( sec <= 0 )
		sec = 1.f;
	m_timerDelay.Set( sec );
}

void XFSMDelayAfterConnectingG::Process()
{
	XBREAK( m_timerDelay.IsOff() );
	if( m_timerDelay.IsOver() ) {
		GetpMng()->SetidNextFSM( xFID_CONNECTING_GAMESVR );
		m_timerDelay.Off();
		return;
	}
}
