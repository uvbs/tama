#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "XFramework/server/XNetworkConnection.h"
#include "XFramework/XDBAccount.h"
// #include "Network/XPacket.h"
//#include "XLock.h"
//#include "XFramework/server/XEUser.h"
//#include "XCrypto.h"
//#include "XFramework/XDBMng2.h"

class XDBAccount;
class XEUser;
class XEWinSocketSvr;
class XEWinConnectionInServer;

XE_NAMESPACE_START( XE )
XE_NAMESPACE_END; // XE

/**
 서버쪽으로 클라이언트가 접속해오면 서버쪽에 생성되는 커넥션 객체
 서버가 되는쪽(!!)에서만 쓴다.
 (!!: 클라도 다른클라가 붙으면 서버가 될수있다는 뜻임)
*/
class XEWinConnectionInServer : public XENetworkConnection
															, public std::enable_shared_from_this<XEWinConnectionInServer>
{
	XSPLock m_spLock;
	static int s_numCreateObj;		// 생성된 커넥션 수-메모리 릭 조사용
public:
	static int sGetNumCreateObj() {
		return s_numCreateObj;
	}
private:
	//
	enum xtModeHB {
		xHB_READY,			// 다음 HB를 보내기까지 딜레이중
		xHB_REQUEST,		// HB를 보내고 응답이 오기를 기다리는중.
//		xHB_NOW_WORKING,	// 클라가 잠시 할일이 있는중(결제라도 클라가 죽어버릴수 있으니 이런건 없는게 나을듯.) 차라리 타이머를 자율적으로 리셋할수 있도록.
	};
	ID m_idConnectUserDef;		// 유저 커스텀 정의용 커넥션 아이디
  ID m_idAccount = 0;       // 이 커넥션의 계정 아이디
	SOCKET m_Socket;
	TCHAR m_szIP[ 16 ];
//	OVERLAPPED m_op;
	XE::xOVERLAPPED m_opSend;
	XE::xOVERLAPPED m_opRecv;
	BYTE m_Buffer[ 1024 ];
//	BOOL m_bFlush;				// m_Buffer의 내용을 큐에다 다 밀어넣었으면 1이 된다.
	XEWinSocketSvr *m_pSocketSvr;	// 걍 내 부모가 누군지 알자는 차원.
	CTimer m_timerHeartBeat;
	CTimer m_timerAsyncDestroy;			// 비동기 파괴 타이머( 타이머가 켜진상태라고 해서 IsDestroy()로 인식해선 안됨. 말그대로 일정시간후 커넥션 삭제를 의미함)
	xtModeHB m_modeHeartBeat;		// 0: 대기중 1:요청중.
	bool m_bAutoDisconnect = true;	// 응답없으면 자동으로 끊을지..
	_tstring m_strtTag;			// 커넥션 태그(디버깅용)
	bool m_bConnectedFlag = false;		// connect되면 true되었다가 SendConnectComplete()보낸후 다시 false된다.
	/**
	 커넥션이 소유할수 있는 컨텐츠성격의 객체. NULL일수도 있다.
	 XUser는 독자적으로 생성되 돌아다닐수 없고 반드시 커넥션에 종속적이다.
	*/
	XSPUserBase m_spUser;		
	//
	void Init() {
		m_Socket = 0;
		m_szIP[0] = 0;
		memset( &m_opSend, 0, sizeof(m_opSend) );
		memset( &m_opRecv, 0, sizeof( m_opRecv ) );
		memset( m_Buffer, 0, sizeof(m_Buffer) );
//		m_bFlush = FALSE;
		m_pSocketSvr = NULL;
		m_cntDoubt = 0;
//		m_pUser = NULL;
		m_modeHeartBeat = xHB_READY;
		m_idConnectUserDef = 0;
	}
	void Destroy();
	void CloseSocket() {
		if( m_Socket )
			closesocket( m_Socket );
		m_Socket = 0;
	}
protected:
	int m_cntDoubt;
	SET_ACCESSOR( ID, idConnectUserDef );
  SET_ACCESSOR( ID, idAccount );
//	GET_READONLY_ACCESSOR( XWinSocketSvr*, pSocketSvr );
public:
	XEWinConnectionInServer( LPCTSTR szTag, XEWinSocketSvr *pSockServer, SOCKET socket, LPCTSTR szIP, int sizeQueue );
	virtual ~XEWinConnectionInServer() { Destroy(); }
	//
	XSPWinConnInServer GetThis() {
		return shared_from_this();
	}
  GET_READONLY_ACCESSOR( ID, idAccount );
	GET_READONLY_ACCESSOR( LPCTSTR, szIP );
	GET_SET_ACCESSOR( XSPUserBase, spUser );		// retain으로 하지 않아서 내가 '참조'가 아니라 완전히 소유하고 있다는 의미
	GET_ACCESSOR_CONST( ID, idConnectUserDef );
	GET_SET_ACCESSOR_CONST( bool, bAutoDisconnect );
	GET_ACCESSOR( XSPLock&, spLock );
	GET_SET_BOOL_ACCESSOR( bConnectedFlag );
	//
protected:
//	friend void XDBMng2<XEWinConnectionInServer>::Process();
	void Send( XPacket& ar );
public:
	// ** 이함수는 엔진내부용으로 사용하기 위한 용도. 엔진개발자가 아니면 쓰지 말것.
	inline void __SendForXE( XPacket& ar ) {
		Send( ar );
	}
	/*virtual */void Process();
	XSPUserBase CreateAddUser( XSPDBAcc	spAcc );
	// 접속을 끊어라(이미 하드웨어적으로 끊긴상태면, 내부변수들도 끊긴상태로 바꿔놔라)
	virtual void DoDisconnect() override;
	// virtual
	virtual void OnDestroy() {}
	virtual BOOL IsDisconnected() const override;
//	virtual int Release() override;
  /// 서버측에서 커넥션을 잘라버리기 직전에 호출된다.  // 살아있는 커넥션을 서버가 명시적으로 잘라낼때만 해당된다.
  virtual void OnFireConnectBefore() {}
	virtual void OnDisconnect() {}
	void ResetHeartBeatTimer( int secTimeout );
	/// sec초를 기다린 후 소켓을 닫는다.
	void DoAsyncDestroy( float sec ) {
		m_timerAsyncDestroy.Set( sec );
//		CONSOLE("DoAsyncDestroy");
	}
	/// 비동기로 죽고 있는 중인지
	bool IsDying() const {
		return m_timerAsyncDestroy.IsOn() != FALSE;
	}
// 	inline bool IsDestroy() {
// //		XLOCK_OBJ;
// 		return GetbDestroy();
// // 		return GetbDestroy() || m_timerAsyncDestroy.IsOn();
// 	}
private:
	BOOL PumpPacket( XPacket *pOutPacket );
	void tWSARecv();
	bool tRecvData( DWORD readbytes );
	void SendData( const BYTE *pBuffer, int len );
	void ClearConnection();
	void ProcessHeartBeat();
	void ProcesssAsyncDisconnect();
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) = 0;
friend class XEWinSocketSvr;
};




