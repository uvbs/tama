/*
 *  Key.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "etc/Types.h"
#include "Key.h"
#include "SprObj.h"
#include "SprDat.h"
#include "XBaseObj.h"
#include "XResObj.h"
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


#ifdef _XSPR_LOAD2
XBaseKey* XBaseKey::sCreateLoad( XBaseRes* pRes, XSprDat* pSprDat, int verSpr )
{
	char c0;
	XBaseRes& res = (*pRes);
	ID snKey = 0;
	xSUBTYPE typeSub = xSUB_MAIN;
	float secFrame;
	//
	if( verSpr >= 18 ) {
		res >> snKey;
	}
	res >> c0;		auto type = (xTYPE)c0;
	XBREAK( XE::IsInvalidKeyType( type ) );
	if( verSpr >= 26 ) {
		res >> c0;	typeSub = (xSUBTYPE)c0;
	}
	res >> secFrame;
	ID idLocalInLayer = 0;
	xSpr::xtLayer typeLayer = xSpr::xLT_NONE;

	if( verSpr >= 29 ) {
		res >> idLocalInLayer;
		XBREAK( idLocalInLayer == 0 );
		WORD w0;
		res >> w0;	typeLayer = XBaseLayer::sConvertNewToOld( w0 );
	} else {
		res >> c0;	typeLayer = (xSpr::xtLayer)c0;
	}
	ID idLayer = 0;
//	short s0;
	res >> c0;		const int nLayer = c0;
	if( verSpr >= 26 ) {
		res >> idLayer;
		XBREAK( idLayer == 0 );
	} else {
		// 이전버전 파일은 idLayer가 없기때문에 레이어 타입과 번호로 찾아야 한다.
	}
	res >> c0;
	if( verSpr < 26 )
		typeSub = (xSUBTYPE)c0;
	bool bLuaIncluded = false;
	if( verSpr >= 17 ) {
		int len;
		res >> len;					// 루아코드 길이. 널포함안된거임. 젠장
		if( len > 0 ) {
			res.Seek( len + 1 );		// 게임에선 합쳐진 루아가 있으므로 개별루아는 읽지 않는다
			bLuaIncluded = true;		// 읽지는 않았지만 이 키에 루아코드가 있다
		}
	}
	//
	XBaseKey* pKey = sCreate( snKey, type, typeSub, idLayer, idLocalInLayer, verSpr );
	if( XBREAK(pKey == nullptr) )
		return nullptr;
// 	if( verSpr >= 29 ) {
// 		XBREAK( pKey->GetidLocalInLayer() == 0 );
// 	} else {
// 		// 이하버전은 XAniAction::Load에서 넣어준다.
// 	}
	//
	pKey->m_idKey = snKey;
	pKey->m_idLayer = idLayer;
	pKey->m_idLocalInLayer = idLocalInLayer;
	pKey->m_Type = type;
	pKey->m_SubType = typeSub;
	pKey->m_fFrame = secFrame;
	pKey->m_LayerType = typeLayer;
	pKey->m_nLayer = nLayer;
	pKey->m_bLua = bLuaIncluded;
	// virtual load
	pKey->Load( pSprDat, pRes );
	return pKey;
}

XBaseKey* XBaseKey::sCreate( ID snKey, xTYPE type, xSUBTYPE typeSub, ID idLayer, ID idLocalInLayer, int verSpr )
{
	switch( type ) {
	case XBaseKey::xKEY_IMAGE:
	case XBaseKey::xKEY_CREATEOBJ:
	case XBaseKey::xKEY_DUMMY: {
		switch( typeSub ) {
		case XBaseKey::xSUB_MAIN: {
			switch( type ) {
			case XBaseKey::xKEY_IMAGE:
				return new XKeyImage();
			case XBaseKey::xKEY_CREATEOBJ:
				return new XKeyCreateObj();
			case XBaseKey::xKEY_DUMMY:
				return new XKeyDummy();
			default:
				XBREAK(1);
				break;
			}
		} break;
		case XBaseKey::xSUB_POS:
			return new XKeyPos();
		case XBaseKey::xSUB_EFFECT:
			return new XKeyEffect();
		case XBaseKey::xSUB_ROT:
			return new XKeyRot();
		case XBaseKey::xSUB_SCALE:
			return new XKeyScale();
		default:
			XBREAKF( 1, "알수없는 서브키타입: %d", typeSub );
			break;
		}
	} break;
	case XBaseKey::xKEY_SOUND:
		return new XKeySound();
	case XBaseKey::xKEY_EVENT:
		return new XKeyEvent();
	default:
		XBREAKF( 1, "알수없는 키타입:%d", type );
		break;
	}
	return nullptr;
}
#else
/**
 @brief 
*/
void XBaseKey::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	if( pSprDat->IsUpperVersion(18) )
		pRes->Read( &m_idKey, 4 );
	BYTE bData, b2;
	pRes->Read( &bData, 1 );  m_Type = (XBaseKey::xTYPE)bData;	// key type;
	XBREAK( XE::IsInvalidKeyType(m_Type) );
	if( pSprDat->IsUpperVersion(26) ) {
		pRes->Read( &bData, 1 ); m_SubType = (XBaseKey::xSUBTYPE)bData;
	}
	pRes->Read( &m_fFrame, 4 );				// key frame
//	ID idLocalInLayer = 0;
	if( pSprDat->IsUpperVersion(29) ) {
		pRes->Read( &m_idLocalInLayer, 4 );
		XBREAK( m_idLocalInLayer == 0 );
	} else {

	}
	if( pSprDat->IsUpperVersion(29) ) {
		WORD w0;
		pRes->Read( &w0, 2 );				// layer type(2바이트로 바뀜)
		m_LayerType = XBaseLayer::sConvertNewToOld( w0 );
	} else {
		pRes->Read( &bData, 1 );				// layer type
		m_LayerType = (xSpr::xtLayer)bData;
	}
	pRes->Read( &b2, 1 );					// layer num
	m_nLayer = b2;
	if( pSprDat->IsUpperVersion(26) ) {
		pRes->Read( &m_idLayer, 4 );	// 이값으로 레이어를 검색하게 바꿔야 함.
		XBREAK( m_idLayer == 0 );
	} else {
		// 이전버전 파일은 idLayer가 없기때문에 레이어 타입과 번호로 찾아야 한다.
	}
	pRes->Read( &bData, 1 );	// sub type
	if( pSprDat->IsLowerVersion(26) )
		m_SubType = (XBaseKey::xSUBTYPE)bData;
	if( pSprDat->IsUpperVersion(17) ) {
		int len;
		pRes->Read( &len, 4 );		// 루아코드 길이. 널포함안된거임. 젠장
		if( len > 0 ) {
			pRes->Seek( len+1 );		// 게임에선 합쳐진 루아가 있으므로 개별루아는 읽지 않는다
			m_bLua = TRUE;			// 읽지는 않았지만 이 키에 루아코드가 있다
		}
	}
}
#endif // _XSPR_LOAD2
//////////////////////////////////////////////////////////////////////////
void XBaseKey::Execute( XSprObj *pSprObj, float fOverSec )
{
	if( GetbLua() )
	{
		// lua Execute()핸들러 호출
#ifdef _SPR_USE_LUA
		XLua *pLua = pSprObj->GetpLua();
		XBREAK( pLua == NULL );
		char cTable[ 64 ];		// 최적화하려면 이거 사전 생성해놓도록.
		sprintf_s( cTable, "key_%08x", (int)GetidKey() );		// 테이블 이름 생성
		if( pLua->IsHaveMemberFunc( cTable, "Execute" ) )	
			pLua->MemberCall<int>( cTable, "Execute", pSprObj->GetpDelegate() );
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyImageLayer::Execute( XSprObj *pSprObj, float fOverSec )
{
	XBaseKey::Execute( pSprObj );
#ifdef _XDEBUG
	XBaseLayer *pLayer = pSprObj->GetLayer(GetLayerType(), GetnLayer());
	if( pLayer == NULL ) {
		XLOG( "IMAGE_LAYER %d를 찾을 수 없다", GetnLayer() );
		return;
	}
	if( pLayer->GetType() != xSpr::xLT_IMAGE ) {
		XLOG( "레이어타입이 IMAGE_LAYER가 아니다. %d", (int)pLayer->GetType() );
		return;
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyObjLayer::Execute( XSprObj *pSprObj, float fOverSec )
{
	XBaseKey::Execute( pSprObj );
	XBaseLayer *pLayer = pSprObj->GetLayer(GetLayerType(), GetnLayer());
	if( pLayer == NULL ) {
		XLOG( "OBJ_LAYER %d를 찾을 수 없다", GetnLayer() );
		return;
	}
	if( pLayer->GetType() != xSpr::xLT_OBJ ) {
		XLOG( "레이어타입이 OBJ_LAYER가 아니다. %d", (int)pLayer->GetType() );
		return;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyImage::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	pRes->Read( &m_nSpr, 4 );
#ifdef _XSPR_LOAD2
	m_pSprite = pSprDat->GetSprite( m_nSpr );
#endif // _XSPR_LOAD2
}
void XKeyImage::Execute( XSprObj *pSprObj, float fOverSec )
{
	XKeyImageLayer::Execute( pSprObj, fOverSec );
	XLayerImage *pLayer = (XLayerImage *)pSprObj->GetLayer(GetLayerType(), GetnLayer());
  if( pLayer )
	  pLayer->SetpSpriteCurr( m_pSprite );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyPos::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	pRes->Read( &m_vPosOrig.x, 4 );
	pRes->Read( &m_vPosOrig.y, 4 );
	if( pSprDat->IsUpperVersion(30) ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRangeRandomX.v1 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeRandomX.v2 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeRandomY.v1 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeRandomY.v2 = (float)s0;
		pRes->Read( &s0, 2 );		m_radiusRandom = (float)s0;
	}
	BYTE bData;
	pRes->Read( &bData, 1 );
	m_Interpolation = (XBaseLayer::xtInterpolation)bData;			
	if( pSprDat->IsUpperVersion(21) ) {
		DWORD dw=0;
		pRes->Read( &dw, 4 );							// idPath(skip)
		pRes->Read( &dw, 4 ); //pRes->Read( &m_idxNextPathKey, 4 );		// idxNext
		pRes->Read( &dw, 4 );							// idNext(skip)
		pRes->Read( &dw, 4 ); //pRes->Read( &m_idxPrevPathKey, 4 );		// idxPrev
		pRes->Read( &dw, 4 );							// idPrev(skip)
		pRes->Read( &m_idxInterpolationEndKey, 4 );	// idxEndKey
		pRes->Read( &dw, 4 );							// idEndKey(skip)
		pRes->Read( &dw, 4 );		//m_PathType = (SPR::xtLINE)dw; // path type
		pRes->Read( &dw, 4 );		// color(skip)
		//
		pRes->Read( &dw, 4 );		// reserved
		pRes->Read( &dw, 4 );
		pRes->Read( &dw, 4 );
		pRes->Read( &dw, 4 );
	} else {
		XBREAK( pSprDat->IsUpperVersion(20) );	// 이거 로딩못함 SE에서 다시 저장할것
		DWORD dwData=0;
		pRes->Read( &dwData, 4 );			// m_ox
		pRes->Read( &dwData, 4 );			// m_oy
		float fData;
		pRes->Read( &fData, 4 );			//  m_fNextKeyFrame
		if( pSprDat->IsLowerVersion(15) ) {
			m_vPosOrig *= 0.5f;
		}
	}
}

XE::VEC2 XKeyPos::AssignRandom() const
{
	if( m_radiusRandom ) {
		return m_vPosOrig + XE::GetAngleVector( xRandomF(360.f), m_radiusRandom );
	} else {
		auto vPosRandomed = m_vPosOrig;
		if( !m_vRangeRandomX.IsZero() )
			vPosRandomed.x += xRandomF( m_vRangeRandomX.v1, m_vRangeRandomX.v2 );
		if( !m_vRangeRandomY.IsZero() )
			vPosRandomed.y += xRandomF( m_vRangeRandomY.v1, m_vRangeRandomY.v2 );
		return vPosRandomed;
	}
	return m_vPosOrig;
}


void XKeyPos::Execute( XSprObj *pSprObj, float fOverSec )
{
	auto pActObj = pSprObj->GetpObjActCurr();
	if( XASSERT( pActObj ) ) {
		XLayerMove* pLayer = nullptr;
		if( GetidLayer() ) {
			pLayer = SafeCast<XLayerMove*>( pActObj->GetpLayerByidLocalInLayer( GetidLayer() ) );
		} else {
			pLayer = SafeCast<XLayerMove*>( pSprObj->GetLayer( GetLayerType(), GetnLayer() ) );
		}
// 		auto pLayer = GetpLayerByidLayerT<XLayerMove>( pSprObj );
		if( XASSERT( pLayer ) ) {
			auto& cn = pLayer->GetcnPos();
			if( m_pInterpolationEndKey ) {
				cn.interpolation = m_Interpolation;
				cn.m_frameStartKey = GetfFrame();
				cn.m_vStart = GetvPosRandomed();
				cn.m_frameEndKey = m_pInterpolationEndKey->GetfFrame();
				cn.m_vEnd = m_pInterpolationEndKey->GetvPosRandomed();
			} else {
				// 다음키가 없다는것은 보간옵션이 켜져있어도 보간할게 없다는뜻임.
				cn.interpolation = XBaseLayer::xNONE;
				cn.m_frameStartKey = GetfFrame();
				cn.m_vStart = GetvPosRandomed();
// 				cn.m_frameEndKey = GetfFrame();
			}
			m_vPosRandomed = AssignRandom();
		} // pLayer
	} // pActObj
// 	auto pActObj = pSprObj->GetpObjActCurr();
// 	if( XASSERT( pActObj ) ) {
// 		auto pLayer = GetpLayerByidLayerT<XLayerMove>( pSprObj );
// 		if( XASSERT( pLayer ) ) {
// 			if( m_Interpolation ) {
// 				XKeyPos *pInterEndKey = m_pInterpolationEndKey;
// 				if( pInterEndKey ) {
// 					pLayer->SetcnPos( this, pInterEndKey, m_PathType, m_Interpolation );
// 				} else {
// 					if( m_pPrevPathKey == NULL ) {	// path's head
// 						pLayer->SetcnPos( NULL, NULL, SPR::xLINE_NONE, XBaseLayer::xNONE );
// 						pLayer->Setxy( m_x, m_y );
// 					} else {
// 						// 보간구간의 중간키는 암것도 안함
// 					}
// 				}
// 			} else {
// 				pLayer->SetcnPos( NULL, NULL, SPR::xLINE_NONE, XBaseLayer::xNONE );		// 레이어에 패스세팅을 클리어함
// 				pLayer->Setxy( m_x, m_y );		
// 			}
// 		}
// 	}
}
// 키로딩이 끝나면 idx키들의 실제 포인터를 구해야 한다
void XKeyPos::OnFinishLoad( XAniAction *pAction )
{
	if( m_idxInterpolationEndKey >= 0 )	{// 키의 인덱스
		m_pInterpolationEndKey = SafeCast<XKeyPos*>( pAction->GetKey( m_idxInterpolationEndKey ) );	
		XBREAK( m_pInterpolationEndKey == nullptr );
	}
// 	if( m_idxNextPathKey >= 0 )
// 	{
// 		m_pNextPathKey = SafeCast<XKeyPos*, XBaseKey*>( pAction->GetKey( m_idxNextPathKey ) );	XBREAK( m_pNextPathKey == NULL );
// 	}
// 	if( m_idxPrevPathKey >= 0 )
// 	{
// 		m_pPrevPathKey = SafeCast<XKeyPos*, XBaseKey*>( pAction->GetKey( m_idxPrevPathKey ) );	XBREAK( m_pPrevPathKey == NULL );
// 	}
}

float XKeyEffect::GetOpacityRandomed() 
{
	if( m_fOpacityRandomed == 9999.f )
		m_fOpacityRandomed = AssignRandom();
	return m_fOpacityRandomed;
}

void XKeyEffect::Execute( XSprObj *pSprObj, float fOverSec )
{
	auto pLayer = pSprObj->GetpLayerMove( GetLayerType(), GetnLayer() );
	auto& cn = pLayer->GetcnEffect();
	cn.interpolation = m_Interpolation;
	cn.fAlphaSrc = GetOpacityRandomed();
	cn.fStartKeyFrame = GetfFrame();
	auto pNextKey = m_pNextKey;
	if( pNextKey ) {
		cn.fAlphaDest = pNextKey->GetOpacityRandomed();
		cn.fNextKeyFrame = pNextKey->GetfFrame();
	} else {
		cn.interpolation = XBaseLayer::xNONE;
		cn.fNextKeyFrame = GetfFrame();
	}
	cn.DrawMode = m_DrawMode;
	cn.dwDrawFlag = m_dwFlag;
	m_fOpacityRandomed = AssignRandom();
// 	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
// 	pLayer->GetcnEffect().fAlpha = m_fOpacity;
// 	XKeyEffect *pNextKey = m_pNextKey;
// 	
// 	if( m_Interpolation && pNextKey )			// 다음키까지 보간모드로 변해야 하고 다음키가 있으면
// 		pLayer->SetcnEffect( m_Interpolation, pNextKey->GetfOpacity(), m_fOpacity, m_dwFlag, m_DrawMode, GetfFrame(), pNextKey->GetfFrame() );
// 	else
// 		pLayer->SetcnEffect( XBaseLayer::xNONE, 0, 0, m_dwFlag, m_DrawMode, 0, 0 );
}
void XKeyEffect::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	BYTE b1;
	if( pSprDat->IsUpperVersion(31) ) {
		short s0;
		pRes->Read( &b1, 1 );		m_Interpolation = ( XBaseLayer::xtInterpolation )b1;
		pRes->Read( &b1, 1 );		m_DrawMode = (xDM_TYPE)b1;
		pRes->Read( &s0, 2 );		m_fOpacityOrig = ((float)s0 / 100.f);
		pRes->Read( &m_dwFlag, 4 );
		pRes->Read( &s0, 2 );		m_vRangeRandomed.v1 = ( (float)s0 / 100.f );
		pRes->Read( &s0, 2 );		m_vRangeRandomed.v2 = ( (float)s0 / 100.f );
	} else {
		pRes->Read( &b1, 1 );		m_Interpolation = ( XBaseLayer::xtInterpolation )b1;
		pRes->Read( &m_dwFlag, 4 );
		pRes->Read( &b1, 1 );		m_DrawMode = (xDM_TYPE)b1;
		pRes->Read( &m_fOpacityOrig, 4 );
		DWORD dw1;
		pRes->Read( &dw1, 4 );		// reserved
	}
}
///////////////// Key Image Rot /////////////////////////////////////////
void XKeyRot::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = (XBaseLayer::xtInterpolation)b1;
	if( pSprDat->IsUpperVersion(31) ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRangeRandom.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandom.v2 = (float)s0 / 100.f;
	} else {
		pRes->Read( &dw1, 4 );
	}
	pRes->Read( &dw1, 4 );
	pRes->Read( &m_dAngleOrig, 4 );
}
void XKeyRot::Execute( XSprObj *pSprObj, float fOverSec )
{
	auto pLayer = pSprObj->GetpLayerMove( GetLayerType(), GetnLayer() );
	auto& cn = pLayer->GetcnRot();
	cn.fAngleSrc = GetdAngRandomed();
	cn.fStartKeyFrame = GetfFrame();
	if( m_pNextKey ) {
		cn.interpolation = m_Interpolation;
		cn.fAngleDest = m_pNextKey->GetdAngRandomed();
		cn.fNextKeyFrame = m_pNextKey->GetfFrame();
	} else {
		cn.interpolation = XBaseLayer::xNONE;
	}
	m_dAngleRandomed = AssignRandom();		// 새로운 랜덤값을 만들어냄.
// 	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
// 	pLayer->SetAngleZ( m_fAngleZ );
// 	XKeyRot *pNextKey = m_pNextKey;
// 	
// 	if( m_Interpolation && pNextKey )			// 다음키가 있으면 보간으로 움직이도록 한다
// 		pLayer->SetcnRot( m_Interpolation, pNextKey->GetfAngleZ(), m_fAngleZ, GetfFrame(), pNextKey->GetfFrame() );
// 	else
// 		pLayer->SetcnRot( XBaseLayer::xNONE, 0, 0, 0, 0 );
}
///////////////// Key Image Scale /////////////////////////////////////////
void XKeyScale::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = (XBaseLayer::xtInterpolation)b1;
	if( pSprDat->IsUpperVersion(31) ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRangeRandomX.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandomX.v2 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandomY.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandomY.v2 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandomXY.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRangeRandomXY.v2 = (float)s0 / 100.f;
	} else {
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
	}
	pRes->Read( &m_vScaleOrig.x, 4 );
	pRes->Read( &m_vScaleOrig.y, 4 );
}

XE::VEC2 XKeyScale::AssignRandom() const
{
	if( !m_vRangeRandomXY.IsZero() ) {
		const float rangeRandom = xRandomF( m_vRangeRandomXY.v1, m_vRangeRandomXY.v2 );
		return m_vScaleOrig + XE::VEC2( rangeRandom, rangeRandom );
	} else {
		auto vRandomed = m_vScaleOrig;
		if( !m_vRangeRandomX.IsZero() ) {
			vRandomed.x += xRandomF( m_vRangeRandomX.v1, m_vRangeRandomX.v2 );
		}
		if( !m_vRangeRandomY.IsZero() ) {
			vRandomed.y += xRandomF( m_vRangeRandomY.v1, m_vRangeRandomY.v2 );
		}
		return vRandomed;
	}
	return m_vScaleOrig;
}

void XKeyScale::Execute( XSprObj *pSprObj, float fOverSec )
{
	auto pLayer = pSprObj->GetpLayerMove( GetLayerType(), GetnLayer() );
	auto& cn = pLayer->GetcnScale();
	cn.m_vScaleSrc = GetvScaleRandomed();
	cn.fStartKeyFrame = GetfFrame();
	if( m_pNextKey ) {
		cn.interpolation = m_Interpolation;
		cn.m_vScaleDest = m_pNextKey->GetvScaleRandomed();
		cn.fNextKeyFrame = m_pNextKey->GetfFrame();
	} else {
		cn.interpolation = XBaseLayer::xNONE;
	}
	m_vScaleRandomed = AssignRandom();
// 	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
// 	pLayer->SetScale( m_fScaleX, m_fScaleY );
// 
// 	XKeyScale *pNextKey = m_pNextKey;
// 	
// 	if( m_Interpolation && pNextKey )			// 다음키가 있으면 보간으로 움직이도록 한다
// 		pLayer->SetcnScale( m_Interpolation, pNextKey->GetfScaleX(), pNextKey->GetfScaleY(), GetfScaleX(), GetfScaleY(), GetfFrame(), pNextKey->GetfFrame() );
// 	else
// 		pLayer->SetcnScale( XBaseLayer::xNONE, 1.0f, 1.0f, 1.0f, 1.0f, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////
void XKeyCreateObj::Execute( XSprObj *pSprObj, float fOverSec )
{
	// base class
	XKeyObjLayer::Execute( pSprObj, fOverSec );
	//
	XLayerObject *pLayer = (XLayerObject *)pSprObj->GetpLayerObj( GetLayerType(), GetnLayer() );
	if( XBREAK(pLayer == nullptr) )
		return;
	XSprObj *pNewSprObj = NULL;
	// 루아코드가 있으면 실행한다
	if( GetbLua() )	{
#ifdef _SPR_USE_LUA
		XLua* pLua = pSprObj->GetpLua();
		char cTable[ 64 ];
		sprintf_s( cTable, "key_%08x", (unsigned int)GetidKey() );		// 테이블 이름 생성
		// AddSprObj핸들러
		if( pLua->IsHaveMemberFunc( cTable, "CreateSprObj" ) )		// 핸들러 호출
			// 원래라면 m_szFilename을 컨버트 해서 넘겨줘야한다
			pNewSprObj = pLua->MemberCall<XSprObj*>( cTable, "CreateSprObj", 0, m_nAction, m_PlayType, 
													GetdwID(), 
													pSprObj, pSprObj->GetpDelegate() );		// SprObj생성요청 핸들러 호출
		else
			XBREAKF( 1, "spr:%s: key id=0x%08x: CreateSprObj호출 실패", pSprObj->GetSprFilename(), GetidKey() );
#endif // not use
	} else {
		const ID idAct = pSprObj->GetActionID();
		const ID idLayer = pLayer->GetidLayer();
		pNewSprObj = pSprObj->AddSprObj( GetszSprName()
																	, m_nAction
																	, m_PlayType
																	, GetdwID()
																	, -1
																	, idAct
																	, idLayer );		// 루아에서 0보다 큰수를 아이디를 사용하므로 겹치지 않게 하기 위해 -1을 씀
	}
	if( pNewSprObj )
		pNewSprObj->SetpParentSprObj( pSprObj );
	//
	//
	pLayer->SetpSprObjCurr( pNewSprObj );								// 오브젝트레이어에 추가한 차일드를 기억시켜둔다. 
}

void XKeyCreateObj::Destroy( void )
{
}
void XKeyCreateObj::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	pRes->Read( &m_dwID, 4 );
	WORD w1;
	UNICHAR sztSprName[256];
	pRes->Read( &w1, 2 );						// string 길이
	pRes->Read( sztSprName, w1 );
#ifdef WIN32
	_tcscpy_s( m_szSprName, sztSprName );
#else
    char buff[64];
	strcpy( m_szSprName, _ConvertUTF16ToUTF8(buff, sztSprName) );
#endif
	pRes->Read( &m_nAction, 4 );
	if( pSprDat->IsUpperVersion(9) )
	{
		DWORD dw1 = 0;
		pRes->Read( &dw1, 4 );	// playtype
		m_PlayType = (xRPT_TYPE)dw1;
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );	// rotate
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );	// scale
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );	// etc
	}
	
}

///////////////////// Key Event //////////////////////////////////////
void XKeyEvent::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	BYTE b1;
	pRes->Read( &b1, 1 );		m_Event = (KE::xTYPE)b1;
	pRes->Read( &m_lx, 4 );
	pRes->Read( &m_ly, 4 );
	pRes->Read( m_Param, sizeof(KE::KEY_PARAM), sizeof(m_Param) / sizeof(KE::KEY_PARAM) );
	if( pSprDat->IsLowerVersion( 15 ) ) {
		m_lx *= 0.5f;
		m_ly *= 0.5f;
	}
	if( pSprDat->IsUpperVersion( 23 ) ) {
		char cBuff[256] = {0, };
		int len = 0;
		pRes->Read( &len, 4 );
		if( len > 0 ) {
			pRes->Read( cBuff, sizeof( char ), len );		// spr filename
			m_strSpr = C2SZ( cBuff );
			if( XE::IsEmpty( XE::GetFileExt( m_strSpr.c_str() ) ) )
				m_strSpr += _T( ".spr" );
		}
	}
	if( pSprDat->IsUpperVersion(33) ) {
		(*pRes) >> m_aryStrParam;
	}
}
void XKeyEvent::Execute( XSprObj *pSprObj, float fOverSec )
{
	//
	XBaseKey::Execute( pSprObj, fOverSec );
	//
	XDelegateSprObj *pDelegate = pSprObj->GetpDelegate();
	if( pDelegate ) {
		const ID idObj = GeteventidObj();//m_Param[0].w[0];
		const float fAngle = GeteventdAng(); // m_Param[1].fParam;
		const float lx = m_lx;
		const float ly = m_ly;
//		pSprObj->Transform( &lx, &ly );
//		pSprObj->Transform( &fAngle );
    pSprObj->SetbCallHandler( true );   // 콜백 실행중.
		const float scale = GetScale();
		xSpr::xEvent event;
		event.m_strSpr = m_strSpr.c_str();
		event.m_pSprObj = pSprObj;
		event.m_pKey = this;
		event.m_vLocal = XE::VEC2( lx, ly );
		event.m_idEvent = idObj;
		event.m_dAng = fAngle;
		event.m_fOverSec = fOverSec;
		event.m_Scale = scale;
		event.m_bTraceParent = GetbTraceParent() != FALSE;
		event.m_secLifeTime = GetsecLifeTime();
		event.m_idAct = GetidAct();
		event.m_aryStrParam = m_aryStrParam;
		if( m_Event == KE::CREATE_SFX ) {
			pDelegate->OnEventCreateSfx( pSprObj, this, lx, ly, scale,
										m_strSpr.c_str(), GetidAct(), 
										(xRPT_TYPE)GettypeLoop(), GetsecLifeTime(), 
										GetbTraceParent(),
										fAngle, fOverSec );
		} else 
		if( m_Event == KE::HIT ) {
			// 정식 타격 이벤트
			pDelegate->OnEventHit( event );
			pDelegate->OnEventSprObj( pSprObj, this, lx, ly, idObj, fAngle, fOverSec );
		} else {
			// 새로운 기타이벤트 핸들러
			pDelegate->OnEventEtc( event );
			// 현재 "기타"이벤트도 타격으로 되어있는곳이 있음
			pDelegate->OnEventSprObj( pSprObj, this, lx, ly, idObj, fAngle, fOverSec );
		}
    pSprObj->SetbCallHandler( false );   // 콜백 실행 해제
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
#include "XSoundMng.h"
void XKeySound::Execute( XSprObj *pSprObj, float fOverSec )
{
//#ifdef _VER_IOS
#ifndef _SPR_NOT_USE_SOUND
	SOUNDMNG->OpenPlaySoundBySec( m_idSound, 1.f );
#endif
//#endif
}

void XKeySound::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	pRes->Read( &m_idSound, 4 );
	pRes->Read( &m_fVolume, 4 );
	char buff[32];
	pRes->Read( buff, 32 );		// reserved
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyDummy::Load( XSprDat *pSprDat, XBaseRes *pRes )
{
	pRes->Read( &m_bActive, 4 );
	pRes->Read( &m_id, 4 );
	DWORD dw[4]={0,};
	pRes->Read( dw, sizeof(DWORD), 3 );
}

void XKeyDummy::Execute( XSprObj *pSprObj, float fOverSec )
{
	XKeyDummyLayer::Execute( pSprObj, fOverSec );
	auto pActObj = pSprObj->GetpObjActCurr();
	if( XASSERT(pActObj) ) {
//		auto pLayer = GetpLayerByidLayerT<XLayerDummy>( pSprObj );
		XLayerDummy* pLayer = nullptr;
		if( GetidLayer() ) {
			pLayer = SafeCast<XLayerDummy*>( pActObj->GetpLayerByidLocalInLayer( GetidLayer() ) );
//			pLayer = pActObj->GetpLayerByidLocalInLayerT<XLayerDummy>( GetidLayer() );
		} else {
			pLayer = SafeCast<XLayerDummy*>( pSprObj->GetLayer( GetLayerType(), GetnLayer() ) );
		}
		if( XASSERT(pLayer) ) {
			pLayer->SetbActive( m_bActive );		// 더미레이어의 더미를 켜거나 끈다.
			pLayer->Setid( m_id );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

