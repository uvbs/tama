#pragma once
#ifdef _VER_OPENGL

/*
 *  XGraphicsOpenGL.h
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
//#define _XBLUR

#include "XOpenGL.h"

#include "etc/xGraphics.h"
#include "XSurfaceOpenGL.h"
#include "etc/xMath.h"


#define _CHECK_GL_ERROR() \
		{ volatile auto glErr = glGetError(); \
			XASSERT( glErr == GL_NO_ERROR ); }
//#define CHECK_GL_ERROR() _CHECK_GL_ERROR()
#define CHECK_GL_ERROR() (0)

namespace XE {
	enum xtAttib {
		ATTRIB_POS,
		ATTRIB_COLOR,
		ATTRIB_TEXTURE,
		ATTRIB_SIZE,
	};
extern MATRIX  x_mViewProjection;
//    extern GLKMatrix4 x_mViewProjection;
	//
	extern double x_tblGauss[ 32 ];
	extern int x_lenGaussTable;		// 가우시안 테이블의 실제 크기
	void build_mask( double* mask, int lenMask, int* n, float d );
}

class XShader {
	enum {
		UNIFORM_MVP_MATRIX,
		UNIFORM_COLOR,
		UNIFORM_FLOAT,
		NUM_UNIFORMS
	};
	GLint m_glProgram;
	GLint m_locUniforms[NUM_UNIFORMS];
	void Init() {
		m_glProgram = 0;
		XCLEAR_ARRAY( m_locUniforms );
	}
	void Destroy() {
		glDeleteProgram( m_glProgram );
	}
public:
	XShader() {
		Init();
	}
	virtual ~XShader() {
		Destroy();
	}
	GET_ACCESSOR_CONST( GLint, glProgram );
	//
	//    BOOL LoadShader( LPCTSTR szVertexShader, LPCTSTR szFragShader );
	BOOL LoadShaderFromStr( const GLchar *cVertShader, const GLchar *cFragShader, const char *cszTag );
	//    BOOL CompileShader( GLuint *shader, GLenum type, NSString *pathFile );
	BOOL CompileShaderFromString( GLuint *shader, GLenum type, const GLchar *cShader, const char *cszTag );
	BOOL LinkShader( GLuint prog, const char *cszTag );
	void SetShader( const MATRIX& mMVP,
									float r, float g, float b, float a );
	inline void SetShader( const MATRIX& mMVP, const XE::VEC4& vColor ) {
		SetShader( mMVP, vColor.x, vColor.y, vColor.z, vColor.a );
	}
	void SetUniformMVP( const MATRIX& mMVP );
	void SetUniformColor( float r, float g, float b, float a );
	void SetUniformFloat( float v );
	//	void SetColor( float r, float g, float b, float a );
	//    void SetMatrixModel( MATRIX& mModel ) {
	//        glUniformMatrix4fv(m_locUniforms[UNIFORM_MODEL_MATRIX], 1, 0, mModel.f);
	//    }
};


class XRenderTargetImpl
{
	XE::VEC2 m_Size;
	XE::xRESULT m_Error;
	void Init() {
		m_Error = XE::xSUCCESS;
	}
	void Destroy() {}
public:
	XRenderTargetImpl( float w, float h ) {
		Init();
		m_Size = XE::VEC2( w, h );
	}
	virtual ~XRenderTargetImpl() { Destroy(); }
	//
	GET_SET_ACCESSOR( XE::xRESULT, Error );
	GET_ACCESSOR( const XE::VEC2&, Size );
	//
	virtual void BindRenderTarget( void ) {}
	virtual void SetTexture( void ) {}
	virtual GLuint GetTexture( void ) { return 0; }
};
//
class XRenderTargetGLImpl : public XRenderTargetImpl
{
	GLuint m_idFBO;
	GLuint m_idRBO;
	GLuint m_idTexture;
	void Init() {
		m_idFBO = 0;
		m_idRBO = 0;
		m_idTexture = 0;
	}
	void Destroy() {
	}
public:
	XRenderTargetGLImpl( float w, float h );
	virtual ~XRenderTargetGLImpl() { Destroy(); };
	//
	virtual void BindRenderTarget( void );
	virtual void SetTexture( void );
	virtual GLuint GetTexture( void ) { return m_idTexture; }
	
};


// 렌더가능하고 텍스쳐로도 사용가능한 렌더타겟을 생성한다.
class XRenderTarget
{
	XRenderTargetImpl *m_pImpl;
	void Init() {
		m_pImpl = NULL;
	}
	void Destroy();
public:
	XRenderTarget( float w, float h );
	virtual ~XRenderTarget() { Destroy(); };
	//
	XE::xRESULT GetError( void ) {
		return m_pImpl->GetError();
	}
	XE::VEC2 GetSize( void ) {
		return m_pImpl->GetSize();
	}
	virtual void BindRenderTarget( void ) {
		m_pImpl->BindRenderTarget();
	}
	virtual void SetTexture( void ) {
		m_pImpl->SetTexture();
	}
	GLint GetTexture( void ) {
		return m_pImpl->GetTexture();
	}
};
//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XGraphicsOpenGL;
#define	GRAPHICS_GL	XGraphicsOpenGL::s_pGraphicsOpenGL

class XGraphicsOpenGL : public XGraphics
{
	static XShader *m_pCurrShader;
public:
	static XGraphicsOpenGL* sGet() { return s_pGraphicsOpenGL;	}
	static XGraphicsOpenGL *s_pGraphicsOpenGL;		// 일단 이렇게 하고 나중에 멀티플랫폼용으로 고치자.
#ifdef _XBLUR
    static GLuint s_glBlurFBO, s_glBlurRBO, s_glBlurTexture;
    static GLuint s_glBlurFBO2, s_glBlurRBO2, s_glBlurTexture2;
#endif // _XBLUR
	static void sSetShader( XShader *pShader ) {
		m_pCurrShader = pShader;
	}
	static XShader* sGetShader( void ) {
		return m_pCurrShader;
	}
	// texel format
	static GLint sToGLFormat( XE::xtPixelFormat format ) {
		switch( format ) {
		case XE::xPF_ARGB8888: 
		case XE::xPF_ARGB4444:
		case XE::xPF_ARGB1555:
			return GL_RGBA;
		case XE::xPF_RGB565:
		case XE::xPF_RGB555:
			return GL_RGB;
		default: XBREAK( 1 ); break;
		}
		return 0;
	}
	// texel type
	static GLint sToGLType( XE::xtPixelFormat format ) {
		switch( format ) {
		case XE::xPF_ARGB8888: return GL_UNSIGNED_BYTE;
		case XE::xPF_ARGB4444: return GL_UNSIGNED_SHORT_4_4_4_4;
		case XE::xPF_ARGB1555: return GL_UNSIGNED_SHORT_5_5_5_1;
		case XE::xPF_RGB565: 
		case XE::xPF_RGB555: return GL_UNSIGNED_SHORT_5_6_5;
		default: XBREAK( 1 ); break;
		}
		return 0;
	}
	static int s_numCallBindTexture;
private:
	GLuint m_defaultFrameBuffer, m_defaultRenderBuffer;
	WORD *m_pLockBackBuffer;
    XShader *m_pShaderColTex;     // 기본 쉐이더
		XShader *m_pShaderColTexAlphaTest = nullptr;     // 알파테스트 쉐이더
		XShader *m_pTextureShader;    // 컬러없이 텍스쳐만 있는 쉐이더
    XShader *m_pGrayShader = nullptr;    // 그레이스케일 쉐이더
    XShader *m_pColorShader;    // 컬러만 있는 쉐이더
    XShader *m_pBlurShaderH;
    XShader *m_pBlurShaderV;
    XShader *m_pOneColorShader;
	
	void Init( void );
	void Destroy( void );
public:
	XGraphicsOpenGL() { Init(); }
	XGraphicsOpenGL( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	virtual ~XGraphicsOpenGL() { Destroy(); }
    //
	GET_ACCESSOR( XShader*, pShaderColTex );
	GET_ACCESSOR( XShader*, pShaderColTexAlphaTest );
	GET_ACCESSOR( XShader*, pTextureShader );
	GET_ACCESSOR( XShader*, pColorShader );
	GET_ACCESSOR( XShader*, pGrayShader );
	GET_ACCESSOR( XShader*, pOneColorShader );
	GET_ACCESSOR( XShader*, pBlurShaderH );
	GET_ACCESSOR( XShader*, pBlurShaderV );
	// wSrc,hSrc크기의 pSrc이미지로 gl용 텍스쳐를 만들어 아이디를 리턴한다. pOutAligned에는 2^로 정렬된 크기를 받는다.
	GLuint CreateTextureGL( void* const pImgSrc
														, int wSrc, int hSrc
														, XE::xtPixelFormat formatImgSrc
														, int wSrcAligned, int hSrcAligned
														, XE::xtPixelFormat formatSurface ) const;
	inline GLuint CreateTextureGL( void* const pSrc
															, const XE::POINT& sizeSrc
															, XE::xtPixelFormat formatImgSrc
															, const XE::POINT& sizeSrcAligned
															, XE::xtPixelFormat formatSurface ) {
		return CreateTextureGL( pSrc, sizeSrc.w, sizeSrc.h, formatImgSrc, sizeSrcAligned.w, sizeSrcAligned.h, formatSurface );
	}
	inline GLuint CreateTextureGL( void* const pSrc, 
																 const XE::VEC2& sizeSrc, 
																 XE::xtPixelFormat formatImgSrc, 
																 const XE::VEC2& sizeSrcAligned,
																 XE::xtPixelFormat formatSurface ) {
		return CreateTextureGL( pSrc, (int)sizeSrc.w, (int)sizeSrc.h, formatImgSrc, (int)sizeSrcAligned.w, (int)sizeSrcAligned.h, formatSurface );
	}
	void ResizeTexture( ID idTex, const XE::POINT& sizeTexPrev, const XE::POINT& sizeTexNew, GLenum glType, GLenum glFormatSurface );
	GLuint CreateTextureSubGL( void* const pImgSrc
													, const XE::POINT& sizeSrc			// pImgSrc의 크기
													, const XE::xRECTi& rectSrc			// pImgSrc에서 잘라낼 영역
													, XE::xtPixelFormat formatImgSrc
													, XE::xtPixelFormat formatSurface );
  //
	virtual void*	ReadBackBuffer( int phywidth, int phyheight, void *pBuffer );
	XSurface* CreateScreenToSurface( BOOL bHighReso );
	virtual void	ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface );
	inline void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) {
		ScreenToSurface(0, 0, (int)GetLogicalScreenSize().w, (int)GetLogicalScreenSize().h, pSurface );
	}
	virtual void		UnlockBackBufferPtr( void );
	virtual void*	LockBackBufferPtr( int *pWidth, BOOL bReadOnly );
	virtual void RestoreDevice( void );
	
	xRESULT Create( void );
//	xRESULT	Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	void RestoreFrameBuffer( void );

	void	SetDefaultFrameBuffer( GLuint frameBuffer, GLuint renderBuffer ) {
        m_defaultFrameBuffer = frameBuffer;
        m_defaultRenderBuffer = renderBuffer;
    }
    GLint GetDefaultFrameBuffer( void ) {
        return m_defaultFrameBuffer;
    }
	virtual void	SetDrawTarget( XSurface *pSurface );
	virtual void	ReleaseDrawTarget( void );
    virtual void SetViewport( int left, int top, int right, int bottom );
    virtual void RestoreViewport( void );

	
	// Draw
	virtual void	ClearScreen( XCOLOR color ) ;
	virtual int		GetPixel( int x, int y ) ;
	virtual void FillRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb );
	virtual void DrawRect( float x, float y, float w, float h, XCOLOR color );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR color );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 );
    virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines );
		// x, y를 중심으로 반지름radius크기의 파이를 그린다. 시작각도angStart를 시작으로 끝각도angEnd까지 돌아간다. maxSlice이 클수록 원은 더 부드러워진다
//	void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice ) override;	
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice ); // DrawPie와 같지만 pvLines에 라인리스트를 넣어주면 그 내부에서 클리핑처리된다. numLine은 라인개수
    void DrawTexture( GLint idTexture, float x, float y, float w, float h, BOOL bBlendAdd=FALSE );
	// craete surface
//	virtual XSurface* CreateSurface( BOOL bHighReso );
	XSurface* CreateSurface() override;
	XSurface* CreateSurfaceByType( bool bAtlas, bool bBatch ) override;
	XSurface* CreateSurfaceAtlasBatch() override;
	XSurface* CreateSurfaceAtlasNoBatch() override;
	//	XSurface* CreateSurface2( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep ) override;
	// 이미지의 일부분만 서피스로 만드는 버전
	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage );
	virtual XSurface*	CreateSurface( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep );
	// pure virtual
// 	XSurface* _CreateSurface( const XE::POINT& sizeSurfaceOrig
// 													, const XE::VEC2& vAdj
// 													, XE::xtPixelFormat formatSurface								
// 													, void* const pImgSrc
// 													, XE::xtPixelFormat formatImgSrc
// 													, const XE::POINT& sizeMemSrc
// 													, bool bSrcKeep, bool bMakeMask ) override;
	//
	void DrawFan( float *pAryPos, float *pAryCol, int numVertex, int numFan ) override;
	void CopyValueSurface( XSurface* pDst, XSurface* pSrc ) override;
	static void sBindTexture( ID idTex );
}; // XGraphicsOpenGL



#endif // gl