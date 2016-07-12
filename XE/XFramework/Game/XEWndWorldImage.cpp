#include "stdafx.h"
#include "XEWndWorldImage.h"
#include "XFramework/Game/XEWorld.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
//#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XEWndWorldImage::XEWndWorldImage( LPCTSTR resImg )
{
	Init();
	if( XE::IsHave(resImg) ) {
		m_strResBg = resImg;
		m_psfcBg = IMAGE_MNG->Load( FALSE, resImg );
	}
}

void XEWndWorldImage::Destroy()
{
	CONSOLE("XEWndWorldImage::Destroy(),m_psfcBg=0x%08x, refCnt=%d", (DWORD)m_psfcBg, m_psfcBg->GetnRefCnt());
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
}

XSurface* XEWndWorldImage::LoadImg( LPCTSTR resImg )
{
	XBREAK( XE::IsEmpty(resImg) );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
	m_strResBg = resImg;
	m_psfcBg = IMAGE_MNG->Load( false, resImg, XE::xPF_RGB565 );
	return m_psfcBg;
}

BOOL XEWndWorldImage::OnCreate( void )
{
	// 월드 전체 크기를 넘겨주며 월드객체 생성
	// 월드크기는 게임좌표계 기준으로 1280x720. 게임좌표계 기준 풀화면 크기는 640x360
	// 월드크기는 화면크기의 가로세로2배 크기

	XEWorld *pWorld = OnCreateWorld( m_psfcBg->GetSize() );
//	XBattleField *pWorld = new XBattleField( m_psfcBg->GetSize() );
	XBREAK( pWorld == NULL );
	SetpWorld( pWorld );		// 월드 객체를 붙여줌
	//
	XE::VEC2 vwCamera = pWorld->GetvwSize() / 2.f;
	XE::VEC2 vwLT = vwCamera - ( GetSizeLocal() / 2.f ) * ( 1 / GetscaleCamera() );
	InitScroll( XE::VEC2( 0 ),
				pWorld->GetvwSize(),
				vwLT,	// 화면좌상귀에 해당하는 월드좌표
				GetSizeLocal(),
				XScroll::xTP_ALL );

//	SetvwCamera( XScroll::GetvCenter() );
	SetFocus( XScroll::GetvCenter() );
	// 최초는 50%크기로 줌아웃 시작
	// 월드좌표 75% 지점
	//	SetvwCamera( pWorld->GetvwSize() / 2.f );	// 카메라 위치
	// 	SetvwCamera( ( pWorld->GetvwSize() / 2.f ) + ( pWorld->GetvwSize() / 4.f ) );	// 카메라 위치
	// 	SetscaleCamera( 0.25f );

	return TRUE;
}


/**
 
*/
int XEWndWorldImage::Process( float dt )
{
	return XEWndWorld::Process( dt );
}
/**
 
*/
void XEWndWorldImage::Draw( void )
{
	if( m_psfcBg )
	{
		float scaleBg = GetscaleCamera();			// 그림의 스케일값
		XE::VEC2 vwCamera = GetvwCamera();			// 카메라 월드 위치
		XE::VEC2 vsWinCenter = GetSizeLocal() / 2.f;	// 윈도우의 센터
		XE::VEC2 vsLT = vsWinCenter + (-vwCamera * scaleBg);		// 그림이 찍혀야할 화면 좌표

		m_psfcBg->SetScale( scaleBg );
		m_psfcBg->Draw( vsLT );
	}
	XEWndWorld::Draw();
}

