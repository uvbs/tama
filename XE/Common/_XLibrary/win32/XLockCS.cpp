#include "stdafx.h"
#include "XLockCS.h"
#include "XFramework/XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

ID XLockCS::s_idGlobal = 0;

XThreadID_CS XLockCS::sGetThreadID( void )
{
	DWORD idt = GetCurrentThreadId();
	XThreadID_CS idThread( idt );
	return idThread;
	
}
/**
 @brief 
*/
void XLockCS::Lock( LPCTSTR szFunc ) 
{
//	xOUTPUT_DEBUG_STRING( "idGlobalLock:id=%d", s_idGlobal );
	// 크리티컬섹션은 내부적으로 락카운트와 자신이 어느 스레드에 소유되어 있는지를 기억한단다.
	// 그래서 같은 스레드 내부에서는 중첩 호출해도 문제없다.  
	// 다만 두번 락했으면 두번 언락해줘야 한다.
#if defined(_SERVER) || defined(_XBOT)
	XThread* pThCurr = nullptr;
	if( XDetectDeadLock::sGet() ) {
		if( m_idLockObj == 0 ) {
			XBREAK_POINT();
		}
		volatile const DWORD idThread = ::GetCurrentThreadId();
		m_idThread = idThread;
		XDetectDeadLock::sGet()->Lock();
		if( XDetectDeadLock::sGet()->IsWillDeadlock( idThread, m_idLockObj ) ) {
			XBREAK_POINT();	// 데드락!
		}
		//	XDetectDeadLock::sGet()->Enter( idThread, szFunc );
		pThCurr = XDetectDeadLock::sGet()->FindpThreadByID( idThread );
		if( XASSERT( pThCurr ) ) {
			pThCurr->SetTryEnterObj( m_idLockObj );
		}
		XDetectDeadLock::sGet()->Unlock();
	}
#endif // _SERVER
//	xOUTPUT_DEBUG_STRING("Lock:%s", szFunc);
	EnterCriticalSection( &m_cs );

#if defined(_SERVER) || defined(_XBOT)
	if( XDetectDeadLock::sGet() ) {
		XDetectDeadLock::sGet()->Lock();
		if( pThCurr ) {
			pThCurr->Enter( m_idLockObj, szFunc );
		}
		XDetectDeadLock::sGet()->Unlock();
	}
#endif // _SERVER
}
/**
 @brief 
*/
void XLockCS::Unlock() 
{
#if defined(_SERVER) || defined(_XBOT)
	if( XDetectDeadLock::sGet() ) {
		volatile const DWORD idThread = ::GetCurrentThreadId();
		XDetectDeadLock::sGet()->Lock();
		m_idThread = 0;
		auto pTh = XDetectDeadLock::sGet()->FindpThreadByID( idThread );
		pTh->Leave( m_idLockObj );
		XDetectDeadLock::sGet()->Unlock();
	}
	LeaveCriticalSection( &m_cs );
#else
	LeaveCriticalSection( &m_cs );
#endif // _SERVER
}
/**
 @brief 
*/
BOOL XLockCS::IsLock() const
{
	DWORD idThread = GetCurrentThreadId();
	if( m_cs.OwningThread != (HANDLE) idThread && m_cs.LockCount != -1 )
		return TRUE;
	return FALSE;
}
