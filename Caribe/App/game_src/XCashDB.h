#pragma once
#ifdef _CASHE_MEMORY
#include <map>
#include "XPacketGDB.h"

class XDBUAccount;

class XCashDB
{
	static XCashDB *s_pCashDB;

public:
	static XCashDB* sCreateInstance(void);
	static XCashDB* sGet() {
		return s_pCashDB;
	}

	XCashDB( void );
	~XCashDB();

	ID	CreateUserAccount(_tstring strUserName);
	ID	CreateUserAccountUUID(_tstring strUUID);
	
	XDBUAccount*	CreateAccountID(_tstring strID);
	XDBUAccount*	CreateAccountUUID(_tstring strUUID);

	BOOL UpdateUserCasheData(ID idaccount, XDBUAccount* pData );	//DB Data -> Cashe Data 동기화 한다.
	BOOL UpdateUserCasheData(ID idaccount, XArchive& ar);			//DB Data -> Cashe Data 동기화 한다.

	BOOL UpdateUserDBData(ID idaccount, XDBUAccount* pData);		//CashData -> DB 동기화 한다.	
	BOOL UpdateUserDBData(ID idaccount, XArchive& ar);				//CashData -> DB 동기화 한다.

	BOOL SaveUserAccountData(ID idaccount);
	BOOL SaveUserAccountDataAll();

	xtDB LoadUserData(ID idaccount, XDBUAccount *pAccount );

	GET_SET_ACCESSOR(ID, snLastCreateUserID);
	GET_SET_ACCESSOR(int, snIncreaseValue);
private:

	void Init();
	
	//CCriticalSection m_CS;

	std::map<ID, XDBUAccount*>*		m_mapIDAcc;			// IDAccount 검색용 
//	std::map<_tstring, ID>*			m_mapUserName;		// 유저네임
//	std::map<_tstring, ID>*			m_mapAccountName;	// 계정네임
	
	//여기 들어 오는 순간에 이미 정보를 다 가지고 있는 상태 이므로 string 검색을 할 필요는 없다.
//	std::hash_map<_tstring, ID>			m_mapUUID;					// UUID 용
//	std::hash_map<_tstring, ID>			m_mapID;						// 계정 검색용

	ID									m_snLastCreateUserID;
	int									m_snIncreaseValue;
};


inline XCashDB* GetCashDB() {
	return XCashDB::sGet();
}
//extern XCashDB			*DBCASHE_MEM;

#endif