/*
 *  XTransition.cpp
 *  ShakeMP
 *
 *  Created by xuzhu on 10. 7. 2..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "stdafx.h"
#include "etc/XSurface.h"
#include "etc/Timer.h"
#include "etc/xGraphics.h"
#include "XImageMng.h"
#include "XTransition.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XTransition::~XTransition() 
{
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface1 );
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface2 );
}

BOOL XTransition::Process( float dt )
{
	if( m_nDrawCnt == 0 )		return 1;	// Draw를 먼저하게 하기위해 첫번째 프레임은 처리하지 않는다
	// 이전씬의 화면과 새씬의 화면을 트랜지션 하는 경우, 가령 오버랩같은 경우는
	// 이전씬화면이 프론트화면에 떠있을때 트랜지션의 생성자를 불러서 스크린을 캡쳐한후
	// 새씬을 Draw해서 프론트버퍼로 오게하고 다시 화면을 잡는다. 그리고 그 프레임에서 즉시 트랜지션을 덮어씌운다
	if( m_nDrawCnt == 1 )		// draw를 1회 하면 이변수가 1이 된다
	{
#ifdef _VER_OPENGL
//		Create();		// virtual. gl은 새씬을 draw하고 플립을 한다음 프로세스에서 프론트화면을 캡쳐해야하기땜에 여기서 부른다
#endif
		m_Timer.Set( GetfSec() );	// 타이머 시작
	}
	// process
	if( IsComplete() )
		return 0;
	float slerp = m_Timer.GetPassTime() / (float)m_Timer.GetWaitTime();
	if( slerp > 1.0f )
	{
		slerp = 1.0f;
		SetComplete( TRUE );
	}
	SetfSlerp( slerp );
	return 1;
}

XSlideInOut::XSlideInOut( BOOL bHighReso, XSurface *pFrom, int nTransIn, int nSpeed )
: XTransition( bHighReso, 1.0f, nTransIn )
{
	m_nType = nTransIn;
	m_nSpeed = nSpeed;
	if( m_nType == 1 )
		m_nXPos = (int)XE::GetGameWidth();
	else 
		m_nXPos = 0;
	SetpSurface1( pFrom );
	SetpSurface2( GRAPHICS->CreateSurface( bHighReso, (int)XE::GetGameWidth(), (int)XE::GetGameHeight(), 0, 0, NULL ) );
//	Draw();		// 이거 여기다 draw넣으면 안될텐데... 
}

BOOL XSlideInOut::Create( void )
{
	// 스크린에서 픽셀얻어오는건 반드시 Create()에서 해야한다. 오버랩같은건 씬을 한번 그리고 다음턴에서 그화면을 얻어와야 하기때문에 생성자에서 스크린픽셀을 잡으면 안되기 때문이다
	GRAPHICS->ScreenToSurface( GetbHighReso(), GetpSurface2() );
	return 1;
}
void XSlideInOut::Draw( void )
{
	if( IsComplete() )
		return;
	if( m_nType == 1 )
	{
		// m_nXPos를 중심으로 좌측에 이전 화면을 찍는다.
		GetpSurface1()->Draw( (float)m_nXPos - GetpSurface1()->GetWidth(), 0.f );
		// m_nXPos를 중심으로 우측에 새 화면을 찍는다.
		GetpSurface2()->Draw( (float)m_nXPos, 0.f );
		m_nXPos -= m_nSpeed;
		if( m_nXPos <= 0 )				// 화면 왼쪽까지 다 이동했으면 완료
			SetComplete( TRUE );
	} else 
	{
		// m_nXPos를 중심으로 좌측에 새 화면을 찍는다.
		GetpSurface2()->Draw( (float)m_nXPos - GetpSurface1()->GetWidth(), 0.f );
		// m_nXPos를 중심으로 우측에 이전 화면을 찍는다.
		GetpSurface1()->Draw( (float)m_nXPos, 0 );
		m_nXPos += m_nSpeed;
		if( m_nXPos >= XE::GetGameWidth() )				// 화면 왼쪽까지 다 이동했으면 완료
			SetComplete( TRUE );		
	}
	XTransition::Draw();
}
//---------------------------------------------------------------------------------------------------------------
XFadeInOut::XFadeInOut( BOOL bHighReso, BOOL bTransIn, float fSec )
	: XTransition( bHighReso, fSec, bTransIn )
{
	Init();
}

void XFadeInOut::Draw( void )
{
	BYTE alpha = 0;
	if( IsTransIn() )		// fade in- 어두움 -> 밝아짐
		alpha = 255 - (BYTE)(255 * GetfSlerp());
	else					// fade out - 밝음 -> 어두움
		alpha = (BYTE)(255 * GetfSlerp());
	//
	GRAPHICS->FillRect( 0, 0, XE::GetGameWidth(), XE::GetGameHeight(), XCOLOR_RGBA( 0, 0, 0, alpha ) );
	XTransition::Draw();
}
//---------------------------------------------------------------------------------------------------------------
//#ifdef WIN32
//#include "XGraphicsD3D.h"
//#else
//#include "XGraphicsOpenGL.h"
//#endif
// 현재화면에서 szImgTo파일로 오버랩시킴
XOverlap::XOverlap( BOOL bHighReso, LPCTSTR szImgTo, float fSec ) 
	: XTransition( bHighReso, fSec, TRUE )
{ 
	Init(); 
	_tcscpy_s( m_szImgTo, szImgTo );
	SetpSurface1( GRAPHICS->s_pLastBackBuffer );		// from 이전화면을 XGraphics로부터 받아온다
	if( GetpSurface1() == NULL )		// 최초로 띄워지는 씬이 오버랩으로 시작한다면 캡쳐해놓은게 없을거다. 그럼 현재 백버퍼를 그냥 캡쳐함
		SetpSurface1( GRAPHICS->CreateScreenToSurface( GetbHighReso() ) );
	GRAPHICS->s_pLastBackBuffer = NULL;				// 포인터를 옮겨옴
}
// 이전화면과 현재화면을 오버랩시킴
XOverlap::XOverlap( BOOL bHighReso, float fSec ) 
	: XTransition( bHighReso, fSec, TRUE )
{ 
	Init(); 
	SetpSurface1( GRAPHICS->s_pLastBackBuffer );		// from 이전화면을 XGraphics로부터 받아온다
	if( GetpSurface1() == NULL )		// 최초로 띄워지는 씬이 오버랩으로 시작한다면 캡쳐해놓은게 없을거다. 그럼 현재 백버퍼를 그냥 캡쳐함
		SetpSurface1( GRAPHICS->CreateScreenToSurface( GetbHighReso() ) );
	GRAPHICS->s_pLastBackBuffer = NULL;				// 포인터를 옮겨옴
}
/*
 GL은 백버퍼만 읽을수 있고(GLES1.x) d3d는 백/프론트 다읽을수 있지만 프론트 읽는건 에러가 나서 백버퍼만 읽게 했음
백버퍼를 읽어서 오버랩을 해야하므로 씬의 마지막 장면을 캡쳐하려면 
씬이 SetDestroy()상태가 되고 그프레임의 Present()가 되기전에 마지막 장면을 캡쳐해야한다.
	// 플립
	// 백버퍼:이전씬-2 프론트:이전씬-1
	// 파괴예약
	// 백버퍼:이전씬-0 프론트:이전씬-1	-> 최후씬을 백버퍼에 찍음
	// 이전씬의 마지막화면을 캡쳐하려면 이곳밖에 없는데...
	// 플립	-> 최후씬이 화면에 나옴
	// 백버퍼:이전씬-1 프론트:이전씬-0 
	// 새씬생성(아직 캡쳐안함)
	// 백버퍼:새씬+0 프론트:이전씬+0	
	// 바뀔화면을 서피스로 준비한다
*/
// 최초 draw전에 호출되서 새화면이 백버퍼에 그려져 있다고 가정함
BOOL XOverlap::Create( void )
{
	if( XE::IsEmpty( m_szImgTo ) )		// 이미지파일이 없으면 이전화면 프레임버퍼에서 새화면 프레임버퍼로 오버랩되는것이다.
		SetpSurface2( GRAPHICS->CreateScreenToSurface( GetbHighReso() ) );	// to	현재 백버퍼에 새화면이 그려져있어야 한다
	else
		SetpSurface2( IMAGE_MNG->Load( /*GetbHighReso(), */m_szImgTo ) );	
	return 1;
}

void XOverlap::Draw( void )
{
	XTransition::Draw();		// 가장먼저 불러줘야 함
	{
		XBREAK( GetpSurface1() == NULL );
        
		float sx = XE::GetGameWidth() / (float)GRAPHICS->GetPhyScreenWidth();
		float sy = XE::GetGameHeight() / (float)GRAPHICS->GetPhyScreenHeight();
		GetpSurface1()->SetScale( sx, sy );		// 스크린을 캡쳐한 데이타라서 해상도가 다를수도 있으므로 스케일링 함
		GetpSurface1()->Draw( 0, 0 );
		if( GetpSurface2() )	// To이미지가 아직 준비가 안된상태라면 이전씬(from) 이미지를 찍어서 이전씬화면을 유지해야한다
		{
            GetpSurface2()->SetScale( sx, sy );		// 스크린을 캡쳐한 데이타라서 해상도가 다를수도 있으므로 스케일링 함
			GetpSurface2()->SetfAlpha( GetfSlerp() );
			GetpSurface2()->Draw( 0, 0 );
		}
	}
	// debug
	if( 0 )
	{
		GetpSurface1()->SetScale( 0.25f, 0.25f );
		GetpSurface1()->Draw( 0, 160 );
		if( GetpSurface2() )	// To이미지가 아직 준비가 안된상태라면 이전씬(from) 이미지를 찍어서 이전씬화면을 유지해야한다
		{
			GetpSurface2()->SetScale( 0.25f, 0.25f );
			GetpSurface2()->SetfAlpha( 1.0f );
			GetpSurface2()->Draw( 240, 0 );
		}
        
	}
}

