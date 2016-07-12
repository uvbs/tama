#pragma once
#include <WinSock2.h>

#define _USING_WSA

// namespace XE {
// 	struct xWinConn;
// };
class XEWinConnectionInServer;
class XEUser;

//typedef std::shared_ptr<XE::xWinConn> XSPWinConn;
//typedef std::shared_ptr<const XE::xWinConn> XSPWinConnConst;
typedef std::shared_ptr<XEWinConnectionInServer> XSPWinConnInServer;
typedef std::shared_ptr<const XEWinConnectionInServer> XSPWinConnInServerConst;
typedef std::shared_ptr<XEUser> XSPUserBase;
typedef std::shared_ptr<const XEUser> XSPUserBaseConst;
typedef std::weak_ptr<XEWinConnectionInServer> XSPWinConnInServerW;
typedef std::weak_ptr<const XEWinConnectionInServer> XSPWinConnInServerConstW;

//////////////////////////////////////////////////////////////////////////
XE_NAMESPACE_START( XE )
//
// struct xWinConn {
// 	XLock m_Lock;			// XSPWinConn객체가 파괴될때 크리티컬섹션도 파괴된다.
// 	XSPWinConnInServer m_spConnect;
// 	xWinConn( XSPWinConnInServer spConnect )
// 		: m_spConnect( spConnect ) {
// 		// 		m_Lock = std::make_shared<XLock>();
// 	}
// 	void Lock( LPCTSTR szFunc ) {
// 		m_Lock.Lock( szFunc );
// 	}
// 	void Unlock() {
// 		m_Lock.Unlock();
// 	}
// 	ID GetidConnect() const;
// };

//
XE_NAMESPACE_END; // XE
//////////////////////////////////////////////////////////////////////////

/****************************************************************
* @brief 서버/클라 윈도우 소켓의 공통 static 모듈. 
* @author xuzhu
* @date	2016/03/14 19:44
*****************************************************************/
class XWinNetwork
{
public:
	static WSADATA s_WSAData;
public:
	~XWinNetwork() {}
	// get/setter
	// public member
	static bool sStartUp();
private:
	// private member
private:
	XWinNetwork() {}			// static 용도로만 사용해아함.
	// private method
}; // class XNetworkInServer

