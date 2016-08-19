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
BOOL XLayerMove::CalcInterpolation( float *pfOut, float fTimeLerp
																	, xSpr::xtInterpolation inter )
{
	float fSpeedLerp = 0;
	if( pfOut )
		*pfOut = 0;
	switch( inter )
	{
	case xSpr::xNONE:		// 포지션키가 없을땐 none이 될수도 있다. 이럴댄 리니어로..
	case xSpr::xLINEAR:	fSpeedLerp = XE::xiLinearLerp( fTimeLerp );		break;
	case xSpr::xACCEL:		fSpeedLerp = XE::xiHigherPowerAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xDEACCEL:	fSpeedLerp = XE::xiHigherPowerDeAccel( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xSMOOTH:	fSpeedLerp = XE::xiSmoothStep( fTimeLerp, 1.0f, 0 );		break;
	case xSpr::xSPLINE:	fSpeedLerp = XE::xiCatmullrom( fTimeLerp, -10.0f, 0, 1, 1 );	break;
	default:
		XBREAK(1);	// none도 아닌상황
		fSpeedLerp = XE::xiLinearLerp( fTimeLerp );
		return FALSE;
	}
	if( pfOut )
		*pfOut = fSpeedLerp;
	return TRUE;
}

void XLayerMove::FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj )
{
	// 좌표 보간
	if( m_cnPos.interpolation && m_cnPos.pEndKey )	{	// 보간해야하고 다음키가 있을때
		// 보간
		XKeyPos *pStartKey = m_cnPos.pStartKey;
		XKeyPos *pEndKey = m_cnPos.pEndKey;
		XBREAK( pStartKey == nullptr );
//		XBREAK( pEndKey == nullptr );
		float fInterLength = pEndKey->GetfFrame() - pStartKey->GetfFrame();		// 보간구간의 전체 길이
		XBREAK( fInterLength == 0 );
		float fTimeLerp = (fFrmCurr - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간중 현재 플레잉중인 프레임의 위치(0~1)
		// 패스의 끝까지 왔다
		if( fTimeLerp >= 1.0f )
			fTimeLerp = 1.0f;
		// 속도보간
		float fSpeedLerp = 0;
		if( CalcInterpolation( &fSpeedLerp, fTimeLerp, m_cnPos.interpolation ) == FALSE )
			m_cnPos.interpolation = xSpr::xLINEAR;
		// 속도보간값에 따른 패스상의 현재 위치
		{	// fSlerp값이 전체패스상에서 어느위치인지 계산
			switch( m_cnPos.typePath ) {
			case SPR::xLINE_LINE: {
					if( pStartKey->GetpInterpolationEndKey() == pStartKey->GetpNextPathKey() ) {	// 보간끝키가 다음키와 같다면 단칸패스이므로 기존과 같이 계산
						XE::VEC2 vStart = pStartKey->GetPos();
						XE::VEC2 vEnd = pEndKey->GetPos();
						m_cnPos.vPos = vStart +((vEnd - vStart) * fSpeedLerp);		// 단칸짜리 패스는 기존과 같이 계산
					} else {
//						XBREAK( pStartKey->GetpNextPathKey() == nullptr );
						if( pStartKey->GetpNextPathKey() ) {
							// fSlerp가 어느키 구간에 걸려있는지 탐색
							XKeyPos *pCurrKey = pStartKey;
							float keyLerp=0, nextKeyLerp=0;
							while(1) {
								if( pCurrKey )
									keyLerp = (pCurrKey->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간에서의 현재키lerp값
								if( pCurrKey && pCurrKey->GetpNextPathKey() )
									nextKeyLerp = (pCurrKey->GetpNextPathKey()->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 다음키의 전체보간구간에서의 lerp값
								if( fSpeedLerp < nextKeyLerp )
									break;
								if( pCurrKey )
									pCurrKey = pCurrKey->GetpNextPathKey();
							}
							// pCurrKey ~ pNextKey구간을 0~1로 보고 fSlerp가 그중 어느위치인지 계산
							if( pCurrKey ) {
								XBREAK( nextKeyLerp - keyLerp == 0 );
								float fLerp = (fSpeedLerp - keyLerp) / (nextKeyLerp - keyLerp);	// 키구간사이에서의 lerp값
								XE::VEC2 v1 = pCurrKey->GetPos();
								XE::VEC2 v2 = pCurrKey->GetpNextPathKey()->GetPos();
								m_cnPos.vPos = v1 + (v2 - v1) * fLerp;		// 최종좌표
							}
						}
					}
				}
				break;
			case SPR::xLINE_SPLINE_CATMULLROM: {
					D3DXVECTOR2 v0, v1, v2, v3, vOut;
					float fLerp = fSpeedLerp;
					// 스플라인보간값을 계산하기위해 v0, v1, v2, v3좌표를 얻는다
					if( pStartKey->GetpNextPathKey() ) {	// 시작키의 다음키가 없으면 보간하지 않음
//						if( pStartKey->GetpNextPathKey() == pEndKey )		
						if( pStartKey->GetpInterpolationEndKey() == pStartKey->GetpNextPathKey() ) {
							// 스플라인타입인데 단칸짜리
							v0 = ( pStartKey->GetpPrevPathKey() )? pStartKey->GetpPrevPathKey()->GetPos() : pStartKey->GetPos();
							v1 = pStartKey->GetPos();
							v2 = pEndKey->GetPos();
							v3 = ( pEndKey->GetpNextPathKey() )? pEndKey->GetpNextPathKey()->GetPos() : pEndKey->GetPos();
						} else {
							// 스플라인타입 다구간 보간
							// 현재 fSpeedLerp가 스플라인구간중에서 어느구간인지 찾아낸다
							XKeyPos *pCurrKey = pStartKey;
							float keyLerp=0, nextKeyLerp=0;
							while(1) {
								if( XBREAK( pCurrKey->GetpNextPathKey() == nullptr ) )
									break;
								keyLerp = (pCurrKey->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 전체 보간구간에서의 현재키lerp값
								nextKeyLerp = (pCurrKey->GetpNextPathKey()->GetfFrame() - pStartKey->GetfFrame()) / fInterLength;		// 다음키의 전체보간구간에서의 lerp값
								if( fSpeedLerp <= nextKeyLerp )
									break;
								pCurrKey = pCurrKey->GetpNextPathKey();
							}
							// pCurrKey ~ pNextKey구간을 0~1로 보고 fSpeedLerp가 그중 어느위치인지 계산
							{
								XBREAK( nextKeyLerp - keyLerp == 0 );
								fLerp = (fSpeedLerp - keyLerp) / (nextKeyLerp - keyLerp);	// 키구간사이에서의 lerp값
								v0 = ( pCurrKey->GetpPrevPathKey() )? pCurrKey->GetpPrevPathKey()->GetPos() : pCurrKey->GetPos();
								v1 = pCurrKey->GetPos();
								v2 = ( pCurrKey->GetpNextPathKey() )? pCurrKey->GetpNextPathKey()->GetPos() : pCurrKey->GetPos();
								if( pCurrKey->GetpNextPathKey() ) {
									if( pCurrKey->GetpNextPathKey()->GetpNextPathKey( pCurrKey->GetidPath() ) )
										v3 = pCurrKey->GetpNextPathKey()->GetpNextPathKey( pCurrKey->GetidPath() )->GetPos();
									else
										v3 = pCurrKey->GetpNextPathKey()->GetPos();
								} else
									v3 = pCurrKey->GetPos();
							}
						}
						D3DXVec2CatmullRom( &vOut, &v0, &v1, &v2, &v3, fLerp );
						m_cnPos.vPos = vOut;		// 스플라인위에서의 최종 좌표
					}
				}
				break;
			} // switch
		} // if( CalcInterpolation( &fSpeedLerp, fTimeLerp, m_cnPos.interpolation ) == FALSE )
		// 패스의 끝까지 왔으면 패스보간모드 끝
		if( fTimeLerp >= 1.0f )
			m_cnPos.interpolation = xSpr::xNONE;		
	} else {// if( m_cnPos.interpolation && m_cnPos.pEndKey )	{	// 보간해야하고 다음키가 있을때
		// 이경우는 키에서 직접 좌표를 입력한다.
	}
// 	} else { // cnPos
// 		// 보간이 없거나 다음키가 없을땐 첫번째 키의 좌표를 유지한다.
// 		if( m_cnPos.pStartKey )
// 			m_cnPos.vPos = m_cnPos.pStartKey->GetPos();
// 	}
	// 알파보간
	if( m_cnEffect.interpolation ) {		// 
		float fFrmLength = m_cnEffect.fNextKeyFrame - m_cnEffect.fStartKeyFrame;
		XBREAK( fFrmLength == 0 );
		float fLerp = (fFrmCurr - m_cnEffect.fStartKeyFrame) / fFrmLength;
		float fSlerp = 0;
		switch( m_cnEffect.interpolation ) {
		case xSpr::xNONE:		// 키가 없을땐 none이 될수도 있다. 이럴댄 리니어로..
		case xSpr::xLINEAR:	fSlerp = XE::xiLinearLerp( fLerp );		break;
		case xSpr::xACCEL:		fSlerp = XE::xiHigherPowerAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xDEACCEL:	fSlerp = XE::xiHigherPowerDeAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xSMOOTH:	fSlerp = XE::xiSmoothStep( fLerp, 1.0f, 0 );		break;
		case xSpr::xSPLINE:	fSlerp = XE::xiCatmullrom( fLerp, -10.0f, 0, 1, 1 );	break;
		default:
			XBREAK(1);	// none도 아닌상황
			m_cnEffect.interpolation = xSpr::xLINEAR;
		}
		if( fSlerp >= 0 ) {
			m_cnEffect.fAlpha 
				= m_cnEffect.fAlphaSrc + ((m_cnEffect.fAlphaDest - m_cnEffect.fAlphaSrc) * fSlerp);
			if( fLerp >= 1.0f )
				m_cnEffect.interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	} else {
	}
	// 회전보간
	if( m_cnRot.interpolation ) {
		float fFrmLength = m_cnRot.fNextKeyFrame - m_cnRot.fStartKeyFrame;
		XBREAK( fFrmLength == 0 );
		float fLerp = (fFrmCurr - m_cnRot.fStartKeyFrame) / fFrmLength;
		float fSlerp = 0;
		switch( m_cnRot.interpolation )
		{
		case xSpr::xNONE:		// 키가 없을땐 none이 될수도 있다. 이럴댄 리니어로..
		case xSpr::xLINEAR:	fSlerp = XE::xiLinearLerp( fLerp );		break;
		case xSpr::xACCEL:		fSlerp = XE::xiHigherPowerAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xDEACCEL:	fSlerp = XE::xiHigherPowerDeAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xSMOOTH:	fSlerp = XE::xiSmoothStep( fLerp, 1.0f, 0 );		break;
		case xSpr::xSPLINE:	fSlerp = XE::xiCatmullrom( fLerp, -10.0f, 0, 1, 1 );	break;
		default:
			XBREAK(1);	// none도 아닌상황
			m_cnRot.interpolation = xSpr::xLINEAR;
		}
		if( fSlerp >= 0 ) {
			m_cnRot.fAngle = m_cnRot.fAngleSrc + ((m_cnRot.fAngleDest - m_cnRot.fAngleSrc) * fSlerp); 
			if( fLerp >= 1.0f )
				m_cnRot.interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	} else {
		// 회전 보간이 없을때
		m_cnRot.fAngle = m_cnRot.fAngleSrc;
	}
	// 스케일 보간
	if( m_cnScale.interpolation ) {		// 보간
		float fFrmLength = m_cnScale.fNextKeyFrame - m_cnScale.fStartKeyFrame;
		XBREAK( fFrmLength == 0);
		float fLerp = (fFrmCurr - m_cnScale.fStartKeyFrame) / fFrmLength;
		float fSlerp = 0;
		switch( m_cnScale.interpolation ) {
		case xSpr::xNONE:		// 키가 없을땐 none이 될수도 있다. 이럴댄 리니어로..
		case xSpr::xLINEAR:	fSlerp = XE::xiLinearLerp( fLerp );		break;
		case xSpr::xACCEL:		fSlerp = XE::xiHigherPowerAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xDEACCEL:	fSlerp = XE::xiHigherPowerDeAccel( fLerp, 1.0f, 0 );		break;
		case xSpr::xSMOOTH:	fSlerp = XE::xiSmoothStep( fLerp, 1.0f, 0 );		break;
		case xSpr::xSPLINE:	fSlerp = XE::xiCatmullrom( fLerp, -10.0f, 0, 1, 1 );	break;
		default:
			XBREAK(1);
			m_cnScale.interpolation = xSpr::xLINEAR;
		}
		if( fSlerp >= 0 ) {
			m_cnScale.vScale = m_cnScale.vScaleSrc + ((m_cnScale.vScaleDest - m_cnScale.vScaleSrc) * fSlerp );
			if( fLerp >= 1.0f )
				m_cnScale.interpolation = xSpr::xNONE;		// 묙표지점까지 왔으니 보간모드 끝
		}
	}
	// 현재 좌표,회전,스케일의 매트릭스 구함
	auto vAdjAxis = GetvAdjustAxis();
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
	D3DXMatrixTranslation( &mAxis, -vAdjAxis.x, -vAdjAxis.y, 0 );
	D3DXMatrixScaling( &mScale, GetcnScale().vScale.x, GetcnScale().vScale.y, 1.0f );
	D3DXMatrixRotationYawPitchRoll( &mRot, 0, 0, D3DXToRadian( GetcnRot().fAngle ) );
	D3DXMatrixTranslation( &mReverseAxis, vAdjAxis.x, vAdjAxis.y, 0 );
	D3DXMatrixTranslation( &mTrans, GetcnPos().vPos.x, GetcnPos().vPos.y, 0 );
	m_mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans * pSprObj->GetMatrix();	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김 * 마지막으로 부모곱함
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
		pNewKey = spAction->AddKeyPos( fFrame, GetThis(), GetcnPos().vPos );
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
	D3DXVECTOR2 v = m_cnPos.vPos;
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
