/*
 *  XGraphicsOpenGL.cpp
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#ifdef _VER_OPENGL
#include "XGraphicsOpenGL.h"
#include "XOpenGL.h"
#include "XSurfaceOpenGL.h"
#include "XSurfaceOpenGL2.h"
#include "XImage.h"
#include "etc/xMath.h"
#include "xShader.h"
#include "etc/Debug.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define BTRACE(F, ...)					XTRACE( _T(F), ##__VA_ARGS__ )
#define BTRACE(F, ...)					(0)

XGraphicsOpenGL* XGraphicsOpenGL::s_pGraphicsOpenGL = nullptr;
int XGraphicsOpenGL::s_numCallBindTexture = 0;
#ifdef _XBLUR
GLuint XGraphicsOpenGL::s_glBlurFBO = 0;
GLuint XGraphicsOpenGL::s_glBlurRBO = 0;
GLuint XGraphicsOpenGL::s_glBlurTexture = 0;
GLuint XGraphicsOpenGL::s_glBlurFBO2 = 0;
GLuint XGraphicsOpenGL::s_glBlurRBO2 = 0;
GLuint XGraphicsOpenGL::s_glBlurTexture2 = 0;
#endif // _XBLUR


namespace XE
{
		MATRIX  x_mViewProjection;
	
	double x_tblGauss[ 32 ];	//가우시안 테이블의 최대크기
	int x_lenGaussTable;		// 가우시안 테이블의 실제 크기
	void build_mask( double* mask, int lenMask, int* n, double d ) {
		int i;
		double var, div, sum = 0.0;
		for( int i = 0; i < lenMask; ++i )
			mask[i] = 0;
		div = pow(2*M_PI*d*d, 0.5);
		if( d <= 0 ){
			mask[0] = 1.0;
			*n = 1;
			return;
		}
		for( i = 0 ; i < lenMask ; i++ ){
			var = pow(M_E, (i*i)/(-2.0*d*d)) / div;
			if( var < 0.001 )
				break;
			mask[i] = var;
			if( i == 0 )
				sum += var;
			else
				sum += var + var;
		}
		*n = i;
#ifdef _DEBUG
		{
			double sss = 0.0;
			for( int i = 0; i < *n; ++i )
				sss += mask[i];
			BTRACE("gauss sum1:%.5f", sss);
		}
#endif
		for( i = 0 ; i < *n ; i++ )
			mask[i] /= sum;
#ifdef _DEBUG
		{
			double sss = 0.0;
			for( int i = 0; i < *n; ++i )
				sss += mask[i];
			BTRACE("gauss sum2:%.5f", sss);
			sss = 0;
			for( int i = 0; i < 3; ++i )
				sss += mask[i];
			BTRACE("size 3 sum:%.5f", sss);
		}
#endif
#ifdef _DEBUG
		for( int i = 0; i < *n; ++i )
			BTRACE("gauss tbl[%0d]:%.5f", i, mask[i]);
#endif
		
	}
};

void XE::SetProjection(float wLogicalResolution, float hLogicalResolution)
{
	MatrixMakeOrtho( x_mViewProjection,
									 0,
									 wLogicalResolution,
									 hLogicalResolution,
									 0,
									 1000.0f,
									 -1000.0f );
}


BOOL XShader::LoadShaderFromStr( const GLchar *cVertShader
																	, const GLchar *cFragShader
																	, const char *cszTag )
{
	
	GLuint vertShader, fragShader;
	BOOL bRet = FALSE;
	do {
		// Create shader program.
		m_glProgram = glCreateProgram();
		// Create and compile vertex shader.
		if( !CompileShaderFromString( &vertShader, GL_VERTEX_SHADER, cVertShader, cszTag ) ) {
			XBREAKF( 1, "Failed to compile vertex shader" );
			bRet = FALSE;
			break;
		}
		// Create and compile fragment shader.
		if( !CompileShaderFromString( &fragShader, GL_FRAGMENT_SHADER, cFragShader, cszTag ) ) {
			XBREAKF( 1, "Failed to compile fragment shader" );
			bRet = FALSE;
			break;
		}
		// Attach vertex shader to program.
		glAttachShader( m_glProgram, vertShader );
		// Attach fragment shader to program.
		glAttachShader( m_glProgram, fragShader );
		// Bind attribute locations.
		// This needs to be done prior to linking.
		glBindAttribLocation( m_glProgram, XE::ATTRIB_POS, "position" );
		glBindAttribLocation( m_glProgram, XE::ATTRIB_TEXTURE, "texture" );
		glBindAttribLocation( m_glProgram, XE::ATTRIB_COLOR, "color" );
		glBindAttribLocation( m_glProgram, XE::ATTRIB_SIZE, "size" );
		// Link program.
		if( !LinkShader( m_glProgram, cszTag ) ) {
			XTRACE( "Failed to link program: %d %s", m_glProgram, C2SZ(cszTag) );
			if( vertShader ) {
				glDeleteShader( vertShader );
				vertShader = 0;
			}
			if( fragShader ) {
				glDeleteShader( fragShader );
				fragShader = 0;
			}
			if( m_glProgram ) {
				glDeleteProgram( m_glProgram );
				m_glProgram = 0;
			}
			bRet = FALSE;
			break;
		}
		// Get uniform locations.
		// 세이더내 유저변수의 로케이션값을 받아둔다. 이 변수에다 값을 쓰려면 이 로케이션 값을 이용해야 한다.
		m_locUniforms[UNIFORM_MVP_MATRIX] = glGetUniformLocation( m_glProgram, "mMVP" );
		m_locUniforms[UNIFORM_COLOR] = glGetUniformLocation( m_glProgram, "col" );
		m_locUniforms[UNIFORM_FLOAT] = glGetUniformLocation( m_glProgram, "value" );
		//    m_locUniforms[UNIFORM_MODEL_MATRIX] = glGetUniformLocation(m_glProgram, "worldMatrix");
		// Release vertex and fragment shaders.
		if( vertShader ) {
			glDetachShader( m_glProgram, vertShader );
			glDeleteShader( vertShader );
		}
		if( fragShader ) {
			glDetachShader( m_glProgram, fragShader );
			glDeleteShader( fragShader );
		}
		bRet = TRUE;
	} while( 0 );
	// 	if( bRet )
	// 			BTRACE("success.");
	// 	else
	// 			XTRACE("failed.");
	return bRet;
}

BOOL XShader::CompileShaderFromString( GLuint *shader
																		, GLenum type
																		, const GLchar *cShader
																		, const char *cszTag )
{
	
	GLint status;
	*shader = glCreateShader( type );
	glShaderSource( *shader, 1, &cShader, nullptr );
	glCompileShader( *shader );
	GLint logLength;
	glGetShaderiv( *shader, GL_INFO_LOG_LENGTH, &logLength );
	if( logLength > 0 ) {
		GLchar *log = (GLchar *)malloc( logLength );
		glGetShaderInfoLog( *shader, logLength, &logLength, log );
		if( logLength > 0 ) {
			const _tstring strTag = C2SZ(cszTag);
			const _tstring strLog = C2SZ(log);
			XTRACE( "Shader compile log:\n    %s\n    [%s]\n", strTag.c_str(), strLog.c_str() );
		}
		free( log );
	}
	glGetShaderiv( *shader, GL_COMPILE_STATUS, &status );
	if( status == 0 ) {
		glDeleteShader( *shader );
		return NO;
	}
	return YES;
}

BOOL XShader::LinkShader( GLuint prog, const char *cszTag )
{
	
	GLint status;
	glLinkProgram( prog );
	GLint logLength;
	glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &logLength );
	if( logLength > 0 ) {
		GLchar *log = (GLchar *)malloc( logLength );
		glGetProgramInfoLog( prog, logLength, &logLength, log );
		if( logLength > 0 ) {
			const _tstring strTag = C2SZ( cszTag );
			const _tstring strLog = C2SZ( log );
			XTRACE( "Program link log:\n    %s\n    [%s]\n", strTag.c_str(), strLog.c_str() );
		}
		free( log );
	}
	glGetProgramiv( prog, GL_LINK_STATUS, &status );
	if( status == 0 ) {
		return NO;
	}
	return YES;
}

void XShader::SetShader( const MATRIX& mMVP,
						float r, float g, float b, float a )
{
	
	glUseProgram( m_glProgram );
	glUniformMatrix4fv( m_locUniforms[UNIFORM_MVP_MATRIX], 1, 0, GetMatrixPtr( mMVP ) );
	if( m_locUniforms[UNIFORM_COLOR] >= 0 )
		glUniform4f( m_locUniforms[UNIFORM_COLOR], r, g, b, a );
}

void XShader::SetUniformMVP( const MATRIX& mMVP )
{
	
	glUseProgram( m_glProgram );
	glUniformMatrix4fv( m_locUniforms[UNIFORM_MVP_MATRIX], 1, 0, GetMatrixPtr( mMVP ) );
}

void XShader::SetUniformColor( float r, float g, float b, float a )
{
	
	glUseProgram( m_glProgram );
	if( m_locUniforms[UNIFORM_COLOR] >= 0 )
		glUniform4f(m_locUniforms[UNIFORM_COLOR], r, g, b, a);
}
void XShader::SetUniformFloat( float v )
{
	
	glUseProgram( m_glProgram );
	if( m_locUniforms[UNIFORM_FLOAT] >= 0 )
		glUniform1f(m_locUniforms[UNIFORM_FLOAT], v);
}
//////////////////////////////////////////////////////////////////////////////////
XRenderTargetGLImpl::XRenderTargetGLImpl( float _w, float _h )
	: XRenderTargetImpl( _w, _h )
{
	
	Init();
	const GLsizei w = (GLsizei)_w;
	const GLsizei h = (GLsizei)_h;
	glGenRenderbuffers( 1, &m_idRBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_idRBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8_OES, w, h );
	glGenFramebuffers(1, &m_idFBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_idFBO );
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_idRBO );
	glGenTextures(1, &m_idTexture );
	XGraphicsOpenGL::sBindTexture( m_idTexture );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_idTexture, 0 );
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		SetError( XE::xFAIL );
		return;
	} else
		SetError( XE::xSUCCESS );
}

void XRenderTargetGLImpl::BindRenderTarget()
{
	
	glBindFramebuffer( GL_FRAMEBUFFER, m_idFBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_idRBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8_OES, (GLsizei)GetSize().w, (GLsizei)GetSize().h );
	glViewport( 0, 0, (GLsizei)GetSize().w, (GLsizei)GetSize().h );
	MatrixMakeOrtho( XE::x_mViewProjection, 0, GetSize().w, 0, GetSize().h, -1.0f, 1.0f );
	CHECK_GL_ERROR();
	
}

void XRenderTargetGLImpl::SetTexture()
{
	XGraphicsOpenGL::sBindTexture( m_idTexture );
}

//////////////////////////////////////////////////////////////////
XRenderTarget::XRenderTarget( float w, float h )
{
#ifdef _VER_OPENGL
	m_pImpl = new XRenderTargetGLImpl( w, h );
#endif
}

void XRenderTarget::Destroy()
{
	SAFE_DELETE( m_pImpl );
}



//===============================================================================
XShader* XGraphicsOpenGL::m_pCurrShader = nullptr;

XGraphicsOpenGL::XGraphicsOpenGL( int nResolutionWidth, 
																	int nResolutionHeight, 
																	xPixelFormat pixelFormat )
	: XGraphics( nResolutionWidth, nResolutionHeight, pixelFormat ) 
{
	Init();
	Create();
}


void XGraphicsOpenGL::Init()
{
	XGraphicsOpenGL::s_pGraphicsOpenGL = nullptr;
	m_pLockBackBuffer = nullptr;
	m_pShaderColTex = nullptr;
	m_pShaderColTexAlphaTest = nullptr;
	m_pTextureShader = nullptr;
	m_pColorShader = nullptr;
	m_pBlurShaderH = nullptr;
	m_pBlurShaderV = nullptr;
	m_pOneColorShader = nullptr;
	m_defaultFrameBuffer = 0;
	m_defaultRenderBuffer = 0;

	XE::SetProjection( GetLogicalScreenSize().w, GetLogicalScreenSize().h );

	// 가우시안 값 테이블 생성
	/*	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 3.5 );
	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 3.0 );
	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 2.5 );
	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 2.0 );
	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 1.5 );
	XE::build_mask( XE::x_tblGauss, XNUM_ARRAY( XE::x_tblGauss), &XE::x_lenGaussTable, 1.0 ); */
}

void XGraphicsOpenGL::RestoreDevice()
{
	
	XTRACE("restore device resource");
	GLint sizeMax = 512;
	CHECK_GL_ERROR();
	
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint*)&sizeMax );
	XSurface::SetMaxSurfaceWidth( sizeMax );
	XTRACE( "max texture size:%d", sizeMax );
	// 여기서 에러나면 그것은 OpenGL ES 2.0으로 초기화했기때문이다. 2.0에는 glMatrixMode라는게 없다.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Set a blending function to use
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable blending
	glEnable(GL_BLEND);
	CHECK_GL_ERROR();
	
//  	glEnable( GL_DEPTH_TEST );
//  	CHECK_GL_ERROR();
	glDisable( GL_DEPTH_TEST );
	CHECK_GL_ERROR();
	glDepthFunc( GL_GEQUAL );
	CHECK_GL_ERROR();
	glClearDepthf( -1000.f );
	CHECK_GL_ERROR();

	// 세이더 로딩
	XTRACE("col tex shader loading....");
	m_pShaderColTex->LoadShaderFromStr( xvShaderColTex,
																			xfShaderColTex,
																			"base_shader" );
	XTRACE( "col tex alpha test shader loading...." );
	m_pShaderColTexAlphaTest->LoadShaderFromStr( xvShaderColTex,
																							 xfShaderColTexAlphaTest,
																							 "alpha_test_shader" );
	XTRACE("texture shader loading....");
	m_pTextureShader->LoadShaderFromStr( xvShader_Texture, xfShader_Texture, "texture_shader" );
	XTRACE("gray shader loading....");
	m_pGrayShader->LoadShaderFromStr( xvShaderColTex, xfShader_Gray, "gray_shader" );
	XTRACE("color shader loading....");
	m_pColorShader->LoadShaderFromStr( xvShader_Color, xfShader_Color, "color_shader");
	XTRACE("onecolor shader loading....");
	m_pOneColorShader->LoadShaderFromStr( xvShader_OneColor, xfShader_OneColor, "one_color shader" );
	XTRACE("blurH shader loading....");
	m_pBlurShaderH->LoadShaderFromStr( xvShaderColTex, xfShader_blurH, "blurH_shader" );
	XTRACE("blurV shader loading....");
	m_pBlurShaderV->LoadShaderFromStr( xvShaderColTex, xfShader_blurV, "blurV_shader" );
	// 현재 쉐이더를 지정
	XGraphicsOpenGL::sSetShader( m_pShaderColTex );
	
}

void XGraphicsOpenGL::Destroy()
{
	SAFE_DELETE( m_pShaderColTex );
	SAFE_DELETE( m_pShaderColTexAlphaTest );
	SAFE_DELETE( m_pGrayShader );
	SAFE_DELETE( m_pTextureShader );
	SAFE_DELETE( m_pColorShader );
	SAFE_DELETE( m_pOneColorShader );
	SAFE_DELETE( m_pBlurShaderH );
	SAFE_DELETE( m_pBlurShaderV );
#ifdef _XBLUR
	glDeleteFramebuffers( 1, &s_glBlurFBO );
	glDeleteRenderbuffers( 1, &s_glBlurRBO );
	glDeleteTextures( 1, &s_glBlurTexture );
	glDeleteFramebuffers( 1, &s_glBlurFBO2 );
	glDeleteRenderbuffers( 1, &s_glBlurRBO2 );
	glDeleteTextures( 1, &s_glBlurTexture2 );
#endif // _XBLUR
}

xRESULT XGraphicsOpenGL::Create()
{
	XGraphicsOpenGL::s_pGraphicsOpenGL = this;
	//
	m_pShaderColTex = new XShader;
	m_pShaderColTexAlphaTest = new XShader;
	m_pGrayShader = new XShader;
	m_pTextureShader = new XShader;
	m_pColorShader = new XShader;
	m_pOneColorShader = new XShader;
	m_pBlurShaderH = new XShader;
	m_pBlurShaderV = new XShader;

	return xSUCCESS;
}

void XGraphicsOpenGL::RestoreFrameBuffer()
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFrameBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_defaultRenderBuffer );
	glViewport(0, 0, GetPhyScreenWidth(), GetPhyScreenHeight());
		XE::SetProjection( GetLogicalScreenSize().w, GetLogicalScreenSize().h );
}

#pragma mark pure virtual
// 백버퍼의 픽셀을 읽어서 pBuffer에 담아줌.
// m_nWidth는 논리적서피스의 크기이고 work와 같고, back,frame 버퍼는 실제물리적 해상도인것으로 구분해줄 필요 있다.
void*	XGraphicsOpenGL::ReadBackBuffer( int phywidth, int phyheight, void *pBuffer )
{
	
	if( GetPixelFormat() != xPIXELFORMAT_RGB565 )
		XERROR( "아직은 RGB565포맷만 사용가능함." );
//	glReadBufferOES( GL_BACK );	// gl1.2 ES엔 없는듯.
		// gl1.x에선 프론트가 아니고 백버퍼만 읽을수 있는듯 하다. 잘못알고 있었음-_-;;
	glReadPixels(0, 0, phywidth, phyheight, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pBuffer );
	return pBuffer;
}

//
// 지정된영역의 백버퍼데이타를 읽어서 surface에 옮긴다.
// GL은 프론트만 읽을수 있는줄알았는데 반대였다. 백버퍼만 읽을수 있다
// w,y,width,height: 스크린 좌표
void	XGraphicsOpenGL::ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface )
{
	
	if( GetPixelFormat() != xPIXELFORMAT_RGB565 )
		XERROR( "아직은 RGB565포맷만 사용가능함." );
	int px, py, pw, ph;		// 물리적스크린크기와 좌표.
	float fRatioX, fRatioY;
	// phyScreen과 screen의 비율로 좌표들을 변환.
	fRatioX = GetRatioWidth();
	fRatioY = GetRatioHeight();
	px = (int)(x * fRatioX);		py = (int)(y * fRatioY);	// screen->physcreen 좌표로 변환
	pw = (int)(width * fRatioX);	ph = (int)(height * fRatioY);
	//GL_NO_ERROR
	DWORD *pdwBuffer = new DWORD[ pw * ph ];	
	glReadPixels(px, py, pw, ph, GL_RGBA, GL_UNSIGNED_BYTE, pdwBuffer );	// 위아래 뒤집혀진 이미지
	DWORD *pDst = new DWORD[ pw * ph ];
		// 위아래를 바꿈
	for( int i = 0; i < ph; i ++ )
		for( int j = 0; j < pw; j ++ )
			pDst[ i * pw + j ] = pdwBuffer[ (ph-1-i) * pw + j ];	
	SAFE_DELETE_ARRAY( pdwBuffer );

	pSurface->Create( (float)pw, (float)ph, 0, 0, xALPHA, pDst, sizeof(DWORD), 0, FALSE, FALSE );
	XBREAK(1);		// 여기 쓰는데 있음?

#pragma messages( "pdwBuffer를 XSurfaceOpenGL::Create()내부에서 뽀개주도록 바꿔야 한다. 버그날까봐 일단 이상태로 놔둠" )
}

void XGraphicsOpenGL::SetDrawTarget( XSurface *pSurface )
{
}
void XGraphicsOpenGL::ReleaseDrawTarget()
{
}

XSurface* XGraphicsOpenGL::CreateScreenToSurface( BOOL bHighReso )
{
	
	XSurface *pSurface = new XSurfaceOpenGL( FALSE );
	ScreenToSurface( 0, 0, (int)GetLogicalScreenSize().w, (int)GetLogicalScreenSize().h, pSurface );
	return pSurface;
}

XSurface*	XGraphicsOpenGL::CreateSurface( BOOL bHighReso
																				, int srcx, int srcy
																				, int srcw, int srch
																				, float dstw, float dsth
																				, float adjx, float adjy
																				, DWORD *pSrcImg
																				, BOOL bSrcKeep )
{
	
	XSurface *pSurface = new XSurfaceOpenGL( bHighReso
																				, srcx, srcy
																				, srcw, srch
																				, (int)dstw, (int)dsth
																				, (int)adjx, (int)adjy
																				, pSrcImg
																				, 4
																				, bSrcKeep );
	return pSurface;
}

// XSurface*	XGraphicsOpenGL::CreateSurface2( BOOL bHighReso, 
// 																					 int srcx, int srcy, 
// 																					 int srcw, int srch, 
// 																					 float dstw, float dsth, 
// 																					 float adjx, float adjy, 
// 																					 DWORD *pSrcImg, 
// 																					 BOOL bSrcKeep )
// {
// 	auto pSurface = new XSurfaceOpenGL2( bHighReso,
// 																			srcx, srcy,
// 																			srcw, srch,
// 																			(int)dstw, (int)dsth,
// 																			(int)adjx, (int)adjy,
// 																			pSrcImg,
// 																			4,
// 																			bSrcKeep );
// 	return pSurface;
// }

/**
 @brief 구버전
*/
XSurface* XGraphicsOpenGL::CreateSurface()
{
	return new XSurfaceOpenGL();
}

/**
 @brief 배치렌더 버전
*/
XSurface* XGraphicsOpenGL::CreateSurface2()
{
	return new XSurfaceOpenGL2();
}

BOOL XGraphicsOpenGL::LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage )
{
	
	XImage image(TRUE);
	BOOL bRet = FALSE;
	if( image.Load( szFilename ) ) {
		*pWidth = image.GetMemWidth();
		*pHeight = image.GetMemHeight();
		if( (*pWidth) & 1 ) {
			XERROR( "경고: %s의 가로크기가 2로 나누어 떨어지지 않습니다", szFilename );
		}
		DWORD **pp = (DWORD **)ppImage;
		image.MovePtr( pp );
		*ppImage = (DWORD *)*pp;
		bRet = TRUE;
	}
	return bRet;
}

void XGraphicsOpenGL::SetViewport( int left, int top, int right, int bottom )
{
	XGraphics::SetViewport( left, top, right, bottom );
	//
	float ratioX = GetRatioWidth();
	float ratioY = GetRatioHeight();
	int w = (int)( ( right - left ) * ratioX );
	int h = (int)( ( bottom - top ) * ratioY );
	int x = (int)( left * ratioX );
	int y = (int)( ( GetLogicalScreenSize().h - ( top + ( bottom - top ) ) ) * ratioY );
#ifdef _XDYNA_RESO
	x += (int)( GetvScreenLT().x * ratioX );
	y -= (int)( GetvScreenLT().y * ratioY );
#endif
	glViewport( x, y, w, h );	// EAGLView에서 프레임버퍼 바인드할때 하도록 바꿔라
	CHECK_GL_ERROR();
	
	//    glViewport( 100, 100, 200, 200 );	// EAGLView에서 프레임버퍼 바인드할때 하도록 바꿔라
}
void XGraphicsOpenGL::RestoreViewport()
{
		SetViewport( m_ptViewportStackLT.x, m_ptViewportStackLT.y, m_ptViewportStackRB.x, m_ptViewportStackRB.y );
}
#pragma mark Draw tile

// virtual draw function
void XGraphicsOpenGL::ClearScreen( XCOLOR color )
{
	
	float r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.f;
	g = XCOLOR_RGB_G(color) / 255.f;
	b = XCOLOR_RGB_B(color) / 255.f;
	a = XCOLOR_RGB_A(color) / 255.f;
	glClearColor( r, g, b, a );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	CHECK_GL_ERROR();
	
}
int	 XGraphicsOpenGL::GetPixel( int x, int y )
{
	return 0;
}
void XGraphicsOpenGL::FillRect( float x, float y, float w, float h, XCOLOR color )
{
	
	CHECK_GL_ERROR();
	
	if( w == 0 || h == 0 )	return;
		if( w < 0 )    {
				w = -w;     // w는 부호 바꿈
				x -= w;     // 좌측 좌표를 -w만큼 이동시켜주고
		}
		if( h < 0 )    {
				h = -h;
				y -= h;
		}
	if( x > GetViewportRight() || y > GetViewportBottom() )
		return;
	if( x + w < 0 || y + h < 0 )
		return;
	
	GLfloat r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.0f;
	a = XCOLOR_RGB_A(color) / 255.0f;
	
	// width-1이 맞나? 안하는게 맞나?
	GLfloat pos[8] = { 0, h, w, h, 0, 0, w, 0 };
	GLfloat col[16] = {  r,g,b,a,
						 r,g,b,a,
						 r,g,b,a,
						 r,g,b,a	};
		MATRIX mMVP;
		MATRIX mModel;
		MatrixTranslation( mModel, x, y, 0 );
		MatrixMultiply( mMVP, mModel, XE::x_mViewProjection );
		GetpColorShader()->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );

	glEnable(GL_BLEND);
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
//    glDisableVertexAttribArray( XE::ATTRIB_TEXTURE );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	CHECK_GL_ERROR();
	

//	glEnable( GL_TEXTURE_2D );
 
		
}


void XGraphicsOpenGL::FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb  )
{
	
	CHECK_GL_ERROR();
	
#define _XXR(C)	(XCOLOR_RGB_R(C) / 255.0f)
#define _XXG(C)	(XCOLOR_RGB_G(C) / 255.0f)
#define _XXB(C)	(XCOLOR_RGB_B(C) / 255.0f)
#define _XXA(C)	(XCOLOR_RGB_A(C) / 255.0f)
	if( w == 0 || h == 0 )	return;
		if( w < 0 )
		{
				x -= w;     // 좌측 좌표를 -w만큼 이동시켜주고
				w = -w;     // w는 부호 바꿈
		}
		if( h < 0 )
		{
				y -= h;
				h = -h;
		}
	if( x > GetViewportRight() || y > GetViewportBottom() )
		return;
	if( x + w < 0 || y + h < 0 )
		return;
	
	//	if( x > GetScreenWidth() || y > GetScreenHeight() )
	//		return;
	//	if( w < 0 || h < 0 )
	//		return;
	
	
//	GLfloat r, g, b, a;
//	r = XCOLOR_RGB_R(color) / 255.0f;
//	g = XCOLOR_RGB_G(color) / 255.0f;
//	b = XCOLOR_RGB_B(color) / 255.0f;
//	a = XCOLOR_RGB_A(color) / 255.0f;
	//	if( a != 255 )	glEnable(GL_BLEND);		// 이거 자주불러주면 부하걸릴거 같다. 외부에서 블럭단위로 셋하게 하자.
	
	// width-1이 맞나? 안하는게 맞나?
	GLfloat pos[8] = { 0, h, w, h, 0, 0, w, 0 } ;
	GLfloat col[16] = {  _XXR(collb), _XXG(collb),_XXB(collb),_XXA(collb),	// 좌하
						 _XXR(colrb), _XXG(colrb),_XXB(colrb),_XXA(colrb),	// 우하
						 _XXR(collt), _XXG(collt),_XXB(collt),_XXA(collt),	// 좌상
						 _XXR(colrt), _XXG(colrt),_XXB(colrt),_XXA(colrt)	};	// 우상
		MATRIX mModel;
		MATRIX mMVP;
		MatrixTranslation( mModel, x, y, 0 );
		MatrixMultiply( mMVP, mModel, XE::x_mViewProjection );
		GetpColorShader()->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );
//    GetpColorShader()->SetMatrixModel( mModel );
		
	glEnable(GL_BLEND);
//	glDisable( GL_TEXTURE_2D );
#ifdef _XVAO
		glBindVertexArray( 0 );
#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray( XE::ATTRIB_TEXTURE );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	CHECK_GL_ERROR();
	

//	glEnable( GL_TEXTURE_2D );
	
}

void XGraphicsOpenGL::DrawRect( float x, float y, float w, float h, XCOLOR color )
{
	
	CHECK_GL_ERROR();
	
	//	if( x > GetScreenWidth() || y > GetScreenHeight() )
	if( x > GetViewportRight() || y > GetViewportBottom() )
		return;
	if( w < 0 || h < 0 )
		return;
	
	
	GLfloat r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.0f;
	a = XCOLOR_RGB_A(color) / 255.0f;
	//	if( a != 255 )	glEnable(GL_BLEND);		// 이거 자주불러주면 부하걸릴거 같다. 외부에서 블럭단위로 셋하게 하자.
	
	// width-1이 맞나? 안하는게 맞나?
//	GLfloat pos[8] = { 0, h, w, h, 0, 0, w, 0 };
	GLfloat pos[10] = { 0, 0, w, 0, w, h, 0, h, 0, 0 };
	GLfloat col[20] = {  r,g,b,a,
		r,g,b,a,
		r,g,b,a,
		r,g,b,a,
		r,g,b,a	};
		MATRIX mModel;
		MATRIX mMVP;
		MatrixTranslation( mModel, x, y, 0 );
		MatrixMultiply( mMVP, mModel, XE::x_mViewProjection );
		GetpColorShader()->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );
//    GetpColorShader()->SetMatrixModel( mModel );
	glLineWidth( GetLineWidth() );
//	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
	
	glDrawArrays(GL_LINE_STRIP, 0, 5);
		
//	glEnable( GL_TEXTURE_2D );	
	CHECK_GL_ERROR();
	
	}

void XGraphicsOpenGL::DrawLine( float x1, float y1, float x2, float y2, XCOLOR color )
{
	
	CHECK_GL_ERROR();
	
	if( x1 > GetViewportRight() || y1 > GetViewportBottom() )
		return;
	if( x2 < 0 || y2 < 0 )
		return;
	
	
	GLfloat r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.0f;
	a = XCOLOR_RGB_A(color) / 255.0f;
	//	if( a != 255 )	glEnable(GL_BLEND);		// 이거 자주불러주면 부하걸릴거 같다. 외부에서 블럭단위로 셋하게 하자.
	
	// width-1이 맞나? 안하는게 맞나?
//	x1 = ((int)x1) - 0.5f;
//	y1 = ((int)y1) - 0.5f;
//	x2 = ((int)x2) - 0.5f;
//	y2 = ((int)y2) - 0.5f;
	GLfloat pos[4] = { x1, y1, x2, y2 };
	GLfloat col[8] = {  r,g,b,a,  r,g,b,a	};
		GetpColorShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
//    MATRIX mModel;
//    MatrixIdentity( mModel );
//    GetpColorShader()->SetMatrixModel( mModel );
	glLineWidth( GetLineWidth() );
	glEnable(GL_BLEND);
//	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
	
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	CHECK_GL_ERROR();
	

//	glEnable( GL_TEXTURE_2D );
}
void XGraphicsOpenGL::DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 )
{
	
	CHECK_GL_ERROR();
	
	if( x1 > GetViewportRight() || y1 > GetViewportBottom() )
		return;
	if( x2 < 0 || y2 < 0 )
		return;
	
	
	GLfloat r1, g1, b1, a1;
	r1 = XCOLOR_RGB_R(col1) / 255.0f;
	g1 = XCOLOR_RGB_G(col1) / 255.0f;
	b1 = XCOLOR_RGB_B(col1) / 255.0f;
	a1 = XCOLOR_RGB_A(col1) / 255.0f;
	GLfloat r2, g2, b2, a2;
	r2 = XCOLOR_RGB_R(col2) / 255.0f;
	g2 = XCOLOR_RGB_G(col2) / 255.0f;
	b2 = XCOLOR_RGB_B(col2) / 255.0f;
	a2 = XCOLOR_RGB_A(col2) / 255.0f;
	
	GLfloat pos[4] = { x1, y1, x2, y2 };
	GLfloat col[8] = {  r1,g1,b1,a1,  r2,g2,b2,a2	};
		GetpColorShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
	float ratio = GetPhyScreenWidth() / GetLogicalScreenSize().w;
	float wLine = GetLineWidth() * ratio;
	glLineWidth( wLine );
	glEnable(GL_BLEND);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
	
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	CHECK_GL_ERROR();
	

}

void XGraphicsOpenGL::DrawLineList( XGraphics::xVERTEX *vList, int numLines )
{
	
	CHECK_GL_ERROR();
	
	// 클리핑
		// 버텍스버퍼를 생성
		static GLuint s_glVB=0;
		if( s_glVB == 0 )
				glGenBuffers( 1, &s_glVB );
		// 버텍스버퍼에 라인정보 밀어넣음
		XBREAK( s_glVB == 0 );
		GetpColorShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
//    MATRIX mModel;
//    MatrixIdentity( mModel );
//    GetpColorShader()->SetMatrixModel( mModel );
	glEnable(GL_BLEND);
//    glDisable( GL_TEXTURE_2D );
		glBindBuffer( GL_ARRAY_BUFFER, s_glVB );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof(XGraphics::xVERTEX), 0);
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(XGraphics::xVERTEX), (GLvoid*)(sizeof(GL_FLOAT)*2));
		glBufferData( GL_ARRAY_BUFFER, sizeof(XGraphics::xVERTEX) * numLines * 2, vList, GL_DYNAMIC_DRAW );
		
	float ratio = GetPhyScreenWidth() / GetLogicalScreenSize().w;
	float wLine = GetLineWidth() * ratio;
	glLineWidth( wLine );
		glDrawArrays( GL_LINES, 0, numLines * 2 );
		
//     glEnable( GL_TEXTURE_2D );    
		CHECK_GL_ERROR();
		
}


 #define MAX_VERTEX	256
 #define POSX(I)	pos[I*2+0]
 #define POSY(I)	pos[I*2+1]
 #define POS(I,X,Y) {POSX(I)=X; POSY(I)=Y;}
#define COLOR_R(I) col[I*4+0]
#define COLOR_G(I) col[I*4+1]
#define COLOR_B(I) col[I*4+2]
#define COLOR_A(I) col[I*4+3]
#define COLOR(I,R,G,B,A) {COLOR_R(I) = R;	COLOR_G(I) = G;	COLOR_B(I) = B;	COLOR_A(I) = A;}

void XGraphicsOpenGL::DrawFan( float *pAryPos, float *pAryCol, int numVertex, int numFan )
{
	
	CHECK_GL_ERROR();
	
	GetpColorShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
//	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pAryPos);
	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, pAryCol);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifdef _XVAO
	glBindVertexArrayOES( 0 );
#endif
	glDrawArrays(GL_TRIANGLE_FAN, 0, numVertex);	// i==버텍스개수
//	glEnable( GL_TEXTURE_2D );    
	CHECK_GL_ERROR();
	
}


static XE::VEC2 _vLists[ MAX_VERTEX ];

void XGraphicsOpenGL::DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice )
{
	
	CHECK_GL_ERROR();
	
	if( angStart == angEnd )
		return;
	XE::VEC2 *pvList = _vLists;
	float angSlice = 360.0f / (float)maxSlice;		// 
	float ang = 0;
	
	pvList->x = x;	pvList->y = y;	// 파이의 중심점
	pvList++;
	XE::VEC2 vOut;
	XE::VEC2 vo;
	vo.x = x + (sinf(D2R(angStart)) * radius);		// 시작각도 버텍스 하나 만들어줌
	vo.y = y + (-cosf(D2R(angStart)) * radius);
	if( ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
		*pvList = vOut;
	else
		*pvList = vo;
	pvList++;
	ang += angSlice;
	const XE::VEC2 *pEnd = &_vLists[ MAX_VERTEX ];
	int num = 0;
	while( ang < angEnd )
	{
		if( ang >= angStart )		// 각도범위에 포함되면 버텍스를 추가
		{
			float rAng = D2R(ang);		// 디그리 각도를 라디안각도로 변환
			vo.x = x + (sinf(rAng) * radius);
			vo.y = y + (-cosf(rAng) * radius);
			if( ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
				*pvList = vOut;
			else
				*pvList = vo;
			pvList++;
			num++;		// 삼각형 개수
			if( XBREAK(pvList > pEnd) )		// 버퍼 오버플로우 되지 않도록
				break;
		}
		ang += angSlice;
	}
	// 마지막각도에 버텍스 하나 더 추가
	vo.x = x + (sinf(D2R(angEnd)) * radius);
	vo.y = y + (-cosf(D2R(angEnd)) * radius);
	if( ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
		*pvList = vOut;
	else
		*pvList = vo;
	num++;
	
	// gl버텍스버퍼에 옮김
	{
				GLfloat r, g, b, a;
				r = XCOLOR_RGB_R(color) / 255.0f;
				g = XCOLOR_RGB_G(color) / 255.0f;
				b = XCOLOR_RGB_B(color) / 255.0f;
				a = XCOLOR_RGB_A(color) / 255.0f;
				
						GLfloat pos[MAX_VERTEX * 2]={0,};
						GLfloat col[MAX_VERTEX * 4]={0,};
				
		//	float ratioX = (GetRatioWidth()) ;
		//	float ratioY = (GetRatioHeight());
				for( int i = 0; i < num+2; i ++ )	// num은 삼각형개수고 +2를 해야 버텍스 개수다
				{
						POS(i, _vLists[i].x, _vLists[i].y);
						COLOR(i,r,g,b,a);
		//		m_aVertex[i].vPos.x = _vLists[i].x * ratioX;
		//		m_aVertex[i].vPos.y = _vLists[i].y * ratioY;
		//		m_aVertex[i].dwColor = color;
				}
	
				GetpColorShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
//        MATRIX mModel;
//        MatrixIdentity( mModel );
//        GetpColorShader()->SetMatrixModel( mModel );
				
//         glDisable( GL_TEXTURE_2D );
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glEnableVertexAttribArray( XE::ATTRIB_POS );
				glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos);
				glEnableVertexAttribArray( XE::ATTRIB_COLOR );
				glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifdef _XVAO
				glBindVertexArrayOES( 0 );
#endif
				
				glDrawArrays(GL_TRIANGLE_FAN, 0, num+2);
				
//         glEnable( GL_TEXTURE_2D );    
	}
	CHECK_GL_ERROR();
	
}
void XGraphicsOpenGL::DrawTexture( GLint idTexture, 
																	 float xpos, float ypos, 
																	 float width, float height, 
																	 BOOL bBlendAdd )
{
	
	CHECK_GL_ERROR();
	
	GLfloat tex[8] = { 0, 1.0f, 1.0f, 1.0f, 0, 0, 1.0f, 0 };
	GLfloat pos[8] = { xpos, ypos + height, xpos + width, ypos + height, xpos, ypos, xpos + width, ypos };
	GLfloat col[16] = { 
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f };
	MATRIX mTrans, mScale, mMVP;
	MatrixTranslation( mTrans, xpos, ypos, 0 );
	MatrixScaling( mScale, 1.f, 1.f, 1.f );
	MatrixIdentity( mMVP );
	MatrixMultiply( mMVP, mMVP, mScale );
	MatrixMultiply( mMVP, mMVP, mTrans );
	MatrixMultiply( mMVP, mMVP, XE::x_mViewProjection );
	// 8픽셀 크기로 블러가 되게 하려고 8픽셀 여유를 뒀다.
	// 현재 쉐이더를 얻어온다
	GRAPHICS_GL->sSetShader( GRAPHICS_GL->GetpShaderColTex() );
	XShader *pShader = XGraphicsOpenGL::sGetShader();
	pShader->SetUniformMVP( mMVP );
	pShader->SetUniformColor( 1.0f, 1.0f, 1.0f, 1.0f );
	//	pShader->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );
	//    GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );
	//    GRAPHICS_GL->GetpBaseShader()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );

	CHECK_GL_ERROR();
	
	glEnable( GL_BLEND );
	if( bBlendAdd )
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	else
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef _XVAO
	glBindVertexArrayOES( 0 );
#endif
	CHECK_GL_ERROR();
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
	glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
	glEnableVertexAttribArray( XE::ATTRIB_SIZE );
	CHECK_GL_ERROR();
	
	// bind texture
	XGraphicsOpenGL::sBindTexture( idTexture );
	CHECK_GL_ERROR();
	

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	CHECK_GL_ERROR();
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	CHECK_GL_ERROR();
	
}
//////////////////////////////
#pragma mark create manager obj
/*
CSpriteManagerGL* XGraphicsOpenGL::CreateSprMngObj()
{
	CSpriteManagerGL *pSprMng;// = new CSpriteManagerGL;
	return pSprMng;
}
*/


void*	XGraphicsOpenGL::LockBackBufferPtr( int *pWidth, BOOL bReadOnly )
{
	
	int pw, ph;
	pw = GetPhyScreenWidth();
	ph = GetPhyScreenHeight();
	DWORD *pdwBuffer = new DWORD[pw*ph];
	glReadPixels(0, 0, pw, ph, GL_RGBA, GL_UNSIGNED_BYTE, pdwBuffer );
	
	WORD *pBuffer = new WORD[pw*ph];
	m_pLockBackBuffer = pBuffer;
	int i, j;
	WORD *_pBuffer = pBuffer;
	DWORD dwData;
	for( i = ph-1; i >= 0; i -- )
		for( j = 0; j < pw; j ++ )
		{
			dwData = pdwBuffer[i*pw+j];
			BYTE r = XCOLOR_RGB_R(dwData);
			BYTE g = XCOLOR_RGB_G(dwData);
			BYTE b = XCOLOR_RGB_B(dwData);
			*_pBuffer++ = XCOLOR_RGB565(r, g, b);
		}
	SAFE_DELETE_ARRAY( pdwBuffer );
	*pWidth = pw;
	return (void*)m_pLockBackBuffer;
}

void XGraphicsOpenGL::UnlockBackBufferPtr()
{
	
	SAFE_DELETE_ARRAY( m_pLockBackBuffer );
}

int s_glFmt = 0;
int s_glType = 0;
/**
 @brief openGL 디바이스 텍스쳐를 생성하고 pSrc의 이미지를 로딩시킨다.
*/
GLuint XGraphicsOpenGL::CreateTextureGL( void* const pImgSrc
																			, int wSrc, int hSrc
																			, XE::xtPixelFormat formatImgSrc
																			, int wSrcAligned, int hSrcAligned
																			, XE::xtPixelFormat formatSurface ) const
{
	
	//	XBREAK( pImgSrc == nullptr );
	const int bppImgSrc = XE::GetBpp( formatImgSrc );
	const int bppSurface = XE::GetBpp( formatSurface );
	const auto glFormatSurface = XGraphicsOpenGL::sToGLFormat( formatSurface );
	const auto glTypeSurface = XGraphicsOpenGL::sToGLType( formatSurface );
	XBREAK( pImgSrc && bppImgSrc == 2 );		// 아직 16bit픽셀소스는 지원하지 않음.
	GLuint idTexture = 0;
	glGenTextures( 1, &idTexture );
	CHECK_GL_ERROR();
	
	if( idTexture == 0 ) {
		XERROR( "failed create texture: sizeSrc(%d,%d)", wSrc, hSrc );
		return 0;
	}
	XGraphicsOpenGL::sBindTexture( idTexture );
	CHECK_GL_ERROR();
	const int sizeSrcAligned = wSrcAligned * hSrcAligned;
	//////////////////////////////////////////////////////////////////////////
	if( bppSurface == 4 ) {
		DWORD *temp = nullptr;
		if( pImgSrc ) {
			// gl텍스쳐로 쓰기 위해 정렬된 사이즈의 메모리에 이미지를 복사
			DWORD* _temp = temp = new DWORD[sizeSrcAligned];
			memset( _temp, 0, sizeSrcAligned * bppSurface );
			int i, j;
			DWORD *src = (DWORD* const)pImgSrc;
			for( i = 0; i < hSrc; i++ ) {
				for( j = 0; j < wSrc; j++ ) {
					*_temp++ = *src++;
				}
				_temp += (wSrcAligned - wSrc);
			}
		}
		glTexImage2D( GL_TEXTURE_2D,
									0,
									GL_RGBA,
									wSrcAligned,
									hSrcAligned,
									0,
									GL_RGBA,
									GL_UNSIGNED_BYTE,
									temp );
		s_glFmt = GL_RGBA;
		s_glType = GL_UNSIGNED_BYTE;
#ifdef _DEBUG
		auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR );
#endif // _DEBUG
		SAFE_DELETE_ARRAY( temp );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( bppSurface == 2 ) {
		WORD *pDst = nullptr;
			// gl텍스쳐로 쓰기 위해 정렬된 사이즈의 메모리에 이미지를 복사
		if( pImgSrc ) {
			WORD *_pDst = pDst = new WORD[ sizeSrcAligned ];
			memset( _pDst, 0, sizeSrcAligned * bppSurface );
			if( formatSurface == XE::xPF_RGB565 
				|| formatSurface == XE::xPF_RGB555 ) {
				XE::ConvertBlockABGR8888ToRGB565( pDst, wSrcAligned, hSrcAligned, (DWORD*)pImgSrc, wSrc, hSrc );
			} else
			if( formatSurface == XE::xPF_ARGB4444 ) {
				// 현재 WIN32 opengl에서는 RGBA4444로 넘겨야 제대로 나오지만 기기 opengl에서는 어떤게 맞는지 알수없음. 아마도 같지 않을까 함.
				XE::ConvertBlockABGR8888ToRGBA4444( pDst, wSrcAligned, hSrcAligned, (DWORD*)pImgSrc, wSrc, hSrc );
			} else
			if( formatSurface == XE::xPF_ARGB1555 ) {
				XE::ConvertBlockABGR8888ToRGBA1555( pDst, wSrcAligned, hSrcAligned, (DWORD*)pImgSrc, wSrc, hSrc );
			} else {
				XBREAK(1);
			}
		}
		const auto glFormatImgSrc = glFormatSurface;
		const auto glTypeSrc = glTypeSurface;
		glTexImage2D( GL_TEXTURE_2D,
									0,
									glFormatSurface,		// internal format
									wSrcAligned,
									hSrcAligned,
									0,
									glFormatImgSrc,	// pImgSrc의 포맷이지만 위에거랑 맞춰야 해서 같은걸 씀.
									glTypeSrc,
									pDst );
		s_glFmt = glFormatSurface;
		s_glType = glTypeSrc;
		CHECK_GL_ERROR();
		SAFE_DELETE_ARRAY( pDst );
	}
// 	XGraphicsOpenGL::sBindTexture( 0 );
	CHECK_GL_ERROR();
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//
	XSurface::sAddSizeTotalVMem( sizeSrcAligned * bppSurface );
	CHECK_GL_ERROR();
	
	//
	return idTexture;
} // CreateTextureGL


/**
 @brief 이미지의 일부분을 gl텍스쳐로 만든다.
*/
GLuint XGraphicsOpenGL::CreateTextureSubGL( void* const pImgSrc
																		, const XE::POINT& sizeSrc			// pImgSrc의 크기
																		, const XE::xRECTi& rectSrc			// pImgSrc에서 잘라낼 영역
																		, XE::xtPixelFormat formatImgSrc
																		, XE::xtPixelFormat formatSurface )
{
	
	const XE::POINT sizeTex = rectSrc.GetSize();
	const XE::POINT sizeTexAligned = XGraphics::sAlignPowSize( sizeTex );
	const int bppSurface = XE::GetBpp( formatSurface );
	GLuint glTexture = 0;
	glGenTextures( 1, &glTexture );
	CHECK_GL_ERROR();
	
	if( XBREAK( glTexture == 0 ) )
		return 0;
	if( pImgSrc ) {
		XGraphicsOpenGL::sBindTexture( glTexture );
		CHECK_GL_ERROR();
		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		CHECK_GL_ERROR();
		
		const auto glTypeTexel = XGraphicsOpenGL::sToGLType( formatSurface );
		const auto glformatTexel = XGraphicsOpenGL::sToGLFormat( formatSurface );
		// 영역만큼 이미지 잘라내어 pDst에 옮김.
		DWORD *pSrc = (DWORD*)pImgSrc;
		DWORD *pBuffClipped = new DWORD[ sizeTexAligned.Size() ];
		DWORD *_pDst = pBuffClipped;
		pSrc += (rectSrc.GetTop() * sizeSrc.w + rectSrc.GetLeft());
		memset( pBuffClipped, 0, sizeTexAligned.Size() * bppSurface );
		for( int i = 0; i < sizeTexAligned.h; ++i ) {
			for( int j = 0; j < sizeTexAligned.w; ++j )
				*_pDst++ = *pSrc++;
			pSrc += ( sizeSrc.w - sizeTex.w );
			_pDst += ( sizeTexAligned.w - sizeTex.w );
		}
		if( bppSurface == 4 ) {
			glTexImage2D( GL_TEXTURE_2D,
										0,
										glformatTexel,
										sizeTexAligned.w,
										sizeTexAligned.h,
										0,
										glformatTexel,
										glTypeTexel,
										pBuffClipped );
			CHECK_GL_ERROR();
			
		} else // if( bppSurface == 4 ) {
		if( bppSurface == 2 ) {
			WORD *pDst16 = new WORD[ sizeTexAligned.Size() ];
			memset( pDst16, 0, sizeTexAligned.Size() * bppSurface );
			if( formatSurface == XE::xPF_RGB565 ) {
				XE::ConvertBlockABGR8888ToRGB565( pDst16, sizeTexAligned, pBuffClipped, sizeTexAligned );
			} else
			if( formatSurface == XE::xPF_ARGB4444 ) {
				XE::ConvertBlockABGR8888ToRGBA4444( pDst16, sizeTexAligned, pBuffClipped, sizeTexAligned );
			}
			glTexImage2D( GL_TEXTURE_2D,
										0,
										glformatTexel,
										sizeTexAligned.w,
										sizeTexAligned.h,
										0,
										glformatTexel,
										glTypeTexel,
										pDst16 );
			CHECK_GL_ERROR();
			
			SAFE_DELETE_ARRAY( pDst16 );
		} // if( bppSurface == 2 ) {
//		XSurface::s_sizeTotalVMem += sizeTexAligned.Size() * bppSurface;
		XSurface::sAddSizeTotalVMem( sizeTexAligned.Size() * bppSurface );
		SAFE_DELETE_ARRAY( pBuffClipped );
	} // if( pImgSrc ) {
	return glTexture;
} // CreateTextureSubGL()

void XGraphicsOpenGL::CopyValueSurface( XSurface* pDst, XSurface* pSrc )
{
	*(static_cast<XSurfaceOpenGL*>(pDst)) = *(static_cast<XSurfaceOpenGL*>(pSrc));
}

void XGraphicsOpenGL::sBindTexture( ID idTex )
{
// 	static ID s_idPrev = -1;
// 	if( s_idPrev == idTex )
// 		return;
// 	
	glBindTexture( GL_TEXTURE_2D, (GLuint)idTex );
	++s_numCallBindTexture;
// 	s_idPrev = idTex;
	CHECK_GL_ERROR();
	
}






#endif // gl