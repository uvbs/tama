#include "stdafx.h"
#include "etc/XSurface.h"
#include "XDrawGraph.h"
#include "etc/xGraphics.h"
#include "XImageMng.h"
#include "XFramework/client/XClientMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace XUTIL
{
// 간단하게 녹색바탕의 진행바를 그림
void DrawProgressBar( const XE::VEC2& vPos, float lerp, XE::VEC2 vSize, XCOLOR col, XCOLOR _colBack )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDrawBar )
		return;
#endif // _XPROFILE
	XCOLOR colBack = _colBack;
	if( colBack == 0 )
		colBack = XCOLOR_RGBA( XCOLOR_RGB_R(col), XCOLOR_RGB_G(col), XCOLOR_RGB_B(col), 64 );
	GRAPHICS->FillRectSize( vPos, vSize, colBack );		// 바탕 바
	float w = lerp * vSize.w;		// 현재 위치의 도트 너비
	GRAPHICS->FillRectSize( vPos, XE::VEC2( w, vSize.h ), col );
	GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
}
// 진행바 그리는 서피스 버전(자르는 버전)
void DrawBarHoriz( const XE::VEC2& vPos, float lerp, XSurface *pSurface, bool bReverse )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDrawBar )
		return;
#endif // _XPROFILE
	const auto vScale = pSurface->GetScalev2();
	XE::xRECTi rect;
	rect.Top() = 0;
	if( !bReverse ) {
		rect.Left() = 0;
		rect.Right() = (int)(pSurface->GetWidth() * lerp);
	} else {
		rect.Left() = (int)( pSurface->GetWidth() * (1.f - lerp) );
		rect.Right() = (int)( pSurface->GetWidth() );
	}
	rect.Bottom() = (int)pSurface->GetHeight();
	if( rect.Right() == 0 )
		return;
	float x = vPos.x;
	if( bReverse )
		x += rect.Left();
	pSurface->DrawSub( x, vPos.y, &rect );
}

// 확축으로 하는 버전
void DrawBarHoriz2( const XE::VEC2& vPos, float lerp, XSurface *pSurface, const XE::VEC2& vScale, bool bReverse )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDrawBar )
		return;
#endif // _XPROFILE
	float x = vPos.x;
	XE::VEC2 vScaleLerp = vScale;
	vScaleLerp.x *= lerp;
	if( !bReverse ) {
//		auto sizeBar = pSurface->GetSize() * vScale;
// 		vScaleLerp.x *= lerp;
	} else {
		const auto sizeBar = pSurface->GetSize() * vScale;
		x = vPos.x + (sizeBar.w * (1.f - lerp));

	}
// 	if( bReverse )
// 		x += rect.Left();
	pSurface->SetScale( vScaleLerp );
	pSurface->Draw( x, vPos.y );
}
// 진행바 그리는 서피스 버전
void DrawBarVert( const XE::VEC2& vPos, float lerp, XSurface *pSurface, bool bReverse )
{
#ifdef _XPROFILE
	if( XGraphics::s_dwDraw & XE::xeBitNoDrawBar )
		return;
#endif // _XPROFILE
	XE::xRECTi rect;
	rect.Right() = (int)pSurface->GetWidth();
	rect.Bottom() = (int)pSurface->GetHeight();
	rect.Left() = 0;
	rect.Top() = rect.Bottom() - (int)(pSurface->GetHeight() * lerp);
	if( rect.Right() == 0 )
		return;
	float ofs = pSurface->GetHeight() * (1 - lerp);
	pSurface->DrawSub( vPos.x, vPos.y + ofs, &rect );
}


} // namespace XUTIL

//========================================================
void XProgressBar::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
}
XProgressBar::XProgressBar( LPCTSTR szImg, LPCTSTR szImgBg, BOOL bHoriz )
{
	Init();
	XBREAK( XE::IsEmpty( szImg ) == TRUE );
	m_bHoriz = bHoriz;
	m_pSurface = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImg ),
																XE::xPF_ARGB4444,
																true, false, false, true );
	if( szImgBg )
		m_psfcBg = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImgBg ),
																XE::xPF_ARGB4444,
																true, false, false, true );
}

XProgressBar::XProgressBar( const XE::VEC2& vPos, LPCTSTR szImg, LPCTSTR szImgBg, BOOL bHoriz )
{
	Init();
	m_bHoriz = bHoriz;
	m_pSurface = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImg ),
																XE::xPF_ARGB4444,
																true, false, false, false );
	if( XE::IsHave( szImgBg ) )
		m_psfcBg = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImgBg ),
																XE::xPF_ARGB4444,
																true, false, false, false );
	m_vPos = vPos;
}

XProgressBar::XProgressBar( const XE::VEC2& vPos, const XE::VEC2& vSize, BOOL bHoriz )
{
	Init();
	m_bHoriz = bHoriz;
	m_vPos = vPos;
	m_vSize = vSize;
}

void XProgressBar::Draw( const XE::VEC2& vPos, float lerp )
{
	if( m_psfcBg )
		m_psfcBg->Draw( vPos );

	if( m_pSurface )
	{
		if( m_bHoriz )
			XUTIL::DrawBarHoriz2( vPos + m_vAdjBar, lerp, m_pSurface, XE::VEC2(1.f,1.f), m_bReverse );
		else
			XUTIL::DrawBarVert( vPos + m_vAdjBar, lerp, m_pSurface, m_bReverse );
	}
	else
		XUTIL::DrawProgressBar( vPos + m_vAdjBar, lerp, m_vSize, XCOLOR_GREEN );

}

void XProgressBar::Draw( float lerp )
{
	if( m_psfcBg )
		m_psfcBg->Draw( m_vPos );
	if( m_pSurface )
	{
		if( m_bHoriz )
			XUTIL::DrawBarHoriz( m_vPos + m_vAdjBar, lerp, m_pSurface, m_bReverse );
		else
			XUTIL::DrawBarVert( m_vPos + m_vAdjBar, lerp, m_pSurface, m_bReverse );
	}
	else
		XUTIL::DrawProgressBar( m_vPos + m_vAdjBar, lerp, m_vSize, XCOLOR_GREEN );

}

void XProgressBar::Draw( const XE::VEC2& vPos, const XE::VEC2& vSize, float lerp )
{
	if( m_psfcBg )
		m_psfcBg->Draw( vPos );
	if( m_pSurface )
	{
		if( m_bHoriz )
			XUTIL::DrawBarHoriz( vPos + m_vAdjBar, lerp, m_pSurface, m_bReverse );
		else
			XUTIL::DrawBarVert( vPos + m_vAdjBar, lerp, m_pSurface, m_bReverse );
	}
	else
		XUTIL::DrawProgressBar( vPos + m_vAdjBar, lerp, vSize, XCOLOR_GREEN );

}

