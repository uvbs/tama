/*
 *  XImage.h
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#include <stdlib.h>
#include "etc/XBaseImage.h"

XE_NAMESPACE_START( XE )
//
struct xImage {
	DWORD *m_pImg = nullptr;
	XE::POINT m_ptSize;
	xImage() {}
	xImage( DWORD *pImg, int w, int h ) {
		Set( pImg, w, h );
	}
	xImage( DWORD *pImg, const XE::POINT& size ) {
		Set( pImg, size );
	}
	~xImage() {
		Destroy();
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pImg );
		m_ptSize.Set( 0, 0 );
	}
	void Set( DWORD *pImg, int w, int h ) {
		m_pImg = pImg;
		m_ptSize.Set( w, h );
	}
	void Set( DWORD *pImg, const XE::POINT& size ) {
		m_pImg = pImg;
		m_ptSize = size;
	}
};
//
XE_NAMESPACE_END; // XE

class XImage : public XBaseImage<DWORD>
{
	void Init( void ) {
	}
	void Destroy( void ) {
	}
public:
	XImage( BOOL bHighReso ) : XBaseImage<DWORD>( bHighReso ) { Init(); }
	XImage( BOOL bHighReso, LPCTSTR szFilename ) : XBaseImage<DWORD>( bHighReso ){
        Init();
        Load( szFilename );
    }
	XImage( BOOL bHighReso, void *pFileData, int sizeFile ) : XBaseImage<DWORD>( bHighReso ){
        Init();
        LoadFromFileData( pFileData, sizeFile );
    }
	virtual ~XImage() { Destroy(); }
    //

    BOOL Load( LPCTSTR szFilename );
    BOOL LoadFromFileData( void *pFileData, int sizeFile );
};



