#include "stdafx.h"
#include "XDetectDeadLock.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/**
 락 정책
 1.Lock을 했으면 언락을 해줘야 한다.
 2. 락을 걸어 진입한 상태에서 다른 객체로의 진입(락)을 시도해선 안된다. 교차락으로 인한 데드락.
*/

std::shared_ptr<XDetectDeadLock> XDetectDeadLock::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XDetectDeadLock>& XDetectDeadLock::sGet() {	if( s_spInstance == nullptr ) s_spInstance = std::shared_ptr<XDetectDeadLock>( new XDetectDeadLock ); return s_spInstance; }
void XDetectDeadLock::sDestroyInstance() {
	s_spInstance.reset();
}

unsigned int __stdcall XDetectDeadLock::sWorkThread( void *param )
{
//	XBREAK( param == 0 );
	XDetectDeadLock::sGet()->WorkThread();
	_endthreadex( 0 );
	return 1;
}

////////////////////////////////////////////////////////////////
XDetectDeadLock::XDetectDeadLock()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XDetectDeadLock::Destroy()
{
	DeleteCriticalSection( &m_cs );
}

void XDetectDeadLock::OnCreate()
{
	InitializeCriticalSection( &m_cs );
	DWORD idThread;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, sWorkThread, nullptr/*(void*)th.m_pDB*/, 0, (unsigned*)&idThread );

}

void XDetectDeadLock::Add( ID idThread, HANDLE hThread, LPCTSTR szTag )
{
	EnterCriticalSection( &m_cs );
	auto pTh = Find( idThread );
	if( pTh == nullptr ) {
		xThread th;
		th.m_idThread = idThread;
		th.m_hThread = hThread;
		if( szTag )
			th.m_strtTag = szTag;
		m_listThread.push_back( th );
	} else {
		pTh->m_hThread = hThread;
		if( pTh->m_cntEnter > 0 ) {
			XBREAK_POINT();
		}
	}
	LeaveCriticalSection( &m_cs );
}

void XDetectDeadLock::Add( const XE::xThread& th, LPCTSTR szTag )
{
	Add( th.m_idThread, th.m_hHandle, szTag );
}

void XDetectDeadLock::Del( ID idThread )
{
	if( idThread == 0 ) {
		XBREAK_POINT();
	}
	for( auto itor = m_listThread.begin(); itor != m_listThread.end(); ) {
		if( (*itor).m_idThread == idThread ) {
			m_listThread.erase( itor++ );
			return;
		} else {
			++itor;
		}
	}
	XBREAK_POINT();		// 일단 이런경우는 없는걸로.
}

/**
 @brief Lock이 되기전 이곳에 스레드 정보를 기록한다.
*/
void XDetectDeadLock::Enter( DWORD idThread, LPCTSTR szFunc )
{
	EnterCriticalSection( &m_cs );
	auto pTh = Find( idThread );
	if( pTh == nullptr ) {
		XBREAK_POINT();
// 		// 찾아보고 없는 스레드 아이디면 새로 생성.
// 		Add( idThread, nullptr );
// 		pTh = Find( idThread );
	}
	if( pTh ) {
		++pTh->m_cntEnter;
		pTh->m_sec = XTimer2::sGetTime();		// 마지막으로 락건시간.
		if( szFunc )
			pTh->m_strtFunc = szFunc;
	} else {
		XBREAK_POINT();
	}
	LeaveCriticalSection( &m_cs );
}

/**
 @brief Leave가 되기전 이곳에 쓰레드 정보를 기록한다.
*/
void XDetectDeadLock::Leave( DWORD idThread, LPCTSTR szFunc )
{
	EnterCriticalSection( &m_cs );
	auto pTh = Find( idThread );
	if( pTh == nullptr ) {
		XBREAK_POINT();
	}
	if( pTh ) {
		--pTh->m_cntEnter;
		if( pTh->m_cntEnter < 0 ) {
			XBREAK_POINT();
		}
		if( szFunc )
			pTh->m_strtFunc = szFunc;
	}
	LeaveCriticalSection( &m_cs );
}

/**
 @brief 어떤스레드의 정보를 업데이트해야할지 찾는다.
*/
XDetectDeadLock::xThread* XDetectDeadLock::Find( ID idThread )
{
	if( idThread == 0 ) {
		XBREAK_POINT();
	}
	for( auto& th : m_listThread ) {
		if( th.m_idThread == idThread )
			return &th;
	}
	return nullptr;
}

void XDetectDeadLock::WorkThread()
{
	while(1) {
		// 주기적으로 검사하여 leave하지 못한 락이 있으면 경고를 띄운다.
		EnterCriticalSection( &m_cs );
		for( const auto& th : m_listThread ) {
			if( th.m_cntEnter > 0 ) {
				const xSec secPass = XTimer2::sGetTime() - th.m_sec;
				if( secPass > 5 ) {
//					XBREAK_POINT();
					int a= 0;
				}
			}
		}
		LeaveCriticalSection( &m_cs );
		Sleep(1);
	}	
}

