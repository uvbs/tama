#include "stdafx.h"
#include "XDBAccount.h"
#include "XArchive.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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

