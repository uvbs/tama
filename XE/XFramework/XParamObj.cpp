#include "stdafx.h"
#include "XParamObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XParamObj2::Serialize( XArchive& ar ) const
{
	m_params.Serialize( ar );
}

void XParamObj2::DeSerialize( XArchive& ar, int ver )
{
	m_params.DeSerialize( ar, ver );
}
