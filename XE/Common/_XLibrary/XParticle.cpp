/*
 *  XParticle.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "etc/XSurface.h"
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
															XCOLOR col )
	: m_BlendFunc( XE::xBF_MULTIPLY )
{ 
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
XPointSpriteParticle::XPointSpriteParticle( XParticleDelegate *pDelegate, 
																						int type, 
																						const XE::VEC3& vPos, 
																						XSurface *psfcTexture, 
																						float scale, 
																						XCOLOR col )
	: XBaseParticle( pDelegate, type, vPos, (ID)psfcTexture, scale, col ) {
	Init();
	XBREAK( psfcTexture == NULL );
	m_psfcTexture = psfcTexture;
	m_Size = (psfcTexture->GetTextureSize() * m_Scale).w;
}
XPointSpriteParticle::XPointSpriteParticle( XParticleDelegate *pDelegate,
																						int type,
																						const XE::VEC3& vPos,
																						XSurface *psfcTexture,
																						ID idTexture,
																						float scale,
																						XCOLOR col )
	: XBaseParticle( pDelegate, type, vPos, idTexture, scale, col ) {
	Init();
	XBREAK( psfcTexture == NULL );
	m_psfcTexture = psfcTexture;
	//		m_Size = ( psfcTexture->GetTextureSize() * m_Scale ).w;
	m_Size = psfcTexture->GetTextureSize().w;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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