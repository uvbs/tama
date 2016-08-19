#ifndef __XGRAPHICS_H__
#define __XGRAPHICS_H__
/*
 *  XGraphics.h
 *  GLTest
 *
 *  Created by xuzhu on 09. 07. 18.
 *  Copyright 2009 LINKS CO.,LTD. All rights reserved.
 *
 */

#ifdef _WIN32
#include <windows.h>
#pragma warning( disable : 4003 )		
#pragma warning( disable : 4068 )		
#endif
#include "global.h"
#include "Debug.h"
#include "xColor.h"
#include "XE.h"
#include "XGraphicsDef.h"
#include "XSurface.h"

using namespace std;
class XSurface;
class CKSurface;

#define WARNING_NOT_IMPLEMENT	XBREAKF(1,"구현되지 않은 virtual멤버%s를 사용했습니다", __FUNCTION__)

class XGraphics;
#define GRAPHICS	XGraphics::s_pGraphics

class XGraphics
{
public:
	struct xVERTEX {
		float x, y;
		float r, g, b, a;
	};
	enum xtLine { xLINE_NONE=0, xLINE_POINT, xLINE_LINE };
	static XGraphics *s_pGraphics;		
	static BOOL s_bCaptureBackBuffer;			// Present전에 현재 백버퍼를 카피해둬야 함
	static XSurface* s_pLastBackBuffer;			// 마지막으로 캡쳐된 백버퍼 화면
	static void CaptureBackBuffer( BOOL bHighReso ) {		// 
		if( s_bCaptureBackBuffer )	{	// 캡쳐가 예약이 됐으면 
			SAFE_DELETE( s_pLastBackBuffer );
			s_pLastBackBuffer = GRAPHICS->CreateScreenToSurface( bHighReso );		// 백버퍼를 캡쳐해서 보관
		}
		s_bCaptureBackBuffer = FALSE;
	}
private:
	int	m_nWidth, m_nHeight;			
	xPixelFormat	m_pixelFormat;
	
	int m_nViewportLeft, m_nViewportTop;			
	int m_nViewportRight, m_nViewportBottom;		// 
	int m_nBackBufferWidth, m_nBackBufferHeight;	
	int m_nWorkBufferWidth, m_nWorkBufferHeight;	
	int m_nPhyScreenWidth, m_nPhyScreenHeight;		
//	XE::VEC2 m_vGScale;		// 글로벌 스케일	
	float m_wLine;		// 선굵기	
	
	void Init()
	{
		XGraphics::s_pGraphics = this;
		m_nWidth = m_nHeight = 0;
		m_pixelFormat = xPIXELFORMAT_NONE;
		m_nViewportLeft = m_nViewportTop = 0;		
		m_nViewportRight = m_nViewportBottom = 0;		// 
		m_nBackBufferWidth = m_nBackBufferHeight = 0;
		m_nWorkBufferWidth = m_nWorkBufferHeight = 0;
		m_nPhyScreenWidth = m_nPhyScreenHeight = 0;
//		m_vGScale.Set( 1.0f, 1.0f );
		m_wLine = 1.0f;
	}
	void Destroy() {	XLOG(""); Init();	}
protected:
	XE::POINT m_ptViewportStackLT, m_ptViewportStackRB;
public:
	
	XGraphics() { Init(); }
	XGraphics( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat )
	{
		Init( nResolutionWidth, nResolutionHeight, pixelFormat );
	}
	virtual ~XGraphics() { Destroy(); }
	
	void	Init( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat );
	
	// Ω∫≈©∏∞ «ÿªÛµµ
	int		GetScreenWidth() { XBREAK( m_nWidth == 0 ); return m_nWidth; }
	int		GetScreenHeight() { XBREAK( m_nWidth == 0 ); return m_nHeight; }
	void	SetScreenSize( int w, int h ) { m_nWidth = w; m_nHeight = h; }
	xPixelFormat GetPixelFormat() { return m_pixelFormat; }
	void	SetPixelFormat( xPixelFormat pixelFormat ) { m_pixelFormat = pixelFormat; }
	int		GetBackBufferWidth() { return m_nBackBufferWidth; }
	int		GetBackBufferHeight() { return m_nBackBufferHeight; }
	void	SetBackBufferSize( int w, int h ) { m_nBackBufferWidth = w; m_nBackBufferHeight = h; }
	int		GetWorkBuffereWidth() { return m_nWorkBufferWidth; }
	int		GetWorkBuffereHeight() { return m_nWorkBufferHeight; }
	void	SetWorkBufferSize( int w, int h ) { m_nWorkBufferWidth = w; m_nWorkBufferHeight = h; }
//	const XE::VEC2& GetGScale() { return m_vGScale; }
//	void	SetGScale( float sx, float sy ) { m_vGScale.Set( sx, sy ); }
//	void	SetGScale( const XE::VEC2& vGScale ) { m_vGScale = vGScale; }
	int		GetPhyScreenWidth() { return m_nPhyScreenWidth; }
	int		GetPhyScreenHeight() { return m_nPhyScreenHeight; }
	void	SetPhyScreenSize( int w, int h ) { m_nPhyScreenWidth = w; m_nPhyScreenHeight = h; 	}
	float GetLineWidth() { return m_wLine; }
	void SetLineWidth( float w ) { m_wLine = w; }
	
	virtual void*	GetScreenPtr() { XLOG( "구현되지 않았습니다" );return nullptr; }
	virtual void*	LockBackBufferPtr( int *pWidth, BOOL bReadOnly ) { XLOG( "구현되지 않았습니다" );return nullptr; }		
	virtual void*	GetWorkBufferPtr() { XLOG( "구현되지 않았습니다" );return nullptr; }
	virtual void*	ReadBackBuffer( int width, int height, void *pBuffer ) { XLOG( "구현되지 않았습니다" );return nullptr; }
	virtual void		UnlockBackBufferPtr() {XLOG( "구현되지 않았습니다" );}
//	virtual void	ScreenToSurface( int x, int y, int width, int height, CKSurface *surface ) {}
//	virtual void	ScreenToSurface( CKSurface *surface ) {}
	virtual void	ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual XSurface* CreateScreenToSurface( BOOL bHighReso ) {XLOG( "구현되지 않았습니다" ); return nullptr;}
	virtual void	SetDrawTarget( XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual void	ReleaseDrawTarget() {XLOG( "구현되지 않았습니다" );}
	
	// ∫‰∆˜∆Æ ≈©±‚.
	int GetViewportLeft() { return m_nViewportLeft; }
	int GetViewportRight() { return m_nViewportRight; }
	int GetViewportTop() { return m_nViewportTop; }
	int GetViewportBottom() { return m_nViewportBottom; }
	
	int GetViewportWidth() { return m_nViewportRight - m_nViewportLeft + 1; }
	int GetViewportHeight() { return m_nViewportBottom - m_nViewportTop + 1; }
	void SetViewportWidth( int vw ) { m_nViewportRight = m_nViewportLeft + vw - 1; }
	void SetViewportHeight( int vh ) { m_nViewportBottom = m_nViewportTop + vh - 1; }
	
	void SetViewportSize( int w, int h ) { SetViewportWidth(w); SetViewportHeight(h); }
	virtual void SetViewport( int left, int top, int right, int bottom ) { 	m_nViewportLeft = left; m_nViewportRight = right; m_nViewportTop = top; m_nViewportBottom = bottom; }
	void SetViewport( XE::POINT ptLT, XE::POINT ptSize ) { SetViewport( ptLT.x, ptLT.y, ptLT.x + ptSize.w, ptLT.y + ptSize.h ); }
	void BackupViewport() { 
		m_ptViewportStackLT.Set( m_nViewportLeft, m_nViewportTop );
		m_ptViewportStackRB.Set( m_nViewportRight, m_nViewportBottom );
	}
	virtual void RestoreViewport() {
		m_nViewportLeft = m_ptViewportStackLT.x;
		m_nViewportTop = m_ptViewportStackLT.y;
		m_nViewportRight = m_ptViewportStackRB.x;
		m_nViewportBottom = m_ptViewportStackRB.y;
	}

	BOOL IsViewportOut( int x, int y );
	
	// Convert pixel
	WORD ConvertRGB565( WORD rgb ) const;
	WORD ConvertRGB555( WORD rgb ) const;
	DWORD Convert555ToRGBA8( WORD rgb, BYTE a ) const;
	DWORD Convert565ToRGBA8( WORD rgb, BYTE a ) const;

	static int AlignPow2Width( int width );
	static int AlignPow2Height( int height );
	static int AlignPowSize( int *width, int *height );
	BOOL ClippingLine( XE::VEC2 *pvOut, const XE::VEC2 *pvLines, int numLine, float x1, float y1, float x2, float y2 );

	// virtual
	virtual XE::xRESULT BeginScene() { return XE::xSUCCESS; }
	virtual void EndScene() {}

	#pragma mark draw
	// draw
	virtual void	ClearScreen( XCOLOR color ) {}
	virtual int		GetPixel( int x, int y ) { return 0; }
	virtual void DrawPoint( float x, float y, float size, XCOLOR dwColor ) {}
	void DrawPoint( const XE::VEC2& vPos, float size, XCOLOR col ) { DrawPoint( vPos.x, vPos.y, size, col ); }
	void DrawRect( const XE::VEC2& vLT, const XE::VEC2& vRB, XCOLOR dwColor ) { DrawRect( vLT.x, vLT.y, vRB.x-vLT.x+1, vRB.y-vLT.y+1, dwColor ); }
	void DrawRectSize( const XE::VEC2& vLT, const XE::VEC2& vSize, XCOLOR dwColor ) { DrawRect( vLT.x, vLT.y, vSize.x, vSize.y, dwColor ); }
	virtual void DrawRect( float x, float y, float w, float h, XCOLOR dwColor ) {}
	void Draw3DRect( float x, float y, float w, float h, BOOL bBump ) {
		XCOLOR colLT, colRB;
		if( bBump ) {
			colLT = XCOLOR_RGBX( 255, 255, 255 );
			colRB = XCOLOR_RGBX( 160, 160, 160 );
		} else
		{
			colLT = XCOLOR_RGBX( 160, 160, 160 );
			colRB = XCOLOR_RGBX( 255, 255, 255 );
		}
		DrawHLine( x, y, w, colLT );
		DrawVLine( x, y, h, colLT );
		DrawHLine( x, y+h-1, w, colRB );
		DrawVLine( x+w-1, y, h, colRB );
	}
	template <typename T>
	void FillRect( T x, T y, T w, T h, XCOLOR dwColor ) { FillRect( (float)x, (float)y, (float)w, (float)h, dwColor ); }
	virtual void FillRect( float x, float y, float w, float h, XCOLOR dwColor ) {XLOG("아직 구현되지 않았습니다");}
//	template <typename T>
//	void FillRect( T x, T y, T w, T h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb ) { FillRect( x, y, w, h, collt, colrt, collb, colrb ); }
	virtual void FillRect( float x, float y, float w, float h, XCOLOR collt, XCOLOR colrt, XCOLOR collb, XCOLOR colrb ) {XLOG("아직 구현되지 않았습니다");}
	void FillRect( const XE::VEC2& vLT, const XE::VEC2& vRB, XCOLOR dwColor ) { 
		FillRect( vLT.x, vLT.y, vRB.x-vLT.x+1, vRB.y-vLT.y+1, dwColor ); 
	}
	void FillRectSize( const XE::VEC2& vLT, const XE::VEC2& vSize, XCOLOR dwColor ) { 
		FillRect( vLT, vLT + vSize, dwColor ); 
	}
	template <typename T>
	void DrawLine( T x1, T y1, T x2, T y2, XCOLOR color ) { DrawLine( (float)x1, (float)y1, (float)x2, (float)y2, color ); }
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR dwColor ) { XLOG("아직 구현되지 않았습니다"); }
	virtual void DrawLine( float x1, float y1, float x2, float y2, XCOLOR col1, XCOLOR col2 ) { XLOG("아직 구현되지 않았습니다"); }
	void DrawLine( const XE::VEC2& v1, const XE::VEC2& v2, XCOLOR dwColor ) { DrawLine( v1.x, v1.y, v2.x, v2.y, dwColor ); }
	void DrawLine( const XE::VEC2& v1, const XE::VEC2& v2, XCOLOR col1, XCOLOR col2 ) { DrawLine( v1.x, v1.y, v2.x, v2.y, col1, col2 ); }
	template<typename T>
	void DrawHLine( T x, T y, T w, XCOLOR dwColor ) { DrawLine( x, y, x+w, y, dwColor );  }
	template<typename T>
	void DrawVLine( T x, T y, T h, XCOLOR dwColor ) { DrawLine( x, y, x, y+h, dwColor );  }
	virtual void DrawCircle( float x, float y, float radius, DWORD dwColor, int numLine = 0 ) { XLOG("아직 구현되지 않았습니다"); }
	inline void DrawCircle( const XE::VEC2& vPos, float radius, DWORD dwColor, int numLine = 0 ) { 
		DrawCircle( vPos.x, vPos.y, radius, dwColor, numLine );
	}
	virtual void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) {WARNING_NOT_IMPLEMENT;}	// x, y를 중심으로 반지름radius크기의 파이를 그린다. 시작각도angStart를 시작으로 끝각도angEnd까지 돌아간다. maxSlice이 클수록 원은 더 부드러워진다
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) {WARNING_NOT_IMPLEMENT;} // DrawPie와 같지만 pvLines에 라인리스트를 넣어주면 그 내부에서 클리핑처리된다. numLine은 라인개수
	void DrawPie( const XE::VEC2& vPos, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) { DrawPie( vPos.x, vPos.y, radius, angStart, angEnd, color, maxSlice ); }	
	void DrawPieClip( const XE::VEC2 *pvLines, int numLine, const XE::VEC2& vPos, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) { DrawPieClip( pvLines, numLine, vPos.x, vPos.y, radius, angStart, angEnd, color, maxSlice ); } 
	virtual void DrawLineList( XE::VEC2 *pvList, int maxPoint, XCOLOR col ) { XBREAK(1); }
	virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines ) { XBREAK(1); }
	virtual void DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, const XE::VEC2& v2, const XE::VEC2& v3, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE ) { XBREAK(1); }
	virtual void DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE ){ XBREAK(1); }
	
	virtual XSurface* CreateSurface( BOOL bHighReso, LPCSTR szFilename, XCOLOR dwColor, BOOL bSrcKeep = FALSE ) { XBREAK(1); return nullptr; } // 옛날버전은 dwColor에 디폴트로 그린색이 있었음. CreateSurface( LPCTSTR, BOOL )과 충돌해서 뺌
	virtual XSurface* CreateSurface( BOOL bHighReso, float w, float h, float adjx, float adjy, WORD *pImg, XCOLOR dwColor = XCOLOR_GREEN, BOOL bSrcKeep = FALSE ) { XBREAK(1); return nullptr; }
	XSurface*	CreateSurface( BOOL bHighReso, int w, int h, int adjx, int adjy, WORD *pImg, XCOLOR dwColor = XCOLOR_GREEN, BOOL bSrcKeep = FALSE ) { return CreateSurface( bHighReso, (float)w, (float)h, (float)adjx, (float)adjy, pImg, dwColor, bSrcKeep ); }
	virtual XSurface* CreateSurface( BOOL bHighReso, int srcx, int srcy, int srcw, int srch, float dstw, float dsth, float adjx, float adjy, DWORD *pSrcImg, BOOL bSrcKeep = FALSE ) { XBREAK(1); return nullptr; }	// 이미지의 일부분만 서피스로 만드는 버전
//	XSurface*	CreateSurface( int srcx, int srcy, int w, int h, int adjx, int adjy, DWORD *pSrcImg, BOOL bSrcKeep = FALSE ) { return CreateSurface( srcx, srcy, w, h, w, h, adjx, adjy, pSrcImg, bSrcKeep ); }	// 이미지의 일부분만 서피스로 만드는 버전
	virtual XSurface* CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep = FALSE ) { XBREAK(1); return nullptr; }
	virtual XSurface* CreateSurface( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE ) {	XBREAK(1); return nullptr; }
//	virtual XSurface*	CreateSurfacePNG( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep=FALSE ) { XBREAK(1); return nullptr; }
	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage ) { XBREAK(1); return FALSE; }
	XSurface* CreateSurfaceFromFile( LPCTSTR szFilename, BOOL bSrcKeep=FALSE ) {
		int w, h;
		DWORD *pImage;
		if( LoadImg( szFilename, &w, &h, &pImage ) == FALSE )
			return FALSE;
		return CreateSurface( w, h, w, h, 0, 0, pImage, bSrcKeep );
	}
	XSurface* CreateSurfaceFromFile( LPCTSTR szFilename, int surfaceW, int surfaceH, BOOL bSrcKeep=FALSE ) {
		int w, h;
		DWORD *pImage;
		if( LoadImg( szFilename, &w, &h, &pImage ) == FALSE )
			return FALSE;
		return CreateSurface( surfaceW, surfaceH, w, h, 0, 0, pImage, bSrcKeep );
	}
	// 
};

inline BOOL XGraphics::IsViewportOut( int x, int y )
{
    if( x < GetViewportLeft() || x > GetViewportRight()  )	return TRUE;
    if( y > GetViewportBottom() || y < GetViewportTop() )	return TRUE;
	
	return FALSE;
}

inline XGraphics*	GetGraphics() { return XGraphics::s_pGraphics; }
#define XSCREEN_WIDTH		GRAPHICS->GetScreenWidth()
#define XSCREEN_HEIGHT		GRAPHICS->GetScreenHeight()

BOOL Clipper2( int _xl, int _yt, int _xr, int _yb, int *x, int *y, int *x2, int *y2,int *sx, int *sy, int *sx2, int *sy2 );
BOOL Clipper( int _xl, int _yt, int _xr, int _yb,  int *x, int *y, int *w, int *h,int *sx, int *sy, int *sw, int *sh );

// 화면좌표를 0~1사이로 보고 소숫점으로 넣으면 좌표로 변환해주는 매크로
#define GET_LOGI_X(X)	(GRAPHICS->GetScreenWidth() * (X))
#define GET_LOGI_Y(Y)	(GRAPHICS->GetScreenHeight() * (Y))

namespace XE 
{
	void SetProjection( int nScrWidth, int nScrHeight );
	//
	inline void DrawLine( float x1, float y1, float x2, float y2, XCOLOR color, float thickness=1.0f )	{
		if( GRAPHICS ) {
			float widthOld = GRAPHICS->GetLineWidth();
			GRAPHICS->SetLineWidth( thickness );
			GRAPHICS->DrawLine( x1, y1, x2, y2, color );
			GRAPHICS->SetLineWidth( widthOld );
		}
	}
	inline void DrawLine( const VEC2& vPos1, const VEC2& vPos2, XCOLOR color, float thickness=1.0f )	{
		if( GRAPHICS ) {
			float widthOld = GRAPHICS->GetLineWidth();
			GRAPHICS->SetLineWidth( thickness );
			GRAPHICS->DrawLine( vPos1, vPos2, color );
			GRAPHICS->SetLineWidth( widthOld );
		}
	}
	inline void FillRectangle( float left, float top, float right, float bottom, XCOLOR color )	{
		float width = right - left;
		float height = bottom - top;
		if( GRAPHICS )
			GRAPHICS->FillRect( left, top, width, height, color );
	}
	inline void FillRectangle( const XE::VEC2& v1, const XE::VEC2& v2, XCOLOR color )	{
		GRAPHICS->FillRect( v1, v2, color );
	}
	inline void FillRectangleSize( const XE::VEC2& vLT, const XE::VEC2& vSize, XCOLOR color )	{
//		XE::VEC2 vRB = vLT + vSize;
		GRAPHICS->FillRectSize( vLT, vSize, color );
	}
	inline void DrawRectangle( float left, float top, float right, float bottom, XCOLOR color, float thickness = 1.0f )	{
		float width = right - left;
		float height = bottom - top;
		if( GRAPHICS ) {
			float widthOld = GRAPHICS->GetLineWidth();
			GRAPHICS->SetLineWidth( thickness );
			GRAPHICS->DrawRect( left, top, width, height, color );
			GRAPHICS->SetLineWidth( widthOld );
		}
	}
	inline void DrawRectangle( const XE::VEC2& v1, const XE::VEC2& v2, XCOLOR color, float thickness = 1.0f ) {
		float l  = xmin( v1.x, v2.x );
		float r = xmax( v1.x, v2.x );
		float t = xmin( v1.y, v2.y );
		float b = xmax( v1.y, v2.y );
		DrawRectangle( l, t, r, b, color, thickness );
	}
	inline void DrawRectangleSize( const XE::VEC2& v, const XE::VEC2& vSize, XCOLOR color, float thickness = 1.0f ) {
		XE::VEC2 v2 = v + vSize;
		DrawRectangle( v, v2, color, thickness );
	}
}

// 글로벌스케일로 변환해주는 매크로
//#define XSX(A)	((A) * GRAPHICS->GetGScale().x)
//#define XSY(A)	((A) * GRAPHICS->GetGScale().y)
//#define XS(V)	((V) * GRAPHICS->GetvGScale())		// XE::VEC2버전


#endif // __XGRAPHICS_H__
