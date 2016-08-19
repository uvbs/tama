#include "stdafx.h"
#include "XLayerMove.h"
#include "xLayerInfo.h"
#include "XAniAction.h"
#include "XKeyBase.h"
#include "XKeyPos.h"
#include "XKeyRot.h"
#include "XKeyScale.h"
#include "SprObj.h"
#include "XKeyEffect.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
CString XLayerMove::GetLabel()
{
	CString str;
#ifdef _DEBUG
	str.Format( _T("%s%d (id:%d)"), (LPCTSTR)GetstrLabel(), GetnLayer(), GetidLayer() );
#else
	str.Format( _T("%s%d"), (LPCTSTR)GetstrLabel(), GetnLayer() );
#endif
	return str;
}

// inter타입의 보간함수로 timelerp의 보간값을 계산해서 pfOut에 넣는다. 에러가 나면 0이 리턴된다
bool XLayerMove::sCalcInterpolation( float *pfOut, float fTimeLerp, xSpr::xtInterpolation inter )
{
	float fSpeedLerp = 0;
	if( pfOut )
		*pfOut = 0;
	switch( inter )
	{
	case xSpr::xNONE:		// 키가 없을땐 none이 될수도 있다. 이럴댄 리니어로..
	case xSpr::xLINEAR:	fSpeedLerp = XE::xiLinearLerp( fTimeLerp );		break;
	case xSpr::xACCEL:		fSpeedLerp = XE::xiHigherPowerAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xDEACCEL:	fSpeedLerp = XE::xiHigherPowerDeAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xSMOOTH:	fSpeedLerp = XE::xiSmoothStep( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xSPLINE:	fSpeedLerp = XE::xiCatmullrom( fTimeLerp, -10.0f, 0, 1, 1 );	break;
	default:
		XBREAK(1);	// none도 아닌상황
		fSpeedLerp = XE::xiLinearLerp( fTimeLerp );
// 		return false;
	}
	if( pfOut )
		*pfOut = fSpeedLerp;
	return true;
}

void CHANNEL_POS::FrameMove( float dt, float fFrmCurr, XSprObj *pParentSprObj ) {
// 	if( m_vStart == -9999.f )
// 		m_vStart.Set( 0 );
	if( m_interpolation ) {
		float fInterLength = m_fEndKeyFrame - m_fStartKeyFrame;		// 보간구간의 전체 길이
		if( fInterLength == 0 )
			fInterLength = 1;
		// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		float fTimeLerp = ( fFrmCurr - m_fStartKeyFrame ) / fInterLength;		
		if( fTimeLerp >= 1.0f )
			fTimeLerp = 1.0f;
		float lerp = 0;
		// 보간함수에 따라 timeLerp값으로 보간값을 얻어냄
		XLayerMove::sCalcInterpolation( &lerp, fTimeLerp, m_interpolation );
		m_vPos = m_vStart + ( m_vEnd - m_vStart ) * lerp;		// 최종좌표
	} else {
		// 보간이 없을경우는 시작점을 그대로 유지한다.
		m_vPos = m_vStart;
	}
}

void CHANNEL_ROT::FrameMove( float dt, float fFrmCurr, XSprObj *pParentSprObj )
{
	if( interpolation ) {
		float fFrmLength = fNextKeyFrame - fStartKeyFrame;
		if( fFrmLength == 0 )
			fFrmLength = 1;
		float lerpTime = (fFrmCurr - fStartKeyFrame) / fFrmLength;
		if( lerpTime >= 1.f )
			lerpTime = 1.f;
		float fSlerp = 0;
		XLayerMove::sCalcInterpolation( &fSlerp, lerpTime, interpolation );
		fAngle = fAngleSrc + ((fAngleDest - fAngleSrc) * fSlerp);
// 		if( fLerp >= 1.0f )
// 			interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
	} else {
		// 회전 보간이 없을때
		fAngle = fAngleSrc;
	}
}

void CHANNEL_SCALE::FrameMove( float dt, float fFrmCurr, XSprObj *pParentSprObj )
{
	if( interpolation ) {		// 보간
		float fFrmLength = fNextKeyFrame - fStartKeyFrame;
		if( fFrmLength == 0 )
			fFrmLength = 1;
		float lerpTime = (fFrmCurr - fStartKeyFrame) / fFrmLength;
		if( lerpTime >= 1.f )
			lerpTime = 1.f;
		float fSlerp = 0;
		XLayerMove::sCalcInterpolation( &fSlerp, lerpTime, interpolation );
		vScale = vScaleSrc + ((vScaleDest - vScaleSrc) * fSlerp );
// 		if( lerpTime >= 1.0f )
// 			interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
	} else {
		vScale = vScaleSrc;
	}
}

void CHANNEL_EFFECT::FrameMove( float dt, float fFrmCurr, XSprObj* pParentSprObj )
{
	if( interpolation ) {		// 
		float fFrmLength = fNextKeyFrame - fStartKeyFrame;
		if( fFrmLength == 0 )
			fFrmLength = 1;
		float lerpTime = (fFrmCurr - fStartKeyFrame) / fFrmLength;
		if( lerpTime >= 1.f )
			lerpTime = 1.f;
		float fSlerp = 0;
		XLayerMove::sCalcInterpolation( &fSlerp, lerpTime, interpolation );
		fAlpha = fAlphaSrc + ((fAlphaDest - fAlphaSrc) * fSlerp);
// 		if( lerpTime >= 1.0f )
// 			interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
	} else {
		fAlpha = fAlphaSrc;
	}
// 	if( pParentSprObj )
// 		fAlphaSrc *= pParentSprObj->getfAlpha
}

void XLayerMove::FrameMove( float dt, float fFrmCurr, XSprObj *pParentSprObj )
{
	// 좌표 보간
	m_cnPos.FrameMove( dt, fFrmCurr, pParentSprObj );
	// 회전보간
	m_cnRot.FrameMove( dt, fFrmCurr, pParentSprObj );
	// 스케일 보간
	m_cnScale.FrameMove( dt, fFrmCurr, pParentSprObj );
	// 알파보간
	m_cnEffect.FrameMove( dt, fFrmCurr, pParentSprObj );

	// 현재 좌표,회전,스케일의 매트릭스 구함
	auto vAdjAxis = GetvAdjustAxis();
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
	D3DXMatrixTranslation( &mAxis, -vAdjAxis.x, -vAdjAxis.y, 0 );
	D3DXMatrixScaling( &mScale, GetcnScale().vScale.x, GetcnScale().vScale.y, 1.0f );
	D3DXMatrixRotationYawPitchRoll( &mRot, 0, 0, D3DXToRadian( GetcnRot().fAngle ) );
	D3DXMatrixTranslation( &mReverseAxis, vAdjAxis.x, vAdjAxis.y, 0 );
	D3DXMatrixTranslation( &mTrans, GetcnPos().m_vPos.x, GetcnPos().m_vPos.y, 0 );
	m_mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans * pParentSprObj->GetMatrix();	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김 * 마지막으로 부모곱함
}
void XLayerMove::SetcnEffect( xSpr::xtInterpolation inter, float dest, float src, DWORD dwDrawFlag, xDM_TYPE drawMode, float fStartKeyFrame, float fNextKeyFrame ) 
{
	m_cnEffect.interpolation = inter;
	m_cnEffect.fAlphaDest = dest;
	m_cnEffect.fAlphaSrc = src;
	m_cnEffect.dwDrawFlag = dwDrawFlag;
	m_cnEffect.DrawMode = drawMode;
	m_cnEffect.fNextKeyFrame = fNextKeyFrame;
	m_cnEffect.fStartKeyFrame = fStartKeyFrame;
}
// void XLayerMove::SetcnPos( XKeyPos *pStartKey, XKeyPos *pEndKey, SPR::xtLINE pathType, xSpr::xtInterpolation inter ) 
// { 
// 	m_cnPos.interpolation = inter;
// 	m_cnPos.pStartKey = pStartKey;
// 	m_cnPos.pEndKey = pEndKey;
// 	m_cnPos.typePath = pathType;
// 	// vPos는 일부러 세팅하지 않았다. 레이어 보간에서 초기값을 계산하도록 하기위해...
// }
// void XLayerMove::SetcnRot( xSpr::xtInterpolation inter, float dest, float src, float fStartKeyFrame, float fNextKeyFrame )
// {
// 	m_cnRot.interpolation = inter;
// 	m_cnRot.fAngleDest = dest;
// 	m_cnRot.fAngleSrc = src;
// 	m_cnRot.fNextKeyFrame = fNextKeyFrame;
// 	m_cnRot.fStartKeyFrame = fStartKeyFrame;
// }
// void XLayerMove::SetcnScale( xSpr::xtInterpolation inter, float osx, float osy, float ssx, float ssy, float fStartKeyFrame, float fNextKeyFrame ) 
// { 
// 	m_cnScale.interpolation = inter;
// 	m_cnScale.fScaleDestX = osx;		m_cnScale.fScaleDestY = osy;
// 	m_cnScale.fScaleSrcX = ssx;		m_cnScale.fScaleSrcY = ssy;
// 	m_cnScale.fNextKeyFrame = fNextKeyFrame;
// 	m_cnScale.fStartKeyFrame = fStartKeyFrame;
// }

bool XLayerMove::IsExistSamePosKey( XBaseKey *pExist ) const
{
	if( pExist && (pExist->GetType() != xSpr::xKT_EVENT) &&
		(pExist && pExist->GetType() != xSpr::xKT_SOUND) ) {
		CONSOLE( "이미 같은 위치에 같은키가 있습니다." );
		return true;
	}
	return false;
}
// subType키에 맞는 키를 현재 위치에 생성한다
XBaseKey* XLayerMove::DoCreateKeyDialog( XSPAction spAction, 
										float fFrame, 
										xSpr::xtLayerSub subTypeLayer )
{
	XBaseKey *pNewKey = nullptr;
	auto bIgnoreType = xSpr::xKT_NONE;
	// 이미 같은 레이어에 같은종류의 키가 같은위치에 있으면 생성하지 않음.
	auto typeKeySub = XBaseKey::sGetSubTypeFromLayer( subTypeLayer );
	XBaseKey *pExistKey = spAction->FindKey( GetThis(), bIgnoreType, typeKeySub, fFrame );
	if( IsExistSamePosKey( pExistKey ) )
		return nullptr;
	switch( subTypeLayer ) {
	case xSpr::xKTS_MAIN:	 	return nullptr;	// 메인키는 하위클래스에서 만들어야 함
	case xSpr::xLTS_POS: {
		pNewKey = spAction->AddKeyPos( fFrame, GetThis(), GetcnPos().m_vPos );
	} break;
	case xSpr::xLTS_ROT: {
		pNewKey = spAction->AddKeyRot( fFrame, GetThis(), GetcnRot().fAngle );
	} break;
	case xSpr::xLTS_SCALE: {
		pNewKey = spAction->AddKeyScale( fFrame, GetThis(), GetcnScale().vScale );
	} break;
	case xSpr::xLTS_EFFECT: {
		pNewKey = spAction->AddKeyEffect( fFrame, GetThis(), GetcnEffect().interpolation, GetcnEffect().dwDrawFlag, GetcnEffect().DrawMode, GetcnEffect().fAlpha );
	} break;
	default:
		XBREAKF( 1, "뭐임? %d", (int)subTypeLayer );
		break;
	}
	return pNewKey;
}
void XLayerMove::Transform( float *pOutLx, float *pOutLy ) 
{
	D3DXMATRIX m, mRotX, mRotY, mRotZ, mScale;
	D3DXMatrixRotationX( &mRotX, 0 );
	D3DXMatrixRotationY( &mRotY, ( GetcnEffect().IsFlipHoriz() )? D2R(180.f) : 0 );
	D3DXMatrixRotationZ( &mRotZ, D2R(m_cnRot.fAngle) );
	D3DXMatrixScaling( &mScale, m_cnScale.vScale.x, m_cnScale.vScale.y, 1.0f );
	D3DXMatrixIdentity( &m );
	D3DXMatrixMultiply( &m, &m, &mScale );
	D3DXMatrixMultiply( &m, &m, &mRotX );
	D3DXMatrixMultiply( &m, &m, &mRotY );
	D3DXMatrixMultiply( &m, &m, &mRotZ );
	D3DXVECTOR2 v = m_cnPos.m_vPos;
	D3DXVec2TransformCoord( &v, &v, &m );
	*pOutLx = v.x;
	*pOutLy = v.y;
}
void XLayerMove::Clear() 
{
	XBaseLayer::Clear();
	// 키에 의해서 변하는것들만 이곳에 넣는다. m_fAdjustX같은건 키에의해서 변하지 않으므로 넣지 않는다
	m_cnPos.Clear();
	m_cnRot.Clear();
	m_cnScale.Clear();
	m_cnEffect.Clear();
// 	memset( &m_cnPos, 0, sizeof(CHANNEL_POS) );
// 	memset( &m_cnEffect, 0, sizeof(CHANNEL_EFFECT) );
// 	memset( &m_cnRot, 0, sizeof(CHANNEL_ROT) );
// 	memset( &m_cnScale, 0, sizeof(CHANNEL_SCALE) );
// 	m_cnScale.vScale.Set(1.f);
// 	m_cnEffect.fAlpha = 1.0f;
// 	m_cnEffect.DrawMode = xDM_NORMAL;
}
