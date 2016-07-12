/*
 *  XSurfaceOpenGL.cpp
 *  BnBTest
 *
 *  Created by xuzhu on 09. 07. 16.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "XSurfaceOpenGL.h"
#include "XGraphics.h"
#include "XGraphicsOpenGL.h"
#include "Debug.h"
#include "XImage.h"
//#include "etc.h"

void XSurfaceOpenGL::Init( void )
{
	m_textureID = 0;
	m_format = xPIXELFORMAT_NONE;
	m_type = 0;
	m_vertexBufferID = m_indexBufferID = 0;
//	m_pSrcImg = NULL;
}

void XSurfaceOpenGL::Destroy( void )
{
	if( m_textureID )
	{
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
	}
	if( m_vertexBufferID )
	{
		glDeleteBuffers(1, &m_vertexBufferID);
		m_vertexBufferID = 0;
	}
	if( m_indexBufferID )
	{
		glDeleteBuffers(1, &m_indexBufferID);
		m_indexBufferID = 0;
	}
	Init();
}

#include "XImage.h"

BOOL XSurfaceOpenGL::Create( LPCTSTR szFilename, XCOLOR dwColorKey, BOOL bSrcKeep )
{
    XBREAK(1);  // WORD타입으로 할때 문제가 있다 다시 작성할것
/*	XImageiOS<unsigned short> *pImage = new XImageiOS<unsigned short>( GetbHighReso(), szFilename );
	if( pImage == NULL )
		XERROR( "XSurfaceOpenGL::Create() : %s", szFilename );
	Create( pImage->GetWidth(), pImage->GetHeight(), 0, 0, 
		   (dwColorKey)? xALPHA : xNO_ALPHA, (WORD *)pImage->GetTextureData(), dwColorKey, bSrcKeep );
	SAFE_DELETE_ARRAY( pImage );*/
	return TRUE;
}

BOOL XSurfaceOpenGL::CreatePNG( LPCTSTR szFilename, BOOL bSrcKeep )
{
//	g_Pool.AllocMark(5);
	XImage<DWORD> *pImage = new XImageiOS<DWORD>( GetbHighReso() );
	pImage->Load( szFilename );

	// 빈 텍스쳐 만듬.
    SetSrcImg( NULL, pImage->GetMemWidth(), pImage->GetMemHeight() );
    int memw = GetMemWidth();
    int memh = GetMemHeight();
//	m_nWidth = pImage->GetWidth();
//	m_nHeight = pImage->GetHeight();
	// w,h값을 2^단위로 정렬한다.
	GLsizei alignedW = pImage->GetMemWidth();
	GLsizei alignedH = pImage->GetMemHeight();
	GRAPHICS->AlignPowSize( &alignedW, &alignedH );
	m_nAlignedWidth = alignedW;
	m_nAlignedHeight = alignedH;
	
	// 정렬된 크기로 버텍스버퍼생성.
	CreateVertexBuffer( GetWidth(), GetHeight(), 0, 0, memw, memh, alignedW, alignedH );

	glGenTextures( 1, &m_textureID );
	XBREAK( m_textureID == 0 );
	glBindTexture(GL_TEXTURE_2D, m_textureID );
	if( m_textureID == 0 )
	{
		XERROR( "memw:%d memh:%d", GetMemWidth(), GetMemHeight() );
		return FALSE;
	}
    // gl텍스쳐로 쓰기 위해 크기를 정렬한다
    {
//		g_Pool.AllocMark(4);
        DWORD *temp, *_temp, *src;
        temp = _temp = new DWORD[ alignedW * alignedH ];
        memset( temp, 0, alignedW * alignedH * sizeof(DWORD) );
        int i, j;	
        src = pImage->GetTextureData();
        for( i = 0; i < memh; i ++ )
        {
            for( j = 0; j < memw; j ++ )
            {
                *_temp++ = *src++;
            }
            _temp += (alignedW - memw);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 
                     0, 
                     GL_RGBA, 
                     alignedW,
                     alignedH,
                     0, 
                     GL_RGBA, 
                     GL_UNSIGNED_BYTE, 
                     temp );
        SAFE_DELETE( temp );
//		g_Pool.RestoreMark(4);
    }
	// 클래스내 보관용 이미지를 가져옴
    if( bSrcKeep )
    {
        DWORD *pSrcImg;
        pImage->MovePtr( &pSrcImg );    // Image메모리 소유권을 가져온다
        SetSrcImg( pSrcImg, memw, memh );
    }
	SAFE_DELETE( pImage );
//	g_Pool.RestoreMark(5);
/*	if( bSrcKeep == FALSE ) {
		SAFE_DELETE( temp );
	}
	else
		m_pSrcImg = temp; */
	return TRUE;
}

// pImg로 버텍스/텍스쳐버퍼 생성.
xRESULT XSurfaceOpenGL::Create( const float surfaceW, const float surfaceH, 
							  const float adjx, const float adjy, 
							  xAlpha alpha, const void *pImg, int bytePerPixel, XCOLOR dwKeyColor, BOOL bSrcKeep )
{
	// 빈 텍스쳐 만듬.
    float memSrcw = ConvertToMemSize( surfaceW );
    float memSrch = ConvertToMemSize( surfaceH );
//	m_nWidth = width;
//	m_nHeight = height;
    SetAdjust( adjx, adjy );
	GLsizei alignedW = memSrcw;
	GLsizei alignedH = memSrch;
//	m_AdjustX = adjx;
//	m_AdjustY = adjy;
	// w,h값을 2^단위로 정렬한다.
	GRAPHICS->AlignPowSize( &alignedW, &alignedH );
	
	// 정렬된 크기로 버텍스버퍼생성.
//	CreateVertexBuffer( alignedW, alignedH, adjx, adjy );
	CreateVertexBuffer( surfaceW, surfaceH, adjx, adjy, memSrcw, memSrch, alignedW, alignedH );
	
	m_nAlignedWidth = alignedW;
	m_nAlignedHeight = alignedH;
	
	// pImg내용을 this로 옮김.
	Blt( (DWORD *)pImg, NULL, bytePerPixel, alpha, memSrcw, memSrch, dwKeyColor, bSrcKeep );
	
	return xSUCCESS;
}

typedef struct tagSTRUCT_VERTEX_SURFACE{
	GLfloat x[2];//, y;
	GLfloat t[2];//u, tv;
	GLfloat c[4];// r,g,b,a;
} STRUCT_VERTEX_SURFACE;

// 인수는 2^정렬에 비율 리사이징까지된것.
xRESULT XSurfaceOpenGL::CreateVertexBuffer( float surfaceW, float surfaceH, const float _adjx, const float _adjy, int memw, int memh, int alignW, int alignH )
{
//	int width = nAlignedW;
//	int height = nAlignedH;
    GLsizei adjx = (GLsizei)_adjx;
    GLsizei adjy = (GLsizei)_adjy;
	
    float u = (float)memw / alignW; // 텍스쳐 우하귀의 u,v좌표 
    float v = (float)memh / alignH;
	const STRUCT_VERTEX_SURFACE vertices[4] = 
	{ 
		//adjx, surfaceH+adjy,		0, 1,  1.0f, 1.0f, 1.0f, m_fAlpha,	// left/bottom
		//surfaceW+adjx, surfaceH+adjy,1, 1,  1.0f, 1.0f, 1.0f, m_fAlpha,  // right/bottom
		//adjx, adjy,				0, 0,  1.0f, 1.0f, 1.0f, m_fAlpha,		// left/top
		//surfaceW+adjx, adjy,		1, 0,  1.0f, 1.0f, 1.0f, m_fAlpha	// right/top
		adjx, surfaceH+adjy,		0, v,  1.0f, 1.0f, 1.0f, m_fAlpha,	// left/bottom
		surfaceW+adjx, surfaceH+adjy,u, v,  1.0f, 1.0f, 1.0f, m_fAlpha,  // right/bottom
		adjx, adjy,				0, 0,  1.0f, 1.0f, 1.0f, m_fAlpha,		// left/top
		surfaceW+adjx, adjy,		u, 0,  1.0f, 1.0f, 1.0f, m_fAlpha	// right/top
	};
	static GLubyte indices[4] = { 0, 1, 2, 3 };
	
	glGenBuffers(1, &m_vertexBufferID);
	glGenBuffers(1, &m_indexBufferID);
	if( XBREAK( m_vertexBufferID == 0 ) )	// 나중에 assert종류로 바꾸자.
		return xFAIL;
	if( XBREAK( m_indexBufferID == 0 ) )
		return xFAIL;
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_FLOAT, sizeof(STRUCT_VERTEX_SURFACE), (void*)offsetof(STRUCT_VERTEX_SURFACE,x));
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glTexCoordPointer(2, GL_FLOAT, sizeof(STRUCT_VERTEX_SURFACE), (void*)offsetof(STRUCT_VERTEX_SURFACE,t));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return xSUCCESS;
}

void XSurfaceOpenGL::Blt( const DWORD *pSrc,
						 RECT *pmemSrcRect,	// 소스의 일부분만 옮기고 싶을때. 널이면 전체
						 int bytePerPixel,	// pSrc의 bytePerPixel
						 xAlpha alpha,
						 float _memSrcw, float _memSrch,
						 XCOLOR dwKeyColor, BOOL bSrcKeep
						 )
{
    GLint memSrcw = (GLint)_memSrcw;
    GLint memSrch = (GLint)_memSrch;
	glGenTextures(1, &m_textureID);
	
	if( XBREAKF( m_textureID == 0, "w:%d h:%d", memSrcw, memSrch ) )
		return;
	
	const GLsizei alignedW = m_nAlignedWidth;		// 이미 CreateBlank를 했다는 가정.
	const GLsizei alignedH = m_nAlignedHeight;
	
	if( pSrc )		// NULL이면 빈텍스쳐로 만듬.
	{
		void *alignedImg = NULL;
		if( bytePerPixel == 2 )
		{
			XBREAK(1);	// pSrcRect지원땜에 많이 바꼈으니 다시 작성할것
/*			if( alpha == xALPHA )
			{
				m_format = GL_RGBA;
				m_type = GL_UNSIGNED_BYTE;
				// 2^단위로 정렬된 크기로 텍스쳐를 만들기위해 임시버퍼에 한번찍고 그것을 다시 텍스쳐로 전송한다.
				{
					DWORD *temp, *_temp, rgba;
					temp = _temp = (DWORD *)malloc( alignedW * alignedH * sizeof(DWORD) );
					memset( temp, 0, alignedW * alignedH * sizeof(DWORD) );
					const WORD *src = (const WORD *)pSrc;
					BYTE r = XCOLOR_RGB_R( dwKeyColor );
					BYTE g = XCOLOR_RGB_G( dwKeyColor );
					BYTE b = XCOLOR_RGB_B( dwKeyColor );
					WORD wKeyColor = XCOLOR_RGB565( r, g, b );
					int i, j;
					WORD data;
					for( i = 0; i < memSrch; i ++ )
					{
						for( j = 0; j < memSrcw; j ++ )
						{
							data = *src++;
							if( data == wKeyColor )
								rgba = GRAPHICS->Convert565ToRGBA8( data, 0 );
							else
								rgba = GRAPHICS->Convert565ToRGBA8( data, 255 );
							*_temp++ = rgba;
						}
						_temp += (alignedW - memSrcw);
					}
					alignedImg = temp;
				} 
			} else
			{
				m_format = GL_RGB;
				m_type = GL_UNSIGNED_SHORT_5_6_5;
                SetSrcImg( (WORD*)pSrc, memSrcw, memSrch )
				// bSrcKeep이 TRUE면 원본크기와 정렬된 크기가 같은지 여부에 관계없이 복사본을 만들어야 한다.
				if( bSrcKeep || (memSrcw != alignedW || memSrch != alignedH) )	// 정렬된 크기가 같으면 굳이 재배열할필요 없다.
				{
					// 2^단위로 정렬된 크기로 텍스쳐를 만들기위해 임시버퍼에 한번찍고 그것을 다시 텍스쳐로 전송한다.
					WORD *temp, *_temp;
					int size = alignedW * alignedH * sizeof(WORD);
					temp = _temp = (WORD *)malloc( size );
					memset( temp, 0, size );
					const WORD *src = (const WORD *)pSrc;
					int i, j;
					for( i = 0; i < memSrch; i ++ )
					{
						for( j = 0; j < memSrcw; j ++ )
							*_temp++ = *src++;
						_temp += (alignedW - memSrcw);
					}
					alignedImg = temp;
				}
			} */
		} else 
		if( bytePerPixel == 4 )
		{
			m_format = GL_RGBA;
			m_type = GL_UNSIGNED_BYTE;
			if( pmemSrcRect == NULL )	// 이미지전체
			{
				int dstw = memSrcw, dsth = memSrch;
				int dstSize = dstw * dsth;
				if( bSrcKeep )		// 이미지 보관
				{
					DWORD *pImg = new DWORD[ dstSize ];
					memcpy( pImg, pSrc, dstSize * sizeof(DWORD) );
                    SetSrcImg( (DWORD *)pImg, dstw, dsth );
				} else
                    SetSrcImg( NULL, dstw, dsth );
				// 2^단위로 정렬된 크기로 텍스쳐를 만들기위해 임시버퍼에 한번찍고 그것을 다시 텍스쳐로 전송한다.
				{
					DWORD *_temp;
					alignedImg = _temp = new DWORD[ alignedW * alignedH ];
					memset( alignedImg, 0, alignedW * alignedH * sizeof(DWORD) );
					const DWORD *src = (const DWORD *)pSrc;
					int i, j;
					for( i = 0; i < memSrch; i ++ )
					{
						for( j = 0; j < memSrcw; j ++ )
						{
							*_temp++ = *src++;
						}
						_temp += (alignedW - memSrcw);
					}
				}
			} else 
			{	// 이미지 일부분만 옮김
				int memDstw = (pmemSrcRect->right - pmemSrcRect->left + 1);
				int memDsth = (pmemSrcRect->bottom - pmemSrcRect->top + 1);
				int dstSize = memDstw * memDsth;
				if( bSrcKeep )
				{
                    DWORD *pDst = new DWORD[ dstSize ];
					DWORD *_pDst = pDst;
					const DWORD *_pSrc = pSrc + (pmemSrcRect->top * memSrcw + pmemSrcRect->left);
					for( int i = 0; i < memDsth; i ++ )
					{
						for( int j = 0; j < memDstw; j ++ )
						{
							*_pDst++ = *_pSrc++;
						}
						_pSrc += (memSrcw - memDstw);
					}
                    SetSrcImg( (DWORD *)pDst, memDstw, memDsth );
				} else
                    SetSrcImg( NULL, memDstw, memDsth );
				// 정렬된 이미지 만들기
				{
					DWORD *_temp = new DWORD[ alignedW * alignedH ];
					alignedImg = (void *)_temp;
					memset( alignedImg, 0, alignedW * alignedH * sizeof(DWORD) );
					const DWORD *_pSrc = pSrc + (pmemSrcRect->top * memSrcw + pmemSrcRect->left);
					int i, j;
					for( i = 0; i < memDsth; i ++ )
					{
						for( j = 0; j < memDstw; j ++ )
						{
							*_temp++ = *_pSrc++;
						}
						_temp += (alignedW - memDstw);
						_pSrc += (memSrcw - memDstw);
					}
				}
			}
		}
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 
					0, 
					m_format, 
					alignedW, 
					alignedH, 
					0, 
					m_format, 
					m_type, 
					alignedImg);
        DWORD *pTemp = (DWORD*)alignedImg;
		SAFE_DELETE_ARRAY( pTemp );
        alignedImg = NULL;

	} else // (alignedImg)
	{
		if( alpha == xALPHA )
		{
			m_format = GL_RGBA;
			m_type = GL_UNSIGNED_BYTE;
		} else
		{
			m_format = GL_RGB;
			m_type = GL_UNSIGNED_SHORT_5_6_5;
		}
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 
					 0, 
					 m_format, 
					 alignedW, 
					 alignedH, 
					 0, 
					 m_format, 
					 m_type, 
					 NULL );
	}
	
	
}

// 이미지의 일부분만 서피스로 만드는 버전
xRESULT XSurfaceOpenGL::Create( const int memSrcx, const int memSrcy, const int memSrcw, const int        memSrch, const float surfaceDstw, const float surfaceDsth, const float adjx, const float adjy,  void *_pSrcImg, const int bpp, BOOL bSrcKeep )
{
	if( XBREAK( bpp == 2 ) )
		return xFAIL;
	// 빈 텍스쳐 만듬.
	
//	m_nWidth = dstw;
//	m_nHeight = dsth;
    SetAdjust( adjx, adjy );
//	m_AdjustX = adjx;
//	m_AdjustY = adjy;
	// w,h값을 2^단위로 정렬한다.
    int memDstw = ConvertToMemSize( surfaceDstw );
    int memDsth = ConvertToMemSize( surfaceDsth );
	GLsizei alignedW = memDstw;
	GLsizei alignedH = memDsth;
	GRAPHICS->AlignPowSize( &alignedW, &alignedH );
	
	// 정렬된 크기로 버텍스버퍼생성.
	CreateVertexBuffer( surfaceDstw, surfaceDsth, adjx, adjy, memDstw, memDsth, alignedW, alignedH );
	
	m_nAlignedWidth = alignedW;
	m_nAlignedHeight = alignedH;
	
	// pImg내용을 this로 옮김.
//    int areaw = ConvertToMemSize( surfaceDstw );
  //  int areah = ConvertToMemSize( surfaceDsth );
	RECT rect = { memSrcx, memSrcy, memSrcx+memDstw-1, memSrcy+memDsth-1 };
	Blt( (DWORD *)_pSrcImg, &rect, bpp, xNO_ALPHA, memSrcw, memSrch, 0, bSrcKeep );
	
	return xSUCCESS;
}

// src서피스를 this로 카피.
void XSurfaceOpenGL::CopySurface( XSurface *src )
{
	XSurfaceOpenGL *pSrc = (XSurfaceOpenGL *)src;
	// src를 FBO에 연결
	// glCopyTexImage를 이용해 src에서 this로 옮김.
	GLuint fbo;
	glGenFramebuffersOES(1, &fbo);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, pSrc->GetTextureID(), 0);
	
	GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	if( status != GL_FRAMEBUFFER_COMPLETE_OES )
	{
		XLOG( "status=%d", status );
		return;
	}
	
	// copy texture from framebuffer
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, pSrc->GetMemWidth(), pSrc->GetMemHeight());    
	
	// FBO해제
	GRAPHICS_GL->RestoreFrameBuffer();
	glDeleteFramebuffersOES(1, &fbo);
}


#pragma mark Draw

void XSurfaceOpenGL::Draw( float x, float y )
{
	if( GetDrawMode() != xDM_NONE )
	{
		glPushMatrix();

		if( GetDrawMode() == xDM_NORMAL )
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else 
		if( GetDrawMode() == xDM_SCREEN )
			glBlendFunc(GL_SRC_ALPHA, GL_ONE );
//		x += m_fAdjustAxisX;
//		y += m_fAdjustAxisY;
		glTranslatef( x, y, 0);	// 어느쪽을하든 차이가 없더라. 아이폰/아이패드 둘다 3g는 테스트 안해봄 
        glTranslatef( m_fAdjustAxisX, m_fAdjustAxisY, 0 );
		if( m_fRotX )		
			glRotatef( m_fRotX, 1.0f, 0, 0 );
		if( m_fRotY )
			glRotatef( m_fRotY, 0, 1.0f, 0 );
		if( m_fRotZ )
			glRotatef( m_fRotZ, 0, 0, 1.0f );
		if( m_fScaleX != 1.0f || m_fScaleY != 1.0f || m_fScaleZ != 1.0f )
			glScalef( m_fScaleX, m_fScaleY, m_fScaleZ );
		glTranslatef( -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );		// 좌표축을 돌린 후 움직인다
		if( m_fAlpha < 1.0f )
		{
			DrawCoreAlpha();
		} else 
		{		
			DrawCore();		
		}
		
		glPopMatrix();
	}
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
}
//-------------------------------------------------------------
void XSurfaceOpenGL::DrawLocal( float x, float y, float lx, float ly )
{
	if( GetDrawMode() != xDM_NONE )
	{
		glPushMatrix();
        
		if( GetDrawMode() == xDM_NORMAL )
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else 
            if( GetDrawMode() == xDM_SCREEN )
                glBlendFunc(GL_SRC_ALPHA, GL_ONE );
		glTranslatef( x, y, 0);	
        glTranslatef( m_fAdjustAxisX, m_fAdjustAxisY, 0 );
		if( m_fRotX )		
			glRotatef( m_fRotX, 1.0f, 0, 0 );
		if( m_fRotY )
			glRotatef( m_fRotY, 0, 1.0f, 0 );
		if( m_fRotZ )
			glRotatef( m_fRotZ, 0, 0, 1.0f );
		if( m_fScaleX != 1.0f || m_fScaleY != 1.0f || m_fScaleZ != 1.0f )
			glScalef( m_fScaleX, m_fScaleY, m_fScaleZ );
		glTranslatef( -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );		// 좌표축을 돌린 후 움직인다
		glTranslatef( lx, ly, 0);	
		if( m_fAlpha < 1.0f )
			DrawCoreAlpha();
		else 
			DrawCore();		
		
		glPopMatrix();
	}
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
    m_ColorR = m_ColorG = m_ColorB = 1.0f;
} // draw local

/*void XSurfaceOpenGL::DrawFlip( float x, float y )
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(180, 0, 1, 0);
	
	DrawCore();
	
	glPopMatrix();
}*/

// 매트릭스 변환이 없는 코어버전.
void XSurfaceOpenGL::DrawCore( void )
{
//    glColor4f( m_ColorR, m_ColorG, m_ColorB, 1.0f );
    glColor4f( 1.0f, 0, 0, 1.0f );
	// bind vertex/index buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(STRUCT_VERTEX_SURFACE), (void*)offsetof(STRUCT_VERTEX_SURFACE,x));
	glTexCoordPointer(2, GL_FLOAT, sizeof(STRUCT_VERTEX_SURFACE), (void*)offsetof(STRUCT_VERTEX_SURFACE,t));
	if( m_fAlpha < 1.0f )
	{
		glColorPointer(4, GL_FLOAT, sizeof(STRUCT_VERTEX_SURFACE), (void*)offsetof(STRUCT_VERTEX_SURFACE,c));
		glEnableClientState(GL_COLOR_ARRAY);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID); 
	// bind texture
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	
	if( m_fAlpha < 1.0f )
		glDisableClientState(GL_COLOR_ARRAY);
}

void XSurfaceOpenGL::DrawCoreAlpha( void )
{
    float memw = GetMemWidth();
    float memh = GetMemHeight();
    float surfacew = GetWidth();
    float surfaceh = GetHeight();
    float alignw = GetAlignedWidth();
    float alignh = GetAlignedHeight();
    float u = memw / alignw;
    float v = memh / alignh;
    float adjx = GetAdjustX();
    float adjy = GetAdjustY();
	GLfloat tex[8] = { 0, v, u, v, 0, 0, u, 0 };
	GLfloat pos[8] = { adjx, surfaceh+adjy, surfacew+adjx, surfaceh+adjy, adjx, adjy, surfacew+adjx, adjy };
	GLfloat col[16] = { 1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha };
	
    glColor4f( m_ColorR, m_ColorG, m_ColorB, 1.0f );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glColorPointer(4, GL_FLOAT, 0, col);
	// bind texture
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_COLOR_ARRAY);
}

// this의 RECT:src영역을 x,y위치에 그린다.
void XSurfaceOpenGL::DrawSub( float x, float y, const RECT *src )
{
	int memw, memh;
	GLfloat l, t, r, b;

	if( src )
	{
		RECT memRect;
		memRect.left = ConvertToMemSize( (float)src->left );
		memRect.right = ConvertToMemSize( (float)src->right );
		memRect.top = ConvertToMemSize( (float)src->top );
		memRect.bottom = ConvertToMemSize( (float)src->bottom );

		memw = (memRect.right - memRect.left);
		memh = (memRect.bottom - memRect.top);
		
		l = memRect.left / (GLfloat)GetAlignedWidth();
		t = memRect.top / (GLfloat)GetAlignedHeight();
		r = memRect.right / (GLfloat)GetAlignedWidth();
		b = memRect.bottom / (GLfloat)GetAlignedHeight();
	} else
	{
		// src가 지정되어 있지 않으면 전체 출력.
		Draw(x, y);
		return;
	}
	
	GLfloat tex[8] = { l, b, r, b, l, t, r, t };
	
    float surfacew = (src->right - src->left);
    float surfaceh = (src->bottom - src->top);
	GLfloat pos[8] = { 0, surfaceh, surfacew, surfaceh, 0, 0, surfacew, 0 };
	GLfloat col[16] = { 1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha, 
						1.0f, 1.0f, 1.0f, m_fAlpha };
	
    if( GetDrawMode() == xDM_NORMAL )
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    else 
    if( GetDrawMode() == xDM_SCREEN )
        glBlendFunc(GL_SRC_ALPHA, GL_ONE );
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	// bind texture
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
    //
    m_DrawMode = xDM_NORMAL;
}

void XSurfaceOpenGL::Fill( XCOLOR col )
{
	// 기존의 텍스쳐 삭제
	glDeleteTextures(1, &m_textureID);

	// col로 채워진 텍스쳐를 새로 할당.
	int nSize = (m_nAlignedWidth*m_nAlignedHeight) * sizeof(WORD);
	WORD *buffer = new WORD[nSize];
	memset(buffer, 0, nSize);
	
	XBREAK(1);		// WORD* 지원되게 다시 구현할것
//	Blt(buffer, NULL, sizeof(WORD), xNO_ALPHA, m_nAlignedWidth, m_nAlignedHeight, 0 );
	
	SAFE_DELETE_ARRAY(buffer);
	
}
// 트랜스폼과 관계없이 이미지내 좌표에서 픽셀을 읽어줌
/*DWORD XSurfaceOpenGL::GetPixel( float _x, float _y )
{
	int x, y;
	x = (int)_x;	y = (int)_y;
    int memw, memh;
	DWORD *pImg = GetSrcImg( &memw, &memh );
	XBREAK( pImg == NULL );
	if( x < 0 || x >= memw )	return 0;
	if( y < 0 || y >= memh )	return 0;
	return 	pImg[ y * GetAlignedWidth() + x ];
}*/

void*	XSurfaceOpenGL::Lock( int *pMemW, BOOL bReadOnly)
{
	*pMemW = GetAlignedWidth();
    int w, h;
	return GetSrcImg( &w, &h );
}
