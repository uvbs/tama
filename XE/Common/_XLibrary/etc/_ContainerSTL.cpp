#include "stdafx.h"
#include "ContainerSTL.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<typename T>
void XQueueArray<T>::Create( int size ) {
	XBREAK( m_Array );
	m_Array = new T[ size ];
	m_nMax = size;
	m_nNum = 0;
	m_idxFront = m_idxBack = 0;
}
