#pragma once
#include "server/XWinSocketSvr.h"
//#include "XPropMng.h"

class XEWinConnectionInServer;
class XGameUserMng;
class XGameUser;
class XPVPMng;

class XSocketForClient : public XEWinSocketSvr
{
	XGameUserMng *m_pUserMng;

	void Init() {
		m_pUserMng = NULL;
	}
	void Destroy() {}
public:
	XSocketForClient( unsigned short port );
	virtual ~XSocketForClient() { Destroy(); }
	//
	GET_ACCESSOR( XGameUserMng*, pUserMng );

	void ProcessTimer();
	void DoLoadData (  );
	//
	virtual void OnCreate( void );
	virtual XEWinConnectionInServer* tCreateConnectionObj( SOCKET socket, LPCTSTR szIP );
	virtual XEUserMng* CreateUserMng( int maxConnect );
	virtual void OnConnectFromClient( XEWinConnectionInServer *pConnect );
	virtual XEUser* CreateUser( XEWinConnectionInServer *pConnect );
};

extern XSocketForClient *SOCKET_CLIENT;
