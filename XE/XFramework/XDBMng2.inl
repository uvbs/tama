#include "XFramework/XDBMng2.h"
#include "XLock.h"
//#include "XDatabase.h"

template<typename T>
unsigned int __stdcall XDBMng2<T>::_WorkThread( void *param ) 
{
	XBREAK( param == 0 );
	XDatabase *pDB = static_cast<XDatabase*>( param );
	XDBMng2<T>::sGet()->WorkThread( pDB );
//	_endthreadex( 0 );
	return 0;
}
//////////////////////////////////////////////////////////////////////////
template<typename T>
XDBMng2<T>::XDBMng2() 
{
	Init();
// 	Create();
}
template<typename T>
void XDBMng2<T>::Process() {
	XBREAK( m_aryThread.size() == 0 );
	{
		auto& refQueue = m_shoQFound2.GetSharedObj();
		int sizeQueue = refQueue.size();
		m_shoQFound2.ReleaseSharedObj();
//		if( sizeQueue > 0 ) {
		int cnt = 10;
		while( sizeQueue > 0 && cnt-- > 0 ) {
			auto pQueue = &m_shoQFound2.GetSharedObj();
			xResultSend resultObj = pQueue->front();
			pQueue->pop();
			sizeQueue = pQueue->size();
			m_shoQFound2.ReleaseSharedObj();
			// 객체 락
			if( resultObj.spOwner ) {
#ifdef _DB_SERVER
				MAIN->m_fpsToGameSvr.Process();	// DB서버에선 이걸 마땅히 넣을곳이 없어 여기다 둠.
#endif // _DB_SERVER
				//					resultObj.spOwner->GetspLock()->Lock( __TFUNC__ );
				if( resultObj.callbackResult != nullptr )
					resultObj.callbackResult( resultObj.spOwner.get() );
				else
					resultObj.spOwner->__SendForXE( resultObj.pk );
				//					resultObj.spOwner->GetspLock()->Unlock();
			}
		}
		m_sizeResultQ = sizeQueue;
	}
}
// template<typename T>
// void XDBMng2<T>::Process() {
// 	XBREAK( m_aryThread.size() == 0 );
// 	{
// 		auto& refQueue = m_shoQFound2.GetSharedObj();
// 		int sizeQueue = refQueue.size();
// 		m_shoQFound2.ReleaseSharedObj();
// 		if( sizeQueue > 0 ) {
// 			auto pQueue = &m_shoQFound2.GetSharedObj();
// 			xResultSend resultObj = pQueue->front();
// 			pQueue->pop();
// 			m_sizeResultQ = pQueue->size();
// 			m_shoQFound2.ReleaseSharedObj();
// 			// 객체 락
// 			if( resultObj.spOwner ) {
// #ifdef _DB_SERVER
// 				MAIN->m_fpsToGameSvr.Process();	// DB서버에선 이걸 마땅히 넣을곳이 없어 여기다 둠.
// #endif // _DB_SERVER
// //					resultObj.spOwner->GetspLock()->Lock( __TFUNC__ );
// 				if( resultObj.callbackResult != nullptr )
// 					resultObj.callbackResult( resultObj.spOwner.get() );
// 				else
// 					resultObj.spOwner->__SendForXE( resultObj.pk );
// //					resultObj.spOwner->GetspLock()->Unlock();
// 			}
// 		}
// 	}
// }

template<typename T>
void XDBMng2<T>::WorkThread( XDatabase *pDB )
{
	while( 1 ) {
		XQueueRequest* pQueueRequest = nullptr;
		while( 1 ) {
			if( m_bExit )
				return;
			// Lock
			pQueueRequest = &m_shoQRequest.GetSharedObj();
			if( pQueueRequest->size() > 0 )
				break;
			m_shoQRequest.ReleaseSharedObj();
			Sleep( m_msecSleepReady );
		}
		int cnt = 10;
		while(1) {
			// 요청큐에서 하나 꺼냄
			xCallback callbackObj = pQueueRequest->front();
			pQueueRequest->pop();
			// Unlock: 
			m_sizeReqQ = pQueueRequest->size();
			m_shoQRequest.ReleaseSharedObj();		// 콜백하는동안엔 process에서 result를 못보내도록 막음.
			BOOL bFind = FALSE;
			// 콜백함수 콜
			callbackObj.callback( callbackObj.spOwner.get(), pDB );
			if( m_sizeReqQ > 0 && cnt-- > 0 ) {
				// Lock
				pQueueRequest = &m_shoQRequest.GetSharedObj();
				m_sizeReqQ = pQueueRequest->size();
				if( m_sizeReqQ <= 0 ) {	// 잠깐 unlock시킨동안에 큐가 빠졌을수도 있다.
					m_shoQRequest.ReleaseSharedObj();		// 콜백하는동안엔 process에서 result를 못보내도록 막음.
					break;
				}
			} else {
				break;
			}
		}
		if( m_sizeReqQ > 500 )	// 큐가 많이 쌓이면 busy모드로 전환된다.
			m_bBusy = true;
		else 
		if( m_sizeReqQ == 0 )		// 큐를 다 소화하면 다시 정상모드로 돌아간다.
			m_bBusy = false;
		const int msec = (m_bBusy)? 0 : m_msecSleepPump;
		Sleep( msec );
	}
}

// template<typename T>
// void XDBMng2<T>::WorkThread( XDatabase *pDB ) 
// {
// 	while( 1 ) {
// 		XQueueRequest* pQueueRequest = nullptr;
// 		while( 1 ) {
// 			if( m_bExit )
// 				return;
// 			// Lock
// 			pQueueRequest = &m_shoQRequest.GetSharedObj();
// 			if( pQueueRequest->size() > 0 )
// 				break;
// 			m_shoQRequest.ReleaseSharedObj();
// 			Sleep( m_msecSleepReady );
// 		}
// 		// 요청큐에서 하나 꺼냄
// 		xCallback callbackObj = pQueueRequest->front();
// 		pQueueRequest->pop();
// 		// Unlock: 
// 		m_sizeReqQ = pQueueRequest->size();
// 		m_shoQRequest.ReleaseSharedObj();		// 콜백하는동안엔 process에서 result를 못보내도록 막음.
// 		BOOL bFind = FALSE;
// 		// 콜백함수 콜
// #ifdef _DB_SERVER
// // 		callbackObj.spOwner->GetspLock()->Lock(__TFUNC__);
// #endif // _DB_SERVER
// 		callbackObj.callback( callbackObj.spOwner.get(), pDB );
// #ifdef _DB_SERVER
// // 		callbackObj.spOwner->GetspLock()->Unlock();
// #endif // _DB_SERVER
// 		Sleep( m_msecSleepPump );
// 	}
// }
