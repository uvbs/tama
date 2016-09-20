/*
 *  XSurfaceOpenGL.cpp
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 16.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 */
#include "stdafx.h"
#ifdef _VER_OPENGL
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "etc/xGraphics.h"
#include "etc/XSurface.h"
#include "etc/xMath.h"
#include "etc/xGraphics.h"
#include "XGraphicsOpenGL.h"
#include "etc/Debug.h"
#include "XImage.h"
//#include "Mathematics.h"
#include "etc/xMath.h"
#include "XFramework/client/XClientMain.h"
#include "XTextureAtlas.h"
#include "XSurfaceOpenGL.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;
//#define _XDEBUG_SURFACE

#ifdef _XDEBUG_SURFACE
#define S_TRACE(...)		XTRACE(__VA_ARGS__)
#else
#define S_TRACE(...)		{}
#endif


XSurfaceOpenGL::XSurfaceOpenGL( BOOL bHighReso
															, const int srcx, const int srcy
															, const int srcw, const int srch
															, const int dstw, const int dsth
															, const int adjx, const int adjy
															, void *_pSrcImg
															, const int bpp
															, BOOL bSrcKeep ) 
		: XSurface( bHighReso )	{
	Init();
	auto sizeDstMem = XE::POINT( dstw, dsth ) * 2;
	XSurface::CreateSub( XE::POINT(srcx, srcy)
				, sizeDstMem // XE::POINT(memDstw, memDsth)
				, XE::POINT(srcw,srch)
				, _pSrcImg
				, XE::xPF_ARGB8888
				, XE::VEC2(dstw, dsth)
				, XE::VEC2(adjx, adjy)
				, XE::xPF_ARGB4444
				, bSrcKeep != FALSE
				, false );
}

void XSurfaceOpenGL::Init()
{
	m_glTexture = 0;
	m_format = xPIXELFORMAT_NONE;
	m_type = 0;
	m_glVertexBuffer = m_glIndexBuffer = 0;
#ifdef _XVAO
	m_idVertexArray = 0;
#endif
}

void XSurfaceOpenGL::Destroy()
{
	S_TRACE("destroy Surface: %d,%d,%d - %.1f x %.1f", m_glTexture,
																										m_glVertexBuffer,
																										m_glIndexBuffer,
																										GetWidth(), GetHeight() );
	DestroyDevice();
}

void XSurfaceOpenGL::ClearDevice()
{
	m_glTexture = 0;
	m_glVertexBuffer = 0;
	m_glIndexBuffer = 0;
#ifdef _XVAO
	m_idVertexArray = 0;
#endif
}

/**
 @brief 
 pure virtual
*/
bool XSurfaceOpenGL::Create( const XE::POINT& sizeSurfaceOrig
													, const XE::VEC2& vAdj
													, XE::xtPixelFormat formatSurface
													, void* const pImgSrc
													, XE::xtPixelFormat formatImgSrc
													, const XE::POINT& sizeMemSrc
													, const XE::POINT& sizeMemSrcAligned
													, bool bUseAtlas )
{
	if( XBREAK(pImgSrc == nullptr) )
		return false;
	auto _sizeMemSrc = sizeMemSrc;
	auto _sizeMemSrcAligned = sizeMemSrcAligned;
	// 텍스쳐 생성
#if defined(_XPROFILE)
	if( XGraphics::s_dwDraw & XE::xeBitSmallTex )	{
		if( _sizeMemSrc.w > 32 ) {
			_sizeMemSrc.w = 32;
			_sizeMemSrcAligned.w = 32;
		}
		if( _sizeMemSrc.h > 32 ) {
			_sizeMemSrc.h = 32;
			_sizeMemSrcAligned.h = 32;
		}
	}
#endif // _XPROFILE
	XBREAK( bUseAtlas );	// 아틀라스를 사용하려면 XSurfaceOpenGL2를 이용해야함.
// 	if( bUseAtlas ) {
// 		XBREAK(1);
// 	} else {
		// no atlas
		m_glTexture = GRAPHICS_GL->CreateTextureGL( pImgSrc
																								, _sizeMemSrc
																								, formatImgSrc
																								, _sizeMemSrcAligned
																								, formatSurface );
		if( XBREAK( m_glTexture == 0 ) )
			return false;
		// 버텍스버퍼생성.
		return CreateVertexBuffer( sizeSurfaceOrig
															 , vAdj
															 , sizeMemSrc
															 , sizeMemSrcAligned );
// 	}
}

// pure virtual
/**
 @brief 
*/
bool XSurfaceOpenGL::CreateSub( const XE::POINT& posMemSrc
															, const XE::POINT& sizeArea
															, const XE::POINT& sizeAreaAligned
															, const XE::POINT& sizeMemSrc
															, void* const _pSrcImg
															, XE::xtPixelFormat formatImgSrc
															, const XE::VEC2& sizeRender
															, const XE::VEC2& vAdj
															, XE::xtPixelFormat formatSurface )
{
	const auto bppSrc = XE::GetBpp( formatImgSrc );
	XBREAK( bppSrc == 2 );		// 이건 아직 지원하지 않음.
	XE::xRECTi rectSrc( posMemSrc, sizeArea );
	m_glTexture = GRAPHICS_GL->CreateTextureSubGL( _pSrcImg
																								, sizeMemSrc
																								, rectSrc
																								, formatImgSrc
																								, formatSurface );
	// 정렬된 크기로 버텍스버퍼생성.
	return CreateVertexBuffer( sizeRender
													, vAdj
													, sizeArea
													, sizeAreaAligned );
}

/**
 @brief 기존코드 호환용.(단 외부에서 쓰는건 막음)
*/
bool XSurfaceOpenGL::CreatePNG( LPCTSTR szRes, bool bSrcKeep, bool bMakeMask )
{
	XE::POINT sizeMem;
	DWORD *pImg = nullptr;
	if( !GRAPHICS_GL->LoadImg( szRes, &sizeMem.w, &sizeMem.h, &pImg ) )
		return false;
	//
	const auto vss = GetsizeSurface();
	const auto vms = GetsizeMemToPoint();
	// 리스토어 할때 서피스크기와 메모리크기가 다르면 highReso로 인식(기존코드 호환용)
	bool bHighReso = (((int)vss.w != (int)vms.w) && ((int)vss.h != (int)vms.h));
	const auto sizeSurface = ( bHighReso ) ? sizeMem / 2 : sizeMem;
	return XSurface::Create( sizeSurface
													, GetAdjust()
													, GetformatSurface()
													, pImg
													, XE::xPF_ARGB8888		// formatImgSrc
													, sizeMem
													, bSrcKeep, bMakeMask
													, false );
}

typedef struct tagSTRUCT_VERTEX_SURFACE{
	GLfloat x[2];//, y;
	GLfloat t[2];//u, tv;
	GLfloat c[4];// r,g,b,a;
} xVertexNoBatch;

// 인수는 2^정렬에 비율 리사이징까지된것.
/**
 @brief this에 디바이스 버텍스버퍼 객체를 생성한다.
*/
bool XSurfaceOpenGL::CreateVertexBuffer( float surfaceW, float surfaceH
																					, const float _adjx, const float _adjy
																					, int memw, int memh
																					, int alignW, int alignH )
{
	
	XBREAK( alignW == 0 );
	XBREAK( alignH == 0 );
	float adjx = (float)((int)_adjx);
	float adjy = (float)((int)_adjy);
	float u = (float)memw / alignW; // 텍스쳐 우하귀의 u,v좌표 
	float v = (float)memh / alignH;
	XBREAK( u < 0 || u > 1.0f );
	XBREAK( v < 0 || v > 1.0f );
	const xVertexNoBatch vertices[4] = {
		adjx, surfaceH + adjy,		0, v,  1.0f, 1.0f, 1.0f, 1.0f,	// left/bottom
		surfaceW + adjx, surfaceH + adjy,u, v,  1.0f, 1.0f, 1.0f, 1.0f,  // right/bottom
		adjx, adjy,					0, 0,  1.0f, 1.0f, 1.0f, 1.0f,		// left/top
		surfaceW + adjx, adjy,		u, 0,  1.0f, 1.0f, 1.0f, 1.0f	// right/top
	};
	static GLubyte indices[4] = { 0, 1, 2, 3 };
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }

#ifdef _XVAO
	glGenVertexArraysOES( 1, &m_idVertexArray );
	glBindVertexArrayOES( m_idVertexArray );
#endif
	glGenBuffers( 1, &m_glVertexBuffer );
	glGenBuffers( 1, &m_glIndexBuffer );
	if( XBREAK( m_glVertexBuffer == 0 ) )
		return false;
	if( XBREAK( m_glIndexBuffer == 0 ) )
		return false;

	glBindBuffer( GL_ARRAY_BUFFER, m_glVertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }

	return true;
} // CreateVertexBuffer

// bool XSurfaceOpenGL::CreateVertexBuffer2( const XE::VEC2& sizeSurface,
// 																					const XE::VEC2& vAdj,
// 																					const XE::VEC2& uvlt,
// 																					const XE::VEC2& uvrb ) {
// 	
// 	const float u = uvlt.x;		// 아틀라스내 좌상귀
// 	const float v = uvlt.y;
// 	const float u2 = uvrb.x;		// 아틀라스내 우하귀
// 	const float v2 = uvrb.y;
// 	XBREAK( u < 0 || u > 1.0f );
// 	XBREAK( v < 0 || v > 1.0f );
// 	XBREAK( u2 < 0 || u2 > 1.0f );
// 	XBREAK( v2 < 0 || v2 > 1.0f );
// 	const STRUCT_VERTEX_SURFACE vertices[4] =	{
// 		vAdj.x, sizeSurface.h + vAdj.y,		u, v2,  1.0f, 1.0f, 1.0f, 1.0f,	// left/bottom
// 		sizeSurface.w + vAdj.x, sizeSurface.h + vAdj.y, u2, v2,  1.0f, 1.0f, 1.0f, 1.0f,  // right/bottom
// 		vAdj.x, vAdj.y,					u, v,  1.0f, 1.0f, 1.0f, 1.0f,		// left/top
// 		sizeSurface.w + vAdj.x, vAdj.y,		u2, v,  1.0f, 1.0f, 1.0f, 1.0f	// right/top
// 	};
// 	static GLubyte indices[4] = { 0, 1, 2, 3 };
// 	{ auto glErr = glGetError();
// 	XASSERT( glErr == GL_NO_ERROR ); }
// 
// #ifdef _XVAO
// 	glGenVertexArraysOES( 1, &m_idVertexArray );
// 	glBindVertexArrayOES( m_idVertexArray );
// #endif
// 	glGenBuffers( 1, &m_glVertexBuffer );
// 	glGenBuffers( 1, &m_glIndexBuffer );
// 	if( XBREAK( m_glVertexBuffer == 0 ) )
// 		return bool;
// 	if( XBREAK( m_glIndexBuffer == 0 ) )
// 		return xFAIL;
// 	glBindBuffer( GL_ARRAY_BUFFER, m_glVertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer );
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );
// 
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
// 	glBindBuffer( GL_ARRAY_BUFFER, 0 );
// 	{ auto glErr = glGetError();
// 	XASSERT( glErr == GL_NO_ERROR ); }
// 
// 	return xSUCCESS;
// } // CreateVertexBuffer2

void XSurfaceOpenGL::DestroyDevice()
{
	
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	if( m_glTexture )
		XSurface::DestroyDevice();
	if( GetstrRes().empty() == false ) {
		S_TRACE("destroy Surface: %s", GetstrRes().c_str() );
	}
	// 홈으로 나갈때 자동으로 디바이스 자원은 파괴되지만 m_glTexture등도 클리어 시켜주지 않으면 돌아왔을때 새로 할당한 번호가 겹쳐서 다시 지워버릴 수 있다.
	if( IsbAtlas() ) {
		XBREAK(1);
//		XTextureAtlas::sGet()->Release( m_glTexture );
	} else {
		if( m_glTexture ) {
			glDeleteTextures( 1, &m_glTexture );
		}
	}
	m_glTexture = 0;
	if( m_glVertexBuffer ) {
		glDeleteBuffers(1, &m_glVertexBuffer);
		m_glVertexBuffer = 0;
	}
	if( m_glIndexBuffer )	{
		glDeleteBuffers(1, &m_glIndexBuffer);
		m_glIndexBuffer = 0;
	}
#ifdef _XVAO
	if( m_idVertexArray ) {
		glDeleteVertexArraysOES( 1, &m_idVertexArray );
		m_idVertexArray = 0;
	}
#endif
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
}


/**
 OpenGL 디바이스 자원을 다시 생성한다.
 보관해 두었던 파일명을 기반으로 다시 로딩한다.
 파일로 로딩한게 아니었다면 pSrcImg로 복원하고
 그것마저 없다면 에러를 낸다.
 */
void XSurfaceOpenGL::RestoreDevice()
{
	
	XE::VEC2 vSize;
	DWORD *pSrcImg = GetSrcImg( &vSize );
//#ifdef _VER_IOS
	// iSO에서 리스토어의 시뮬레이션을 위해 파괴시킴.
	DestroyDevice();
//#endif	// 혹시 파괴해야되나 싶어서 지워봄.
	ClearDevice();
	if( pSrcImg )	{
		// 소스이미지가 보관되어 있느냐
		RestoreDeviceFromSrcImg();
		XTRACE("RestoreDeviceFromSrcImg");
		return;
	} else
	if( IsHavestrRes() ) {
//		XTRACE("RestorePNG: %s", GetstrRes().c_str() );
		BOOL bOk = CreatePNG( GetstrRes().c_str(), FALSE, FALSE );
		if( bOk == FALSE ) {
			// 파일을 못찾았으면 국가별 폴더에서 다시 찾아봄
			LPCTSTR szRes = GetstrRes().c_str();
			XTRACE("restore file not found:%s...", szRes);
			TCHAR szLangPath[ 1024 ];
			XE::LANG.ChangeToLangDir( szRes, szLangPath );
			XTRACE("lang path try agin:%s", szLangPath );
			bOk = CreatePNG( szLangPath, FALSE, FALSE );
		}
		XBREAK( bOk == FALSE );
	} else {
		// XImageMng::CreateSurface()로 만든 서피스는 여기에 걸림.
//		XBREAKF(1, "have not strRes or SrcImg" );
	}
}

/** 
 소스이미지가 보관되어있을때 그것을 기반으로 텍스쳐를 재 생성한다.
 최초 이 서피스가 png로부터 생성된것이라 해도 소스이미지와 png파일은 같다는 가정하에
 png를 다시 읽지 않기위해 소스이미지로만 읽는다.
*/
void XSurfaceOpenGL::RestoreDeviceFromSrcImg()
{
	XE::POINT sizeMem;
	DWORD *pSrcImg = GetSrcImg( &sizeMem );
	XBREAK( pSrcImg == NULL );
	const float wSurface = GetWidth();
	const float hSurface = GetHeight();
	const float adjx = GetAdjustX();
	const float adjy = GetAdjustY();
	const auto sizeMemAligned = GetsizeMemAligned();
	// 버텍스 버퍼 재생성
	CreateVertexBuffer( wSurface, hSurface, adjx, adjy,
						sizeMem.w, sizeMem.h, sizeMemAligned.w, sizeMemAligned.h );
	// 텍스쳐 재생성
	XE::POINT sizeAligned;
	const auto formatImgSrc = XE::xPF_ARGB8888;
	const auto formatSurface = GetformatSurface();
	m_glTexture = GRAPHICS_GL->CreateTextureGL( (void* const)pSrcImg, sizeMem, formatImgSrc, sizeMemAligned, formatSurface );
	if( m_glTexture ) {
		XSurface::RestoreDeviceFromSrcImg();
	}
	XBREAK( m_glTexture == 0 );
}

// src서피스를 this로 카피.
void XSurfaceOpenGL::CopySurface( XSurface *src )
{
	
	XSurfaceOpenGL *pSrc = (XSurfaceOpenGL *)src;
	// src를 FBO에 연결
	// glCopyTexImage를 이용해 src에서 this로 옮김.
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pSrc->GetTextureID(), 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE ) {
		XTRACE( "status=%d", status );
		return;
	}
	// copy texture from framebuffer
	XGraphicsOpenGL::sBindTexture( m_glTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, pSrc->GetMemWidth(), pSrc->GetMemHeight());    
	// FBO해제
	GRAPHICS_GL->RestoreFrameBuffer();
	glDeleteFramebuffers(1, &fbo);
}


#pragma mark Draw
void XSurfaceOpenGL::DrawByParam( const MATRIX &mParent, const XE::xRenderParam& paramRender ) const
{
	if( GetsizeMem().w == 0 || GetsizeMem().h == 0 ) {
		// 비동기상태로 로딩을 기다리고 있는 중.
		return;
	}
	//	XBREAK( GetsizeMem().IsZero() );
	CHECK_GL_ERROR();
	do {
		const auto blend = paramRender.m_funcBlend;
		if( blend != xBF_NO_DRAW ) {
			glEnable( GL_BLEND );
			const auto funcBlend = paramRender.m_funcBlend;
			GLenum glsFactor, gldFactor;
			sSetglBlendFunc( funcBlend, (GLenum*)&glsFactor, (GLenum*)&gldFactor );
			glBlendFunc( glsFactor, gldFactor );
			if( blend == xBF_SUBTRACT ) {
				glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
			} else {
				glBlendEquation( GL_FUNC_ADD );
			}
			CHECK_GL_ERROR();
			MATRIX mWorld;
			paramRender.GetmTransform( &mWorld );
			MatrixMultiply( mWorld, mWorld, mParent );
			MATRIX mMVP;
			MatrixMultiply( mMVP, mWorld, XE::x_mViewProjection );
			if( XSurface::IsInViewport( 0, 0, mWorld ) == FALSE )
				break;
			// 현재 쉐이더를 얻어온다
			XShader *pShader = XGraphicsOpenGL::sGetShader();
			if( blend == xBF_GRAY )
				pShader = GRAPHICS_GL->GetpGrayShader();
			const auto vColor = paramRender.m_vColor;
			pShader->SetShader( mMVP, vColor );
			CHECK_GL_ERROR();
			DrawCore();
		}
	} while( 0 );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	CHECK_GL_ERROR();
}

/**
 @brief 
*/
void XSurfaceOpenGL::Draw( float x, float y, const MATRIX &mParent )
{
	
	if( GetsizeMem().w == 0 || GetsizeMem().h == 0 ) {
		// 비동기상태로 로딩을 기다리고 있는 중.
		return;
	}
//	XBREAK( GetsizeMem().IsZero() );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	do	{
		if( GetDrawMode() != xDM_NONE )	{
			glEnable( GL_BLEND );
			glBlendEquation (GL_FUNC_ADD);
			if( GetDrawMode() == xDM_NORMAL )
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			else 
			if( GetDrawMode() == xDM_SCREEN )
				glBlendFunc(GL_SRC_ALPHA, GL_ONE );
			else
			if( GetDrawMode() == xDM_MULTIPLY )
				glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
			else
			if( GetDrawMode() == xDM_SUBTRACT ) {
				glBlendFunc (GL_ONE, GL_ONE);
				glBlendEquation (GL_FUNC_REVERSE_SUBTRACT);
			}
			{ auto glErr = glGetError();
			XASSERT( glErr == GL_NO_ERROR ); }
			MATRIX mWorld;
			GetMatrix( XE::VEC2(x,y), &mWorld );
			MatrixMultiply( mWorld, mWorld, mParent );
			MATRIX mMVP;
			MatrixMultiply( mMVP, mWorld, XE::x_mViewProjection );
			if( XSurface::IsInViewport( 0, 0, mWorld ) == FALSE )
				break;
			// 현재 쉐이더를 얻어온다
			XShader *pShader = XGraphicsOpenGL::sGetShader();
			if( GetDrawMode() == xDM_GRAY )
				pShader = GRAPHICS_GL->GetpGrayShader();
			const auto vColor = Getv4Color();
//			pShader->SetShader( mMVP,  m_ColorR, m_ColorG, m_ColorB, m_fAlpha );
			pShader->SetShader( mMVP, vColor );

			DrawCore();		
		}
	} while(0);
	XSurface::ClearAttr();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CHECK_GL_ERROR();
}

/*
매트릭스 변환이 없는 코어버전.
*/
void XSurfaceOpenGL::DrawCore() const
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDraw )
		return;
#endif // _XPROFILE
	
	XBREAK( GetsizeMem().IsZero() );
	// Restore가 아직 안되어 0일수도 있으므로(페북프로필사진같은) 그때는 그냥 그리지만 않게 한다.
	if( m_glVertexBuffer == 0 )
		return;
	if( m_glIndexBuffer == 0 )
		return;
	if( m_glTexture == 0 )
		return;
#ifdef _XVAO
	XBREAK( m_idVertexArray == 0 );
    glBindVertexArrayOES( m_idVertexArray );
#endif
		CHECK_GL_ERROR();
		// bind vertex/index buffer
		glBindBuffer( GL_ARRAY_BUFFER, m_glVertexBuffer );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, x ) );
		glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
		glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, t ) );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, c ) );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer );
		CHECK_GL_ERROR();
		// bind texture
#ifdef _XPROFILE
		if( !(XGraphics::s_dwDraw & XE::xeBitNoTexture) )
#endif // _XPROFILE
		{
			XGraphicsOpenGL::sBindTexture( m_glTexture );
		}
		CHECK_GL_ERROR();

#ifdef _XPROFILE
		if( !(XGraphics::s_dwDraw & XE::xeBitNoDP) )
#endif // _XPROFILE
		{
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}
		CHECK_GL_ERROR();

}

// this의 RECT:src영역을 x,y위치에 그린다.
void XSurfaceOpenGL::DrawSub( float x, float y, const XE::xRECTi *src )
{
	
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDraw )
		return;
#endif // _XPROFILE
	XBREAK( GetsizeMem().IsZero() );
	int memw, memh;
	GLfloat l, t, r, b;
	const auto sizeMemAligned = GetsizeMemAlignedVec2();
	if( src ) {
		XE::xRECTi memRect;
		memRect.SetLeft( src->GetLeft() * 2 );
		memRect.SetRight( src->GetRight() * 2 );
		memRect.SetTop( src->GetTop() * 2 );
		memRect.SetBottom( src->GetBottom() * 2 );
		memw = ( memRect.Right() - memRect.Left() );
		memh = ( memRect.Bottom() - memRect.Top() );
		l = memRect.GetLeft() / (GLfloat)sizeMemAligned.w;
		t = memRect.GetTop() / (GLfloat)sizeMemAligned.h;
		r = memRect.GetRight() / (GLfloat)sizeMemAligned.w;
		b = memRect.GetBottom() / (GLfloat)sizeMemAligned.h;
	} else {
		// src가 지정되어 있지 않으면 전체 출력.
		Draw( x, y );
		{ auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR ); }
		return;
	}
	GLfloat tex[ 8 ] = {l, b, r, b, l, t, r, t};
	float surfacew = (float)( src->GetRight() - src->GetLeft() );
	float surfaceh = (float)( src->GetBottom() - src->GetTop() );
	GLfloat pos[ 8 ] = {0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0};
	const float alpha = GetfAlpha();
	GLfloat col[ 16 ] = {1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha};

	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	glEnable( GL_BLEND );
	glBlendEquation( GL_FUNC_ADD );
	if( GetDrawMode() == xDM_NORMAL )
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	else
	if( GetDrawMode() == xDM_SCREEN )
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	else
	if( GetDrawMode() == xDM_MULTIPLY )
		glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
	else
	if( GetDrawMode() == xDM_SUBTRACT ) {
		glBlendFunc( GL_ONE, GL_ONE );
		glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
	}
	//
	MATRIX m, mMVP;
	MatrixTranslation( m, x, y, 0 );
	MatrixMultiply( mMVP, m, XE::x_mViewProjection );
	const auto vColor = Getv4Color();
// 	GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, m_ColorR, m_ColorG, m_ColorB, 1.0f );
	GRAPHICS_GL->GetpShaderColTex()->SetShader( mMVP, vColor );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
#ifdef _XVAO
	glBindVertexArrayOES( 0 );
#endif
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
	glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
	glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	// bind texture
#ifdef _XPROFILE
	if( !(XGraphics::s_dwDraw & XE::xeBitNoTexture) )
#endif // _XPROFILE
		XGraphicsOpenGL::sBindTexture( m_glTexture );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }

#ifdef _XPROFILE
	if( !(XGraphics::s_dwDraw & XE::xeBitNoDP) )
#endif // _XPROFILE
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	//
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	XSurface::ClearAttr();
// 	m_DrawMode = xDM_NORMAL;
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
}

//
//-------------------------------------------------------------
// 가변윈도우 프레임 그릴때 사용
void XSurfaceOpenGL::DrawLocal( float x, float y, float lx, float ly )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDraw )
		return;
#endif // _XPROFILE
	
	XBREAK( GetsizeMem().IsZero() );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	if( GetDrawMode() != xDM_NONE )	{
		glEnable( GL_BLEND );
		glBlendEquation( GL_FUNC_ADD );
		if( GetDrawMode() == xDM_NORMAL )
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		else
		if( GetDrawMode() == xDM_SCREEN )
			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		else
		if( GetDrawMode() == xDM_MULTIPLY )
			glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
		else
		if( GetDrawMode() == xDM_SUBTRACT )	{
			glBlendFunc( GL_ONE, GL_ONE );
			//glBlendEquation (GL_FUNC_SUBTRACT);
			glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
		}
		{ auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR ); }
		MATRIX mWorld, m;
		MatrixIdentity( mWorld );
		MatrixTranslation( m, lx, ly, 0 );
		MatrixMultiply( mWorld, mWorld, m );
		if( !GetAdjustAxis().IsZero() ) {
			MatrixTranslation( m, -GetfAdjustAxisX(), -GetfAdjustAxisY(), 0 );
			MatrixMultiply( mWorld, mWorld, m );
		}
		const auto vScale = GetScale();
		if( vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f ) {
			MatrixScaling( m, vScale.x, vScale.y, 1.0f );
			MatrixMultiply( mWorld, mWorld, m );
		}
		if( GetfRotZ() ) {
			MatrixRotationZ( m, D2R( GetfRotZ() ) );
			MatrixMultiply( mWorld, mWorld, m );
		}
		if( GetfRotY() ) {
			MatrixRotationY( m, D2R(GetfRotY()) );
			MatrixMultiply( mWorld, mWorld, m );
		}
		if( !GetAdjustAxis().IsZero() ) {
			MatrixTranslation( m, GetfAdjustAxisX(), GetfAdjustAxisY(), 0 );
			MatrixMultiply( mWorld, mWorld, m );
		}
		MATRIX mMVP;
		MatrixTranslation( m, x, y, 0 );
		MatrixMultiply( mWorld, mWorld, m );
		MatrixMultiply( mMVP, mWorld, XE::x_mViewProjection );
//		GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, m_ColorR, m_ColorG, m_ColorB, m_fAlpha );
		const XE::VEC4 vColor = Getv4Color();
		GRAPHICS_GL->GetpShaderColTex()->SetShader( mMVP, vColor );
		{ auto glErr = glGetError();
		XASSERT( glErr == GL_NO_ERROR ); }
		DrawCore();
	}
	XSurface::ClearAttr();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
} // draw local

void XSurfaceOpenGL::Fill( XCOLOR col )
{
	
}
void*	XSurfaceOpenGL::Lock( int *pMemW, BOOL bReadOnly)
{
//	*pMemW = GetAlignedWidth();
	*pMemW = GetsizeMemAligned().w;
	int w, h;
	return GetSrcImg( &w, &h );
}

void XSurfaceOpenGL::SetTexture()
{
	XGraphicsOpenGL::sBindTexture( m_glTexture );
}

#ifdef _XBLUR
//#define _XSCALE_BLUR	//확대해서 블러 하는 버전
#ifdef _XBLUR
#define _XDOWNSCALE_BLUR	// 다운스케일링 방식 블러
#define RT_WIDTH	512
#define RT_HEIGHT	512
#define BLUR_MARGIN	16.f
#define SWAP_RENDER_TARGET() \
	SWAP( currRenderTargetFBO, idleRenderTargetFBO ); \
	SWAP( currRenderTargetRBO, idleRenderTargetRBO ); \
	SWAP( idCurrSrcTexture, idIdleSrcTexture );
#endif // _XBLUR

static void DrawTexture( XShader *pShader,
												 float x, float y, float w, float h,
												 float _u2, float _v2,
												 GLuint idTex,
												 GLuint idFBO, GLuint idRBO,
												 MATRIX *pmParent )
{

	float u1 = 0;
	float v1 = 0;
	float u2 = _u2;
	float v2 = _v2;
	float surfacew = w;
	float surfaceh = h;
	GLfloat tex[8] = { u1, v2, u2, v2, u1, v1, u2, v1 };
	GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
	GLfloat col[16] = { 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f };
#ifdef _XVAO
	glBindVertexArrayOES( 0 );
#endif
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
	glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	//
	MATRIX mTrans/*, mScale*/, mMVP;
	MatrixIdentity( mMVP );
	MatrixTranslation( mTrans, x, y, 0 );
	MatrixMultiply( mMVP, mMVP, mTrans );
	if( pmParent )
		MatrixMultiply( mMVP, mMVP, *pmParent );
	MatrixMultiply( mMVP, mMVP, XE::x_mViewProjection );
	//
	pShader->SetShader( mMVP, 1.f, 1.f, 1.f, 1.f );
	for( int i = 0; i < 1; ++i )	// pass
	{
		glBindFramebuffer( GL_FRAMEBUFFER, idFBO );
		glBindRenderbuffer( GL_RENDERBUFFER, idRBO );
		// bind texture
		XGraphicsOpenGL::sBindTexture( idTex );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}
	{ auto glErr = glGetError();
	XASSERT( glErr == GL_NO_ERROR ); }
}
void XSurfaceOpenGL::DrawBlur( float x, float y, const MATRIX &mParent )
{

	float rtw = RT_WIDTH;
	float rth = RT_HEIGHT;
	GLint wrap = GL_REPEAT;	// CLAMP_EDGE로 하니까 블러 끝에 밀리는색이 나타난다
	if( GRAPHICS_GL->s_glBlurRBO == 0 ) {
		// 렌더버퍼가 생성이 안되어 있다면 새로 만든다.
		glGenRenderbuffers( 1, &GRAPHICS_GL->s_glBlurRBO );
		glBindRenderbuffer( GL_RENDERBUFFER, GRAPHICS_GL->s_glBlurRBO );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8_OES, RT_WIDTH, RT_HEIGHT );
		glGenFramebuffers( 1, &GRAPHICS_GL->s_glBlurFBO );
		glBindFramebuffer( GL_FRAMEBUFFER, GRAPHICS_GL->s_glBlurFBO );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, GRAPHICS_GL->s_glBlurRBO );
		glGenTextures( 1, &GRAPHICS_GL->s_glBlurTexture );
		XGraphicsOpenGL::sBindTexture( GRAPHICS_GL->s_glBlurTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, RT_WIDTH, RT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GRAPHICS_GL->s_glBlurTexture, 0 );
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE ) {
			return;
		}
		glGenRenderbuffers( 1, &GRAPHICS_GL->s_glBlurRBO2 );
		glBindRenderbuffer( GL_RENDERBUFFER, GRAPHICS_GL->s_glBlurRBO2 );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8_OES, RT_WIDTH, RT_HEIGHT );
		glGenFramebuffers( 1, &GRAPHICS_GL->s_glBlurFBO2 );
		glBindFramebuffer( GL_FRAMEBUFFER, GRAPHICS_GL->s_glBlurFBO2 );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, GRAPHICS_GL->s_glBlurRBO2 );
		glGenTextures( 1, &GRAPHICS_GL->s_glBlurTexture2 );
		XGraphicsOpenGL::sBindTexture( GRAPHICS_GL->s_glBlurTexture2 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, RT_WIDTH, RT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GRAPHICS_GL->s_glBlurTexture2, 0 );
		status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE ) {
			return;
		}
	}
	// 렌더타겟을 설정한다.
	glBindFramebuffer( GL_FRAMEBUFFER, GRAPHICS_GL->s_glBlurFBO );
	glBindRenderbuffer( GL_RENDERBUFFER, GRAPHICS_GL->s_glBlurRBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8_OES, RT_WIDTH, RT_HEIGHT );
	glViewport( 0, 0, RT_WIDTH, RT_HEIGHT );
	//    XE::SetProjection( 256, 256 );
	MatrixMakeOrtho( XE::x_mViewProjection, 0, RT_WIDTH, 0, RT_HEIGHT, -1.0f, 1.0f );
	//	XE::SetProjection(XE::GetGameWidth(), XE::GetGameHeight());
	//	XE::SetProjection(256, 256);
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	//
	MATRIX mTrans, mScale, mMVP;
	// 블러를 하면 원래크기보다 조금 줄어들기때문에 소스텍스쳐를 살짝 크게 만들어 넣는다.
	// 세이더만드러 블러가 커지려면 멀티패스로 반복해서 블러를 먹여야 하는데 그것보다 소스자체를 살짝 크게 만들어 넣는 꼼수를 택함
	MatrixIdentity( mMVP );
#ifdef _XSCALE_BLUR
	float mw = GetMemWidth() / 2.f;
	MatrixTranslation( mTrans, -mw, -mw, 0 );	// 중심을 기준으로 확대하기 위해 축조정
	MatrixMultiply( mMVP, mMVP, mTrans );
#endif
	// 8픽셀 크기로 블러가 되게 하려고 8픽셀 여유를 뒀다.
	MatrixTranslation( mTrans, BLUR_MARGIN, BLUR_MARGIN, 0 );
#ifdef _XSCALE_BLUR
	MatrixScaling( mScale, 1.3f, 1.3f, 1.f );
	MatrixMultiply( mMVP, mMVP, mScale );
#endif
	MatrixMultiply( mMVP, mMVP, mTrans );
#ifdef _XSCALE_BLUR
	MatrixTranslation( mTrans, mw, mw, 0 );		// 축조정
	MatrixMultiply( mMVP, mMVP, mTrans );
#endif
	MatrixMultiply( mMVP, mMVP, XE::x_mViewProjection );
	GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

#ifdef _XVAO
	glBindVertexArrayOES( m_idVertexArray );
#endif
	glBindBuffer( GL_ARRAY_BUFFER, m_glVertexBuffer );
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, x ) );
	glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
	glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, t ) );

	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, c ) );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer );

	{
		// this원본을 렌더타겟에 <원본크기>그대로 렌더링한다
		float u = (float)GetMemWidth() / GetAlignedWidth();
		float v = (float)GetMemHeight() / GetAlignedHeight();
		float surfacew = GetMemWidth();
		float surfaceh = GetMemHeight();
		GLfloat tex[8] = { 0, v, u, v, 0, 0, u, 0 };
		GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
		GLfloat col[16] = { 1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f };
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
		glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );

		// bind texture
		XGraphicsOpenGL::sBindTexture( m_glTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}

	int numPass = 1;
	GLuint currRenderTargetFBO = GRAPHICS_GL->s_glBlurFBO;
	GLuint currRenderTargetRBO = GRAPHICS_GL->s_glBlurRBO;
	GLuint idleRenderTargetFBO = GRAPHICS_GL->s_glBlurFBO2;
	GLuint idleRenderTargetRBO = GRAPHICS_GL->s_glBlurRBO2;
	GLuint idCurrSrcTexture = GRAPHICS_GL->s_glBlurTexture2;
	GLuint idIdleSrcTexture = GRAPHICS_GL->s_glBlurTexture;
	float sizeFilter = 0.125f;

	// horiz blur
	{
#ifdef _XDOWNSCALE_BLUR
		float u2 = (GetMemWidth() + (BLUR_MARGIN*2.f)) / RT_WIDTH;
		float v2 = (GetMemHeight() + (BLUR_MARGIN*2.f)) / RT_HEIGHT;
		float surfacew = GetMemWidth() + (BLUR_MARGIN*2.f);
		float surfaceh = GetMemHeight() + (BLUR_MARGIN*2.f);
		surfacew *= sizeFilter;
		surfaceh *= sizeFilter;
		// 일반 쉐이더로 다운스케일링해서 먼저 찍는다.
		SWAP_RENDER_TARGET();
		//		glClearColor(0, 0, 0, 0);
		//		glClear(GL_COLOR_BUFFER_BIT);
		DrawTexture( GRAPHICS_GL->GetpBaseShader(),
								 0, 0,
								 surfacew, surfaceh,
								 u2, v2,
								 idCurrSrcTexture,
								 currRenderTargetFBO, currRenderTargetRBO,
								 NULL );
		// 렌더 타겟을 스왑한다.
		SWAP_RENDER_TARGET();
		//		glClearColor(0, 0, 0, 0);
		//		glClear(GL_COLOR_BUFFER_BIT);
		// horiz blur 쉐이더로 다운스케일링 된 이미지를 블러시킨다.
		u2 *= sizeFilter;
		v2 *= sizeFilter;
		DrawTexture( GRAPHICS_GL->GetpBlurShaderH(),
								 0, 0,
								 surfacew, surfaceh,
								 u2, v2,
								 idCurrSrcTexture,
								 currRenderTargetFBO, currRenderTargetRBO,
								 NULL );


		// _XDOWNSCALE_BLUR
		//////////////
#else
		//////////////
		// not _XDOWNSCALE_BLUR
		float u1 = 0;
		float v1 = 0;
		float u2 = (GetMemWidth() + (BLUR_MARGIN*2.f)) / RT_WIDTH;
		float v2 = (GetMemHeight() + (BLUR_MARGIN*2.f)) / RT_HEIGHT;
		float surfacew = GetMemWidth() + (BLUR_MARGIN*2.f);
		float surfaceh = GetMemHeight() + (BLUR_MARGIN*2.f);
#ifdef _XDOWNSCALE_BLUR
		surfacew *= sizeFilter;
		surfaceh *= sizeFilter;
#endif
		GLfloat tex[8] = { u1, v2, u2, v2, u1, v1, u2, v1 };
		GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
		GLfloat col[16] = { 1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f };
		glViewport( 0, 0, RT_WIDTH, RT_HEIGHT );
#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
		glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
		//
		for( int i = 0; i < numPass; ++i ) {
			SWAP_RENDER_TARGET();
			glBindFramebuffer( GL_FRAMEBUFFER, currRenderTargetFBO );
			glBindRenderbuffer( GL_RENDERBUFFER, currRenderTargetRBO );
			glClearColor( 0, 0, 0, 0 );
			glClear( GL_COLOR_BUFFER_BIT );
			GRAPHICS_GL->GetpBlurShaderH()->SetShader( XE::x_mViewProjection, 1.f, 1.f, 1.f, 1.f );
			// bind texture
			XGraphicsOpenGL::sBindTexture( idCurrSrcTexture );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}
#endif // not xDownScale_blur
	}

	// vert blur shader
	{
		for( int i = 0; i < 0; ++i ) {
			SWAP_RENDER_TARGET();
			glBindFramebuffer( GL_FRAMEBUFFER, currRenderTargetFBO );
			glBindRenderbuffer( GL_RENDERBUFFER, currRenderTargetRBO );
			glViewport( 0, 0, RT_WIDTH, RT_HEIGHT );
			GRAPHICS_GL->GetpBlurShaderV()->SetShader( XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
			//		GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, 1.f, 1.f, 1.f, 1.f );
			glClearColor( 0, 0, 0, 0 );
			glClear( GL_COLOR_BUFFER_BIT );
			float u = (GetMemWidth() + (BLUR_MARGIN*2.f)) / RT_WIDTH;
			float v = (GetMemHeight() + (BLUR_MARGIN*2.f)) / RT_HEIGHT;
			float surfacew = GetMemWidth() + (BLUR_MARGIN*2.f);
			float surfaceh = GetMemHeight() + (BLUR_MARGIN*2.f);
			GLfloat tex[8] = { 0, v, u, v, 0, 0, u, 0 };
			GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
			GLfloat col[16] = { 1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f };

#ifdef _XVAO
			glBindVertexArrayOES( 0 );
#endif
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glEnableVertexAttribArray( XE::ATTRIB_POS );
			glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
			glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
			glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
			glEnableVertexAttribArray( XE::ATTRIB_COLOR );
			glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
			// bind texture
			XGraphicsOpenGL::sBindTexture( idCurrSrcTexture );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}
	}
	// draw back-buffer
	{
		SWAP_RENDER_TARGET();
		GRAPHICS_GL->RestoreFrameBuffer();
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//		glBlendFunc(GL_SRC_ALPHA, GL_ONE );
		MatrixTranslation( mTrans, x - (BLUR_MARGIN*0.5f), y - (BLUR_MARGIN*0.5f), 0 );
		MatrixMultiply( mMVP, mTrans, XE::x_mViewProjection );
		GRAPHICS_GL->GetpBaseShader()->SetShader( mMVP, 1.f, 1.f, 1.f, 1.f );
		float u = (GetMemWidth() + (BLUR_MARGIN*2.f)) / RT_WIDTH;
		float v = (GetMemHeight() + (BLUR_MARGIN*2.f)) / RT_HEIGHT;
#ifdef _XDOWNSCALE_BLUR
		u *= sizeFilter;
		v *= sizeFilter;
#endif
		float surfacew = (BLUR_MARGIN*0.5f) + GetWidth() + (BLUR_MARGIN*0.5f);
		float surfaceh = (BLUR_MARGIN*0.5f) + GetHeight() + (BLUR_MARGIN*0.5f);
		GLfloat tex[8] = { 0, v, u, v, 0, 0, u, 0 };
		GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
		GLfloat col[16] = { 1.0f, 1.0f, 1.0f, m_fAlpha,
			1.0f, 1.0f, 1.0f, m_fAlpha,
			1.0f, 1.0f, 1.0f, m_fAlpha,
			1.0f, 1.0f, 1.0f, m_fAlpha };

#ifdef _XVAO
		glBindVertexArrayOES( 0 );
#endif
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glEnableVertexAttribArray( XE::ATTRIB_POS );
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pos );
		glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, tex );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, col );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
		// bind texture
		XGraphicsOpenGL::sBindTexture( idCurrSrcTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}
	GRAPHICS_GL->RestoreFrameBuffer();


}
#endif // _XBLUR


#endif // gl