#include "StdAfx.h"
#include "XFramework/XSoundTable.h"
#include "etc/Token.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSoundTable *SOUND_TBL = NULL;

void XSoundTable::Destroy()
{
}

const char* XSoundTable::GetFileFromID( ID idSound ) const 
{
	if( idSound == 0 )
		return "";
	auto itor = m_mapFile.find( idSound );
	if( itor == m_mapFile.end() )
		return "";
	XBREAK( ( *itor ).second.empty() == true );
	return ( *itor ).second.c_str();
}

const char* XSoundTable::Find( ID idSound ) const
{
	auto itor = m_mapFile.find( idSound );
	if( itor == m_mapFile.end() )
		return NULL;
	std::string str = ( *itor ).second;
	XBREAK( str.empty() == true );
	return str.c_str();
}

ID XSoundTable::Find( const std::string& strKey ) const
{
	auto itor = m_mapByKey.find( strKey );
	if( itor != m_mapByKey.end() ) {
		return itor->second;
	}
	return 0;
}

BOOL XSoundTable::Load( LPCTSTR szFile )
{
	CToken token;
	TCHAR szRes[ 1024 ];
	_tcscpy_s( szRes, XE::MakePath( DIR_PROP, szFile ) );
	if( token.LoadFile( szRes, XE::TXT_UTF16 ) == xFAIL )
	{
#if defined(WIN32)|| defined(_VER_IOS)
		_tstring strPath = XE::_GetPathPackageRoot();
#else
		_tstring strPath;
#endif
		strPath += szRes;
		XERROR( "\"%s\" not found.", strPath.c_str() );
		return FALSE;
	}
	BOOL result = TRUE;
	ID idLast = 0;
	while( 1 ) {
		DWORD id = (DWORD)token.GetNumber();		// id번호
		if( id == TOKEN_ERROR ) {
			XALERT( "sound id %d read after error", idLast );
			result = FALSE;
			break;
		}
		if( id == TOKEN_EOF )
			break;
		if( Find( id ) ) {
			XALERT( "sound id %d: duplication id", id );
		}
		TCHAR szBuff[ 256 ];
		_tcscpy_s( szBuff, token.GetToken() );
		std::string strFile = SZ2C( szBuff );		// filename
//		// 파일명이 별도로 붙지않으면 wav가 디폴트로 붙는다.
// 		if( XE::IsEmpty( XE::GetFileExt( szBuff ) ) )
// #ifdef WIN32
// 			strFile += ".wav";
// #else
// 			strFile += ".ogg";
// #endif // WIN32
		// 파일명엔 확장자를 붙이지 않는다.
		m_mapFile[ id ] = strFile;			// 맵구조에 id를 키로 하여 텍스트 추가
		m_mapByKey[ strFile ] = id;
		idLast = id;
		if( id > m_maxID )
			m_maxID = id;
	}
	XLOGXN( "sound loading success.....%s. lastID:%d", szRes, idLast );
	return result;
}

void XSoundTable::Reload()
{
	m_mapFile.clear();
	m_mapByKey.clear();
	m_maxID = 0;
}

