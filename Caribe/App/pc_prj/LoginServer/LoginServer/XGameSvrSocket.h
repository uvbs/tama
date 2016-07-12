#pragma once
#include "XFramework/server/XWinSocketSvr.h"
#include "XGameSvrMng.h"

class XEWinConnectionInServer;
//class XEUserMng;
//class XGameUserMng;
class XGameUser;
class XGameSvrConnection;

class XSocketForGameSvr : public XEWinSocketSvr
{
	//XSPLock m_spLock;
	XList4<XSPGameSvrConnect> m_listGameSvr;
	void Destroy() {}
public:
	XSocketForGameSvr();
	virtual ~XSocketForGameSvr() { Destroy(); }
	//
	GET_ACCESSOR( const XList4<XSPGameSvrConnect>&, listGameSvr );
	//GET_ACCESSOR( XSPLock&, spLock );
//	void Create( void );
	//XGameSvrMng* CreateSvrMng( int maxConnect );
	XSPGameSvrConnect GetGameSvrConnection( WORD widGSvr );
	inline XSPGameSvrConnect GetGameSvrConnection( ID idGSvr ) {
		XBREAK( idGSvr > 0xffff );
		return GetGameSvrConnection( (WORD)idGSvr );
	}
	XSPGameSvrConnect GetFreeConnection( void );
	//
	XSPWinConnInServer tCreateConnectionObj( SOCKET socket, LPCTSTR szIP ) override;
//	XEUserMng* CreateUserMng( int maxConnect ) override;
	void OnLoginedFromClient( XSPWinConnInServer spConnect ) override;
	void OnDestroyConnection( XSPWinConnInServer spConnect ) override;
	void DrawConnections( _tstring* pOutStr ) override;
	bool IsExist( WORD widGSvr ) const;
	void OnLoginedGameSvr( XSPGameSvrConnect spConnGameSvr, WORD widSvr, const std::string& strcIPExternal, WORD portExternal );
};


