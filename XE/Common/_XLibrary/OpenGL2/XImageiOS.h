/*
 *  XImageiOS.h
 *
 *  Created by xuzhu on 10. 6. 16..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#include <stdlib.h>
#include "XBaseImage.h"
//#include "XImage.h"

BOOL LoadImageiOS( LPCTSTR szFilename, int *pOutW, int *pOutH, DWORD **pOutImg );

template<typename T>
class XImageiOS : public XBaseImage<T>
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
	XImageiOS( BOOL bHighReso ) : XBaseImage<T>( bHighReso ) { Init(); }
	XImageiOS( BOOL bHighReso, LPCTSTR szFilename )
        : XBaseImage<T>( bHighReso ) {
        Init();
        Load( szFilename );
    }
	virtual ~XImageiOS() { Destroy(); }

    BOOL Load( LPCTSTR szFilename ) { // A8888포맷으로 이미지 읽는 로드버전
/*        XImage image( TRUE, szFilename );
        int w = (int)image.GetMemWidth();
        int h = (int)image.GetMemHeight();
        T *pImg;
        image.MovePtr( &pImg );
//        image.Set_pData(NULL);
        XBaseImage<T>::SetSize( w, h );
        XBaseImage<T>::SetTextureData( (T *)pImg );
        m_bpp = sizeof(T);
        return XBaseImage<T>::Load( szFilename );
        */
        int w, h;
        T *pImg;
        LoadImageiOS( szFilename, &w, &h, &pImg );
        XBaseImage<T>::SetSize( w, h );
        XBaseImage<T>::SetTextureData( (T *)pImg );
        m_bpp = sizeof(T);
        return XBaseImage<T>::Load( szFilename ); 
    }   
	BOOL Load16( LPCTSTR szFilename );
};
