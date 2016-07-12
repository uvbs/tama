#include "stdafx.h"
#include "XBotObj.h"
#include "BotView.h"
#include "XAccount.h"
#include "XSockLoginSvrBot.h"
#include "XSockGameSvrBot.h"
#include "XFSMConnector.h"
#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xConnector;

////////////////////////////////////////////////////////////////
XBotObj::XBotObj( ID idObj )
{
	Init();
	m_idObj = idObj;
}

void XBotObj::Destroy()
{
}

void XBotObj::OnCreate()
{
	m_pSockLoginSvr = new XSockLoginSvrBot( this, this );
//	m_pSockLoginSvr->SetIpAndPort( "192.168.0.191", 8300 );
//	m_pSockLoginSvr->SetIpAndPort( "211.125.93.154", 8300 );
	auto& option = XMain::sGet()->GetOption();
	std::string strIP = "211.125.93.154";
	if( !option.GetstrIP().empty() ) {
		strIP = option.GetstrIP();
	}
	m_pSockLoginSvr->SetIpAndPort( strIP.c_str(), 8300 );
	//
	m_pSockGameSvr = new XSockGameSvrBot( this, this );
//	m_strcUUID = XE::GetUUID();
//	m_strcUUID = XE::Format("%040d", m_idObj );
	CreateFSMs();
}

void XBotObj::OnStart()
{
	DispatchFSMEvent( xFE_START_CONNECTING );
//	SetidNextFSM( xConnector::xFID_CONNECTING_LOGINSVR );
// 	if( XASSERT(m_pSockLoginSvr->CreateSocket()) ) {
// 		m_pSockLoginSvr->TryConnect();
// 		m_stateConnect = xST_CONNECTING_L;
// 	}
}

void XBotObj::CreateFSMs()
{
	m_aryFSMObj.Add( new XFSMOffline( this ) );
	m_aryFSMObj.Add( new XFSMConnectingL( this ) );
	m_aryFSMObj.Add( new XFSMLoginingL( this ) );
	m_aryFSMObj.Add( new XFSMConnectingG( this ) );
	m_aryFSMObj.Add( new XFSMLoginingG( this ) );
	m_aryFSMObj.Add( new XFSMMakeNewAccount( this ) );
	m_aryFSMObj.Add( new XFSMOnline( this ) );
	m_aryFSMObj.Add( new XFSMDelayAfterConnectingL( this ) );
	m_aryFSMObj.Add( new XFSMDelayAfterConnectingG( this ) );
	//
	XBREAK( m_aryFSMObj.size() != xFID_MAX - 1);
	auto fsmOffline = FindFSMObj( xFID_OFFLINE );
	auto fsmConnectingL = FindFSMObj( xFID_CONNECTING_LOGINSVR );
	auto fsmLoginingL = FindFSMObj( xFID_LOGINING_LOGINSVR );
	auto fsmConnectingG = FindFSMObj( xFID_CONNECTING_GAMESVR );
	auto fsmLoginingG = FindFSMObj( xFID_LOGINING_GAMESVR );
	auto fsmMakeNewAcc = FindFSMObj( xFID_MAKE_NEW_ACCOUNT );
	auto fsmOnline = FindFSMObj( xFID_ONLINE );
	auto fsmDelayAfterConnectingL = FindFSMObj( xFID_DELAY_AFTER_CONNECTING_LOGINSVR );
	auto fsmDelayAfterConnectingG = FindFSMObj( xFID_DELAY_AFTER_CONNECTING_GAMESVR );
	//
//	fsmConnectingL->AddFSMEvent( xFE_START_CONNECTING, fsmConnectingL );
	fsmOffline->AddFSMEvent( xFE_WILL_CONNECT, fsmConnectingL );
	fsmConnectingL->AddFSMEvent( xFE_CONNECTED_LOGINSVR, fsmLoginingL );
	fsmConnectingL->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmConnectingL->AddFSMEvent( xFE_FAILED_CREATE_SOCKET, fsmDelayAfterConnectingL );
	// loginL fsm에게 이벤트를 등록한다.
	fsmLoginingL->AddFSMEvent( xFE_LOGINED_LOGINSVR, fsmConnectingG );
	fsmLoginingL->AddFSMEvent( xFE_DISCONNECTED_LOGINSVR, fsmDelayAfterConnectingL );
	fsmLoginingL->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmLoginingL->AddFSMEvent( xFE_NO_ACCOUNT, fsmMakeNewAcc );
	fsmLoginingL->AddFSMEvent( xFE_RECONNECT_TRY_LOGINSVR, fsmDelayAfterConnectingL );

	fsmConnectingG->AddFSMEvent( xFE_CONNECTED_GAMESVR, fsmLoginingG );
	fsmConnectingG->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmConnectingG->AddFSMEvent( xFE_FAILED_CREATE_SOCKET, fsmDelayAfterConnectingL );
	fsmLoginingG->AddFSMEvent( xFE_LOGINED_GAMESVR, fsmOnline );
	fsmLoginingG->AddFSMEvent( xFE_DISCONNECTED_GAMESVR, fsmDelayAfterConnectingL );
	fsmLoginingG->AddFSMEvent( xFE_NO_RESPONSE, fsmDelayAfterConnectingL );
	fsmLoginingG->AddFSMEvent( xFE_RECONNECT_TRY_GAMESVR, fsmDelayAfterConnectingG );
	fsmLoginingG->AddFSMEvent( xFE_RECONNECT_TRY_LOGINSVR, fsmDelayAfterConnectingL );
	fsmDelayAfterConnectingL->AddFSMEvent( xFE_DELAY_OVER, fsmConnectingL );
	fsmDelayAfterConnectingG->AddFSMEvent( xFE_DELAY_OVER, fsmLoginingG );
	fsmOnline->AddFSMEvent( xFE_DISCONNECTED_GAMESVR, fsmDelayAfterConnectingL );
	fsmOnline->AddFSMEvent( xFE_WILL_DISCONNECT, fsmOffline );

	fsmMakeNewAcc->AddFSMEvent( xFE_LOGINED_LOGINSVR, fsmConnectingG );
	//	m_pNextFSM = fsmConnectingL;
}

XFSMBaseConnector* XBotObj::FindFSMObj( xtFSMId idFSM )
{
	for( auto pObj : m_aryFSMObj ) {
		if( pObj->GetidFSM() == idFSM )
			return pObj;
	}
	return nullptr;
}

void XBotObj::SetParam( xtFSMId idFSM, const XParamObj& paramObj )
{
	auto fsm = FindFSMObj( idFSM );
	if( XASSERT( fsm ) )
		fsm->SetParam( paramObj );
}

void XBotObj::Process( float dt )
{
	XAUTO_LOCK2( &m_Lock );
	m_pSockLoginSvr->Process();
	m_pSockGameSvr->Process();
	//
	//	DoFSMConnect( m_fsmStateConnect, xFE_PROCESS, 0 );
	// next로 등록된 fsm을 현재  fsm으로 올린다.
	auto pCurrFSMPrev = m_pCurrFSM;
	if( m_pNextFSM ) {
		m_pCurrFSM = m_pNextFSM;
		if( m_pCurrFSM )
			m_pCurrFSM->OnStart();
		m_pNextFSM = nullptr;
	}
	if( m_pCurrFSM )
		m_pCurrFSM->Process();
}

/**
 @brief 로그인서버의 로그인이 성공함.
*/
void XBotObj::OnLoginSuccessByLoginSvr( ID idAcc, const std::string& strcIP, WORD port )
{
//	SetidNextFSM( xFID_CONNECTING_GAMESVR );
	XLOGB("%d:Logined loginsvr", m_idObj);
	DispatchFSMEvent( xFE_LOGINED_LOGINSVR );

	m_strcIP = strcIP;
	m_Port = port;
	m_idAcc = idAcc;
	{
		XAUTO_LOCK2( &m_Lock );
		m_pSockGameSvr->SetIpAndPort( m_strcIP, m_Port );
	}
// 	m_pSockLoginSvr->DoDisconnect();
// 	m_pSockLoginSvr->ClearConnection();
// 	m_pSockGameSvr->DoDisconnect();
// 	// 게임서버 소켓 생성
// 	if( m_pSockGameSvr->CreateSocket() ) {
// 		m_pSockGameSvr->TryConnect();
// 		SetidNextFSM( xFID_CONNECTING_GAMESVR );
// // 		m_stateConnect = xST_CONNECTING_G;
// 	}
}

void XBotObj::OnLoginSuccessByGameSvr()
{
//	m_stateConnect = xST_ONLINE;
//	SetidNextFSM( xFID_ONLINE );
	XLOGB( "%d:Logined gamesvr", m_idObj );
	DispatchFSMEvent( xFE_LOGINED_GAMESVR );
}

/**
 @brief connect(소켓연결)성공
*/
void XBotObj::DelegateConnect( XENetworkConnection *pConnect, DWORD param )
{
	if( pConnect->GetidConnect() == m_pSockLoginSvr->GetidConnect() ) {
		XLOGB( "%d:connected loginsvr", m_idObj );
		DispatchFSMEvent( xFE_CONNECTED_LOGINSVR );
		XMain::sGet()->m_fpsConnL.Process();
	} else
	if( pConnect->GetidConnect() == m_pSockGameSvr->GetidConnect() ) {
		XLOGB( "%d:connected gamesvr", m_idObj );
		DispatchFSMEvent( xFE_CONNECTED_GAMESVR );
		XMain::sGet()->m_fpsConnG.Process();
	}
}

void XBotObj::OnNoAccount( xtConnectParam param )
{
//	XBREAK(1);
	XLOGB( "%d:no account", m_idObj );
	XBREAK( !m_strName.empty() );
	DispatchFSMEvent( xFE_NO_ACCOUNT );
// 	const _tstring strtNick = GetstrtNickGenerate();
//  	m_pSockLoginSvr->SendReqCreateAccount( strtNick );
// 	m_stateConnect = xST_MAKE_NEW_ACCOUNT;
}

/**
 @brief 
*/
void XBotObj::OnDuplicateNick()
{
	XLOGB( "%d:OnDuplicateNick", m_idObj );
	XBREAK(1);	// DB에서 아이디/비번 없는 봇계정 다 지우고 다시 해야함.
	DispatchFSMEvent( xFE_DUPLICATE_NICK );
// 	// 닉네임과 함께 새로 계정을 생성하도록 다시 보냄.
// 	static _tstring s_strNick;
// 	XBREAK( !s_strNick.empty() );		// 한번 보냈는데 다시 온것임. 닉을 잘못보낸것임.
// 	const _tstring strtNick = XE::Format(_T("User%08d"), m_idObj );
// 	s_strNick = strtNick;
// 		m_pSockLoginSvr->SendReqCreateAccount( strtNick );
}

/**
@brief 현재 fsm에 대한 상태전환 이벤트를 처리한다.
*/
void XBotObj::DispatchFSMEvent( xtFSMEvent event, const XParamObj& paramObj )
{
	XAUTO_LOCK2( &m_Lock );
#ifndef _DEBUG
	// 디버그 모드에선 편의상 응답없음에 대한 처리를 무시한다.
	if( event == xFE_NO_RESPONSE )
		return;
#endif // not _DEBUG
	if( m_pCurrFSM || m_pNextFSM ) {
		// 현재 fsm에 이벤트 전달
		// 현재 상태가 "event"이벤트로 전환될수 있는지 검색.
		auto pfsmNext = (m_pCurrFSM)? m_pCurrFSM->GetpfsmByEvent( event ) : m_pNextFSM;
		if( XASSERT(pfsmNext) ) {
			auto idFSM = pfsmNext->GetidFSM();
			pfsmNext->SetParam( paramObj );
			// 현재 fsm종료 핸들러
			m_pCurrFSM->OnFinish();
			// 다음 fsm으로 등록.
			m_pNextFSM = pfsmNext;
		}
	} else
	if( event == xFE_START_CONNECTING ) {
		SetidNextFSM( xFID_OFFLINE );
	}
	CBotView::sGet()->Update();
}

void XBotObj::DispatchFSMEvent( xtFSMEvent event )
{
	XParamObj paramObj;
	DispatchFSMEvent( event, paramObj );
}
