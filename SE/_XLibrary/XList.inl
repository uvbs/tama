#include "XList.h"

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

