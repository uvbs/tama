#include "stdafx.h"
#include "XSurfaceMem.h"

// this���ǽ��޸��� dstx, dsty ������ �»�ͷ� �ϴ� ������ pSrc�� srcw * srch ũ���� �޸𸮸� ī���Ѵ�.
void XSurfaceMem::CopyRect( int dstx, int dsty, DWORD *pSrc, int srcw, int srch )
{
	DWORD *pMem;
	int wPage, hPage;
	pMem = GetSrcImg( &wPage, &hPage );
	XBREAK( pMem == NULL );
	// ��������
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
	DWORD *p = pMem + dsty * wPage + dstx;		// dst��ġ�� �»�� ��巹��
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
