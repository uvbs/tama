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

	BOOL UpdateUserCasheData(ID idaccount, XDBUAccount* pData );	//DB Data -> Cashe Data ����ȭ �Ѵ�.
	BOOL UpdateUserCasheData(ID idaccount, XArchive& ar);			//DB Data -> Cashe Data ����ȭ �Ѵ�.

	BOOL UpdateUserDBData(ID idaccount, XDBUAccount* pData);		//CashData -> DB ����ȭ �Ѵ�.	
	BOOL UpdateUserDBData(ID idaccount, XArchive& ar);				//CashData -> DB ����ȭ �Ѵ�.

	BOOL SaveUserAccountData(ID idaccount);
	BOOL SaveUserAccountDataAll();

	xtDB LoadUserData(ID idaccount, XDBUAccount *pAccount );

	GET_SET_ACCESSOR(ID, snLastCreateUserID);
	GET_SET_ACCESSOR(int, snIncreaseValue);
private:

	void Init();
	
	//CCriticalSection m_CS;

	std::map<ID, XDBUAccount*>*		m_mapIDAcc;			// IDAccount �˻��� 
//	std::map<_tstring, ID>*			m_mapUserName;		// ��������
//	std::map<_tstring, ID>*			m_mapAccountName;	// ��������
	
	//���� ��� ���� ������ �̹� ������ �� ������ �ִ� ���� �̹Ƿ� string �˻��� �� �ʿ�� ����.
//	std::hash_map<_tstring, ID>			m_mapUUID;					// UUID ��
//	std::hash_map<_tstring, ID>			m_mapID;						// ���� �˻���

	ID									m_snLastCreateUserID;
	int									m_snIncreaseValue;
};


inline XCashDB* GetCashDB() {
	return XCashDB::sGet();
}
//extern XCashDB			*DBCASHE_MEM;

#endif