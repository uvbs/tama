/********************************************************************
	@date:	2015/03/28 11:52
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XConnector.h
	@author:	xuzhu
	
	@brief:	연결 관리자
*********************************************************************/
#pragma once
#include "_Wnd2/XWnd.h"
#include "Network/XNetworkDelegate.h"

#ifdef WIN32
//#define AUTO_CONNECT	// 서버연결이 안될때 자동접속을 시도할지 말지.
#else
#define AUTO_CONNECT	// 서버연결이 안될때 자동접속을 시도할지 말지.
#endif


class XSockLoginSvr;
class XSockGameSvr;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/03/28 11:52
*****************************************************************/
class XConnector : /*public XWnd,*/ public XNetworkDelegate
{
public:
	static std::shared_ptr<XConnector>& sGet();
private:
	static std::shared_ptr<XConnector> s_spInstance;
public:
	struct xLOGIN_INFO {
		ID idAccount = 0;
		_tstring strIP;
		WORD port = 0;
	};
	// FSM 상태
	enum xtFSMConnectState {
		xFS_NONE,
//		xFS_CREATE_SOCKET_OBJ,	// 소켓객체를 생성한다. 처음에 한번만 이루어진다.
		xFS_CONNECT_LOGINSVR,	// 로그인서버로 연결시도
		xFS_RECONNECT_TO_LOGINSVR,	// 소켓을 재생성하고 로긴서버로 재연결을 시도함.
// 		xFS_RECONNECT_LOGINSVR, // 로그인서버로 재연결시도
		xFS_CONNECTED_LOGINSVR,	// 로그인서버에 connect()됨. 접속서버정보 기다리는중.
		xFS_SUCCESS_LOGIN_LOGINSVR,		// 로그인서버에 접속이 성공해서 게임서버정보를 받음.
		xFS_CONNECT_GAMESVR,	// 게임서버로 연결시도
// 		xFS_RECONNECT_GAMESVR,	// 게임서버로 재연결시도
		xFS_CONNECTED_GAMESVR,	// 게임서버에 connect()됨. 계정정보 기다리는 중
		xFS_ONLINE,		// 게임서버에 접속이 성공함
		xFS_DELAY_AFTER_CONNECT_LOGINSVR,	// 1초 딜레이 후 로긴서버 재접속
		xFS_DISCONNECT_GAMESVR,	// 게임서버와 연결이 끊긴후 대기시간
		xFS_GOTO_START,			// 접속을 끊고 게임 초기화면으로 돌아감
	};
	// FSM 이벤트
	enum xtFSMEvent {
		xFE_NONE,
		xFE_INIT,
		xFE_PROCESS,
		xFE_UNINIT,
		xFE_ETC
	};
private:
	xtFSMConnectState m_fsmStateConnect = xFS_NONE;		// 현재 FSM상태
	_tstring m_strNickname;
	xLOGIN_INFO m_LoginInfo;	// 게임서버 접속용 로그인 정보
	CTimer m_timerTimeout;		// 공통 타임아웃 타이머
	CTimer m_timerDelay;
	int m_numRetryForGameSvr = 0;			// 재시도 횟수.
	void Init() {}
	void Destroy();
public:
	XConnector();
	virtual ~XConnector() { Destroy(); }
	//
	GET_SET_TSTRING_ACCESSOR( strNickname );
	GET_ACCESSOR( CTimer&, timerTimeout );
	GET_ACCESSOR( xtFSMConnectState, fsmStateConnect );
	//
	void OnCreate();
	void Process( float dt );
private:
	XSockGameSvr* CreateGameSvrSocketObj( std::string strIP, WORD port, DWORD param );
	XSockLoginSvr* CreateLoginSocketObj( DWORD param );
	void DelegateConnect( XENetworkConnection *pConnect, DWORD param ) override;
 	void DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error ) override;
public:
	XSockLoginSvr* TryConnectLoginSvr( bool bReconnect );
	XSockGameSvr* TryConnectGameSvr( bool bReconnect );
	void OnLoginedLoginSvr( ID idAccount, _tstring& strIP, WORD port );
	void ReconnectLoginSvr( float secDelay = 0 );
	void StopFSM() {
		ChangeFSMConnect( xFS_NONE );
	}
	LPCTSTR GetStringStateConnect() {
		return GetStringStateConnect( m_fsmStateConnect );
	}
	bool IsOnline() {
		return m_fsmStateConnect == xFS_ONLINE;
	}
	bool IsDisconnect() {
// 		if( m_fsmStateConnect != xFS_NONE && m_fsmStateConnect != xFS_ONLINE ) {
// 			XBREAK( GAMESVR_SOCKET->IsConnected() );	// state는 끊어진 상
// 		}
		return m_fsmStateConnect != xFS_NONE && m_fsmStateConnect != xFS_ONLINE;
	}
	void Initialize() {
		m_fsmStateConnect = xFS_NONE;
		m_timerTimeout.Off();
		m_timerDelay.Off();
		m_numRetryForGameSvr = 0;
	}
	void Draw();
	// fsm
public:
	void ChangeFSMConnect( xtFSMConnectState stateNew, DWORD param = 0 );
	int SendFSMEvent( xtFSMConnectState state, xtFSMEvent event, DWORD param );
private:
	int DoFSMConnect( xtFSMConnectState state, xtFSMEvent event, DWORD param );
	LPCTSTR GetStringStateConnect( xtFSMConnectState state );
//	int FSMCreateSocketObj( xtFSMEvent event, DWORD param );
	int FSMDoConnectLoginSvr( xtFSMEvent event, DWORD param );
	int FSMConnectedLoginSvr( xtFSMEvent event, DWORD param );
	int FSMReconnectToLoginSvr( xtFSMEvent event, DWORD param );
	int FSMDoConnectGameSvr( xtFSMEvent event, DWORD param );
	int FSMConnectedGameSvr( xtFSMEvent event, DWORD param );
	int FSMOnline( xtFSMEvent event, DWORD param );
	bool RecreateSocketForLoginSvr( DWORD param );
	bool RecreateSocketForGameSvr( DWORD param );
	int FSMDelayAfterConnectLoginSvr( xtFSMEvent event, DWORD param );
// 	void DoConnectGameSvr( void ) {
// 		ChangeFSMConnect( xFS_CONNECT_GAMESVR, 0 );
// 	}
// 	void DoConnectLoginSvr( XGAME::xtConnectParam typeConnect ) {
// 		ChangeFSMConnect( xFS_CONNECT_LOGINSVR, (DWORD)typeConnect );
// 	}
public:
	void DoConnectLoginSvrByUUID();
// 	void DoLoginIDPW();
  void DoDisconnectLoginSvr();
  void DoDisconnectGameSvr();
}; // class XConnector

