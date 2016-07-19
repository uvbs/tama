#include "stdafx.h"
#include "constGame.h"
#include "XGameCommon.h"
#include "XGameLua.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 글로벌 상수 객체
XGlobalConst *XGC;

void XGlobalConst::sCreateSingleton( XGameCommon *pGameCommon ) 
{
	XGC = new XGlobalConst( pGameCommon, _T("global.xml") );
	if( XGC->IsError() )
	{
		XERROR( "load failed global.xml" );
		return;
	}
}
void XGlobalConst::sDestroySingleton( void ) 
{
	SAFE_DELETE( XGC );
}

void XGlobalConst::LoadConst( LPCTSTR szXml )
{
	XLua *pLua = m_pGameCommon->GetpLua();

	m_SampleInt = GetVal<int>( _T("sample_int") );
	m_SampleFloat = (float)GetVal<double>( _T("sample_float"), _T("group") );
	m_SampleLuaFloat = pLua->GetVar<float>( "SAMPLE01" );
	m_strSampleLua = pLua->GetVar<char*>( "SAMPLE_STR" );


} // XGlobalConst::LoadFromLua

