#include "stdafx.h"
#include "XHSLMap.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<XHSLMap> XHSLMap::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XHSLMap>& XHSLMap::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XHSLMap>( new XHSLMap );
	return s_spInstance;
}
void XHSLMap::sDestroyInstance() {
	s_spInstance.reset();
}

//////////////////////////////////////////////////////////////////////////
bool XHSLMap::Load( LPCTSTR szFile )
{
	CToken token;
	if( token.LoadFile( XE::MakePath( DIR_PROP, szFile ), XE::TXT_UTF16 ) == xFAIL )
		return false;
	while( 1 ) {
		token.GetToken();
		if( token.IsEof() )
			break;
		const _tstring strKey = token.m_Token;
		token.GetToken();
		if( XBREAK( token.IsEof() ) )
			break;
		XE::xHSLFile hslFile;
		hslFile.m_strFile = token.m_Token;
		hslFile.m_HSL.m_vHSL.x = token.GetNumberF();
		hslFile.m_HSL.m_vHSL.y = token.GetNumberF();
		hslFile.m_HSL.m_vHSL.z = token.GetNumberF();
		hslFile.m_HSL.m_vRange1.v1 = token.GetNumberF();
		hslFile.m_HSL.m_vRange1.v2 = token.GetNumberF();
		hslFile.m_HSL.m_vRange2.v1 = token.GetNumberF();
		hslFile.m_HSL.m_vRange2.v2 = token.GetNumberF();
		m_mapTable[strKey] = hslFile;
		// 		CONSOLE("load hslmap: key=%s file=%s, hsl(%.0f,%.0f,%.0f), range(%.0f,%.0f,%.0f,%.0f", strKey.c_str() );
		CONSOLE( "load hslmap: key=%s", strKey.c_str() );
	}
	return true;
}

const XE::xHSLFile* XHSLMap::GetInfo( const _tstring& strKey ) const
{
	auto itor = m_mapTable.find( strKey );
	if( itor != m_mapTable.end() ) {
		return &( itor->second );
	}
	return nullptr;
}

