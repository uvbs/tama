#pragma once
#ifdef WIN32
#include "pthread_win32/include/pthread.h"
#else
#include <pthread.h>
#endif
#include "XDownloader.h"
#ifdef WIN32
#include "curl/include/curl/CURL.h"
#endif
#ifdef _VER_IOS
#include "curl/ios/curl/CURL.h"
#endif
#ifdef _VER_ANDROID
#include "curl/android/prebuilt/libcurl/include/curl/CURL.h"
#endif
#include "XLockPThreadMutex.h"

// 싱글톤객체
class XCurlSingleton : public XLockPthreadMutex
{
public:
	// 일단은 public으로 해놓음. 나중에 curl관련함수들 안으로 다 들여와서 private으로 바꿀예정
	static std::string s_strServerName;
	static CURL* s_pCurl;
private:
	void Init() {}
	static void Destroy();
public:
	XCurlSingleton() { 
		XBREAK( s_pCurl != NULL );
	}
	virtual ~XCurlSingleton() { Destroy(); }
	static BOOL SingletonCreate( const char *cServerName );
};
 

// curl을 사용하는 패치클라이언트 객체
class XDownloaderCurl : public XDownloader, public XLock
{
public:
	static XCurlSingleton s_Curl;
	static XFps s_FpsDown;
	static int s_readTotal;
	static void sClearReadTotal() {
		s_readTotal = 0;
	}
private:
	FILE *m_fp;			// 다운받는중인 파일포인터
	DWORD m_sizeFile;		// 다운받은 파일 사이즈
	int m_Invoke;		// 몇번에 걸쳐서 write를 했는가
	pthread_t m_hWorkThread;
	volatile BOOL m_bGo;
	volatile BOOL m_bComplete;
	CURLcode m_codeError;
//	float m_secTimeout;
	CTimer m_timerTimeout;
	std::string m_strFullURL;
	void Init() {
///		m_pCurl = 0;
		m_fp = NULL;
		m_sizeFile = 0;
		m_Invoke = 0;
		m_bGo = FALSE;
		m_bComplete = FALSE;
		m_codeError = CURLE_OK;
//		m_secTimeout = 0;
	}
	void Destroy();
public:
	XDownloaderCurl( const char *cServerName, const char* cSubDir );
	virtual ~XDownloaderCurl() { Destroy(); }
	//
	GET_SET_ACCESSOR( FILE*, fp );
//	GET_SET_ACCESSOR( int, sizeFile );
	GET_ACCESSOR( BOOL, bGo );
	GET_ACCESSOR( BOOL, bComplete );
//	GET_ACCESSOR( float, secTimeout );
	GET_ACCESSOR( CTimer&, timerTimeout );
	DWORD GetsizeFile( void ) {
		XLockObj lockObj( this );
		return m_sizeFile;
	}
	int AddSizeFile( int add ) {
		m_sizeFile += add;
		++m_Invoke;
		return m_sizeFile;
	}
	bool IsDownloading() {
		return m_Invoke > 0;
	}
	void Go( void );
	XDownloader::xtError GetErrorCode( void ) {
		switch( m_codeError ) {
		case CURLE_OK:				return XDownloader::xOK;
		case CURLE_COULDNT_CONNECT:	return XDownloader::xERR_COULD_NOT_CONNECT;
		default:
			return XDownloader::xERR_UNKNOWN;
		}
	}
	void ClearComplete( void );
	int Process( float dt );
	void WorkThread( void );
	//
	virtual bool Create( void );
	virtual bool RequestFile( const char* cURL, const char* cDstFullpath );

};


