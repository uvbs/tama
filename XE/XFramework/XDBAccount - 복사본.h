#pragma once

#ifdef _XACCOUNT_REF
//#pragma message("------------------------_XACCOUNT_REF")
#endif

// 유저1인의 DB계정 추상화객체(아무래도 서버/클라버전 나눠야 할듯. 서버버전엔 락같은거도 나중에 붙을듯)
#ifdef _XACCOUNT_REF
#include "XRefObj.h"
class XDBAccount : public XRefObj
#else
class XDBAccount
#endif
{
	void Init() {
		m_idAccount = 0;
	}
	void Destroy() {}
protected:
	ID m_idAccount;			// 계정 시리얼 번호
public:
	XDBAccount() : m_idAccount(0) { 
		Init();
		// idAccount를 생성자에 넣지 않는 이유.
		// XUser객체는 커넥션이 된직후 생성되며 커넥션되고 첫번째패킷(보통 로긴정보)이
		// 날아오기까지의 사이시간동안(길수도 있음)엔 계정번호를 알수가 없다.
		// 그러므로 그시간동안의 XUser객체는 정상적인 객체로 처리해선 안될듯 하다.
	}
	virtual ~XDBAccount() { Destroy(); }
	//
	GET_ACCESSOR_CONST( ID, idAccount );
  void _SetidAccount( ID idAcc ) {
    m_idAccount = idAcc;
  }
	//
	virtual int Serialize( XArchive& ar );
	virtual int DeSerialize( XArchive& ar );
#if defined(_DB_SERVER) || defined(_LOGIN_SERVER)
	friend class XDatabase;
	friend class CUserDB;
#endif
};

