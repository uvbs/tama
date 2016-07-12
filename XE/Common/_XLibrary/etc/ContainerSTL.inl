#include "ContainerSTL.h"

template<typename T>
void XQueueArray<T>::Create( int size ) {
	XBREAK( m_Array );
	m_Array = new T[ size ];
	m_nMax = size;
	m_nNum = 0;
	m_idxFront = m_idxBack = 0;
}
