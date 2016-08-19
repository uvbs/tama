#include "stdafx.h"
#include "XPacket.h"

void XPacket::WriteString( LPCTSTR szStr ) 
{
    int leftsize = (int)m_pEnd - (int)m_pCurr;
    int len = _tcslen( szStr ) + 1;
    int size = len * sizeof(TCHAR);		// ???�함
    // 4byte ?�렬
    if( size & 0x03 )                // ?�위??찌끄?�기 비트가 ?�아?�으�?
        size = (size & ~(0x03)) + 4;		// 비트 ?�삭?�고 4바이?????�해�?
    XBREAK( (int)m_pCurr & 0x03 );		// 4byte?�렬 검??
    XBREAK( size > 0xffff );		// ?�단 64k?�상???�트링�? 못보?�는걸로 ?�자.
    *((WORD*)m_pCurr) = (WORD)size;		// ?�트링의 메모리사?�즈(???�함)
#ifdef _VER_IOS
    *((WORD*)(m_pCurr+2)) = (WORD)2;		// ?�스???�코??UTF8)
#else
    *((WORD*)(m_pCurr+2)) = (WORD)1;		// ?�스???�코??UTF16)
#endif
    m_pCurr += sizeof(int);		// ?�트링패?�의 맨앞 ?�이�?부분을 건너?�
    memcpy_s( (void*)m_pCurr, leftsize, (void*)szStr, len * sizeof(TCHAR) );	// ?�트�?길이부분만 ?�확?�게 메모�?카피??
    m_pCurr += size;	// ?�트�??�이�??�렬?? 건너?�
    AddPacketLength( size + 4 );		// �??�킷길이???�함.
}
