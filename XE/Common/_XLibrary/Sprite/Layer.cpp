/*
 *  Layer.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "etc/XSurface.h"
#include "SprObj.h"
#include "XBaseObj.h"
#include "etc/xMath.h"
#include "XArchive.h"
#include "Sprite.h"
#include "Layer.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 28버전이후 바뀌어 저장된 레이어 타입
enum xtLayerFile {
	xLT_NONE = 0,
	xLT_MOVE = 0x8000,
	xLT_IMAGE = 0x0001 | xLT_MOVE,
	xLT_OBJECT = 0x0002 | xLT_MOVE,
	xLT_SOUND = 0x0004,
	xLT_EVENT = 0x0008,
	xLT_DUMMY = 0x0010 | xLT_MOVE,
	//	xLT_MAX 
};

// int로 넘기면 상위비트에 ffff가 붙어서 일부러 word형으로 받음.
xSpr::xtLayer XBaseLayer::sConvertNewToOld( WORD w0 ) {
	switch( w0 ) {
	case xLT_NONE: {
		XBREAK(1);		// 0은 있을수 없음.
	} break;
	case xLT_IMAGE: return xSpr::xLT_IMAGE;
	case xLT_OBJECT: return xSpr::xLT_OBJ;
	case xLT_SOUND: return xSpr::xLT_SOUND;
	case xLT_EVENT: return xSpr::xLT_EVENT;
	case xLT_DUMMY: return xSpr::xLT_DUMMY;
	default:
		XBREAKF( 1, "변환할 수 없는 레이어 타입:b0=%d", w0 );
		break;
	}
	return xSpr::xLT_NONE;
}

//////////////////////////////////////////////////////////////////////////
// inter타입의 보간함수로 timelerp의 보간값을 계산해서 pfOut에 넣는다. 에러가 나면 0이 리턴된다
float XLayerMove::sCalcInterpolation( float fTimeLerp, XBaseLayer::xtInterpolation inter )
{
	float fSpeedLerp = 0;
	switch( inter )	{
	case xLINEAR:	fSpeedLerp = XE::xiLinearLerp( fTimeLerp );		break;
	case xACCEL:		fSpeedLerp = XE::xiHigherPowerAccel( fTimeLerp, 1.0f, 0 );		break;
	case xDEACCEL:	fSpeedLerp = XE::xiHigherPowerDeAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSMOOTH:	fSpeedLerp = XE::xiSmoothStep( fTimeLerp, 1.0f, 0 );		break;
	case xSPLINE:	fSpeedLerp = XE::xiCatmullrom( fTimeLerp, -10.0f, 0, 1, 1 );	break;
	default:
		XBREAKF( 1, "invalide interpolation type:%d", (int)inter );
		fSpeedLerp = XE::xiLinearLerp( fTimeLerp );		
		break;
	}
	return fSpeedLerp;
}

void CHANNEL_POS::FrameMove( float dt, float frmCurr ) {
	if( interpolation ) {
		const float fInterLength = m_frameEndKey - m_frameStartKey;		// 보간구간의 전체 길이
		XBREAK( fInterLength == 0.f );
		auto timeLerp = (frmCurr - m_frameStartKey) / fInterLength;	// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		if( timeLerp >= 1.f )
			timeLerp = 1.f;
		float lerp = XLayerMove::sCalcInterpolation( timeLerp, interpolation );
		vPos = m_vStart + (m_vEnd - m_vStart) * lerp;
	} else {
		vPos = m_vStart;
	}
}

void CHANNEL_ROT::FrameMove( float dt, float frmCurr ) {
	if( interpolation ) {
		const float fInterLength = fNextKeyFrame - fStartKeyFrame;		// 보간구간의 전체 길이
		XBREAK( fInterLength == 0.f );
		auto timeLerp = (frmCurr - fStartKeyFrame) / fInterLength;	// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		if( timeLerp >= 1.f )
			timeLerp = 1.f;
		float lerp = XLayerMove::sCalcInterpolation( timeLerp, interpolation );
		fAngle = fAngleSrc + (fAngleDest - fAngleSrc) * lerp;
	} else {
		fAngle = fAngleSrc;
	}
}

void CHANNEL_SCALE::FrameMove( float dt, float frmCurr ) {
	if( interpolation ) {
		const float fInterLength = fNextKeyFrame - fStartKeyFrame;		// 보간구간의 전체 길이
		XBREAK( fInterLength == 0.f );
		auto timeLerp = ( frmCurr - fStartKeyFrame ) / fInterLength;	// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		if( timeLerp >= 1.f )
			timeLerp = 1.f;
		float lerp = XLayerMove::sCalcInterpolation( timeLerp, interpolation );
		m_vScale = m_vScaleSrc + ( m_vScaleDest - m_vScaleSrc ) * lerp;
	}
	else {
		m_vScale = m_vScaleSrc;
	}
}

void CHANNEL_EFFECT::FrameMove( float dt, float frmCurr ) {
	if( interpolation ) {
		const float fInterLength = fNextKeyFrame - fStartKeyFrame;		// 보간구간의 전체 길이
		XBREAK( fInterLength == 0.f );
		auto timeLerp = ( frmCurr - fStartKeyFrame ) / fInterLength;	// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		if( timeLerp >= 1.f )
			timeLerp = 1.f;
		float lerp = XLayerMove::sCalcInterpolation( timeLerp, interpolation );
		fAlpha = fAlphaSrc + ( fAlphaDest - fAlphaSrc ) * lerp;
	}
	else {
		fAlpha = fAlphaSrc;
	}
}

void XLayerMove::Transform( float *pOutLx, float *pOutLy ) 
{
	MATRIX m, mRotX, mRotY, mRotZ, mScale;
	MatrixRotationX( mRotX, 0 );
	MatrixRotationY( mRotY, (GetcnEffect().IsFlipHoriz()) ? D2R( 180.f ) : 0 );
	MatrixRotationZ( mRotZ, D2R( m_cnRot.fAngle ) );
	MatrixScaling( mScale, m_cnScale.m_vScale.x, m_cnScale.m_vScale.y, 1.0f );
	MatrixIdentity( m );
	MatrixMultiply( m, m, mScale );
	MatrixMultiply( m, m, mRotX );
	MatrixMultiply( m, m, mRotY );
	MatrixMultiply( m, m, mRotZ );
	X3D::VEC3 v( m_cnPos.vPos.x, m_cnPos.vPos.y, 0 );
	Vec4 v4d;
	MatrixVec4Multiply( v4d, v, m );
	*pOutLx = v4d.x;
	*pOutLy = v4d.y;
}

/**
 @brief 
*/
void XLayerMove::FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr )
{
	// 좌표 보간
	m_cnPos.FrameMove( dt, fFrmCurr );
	// 회전보간
	m_cnRot.FrameMove( dt, fFrmCurr );
	// 스케일보간
	m_cnScale.FrameMove( dt, fFrmCurr );
	// 알파보간
	m_cnEffect.FrameMove( dt, fFrmCurr );
}

// void XLayerImage::SetDrawInfoToSpr( XSprObj *pSprObj, 
// 																		const XEFFECT_PARAM& effParam,
// 																		XE::xRenderParam* pOut )
// {
// 	XE::xRenderParam param;
// 	param
// 
// 	pSpr->SetAdjustAxis( GetvAdjAxis() );		// 회전축을 보정함
// 	pSpr->SetRotateZ( GetcnRot().fAngle );
// 	pSpr->SetScale( GetcnScale().m_vScale );
// 	pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
// 	pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
// 	pSpr->SetColor( pSprObj->GetColorR(), pSprObj->GetColorG(), pSprObj->GetColorB() );
// 	// 이버전부터는 외부에서 주는대로만 찍는다.
// 	pSpr->SetDrawMode( effParam.drawMode );
// 	pSpr->SetfAlpha( effParam.fAlpha );
// 	pSpr->GetpSurface()->SetadjZ( effParam.m_adjZ );
// 
// 
// }


void XLayerImage::Draw( XSprObj *pSprObj, 
												float x, float y, 
												const MATRIX &mParent, 
												XEFFECT_PARAM *pEffectParam )
{
	const XE::VEC2 vPos(x, y);
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr ) {
		auto vLocal = GetPos();
		auto funcBlend = XE::xBF_MULTIPLY;
		if( GetcnEffect().DrawMode != xDM_NONE ) {
			//
			BOOL bDelegateDraw = FALSE;
			XDelegateSprObj *pDelegate = pSprObj->GetpDelegate();
			XEFFECT_PARAM effParam;
				if( pEffectParam ) {
				effParam = *pEffectParam;
				if( effParam.drawMode != xDM_ERROR )
					funcBlend = effParam.GetfuncBlend();
				// 외부이펙트와 내부이펙트를 곱함(둘중 하나라도 스크린이면 스크린으로 찍힘)
				if( effParam.drawMode == xDM_SCREEN || GetcnEffect().DrawMode == xDM_SCREEN )
					effParam.drawMode = xDM_SCREEN;
				if( effParam.drawMode == xDM_ERROR )
					effParam.drawMode = GetcnEffect().DrawMode;	// 이게 필요한건가?
				effParam.fAlpha *= GetcnEffect().fAlpha;	// 외부 알파와 내부알파를 곱함
			} else {
				effParam.drawMode = GetcnEffect().DrawMode;
				effParam.fAlpha = GetcnEffect().fAlpha;
			}
			// 델리게이트가 있다면 델리게이트를 먼저실행함
			if( pDelegate )
				bDelegateDraw = pDelegate->OnDelegateDrawImageLayerBefore( pSprObj, 
																	pSpr, 
																	this, 
																	&effParam, // pEffectParam, 
																	vPos.x + vLocal.x, 
																	vPos.y + vLocal.y, 
																	mParent );
			// 델리게이트에서 드로우를 하지 않았다면 자체드로우를 함.
			if( bDelegateDraw == FALSE ) {
				XE::xRenderParam param;
				param.m_vPos = vPos + vLocal;
				param.m_vAdjAxis = GetvAdjAxis();
				param.m_vRot.z = GetcnRot().fAngle;
				param.m_vScale = GetcnScale().m_vScale;
				param.m_vColor = pSprObj->Getv4Color();
				param.m_vColor.a = effParam.fAlpha;
				param.SetFlipHoriz( GetcnEffect().IsFlipHoriz() );
				param.SetFlipVert( GetcnEffect().IsFlipVert() );
				param.m_funcBlend = effParam.GetfuncBlend();
				param.m_adjZ = effParam.m_adjZ;
				param.m_bZBuff = GRAPHICS->IsbEnableZBuff();
//				param.m_bZBuff = pSpr->IsBatch();
				param.m_bAlphaTest = (param.m_bZBuff == true);
				if( funcBlend == XE::xBF_ADD || funcBlend == XE::xBF_SUBTRACT ) {
					param.m_bZBuff = false;
					param.m_bAlphaTest = false;
				}
//				SetDrawInfoToSpr( pSprObj, pSpr, effParam, &paramRender );
				if( pSpr->GetpSurface() ) {
					// 전용 배치렌더러에 렌더명령을 전달한다.
					pSpr->GetpSurface()->DrawByParam( mParent, param );
// 				pSpr->Draw( vPos + vLocal, m );
//				pSpr->Draw( x + lx, y + ly, m );
				}
			}
			if( pDelegate )
				bDelegateDraw 
				= pDelegate->OnDelegateDrawImageLayerAfter( pSprObj,
																										pSpr,
																										this,
																										&effParam, // pEffectParam, 
																										vPos + vLocal,
																										mParent );
		}
	}
}

// cx,cy:XSprObj이 Draw되는 중심위치, mx, my:픽킹좌표
DWORD XLayerImage::GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		XE::VEC2 vLocal = GetcnPos().vPos;
		pSpr->SetAdjustAxis( GetvAdjAxis() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().m_vScale );
		pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		MATRIX mWorld;
		pSpr->GetMatrix( &mWorld, vLocal.x, vLocal.y );		
		MatrixMultiply( mWorld, mWorld, m );			// 부모의행렬과 스프라이트의 로컬행렬을곱함
		MATRIX mInv;
		MatrixInverse( mInv, mWorld );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		X3D::VEC3 v( mx, my, 0 );		// 마우스 클릭한 좌표
		Vec4 vInv;
		MatrixVec4Multiply( vInv, v, mInv );			// 최종 매트릭스 기준 좌표가 나온다.
		DWORD pixel = pSpr->GetPixel( vInv.x, vInv.y, pa, pr, pg, pb );
		pSpr->SetScale( 1.0f, 1.0f );
		pSpr->SetRotate( 0, 0, 0 );
		return pixel;
	}
	return 0;
}
int XLayerImage::Serialize( XArchive& ar, XSprObj *pSprObj )
{
	if( m_pSpriteCurr )
		ar << (int)m_pSpriteCurr->GetidxSprite();
	else
		ar << -1;
	return 1;
}

int XLayerImage::DeSerialize( XArchive& ar, XSprObj *pSprObj )
{
	int idxSpr;
	ar >> idxSpr;
	if( idxSpr >= 0 )
		m_pSpriteCurr = pSprObj->GetSpriteMutable( idxSpr );
	else
		m_pSpriteCurr = NULL;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _VER_OPENGL
void XLayerObject::Draw( XSprObj*, float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam )
#else
void XLayerObject::Draw( XSprObj *, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam )
#endif
{
	const XE::VEC2 vPos( x, y );
	XSprObj *pSprObj = m_pSprObjCurr;
	if( pSprObj ) {
// 		float lx = Getx();
// 		float ly = Gety();
		auto vLocal = GetPos();
		pSprObj->SetAdjustAxis( GetvAdjAxis() );		// 회전축을 보정함
		pSprObj->SetRotateZ( GetcnRot().fAngle );
		pSprObj->SetScale( GetcnScale().m_vScale );
		pSprObj->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSprObj->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		if( pEffectParam ) {
			if( pEffectParam->drawMode != xDM_ERROR )		// xDM_ERROR은 처리 하지 않음
				pSprObj->SetDrawMode( pEffectParam->drawMode );
			else
				pSprObj->SetDrawMode( GetcnEffect().DrawMode );

			pSprObj->SetfAlpha( GetcnEffect().fAlpha * pEffectParam->fAlpha );		// fAlpha의 디폴트값은 1.0
		} else {
			pSprObj->SetDrawMode( GetcnEffect().DrawMode );
			pSprObj->SetfAlpha( GetcnEffect().fAlpha );
		}
		pSprObj->Draw( vPos + vLocal, m );
//		pSprObj->Draw( x + lx, y + ly, m );
	} 
}
void XLayerObject::FrameMove( XSprObj *pParent, float dt, float fFrmCurr )
{
	XLayerMove::FrameMove( pParent, dt, fFrmCurr );
	XSprObj *pSprObj = m_pSprObjCurr;
	if( pSprObj )
	{
		pSprObj->SetmultiplySpeed( pParent->GetmultiplySpeed() );
		pSprObj->FrameMove( dt );
	}
}
// lx, ly는 0,0기준 로컬좌표
/*
DWORD XLayerObject::GetPixel( float lx, float ly, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	lx -= Getx();
	ly -= Gety();
	if( m_pSprObjCurr )
		return m_pSprObjCurr->GetPixel( lx, ly, pa, pr, pg, pb );
	return 0;
}

*/

void XLayerDummy::FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr )
{
	XLayerMove::FrameMove( pSprObj, dt, fFrmCurr );
	if( m_bActive )	{// 더미가 on일때만 핸들러를 호출
		XDelegateSprObj *pDelegate = pSprObj->GetpDelegate();
		if( pDelegate ) {
//			XE::VEC2 vPos( GetcnPos().x, GetcnPos().y );
			float angle = GetcnRot().fAngle;
//			XE::VEC2 vScale( GetcnScale().mv .fScaleX, GetcnScale().fScaleY );
			XE::VEC2 vPosT;
			Transform( &vPosT );
			pSprObj->Transform( &vPosT );
			pSprObj->Transform( &angle );
			//pSprObj->Transform( &vScale );	<<- 이것도 해야할듯
			pDelegate->OnProcessDummy( m_id, vPosT, angle, GetcnScale().m_vScale, GetcnEffect(), dt, fFrmCurr );
		}
	}
}

