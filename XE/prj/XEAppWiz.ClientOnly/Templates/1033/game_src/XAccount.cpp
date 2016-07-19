#include "stdafx.h"
#include "XAccount.h"
#include "VerPacket.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define VER_SERIALIZE	1

XAccount::XAccount() 
{ 
	Init(); 
#ifdef _CLIENT
	CONSOLE( "create account: %s", XE::GetTimeString() );
#endif
}

XAccount::XAccount( ID idAccont ) 
{ 
	Init(); 
	m_idAccount = idAccont;
}
XAccount::XAccount( ID idAccount, LPCTSTR szID ) 
{ 
	Init(); 
	m_idAccount = idAccount;
	m_strID = szID;
}

void XAccount::Create( void )
{
} 

void XAccount::Destroy( void )
{
//	SAFE_DELETE( m_pWorldObj );
}


int XAccount::Serialize( XArchive& ar )
{
	XDBAccount::Serialize( ar );
	//
	ar << (DWORD) VER_SERIALIZE;
	ar << m_strID.c_str();
	ar << m_strPassword.c_str();
	ar << m_strName.c_str();
	ar << m_strUUID.c_str();
	ar << m_StrSessionKey.c_str();
	ar << m_CurrSN;
//	m_Level.Serialize( ar );

	return 1;
}
int XAccount::DeSerialize( XArchive& ar )
{
	XDBAccount::DeSerialize( ar );
	CONSOLE("Deserialize:idAcc=%d", GetidAccount());
	TCHAR szBuff[1024];
	//
	ar >> m_Ver;
	ar.ReadString( szBuff );	m_strID = szBuff;
	ar.ReadString( szBuff );	m_strPassword = szBuff;
	ar.ReadString( szBuff );	m_strName = szBuff;
	ar.ReadString( szBuff );	m_strUUID = szBuff;
	ar.ReadString( szBuff );	m_StrSessionKey = szBuff;
	ar >> m_CurrSN;
//	m_Level.DeSerialize( ar );

	return 1;
}

