#pragma once

// 다중스레드에서 읽기와 쓰기가 일어나는 변수에 대해 사용한다.
#define SET_LOCK_ACCESSOR( x, y )       inline void Set##y( x t )   { Lock(__TFUNC__); m_##y = t; Unlock(); };
#define GET_LOCK_ACCESSOR( x, y )       inline x Get##y()           { XLockObj lockObj(this, __TFUNC__); return m_##y; };
#define GET_SET_LOCK_ACCESSOR( x, y )   SET_LOCK_ACCESSOR( x, y ) GET_LOCK_ACCESSOR( x, y )

#if defined(_SERVER) || defined(_XBOT)
#include "win32/XLockCS.h"
class XLock : public XLockCS
#else
#include "XLockPThreadMutex.h"
class XLock : public XLockPthreadMutex
#endif
{

	// 비어있는 클래스
	/**
	 XLock의 Lock(), Unlock()은같은스레드내에서 반복실행됬을때는 블럭킹걸리지 않는다는걸 보장함.
	*/
};

typedef std::shared_ptr< XLock > XSPLock;	

#if defined(_SERVER) || defined(_XBOT)
#define XTHREAD_ID		XThreadID_CS
#define GET_CURR_THREAD_ID()	XLockCS::sGetThreadID()
#else
#define XTHREAD_ID		XThreadID_pthread_t
#define GET_CURR_THREAD_ID()	XLockPthreadMutex::sGetThreadID()
#endif

// 락을 걸어주고 블럭을 빠져나갈때 자동으로 언락을 해주는 매크로(XLock계열에서 사용될수 있다.)
#define XLOCK_OBJ		XLockObj _xLockObj( this, __TFUNC__ )
#define XLOCK_OBJ2( LOCK )		XLockObj _xLockObj( LOCK, __TFUNC__ )
#define XAUTO_LOCK						XLockObj _xLockObj( GetspLock(), __TFUNC__ )
#define XAUTO_LOCK2( LOCK )		XLockObj _xLockObj( LOCK, __TFUNC__ )
#define XAUTO_LOCK3( OBJ )		XLockObj _xLockObj( OBJ->GetspLock(), __TFUNC__ )
#define XAUTO_LOCK4( OBJ )		XLockObj2 _xLockObj( (OBJ)? OBJ->GetspLock() : nullptr, __TFUNC__ )

// 락을 걸어주고 블럭을 벗어날때 자동으로 락을 해제하는 락객체
class XLockObj
{
	XLock *m_pLock = nullptr;
	void Init() {}
	void Destroy() {}
public:
#if defined(_SERVER) || defined(_XBOT)
	XLockObj( XLock *pLock, LPCTSTR szFunc ) { 
#else
	XLockObj( XLock *pLock, LPCTSTR szFunc = nullptr ) {
#endif
		Init(); 
		m_pLock = pLock;
		pLock->Lock( szFunc );
	}
#if defined(_SERVER) || defined(_XBOT)
	XLockObj( XLock& lock, LPCTSTR szFunc ) {
#else
	XLockObj( XLock& lock, LPCTSTR szFunc = nullptr ) {
#endif
		Init();
		m_pLock = &lock;
		lock.Lock( szFunc );
	}
#if defined(_SERVER) || defined(_XBOT)
	XLockObj( std::shared_ptr<XLock>& spLock, LPCTSTR szFunc ) {
#else
	XLockObj( std::shared_ptr<XLock> spLock, LPCTSTR szFunc = nullptr ) {
#endif
		Init();
		m_pLock = spLock.get();
		spLock->Lock( szFunc );
	}
	template<typename T>
#if defined(_SERVER) || defined(_XBOT)
	XLockObj( std::shared_ptr<T>& _spLock, LPCTSTR szFunc ) {
#else
	XLockObj( std::shared_ptr<T>& _spLock, LPCTSTR szFunc = nullptr ) {
#endif
		Init();
		auto spLock = std::static_pointer_cast<XLock>( _spLock );
		m_pLock = spLock.get();
		spLock->Lock( szFunc );
	}
	~XLockObj() { 
		m_pLock->Unlock();
	}
};

// 락을 걸어야 하는 모객체에서 락객체를 복사해두고 락을 걸어서 모객체가 파괴되더라도 안전한 자동락 버전
class XLockObj2
{
	XSPLock m_spLock;	
	void Init() {}
	void Destroy() {}
public:
#if defined(_SERVER) || defined(_XBOT)
	XLockObj2( XSPLock spLock, LPCTSTR szFunc ) { 
#else
	XLockObj2( XSPLock spLock, LPCTSTR szFunc = nullptr ) {
#endif
		Init(); 
		m_spLock = spLock;
		if( spLock )		// null일수도 있음.
			spLock->Lock( szFunc );
	}
	~XLockObj2() { 
		if( m_spLock )
			m_spLock->Unlock();
	}
}; // XLockObj2

/**
	m_shoA를 락을걸고 pa에 담아준다. 블럭을 벗어날때 자동으로 락을 해제시켜준다.
	VAL *pa;
	SHARED_OBJ_BLOCK( pa, m_shoA )
	{
		*pa = 1;
	} END_BLOCK;
*/

// 여러스레드에서 함께쓰는 객체
template<typename T>
class XAutoSharedObj;

template<typename T>
class XSharedObj : public XLock {
	T m_Obj;
	// 락카운트. 같은스레드내에서 GetSharedObj()가 중첩으로 호출될때 Get/Release짝을 제대로 맞췄는지 검사하기 위해 사용.
	int m_cntLock;		
	DWORD m_idOwnThread;		// 점유중인 스레드아이디
	/**
	이함수는 오직 XAutoShareObj에서만 부를수 있다.
	XAutoSharedObj가 직접 락을 걸기때문에 락을 걸지않고 얻는 이 함수를 이용한다.
	*/
	T* _GetpForXAutoSharedObj() {
		return &m_Obj;
	}
public:
	XSharedObj() {
		m_cntLock = 0;
		m_idOwnThread = 0;
	}
	~XSharedObj() {
		if( m_cntLock )
			ReleaseSharedObj();
	}
	// 쉐어드객체의 포인터를 돌려줌과 동시에 락을 건다.
	T& GetSharedObj() {
		Lock(__TFUNC__);
		++m_cntLock;
		return m_Obj;
	}
	// 락걸린 쉐어드객체의 락을 해제한다.
	void ReleaseSharedObj() {
		--m_cntLock;
		Unlock();
	}
	// 제일먼저 한 락인지 확인.
	BOOL IsFirstLock() const {
		return (m_cntLock == 1 );
	}
#if defined(_SERVER) || defined(_XBOT)
// 	void Set( T& obj ) {
// 		m_Obj = &obj;
// 	}
	// pthread_mutex에서 현재 스레드가 락걸린건지 알수 없어서 일단 클라에선 사용하지 않기로 함.
	BOOL IsLock() const {
		return XLock::IsLock();
	}
#endif
friend class XAutoSharedObj<T>;
};
// template<typename T>
// class XSharedObj : protected XLock {
// 	T m_pObj = nullptr;
// 	// 락카운트. 같은스레드내에서 GetSharedObj()가 중첩으로 호출될때 Get/Release짝을 제대로 맞췄는지 검사하기 위해 사용.
// 	int m_cntLock;		
// 	DWORD m_idOwnThread;		// 점유중인 스레드아이디
// 	/**
// 	이함수는 오직 XAutoShareObj에서만 부를수 있다.
// 	XAutoSharedObj가 직접 락을 걸기때문에 락을 걸지않고 얻는 이 함수를 이용한다.
// 	*/
// 	T* _GetpForXAutoSharedObj() {
// 		return m_pObj;
// 	}
// public:
// 	XSharedObj() {
// 		m_cntLock = 0;
// 		m_idOwnThread = 0;
// 	}
// 	~XSharedObj() {
// 		if( m_cntLock )
// 			ReleaseSharedObj();
// 	}
// 	// 쉐어드객체의 포인터를 돌려줌과 동시에 락을 건다.
// 	T* GetpSharedObj() {
// 		Lock(__TFUNC__);
// 		++m_cntLock;
// 		return m_pObj;
// 	}
// 	// 락걸린 쉐어드객체의 락을 해제한다.
// 	void ReleaseSharedObj() {
// 		--m_cntLock;
// 		Unlock();
// 	}
// 	// 제일먼저 한 락인지 확인.
// 	BOOL IsFirstLock() const {
// 		return (m_cntLock == 1 );
// 	}
// #ifdef _SERVER
// 	void Set( T& obj ) {
// 		m_pObj = &obj;
// 	}
// 	// pthread_mutex에서 현재 스레드가 락걸린건지 알수 없어서 일단 클라에선 사용하지 않기로 함.
// 	BOOL IsLock() const {
// 		return XLock::IsLock();
// 	}
// #endif
// friend class XAutoSharedObj<T>;
// };


/**
 ex)
 // m_shoData는 DWORD형으로 들어있으며 pData는 DWORD*형으로 받는다.
 SHARED_OBJ_BLOCK( pm_Data, DWORD, m_shoData )
 {
	DWORD copy = *pData;
	*pm_Data = 2;
 } END_BLOCK;
*/
#define SHARED_OBJ_BLOCK( PVAL, TYPE, SHARE_OBJ ) { \
	XAutoSharedObj<TYPE> __xautoObj( SHARE_OBJ ); \
	TYPE* PVAL = __xautoObj.Get(); 

#define END_BLOCK		}

/**
 XSharedObj를 실어 대신 값을 꺼내주고 블럭을 빠져나갈때 자동으로 Relese를 해준다.
 단, 이것은 블럭이 끝날때 해제를 시켜 점유기간이 길기때문에 사용기간은 짧지만 
 블럭은 긴 함수에서는 사용을 안하고 직접 XSharedObj를 사용하여 해제시키는것이 좋다.
 다른방법으로는 쓸만큼만 블럭을 잡아놓고 그안에서 쓰는것이다.

 {
	// 다중스레드에서 공유되고 있는 자원 m_shoObj에 대한 사용법
	XSharedObj<XObj*> m_shoObj;
	XAutoSharedObj<XObj*> obj( m_shoObj );
	XObj *pObj = obj.Get();
	pObj->SetValue( 1 );
 }
*/
template<typename T>
class XAutoSharedObj
{
	XSharedObj<T> *m_pSharedObj;
public:
	XAutoSharedObj( XSharedObj<T>& sho) { 
		sho.Lock( __TFUNC__);
		m_pSharedObj = &sho;
	}
	virtual ~XAutoSharedObj() { 
		m_pSharedObj->Unlock();
//		m_pSharedObj->ReleaseSharedObj();
	}
	T* Get() const {
		return m_pSharedObj->_GetpForXAutoSharedObj();
//		return m_pSharedObj->GetSharedObj();
	}
};

