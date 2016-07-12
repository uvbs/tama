#pragma once

template<typename T, int N>
class XArrayLinearN;
// 리니어하게 데이타가 들어있는 배열의 템플릿.
template<typename T>
class XArrayLinear
{
	void Init() {
		m_nNum = m_nMax = 0;
		m_pArray = NULL;
#ifdef _DEBUG
//		XCLEAR_ARRAY( m_debugArray );	// T가 클래스일수도 있으므로 이런식으로 쓰지 말것.
#endif
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pArray );	
		/*
		음.... delete[]로 지우는게 맞다. 내부가 안지워지길 원한다면 애초에 <클래스*>형으로 넣었어야 했다.
		delete로 지운다고 해도 여전히 파괴자 호출하더라.
		*/
	}
protected:
	T *m_pArray;			// 배열
#ifdef DEBUG_ARRAY
	T *m_ppDebugArray[ 1024 ];		// 디버깅용.
#endif
	int m_nNum;			// 배열 크기
	int m_nMax;			// 배열 최대크기
public:	
	XArrayLinear() { Init(); }
	virtual ~XArrayLinear() { Destroy(); }
	// 배열을 최초 생성한다.
	void Create( int max );
	void DestroyAll( void ) {
		Destroy();
		Init();
	}
	void Clear( void ) {
//		Destroy();		// 어레이 껍데기만 날림.
//		Init();
		m_nNum = 0;
	}
	void resize( int maxNew );
	int GetNum( void ) const { return m_nNum; }
	int GetMaxSize( void ) const { return m_nMax; }
	int size( void ) const { return m_nNum; }
	bool IsFull() const {
		return m_nNum >= m_nMax;
	}

//	T* operator&( void ) const {		
//		return m_pArray;
		// XArrayLinear<> *pAry = &ary;		같은식으로 쓰기위해서 삭제함
//	}
	T* GetStartPtr( void ) {
		return m_pArray;
	}
	void operator=( XArrayLinear<T>& ary ) {
		Destroy();
		Init();
		if( ary.size() == 0 )
			return;
		Create( ary.size() );
		int num = ary.size();
		for( int i = 0; i < num; ++i ) {
			Add( ary[i] );
		}
	}
	void operator += ( XArrayLinear<T>& aryRhs ) {
		XBREAK( m_pArray == NULL );
		int numRhs = aryRhs.GetNum();
		int sumMax = m_nNum + numRhs;
		if( sumMax > m_nMax )
			resize( sumMax );
		for (int i = 0; i < aryRhs.GetMaxSize(); ++i)
			Add( aryRhs[i] );
	}
	template<int N>
	void operator += ( XArrayLinearN<T, N>& aryRhs );

	template<int N>
	void operator=( XArrayLinearN<T, N>& ary );
	//XArrayLinearN을 앞에 선언할수 없어서 뺌
/*
	template<int N>
	void operator=( XArrayLinearN<T, N>& ary ) {
		Destroy();
		Init();
		Create( ary.GetMaxSize() );
		int num = ary.GetNum();
		for( int i = 0; i < num; ++i ) {
			Add( ary[i] );
		}
	} 
*/
	// idx번째의 배열요소를 꺼낸다.
	// ArrayLinear계열은 오직 Readonly만 하는걸로 정책을 결정했다. const빼지말것.
	// Read/Write를 하고 싶으면 XArray나 XArrayN을 쓸것.
	// T.foo()를 할때 void foo const {} 로 반드시 할것.
	const T& operator [] ( int idx ) const {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nMax );
		XBREAK( idx < 0 );
		return m_pArray[idx];
//		return Get( idx );
	}
	const T& Get( int idx ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		XBREAK( idx < 0 );
		return m_pArray[idx];
	}
	const T* GetPtr( int idx ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		XBREAK( idx < 0 );
		return &m_pArray[idx];
	}
	// 배열의 가장 뒤에 있는 요소를 꺼낸다.
	const T& GetTail( void ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum <= 0 );
		return m_pArray[ m_nNum-1 ];
	}
	const T& GetFromRandom( void ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum <= 0 );
		return m_pArray[ random(m_nNum) ];
	}
	void DebugArrayCopy( void )	{
#ifdef DEBUG_ARRAY
		int num = (m_nMax>1024)? 1024:m_nMax;
		for( int i = 0; i < num; ++i )
			m_ppDebugArray[i] = &m_pArray[i];	// T가 클래스일수도 있으므로 memcpy쓰면 안됨.
#endif
	}
	// idx번째의 배열에 값을 덮어씌운다.
	// 리니어한 배열전용이므로 이것은 사용하지 말것. 모두 Add로 추가시킬것
/*	void Set( int idx, const T& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nNum );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	} */
	void Clear( int idx, const T& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( idx >= m_nMax );
		m_pArray[ idx ] = elem;
		DebugArrayCopy();
	}
	// elem을 배열에 추가시킨다.
	int Add( const T& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum >= m_nMax );
		m_pArray[ m_nNum ] = elem;
		DebugArrayCopy();
		return m_nNum++;
	}
	int Add( T& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum >= m_nMax );
		m_pArray[ m_nNum ] = elem;
		DebugArrayCopy();
		return m_nNum++;
	}
	int Add( T&& elem ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum >= m_nMax );
		m_pArray[ m_nNum ] = elem;
		DebugArrayCopy();
		return m_nNum++;
	}
	// 배열 맨 뒤를 삭제
	void DelTail( void ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum <= 0 );
		--m_nNum;
	}
	// 배열의 크기를 바꾼다.
	// 리사이즈가 되면 m_pArray의 어드레스를 다른곳에서 참조하는경우 크래쉬 되는 위험이 있다.
	void Resize( int newSize );
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int Find( T e ) {
		XBREAK( m_pArray == NULL );
		XBREAK( m_nNum < 0 );
		if( m_nNum == 0 )
			return -1;
		T *p = m_pArray;
		for( int i = 0; i < m_nNum; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
	inline int FindIdx( T e ) {
		return Find( e );
	}
// 	template<int _Size>
// 	inline void MemCopy( T (&dst)[_Size] ) {
// 		MemCopy( dst, _Size );
// 	}
// 	void MemCopy( T* dst, size_t lenDst ) {
// 		XBREAK( lenDst < m_nNum );
// 		memcpy_s( dst, lenDst * sizeof(T), m_pArray, sizeof(T) * m_nNum );
// 	}
/*	template<typename T, int N>
	void MemCopy( XArrayN<T, N> *pDstAry ) {
		memcpy_s( pDstAry->GetStartPtr(), N, m_pArray, sizeof(T) * m_nNum );
		pDstAry->m_nNum = m_nNum;
	} */
/*	소트함수 예제(높은순서배열)
	int XCard::compLevel( const void *p1, const void *p2 )
	{
		XCard **ppCard1 = (XCard **) p1;
		XCard **ppCard2 = (XCard **) p2;
		int level1 = (*ppCard1)->GetLevel();
		int level2 = (*ppCard2)->GetLevel();
		return level2 - level1;
	} */
	/**
	 @brief T는 비포인터형인 버전
	*/
// 	void CopyClone( const XArrayLinear<T>& arySrc ) {
// 		XARRAYLINEAR_LOOP_AUTO( arySrc, const T& nodeSrc  ) {
// 			Add( nodeSrc );
// 		} END_LOOP;
// 	}
	void sort( int (*compFunc)( const void *p1, const void *p2 ) ) {
		qsort( m_pArray, m_nNum, sizeof(T), compFunc );
	}
}; // XArrayLinear

#include "ArrayLinear.inl"
