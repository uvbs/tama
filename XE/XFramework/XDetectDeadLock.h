/********************************************************************
	@date:	2016/03/11 13:32
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XDetectDeadLock.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#if defined(_SERVER) || defined(_XBOT)
namespace XE {
	struct xThread;
};

//////////////////////////////////////////////////////////////////////////
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/12 17:24
*****************************************************************/
class XThread
{
public:
	XThread( ID idThread, LPCTSTR szTag ) { 
		Init(); 
		m_idThread = idThread;
		if( szTag )
			m_strtTag = szTag;
	}
	virtual ~XThread() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( ID, idThread );
	GET_SET_ACCESSOR_CONST( ID, idLockTryEnter );
	GET_SET_ACCESSOR_CONST( int, cntEnter );
//	GET_SET_ACCESSOR_CONST( const _tstring&, strtFunc );
	GET_SET_ACCESSOR_CONST( xSec, sec );
	GET_SET_ACCESSOR_CONST( const _tstring&, strtTag );
	GET_ACCESSOR_CONST( const std::list<ID>&, listLockedObj );
	GET_ACCESSOR( std::stack<_tstring>&, stackStrtFunc );
	const _tstring GetstrtFunc() {
		if( m_stackStrtFunc.size() == 0 ) {
			XBREAK_POINT();
		}
		return m_stackStrtFunc.top();
	}
	void AddCntEnter() {
		++m_cntEnter;
	}
	void DecCntEnter() {
		--m_cntEnter;
		if( m_cntEnter < 0 ) {
			XBREAK_POINT();
		}
	}
	// public member
	// this스레드가 idLockObj를 이미 잠그고 있는 상태인가.
	bool IsLockedObj( ID idLockObj ) {
		for( const auto idLock : m_listLockedObj ) {
			if( idLock == idLockObj )
				return true;
		}
		return false;
	}
	// idLockObj를 this스레드에서 진입시도중인가.
	bool IsTryLock( ID idLockObj ) {
		return m_idLockTryEnter == idLockObj;
	}
	void SetTryEnterObj( ID idLockObj ) {
		m_idLockTryEnter = idLockObj;
	}
	// idLockObj를 잠금.
	void AddLockedObj( ID idLockObj );
	void Enter( ID idLockObj, LPCTSTR szFunc );
	void Leave( ID idLockObj );
	void DelLockedObj( ID idLockObj );
private:
	// private member
	ID m_idThread;
	std::list<ID> m_listLockedObj;			// idThread가 이미 진입한 객체
	ID m_idLockTryEnter = 0;							// idThread가 진입시도중인 객체
	int m_cntEnter = 0;						// 락이 걸린 횟수.
	std::stack<_tstring> m_stackStrtFunc;					// Lock을 사용한 하ㅐㅁ수명
	xSec m_sec = 0;								// 락건 시간.
	_tstring m_strtTag;						// 디버깅용 태그
private:
	// private method
	void Init() {}
	void Destroy() {}
friend class XDetectDeadLock;
}; // class XThread


/****************************************************************
* @brief 데드락 감지기. 스레드하나당 하나씩 붙는다.
* @author xuzhu
* @date	2016/03/11 13:34
*****************************************************************/
class XDetectDeadLock
{
public:
	static std::shared_ptr<XDetectDeadLock>& sGet();
	static void sDestroyInstance();
public:
	XDetectDeadLock();
	virtual ~XDetectDeadLock() { Destroy(); }
	//
	void OnCreate();
	void AddThread( ID idThread, HANDLE hThread, LPCTSTR szTag );
	void AddThread( const XE::xThread& th, LPCTSTR szTag );
	void DelThread( ID idThread );
// 	void Enter( DWORD idThread, LPCTSTR szFunc );
// 	void Leave( DWORD idThread, LPCTSTR szFunc = nullptr );
	XThread* FindpThreadByID( ID idThread );
	XThread* FindpThreadByLockedObj( ID idLockObj );
	bool IsWillDeadlock( ID idThreadCurr, ID idLockObjTry );
	void Lock() {
		if( this == nullptr )
			return;
		EnterCriticalSection( &m_cs );
	}
	void Unlock() {
		if( this == nullptr )
			return;
		LeaveCriticalSection( &m_cs );
	}
private:
	static std::shared_ptr<XDetectDeadLock> s_spInstance;
	std::list<XThread> m_listThread;
	HANDLE m_hThread = nullptr;			// 데드락 감지용 자체 스레드
	CRITICAL_SECTION m_cs;
private:
	static unsigned int __stdcall sWorkThread( void *param );
	void Init() {}
	void Destroy();
// 	XThread* Find( ID idThread );
	void WorkThread();
}; // class XDetectDeadLock
#endif // defined(_SERVER) || defined(_XBOT)
