#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "server/XNetworkConnection.h"
#include "Network/XPacket.h"
#include "XLock.h"
#include "server/XEUser.h"
#include "XCrypto.h"
//#include "XDBMng2.h"

class XEUser;
class XEWinSocketSvr;
class XEWinConnectionInServer;
/**
 서버쪽으로 클라이언트가 접속해오면 서버쪽에 생성되는 커넥션 객체
 서버가 되는쪽(!!)에서만 쓴다.
 (!!: 클라도 다른클라가 붙으면 서버가 될수있다는 뜻임)
*/
class XEWinConnectionInServer : public XENetworkConnection
{
	static XSharedObj<int> m_shoNumCreateObj;	// 생성된 커넥션 수-메모리 릭 조사용
public:
	static int sGetNumCreateObj() {
		int num = m_shoNumCreateObj.GetSharedObj();
		m_shoNumCreateObj.ReleaseSharedObj();
		return num;
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
	OVERLAPPED m_op;
	BYTE m_Buffer[ 1024 ];
	BOOL m_bFlush;				// m_Buffer의 내용을 큐에다 다 밀어넣었으면 1이 된다.
	XEWinSocketSvr *m_pSocketSvr;	// 걍 내 부모가 누군지 알자는 차원.
	CTimer m_timerHeartBeat;
	CTimer m_timerAsyncDestroy;			// 비동기 파괴 타이머
	xtModeHB m_modeHeartBeat;		// 0: 대기중 1:요청중.
	bool m_bAutoDisconnect = true;	// 응답없으면 자동으로 끊을지..
	_tstring m_strtTag;			// 커넥션 태그(디버깅용)
	/**
	 커넥션이 소유할수 있는 컨텐츠성격의 객체. NULL일수도 있다.
	 XUser는 독자적으로 생성되 돌아다닐수 없고 반드시 커넥션에 종속적이다.
	*/
	XEUser *m_pUser;		
	//
	void Init() {
		m_Socket = 0;
		m_szIP[0] = 0;
		memset( &m_op, 0, sizeof(m_op) );
		memset( m_Buffer, 0, sizeof(m_Buffer) );
		m_bFlush = FALSE;
		m_pSocketSvr = NULL;
		m_cntDoubt = 0;
		m_pUser = NULL;
		m_modeHeartBeat = xHB_READY;
		m_idConnectUserDef = 0;
	}
	void Destroy();
	void CloseSocket( void ) {
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
  GET_READONLY_ACCESSOR( ID, idAccount );
	GET_READONLY_ACCESSOR( LPCTSTR, szIP );
	GET_SET_LOCK_ACCESSOR( XEUser*, pUser );		// retain으로 하지 않아서 내가 '참조'가 아니라 완전히 소유하고 있다는 의미
	GET_ACCESSOR( ID, idConnectUserDef );
	GET_SET_ACCESSOR( bool, bAutoDisconnect );
	//
	BOOL PumpPacket( XPacket *pOutPacket );
	void tWSARecv( void );
	void tRecvData( DWORD readbytes );
	void SendData( const BYTE *pBuffer, int len );
protected:
//	friend void XDBMng2<XEWinConnectionInServer>::Process();
	void Send( XPacket& ar );
public:
	// ** 이함수는 엔진내부용으로 사용하기 위한 용도. 엔진개발자가 아니면 쓰지 말것.
	inline void __SendForXE( XPacket& ar ) {
		Send( ar );
	}
	virtual void Process( void );
	XEUser* CreateAddUser( XDBAccount *pAccount );
	// 접속을 끊어라(이미 하드웨어적으로 끊긴상태면, 내부변수들도 끊긴상태로 바꿔놔라)
	virtual void DoDisconnect() override;
	// virtual
	virtual BOOL ProcessPacket( DWORD idPacket, ID idKey, XPacket& p ) = 0;
	virtual void OnDestroy( void ) {}
	virtual BOOL IsDisconnected( void ) override;
	virtual int Release() override;
  /// 서버측에서 커넥션을 잘라버리기 직전에 호출된다.  // 살아있는 커넥션을 서버가 명시적으로 잘라낼때만 해당된다.
  virtual void OnFireConnectBefore() {}
	void ResetHeartBeatTimer( int secTimeout );
	/// sec초를 기다린 후 소켓을 닫는다.
	void DoAsyncDestroy( float sec ) {
		m_timerAsyncDestroy.Set( sec );
//		CONSOLE("DoAsyncDestroy");
	}
	void ProcessHeartBeat();
	void ProcesssAsyncDisconnect();
	/// 비동기로 죽고 있는 중인지
	bool IsDying() const {
		return m_timerAsyncDestroy.IsOn() != FALSE;
	}
	bool IsDestroy() {
		XLOCK_OBJ;
		return GetbDestroy() || m_timerAsyncDestroy.IsOn();
	}
#ifdef _BOT
  friend class XVirtualPort;
#endif // bot
//friend class XWinSocketSvr; 
};




