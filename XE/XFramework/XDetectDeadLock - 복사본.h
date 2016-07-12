/********************************************************************
	@date:	2016/03/11 13:32
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XDetectDeadLock.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
namespace XE {
	struct xThread;
};

//////////////////////////////////////////////////////////////////////////
class XThread {
	ID idThread;
	list<ID> m_listLockedObj;			// idThread가 이미 진입한 객체
	ID m_idLockTryEnter;							// idThread가 진입시도중인 객체
	// this스레드가 idLockObj를 이미 잠그고 있는 상태인가.
	bool IsLockedObj( ID idLockObj ) {
		return m_listLockedObj.Find( idLockObj );
	}
	// idLockObj를 this스레드에서 진입시도중인가.
	bool IsTryLock( ID idLockObj ) {
		return m_idLockTryEnter == idLockObj;
	}
	void SetTryEnterObj( ID idLockObj ) {
		m_idLockTryEnter = idLockObj;
	}
	// idLockObj를 잠금.
	void AddLockedObj( ID idLockObj ) {
		m_idLockTryEnter = 0;
		m_listLockedObj.push_back( idLockObj );
	}
	void DelLockedObj( ID idLockObj ) {
		m_listLockedObj.Del( idLockObj );
	}
};


/****************************************************************
* @brief 데드락 감지기. 스레드하나당 하나씩 붙는다.
* @author xuzhu
* @date	2016/03/11 13:34
*****************************************************************/
class XDetectDeadLock
{
	struct xThread {
		ID m_idThread = 0;
		HANDLE m_hThread = nullptr;
		int m_cntEnter = 0;					// 락이걸린 횟수
		_tstring m_strtFunc;			// Lock을 사용한 함수명
		xSec m_sec = 0;						// 락 건 시간.
		_tstring m_strtTag;
	};
public:
	static std::shared_ptr<XDetectDeadLock>& sGet();
	static void sDestroyInstance();
public:
	XDetectDeadLock();
	virtual ~XDetectDeadLock() { Destroy(); }
	//
	void OnCreate();
	void Add( ID idThread, HANDLE hThread, LPCTSTR szTag );
	void Add( const XE::xThread& th, LPCTSTR szTag );
	void Del( ID idThread );
	void Enter( DWORD idThread, LPCTSTR szFunc );
	void Leave( DWORD idThread, LPCTSTR szFunc = nullptr );
private:
	static std::shared_ptr<XDetectDeadLock> s_spInstance;
	std::list<xThread> m_listThread;
	HANDLE m_hThread = nullptr;			// 데드락 감지용 자체 스레드
	CRITICAL_SECTION m_cs;
private:
	static unsigned int __stdcall sWorkThread( void *param );
	void Init() {}
	void Destroy();
	xThread* Find( ID idThread );
	void WorkThread();
}; // class XDetectDeadLock

class XDetectDeadLock {
	std::list<XThread> m_listThread;
	//
	XThread* FindThreadByID( ID idThread ) {
		return m_listThread.Find( idThread );
	}
	XThread* FindThreadByLockedObj( ID idLockObj ) {
		for( auto& th : m_listThread ) {
			if( th.IsLockedObj( idLockObj ) )
				return &th;
		}
		return nullptr;
	}
	// 진입하려하는 락객체 등록
	void SetTryEnterObj( ID idThread, ID idLockObjTry ) {
		auto pTh = FindThreadByID( idThread );
		pTh->SetTryEnterObj( idLockObjTry );
	}
	// 이미 진입한 락객체 등록
	void AddLockedObj( ID idThread, ID idLockedObj ) {
		auto pTh = FindThreadByID( idThread );
		pTh->SetTryEnterObj( 0 );
		pTh->AddLockedObj( idLockedObj );
	}
	//
	bool IsWillDeadlock( ID idThreadCurr, ID idLockObjTry ) {
		// 현재스레드 idthreadCurr이 잠그고 있는 객체 목록을 꺼냄
		auto pThCurr = FindThreadByID( idThreadCurr );
		// [현재스레드에서 잠그려는 객체를 이미 잠그고 있는 스레드]A 검색
		auto thOtherAlreadyLocked = FindThreadByLockedObj( idLockObjTry );
		if( !thOtherAlreadyLocked )
			return false;
		// 현재스레드에서 잠근객체목록중 하나를 thOtherAlreadyLocked스레드가 진입시도중인가.
		bool bLock1 = pThCurr->IsLockedObj( thOtherAlreadyLocked.m_idLockTryEnter );
		// thOtherAlreadyLocked가 잠근 객체를 curr스레드에서 진입 시도중인가
		bool bLock2 = thOtherAlreadyLocked->IsLockedObj( idLockObjTry );
		if( bLock1 && bLock2 )
			return true;
		return false;
	}
};

class Lock {
	ID m_idLock;		// 락객체의 고유번호
	//
	Enter() {
		auto idThread = GetCurrentThreadId();
		// 진입전 데드락상황인지 검사
		if( XDetectDeadLock::sGet()->IsWillDeadlock( idThread, m_idLock ) ) {
			XBREAK( 1 );		// 데드락!
		}
		// this락객체 접근 시도.
		auto pTh = XDetectDeadLock::sGet()->FindThreadByID( idThread );
		pTh->SetTryEnterObj( m_idLock );
		EnterCriticalSection();
		// 접근시도 클리어
		pTh->SetTryEnterObj( 0 );
		// 진입성공한 락객체 등록
		pTh->AddLockedObj( m_idLock );
	}
	//
	Leave() {
		auto idThread = GetCurrentThreadId();
		auto pTh = XDetectDeadLock::sGet()->FindThreadByID( idThread );
		pTh->DelLockedObj( m_idLock );
		LeaveCriticalSection();
	}
};

