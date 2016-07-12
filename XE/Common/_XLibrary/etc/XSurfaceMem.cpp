#include "stdafx.h"
#if 0
#include "XSurfaceMem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// this서피스메모리의 dstx, dsty 지점을 좌상귀로 하는 지점에 pSrc의 srcw * srch 크기의 메모리를 카피한다.
void XSurfaceMem::CopyRect( int dstx, int dsty, DWORD *pSrc, int srcw, int srch )
{
	DWORD *pMem;
	int wPage, hPage;
	pMem = GetSrcImg( &wPage, &hPage );
	XBREAK( pMem == NULL );
	// 에러검출
	XBREAK( pSrc == NULL );
	XBREAK( srcw <= 0 );
	XBREAK( srch <= 0 );
	XBREAK( srcw > wPage );
	XBREAK( srch > hPage );
	XBREAK( dstx < 0 );
	XBREAK( dsty < 0 );
	XBREAK( dstx + srcw > wPage );
	XBREAK( dsty + srch > hPage );

	//
	DWORD *p = pMem + dsty * wPage + dstx;		// dst위치의 좌상귀 어드레스
	int xSkip = wPage - srcw;
	for( int i = 0; i < srch; ++i )
	{
		for( int j = 0; j < srcw; ++j )
		{
			*p++ = *pSrc++;
		}
		p += xSkip;
	}
}
#endif // 0