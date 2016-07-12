/*
 *  XImage.cpp
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XImage.h"
#ifdef _VER_IOS
#include "XImageLibPng.h"
#include "XImageiOS.h"
#endif
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#endif
#ifdef _XLIBPNG
#include "XImageLibPng.h"
#endif
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// image파일 로딩. 현재는 png만 지원
BOOL XImage::Load( LPCTSTR szRes )
{
    XIMAGE_INFO info;
    BOOL bRet = FALSE;
    
#ifdef _VER_IOS
    TCHAR szPath[ 1024 ];
    XE::SetReadyRes( szPath, szRes );
    DWORD *pData = NULL;
    bRet = LoadImageiOS( szPath, &info.width, &info.height, &pData );
    info.pData = (BYTE*)pData;
#endif // iOS
#ifdef _XLIBPNG
	bRet = XE::LoadPngByLibpng( szRes, &info );
// _XLIBPNG
#else
  #ifdef _VER_DX
	DWORD *pData = NULL;
	bRet = GRAPHICS_D3DTOOL->LoadImg( szRes, &info.width, &info.height, &pData );
	info.pData = (BYTE*)pData;
  #endif // ver_dx
// not _XLIBPNG
#endif
// #ifdef _VER_DX
// 	DWORD *pData = NULL;
// 	bRet = GRAPHICS_D3DTOOL->LoadImg( szRes, &info.width, &info.height, &pData );
// 	info.pData = (BYTE*)pData;
// #endif
// #ifdef _VER_ANDROID
//     bRet = XE::LoadPngByLibpng( szRes, &info );
// #endif // android
 
    if( bRet == FALSE )
        return FALSE;
    
//	XLOGXN( "XImage::Load:%s", szRes );
    XBaseImage<DWORD>::SetSize( info.width, info.height );
    XBaseImage<DWORD>::SetTextureData( (DWORD *)info.pData );
    Setbpp( sizeof(DWORD) );
    return XBaseImage<DWORD>::Load( szRes );
}

BOOL XImage::LoadFromFileData( void *pFileData, int sizeFile )
{
    XIMAGE_INFO info;
    BOOL bRet = FALSE;
#ifdef _VER_IOS
    XBREAK(1);
#endif // iOS
#ifdef WIN32
    XBREAK(1);  // 미구현
#endif
#ifdef _VER_OPENGL
    bRet = XE::LoadPngByLibpng( pFileData, sizeFile, &info );
#endif // android
    
    XBaseImage<DWORD>::SetSize( info.width, info.height );
    XBaseImage<DWORD>::SetTextureData( (DWORD *)info.pData );
    Setbpp( sizeof(DWORD) );
    return XBaseImage<DWORD>::Load( NULL );
}
