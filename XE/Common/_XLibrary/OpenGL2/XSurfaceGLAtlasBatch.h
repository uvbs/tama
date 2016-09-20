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
#include "etc/xMath.h"

class XSurfaceGLAtlasBatch : public XSurface {
private:
	GLuint	m_glTexture;
	GLint	m_format;
	GLenum	m_type;
#ifdef _XVAO
	GLuint  m_idVertexArray;
#endif
	GLuint  m_glVertexBuffer;
	GLuint	m_glIndexBuffer;
	XE::xVertex m_Vertices[4];
	bool CreateVertexBuffer( float surfaceW, float surfaceH, const float _adjx, const float _adjy, int memw, int memh, int alignW, int alignH );
	inline bool CreateVertexBuffer( const XE::VEC2& sizeSurface
																	, const XE::VEC2& vAdj
																	, const XE::POINT& sizeTexture
																	, const XE::POINT& sizeTextureAligned ) {
		return CreateVertexBuffer( sizeSurface.w, sizeSurface.h, vAdj.x, vAdj.y, sizeTexture.w, sizeTexture.h, sizeTextureAligned.w, sizeTextureAligned.h );
	}
	bool CreateVertexBuffer2( const XE::VEC2& sizeSurface, const XE::VEC2& vAdj, const XE::VEC2& uvlt, const XE::VEC2& uvrb );
	void Init();
	void Destroy();
	bool CreatePNG( LPCTSTR szRes, bool bSrcKeep, bool bMakeMask );
protected:
public:
	XSurfaceGLAtlasBatch() : XSurface( true ) {
		Init();
	}
	XSurfaceGLAtlasBatch( BOOL bHighReso ) : XSurface( bHighReso ) {
		Init();
	}
	XSurfaceGLAtlasBatch( BOOL bHighReso
									, const int srcx, const int srcy
									, const int srcw, const int srch
									, const int dstw, const int dsth
									, const int _adjx, const int _adjy
									, void *_pSrcImg
									, const int bpp
									, BOOL bSrcKeep = FALSE );

	~XSurfaceGLAtlasBatch() {
		Destroy();
	}

	GLuint	GetTextureID() {
		return m_glTexture;
	}
	GET_ACCESSOR_CONST( GLuint, glTexture );
// 	GET_ACCESSOR( GLuint, glVertexBuffer );
// 	GET_ACCESSOR( GLuint, glIndexBuffer );
	GLint	GetFormat() const {
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
	void RestoreDeviceFromSrcImg();

	//	RESULT	LoadTexture( LPCTSTR szFilename, XCOLOR dwColorKey );
	void*	Lock( int *pWidth, BOOL bReadOnly = TRUE );
	void CopySurface( XSurface *src ) override;
// 	void DrawBatch( const MATRIX &mParent, const XE::xRenderParam& paramRender ) const override;
	void DrawByParam( const MATRIX &mParent, const XE::xRenderParam& paramRender ) const override;
	void DrawCore();
	//	void DrawCoreAlpha();
	void DrawCoreSub( float x, float y, const RECT *src );
	void Draw( float x, float y ) override {
		MATRIX m;
		MatrixIdentity( m );
		Draw( x, y, m );
	}
	void Draw( float x, float y, const MATRIX &mParent ) override;
	void DrawLocal( float x, float y, float lx, float ly );
// 	void Draw( const XE::VEC2& vPos ) {
// 		XBREAK( 1 );
// 		// 		Draw( vPos.x, vPos.y );
// 	}
	void DrawSub( float x, float y, const XE::xRECTi *src ) override;
	void Fill( XCOLOR col );
	void SetTexture() override;
	inline bool IsEmpty() override {
// 		if( !m_glVertexBuffer || !m_glIndexBuffer || !m_glTexture ) {
		if( !m_glTexture ) {
			DestroyDevice();
			return true;
		}
		return false;
	}
	bool IsBatch() const override {
		return true;
	}
private:
	void DrawNoBatch( float x, float y, const MATRIX &mParent ) const;
	inline void DrawNoBatch( const XE::VEC2& vPos, const MATRIX &mParent ) const {
		DrawNoBatch( vPos.x, vPos.y, mParent );
	}
};


