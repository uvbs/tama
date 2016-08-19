#ifndef __XPOOL_H__
#define __XPOOL_H__

#include "global.h"
/*
	XPool<XPARTICLE> *m_pPoolParticle;	// 스트럭트 형태로 넣으면 내부적으로 포인터 리스트로 관리한다
	m_pPoolParticle = new XPool( 10000 );
*/
template <typename T>
class XPool
{
	struct XNODE
	{
		int active;				// 사용/비사용
		int idxNextEmpty;	// 비어있는 다음풀의 인덱스
	};
	int m_idxLastEmpty;		// 비어있는 풀의 인덱스
	int m_nNum;				// 실제 값이 채워진 풀의 개수
	int m_nMax;				// 풀의 전체 크기
	XNODE *m_pUsedPool;		// 각 풀노드가 사용이 되었는지를 가리키는 플래그. m_pPool의 인덱스에 대치된다
	char *m_pPool;
	void Init( void ) {
		m_nNum = m_idxLastEmpty = 0; 
		m_nMax = 0; 
		m_pUsedPool = NULL;
		m_pPool = NULL;

	}
	// 비어있는 노드를 찾는다
	T* GetEmptyNode( void ) {		
		if( m_idxLastEmpty < 0 )		// 더이상 비어있는 노드가 없다
			return NULL;
		return (T*)(m_pPool + sizeof(T) * m_idxLastEmpty);	
	}
public:
	XPool() {	Init(); 	}			
	XPool( int nMax ) { 
		Init(); 
		Create( nMax );
	}
	virtual ~XPool() { Destroy(); }
	void Destroy( void ) {
		SAFE_DELETE_ARRAY( m_pPool );
		SAFE_DELETE_ARRAY( m_pUsedPool );
	}
	void Create( int nMax ) {
		m_nMax = nMax;
//		m_pPool = new T[ nMax ];		// 메모리 풀 할당
		m_pPool = new char[ sizeof( T ) * nMax ];
		m_pUsedPool = new XNODE[ nMax ];
		// 비어있는 다음 노드를 세팅
		int i;
		for( i = 0; i < nMax-1; i ++ )
		{
			m_pUsedPool[i].active = 0;
			m_pUsedPool[i].idxNextEmpty = i+1;
		}
		m_pUsedPool[ i ].active = 0;
		m_pUsedPool[ i ].idxNextEmpty = -1;		// 가장 끝은 '뒤가'없다
//		T *pObj = (T *)m_pPool;
//		for( i = 0; i < nMax; i ++ )
//			pObj[ i ].Create();
		
	}
	//
	// 모든 요소를 클리어한다.
	void Clear( void ) {
		// 비어있는 다음 노드를 세팅
		int nMax = m_nMax;
		int i;
		for( i = 0; i < nMax-1; i ++ )	{
			m_pUsedPool[i].active = 0;
			m_pUsedPool[i].idxNextEmpty = i+1;
		}
		m_pUsedPool[ i ].active = 0;
		m_pUsedPool[ i ].idxNextEmpty = -1;		// 가장 끝은 '뒤가'없다
		m_idxLastEmpty = 0;
		m_nNum = 0;
	}
	void Resize( int size ) { 

		XBREAKF( 1, "아직 구현안됨" ); 
		XLOG( "Resize pool:%d", m_nMax );
	}
	GET_ACCESSOR( int, nNum );
	GET_ACCESSOR( int, nMax );
	// 비어있는 노드를 할당 받는다.
	T* NewNode( void ) {
		T *pNewNode = GetEmptyNode();
		if( pNewNode )	{
			m_pUsedPool[ m_idxLastEmpty ].active = 1;	// 노드 "사용됨"으로 바꿈
			m_idxLastEmpty = m_pUsedPool[ m_idxLastEmpty ].idxNextEmpty;	// 비어있는 다음 노드를 가리킴
			++m_nNum;		// 노드 개수 증가
		} else
			XLOG( "풀이 꽉찼습니다" );
		return pNewNode;
	}

	void Delete( int idx ) {		// m_pPool의 인덱스
		m_pUsedPool[ idx ].active = 0;	// 풀 반납
		m_pUsedPool[ idx ].idxNextEmpty = m_idxLastEmpty;		// 현재 비어있는 노드를 다음 노드로 받음
		m_idxLastEmpty = idx;		// 현재 비어있는 노드를 방금삭제한 노드로 설정
		--m_nNum;
	}
	void Delete( const T* pNode ) { Delete( FindIdx(pNode) ); }

	// pNode와 같은 데이타를 찾아 그 인덱스를 리턴한다.
	int	FindIdx( const T* pNode )	{
		T* pPool = (T*)m_pPool;
		int idx = pNode - &pPool[0];		// 개 뽀록. 포인터빼기 기술 작렬
		if( idx >= 0 && idx < m_nMax )		// 포인터가 범위내에 있는지
			return idx;
		XBREAK(1);
		return -1;
	}
}; // class XPool

template<typename T>
class XMemPool
{
public:
	static XPool<T>* s_pPool;
	XMemPool() {}
	virtual ~XMemPool() {}

//#ifdef DEBUG_NEW
//	void* operator new( size_t size, LPCSTR lpszFilename, int nLine )	{
//#else
	void* operator new( size_t size )	{
//#endif
		XBREAK( s_pPool == NULL );
		T *pNode = s_pPool->NewNode();
		XBREAK( pNode == NULL );
		return pNode;
	}
	
	void operator delete( void *p ) {
		if( s_pPool )
			s_pPool->Delete( (T*)p );
	}  
/*	void operator delete( void *p ) {
		if( s_pPool )
			s_pPool->Delete( (T*)p );
	} */
};

template<typename T>
class XStackStatic
{
	T *m_pStack;
	int m_offset;		// 스택 위치
	int m_nMax;		// 스택 최대 크기
public:
	XStackStatic() {
		m_pStack = NULL;
		m_offset = 0;
	}
	XStackStatic( int nMax ) {
		Create();
	}
	~XStackStatic() {
		SAFE_FREE( m_pStack );
	}
	void Create( int nMax ) {
		m_offset = 0;
		m_pStack = (T*)malloc( sizeof(T) * nMax );		// new가 오버로딩된상태에서 스택이 쓰여질수 있으므로 malloc으로 함
		memset( m_pStack, 0, sizeof(T) * nMax );
		m_nMax = nMax;
	}
	
	void Push( T obj ) {
		m_pStack[ m_offset++ ] = obj;
	}
	T Pop( void ) {
		return m_pStack[ --m_offset ];
	}
	T Top( void ) {
		return m_pStack[ m_offset-1 ];
	}
};

class XHugePool
{
	BOOL m_bActive;
	DWORD *m_pPool;		// 거대 메모리 풀
	DWORD *m_pCurr;		// 현재 할당된 풀 위치
	int m_nMax;				// 메모리풀 전체 크기
	int m_nUsed;				// 사용한 메모리 양
	struct STACK {
		ID idMark;
		DWORD *ptr;
	};
	XStackStatic<STACK> m_Stack;
	void Init() {
		m_bActive = FALSE;		// 처음엔 풀로 할당안함
		m_pPool = NULL;
		m_pCurr = NULL;
		m_nMax = 0;
		m_nUsed = 0;
	}
	void Destroy() {
		SAFE_FREE( m_pPool );
	}
public:
	// nSize는 4바이트정렬된 숫자여야 한다.
	XHugePool( int n4byteAlignedSize ) { 
#ifdef __HUGE_POOL
		Init(); 
		XBREAK( (n4byteAlignedSize % 4) != 0 );
		m_pPool = (DWORD *)malloc( n4byteAlignedSize );	// 거대 메모리풀을 할당
		m_pCurr = m_pPool;
		m_nMax = n4byteAlignedSize;
		m_Stack.Create( 100 );
#else
		return;
#endif
	}
	virtual ~XHugePool() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bActive );
	GET_ACCESSOR( int, nUsed );

	// 현재 풀 위치를 마킹함
	void AllocMark( ID idMark ) {		
#ifdef __HUGE_POOL
		STACK st;
		st.idMark = idMark;
		st.ptr = m_pCurr;
		m_Stack.Push( st );
#else
		return;
#endif
	}
	// 마킹해뒀던 곳으로 복귀함
	void RestoreMark( ID idMark ) {
#ifdef __HUGE_POOL
		STACK st = m_Stack.Top();
		XBREAK( st.idMark != idMark );
		m_Stack.Pop();
//		DWORD offset = (DWORD)m_pCurr - (DWORD)m_pPool;
//		m_pCurr = st.ptr + ((offset/4)/2) * 4;				
		m_pCurr = st.ptr;		
		m_nUsed = (DWORD)m_pCurr - (DWORD)m_pPool;
#else
		return;
#endif
	}
	void* Alloc( size_t size ) {
#ifdef __HUGE_POOL
		int num = (size / 4);	// 4바이트 블럭 몇개를 할당해야하는지 계산
		if( (size % 4) != 0 )		// 4바이트로 딱떨어지지 않으면 1블럭 더 추가
			num ++;
		// 메모리 시작 어드레스를 넘겨줌
		XBREAK( m_pCurr + size >= m_pPool + m_nMax );
		DWORD *pAlloc = m_pCurr;
		m_pCurr += num;
		m_nUsed += num * 4;		// 오프셋 증가
		return (void *)pAlloc;
#else
		return NULL;
#endif
	}
};

#endif // XPOOL