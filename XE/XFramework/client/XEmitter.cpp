#include "stdafx.h"
#include "XEmitter.h"
#include "XParticle.h"
#include "Sprite/SprObj.h"
#include "Sprite/sprite.h"
#include "XParticleMng.h"
#include "XPropParticle.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xParticle;
////////////////////////////////////////////////////////////////
/**
 @brief sprObj객체에서 액션은 쓰지않고 스프라이트만 쓴다.
 현재 png,알파읽는방식이 ,sprObj에서만 제대로 동작해서 일단 이렇게 씀.
*/
// XEmitter::XEmitter( const XE::VEC2& vPos, XSprObj* psoSurfaces, int idxSpr )
// {
// 	Init();
// 	m_vPos = vPos;
// 	m_idxSpr = idxSpr + 1;	// id는 0값이 초기화값이므로.
// 	m_timerCycle.Set( 0.1f );
// 	m_numEmit = 1;
// 	m_psoSurfaces = psoSurfaces;
// 	m_psfcParticle = psoSurfaces->GetSprite( idxSpr )->GetpSurface();
// 	m_pCompSpeed = new XCompFuncFixed(1.f);
// 	m_pCompDelta = new XCompFuncFixed(1.f);
// // 	m_pCompScale = new XCompFuncFixed(1.f);
// }
// 
// /**
//  @brief 파티클입자가 각자 텍스쳐 애니메이션이 있는경우.
// */
// XEmitter::XEmitter( const XE::VEC2& vPos, LPCTSTR szSpr )
// {
// 	Init();
// 	m_vPos = vPos;
// 	m_timerCycle.Set( 0.1f );
// 	m_numEmit = 1;
// 	m_strSpr = szSpr;
// 	m_pCompSpeed = new XCompFuncFixed(1.f);
// 	m_pCompDelta = new XCompFuncFixed(1.f);
// //	m_pCompScale = new XCompFuncFixed(1.f);
// }
/**
 @brief 
 @param vPos 이미터 위치
 @param idxArySpr 프로퍼티의 m_aryIdxSpr배열의 인덱스. 어떤 스프라이트를 쓰는 이미터인지 명시해줘야 함.
*/
XEmitter::XEmitter( const XE::VEC2& vPos, XPropParticle::xEmitter* pPropEmitter, int idxArySpr )
{
	Init();
	m_vPos = vPos;
	_m_pProp = pPropEmitter;
	m_idKey = XPropParticle::sGet()->GetidKey();
	for( int i = 0; i < xIC_MAX; ++i ) {
		auto idxComp = (xtIdxComp)i;
		auto pComponent 
			= XParticleMng::sCreateFuncComponent( pPropEmitter->aryFunc[ i ] );
		if( pComponent )
			AddComponent( idxComp, pComponent );
	}
	float secLife = GetValueByComp( xIC_LIFE );
	if( secLife > 0 )
		m_timerLife.Set( secLife );
	else
		m_timerLife.Off();		// 이미터 사라지지 않음.
//	m_numEmit = 1;
	if( idxArySpr >= 0 ) {
		auto psoSurfaces = new XSprObj( pPropEmitter->strSpr );
		m_idxSpr = idxArySpr + 1;	// id는 0값이 초기화값이므로.
		m_psoSurfaces = psoSurfaces;
		m_psfcParticle = psoSurfaces->GetSpriteMutable( idxArySpr )->GetpSurface();
	} else {
		m_strSpr = pPropEmitter->strSpr;
	}
}

void XEmitter::Destroy()
{
	SAFE_DELETE( m_psoSurfaces );
}

// XPropParticle::xEmitter* XEmitter::GetpProp()
// {
// 	if( m_idKey != XPropParticle::sGet()->GetidKey() ) {
// 		_m_pProp = XPropParticle::sGet()->GetpProp( );
// 
// 	}
// }

void XEmitter::FrameMove( XParticleMng *pMng, float dt )
{
	if( m_bDestroy )
		return;
	if( m_timerCycle.IsOff() || m_timerCycle.IsOver() ) {
		int numEmit = (int)GetValueByComp( xIC_NUM_EMIT );
		float secInterval = GetValueByComp( xIC_INTERVAL );
		m_timerCycle.Set( secInterval );
		//
		for( int i = 0; i < numEmit; ++i ) {
			float scale = GetValueByComp( xIC_SCALE );
			XBaseParticle *pBasePtc = nullptr;
			if( m_psoSurfaces ) {
				auto ptc = new XPointSpriteParticle( nullptr,
																						0,
																						m_vPos,
																						m_psfcParticle,
																						(ID)m_idxSpr,
																						scale,
																						XCOLOR_WHITE );
				pBasePtc = ptc;
				pMng->Add( ptc );
			} else
			if( !m_strSpr.empty() ) {
				auto ptc = new XParticleSprite( m_strSpr, 1, m_vPos, scale );
				pBasePtc = ptc;
				pMng->Add( ptc );
			}
			XBREAK( pBasePtc == nullptr );
			for( int i = 0; i < xIC_MAX; ++i ) {
				auto idxComp = (xtIdxComp)i;
				auto pComp = XParticleMng::sCreateFuncComponent( _m_pProp->particle.aryFunc[i] );
				if( pComp )
					pBasePtc->AddComponent( idxComp, pComp );
			}
	//		float secLife = GetValueByComp( xIC_LIFE );
			// 파티클 생성시점.
			{
				auto pComp = pBasePtc->GetComponent( xIC_LIFE );
				if( pComp )
					pBasePtc->SetsecLife( pComp->GetValue() );
			}
			float speed = GetValueByComp( xIC_SPEED );
			float dAng = GetValueByComp( xIC_ANGLE );
			XE::VEC2 v2 = XE::GetAngleVector( dAng, speed );
			XE::VEC3 vDelta( v2.x, 0, v2.y );
			pBasePtc->SetvDelta( vDelta );
			pBasePtc->SetBlendFunc( _m_pProp->blendFunc );
			pBasePtc->FrameMove( 0.f );		// 최초 올바른 초기값을 갖기위해 한번 실행시킴.
			//
	// 		ptc->Add중력처리기();
	// 		ptc->Add랜덤이동기();
	// 		ptc->Add지면충돌기( 지면y좌표, 탄성 )
	// 		pMng->Add( ptc );
		}
	}
	if( m_timerLife.IsOver() )
		SetbDestroy( true );
}

// 파티클입자::FrameMove( float dt )
// {
// 	m_vPos += m_vDelta * dt;
// 	m_vDelta = 중력처리기( m_vDelta );
// 	m_vDelta = 랜덤이동기( m_vDelta );
// 	m_vDelta = 지면충돌기( m_vDelta, 지면y좌표, 탄성도 );
// }