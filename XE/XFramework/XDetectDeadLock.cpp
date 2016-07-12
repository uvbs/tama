#include "stdafx.h"
#if defined(_SERVER) || defined(_XBOT)
#include "XFramework/XDetectDeadLock.h"
#include <process.h>

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
std::shared_ptr<XDetectDeadLock>& XDetectDeadLock::sGet() {	
	if( s_spInstance == nullptr ) 
		s_spInstance = std::shared_ptr<XDetectDeadLock>( new XDetectDeadLock ); 
	return s_spInstance; 
}
void XDetectDeadLock::sDestroyInstance() {
	s_spInstance.reset();
}

unsigned int __stdcall XDetectDeadLock::sWorkThread( void *param )
{
//	XBREAK( param == 0 );
	XDetectDeadLock::sGet()->WorkThread();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void XThread::Enter( ID idLockObj, LPCTSTR szFunc )
{
	m_idLockTryEnter = 0;
	++m_cntEnter;
	m_sec = XTimer2::sGetTime();
	m_stackStrtFunc.push( (szFunc)? _tstring(szFunc) : _tstring() );
	AddLockedObj( idLockObj );
}

void XThread::Leave( ID idLockObj )
{
	DelLockedObj( idLockObj );
	if( m_stackStrtFunc.size() == 0 ) {
		xOUTPUT_DEBUG_STRING("break:%s", __TFUNC__);
		XBREAK_POINT();
	}
	m_stackStrtFunc.pop();
}

void XThread::AddLockedObj( ID idLockObj ) 
{
// 	if( IsLockedObj( idLockObj ) ) {
// 		return;
// 	}
	m_listLockedObj.push_back( idLockObj );
}

void XThread::DelLockedObj( ID idLockObj ) 
{
	XBREAK( idLockObj == 0 );
	bool bError = false;
	if( m_listLockedObj.size() == 0 ) {
		bError = true;
	} else {
		auto itor = m_listLockedObj.end();
		--itor;
		const auto idLastLock = (*itor);
		if( idLastLock != idLockObj ) {
			bError = true;
		} else {
			m_listLockedObj.erase( itor++ );
		}
		--m_cntEnter;
		if( m_cntEnter < 0 ) {
			bError = true;
		}
	}
	if( bError ) {
		xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
		XBREAK_POINT();
	}
// 	for( auto itor = m_listLockedObj.begin(); itor != m_listLockedObj.end(); ) {
// 		const ID idLock = ( *itor );
// 		if( idLock == idLockObj ) {
// 			--m_cntEnter;
// 			if( m_cntEnter < 0 ) {
// 				XBREAK_POINT();
// 				if( m_cntEnter == 0 ) {
// 					m_listLockedObj.erase( itor++ );
// 				}
// 			}
// 			return;
// 		} else {// if( idLock == idLockObj ) {
// 			++itor;
// 		}
// 	} // for
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
	if( this == nullptr )
		return;
	InitializeCriticalSection( &m_cs );
	DWORD idThread;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, sWorkThread, nullptr/*(void*)th.m_pDB*/, 0, (unsigned*)&idThread );

}

void XDetectDeadLock::AddThread( ID idThread, HANDLE hThread, LPCTSTR szTag )
{
	if( this == nullptr )
		return;
	EnterCriticalSection( &m_cs );
	XThread* pTh = nullptr;
	for( auto& th : m_listThread ) {
		if( th.GetidThread() == idThread ) {
			pTh = &th;
			break;
		}
	}
//	auto pTh = FindpThreadByID( idThread );
	if( pTh == nullptr ) {
		XThread th( idThread, szTag );
//		th.m_hThread = hThread;
// 		if( szTag )
// 			th.m_strtTag = szTag;
		m_listThread.push_back( th );
	} else {
//		pTh->m_hThread = hThread;
		if( szTag )
			pTh->SetstrtTag( szTag );
		if( pTh->GetcntEnter() > 0 ) {
			xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
			XBREAK_POINT();
		}
	}
	LeaveCriticalSection( &m_cs );
}

void XDetectDeadLock::AddThread( const XE::xThread& th, LPCTSTR szTag )
{
	if( this == nullptr )
		return;
	AddThread( th.m_idThread, th.m_hHandle, szTag );
}

void XDetectDeadLock::DelThread( ID idThread )
{
	if( this == nullptr )
		return;
	Lock();
	if( idThread == 0 ) {
		xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
		XBREAK_POINT();
	}
	for( auto itor = m_listThread.begin(); itor != m_listThread.end(); ) {
		if( (*itor).GetidThread() == idThread ) {
			m_listThread.erase( itor++ );
			Unlock();
			return;
		} else {
			++itor;
		}
	}
	xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
	XBREAK_POINT();		// 일단 이런경우는 없는걸로.
	Unlock();
}

/**
 @brief Lock이 되기전 이곳에 스레드 정보를 기록한다.
*/
// void XDetectDeadLock::Enter( DWORD idThread, LPCTSTR szFunc )
// {
// 	EnterCriticalSection( &m_cs );
// 	auto pTh = FindpThreadByID( idThread );
// 	if( pTh == nullptr ) {
// 		XBREAK_POINT();
// 	}
// 	if( pTh ) {
// 		pTh->AddCntEnter();
// 		pTh->Setsec( XTimer2::sGetTime() );		// 마지막으로 락건시간.
// 		if( szFunc )
// 			pTh->SetstrtFunc( szFunc );
// 	} else {
// 		XBREAK_POINT();
// 	}
// 	LeaveCriticalSection( &m_cs );
// }
// 
// /**
//  @brief Leave가 되기전 이곳에 쓰레드 정보를 기록한다.
// */
// void XDetectDeadLock::Leave( DWORD idThread, LPCTSTR szFunc )
// {
// 	EnterCriticalSection( &m_cs );
// 	auto pTh = FindpThreadByID( idThread );
// 	if( pTh == nullptr ) {
// 		XBREAK_POINT();
// 	}
// 	if( pTh ) {
// 		pTh->DecCntEnter();
// 		if( szFunc )
// 			pTh->SetstrtFunc( szFunc );
// 	}
// 	LeaveCriticalSection( &m_cs );
// }

/**
 @brief 어떤스레드의 정보를 업데이트해야할지 찾는다.
*/
XThread* XDetectDeadLock::FindpThreadByID( ID idThread )
{
	if( this == nullptr )
		return nullptr;
	if( idThread == 0 ) {
		xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
		XBREAK_POINT();
	}
	for( auto& th : m_listThread ) {
		if( th.GetidThread() == idThread )
			return &th;
	}
	// 없으면 일단 하나 추가시키고
	AddThread( idThread, nullptr, nullptr );
	// 다시 읽어서 리턴.
	for( auto& th : m_listThread ) {
		if( th.GetidThread() == idThread )
			return &th;
	}
	xOUTPUT_DEBUG_STRING( "break:%s", __TFUNC__ );
	XBREAK_POINT();
	return nullptr;
}

/**
 @brief 진입하려하는 락객체 등록
*/
// void XDetectDeadLock::SetTryEnterObj( ID idThread, ID idLockObjTry ) 
// {
// 	auto pTh = FindpThreadByID( idThread );
// 	if( XASSERT(pTh) ) {
// 		pTh->SetTryEnterObj( idLockObjTry );
// 	}
// }
// /**
//  @brief 이미 진입한 락객체 등록
// */
// void XDetectDeadLock::AddLockedObj( ID idThread, ID idLockedObj ) 
// {
// 	auto pTh = FindpThreadByID( idThread );
// 	if( XASSERT(pTh) ) {
// 		pTh->SetTryEnterObj( 0 );
// 		pTh->AddLockedObj( idLockedObj );
// 	}
// }

XThread* XDetectDeadLock::FindpThreadByLockedObj( ID idLockObj )
{
	if( this == nullptr )
		return nullptr;
	for( auto& th : m_listThread ) {
		if( th.IsLockedObj( idLockObj ) )
			return &th;
	}
	return nullptr;
}

bool XDetectDeadLock::IsWillDeadlock( ID idThreadCurr, ID idLockObjTry ) 
{
	if( this == nullptr )
		return false;
	// 현재스레드 idthreadCurr이 잠그고 있는 객체 목록을 꺼냄
	auto pThCurr = FindpThreadByID( idThreadCurr );
	// [현재스레드에서 잠그려는 객체를 이미 잠그고 있는 스레드]A 검색
	auto pThOtherAlreadyLocked = FindpThreadByLockedObj( idLockObjTry );
	if( !pThOtherAlreadyLocked )
		return false;
	// 현재스레드에서 잠근객체목록중 하나를 thOtherAlreadyLocked스레드가 진입시도중인가.
	bool bLock1 = pThCurr->IsLockedObj( pThOtherAlreadyLocked->GetidLockTryEnter() );
	// thOtherAlreadyLocked가 잠근 객체를 curr스레드에서 진입 시도중인가
	bool bLock2 = pThOtherAlreadyLocked->IsLockedObj( idLockObjTry );
	return ( bLock1 && bLock2 );
}




void XDetectDeadLock::WorkThread()
{
	if( this == nullptr )
		return;
	while(1) {
		// 주기적으로 검사하여 leave하지 못한 락이 있으면 경고를 띄운다.
		EnterCriticalSection( &m_cs );
		for( auto& thread : m_listThread ) {
			if( thread.GetcntEnter() > 0 ) {
				const xSec secPass = XTimer2::sGetTime() - thread.Getsec();
				if( secPass > 5 ) {
					auto thCopy = thread;
					LeaveCriticalSection( &m_cs );	// 카피뜨고 lock을 풀어줌.
					// 카피본으로 로그호출(내부에서 lock을 하면서 다시 이걸 호출을 해서 unlock을 함.)
					CONSOLE( "Detected deadlock:idTh=%d, numLockedObj=%d, idLockTryEnter=%d, cntEnter=%d"
						, thCopy.GetidThread(), thCopy.GetlistLockedObj().size(), thCopy.GetidLockTryEnter(), thCopy.GetcntEnter() );
					_tstring strCallstack;
					while( thCopy.GetstackStrtFunc().size() > 0 ) {
						strCallstack += thCopy.GetstackStrtFunc().top();
						strCallstack += _T(" => ");
						thCopy.GetstackStrtFunc().pop();
					}
//					XBREAK_POINT();
					int a= 0;
					Sleep( 100 );
					EnterCriticalSection( &m_cs );
					continue;
				}
			}
		}
		LeaveCriticalSection( &m_cs );
		Sleep(100);
	}	
}

/*
{
	m_pObjCopy = pObj;
}
{

	m_list.Lock();
	for( pObj : m_list ) {
		// 믜미가 없음. 애초에 객체에 락을 거는건 다른곳에서 이객체를 쓰기전에 기다리라는것인데.
		// 여기서 삭제되버리면 어차피 블로킹되어있던 다른스레드에서 죽는건 마찬가지.
		// 그러므로 리스트에서부터 락을 걸고 지울땐 객체에게 따로 락을 걸지 않아도 된다.
		pObj->Lock();					
		SAFE_DELETE( pObj );			
	}
	m_list.Unlock();

	// 두 리스트가 한 객체를 참조하는 경우
	m_list.Lock();
	for( pObj : m_list ) {
	pObj->Do();							// 리스트만 통과하면 함께 쓸수 있으므로 pObj의 원자성을 보장할수 없음. 
	}
	m_list.Unlock();
	
	// 객체에락까지 거는 경우
	m_list.Lock();
	for( pObj : m_list ) {
		pObj->Lock();
		다른스레드에서 pObj를 삭제만 하지 않는다면 문제 없음. 그러나 언젠가는 삭제가 있을테니 현실성이 없음. 삭제가 없는 정적인 리스트라면 가능.
		pObj->Do();							// 같은객체를 건드리려할때는 되지만 Do내부에서 m_list를 락걸려할때 다른스레드에서 m_list의 진입에서 블로킹되어있는 상태라면 데드락
		pObj->UnLock();
	}
	m_list.Unlock();
	//
	m_list.Lock();
	m_lockDestroy.Lock();
	for( pObj : m_list ) {
		pObj->Lock();
		pObj->Do();							// 같은객체를 건드리려할때는 되지만 Do내부에서 m_list를 락걸려할때 다른스레드에서 m_list의 진입에서 블로킹되어있는 상태라면 데드락
		pObj->UnLock();
	}
	m_lockDestroy.UnLock();
	m_list.Unlock();
	//
	m_list.Lock();
	for( spObj : m_list ) {
		spObj->Lock();
		spObj->Do();							
		spObj->UnLock();
	}
	m_list.Unlock();
	//
	m_list2.Lock();
	for( spObj : m_list2 ) {
		spObj->Lock();
		if( !spObj->IsDestroy() )
			spObj->Do();
		spObj->UnLock();
	}
	m_list2.Unlock();

	그렇다면 객체 개별 락은 아예 필요 없는것인가.
	예를들어 XUser에서 XConnect를 참조하고 있다.
	shared_ptr<XConnect>가 삭제(리스트에서 제거)되어도 다른 클래스에서 아직 참조하고 있으니 참조한다고 죽지는 않는다
	list<XConnect>의 Process를 돌때는 개별락을 해야한다.
	결론은 리스트내부의 객체를 건드리려면 리스트를 계속 락으로 잡고 있어야 한다.
	리스트따로 객체 따로 락을 잡으면 서로 교차되어 데드락이 걸릴수 있다.
	shared_ptr을 쓰더라도 리스트전체에 락을 걸어야 하는것은 같다. 리스트의 객체들이 삭제가 없는 정적이라면 몰라도.
	이는 모두 객체 삭제가 이루어지는것 때문이다.
	아니면....밑에서 위로 올라가는 락. 이를테면 객체락을 먼저잡고 내부에서 리스트의 락을 잡는 행동을 하지않으면 된다.
	  => 객체 삭제때문에 이것도 안됨. 
	고의로 2가지 케이스의 데드락을 내서 디텍터가 잡아내는지 확인할것.
}



*/

/*
struct {
	m_Lock
	m_spConnect;
};

m_listConn.Lock();
for( auto& conn : m_listConn ) {
	conn.m_Lock->Lock();
	conn.spConnect->Process();
	if( conn.spConnect->IsDestroy() ) {
		m_mapConn.Lock();
		m_mapConn.erase( conn.spConnect );
		m_mapConn.Unlock();
		conn.m_spConnect = nullptr;
	}
	conn.m_Lock->Unlock();		=> 달리 참조하는곳이 없다면 위에서 삭제되어 여기서 죽음. sharedObj()방식으로 써야하나.
	if( conn.m_spConnect == nullptr ) {
		m_listConn.erase( conn );

	}
}

class XUser {
	XConnect m_spConnect; = > 안전 ?
		Do() {
		m_spConnect()->Lock();
		m_spConnect()->Do();
	}
	Do2() {
		m_mapConn.Lock();
		m_mapConn.Find();
		m_mapConn.Unlock();
	}
}

*/




#endif // defined(_SERVER) || defined(_XBOT)




