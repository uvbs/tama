#pragma once
#include "XFramework/server/XWinSocketSvr.h"
//#include "XPropMng.h"

class XEWinConnectionInServer;
class XGameUserMng;
class XGameUser;
// class XPVPMng;

class XSocketForGameSvr : public XEWinSocketSvr
{
public:
	static std::shared_ptr<XSocketForGameSvr>& sGet();
	static void sDestroyInstance();
private:
	static std::shared_ptr<XSocketForGameSvr> s_spInstance;
	XGameUserMng *m_pUserMng;
	void Init() {
		m_pUserMng = NULL;
	}
	void Destroy() {}
public:
	XSocketForGameSvr();
	~XSocketForGameSvr() { Destroy(); }
	//
	GET_ACCESSOR( XGameUserMng*, pUserMng );

	void ProcessTimer();
	void DoLoadData (  );
	//
	void OnCreate() override;
	XSPWinConnInServer tCreateConnectionObj( SOCKET socket, LPCTSTR szIP ) override;
	XEUserMng* CreateUserMng( int maxConnect ) override;
	void OnLoginedFromClient( XSPWinConnInServer spConnect ) override;
	XSPUserBase CreateUser( XSPWinConnInServer spConnect ) override;
	void DrawConnections( _tstring* pOutStr ) override;
};

//extern XSocketForGameSvr *SOCKET_GAMESVR;
#define SOCKET_GAMESVR		XSocketForGameSvr::sGet()
