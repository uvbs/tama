#ifndef __XCOLOR_H__
#define __XCOLOR_H__
/*
 *  xColor.h
 *  Skinning
 *
 *  Created by xuzhu on 10. 7. 23..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

typedef DWORD XCOLOR;
typedef WORD XCOLORW;


#ifdef _VER_DX
#define XCOLOR_RGB565(r,g,b)	((XCOLORW)((((r)>>3) << 11) | (((g)>>2) << 5) | ((b)>>3)))
#define XCOLOR_RGB555(r,g,b)	((XCOLORW)((((r)>>3) << 10) | (((g)>>3) << 5) | ((b)>>3)))
#define XCOLOR_RGB565_R(W)		(((((W) >> 11) & 0x1f) * 255) / 31)		// 
#define XCOLOR_RGB565_G(W)		(((((W) >> 5) & 0x1f) * 255) / 63)		// 
#define XCOLOR_RGB565_B(W)		((((W) & 0x1f) * 255) / 31)		// 

#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define XCOLOR_ARGB(a,r,g,b)	XCOLOR_RGBA(r,g,b,a)
//#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((r)<<16) | ((g)<<8) | (b)))	//
#define XCOLOR_RGBX(r,g,b)	XCOLOR_RGBA(r,g,b,0xff)
#define XCOLOR_XRGB(r,g,b)	XCOLOR_RGBX(r,g,b)
#define XCOLOR_ABGR(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))			// mac에 맞게 abgr순으로 저장한다
#define XCOLOR_RGB(r,g,b)		XCOLOR_RGBA(r,g,b,255)
#define XCOLOR_RGB_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_RGB_R(col)		(((col)>>16) & 0xff)
#define XCOLOR_RGB_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_RGB_B(col)		((col) & 0xff)

#else //////////////////////////////////////////////
// _ver_opengl
//
#define XCOLOR_RGB565(r,g,b)	((XCOLORW)((((b)>>3) << 11) | (((g)>>2) << 5) | ((r)>>3)))
#define XCOLOR_RGB555(r,g,b)	((XCOLORW)((((b)>>3) << 10) | (((g)>>3) << 5) | ((r)>>3)))
#define XCOLOR_RGB565_R(W)		((((W) & 0x1f) * 255) / 31) 	//
#define XCOLOR_RGB565_G(W)		(((((W) >> 5) & 0x1f) * 255) / 63)		// 
#define XCOLOR_RGB565_B(W)		(((((W) >> 11) & 0x1f) * 255) / 31)		//

#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
#define XCOLOR_ARGB(a,r,g,b)	XCOLOR_RGBA(r,g,b,a)
#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((b)<<16) | ((g)<<8) | (r)))	//
#define XCOLOR_RGBX(r,g,b)	XCOLOR_RGBA(r,g,b,0xff)
#define XCOLOR_XRGB(r,g,b)	XCOLOR_RGBX(r,g,b)
#define XCOLOR_ABGR(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))			// mac에 맞게 abgr순으로 저장한다
#define XCOLOR_RGB_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_RGB_B(col)		(((col)>>16) & 0xff)
#define XCOLOR_RGB_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_RGB_R(col)		((col) & 0xff)

#endif // not win32
//////////////////////////////////////////////////////////////////////////
// 플랫폼 공통
// ABGR로 되어있는 컬러데이터에서 A,R,G,B각각을 뽑아낸다. 이것은 플랫폼모두 똑같다.
#define XCOLOR_BGR_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_BGR_B(col)		(((col)>>16) & 0xff)
#define XCOLOR_BGR_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_BGR_R(col)		((col) & 0xff)
#ifdef WIN32
// ARGB8888의 순서로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_ARGB(a,r,g,b)	((XCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
// RGB4444형태로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_ARGB4444(a,r,g,b)	((XCOLORW)( (((a)>>4) << 12) | (((r)>>4) << 8) | (((g)>>4) << 4) | ((b)>>4) ))
#endif // WIN32

#ifdef _VER_OPENGL
// ABGR의 순서로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_ABGR(a,b,g,r)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
// BGR565형태로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_BGR565(b,g,r)	((XCOLORW)((((b)>>3) << 11) | (((g)>>2) << 5) | ((r)>>3)))
// BGR4444형태로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_ABGR4444(a,b,g,r)	((XCOLORW)( (((a)>>4) << 12) | (((b)>>4) << 8) | (((g)>>4) << 4) | ((r)>>4) ))
#endif // _VER_OPENGL
// ARGB로 되어있는 데이터에서 A,R,G,B를 각각 뽑아낸다.
#define XCOLOR_A_FROM_ARGB(col)		(((col)>>24) & 0xff)
#define XCOLOR_R_FROM_ARGB(col)		(((col)>>16) & 0xff)
#define XCOLOR_G_FROM_ARGB(col)		(((col)>>8) & 0xff)
#define XCOLOR_B_FROM_ARGB(col)		((col) & 0xff)
// ABGR순서로 되어있는 데이터에서 a,r,g,b를 각각 뽑아낸다.
#define XCOLOR_A_FROM_ABGR(col)		(((col)>>24) & 0xff)
#define XCOLOR_R_FROM_ABGR(col)		((col) & 0xff)
#define XCOLOR_G_FROM_ABGR(col)		(((col)>>8) & 0xff)
#define XCOLOR_B_FROM_ABGR(col)		(((col)>>16) & 0xff)
// RGBA4444형태로 메모리에 쌓인다.
#define XCOLOR_TO_RGBA4444(r,g,b,a)	((XCOLORW)( (((r)>>4) << 12) | (((g)>>4) << 8) | (((b)>>4) << 4) | ((a)>>4) ))
// RGB565형태로 메모리에 쌓이게 만든다.
#define XCOLOR_TO_RGB565(r,g,b)	((XCOLORW)((((r)>>3) << 11) | (((g)>>2) << 5) | ((b)>>3)))

// 인자를 0~1.0으로 주는 버전.
#define XCOLOR_RGBA_FLOAT(fR,fG,fB,fA)	XCOLOR_RGBA( (BYTE)(fR * 255), (BYTE)(fG * 255), (BYTE)(fB * 255), (BYTE)(fA * 255) )

#define XCOLOR_WHITE	XCOLOR_RGBA( 255, 255, 255, 0xff )
#define XCOLOR_BLACK	XCOLOR_RGBA( 0, 0, 0, 0xff )
#define XCOLOR_RED		XCOLOR_RGBA( 255, 0, 0, 0xff )
#define XCOLOR_GREEN	XCOLOR_RGBA( 0, 255, 0, 0xff )
#define XCOLOR_BLUE		XCOLOR_RGBA( 0, 0, 255, 0xff )
#define	XCOLOR_PURPLE	XCOLOR_RGBA( 255, 0, 255, 0xff )
#define XCOLOR_SKY		XCOLOR_RGBA( 0, 255, 246, 0xff )
#define	XCOLOR_GRAY	XCOLOR_RGBA( 0x80, 0x80, 0x80, 0xff )
#define	XCOLOR_LIGHTGRAY	XCOLOR_RGBA( 0xd3, 0xd3, 0xd3, 0xff )
#define	XCOLOR_DARKGRAY	XCOLOR_RGBA( 100, 100, 100, 0xff )
#define XCOLOR_YELLOW	XCOLOR_RGBA( 255, 255, 0, 0xff )
#define XCOLOR_BROWN    XCOLOR_RGBA( 156, 118, 59, 0xff )
#define XCOLOR_ORANGE    XCOLOR_RGBA( 255, 127, 36, 0xff )

#define XCOLORKEY_R	0
#define XCOLORKEY_G	0
#define XCOLORKEY_B	255

// 
#define		_XCOLORKEY555		XCOLOR_RGB555(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B)
#define		IS_COLORKEY555(c)		((c) == _XCOLORKEY555)
#define		IS_COLORKEY555_TILE(c)	((c) == _XCOLORKEY555)
#define		IS_COLORKEY555_SPR(c)	((c) == _XCOLORKEY555)
// 
#define		_XCOLORKEY565		XCOLOR_RGB565(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B)
#define		IS_COLORKEY565(c)		((c) == _XCOLORKEY565)
#define		IS_COLORKEY565_TILE(c)	((c) == _XCOLORKEY565)
#define		IS_COLORKEY565_SPR(c)	((c) == _XCOLORKEY565)
// 
#define		_XCOLORKEYRGBA8		XCOLOR_RGBA(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B,0)	
#define		IS_COLORKEYRGBA8(c)			(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)
#define		IS_COLORKEYRGBA8_TILE(c)	(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)
#define		IS_COLORKEYRGBA8_SPR(c)		(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)


#endif // __XCOLOR_H__
