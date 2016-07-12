/********************************************************************
	@date:	2015/03/28 11:52
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XConnector.h
	@author:	xuzhu
	
	@brief:	연결 관리자
*********************************************************************/
#pragma once
//#include "_Wnd2/XWnd.h"
#include "Network/XNetworkDelegate.h"
#include "XFSMConnector.h"

#ifdef WIN32
//#define AUTO_CONNECT	// 서버연결이 안될때 자동접속을 시도할지 말지.
#else
#define AUTO_CONNECT	// 서버연결이 안될때 자동접속을 시도할지 말지.
#endif


class XSockLoginSvr;
class XSockGameSvr;
class XFSMBaseConnector;
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
// 	struct xLOGIN_INFO {
// 		ID idAccount = 0;
// 		_tstring strIP;
// 		WORD port = 0;
// 	};
private:
	XLock m_Lock;
	XVector<XFSMBaseConnector*> m_aryFSMObj;
	XFSMBaseConnector *m_pNextFSM = nullptr;
	XFSMBaseConnector *m_pCurrFSM = nullptr;
// 	_tstring m_strNickname;
// 	std::string m_strID;
// 	std::string m_strPassword;
//	xLOGIN_INFO m_LoginInfo;	// 게임서버 접속용 로그인 정보
	void Init() {}
	void Destroy();
public:
	XConnector();
	virtual ~XConnector() { Destroy(); }
	//
//	GET_ACCESSOR_CONST( const _tstring&, strNickname );
	void SetidNextFSM( xConnector::xtFSMId idFSM ) {
		XLockObj lockObj( &m_Lock );
		auto fsm = FindFSMObj( idFSM );
		if( XASSERT(fsm) )
			m_pNextFSM = fsm;
	}
	void SetidNextFSM( xConnector::xtFSMId idFSM, const XParamObj& paramObj ) {
		XLockObj lockObj( &m_Lock );
		auto fsm = FindFSMObj( idFSM );
		if( XASSERT( fsm ) )
			fsm->SetParam( paramObj );
			m_pNextFSM = fsm;
	}
	/// 커넥팅 처리를 중지시켜 자동연결을 중지시킨다.
	void StopFSM() {
		XLockObj lockObj( &m_Lock );
		m_pCurrFSM = nullptr;
		m_pNextFSM = nullptr;
	}
	//
	void OnCreate();
	void Process( float dt );
	void Draw();
	void DispatchFSMEvent( xConnector::xtFSMEvent event, const XParamObj& paramObj );
	void DispatchFSMEvent( xConnector::xtFSMEvent event );
	void SetParam( xConnector::xtFSMId idFSM, const XParamObj& paramObj );
	void SetConnectParamByUUID( const std::string& strcUUID );
	void SetConnectParamByIDPW( const std::string& strcID, const std::string& strcPassword );
	void SetConnectParamByNewAcc( const _tstring& strNick
															, XGAME::xtConnectParam typeLogin
															, const std::string& strFbUserId );
	void SetConnectParamByFacebook( const std::string& strFbUserid );
	bool IsOnline();
	// 서버에 접속중인가.
	bool IsConnecting() {
		if( m_pCurrFSM ) {
			XBREAK( m_pCurrFSM->GetidFSM() == xConnector::xFID_NONE );
			return m_pCurrFSM->GetidFSM() != xConnector::xFID_ONLINE;
		}
		// fsm이 없으면 온라인도 아니지만 접속중도 아님.
		return false;
	}
private:
	XSockGameSvr* CreateGameSvrSocketObj();
	XSockLoginSvr* CreateLoginSocketObj();
	void DelegateConnect( XENetworkConnection *pConnect, DWORD param ) override;
	void DelegateNetworkError( XENetworkConnection *pConnect, XE::xtNError error ) override;
	XFSMBaseConnector* FindFSMObj( xConnector::xtFSMId idFSM );
	void CreateFSMs();
}; // class XConnector

