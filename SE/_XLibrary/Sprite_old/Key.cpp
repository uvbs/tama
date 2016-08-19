/*
 *  Key.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "Types.h"
#include "Key.h"
#include "sprObj.h"
#include "XBaseObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseLayer* XBaseKey::GetpLayer( XSprObj *pSprObj ) 
{
	return pSprObj->GetLayer( GetLayerType(), GetnLayer() );
}

void XBaseKey::Load( XSprDat *pSprDat, XResFile *pRes )
{
	if( pSprDat->IsUpperVersion(18) )
		pRes->Read( &m_idKey, 4 );
	BYTE bData, b2;
	pRes->Read( &bData, 1 );				// key type;
	m_Type = (XBaseKey::xTYPE)bData;
	pRes->Read( &m_fFrame, 4 );				// key frame
	pRes->Read( &bData, 1 );				// layer type
	pRes->Read( &b2, 1 );					// layer num
	m_LayerType = (XBaseLayer::xTYPE)bData;
	m_nLayer = b2;
	pRes->Read( &bData, 1 );	// sub type
	m_SubType = (XBaseKey::xSUBTYPE)bData;
	if( pSprDat->IsUpperVersion(17) )
	{
		int len;
		pRes->Read( &len, 4 );		// 루아코드 길이. 널포함안된거임. 젠장
		if( len > 0 )
		{
			pRes->Seek( len+1 );		// 게임에선 합쳐진 루아가 있으므로 개별루아는 읽지 않는다
			m_bLua = TRUE;			// 읽지는 않았지만 이 키에 루아코드가 있다
		}
	}
}
void XBaseKey::Execute( XSprObj *pSprObj, float fOverSec )
{
	if( GetbLua() )
	{
		// lua Execute()핸들러 호출
		XLua *pLua = pSprObj->GetpLua();
		XBREAK( pLua == NULL );
		char cTable[ 64 ];		// 최적화하려면 이거 사전 생성해놓도록.
		sprintf_s( cTable, "key_%08x", (int)GetidKey() );		// 테이블 이름 생성
		if( pLua->IsHaveMemberFunc( cTable, "Execute" ) )	
			pLua->MemberCall<int>( cTable, "Execute", pSprObj->GetpParent() );
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
	if( pLayer->GetType() != XBaseLayer::xIMAGE_LAYER ) {
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
	if( pLayer->GetType() != XBaseLayer::xOBJ_LAYER ) {
		XLOG( "레이어타입이 OBJ_LAYER가 아니다. %d", (int)pLayer->GetType() );
		return;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyImage::Load( XSprDat *pSprDat, XResFile *pRes )
{
	pRes->Read( &m_nSpr, 4 );
}
void XKeyImage::Execute( XSprObj *pSprObj, float fOverSec )
{
	XKeyImageLayer::Execute( pSprObj, fOverSec );
	XLayerImage *pLayer = (XLayerImage *)pSprObj->GetLayer(GetLayerType(), GetnLayer());
	pLayer->SetpSpriteCurr( m_pSprite );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyPos::Load( XSprDat *pSprDat, XResFile *pRes )
{
	pRes->Read( &m_x, 4 );
	pRes->Read( &m_y, 4 );
	BYTE bData;
	pRes->Read( &bData, 1 );
	m_Interpolation = (XBaseLayer::xtInterpolation)bData;			
	if( pSprDat->IsUpperVersion(21) )
	{
		DWORD dw=0;
		pRes->Read( &dw, 4 );							// idPath(skip)
		//
		pRes->Read( &m_idxNextPathKey, 4 );		// idxNext
		pRes->Read( &dw, 4 );							// idNext(skip)
		pRes->Read( &m_idxPrevPathKey, 4 );		// idxPrev
		pRes->Read( &dw, 4 );							// idPrev(skip)
		pRes->Read( &m_idxInterpolationEndKey, 4 );	// idxEndKey
		pRes->Read( &dw, 4 );							// idEndKey(skip)
		pRes->Read( &dw, 4 );		m_PathType = (SPR::xtLINE)dw; // path type
		pRes->Read( &dw, 4 );		// color(skip)
		//
		pRes->Read( &dw, 4 );		// reserved
		pRes->Read( &dw, 4 );
		pRes->Read( &dw, 4 );
		pRes->Read( &dw, 4 );
	} else
	{
		XBREAK( pSprDat->IsUpperVersion(20) );	// 이거 로딩못함 SE에서 다시 저장할것
		DWORD dwData=0;
		pRes->Read( &dwData, 4 );			// m_ox
		pRes->Read( &dwData, 4 );			// m_oy
		float fData;
		pRes->Read( &fData, 4 );			//  m_fNextKeyFrame
		if( pSprDat->IsLowerVersion(15) )
		{
			m_x *= 0.5f;
			m_y *= 0.5f;
		}
	}
}
void XKeyPos::Execute( XSprObj *pSprObj, float fOverSec )
{
	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
	if( m_Interpolation )
	{
		XKeyPos *pInterEndKey = m_pInterpolationEndKey;
		if( pInterEndKey )
		{
			pLayer->SetcnPos( this, pInterEndKey, m_PathType, m_Interpolation );
		} else
		{
			if( m_pPrevPathKey == NULL )	// path's head
			{
				pLayer->SetcnPos( NULL, NULL, SPR::xLINE_NONE, XBaseLayer::xNONE );
				pLayer->Setxy( m_x, m_y );
			} else
			{
				// 보간구간의 중간키는 암것도 안함
			}
		}
	} else
	{
		pLayer->SetcnPos( NULL, NULL, SPR::xLINE_NONE, XBaseLayer::xNONE );		// 레이어에 패스세팅을 클리어함
		pLayer->Setxy( m_x, m_y );		
	}
}
#if 0
void XKeyPos::Execute( XSprObj *pSprObj, float fOverSec )
{
	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
	pLayer->Setxy( m_x, m_y );
	XKeyPos *pNextKey = m_pNextKey;
	if( m_Interpolation && pNextKey )			// 다음키가 있으면 보간으로 움직이도록 한다
		pLayer->SetcnPos( m_Interpolation, pNextKey->Getx(), pNextKey->Gety(), m_x, m_y, GetfFrame(), pNextKey->GetfFrame() );
	else
		pLayer->SetcnPos( XBaseLayer::xNONE, 0, 0, 0, 0, 0, 0 );
}
#endif // 0
// 키로딩이 끝나면 idx키들의 실제 포인터를 구해야 한다
void XKeyPos::OnFinishLoad( XAniAction *pAction )
{
	if( m_idxInterpolationEndKey >= 0 )	// 키의 인덱스
	{
		m_pInterpolationEndKey = SafeCast<XKeyPos*, XBaseKey*>( pAction->GetKey( m_idxInterpolationEndKey ) );	XBREAK( m_pInterpolationEndKey == NULL );
	}
	if( m_idxNextPathKey >= 0 )
	{
		m_pNextPathKey = SafeCast<XKeyPos*, XBaseKey*>( pAction->GetKey( m_idxNextPathKey ) );	XBREAK( m_pNextPathKey == NULL );
	}
	if( m_idxPrevPathKey >= 0 )
	{
		m_pPrevPathKey = SafeCast<XKeyPos*, XBaseKey*>( pAction->GetKey( m_idxPrevPathKey ) );	XBREAK( m_pPrevPathKey == NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////
void XKeyEffect::Execute( XSprObj *pSprObj, float fOverSec )
{
	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
	pLayer->GetcnEffect().fAlpha = m_fOpacity;
	XKeyEffect *pNextKey = m_pNextKey;
	
	if( m_Interpolation && pNextKey )			// 다음키까지 보간모드로 변해야 하고 다음키가 있으면
		pLayer->SetcnEffect( m_Interpolation, pNextKey->GetfOpacity(), m_fOpacity, m_dwFlag, m_DrawMode, GetfFrame(), pNextKey->GetfFrame() );
	else
		pLayer->SetcnEffect( XBaseLayer::xNONE, 0, 0, m_dwFlag, m_DrawMode, 0, 0 );
}
void XKeyEffect::Load( XSprDat *pSprDat, XResFile *pRes )
{
	BYTE b1;
	pRes->Read( &b1, 1 );		m_Interpolation = (XBaseLayer::xtInterpolation)b1;
	pRes->Read( &m_dwFlag, 4 );
	pRes->Read( &b1, 1 );		m_DrawMode = (xDM_TYPE)b1;
	pRes->Read( &m_fOpacity, 4 );
	DWORD dw1;
	pRes->Read( &dw1, 4 );		// reserved
}
///////////////// Key Image Rot /////////////////////////////////////////
void XKeyRot::Load( XSprDat *pSprDat, XResFile *pRes )
{
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = (XBaseLayer::xtInterpolation)b1;
	pRes->Read( &dw1, 4 );
	pRes->Read( &dw1, 4 );
	pRes->Read( &m_fAngleZ, 4 );
}
void XKeyRot::Execute( XSprObj *pSprObj, float fOverSec )
{
	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
	pLayer->SetAngleZ( m_fAngleZ );
	XKeyRot *pNextKey = m_pNextKey;
	
	if( m_Interpolation && pNextKey )			// 다음키가 있으면 보간으로 움직이도록 한다
		pLayer->SetcnRot( m_Interpolation, pNextKey->GetfAngleZ(), m_fAngleZ, GetfFrame(), pNextKey->GetfFrame() );
	else
		pLayer->SetcnRot( XBaseLayer::xNONE, 0, 0, 0, 0 );
}
///////////////// Key Image Scale /////////////////////////////////////////
void XKeyScale::Load( XSprDat *pSprDat, XResFile *pRes )
{
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = (XBaseLayer::xtInterpolation)b1;
	pRes->Read( &dw1, 4 );
	pRes->Read( &dw1, 4 );
	pRes->Read( &m_fScaleX, 4 );
	pRes->Read( &m_fScaleY, 4 );
}
void XKeyScale::Execute( XSprObj *pSprObj, float fOverSec )
{
	XLayerMove *pLayer = pSprObj->GetpLayerMove(GetLayerType(), GetnLayer());
	pLayer->SetScale( m_fScaleX, m_fScaleY );

	XKeyScale *pNextKey = m_pNextKey;
	
	if( m_Interpolation && pNextKey )			// 다음키가 있으면 보간으로 움직이도록 한다
		pLayer->SetcnScale( m_Interpolation, pNextKey->GetfScaleX(), pNextKey->GetfScaleY(), GetfScaleX(), GetfScaleY(), GetfFrame(), pNextKey->GetfFrame() );
	else
		pLayer->SetcnScale( XBaseLayer::xNONE, 1.0f, 1.0f, 1.0f, 1.0f, 0, 0 );
}

void XKeyCreateObj::Execute( XSprObj *pSprObj, float fOverSec )
{
	// base class
	XKeyObjLayer::Execute( pSprObj, fOverSec );
	//
	XSprObj *pNewSprObj = NULL;
	// 루아코드가 있으면 실행한다
	if( GetbLua() )
	{
		XLua* pLua = pSprObj->GetpLua();
		char cTable[ 64 ];
		sprintf_s( cTable, "key_%08x", (unsigned int)GetidKey() );		// 테이블 이름 생성
		// AddSprObj핸들러
		if( pLua->IsHaveMemberFunc( cTable, "CreateSprObj" ) )		// 핸들러 호출
			// 원래라면 m_szFilename을 컨버트 해서 넘겨줘야한다
			pNewSprObj = pLua->MemberCall<XSprObj*>( cTable, "CreateSprObj", 0, m_nAction, m_PlayType, 
																	GetdwID(), 
																	pSprObj, pSprObj->GetpParent() );		// SprObj생성요청 핸들러 호출
		else
			XBREAKF( 1, "spr:%s: key id=0x%08x: CreateSprObj호출 실패", pSprObj->GetpSprDat()->GetszFilename(), GetidKey() );
	} else
	{
		pNewSprObj = pSprObj->AddSprObj( GetszSprName(), m_nAction, m_PlayType, GetdwID(), -1 );		// 루아에서 0보다 큰수를 아이디를 사용하므로 겹치지 않게 하기 위해 -1을 씀
	}
	if( pNewSprObj )
		pNewSprObj->SetpParentSprObj( pSprObj );
	//
	XLayerObject *pLayer = (XLayerObject *)pSprObj->GetpLayerObj(GetLayerType(), GetnLayer());
	//
	pLayer->SetpSprObjCurr( pNewSprObj );								// 오브젝트레이어에 추가한 차일드를 기억시켜둔다. 
}

void XKeyCreateObj::Destroy( void )
{
}
void XKeyCreateObj::Load( XSprDat *pSprDat, XResFile *pRes )
{
	pRes->Read( &m_dwID, 4 );
	WORD w1;
	UNICHAR sztSprName[256];
	pRes->Read( &w1, 2 );						// string 길이
	pRes->Read( sztSprName, w1 );
#ifdef _VER_IPHONE
	strcpy( m_szSprName, _ConvertUTF16ToUTF8(sztSprName) );
#else
	_tcscpy_s( m_szSprName, sztSprName );
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
void XKeyEvent::Load( XSprDat *pSprDat, XResFile *pRes )
{
	BYTE b1;
	pRes->Read( &b1, 1 );		m_Event = (KE::xTYPE)b1;
	pRes->Read( &m_lx, 4 );
	pRes->Read( &m_ly, 4 );
	pRes->Read( m_Param, sizeof(KE::KEY_PARAM), sizeof(m_Param) / sizeof(KE::KEY_PARAM) );
	if( pSprDat->IsLowerVersion(15) )
	{
		m_lx *= 0.5f;
		m_ly *= 0.5f;
	}
}
void XKeyEvent::Execute( XSprObj *pSprObj, float fOverSec )
{
	//
	XBaseKey::Execute( pSprObj, fOverSec );
	//
	XBaseObj *pObj = pSprObj->GetpParent();
	if( pObj )
	{
		WORD id = m_Param[0].w1;
		float fAngle = m_Param[1].fParam;
		float lx = m_lx;
		float ly = m_ly;
		pSprObj->Transform( &lx, &ly );
		pSprObj->Transform( &fAngle );
		pObj->OnEventCreateObj( lx, ly, id, fAngle, fOverSec );
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
#include "XSoundMng.h"
void XKeySound::Execute( XSprObj *pSprObj, float fOverSec )
{
//#ifdef _VER_IPHONE
	SOUNDMNG->OpenPlaySound( m_idSound );
//#endif
}

void XKeySound::Load( XSprDat *pSprDat, XResFile *pRes )
{
	pRes->Read( &m_idSound, 4 );
	pRes->Read( &m_fVolume, 4 );
	char buff[32];
	pRes->Read( buff, 32 );		// reserved
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void XKeyDummy::Load( XSprDat *pSprDat, XResFile *pRes )
{
	pRes->Read( &m_bActive, 4 );
	pRes->Read( &m_id, 4 );
	DWORD dw[4]={0,};
	pRes->Read( dw, sizeof(DWORD), 3 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////

