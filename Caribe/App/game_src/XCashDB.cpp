#include "stdafx.h"

#if defined(_DB_SERVER)
#ifdef _CASHE_MEMORY

#include "XCashDB.h"
#include "XAccount.h"
#include "XDBUAccount.h"
//#include "XDBMng.h"

XCashDB *DBCASHE_MEM = NULL;
XCashDB* XCashDB::sCreateInstance( void ) 
{
	s_pCashDB = new XCashDB;
	return s_pCashDB;
}

XCashDB::XCashDB()
{
	m_snLastCreateUserID = 0;
	m_snIncreaseValue = 0;
	Init();
}


XCashDB::~XCashDB()
{
}

void XCashDB::Init()
{
//	SetsnIncreaseValue( 0 ); //초기값
//	SetsnLastCreateUserID( 0 );

	m_mapIDAcc  = new std::map<ID, XDBUAccount*>;
	m_mapIDAcc->clear();
}

ID XCashDB::CreateUserAccount(_tstring strUserName)
{	
	m_snLastCreateUserID += m_snIncreaseValue;
	
	XDBUAccount* pAccount = new XDBUAccount(m_snLastCreateUserID);
	pAccount->m_strName = strUserName;

	// EnterCriticalSection(m_CS);
	std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pAccount));	
	// LeaveCriticalSection(m_CS);

	return p.second ? m_snLastCreateUserID : 0;
}

ID XCashDB::CreateUserAccountUUID(_tstring strUUID)
{
	m_snLastCreateUserID  += m_snIncreaseValue;
	
	XDBUAccount* pAccount = new XDBUAccount(m_snLastCreateUserID);
	pAccount->m_strUUID = szUUID;

	// EnterCriticalSection(m_CS);
	std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pAccount));
	// LeaveCriticalSection(m_CS);

	return p.second ? m_snLastCreateUserID : 0;
}

BOOL XCashDB::UpdateUserCasheData(ID idaccount, XDBUAccount* pData)
{
	// EnterCriticalSection(m_CS);
	XDBUAccount* pOldData;
	std::map<ID, XDBUAccount*>::iterator itor;
	itor = m_mapIDAcc->find(idaccount);

	XArchive ar;
	pData->Serialize(ar);

	if (itor == m_mapIDAcc->end()) //없는 유저 이므로 Insert해준다.
	{
		pOldData = new XDBUAccount;
		pOldData->DeSerialize(ar);

		if (pData->GetidAccount() == 0) //문제 있는 코드..
		{
			m_snLastCreateUserID += m_snIncreaseValue;
			pOldData->m_idAccount = m_snLastCreateUserID;
		}	

		std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pOldData));
		// LeaveCriticalSection(m_CS);
		return p.second ? TRUE : FALSE;
	}
	else
	{
		//외부 Data -> Cashe Data 동기화 한다.
		pOldData = itor->second;				
		pOldData->DeSerialize(ar);
	}
	// LeaveCriticalSection(m_CS);

	return TRUE;
}

BOOL XCashDB::UpdateUserCasheData(ID idaccount, XArchive& ar )
{
	// EnterCriticalSection(m_CS);
	XDBUAccount* pOldData;
	std::map<ID, XDBUAccount*>::iterator itor;
	itor = m_mapIDAcc->find(idaccount);
	
	if (itor == m_mapIDAcc->end()) //없는 유저 이므로 Insert해준다.
	{
		pOldData = new XDBUAccount;
		pOldData->DeSerialize(ar);

		if (pOldData->GetidAccount() == 0) //문제 있는 코드..
		{
			m_snLastCreateUserID += m_snIncreaseValue;
			pOldData->m_idAccount = m_snLastCreateUserID;
		}

		std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pOldData));

		// LeaveCriticalSection(m_CS);
		return p.second ? TRUE : FALSE;
	}
	else
	{
		//외부 Data -> Cashe Data 동기화 한다.
		pOldData = itor->second;
		pOldData->DeSerialize(ar);
	}

	// LeaveCriticalSection(m_CS);
	return TRUE;
}

BOOL XCashDB::UpdateUserDBData(ID idaccount, XDBUAccount* pData)
{
	// EnterCriticalSection(m_CS);

	XDBUAccount* pOldData;
	std::map<ID, XDBUAccount*>::iterator itor;
	itor = m_mapIDAcc->find(idaccount);

	XArchive ar;
	pData->Serialize(ar);

	if (itor == m_mapIDAcc->end()) //Cashe에 없는 유저 이므로 Insert해준다.
	{
		pOldData = new XDBUAccount;
		pOldData->DeSerialize(ar);

		if (pData->GetidAccount() == 0) //문제 있는 코드..
		{
			m_snLastCreateUserID += m_snIncreaseValue;
			pOldData->m_idAccount = m_snLastCreateUserID;
		}
		std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pOldData));

		if (!p.second)
		{
			// LeaveCriticalSection(m_CS);
			return FALSE;
		}

	}
	else
	{
		pOldData = itor->second;
	}
	// LeaveCriticalSection(m_CS);
	return TRUE;
}

BOOL XCashDB::UpdateUserDBData(ID idaccount, XArchive& ar)
{
	// EnterCriticalSection(m_CS);

	XDBUAccount* pOldData;
	std::map<ID, XDBUAccount*>::iterator itor;
	itor = m_mapIDAcc->find(idaccount);
	
	if (itor == m_mapIDAcc->end()) //Cashe에 없는 유저 이므로 Insert해준다.
	{
		pOldData = new XDBUAccount;
		pOldData->DeSerialize(ar);

		if (pOldData->GetidAccount() == 0) //문제 있는 코드..
		{
			m_snLastCreateUserID += m_snIncreaseValue;
			pOldData->m_idAccount = m_snLastCreateUserID;
		}
		std::pair< std::map<ID, XDBUAccount*>::iterator, bool > p = m_mapIDAcc->insert(std::map<ID, XDBUAccount*>::value_type(m_snLastCreateUserID, pOldData));

		if (!p.second)
		{
			//	LeaveCriticalSection(m_CS);
			return FALSE;
		}

	}
	else
	{
		pOldData = itor->second;
	}
	//	LeaveCriticalSection(m_CS);
	return TRUE;
}

BOOL XCashDB::SaveUserAccountData(ID idaccount)
{
	return TRUE;
}
BOOL XCashDB::SaveUserAccountDataAll()
{
	return TRUE;
}

xtDB XCashDB::LoadUserData(ID idaccount, XDBUAccount *pAccount)
{
//	EnterCriticalSection(m_CS);


	XDBUAccount* pOldData;
	std::map<ID, XDBUAccount*>::iterator itor;
	itor = m_mapIDAcc->find(idaccount);

	if (itor == m_mapIDAcc->end()) //Cashe에 없는 유저 이므로 Insert해준다.
	{
		return xDB_NOT_FOUND;
	}
	else
	{
		pOldData = itor->second;
		XArchive ar( 0x10000 ) ;
		pOldData->Serialize(ar);
		pAccount->DeSerialize(ar);
	}
//	LeaveCriticalSection(m_CS);
	return xDB_OK;
}
#endif
#endif