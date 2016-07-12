#pragma once
#include "XFramework/server/XServerMain.h"
#include "XGameCommon.h"
#include "etc/XTimer2.h"

class XSAccount;
/**
 컨텐츠 데이타의 로딩등을 담당하는 객체
*/
class XGame : public XEContents, public XGameCommon
{
	struct xAccLogin {
		XSPSAcc m_spAcc;
		int m_cntTry = 0;					// 게임서버에 로그인시도 횟수.
		CTimer m_timerLife;		// 일정시간동안 게임서버로 접속을 안하면 계정 날림
	};
	static XGame *s_pGame;
public:
	static XGame* sCreateInstance( void );
	static XGame* sGet() {		return s_pGame;	}
	void Init() {	}
	void Destroy();
	XList4<ID> m_listRequestAccount;		// 로긴서버로부터 계정정보가 오길 기다리는 유저들 리스트
//	XList4<XSPSAcc> m_listAccountFromLogin;	// 로긴서버로부터 받은 계정리스트.
	std::map<ID, xAccLogin> m_mapAccFromLogin;		// 로긴서버로부터 받은 계정리스트.
	std::map<_tstring, xAccLogin> m_mapAccFromLoginBystrID;		// 로긴서버로부터 받은 계정리스트.
	XTimer2 m_timerMain;		// 서버 메인 타이머. 서버가 꺼지면 시간도 같이 멈춘다. DB에 저장되어야 함.
	CTimer m_timerSec;

public:
	XGame();
	virtual ~XGame() { Destroy(); }
	//
	GET_ACCESSOR( XTimer2&, timerMain );
private:
public:

	XGame::xAccLogin* FindLoginAccount( ID idAccount );
	XGame::xAccLogin* FindLoginAccount( const _tstring& strID );
	inline XGame::xAccLogin* FindLoginAccount( LPCTSTR szID ) {
		return FindLoginAccount( _tstring(szID) );
	}
	void UpdateLoginAcc();
	
	void DelLoginAccount( ID idAccount );
	bool AddLoginAccount( ID idAcc, XSPSAcc pAccount );

	virtual void FrameMove( float dt );
	virtual void Create( void );
};

inline XGame* GetGame() {	return XGame::sGet();}



