#ifndef __XGRAPHICSOPENGL_H__
#define __XGRAPHICSOPENGL_H__

/*
 *  XGraphicsOpenGL.h
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "XGraphics.h"
#include "XSurfaceOpenGL.h"

class CKSurface;
class XGraphicsOpenGL;
#define	GRAPHICS_GL	XGraphicsOpenGL::s_pGraphicsOpenGL

class XGraphicsOpenGL : public XGraphics {
public:
	static XGraphicsOpenGL *s_pGraphicsOpenGL;		// 일단 이렇게 하고 나중에 멀티플랫폼용으로 고치자.
private:
	GLuint m_defaultFrameBuffer;
	WORD *m_pLockBackBuffer;
	
	void Init( void );
	void Destroy( void );
public:
	XGraphicsOpenGL() { Init(); }
	XGraphicsOpenGL( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	~XGraphicsOpenGL() { Destroy(); }

	virtual void*	ReadBackBuffer( int phywidth, int phyheight, void *pBuffer );
	XSurface* CreateScreenToSurface( BOOL bHighReso );
	virtual void	ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface );
	virtual void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) { ScreenToSurface(0, 0, GetScreenWidth(), GetScreenHeight(), pSurface ); }
	virtual void		UnlockBackBufferPtr( void );
	virtual void*	LockBackBufferPtr( int *pWidth, BOOL bReadOnly );
	
	xRESULT Create( void );
	xRESULT	Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	void	RestoreFrameBuffer( void );

	void	SetDefaultFrameBuffer( GLuint frameBuffer ) { m_defaultFrameBuffer = frameBuffer; }
	virtual void	SetDrawTarget( XSurface *pSurface );
	virtual void	ReleaseDrawTarget( void );
	
	// Draw
	virtual void	ClearScreen( XCOLOR color ) ;
	virtual int		GetPixel( int x, int y ) ;
	virtual void FillRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb );
	virtual void DrawRect( float x, float y, float w, float h, XCOLOR color );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR color );
    virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines );
	virtual void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice );	// x, y를 중심으로 반지름radius크기의 파이를 그린다. 시작각도angStart를 시작으로 끝각도angEnd까지 돌아간다. maxSlice이 클수록 원은 더 부드러워진다
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice ); // DrawPie와 같지만 pvLines에 라인리스트를 넣어주면 그 내부에서 클리핑처리된다. numLine은 라인개수

	// craete surface
	virtual XSurface*	CreateSurface( BOOL bHighReso, LPCSTR szFilename, XCOLOR dwColor, BOOL bSrcKeep = FALSE );
	virtual XSurface*	CreateSurface( BOOL bHighReso, int w, int h, int adjx, int adjy, WORD *pImg, XCOLOR dwColor = XCOLOR_GREEN, BOOL bSrcKeep = FALSE );
	// 이미지의 일부분만 서피스로 만드는 버전
	virtual XSurface*	CreateSurface( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep ){
		XSurface *pSurface = new XSurfaceOpenGL( bHighReso, srcx, srcy, srcw, srch, dstw, dsth, adjx, adjy, pSrcImg, 4, bSrcKeep );
		return pSurface;
	}
	virtual XSurface*	CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep = FALSE ) { return CreateSurfacePNG( bHighReso, szFilename, bSrcKeep ); }
	virtual XSurface*	CreateSurfacePNG( BOOL bHighReso, LPCSTR szFilename, BOOL bSrcKeep=FALSE );
	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage );
	virtual XSurface* CreateSurface( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE ) {
        XSurface *pSurface = new XSurfaceOpenGL( wSurface, hSurface, wTexture, hTexture, adjx, adjy, pImg, bSrcKeep );
        return pSurface;
    }
};


#endif
