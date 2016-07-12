#include "stdafx.h"
#include "XDownloader.h"
#include "xeDef.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
void XDownloader::Destroy()
{
}

XDownloader::XDownloader( const char *cServerName, const char* cSubDir )
 { 
	 Init(); 
	 //
#ifdef _XE
	 XBREAK( XE::IsEmpty( cServerName ) == TRUE );
	 if( m_cServerName )
		 strcpy_s( m_cServerName, cServerName );
#else
	 assert( cServerName != NULL );
	 assert( cServerName[0] != 0 );
	 if( m_cIP )
		 strcpy( m_cServerName, cServerName );
#endif
	 m_strSubDir = cSubDir;
}

bool XDownloader::Create( void )
{

	return true;
}

bool XDownloader::RequestFile( const char* _cURL, const char* _cDstFullpath ) 
{
	char cURL[256];
	char cDstFullpath[1024];
	strcpy_s( cURL, _cURL );
	strcpy_s( cDstFullpath, _cDstFullpath );
	XUNIX_PATH( cURL );
	XPLATFORM_PATH( cDstFullpath );
	m_strRequestURL = cURL;
	m_strDstFullpath = cDstFullpath;
	char cFilename[256];
	XE::GetFileName( cFilename, m_strDstFullpath.c_str() );
	// cDstFullpath에 패스만 있다면 cURL을 파일명으로 붙여준다.
	if( XE::IsEmpty( cFilename ) ) {
		char __cURL[256];
		strcpy_s( __cURL, _cURL );
		XPLATFORM_PATH( __cURL );
		m_strDstFullpath += __cURL;	// 패스에 파일명까지 붙여준다.
	}
	return true;
}
