#include "stdafx.h"
#include "XLockCS.h"
#include "XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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
	// 크리티컬섹션은 내부적으로 락카운트와 자신이 어느 스레드에 소유되어 있는지를 기억한단다.
	// 그래서 같은 스레드 내부에서는 중첩 호출해도 문제없다.  
	// 다만 두번 락했으면 두번 언락해줘야 한다.
	volatile const DWORD idThread = ::GetCurrentThreadId();
#ifdef _SERVER
	m_idThread = idThread;
	XDetectDeadLock::sGet()->Enter( idThread, szFunc );
#endif // _SERVER
	EnterCriticalSection( &m_cs );
}
/**
 @brief 
*/
void XLockCS::Unlock() 
{
	LeaveCriticalSection( &m_cs );
	volatile const DWORD idThread = ::GetCurrentThreadId();
#ifdef _SERVER
	m_idThread = 0;
	XDetectDeadLock::sGet()->Leave( idThread );
#endif // _SERVER
}
/**
 @brief 
*/
BOOL XLockCS::IsLock( void ) 
{
	DWORD idThread = GetCurrentThreadId();
	if( m_cs.OwningThread != (HANDLE) idThread && m_cs.LockCount != -1 )
		return TRUE;
	return FALSE;
}
