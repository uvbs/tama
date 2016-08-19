/*
 *  XImageD3D.h
 *
 *  Created by xuzhu on 11. 5. 19..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

#include "XBaseImage.h"
#include "XGraphicsD3DTool.h"
template<typename T>
class XImageD3D : public XImage<T>
{
	void Init( void ) {	}
	void Destroy( void ) {	}
public:
	XImageD3D( BOOL bHighReso ) : XImage( bHighReso ) { Init(); }		// 이 리소스가 아이폰고해상도용인지 반드시 지정해줘야한다
	virtual ~XImageD3D() { Destroy(); }

	virtual BOOL Load( LPCTSTR szFilename ) {
		int w, h;
		T* pImg;
		if( GRAPHICS_D3DTOOL->LoadImg( szFilename, &w, &h, &pImg ) == FALSE )
			return FALSE;
		XImage<T>::SetSize( w, h );
		SetTextureData( pImg );
		return XImage::Load( szFilename );
	}
};
