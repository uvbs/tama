/********************************************************************
	@date:	2016/03/02 12:54
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj\Bot\Bot\XBotObj.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "Network/XNetworkDelegate.h"
#include "XFSMConnector.h"

//#define XLOGB( F, ... )		CONSOLE(F,##__VA_ARGS__)
#define XLOGB( F, ... )		(0)

class XSockLoginSvrBot;
class XSockGameSvrBot;
class XAccount;
class XFSMBaseConnector;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 12:54
*****************************************************************/
class XBotObj : public XNetworkDelegate
{
public:
public:
	XBotObj( ID idObj );
	virtual ~XBotObj() { Destroy(); }
	// get/setter
	GET_BOOL_ACCESSOR( bDestroy );
	GET_SET_ACCESSOR( XAccount*, pAcc );
	GET_SET_ACCESSOR( const std::string&, strcUUID );
	GET_ACCESSOR_CONST( ID, idObj );
//	GET_ACCESSOR_CONST( XGAME::xtState, stateConnect );
	GET_ACCESSOR_CONST( XSockLoginSvrBot*, pSockLoginSvr );
	GET_ACCESSOR_CONST( XSockGameSvrBot*, pSockGameSvr );
	GET_ACCESSOR_CONST( ID, idAcc );
	GET_SET_ACCESSOR_CONST( const _tstring&, strName );
//	GET_SET_ACCESSOR_CONST( const _tstring&, strID );
		// public member
	void OnLoginSuccessByLoginSvr( ID idAcc, const std::string& strcIP, WORD port );
	void OnLoginSuccessByGameSvr();
	void OnStart();
	void Process( float dt );
	void OnCreate();
	void OnNoAccount( XGAME::xtConnectParam param );
	void OnDuplicateNick();
	_tstring GetstrtNickGenerate() {
		return XE::Format(_T("Bot%d"), m_idObj );
	}
// 	std::string GetstrcIDGenerate() {
// 		return XE::Format("Bot%d", m_idObj );
// 	}
	std::string GetstrcUUIDGenerate() {
		return XE::Format( "Bot%d", m_idObj );
	}
	xConnector::xtFSMId GetstateConnect() const {
		if( m_pCurrFSM )
			return m_pCurrFSM->GetidFSM();
		return xConnector::xFID_NONE;
	}
	void DispatchFSMEvent( xConnector::xtFSMEvent event, const XParamObj& paramObj );
	void DispatchFSMEvent( xConnector::xtFSMEvent event );
private:
	// private member
	int m_idObj = 0;
	XSockLoginSvrBot *m_pSockLoginSvr = nullptr;
	XSockGameSvrBot *m_pSockGameSvr = nullptr;
	bool m_bDestroy = false;
	XAccount* m_pAcc = nullptr;
	std::string m_strcUUID;
	ID m_idAcc = 0;
	std::string m_strcIP;
	WORD m_Port = 0;
	_tstring m_strName;
// 	_tstring m_strID;
//	XGAME::xtState m_stateConnect = XGAME::xST_NONE;		// 현재 접속상태
	XVector<XFSMBaseConnector*> m_aryFSMObj;
	XFSMBaseConnector *m_pNextFSM = nullptr;
	XFSMBaseConnector *m_pCurrFSM = nullptr;
	XLock m_Lock;
private:
	// private method
	void Init() {}
	void Destroy();
	void DelegateConnect(  XENetworkConnection *pConnect, DWORD param ) override;
	void CreateFSMs();
	XFSMBaseConnector* FindFSMObj( xConnector::xtFSMId idFSM );
	void SetParam( xConnector::xtFSMId idFSM, const XParamObj& paramObj );
	/// 커넥팅 처리를 중지시켜 자동연결을 중지시킨다.
	// FSM의 변화는 이걸로 하지말고 DispatchFSMEvent()를 사용해야함.
	void SetidNextFSM( xConnector::xtFSMId idFSM ) {
		XAUTO_LOCK2( &m_Lock );
		auto fsm = FindFSMObj( idFSM );
		if( XASSERT( fsm ) )
			m_pNextFSM = fsm;
	}
	void SetidNextFSM( xConnector::xtFSMId idFSM, const XParamObj& paramObj ) {
		XAUTO_LOCK2( &m_Lock );
		auto fsm = FindFSMObj( idFSM );
		if( XASSERT( fsm ) )
			fsm->SetParam( paramObj );
		m_pNextFSM = fsm;
	}
	void StopFSM() {
		XAUTO_LOCK2( &m_Lock );
		m_pCurrFSM = nullptr;
		m_pNextFSM = nullptr;
	}
friend class XFSMDelayAfterConnectingL;
friend class XFSMDelayAfterConnectingG;
}; // class XBotObj
