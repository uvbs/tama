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
#include "xUtil.h"
#include "xMath.h"
#include "XImage.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGraphics* XGraphics::s_pGraphics = NULL;
BOOL		XGraphics::s_bCaptureBackBuffer = FALSE;			// Present전에 현재 백버퍼를 카피해둬야 함
XSurface* XGraphics::s_pLastBackBuffer = NULL;			// 마지막으로 캡쳐된 백버퍼 화면
DWORD XGraphics::s_dwDraw = 0;
//XSPLock XGraphics::s_spLock;

#pragma mark Init

//////////////////////////////////////////////////////////////////////////
XE_NAMESPACE_START( XE )

// 육각형의 경우:
// v1, v2, v3, v4, v5, v6, v7
BOOL ClippingLine( XE::VEC2 *pvOut, const XE::VEC2* pvLines
										, int numLine
										, float x1, float y1, float x2, float y2 )
{
	for( int i = 0; i < numLine; i++ ) {
		if( XE::GetIntersectPoint( pvLines[ i ],
																pvLines[ i + 1 ],
																XE::VEC2( x1, y1 ), XE::VEC2( x2, y2 ),
																pvOut ) ) {
			return TRUE;
		}
	}
	return FALSE;
}


//
// Integer version with 2^10(1024) resolution
//
// @param hsl_h : [out] long &, hue, 0 ~ 1024
// @param hsl_s : [out] long &, saturation, 0 ~ 1024
// @param hsl_l : [out] long &, lightness, 0 ~ 1024
void RGB2HSL_( XCOLOR clr, long & hsl_h, long & hsl_s, long & hsl_l )
{
	const float divrev3 = 1.0f / 3.0f;
	const long var_r = XCOLOR_RGB_R( clr ) << 2;
	const long var_g = XCOLOR_RGB_G( clr ) << 2;
	const long var_b = XCOLOR_RGB_B( clr ) << 2;
	const long var_min = std::min( var_r, std::min( var_g, var_b ) );
	const long var_max = std::max( var_r, std::max( var_g, var_b ) );
	const long del_max = var_max - var_min;
	hsl_l = ( var_max + var_min ) >> 1;
	if( del_max == 0 ) {
		hsl_h = 0;
		hsl_s = 0;
	}
	else {
		if( hsl_l < 512 ) {
			hsl_s = ( del_max << 10 ) / ( var_max + var_min );
		}
		else {
			hsl_s = ( del_max << 10 ) / ( 2048 - var_max - var_min );
		}
		const long temp = ( var_max << 9 ) / 3 / del_max + ( del_max << 9 ) / del_max;
// 		const long del_r = temp - ( ( var_r << 9 ) / 3 ) / del_max;
// 		const long del_g = temp - ( ( var_g << 9 ) / 3 ) / del_max;
// 		const long del_b = temp - ( ( var_b << 9 ) / 3 ) / del_max;
		const long del_r = temp - ( long(( var_r << 9 ) * divrev3) ) / del_max;
		const long del_g = temp - ( long(( var_g << 9 ) * divrev3 )) / del_max;
		const long del_b = temp - ( long(( var_b << 9 ) * divrev3) ) / del_max;
		if( var_r == var_max ) {
			hsl_h = del_b - del_g;
		}
		else
			if( var_g == var_max ) {
				hsl_h = ( 1024 / 3 ) + del_r - del_b;
			}
			else
				if( var_b == var_max ) {
					hsl_h = ( 2048 / 3 ) + del_g - del_r;
				}
		if( hsl_h < 0 ) {
			hsl_h += 1024;
		}
		else
			if( hsl_h > 1024 ) {
				hsl_h -= 1024;
			}
	}
}

long HUE2RGB_( long var_1, long var_2, long var_h )
{
	if( var_h < 0 ) {
		var_h += 1024;
	}
	else
		if( var_h > 1024 ) {
			var_h -= 1024;
		}
	const long temp1 = 3 * var_h;
	if( temp1 < 512 ) {
		return ( var_1 + ( ( ( 3 * var_2 * var_h ) >> 9 ) - ( ( 3 * var_1 * var_h ) >> 9 ) ) );
	}
	else
		if( var_h < 512 ) {
			return var_2;
		}
		else
			if( temp1 < 2048 ) {
				const long temp2 = 4096 - ( temp1 << 1 );
				return ( var_1 + ( ( var_2 * temp2 - var_1 * temp2 ) >> 10 ) );
			}

	return var_1;
}

//
// RGB to HSL conversion
//
// @param clr : [in] COLORREF, input RGB value to be converted
// @param hue : [out] double &, converted hue value (0 ~ 2*PI)
// @param saturation : [out] double &, converted saturation value (0%~ 100%)
// @param lightness : [out] double &, converted lightness value (0% ~ 100%)
void RGB2HSL( XCOLOR clr, double & hue, double & saturation, double & lightness )
{
	long hsl_h = 0, hsl_s = 0, hsl_l = 0;
	RGB2HSL_( clr, hsl_h, hsl_s, hsl_l );

	// long to double
	const double divrev = 1.0 / 1024.0;
	hue = 2.0 * 3.141592 * (double)hsl_h * divrev;
	saturation = (double)hsl_s * divrev;
	lightness = (double)hsl_l * divrev;
// 	hue = 2.0 * 3.141592 * (double)hsl_h / 1024.0;
// 	saturation = (double)hsl_s / 1024.0;
// 	lightness = (double)hsl_l / 1024.0;
}

//
// Integer version with 2^10(1024) resolution
//
// @param hsl_h : [in] long, hue, 0 ~ 1024
// @param hsl_s : [in] long, saturation, 0 ~ 1024
// @param hsl_l : [in] long, lightness, 0 ~ 1024
XCOLOR HSL2RGB_( long hsl_h, long hsl_s, long hsl_l )
{
	BYTE clrR = 0, clrG = 0, clrB = 0;
	long var_1 = 0, var_2 = 0;
	const long var_h = hsl_h;
	const long var_s = hsl_s;
	const long var_l = hsl_l;
	if( hsl_s == 0 ) {
		clrR = clrG = clrB = (BYTE)( var_l >> 2 );
	}
	else {
		if( var_l < 512 ) {
			var_2 = var_l + ( ( var_l * var_s ) >> 10 );
		}
		else {
			var_2 = var_l + var_s - ( ( var_l * var_s ) >> 10 );
		}
		var_1 = ( var_l << 1 ) - var_2;
		clrR = (BYTE)( HUE2RGB_( var_1, var_2, var_h + ( 1024 / 3 ) ) >> 2 );
		clrG = (BYTE)( HUE2RGB_( var_1, var_2, var_h ) >> 2 );
		clrB = (BYTE)( HUE2RGB_( var_1, var_2, var_h - ( 1024 / 3 ) ) >> 2 );
	}
	return XCOLOR_RGBA( clrR, clrG, clrB, 255 );
}

//
// Calculate complementary color 
//
// @param clr : [in] COLORREF, input RGB color from which complementary color is calculated
//
// @return RGB color value of complementary color of input RGB color
XCOLOR CalcComplemental( XCOLOR clr )
{
	long hsl_h = 0, hsl_s = 0, hsl_l = 0;
	RGB2HSL_( clr, hsl_h, hsl_s, hsl_l );   // CONV RGB to HSL
	hsl_h += 0x200;                       // HUE, ADD 180 deg (0x200 = 0x400 / 2)
	hsl_h &= 0x7ff;                       // HUE, MOD 360 deg to HUE
	return HSL2RGB_( hsl_h, hsl_s, hsl_l ); // CONV HSL to RGB
}


//
// HSL to RGB conversion
//
// @param hue : [in] double, input hue value (0 ~ 2*PI)
// @param saturation : [in] double, input saturation value (0% ~ 100%)
// @param lightnelss : [in] double, input lightness value (0% ~ 100%)
//
// @return converted RGB color value from the input HSL value
XCOLOR HSL2RGB( double hue, double saturation, double lightness )
{
	// double to long
	const long hsl_h = (long)( hue / 2.0 / 3.141592 * 1024.0 );
	const long hsl_s = (long)( saturation * 1024.0 );
	const long hsl_l = (long)( lightness * 1024.0 );

	return HSL2RGB_( hsl_h, hsl_s, hsl_l );
}

XE_NAMESPACE_END;

//////////////////////////////////////////////////////////////////////////
void XGraphics::Init( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
{
	Init();
	m_nWidth = nResolutionWidth;
	m_nHeight = nResolutionHeight;
	m_pixelFormat = pixelFormat;
	// 뷰포트와 백버퍼의 크기는 기본적으로 스크린사이즈가 같게 설정된다.
	m_nViewportLeft = m_nViewportTop = 0;		// 클리핑영역.(앞으로 RECT형태로 바껴야 한다.)
	m_nViewportRight = m_nWidth - 1;
	m_nViewportBottom = m_nHeight - 1;		// 
	m_nBackBufferWidth = m_nWidth;
	m_nBackBufferHeight = m_nHeight - 1;
	m_nWorkBufferWidth = m_nWidth;
	m_nWorkBufferHeight = m_nHeight;
	SetPhyScreenSize( m_nWidth, m_nHeight );
//	m_nPhyScreenWidth = m_nWidth;
//	m_nPhyScreenHeight = m_nHeight;
	ClearScreen( XCOLOR_BLACK );
//	XGraphics::s_spLock = std::make_shared<XLock>();
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
	else if( width <= 4096 )	width = 4096;
#ifdef WIN32
	else if (width <= 8192 )	width = 8192;
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
	else if( height <= 4096 )	height = 4096;
#ifdef WIN32
	else if (height <= 8192 )	height = 8192;
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
	if( w == 0 ) {
		XERROR("Too big size texture %d %d", *width, *height);
		return 0;
	}
	h = AlignPow2Height( h );
	if( h == 0 ) {
		XERROR("Too big size texture %d %d", *width, *height);
		return 0;
	}
	*width = w;
	*height = h;
	return 1;
}
// XE::POINT XGraphics::AlignPowSize( int width, int height )
// {
// 	int w = width;
// 	int h = height;
// 	AlignPowSize( &w, &h );
// 	return XE::POINT( w, h );
// }

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

// v1 - v2를 잇는 곡선을 그린다
void XGraphics::DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, 
														const XE::VEC2& v2, const XE::VEC2& v3,
														int maxSlice, XCOLOR col, xtLine lineType )
{
	XE::VEC2 vPrev;
	vPrev = v1;
	XE::VEC2 dv0, dv1, dv2, dv3, vOut;
	for( int i = 1; i <= maxSlice; i ++ )		// 0.x ~ 1.0까지
	{
		dv0 = v0;		dv1 = v1;		dv2 = v2;		dv3 = v3;
		Vec2CatmullRom( vOut, dv0, dv1, dv2, dv3, (float)i / maxSlice );
		float thickness = GetLineWidth();
		if( lineType == xLINE_LINE )
			XE::DrawLine( vPrev.x, vPrev.y, vOut.x, vOut.y, col, thickness );
		else
			if( lineType == xLINE_POINT )
				DrawPoint( vOut.x, vOut.y, 1, col );
		vPrev = vOut;
	}
}


void XGraphics::DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType )
{
	if( pvList == NULL )	return;
	if( maxPoint < 2 )	return;	// 점이 최소한 2개이상은 되어야 한다
	XE::VEC2 v0, v1, v2, v3;
	 
	for( int k = 0; k < maxPoint-1; k++ )
	{
		if( k == 0 )	v0 = pvList[k];
		else			v0 = pvList[k-1];
		v1 = pvList[k];
		v2 = pvList[k+1];
		if( k == maxPoint-2 )	v3 = pvList[k+1];
		else						v3 = pvList[k+2];
		// 점사이를 보간하며 스플라인을 그린다
		DrawSplineCatmullRom( v0, v1, v2, v3, maxSlice, col, lineType );
	}
}

void XGraphics::DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col1, XCOLOR col2, xtLine lineType )
{
	if( pvList == NULL )	return;
	if( maxPoint < 2 )	return;	// 점이 최소한 2개이상은 되어야 한다
	XE::VEC2 v0, v1, v2, v3;
	float r1 = XCOLOR_RGB_R(col1) / 255.f;
	float g1 = XCOLOR_RGB_G(col1) / 255.f;
	float b1 = XCOLOR_RGB_B(col1) / 255.f;
	float a1 = XCOLOR_RGB_A(col1) / 255.f;
	float r2 = XCOLOR_RGB_R(col2) / 255.f;
	float g2 = XCOLOR_RGB_G(col2) / 255.f;
	float b2 = XCOLOR_RGB_B(col2) / 255.f;
	float a2 = XCOLOR_RGB_A(col2) / 255.f;
	float sr = r2 - r1;
	float sg = g2 - g1;
	float sb = b2 - b1;
	float sa = a2 - a1;
	for( int k = 0; k < maxPoint-1; k++ )
	{
		float lerpTime = (float)k / (maxPoint-1);
		float lerpCol = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
		if( k == 0 )	v0 = pvList[k];
		else			v0 = pvList[k-1];
		v1 = pvList[k];
		v2 = pvList[k+1];
		if( k == maxPoint-2 )	v3 = pvList[k+1];
		else						v3 = pvList[k+2];
		// 점사이를 보간하며 스플라인을 그린다
		float cr = r1 + sr * lerpCol;
		float cg = g1 + sg * lerpCol;
		float cb = b1 + sb * lerpCol;
		float ca = a1 + sa * lerpCol;
		XCOLOR colCurr = XCOLOR_RGBA_FLOAT( cr, cg, cb, ca );
		DrawSplineCatmullRom( v0, v1, v2, v3, maxSlice, colCurr, lineType );
	}
}

#define MAX_VERTEX	256
#define POSX(I, V)	pos[I*2+0] = V
#define POSY(I, V)	pos[I*2+1] = V
#define POS(I,X,Y) {POSX(I, X); POSY(I, Y);}
#define COLOR_R(I, R) col[I*4+0] = R
#define COLOR_G(I, G) col[I*4+1] = G
#define COLOR_B(I, B) col[I*4+2] = B
#define COLOR_A(I, A) col[I*4+3] = A
#define COLOR(I,R,G,B,A) {COLOR_R(I, R);	COLOR_G(I, G);	COLOR_B(I, B);	COLOR_A(I, A);}

void XGraphics::DrawPie( float x, float y
											, float radius
											, float angStart, float angEnd
											, XCOLOR _color, int maxSlice )
{
	if( angStart == angEnd )
		return;
	float r = XCOLOR_RGB_R( _color ) / 255.f;
	float g = XCOLOR_RGB_G( _color ) / 255.f;
	float b = XCOLOR_RGB_B( _color ) / 255.f;
	float a = XCOLOR_RGB_A( _color ) / 255.f;
#ifdef WIN32
	x += GetViewportLeft();
	y += GetViewportTop();
#ifdef _XDYNA_RESO
	x += GetvScreenLT().x;
	y += GetvScreenLT().y;
#endif
	float ratioX = ( GetRatioWidth() );
	float ratioY = ( GetRatioHeight() );
	x *= ratioX;	y *= ratioY;
	radius *= ratioX;
#endif // WIN32
#ifdef _VER_OPENGL
	GLfloat pos[ MAX_VERTEX * 2 ];
	GLfloat col[ MAX_VERTEX * 4 ];
#else
	float pos[ MAX_VERTEX * 2 ];
	float col[ MAX_VERTEX * 4 ];
#endif // _VER_OPENGL
	float angSlice = 360.0f / (float)maxSlice;		// 
	float ang = 0;
	POS(0, x, y );	// 파이의 중심점
	COLOR( 0, r, g, b, a );
	POSX(1, x + (sinf(D2R(angStart)) * radius) );
	POSY(1, y + (-cosf(D2R(angStart)) * radius) );
	COLOR( 1, r, g, b, a );
	int idxAry = 2;
// 	aVertex = &m_aVertex[2];
// 	const FVF_2DVERTEX *pEnd = &m_aVertex[ MAX_2DVERTEX ];
	int num = 0;
	if( angEnd > 0 ) {
		ang += angSlice;
		while( ang < angEnd ) {
			if( ang >= angStart ) {		// 각도범위에 포함되면 버텍스를 추가
				float rAng = D2R( ang );		// 디그리 각도를 라디안각도로 변환
				POSX( idxAry, x + ( sinf( rAng ) * radius ) );
				POSY( idxAry, y + ( -cosf( rAng ) * radius ) );
				COLOR( idxAry, r, g, b, a );
				++idxAry;
				num++;		// 삼각형 개수
				if( idxAry >= MAX_VERTEX )		// 버퍼 오버플로우 되지 않도록
					break;
			}
			ang += angSlice;
		}
	} else {
		ang -= angSlice;
		while( ang > angEnd ) {
			if( ang <= angStart ) {		// 각도범위에 포함되면 버텍스를 추가
				float rAng = D2R( ang );		// 디그리 각도를 라디안각도로 변환
				POSX( idxAry, x + ( sinf( rAng ) * radius ) );
				POSY( idxAry, y + ( -cosf( rAng ) * radius ) );
				COLOR( idxAry, r, g, b, a );
				++idxAry;
				num++;		// 삼각형 개수
				if( idxAry >= MAX_VERTEX )		// 버퍼 오버플로우 되지 않도록
					break;
			}
			ang -= angSlice;
		}
	}
	// 마지막각도에 버텍스 하나 더 추가
	POSX( idxAry, x + (sinf(D2R(angEnd)) * radius) );
	POSY( idxAry, y + (-cosf(D2R(angEnd)) * radius) );
	COLOR( idxAry, r, g, b, a );
	++idxAry;
	num++;
	DrawFan( pos, col, idxAry, num );
}
/**
 @brief 파일로부터 이미지를 읽어서 서피스를 만든다.
*/
XSurface* XGraphics::CreateSurface( bool bHighReso
																	, LPCTSTR szRes
																	, XE::xtPixelFormat formatSurface
																	, bool bSrcKeep/* = false*/
																	, bool bMakeMask/* = false*/
																	, bool bAsync/* = false*/ )
{
	if( !bAsync ) {
		DWORD *pImg = nullptr;
		XE::POINT sizeImgMem;
		if( LoadImg( szRes, &sizeImgMem.w, &sizeImgMem.h, &pImg ) == false )
			return nullptr;
		XBREAK( pImg == nullptr );
		XBREAK( sizeImgMem.w <= 0 );
		XBREAK( sizeImgMem.h <= 0 );
		// bHighReso의 잔재. 이 파일이 고해상도로 지정되어 있으면 실제 서피스 크기는 절반이 된다
		const auto sizeSurface = (bHighReso)? sizeImgMem / 2 : sizeImgMem; 
		auto pSurface = CreateSurface();
		if( pSurface ) {
			bool bOk = pSurface->Create( sizeSurface
																, XE::VEC2(0)
																, formatSurface
																, pImg
																, XE::xPF_ARGB8888		// formatImgSrc
																, sizeImgMem
																, bSrcKeep
																, bMakeMask 
																, false );
			if( bOk ) {
				pSurface->SetstrRes( szRes );
			} else {
				SAFE_DELETE( pSurface );
			}
	// 		auto pSurface = _CreateSurface( sizeSurface
	// 																, XE::VEC2(0)		// adj
	// 																, formatSurface
	// 																, pImg
	// 																, XE::xPF_ARGB8888		// formatImgSrc
	// 																, sizeImgMem
	// 																, bSrcKeep, bMakeMask );
			SAFE_DELETE_ARRAY( pImg );		// 이미지 원본 날림.
		} else {
			XBREAKF( pSurface == nullptr, "[%s] createSurface failed", szRes );
		}
		return pSurface;
	} else {
		// 비동기로딩시에는 서피스 객체만 만들어 둔다.
		auto pSurface = CreateSurface();
		return pSurface;
	}
}


XSurface* XGraphics::CreateSurface( const XE::POINT& sizeSurfaceOrig
																	, const XE::VEC2& vAdj
																	, XE::xtPixelFormat formatSurface								
																	, void* const pImgSrc
																	, const XE::POINT& sizeMemSrc
																	, bool bSrcKeep, bool bMakeMask ) {
	auto pSurface = CreateSurface();		// virtual
	if( pSurface ) {
		bool bOk = pSurface->Create( sizeSurfaceOrig
															, vAdj
															, formatSurface
															, pImgSrc
															, XE::xPF_ARGB8888		// formatImgSrc
															, sizeMemSrc
															, bSrcKeep
															, bMakeMask
															, false );
		if( !bOk ) {
			SAFE_DELETE( pSurface );
		}
	} else {
		XBREAKF( pSurface == nullptr, "sizeSurface=(%d,%d), formatSurface=%d, sizeMemSrc=(%d,%d)"
																	, sizeSurfaceOrig.w, sizeSurfaceOrig.h
																	, formatSurface
																	, sizeMemSrc.w, sizeMemSrc.h );
	}
// 	auto pSurface = _CreateSurface( sizeSurfaceOrig
// 													, vAdj
// 													, formatSurface
// 													, pImgSrc
// 													, XE::xPF_ARGB8888	// formatImgSrc
// 													, sizeMemSrc
// 													, bSrcKeep
// 													, bMakeMask );
	return pSurface;
}

void XGraphics::DrawCircle( const XE::VEC2& vCenter, float radius, XCOLOR col, int steps, float dOffset )
{
// 	const XE::VEC2 vRatio( GetRatioResolution() );
// 	const auto vc = vCenter * vRatio;
// 	const auto radius = _radius * vRatio.x;
	const auto vc = vCenter;
	XE::VEC2 v[ 2 ];
	v[ 0 ].x = vc.x + ::cosf( D2R( dOffset ) ) * radius;
	v[ 0 ].y = vc.y + ::sinf( D2R( dOffset ) ) * radius;
	float dUnit = 360.f / ( radius / 4.f );
	if( steps > 0 )
		dUnit = 360.f / steps;
	float rAng = 0;
	for( rAng = D2R( dUnit + dOffset ); rAng < D2R( 360.f ); rAng += D2R( dUnit ) )
	{
		v[ 1 ].x = vc.x + cosf( rAng ) * radius;
		v[ 1 ].y = vc.y + sinf( rAng ) * radius;
		DrawLine( v[0], v[1], col, col );
		//
		v[ 0 ] = v[ 1 ];
	}
	v[ 1 ].x = vc.x + cosf( D2R( dOffset ) ) * radius;
	v[ 1 ].y = vc.y + sinf( D2R( dOffset ) ) * radius;
	DrawLine( v[ 0 ], v[ 1 ], col, col );


}

bool XGraphics::LoadImg( LPCTSTR szFilename, XE::xImage* pOut )
{
	return LoadImg( szFilename, &(pOut->m_ptSize.w), &(pOut->m_ptSize.h), &(pOut->m_pImg) ) != FALSE;
}

bool XGraphics::LoadImg( const _tstring& resImg, XE::xImage* pOut ) 
{
	return LoadImg( resImg.c_str(), pOut );
}

/**
@brief fmtSrc로 되어있는 pImgSrc를 fmtDst포맷으로 바꾼다.
*/
void XE::CreateConvertPixels( const void* pImgSrc,
															int wSrc, int hSrc,
															xtPixelFormat fmtSrc,
															const void** ppDstOut,
															xtPixelFormat fmtDst )
{
	XBREAK( fmtSrc == fmtDst );
	XBREAK( fmtSrc != xPF_ARGB8888 );
	XBREAK( fmtDst == xPF_ARGB8888 );
	const int bppSrc = XE::GetBpp( fmtSrc );
	const int bppDst = XE::GetBpp( fmtDst );
	if( bppDst == 4 ) {
		// 4바이트 포맷은 현재는 8888밖에 없어서 변환할 필요가 없어서 그냥 놔둠.
	} else
		if( bppDst == 2 ) {
			auto pDst = new WORD[wSrc * hSrc];
			*ppDstOut = pDst;
			switch( fmtDst ) {
			case xPF_ARGB1555:
				ConvertBlockABGR8888ToRGBA1555( pDst, wSrc, hSrc, (DWORD*)pImgSrc, wSrc, hSrc );
				break;
			case xPF_ARGB4444:
				XE::ConvertBlockABGR8888ToRGBA4444( pDst, wSrc, hSrc, (DWORD*)pImgSrc, wSrc, hSrc );
				break;
			case xPF_RGB565:
			case xPF_RGB555:
				XE::ConvertBlockABGR8888ToRGB565( pDst, wSrc, hSrc, (DWORD*)pImgSrc, wSrc, hSrc );
				break;
			default:
				XBREAK( 1 );
				break;
			}
		}
}
