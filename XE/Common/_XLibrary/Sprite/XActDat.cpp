﻿#include "stdafx.h"
#include "XActDat.h"
#include "SprDat.h"
#include "SprObj.h"
#include "etc/types.h"
#include "etc/xLang.h"
#include "XResObj.h"
#include "XResMng.h"
#include "XAutoPtr.h"
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "XActObj2.h"		// 이게 여기 인클루드 되면 안됨.

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

/////////////////////////////////////////////////////////
//
// class XAniAction
//
/////////////////////////////////////////////////////////
void XActDat::Destroy( void )
{
	// key list 삭제
	int i;
	for( i = 0; i < m_nNumKeys; i ++ )
		SAFE_DELETE( m_ppKeys[i] );
	SAFE_DELETE_ARRAY( m_ppKeys );
	// 레이어 정보 삭제
	for( i = 0; i < m_nNumLayerInfo; i ++ )
		SAFE_DELETE( m_ppLayerInfo[i] );
	SAFE_DELETE_ARRAY( m_ppLayerInfo );
	
}
#ifdef _XDEBUG
const XBaseKey* XActDat::GetKey( int idx ) const { 
	if( idx >= m_nNumKeys || idx < 0 ) {
		XBREAKF( 1, "%s %s idx(%d) >= m_nNumKeys(%d)", m_pSprDat->GetszFilename(), GetszActName(), idx, m_nNumKeys );
		return NULL;
	}
	return m_ppKeys[ idx ]; 
}
const LAYER_INFO *XActDat::GetLayer( int idx ) const {
	if( idx >= m_nNumLayerInfo || idx < 0 ) {
		XBREAKF( 1, "%s %s idx(%d) >= m_nNumLayerInfo(%d)", m_pSprDat->GetszFilename(), GetszActName(), idx, m_nNumLayerInfo );
		return NULL;
	}
	return m_ppLayerInfo[ idx ];
}
#endif 

void XActDat::Create( void )
{
}
#define KEY_LOOP( K )	int K##i;		\
						XBaseKey **ppKeys = m_ppKeys;	\
						K##i = m_nNumKeys;		\
						while( K##i-- )			\
						{						\
							XBaseKey *K = *ppKeys++;	
#define LAYERINFO_LOOP( K )	int K##i;		\
							LAYER_INFO **ppLayers = m_ppLayerInfo;	\
							K##i = m_nNumLayerInfo;		\
							while( K##i-- )			\
							{						\
								LAYER_INFO *K = *ppLayers++;	
#define LOOP_END		}

//void XAniAction::KeyRemapLayer( XObjAct *pObjAct )
//{
	// 모든 키의 레이어포인터를 새로 갱신해준다
//	KEY_LOOP( pKey )
//	{
//		pKey->SetpLayer( pObjAct->GetLayer( pKey->GetLayerType(), pKey->GetnLayer() ) );
//	} 
//	LOOP_END
//}
LAYER_INFO *XActDat::AddLayerInfo( int idx, xSpr::xtLayer type, int nLayer, ID idLayer )
{
	XBREAK( idLayer == 0 );
#ifdef _XDEBUG
	if( idx >= m_nNumLayerInfo ) {
		XLOG( "%s idx(%d) >= m_nNumLayerInfo(%d)", m_pSprDat->GetszFilename(), idx, m_nNumLayerInfo );
		return NULL;
	}
#endif

	LAYER_INFO *pLayer = new LAYER_INFO;
	pLayer->idLayer = idLayer;
	pLayer->type = type;
	pLayer->nLayer = nLayer;
	pLayer->fAdjustAxisX = pLayer->fAdjustAxisY = 0;
	m_ppLayerInfo[ idx ] = pLayer;
	return pLayer;
}
XKeyPos *XActDat::CreatePosKey( XBaseKey::xTYPE type )
{
	return new XKeyPos;
/*	switch( type )
	{
		case XBaseKey::xKEY_IMAGE:			return new XKeyImagePos;
		case XBaseKey::xKEY_CREATEOBJ:	return new XKeyObjPos;
		case XBaseKey::xKEY_DUMMY:		return new XKeyDummyPos;
		default:
			XBREAK( 1 );
			break;
	}
	return NULL; */
}
XKeyRot *XActDat::CreateRotKey( XBaseKey::xTYPE type )
{
	return new XKeyRot;
/*	switch( type )
	{
		case XBaseKey::xKEY_IMAGE:			return new XKeyImageRot;
		case XBaseKey::xKEY_CREATEOBJ:	return new XKeyObjRot;
		case XBaseKey::xKEY_DUMMY:		return new XKeyDummyRot;
		default:
			XBREAK( 1 );
			break;
	}
	return NULL; */
}
XKeyScale *XActDat::CreateScaleKey( XBaseKey::xTYPE type )
{
	return new XKeyScale;
/*	switch( type )
	{
		case XBaseKey::xKEY_IMAGE:			return new XKeyImageScale;
		case XBaseKey::xKEY_CREATEOBJ:	return new XKeyObjScale;
		case XBaseKey::xKEY_DUMMY:		return new XKeyDummyScale;
		default:
			XBREAK( 1 );
			break;
	}
	return NULL; */
}
XKeyEffect *XActDat::CreateEffectKey( XBaseKey::xTYPE type )
{
	return new XKeyEffect;
/*	switch( type )
	{
		case XBaseKey::xKEY_IMAGE:			return new XKeyImageEffect;
		case XBaseKey::xKEY_CREATEOBJ:	return new XKeyObjEffect;
		case XBaseKey::xKEY_DUMMY:		return new XKeyEffect;
		default:
			XBREAK( 1 );
			break;
	}
	return NULL; */
}

void XActDat::Load( XSprDat *pSprDat, XBaseRes *pRes, int verSpr )
{
	DWORD dw1;
	WORD wData;
	BYTE b1;
	int i;
	pRes->Read( &wData, 2 );					// action name string length
	UNICHAR sztActName[256];
	pRes->Read( sztActName, (int)wData );		// action name + NULL( TCHAR로 읽는다 )
#ifdef WIN32
	_tcscpy_s( m_szActName, sztActName );
#else
    char buff[64];
	_tcscpy_s( m_szActName, _ConvertUTF16ToUTF8(buff, sztActName) );
#endif
	
	pRes->Read( &wData, 2 );					// key 개수
	m_nNumKeys = (int)wData;
	pRes->Read( &m_fMaxFrame, 4 );
	pRes->Read( &m_fSpeed, 4 );
	if( pSprDat->IsUpperVersion(13) ) {
		pRes->Read( &m_PlayMode, 4 );
		pRes->Read( &m_RepeatMark, 4 );
		if( pSprDat->IsUpperVersion(22) ) {
			pRes->Read( &m_vBoundBox[0], sizeof(XE::VEC2) );
			pRes->Read( &m_vBoundBox[1], sizeof(XE::VEC2) );
			if( pSprDat->IsUpperVersion(24) ) {
				XBREAK( sizeof(m_vRotate) != 12 );
				pRes->Read( &m_vScale, sizeof(m_vScale) );
				pRes->Read( &m_vRotate, sizeof(m_vRotate) );
			}
			pRes->Read( &dw1, 4 );	// m_idLayerGlobal.게임에선 걍 스킵
			DWORD reserved[ 9 ] = {0,};
			pRes->Read( reserved, XNUM_ARRAY(reserved), sizeof(DWORD) );
		} else
		{
			BYTE reserved[ 9 ] = {0,};
			pRes->Read( reserved, 9 );
		}
	} else
	{
		BYTE reserved[ 17 ] = {0,};
		pRes->Read( reserved, 17 );
	}
	// 레이어 리스트 로드
	pRes->Read( &b1, 1 );					// 레이어 개수
	m_nNumLayerInfo = (int)b1;
	m_ppLayerInfo = new LAYER_INFO*[ m_nNumLayerInfo ];
	auto type = xSpr::xLT_NONE;
	for( i = 0; i < m_nNumLayerInfo; i ++ ) {
		DWORD id = 0;
		pRes->Read( &id, 4 );	
		if( pSprDat->IsUpperVersion(28) ) {
			pRes->Read( &wData, 2 );						// layer type
			type = XBaseLayer::sConvertNewToOld( wData );
		} else {
			pRes->Read( &b1, 1 );						// layer type
			type = ( xSpr::xtLayer )b1;
		}
		pRes->Read( &b1, 1 );						// layer number
		LAYER_INFO *pLayerInfo = AddLayerInfo( i, type, (int)b1, id );
		pRes->Read( &dw1, 4 );						// fLastFrame 사용하지 않음
		if( pSprDat->IsUpperVersion(8) ) {
			pRes->Read( &pLayerInfo->fAdjustAxisX, 4 );
			pRes->Read( &pLayerInfo->fAdjustAxisY, 4 );
			if( pSprDat->IsLowerVersion(15) ) {
				pLayerInfo->fAdjustAxisX *= 0.5f;
				pLayerInfo->fAdjustAxisY *= 0.5f;
			}
		}
	}
	// Load Keys
	m_ppKeys = new XBaseKey*[ m_nNumKeys ];
#ifdef _XSPR_LOAD2
	for( i = 0; i < m_nNumKeys; i++ ) {
		auto pKey = XBaseKey::sCreateLoad( pRes, pSprDat, verSpr );
		if( verSpr < 29 ) {
			// 29버전 이하 파일은 이 값이 없다. XSprObj::Load에서 차후 넣어준다.
			XASSERT( pKey->GetidLocalInLayer() == 0 );
		}
		AddKey( i, pKey, pKey->GetfFrame() );
	} // for
#else
	for( i = 0; i < m_nNumKeys; i++ ) {
		XBaseKey baseKey;
		baseKey.Load( pSprDat, pRes );
		switch( baseKey.GetType() ) {
		case XBaseKey::xKEY_IMAGE:
		case XBaseKey::xKEY_CREATEOBJ:
		case XBaseKey::xKEY_DUMMY: {
			switch( baseKey.GetSubType() ) {
			case XBaseKey::xSUB_MAIN: {
				if( baseKey.GetType() == XBaseKey::xKEY_IMAGE ) {
					XKeyImage keyImage;
					keyImage.Load( pSprDat, pRes );
					XKeyImage *pKey = new XKeyImage( baseKey, keyImage );		// idKey도 루아도 카피해야하므로 실수하지 않게 이형태의 생성자를 쓰자
					pKey->SetpSprite( pSprDat->GetSprite( keyImage.GetnSpr() ) );
					AddKey( i, pKey, baseKey.GetfFrame() );
				} else
					if( baseKey.GetType() == XBaseKey::xKEY_CREATEOBJ ) {
						XKeyCreateObj keyCreateObj;
						keyCreateObj.Load( pSprDat, pRes );
						XKeyCreateObj *pKey = new XKeyCreateObj( baseKey, keyCreateObj );
						AddKey( i, pKey, baseKey.GetfFrame() );
					} else
						if( baseKey.GetType() == XBaseKey::xKEY_DUMMY ) {
							if( pSprDat->IsUpperVersion( 16 ) ) {
								XKeyDummy keyDummy;
								keyDummy.Load( pSprDat, pRes );
								// 이렇게 new해서 AddKey쓰는게 코딩량을 줄일수 있을듯 AddKeyXXXX()시리즈 만드는것도 일
								XKeyDummy *pKey = new XKeyDummy( baseKey, keyDummy );
								AddKey( i, pKey, baseKey.GetfFrame() );
							}
						}
			} break;
			case XBaseKey::xSUB_POS: {
				XKeyPos keyPos;
				keyPos.Load( pSprDat, pRes );
				XKeyPos *pKey = new XKeyPos( baseKey, keyPos );
				AddKey( i, pKey, baseKey.GetfFrame() );
			} break;
			case XBaseKey::xSUB_ROT: {
				XKeyRot keyRot;
				keyRot.Load( pSprDat, pRes );
				XKeyRot *pKey = new XKeyRot( baseKey, keyRot );
				AddKey( i, pKey, baseKey.GetfFrame() );
			} break;
			case XBaseKey::xSUB_SCALE: {
				XKeyScale keyScale;
				keyScale.Load( pSprDat, pRes );
				XKeyScale *pKey = new XKeyScale( baseKey, keyScale );
				AddKey( i, pKey, baseKey.GetfFrame() );
			} break;
			case XBaseKey::xSUB_EFFECT: {
				XKeyEffect keyEffect;
				keyEffect.Load( pSprDat, pRes );
				XKeyEffect *pKey = new XKeyEffect( baseKey, keyEffect );
				AddKey( i, pKey, baseKey.GetfFrame() );
			} break;
			} // switch( sub )
		} //case img, obj, dummy
															 break;
		case XBaseKey::xKEY_EVENT: {
			XKeyEvent keyEvent;
			keyEvent.Load( pSprDat, pRes );
			XKeyEvent *pKey = new XKeyEvent( baseKey, keyEvent );
			pKey->SetSprFile( SZ2C( keyEvent.GetSprFile() ) );
			AddKey( i, pKey, baseKey.GetfFrame() );
		} break;
		case XBaseKey::xKEY_SOUND: {
			XKeySound keySound;
			keySound.Load( pSprDat, pRes );
			XKeySound *pKey = new XKeySound( baseKey, keySound );
			AddKey( i, pKey, baseKey.GetfFrame() );
#if defined(WIN32) && defined(_CIV)
			SOUNDMNG->OpenSound( keySound.GetidSound() );	// win32 civ 에선 특별히 로딩할때 사운드를 모두 읽는다
#endif
		} break;
		default:
			XBREAK( 1 );
			break;
		} // switch( baseKey.GetType() )	{
		} // for keys
#endif // not _XSPR_LOAD2

	// 키 로딩이 끝나면 호출된다
	for( i = 0; i < m_nNumKeys; i ++ ) {
		XBREAK( m_ppKeys[i] == NULL );
		m_ppKeys[i]->OnFinishLoad( this );
	}
	
	// set nextkey
	for( int i = 0; i < m_nNumKeys; i ++ ) {
		XBaseKey *pKey = m_ppKeys[i];
		if( pKey->GetSubType() == XBaseKey::xSUB_ROT ||		// 일부 키타입만 다음키를 세팅한다
			pKey->GetSubType() == XBaseKey::xSUB_SCALE ||
			pKey->GetSubType() == XBaseKey::xSUB_EFFECT ) {
			for( int j = i + 1; j < m_nNumKeys; j++ ) {
				XBaseKey *pKey2 = m_ppKeys[j];
				//  타입도 같고
				if( pKey2->GetType() == pKey->GetType() && pKey2->GetSubType() == pKey->GetSubType() ) {	
					// 레이어도 같으면
					if( pKey2->GetLayerType() == pKey->GetLayerType() )	{
						if( pKey2->GetnLayer() == pKey->GetnLayer() ) {
							pKey->SetNextKey( pKey2 );
							break;
						}
					}
				}
			}
		}
	} 

}
int XActDat::AddKey( int idx, XBaseKey *pNewKey, float fFrame )
{
#ifdef _XDEBUG
	if( idx >= m_nNumKeys ) {
		XLOG( "%s %s idx(%d) >= m_nNumKeys(%d)", m_pSprDat->GetszFilename(), GetszActName(), idx, m_nNumKeys );
		return 0;
	}
#endif
	m_ppKeys[ idx ] = pNewKey;
	pNewKey->SetfFrame( fFrame );
	return m_nNumKeys-1;
}

/**
 @brief nKeyCurr부터 시작해서 fFrame위치의 키까지 모두 실행한다.
 @param nKeyCurr 시작할 키의 인덱스
 @param fFrame 목표 프레임위치
*/
void XActDat::ExecuteKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame )
{
	if( nKeyCurr >= m_nNumKeys )
		return;
	while(1) {
		auto pKeyCurr = GetKeyMutable( nKeyCurr );	// 일단 mutable로 하고 const형태로 바꾸자
// 		if( fFrame >= pKeyCurr->GetfFrame() ) {
		if( pKeyCurr->GetfFrame() <= fFrame ) {
			float fOverFrame = fFrame - pKeyCurr->GetfFrame();	// 초과된 프레임
			float fOverSec = XSPF * fOverFrame / m_fSpeed;	// 초과된 프레임을 초로 환산
#ifdef DEBUG 
			if( fOverFrame > 0 )
			{ int a=0; a=1; }
#endif
      // 이벤트키의 이벤트내부에서 SetAction()을 바꾼경우에 해당한다. 설계가 잘못됬다. 
      // pSprObj을 파라메터로 넘기면 안되고 this를 넘겨서 처리해야 할듯 하다.
      XBREAK( m_ID != pSprObj->GetpObjActCurr()->GetpAction()->GetID() );
			pKeyCurr->Execute( pSprObj, fOverSec );
			++ nKeyCurr;
			if( nKeyCurr >= GetnNumKeys() )
				break;
		}  else
			break;
	}
}

void XActDat::JumpKey( XSprObj *pSprObj, int &nKeyCurr, float fFrame )
{
	if( nKeyCurr >= m_nNumKeys )
		return;
	while(1) {
		auto pKeyCurr = GetKeyMutable( nKeyCurr ); 
		if( fFrame > pKeyCurr->GetfFrame() ) {
			++ nKeyCurr;
			if( nKeyCurr >= GetnNumKeys() )
				break;
		}  else
			break;
	}
}