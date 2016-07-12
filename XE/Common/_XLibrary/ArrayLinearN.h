#pragma once

template<typename T>
class XArray;
template<typename T, int N>
class XArrayN;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// N개 이하의 아이템을 배열리스트로 관리해주는 클래스. 삭제기능없음, 데이타가 리니어하게 들어가있는것을 보장함.
// 초기 배열크기는 N으로 지정한다. local용 임시배열로 쓰기 좋다.
template<typename T, int N>
class XArrayLinearN
{
	ID m_snArray;		// 디버깅용 시리얼 번호
	int m_nNum;
protected:
public:
	T m_ppList[ N ];
	XArrayLinearN() {
		// 가볍고 빠른게 목적이므로 속도를 위해서 list초기화 하지 않았음
		m_nNum = 0;
		m_snArray = XE::GenerateID();
//		XTRACE( "0x%08x: XArrayLinearN<size(%d), %d>", m_snArray, sizeof(T), N );
	}
	XArrayLinearN( std::initializer_list<T> il ) {
		m_nNum = 0;
		m_snArray = XE::GenerateID();
		for( auto& elem : il ) {
			Add( elem );
		}
	}
	virtual ~XArrayLinearN() {}
	// idx번째의 배열요소를 꺼낸다.
//	const T& operator [] ( int idx ) const {
	// 여러모로 불편해서 const뺌
	T& operator [] ( int idx ) {	
		XBREAK( idx >= m_nNum || idx < 0 );
		return m_ppList[idx];
	}
	T operator [] ( const int idx ) const {
		XBREAK( idx >= m_nNum || idx < 0 );
		return m_ppList[ idx ];
	}
	void operator=( std::initializer_list<T> il ) {
		Clear();
		for( auto& elem : il ) {
			Add( elem );
		}
	}
	void operator=( XArrayLinear<T>& arySrc );
	void operator=( XArrayLinearN<T,N>& ary );
	void operator=( XArray<T>& ary );
	void operator=( XList4<T>& listRhs );
	GET_ACCESSOR_CONST( int, nNum );
	int GetMax( void ) const { 
		return N; 
	}
	int size( void ) const { 
		return m_nNum; 
	}
	T* GetStartPtr( void ) const {
		return m_ppList;
	}
	const T& At( int idx ) const {
		if( XBREAK( idx < 0 || idx >= N ) )
			return m_ppList[0];
		return m_ppList[ idx ];
	}
	void Clear( void ) {
		m_nNum = 0;
	}
	void Clear( T& val ) {
		int max = N;
		m_nNum = 0;
		for( int i = 0; i < max; ++i ) {
			m_ppList[ i ] = val;
		}
	} 
	void Fill( T& val ) {
		for( int i = 0; i < N; ++i ) {
			m_ppList[ i ] = val;
		}
	}
	void Fill( T&& val ) {
		m_nNum = N;
		for( int i = 0; i < N; ++i ) {
			m_ppList[ i ] = val;
		}
	}
	inline void push_back( T& pObj ) { Add( pObj ); }
	void Add( const T& pObj ) {
		if( XBREAKF( m_nNum >= N, "ID=0x%08x: m_nNum(%d) < N(%d)", m_snArray, m_nNum, N ) )
			return;
		m_ppList[ m_nNum++ ] = pObj;
	}
	void Add( T& pObj ) {
		if( XBREAKF( m_nNum >= N, "ID=0x%08x: m_nNum(%d) < N(%d)", m_snArray, m_nNum, N ) )
			return;
		m_ppList[ m_nNum++ ] = pObj;
	}
	void Add( T&& pObj ) {
		if( XBREAKF( m_nNum >= N, "ID=0x%08x: m_nNum(%d) < N(%d)", m_snArray, m_nNum, N ) )
			return;
		m_ppList[ m_nNum++ ] = pObj;
	}
	void Add( XArrayLinearN<T,N>& addAry );
	void Add( XArrayLinear<T>& addAry );
	void Add( XArray<T>& addAry );
	void Add( XArrayN<T, N>& addAry );
	void Add( XList4<T>& addList );
/*	소트함수 예제(높은순서배열)
	int XCard::compLevel( const void *p1, const void *p2 )
	{
		XCard **ppCard1 = (XCard **) p1;
		XCard **ppCard2 = (XCard **) p2;
		int level1 = (*ppCard1)->GetLevel();
		int level2 = (*ppCard2)->GetLevel();
		return level2 - level1;
	} */
	void sort( int (*compFunc)( const void *p1, const void *p2 ) ) {
		qsort( m_ppList, m_nNum, sizeof(T), compFunc );
	}
	// 데이타가 중간에 빈곳없이 linear하게 있다고 가정.
	T& GetFromRandom( void ) {
		XBREAK( m_nNum == 0 );
		int idxRnd = random( m_nNum );
		return m_ppList[ idxRnd ];
	}
	// 랜덤객체를 외부에서 받는 버전
	T& GetFromRandom( XRandom2& randObj ) const {
		XBREAK( m_nNum == 0 );
		int idxRnd = randObj.GetRandom( m_nNum );
		return m_ppList[ idxRnd ];
	}
	// e와 같은걸 찾아서 그 인덱스를 리턴한다.
	int FindLinear( T& e ) {
		T *p = m_ppList;
		int num = m_nNum;
		for( int i = 0; i < num; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
	int FindLinear( T&& e ) {
		T *p = m_ppList;
		int num = m_nNum;
		for( int i = 0; i < num; ++i ) {
			if( *p++ == e )
				return i;
		}
		return -1;
	}
	BOOL Find( T& e ) {
		if( FindLinear( e ) == -1 )
			return FALSE;
		return TRUE;
	}
	BOOL Find( T&& e ) {
		if( FindLinear( e ) == -1 )
			return FALSE;
		return TRUE;
	}
	//
// 	template<int N2>
// 	void _MemCopyLinear( XArrayLinearN<T, N2>& arySrc ) {
// 		XBREAK( N < arySrc.GetnNum() );
// 		memcpy_s( m_ppList, sizeof(T) * N, arySrc.GetStartPtr(), sizeof(T) * arySrc.GetnNum() );
// 		m_nNum = arySrc.GetnNum();
// 	}
// 	void _MemCopyLinear( XArrayLinear<T>& arySrc );
}; // class XArrayLinearN

#include "ArrayLinearN.inl"
