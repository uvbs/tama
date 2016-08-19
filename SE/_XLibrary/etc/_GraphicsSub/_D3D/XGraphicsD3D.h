#ifndef __XGRAPHICSD3D_H__
#define __XGRAPHICSD3D_H__

/*
 *  XGraphicsD3D.h
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */
#include <windows.h>
#include <d3dx9.h>
#include "XGraphics.h"
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
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPD3DXEFFECT m_pEffect;		// shader
	FVF_2DVERTEX	m_aVertex[ MAX_2DVERTEX ];
	LPD3DXLINE m_pd3dxLine;
	int m_nItorVertex;
	
	void _Init()	{
		XGraphicsD3D::s_pGraphicsD3D = nullptr;
		m_pd3dDevice = nullptr;
		m_pEffect = nullptr;	
		m_pd3dxLine = nullptr;
		m_nItorVertex = 0;
		for( int i = 0; i < MAX_2DVERTEX; i ++ )
		{
			m_aVertex[i].vPos    = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
			m_aVertex[i].dwColor = 0xffffffff;
		}
	}
	void Destroy() {
		SAFE_RELEASE( m_pd3dxLine );
	}
public:
	XGraphicsD3D() { _Init(); }
	XGraphicsD3D( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	virtual ~XGraphicsD3D() { Destroy(); }

	void	Initialize( LPDIRECT3DDEVICE9 d3dDevice );
	LPDIRECT3DDEVICE9 GetDevice() { return m_pd3dDevice; }
	GET_ACCESSOR( LPD3DXEFFECT, pEffect );
	// virtual
	virtual void SetViewport( int left, int top, int right, int bottom ) ;
	virtual void RestoreViewport();
	//
	XE::xRESULT Create();
	XE::xRESULT	Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	virtual XSurface*	CreateSurface( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep = FALSE );	// 이미지의 일부분만 서피스로 만드는 버전
//	virtual XSurface*	CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep = FALSE );
	virtual XSurface* CreateSurface( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE ) {
		XSurface *pSurface = new XSurfaceD3D( wSurface, hSurface, wTexture, hTexture, adjx, adjy, pImg, bSrcKeep );
		return pSurface;
	}
	virtual void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface );
	virtual XSurface* CreateScreenToSurface( BOOL bHighReso );
	BOOL SaveBackBuffer( LPCTSTR szFilename, LPDIRECT3DSWAPCHAIN9 pd3dChain, int w, int h );
	
	// XGraphicsD3D에만 있는것
	BOOL LoadShader( int idr );

	// Draw
	void Clear2DVertex() { m_nItorVertex = 0; }
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

	void	DrawPointList( FVF_2DVERTEX *pList=nullptr, int nNum=0, float nSize=1.0f );
	virtual void DrawPoint( float x, float y, float nSize, XCOLOR dwColor );
	virtual void DrawRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR dwColor );
	virtual void FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR dwColor );
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 );
	virtual void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 );	// x, y중심점, radius반지름, angStart시작각도, angEnd끝각도, maxSlice:360각도를 몇등분으로 쪼갤거냐
//	BOOL ClippingLine( XE::VEC2 *pvOut, const XE::VEC2 *pvLines, int numLine, float x1, float y1, float x2, float y2 );
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 );
	virtual void DrawLineList( XE::VEC2 *pvList, int maxPoint, XCOLOR col );
	virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines );
	virtual void DrawCircle( float x, float y, float radius, DWORD dwColor, int numLine = 0 ) override;
	virtual void DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, const XE::VEC2& v2, const XE::VEC2& v3, int maxSlice, XCOLOR col, xtLine lineType );
	virtual void DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE );
	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage ) { XBREAK(1); return FALSE; }
	void DrawCoolUI( float x, float y, DWORD *pMask, int w, int h, float angStart, float angEnd, DWORD color );
	void RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture );
};

#endif
