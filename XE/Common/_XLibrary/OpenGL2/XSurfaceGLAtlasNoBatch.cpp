/********************************************************************
	@date:	2016/09/16 19:56
	@file: 	C:\p4v\iPhone_zero\XE\Common\_XLibrary\OpenGL2\XSurfaceOpenGL2.cpp
	@author:	xuzhu
	
	@brief:	Batch Render버전(spr에서 사용)
*********************************************************************/
#include "stdafx.h"
#ifdef _VER_OPENGL
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "XSurfaceGLAtlasNoBatch.h"
#include "etc/xGraphics.h"
#include "XGraphicsOpenGL.h"
#include "etc/Debug.h"
#include "XImage.h"
#include "etc/xMath.h"
#include "XFramework/client/XClientMain.h"
#include "XTextureAtlas.h"

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

//////////////////////////////////////////////////////////////////////////
XSurfaceGLAtlasNoBatch::XSurfaceGLAtlasNoBatch( BOOL bHighReso
																								, const int srcx, const int srcy
																								, const int srcw, const int srch
																								, const int dstw, const int dsth
																								, const int adjx, const int adjy
																								, void *_pSrcImg
																								, const int bpp
																								, BOOL bSrcKeep )
	: XSurface( bHighReso )
	, m_Vertices( 4 )
{
	Init();
	auto sizeDstMem = XE::POINT( dstw, dsth ) * 2;
	XSurface::CreateSub( XE::POINT( srcx, srcy )
											 , sizeDstMem // XE::POINT(memDstw, memDsth)
											 , XE::POINT( srcw, srch )
											 , _pSrcImg
											 , XE::xPF_ARGB8888
											 , XE::VEC2( dstw, dsth )
											 , XE::VEC2( adjx, adjy )
											 , XE::xPF_ARGB4444
											 , bSrcKeep != FALSE
											 , false );
}

void XSurfaceGLAtlasNoBatch::Init()
{
	m_glTexture = 0;
	m_format = xPIXELFORMAT_NONE;
	m_type = 0;
}

void XSurfaceGLAtlasNoBatch::Destroy()
{
	S_TRACE("destroy Surface: %d,%d,%d - %.1f x %.1f", m_glTexture,
																										m_glVertexBuffer,
																										m_glIndexBuffer,
																										GetWidth(), GetHeight() );
	DestroyDevice();
}

void XSurfaceGLAtlasNoBatch::ClearDevice()
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
bool XSurfaceGLAtlasNoBatch::Create( const XE::POINT& sizeSurfaceOrig
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
// 	if( bUseAtlas ) {
		XE::xRect2 rcInAtlas;
		// 텍스쳐 아틀라스에 pImgSrc를 삽입하고(혹은 아틀라스를 생성) 아틀라스의 아이디를 얻는다. 또한 rect에는 삽입된 위치를 얻는다.
		// pImgSrc를 아틀라스에 배치하고 텍스쳐아이디와 위치를 얻는다.
		XE::VEC2 sizeAtlas;
		XBREAK( XTextureAtlas::sGetpCurrMng() == nullptr );
		auto pAtlasMng = XTextureAtlas::sGetpCurrMng();
		m_glTexture = pAtlasMng->ArrangeImg( 0,				// auto glTex id
																				 &rcInAtlas,
																				 pImgSrc,
																				 _sizeMemSrc,
																				 formatImgSrc,
																				 formatSurface,
																				 &sizeAtlas );
		if( XBREAK( m_glTexture == 0 ) )
			return false;
		// 버텍스버퍼생성.
		XE::VEC2 uvlt = rcInAtlas.vLT / sizeAtlas;
		XE::VEC2 uvrb = rcInAtlas.vRB / sizeAtlas;
		return CreateVertexBuffer2( sizeSurfaceOrig
																, vAdj
																, uvlt, uvrb );
// 	} else {
		// no atlas
// 		m_glTexture = GRAPHICS_GL->CreateTextureGL( pImgSrc
// 																								, _sizeMemSrc
// 																								, formatImgSrc
// 																								, _sizeMemSrcAligned
// 																								, formatSurface );
// 		if( XBREAK( m_glTexture == 0 ) )
// 			return false;
// 	}
}

// pure virtual
/**
 @brief 
*/
bool XSurfaceGLAtlasNoBatch::CreateSub( const XE::POINT& posMemSrc
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
	return CreateVertexBuffer2( sizeRender.ToPoint()
													, vAdj
													, sizeArea
													, sizeAreaAligned );
}

/**
 @brief 기존코드 호환용.(단 외부에서 쓰는건 막음)
*/
bool XSurfaceGLAtlasNoBatch::CreatePNG( LPCTSTR szRes, bool bSrcKeep, bool bMakeMask )
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

bool XSurfaceGLAtlasNoBatch::CreateVertexBuffer2( const XE::VEC2& sizeSurface,
																										const XE::VEC2& vAdj,
																										const XE::VEC2& uvlt,
																										const XE::VEC2& uvrb ) {

	const float u = uvlt.x;		// 아틀라스내 좌상귀
	const float v = uvlt.y;
	const float u2 = uvrb.x;		// 아틀라스내 우하귀
	const float v2 = uvrb.y;
	XBREAK( u < 0 || u > 1.0f );
	XBREAK( v < 0 || v > 1.0f );
	XBREAK( u2 < 0 || u2 > 1.0f );
	XBREAK( v2 < 0 || v2 > 1.0f );
	const xVertexNoBatch vertices[4] = {
		0 + vAdj.x, sizeSurface.h + vAdj.y,							u, v2,		1.0f, 1.0f, 1.0f, 1.0f,	// left/bottom
		sizeSurface.w + vAdj.x, sizeSurface.h + vAdj.y, u2, v2,		1.0f, 1.0f, 1.0f, 1.0f,  // right/bottom
		vAdj.x, vAdj.y,																	u, v,			1.0f, 1.0f, 1.0f, 1.0f,		// left/top
		sizeSurface.w + vAdj.x, vAdj.y,									u2, v,		1.0f, 1.0f, 1.0f, 1.0f	// right/top
	};
	for( int i = 0; i < 4; ++i )
		m_Vertices[i] = vertices[i];
	static const GLubyte indices[4] = { 0, 1, 2, 3 };
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
	CHECK_GL_ERROR();

	return true;
}

void XSurfaceGLAtlasNoBatch::DestroyDevice()
{
	CHECK_GL_ERROR();
	if( m_glTexture )
		XSurface::DestroyDevice();
	if( GetstrRes().empty() == false ) {
		S_TRACE("destroy Surface: %s", GetstrRes().c_str() );
	}
	// 홈으로 나갈때 자동으로 디바이스 자원은 파괴되지만 m_glTexture등도 클리어 시켜주지 않으면 돌아왔을때 새로 할당한 번호가 겹쳐서 다시 지워버릴 수 있다.
	if( IsbAtlas() ) {
		auto pAtlasMng = XTextureAtlas::sGetpCurrMng();	// 현재 아틀라스와 실제 m_glTexture가 있는 텍스쳐가 일치하지 않을 수 있음.
		XBREAK( pAtlasMng == nullptr );
		pAtlasMng->Release( m_glTexture );
	} else {
// 		if( m_glTexture ) {
// 			glDeleteTextures( 1, &m_glTexture );
// 		}
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
	CHECK_GL_ERROR();
	}


/**
 OpenGL 디바이스 자원을 다시 생성한다.
 보관해 두었던 파일명을 기반으로 다시 로딩한다.
 파일로 로딩한게 아니었다면 pSrcImg로 복원하고
 그것마저 없다면 에러를 낸다.
 */
void XSurfaceGLAtlasNoBatch::RestoreDevice()
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
void XSurfaceGLAtlasNoBatch::RestoreDeviceFromSrcImg()
{
// 	XE::POINT sizeMem;
// 	DWORD *pSrcImg = GetSrcImg( &sizeMem );
// 	XBREAK( pSrcImg == NULL );
// 	const float wSurface = GetWidth();
// 	const float hSurface = GetHeight();
// 	const float adjx = GetAdjustX();
// 	const float adjy = GetAdjustY();
// 	const auto sizeMemAligned = GetsizeMemAligned();
// 	// 버텍스 버퍼 재생성
// 	CreateVertexBuffer( GetSize().ToPoint(), 
// 											GetAdjust(),
// 											sizeMem, 
// 											sizeMemAligned );
// 	// 텍스쳐 재생성
// 	XE::POINT sizeAligned;
// 	const auto formatImgSrc = XE::xPF_ARGB8888;
// 	const auto formatSurface = GetformatSurface();
// 	m_glTexture = GRAPHICS_GL->CreateTextureGL( (void* const)pSrcImg, sizeMem, formatImgSrc, sizeMemAligned, formatSurface );
// 	if( m_glTexture ) {
// 		XSurface::RestoreDeviceFromSrcImg();
// 	}
// 	XBREAK( m_glTexture == 0 );
}

// src서피스를 this로 카피.
void XSurfaceGLAtlasNoBatch::CopySurface( XSurface *src )
{
	
	XSurfaceGLAtlasNoBatch *pSrc = (XSurfaceGLAtlasNoBatch *)src;
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
/**
 @brief 
*/
void XSurfaceGLAtlasNoBatch::Draw( float x, float y, const MATRIX &mParent )
{
	XE::xRenderParam paramRender;
	paramRender.m_vPos.Set( x, y, 0 );
	paramRender.m_vAdjAxis = GetAdjustAxis();
	paramRender.m_vRot = GetvRotate();
	paramRender.m_vScale = GetScale();
	paramRender.m_vColor = Getv4Color();
	paramRender.m_dwDrawFlag = GetdwDrawFlag();
	paramRender.m_funcBlend = GetfuncBlend();
	paramRender.m_adjZ = GetadjZ();
	//
	DrawByParam( mParent, paramRender );
	XSurface::ClearAttr();		// 이건 장차 없어질 예정
}

void XSurfaceGLAtlasNoBatch::DrawByParam(
	const MATRIX &mParent,
	const XE::xRenderParam& paramRender ) const
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

Restore코드
DrawLocal코드
아틀라스에 줄생기는거 해결



/*
매트릭스 변환이 없는 코어버전.
*/
void XSurfaceGLAtlasNoBatch::DrawCore() const
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
		glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, xy ) );
		glEnableVertexAttribArray( XE::ATTRIB_TEXTURE );
		glVertexAttribPointer( XE::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, uv ) );
		glEnableVertexAttribArray( XE::ATTRIB_COLOR );
		glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof( xVertexNoBatch ), (void*)offsetof( xVertexNoBatch, rgba ) );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer );
		CHECK_GL_ERROR();
		// bind texture
#ifdef _XPROFILE
		if( !(XGraphics::s_dwDraw & XE::xeBitNoTexture) )
#endif // _XPROFILE
			XGraphicsOpenGL::sBindTexture( m_glTexture );
		CHECK_GL_ERROR();

#ifdef _XPROFILE
		if( !(XGraphics::s_dwDraw & XE::xeBitNoDP) )
#endif // _XPROFILE
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		CHECK_GL_ERROR();

}

// this의 RECT:src영역을 x,y위치에 그린다.
void XSurfaceGLAtlasNoBatch::DrawSub( float x, float y,
																			const XE::xRECTi *src )
{
	CHECK_GL_ERROR();
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDraw )
		return;
#endif // _XPROFILE
	XBREAK( GetsizeMem().IsZero() );
	int memw, memh;
//	GLfloat l, t, r, b;
	XE::VEC2 uvlt, uvrb;
	const auto sizeMemAligned = GetsizeMemAlignedVec2();
	if( src ) {
		XE::xRECTi memRect;
		memRect.SetLeft( src->GetLeft() * 2 );
		memRect.SetRight( src->GetRight() * 2 );
		memRect.SetTop( src->GetTop() * 2 );
		memRect.SetBottom( src->GetBottom() * 2 );
		memw = ( memRect.Right() - memRect.Left() );
		memh = ( memRect.Bottom() - memRect.Top() );
		XE::VEC2 uvLT( m_Vertices[2].uv );		// left-top
		XE::VEC2 uvRB( m_Vertices[1].uv );		// right-bottom
		const auto sizeMem = GetsizeMem();
		const auto sizeUV = (uvRB - uvLT);
		uvlt = uvLT + sizeUV * (memRect.ptLT.ToVec2() / sizeMem);
		uvrb = uvLT + sizeUV * (memRect.ptRB.ToVec2() / sizeMem);
// 		l = memRect.GetLeft() / (GLfloat)sizeMemAligned.w;
// 		t = memRect.GetTop() / (GLfloat)sizeMemAligned.h;
// 		r = memRect.GetRight() / (GLfloat)sizeMemAligned.w;
// 		b = memRect.GetBottom() / (GLfloat)sizeMemAligned.h;
	} else {
		// src가 지정되어 있지 않으면 전체 출력.
		Draw( x, y );
		CHECK_GL_ERROR();
		return;
	}
	GLfloat tex[ 8 ] = {uvlt.x, uvrb.y, uvrb.x, uvrb.y, uvlt.x, uvlt.y, uvrb.x, uvlt.y };
	float surfacew = (float)( src->GetRight() - src->GetLeft() );
	float surfaceh = (float)( src->GetBottom() - src->GetTop() );
	GLfloat pos[ 8 ] = {0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0};
	const float alpha = GetfAlpha();
	GLfloat col[ 16 ] = {1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha,
											1.0f, 1.0f, 1.0f, alpha};

	CHECK_GL_ERROR();
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
	GRAPHICS_GL->GetpShaderColTex()->SetShader( mMVP, vColor );
	CHECK_GL_ERROR();
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
	//
	CHECK_GL_ERROR();
	XSurface::ClearAttr();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	CHECK_GL_ERROR();
}

//
//-------------------------------------------------------------
// 가변윈도우 프레임 그릴때 사용
void XSurfaceGLAtlasNoBatch::DrawLocal( float x, float y, float lx, float ly )
{
// #ifdef _XPROFILE
// 	if( XGraphics::s_dwDraw & XE::xeBitNoDraw )
// 		return;
// #endif // _XPROFILE
// 	
// 	XBREAK( GetsizeMem().IsZero() );
// 	CHECK_GL_ERROR();
// 	if( GetDrawMode() != xDM_NONE )	{
// 		glEnable( GL_BLEND );
// 		glBlendEquation( GL_FUNC_ADD );
// 		if( GetDrawMode() == xDM_NORMAL )
// 			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
// 		else
// 		if( GetDrawMode() == xDM_SCREEN )
// 			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
// 		else
// 		if( GetDrawMode() == xDM_MULTIPLY )
// 			glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
// 		else
// 		if( GetDrawMode() == xDM_SUBTRACT )	{
// 			glBlendFunc( GL_ONE, GL_ONE );
// 			//glBlendEquation (GL_FUNC_SUBTRACT);
// 			glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
// 		}
// 		CHECK_GL_ERROR();
// 		MATRIX mWorld, m;
// 		MatrixIdentity( mWorld );
// 		MatrixTranslation( m, lx, ly, 0 );
// 		MatrixMultiply( mWorld, mWorld, m );
// 		if( !GetAdjustAxis().IsZero() ) {
// 			MatrixTranslation( m, -GetfAdjustAxisX(), -GetfAdjustAxisY(), 0 );
// 			MatrixMultiply( mWorld, mWorld, m );
// 		}
// 		const auto vScale = GetScale();
// 		if( vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f ) {
// 			MatrixScaling( m, vScale.x, vScale.y, 1.0f );
// 			MatrixMultiply( mWorld, mWorld, m );
// 		}
// 		if( GetfRotZ() ) {
// 			MatrixRotationZ( m, D2R( GetfRotZ() ) );
// 			MatrixMultiply( mWorld, mWorld, m );
// 		}
// 		if( GetfRotY() ) {
// 			MatrixRotationY( m, D2R(GetfRotY()) );
// 			MatrixMultiply( mWorld, mWorld, m );
// 		}
// 		if( !GetAdjustAxis().IsZero() ) {
// 			MatrixTranslation( m, GetfAdjustAxisX(), GetfAdjustAxisY(), 0 );
// 			MatrixMultiply( mWorld, mWorld, m );
// 		}
// 		MATRIX mMVP;
// 		MatrixTranslation( m, x, y, 0 );
// 		MatrixMultiply( mWorld, mWorld, m );
// 		MatrixMultiply( mMVP, mWorld, XE::x_mViewProjection );
// 		const XE::VEC4 vColor = Getv4Color();
// 		GRAPHICS_GL->GetpShaderColTex()->SetShader( mMVP, vColor );
// 		CHECK_GL_ERROR();
// 		DrawCore();
// 	}
// 	XSurface::ClearAttr();
// 	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
// 	CHECK_GL_ERROR();
} // draw local

void XSurfaceGLAtlasNoBatch::Fill( XCOLOR col )
{
	
}
void*	XSurfaceGLAtlasNoBatch::Lock( int *pMemW, BOOL bReadOnly)
{
//	*pMemW = GetAlignedWidth();
	*pMemW = GetsizeMemAligned().w;
	int w, h;
	return GetSrcImg( &w, &h );
}

void XSurfaceGLAtlasNoBatch::SetTexture()
{
	XGraphicsOpenGL::sBindTexture( m_glTexture );
}

#endif // gl