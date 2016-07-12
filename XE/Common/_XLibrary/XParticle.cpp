/*
 *  XParticle.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XParticle.h"
#include "XParticleFramework.h"
#include "Sprite/SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xParticle;
//////////////////////////////////////////////////////////////////////////
XBaseParticle::XBaseParticle( XParticleDelegate *pDelegate, 
															int type, 
															const XE::VEC3& vPos, 
															DWORD idTexture, 
															float scale, 
															XCOLOR col ) { 
	Init(); 
	m_pDelegate = pDelegate;
	m_Type = type;
	m_vPos = vPos;
//		XBREAK( psfcTexture == NULL );
	XBREAK( scale == 0 );
//		m_psfcTexture = psfcTexture;
//		m_idTexture = (DWORD)psfcTexture;		// 일단은 쓸게 없으니까. 서피스포인터를 아이디화해서 쓴다.
	m_idTexture = idTexture;
	m_Scale = scale;
	m_r = XCOLOR_RGB_R(col) / 255.f;
	m_g = XCOLOR_RGB_G(col) / 255.f;
	m_b = XCOLOR_RGB_B(col) / 255.f;
	m_a = XCOLOR_RGB_A(col) / 255.f;
	m_timerLife.Set(1.f);
}

void XBaseParticle::Destroy()
{
	for( auto pElem : m_aryComponents ) {
		SAFE_DELETE( pElem );
	}
}

/**
 @brief 
*/
void XBaseParticle::FrameMove( float dt ) 
{
	auto pCompGravity = m_aryComponents[ xIC_GRAVITY ];
	if( pCompGravity )
		m_vDelta.z += pCompGravity->GetValue() * dt;
	m_vPos += m_vDelta * dt;
	if( m_timerLife.IsOver() ) {
		m_bDestroy = TRUE;
	}
	// 자체 스케일컴포넌트가 있을땐 컴포넌트가 뽑아준값으로 쓴다.
	auto pCompScale = m_aryComponents[ xIC_SCALE ];
	if( pCompScale )
		m_Scale = pCompScale->GetValue();	
//	m_Scale = scale;
	float lerpTime = m_timerLife.GetSlerp();
	if( lerpTime > 1.f )
		lerpTime = 1.f;
// 	float alpha = 1.f;
	auto pCompAlpha = m_aryComponents[ xIC_ALPHA ];
	if( pCompAlpha )
		m_a = pCompAlpha->GetValue( lerpTime );
// 	m_a = alpha;
		
}

// .cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
// 일반적인 눈. 떨어지는 속도나 방향은 디폴트 값으로 정해진다.
XParticleSnow::XParticleSnow( XParticleDelegate *pDelegate, const XE::VEC3& vPos, XSurface *psfcSnow, float scale )
	: XPointSpriteParticle( pDelegate, XE::xPTC_SNOW, vPos, psfcSnow, scale, XCOLOR_RGBA(230,195,211,255) ) 
//	: XPointSpriteParticle( pDelegate, XE::xPTC_SNOW, x, y, psfcSnow, scale, XCOLOR_WHITE ) 
{
	Init();
//	m_vDelta.y = xRandomF( 0.2f, 0.5f );
	m_vDelta.y = xRandomF( 0.5f, 1.0f );
	if( pDelegate )
	{
		XE::VEC2 vGround = pDelegate->OnDelegateGroundPos( this );
		m_yGround = vGround.y;
	}
}
void XParticleSnow::FrameMove( float dt )
{
	if( m_vDelta.y )		// 눈이 움직이고 있을땐
	{
		float yColl=0;
		if( IsCollision( m_vPos.x, m_vPos.y, &yColl ) )		// 충돌검사를 한다.
			OnCollision( m_vPos.x, yColl );
		else
			m_vDelta.x = xRandomF( -0.4f, 0.4f );
		if( m_yGround && GetvPos().y >= m_yGround )
			OnCollision( m_vPos.x, m_vPos.y );
	} else
	{
		if( m_timerLife.IsOver() )	// 바닥에 떨어지고 나서 일정시간이 흐르면
		{
			SetbDestroy( TRUE );	// 눈은 사라진다.
		}
			
	}
	// 델레게이트로 물어봐서 사라져야 하는 상황이면 삭제된다.
	if( GetpDelegate() && GetpDelegate()->OnDelegateIsDestroy( this ) )
		SetbDestroy( TRUE );	
	//
	//
	XBaseParticle::FrameMove( dt );
}

// 바닥에 떨어졌을때 기본동작
void XParticleSnow::OnCollision( float x, float y )
{
	XBREAK( y < 0 );
	m_timerLife.Set( 1.0f );		// 1초동안 녹는다.
	m_vPos.x = x;
	m_vPos.y = y;
	m_vDelta.y = 0;				// 더이상 떨어지지 않는다.
	m_vDelta.x = 0;
}

BOOL XParticleSnow::Draw( float x, float y, xOUT *pOut )
{
	if( m_timerLife.IsOn() )		// 바닥에 떨어졌으면
	{
		m_a = 1.0f - m_timerLife.GetSlerp();		// 녹는시간동안 알파값이 점점 옅어짐
		if( m_a < 0 )
			m_a = 0;
		if( m_a > 1.f )
			m_a = 1.f;
	}
	else
		m_a = 1.0f;
	return XPointSpriteParticle::Draw( x, y, pOut );
}

////////////////////////////////////////////////////////////////////////
XParticleRain::XParticleRain( XParticleDelegate *pDelegate, 
							XParticleMngRain *pRainMng,
							float x, float y, float dx, float dy )
: XBaseParticle( pDelegate, XE::xPTC_RAIN, XE::VEC3( x, y, 0 ), 0x12345678, 1.0f, XCOLOR_WHITE )
{
	XBREAK( pRainMng == NULL );
	m_pRainMng = pRainMng;
	m_vDelta.Set( dx, dy, 0 );
	XE::VEC2 vDelta( dx, dy );
	vDelta.Normalize();
	m_vPrev = XE::VEC2( x, 0 ) - vDelta * 100.f;
	m_vPrevDelta = XE::VEC2( dx, dy );
}

void XParticleRain::FrameMove( float dt )
{
	float yColl = 0;
	// 빗줄기 앞부분의 충돌검사
	if( IsCollision( m_vPos.x, m_vPos.y, &yColl ) )
	{
		m_vDelta.Set( 0 );
		m_vPos.y = yColl;		// 빗줄기의 앞부분은 충돌한 좌표에 멈춤.
	}
	// 빗줄기 뒷부분의 충돌검사.
	if( IsCollision( m_vPrev.x, m_vPrev.y, &yColl ) )
	{
		m_vPrevDelta.Set( 0 );
		m_vPrev.y = yColl;
		SetbDestroy( TRUE );		//  빗줄기 뒷부분까지 땅에 내려왔으면 삭제
	}
	if( IsDestroy() )
		return;
	m_vPrev += m_vPrevDelta * dt;
	m_vPos += m_vDelta * dt;
}

BOOL XParticleRain::Draw( float x, float y, XGraphics::xVERTEX *pOut ) 
{
	float x2, y2;
	Projection( m_vPrev, &x2, &y2 );
	pOut->x = x;
	pOut->y = y;
	pOut->r = 1.0f;
	pOut->g = 1.0f;
	pOut->b = 1.0f;
	pOut->a = 0.25f;
	++pOut;
	pOut->x = x2;
	pOut->y = y2;
	pOut->r = 1.0f;
	pOut->g = 1.0f;
	pOut->b = 1.0f;
	pOut->a = 0;
	return TRUE;
}

BOOL XParticleRain::IsCollision( float x, float y, float *pOutyColl )
{ 
	if( GetpDelegate() )
		return GetpDelegate()->OnDelegateIsCollision( this, x, y, pOutyColl );
	if( m_pRainMng )
		return ( y >= m_pRainMng->GetvFogLT().y + m_pRainMng->GetvFogSize().h )? TRUE : FALSE;
	return ( y >= XE::GetGameHeight() )? TRUE : FALSE;  
}
#endif // 0
////////////////////////////////////////////////////////////////
XParticleSprite::XParticleSprite( LPCTSTR szSpr, ID idAct, const XE::VEC3& vPos, float scale )
	: XBaseParticle( nullptr, 0, vPos, 0, scale, XCOLOR_WHITE )
{
	Init();
	m_pSprObj = new XSprObj( szSpr );
	m_pSprObj->SetAction( idAct );
	m_pSprObj->SetScale( scale );
}

void XParticleSprite::Destroy()
{
	SAFE_DELETE( m_pSprObj );
}

void XParticleSprite::FrameMove( float dt )
{
	XBaseParticle::FrameMove( dt );
	m_pSprObj->FrameMove( dt );
}


//spr파티클은 생존시간외에도 애니메이션 루프가 끝나면 자동 소멸되는 옵션도 필요함.

/**
 @brief 매니저에서 3d좌표를 2d좌표로 프로젝션해서 호출한다.
*/
void XParticleSprite::Draw( const XE::VEC2& vScr, float scaleParent )
{

	auto vScale = m_Scale * scaleParent;
	m_pSprObj->SetScale( vScale );
	m_pSprObj->Draw( vScr );
}