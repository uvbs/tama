#include "stdafx.h"
#include "XDownloaderCurl.h"
#include "XSystem.h"

//#define _XDEBUG_CURL

#ifdef _XDEBUG_CURL
#define _XCTRACE(F,...)		XTRACE( F,##__VA_ARGS__ )
#else
#define _XCTRACE(F,...)		(0)			
#endif
#ifndef _XE
#define XLOGXN(F,...)	printf( F,##__VA_ARGS__ )
#define XLOG(F,...)		printf( F,##__VA_ARGS__ )
#endif // not xe

#ifndef NULL
#define NULL 0
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XCurlSingleton XDownloaderCurl::s_Curl;
XFps XDownloaderCurl::s_FpsDown;
int XDownloaderCurl::s_readTotal = 0;

//////////////////////////////////////////////////////////////////////////
// static 
static void *_WorkPThread( void *param )
{
	XBREAK( param == 0 );
	XDownloaderCurl *This = static_cast<XDownloaderCurl*>( param );
	//
	This->WorkThread();
	XTRACE( "exit WorkPThread" );
	return NULL;
}

static size_t CallBackWrite( void* buffer, size_t size, size_t nmemb, void* userData )
{
	XDownloaderCurl *This = (XDownloaderCurl*) userData;
	if( This == NULL )
		return size * nmemb;
	if( This->Getfp() == NULL )
	{
		FILE *fp = NULL;
		_XCTRACE("curl: callbackWrite: %s open", This->GetstrDstFullpath().c_str() );
		fopen_s( &fp, This->GetstrDstFullpath().c_str(), "wb" );
		if( fp == NULL )	{
			XTRACE("curl: failed callbackwrite" );
			return -1;
		}
		This->Setfp( fp );
		_XCTRACE("curl: success callback. fp=0x%08x", (DWORD)fp );
	}
	int sizeInvoke = size * nmemb;
	This->AddSizeFile( sizeInvoke );
	const auto sizeRet = fwrite( buffer, size, nmemb, This->Getfp() );
	XDownloaderCurl::s_FpsDown.AddCnt( sizeInvoke );
	XDownloaderCurl::s_readTotal += sizeInvoke;
	return sizeRet;
}

CURL* XCurlSingleton::s_pCurl = NULL;
std::string XCurlSingleton::s_strServerName;

BOOL XCurlSingleton::SingletonCreate( const char *cServerName )
{
	if( s_pCurl )
		return FALSE;
	s_strServerName = cServerName;
	XLOGX( "curl init......server: %s", C2SZ( cServerName ) );
	curl_global_init( CURL_GLOBAL_ALL ) ;
	s_pCurl = curl_easy_init();
	if( XBREAK( s_pCurl == NULL ) )
	{
		XLOGX( "failed" );
		return FALSE;
	}
	auto 
	codeCurl = curl_easy_setopt( s_pCurl, CURLOPT_URL, cServerName );
	XASSERT( codeCurl == CURLE_OK );
	codeCurl = curl_easy_setopt( s_pCurl , CURLOPT_NOPROGRESS , 1 ) ; // no progress bar:
	XASSERT( codeCurl == CURLE_OK );
	//	curl_easy_setopt( s_pCurl , CURLOPT_WRITEDATA , 0 ) ;
	//	CURLcode rc = curl_easy_perform( s_pCurl ) ;
	//	s_codeError = rc;
	//	if( rc != CURLE_OK )
	//		return false;
	codeCurl = curl_easy_setopt( s_pCurl , CURLOPT_WRITEFUNCTION , CallBackWrite ) ; // what to do with returned data
	XASSERT( codeCurl == CURLE_OK );
	XLOGX( "success" );
	return TRUE;
}

void XCurlSingleton::Destroy( void )
{
	if( s_pCurl )
	{
		curl_easy_cleanup( s_pCurl );
		curl_global_cleanup();
		s_pCurl = NULL;
	}
}
// static end
//////////////////////////////////////////////////////////////////////////

void XDownloaderCurl::Destroy()
{
	XTRACE("curl destroy=%d", GetDestroy() );
	int status;
	XTRACE( "thread detach, hThread=%d", m_hWorkThread );
	pthread_detach( m_hWorkThread );
	XTRACE( "thread join" );
	pthread_join( m_hWorkThread, (void **)&status );
	XTRACE( "thread join finish" );
	XSYSTEM::xSleep(1);
	XTRACE( "thread cancel" );
//	pthread_cancel( m_hWorkThread );	// 안드로이드에서 지원안하네
//	XBREAK( s_Curl.IsLock() == TRUE );
	
	if( m_fp )
		fclose( m_fp );
	m_fp = NULL;
/*	if( m_pCurl )
	{
		curl_easy_cleanup( m_pCurl );
		m_pCurl = NULL;
	}
	curl_global_cleanup();
	*/
	XTRACE( "XDownloaderCurl destroy complete." );
}

XDownloaderCurl::XDownloaderCurl( const char *cServerName, const char* cSubDir )
	: XDownloader( cServerName, cSubDir )
 { 
	 Init(); 
}

bool XDownloaderCurl::Create( void )
{
	// 컬 싱글톤 생성
	s_Curl.SingletonCreate( GetcServerName() );
/*	m_pCurl = curl_easy_init();
	if( XBREAK( m_pCurl == NULL ) )
		return false;
	curl_easy_setopt( m_pCurl, CURLOPT_URL, GetcServerName() );
	curl_easy_setopt( m_pCurl , CURLOPT_NOPROGRESS , 1 ) ; // no progress bar:
	*/
//	curl_easy_setopt( m_pCurl , CURLOPT_WRITEDATA , 0 ) ;
//	CURLcode rc = curl_easy_perform( m_pCurl ) ;
//	m_codeError = rc;
//	if( rc != CURLE_OK )
//		return false;
	
/*	curl_easy_setopt( m_pCurl , CURLOPT_WRITEFUNCTION , CallBackWrite ) ; // what to do with returned data
	XLOGXN( "success" );
	*/
	//
	int idThr = pthread_create( &m_hWorkThread, NULL, _WorkPThread, (void*)this );
	XLOGXN("XDownloadCurl: hThread=%d", m_hWorkThread );
	XBREAK( idThr < 0 );
	return true;
}

void XDownloaderCurl::WorkThread( void )
{
	while(1)
	{
		_XCTRACE("curl thread: ready go");
		while( m_bGo == FALSE )
		{
			if( GetDestroy() )
			{
				_XCTRACE( "thread exit 1" );
				pthread_exit( (void*)0 );
				return;
			}
		}
		int cntFailed = 0;
		// 다운로드 시도
//		m_secTimeout = 0;
		CURLcode rc = CURLE_OK;
		do 
		{
			// 첫번째 방법. nslog를 스레드 세이프 하게 만들어서 로그찍는다.
			// 두번째 방법. 스레드 죽일때 스레드가 완전히 빠져나오길 기다린다.
			// 세번째 방법. 락을 한번 빼본다.
			// 다른컬이 못쓰도록 잠시 잡음.
//			XTRACE("lock, hThread=%d", pthread_self());
			_XCTRACE("go=%d", m_bGo);
			XDownloaderCurl::s_Curl.Lock();
//			XTRACE("enter, hThread=%d", pthread_self());
//			XBREAK( GetDestroy() != 0 );
			//
			curl_easy_setopt( XCurlSingleton::s_pCurl , CURLOPT_URL,  m_strFullURL.c_str() );
			curl_easy_setopt( XCurlSingleton::s_pCurl , CURLOPT_WRITEDATA , this ) ;
			_XCTRACE( "thread curl_easy_perform. pCurl=0x%08x", (DWORD)XCurlSingleton::s_pCurl );
			// 연결이 안되면 여기서 블로킹됨.
			rc = curl_easy_perform( XCurlSingleton::s_pCurl ) ;
			_XCTRACE( "thread curl_easy_perform. fclose fp=0x%08x", (DWORD)m_fp );
			
//			XTRACE("unlock, hThread=%d", pthread_self());
			if( m_fp )
				fclose( m_fp );
			m_fp = NULL;
			XDownloaderCurl::s_Curl.Unlock();
			m_codeError = rc;
			if( rc == CURLE_OK )
				break;
			// 접속이 안되면 잠시 쉬다 계속 돔
			XSYSTEM::xSleep( 2 );
//			m_secTimeout += 2.f;
			++cntFailed;
			_XCTRACE( "could not connect curl. cntFailed=%d", cntFailed );

		} while( rc == CURLE_COULDNT_CONNECT );
//		m_secTimeout = 0;
		_XCTRACE("curl thread: complete release ready");
		m_bComplete = TRUE;
		// m_bComplete가 풀릴때까지 멈춰있는다.
		while( m_bComplete ) {
			if( GetDestroy() ) {
				_XCTRACE( "thread exit 2" );
				pthread_exit( (void*)0 );
				return;
			}
		}
		m_bGo = FALSE;
	} // while(1)
}


void XDownloaderCurl::ClearComplete( void )
{
	m_bComplete = FALSE;
	m_strRequestURL = "";
	m_strDstFullpath = "";
	m_sizeFile = 0;
	m_Invoke = 0;
}
/**
 서버에 cURL경로의 파일을 다운로드 받기 위해 요청한다.
 cURL은 spr/test.spr 과같은 형태로 와야함.
 cDstFullpath는 다운받아서 저장할 풀패스 파일명을 지정한다. 
   파일명을 지정않고 패스만 지정할 경우는 cDstFullpath + cURL로 자동으로 생성된다.
   스레드내부에서 호출되기때문에 콘솔창출력은 삼갈것.
 */
bool XDownloaderCurl::RequestFile( const char* cLocalURL, const char* cDstFullpath )
{
	bool ret = false;
	do 
	{
		// url이 비어있어도 파일받기 에러핸들링을 내고 끝낸다.
		// 파일 다운받기는 언제든지 실패가 일어날수 있는 상황이기때문에 이런류의 에러도 다운실패로 간주하고 에러핸들링을 부른다.
		if( XE::IsEmpty( cLocalURL ) )
			break;
		if( XBREAK( m_bGo == TRUE ) )
			break;
		XDownloader::RequestFile( cLocalURL, cDstFullpath );

		char cURL[ 1024 ];
		strcpy_s( cURL, m_cServerName );
		strcat_s( cURL, "/" );
		strcat_s( cURL, m_strSubDir.c_str() );
		strcat_s( cURL, cLocalURL );
		/* Switch on full protocol/debug output */ 
	//	curl_easy_setopt( m_pCurl, CURLOPT_VERBOSE, 1L );
		// curl등록이랑 this등록을 perform하기직전에 하고 앞뒤로 mutex로 막아주는게 좋을듯.

		m_strFullURL = cURL;
//		curl_easy_setopt( s_pCurl , CURLOPT_URL,  cURL );
//		curl_easy_setopt( s_pCurl , CURLOPT_WRITEDATA , this ) ; 

		// 이미 어디선가 요청한 상태면 실패로 처리
/*		if( XBREAK( s_idRequest != 0 ) )
		{
			ret = false;
			break;
		}
		// 요청아이디를 등록한다.
		s_idRequest = XE::GenerateID(); */
//		XTRACE( "%s file request", C2SZ( cURL ) );
		m_bComplete = FALSE;

		{
			char cDstPath[ 1024 ];
			XE::GetFilePath( cDstPath, m_strDstFullpath.c_str() );
			XSYSTEM::MakeRecursiveDir( cDstPath );
		}
		m_fp = NULL;
		// 다운로드 시작
		XTRACE("XDownloaderCurl::Go! %s", C2SZ(cURL) );
		Go();
		ret = true;

	} while (0);
	if( ret == false )
		XTRACE("XDownloaderCurl::RequestFile failed" );

	return ret;
}

int XDownloaderCurl::Process( float dt )
{
	return 1;
}

void XDownloaderCurl::Go()
{
	m_bGo = TRUE;
	m_timerTimeout.Set( 10.f );
}

