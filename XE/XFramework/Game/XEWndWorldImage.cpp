#include "stdafx.h"
#include "XEWndWorldImage.h"
#include "XFramework/Game/XEWorld.h"
#include "XFramework/Game/XEWorldCamera.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#include "OpenGL2/XRenderCmd.h"
#include "OpenGL2/XTextureAtlas.h"
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XEWndWorldImage::XEWndWorldImage( XSurface* psfcBG, 
																	std::shared_ptr<XEWorld> spWorld )
	: m_spWorld( spWorld )
// 	, m_pRenderer( new XRenderCmdMng( __FUNCTION__ ) )
// 	, m_pAtlas( new XTextureAtlas( __FUNCTION__ ) )
{
	Init();
//	m_strResBg = resBg;
	m_psfcBg = psfcBG;
	SetSizeLocal( XE::GetGameSize() );
// 	if( !resImg.empty() ) {
//		LoadImg( resImg.c_str() );
// 		m_strResBg = resImg;
// 		m_psfcBg = IMAGE_MNG->Load( FALSE, resImg );
// 	}
}

void XEWndWorldImage::Destroy()
{
	CONSOLE("XEWndWorldImage::Destroy(),m_psfcBg=0x%08x, refCnt=%d", (DWORD)m_psfcBg, m_psfcBg->GetnRefCnt());
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
}

XSurface* XEWndWorldImage::LoadImg( LPCTSTR resImg )
{
// 	SET_ATLASES( m_pAtlas ) {
		XBREAK( XE::IsEmpty( resImg ) );
		SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
//		m_strResBg = resImg;
		m_psfcBg = IMAGE_MNG->Load( false, resImg, XE::xPF_RGB565 );
// 	} END_ATLASES;
	return m_psfcBg;
}

// BOOL XEWndWorldImage::OnCreate()
// {
// 	// 월드 전체 크기를 넘겨주며 월드객체 생성
// 	// 월드크기는 게임좌표계 기준으로 1280x720. 게임좌표계 기준 풀화면 크기는 640x360
// 	// 월드크기는 화면크기의 가로세로2배 크기
// 
// //	XEWorld *pWorld = OnCreateWorld( m_psfcBg->GetSize() );
// //	XBattleField *pWorld = new XBattleField( m_psfcBg->GetSize() );
// // 	XBREAK( pWorld == NULL );
// // 	SetpWorld( pWorld );		// 월드 객체를 붙여줌
// 	//
// // 	auto sizeWorld = GetvwSizeWorld();
// // 	XE::VEC2 vwCamera = sizeWorld * 0.5f;
// // 	XE::VEC2 vwLT = vwCamera - ( GetSizeLocal() / 2.f ) * ( 1 / GetscaleCamera() );
// // 	InitScroll( XE::VEC2( 0 ),
// // 							sizeWorld,
// // 							vwLT,	// 화면좌상귀에 해당하는 월드좌표
// // 							GetSizeLocal(),
// // 							XScroll::xTP_ALL );
// // 
// // 	SetFocus( XScroll::GetvCenter() );
// 	return XEWndWorld::OnCreate();
// }


/**
 
*/
// int XEWndWorldImage::Process( float dt )
// {
// 	return XEWndWorld::Process( dt );
// }
/**
 
*/
void XEWndWorldImage::Draw()
{
	if( m_psfcBg )
	{
		const float scaleBg = m_spCamera->GetscaleCamera();			// 그림의 스케일값
		XE::VEC2 vwCamera = m_spCamera->GetvwCamera();			// 카메라 월드 위치
		XE::VEC2 vsWinCenter = GetSizeLocal() * 0.5f;	// 윈도우의 센터
		XBREAK( vsWinCenter.IsZero() );
		XE::VEC2 vsLT = vsWinCenter + (-vwCamera * scaleBg);		// 그림이 찍혀야할 화면 좌표

// 		SET_RENDERER( m_pRenderer ) {
			m_psfcBg->SetScale( scaleBg );
// 			GRAPHICS->SetPriority( 1000 );
			m_psfcBg->Draw( vsLT );
// 			GRAPHICS->SetPriority( 0 );
// 		} END_RENDERER;
	}
// 	GRAPHICS->SetPriority( 500 );
	XWnd::Draw();
// 	GRAPHICS->SetPriority( 0 );
	// ui는 priority 0에 찍힌다.
}

XE::VEC2 XEWndWorldImage::GetvwSizeWorld() const
{
	XBREAK( m_psfcBg == nullptr );
	return m_psfcBg->GetSize();
}
