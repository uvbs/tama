#pragma once

#include "../../../game_src/XPacketCG.h"
#include "XWinSocketBot.h"

namespace XGAME {
};

class XWnd;

/**
@brief 클라이언트에서 게임서버쪽으로의 연결을 하게될 소켓객체
*/
class XSockGameSvrBot : public XWinSocketBot
{
public:
// 	static std::shared_ptr<XSockGameSvr>& sGet();
private:
// 	static std::shared_ptr<XSockGameSvr> s_spInstance;
// 	struct xCALLBACK_PARAM {
// 		ID idParam;
// 	};
// 	struct xPARAM_RECON_SPOT : public xCALLBACK_PARAM {
// 		int sample;
// 	};
	struct xCALLBACK;
	struct xCALLBACK {
		void (XWinSocketBot::*pCallback)(XPacket& p, const xCALLBACK& c);
		ID idPacket;
//		ID idCallback;
		xCALLBACK() {
			idPacket = 0;
//			idCallback = 0;
			pCallback = nullptr;
		}
	};
	std::map<ID, xCALLBACK> m_mapCallback;
// 	struct xCALLBACK_WIN {
// 		void (XWnd::*pCallback)(xCALLBACK_PARAM *pParam);
// 		xCALLBACK_WIN() {
// 			pCallback = nullptr;
// 		}
// 	};
// 	std::map<std::string, xCALLBACK_WIN> m_mapCallback_Win;
private:
	BOOL m_bReconnect;
	int m_numRetryConnectGameSvr;		///< 게임서버재접속 시도 횟수.
	XCrypto m_CryptoObj;
	void Init() {
		m_bReconnect = FALSE;
		m_numRetryConnectGameSvr = 0;
	}
	void Destroy() {}
public:
	XSockGameSvrBot(XBotObj* pOwner, XNetworkDelegate *pDelegate );
	virtual ~XSockGameSvrBot() { Destroy(); }
	//
	GET_SET_ACCESSOR(BOOL, bReconnect);
	BOOL ProcessPacket(DWORD idPacket, ID idKey, XPacket& p) override;
	/**
	각 리시브 패킷이 호출해야할 콜백함수를 등록함.
	클라의 요청없이 날아오는 패킷류를 이것으로 등록한다.
	*/
	template<typename T>
	void AddRecv(ID idPacket, T func) {
		typedef void (XSOCKET::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		xCALLBACK c;
		c.idPacket = idPacket;
		c.pCallback = static_cast<CALLBACK_FUNC>(func);
		m_mapCallback[idPacket] = c;
	}
	/**
	Request에 응답할 Response 콜백함수들을 등록한다.
	콜백함수를 초기화함수에서 미리 등록하지 않는이유는 보내고 받는 함수를
	한자리에 몰아놓음으로써 작업자가 따라기기 쉽게 하기 위해서다.
	AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
private:
	template<typename T>
	ID _AddResponse(ID idKey, ID idPacket, T func ) {
		typedef void (XWinSocketBot::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		auto itor = m_mapCallback.find(idPacket);
		if (itor == m_mapCallback.end())	{
			// 찾아보고 없으면 등록시키고 있으면 재등록 시키지 않는다.
			xCALLBACK c;
			c.idPacket = idPacket;
// 			if (pTimeoutCallback)
// 				c.idCallback = pTimeoutCallback->getid();	// << 뭐에 쓰는거지?
			c.pCallback = static_cast<CALLBACK_FUNC>(func);
			m_mapCallback[idPacket] = c;
		}
// 		if (pTimeoutCallback)	{
// 			//			XTimeoutMng::sGet()->AddByKey( pTimeoutCallback->GetID(), 
// 			XTimeoutMng::sGet()->AddByKey(GAME->getid(),
// 				idKey,
// 				idPacket,
// 				TRUE,		// 응답올때까지 입력 blocking
// 				secTimeout );		// 타임아웃 시간
// 		}

		return idKey;
	}
public:
	template<typename T>
	ID AddResponse(ID idPacket, T func, XWnd *pTimeoutCallback, float secTimeout = 10.f) {
		ID idKey = XE::GenerateID();
		_AddResponse(idKey, idPacket, func);
		return idKey;
	}
	template<typename T>
	ID AddResponse(ID idKey, ID idPacket, T func, XWnd *pTimeoutCallback, float secTimeout = 10.f ) {
		return _AddResponse(idKey, idPacket, func );
	}
	// 요청/응답류 send함수. 보낸 패킷아이디를 식별자로 타임아웃이 처리된다.
	/**
	AddResponse는 SendRequest보다 먼저 실행하는게 좋다. 확률은 낮지만
	AddResponse하기전에 응답패킷이 도착해버릴 가능성도 있기때문이다.
	*/
	template<typename T>
	BOOL SendRequest(XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout) {
		ID idKey = XE::GenerateID();
		return SendRequest(idKey, ar, func, pTimeoutCallback, idTextTimeout);
	}
	template<typename T>
	BOOL SendRequest(ID idKey, XPacket& ar, T func, XWnd *pTimeoutCallback, DWORD idTextTimeout) {
		typedef int (XSOCKET::*CALLBACK_FUNC)(XPacket& p, const xCALLBACK& c);
		_XCHECK_CONNECT(idTextTimeout);	// 커넥션이 끊어져있다면 알림창을 띄우게 하고 FALSE를 리턴한다.
		DWORD idPacket = ar.GetPacketHeader();
		AddResponse(idKey, idPacket, func, pTimeoutCallback, 10.f );
		Send(ar);
		return TRUE;
	}
	BOOL CallRecvCallback(ID idPacket, XPacket& p);

	BOOL SendNormal(XPacket& ar, ID idTextDisconnect = 0);
	BOOL SendPacketHeaderOnly( /*XWnd *pTimeoutCallback,*/ xtPacket idPacket/*, BOOL bTimeout=TRUE*/);
	BOOL SendReqLogin( ID idAccount);
	BOOL SendReqRegistName( const _tstring& strID, const _tstring& strPassword );

private:
	void RecvSuccessLogin( XPacket& p, const xCALLBACK& c );
	void RecvNoAccount( XPacket& p, const xCALLBACK& c ) {}
	void RecvNewAccount( XPacket& p, const xCALLBACK& c ) {}
	void RecvInvalidVerify( XPacket& p, const xCALLBACK& c );
	void RecvDuplicateLogout( XPacket& ar, const xCALLBACK& c );
	void RecvReconnectTry( XPacket& ar );
	void RecvIsOnline( XPacket& p, const xCALLBACK& c );
	void cbOnDisconnectByExternal() override;
	void RecvRegistID( XPacket& p, const xCALLBACK& c );
	void RecvReconnectTryToGameSvr( XPacket& p, const xCALLBACK& c );
};


