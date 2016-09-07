#pragma once
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

class XSurface;
namespace XE {
	struct xImage;
}
//class CKSurface;

namespace XE {
	BOOL ClippingLine( XE::VEC2 *pvOut
									, const XE::VEC2* pvLines
									, int numLine
									, float x1, float y1, float x2, float y2 );
	void CreateConvertPixels( const void* pImgSrc, 
														int wSrc, int hSrc, 
														XE::xtPixelFormat fmtSrc, 
														const void** ppDstOut,
														XE::xtPixelFormat fmtDst );
	inline void CreateConvertPixels( const void* pImgSrc,
														const XE::VEC2& sizeMemSrc,
														XE::xtPixelFormat fmtSrc,
														const void** ppDstOut,
														XE::xtPixelFormat fmtDst ) {
		CreateConvertPixels( pImgSrc, (int)sizeMemSrc.x, (int)sizeMemSrc.y, fmtSrc, ppDstOut, fmtDst );
	}
	inline void CreateConvertPixels( const void* pImgSrc,
																	 const XE::POINT& sizeMemSrc,
																	 XE::xtPixelFormat fmtSrc,
																	 const void** ppDstOut,
																	 XE::xtPixelFormat fmtDst ) {
		CreateConvertPixels( pImgSrc, sizeMemSrc.x, sizeMemSrc.y, fmtSrc, ppDstOut, fmtDst );
	}
};

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
	static DWORD s_dwDraw;
// 	static XSPLock GetspLock() {
// 		return s_spLock;
// 	}
	static XSPLock s_spLock;
private:
	/**
	스크린의 논리적 해상도. 게임의 논리적 해상도라고도 불린다. 
	기기는 해상도가 여러개지만 게임은 일정한 논리적 해상도를 정해두고 거기에 맞춰 게임을 만든다.
	주의할점은 기기의 해상도 가로세로 비율과 논리적해상도의 가로세로 비율은 같아야 그림이 찌그러져 보이지 않는다.
	*/
	int	m_nWidth, m_nHeight;			
	xPixelFormat	m_pixelFormat;
	
	int m_nViewportLeft, m_nViewportTop;			
	int m_nViewportRight, m_nViewportBottom;		// 
	int m_nBackBufferWidth, m_nBackBufferHeight;	
	int m_nWorkBufferWidth, m_nWorkBufferHeight;	
	int m_nPhyScreenWidth, m_nPhyScreenHeight;		
#ifdef _XDYNA_RESO
	XE::VEC2 m_vScreenLT;		// 다이나믹 해상도에서 게임화면이 위나 옆이 짤렸을때 보정된 게임화면의 좌상귀좌표
#endif
	float m_wLine;		// 선굵기	
	
	void Init( void ) {
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
	void Destroy( void ) {	
		XTRACE(""); Init();	
	}
protected:
	XE::POINT m_ptViewportStackLT, m_ptViewportStackRB;
public:
	
	XGraphics() { Init(); }
	XGraphics( int widthLogicalResolution, int heightLogicalResolution, xPixelFormat pixelFormat )
	{
		Init( widthLogicalResolution, heightLogicalResolution, pixelFormat );
	}
	virtual ~XGraphics() { Destroy(); }
	
	void	Init( int widthLogicalResolution, int heightLogicalResolution, xPixelFormat pixelFormat );
	
#ifdef _XDYNA_RESO
	GET_SET_ACCESSOR( const XE::VEC2&, vScreenLT );
#endif
	/**
	 Physical해상도: 기기의 실제 물리적인 해상도 ex:아이패드1(768x1024), iPhone4(640x960)
	 게임의 좌표계: 실제 프로그래밍에 쓰이는 좌표계. 물리적인 해상도와 관계없이 프로젝트가 정한 화면비율에 따라 독자적인 좌표계를 쓴다.
	              아이폰의 비율을 따른다면 보통 320x480을 쓰며 16:9를 쓴다면 720:1280을 쓴다. 이는 프로젝트가 얼마나 고해상도 텍스쳐를 쓰느냐에 따라 다르다.
	 Logical해상도: 물리적인 해상도를 게임좌표계의 해상도로 변환했을때의 논리적인 해상도. 
	               기기마다 화면비율이 다르므로 가로 혹은 세로 해상도가 달라질 수 있다. 예를들어 게임좌표계를 320X480으로 썼으나
				   기기가 16:9비율이라면 가로해상도를 기준으로 잡았을때 논리적인 해상도는 320x568이 된다.

	*/
//	int		GetScreenWidth( void ) { XBREAK( m_nWidth == 0 ); return m_nWidth; }
//	int		GetScreenHeight( void ) { XBREAK( m_nWidth == 0 ); return m_nHeight; }
//	XE::VEC2 GetScreenSize( void ) { return XE::VEC2( m_nWidth, m_nHeight ); }
	XE::VEC2 GetLogicalScreenSize( void ) { return XE::VEC2( m_nWidth, m_nHeight ); }
	void	SetScreenSize( int w, int h ) { m_nWidth = w; m_nHeight = h; }
	xPixelFormat GetPixelFormat( void ) { return m_pixelFormat; }
	void	SetPixelFormat( xPixelFormat pixelFormat ) { m_pixelFormat = pixelFormat; }
	int		GetBackBufferWidth( void ) { return m_nBackBufferWidth; }
	int		GetBackBufferHeight( void ) { return m_nBackBufferHeight; }
	void	SetBackBufferSize( int w, int h ) { m_nBackBufferWidth = w; m_nBackBufferHeight = h; }
	int		GetWorkBuffereWidth( void ) { return m_nWorkBufferWidth; }
	int		GetWorkBuffereHeight( void ) { return m_nWorkBufferHeight; }
	void	SetWorkBufferSize( int w, int h ) { m_nWorkBufferWidth = w; m_nWorkBufferHeight = h; }
//	const XE::VEC2& GetGScale( void ) { return m_vGScale; }
//	void	SetGScale( float sx, float sy ) { m_vGScale.Set( sx, sy ); }
//	void	SetGScale( const XE::VEC2& vGScale ) { m_vGScale = vGScale; }
	int		GetPhyScreenWidth( void ) { return m_nPhyScreenWidth; }
	int		GetPhyScreenHeight( void ) { return m_nPhyScreenHeight; }
    XE::VEC2 GetPhyScreenSize( void ) { return XE::VEC2( m_nPhyScreenWidth, m_nPhyScreenHeight ); }
	void	SetPhyScreenSize( int w, int h ) { m_nPhyScreenWidth = w; m_nPhyScreenHeight = h; 	}
/*	XE::VEC2 GetLogicalDeviceSize( void ) {
		XE::VEC2 size;
		size.w = XE::GetGameWidth();
		m_nPhyScreenHeight
	} */
	float GetLineWidth( void ) { return m_wLine; }
	float SetLineWidth( float w ) { 
		float oldw = m_wLine;
		m_wLine = w; 
		return oldw;
	}
	float GetRatioWidth( void ) {
		return GetPhyScreenWidth() / GetLogicalScreenSize().w;
	}
	float GetRatioHeight( void ) {
		return GetPhyScreenHeight() / GetLogicalScreenSize().h;
	}
	XE::VEC2 GetRatioResolution( void ) {
		return GetPhyScreenSize() / GetLogicalScreenSize();
	}

	
	virtual void*	GetScreenPtr( void ) { XLOG( "구현되지 않았습니다" );return NULL; }
	virtual void*	LockBackBufferPtr( int *pWidth, BOOL bReadOnly ) { XLOG( "구현되지 않았습니다" );return NULL; }		
	virtual void*	GetWorkBufferPtr( void ) { XLOG( "구현되지 않았습니다" );return NULL; }
	virtual void*	ReadBackBuffer( int width, int height, void *pBuffer ) { XLOG( "구현되지 않았습니다" );return NULL; }
	virtual void		UnlockBackBufferPtr( void ) {XLOG( "구현되지 않았습니다" );}
//	virtual void	ScreenToSurface( int x, int y, int width, int height, CKSurface *surface ) {}
//	virtual void	ScreenToSurface( CKSurface *surface ) {}
	virtual void	ScreenToSurface( int x, int y, int width, int height, XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual void	ScreenToSurface( BOOL bHighReso, XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual XSurface* CreateScreenToSurface( BOOL bHighReso ) {XLOG( "구현되지 않았습니다" ); return NULL;}
	virtual void	SetDrawTarget( XSurface *pSurface ) {XLOG( "구현되지 않았습니다" );}
	virtual void	ReleaseDrawTarget( void ) {XLOG( "구현되지 않았습니다" );}
	virtual void RestoreDevice() = 0;

	
	//
	int GetViewportLeft( void ) { return m_nViewportLeft; }
	int GetViewportRight( void ) { return m_nViewportRight; }
	int GetViewportTop( void ) { return m_nViewportTop; }
	int GetViewportBottom( void ) { return m_nViewportBottom; }
	
	int GetViewportWidth( void ) { return m_nViewportRight - m_nViewportLeft + 1; }
	int GetViewportHeight( void ) { return m_nViewportBottom - m_nViewportTop + 1; }
    XE::VEC2 GetViewportSize( void ) { return XE::VEC2( GetViewportWidth(), GetViewportHeight() ); }
	XE::VEC2 GetViewportLT( void ) { return XE::VEC2( m_nViewportLeft, m_nViewportTop ); }
	XE::VEC2 GetViewportRB( void ) { return XE::VEC2( m_nViewportRight, m_nViewportBottom ); }
	void SetViewportWidth( int vw ) { m_nViewportRight = m_nViewportLeft + vw - 1; }
	void SetViewportHeight( int vh ) { m_nViewportBottom = m_nViewportTop + vh - 1; }
	
	void SetViewportSize( int w, int h ) { 
		SetViewportWidth(w); 
		SetViewportHeight(h); 
	}
	virtual void SetViewport( int left, int top, int right, int bottom ) { 	
		m_nViewportLeft = left; 
		m_nViewportRight = right; 
		m_nViewportTop = top; 
		m_nViewportBottom = bottom; 
	}
	void SetViewport( XE::POINT ptLT, XE::POINT ptSize ) { 
		SetViewport( ptLT.x, ptLT.y, ptLT.x + ptSize.w - 1, ptLT.y + ptSize.h - 1 ); 
	}
	void SetViewport( const XE::VEC2& ptLT, const XE::VEC2& ptSize ) { 
		SetViewport( (int)ptLT.x, (int)ptLT.y, (int)(ptLT.x + ptSize.w - 1.f), (int)(ptLT.y + ptSize.h - 1.f) ); 
	}
	// BACKUP/RESTORE_VIEWPORT 매크로로 쓰는게 더 편할걸?
	void SetViewport( XE::xRECT& rect ) {
		SetViewport( rect.vLT, rect.GetSize() );
	}
	XE::xRECT GetViewportRECT( void ) {
		XE::xRECT rect;
		rect.vLT = GetViewportLT();
		rect.vRB = GetViewportRB();
		return rect;
	}
/*	void BackupViewport( void ) { 
	void BackupViewport( void ) { 
		m_ptViewportStackLT.Set( m_nViewportLeft, m_nViewportTop );
		m_ptViewportStackRB.Set( m_nViewportRight, m_nViewportBottom );
	}
	virtual void RestoreViewport( void ) {
		m_nViewportLeft = m_ptViewportStackLT.x;
		m_nViewportTop = m_ptViewportStackLT.y;
		m_nViewportRight = m_ptViewportStackRB.x;
		m_nViewportBottom = m_ptViewportStackRB.y;
	} */
	// 뷰포트 로컬좌표를 전체 스크린좌표로 바꿔준다.
	XE::VEC2 ViewportPosToScreenPos( const XE::VEC2& posViewport ) {
		return GetViewportLT() + posViewport;
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
	static XE::POINT sAlignPowSize( int width, int height ) {
		int w = width;
		int h = height;
		AlignPowSize( &w, &h );
		return XE::POINT( w, h );
	}
	static XE::POINT sAlignPowSize( const XE::POINT& sizeSrc ) {
		return sAlignPowSize( sizeSrc.w, sizeSrc.h );
	}
//	BOOL ClippingLine( XE::VEC2 *pvOut, const XE::VEC2 *pvLines, int numLine, float x1, float y1, float x2, float y2 );

	// virtual
	virtual xRESULT BeginScene( void ) { return xSUCCESS; }
	virtual void EndScene( void ) {}

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
	void FillRect( const XE::VEC2& vLT, const XE::VEC2& vRB, XCOLOR dwColor ) { FillRect( vLT.x, vLT.y, vRB.x-vLT.x+1, vRB.y-vLT.y+1, dwColor ); }
	void FillRectSize( const XE::VEC2& vLT, const XE::VEC2& vSize, XCOLOR dwColor ) { FillRect( vLT.x, vLT.y, vSize.x, vSize.y, dwColor ); }
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
	virtual void DrawCircle( const XE::VEC2& vCenter, float radius, XCOLOR col, int steps, float dOffset );
	inline void DrawCircle( float x, float y, float radius, XCOLOR col, int steps = 0, float dOffset = 0.f ) { 
		DrawCircle( XE::VEC2(x,y), radius, col, steps, dOffset );
	}
	// x, y를 중심으로 반지름radius크기의 파이를 그린다. 시작각도angStart를 시작으로 끝각도angEnd까지 돌아간다. maxSlice이 클수록 원은 더 부드러워진다
	virtual void DrawPie( float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 );
	// DrawPie와 같지만 pvLines에 라인리스트를 넣어주면 그 내부에서 클리핑처리된다. numLine은 라인개수
	virtual void DrawPieClip( const XE::VEC2 *pvLines, int numLine, float x, float y, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) {WARNING_NOT_IMPLEMENT;} 
	void DrawPie( const XE::VEC2& vPos, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) { 
		DrawPie( vPos.x, vPos.y, radius, angStart, angEnd, color, maxSlice ); 
	}	
	void DrawPieClip( const XE::VEC2 *pvLines, int numLine, const XE::VEC2& vPos, float radius, float angStart, float angEnd, XCOLOR color, int maxSlice=36 ) { 
		DrawPieClip( pvLines, numLine, vPos.x, vPos.y, radius, angStart, angEnd, color, maxSlice ); 
	} 
	virtual void DrawLineList( XE::VEC2 *pvList, int maxPoint, XCOLOR col, float thickness=1.0f ) { WARNING_NOT_IMPLEMENT; }
	virtual void DrawLineList( XGraphics::xVERTEX *vList, int numLines ) { WARNING_NOT_IMPLEMENT; }
	virtual void DrawSplineCatmullRom( const XE::VEC2& v0, const XE::VEC2& v1, const XE::VEC2& v2, const XE::VEC2& v3, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE );
	virtual void DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col, xtLine lineType=xLINE_LINE );
	void DrawSplineCatmullRom( XE::VEC2 *pvList, int maxPoint, int maxSlice, XCOLOR col1, XCOLOR col2, xtLine lineType );
	
// private:
// 	friend class XImageMng;
// 	friend class XSprite;
//	virtual XSurface* CreateSurface( BOOL bHighReso ) = 0;
	virtual XSurface* CreateSurface() = 0;
	virtual XSurface* CreateSurface( BOOL bHighReso
																, float w, float h
																, float adjx, float adjy
																, WORD *pImg
																, XCOLOR dwColor = XCOLOR_GREEN
																, BOOL bSrcKeep = FALSE ) { WARNING_NOT_IMPLEMENT; return NULL; }
	XSurface*	CreateSurface( BOOL bHighReso
												, int w, int h
												, int adjx, int adjy
												, WORD *pImg
												, XCOLOR dwColor = XCOLOR_GREEN
												, BOOL bSrcKeep = FALSE ) { 
		return CreateSurface( bHighReso
											, (float)w, (float)h
											, (float)adjx, (float)adjy
											, pImg
											, dwColor
											, bSrcKeep ); 
	}
	virtual XSurface* CreateSurface( BOOL bHighReso
																, int srcx, int srcy
																, int srcw, int srch
																, float dstw, float dsth
																, float adjx, float adjy
																, DWORD *pSrcImg
																, BOOL bSrcKeep = FALSE ) { WARNING_NOT_IMPLEMENT; return NULL; }	// 이미지의 일부분만 서피스로 만드는 버전
	inline XSurface* CreateSurface( const _tstring& strRes, XE::xtPixelFormat formatSurface, bool bSrcKeep = false, bool bMakeMask = false ) {
		return CreateSurface( strRes.c_str(), formatSurface, bSrcKeep, bMakeMask );
	}
	virtual XSurface* CreateSurface( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE ) {	WARNING_NOT_IMPLEMENT; return NULL; }
	virtual BOOL LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage ) { WARNING_NOT_IMPLEMENT; return FALSE; }
	bool LoadImg( LPCTSTR szFilename, XE::xImage* pOut );
	bool LoadImg( const _tstring& resImg, XE::xImage* pOut );
private:
	// pure virtual
public:
	// 외부노출용 인터페이스
	XSurface* CreateSurface( bool bHighReso
												, LPCTSTR szRes
												, XE::xtPixelFormat formatSurface
												, bool bSrcKeep = false, bool bMakeMask = false 
												, bool bAsync = false );
	inline XSurface* CreateSurface( LPCTSTR szRes
												, XE::xtPixelFormat formatSurface
												, bool bSrcKeep = false, bool bMakeMask = false ) {
		return CreateSurface( true, szRes, formatSurface, bSrcKeep, bMakeMask );
	}
	XSurface* CreateSurface( const XE::POINT& sizeSurfaceOrig
												, const XE::VEC2& vAdj
												, XE::xtPixelFormat formatSurface								
												, void* const pImgSrc
												, const XE::POINT& sizeMemSrc
												, bool bSrcKeep, bool bMakeMask );
	// 
	virtual void DrawFan( float *pAryPos, float *pAryCol, int numVertex, int numFan ) {WARNING_NOT_IMPLEMENT;} 
	// 하위상속된 XSurface의 값까지 모두 src에서 dst로 값 복사한다.
	virtual void CopyValueSurface( XSurface* pDst, XSurface* pSrc ) = 0;
}; // class XGraphics

inline BOOL XGraphics::IsViewportOut( int x, int y )
{
    if( x < GetViewportLeft() || x > GetViewportRight()  )	return TRUE;
    if( y > GetViewportBottom() || y < GetViewportTop() )	return TRUE;
	
	return FALSE;
}

inline XGraphics*	GetGraphics( void ) { return XGraphics::s_pGraphics; }

BOOL Clipper2( int _xl, int _yt, int _xr, int _yb, int *x, int *y, int *x2, int *y2,int *sx, int *sy, int *sx2, int *sy2 );
BOOL Clipper( int _xl, int _yt, int _xr, int _yb,  int *x, int *y, int *w, int *h,int *sx, int *sy, int *sw, int *sh );

// 화면좌표를 0~1사이로 보고 소숫점으로 넣으면 좌표로 변환해주는 매크로

namespace XE 
{
	void SetProjection( float wLogicalScr, float hLogicalScr );
	//
	inline void DrawLine( float x1, float y1, float x2, float y2, XCOLOR color, float thickness=1.0f )	{
		if( GRAPHICS )
		{
			float widthOld = GRAPHICS->GetLineWidth();
			GRAPHICS->SetLineWidth( thickness );
			GRAPHICS->DrawLine( x1, y1, x2, y2, color );
			GRAPHICS->SetLineWidth( widthOld );
		}
	}
	inline void DrawLine( const VEC2& vPos1, const VEC2& vPos2, XCOLOR color, float thickness=1.0f )	{
		if( GRAPHICS )
		{
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
	inline void DrawRectangle( float left, float top, float right, float bottom, XCOLOR color, float thickness = 1.0f )	{
		float width = right - left;
		float height = bottom - top;
		if( GRAPHICS )
		{
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
	void RGB2HSL( XCOLOR clr, double & hue, double & saturation, double & lightness );
	XCOLOR HSL2RGB( double hue, double saturation, double lightness );
}

// 글로벌스케일로 변환해주는 매크로
//#define XSX(A)	((A) * GRAPHICS->GetGScale().x)
//#define XSY(A)	((A) * GRAPHICS->GetGScale().y)
//#define XS(V)	((V) * GRAPHICS->GetvGScale())		// XE::VEC2버전

#define BACKUP_VIEWPORT		\
	XE::VEC2 __vpLT = GRAPHICS->GetViewportLT(); \
	XE::VEC2 __vpSize = GRAPHICS->GetViewportSize();

#define RESTORE_VIEWPORT		\
	GRAPHICS->SetViewport( __vpLT, __vpSize ); \
	XE::SetProjection( __vpSize.w, __vpSize.h );


