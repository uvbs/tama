#include "stdafx.h"
#include "XPatch.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

bool XPatch::sLoadFullList( LPCTSTR szFullpath, XList4<XRES_INFO> *pOutList )
{
	CToken token;
	if( !token.LoadAbsolutePath( szFullpath, XE::TXT_EUCKR ) )
		return false;

	while( token.GetToken() )
	{
		if( token.IsFail() )
			return false;
		_tstring strFile = token.m_Token;
		DWORD size = (DWORD)token.GetNumber();
		if( token.IsFail() )
			return false;
		XUINT64 llCheckSum = (XUINT64)token.GetNumber64();
		if( token.IsFail() )
			return false;
		XRES_INFO res;
		res.strFile = strFile;
		res.size = size;
		res.llChecksum = llCheckSum;
		pOutList->Add( res );
	}

	return true;
}