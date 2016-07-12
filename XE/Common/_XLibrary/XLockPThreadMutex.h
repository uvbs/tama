#pragma once
#if !defined(_SERVER) && !defined(_XBOT)		// not server
// 서버에선 사용 불가.
#ifdef WIN32
#include "pthread_win32/include/pthread.h"
#else
#include <pthread.h>
#endif

//#define _XCHECK_DEADLOCK
class XThreadID_pthread_t
{
	pthread_t m_idThread;
public:
	XThreadID_pthread_t() {}
	XThreadID_pthread_t( pthread_t idThread ) {
		m_idThread = idThread;
	}
	GET_ACCESSOR( pthread_t, idThread );
	BOOL IsEqual( XThreadID_pthread_t& idOther ) {
		if( pthread_equal( m_idThread, idOther.GetidThread() ) )
			return TRUE;
		return FALSE;
	}
	BOOL operator == ( XThreadID_pthread_t& rhs ) {
		return IsEqual(rhs);
	}
};

class XLockPthreadMutex
{
public:
	static XThreadID_pthread_t sGetThreadID( void );
private:
	pthread_mutex_t	 m_mutex;
	ID m_idLock;
	void Init() {
		m_idLock = (DWORD) this;
//		pthread_mutex_init( &m_mutex, NULL );
		pthread_mutexattr_t mutexAttributes;
		pthread_mutexattr_init(&mutexAttributes);
		pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
		int iRes = pthread_mutex_init(&m_mutex, &mutexAttributes);
		pthread_mutexattr_destroy(&mutexAttributes);
		XASSERT(iRes == 0);
//		m_cntLock = 0;
		// 썅 이거 초기화 어케 시키냐.
		//m_idOwnThread = 0?
//		InitOwnThread();
	}
	void Destroy() {
		int status = pthread_mutex_destroy( &m_mutex );
	}
/*
	void InitOwnThread( void )
	{
		m_bEmptyOwnThread = TRUE;
	}
*/
public:
	XLockPthreadMutex() { 
		Init(); 
//		XTRACE( "create Lock: id=0x%08x", m_idLock );
	}
	virtual ~XLockPthreadMutex() { Destroy(); }


	// 크리티컬섹션은 같은스레드 안에선 Lock이 두번불려도 안죽지만 이건 데드락 걸림.
	void Lock( LPCTSTR szFunc = nullptr ) {
		pthread_mutex_lock( &m_mutex );
	}
	void Unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
};

#endif // not server

