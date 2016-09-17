﻿#include "stdafx.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XBaseUnit.h"
#include "skill/XSkillUser.h"
#include "XEObjMngWithType.h"
#include "XLegionObj.h"
#include "Sprite/SprObj.h"
#include "XSkillMng.h"
#include "XMsgUnit.h"
#include "XComp.h"
#include "XFramework/XEProfile.h"
#include "XFramework/Game/XEWndWorld.h"
#if defined(_CHEAT) && defined(WIN32)
#include "client/XAppMain.h"
#endif


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace XSKILL;

#ifdef _XMEM_POOL
template<> XPool<XObjArrow>* XMemPool<XObjArrow>::s_pPool = NULL;
#endif



//////////////////////////////////////////////////////////////////////////
XObjBullet::XObjBullet( ID idBullet, 
						const XSPUnit& spOwner, 
						const XSPUnit spTarget,
						const XE::VEC3& vwSrc,
						const XE::VEC3& vwDst,
						float damage,
						bool bCritical,
						LPCTSTR szSpr, ID idAct,
						float secFly )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwSrc, szSpr, idAct )
{
	Init();
	m_idBullet = idBullet;
	m_secLife = secFly;
	m_vDst = vwDst;
	m_vSrc = vwSrc;
	m_timerLife.Set( secFly );
	m_spOwner = spOwner;
	m_spTarget = spTarget;
	m_Damage = damage;
	m_bCritical = bCritical;
//	XBREAK( GetpSprObj() == nullptr );
	
	// 목표쪽으로의 각도
// 	XE::VEC2 vs = GetpWorld()->GetPosWorldToWindow( vwSrc, nullptr );
// 	XE::VEC2 vd = GetpWorld()->GetPosWorldToWindow( vwDst, nullptr );
// 	float dAng = XE::CalcAngle( vs, vd );
// 	XBREAK( GetpSprObj() == nullptr );
// 	GetpSprObj()->SetRotateZ( dAng );
}

void XObjBullet::Destroy() 
{
}

void XObjBullet::FrameMove( float dt )
{
	float timeLerp = m_timerLife.GetSlerp();
	if( timeLerp > 1.0f )
		timeLerp = 1.0f;
//	m_vDst = m_spTarget->GetvCenterWorld();
	XE::VEC3 vDst = (m_spTarget != nullptr)? m_spTarget->GetvwPos() : m_vDst;
	vDst += m_vOffset;
	XE::VEC3 vwPos = OnInterpolation( m_vSrc, vDst, timeLerp );
	XE::VEC2 vCurr = GetpWndWorld()->GetPosWorldToWindow( vwPos, nullptr );
	XE::VEC2 vPrev = GetpWndWorld()->GetPosWorldToWindow( GetvwPos(), nullptr );
	SetvwPos( vwPos );
	float dAng = XE::CalcAngle( vPrev, vCurr );
	GetpSprObj()->SetRotateZ( dAng );
	if( timeLerp >= 1.0f )	{
		if( m_sprArrive.empty() == false )		{
			auto pSfx = new XObjLoop( vDst, m_sprArrive.c_str(), m_idActArrive );
			XBattleField::sGet()->AddObj( XSPWorldObj(pSfx) );
			// 도착시 sfx가 지정되어있으면 타겟에게 생성하도록 한다.
// 			if( m_spTarget != nullptr &&
// 				m_spTarget->IsLive() )
// 				m_spTarget->CreateHitSfx( m_spOwner.get(), FALSE );
		}
		XBREAK( IsDestroy() );
		OnArriveBullet( 0 );
		SetDestroy( 1 );
	}

	//
	XEBaseWorldObj::FrameMove( dt );
}

int XObjBullet::AddInvokeSkill( const _tstring& strInvokeSkill ) 
{
	if( !m_aryInvokeSkill.IsExist( strInvokeSkill ) ) {
		m_aryInvokeSkill.Add( strInvokeSkill );
	}
	return m_aryInvokeSkill.size();
}

/**
	@brief vSrc에서 vDst로 날아갈때 보간계산을 하위클래스에 맡길수 있다.
*/
XE::VEC3 XObjBullet::OnInterpolation( const XE::VEC3& vSrc, const XE::VEC3& vDst, float lerpTime )
{
//	float lerp = XE::xiHigherPowerDeAccel( lerpTime, 1.f, 0 );
	float lerp = XE::xiLinearLerp( lerpTime );
	XE::VEC3 vDist = vDst - vSrc;
	return m_vSrc + vDist * lerp;
}

void XObjBullet::OnArriveBullet( DWORD dwParam )
{
	// 발사체에 발동스킬이 있으면 발동시킨다.
	for( auto& strInvokeSkill : m_aryInvokeSkill ) {
#ifdef _XSINGLE
//		XLOGXN( "스킬발동: %s", strInvokeSkill.c_str() );
#endif // _XSINGLE
		if( m_spTarget ) {
			// 발동스킬을 액티브로 실행시킨다.
			const XE::VEC2 vZero;
			auto infoUseSkill = m_spOwner->UseSkillByIds( strInvokeSkill.c_str(),
																										0,
																										m_spTarget.get(), vZero );
			XASSERT( infoUseSkill.errCode == XSKILL::xOK );
			ID idCaller = 0;
			auto pSkillDat = SKILL_MNG->FindByIds( strInvokeSkill );
			if( pSkillDat ) {
				idCaller = pSkillDat->GetidSkill();
			}
			m_spOwner->OnShootSkill( infoUseSkill, idCaller );
		}
	}
	// 어태커가 지정되어있을경우 도착 델리게이트를 날려준다.
	if( GetpDelegate() ) {
		GetpDelegate()->OnArriveBullet( this,
																		m_spOwner,
																		m_spTarget,
																		m_vDst,
																		m_Damage,
																		m_bCritical,
																		m_sprArrive.c_str(), m_idActArrive,
																		dwParam );
	}
}

#ifdef _XPROFILE
void XObjBullet::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ )
{
	XPROF_OBJ_AUTO();
	XEBaseWorldObj::Draw( vPos, scale, alpha );
}
#endif // _XPROFILE

////////////////////////////////////////////////////////////////
/*
DPS = 초당이동하는거리
DPS = 거리 / 총시간
총시간(x) = 거리 / DPS
초당이동픽셀 = pixelPerFrame(프레임당이동픽셀) * 60;
*/
XObjArrow::XObjArrow( XEWndWorld *pWndWorld,
											const XSPUnit& spOwner,
											const XSPUnit& spTarget,
											const XE::VEC3& vwSrc,
											const XE::VEC3& vwDst,
											float damage,
											bool bCritical,
											LPCTSTR szSpr, ID idAct,
											float pixelPerFrame )	// 프레임당 이동픽셀
	: XObjBullet( 1, spOwner, spTarget, vwSrc, vwDst, damage, bCritical, szSpr, idAct )
{
	Init();
	XE::VEC3 vDist = vwDst - vwSrc;
	vDist.z = 0;
 	const float distsq = vDist.Length();
 	float secFly = distsq / ((pixelPerFrame * XFPS));
	SetSecFly( secFly );
	XE::VEC3 size = spTarget->GetSize() * 0.8f;		// BB가 보통, 실제 이미지보다 살짝 크므로 자연스럽게 보이기 위해 0.8을 곱함.
	size *= XWndBattleField::sGet()->GetscaleCamera();
	m_vOffset.z = -((float)( random((int)( size.h )) ));
	m_vOffset.x = (size.w / 2.f) - (float)random((int)( size.w ));
	// 맞는타겟이 대형일경우만 화살타격이펙을 뿌려준다.
	if( spTarget != nullptr /*&& spTarget->IsBig()*/ )
		SetArriveSfx( _T("eff_hit02.spr"), 1 );

}

void XObjArrow::Destroy()
{
}

XE::VEC3 XObjArrow::OnInterpolation( const XE::VEC3& vSrc,
																		 const XE::VEC3& vDst,
																		 float lerpTime )
{
	if( m_MoveType == xMT_STRAIGHT ) {
		// 직선
		XE::VEC3 vCurr = vSrc + (vDst - vSrc) * lerpTime;
		return vCurr;
	} else {
		// 포물선
		XE::VEC3 vDir = vDst - vSrc;
		XE::VEC2 v2Src( -vDir.x, -vDir.y );	// XY평면의 목표쪽벡터
		XE::VEC2 v2Cross = v2Src.Cross();					// v2Src의 수평직각벡터
		XE::VEC3 vHoriz = v2Cross;
		vHoriz.z = 0;
		XE::VEC3 vCross = vDir.Cross( vHoriz );
		vCross.Normalize();
		// 거리에따라 휘어지능 정도가 다르게 함.
		float multiply = ((vDir.Lengthsq() + random( 50 * 50 )) / (450.f * 450.f)) * 500.f;
		vCross *= multiply;		// 직각벡터를 길게 만든다.
		XE::VEC3 vCurr;
		Vec3CatmullRom( vCurr, vSrc + vCross, vSrc, vDst, vDst + vCross, lerpTime );
		return vCurr;
	}
}

void XObjArrow::OnArriveBullet( DWORD dwParam )
{
	CallCallbackFunc();
	if( random(5) == 0 ) {
		XE::VEC3 vwDst = GetvDst();
		vwDst.x += 24 - random( 48 );
		vwDst.y += 24 - random( 48 );
		XObjLoop *pStuck = new XObjLoop( vwDst, _T("arrow_stuck.spr"), 1 );
		XBattleField::sGet()->AddObj( XSPWorldObj(pStuck) );
	}
	XObjBullet::OnArriveBullet( dwParam );
}


void XObjArrow::FrameMove( float dt )
{
	XObjBullet::FrameMove( dt );
}
void XObjArrow::Draw( const XE::VEC2& vPos, float scale/* = 1.f*/, float alpha )
{
	XPROF_OBJ_AUTO();
	XObjBullet::Draw( vPos, scale, alpha );
}

////////////////////////////////////////////////////////////////
XObjRock::XObjRock( XEWndWorld *pWndWorld,
					const XSPUnit& spOwner,
					const XSPUnit spTarget,
					const XE::VEC3& vwSrc,
					const XE::VEC3& vwDst,
					float damage,
					bool bCritical,
					LPCTSTR szSpr, ID idAct )
	: XObjBullet( 1, spOwner, spTarget, vwSrc, vwDst, damage, bCritical, szSpr, idAct )
{
	Init();
	XE::VEC3 vDist = vwDst - vwSrc;
	vDist.z = 0;
// 	float distsq = vDist.Lengthsq();
// 	float secFly = distsq / ( ( 7.f * XFPS ) * ( 7.f * XFPS ) );
	float distsq = vDist.Length();
	float secFly = distsq / ( ( 10.f * XFPS ) );
	SetSecFly( secFly );
	m_psfcShadow = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_IMG, _T( "shadow.png" ) ) );
	SetScaleObj( 2.0f );
	SetArriveSfx( _T("eff_ravfire_rava.spr"), 1 );
}

void XObjRock::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcShadow );
}

XE::VEC3 XObjRock::OnInterpolation( const XE::VEC3& vwSrc, 
									const XE::VEC3& vwDst, 
									float lerpTime )
{
//	float lerp = XE::xiGravity( lerpTime, 1.f, 1.f );
	float lerp = lerpTime;		// 일단 대충해놓고 중력가속도는 나중에 구현.

	// XY 
	XE::VEC2 vSrcXY = vwSrc.ToVec2();
	XE::VEC2 vDstXY = vwDst.ToVec2();
	XE::VEC2 vDirXY = vDstXY - vSrcXY;
	float y = ( vSrcXY + vDirXY * lerp ).y;
	// XZ평면에서의 현재 lerpTime의 포물선 위치
// 	float multiply = ( ( vDirXY.Lengthsq() + random( 50 * 50 ) ) / ( 450.f * 450.f ) ) * 500.f;
	XE::VEC2 vSrcXZ = vwSrc.ToVec2XZ();
	XE::VEC2 vDstXZ = vwDst.ToVec2XZ();
	XE::VEC2 vDirXZ = vDstXZ - vSrcXZ;
	XE::VEC2 vCrossXZ = vDirXZ.Cross();
	if( vwSrc.x < vwDst.x)
		vCrossXZ = -vCrossXZ;
//	XE::VEC2 vCrossXZ = -(vDirXZ.Cross());
	XE::VEC2 vCurrXZ;
	Vec2CatmullRom( vCurrXZ, vSrcXZ + vCrossXZ, vSrcXZ, vDstXZ, vDstXZ + vCrossXZ, lerp );
	XE::VEC3 vCurr( vCurrXZ.x, y, vCurrXZ.y );
	return vCurr;
}


void XObjRock::FrameMove( float dt )
{
	XObjBullet::FrameMove( dt );
}

void XObjRock::OnArriveBullet( DWORD _dwParam )
{
	DWORD dwParam = 0;
	XObjBullet::OnArriveBullet( dwParam );
	if( m_maxElastic > 0 )
	{
		// 탄성으로 튀기는 거리는 날아온거리의 1/3을 더 날아간다.
		XE::VEC2 v2Dst = (GetvDst().ToVec2() - GetvSrc().ToVec2());
		XE::VEC3 vwDst;
		if( m_cntElastic == 0 )
			vwDst = GetvwPos() + XE::VEC3( v2Dst.x, v2Dst.y ) * 0.33f;
		else
			vwDst = GetvwPos() + XE::VEC3( v2Dst.x, v2Dst.y ) * 0.75f;
		vwDst.z = 0;
		auto pRock = new XObjRock( GetpWndWorld(),
															 GetspOwner(),
															 XSPUnit(),
															 GetvwPos(),
															 vwDst,
															 m_AddDamage,
															 false,
															 GetstrSpr().c_str(),
															 GetpSprObjConst()->GetActionID() );
		++m_cntElastic;
		pRock->SetpDelegate( GetpDelegate() );	
		pRock->SetfactorSpline( 3.f );
		pRock->SetElastic( m_AddDamage / 2.f, m_maxElastic - 1, m_cntElastic );
		pRock->SetSplash( GetmeterRadius(), GetratioDamageSplash() );
		GetpWndWorld()->AddObj( XSPWorldObj(pRock) );
	}
}

void XObjRock::Draw( const XE::VEC2& vPos, float scale/* = 1.f*/, float alpha )
{
	XPROF_OBJ_AUTO();
	if( m_psfcShadow )
	{
		XE::VEC3 vGround = GetvwPos();
		vGround.z = 0;
		XE::VEC2 vShadow = GetpWndWorld()->GetPosWorldToWindow( vGround, nullptr );
//		m_psfcShadow->SetfAlpha( 0.6f );
		m_psfcShadow->Draw( vShadow );
	}
	XObjBullet::Draw( vPos, scale, alpha );
}

////////////////////////////////////////////////////////////////
XObjLaser::XObjLaser( LPCTSTR szSpr, const XE::VEC3& vwStart, const XE::VEC3& vwEnd )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwStart, szSpr, 1 )
{
	Init();
	///< 
	float scale = 1.f;
	m_vwStart = vwStart;
	m_vwEnd = vwEnd;
	m_timerLife.Set( 0.6f );
//	m_timerLaser.Set( 0.1f );
}

void XObjLaser::Destroy()
{
}

void XObjLaser::Release( void )
{

}

void XObjLaser::FrameMove( float dt )
{
	if( m_timerLife.IsOver() )
	{
		SetDestroy(1);
		return;
	}
	if( m_timerLaser.IsOver() || m_timerLaser.IsOff() )
	{
		if( m_timerLaser.IsOff() )
			m_timerLaser.Set( 0.1f );
		else
		{
			m_vDelta.x = 2.0f - xRandomF( 4.f );
			m_vDelta.y = 2.0f - xRandomF( 4.f );
		}
		XE::VEC3 vDst = m_vwEnd;
		vDst.y += 0.1f;
		XObjLoop *pEff = new XObjLoop( vDst, _T( "eff_flame.spr" ), 2, 0.f );
		if( m_vwStart.x < m_vwEnd.x )
			pEff->SetRotateY( 180.f );
		GetpWndWorld()->AddObj( XSPWorldObj( pEff ) );
		m_timerLaser.Reset();
	}
	m_vwEnd += m_vDelta * dt;
	
	XEBaseWorldObj::FrameMove( dt );
}

void XObjLaser::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 vStart = GetpWndWorld()->GetPosWorldToWindow( m_vwStart, nullptr );
	XE::VEC2 vEnd = GetpWndWorld()->GetPosWorldToWindow( m_vwEnd, nullptr );
	float dAng = XE::CalcAngle( vStart, vEnd );
	GetpSprObj()->SetRotate( dAng );
	XE::VEC3 vDist = vEnd - vStart;
	float dist = vDist.Length();
	XE::VEC2 vScale(1.f, 0.5f);	// 32x짜리 레이저를 줄여서 사용
	vScale.x *= dist / (32.f * scale);	// 레이저 리소스 게임좌표사이즈 32
	vScale *= scale;
	GetpSprObj()->SetScale( GetScaleObj().ToVec2XZ() * vScale );
	GetpSprObj()->Draw( vPos );
	
}

////////////////////////////////////////////////////////////////
/**
 @brief 
 @param secLife 0은 한번만 플레이하고 중지. -1은 무한루프,>0 값은 해당 초동안 생존
*/
XObjLoop::XObjLoop( const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife/*=0.f*/ )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwPos, szSpr, idAct )
{
	Init();
	m_timerLife.Set( secLife );
	m_secLife = secLife;
	if( secLife == 0.f )
		GetpSprObj()->SetAction( idAct, xRPT_1PLAY );
	XBREAK( GetpSprObj() == nullptr );
}

XObjLoop::XObjLoop( int typeObj, const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife/*=0.f*/ )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), typeObj, vwPos, szSpr, 1 )
{
	Init();
	m_timerLife.Set( secLife );
	m_secLife = secLife;
	if( secLife == 0.f )
		GetpSprObj()->SetAction( idAct, xRPT_1PLAY );
	XBREAK( GetpSprObj() == nullptr );
}

XObjLoop::XObjLoop( int typeObj, XSPWorldObjConst spTrace, LPCTSTR szSpr, ID idAct, float secLife/*=0.f*/ )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), typeObj, spTrace->GetvwPos(), szSpr, idAct )
{
	Init();
	m_timerLife.Set( secLife );
	m_secLife = secLife;
	m_spTraceObj = spTrace;
	if( spTrace->GetType() == xOT_UNIT )
		m_spTraceUnit2 = std::static_pointer_cast<const XBaseUnit>( spTrace );
	m_bTraceRefObj = TRUE;
	if( secLife == 0.f )
		GetpSprObj()->SetAction( idAct, xRPT_1PLAY );
	XBREAK( GetpSprObj() == nullptr );
}

/**
 @brief 이펙트 생성은 vwPos에 생성되나 spTrace를 참조해야 하는 버전
*/
XObjLoop::XObjLoop( int typeObj, XSPWorldObjConst spTrace, const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife/*=0.f*/ )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), typeObj, vwPos, szSpr, idAct )
{
	Init();
	m_timerLife.Set( secLife );
	m_secLife = secLife;
	m_spTraceObj = spTrace;
	if( spTrace->GetType() == xOT_UNIT )
		m_spTraceUnit2 = std::static_pointer_cast<const XBaseUnit>( spTrace );
	if( secLife == 0.f )
		GetpSprObj()->SetAction( idAct, xRPT_1PLAY );
	XBREAK( GetpSprObj() == nullptr );
}


void XObjLoop::Destroy() {}

void XObjLoop::SetBounce( float power, float dAngZ, float gravity )
{
	auto spCompBounce = std::make_shared<XCompObjMoveBounce>( power, dAngZ );
	spCompBounce->SetGravity( gravity );
	m_spCompMove = spCompBounce;
}

void XObjLoop::FrameMove( float dt )
{
	if( m_spTraceObj ) {
		if( m_bTraceRefObj ) {
			SetvwPos( m_spTraceObj->GetvwPos() + m_vAdjust );
			if( m_spTraceUnit2 && m_spTraceUnit2->IsDead() )
				SetDestroy( 1 );
		}
	}
	if( m_spCompMove ) {
		m_spCompMove->SetvwPos( GetvwPos() );
		m_spCompMove->FrameMove( dt );
		SetvwPos( m_spCompMove->GetvwPos() );
	}
	if( m_State == 0 ) {
		if( m_secLife > 0.f ) {
			// 일정시간 루프
			if( m_timerLife.IsOver() ) {
				m_State = 1;
				m_timerLife.Set( 1.f );
//				SetDestroy(1);
			}
		} else
		if( m_secLife == 0.f ) {
			if( GetpSprObj()->IsFinish() )
				SetDestroy(1);
		}
	} else {
		float alpha = 1.f - m_timerLife.GetSlerp();
		SetAlpha( alpha );
		if( m_timerLife.IsOver() )
			SetDestroy( 1 );
	}
	if( m_Dir == XE::HDIR_RIGHT )
		SetRotateY( 180.f );
	else
	if( m_Dir == XE::HDIR_LEFT )
		SetRotateY( 0 );
	XEBaseWorldObj::FrameMove( dt );
}

/**
 @brief sfx 애니메이션에 타점이 있다면 호출된다.
*/
void XObjLoop::OnEventSprObj( XSprObj *pSprObj,
															XKeyEvent *pKey,
															float lx, float ly,
															ID idEvent,
															float fAngle,
															float fOverSec )
{
	XSkillSfx::CallCallbackFunc();
}

void XObjLoop::Draw( const XE::VEC2& vPos, float scale, float alpha )
{
	XPROF_OBJ_AUTO();
#if defined(_CHEAT) && defined(WIN32)
	if( !(XAPP->m_dwFilter & xBIT_NO_DRAW_SKILL_SFX) )
#endif // defined(_CHEAT) && defined(WIN32)
	{
		// 이펙트류는 z버퍼와 알파테스트를 사용하지 않는다.
// 		auto bAlphaTest = GRAPHICS->GetbAlphaTest();
// 		auto bZBuffer = GRAPHICS->GetbEnableZBuff();
//  		GRAPHICS->SetbAlphaTest( false );
//  		GRAPHICS->SetbEnableZBuff( false );
		XEBaseWorldObj::Draw( vPos, scale, alpha );
//  		GRAPHICS->SetbEnableZBuff( bZBuffer );
//  		GRAPHICS->SetbAlphaTest( bAlphaTest );
	}
}
////////////////////////////////////////////////////////////////
XSkillSfxReceiver::XSkillSfxReceiver( BIT bitCamp, 
																			const XE::VEC3& vwPos, 
																			const _tstring& strSpr, 
																			ID idAct, 
																			float sec )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(),
										XGAME::xOT_SKILL_EFFECT, vwPos,
										strSpr.c_str(), idAct )
	, XSkillReceiver( 1, XGAME::xMAX_PARAM, 0 )
{
	Init();
	m_Camp.SetbitCamp( bitCamp );
	m_timerLife.Set( sec );
}

void XSkillSfxReceiver::Destroy()
{
}

void XSkillSfxReceiver::FrameMove( float dt )
{
	if( m_timerLife.IsOver() )	{
		SetDestroy(1);
		return;
	}
	XEBaseWorldObj::FrameMove( dt );
}

#ifdef _XPROFILE
void XSkillSfxReceiver::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ )
{
	XPROF_OBJ_AUTO();
	XEBaseWorldObj::Draw(vPos,scale, alpha);
}
#endif // _XPROFILE

////////////////////////////////////////////////////////////////
XSkillShootObj::XSkillShootObj( XEWndWorld *pWndWorld,
																const XSPUnit& spOwner,
																const XSPUnit& spTarget,
																const XE::VEC3& vwSrc,
																const XE::VEC3& vwDst,
																float damage,
																LPCTSTR szSpr, ID idAct,
																float factorSpeed )
: XObjArrow( pWndWorld, spOwner, spTarget, vwSrc, 
			vwDst, damage, false, szSpr, idAct, factorSpeed ) 
{
	Init();

}
void XSkillShootObj::Destroy()
{
}

void XSkillShootObj::CallCallbackFunc( void ) 
{
	if( m_Callback.funcCallback )
		m_Callback.funcCallback( m_Callback.pOwner, this );
}

#ifdef _XPROFILE
void XSkillShootObj::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ )
{
	XPROF_OBJ_AUTO();
	XObjArrow::Draw( vPos, scale, alpha );
}
#endif // _XPROFILE

////////////////////////////////////////////////////////////////
_tstring XObjDmgNum::s_strFont = _T("damage.ttf");
int XObjDmgNum::s_numObj = 0;
/**
 @brief 
 @param paramHit typeHit가 xHT_STATE(상태이상)일경우 어떤상태이상인지 파라메터가 붙는다.
*/
XObjDmgNum::XObjDmgNum( float num
											, XGAME::xtHit typeHit
											, int paramHit
											, const XE::VEC3& vwPos
											, XCOLOR col )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwPos )
{
	Init();
	m_Number = (int)num;
	m_Col = col;
	m_typeHit = typeHit;
	if( typeHit == xHT_CRITICAL )
		m_strNumber = XTEXT(2293);
	else
	if( typeHit == xHT_EVADE )
		m_strNumber = XTEXT(2292);		// 회피
	else
	if( typeHit == xHT_MISS )
		m_strNumber = XTEXT(2291);
	else
	if( typeHit == xHT_STATE ) {
		m_strNumber = XGAME::GetStrState( (XGAME::xtState)paramHit );
	} else
	if( typeHit == xHT_IMMUNE )
		m_strNumber = XTEXT(2290);
// 	if( typeHit == xHT_STUN )
// 		m_strNumber = _T("기절");
	else
	if( typeHit == xHT_VORPAL )
		m_strNumber = XTEXT(2289);		// 즉사
	else {
		if( num == 999999.f )
			m_strNumber = XTEXT(2289);	// 즉사
		else
			m_strNumber = XE::Format(_T("%d"), (int)num );
	}
	if( typeHit == xHT_HIT ) {
		SetBounce( XE::VEC2( 10, 15 ), XE::VEC2( 225, 315 ) );
	} else {
		m_spCompMove = std::make_shared<XCompObjMoveNormal>( XE::VEC3( 0, 0, -4.f) );
	}
	InitEffect();
}

XObjDmgNum::XObjDmgNum( LPCTSTR szStr
											, const XE::VEC3& vwPos, XCOLOR col )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwPos )
{
	Init();
	m_strNumber = szStr;
	m_Number = 0;
	m_Col = col;
	m_typeHit = xHT_CUSTOM;
//	m_compBounce.SetGravity( 1.f );
	//SetBounce( XE::VEC2( 10, 15 ), XE::VEC2( 225, 315 ) );
	m_spCompMove = std::make_shared<XCompObjMoveNormal>( XE::VEC3( 0, 0, -4.f) );
	InitEffect();
}

void XObjDmgNum::InitEffect()
{
	m_vDelta.Set( 0, 0, -4 );
	float size = 18.f;
	_tstring strFont;
	if( XE::IsAsciiStr(m_strNumber.c_str()) ) {
//		strFont = _T("damage.ttf");
		strFont = s_strFont;
		size = 20.f;
	} else
		strFont = FONT_NANUM_BOLD;
	XBREAK( m_pfdNumber != nullptr );
	m_pfdNumber = FONTMNG->Load( strFont.c_str(), size );
	m_State = 0;
	if( m_typeHit != xHT_HIT )	// 숫자가 아닌 글자형태면 속도 더빠르게
		m_vDelta.z *= 2.f;
}

void XObjDmgNum::SetBounce( float power, float dAngZ, float gravity )
{
	auto spCompBounce = std::make_shared<XCompObjMoveBounce>( power, dAngZ );
	spCompBounce->SetGravity( gravity );
	m_spCompMove = spCompBounce;
}

void XObjDmgNum::Destroy()
{
	SAFE_RELEASE2( FONTMNG, m_pfdNumber );
}

void XObjDmgNum::FrameMove( float dt )
{
	XBREAK( m_spCompMove == nullptr );
	m_spCompMove->SetvwPos( GetvwPos() );
	if( m_spCompMove->FrameMove( dt ) == 1 ) {
	}
	SetvwPos( m_spCompMove->GetvwPos() );
	if( m_spCompMove->IsDisappear() ) {
		float alpha = 1.0f - m_spCompMove->GetLerpTime();
		SetAlpha( alpha );
		XBREAK( m_spCompMove->GettimerState().IsOff() );
		if( m_spCompMove->GettimerState().IsOver() ) {
			SetDestroy( 1 );
		}
	} else
	if( m_spCompMove->IsDestroy() ) {
		SetDestroy( 1 );
	}
	XEBaseWorldObj::FrameMove( dt );
}

void XObjDmgNum::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ )
{
	XPROF_OBJ_AUTO();
	//#ifndef _XUZHU_HOME
	BYTE a = (BYTE)(GetAlpha() * 255);
	if( a > 255 )
		a = 255;
	XCOLOR col = XCOLOR_RGBA( 209, 21, 21, a );
	if( m_Col != 0 ) {
		col = m_Col;
		const BYTE r = XCOLOR_RGB_R( col );
		const BYTE g = XCOLOR_RGB_G( col );
		const BYTE b = XCOLOR_RGB_B( col );
		col = XCOLOR_RGBA(r,g,b,a);
	}
#if defined(_CHEAT) && defined(WIN32)
	if( !(XAPP->m_dwFilter & xBIT_NO_DRAW_DMG_NUM) )
#endif // defined(_CHEAT) && defined(WIN32)
		m_pfdNumber->DrawStringStyle( vPos.x, vPos.y, col, xFONT::xSTYLE_STROKE, m_strNumber.c_str() );
//#endif 
}

//////////////////////////////////////////////////////////////////////////
XObjYellSkill::XObjYellSkill( LPCTSTR szText, 
															const XSPUnit& spOwner,
															const XE::VEC3& vwPos, 
															XCOLOR col )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwPos )
{
	Init();
	m_Col = col;
	m_strText = szText;
	m_spOwner = spOwner;
	float size = 20.f;
//	m_pFontDat = FONTMNG->Load( FONT_NANUM_BOLD, size );
	m_pFontObj = FONTMNG->CreateFontObj( FONT_NANUM_BOLD, 20.f );
	// 0.5초간 떠오른다.
	m_timerLife.Set(0.5f);
	m_vDelta.Set(0,0,-0.2f);
	m_State = 0;
	m_psfcBg = IMAGE_MNG->Load( PATH_UI("yell_banner.png") );
	XBREAK( m_psfcBg == nullptr );
	float width =  m_psfcBg->GetWidth();
	m_pFontObj->SetLineLength( width );
	m_pFontObj->SetAlign( XE::xALIGN_HCENTER );
}

void XObjYellSkill::Destroy()
{
//	SAFE_RELEASE2( FONTMNG, m_pFontDat );
	SAFE_DELETE( m_pFontObj );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBg );
}

void XObjYellSkill::FrameMove( float dt )
{
	if( m_spOwner ) {
		auto vwPos = GetvwPos();
		vwPos.x = m_spOwner->GetvwPos().x;
		vwPos.y = m_spOwner->GetvwPos().y;
		SetvwPos( vwPos );
		if( m_spOwner->IsDead() ) {
			SetDestroy(1);
			return;
		}
	}
	AddPos( m_vDelta * dt );
	if( m_State == 0 ) {
		// 떠오르기
		if( m_timerLife.IsOver() ) {
			// 1.5초간 대기.
			m_State = 1;
			m_timerLife.Set(1.5f);
			m_vDelta.Set(0);
		}
	} else 
	if( m_State == 1 ) {
		// 대기
		if( m_timerLife.IsOver() ) {
			// x초간 빠르게 떠오르며 사라진다.
			m_State = 2;
			m_timerLife.Set(4.0f);
			m_vDelta.z = -1.0f;
		}
	} else {
		float alpha = 1.0f - m_timerLife.GetSlerp();
		SetAlpha( alpha );
		if( m_timerLife.IsOver() )
			SetDestroy(1);
	}
	XEBaseWorldObj::FrameMove( dt );
}

void XObjYellSkill::Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ )
{
	XPROF_OBJ_AUTO();
	auto vLT = vPos - m_psfcBg->GetSize() * 0.5f;
	if( m_psfcBg ) {
		m_psfcBg->SetfAlpha( GetAlpha() * 0.75f );
		m_psfcBg->Draw( vLT );
	}
	BYTE rp = XCOLOR_RGB_R( m_Col );
	BYTE gp = XCOLOR_RGB_G( m_Col );
	BYTE bp = XCOLOR_RGB_B( m_Col );
	BYTE ap = XCOLOR_RGB_R( m_Col );
	float fap = ap / 255.f;
	BYTE a = (BYTE)((GetAlpha() * fap) * 255);
	if( a > 255 )
		a = 255;
	XCOLOR col = XCOLOR_RGBA( rp, gp, bp, a );
// 	if( m_Col != 0 ) {
// 		col = m_Col;
// 	}
	m_pFontObj->SetColor( col );
	m_pFontObj->DrawString( vLT + XE::VEC2(0,5), m_strText.c_str() );
//	m_pfdNumber->DrawString( vPos.x, vPos.y, m_strNumber.c_str(), col );
//	m_pFontDat->DrawStringStyle( vPos.x, vPos.y, col, xFONT::xSTYLE_STROKE, m_strText.c_str() );
//	m_pFontDat->DrawString( vPos.x, vPos.y, col, m_strText.c_str() );
}


////////////////////////////////////////////////////////////////
/**
 @brief 
 @param radius 데미지 반경
 @param secLife 객체 지속시간
 @param bitCampTarget 데미지를 줄 진영
*/
XObjFlame::XObjFlame( const XSPUnit& spAttacker,
											const XE::VEC3& vwPos,
											float damage,
											float radius,
											float secLife,
											BIT bitCampTarget,
											LPCTSTR szSpr, ID idAct )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_DAMAGE, vwPos, szSpr, idAct )
{
	Init();
	m_spAttacker = spAttacker;
	m_Radius = radius;
	XBREAK( secLife == 0.f );
	m_timerLife.Set( secLife );
	m_timerDOT.Set( 1.f );		// 기본주기는 1초
	m_bitCamp = bitCampTarget;
}

void XObjFlame::Destroy()
{
}

void XObjFlame::FrameMove( float dt )
{
	if( m_timerDOT.IsOver() ) {
		// DOT타이머를 먼저 호출해야 한다. (지속시간/DOT둘다 1초일경우 한번은 들어가게)
		m_timerDOT.Reset();
		//XArrayLinearN<XBaseUnit*, 512> ary;
		XVector<XSPUnit> ary;
		XEObjMngWithType::sGet()->GetListUnitRadius2( &ary,
													nullptr,
													GetvwPos().ToVec2(),
													m_Radius,
													m_bitCamp,
													5,
													false,
													XSKILL::xTL_LIVE );
		for( auto spUnit : ary ) {
			if( spUnit->IsDestroy() )
				continue;
			if( XASSERT( spUnit ) ) {
				BIT bitHit = xBHT_HIT | xBHT_BY_SKILL;
				if( m_Damage == 0 )
					bitHit &= ~xBHT_HIT;
//				pUnit->DoDamage( m_spAttacker.get(), m_Damage, 0.f, XSKILL::xDMG_MAGIC, bitHit, xDA_FIRE );
				auto pMsg = std::make_shared<xnUnit::XMsgDmg>( m_spAttacker
																										, spUnit
																										, m_Damage
																										, 0.f
																										, xDMG_MAGIC
																										, bitHit
																										, xDA_FIRE );
				spUnit->PushMsg( pMsg );
			}
		}
	}
	if( m_timerLife.IsOver() ) {
		SetDestroy( 1 );
	}
	XEBaseWorldObj::FrameMove( dt );
}

//////////////////////////////////////////////////////////////////////////
XObjRes::XObjRes( const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct
									, const XVector<XGAME::xRES_NUM>& aryLoots )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), XGAME::xOT_ETC, vwPos, szSpr, idAct )
//	, m_spCompMove( 20.f, XE::VEC2( 270.f, 290.f ) )
{
	m_psfcShadow = IMAGE_MNG->Load( TRUE, PATH_IMG( "shadow.png" ) );
	m_aryLoots = aryLoots;
	m_spCompFont = std::make_shared<XCompObjFont>();
	m_spCompFont->Load( FONT_RESNUM, 18.f );
	SetBounce( 20.f, xRandomF(XE::VEC2( 270.f, 290.f )) );
}

XObjRes::XObjRes( const XE::VEC3& vwPos, XGAME::xtResource resType, int num )
	: XObjRes( vwPos, _T( "obj_res.spr" ), (int)resType + 1, XVector<XGAME::xRES_NUM>() )
{
	m_resType = resType;
	m_numRes = num;
	XBREAK( m_spCompFont == nullptr );
	m_spCompFont->SetstrText( XE::NtS(num) );
	if( GetpSprObj() )
		GetpSprObj()->SetAction( (ID)(resType + 1) );
}

XObjRes::~XObjRes()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcShadow );
}

void XObjRes::SetBounce( float power, float dAngZ, float gravity )
{
	auto spCompBounce = std::make_shared<XCompObjMoveBounce>( power, dAngZ );
	spCompBounce->SetGravity( gravity );
	m_spCompMove = spCompBounce;
}
//
void XObjRes::FrameMove( float dt )
{
	m_spCompMove->SetvwPos( GetvwPos() );
	if( m_spCompMove->FrameMove( dt ) ) {
		if( m_spCompMove->IsDisappear() )	// 바닥에 떨어져서 멈춤
			m_timerAlpha.Set( 5.f );		// 5초후엔 자동으로 먹어짐
	}
	SetvwPos( m_spCompMove->GetvwPos() );
	//
	if( m_spCompMove->IsDisappear() ) {
		// 대기상태( 유닛이 닿으면 2로 바뀐다 )
		// 동시에 먹은리소스아이콘과 +숫자로 얼마를 먹었는지 표시
		// 리소스 종류가 여러개면 차례대로 올라온다.
		const auto pLegionObj = XBattleField::sGet()->GetPlayerLegionObj();
		auto spUnit = pLegionObj->FindNearUnit( GetvwPos(), 5.f );
		if( spUnit || m_timerAlpha.IsOver() ) {
			auto pUI = new XObjResNum( GetvwPos(), m_resType, m_numRes );
			XBattleField::sGet()->AddpObj( pUI );
			SetDestroy( 1 );		// 바로 삭제
		}
	}
	XEBaseWorldObj::FrameMove( dt );
}

void XObjRes::Draw( const XE::VEC2& vPos, float scale/* = 1.f*/, float alpha )
{
	XPROF_OBJ_AUTO();
	const XE::VEC3 vwPos = GetvwPos();
	if( m_psfcShadow ) {
		float scaleDraw = GetScaleObj().x * scale;
		auto vGround = vwPos;
		vGround.z = 0;
		XE::VEC2 vShadow = GetpWndWorld()->GetPosWorldToWindow( vGround, nullptr );
//		m_psfcShadow->SetfAlpha( 0.6f );
		const XE::VEC2 vAdj = (m_psfcShadow->GetSize() * -0.5f) * scaleDraw;;
		m_psfcShadow->SetScale( scaleDraw * 2.f );
		m_psfcShadow->Draw( vShadow + vAdj );
	}
	XEBaseWorldObj::Draw( vPos, scale, alpha );
	// 자원 개수를 표시
	const auto _alpha = GetAlpha() * alpha;
//	XCOLOR colText = XCOLOR_RGBA( 0, 255, 0, (BYTE)(255*_alpha) );
	XCOLOR colText = XCOLOR_RGBA( 255, 255, 255, (BYTE)(255*_alpha) );
	m_spCompFont->SetCol( colText );
//	const _tstring strText = XE::NtS( m_numRes );
	XE::VEC2 vDrawLT = vPos + (XE::VEC2(-20,-30) * scale);
	m_spCompFont->Draw( vDrawLT, scale );

}

////////////////////////////////////////////////////////////////
XObjResNum::XObjResNum( const XE::VEC3& vwPos, XGAME::xtResource resType, int num )
	: XEBaseWorldObj( XWndBattleField::sGetObjLayer(), xOT_UI, vwPos, _T("obj_res.spr"), (int)resType+1 )

{
	Init();
	m_spCompFont = std::make_shared<XCompObjFont>();
	m_spCompFont->Load( FONT_RESNUM, 18.f );
	m_spCompFont->SetstrText( XE::NtS( num ) );
	m_State = 2;
	m_timerAlpha.Set( 1.5f );
}

void XObjResNum::FrameMove( float dt )
{
	if( m_State == 2 ) {
		// 그냥 올라가는중
		auto vwPos = GetvwPos();
		vwPos.z -= 1.5f * dt;
		SetvwPos( vwPos );
		if( m_timerAlpha.IsOver() ) {
			m_timerAlpha.Set( 0.5f );
			m_State = 3;
		}
	} else
	if( m_State == 3 ) {
		// 사라지는 중
		auto vwPos = GetvwPos();
		vwPos.z -= 1.f * dt;
		SetvwPos( vwPos );
		auto lerp = m_timerAlpha.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		SetAlpha( 1.0f - lerp );
		if( lerp >= 1.f )
			SetDestroy( 1 );
	}

	XEBaseWorldObj::FrameMove( dt );
}

void XObjResNum::Draw( const XE::VEC2& vPos, float scale/* = 1.f*/, float alpha )
{
	XPROF_OBJ_AUTO();
	const XE::VEC3 vwPos = GetvwPos();
	XEBaseWorldObj::Draw( vPos, scale, alpha );
	// 자원 개수를 표시
	const auto _alpha = GetAlpha() * alpha;
	XCOLOR colText = XCOLOR_RGBA( 0, 255, 0, (BYTE)( 255 * _alpha ) );
	m_spCompFont->SetCol( colText );
	XE::VEC2 vDrawLT = vPos + ( XE::VEC2( -20, -30 ) * scale );
	m_spCompFont->Draw( vDrawLT, scale );

}