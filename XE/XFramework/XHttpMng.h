
#pragma once

#include <wininet.h>
#include <queue>
#include "XLock.h"
#include "XRefObj.h"
#include "fast_delegate/include/delegate.h"

#pragma   comment(lib, "Wininet.lib ")
#pragma   comment(lib, "user32.lib ")

#define xHTTP_MAKE_DELEGATE(RECV, MFN) \
	fd::make_delegate(MFN, RECV)


namespace xHTTP
{
	enum xtMsg {		
		xHTTP_NONE = 0,
		xHTTP_POST,
		xHTTP_GET,
	};
	struct xREQ {
		typedef fd::delegate<void (const char*, xHTTP::xREQ*)> xDELEGATE;
		xDELEGATE delegator;		// 응답이 오면 던져질 델리게이터 함수+객체(콜백)
		XRefObj *pDelegateObj;		// 델리게이터 객체
		xtMsg msgFind;			// 검색 종류
		_tstring tstrURL;
		std::string cstrBody;	
		std::string cstrResponse;	// 응답받은 결과 
		void *pUserData;			// 사용자 맘대로 쓸 포인터
		xREQ() {			
			msgFind = xHTTP_NONE;
			pDelegateObj = NULL;
			pUserData = NULL;
		}
	};
};

//////////////////////////////////////////////////////////////////////////
class XHttpMng;
class XHttpMng
{	
public:
	static XHttpMng* sGet();
	// 최초한번 불러줄것.
	static void sInit( int numConnect ) {
		sGet()->Create( numConnect );
	}
	static void sDestroySingleton() {
		SAFE_DELETE( s_pSingleton );
	}
private:
	// 사용하지 말것.
	static XHttpMng* _sGetSingletonPtr() {
		return s_pSingleton;
	}
	static XHttpMng *s_pSingleton;
	int			m_numConnect;		// 커넥션(스레드)수
//	HANDLE		*m_hThread;
	std::vector<XE::xThread> m_aryThread;
	volatile BOOL m_bExit;
	typedef std::queue<xHTTP::xREQ> XQueueRequest;
	typedef std::queue<xHTTP::xREQ> XQueueFound;
	XSharedObj<XQueueRequest> m_shoQRequest;		// 검색요청 큐
	XSharedObj<XQueueFound> m_shoQFound;		// 검색결과 큐
	//
	HINTERNET	m_hSession;
	HINTERNET	m_hConnect;
	HINTERNET	m_hRequest;
	int m_msecSleepReady = 10;
	int m_msecSleepPump = 1;
	void Init()	{
		m_numConnect = 0;
//		m_hThread = NULL;
		m_bExit = FALSE;
		m_hSession = NULL;
		m_hConnect = NULL;
		m_hRequest = NULL;
	}	
	void Destroy();
private:
	std::string SendRequestPOST( LPCTSTR szFullUrl, const char *cPostData );
	std::string SendRequestGET( LPCTSTR szFullUrl );
	BOOL Create( int numConnect );
	void Process( void );
public:
	XHttpMng();
	~XHttpMng(){ Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( int, msecSleepReady );
	GET_SET_ACCESSOR_CONST( int, msecSleepPump );

	void WorkThread( void );
	void SendReqPOST( LPCTSTR szURL, 
					const char *cPostBody,
					XRefObj *pDelegateObj,
					xHTTP::xREQ::xDELEGATE delegator,
					void *pUserData=NULL );


friend class XEServerMain;
};


