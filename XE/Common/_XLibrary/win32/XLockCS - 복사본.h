#pragma once

class XThreadID_CS
{
	DWORD m_idThread;
public:
	XThreadID_CS() {}
	XThreadID_CS( DWORD idThread ) {
		m_idThread = idThread;
	}
	GET_ACCESSOR( DWORD, idThread );
	BOOL IsEqual( XThreadID_CS& idOther ) {
//		if( pthread_equal( m_idThread, idOther.GetidThread() ) )
		if( m_idThread == idOther.GetidThread() )
			return TRUE;
		return FALSE;
	}
	BOOL operator == ( XThreadID_CS& rhs ) {
		return IsEqual(rhs);
	}
};

// 락 크리티컬 섹션 버전
class XLockCS
{
public:
	static XThreadID_CS sGetThreadID( void );
private:
	CRITICAL_SECTION m_cs;
	DWORD m_idThread = 0;		// 마지막으로 락을 걸었을때 스레드아이디 leave하면 없어짐.
	void Init() {}
	void Destroy() {
		DeleteCriticalSection( &m_cs );
	}
public:
	XLockCS() { 
		Init(); 
		InitializeCriticalSection( &m_cs );
	}
	virtual ~XLockCS() { Destroy(); }

	// 락할때 현재 스레드 아이디 같이 넘겨주면 디버깅 할때 편할듯.
	void Lock( LPCTSTR szFunc = nullptr );
	void Unlock();
	BOOL IsLock();
//
//	inline DWORD GetThreadID( void ) {
//		return GetCurrentThreadId();
//	}	
};

XE_NAMESPACE_START( XE )
//
struct xThread {
	HANDLE m_hHandle = nullptr;
	ID m_idThread = 0;
};
//
XE_NAMESPACE_END; // XE


