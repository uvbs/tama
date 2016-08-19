/*
 *  XGraphics.cpp
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "XGraphics.h"

XGraphics* XGraphics::s_pGraphics = nullptr;
BOOL		XGraphics::s_bCaptureBackBuffer = FALSE;			// Present전에 현재 백버퍼를 카피해둬야 함
XSurface* XGraphics::s_pLastBackBuffer = nullptr;			// 마지막으로 캡쳐된 백버퍼 화면

#pragma mark Init

void XGraphics::Init( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
{
	Init();
	m_nWidth = nResolutionWidth;
	m_nHeight = nResolutionHeight;
	m_pixelFormat = pixelFormat;
	// 뷰포트와 백버퍼의 크기는 기본적으로 스크린사이즈가 같게 설정된다.
	m_nViewportLeft = m_nViewportTop = 0;		// 클리핑영역.(앞으로 RECT형태로 바껴야 한다.)
	m_nViewportRight = m_nWidth;
	m_nViewportBottom = m_nHeight;		// 
	m_nBackBufferWidth = m_nWidth;
	m_nBackBufferHeight = m_nHeight;
	m_nWorkBufferWidth = m_nWidth;
	m_nWorkBufferHeight = m_nHeight;
	SetPhyScreenSize( m_nWidth, m_nHeight );
//	m_nPhyScreenWidth = m_nWidth;
//	m_nPhyScreenHeight = m_nHeight;
	ClearScreen( XCOLOR_BLACK );
}

#pragma mark Convert pixel
// 555포맷의 rgb값을 565로 바꾼다.
WORD XGraphics::ConvertRGB565( WORD rgb ) const
{
    WORD r = ( rgb & 0x7c00 ) >> 10 ;   //  0111 1100 0000 0000
	WORD g = ( rgb & 0x03e0 ) >> 5 ;    //  0000 0011 1110 0000
	WORD b = ( rgb & 0x001f ) ;         //  0000 0000 0001 1111
	
    rgb = (r<<11) | (g<<6) | b;
	
	return  rgb;
}

// 565포맷의 rgb값을 555로 바꾼다
WORD XGraphics::ConvertRGB555( WORD rgb ) const
{
    WORD r = ( rgb & 0xf800 ) >> 11;	//  1111 1000 0000 0000
	WORD g = ( rgb & 0x07e0 ) >> 6;		//  0000 0111 1110 0000
	WORD b = ( rgb & 0x001f );			//  0000 0000 0001 1111
	
    rgb = (r<<10) | (g<<5) | b;
	
	return  rgb;
}

// 555포맷의 rgb값을 8888로 바꾼다
DWORD XGraphics::Convert555ToRGBA8( WORD rgb, BYTE a ) const 
{
	/* TODO: 쉬프트 방식 바꿀것 */
    DWORD r = ( rgb & 0x7c00 ) >> 10 ;   //  0111 1100 0000 0000
	DWORD g = ( rgb & 0x03e0 ) >> 5 ;    //  0000 0011 1110 0000
	DWORD b = ( rgb & 0x001f ) ;         //  0000 0000 0001 1111
	
	r = r * 255 / 31;
	g = g * 255 / 31;
	b = b * 255 / 31;
	
	//    DWORD rgba = (r<<24) | (g<<16) | (b<<8) | (DWORD)a;
    DWORD rgba = ((DWORD)a<<24) | (b<<16) | (g<<8) | r;
	
	return  rgba;
}
// 565포맷의 rgb값을 8888로 바꾼다
DWORD XGraphics::Convert565ToRGBA8( WORD rgb, BYTE a ) const 
{
	
	DWORD r = (rgb >> 11) & 0x1f;	// 1111 1000 0000 0000
	DWORD g = (rgb >> 5) & 0x3f;	// 0000 0111 1110 0000
	DWORD b = rgb & 0x1f;			// 0000 0000 0001 1111
	
	r = r * 255 / 31;
	g = g * 255 / 63;
	b = b * 255 / 31;
	
	//    DWORD rgba = (r<<24) | (g<<16) | (b<<8) | (DWORD)a;
    DWORD rgba = ((DWORD)a<<24) | (b<<16) | (g<<8) | r;
	
	return  rgba;
}

#pragma mark Align Pow2
// 3D하드웨어의 경우 텍스쳐의 크기를 2^로 정렬해야한다. 그럴때 쓰는 범용 함수.
int	XGraphics::AlignPow2Width( int width )
{
	if( width <= 32 )	width = 32;		// 특정그래픽카드에서 텍스쳐 가로사이즈가 32이하가 되면 찌그러져 나오는 현상있어서 최소크기를 32로 잡음
	else if( width <= 64 )	width = 64;
	else if( width <= 128 )	width = 128;
	else if( width <= 256 )	width = 256;
	else if( width <= 512 )	width = 512;
	else if (width <= 1024 )	width = 1024;
	else if (width <= 2048 )	width = 2048;
#ifdef WIN32
	else if (width <= 4096 )	width = 4096;
#endif
	/* TODO: 시스템이 지원하는 텍스쳐크기에 따라서 유동적으로 바뀔수 있게 할것 */
	else 
		width = 0;
	return width;
}		

int	XGraphics::AlignPow2Height( int height )
{
	if( height <= 32 )		height = 32;		// 특정그래픽카드에서 텍스쳐 가로사이즈가 32이하가 되면 찌그러져 나오는 현상있어서 최소크기를 32로 잡음
	else if( height <= 64 )		height = 64;
	else if( height <= 128 )	height = 128;
	else if( height <= 256 )	height = 256;
	else if( height <= 512 )	height = 512;
	else if( height <= 1024 ) height = 1024;
	else if (height <= 2048 )	height = 2048;
#ifdef WIN32
	else if (height <= 4096 )	height = 4096;
#endif
	/* TODO: 시스템이 지원하는 텍스쳐크기에 따라서 유동적으로 바뀔수 있게 할것 */
	else 
		height = 0;
	return height;
}

int XGraphics::AlignPowSize( int *width, int *height )
{
	int w = *width;
	int h = *height;
	w = AlignPow2Width( w );
	if( w == 0 )
	{
		XERROR("Too big size texture %d %d", *width, *height);
		return 0;
	}
	
	h = AlignPow2Height( h );
	if( h == 0 )
	{
		XERROR("Too big size texture %d %d", *width, *height);
		return 0;
	}
	*width = w;
	*height = h;
	return 1;
}

BOOL Clipper2( int _xl, int _yt, int _xr, int _yb,  
						int *x, int *y, int *x2, int *y2,
						int *sx, int *sy, int *sx2, int *sy2 )
{
	int w = *x2 - *x + 1;
	int h = *y2 - *y + 1;
	int sw = *sx2 - *sx + 1;
	int sh = *sy2 - *sy + 1;
	BOOL retv = Clipper( _xl, _yt, _xr, _yb,  x, y, &w, &h,  sx, sy, &sw, &sh );
	if( retv )		return TRUE;
	*x2 = *x + w - 1;
	*y2 = *y + h - 1;
	*sx2 = *sx + sw - 1;
	*sy2 = *sy + sh - 1;
	return FALSE;
}
// XL, YT, XR, YB 클리핑 영역
// x, y, w, h 클리핑해야할 좌표
BOOL Clipper( int _xl, int _yt, int _xr, int _yb,  
					int *x, int *y, int *w, int *h,
					int *sx, int *sy, int *sw, int *sh )
{
	if ( *x > _xr || *y > _yb )		return TRUE;		// right/bottom out
	if ( *x + *w - 1 < _xl )		return TRUE;				// left out
	if( *y + *h - 1 < _yt )		return TRUE;				// top out

    if ( *x + *w > _xr + 1 )								// right cut
	{
		int sub = ((*x + *w) - (_xr + 1));
		*w -= sub;
		*sw -= sub;
	}
    if ( *x < _xl )
    {
		*sw -= (_xl - *x);									// left cut
		*sx += (_xl - *x);
        *w -= (_xl - *x);
        *x = _xl;
    }

    if ( *y + *h > _yb + 1 )		
	{
		int sub = ((*y + *h) - (_yb + 1));
		*h -= sub;						// bottom cut
		*sh -= sub;
	}
    if ( *y < _yt )
    {
		*sh -= (_yt - *y);									// top cut
		*sy += (_yt - *y);
        *h -= (_yt - *y);										
        *y = _yt;
    }
	return FALSE;
}

// 육각형의 경우:
// v1, v2, v3, v4, v5, v6, v7
BOOL XGraphics::ClippingLine( XE::VEC2 *pvOut, const XE::VEC2 *pvLines, int numLine, float x1, float y1, float x2, float y2 )
{
	for( int i = 0; i < numLine; i ++ )
	{
		if( XE::GetIntersectPoint( pvLines[i], 
								  pvLines[i+1], 
								  XE::VEC2(x1, y1), XE::VEC2(x2, y2), 
								  pvOut ) )
		{
			return TRUE;
		}
	}
	return FALSE;	
}
