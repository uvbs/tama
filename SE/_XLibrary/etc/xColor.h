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


#ifdef _VER_MAC
/*
 #define RGB565(r, g ,b)		(((int)(r) << 11) | ((int)(g) << 5) | (int)(b))					// rgb¿Œºˆ∞° 5∫Ò∆Æ«¸≈¬¿œ∂ß.
 #define RGB555(r, g ,b)		(((int)(r) << 10) | ((int)(g) << 5) | (int)(b))
 #define RGB565_8(r, g ,b)		(((int)((r)>>3) << 11) | ((int)((g)>>2) << 5) | (int)((b)>>3)) // rgb¿Œºˆ∞° 8∫Ò∆Æ«¸≈¬
 #define RGB555_8(r, g ,b)		(((int)((r)>>3) << 10) | ((int)((g)>>3) << 5) | (int)((b)>>3))
 
 //#define RGBA(r,g,b,a)	(((DWORD)(a)<<24) | ((DWORD)(b)<<16) | ((DWORD)(g)<<8) | (r) )
 #define RGB(r,g,b)	( ((DWORD)b<<16) | ((DWORD)g<<8) | r )	// æÀ∆ƒ∞™¿∫ ππ∞°µ«µÁ π´Ω√«—¥Ÿ.
 #define RGB565_R(W)		((((W >> 11) & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ R∞™¿ª √ﬂ√‚.
 #define RGB565_G(W)		((((W >> 5) & 0x1f) * 255) / 63)		// 565µ•¿Ã≈Õø°º≠ G∞™¿ª √ﬂ√‚.
 #define RGB565_B(W)		(((W & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ B∞™¿ª √ﬂ√‚.
 */
// 5∫Ò∆Æ«¸≈¬¿« ∏≈≈©∑Œ¥¬ øœ¿¸»˜ ª∞¥Ÿ. rgb¿Œºˆ¥¬ ∏µŒ 0~255∞™¿Ã µ«æÓæﬂ «—¥Ÿ.
#define XCOLOR_RGB565(r,g,b)	((XCOLORW)((((r)>>3) << 11) | (((g)>>2) << 5) | ((b)>>3))) 
#define XCOLOR_RGB555(r,g,b)	((XCOLORW)((((r)>>3) << 10) | (((g)>>3) << 5) | ((b)>>3)))
#define XCOLOR_RGB565_R(W)		(((((W) >> 11) & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ R∞™¿ª √ﬂ√‚.
#define XCOLOR_RGB565_G(W)		(((((W) >> 5) & 0x1f) * 255) / 63)		// 565µ•¿Ã≈Õø°º≠ G∞™¿ª √ﬂ√‚.
#define XCOLOR_RGB565_B(W)		((((W) & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ B∞™¿ª √ﬂ√‚.

//#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
//#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((r)<<16) | ((g)<<8) | (b)))	// æÀ∆ƒ∞™¿∫ ππ∞°µ«µÁ π´Ω√«—¥Ÿ.
// iphone? opengl? ¿∫ rgbº¯º≠∞° π›¥Î¥Ÿ.
#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
#define XCOLOR_ARGB(a,r,g,b)	XCOLOR_RGBA(r,g,b,a)
#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((b)<<16) | ((g)<<8) | (r)))	// æÀ∆ƒ∞™¿∫ ππ∞°µ«µÁ π´Ω√«—¥Ÿ.
#define XCOLOR_RGBX(r,g,b)	XCOLOR_RGBA(r,g,b,0xff)
#define XCOLOR_XRGB(r,g,b)	XCOLOR_RGBX(r,g,b)
#define XCOLOR_RGB_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_RGB_B(col)		(((col)>>16) & 0xff)
#define XCOLOR_RGB_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_RGB_R(col)		((col) & 0xff)

#else // __VER_MAC--------------------------------------------------------------------------------------------

// 5∫Ò∆Æ«¸≈¬¿« ∏≈≈©∑Œ¥¬ øœ¿¸»˜ ª∞¥Ÿ. rgb¿Œºˆ¥¬ ∏µŒ 0~255∞™¿Ã µ«æÓæﬂ «—¥Ÿ.
#define XCOLOR_RGB565(r,g,b)	((XCOLORW)((((r)>>3) << 11) | (((g)>>2) << 5) | ((b)>>3))) 
#define XCOLOR_RGB555(r,g,b)	((XCOLORW)((((r)>>3) << 10) | (((g)>>3) << 5) | ((b)>>3)))
#define XCOLOR_RGB565_R(W)		(((((W) >> 11) & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ R∞™¿ª √ﬂ√‚.
#define XCOLOR_RGB565_G(W)		(((((W) >> 5) & 0x1f) * 255) / 63)		// 565µ•¿Ã≈Õø°º≠ G∞™¿ª √ﬂ√‚.
#define XCOLOR_RGB565_B(W)		((((W) & 0x1f) * 255) / 31)		// 565µ•¿Ã≈Õø°º≠ B∞™¿ª √ﬂ√‚.

//#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
//#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((r)<<16) | ((g)<<8) | (b)))	// æÀ∆ƒ∞™¿∫ ππ∞°µ«µÁ π´Ω√«—¥Ÿ.
// iphone? opengl? ¿∫ rgbº¯º≠∞° π›¥Î¥Ÿ.
#define XCOLOR_RGBA(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define XCOLOR_ARGB(a,r,g,b)	XCOLOR_RGBA(r,g,b,a)
//#define XCOLOR_RGB(r,g,b)		((XCOLOR)(((r)<<16) | ((g)<<8) | (b)))	// æÀ∆ƒ∞™¿∫ ππ∞°µ«µÁ π´Ω√«—¥Ÿ.
#define XCOLOR_RGBX(r,g,b)	XCOLOR_RGBA(r,g,b,0xff)
#define XCOLOR_XRGB(r,g,b)	XCOLOR_RGBX(r,g,b)
#define XCOLOR_ABGR(r,g,b,a)	((XCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))			// mac에 맞게 abgr순으로 저장한다
#define XCOLOR_RGB_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_RGB_R(col)		(((col)>>16) & 0xff)
#define XCOLOR_RGB_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_RGB_B(col)		((col) & 0xff)
#define XCOLOR_BGR_A(col)		(((col)>>24) & 0xff)
#define XCOLOR_BGR_B(col)		(((col)>>16) & 0xff)
#define XCOLOR_BGR_G(col)		(((col)>>8) & 0xff)
#define XCOLOR_BGR_R(col)		((col) & 0xff)
#endif // not __VER_MAC

#define XCOLOR_WHITE	XCOLOR_RGBA( 255, 255, 255, 0xff )
#define XCOLOR_BLACK	XCOLOR_RGBA( 0, 0, 0, 0xff )
#define XCOLOR_RED		XCOLOR_RGBA( 255, 0, 0, 0xff )
#define XCOLOR_GREEN	XCOLOR_RGBA( 0, 255, 0, 0xff )
#define XCOLOR_BLUE		XCOLOR_RGBA( 0, 0, 255, 0xff )
#define	XCOLOR_PURPLE	XCOLOR_RGBA( 255, 0, 255, 0xff )
#define	XCOLOR_GRAY	XCOLOR_RGBA( 0x80, 0x80, 0x80, 0xff )
#define	XCOLOR_LIGHTGRAY	XCOLOR_RGBA( 0xd3, 0xd3, 0xd3, 0xff )
#define	XCOLOR_DARKGRAY	XCOLOR_RGBA( 0xa9, 0xa9, 0xa9, 0xff )
#define XCOLOR_YELLOW	XCOLOR_RGBA( 255, 255, 0, 0xff )
#define XCOLOR_BROWN	XCOLOR_RGBA( 156, 118, 59, 0xff )
#define XCOLOR_ORANGE	XCOLOR_RGBA(255, 180, 0, 255)
#define XCOLORKEY_R	0
#define XCOLORKEY_G	0
#define XCOLORKEY_B	255

// 555ƒ√∑Ø∆˜∏À¿« c∞™¿« ƒ√∑Ø≈∞ø©∫Œ.
#define		_XCOLORKEY555		XCOLOR_RGB555(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B)
#define		IS_COLORKEY555(c)		((c) == _XCOLORKEY555)
#define		IS_COLORKEY555_TILE(c)	((c) == _XCOLORKEY555)
#define		IS_COLORKEY555_SPR(c)	((c) == _XCOLORKEY555)
// 565∆˜∏À
#define		_XCOLORKEY565		XCOLOR_RGB565(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B)
#define		IS_COLORKEY565(c)		((c) == _XCOLORKEY565)
#define		IS_COLORKEY565_TILE(c)	((c) == _XCOLORKEY565)
#define		IS_COLORKEY565_SPR(c)	((c) == _XCOLORKEY565)
// RGBA8∆˜∏À¿« c∞™¿« ƒ√∑Ø≈∞ø©∫Œ.
#define		_XCOLORKEYRGBA8		XCOLOR_RGBA(XCOLORKEY_R,XCOLORKEY_G,XCOLORKEY_B,0)	
#define		IS_COLORKEYRGBA8(c)			(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)
#define		IS_COLORKEYRGBA8_TILE(c)	(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)
#define		IS_COLORKEYRGBA8_SPR(c)		(((c) & 0x00ffffff) == _XCOLORKEYRGBA8)


#endif // __XCOLOR_H__