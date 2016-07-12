#pragma once
#include "XFramework/server/XWinSocketSvr.h"
//#include "XPropMng.h"

class XEWinConnectionInServer;
class XGameUserMng;
class XGameUser;
class XPVPMng;

class XSocketSvrForClient : public XEWinSocketSvr
{
	XGameUserMng *m_pUserMng;
	CTimer m_timerSec;
	void Init() {
		m_pUserMng = NULL;
	}
	void Destroy() {}
public:
	XSocketSvrForClient();
	virtual ~XSocketSvrForClient() { Destroy(); }
	//
	GET_ACCESSOR( XGameUserMng*, pUserMng );

	void ProcessTimer();
	void DoLoadData (  );
	//
	virtual void OnCreate( void );
	XSPWinConnInServer tCreateConnectionObj( SOCKET socket, LPCTSTR szIP ) override;
	XEUserMng* CreateUserMng( int maxConnect ) override;
	void OnLoginedFromClient( XSPWinConnInServer spConnect ) override;
	XSPUserBase CreateUser( XSPWinConnInServer spConnect ) override;
	void OnProcess( float dt ) override;
};

extern XSocketSvrForClient *SOCKET_CLIENT;
