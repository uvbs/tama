/**
 // 메인
 Http요청을 요청큐에 넣는다.
 // 스레드
 스레드내에서 요청큐의 내용을 하나 뽑는다.
 Http서버로 전송한다.
 받는다.
 받은 결과를 결과 큐에 넣는다.
 // 메인
 결과큐에서 하나씩 뽑아서 델리게이트로 던져준다.

 // 보낼데이타
 POST/GET Method
 URL
 Body

*/
#include "StdAfx.h"
#include <afxinet.h>
#include "XFramework/XHttpMng.h"
#include "XFramework/XDetectDeadLock.h"

#define BTRACE		XTRACE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xHTTP;

/*
std::string szInbuff;
//	szInbuff = "fb_id=00007105381931";
szInbuff = "fb_id=";
szInbuff += SZ2C( szFBUserID );
szInbuff += "&app_id=EpicMonster&app_secret=KYj1fqG3WxzRfjB1uTsz3Chxf53llr";
XHttpMng::sGetSingleton()->SendReqPOST(
									_T( "https://api.lyto.mobi/api/dev/0.1/loginfbid.php" ),
									szInbuff.c_str(),
									this,
									xHTTP_MAKE_DELEGATE( this,
									&XGameSvrConnection::OnResponseLoginByLyto ),
									(void*)pUserData );
*/

XHttpMng* XHttpMng::s_pSingleton = NULL;
XHttpMng* XHttpMng::sGet() {
	if( s_pSingleton == NULL ) {
		s_pSingleton = new XHttpMng;
		s_pSingleton->Create( 1 );
	}
	return s_pSingleton;
}

//////////////////////////////////////////////////////////////////////////
static unsigned int __stdcall _WorkThread( void *param )
{
	XBREAK( param == 0 );
	XHttpMng *pHttpMng = static_cast<XHttpMng*>( param );
	//
	pHttpMng->WorkThread();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
XHttpMng::XHttpMng()
{
	Init();

	// 검색 요청 큐버퍼를 만든다.
	XQueueRequest* pQueueRequest = &m_shoQRequest.GetSharedObj();
	m_shoQRequest.ReleaseSharedObj();
	// 검색한 결과를 담을 큐버퍼를 만든다.
	XQueueFound *pQueueFound = &m_shoQFound.GetSharedObj();

	m_shoQFound.ReleaseSharedObj(); 
}

void XHttpMng::Destroy()
{
	m_bExit = TRUE;
	Sleep(1000);		// 스레드 빠져나올시간 줌.
// 	for( int i = 0; i < m_numConnect; ++i ) {
// 		CloseHandle( m_hThread[i] );
// 	}
// 	SAFE_DELETE_ARRAY( m_hThread );
	for( auto& th : m_aryThread ) {
//		CloseHandle( th.m_hHandle );
		SAFE_CLOSE_HANDLE( th.m_hHandle );
		XDetectDeadLock::sGet()->DelThread( th.m_idThread );
	}
	m_aryThread.clear();
//	SAFE_DELETE_ARRAY( m_hThread );

	XBREAK( m_shoQRequest.IsLock() == TRUE );
	//
	XQueueRequest* pQueueRequest = &m_shoQRequest.GetSharedObj();
	while( pQueueRequest->size() > 0 ) {
		xREQ req = pQueueRequest->front();
		pQueueRequest->pop();
	}
	m_shoQRequest.ReleaseSharedObj();
	//
	XBREAK( m_shoQFound.IsLock() == TRUE );
	// Lock: idRequest의 번호로 끝난 결과물이 있는지 찾아본다.
//	SHARED_OBJ_BLOCK( pQueue, XQueueFound, m_shoQFound )
	{
		XAutoSharedObj<XQueueFound> autoObj( m_shoQFound );
		auto pQueueFound = autoObj.Get();
		xREQ req;
		while( pQueueFound->size() > 0 ) {
			req = pQueueFound->front();
			pQueueFound->pop();
		}
	}
}

BOOL XHttpMng::Create( int numConnect )
{
	XBREAK( numConnect <= 0 );		// 최소 1은 있어야함.
//	m_hThread = new HANDLE[ numConnect ];
	m_numConnect = numConnect;
	for( int i = 0; i < numConnect; ++i ) {
//		m_pHttpPost[i] = new XHttpPost;
// 		DWORD idThread;
// 		m_hThread[i] = (HANDLE)_beginthreadex( NULL, 0, _WorkThread, (void*)this, 0, (unsigned*)&idThread );
		XE::xThread th;
		th.m_hHandle = (HANDLE)_beginthreadex( NULL, 0, _WorkThread, ( void* )this, 0, (unsigned*)&th.m_idThread );
		XDetectDeadLock::sGet()->AddThread( th, _T("http") );
// 		m_hThread[i] = CreateThread( NULL, 
// 									0, 
// 									_WorkThread, 
// 									(LPVOID) this, 
// 									0, 
// 									&idThread ); 
	}
	//
	CONSOLE( "XHttpMng create thread num=%d", numConnect );
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
std::string XHttpMng::SendRequestPOST( LPCTSTR szFullUrl, const char *cPostData )
{
	DWORD dwServiceType;
	CString strServerName;
	CString strObject;
	INTERNET_PORT port;
	std::string strRet;
	// 풀 url을 쪼갠다.
	if( ::AfxParseURL( (LPCTSTR)szFullUrl, dwServiceType, strServerName, strObject, port ) == FALSE )
		return strRet;
	if( !(dwServiceType == AFX_INET_SERVICE_HTTPS || dwServiceType == AFX_INET_SERVICE_HTTP) )
		return strRet;
	//
	m_hSession =InternetOpen( L"REQUEST_POST", INTERNET_OPEN_TYPE_PRECONFIG ,NULL, NULL, 0);
	if( m_hSession == NULL)
		return strRet;

	m_hConnect = InternetConnect(m_hSession, (LPCTSTR)strServerName, port, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 0);
	if(m_hConnect == NULL)
		return strRet;

	DWORD dwFlag = 0;
	if( dwServiceType == AFX_INET_SERVICE_HTTPS )	
	{
		dwFlag |= INTERNET_FLAG_SECURE;
		dwFlag |= SECURITY_FLAG_IGNORE_REVOCATION;
	}
	m_hRequest = HttpOpenRequest(m_hConnect, _T("POST"), (LPCTSTR)strObject, HTTP_VERSION, NULL, NULL, dwFlag, 0);
	if(m_hRequest == NULL)
		return strRet;
	//
	LPCSTR szContentType = "Content-Type: application/x-www-form-urlencoded";
	if(HttpSendRequestA(m_hRequest, szContentType, strlen(szContentType), (LPVOID)cPostData, strlen(cPostData)) == FALSE)
	{
		DWORD dr = GetLastError();
		return strRet;
	} 
	char bufQuery[128] = {0,};
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	DWORD bufSize = 0;
	DWORD dwSize = 0;
	if(HttpQueryInfoA(m_hRequest, HTTP_QUERY_CONTENT_LENGTH, bufQuery, &dwLengthBufQuery, NULL) == TRUE)
		// 리턴되는 타입이 json처럼 content length가 없는경우는 작동안됨
		bufSize = atoi(bufQuery);
	else
	{
		DWORD dr = GetLastError();
	}
	// 읽어올 데이터 사이즈 얻는데 실패하면 강제로 1024로 정함.
	if( bufSize == 0 )
		bufSize = 1024;
	char *pBuff = new char[ bufSize + 1 ];
	memset( pBuff, 0, bufSize + 1 );
	// 인터넷에서 bufSize만큼의 데이타를 읽어온다.
	BOOL bRead = InternetReadFile(m_hRequest, pBuff, bufSize, &dwSize);
	pBuff[dwSize] = 0;
	strRet = pBuff;
	SAFE_DELETE_ARRAY( pBuff );
	int len = strRet.length();
	InternetCloseHandle(m_hRequest);
	InternetCloseHandle(m_hConnect);
	InternetCloseHandle(m_hSession);
	return strRet;
}

//////////////////////////////////////////////////////////////////////////
std::string XHttpMng::SendRequestGET( LPCTSTR szFullUrl )
{
	_tstring aHeader;
	aHeader +=_T("Accept: */*\r\n");
	aHeader +=_T("User-Agent: Mozilla/4.0 (compatible; MSIE 5.0;* Windows NT)\r\n");
	aHeader +=_T("Content-type: application/x-www-form-urlencoded\r\n");
	aHeader +=_T("\r\n\r\n");
//	aHeader += _T("\r\nConnection: close\r\n\r\n");
	
	std::string strRet;
	DWORD dwServiceType;
	CString strServerName;
	CString strObject;
	INTERNET_PORT port;
	BOOL bResult = ::AfxParseURL( szFullUrl, dwServiceType, strServerName, strObject, port );

	m_hSession =InternetOpen( L"REQUEST_GET", INTERNET_OPEN_TYPE_PRECONFIG ,NULL, NULL, 0);
	if( m_hSession == NULL)
		return strRet;

	m_hConnect = InternetConnect(m_hSession, strServerName, port, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 0);
	if(m_hConnect == NULL)
		return strRet;


	std::string stdstrObject = SZ2C( (LPCTSTR)strObject );
	m_hRequest = HttpOpenRequestA(m_hConnect, "GET", stdstrObject.c_str(), "HTTP/1.1", "", NULL, 
									INTERNET_FLAG_SECURE |
									INTERNET_FLAG_KEEP_CONNECTION
							//		INTERNET_FLAG_RELOAD |
							//		INTERNET_FLAG_DONT_CACHE |
							//		INTERNET_FLAG_NO_COOKIES
									, 0);
	if(m_hRequest == NULL)
		return strRet;

	{
		DWORD dwFlags = 0;
		DWORD bufferLength = sizeof(dwFlags);
		if (!InternetQueryOption(m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, &bufferLength)) {
			XLOG("InternetQueryOption error");
			return strRet;
		}
		dwFlags |=	SECURITY_FLAG_IGNORE_UNKNOWN_CA | 
					SECURITY_FLAG_IGNORE_REVOCATION | 
					SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTP | 
					SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTPS | 
					SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | 
					SECURITY_FLAG_IGNORE_CERT_CN_INVALID;

		if (!InternetSetOption(m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags)))
		{
			// 타임아웃에 대한 처리가 되어있지 않음.
			InternetCloseHandle(m_hRequest);
			InternetCloseHandle(m_hConnect);
			InternetCloseHandle(m_hSession);
		
			DWORD dr = GetLastError();
			return strRet;
		}
	}

	//통신할 헤더 설정
	DWORD dwFlags = HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD;
	if (!HttpAddRequestHeaders(m_hRequest, aHeader.c_str(), aHeader.length(), dwFlags))
	{
		InternetCloseHandle(m_hRequest);
		InternetCloseHandle(m_hConnect);
		InternetCloseHandle(m_hSession);
		return strRet;
	}

//	LPCSTR szContentType = "Content-Type: application/x-www-form-urlencoded";
	if(HttpSendRequestA(m_hRequest, NULL, 0, NULL, 0) == FALSE)
		return strRet;


	DWORD dwSize = 0;
	char bufQuery[128] = {0,};
	DWORD dwLengthBufQuery = sizeof(bufQuery);

	DWORD bufSize = 0;
	HttpQueryInfoA(m_hRequest, HTTP_QUERY_STATUS_CODE, bufQuery, &dwLengthBufQuery, NULL);

	if(HttpQueryInfoA(m_hRequest, HTTP_QUERY_CONTENT_LENGTH, bufQuery, &dwLengthBufQuery, NULL) == TRUE)
		bufSize = atoi(bufQuery);
	else
	{
		DWORD dr = GetLastError();
		//cout << dr;
	}

	if( bufSize == 0 )
		bufSize = 1024;
	char *pBuff = new char[ bufSize + 1 ];
	memset(pBuff, 0, bufSize + 1);

	BOOL bRead = InternetReadFile(m_hRequest, pBuff, bufSize, &dwSize);
	pBuff[dwSize] = 0;
	strRet = pBuff;
	SAFE_DELETE_ARRAY( pBuff );
	InternetCloseHandle(m_hRequest);
	InternetCloseHandle(m_hConnect);
	InternetCloseHandle(m_hSession);
	return strRet;
	
}


void XHttpMng::WorkThread( void )
{
	while(1)
	{
		XQueueRequest *pQueueRequest = nullptr;
		while(1)
		{
			if( m_bExit )
				return;
			// Lock
			pQueueRequest = &m_shoQRequest.GetSharedObj();
			if( pQueueRequest->size() > 0 )
				break;
			m_shoQRequest.ReleaseSharedObj();
			// http는 빠르게 처리해야 할필요 없으므로 1로 줌.
			Sleep(10);
		}
		// 요청큐에서 하나 꺼냄
		xREQ req = pQueueRequest->front();
		pQueueRequest->pop();	// 요청이 실패할수도 있으니 큐에서 바로 삭제하면 안될듯.
		// Unlock: 
		m_shoQRequest.ReleaseSharedObj();

		BOOL bFind = FALSE;
		switch( req.msgFind )	{
		case xHTTP_POST: {
				req.cstrResponse = SendRequestPOST( req.tstrURL.c_str(), 
													req.cstrBody.c_str() );
		} break;
		case xHTTP_GET: {
				// 아직 GET사용법을 잘 모르겠슴.
				req.cstrResponse = SendRequestGET( req.tstrURL.c_str() );
		} break;
		default:
			XBREAKF( 1, "알수없는 요청 메시지" );
			break;
		}
		// 검색결과 큐에 넣음.
		// Lock:
		if( req.delegator.empty() == false ) {		// 요청결과를 다시 받고 싶은경우에만 넣는다.
			XQueueFound *pQueueFound = &m_shoQFound.GetSharedObj();
			//
			pQueueFound->push( req );			
			m_shoQFound.ReleaseSharedObj();
		}
		Sleep(1);
	}
}

void XHttpMng::Process( void )
{
	if( this == nullptr )
		return;
	xREQ req;
	// Lock: 찾은 결과물 데이타의 큐를 각 커넥션 객체에게 통보해준다.
// 	SHARED_OBJ_BLOCK( pQueue, XQueueFound, m_shoQFound )
	{
		XAutoSharedObj<XQueueFound> autoObj( m_shoQFound );
		auto pQueueFound = autoObj.Get();
		// 큐가 비워질때까지 다 퍼냄
		while( pQueueFound->size() > 0 ) {
			req = pQueueFound->front();
			pQueueFound->pop();
			if( req.delegator.empty() == false ) {
				req.delegator( req.cstrResponse.c_str(), &req );
				SAFE_RELEASE_REF( req.pDelegateObj );
			}	
		}
	}
}

/**
 szURL: 접속할 서버 url
 cPostBody: POST할 데이타
 pDelegate: 응답결과를 받을 델리게이터 객체
 idReq: 요청 식별자. 멀티로 요청했을때 어떤 요청의 응답인지를 식별하기위함.
*/
 

void XHttpMng::SendReqPOST( LPCTSTR szURL, 
							const char *cPostBody,
							XRefObj *pDelegateObj,
							xHTTP::xREQ::xDELEGATE delegator,
							void *pUserData )
{
//	XBREAKF( m_numConnect == 0, "최초 한번 XHttpMng::sInit()를 호출해 주시오" );
	if( s_pSingleton == nullptr ) {
		// 실제 사용될때만 생성됨.
		sInit(4);
	}
	xREQ req;
	req.msgFind = xHTTP_POST;
	req.delegator = delegator;
	req.pDelegateObj = pDelegateObj;
	req.tstrURL = szURL;
	req.cstrBody = cPostBody;
	req.pUserData = pUserData;
	if( pDelegateObj )
		pDelegateObj->Retain();

	XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
	pQueue->push( req );
	m_shoQRequest.ReleaseSharedObj();
}
