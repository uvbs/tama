#pragma once
#include "etc/global.h"

// this가 y를 리테인 시키며 세팅함.
#define SET_RETAIN_ACCESSOR( x, y )       inline void Set##y( x t )   \
													{ \
														if( m_##y ) m_##y->Release(); \
														if( t ) \
															t->Retain(); \
														m_##y = t;  \
													}
#define GET_RETAIN_ACCESSOR( x, y )       inline x Get##y()           { return m_##y; };
#define GET_SET_RETAIN_ACCESSOR( x, y )   SET_RETAIN_ACCESSOR( x, y ) GET_RETAIN_ACCESSOR( x, y )


// this가 y를 리테인 시키며 세팅함.
// this는 자동으로 락이 걸림.
// y도 락오브젝트라면 외부에서 락시켜줘야함.(장차 같이 통합될듯.)
#define SET_LOCK_RETAIN_ACCESSOR( x, y )       inline void Set##y( x t )   \
{ \
	Lock(); \
	if( m_##y ) \
		m_##y->Release(); \
	if( t ) \
		t->Retain(); \
	m_##y = t; \
	Unlock(); \
};
#define GET_LOCK_RETAIN_ACCESSOR( x, y )       inline x Get##y()           { XLockObj lockObj(this); return m_##y; };
#define GET_SET_LOCK_RETAIN_ACCESSOR( x, y )   SET_LOCK_RETAIN_ACCESSOR( x, y ) GET_LOCK_ACCESSOR( x, y )

/**
 레퍼런스 카운팅 객체를 릴리즈 시키고 
 레퍼런스 카운트가 0이되면 삭제시킨다.
 14.7.4
 .이제 스레드안전을 위해 스레드 안에서의 delete PTR을 금지한다.
 스레드안에서는 Release만 한다. 그러므로 delete가 되는시점에서 cntRef가 < 0이되는수도 있음은 정상이다.
 이는 파괴되는 곳을 일원화 하기 위함이다.
 다만 Release가 될때 cntRef가 0이되면 그 객체는 작동을 멈춰야 한다. 예를들어 커넥션 객체라면
 파괴는 되지 않더라도 소켓은 닫아서 이후처리를 하지 않도록 해야 한다.
*/
 #define SAFE_RELEASE_REF( PTR ) {	\
	if( PTR ) { \
		PTR->Release(); \
		if( PTR->GetcntRef() <= 0 ) \
		{ \
			delete PTR; \
		} \
		PTR = NULL; \
	} \
}
/**
레퍼런스 카운트 기반 객체
*/
class XRefObj
{
	int m_cntRef;		// 레퍼런스 카운트
//	ID m_idObj;		// 이것도 장차 이쪽으로 올라와야 할듯.
	void Init() {
		m_cntRef = 0;
	}
	void Destroy() {
		// 레퍼런스 카운트가 0이 아니면 경고.
		XBREAK( m_cntRef > 0 );
	}
protected:
public:
	XRefObj() { 
		Init(); 
		m_cntRef = 1;		// 최초 생성시 1이 된다.
	}
	virtual ~XRefObj() { Destroy(); }

	// 이 객체 나도 쓰겠다고 침발라둠.
	virtual int Retain( void ) { 
		return ++m_cntRef; 
	}
	// 이 객체 다썼다고 풀어줌( 관리자에의해 내부 레퍼런스 카운트가 0이되면 자동삭제)
	virtual int Release( void ) { 
		--m_cntRef;		// 이제 마이너스가 되는것도 정상 SAFE_RELEASE_REF의 설명참조
		return m_cntRef;; 
	}
	GET_ACCESSOR( int, cntRef );
};

