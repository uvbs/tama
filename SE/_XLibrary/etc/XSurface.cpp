#include "stdafx.h"
#include "XSurface.h"

DWORD XSurface::s_dwMaxSurfaceWidth = 0;
int XSurface::s_sizeTotalVMem = 0;


XE_NAMESPACE_START( XE )
//
LPCTSTR GetstrEnum( XE::xtPixelFormat pixelFormat )
{
	switch( pixelFormat )
	{
	case XE::xPF_ARGB8888:	return _T("xPF_ARGB8888");
	case XE::xPF_ARGB4444:	return _T("xPF_ARGB4444");
	case XE::xPF_ARGB1555:	return _T("xPF_ARGB1555");
	case XE::xPF_RGB565:	return _T("xPF_RGB565");
	case XE::xPF_RGB555:	return _T("xPF_RGB555");
	default:
		XBREAK(1);
		return _T("");
		break;
	}
	return _T("");
}
//
XE_NAMESPACE_END; // XE


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// __pSrcImg의 처리들.
// lx,ly: 서피스크기 기준 로컬좌표
DWORD XSurface::GetPixel( float lx, float ly, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) 
{
	int w, h;
	DWORD *pImg = GetSrcImg( &w, &h );
	if( XBREAK( pImg == nullptr ) )
		return 0;
	lx -= GetAdjustX();			// 버텍스좌표는 adj가 적용되어있으므로 그것을 원점기준으로 이동시킴
	ly -= GetAdjustY();			// 이미지 좌상귀 기준좌표로 변환
	if( lx < 0 || lx >= GetWidth() )		// 영역밖을 벗어나면 실패로 0리턴
		return 0;
	if( ly < 0 || ly >= GetHeight() )
		return 0;
	int memlx = ConvertToMemSize( lx );
	int memly = ConvertToMemSize( ly );
	XBREAK( memlx >= w );
	XBREAK( memly >= h );
	XBREAKF( memlx < 0, "memlx(%d) < 0, lx=%f, ly=%f", memlx, lx, ly );
	XBREAKF( memly < 0, "memly(%d) < 0, lx=%f, ly=%f", memlx, lx, ly );
	DWORD pixel = pImg[ memly * w + memlx ];
	if( pa ) *pa = XCOLOR_RGB_A( pixel );
	if( pr ) *pr =  XCOLOR_RGB_R( pixel );
	if( pg ) *pg =  XCOLOR_RGB_G( pixel );
	if( pb ) *pb =  XCOLOR_RGB_B( pixel );
	return pixel;
}

// 서피스의 xSrc,ySrc - wDst,hDst 영역의 픽셀을 읽어 pDst에 카피한다.
void XSurface::CopyRectTo( DWORD *pDst, int wDst, int hDst, int xSrc, int ySrc )
{
	XBREAK( pDst == nullptr );
	int w, h;
	DWORD *pImg = GetSrcImg( &w, &h );
	XBREAK( pImg == nullptr );
	XBREAK( w <= 0 );
	XBREAK( h <= 0 );
//	XBREAK( xSrc < 0 );
//	XBREAK( ySrc < 0 );
	XBREAK( wDst <= 0 );
	XBREAK( hDst <= 0 );
//	XBREAK( xSrc+wDst > GetMemWidth() );
//	XBREAK( ySrc+hDst > GetMemHeight() );
	//
	DWORD *p = pDst;
	for( int i = ySrc; i < ySrc+hDst; ++i )
	{
		for( int j = xSrc; j < xSrc+wDst; ++j )
		{
			if( j >= 0 && j < w && i >= 0 && i < h )
				*p++ = pImg[ i * w + j ];
			else
				*p++ = 0;
		}
	}
}

// __pSrcImg의 처리들 끝
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
