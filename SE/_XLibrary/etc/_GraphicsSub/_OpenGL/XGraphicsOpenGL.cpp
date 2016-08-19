/*
 *  XGraphicsOpenGL.cpp
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "XGraphicsOpenGL.h"
#include "XSurfaceOpenGL.h"
#include "XImage.h"
#include "xMath.h"
XGraphicsOpenGL* XGraphicsOpenGL::s_pGraphicsOpenGL = NULL;

XGraphicsOpenGL::XGraphicsOpenGL( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
	: XGraphics( nResolutionWidth, nResolutionHeight, pixelFormat ) 
{
	Init();
	Create();
}


void XGraphicsOpenGL::Init( void )
{
	XGraphicsOpenGL::s_pGraphicsOpenGL = NULL;
	m_pLockBackBuffer = NULL;
	
//	glViewport( 0, 0, 320, 480 );	// EAGLView에서 프레임버퍼 바인드할때 하도록 바꿔라
	// 여기서 에러나면 그것은 OpenGL ES 2.0으로 초기화했기때문이다. 2.0에는 glMatrixMode라는게 없다.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GetScreenWidth(), GetScreenHeight(), 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glEnable(GL_TEXTURE_2D);
	// Set a blending function to use
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable blending
	glEnable(GL_BLEND);

}

void XGraphicsOpenGL::Destroy( void )
{
	
	Init();
}

xRESULT XGraphicsOpenGL::Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
{
	XGraphics::Init( nResolutionWidth, nResolutionHeight, pixelFormat );
	Init();
	
	Create();
	
	return xSUCCESS;
}

xRESULT XGraphicsOpenGL::Create( void )
{
	XGraphicsOpenGL::s_pGraphicsOpenGL = this;
	return xSUCCESS;
}

void XGraphicsOpenGL::RestoreFrameBuffer( void )
{
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_defaultFrameBuffer);
	glViewport(0, 0, GetPhyScreenWidth(), GetPhyScreenHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, GetScreenWidth(), GetScreenHeight(), 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
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
	fRatioX = (float)GetPhyScreenWidth() / GetScreenWidth();
	fRatioY = (float)GetPhyScreenHeight() / GetScreenHeight();
	px = x * fRatioX;		py = y * fRatioY;	// screen->physcreen 좌표로 변환
	pw = width * fRatioX;	ph = height * fRatioY;
	//GL_NO_ERROR
	DWORD *pdwBuffer = new DWORD[ pw * ph ];	
	glReadPixels(px, py, pw, ph, GL_RGBA, GL_UNSIGNED_BYTE, pdwBuffer );	// 위아래 뒤집혀진 이미지
	DWORD *pDst = new DWORD[ pw * ph ];
    // 위아래를 바꿈
	for( int i = 0; i < ph; i ++ )
		for( int j = 0; j < pw; j ++ )
			pDst[ i * pw + j ] = pdwBuffer[ (ph-1-i) * pw + j ];	
	SAFE_DELETE_ARRAY( pdwBuffer );
	
    
	pSurface->Create( pw, ph, 0, 0, xALPHA, pDst, sizeof(DWORD), 0, 0 );
#pragma messages( "pdwBuffer를 XSurfaceOpenGL::Create()내부에서 뽀개주도록 바꿔야 한다. 버그날까봐 일단 이상태로 놔둠" )
}

void XGraphicsOpenGL::SetDrawTarget( XSurface *pSurface )
{
}
void XGraphicsOpenGL::ReleaseDrawTarget( void )
{
}

XSurface* XGraphicsOpenGL::CreateScreenToSurface( BOOL bHighReso )
{
	XSurface *pSurface = new XSurfaceOpenGL( FALSE );
//	ScreenToSurface( pSurface );
	ScreenToSurface( 0, 0, GetScreenWidth(), GetScreenHeight(), pSurface );
//	ScreenToSurface( bHighReso, 0, 0, GetPhyScreenWidth(), GetPhyScreenHeight(), pSurface );
	return pSurface;
}

XSurface*	XGraphicsOpenGL::CreateSurface( BOOL bHighReso, LPCSTR szFilename, XCOLOR dwColor, BOOL bSrcKeep )
{
	XSurface *pSurface = new XSurfaceOpenGL( bHighReso, szFilename, dwColor, bSrcKeep );
	return pSurface;
}

XSurface*	XGraphicsOpenGL::CreateSurface( BOOL bHighReso, int w, int h, int adjx, int adjy, WORD *pImg, XCOLOR dwColor, BOOL bSrcKeep )
{
	XSurface *pSurface = new XSurfaceOpenGL( bHighReso, w, h, adjx, adjy, pImg, dwColor, bSrcKeep );
	return pSurface;
}

// PNG로 서피스를 만드는 버전
XSurface*	XGraphicsOpenGL::CreateSurfacePNG( BOOL bHighReso, LPCSTR szFilename, BOOL bSrcKeep )
{
	XSurfaceOpenGL *pSurfaceGL = new XSurfaceOpenGL( bHighReso );
	pSurfaceGL->CreatePNG( szFilename, bSrcKeep );
	return pSurfaceGL;
}

BOOL XGraphicsOpenGL::LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage )
{
	XImage<DWORD> *pImage = new XImageiOS<DWORD>( TRUE );
	if( pImage->Load( szFilename ) )
	{
		*pWidth = pImage->GetMemWidth();
		*pHeight = pImage->GetMemHeight();
		DWORD **pp = (DWORD **)ppImage;
		pImage->MovePtr( pp );
		*ppImage = (DWORD *)*pp;
		SAFE_DELETE( pImage );
		return TRUE;
	}
    SAFE_DELETE_ARRAY( pImage ); 
	return FALSE;
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
	glClear(GL_COLOR_BUFFER_BIT);
}
int	 XGraphicsOpenGL::GetPixel( int x, int y )
{
	return 0;
}
void XGraphicsOpenGL::FillRect( float x, float y, float w, float h, XCOLOR color )
{
	if( w == 0 || h == 0 )	return;
    if( w < 0 )
    {
        w = -w;     // w는 부호 바꿈
        x -= w;     // 좌측 좌표를 -w만큼 이동시켜주고
    }
    if( h < 0 )
    {
        h = -h;
        y -= h;
    }
	if( x > GetScreenWidth() || y > GetScreenHeight() )	// w, h가 마이너스가 올수도 있기땜에 이렇게 함
		return;
	if( x + w < 0 || y + h < 0 )
		return;
	
//	if( x > GetScreenWidth() || y > GetScreenHeight() )
//		return;
//	if( w < 0 || h < 0 )
//		return;
	
		
	GLfloat r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.0f;
	a = XCOLOR_RGB_A(color) / 255.0f;
//	if( a != 255 )	glEnable(GL_BLEND);		// 이거 자주불러주면 부하걸릴거 같다. 외부에서 블럭단위로 셋하게 하자.
	
	// width-1이 맞나? 안하는게 맞나?
	GLfloat pos[8] = { 0, h, w, h, 0, 0, w, 0 };
	GLfloat col[16] = {  r,g,b,a,
						 r,g,b,a,
						 r,g,b,a,
						 r,g,b,a	};
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glScalef(1.0f, 1.0f, 1.0f);
//    glLoadIdentity();
	
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();
//	if( a != 255 )	glDisable(GL_BLEND);
	
}

#define _R(C)	(XCOLOR_RGB_R(C) / 255.0f)
#define _G(C)	(XCOLOR_RGB_G(C) / 255.0f)
#define _B(C)	(XCOLOR_RGB_B(C) / 255.0f)
#define _A(C)	(XCOLOR_RGB_A(C) / 255.0f)
void XGraphicsOpenGL::FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb  )
{
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
	if( x > GetScreenWidth() || y > GetScreenHeight() )	// w, h가 마이너스가 올수도 있기땜에 이렇게 함
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
	GLfloat pos[8] = { 0, h, w, h, 0, 0, w, 0 };
	GLfloat col[16] = {  _R(collb), _G(collb),_B(collb),_A(collb),	// 좌하
						_R(colrb), _G(colrb),_B(colrb),_A(colrb),	// 우하
						_R(collt), _G(collt),_B(collt),_A(collt),	// 좌상
						_R(colrt), _G(colrt),_B(colrt),_A(colrt)	};	// 우상
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glScalef(1.0f, 1.0f, 1.0f);
	//    glLoadIdentity();
	
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();
	//	if( a != 255 )	glDisable(GL_BLEND);
	
}

void XGraphicsOpenGL::DrawRect( float x, float y, float w, float h, XCOLOR color )
{
	if( x > GetScreenWidth() || y > GetScreenHeight() )
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
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glScalef(1.0f, 1.0f, 1.0f);
	//    glLoadIdentity();
	
	glLineWidth( GetLineWidth() );
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawArrays(GL_LINE_STRIP, 0, 5);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();
	//	if( a != 255 )	glDisable(GL_BLEND);
	
}

void XGraphicsOpenGL::DrawLine( float x1, float y1, float x2, float y2, XCOLOR color )
{
	if( x1 > GetScreenWidth() || y1 > GetScreenHeight() )
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
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, 0);
	glScalef(1.0f, 1.0f, 1.0f);
	//    glLoadIdentity();

	
	glLineWidth( GetLineWidth() );
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();
//	glLineWidth( 1.0f );
	//	if( a != 255 )	glDisable(GL_BLEND);
	
}

void XGraphicsOpenGL::DrawLineList( XGraphics::xVERTEX *vList, int numLines )
{
    // 클리핑
    // 버텍스버퍼를 생성
    static GLuint s_glVB=0;
    if( s_glVB == 0 )
        glGenBuffers( 1, &s_glVB );
    // 버텍스버퍼에 라인정보 밀어넣음
    XBREAK( s_glVB == 0 );
    glDisable( GL_TEXTURE_2D );
    glBindBuffer( GL_ARRAY_BUFFER, s_glVB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glVertexPointer( 2, GL_FLOAT, sizeof(XGraphics::xVERTEX), 0 );
    glColorPointer( 4, GL_FLOAT, sizeof(XGraphics::xVERTEX), (GLvoid*)(sizeof(GL_FLOAT)*2) );
    glBufferData( GL_ARRAY_BUFFER, sizeof(XGraphics::xVERTEX) * numLines * 2, vList, GL_DYNAMIC_DRAW );

    // draw
	glLineWidth( GetLineWidth() );
    glDrawArrays( GL_LINES, 0, numLines * 2 );
    //
    glDisableClientState( GL_COLOR_ARRAY );
    glEnable( GL_TEXTURE_2D );
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
void XGraphicsOpenGL::DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice )
{
	if( angStart == angEnd )
		return;
	
	GLfloat r, g, b, a;
	r = XCOLOR_RGB_R(color) / 255.0f;
	g = XCOLOR_RGB_G(color) / 255.0f;
	b = XCOLOR_RGB_B(color) / 255.0f;
	a = XCOLOR_RGB_A(color) / 255.0f;
	
	GLfloat pos[MAX_VERTEX * 2];
	GLfloat col[MAX_VERTEX * 4];
	float angSlice = 360.0f / (float)maxSlice;		// 
	float ang = 0;
	int i = 0;
	POSX(i) = x;	POSY(i) = y;
	COLOR(i, r, g, b, a);
	i++;
	POSX(i) = x + (sinf(D2R(angStart)) * radius);		// 시작각도 버텍스 하나 만들어줌	
	POSY(i) = y + (-cosf(D2R(angStart)) * radius);
	COLOR(i, r, g, b, a);
	i++;
	ang += angSlice;
	int num = 0;
	while( ang < angEnd )
	{
		if( ang >= angStart )		// 각도범위에 포함되면 버텍스를 추가
		{
			float rAng = D2R(ang);		// 디그리 각도를 라디안각도로 변환
			POSX(i) = x + (sinf(rAng) * radius);
			POSY(i) = y + (-cosf(rAng) * radius);
			COLOR(i, r, g, b, a)
			i++;
			num++;		// 삼각형 개수
			if( XBREAK(i >= MAX_VERTEX) )		// 버퍼 오버플로우 되지 않도록
				break;
		}
		ang += angSlice;
	}
	// 마지막각도에 버텍스 하나 더 추가
	POSX(i) = x + (sinf(D2R(angEnd)) * radius);
	POSY(i) = y + (-cosf(D2R(angEnd)) * radius);
	COLOR(i,r,g,b,a);
	i++;
	num++;
	
	// gl draw
	glPushMatrix();
	glLoadIdentity();
	
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, i);	// i==버텍스개수
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();

/*	
	
	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = x;	aVertex[0].vPos.y = y;	// 파이의 중심점
	aVertex[0].dwColor = color;
	aVertex[1].vPos.x = x + (sinf(D2R(angStart)) * radius);		// 시작각도 버텍스 하나 만들어줌
	aVertex[1].vPos.y = y + (-cosf(D2R(angStart)) * radius);
	aVertex[1].dwColor = color;
	ang += angSlice;
	aVertex = &m_aVertex[2];
	const FVF_2DVERTEX *pEnd = &m_aVertex[ MAX_2DVERTEX ];
	int num = 0;
	while( ang < angEnd )
	{
		if( ang >= angStart )		// 각도범위에 포함되면 버텍스를 추가
		{
			float rAng = D2R(ang);		// 디그리 각도를 라디안각도로 변환
			aVertex->vPos.x = x + (sinf(rAng) * radius);
			aVertex->vPos.y = y + (-cosf(rAng) * radius);
			aVertex->dwColor = color;
			aVertex++;
			num++;		// 삼각형 개수
			if( aVertex > pEnd )		// 버퍼 오버플로우 되지 않도록
				break;
		}
		ang += angSlice;
	}
	// 마지막각도에 버텍스 하나 더 추가
	aVertex->vPos.x = x + (sinf(D2R(angEnd)) * radius);
	aVertex->vPos.y = y + (-cosf(D2R(angEnd)) * radius);
	aVertex->dwColor = color;
	num++;
	
	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, num, m_aVertex, sizeof(FVF_2DVERTEX) );
 */
}

static XE::VEC2 _vLists[ MAX_VERTEX ];

void XGraphicsOpenGL::DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice )
{
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
	
//	float ratioX = ((float)GetPhyScreenWidth() / GetScreenWidth()) ;
//	float ratioY = ((float)GetPhyScreenHeight() / GetScreenHeight());
	for( int i = 0; i < num+2; i ++ )	// num은 삼각형개수고 +2를 해야 버텍스 개수다
	{
		POS(i, _vLists[i].x, _vLists[i].y);
		COLOR(i,r,g,b,a);
//		m_aVertex[i].vPos.x = _vLists[i].x * ratioX;
//		m_aVertex[i].vPos.y = _vLists[i].y * ratioY;
//		m_aVertex[i].dwColor = color;
	}
	
	// gl draw
	glPushMatrix();
	glLoadIdentity();
	
	glDisable( GL_TEXTURE_2D );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// 이건 안해줘도 되네.
	glVertexPointer(2, GL_FLOAT, 0, pos);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, col);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, num+2);	// num+2: 버텍스개수
	
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable( GL_TEXTURE_2D );
	glPopMatrix();
	}
}


//////////////////////////////
#pragma mark create manager obj
/*
CSpriteManagerGL* XGraphicsOpenGL::CreateSprMngObj( void )
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

void XGraphicsOpenGL::UnlockBackBufferPtr( void )
{
	SAFE_DELETE_ARRAY( m_pLockBackBuffer );
}

#if 0
/*
void	XGraphicsOpenGL::ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface )
{
	if( GetPixelFormat() != xPIXELFORMAT_RGB565 )
		XERROR( "아직은 RGB565포맷만 사용가능함." );
	// 아래코드는 다분히 iPhone의존적이기때문에 XGragphicsiPhone::ScreenToSurface로 만들어야 한다.
	int px, py, pw, ph;		// 물리적스크린크기와 좌표.
	float fRatioX, fRatioY;
	// phyScreen과 screen의 비율로 좌표들을 변환.
	fRatioX = (float)GetPhyScreenWidth() / GetScreenWidth();
	fRatioY = (float)GetPhyScreenHeight() / GetScreenHeight();
	px = x * fRatioX;		py = y * fRatioY;	// screen->physcreen 좌표로 변환
	pw = width * fRatioX;	ph = height * fRatioY;
	//GL_NO_ERROR
	DWORD *pdwBuffer = new DWORD[ pw * ph ];	
	glReadPixels(px, py, pw, ph, GL_RGBA, GL_UNSIGNED_BYTE, pdwBuffer );	// 위아래 뒤집혀진 이미지
	DWORD *pDst = new DWORD[ pw * ph ];
	for( int i = 0; i < ph; i ++ )
	{
		for( int j = 0; j < pw; j ++ )
		{
			pDst[ i * pw + j ] = pdwBuffer[ (ph-1-i) * pw + j ];	// 위아래를 바꿈
		}
	}
	SAFE_DELETE_ARRAY( pdwBuffer );
	
    
	pSurface->Create( pw, ph, 0, 0, xALPHA, pDst, sizeof(DWORD), 0, 0 );
    /*	WORD *pBuffer = new WORD[pw*ph];
     int i, j;
     WORD *_pBuffer = pBuffer;
     DWORD dwData;
     for( i = 0; i < ph; i ++ )
     for( j = 0; j < pw; j ++ )
     {
     dwData = pdwBuffer[i*pw+j];
     BYTE r = XCOLOR_RGB_R(dwData);
     BYTE g = XCOLOR_RGB_G(dwData);
     BYTE b = XCOLOR_RGB_B(dwData);
     *_pBuffer++ = XCOLOR_RGB565(r, g, b);
     }
     SAFE_DELETE_ARRAY( pdwBuffer );
	 // buffer를 텍스쳐로 만든다.
	 XSurfaceOpenGL screenData;
	 screenData.Create(pw, ph, 0, 0, xNO_ALPHA, pBuffer, sizeof(WORD) );
     */
	// buffer를 텍스쳐로 만든다.
    /*	XSurfaceOpenGL screenData;
     screenData.Create(pw, ph, 0, 0, xNO_ALPHA, pdwBuffer, sizeof(DWORD) );
     
     // FBO생성
     GLuint idFrameBuffer;
     glGenFramebuffersOES(1, &idFrameBuffer);
     //	glGenRenderbuffersOES(1, &idRenderBuffer);
     glBindFramebufferOES(GL_FRAMEBUFFER_OES, idFrameBuffer);
     
     // FBO에 연결될 텍스쳐 생성. 
     //	XSurfaceOpenGL *pSurfaceGL = (XSurfaceOpenGL *)pSurface;
     XSurfaceOpenGL *pSurfaceGL = SafeCast<XSurfaceOpenGL *>( pSurface );
     XBREAKF( pSurfaceGL->GetTextureID() != 0, "ScreenToSurface()내부에서 Surface Create하는걸로 방식이 바뀜" );
     
     //	pSurfaceGL->Create( GetPhyScreenWidth(), GetPhyScreenHeight(), 0, 0, xNO_ALPHA, NULL, sizeof(DWORD), 0, 0 );
     pSurfaceGL->Create( GetPhyScreenWidth(), GetPhyScreenHeight() );
     //	pSurfaceGL->Create( 1024, 768 );
     // attach texture to framebuffer color buffer
     glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, pSurfaceGL->GetTextureID(), 0);
     
     // initialize depth renderbuffer
     //
     int alignedw, alignedh;
     alignedw = pSurfaceGL->GetAlignedWidth();
     alignedh = pSurfaceGL->GetAlignedHeight();
     
     GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
     if( status != GL_FRAMEBUFFER_COMPLETE_OES )
     {
     XLOG( "status=%d", status );
     return;
     }
     // FBO 활성
     glBindFramebufferOES(GL_FRAMEBUFFER_OES, idFrameBuffer);
     
     glClearColor(0, 0, 1, 1);
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
     glViewport(0, 0, alignedw, alignedh);
     //	glViewport(0, 0, 1024, 768);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrthof(0, alignedw, 0, alignedh, -1.0f, 1.0f);
     //	glOrthof(0, 1024, 0, 768, -1.0f, 1.0f);
     glMatrixMode(GL_MODELVIEW);
     
     // FBO texture에 렌더.
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     
     glPushMatrix();
     GLfloat fZoomRatioW = GetScreenWidth() / (GLfloat)GetPhyScreenWidth();
     GLfloat fZoomRatioH = GetScreenHeight() / (GLfloat)GetPhyScreenHeight();
     // 반드시 이동,회전,확축 순으로 불러야 한다. 행렬은 반대 M=SxRxT
     glTranslatef(0, GetScreenHeight(), 0);	// 중앙기준이 아니라 맨윗줄 기준으로 돌아간것이므로 화면크기만큼 내려찍는다.
     //	glTranslatef(0, 0, 0);	// 중앙기준이 아니라 맨윗줄 기준으로 돌아간것이므로 화면크기만큼 내려찍는다.
     glRotatef(180, 1, 0, 0);				// x축기준으로(위아래플립) 180도 돌린다.
     //	glScalef(fZoomRatioW, fZoomRatioH, 1);
     glScalef(0.5, 0.5, 1);
     screenData.Draw(0,0);
     //	GRAPHICS->DrawBox( 0, 0, 64, 64, XCOLOR_RGBX(0,255,0) );
     glPopMatrix();
     
     // FBO비활성
     RestoreFrameBuffer();
     
     // delete objects
     glDeleteFramebuffersOES(1, &idFrameBuffer);
     //	SAFE_DELETE_ARRAY( pBuffer );
     SAFE_DELETE_ARRAY( pdwBuffer );
     */
#pragma messages( "pdwBuffer를 XSurfaceOpenGL::Create()내부에서 뽀개주도록 바꿔야 한다. 버그날까봐 일단 이상태로 놔둠" )
} */
#endif // 0
