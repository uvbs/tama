#pragma once
#include "server/XServerMain.h"
#include "XGameCommon.h"

class XSAccount;
/**
 컨텐츠 데이타의 로딩등을 담당하는 객체
*/
class XGame : public XEContents, public XGameCommon
{
public:
	static XGame* sCreateInstance();
	static XGame* sGet() {
		XBREAK( s_pGame == nullptr );
		return s_pGame;
	}
public:
	XGame();
	virtual ~XGame() { Destroy(); }
	//
	GET_ACCESSOR( XTimer2&, timerMain );
	//
	void FrameMove( float dt ) override;
	void Create() override;
	//
	XSAccount* FindLoginAccount( ID idAccount );
	XSAccount* FindLoginAccount( LPCTSTR szID );
	void DelLoginAccount( ID idAccount );
	BOOL AddLoginAccount( XSAccount *pAccount );
private:
	void Destroy();
private:
	static XGame *s_pGame;
private:
	XList<ID> m_listRequestAccount;		// 로긴서버로부터 계정정보가 오길 기다리는 유저들 리스트
	XList2<XSAccount> m_listAccountFromLogin;	// 로긴서버로부터 받은 계정리스트.
	XTimer2 m_timerMain;		// 서버 메인 타이머. 서버가 꺼지면 시간도 같이 멈춘다. DB에 저장되어야 함.
};

extern XGame		*GAME;

