#pragma once
//#include "XFramework/server/XWinConnection.h"
#include "etc/xUtil.h"
// #include "XRefObj.h"
#include "XFramework/XDBAccount.h"

class XEWinConnectionInServer;
class XPacket;
class XEWinSocketSvr;
class XEUserMng;
class XEUser;

// 커넥션이 직접 붙잡고 있는데 유저를 리테인걸어야 할이유가 없어 보이지만 일단 좀 써보고 없애자
// class XEUser : public XRefObj//: protected XDestroyObj
class XEUser
{
private:
public:
	static int s_numUsed;
private:
	XSPWinConnInServerW m_spConnect;		// 다시 부활~이제 this가 죽기전까진 커넥션은 살아있으니 맘껏써도 됨.
	/**
	유저의 정보가 들어있는 DB계정정보의 추상화 객체. 
	계정정보 객체를 필요로 한다면 하위상속클래스는 CreateDBAccount()를 구현해야 한다.
	*/
	XSPDBAccW m_spDBAccount;
	XEUserMng *m_pUserMng;		// this가 속해있는 유저매니저
	CTimer m_timerSave;
	void Init() {
		m_pUserMng = NULL;
	}
	void Destroy();
protected:
	// 유저번호와 계정번호를 일치시키려고 이렇게 한듯 한데. 
	// 이렇게 쓰면 코드재사용도 어렵고 버그발생여지 있음. 
	// 유저가 id를 갖고 있지 않은 시간이 잠시라도 생기기때문.
	// 계정번호로 유저검색이 복잡하더래도 다른방법이 필요.
	// 결국 답은 계정번호만 가진 추상화 계정을 XUser가 가지고 있고 유저아이디는 따로 갖지 않는것.
	// 이방식의 문제라면 반드시 DB의 계정번호를 받아온다음에 유저계정을 생성해야한다는것. 만약
	// 그렇지 못한 경우가 생긴다면 임시아이디를 발급해서 어디 잠시 대기시켜놔야한다.
	// 만약 계정이라는 항목을 사용하지 않는 게임류라면?
//	SET_ACCESSOR( ID, idUser );	
	GET_ACCESSOR( XEUserMng*, pUserMng );
public:
	XEUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect );
	virtual ~XEUser() { Destroy(); }
	//
	GET_SHARED_ACCESSOR( XSPWinConnInServer, spConnect );
	XSPWinConnInServerConst GetspConnectConst() const {
		return std::static_pointer_cast<const XEWinConnectionInServer>( m_spConnect.lock() );
	}
	void ClearConnect() {
		m_spConnect.reset();
//		m_spDBAccount.reset();
	}
	GET_SHARED_ACCESSOR( XSPDBAcc, spDBAccount );
	SET_ACCESSOR( XSPDBAcc, spDBAccount );
	XSPDBAccConst GetspDBAccountConst() const {
		return m_spDBAccount.lock();
	}
	// 계정번호(유저번호)를 돌려준다.
	ID GetidUser() const;
	ID GetidConnect() const;
	ID getid() const {
		return GetidUser();
	}
	GET_ACCESSOR( CTimer&, timerSave );
	// 유저에게 죽음명령을 내리려면 무조건 유저매니저를 통해야 한다.
	// 즉각 접속을 끊으라는 명령을 받음.(이거외엔 유저에게 죽음을 명령하는건 만들지 말것.)
	void DoDisconnect();
	void DoDestroy();

	bool IsDestroy() const;
	virtual void Process( float dt ) {}
	virtual BOOL Save() { return TRUE; }
	virtual void OnLogined() {}
	virtual void OnDestroy() {}
friend class XEUserMng;
};

