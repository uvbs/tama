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
	virtual XSPWinConnInServer tCreateConnectionObj( SOCKET socket, LPCTSTR szIP );
	virtual XEUserMng* CreateUserMng( int maxConnect );
	virtual void OnLoginedFromClient( XSPWinConnInServer spConnect );
	virtual XSPUserBase CreateUser( XSPWinConnInServer spConnect );
 };

extern XSocketSvrForClient *SOCKET_CLIENT;
