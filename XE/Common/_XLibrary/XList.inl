#include "XList.h"

template<typename T>
void XList2<T>::Create( int nMax )
{
	XBREAK( nMax == 0 );
	m_nMax = nMax;
	XBREAK( m_ppList != NULL );
	XBREAK( m_pIdxNextEmpty != NULL );
	m_ppList = new T*[ nMax ];
	m_pIdxNextEmpty = new int[ nMax ];
	memset( m_ppList, 0, sizeof( T* ) * nMax );
	// 비어있는 다음 노드를 세1팅
	int i;
	for( i = 0; i < nMax - 1; i++ )
		m_pIdxNextEmpty[ i ] = i + 1;
	m_pIdxNextEmpty[ i ] = -1;		// 가장 끝은 '뒤가'없다
}

template<typename T>
void XQueue2<T>::Create( int nMax ) {
	m_nMax = nMax;
	m_ppList = new T*[ nMax ];
	memset( m_ppList, 0, sizeof( T* ) * nMax );
}

template<typename T>
void XArray<T>::Create( int max )
{
	XBREAK( m_pArray );
	XBREAK( max <= 0 );
	m_pArray = new T[ max ];		// 배열 최대크기만큼 할당.
	m_nMax = max;
}

template<typename T>
void XArray<T>::Resize( int newSize ) {
	XBREAK( m_pArray == NULL );
	XBREAK( newSize <= 0 );
	XBREAK( newSize <= m_nMax );
	T *pNewArray = new T[ newSize ];
	// 새 버퍼로 옮김.
	int Min = xmin( newSize, m_nMax );
	for( int i = 0; i < Min; ++i )
		pNewArray[ i ] = m_pArray[ i ];
	SAFE_DELETE_ARRAY( m_pArray );
	m_pArray = pNewArray;
	m_nMax = newSize;
}

template<typename T>
void XList3<T>::Create( int nMax ) {
	XBREAK( nMax == 0 );
	m_nMax = nMax;
	XBREAK( m_aryNodes != NULL );
	m_aryNodes = new xNode[ nMax ];
	clear();
}

template<typename T>
template<int N>
void XList<T>::operator = ( XArrayLinearN<T, N>& ary )
{
	Clear();
	int num = ary.size();
	for( int i = 0; i < num; ++i ) {
		Add( ary[ i ] );
	}
}

