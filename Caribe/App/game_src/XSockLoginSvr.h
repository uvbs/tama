#pragma once
#include "XPacketCL.h"
#include "Network/XSocket.h"
#include "XFramework/client/XTimeoutMng.h"
#include "XSceneTitle.h"
#include "XGame.h"
#include "XGameWnd.h"

namespace XGAME {
};

//#define XWND_ALERT(...)	{}

// #define _XCHECK_CONNECT( ID_TEXT ) \
// 	if( IsDisconnected() ) { \
// 		int _idText = ID_TEXT; \
// 		if( _idText == 0 )	_idText = 6; \
// 		XWND_ALERT( "%s", XTEXT(_idText) ); \
// 		return FALSE; \
// 		}
// 
class XWnd;
class XSockLoginSvr : public XSOCKET
{
public:
// 	static std::shared_ptr<XSockLoginSvr>& sGet();
private:
private:
	struct xCALLBACK {
		ID idPacket;
		void (XSOCKET::*pCallback)( XPacket& p );
		xCALLBACK() {
			idPacket = 0;
			pCallback = nullptr;
		}
	};
	std::map<ID,xCALLBACK> m_mapCallback;
private:
// 	static std::shared_ptr<XSockLoginSvr> s_spInstance;
	BOOL m_bReconnect;
	int m_numRetryConnectGameSvr;		// 게임서버재접속 시도 횟수.
	void Init() {
		m_bReconnect = FALSE;
		m_numRetryConnectGameSvr = 0;
	}
	void Destroy() {}
public:
	XSockLoginSvr( XNetworkDelegate *pDelegate );
	virtual ~XSockLoginSvr() { Destroy(); }
	//
	GET_SET_ACCESSOR( BOOL, bReconnect );
	BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) override;
	/**
	 각 리시브 패킷이 호출해야할 콜백함수를 등록함.
	 클라의 요청없이 날아오는 패킷류를 이것으로 등록한다.
	*/
	template<typename T>
	void AddRecv( ID idPacket, T func ) {
		typedef void (XSOCKET::*CALLBACK_FUNC)( XPacket& p );
		xCALLBACK c;
		c.idPacket = idPacket;
		c.pCallback = static_cast<CALLBACK_FUNC>( func );
		m_mapCallback[ idPacket ] = c;
	}
	/**
	 Request에 응답할 Response 콜백함수들을 등록한다.
	 콜백함수를 초기화함수에서 미리 등록하지 않는이유는 보내고 받는 함수를 
	 한자리에 몰아놓음으로써 작업자가 따라기기 쉽게 하기 위해서다. 
	 AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	 AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
	template<typename T>
	ID AddResponse( ID idPacket, T func, XWnd *pTimeoutCallback ) {
		ID idKey = XE::GenerateID();
		AddResponse( idKey, idPacket, func, pTimeoutCallback );
		return idKey;
	}
	template<typename T>
	ID AddResponse( ID idKey, ID idPacket, T func, XWnd *pTimeoutCallback ) {
		typedef void (XSOCKET::*CALLBACK_FUNC)( XPacket& p );
		std::map<ID, xCALLBACK>::iterator itor;
		itor = m_mapCallback.find( idPacket );
		if( itor == m_mapCallback.end() )
		{
			// 찾아보고 없으면 등록시키고 있으면 재등록 시키지 않는다.
			xCALLBACK c;
			c.idPacket = idPacket;
			c.pCallback = static_cast<CALLBACK_FUNC>( func );
			m_mapCallback[ idPacket ] = c;
		}
		if( pTimeoutCallback )
//			XTimeoutMng::sGet()->AddByKey( pTimeoutCallback->GetID(), 
			XTimeoutMng::sGet()->AddByKey( GAME->getid(),
											idKey, 
											idPacket, 
											TRUE,		// 응답올때까지 입력 blocking
											30.f );		// 타임아웃 시간 10초
		return idKey;
	}
	// 요청/응답류 send함수. 보낸 패킷아이디를 식별자로 타임아웃이 처리된다.
	/**
	 AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	 AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
	template<typename T>
	BOOL SendRequest( XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout )
	{
		ID idKey = XE::GenerateID();
		return SendRequest( idKey, ar, func, pTimeoutCallback, idTextTimeout );
	}
	template<typename T>
	BOOL SendRequest( ID idKey, XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout )
	{
		typedef int (XSOCKET::*CALLBACK_FUNC)( XPacket& p );
		_XCHECK_CONNECT( idTextTimeout );	// 커넥션이 끊어져있다면 알림창을 띄우게 하고 FALSE를 리턴한다.
		DWORD idPacket = ar.GetPacketHeader();
		AddResponse( idKey, idPacket, func, pTimeoutCallback );
		Send( ar );
		return TRUE;
	}
	BOOL CallRecvCallback( ID idPacket, XPacket& p );

	//
	//	BOOL SendRequest( XPacket& ar, DWORD idTextTimeout=0 );
	BOOL SendNormal( XPacket& ar, ID idTextDisconnect=0 );
	BOOL SendPacketHeaderOnly( /*XWnd *pTimeoutCallback,*/ xtCLPacket idPacket/*, BOOL bTimeout=TRUE*/ );
// 	void SendRequestPacketHeaderOnly( XWnd *pCallback, xtCLPacket idPacket );
	//
	void SendRequestAccount( ID idAccount );
	BOOL SendReqCreateAccount( XWnd *pCallback, const _tstring& strNickname, const std::string& strFbUserId, const std::string& strDeviceId );
	BOOL SendLoginFromUUID( XWnd *pCallback, const char *cUUID, const std::string& strDeviceId );
	inline BOOL SendLoginFromUUID( XWnd *pCallback, const std::string& strcUUID, const std::string& strDeviceId ) {
		return SendLoginFromUUID( pCallback, strcUUID.c_str(), strDeviceId );
	}
	BOOL SendLoginFromIDPW(XWnd *pCallback, const char *cID, const char *cPW, const std::string& strDeviceId );
	inline BOOL SendLoginFromIDPW( XWnd *pCallback, const std::string& strcID, const std::string& strcPW, const std::string& strDeviceId ) {
		return SendLoginFromIDPW( pCallback, strcID.c_str(), strcPW.c_str(), strDeviceId );
	}
	bool SendLoginFromFacebook( XWnd *pCallback, const std::string& strcFbUserid, const std::string& strDeviceId );
	void RecvNoAccount(XPacket& p);
//	void RecvNewAccount( XPacket& p ) {}
  void RecvLoginLockForBattle( XPacket& p );
  void RecvLoginLockFree( XPacket& p );
	//
	void RecvMsgBox( XPacket& p );
	void RecvInvalidClient( XPacket& p );
	void RecvLoginSuccess(XPacket& p);
// 	void RecvLoginFromIDPW(XPacket& p);
	void RecvDuplicateLogout(XPacket& ar);
	void RecvReconnectTry( XPacket& ar );
	void RecvDiffPacketVer( XPacket& p );
	void RecvDuplicateNickName(XPacket& p);
	void RecvWrongPassword(XPacket &p);
	void RecvClosedServer(XPacket &p);
	BOOL SendCrashDump( ID idAcc, bool *pOutRet );
	void SerializeCrashDump( XArchive& ar, ID idAcc );
	void cbOnDisconnectByExternal() override;
};

extern XSockLoginSvr *LOGINSVR_SOCKET;			// 로긴서버와의 커넥션
