/*
 *  XImage.cpp
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#ifdef _XLIBPNG
#include "XImageLibPng.h"
#ifdef _VER_OPENGL
#include "OpenGL2/XOpenGL.h"
#endif
#include "etc/xGraphics.h"
#if defined(_VER_ANDROID)
//	#include "png.h"	// 안드로이드는 prebuilt된 안드로이드 전용 라이브러리를 써야 한다.
#include "libpng1256/png.h"
#elif defined(WIN32)
//	#include "libpng_win/include/png.h"
	#include "libpng1256/png.h"
#elif defined(_VER_IOS)
	#include "libpng/png.h"
//#elif defined(WIN32)
//#include "win_libpng/include/png.h"
#else
	#error("unknown platform")
#endif
#include "XImage.h" // 임시
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


#ifdef _VER_ANDROID
// 	#define BTRACE(F, ...)					__xLog( XLOGTYPE_LOG, _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)
#else
	#define BTRACE(F, ...)					(0)
#endif // _VER_ANDROID
//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
//#define BTRACE(F, ...)					(0)

#define CC_BREAK_IF(cond, err)            if(XBREAK(cond)) { nRet = err; break; }

// premultiply alpha, or the effect will wrong when want to use other pixel format in CCTexture2D,
// such as RGB888, RGB5A1
#define CC_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
(unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
((unsigned)(unsigned char)(va) << 24))

namespace XE
{
typedef struct
{
    unsigned char* data;
    int size;
    int offset;
}tImageSource;

static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);
    
    if((int)(isource->offset + length) <= isource->size)
    {
        memcpy(data, isource->data+isource->offset, length);
        isource->offset += length;
    }
    else
    {
        png_error(png_ptr, "pngReaderCallback failed");
    }
}

int LoadPngByLibpng( void *pPngData, int nDatalen, XIMAGE_INFO *pOutInfo )
{
    // length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
    int nRet = 0;
    png_byte        header[PNGSIGSIZE]   = {0};
    png_structp     png_ptr     =   0;
    png_infop       info_ptr    = 0;
    XIMAGE_INFO info;
    do {
        // png header len is 8 bytes
        CC_BREAK_IF(nDatalen < PNGSIGSIZE, -1);
        // check the data is png or not
        memcpy(header, pPngData, PNGSIGSIZE);
        CC_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE), -2);
        // init png_struct
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        CC_BREAK_IF(! png_ptr, -3);
        // init png_info
        info_ptr = png_create_info_struct(png_ptr);
        CC_BREAK_IF(!info_ptr, -4);
        CC_BREAK_IF(setjmp(png_jmpbuf(png_ptr)), -5);
        // set the read call back function
        tImageSource imageSource;
        imageSource.data    = (unsigned char*)pPngData;
        imageSource.size    = nDatalen;
        imageSource.offset  = 0;
        png_set_read_fn(png_ptr, &imageSource, pngReadCallback);
        // read png header info
        // read png file info
        png_read_info(png_ptr, info_ptr);
        info.width = png_get_image_width(png_ptr, info_ptr);
        info.height = png_get_image_height(png_ptr, info_ptr);
        info.nBitsPerComponent = png_get_bit_depth(png_ptr, info_ptr);
        png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);
        // force palette images to be expanded to 24-bit RGB
        // it may include alpha channel
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_palette_to_rgb(png_ptr);
        }
        // low-bit-depth grayscale images are to be expanded to 8 bits
        if (color_type == PNG_COLOR_TYPE_GRAY && info.nBitsPerComponent < 8) {
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        }
        // expand any tRNS chunk data into a full alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png_ptr);
        }
        // reduce images with 16-bit samples to 8 bits
        if (info.nBitsPerComponent == 16) {
            png_set_strip_16(png_ptr);
        }
        // expand grayscale images to RGB
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            png_set_gray_to_rgb(png_ptr);
        }
        // read png data
        // m_nBitsPerComponent will always be 8
        info.nBitsPerComponent = 8;
        png_uint_32 rowbytes;
        png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * info.height );
        png_read_update_info(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        info.pData = new BYTE[rowbytes * info.height ];
        CC_BREAK_IF(!info.pData, -6);
        for (unsigned short i = 0; i < info.height; ++i) {
            row_pointers[i] = info.pData + i*rowbytes;
        }
        png_read_image(png_ptr, row_pointers);
        png_read_end(png_ptr, NULL);
        png_uint_32 channel = rowbytes / info.width;
        if (channel == 4) {
            info.bHasAlpha = TRUE;
            unsigned int *tmp = (unsigned int *)info.pData;
            for(unsigned short i = 0; i < info.height; i++) {
                for(unsigned int j = 0; j < rowbytes; j += 4) {
//                     *tmp++ = CC_RGB_PREMULTIPLY_ALPHA( row_pointers[i][j], row_pointers[i][j + 1],
//                                                       row_pointers[i][j + 2], row_pointers[i][j + 3] );
					*tmp++ = XCOLOR_RGBA( row_pointers[i][j], row_pointers[i][j+1], row_pointers[i][j+2], row_pointers[i][j+3] );
                }
            }
            info.bPreMulti = TRUE;
        } else
        if( channel == 3 ) {
            DWORD *pData = new DWORD[ info.width * info.height ];
            DWORD *tmp = pData;
            for(unsigned short i = 0; i < info.height; i++) {
                for(unsigned int j = 0; j < rowbytes; j += 3) {
                    *tmp++ = XCOLOR_RGBA( row_pointers[i][j], row_pointers[i][j+1], row_pointers[i][j+2], 255 );
                }
            }
            SAFE_DELETE_ARRAY( info.pData );
            info.pData = (BYTE*)pData;
        }
        SAFE_FREE(row_pointers);
        nRet = 1;
    } while (0);
    
    if (png_ptr)  {
        png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
    }
    XBREAK( pOutInfo == NULL );
    if( pOutInfo )
        *pOutInfo = info;
    return nRet;
}
 
int LoadPngByLibpngFromPackage( LPCTSTR szRes, XIMAGE_INFO *pOutInfo )
{
    DWORD sizeFile = 0;
    BYTE *pPngData = XE::CreateMemFromPackage( szRes, &sizeFile );
		if( sizeFile > 0 ) {
			auto errCode = LoadPngByLibpng( pPngData, sizeFile, pOutInfo );
			XBREAKF( errCode <= 0, "err=%d[%s]", errCode, szRes );
			SAFE_DELETE_ARRAY( pPngData );
			return errCode;
		}
		return 0;
}
int LoadPngByLibpng( LPCTSTR szRes, XIMAGE_INFO *pOutInfo )
{
    DWORD sizeFile = 0;
    BYTE *pPngData = XE::CreateMemFromRes( szRes, &sizeFile );
		if( sizeFile > 0 ) {
			BTRACE("1:[%s],sizeFile=%d, pPngData=0x%08x", szRes, sizeFile, (DWORD)pPngData );
			auto errCode = LoadPngByLibpng( pPngData, sizeFile, pOutInfo );
			BTRACE("2:errCode", errCode );
			if( errCode <= 0 ) {
				BTRACE("%s", _T("3:log"));
				CONSOLE( "err=%d,[%s]", errCode, szRes );
				XBREAKF( 1, "%s", szRes );
				BTRACE("%s", _T("4:log"));
			}
			BTRACE("5:");
			SAFE_DELETE_ARRAY( pPngData );
			BTRACE("6:");
			return errCode;
		}
		return 0;
}
}; // namespace XE
//////////////////////////////////////////////////////////////////
// png로딩. szFilename은 풀패스
BOOL XImageByLibpng::Load( LPCTSTR szRes )
{
    DWORD sizeFile = 0;
    BYTE *pData = XE::CreateMemFromRes( szRes, &sizeFile );
    XIMAGE_INFO info;
    auto errCode = XE::LoadPngByLibpng( pData, sizeFile, &info );
    if( errCode <= 0 )
        return FALSE;
    XBaseImage<DWORD>::SetSize( info.width, info.height );
    XBaseImage<DWORD>::SetTextureData( (DWORD *)info.pData );
    Setbpp( sizeof(DWORD) );
    SAFE_DELETE_ARRAY( pData );
    return XBaseImage<DWORD>::Load( szRes );
}

BOOL XImageByLibpng::LoadFromFileData( void *pFileData, int sizeFile )
{
    XIMAGE_INFO info;
    XE::LoadPngByLibpng( pFileData, sizeFile, &info );
    XBaseImage<DWORD>::SetSize( info.width, info.height );
    XBaseImage<DWORD>::SetTextureData( (DWORD *)info.pData );
    Setbpp( sizeof(DWORD) );
    return XBaseImage<DWORD>::Load( NULL );
}


#endif // _XLIBPNG
