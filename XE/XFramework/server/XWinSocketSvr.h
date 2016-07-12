#pragma once
#include "Network/XWinNetwork.h"
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "win32/XLockCS.h"
#include "XCrypto.h"
#include "XFramework/XDBAccount.h"
#define _USING_WSA

class XEWinConnectionInServer;
class XEUser;
class XEUserMng;
class XDBAccount;
class XEWinSocketSvr;
// namespace XE {
// 	struct xWinConn;
// };
typedef std::shared_ptr<XEWinSocketSvr> XSPWinSocketSvr;
typedef std::shared_ptr<const XEWinSocketSvr> XSPWinSocketSvrConst;

typedef XList4<XSPWinConnInServer> XList_Connection;
typedef std::unordered_map<ID, XSPWinConnInServer> XMap_Connection;

// 윈도우 소켓 서버용
class XEWinSocketSvr : public XCrypto
{
//	static WSADATA s_WSAData;
	struct xProfile {
		_tstring m_str;
		DWORD m_dwPass;
		xProfile( LPCTSTR szStr, XINT64 llPass ) : m_str(szStr), m_dwPass((DWORD)llPass) {}
	};
private:
	int m_sizeMaxQ = 0;			// this에 물린 커넥션들 중에 가장 많이 쌓인 큐크기
	XSPLock m_spLock;
//	enum { MAX_THREAD=5 };
	SOCKET m_Socket;
	HANDLE m_hIOCP; 
	XVector<XE::xThread> m_aryThreadWork;
	XE::xThread m_thAccept;
	unsigned short m_Port;
	int m_maxConnect;		// 최대 동시접속자수 설정(현재 동접자가 아님)
	// this내 여러스레드에서 함께쓰는 객체들은 이안에 있다.
	struct xConnects {
		XSharedObj<XList_Connection> m_shoList;			// accept만 된 커넥션인가?
		XSharedObj<XMap_Connection> m_shoMap;
		XSharedObj<XMap_Connection> m_shoMapByidAcc;	// idAcc-커넥션객체를 맵으로 한것.
	};
	xConnects m_Connected;	// connect()된 커넥션들
	xConnects m_Logined;		// logined된 커넥션들
	int m_Count;
	BOOL m_bLoopAccept;
	BOOL m_bExitAccept;
	XEUserMng *m_pUserMng;
	_tstring m_strName;			// 서버 이름
	int m_numDestroyAdd;		// 삭제된 커넥션 누적숫자.
	int m_numConnected, m_maxConnected;			// 접속된 커넥션 수(UI 디스플레이용). 변수로 따로 둔것은 UI찍을때 매번 락을 걸지 않기 위해서임
	int m_numLogined, m_maxLogined;			// 접속된 커넥션 수(UI 디스플레이용). 변수로 따로 둔것은 UI찍을때 매번 락을 걸지 않기 위해서임
	CTimer m_timerSec;
	int m_msecSleepConn = 1;
	int m_msecSleepRecv = 0;
public:
	XVector<xProfile> m_aryTime;
private:
	int m_cntDeleted = 0;
	int m_cntDestroyed = 0;
protected:
	ID m_idDebug = 0;				// 디버깅용 식별코드
private:
//	bool m_bConnectedFlag = false;		// connect되면 true되었다가 SendConnectComplete()보낸후 다시 false된다.
	//
	void Init() {
		m_hIOCP = 0;
// 		memset( m_hThread, 0, sizeof(m_hThread) );
// 		m_hAcceptThread = 0;
		m_Port = 0;
		m_maxConnect = 0;
		m_Count = 0;
		m_bLoopAccept = TRUE;
		m_bExitAccept = FALSE;
		m_pUserMng = NULL;
		m_numDestroyAdd = 0;
		m_numConnected = m_maxConnected = 0;
		m_numLogined = m_maxLogined = 0;
	}
	void Destroy();
	void CloseAllThread();
// 	int GetnumDestroyList() {
// 		return m_listDestroy.size();
// 	}
	// 자동 Release객체를 생성
// 	XAutoSharedObj<XList_Connection> GetAutoSharedListLogined() {
// 		return XAutoSharedObj<XList_Connection>( m_shoListLogined );
// 	}
// 	XAutoSharedObj<XList_Connection> GetAutoSharedListConnected() {
// 		return XAutoSharedObj<XList_Connection>( m_shoListConnected );
// 	}
// 	XEUser* CreateAddUserMng( XSPWinConnInServer spConnect );
	// this만이 사용하는 커넥션 삭제등록 함수. 혹시나 밖으로 빼지말라고 ForSelf라고까지 붙여둠.
// 	BOOL AddDestroyConnectionForSelf( const XList4<XE::xWinConn>& listConn
// 																	, XSPWinConnInServer spConnect );
	// 커넥션 리스트별로 따로 검색하는 버전.
// 	XE::xWinConn* FindspConnection( const XList4<XE::xWinConn>& plistReady
// 																, const XList4<XE::xWinConn>& plistAdded
// 																, ID idConnect );
// 	XE::xWinConn* _FindpConnInConnected( const XList4<XE::xWinConn>& listConn, ID idConnect );
// 	XE::xWinConn* _FindpConnInLogined( const XList4<XE::xWinConn>& listConn, ID idConnect );
//	XE::xWinConn* _FindpConnInConnected( ID idConnect );
// 	XE::xWinConn* _FindpConnInLogined( ID idConnect );
//   XE::xWinConn* _FindpConnectionInAddedByidAcc( ID idAcc );
//   XE::xWinConn*_FindpConnectionInReadyByidAcc( ID idAcc );
	XSPWinConnInServer _FindspConnInList( const XList4<XSPWinConnInServer>& listConn, ID idConnectFind );
	XSPWinConnInServer _FindspConnInMap( const XMap_Connection& mapConn, ID idConnectFind );
	XSPWinConnInServer _FindspConnInConnectedByidAcc( ID idAcc );
	XSPWinConnInServer _FindspConnInLoginedByidAcc( ID idAcc );
	//
protected:
	// 디버깅용으로만 사용할것
	XSharedObj<XList_Connection>& GetlistConnected() {
		return m_Connected.m_shoList;
	}
	XSharedObj<XList_Connection>& GetlistLogined() {
		return m_Logined.m_shoList;
	}
public:
	XEWinSocketSvr( LPCTSTR szName
// 								, unsigned short port
								, int numReadyConnect
								, int maxConnect
								, BOOL bCryptPacket );
	virtual ~XEWinSocketSvr() { Destroy(); }
	//
	GET_READONLY_ACCESSOR( WORD, Port );
	GET_READONLY_ACCESSOR( HANDLE, hIOCP );
	GET_SET_ACCESSOR_CONST( BOOL, bLoopAccept );
	GET_ACCESSOR_CONST( BOOL, bExitAccept );
	GET_ACCESSOR_CONST( int, maxConnect );
	GET_ACCESSOR_CONST( int, numConnected );
	GET_ACCESSOR_CONST( int, numLogined );
	GET_ACCESSOR_CONST( int, maxConnected );
	GET_ACCESSOR_CONST( int, maxLogined );
	int GetNumConnection();
	GET_ACCESSOR_CONST( XEUserMng*, pUserMng );
	GET_ACCESSOR( XSPLock&, spLock );
	GET_ACCESSOR_CONST( int, sizeMaxQ );
	GET_ACCESSOR_CONST( int, msecSleepConn );
	GET_ACCESSOR_CONST( int, msecSleepRecv );
	inline void SetmsecSleepConn( int msec ) {
		XBREAK( msec < 0 || msec > 1000 || msec == XE::INVALID_INT );
		m_msecSleepConn = msec;
	}
	inline void SetmsecSleepRecv( int msec ) {
		XBREAK( msec < 0 || msec > 1000 || msec == XE::INVALID_INT );
		m_msecSleepRecv = msec;
	}
	void ClearSizeMaxQ() {
		m_sizeMaxQ = 0;
	}
	LPCTSTR GetszName() const {
		return m_strName.c_str();
	}
	int GetClearNumDestroyAdd() {
		int num = m_numDestroyAdd;
		m_numDestroyAdd = 0;
		return num;
	}
	//
	void WorkThread();
	void AcceptThread();
	inline XSPWinConnInServer GetspConnect( ID idConnect ) {
		return FindspConnect( idConnect );
	}
	XSPWinConnInServer FindspConnect( ID idConnect );
	XSPWinConnInServer GetspConnByIdAcc( ID idAcc );
	// pConnect쪽에서 spAccount를 주며 유저객체 생성을 요청함.
//	XSPUserBase CreateAddUser( XSPWinConnInServer spConnect, XSPDBAcc spAccount );
//	XSPUserBase CreateAddToUserMng( XSPWinConnInServer spConnect );
	XSPUserBase CreateAddToUserMng( XSPWinConnInServer spConnect, XSPDBAcc spAccount );
//	void SendConnectComplete( XSPWinConnInServer spConnect );
	//
	void Process( float dt );
	virtual void OnProcess( float dt ) {}
	//
	// 상속받는 서버는 커넥션 되는 오브젝트의 생성을 정의해야 함
	void Create( WORD port, int numWorkThread = 0 );
	virtual void OnCreate() {}
	virtual XEUserMng* CreateUserMng( int maxConnect );
	virtual XSPWinConnInServer tCreateConnectionObj( SOCKET socket, LPCTSTR szIP ) = 0;		
	virtual void OnLoginedFromClient( XSPWinConnInServer spConnect );
	virtual XSPUserBase CreateUser( XSPWinConnInServer spConnect );
	virtual void OnDestroyConnection( XSPWinConnInServer spConnect ) {}
	// 외부에서 쓰지말것. 항상 XUser를 파괴해서 호출되게 할것.
	void _DoDestroyConnection( XSPWinConnInServer spConnect );
	virtual void DrawConnections( _tstring* pOutStr ) {}
	virtual void OnDisconnectConnection( XSPWinConnInServer spConnect ) {}
public:
	virtual XE::xThread CreateWorkThread();
	virtual XE::xThread CreateAcceptThread();
	virtual void OnDestroy();
	//
	template<typename T, typename F>
	void CallFuncToConnect( F func ) {
		std::function<void( T* )> callobj = 
			std::bind( func, std::placeholders::_1 );
		auto plist = &m_Logined.m_shoList.GetSharedObj();
		for( auto spConnect : *plist ) {
			callobj( static_cast<T*>( spConnect.get() ) );
		}
		// Unlock
		m_Logined.m_shoList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1>
	void CallFuncToConnect( F func, T1 p1 ) {
		std::function<void(T*)> callobj 
			= std::bind( func, std::placeholders::_1, p1 );
		auto plist = &m_Logined.m_shoList.GetSharedObj();
		for( auto spConnect : *plist ) {
			callobj( static_cast<T*>( spConnect.get() ) );
		}
		// Unlock
		m_Logined.m_shoList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2>
	void CallFuncToConnect( F func, T1 p1, T2 p2 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2 );
		auto plist = &m_Logined.m_shoList.GetSharedObj();
		for( auto spConnect : *plist ) {
			callobj( static_cast<T*>( spConnect.get() ) );
		}
		// Unlock
		m_Logined.m_shoList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2, typename T3>
	void CallFuncToConnect( F func, T1 p1, T2 p2, T3 p3 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2, p3 );
		auto plist = &m_Logined.m_shoList.GetSharedObj();
		for( auto spConnect : *plist ) {
			callobj( static_cast<T*>( spConnect.get() ) );
		}
		// Unlock
		m_Logined.m_shoList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2, typename T3, typename T4>
	void CallFuncToConnect( F func, T1 p1, T2 p2, T3 p3, T4 p4 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2, p3, p4 );
		auto plist = &m_Logined.m_shoList.GetSharedObj();
		for( auto spConnect : *plist ) {
			callobj( static_cast<T*>( spConnect.get() ) );
		}
		// Unlock
		m_Logined.m_shoList.ReleaseSharedObj();
	}
private:
	void SendConnectComplete( XSPWinConnInServer spConnect );
	void DelUserProcess( XSPUserBase spUser );
	void ReleaseConnectInMap( ID idKey, XMap_Connection& mapConnects );
	void ReleaseConnectInMap( ID idKey, XSharedObj<XMap_Connection>& shoMapConnects );
	void DestroyConnectProcess( XSPWinConnInServer spConnect );
	void ProcessConnectedList();
	void ProcessLoginedList();
	void ProcessDestroyList( XVector<XSPWinConnInServer>& aryDestroy );
friend class XEUserMng;
};

