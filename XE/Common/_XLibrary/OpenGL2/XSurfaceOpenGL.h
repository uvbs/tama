#pragma once
/*
*  XOpenglSurface.h
*  BnBTest
*
*  Created by xuzhu on 09. 07. 16.
*  Copyright 2009 LINKS CO.,LTD. All rights reserved.
*
*/
#include "XOpenGL.h"
#include "etc/xGraphics.h"
#include "etc/XSurface.h"
#include "etc/xMath.h"

class XSurfaceOpenGL : public XSurface {
private:
	GLuint	m_glTexture;
	GLint	m_format;
	GLenum	m_type;
#ifdef _XVAO
	GLuint  m_idVertexArray;
#endif
	GLuint  m_glVertexBuffer;
	GLuint	m_glIndexBuffer;
	//	int m_sizeByte = 0;			// vram에 로딩된 텍스쳐의 크기

	xRESULT CreateVertexBuffer( float surfaceW, float surfaceH, const float _adjx, const float _adjy, int memw, int memh, int alignW, int alignH );
	inline bool CreateVertexBuffer( const XE::VEC2& sizeSurface
																	, const XE::VEC2& vAdj
																	, const XE::POINT& sizeTexture
																	, const XE::POINT& sizeTextureAligned ) {
		auto result = CreateVertexBuffer( sizeSurface.w, sizeSurface.h, vAdj.x, vAdj.y, sizeTexture.w, sizeTexture.h, sizeTextureAligned.w, sizeTextureAligned.h );
		return result == xSUCCESS;
	}
	bool CreateVertexBuffer2( const XE::VEC2& sizeSurface, const XE::VEC2& vAdj, const XE::xRect2& rcInAtlas );
	void Init();
	void Destroy();
	bool CreatePNG( LPCTSTR szRes, bool bSrcKeep, bool bMakeMask );
protected:
public:
	XSurfaceOpenGL() : XSurface( true ) {
		Init();
	}
	XSurfaceOpenGL( BOOL bHighReso ) : XSurface( bHighReso ) {
		Init();
	}
	XSurfaceOpenGL( BOOL bHighReso
									, const int srcx, const int srcy
									, const int srcw, const int srch
									, const int dstw, const int dsth
									, const int _adjx, const int _adjy
									, void *_pSrcImg
									, const int bpp
									, BOOL bSrcKeep = FALSE );

	~XSurfaceOpenGL() {
		Destroy();
	}

	GLuint	GetTextureID( void ) {
		return m_glTexture;
	}
	GET_ACCESSOR_CONST( GLuint, glTexture );
	GET_ACCESSOR( GLuint, glVertexBuffer );
	GET_ACCESSOR( GLuint, glIndexBuffer );
	GLint	GetFormat( void ) {
		return m_format;
	}
	// surface출력시 외부지정 매트릭스 설정.
	void SetTranslate( GLfloat x, GLfloat y, GLfloat z );
	//	void SetRotate( GLfloat angle, GLfloat ax, GLfloat ay, GLfloat az );

	xRESULT CreateFromImg( const XE::VEC2& sizeSurface,
												 const XE::VEC2& vAdj,
												 const XE::VEC2& sizeMem,
												 DWORD *pImg,
												 xAlpha alpha, int bytePerPixel, BOOL bSrcKeep );
private:
	// pure virtual
	bool Create( const XE::POINT& sizeSurfaceOrig
							 , const XE::VEC2& vAdj
							 , XE::xtPixelFormat formatSurface
							 , void* const pImgSrc
							 , XE::xtPixelFormat formatImgSrc
							 , const XE::POINT& sizeMemSrc
							 , const XE::POINT& sizeMemSrcAligned
							, bool bUseAtlas ) override;
	// pure virtual
	bool CreateSub( const XE::POINT& posMemSrc
									, const XE::POINT& sizeArea
									, const XE::POINT& sizeAreaAligned
									, const XE::POINT& sizeMemSrc
									, void* const _pSrcImg
									, XE::xtPixelFormat formatImgSrc
									, const XE::VEC2& sizeRender
									, const XE::VEC2& vAdj
									, XE::xtPixelFormat formatSurface ) override;
public:
	void ClearCreated() override {
		ClearDevice();
		XSurface::ClearCreated();
	}
	void RestoreDevice() override;
	void DestroyDevice() override;
	void ClearDevice() override;
	void RestoreDeviceFromSrcImg( void );

	//	RESULT	LoadTexture( LPCTSTR szFilename, XCOLOR dwColorKey );
	void*	Lock( int *pWidth, BOOL bReadOnly = TRUE );

	void CopySurface( XSurface *src );

	void DrawCore( void );
	//	void DrawCoreAlpha( void );
	void DrawCoreSub( float x, float y, const RECT *src );
	void Draw( float x, float y ) {
		MATRIX m;
		MatrixIdentity( m );
		Draw( x, y, m );
	}
	void Draw( float x, float y, const MATRIX &mParent );
#ifdef _XBLUR
	void DrawBlur( float x, float y, const MATRIX &mParent );
#endif // _XBLUR
	void DrawLocal( float x, float y, float lx, float ly );
	void Draw( const XE::VEC2& vPos ) {
		Draw( vPos.x, vPos.y );
	}
	//	void DrawScale( float x, float y, float fScale, int nAlpha=255 ) { DrawScale( x, y, m_AdjustX, /m_AdjustY, fScale, nAlpha ); }
	//	void DrawScale( float x, float y, int adjx, int adjy, float fScale, int nAlpha=255 );
	//	void DrawFlip( float x, float y );
	void DrawSub( float x, float y, const XE::xRECTi *src ) override;
	void Fill( XCOLOR col );
	//	DWORD GetPixel( float x, float y );			
	void SetTexture( void ) override {
		glBindTexture( GL_TEXTURE_2D, m_glTexture );
	}
	bool IsEmpty() override {
		if( !m_glVertexBuffer || !m_glIndexBuffer || !m_glTexture ) {
			DestroyDevice();
			return true;
		}
		return false;
	}
	// #ifdef WIN32
	// 	void DestroyDeviceByWin32() override;
	// #endif // WIN32

};


