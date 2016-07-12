#include "StdAfx.h"
#include "XFramework/XTextTableUTF8.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XTextTableUTF8::Destroy() 
{
	map<ID, const char*>::iterator itor;
	for( itor = m_mapText.begin(); itor != m_mapText.end(); itor++ )
	{
		const char *pText = itor->second;
		SAFE_DELETE_ARRAY( pText );
	}
}

const char* XTextTableUTF8::Find( ID idText )
{
	map<ID, const char*>::iterator itor;
	itor = m_mapText.find( idText );
	if( itor == m_mapText.end() )
		return NULL;
	const char* szText = (*itor).second;
	XBREAK( szText == NULL );
	return szText;
}

BOOL XTextTableUTF8::Load( LPCTSTR szRes )
{
	CToken token;
	if( token.LoadFile( szRes, XE::TXT_UTF16 ) == xFAIL )
	{
#if defined(WIN32)|| defined(_VER_IOS)
		_tstring strPath = XE::_GetPathPackageRoot();
#else
		_tstring strPath;
#endif
		strPath += szRes;
		XLOG( "\"%s\" not found.", strPath.c_str() );
		return FALSE;
	}
	BOOL result = TRUE;
    ID idLast = 0;
	_tstring utf16;
	while(1)
	{
		DWORD id = (DWORD)token.GetNumber();		// id번호
		if( id == TOKEN_ERROR )
		{
			XALERT( "%s:text id %d read after error", szRes, idLast );
			result = FALSE;
			break;
		}
		if( id == TOKEN_EOF )
			break;
		if( Find( id ) )
		{
			XALERT( "%s:text id %d: duplication id", szRes, id );
		}
		token.GetToken();		// text
#ifdef WIN32
		utf16 = token.m_Token;
		const char *utf8 = CreateConvertUTF16toUTF8( utf16.c_str() );
		m_mapText[ id ] = utf8;			// 맵구조에 id를 키로 하여 텍스트 추가
#else
		int len = strlen( token.m_Token );
		if( len > 0 )
		{
			char *utf8 = new char[ len + 1 ];
			strcpy_s( utf8, len+1, token.m_Token );
			m_mapText[ id ] = utf8;			// 맵구조에 id를 키로 하여 텍스트 추가
		}
#endif
        idLast = id;
		if( id > m_maxID )
			m_maxID = id;
	}
    XLOGXN("text loading success.....%s. lastID:%d", szRes, idLast );
	return result;
}
