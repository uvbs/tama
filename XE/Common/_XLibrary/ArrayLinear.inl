#include "ArrayLinearN.h"

/*
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
*/

template<typename T>
void XArrayLinear<T>::Create( int max ) 
{
	XBREAK( m_pArray != NULL );
	XBREAK( max <= 0 );
	m_pArray = new T[ max ];		// 배열 최대크기만큼 할당.
	m_nNum = 0;
	m_nMax = max;
}

template<typename T>
void XArrayLinear<T>::resize( int maxNew )
{
	XBREAK( maxNew <= m_nMax );		// 기존크기보다 작을순 없다.
	T *pArray = new T[ maxNew ];

	for( int i = 0; i < m_nNum; ++i )
	{
		pArray[i] = m_pArray[i];		// 기존데이타를 옮김.
	}
	SAFE_DELETE_ARRAY( m_pArray );		// 기존버퍼 삭제.
	m_pArray = pArray;
	m_nMax = maxNew;
}


template<typename T>
template<int N>
void XArrayLinear<T>::operator=( XArrayLinearN<T, N>& ary ) 
{
	XBREAKF( m_pArray != NULL, "XArrayLinear is not empty" );
	Init();
	if( ary.size() == 0 )
		return;
	Create( ary.size() );
	int num = ary.size();
	for( int i = 0; i < num; ++i ) {
		Add( ary[i] );
	}
} 

template<typename T>
template<int N>
void XArrayLinear<T>::operator += ( XArrayLinearN<T, N>& aryRhs ) 
{
	if( m_pArray == nullptr ) 
	{
		Init();
		if( aryRhs.size() == 0 )
			return;
		Create( aryRhs.size() );
	}
	int numRhs = aryRhs.size();
	int sumMax = m_nNum + numRhs;
	if( sumMax > m_nMax )
		resize( sumMax );
	for( int i = 0; i < numRhs; ++i )
		Add( aryRhs[ i ] );
}
