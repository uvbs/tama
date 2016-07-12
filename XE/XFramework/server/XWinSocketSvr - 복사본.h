#pragma once
#include "XList.h"
#include "etc/ContainerSTL.h"
#include "win32/XLockCS.h"
#ifdef _XCRYPT_PACKET
#include "XCrypto.h"
#endif
#define _USING_WSA

class XEWinConnectionInServer;
class XEUser;
class XEUserMng;
class XDBAccount;

// 윈도우 소켓 서버용
class XEWinSocketSvr : public XCrypto
{
	static WSADATA s_WSAData;
private:
	enum { MAX_THREAD=5 };
	SOCKET m_Socket;
	HANDLE m_hIOCP; 
	XVector<XE::xThread> m_aryThreadWork;
	XE::xThread m_thAccept;
	unsigned short m_Port;
	int m_maxConnect;		// 최대 동시접속자수 설정
	// this내 여러스레드에서 함께쓰는 객체들은 이안에 있다.
	XSharedObj<XList4<XE::xWinConn>> m_shoConnectionList;
	XSharedObj<XList4<XE::xWinConn>> m_shoConnectReady;
//	XList4<XEWinConnectionInServer> m_listDestroy;
	XSharedObj<std::map<ID, XE::xWinConn>> m_shoMapConnection;
	int m_Count;
	BOOL m_bLoopAccept;
	BOOL m_bExitAccept;
	XEUserMng *m_pUserMng;
	_tstring m_strName;			// 서버 이름
	int m_numDestroyAdd;		// 삭제된 커넥션 누적숫자.
	int m_numConnect;			// 접속된 커넥션 수(UI 디스플레이용). 변수로 따로 둔것은 UI찍을때 매번 락을 걸지 않기 위해서임
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
		m_numConnect = 0;
	}
	void Destroy();
	void CloseAllThread();
// 	int GetnumDestroyList() {
// 		return m_listDestroy.size();
// 	}
	XEUser* CreateAddUserMng( XEWinConnectionInServer *pConnect );
	// this만이 사용하는 커넥션 삭제등록 함수. 혹시나 밖으로 빼지말라고 ForSelf라고까지 붙여둠.
	BOOL AddDestroyConnectionForSelf( const XList4<XE::xWinConn>& listConn
																	, XSPWinConnInServer spConnect );
	// 커넥션 리스트별로 따로 검색하는 버전.
	XE::xWinConn* FindConnection( const XList4<XE::xWinConn>& plistReady
															, const XList4<XE::xWinConn>& plistAdded
															, ID idConnect );
	XE::xWinConn* _FindConnectionInReady( const XList4<XE::xWinConn>& listConn, ID idConnect );
	XE::xWinConn* _FindConnectionInAdded( const XList4<XE::xWinConn>& listConn, ID idConnect );
	XE::xWinConn* _FindConnectionInReady( ID idConnect );
	XE::xWinConn* _FindConnectionInAdded( ID idConnect );
  XE::xWinConn* _FindConnectionInAddedByidAcc( ID idAcc );
  XE::xWinConn*_FindConnectionInReadyByidAcc( ID idAcc );
	XEUser* CreateAddUserMng( XSPWinConnInServer spConnect, XDBAccount *pAccount );
protected:

public:
	XEWinSocketSvr( LPCTSTR szName
								, unsigned short port
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
	GET_ACCESSOR_CONST( int, numConnect );
	int GetNumConnection() const { 
		XAutoSharedObj<XList4<XE::xWinConn>> autoObj( m_shoConnectionList );
		return autoObj.Get().size();
//  		auto plist = &m_shoConnectionList.GetSharedObj();
//  		int num = plist->size();
//  		m_shoConnectionList.ReleaseSharedObj();
//		return num;
	}
	GET_ACCESSOR_CONST( XEUserMng*, pUserMng );
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
	inline XSPWinConnInServerConst GetspConnection( ID idConnect ) {
		return FindspConnection( idConnect );
	}
	XSPWinConnInServerConst FindspConnection( ID idConnect );
  XSPWinConnInServerConst GetspConnectionByIdAcc( ID idAcc );
	// pConnect쪽에서 pAccount를 주며 유저객체 생성을 요청함.
	XEUser* CreateAddUser( XSPWinConnInServer spConnect, XDBAccount *pAccount );
	void SendConnectComplete( XSPWinConnInServer spConnect );
	//
	virtual void Process( float dt );
	//
	// 상속받는 서버는 커넥션 되는 오브젝트의 생성을 정의해야 함
	void Create();
	virtual void OnCreate() {}
	virtual XEUserMng* CreateUserMng( int maxConnect );
	virtual XE::xWinConn tCreateConnectionObj( SOCKET socket, LPCTSTR szIP ) = 0;		
	virtual void OnConnectFromClient( XE::xWinConn& spConnect );
	virtual XEUser* CreateUser( XE::xWinConn& conn );
	virtual void OnDestroyConnection( XEWinConnectionInServer *pConnect ) {}
	// 외부에서 쓰지말것. 항상 XUser를 파괴해서 호출되게 할것.
	void _DoDestroyConnection( XEWinConnectionInServer *pConnect );
	virtual void DrawConnections( _tstring& str ) {}
friend class XEUserMng;
public:
	virtual XE::xThread CreateWorkThread();
	virtual XE::xThread CreateAcceptThread();
	template<typename T, typename F>
	void CallFuncToConnect( F func ) {
		std::function<void( T* )> callobj = 
			std::bind( func, std::placeholders::_1 );
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			callobj( static_cast<T*>( pConnect ) );
		} END_LOOP;
		// Unlock
		m_shoConnectionList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1>
	void CallFuncToConnect( F func, T1 p1 ) {
		std::function<void(T*)> callobj 
			= std::bind( func, std::placeholders::_1, p1 );
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			callobj( static_cast<T*>(pConnect) );
		} END_LOOP;
		// Unlock
		m_shoConnectionList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2>
	void CallFuncToConnect( F func, T1 p1, T2 p2 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2 );
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			callobj( static_cast<T*>( pConnect ) );
		} END_LOOP;
		// Unlock
		m_shoConnectionList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2, typename T3>
	void CallFuncToConnect( F func, T1 p1, T2 p2, T3 p3 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2, p3 );
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			callobj( static_cast<T*>( pConnect ) );
		} END_LOOP;
		// Unlock
		m_shoConnectionList.ReleaseSharedObj();
	}
	template<typename T, typename F, typename T1, typename T2, typename T3, typename T4>
	void CallFuncToConnect( F func, T1 p1, T2 p2, T3 p3, T4 p4 ) {
		std::function<void( T* )> callobj
			= std::bind( func, std::placeholders::_1, p1, p2, p3, p4 );
		auto plistConnection = &m_shoConnectionList.GetSharedObj();
		XLIST2_LOOP( *plistConnection, XEWinConnectionInServer*, pConnect ) {
			callobj( static_cast<T*>( pConnect ) );
		} END_LOOP;
		// Unlock
		m_shoConnectionList.ReleaseSharedObj();
	}

};

