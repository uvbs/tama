/*
 *  XImage.h
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#include <stdlib.h>
#include "XBaseImage.h"

BOOL LoadImageiOS( LPCTSTR szFilename, int *pOutW, int *pOutH, DWORD **pOutImg );

template<typename T>
class XImageiOS : public XImage<T>
{
	int m_bpp;	// byte per pixel
	
	void Init( void ) {
		m_bpp = 0;
	}
	void Destroy( void ) {
/*		if( m_bpp == 2 ) {
			WORD *p = (WORD*)m_pTextureData;
			SAFE_DELETE_ARRAY( p );
		}
		else if( m_bpp == 4 ) {
			DWORD *p = (DWORD*)m_pTextureData;
			SAFE_DELETE_ARRAY( p );
		}
		else 
			XBREAK( m_bpp == 0 );
 */
	}
public:
	XImageiOS( BOOL bHighReso ) : XImage<T>( bHighReso ) { Init(); }
	XImageiOS( BOOL bHighReso, LPCTSTR szFilename ) : XImage<T>( bHighReso ){ Init(); Load( szFilename ); }
	~XImageiOS() { Destroy(); }

    BOOL Load( LPCTSTR szFilename ) { // A8888포맷으로 이미지 읽는 로드버전
        int w, h;
        T *pImg;
        LoadImageiOS( szFilename, &w, &h, &pImg );
        XImage<T>::SetSize( w, h );
        XImage<T>::SetTextureData( (T *)pImg );
        m_bpp = sizeof(T);
        return XImage<T>::Load( szFilename );
    }   
	BOOL Load16( LPCTSTR szFilename );
};
