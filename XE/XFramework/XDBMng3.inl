#if 0
#include "XDBMng3.h"
#include "XLock.h"
//#include "XDatabase.h"

template<typename T>
unsigned int __stdcall XDBMng3<T>::_WorkThread( void *param ) 
{
	XBREAK( param == 0 );
	XDatabase *pDB = static_cast<XDatabase*>( param );
	XDBMng3<T>::sGet()->WorkThread( pDB );
	_endthreadex( 0 );
	return 1;
}
//////////////////////////////////////////////////////////////////////////
template<typename T>
XDBMng3<T>::XDBMng3() 
{
	Init();
	// 검색 요청 큐버퍼를 만든다.
// 	XQueueRequest* pQueueRequest = &m_shoQRequest.GetSharedObj();
// 	m_shoQRequest.ReleaseSharedObj();
	// 검색한 결과를 담을 큐버퍼를 만든다.
// 	XQueueFound* pQueueFound = &m_shoQFound.GetSharedObj();
// 	m_shoQFound.ReleaseSharedObj();
	//
	Create();
}
template<typename T>
void XDBMng3<T>::Process() {
	{
		auto& refQueue = m_shoQFound2.GetSharedObj();
		int sizeQueue = refQueue.size();
		m_shoQFound2.ReleaseSharedObj();
		if( sizeQueue > 0 ) {
//			int sizeOld = 0;
//			int sizeAfter = 0;
			{
				auto pQueue = &m_shoQFound2.GetSharedObj();
				xResultSend resultObj = pQueue->front();
//				sizeOld = resultObj.spOwner.use_count();
				pQueue->pop();
//				const int size2 = resultObj.spOwner.use_count();
				m_shoQFound2.ReleaseSharedObj();
//				const int size3 = resultObj.spOwner.use_count();
				// 객체 락
				if( resultObj.spOwner ) {
//					MAIN->m_fpsToGameSvr.Process();
					resultObj.spOwner->GetspLock()->Lock( __TFUNC__ );
					if( resultObj.callbackResult != nullptr )
						resultObj.callbackResult( resultObj.spOwner.get() );
					else
						resultObj.spOwner->__SendForXE( resultObj.pk );
					resultObj.spOwner->GetspLock()->Unlock();
				}
// 				sizeAfter = resultObj.spOwner.use_count();
// 				int a = 0;
			}
		}
	}
}

template<typename T>
void XDBMng3<T>::WorkThread( XDatabase *pDB ) 
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
			Sleep( 1 );
		}
		// 요청큐에서 하나 꺼냄
		xCallback callbackObj = pQueueRequest->front();
		pQueueRequest->pop();
		// Unlock: 
		m_shoQRequest.ReleaseSharedObj();		// 콜백하는동안엔 process에서 result를 못보내도록 막음.
		BOOL bFind = FALSE;
		// 콜백함수 콜
//		m_shoQFound2.Lock();
		callbackObj.callback( pDB, callbackObj.spOwner );
//		m_shoQFound2.Unlock();
		// 원래 SAFE_RELEASE_REF를 해야 맞지만 스레드 안에서의 커넥션삭제 구조가 문제가 있어서 Release만 일단 하도록 바꿈
		//			SAFE_RELEASE_REF( callbackObj.spOwner );
//		callbackObj.spOwner->Release();
		Sleep(m_msecSleep);
	}
}
#endif // 0