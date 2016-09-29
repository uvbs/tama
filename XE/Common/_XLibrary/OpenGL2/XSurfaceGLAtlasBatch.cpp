﻿/********************************************************************
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
#include "etc/xGraphics.h"
#include "etc/XSurface.h"
#include "XSurfaceGLAtlasBatch.h"
#include "etc/xGraphics.h"
#include "XGraphicsOpenGL.h"
#include "etc/Debug.h"
#include "XImage.h"
#include "etc/xMath.h"
#include "XFramework/client/XClientMain.h"
#include "XTextureAtlas.h"
#include "XBatchRenderer.h"

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


XSurfaceGLAtlasBatch::XSurfaceGLAtlasBatch( BOOL bHighReso
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

void XSurfaceGLAtlasBatch::Init()
{
	m_glTexture = 0;
	m_format = xPIXELFORMAT_NONE;
	m_type = 0;
// 	m_glVertexBuffer = m_glIndexBuffer = 0;
// #ifdef _XVAO
// 	m_idVertexArray = 0;
// #endif
}

void XSurfaceGLAtlasBatch::Destroy()
{
	S_TRACE("destroy Surface: %d,%d,%d - %.1f x %.1f", m_glTexture,
																										m_glVertexBuffer,
																										m_glIndexBuffer,
																										GetWidth(), GetHeight() );
	XBREAK( IsbAtlas() == false );
	XTextureAtlas::sRelease( m_glTexture, m_idAtlasNode );
	m_glTexture = 0;
	DestroyDevice();
}

void XSurfaceGLAtlasBatch::ClearDevice()
{
	m_glTexture = 0;
// 	m_glVertexBuffer = 0;
// 	m_glIndexBuffer = 0;
// #ifdef _XVAO
// 	m_idVertexArray = 0;
// #endif
}

/**
 @brief 
 pure virtual
*/
bool XSurfaceGLAtlasBatch::Create( const XE::POINT& sizeSurfaceOrig
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
	if( bUseAtlas ) {

		XE::xRect2 rcInAtlas;
		// 텍스쳐 아틀라스에 pImgSrc를 삽입하고(혹은 아틀라스를 생성) 아틀라스의 아이디를 얻는다. 또한 rect에는 삽입된 위치를 얻는다.
		// pImgSrc를 아틀라스에 배치하고 텍스쳐아이디와 위치를 얻는다.
		XE::VEC2 sizeAtlas;
		XBREAK( XTextureAtlas::sGetspCurrMng() == nullptr );
		auto pAtlasMng = XTextureAtlas::sGetspCurrMng();
		ID idNode = 0;
		m_glTexture = pAtlasMng->ArrangeImg( 0,				// auto glTex id
																				 &rcInAtlas,
																				 &idNode,
																				 pImgSrc,
																				 _sizeMemSrc,
																				 formatImgSrc,
																				 formatSurface,
																				 &sizeAtlas );
		if( XBREAK( m_glTexture == 0 ) )
			return false;
		m_idAtlasNode = idNode;
		// 버텍스버퍼생성.
		XE::VEC2 uvlt = rcInAtlas.vLT / sizeAtlas;
		XE::VEC2 uvrb = rcInAtlas.vRB / sizeAtlas;
		return CreateVertexBuffer2( sizeSurfaceOrig
																, vAdj
																, uvlt, uvrb );
	} else {
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
	}
	if( bUseAtlas ) {
	} else {
	}
}

// pure virtual
/**
 @brief 
*/
bool XSurfaceGLAtlasBatch::CreateSub( const XE::POINT& posMemSrc
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
bool XSurfaceGLAtlasBatch::CreatePNG( LPCTSTR szRes, bool bUseAtlas, 
																			bool bSrcKeep, bool bMakeMask )
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
													, bUseAtlas  );
}

// 인수는 2^정렬에 비율 리사이징까지된것.
/**
 @brief this에 디바이스 버텍스버퍼 객체를 생성한다.
*/
bool XSurfaceGLAtlasBatch::CreateVertexBuffer( float surfaceW, float surfaceH
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
	// 좌우로뒤집힌Z모양
	XE::xVertex vertices[4];
	vertices[0] = { { adjx, surfaceH + adjy, 0.f },{ 0, v },{ 1.0f, 1.0f, 1.0f, 1.0f } }; // left/bottom
	vertices[1] = {{surfaceW + adjx, surfaceH + adjy, 0 }, {u, v},  {1.0f, 1.0f, 1.0f, 1.0f}};  // right/bottom
	vertices[2] = {{adjx, adjy, 0},					{0, 0},  {1.0f, 1.0f, 1.0f, 1.0f}};		// left/top
	vertices[3] = {{surfaceW + adjx, adjy, 0 },		{u, 0},  {1.0f, 1.0f, 1.0f, 1.0f}};	// right/top
	for( int i = 0; i < 4; ++i )
		SetVertex( i, vertices[i] );

	return true;
} // CreateVertexBuffer

bool XSurfaceGLAtlasBatch::CreateVertexBuffer2( const XE::VEC2& sizeSurface,
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
	XE::xVertex vertices[4];
	vertices[0] = { {vAdj.x, sizeSurface.h + vAdj.y, 0},		{u, v2},  {1.0f, 1.0f, 1.0f, 1.0f} };
	vertices[1] = { {sizeSurface.w + vAdj.x, sizeSurface.h + vAdj.y, 0}, {u2, v2},  {1.0f, 1.0f, 1.0f, 1.0f} };  // right/bottom
	vertices[2] = { {vAdj.x, vAdj.y, 0},					{u, v},  {1.0f, 1.0f, 1.0f, 1.0f} };		// left/top
	vertices[3] = { {sizeSurface.w + vAdj.x, vAdj.y, 0},		{u2, v},  {1.0f, 1.0f, 1.0f, 1.0f} };	// right/top
	for( int i = 0; i < 4; ++i )
		SetVertex( i, vertices[i] );

	return true;
} // CreateVertexBuffer2

void XSurfaceGLAtlasBatch::DestroyDevice()
{
	
	CHECK_GL_ERROR();
	if( m_glTexture )
		XSurface::DestroyDevice();
	if( GetstrRes().empty() == false ) {
		S_TRACE("destroy Surface: %s", GetstrRes().c_str() );
	}
	// 홈으로 나갈때 자동으로 디바이스 자원은 파괴되지만 m_glTexture등도 클리어 시켜주지 않으면 돌아왔을때 새로 할당한 번호가 겹쳐서 다시 지워버릴 수 있다.
	auto pAtlasMng = XTextureAtlas::sGetspCurrMng();	// 현재 아틀라스와 실제 m_glTexture가 있는 텍스쳐가 일치하지 않을 수 있음.
	XBREAK( pAtlasMng == nullptr );
// 		pAtlasMng->Release( m_glTexture );
	XTextureAtlas::sRelease( m_glTexture, m_idAtlasNode );
	m_glTexture = 0;
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
void XSurfaceGLAtlasBatch::RestoreDevice()
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
		BOOL bOk = CreatePNG( GetstrRes().c_str(), GetbAtlas(), false, false );
		if( bOk == FALSE ) {
			// 파일을 못찾았으면 국가별 폴더에서 다시 찾아봄
			LPCTSTR szRes = GetstrRes().c_str();
			XTRACE("restore file not found:%s...", szRes);
			TCHAR szLangPath[ 1024 ];
			XE::LANG.ChangeToLangDir( szRes, szLangPath );
			XTRACE("lang path try agin:%s", szLangPath );
			bOk = CreatePNG( szLangPath, GetbAtlas(), false, false );
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
void XSurfaceGLAtlasBatch::RestoreDeviceFromSrcImg()
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
void XSurfaceGLAtlasBatch::CopySurface( XSurface *src )
{
	
	XSurfaceGLAtlasBatch *pSrc = (XSurfaceGLAtlasBatch *)src;
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

// void XSurfaceGLAtlasBatch::DrawBatch( const MATRIX &mParent,
// 																	 const XE::xRenderParam& paramRender ) const
// {
// 	DrawByParam( mParent, paramRender );
// }

void XSurfaceGLAtlasBatch::DrawByParam( const MATRIX &mParent, 
																				const XE::xRenderParam& paramRender ) const
{
	// 	if( XBREAK( XRenderCmdMng::sGetpCurrRenderer() == nullptr ) )
	// 		return;
	XBREAK(XBatchRenderer::sGetpCurrRenderer() == nullptr);
	if( GetsizeMem().w == 0 || GetsizeMem().h == 0 ) {
		// 비동기상태로 로딩을 기다리고 있는 중.
		return;
	}
	// 버텍스 월드변환
	do {
		XBREAK( !paramRender.m_funcBlend );
		if( paramRender.m_funcBlend != xBF_NO_DRAW ) {
			xRenderCmd::xCmd cmd;
			MATRIX mWorld;
			paramRender.GetmTransform( &mWorld );
			MatrixMultiply( mWorld, mWorld, mParent );
			for( int i = 0; i < 4; ++i ) {
				cmd.m_aryVertices[i] = GetVertex( i );
			}
			for( int i = 0; i < 4; ++i ) {
				Vec4 v4d;
				MatrixVec4Multiply( v4d, GetVertex( i ).pos, mWorld );
				cmd.m_aryVertices[i].pos.x = v4d.x;
				cmd.m_aryVertices[i].pos.y = v4d.y;
				cmd.m_aryVertices[i].pos.z = v4d.z + paramRender.m_adjZ;
				cmd.m_aryVertices[i].rgba = (const Vec4)paramRender.m_vColor;
			}
			cmd.m_bBlend = true;
			//			const auto funcBlend = GetfuncBlend();
			const auto funcBlend = paramRender.m_funcBlend;
			sSetglBlendFunc( funcBlend, (GLenum*)&cmd.m_glsFactor, (GLenum*)&cmd.m_gldFactor );
			cmd.m_glBlendEquation = (funcBlend == xBF_SUBTRACT) ?
				GL_FUNC_REVERSE_SUBTRACT
				: GL_FUNC_ADD;
//			cmd.m_v4Color = XE::VEC4( GetColorR(), GetColorG(), GetColorB(), 1.f ); //Getv4Color();
// 			cmd.m_pShader = (funcBlend == xBF_GRAY) ?
// 				cmd.m_pShader = GRAPHICS_GL->GetpGrayShader()
// 				: cmd.m_pShader = GRAPHICS_GL->GetpShaderColTex();
			cmd.m_glTex = m_glTexture;
			cmd.m_bZBuffer = paramRender.m_bZBuff;		// 배치서피스라도 UI등에서 이미지가 겹치지 않는 범위내에서 false로 사용할 수 있다.
			bool bAlphaTest = paramRender.m_bAlphaTest;
			// 영웅은 검기가 있기때문에 배치모드로 생성되었다해도 검기등이 add 블렌딩으로 들어올수 있음. 하지만 zbuff가 꺼지기때문에 정확하게 그려지진 않을 수 있음.
 			if( funcBlend == XE::xBF_ADD || funcBlend == XE::xBF_SUBTRACT ) {
 				cmd.m_bZBuffer = false;
 				bAlphaTest = false;
 			}
//			cmd.m_Priority = GRAPHICS->GetPriority();
			cmd.m_Priority = paramRender.m_Priority;
			if( funcBlend == xBF_GRAY ) {
				cmd.m_pShader = GRAPHICS_GL->GetpGrayShader();
			} else {
				if( bAlphaTest ) {
					cmd.m_pShader = GRAPHICS_GL->GetpShaderColTexAlphaTest();
				} else {
					cmd.m_pShader = GRAPHICS_GL->GetpShaderColTex();
				}
			}
			cmd.m_ltViewport = GRAPHICS->GetViewportLT().ToPoint();
			cmd.m_sizeViewport = GRAPHICS->GetViewportSize().ToPoint();
			//Batch명령 push
			XBatchRenderer::sGetpCurrRenderer()->PushCmd( cmd );
		}
	} while( 0 );
} // DrawByParam
/**
 @brief 
*/
void XSurfaceGLAtlasBatch::Draw( float x, float y, const MATRIX &mParent )
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

void XSurfaceGLAtlasBatch::DrawNoBatch( float x, 
																	float y, 
																	const MATRIX &mParent ) const
{
	XBREAK(1);
}

/*
매트릭스 변환이 없는 코어버전.
*/
void XSurfaceGLAtlasBatch::DrawCore()
{
}

// this의 RECT:src영역을 x,y위치에 그린다.
void XSurfaceGLAtlasBatch::DrawSub( float x, float y, const XE::xRECTi *src )
{
}

//
//-------------------------------------------------------------
// 가변윈도우 프레임 그릴때 사용
void XSurfaceGLAtlasBatch::DrawLocal( float x, float y, float lx, float ly )
{
} // draw local

void XSurfaceGLAtlasBatch::Fill( XCOLOR col )
{
	
}
void*	XSurfaceGLAtlasBatch::Lock( int *pMemW, BOOL bReadOnly)
{
//	*pMemW = GetAlignedWidth();
	*pMemW = GetsizeMemAligned().w;
	int w, h;
	return GetSrcImg( &w, &h );
}

void XSurfaceGLAtlasBatch::SetTexture()
{
	XGraphicsOpenGL::sBindTexture( m_glTexture );
}

void XSurfaceGLAtlasBatch::UpdateUV( ID idTex,
																		 const XE::POINT& sizePrev,
																		 const XE::POINT& sizeNew )
{
	if( m_glTexture != idTex )
		return;
	for( int i = 0; i < 4; ++i ) {
		auto vertex = GetVertex( i );
		// 새 크기 기준으로 uv를 다시 계산
		float xPrev = sizePrev.w * vertex.uv.x;
		float yPrev = sizePrev.h * vertex.uv.y;
		vertex.uv.x = xPrev / sizeNew.w;
		vertex.uv.y = yPrev / sizeNew.h;
		SetVertex( i, vertex );
	}
}


#endif // gl