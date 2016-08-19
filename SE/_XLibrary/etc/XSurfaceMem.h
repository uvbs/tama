#pragma once 

#include "global.h"
#include "XSurfaceDef.h"
#include "XGraphicsDef.h"
#include "xColor.h"
#include "XSurface.h"
//#include "XGraphicsD3DTool.h"

class XSurfaceMem : public XSurface
{
	void Init() {
	}
	void Destroy() {}
public:
	XSurfaceMem( int w, int h ) : XSurface( FALSE, (float)w, (float)h, 0, 0 ) { 
		Init(); 
		CreateSrcImg( w, h );		// 메모리 할당
	}
	XSurfaceMem( DWORD *pImg, int w, int h ) : XSurface( FALSE, (float)w, (float)h, 0, 0 ) {
		Init();
		SetSrcImg( pImg, w, h );		// 
	}
	virtual ~XSurfaceMem() { Destroy(); }
	//
	virtual xRESULT Create( const float width, const float height, const float adjx, const float adjy, xAlpha alpha, const void *pImg, int bytePerPixel, XCOLOR dwKeyColor=0, BOOL bSrcKeep = FALSE ) { XBREAKF(1, "구현되지않음"); return xFAIL; }		// 이미지의 일부분만 서피스로 만드는 버전
	virtual xRESULT Create( const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float adjx, const float adjy, void *_pSrcImg, const int bpp, BOOL bSrcKeep = FALSE ) { XBREAKF(1, "구현되지않음"); return xFAIL; }		// 이미지의 일부분만 서피스로 만드는 버전
	virtual void CopyRect( int dstx, int dsty, DWORD *pSrc, int srcw, int srch );
};

