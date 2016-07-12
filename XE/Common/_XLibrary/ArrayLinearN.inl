#include "ArrayLinear.h"


// 이거 사용하지 말것. T가 객체인경우 작살남.
// template<typename T, int N>
// void XArrayLinearN<T,N>::_MemCopyLinear( XArrayLinear<T>& arySrc ) {
// 	XBREAK( N < arySrc.GetNum() );
// 	memcpy_s( m_ppList, sizeof(T) * N, arySrc.GetStartPtr(), sizeof(T) * arySrc.GetNum() );
// 	m_nNum = arySrc.GetNum();
// }

template<typename T, int N>
void XArrayLinearN<T,N>::operator=( XArrayLinear<T>& arySrc ) 
{
	XBREAK( N < arySrc.GetNum() );
	Clear();
	int size = arySrc.size();
	for( int i = 0; i < size; ++i )
		Add( arySrc[i] );
}	



template<typename T, int N>
void XArrayLinearN<T,N>::operator=( XArray<T>& ary )
{
	int size = ary.GetMax();
	XBREAK( size > N );
	Clear();
	for( int i = 0; i < size; ++i )
	{
		Add( ary[i] );
	}
}

template<typename T, int N>
void XArrayLinearN<T,N>::operator=( XArrayLinearN<T,N>& ary ) 
{
	int num = ary.size();
	for( int i = 0; i < num; ++i ) {
		Add( ary[i] );
	}
}	

template<typename T, int N>
void XArrayLinearN<T, N>::operator=( XList4<T>& listRhs )
{
	XBREAK( GetMax() < (int)listRhs.size() );
	for( T elem : listRhs )
		Add( elem );
}

template<typename T, int N>
void XArrayLinearN<T,N>::Add( XArrayLinearN<T,N>& addAry )
{
	int num = addAry.size();
	for( int i = 0; i < num; ++i ) {
		Add( addAry[ i ] );
	}
}

template<typename T, int N>
void XArrayLinearN<T,N>::Add( XArrayLinear<T>& addAry )
{
	int num = addAry.size();
	for( int i = 0; i < num; ++i ) {
		Add( addAry[ i ] );
	}
}

template<typename T, int N>
void XArrayLinearN<T,N>::Add( XArray<T>& addAry )
{
	int num = addAry.size();
	for( int i = 0; i < num; ++i ) {
		Add( addAry[ i ] );
	}
}

template<typename T, int N>
void XArrayLinearN<T,N>::Add( XArrayN<T, N>& addAry )
{
	int num = addAry.size();
	for( int i = 0; i < num; ++i ) {
		Add( addAry[ i ] );
	}
}

template<typename T, int N>
void XArrayLinearN<T,N>::Add( XList4<T>& addList )
{
	for( auto& elem : addList ) {
		Add( elem );
	}
}

