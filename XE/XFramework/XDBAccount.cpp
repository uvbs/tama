#include "stdafx.h"
#include "XFramework/XDBAccount.h"
#include "XArchive.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _SERVER
int XDBAccount::s_numUsed = 0;			// 현재 생성되어있는 객체수
#endif // _SERVER

//////////////////////////////////////////////////////////////////////////
int XDBAccount::Serialize( XArchive& ar )
{
	ar << m_idAccount;
	return 1;
}
int XDBAccount::DeSerialize( XArchive& ar )
{
	ar >> m_idAccount;
	return 1;
}

