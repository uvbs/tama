#pragma once
/*
 *  XGraphicsD3D.h
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#include <windows.h>
#include <d3dx9.h>
#include "etc/XGraphics.h"
#include "XSurfaceD3D.h"

#define D3DFVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct FVF_2DVERTEX
{
    D3DXVECTOR4 vPos;	// The 3D position for the vertex
    DWORD		dwColor;		// The vertex color
};

#define		MAX_2DVERTEX		4096

class XGraphicsD3D;
#define	GRAPHICS_D3D	XGraphicsD3D::s_pGraphicsD3D

class XGraphicsD3D : public XGraphics {
public:
	static XGraphicsD3D *s_pGraphicsD3D;		// 일단 이렇게 하고 나중에 멀티플랫폼용으로 고치자.
	// 아래 지저분한 캡쳐함수가 필요한 이유는 오버랩시에 프론트화면 캡쳐기능이 필요한데. d3d에선 자꾸 실패해서 뽀록을 쓴거임
	static BOOL s_bCaptureBackBuffer;			// Present전에 현재 백버퍼를 카피해둬야 함
	static XSurface* s_pLastBackBuffer;			// 마지막으로 캡쳐된 백버퍼 화면
	static void CaptureBackBuffer( BOOL bHighReso ) {		// 
		if( s_bCaptureBackBuffer )	{	// 캡쳐가 예약이 됐으면 
			SAFE_DELETE( s_pLastBackBuffer );
			s_pLastBackBuffer = GRAPHICS_D3D->CreateScreenToSurface( bHighReso );		// 백버퍼를 캡쳐해서 보관
		}
		s_bCaptureBackBuffer = FALSE;
	}
	static D3DFORMAT sToD3DFormat( XE::xtPixelFormat format ) {
		switch( format ) {
		case XE::xPF_ARGB8888: return D3DFMT_A8R8G8B8;
		case XE::xPF_ARGB4444: return D3DFMT_A4R4G4B4;
		case XE::xPF_ARGB1555: return D3DFMT_A4R4G4B4;
		case XE::xPF_RGB565: return D3DFMT_R5G6B5;
		case XE::xPF_RGB555: return D3DFMT_R5G6B5;	// 이거 제대로 작동 안될수도 있음.
		default: XBREAK(1); break;
		}
		return D3DFMT_UNKNOWN;
	}
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPD3DXEFFECT m_pEffect;		// shader
	FVF_2DVERTEX	m_aVertex[ MAX_2DVERTEX ];
	LPD3DXLINE m_pd3dxLine;
	int m_nItorVertex;
	
	void _Init( void )	{
		XGraphicsD3D::s_pGraphicsD3D = NULL;
		m_pd3dDevice = NULL;
		m_pEffect = NULL;	
		m_pd3dxLine = NULL;
		m_nItorVertex = 0;
		for( int i = 0; i < MAX_2DVERTEX; i ++ )
		{
			m_aVertex[i].vPos    = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
			m_aVertex[i].dwColor = 0xffffffff;
		}
	}
	void Destroy( void ) {
		SAFE_RELEASE( m_pd3dxLine );
	}
public:
	XGraphicsD3D() { _Init(); }
	XGraphicsD3D( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	virtual ~XGraphicsD3D() { Destroy(); }

	void	Initialize( LPDIRECT3DDEVICE9 d3dDevice );
	LPDIRECT3DDEVICE9 GetDevice( void ) { return m_pd3dDevice; }
	GET_ACCESSOR( LPD3DXEFFECT, pEffect );
	// virtual
	void SetViewport( int left, int top, int right, int bottom ) override;
	void RestoreDevice() override;
	//
	xRESULT Create( void );
	xRESULT	Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	XSurface* CreateSurface( BOOL bHighReso ) override;
	// 이미지의 일부분만 서피스로 만드는 버전
	XSurface*	CreateSurface( BOOL bHighReso
												, int srcx, int srcy
												, int srcw, int srch
												, float dstw, float dsth
												, float adjx, float adjy
												, DWORD *pSrcImg
												, BOOL bSrcKeep = FALSE ) override;	
//	virtual XSurface*	CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep = FALSE );
	XSurface* CreateSurface( int wSurface, int hSurface
												, int wTexture, int hTexture
												, int adjx, int adjy
												, DWORD *pImg
												, BOOL bSrcKeep=FALSE ) override;
//	XSurface* CreateSurface( BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep/*=FALSE*/, BOOL bMakeMask/*=FALSE*/ ) override;
// 	XSurface* CreateSurface( LPCTSTR szRes
// 													, XE::xtPixelFormat formatSurface
// 													, bool bSrcKeep = false, bool bMakeMask=false ) override;
	// pure virtual
	XSurface* _CreateSurface( const XE::POINT& sizeSurfaceOrig
													, const XE::VEC2& vAdj
													, XE::xtPixelFormat formatSurface								
													, void* const pImgSrc
													, XE::xtPixelFormat formatImgSrc
													, const XE::POINT& sizeMemSrc
													, bool bSrcKeep, bool bMakeMask ) override;
	void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) override;
	XSurface* CreateScreenToSurface( BOOL bHighReso ) override;
	BOOL SaveBackBuffer( LPCTSTR szFilename, LPDIRECT3DSWAPCHAIN9 pd3dChain, int w, int h );
	
	// XGraphicsD3D에만 있는것
	BOOL LoadShader( int idr );

	// Draw
	void Clear2DVertex( void ) { m_nItorVertex = 0; }
	BOOL Add2DVertex( float x, float y, XCOLOR color ) {
		int i = m_nItorVertex++;
		if( XBREAK(i >= MAX_2DVERTEX) )
			return FALSE;
		m_aVertex[i].vPos.x = x;
		m_aVertex[i].vPos.y = y;
		m_aVertex[i].vPos.z = 0;
		m_aVertex[i].vPos.w = 1.0f;
		m_aVertex[i].dwColor = color;
		return TRUE;
	}
	virtual void	ClearScreen( XCOLOR color ) ;

	void	DrawPointList( FVF_2DVERTEX *pList=NULL, int nNum=0, float nSize=1.0f );
	virtual void DrawPoint( float x, float y, float nSize, XCOLOR dwColor );
	virtual void DrawRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR dwColor );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 );
	// x, y중심점, radius반지름, angStart시작각도, angEnd끝각도, maxSlice:360각도를 몇등분으로 쪼갤거냐
	void DrawPie2( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice = 36 );
// 	void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) override;	
//	BOOL ClippingLine( XE::VEC2 *pvOut, const XE::VEC2 *pvLines, int numLine, float x1, float y1, float x2, float y2 );
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 );
	virtual void DrawLineList( XE::VEC2 *pvList, int maxPoint, XCOLOR col, float thickness=1.0f );
	virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines );
//	virtual void DrawCircle( float x, float y, float radius, DWORD dwColor, int steps = 0, float dOffset = 0.f );
//	virtual void DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, const XE::VEC2& v2, const XE::VEC2& v3, int maxSlice, XCOLOR col, xtLine lineType );
//	virtual void DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE );
// 	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage ) { XBREAK(1); return NULL; }
	BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage ) override;
	void DrawCoolUI( float x, float y, DWORD *pMask, int w, int h, float angStart, float angEnd, DWORD color );
	void RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture );
	void DrawFan( float *pAryPos, float *pAryCol, int numVertex, int numFan ) override;
};
#endif // dx

