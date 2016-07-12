#pragma once
//#ifdef _XDBMNG2


#include <queue>
#include "XLock.h"
#include "XRefObj.h"
// #pragma warning( disable : 4005 )	//  warning C4005: 'UINT8_MAX' : 매크로 재정의
// #include <std/bind.hpp>
// #include <std/function.hpp>
// #pragma warning( default : 4005 )	//  warning C4005: 'UINT8_MAX' : 매크로 재정의
#include "XDatabase.h"
#include "XFramework/XDetectDeadLock.h"

class XDatabase;

//////////////////////////////////////////////////////////////////////////
template<typename T>
class XDBMng2
{	
#ifdef _XNUM_CORE
	enum { MAX_DB = _XNUM_CORE };
#else
 #if defined(_DEBUG) && !defined(_XBOT)
	enum { MAX_DB = 2 };		// 개발중엔 편의상 스레드 2개만 쓴다.
 #else
	enum { MAX_DB = 8 };
 #endif 
#endif
public:
	static XDBMng2<T>* sGet() {		static XDBMng2<T> instance;		return &instance;	}
	// 싱글톤 파괴는 autoDestroy가 자동으로 함.
public:
	struct xCallback {
		// DB작업을 수행할 멀티스레드용 콜백함수
		std::function<void( T*, XDatabase* )> callback;
		std::shared_ptr<T> spOwner;
		xCallback() {}
	};
	struct xResultSend {
		std::shared_ptr<T> spOwner;
		XPacket pk;
		// 메인스레드에서 pk패킷을 보내기전에 해야할일이 있으면 콜백함수를 넣어쓴다.
		std::function<void( T* )> callbackResult;
		xResultSend() : pk( false ) {
		}
	};
private:
	//
	struct xThread {
		ID m_idThread = 0;
		HANDLE m_hThread = nullptr;
		XDatabase* m_pDB = nullptr;
	};
	std::vector<xThread> m_aryThread;
	volatile BOOL m_bExit;
	typedef std::queue<xCallback> XQueueRequest;
	typedef std::queue<xCallback> XQueueFound;
	typedef std::queue<xResultSend> XQueueFound2;
	XSharedObj<XQueueRequest> m_shoQRequest;		// 검색요청 큐
	XSharedObj<XQueueFound2> m_shoQFound2;		// 검색결과 큐
	ID m_idMng = 0;
	int m_msecSleepReady = 10;
	int m_msecSleepPump = 1;
	int m_sizeReqQ = 0;
	int m_sizeResultQ = 0;
	bool m_bBusy = false;
	//
	void Init() {
		m_bExit = FALSE;
		m_idMng = XE::GenerateID();
	}	
	void Destroy() {
		m_bExit = TRUE;
		for( auto& th : m_aryThread ) {
			::WaitForSingleObject( th.m_hThread, INFINITE );  //Thread 가 종료 될때 까지 기다렸다가 CloseHandle 시킨다.
			SAFE_CLOSE_HANDLE( th.m_hThread );
			th.m_hThread = INVALID_HANDLE_VALUE;
			XDetectDeadLock::sGet()->DelThread( th.m_idThread );
		}
		XBREAK( m_shoQRequest.IsLock() == TRUE );
		//
		for( auto& th : m_aryThread ) {
			SAFE_DELETE( th.m_pDB );
		}
		m_aryThread.clear();
	}
private:
	static unsigned int __stdcall _WorkThread(void *param);
public:
	XDBMng2();
	virtual ~XDBMng2(){ Destroy(); }
	//
	BOOL Create( int numThread )	{
#if defined(_DEBUG) && !defined(_XBOT)
		// 봇이 아닌 디버그모드에선 개발편의상 스레드 적게 쓴다.
		numThread = 2;
#endif
		if( XBREAK( numThread <= 0 || numThread > 32 ) )
			return FALSE;
		// DB 스레드 생성
		for( int i = 0; i < numThread; ++i )  {
			xThread th;
			th.m_pDB = new XDatabase;
			th.m_hThread = (HANDLE)_beginthreadex( NULL
																						, 0
																						, _WorkThread
																						, (void*)th.m_pDB
																						, 0
																						, (unsigned*)&th.m_idThread );
			XDetectDeadLock::sGet()->AddThread( th.m_idThread, th.m_hThread, _T("dbmng") );
			m_aryThread.push_back( th );
		}
		CONSOLE( "XDBMng2 create thread num=%d", MAX_DB );
		return TRUE;
	}
	void Process( void );
	void WorkThread( XDatabase *pDB );
	GET_ACCESSOR_CONST( int, msecSleepReady );
	GET_ACCESSOR_CONST( int, msecSleepPump );
	GET_BOOL_ACCESSOR( bBusy );
	inline void SetmsecSleepReady( int msec ) {
		XBREAK( msec < 0 || msec > 1000 || msec == XE::INVALID_INT );
		m_msecSleepReady = msec;
	}
	inline void SetmsecSleepPump( int msec ) {
		XBREAK( msec < 0 || msec > 1000 || msec == XE::INVALID_INT );
		m_msecSleepPump = msec;
	}
	GET_ACCESSOR_CONST( int, sizeReqQ );
	GET_ACCESSOR_CONST( int, sizeResultQ );

	//
	template<typename F, typename T1>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1 );
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2 );
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3);
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4);
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4, p5);
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4, p5, p6);
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7 ) {
		xCallback callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4, p5, p6, p7 );
		callBackObj.spOwner = spOwner;
		XQueueRequest *pQueue = &m_shoQRequest.GetSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}

/*
	bind가 인수 10개이상 지원 안하는듯.
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8 ) {
		xCALLBACK callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4, p5, p6, p7, p8 );
		callBackObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueRequest *pQueue = m_shoQRequest.GetpSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	void DoAsyncCall( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8, T9 p9 ) {
		xCALLBACK callBackObj;
		callBackObj.callback
			= std::bind( func, std::placeholders::_1, std::placeholders::_2, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
		callBackObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueRequest *pQueue = m_shoQRequest.GetpSharedObj();
		pQueue->push( callBackObj );
		m_shoQRequest.ReleaseSharedObj();
	}
*/
	/**
	 DB작업후 결과를 받을 이벤트 핸들러를 등록한다.
	*/
	void RegisterSendPacket( std::shared_ptr<T> spOwner, XPacket& pk) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		resultObj.spOwner = spOwner;
		resultObj.pk = pk;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3, p4 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3, p4, p5 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3, p4, p5, p6 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3, p4, p5, p6, p7 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}
	template<typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void RegisterSendCallback( std::shared_ptr<T> spOwner, F func, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8 ) {
		// 메인스레드에서 사용금지
		XBREAK( XEServerMain::sGet()->GetidThreadMain() == ::GetCurrentThreadId() );
		xResultSend resultObj;
		if( func )
			resultObj.callbackResult = std::bind( func, std::placeholders::_1, p1, p2, p3, p4, p5, p6, p7, p8 );
		resultObj.spOwner = spOwner;
		//spOwner->Retain();
		XQueueFound2 *pQueueFound = &m_shoQFound2.GetSharedObj();
		//
		pQueueFound->push( resultObj );			
		m_shoQFound2.ReleaseSharedObj();
	}


friend class XEServerMain;
};

#include "XDBMng2.inl"

#pragma warning ( default : 4005 )
//#endif // #ifdef _XDBMNG2
