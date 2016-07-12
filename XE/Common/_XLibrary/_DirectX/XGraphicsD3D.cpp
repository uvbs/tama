/*
 *  XGraphicsD3D.cpp
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#include "XGraphicsD3D.h"
#include "XSurfaceD3D.h"
#include "XImage.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//#include "_Wnd2/XWnd.h"
#pragma message("--------------------------build DirectX")
void XE::SetProjection( float wLogicalScr, float hLogicalScr )
{
    D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0, wLogicalScr, hLogicalScr, 0, 0, 1.0f );
//	HRESULT hr = GRAPHICS_D3D->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
	SetMatrixViewProj( &matProj );
/*
	if( XWnd::s_bDebugMode )
		CONSOLE("SetProjection:%f,%f, _41=%f, _42=%f", 
									wLogicalScr, hLogicalScr,
									matProj._41, matProj._42 );
*/
}

// static
XGraphicsD3D* XGraphicsD3D::s_pGraphicsD3D = NULL;
BOOL		XGraphicsD3D::s_bCaptureBackBuffer = FALSE;			// Present전에 현재 백버퍼를 카피해둬야 함
XSurface* XGraphicsD3D::s_pLastBackBuffer = NULL;			// 마지막으로 캡쳐된 백버퍼 화면

XGraphicsD3D::XGraphicsD3D( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
	: XGraphics( nResolutionWidth, nResolutionHeight, pixelFormat ) 
{
	XLOG("");
	_Init();
	Create();
}

void	XGraphicsD3D::Initialize( LPDIRECT3DDEVICE9 d3dDevice ) 
{ 
	m_pd3dDevice = d3dDevice; 
	D3DXMATRIXA16 mView;
	D3DXMatrixIdentity( &mView );
	GetDevice()->SetTransform( D3DTS_VIEW, &mView );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// create d3dx line 
	D3DXCreateLine( m_pd3dDevice, &m_pd3dxLine );
}

BOOL XGraphicsD3D::LoadShader( int idr )
{
	LPD3DXBUFFER pCompileError;
//	hr = D3DXCreateEffectFromFile( m_pd3dDevice, L"2dsprite.fx", NULL, NULL, 0, NULL, &g_pEffect, &pCompileError );
	HRESULT hr = D3DXCreateEffectFromResource( m_pd3dDevice, NULL, MAKEINTRESOURCE(idr), NULL, NULL, 0, NULL, &m_pEffect, &pCompileError );
    if (FAILED(hr))
    {
		if( pCompileError )
		{
			LPCTSTR szErr = (LPCTSTR)pCompileError->GetBufferPointer();
			XERROR( "Failed to load effect file\r\n%s", szErr );
		} else
		{
			XERROR( "Failed to load effect file\r\n" );
		}
		return FALSE;
    } 
	return TRUE;
}
xRESULT XGraphicsD3D::Create( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
{
	XGraphics::Init( nResolutionWidth, nResolutionHeight, pixelFormat );
	_Init();
	
	Create();
	
	return xSUCCESS;
}

xRESULT XGraphicsD3D::Create( void )
{
	XGraphicsD3D::s_pGraphicsD3D = this;
	return xSUCCESS;
}
XSurface* XGraphicsD3D::CreateSurface( BOOL bHighReso )
{
	return new XSurfaceD3D( bHighReso );
}
// 이미지의 일부분만 서피스로 만드는 버전
XSurface*	XGraphicsD3D::CreateSurface( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep )
{
	auto pSurface = new XSurfaceD3D( bHighReso, srcx, srcy, srcw, srch, dstw, dsth, adjx, adjy, pSrcImg, bSrcKeep );
	return pSurface;
}
XSurface* XGraphicsD3D::CreateSurface( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep ) 
{
	auto pSurface = new XSurfaceD3D( wSurface, hSurface, wTexture, hTexture, adjx, adjy, pImg, bSrcKeep );
	return pSurface;
}

/**
 @brief 플랫폼별 서피스생성. pure virtual
*/
XSurface* XGraphicsD3D::_CreateSurface( const XE::POINT& sizeSurfaceOrig
																		, const XE::VEC2& vAdj
																		, XE::xtPixelFormat formatSurface								
																		, void* const pImgSrc
																		, XE::xtPixelFormat formatImgSrc
																		, const XE::POINT& sizeMemSrc
																		, bool bSrcKeep, bool bMakeMask ) 
{
	XSurface* pSurface = new XSurfaceD3D( );
	bool bOk = pSurface->Create( sizeSurfaceOrig
									, vAdj
									, formatSurface
									, pImgSrc
									, formatImgSrc
									, sizeMemSrc
									, bSrcKeep
									, bMakeMask );
	if( !bOk ) {
		SAFE_DELETE( pSurface );
	}
	return pSurface;
}


// XSurface* XGraphicsD3D::z BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep/*=FALSE*/, BOOL bMakeMask/*=FALSE*/ )
// {
// 	int w, h;
// 	DWORD *pImg;
// // 	if( LoadImg( szRes, &w, &h, &pImg ) == FALSE )
// // 		return NULL;
// 	XImage imgObj( TRUE, szRes );
// 	if( imgObj.GetTextureData() == nullptr )
// 		return nullptr;
// 	w = imgObj.GetMemWidth();
// 	h = imgObj.GetMemHeight();
// 	imgObj.MovePtr( &pImg );
// 
// 	if( bHighReso ) {
// 		if( w & 1 )
// 			XERROR( "경고: %s의 가로크기가 2로 나누어 떨어지지 않습니다", szRes );
// 		w /= 2;
// 		h /= 2;		// 이 파일이 고해상도로 지정되어 있으면 실제 서피스 크기는 절반이 된다
// 	}
// 	auto pSurface = new XSurfaceD3D( bHighReso, (float)w, (float)h, 0, 0, pImg, bSrcKeep, bMakeMask );
// 	pSurface->SetstrRes( szRes );
// 	return pSurface;
// 
// }
/**
 @brief pixelFormat버전
*/
// XSurface* XGraphicsD3D::CreateSurface( LPCTSTR szRes
// 																		, XE::xtPixelFormat formatSurface
// 																		, bool bSrcKeep/* = false*/, bool bMakeMask/* = false*/ )
// {
// 	auto pSurface = new XSurfaceD3D();
// 	pSurface->Create( )
// 	auto pSurface = new XSurfaceD3D( bHighReso, (float)w, (float)h, 0, 0, pImg, bSrcKeep, bMakeMask );
// 	pSurface->SetstrRes( szRes );
// 	return pSurface;
// }

BOOL XGraphicsD3D::LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage )		
{

	return TRUE;
}
/*#ifdef _CIV
#include "LE2View.h"
#endif
#ifdef _LE
#include "LE2View.h"
#endif */
// 스크린픽셀을 서피스로 저장
XSurface* XGraphicsD3D::CreateScreenToSurface( BOOL bHighReso )
{
	XSurface *pSurface = new XSurfaceD3D( bHighReso );
	ScreenToSurface( bHighReso, pSurface );
	return pSurface;
}

BOOL XGraphicsD3D::SaveBackBuffer( LPCTSTR szFilename, LPDIRECT3DSWAPCHAIN9 pd3dChain, int w, int h )
{
	IDirect3DSurface9* pBackBuff = NULL;
	IDirect3DSurface9* pd3dSurface;
	HRESULT hr;
	hr = m_pd3dDevice->CreateOffscreenPlainSurface( w, h, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pd3dSurface, NULL );       //백버퍼의 얻어옴  
	XBREAK( hr != S_OK );
	hr = pd3dChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuff );
	XBREAK( hr != S_OK );
	hr = m_pd3dDevice->GetRenderTargetData( pBackBuff, pd3dSurface );
	XBREAK( hr != S_OK );
	hr = D3DXSaveSurfaceToFile( szFilename, D3DXIFF_PNG, pd3dSurface, NULL, NULL );
	XBREAKF( hr != S_OK, "%s 저장실패", szFilename );
	SAFE_RELEASE( pBackBuff );
	SAFE_RELEASE( pd3dSurface );
	return hr == S_OK;
}

// gl버전은 프론트버퍼를 캡쳐잡지만 D3D는 프론트버퍼를 캡쳐하면 자꾸 실패해서 백버퍼 캡쳐로 바꿈
void XGraphicsD3D::ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) 
{
#ifndef _SE
	//임시로 사용할 Surface  
	IDirect3DSurface9* BackBuff = NULL;
	IDirect3DSurface9* pd3dSurface;
	HRESULT hr;
	//시스템 메모리에 정해진 크기의 버퍼를 만듬.  
//		D3DDISPLAYMODE mode;
//        hr = GetView()->GetChain()->GetDisplayMode( &mode );
//		hr = m_pd3dDevice->CreateOffscreenPlainSurface( mode.Width, mode.Height, mode.Format, D3DPOOL_SYSTEMMEM, &pd3dSurface, NULL );       //백버퍼의 얻어옴  
	hr = m_pd3dDevice->CreateOffscreenPlainSurface( GetPhyScreenWidth(), GetPhyScreenHeight(), D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pd3dSurface, NULL );       //백버퍼의 얻어옴  
	if( hr != S_OK )
		XLOG( "m_pd3dDevice->CreateOffscreenPlainSurface failed" );
//	if( GetView()->GetChain() )
	{
		hr = m_pd3dDevice->GetRenderTarget( 0, &BackBuff );
		hr = m_pd3dDevice->GetRenderTargetData( BackBuff, pd3dSurface );
//		hr = GetView()->GetChain()->GetFrontBufferData( pd3dSurface );	// ㅅㅂ이거 왜 안되는거야
		switch( hr )
		{
		case D3DERR_DEVICELOST:	XLOG( "D3DERR_DEVICELOST" );		break;
		case D3DERR_INVALIDCALL:	XLOG( "D3DERR_INVALIDCALL" );		break;
		}

////		hr = GetView()->GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &BackBuff );
		if( BackBuff )
		{
			SAFE_RELEASE( BackBuff );
//			D3DXSaveSurfaceToFile( _T("test.bmp"), D3DXIFF_BMP, pd3dSurface, NULL, NULL );
			D3DLOCKED_RECT d3dLockRect;
			memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
			hr = pd3dSurface->LockRect( &d3dLockRect, NULL, D3DLOCK_READONLY  );
			switch( hr )
			{
			case D3DERR_WASSTILLDRAWING:	XLOG( "D3DERR_WASSTILLDRAWING" );		break;
			case D3DERR_INVALIDCALL:			XLOG( "D3DERR_INVALIDCALL" );				break;
			}
			if( hr != S_OK )
			{
				SAFE_RELEASE( pd3dSurface );
				return;
			}
			DWORD *pSrc = (DWORD *)d3dLockRect.pBits;
			{
				int w = GetPhyScreenWidth();
				int h = GetPhyScreenHeight();
				XBREAK( (d3dLockRect.Pitch / sizeof(DWORD)) != w );
				DWORD *p = new DWORD[ w * h ];
				DWORD *_p = p;
				memcpy_s( p, w * h * sizeof(DWORD), pSrc, w * h * sizeof(DWORD) );
				int size = w*h;
				DWORD col = XCOLOR_RGBA( 0, 0, 0, 255 );
				for( int i = 0; i < size; i ++ )		// 알파값을 255로 채워줌
					*_p++ = *_p | col;
//				BOOL bHighReso = TRUE;
				if( bHighReso )
				{
					w /= 2;	// 고해상도 리소스로 만들어야 하면 서피스 크기를 반으로 줄여준다
					h /= 2;
				}
				XBREAK( pSurface->GetbHighReso() != bHighReso );
				pSurface->Create( (float)w, (float)h
												, 0, 0
												, xNO_ALPHA
												, p
												, sizeof(DWORD)
												, 0
												, FALSE, FALSE );
			}
			pd3dSurface->UnlockRect();
		} // pBackBuff
	}
	SAFE_RELEASE( pd3dSurface );
#endif
}

void XGraphicsD3D::SetViewport( int left, int top, int right, int bottom ) 
{ 	
	XGraphics::SetViewport( left, top, right, bottom );
	//
	D3DVIEWPORT9 vp;
	float ratioX = GetRatioWidth();
	float ratioY = GetRatioHeight();
	float l = left * ratioX;
	float t = top * ratioY;
	float w = (right - left + 1) * ratioX;
	float h = (bottom - top + 1) * ratioY;
	if( l < 0 )
		l = 0;
	if( t < 0 )
		t = 0;
	vp.X = (DWORD)l;
	vp.Y = (DWORD)t;
	vp.Width = (DWORD)w;
	vp.Height = (DWORD)h;

#ifdef _XDYNA_RESO
	vp.X += (DWORD) (GetvScreenLT().x * ratioX);
	vp.Y += (DWORD) (GetvScreenLT().y * ratioY);
#endif
	/*
	*/
	m_pd3dDevice->SetViewport( &vp );
/*
	if( XWnd::s_bDebugMode )
		CONSOLE("SetViewport:%d,%d - %d,%d", vp.X, vp.Y, vp.Width, vp.Height );
*/

}

void XGraphicsD3D::RestoreDevice()
{
	SetViewport( m_ptViewportStackLT.x, m_ptViewportStackLT.y
						, m_ptViewportStackRB.x, m_ptViewportStackRB.y );
}

// virtual draw function
void XGraphicsD3D::ClearScreen( XCOLOR color )
{
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
}

void	XGraphicsD3D::DrawRect( float x, float y, float w, float h, DWORD dwColor )
{
/*	D3DVIEWPORT9 vp;
	m_pd3dDevice->GetViewport( &vp );
	x -= vp.X;
	y -= vp.Y; */
//	x -= GetViewportLeft();
//	y -= GetViewportTop();
#ifdef _XDYNA_RESO
//	x += GetvScreenLT().x;
//	y += GetvScreenLT().y;
#endif
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x *= ratioX;	y *= ratioY;
	w *= ratioX;	h *= ratioY;
	float x2 = x + w;
	float y2 = y + h;
	m_pd3dxLine->SetWidth( GetLineWidth() );
	m_pd3dxLine->Begin();
	D3DXVECTOR2 vList[] = { D3DXVECTOR2( x, y ), 
									D3DXVECTOR2( x2, y ), 
									D3DXVECTOR2( x2, y2 ),
									D3DXVECTOR2( x, y2 ),
									D3DXVECTOR2( x, y ) };
	m_pd3dxLine->Draw( vList, 5, dwColor );
	m_pd3dxLine->End(); 
/*
	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = (float)x;	aVertex[0].vPos.y = (float)y;
	aVertex[0].dwColor = dwColor;

	aVertex[1].vPos.x = (float)(x + w);	aVertex[1].vPos.y = (float)y;
	aVertex[1].dwColor = dwColor;

	aVertex[2].vPos.x = (float)(x + w);	aVertex[2].vPos.y = (float)(y + h);
	aVertex[2].dwColor = dwColor;

	aVertex[3].vPos.x = (float)x;	aVertex[3].vPos.y = (float)(y + h);
	aVertex[3].dwColor = dwColor;

	aVertex[4].vPos.x = (float)x;	aVertex[4].vPos.y = (float)y;
	aVertex[4].dwColor = dwColor;

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, aVertex, sizeof(FVF_2DVERTEX) );
	*/
}
void XGraphicsD3D::FillRect( float x, float y, float w, float h, DWORD dwColor )
{
	x += GetViewportLeft();
	y += GetViewportTop();
#ifdef _XDYNA_RESO
	x += GetvScreenLT().x;
	y += GetvScreenLT().y;
#endif
	if( w == 0 )		return;
	if( h == 0 )		return;
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x *= ratioX;	y *= ratioY;
	w *= ratioX;	h *= ratioY;
	FVF_2DVERTEX	*aVertex = m_aVertex;
//	x = (int)x - 0.5f;
//	y = (int)y - 0.5f;
	w = (float)((int)w);
	h = (float)((int)h);
	float x2 = (float)(x + w);//	float x2 = (float)(x + w - 1);
	float y2 = (float)(y + h);
	aVertex[0].vPos.x = (float)x;	aVertex[0].vPos.y = (float)y2;		// 좌하
	aVertex[0].dwColor = dwColor;
	aVertex[1].vPos.x = (float)x2;	aVertex[1].vPos.y = (float)y2;		// 우하
	aVertex[1].dwColor = dwColor;
	aVertex[2].vPos.x = (float)x;	aVertex[2].vPos.y = (float)y;		// 좌상
	aVertex[2].dwColor = dwColor;
	aVertex[3].vPos.x = (float)x2;	aVertex[3].vPos.y = (float)y;		// 우상
	aVertex[3].dwColor = dwColor;

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
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, aVertex, sizeof(FVF_2DVERTEX) );
}

void XGraphicsD3D::FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb )
{
	x += GetViewportLeft();
	y += GetViewportTop();
#ifdef _XDYNA_RESO
	x += GetvScreenLT().x;
	y += GetvScreenLT().y;
#endif
	if( w == 0 )		return;
	if( h == 0 )		return;
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x *= ratioX;	y *= ratioY;
	w *= ratioX;	h *= ratioY;
	FVF_2DVERTEX	*aVertex = m_aVertex;
	x = (int)x - 0.5f;
	y = (int)y - 0.5f;
	w = (float)((int)w);
	h = (float)((int)h);
	float x2 = (float)(x + w);//	float x2 = (float)(x + w - 1);
	float y2 = (float)(y + h);
	aVertex[0].vPos.x = (float)x;	aVertex[0].vPos.y = (float)y2;		// 좌하
	aVertex[0].dwColor = collb;
	aVertex[1].vPos.x = (float)x2;	aVertex[1].vPos.y = (float)y2;		// 우하
	aVertex[1].dwColor = colrb;
	aVertex[2].vPos.x = (float)x;	aVertex[2].vPos.y = (float)y;		// 좌상
	aVertex[2].dwColor = collt;
	aVertex[3].vPos.x = (float)x2;	aVertex[3].vPos.y = (float)y;		// 우상
	aVertex[3].dwColor = colrt;

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
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, aVertex, sizeof(FVF_2DVERTEX) );
}


void XGraphicsD3D::DrawPie2( float x, float y
													, float radius
													, float angStart, float angEnd
													, DWORD color, int maxSlice )
{
	if( angStart == angEnd )
		return;
	x += GetViewportLeft();
	y += GetViewportTop();
#ifdef _XDYNA_RESO
	x += GetvScreenLT().x;
	y += GetvScreenLT().y;
#endif
	float ratioX = ( GetRatioWidth() );
	float ratioY = ( GetRatioHeight() );
	x *= ratioX;	y *= ratioY;
	radius *= ratioX;
//	float ratioX = (GetRatioWidth()) ;
//	float ratioY = (GetRatioHeight());
//	x *= ratioX;	y *= ratioY;
//	radius *= ratioX;
	FVF_2DVERTEX	*aVertex = m_aVertex;
//	float maxSlice = 36.0f;		// 360도 전체 파이를 xx개로 분할( 경계원이 얼마나 부드러운지를 정함)
	float angSlice = 360.0f / (float)maxSlice;		// 
	float ang = 0;
	aVertex[0].vPos.x = x;	aVertex[0].vPos.y = y;	// 파이의 중심점
	aVertex[0].dwColor = color;
	aVertex[1].vPos.x = x + (sinf(D2R(angStart)) * radius);		// 시작각도 버텍스 하나 만들어줌
	aVertex[1].vPos.y = y + (-cosf(D2R(angStart)) * radius);
	aVertex[1].dwColor = color;
	aVertex = &m_aVertex[2];
	const FVF_2DVERTEX *pEnd = &m_aVertex[ MAX_2DVERTEX ];
	int num = 0;
	if( angEnd > 0 )
	{
		ang += angSlice;
		while( ang < angEnd )
		{
			if( ang >= angStart )		// 각도범위에 포함되면 버텍스를 추가
			{
				float rAng = D2R( ang );		// 디그리 각도를 라디안각도로 변환
				aVertex->vPos.x = x + ( sinf( rAng ) * radius );
				aVertex->vPos.y = y + ( -cosf( rAng ) * radius );
				aVertex->dwColor = color;
				aVertex++;
				num++;		// 삼각형 개수
				if( aVertex > pEnd )		// 버퍼 오버플로우 되지 않도록
					break;
			}
			ang += angSlice;
		}
	} else
	{
		ang -= angSlice;
		while( ang > angEnd )
		{
			if( ang <= angStart )		// 각도범위에 포함되면 버텍스를 추가
			{
				float rAng = D2R( ang );		// 디그리 각도를 라디안각도로 변환
				aVertex->vPos.x = x + ( sinf( rAng ) * radius );
				aVertex->vPos.y = y + ( -cosf( rAng ) * radius );
				aVertex->dwColor = color;
				aVertex++;
				num++;		// 삼각형 개수
				if( aVertex > pEnd )		// 버퍼 오버플로우 되지 않도록
					break;
			}
			ang -= angSlice;
		}
	}
	// 마지막각도에 버텍스 하나 더 추가
	aVertex->vPos.x = x + (sinf(D2R(angEnd)) * radius);
	aVertex->vPos.y = y + (-cosf(D2R(angEnd)) * radius);
	aVertex->dwColor = color;
	num++;
}
/**
 @brief 
 @param pAryPos (float): x,y
 @param pAryCol (float): r, g, b, a
*/
void XGraphicsD3D::DrawFan( float *pAryPos, float *pAryCol, int numVertex, int numFan )
{
	// clear
	for( int i = 0; i < MAX_2DVERTEX; i++ ) {
		m_aVertex[ i ].vPos = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
		m_aVertex[ i ].dwColor = 0xffffffff;
	}
	// set vertices
	for( int i = 0; i < numVertex; ++i ) {
		float x = pAryPos[ i * 2 + 0 ];
		float y = pAryPos[ i * 2 + 1 ];
		float r = pAryCol[ i * 4 + 0 ];
		float g = pAryCol[ i * 4 + 1 ];
		float b = pAryCol[ i * 4 + 2 ];
		float a = pAryCol[ i * 4 + 3 ];
		m_aVertex[ i ].vPos = D3DXVECTOR4( x, y, 0, 1.0f );
		m_aVertex[ i ].dwColor = XCOLOR_RGBA_FLOAT( r, g, b, a );
	}
		
	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, numFan, m_aVertex, sizeof( FVF_2DVERTEX ) );
}

static XE::VEC2 _vLists[ MAX_2DVERTEX ];

void XGraphicsD3D::DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, DWORD color, int maxSlice )
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
	if( XE::ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
		*pvList = vOut;
	else
		*pvList = vo;
	pvList++;
	ang += angSlice;
	const XE::VEC2 *pEnd = &_vLists[ MAX_2DVERTEX ];
	int num = 0;
	while( ang < angEnd )
	{
		if( ang >= angStart )		// 각도범위에 포함되면 버텍스를 추가
		{
			float rAng = D2R(ang);		// 디그리 각도를 라디안각도로 변환
			vo.x = x + (sinf(rAng) * radius);
			vo.y = y + (-cosf(rAng) * radius);
			if( XE::ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
				*pvList = vOut;
			else
				*pvList = vo;
			pvList++;
			num++;		// 삼각형 개수
			if( pvList > pEnd )		// 버퍼 오버플로우 되지 않도록
				break;
		}
		ang += angSlice;
	}
	// 마지막각도에 버텍스 하나 더 추가
	vo.x = x + (sinf(D2R(angEnd)) * radius);
	vo.y = y + (-cosf(D2R(angEnd)) * radius);
	if( XE::ClippingLine( &vOut, pvLines, numLine, x, y, vo.x, vo.y ) )
		*pvList = vOut;
	else
		*pvList = vo;
	num++;

	// d3d버텍스버퍼에 옮김
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	for( int i = 0; i < num+2; i ++ )	// num은 삼각형개수고 +2를 해야 버텍스 개수다
	{
		m_aVertex[i].vPos.x = _vLists[i].x * ratioX;
		m_aVertex[i].vPos.y = _vLists[i].y * ratioY;
		m_aVertex[i].dwColor = color;
	}

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
//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );	// 디버깅용. 와이어프레임으로 그리기
//	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, num, m_aVertex, sizeof(FVF_2DVERTEX) );
//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

static LPDIRECT3DTEXTURE9 s_pTexture = NULL;
static LPDIRECT3DTEXTURE9 pRenderTexture = NULL;

// 테스트용으로 만듬
void XGraphicsD3D::DrawCoolUI( float x, float y, DWORD *pMask, int w, int h, float angStart, float angEnd, DWORD color )
{
/*	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTexture;
	int aw = 64.0f;
	int ah = 64.0f;
	// 렌더타겟 생성
	D3DXCreateTexture( m_pd3dDevice, aw, ah, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture );
	LPDIRECT3DSURFACE9 pSurface;
	pTexture->GetSurfaceLevel( 0, &pSurface );
	// backup oldRenderTarget & viewport
	LPDIRECT3DSURFACE9 pOldRenderTargetSurface;
	m_pd3dDevice->GetRenderTarget( 0, &pOldRenderTargetSurface );
	D3DVIEWPORT9 oldViewport;
	m_pd3dDevice->GetViewport( &oldViewport );
	// set render target
	hr = m_pd3dDevice->SetRenderTarget(0, pSurface );
	// set viewport
	D3DVIEWPORT9 viewport;
	viewport.X = 0;	viewport.Y = 0;
	viewport.Width = aw;	viewport.Height = ah;
	viewport.MinZ = 0;		viewport.MaxZ = 1.0f;
	hr = m_pd3dDevice->SetViewport( &viewport );
	// set view/projection
	D3DXMATRIXA16 mView;
	D3DXMatrixIdentity( &mView );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &mView );
    D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0, (float)aw, (float)ah, 0, 0, 1.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	// begin scene
	hr = m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(255, 255, 255, 0), 1.0f, 0 );
	if( SUCCEEDED(m_pd3dDevice->BeginScene()) )
	{
		DrawPie( w/2.0f, h/2.0f, 30.0f, angStart, angEnd, color );
		m_pd3dDevice->EndScene();
	}
	// restore
	m_pd3dDevice->SetRenderTarget( 0, pOldRenderTargetSurface );
	m_pd3dDevice->SetViewport( &oldViewport );
	// lock surface
	LPDIRECT3DSURFACE9 pLockSurface;
	hr = m_pd3dDevice->CreateOffscreenPlainSurface( aw, ah, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pLockSurface, NULL );
	hr = m_pd3dDevice->GetRenderTargetData( pSurface, pLockSurface );

	DWORD *dst1, *dst2;
	D3DLOCKED_RECT	d3dLockRect;
	memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
	if( FAILED( pLockSurface->LockRect( &d3dLockRect, NULL, 0 ) ) ) {
		XBREAKF( 1, "텍스쳐 Lock실패" );
	}
	dst1 = (DWORD *)d3dLockRect.pBits;

	{
		if( s_pTexture == NULL )
			D3DXCreateTexture( m_pd3dDevice, aw, ah, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &s_pTexture );
		D3DLOCKED_RECT	d3dLockRect;
		memset( &d3dLockRect, 0, sizeof( D3DLOCKED_RECT ) );
		if( FAILED( s_pTexture->LockRect( 0, &d3dLockRect, NULL, 0 ) ) ) {
			XBREAKF( 1, "텍스쳐 Lock실패" );
		}
		dst2 = (DWORD *)d3dLockRect.pBits;
	}

	DWORD *src = pMask;
	for( int i = 0; i < h; i ++ )
	{
		for( int j = 0; j < w; j ++ )
		{
			if( XCOLOR_RGB_A(*pMask++) == 0 )		// 마스크이미지에 알파가 0인부분만
				*dst2 = *dst1 & ~0xff000000;			// 렌더타겟의 픽셀값의 알파값만 지워서 다이나미텍스쳐에 옮김
			else
				*dst2 = *dst1;			// 마스크가 투명부분이 아니면 그냥 복사
//			*dst2 = *dst1;
			dst1++;
			dst2++;
		}
		dst1 += (aw - w);
		dst2 += (aw - w);
	}
	pLockSurface->UnlockRect();
	s_pTexture->UnlockRect(0);


	SAFE_RELEASE( pLockSurface );
	SAFE_RELEASE( pOldRenderTargetSurface );
	SAFE_RELEASE( pSurface );
//	SAFE_RELEASE( pTexture );
*/
}
// 테스트용으로 만듬
void XGraphicsD3D::RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture )
{
	LPDIRECT3DDEVICE9       pd3dDevice = m_pd3dDevice;
//	pd3dTexture = s_pTexture;
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );		
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	float vertex2D[4][4+2] = 
	{
		{   x,   y,0,1, 0,0 },
		{ x+w, y,0,1, 1,0 },
		{ x+w, y+h,0,1, 1,1 },
		{    x, y+h,0,1, 0,1 },
	};
	pd3dDevice->SetTexture( 0, pd3dTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertex2D, sizeof(float)*(4+2) );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

}

void XGraphicsD3D::DrawLine( float x1, float y1, float x2, float y2, DWORD dwColor )
{
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x1 *= ratioX;	y1 *= ratioY;
	x2 *= ratioX;	y2 *= ratioY;
/*	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = ((int)x1) - 0.5f;
	aVertex[0].vPos.y = ((int)y1) - 0.5f;
	aVertex[0].dwColor = dwColor;

	aVertex[1].vPos.x = ((int)x2) - 0.5f;
	aVertex[1].vPos.y = ((int)y2) - 0.5f;
	aVertex[1].dwColor = dwColor;

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, aVertex, sizeof(FVF_2DVERTEX) );
*/	
//	if( GetLineWidth() != 1.0f )
	float ratio = GetPhyScreenWidth() / GetLogicalScreenSize().w;
	float wLine = GetLineWidth() * ratio;
	m_pd3dxLine->SetWidth( wLine );
	m_pd3dxLine->Begin();
	D3DXVECTOR2 vList[2] = { D3DXVECTOR2( x1, y1 ), D3DXVECTOR2( x2, y2 ) };
	m_pd3dxLine->Draw( vList, 2, dwColor );
	m_pd3dxLine->End(); 
}

void XGraphicsD3D::DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 )
{
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x1 *= ratioX;	y1 *= ratioY;
	x2 *= ratioX;	y2 *= ratioY;
	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = ((int)x1) - 0.5f;
	aVertex[0].vPos.y = ((int)y1) - 0.5f;
	aVertex[0].dwColor = col1;

	aVertex[1].vPos.x = ((int)x2) - 0.5f;
	aVertex[1].vPos.y = ((int)y2) - 0.5f;
	aVertex[1].dwColor = col2;

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, aVertex, sizeof(FVF_2DVERTEX) );
}

void XGraphicsD3D::DrawLineList( XGraphics::xVERTEX *vList, int numLines )
{
	XGraphics::xVERTEX *p = vList;
	float x1, y1;
	XCOLOR col1;
	for( int i = 0; i < numLines; ++i )
	{
		x1 = p->x;
		y1 = p->y;
		col1 = XCOLOR_RGBA( (BYTE)(p->r * 255.f), (BYTE)(p->g * 255.f), (BYTE)(p->b * 255.f), (BYTE)(p->a * 255.f) );
		++p;
		DrawLine( x1, y1, p->x, p->y, 
			col1, 
			XCOLOR_RGBA( (BYTE)(p->r * 255.f), (BYTE)(p->g * 255.f), (BYTE)(p->b * 255.f), (BYTE)(p->a * 255.f) )
			);
		++p;
	}
}
//
//
//
void	XGraphicsD3D::DrawPoint( float x, float y, float nSize, DWORD dwColor )
{
	float ratioX = (GetRatioWidth()) ;
	float ratioY = (GetRatioHeight());
	x *= ratioX;	y *= ratioY;
	FVF_2DVERTEX	*aVertex = m_aVertex;

	aVertex[0].vPos.x = (float)x;
	aVertex[0].vPos.y = (float)y;
	aVertex[0].dwColor = dwColor;

	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, aVertex, sizeof(FVF_2DVERTEX) );
}

void XGraphicsD3D::DrawPointList( FVF_2DVERTEX *pList, int nNum, float nSize )
{
	FVF_2DVERTEX	*aVertex = (pList)? pList : m_aVertex;
	if( nNum == 0 )
		nNum = m_nItorVertex;
//	aVertex[0].vPos.x = (float)x;
//	aVertex[0].vPos.y = (float)y;
//	aVertex[0].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, nNum, aVertex, sizeof(FVF_2DVERTEX) );
}

void XGraphicsD3D::DrawLineList( XE::VEC2 *pvList, int maxPoint, XCOLOR col, float thickness )
{
	if( pvList == NULL )	return;
	if( maxPoint < 2 )	return;	// 점이 최소한 2개이상은 되어야 한다
	for( int k = 1; k < maxPoint; k++ )
	{
		XE::DrawLine( pvList[k-1], pvList[k], col, thickness );		
	}
}
#if 0
// v1 - v2를 잇는 곡선을 그린다
void XGraphicsD3D::DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, 
														const XE::VEC2& v2, const XE::VEC2& v3,
														int maxSlice, XCOLOR col, xtLine lineType )
{
/*	XE::VEC2 vPrev;
	vPrev = v1;
	D3DXVECTOR2 dv0, dv1, dv2, dv3, vOut;
	for( int i = 1; i <= maxSlice; i ++ )		// 0.x ~ 1.0까지
	{
		dv0 = v0;		dv1 = v1;		dv2 = v2;		dv3 = v3;
		XE::xiCatmullrom
		D3DXVec2CatmullRom( &vOut, &dv0, &dv1, &dv2, &dv3, (float)i / maxSlice );
		float thickness = GetLineWidth();
		if( lineType == xLINE_LINE )
			XE::DrawLine( vPrev.x, vPrev.y, vOut.x, vOut.y, col, thickness );
		else
		if( lineType == xLINE_POINT )
			DrawPoint( vOut.x, vOut.y, 1, col );
		vPrev = vOut;
	} */
	XE::VEC2 vPrev;
	vPrev = v1;
	XE::VEC2 dv0, dv1, dv2, dv3, vOut;
	for( int i = 1; i <= maxSlice; i ++ )		// 0.x ~ 1.0까지
	{
		dv0 = v0;		dv1 = v1;		dv2 = v2;		dv3 = v3;
		Vec2CatmullRom( vOut, dv0, dv1, dv2, dv3, (float)i / maxSlice );
		float thickness = GetLineWidth();
		if( lineType == xLINE_LINE )
			XE::DrawLine( vPrev.x, vPrev.y, vOut.x, vOut.y, col, thickness );
		else
			if( lineType == xLINE_POINT )
				DrawPoint( vOut.x, vOut.y, 1, col );
		vPrev = vOut;
	}
}


void XGraphicsD3D::DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType )
{
	if( pvList == NULL )	return;
	if( maxPoint < 2 )	return;	// 점이 최소한 2개이상은 되어야 한다
	XE::VEC2 v0, v1, v2, v3;
	 
	for( int k = 0; k < maxPoint-1; k++ )
	{
		if( k == 0 )	v0 = pvList[k];
		else			v0 = pvList[k-1];
		v1 = pvList[k];
		v2 = pvList[k+1];
		if( k == maxPoint-2 )	v3 = pvList[k+1];
		else						v3 = pvList[k+2];
		// 점사이를 보간하며 스플라인을 그린다
		DrawSplineCatmullRom( v0, v1, v2, v3, maxSlice, col, lineType );
	}
}
#endif // 0
/**
 @brief 속이 빈 원을 그린다.
 @param x 원의 중앙좌표
 @param y 원의 중앙좌표.
 @param radius 원의 반지름
 @param dwColor 원의 컬러
 @param steps 원의 각형. 0이면 자동계산
 @param dOffset 다각형을 그릴때 선을 그리기 시작하는 각도를 조절. 디폴트는 0도
 
*/
// void XGraphicsD3D::DrawCircle( float x, float y, float radius, DWORD dwColor, int steps, float dOffset )
// {
// 	float ratioX = (GetRatioWidth()) ;
// 	float ratioY = (GetRatioHeight());
// 	x *= ratioX;	y *= ratioY;
// 	radius *= ratioX;
// 
// 	m_pd3dxLine->SetWidth( GetLineWidth() );
// 	m_pd3dxLine->Begin();
// 	D3DXVECTOR2 v[2];
// 	v[0].x = x + cosf(D2R(dOffset)) * radius;
// 	v[0].y = y + sinf(D2R(dOffset)) * radius;
// 	float dUnit = 360.f / (radius / 4.f);
// 	if( steps > 0 )
// 		dUnit = 360.f / steps;
// 	float rAng = 0;
// 	for( rAng = D2R(dUnit + dOffset); rAng < D2R(360.f); rAng += D2R(dUnit) )
// 	{
// 		v[1].x = x + cosf(rAng) * radius;
// 		v[1].y = y + sinf(rAng) * radius;
// 		m_pd3dxLine->Draw( v, 2, dwColor );
// 		//
// 		v[0] = v[1];
// 	}
// 	v[1].x = x + cosf(D2R(dOffset)) * radius;
// 	v[1].y = y + sinf(D2R(dOffset)) * radius;
// 	m_pd3dxLine->Draw( v, 2, dwColor );
// 
// 	m_pd3dxLine->End(); 
// }

#endif // dx