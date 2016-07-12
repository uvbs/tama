#ifndef __XSURFACEOPENGL_H__
#define	__XSURFACEOPENGL_H__

/*
 *  XOpenglSurface.h
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 16.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "XGraphics.h"
#include "XSurface.h"

class XSurfaceOpenGL : public XSurface {
private:
	GLuint	m_textureID;
	GLint	m_format;
	GLenum	m_type;
	GLuint  m_vertexBufferID;
	GLuint	m_indexBufferID;
//	CHAR m_szFilename[ MAX_PATH ];

    xRESULT CreateVertexBuffer( float surfaceW, float surfaceH, const float _adjx, const float _adjy, int memw, int memh, int alignW, int alignH );
//	xRESULT CreateVertexBuffer( int nAlignedW, int nAlignedH, const GLsizei adjx, const GLsizei adjy );
	
	void Init();
	void Destroy();
	void Blt( const DWORD *pSrc, RECT *pSrcRect, int bytePerPixel, xAlpha alpha, float memSrcw, float memSrch, XCOLOR dwKeyColor, BOOL bSrcKeep = FALSE );
public:
	XSurfaceOpenGL( BOOL bHighReso ) : XSurface( bHighReso ) { Init(); }
	XSurfaceOpenGL( BOOL bHighReso, LPCTSTR szFilename, XCOLOR dwColorKey = XCOLOR_GREEN, BOOL bSrcKeep = FALSE ) : XSurface( bHighReso ) {
		Init();
		Create( szFilename, dwColorKey, bSrcKeep );
	}
	XSurfaceOpenGL( BOOL bHighReso, const float width, const float height, const float adjx, const float adjy, void *pImg, XCOLOR dwColorKey = XCOLOR_GREEN, BOOL bSrcKeep = FALSE ) 
        : XSurface( bHighReso ) {
		Init();
		Create( width, height, adjx, adjy, (dwColorKey)?xALPHA:xNO_ALPHA, pImg, sizeof(WORD), dwColorKey, bSrcKeep );
	}
	XSurfaceOpenGL( BOOL bHighReso, const float width, const float height, const float adjx, const float adjy, void *pImg, BOOL bSrcKeep = FALSE ) 
        : XSurface( bHighReso )	{
		Init();
		Create( width, height, adjx, adjy, xALPHA, pImg, sizeof(DWORD), 0, bSrcKeep );
	}
	XSurfaceOpenGL( BOOL bHighReso, const int srcx, const int srcy, const int srcw, const int srch, const int dstw, const int dsth, const int _adjx, const int _adjy, void *_pSrcImg, const int bpp, BOOL bSrcKeep = FALSE ) : XSurface( bHighReso )	{
		Init();
		Create( srcx, srcy, srcw, srch, dstw, dsth, _adjx, _adjy, _pSrcImg, bpp, bSrcKeep );
	}
    XSurfaceOpenGL( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep = FALSE ) 
    : XSurface( (wSurface != wTexture)? TRUE : FALSE ) {
        Init();
        XBREAK( wTexture > wSurface && wSurface * 2 != wTexture );
        XBREAK( hTexture > hSurface && hSurface * 2 != hTexture );
        Create( (float)wSurface, (float)hSurface, (float)adjx, (float)adjy, xALPHA, (const void *)pImg, 4, 0, bSrcKeep );
    }
	
	virtual ~XSurfaceOpenGL() { Destroy(); }
	
	GLuint	GetTextureID( void ) { return m_textureID;}
	GLint	GetFormat( void ) { return m_format; }
	// surface출력시 외부지정 매트릭스 설정.
	void SetTranslate( GLfloat x, GLfloat y, GLfloat z );
//	void SetRotate( GLfloat angle, GLfloat ax, GLfloat ay, GLfloat az );
	
	BOOL Create( LPCTSTR szFilename, XCOLOR dwColorKey, BOOL bSrcKeep = FALSE );
	xRESULT Create( const float width, const float height ) { return Create(width, height, 0, 0, xNO_ALPHA, NULL, 0 ); } 
	xRESULT Create( const float width, const float height, const float adjx, const float adjy, xAlpha alpha, const void *pImg, 
					int bytePerPixel, XCOLOR dwKeyColor = 0, BOOL bSrcKeep = FALSE ); // bytePerPixel=pImg의 bpp
	virtual xRESULT Create( const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float _adjx, const float _adjy, void *_pSrcImg, const int bpp, BOOL bSrcKeep = FALSE );		// 이미지의 일부분만 서피스로 만드는 버전
	BOOL CreatePNG( LPCTSTR szFilename, BOOL bSrcKeep=FALSE );
	
//	RESULT	LoadTexture( LPCTSTR szFilename, XCOLOR dwColorKey );
	void*	Lock( int *pWidth, BOOL bReadOnly = TRUE );
	
	void CopySurface( XSurface *src );
	
	void DrawCore( void );
	void DrawCoreAlpha( void );
	void DrawCoreSub( float x, float y, const RECT *src );
	void Draw( float x, float y );
    void DrawLocal( float x, float y, float lx, float ly );
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
//	void DrawScale( float x, float y, float fScale, int nAlpha=255 ) { DrawScale( x, y, m_AdjustX, /m_AdjustY, fScale, nAlpha ); }
//	void DrawScale( float x, float y, int adjx, int adjy, float fScale, int nAlpha=255 );
//	void DrawFlip( float x, float y );
	void DrawSub( float x, float y, const RECT *src );
	void Fill( XCOLOR col );
//	DWORD GetPixel( float x, float y );			
    virtual void SetTexture( void ) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
};

#endif // XSURFACEOPENGL
