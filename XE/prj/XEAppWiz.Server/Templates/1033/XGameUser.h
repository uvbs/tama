#pragma once
#include "server/XEUser.h"
#include "server/XSAccount.h"

class XSAccount;

class XGameUser : public XEUser
								, public XDelegateLevel		//레벨업객체(FLevel)의 델리게이트
								, public XMemPool<XGameUser>
{
public:
	XGameUser( XEUserMng *pUserMng, XEWinConnectionInServer *pConnect );
	virtual ~XGameUser() { Destroy(); } 

	void SuccessLogin( XSAccount *pAccount, BOOL bReconnect = FALSE );
	//
	GET_SET_ACCESSOR( XSAccount*, pAccount );
	GET_SET_ACCESSOR( bool, bDestroySave );
	void OnDestroy() override;
	BOOL Save( void ) override;
	void Process( float dt ) override;
	void Send( XPacket& ar );
	void AddLog( int logtype, const _tstring& strLog );
	//
	int RecvSample( XPacket& p );
	void SendDuplicateConnect();
	void SuccessLoginAfterSend();
	void SuccessLoginBeforeSend( XSAccount *pAccount, BOOL bReconnect );
private:
	XSAccount *m_pAccount = nullptr;
	bool m_bDestroySave = false;
	CTimer m_timer1Min;		// 1분 타이머
	void Init() {}
	void Destroy();
private:
	inline ID GetidAccount( void );
	inline LPCTSTR GetstrName( void );
	inline int GetLevel( void );
};
