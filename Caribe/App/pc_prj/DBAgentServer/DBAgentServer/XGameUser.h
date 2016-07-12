#pragma once
#include "XFramework/server/XEUser.h"
#include "server/XSAccount.h"

class XSAccount;

class XGameUser : public XEUser
{
public:
private:
	XSAccount *m_pAccount;
	BOOL m_bDestroySave;
	CTimer m_timer1Min;		// 1분 타이머	

	void Init() {
	}
	void Destroy();
public:
	XGameUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect );
	virtual ~XGameUser() { Destroy(); } 

	void SuccessLogin( XSAccount *pAccount, BOOL bReconnect = FALSE );
	//
	GET_SET_ACCESSOR( XSAccount*, pAccount );
	GET_SET_ACCESSOR( BOOL, bDestroySave );

	void Send( XPacket& ar );
//	void SendDuplicateConnect();
	void OnDestroy();
	virtual BOOL Save( void );		
	void Process( float dt ) ;
};
