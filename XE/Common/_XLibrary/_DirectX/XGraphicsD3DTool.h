#pragma once
#include "XGraphicsD3D.h"
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#ifndef _XLIBPNG	// libpng를 사용하지 않을때만 wincodec을 사용한다.
#pragma warning(disable: 4005)		// 위 워닝을 끄는 명령임

//#include "DXUT.h"
//#include "SDKmisc.h"
#include <wincodec.h>

// 툴등에서 사용하는 확장판 DXUT/WIC같은걸 쓴다
// 게임에서 png를 로드하는등의 일을 해야할수도 있다. 그러므로 D3DTool이라는 이름을 쓰지말고
// D3DEX 같은이름으로 바꿔서 WIC/DXUT를 링크시켜야할때 확장된 클래스를 쓰도록 하자
class XGraphicsD3DTool : public XGraphicsD3D {
public:
	static XGraphicsD3DTool *s_pGraphicsD3DTool;	
private:
  IWICImagingFactory *m_pWICFactory;
	void Init( void );
	void Destroy( void );
public:
	XGraphicsD3DTool() { Init(); }
	XGraphicsD3DTool( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat ) 
		: XGraphicsD3D( nResolutionWidth, nResolutionHeight, pixelFormat ) {
		XLOG("");
		Init();
	}
	virtual ~XGraphicsD3DTool() { Destroy(); }
	// get/set
	XSurface*	CreateSurface( BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep = FALSE, BOOL bMakeMask=FALSE ) override;
	BOOL LoadImg( LPCTSTR szFilename , int *pWidth, int *pHeight,DWORD **ppImage );		// png 로드하여 메모리에 올린다.
};

#define	GRAPHICS_D3DTOOL	XGraphicsD3DTool::s_pGraphicsD3DTool

#endif // not _XLIBPNG	
#endif // _VER_DX
