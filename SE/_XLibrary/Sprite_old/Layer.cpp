/*
 *  Layer.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "Layer.h"
#include "SprObj.h"
#include "XBaseObj.h"
#include "xMath.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// inter타입의 보간함수로 timelerp의 보간값을 계산해서 pfOut에 넣는다. 에러가 나면 0이 리턴된다
BOOL XLayerMove::CalcInterpolation( float *pfOut, float fTimeLerp, XBaseLayer::xtInterpolation inter )
{
	float fSpeedLerp = 0;
	if( pfOut )
		*pfOut = 0;
	switch( inter )
	{
	case xLINEAR:	fSpeedLerp = XE::xiLinearLerp( fTimeLerp );		break;
	case xACCEL:		fSpeedLerp = XE::xiHigherPowerAccel( fTimeLerp, 1.0f, 0 );		break;
	case xDEACCEL:	fSpeedLerp = XE::xiHigherPowerDeAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSMOOTH:	fSpeedLerp = XE::xiSmoothStep( fTimeLerp, 1.0f, 0 );		break;
	case xSPLINE:	fSpeedLerp = XE::xiCatmullrom( fTimeLerp, -10.0f, 0, 1, 1 );	break;
	default:
		XBREAKF( 1, "invalide interpolation type:%d", (int)inter );
		return FALSE;
	}
	if( pfOut )
		*pfOut = fSpeedLerp;
	return TRUE;
}


void XLayerMove::FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr )
{
	// 좌표 보간
	if( m_cnPos.interpolation )		// 보간이동중이다
	{
		XKeyPos *pStartKey = m_cnPos.pStartKey;
		XKeyPos *pEndKey = m_cnPos.pEndKey;
		XBREAK( pStartKey == NULL );
		XBREAK( pEndKey == NULL );
		float fInterLength = pEndKey->GetfFrame() - pStartKey->GetfFrame();		// 보간구간의 전체 길이
		float fTimeLerp = (fFrmCurr - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		// 패스의 끝까지 왔다
		if( fTimeLerp >= 1.0f )
			fTimeLerp = 1.0f;
		// 속도보간
		float fSpeedLerp = 0;
		CalcInterpolation( &fSpeedLerp, fTimeLerp, m_cnPos.interpolation );
		// 속도보간값에 따른 패스상의 현재 위치
		// fSpeedLerp값이 전체패스상에서 어느위치인지 계산
		switch( m_cnPos.typePath )
		{
		case SPR::xLINE_LINE:
			{
				if( pStartKey->GetpInterpolationEndKey() == pStartKey->GetpNextPathKey() )	// 보간끝키가 다음키와 같다면 단칸패스이므로 기존과 같이 계산
				{
					// 단칸구간
					XE::VEC2 v1 = pStartKey->GetPos();
					XE::VEC2 v2 = pEndKey->GetPos();
					m_cnPos.vPos = v1 + (v2 - v1) * fSpeedLerp;		// 최종좌표
				} else
				{
					XBREAK( pStartKey->GetpNextPathKey() == NULL );
					// fSlerp가 어느키 구간에 걸려있는지 탐색
					XKeyPos *pCurrKey = pStartKey;
					float keyLerp=0, nextKeyLerp=0;
					while(1)
					{
						keyLerp = (pCurrKey->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간에서의 현재키lerp값
						nextKeyLerp = (pCurrKey->GetpNextPathKey()->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 다음키의 전체보간구간에서의 lerp값
						if( fSpeedLerp < nextKeyLerp )
							break;
						pCurrKey = pCurrKey->GetpNextPathKey();
					}
					// pCurrKey ~ pNextKey구간을 0~1로 보고 fSlerp가 그중 어느위치인지 계산
					{
						float fLerp = (fSpeedLerp - keyLerp) / (nextKeyLerp - keyLerp);	// 키구간사이에서의 lerp값
						XE::VEC2 v1 = pCurrKey->GetPos();
						XE::VEC2 v2 = pCurrKey->GetpNextPathKey()->GetPos();
						m_cnPos.vPos = v1 + (v2 - v1) * fLerp;		// 최종좌표
					}
				}
			}
			break;
		case SPR::xLINE_SPLINE_CATMULLROM:
			{
				XE::VEC2 v0, v1, v2, v3, vOut;
				float fLerp = fSpeedLerp;
				// 스플라인보간값을 계산하기위해 v0, v1, v2, v3좌표를 얻는다
				if( pStartKey->GetpNextPathKey() )	// 시작키의 다음키가 없으면 보간하지 않음
				{
					if( pStartKey->GetpInterpolationEndKey() == pStartKey->GetpNextPathKey() )
					{
						// 스플라인타입인데 단칸짜리
						v0 = ( pStartKey->GetpPrevPathKey() )? pStartKey->GetpPrevPathKey()->GetPos() : pStartKey->GetPos();
						v1 = pStartKey->GetPos();
						v2 = pEndKey->GetPos();
						v3 = ( pEndKey->GetpNextPathKey() )? pEndKey->GetpNextPathKey()->GetPos() : pEndKey->GetPos();
					} else
					{
						// 스플라인타입 다구간 보간
						// 현재 fSpeedLerp가 스플라인구간중에서 어느구간인지 찾아낸다
						XKeyPos *pCurrKey = pStartKey;
						float keyLerp=0, nextKeyLerp=0;
						while(1)
						{
							if( XBREAK( pCurrKey->GetpNextPathKey() == NULL ) )
								break;
							keyLerp = (pCurrKey->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간에서의 현재키lerp값
							nextKeyLerp = (pCurrKey->GetpNextPathKey()->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 다음키의 전체보간구간에서의 lerp값
							if( fSpeedLerp <= nextKeyLerp )
								break;
							pCurrKey = pCurrKey->GetpNextPathKey();
						}
						// pCurrKey ~ pNextKey구간을 0~1로 보고 fSpeedLerp가 그중 어느위치인지 계산
						{
							fLerp = (fSpeedLerp - keyLerp) / (nextKeyLerp - keyLerp);	// 키구간사이에서의 lerp값
							v0 = ( pCurrKey->GetpPrevPathKey() )? pCurrKey->GetpPrevPathKey()->GetPos() : pCurrKey->GetPos();
							v1 = pCurrKey->GetPos();
							v2 = ( pCurrKey->GetpNextPathKey() )? pCurrKey->GetpNextPathKey()->GetPos() : pCurrKey->GetPos();
							if( pCurrKey->GetpNextPathKey() )
							{
//								if( pCurrKey->GetpNextPathKey()->GetpNextPathKey( pCurrKey->GetidPath() ) )
								if( pCurrKey->GetpNextPathKey()->GetpNextPathKey() && 
									pCurrKey->GetpNextPathKey()->GetpNextPathKey()->GetpPrevPathKey() )	// 다다음키가 있고 다다음키의 '앞'키가 뭔가 있으면 패스의 일부분으로 간주
									v3 = pCurrKey->GetpNextPathKey()->GetpNextPathKey()->GetPos();
								else
									v3 = pCurrKey->GetpNextPathKey()->GetPos();
							}
							else
								v3 = pCurrKey->GetPos();
						}
					}
					Vec2CatmullRom( vOut, v0, v1, v2, v3, fLerp );
					m_cnPos.vPos = vOut;		// 스플라인위에서의 최종 좌표
				}
			}
			break;
		} // switch
		// 패스의 끝까지 왔으면 패스보간모드 끝
		if( fTimeLerp >= 1.0f )
			m_cnPos.interpolation = xNONE;		
	} // cnPos.interpolation
	// 알파보간
	if( m_cnEffect.interpolation )		// 보간이동중이다
	{
		float fFrmLength = m_cnEffect.fNextKeyFrame - m_cnEffect.fStartKeyFrame;
		float fSlerp = (fFrmCurr - m_cnEffect.fStartKeyFrame) / fFrmLength;
		if( fSlerp >= 0 )
		{
			m_cnEffect.fAlpha = m_cnEffect.fAlphaSrc + ((m_cnEffect.fAlphaDest - m_cnEffect.fAlphaSrc) * fSlerp);
			if( fSlerp >= 1.0f )
				m_cnEffect.interpolation = xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	}
	// 회전보간
	if( m_cnRot.interpolation )
	{
		float fFrmLength = m_cnRot.fNextKeyFrame - m_cnRot.fStartKeyFrame;
		float fSlerp = (fFrmCurr - m_cnRot.fStartKeyFrame) / fFrmLength;
		if( fSlerp >= 0 )
		{
			m_cnRot.fAngle = m_cnRot.fAngleSrc + ((m_cnRot.fAngleDest - m_cnRot.fAngleSrc) * fSlerp); 
			if( fSlerp >= 1.0f )
				m_cnRot.interpolation = xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	}
	// 스케일 보간
	if( m_cnScale.interpolation )		// 보간
	{
		float fFrmLength = m_cnScale.fNextKeyFrame - m_cnScale.fStartKeyFrame;
		float fSlerp = (fFrmCurr - m_cnScale.fStartKeyFrame) / fFrmLength;
		if( fSlerp >= 0 )
		{
			m_cnScale.fScaleX = m_cnScale.fScaleSrcX + ((m_cnScale.fScaleDestX - m_cnScale.fScaleSrcX) * fSlerp);
			m_cnScale.fScaleY = m_cnScale.fScaleSrcY + ((m_cnScale.fScaleDestY - m_cnScale.fScaleSrcY) * fSlerp);
			if( fSlerp >= 1.0f )
				m_cnScale.interpolation = xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	}
}

#if 0
	// 좌표 보간
	if( m_cnPos.interpolation )		// 보간이동중이다
	{
		float fFrmLength = m_cnPos.fNextKeyFrame - m_cnPos.fStartKeyFrame;
		float fLerp = (fFrmCurr - m_cnPos.fStartKeyFrame) / fFrmLength;
		float fSlerp = 0;
		switch( m_cnPos.interpolation )
		{
		case xLINEAR:	fSlerp = xiLinearLerp( fLerp );		break;
		case xACCEL:		fSlerp = xiHigherPowerAccel( fLerp, 1.0f, 0 );		break;
		case xDEACCEL:	fSlerp = xiHigherPowerDeAccel( fLerp, 1.0f, 0 );		break;
		case xSMOOTH:	fSlerp = xiSmoothStep( fLerp, 1.0f, 0 );		break;
		case xSPLINE:	fSlerp = xiCatmullrom( fLerp, -10.0f, 0, 1, 1 );	break;
		default:
			XBREAK(1);
			m_cnPos.interpolation = xLINEAR;
		}
		if( fSlerp >= 0 )
		{
			m_cnPos.x = m_cnPos.sx + ((m_cnPos.ox - m_cnPos.sx) * fSlerp);
			m_cnPos.y = m_cnPos.sy + ((m_cnPos.oy - m_cnPos.sy) * fSlerp);
			if( fLerp >= 1.0f )
				m_cnPos.interpolation = xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	}

#endif // 0

#ifdef _VER_OPENGL
void XLayerImage::Draw( XSprObj *pSprObj, float x, float y, XEFFECT_PARAM *pEffectParam )
#else
void XLayerImage::Draw( XSprObj *pSprObj, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam )
#endif
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		float lx = Getx();
		float ly = Gety();
/*		// x, y가 항상 0으로 오기땜에 여기서 검사못하고 XLayerMove::IsClipping(x,y)를 따로 만들어서 해야할듯
		float sx = GetcnScale().fScaleX;
		float sy = GetcnScale().fScaleY;
		float l = x + lx + (pSpr->GetAdjustX() * sx);
		float t = y + ly + (pSpr->GetAdjustY() * sy);
		if( l > XSCREEN_WIDTH )		// 스프라이트 좌측이 화면오른쪽을 벗어남
			return;
		if( l + (pSpr->GetWidth() * sx) < 0 )			// 스프라이트 우측이 화면왼쪽을 벗어남
			return;
		if( t > XSCREEN_HEIGHT )		// 스프라이트 위쪽이 화면 아래를 벗어남
			return;
		if( t + (pSpr->GetHeight() * sy) < 0 )		// 스프라이트 아래쪽이 화면 위쪽을 벗어남
			return; */

		pSpr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().fScaleX, GetcnScale().fScaleY );
		pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		pSpr->SetColor( pSprObj->GetColorR(), pSprObj->GetColorG(), pSprObj->GetColorB() );
		if( pEffectParam )		// 외부파라메터가 있으면 그걸 우선적용함
		{
			if( pEffectParam->drawMode != xDM_ERROR )		// xDM_ERROR은 처리 하지 않음
				pSpr->SetDrawMode( pEffectParam->drawMode );
			else
				pSpr->SetDrawMode( GetcnEffect().DrawMode );

			pSpr->SetfAlpha( pEffectParam->fAlpha * GetcnEffect().fAlpha );	// 알파는 외부파라메터것을 한번더 곱한다
		}
		else
		{
			pSpr->SetDrawMode( GetcnEffect().DrawMode );
			pSpr->SetfAlpha( GetcnEffect().fAlpha );
		}

		if( GetcnEffect().DrawMode != xDM_NONE )
#ifdef _VER_OPENGL
			pSpr->Draw( x + lx, y + ly );
#else
			pSpr->Draw( x + lx, y + ly, m );
#endif
	}
}

// cx,cy:XSprObj이 Draw되는 중심위치, mx, my:픽킹좌표
DWORD XLayerImage::GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		XE::VEC2 vLocal = GetcnPos().vPos;
		pSpr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().fScaleX, GetcnScale().fScaleY );
		pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		MATRIX mWorld;
		pSpr->GetMatrix( &mWorld, vLocal.x, vLocal.y );		
		MatrixMultiply( mWorld, mWorld, m );			// 부모의행렬과 스프라이트의 로컬행렬을곱함
		MATRIX mInv;
		MatrixInverse( mInv, mWorld );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		XE::VEC3 v( mx, my, 0 );		// 마우스 클릭한 좌표
		Vec4 vInv;
		MatrixVec4Multiply( vInv, v, mInv );			// 최종 매트릭스 기준 좌표가 나온다.
		DWORD pixel = pSpr->GetPixel( vInv.x, vInv.y, pa, pr, pg, pb );
		pSpr->SetScale( 1.0f, 1.0f );
		pSpr->SetRotate( 0, 0, 0 );
		return pixel;
	}
	return 0;
}
#ifdef _VER_OPENGL
void XLayerObject::Draw( XSprObj*, float x, float y, XEFFECT_PARAM *pEffectParam )
#else
void XLayerObject::Draw( XSprObj *, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam )
#endif
{
	XSprObj *pSprObj = m_pSprObjCurr;
	if( pSprObj )
	{
		float lx = Getx();
		float ly = Gety();
		pSprObj->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSprObj->SetRotateZ( GetcnRot().fAngle );
		pSprObj->SetScale( GetcnScale().fScaleX, GetcnScale().fScaleY );
		pSprObj->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSprObj->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		if( pEffectParam )
		{
			if( pEffectParam->drawMode != xDM_ERROR )		// xDM_ERROR은 처리 하지 않음
				pSprObj->SetDrawMode( pEffectParam->drawMode );
			else
				pSprObj->SetDrawMode( GetcnEffect().DrawMode );

			pSprObj->SetfAlpha( GetcnEffect().fAlpha * pEffectParam->fAlpha );		// fAlpha의 디폴트값은 1.0
		} else
		{
			pSprObj->SetDrawMode( GetcnEffect().DrawMode );
			pSprObj->SetfAlpha( GetcnEffect().fAlpha );
		}
		
#ifdef _VER_OPENGL
		pSprObj->Draw( x + lx, y + ly );
#else
		pSprObj->Draw( x + lx, y + ly, m );
#endif
	} 
}
void XLayerObject::FrameMove( XSprObj *pParent, float dt, float fFrmCurr )
{
	XLayerMove::FrameMove( pParent, dt, fFrmCurr );
	XSprObj *pSprObj = m_pSprObjCurr;
	if( pSprObj )
	{
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
	if( m_bActive )	// 더미가 on일때만 핸들러를 호출
	{
		XBaseObj *pObj = pSprObj->GetpParent();
		if( pObj )
		{
//			XE::VEC2 vPos( GetcnPos().x, GetcnPos().y );
			float angle = GetcnRot().fAngle;
			XE::VEC2 vScale( GetcnScale().fScaleX, GetcnScale().fScaleY );
			XE::VEC2 vPosT;
			Transform( &vPosT );
			pSprObj->Transform( &vPosT );
			pSprObj->Transform( &angle );
			//pSprObj->Transform( &vScale );	<<- 이것도 해야할듯
			pObj->OnProcessDummy( m_id, vPosT, angle, vScale, dt, fFrmCurr );
		}
	}
}

