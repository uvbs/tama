/*
 *  XImage.h
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#ifdef _XLIBPNG
#include <stdlib.h>
#include "etc/XBaseImage.h"


// 디버깅용이 아니라면 사실살 이걸 직접 쓰는 일은 없을거다.
class XImageByLibpng : public XBaseImage<DWORD>
{
	void Init( void ) {}
	void Destroy( void ) {}
public:
	XImageByLibpng( BOOL bHighReso ) : XBaseImage<DWORD>( bHighReso ) { Init(); }
	XImageByLibpng( BOOL bHighReso, LPCTSTR szFilename ) : XBaseImage<DWORD>( bHighReso ){
        Init();
        Load( szFilename );
    }
	XImageByLibpng( BOOL bHighReso, void *pFileData, int sizeFile ) : XBaseImage<DWORD>( bHighReso ){
        Init();
        LoadFromFileData( pFileData, sizeFile );
    }
	virtual ~XImageByLibpng() { Destroy(); }
    //

    BOOL LoadFromFileData( void *pFileData, int sizeFile );
    virtual BOOL Load( LPCTSTR szFilename );
};

namespace XE {
    int LoadPngByLibpngFromPackage( LPCTSTR szPng, XIMAGE_INFO *pOutInfo );
    int LoadPngByLibpng( void *pPngData, int nDatalen, XIMAGE_INFO *pOutInfo );
    int LoadPngByLibpng( LPCTSTR szPng, XIMAGE_INFO *pOutInfo );
    
};
#endif // _XLIBPNG
