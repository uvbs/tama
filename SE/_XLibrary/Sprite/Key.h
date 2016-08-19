/*
 *  Key.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#ifndef __KEY_H__
#define __KEY_H__

#include "Layer.h"
#include "Sprdef.h"
#include "ResMng.h"

class XSprDat;
class XSprObj;
class XSprite;
class XBaseKey;
class XAniAction;
////////////////////////////////////////////////////
class XBaseKey
{
public:
	enum xTYPE { xKEY_NONE = 0, xKEY_IMAGE, xKEY_CREATEOBJ, xKEY_SOUND, xKEY_EVENT, xKEY_DUMMY };
	enum xSUBTYPE { xSUB_MAIN, xSUB_POS, xSUB_EFFECT, xSUB_ROT, xSUB_SCALE };
	LPCTSTR GetTypeString( XBaseKey::xTYPE type = XBaseKey::xKEY_NONE ) { 
		if( type == XBaseKey::xKEY_NONE )
			type = GetType();
		switch( type ) 
		{
		case XBaseKey::xKEY_IMAGE:		return _T("xKEY_IMAGE");
		case XBaseKey::xKEY_CREATEOBJ:	return _T("xKEY_CREATEOBJ");
		case XBaseKey::xKEY_SOUND:		return _T("xKEY_SOUND");
		case XBaseKey::xKEY_EVENT:		return _T("xKEY_EVENT");
		case XBaseKey::xKEY_DUMMY:	return _T("xKEY_DUMMY");
		default:
			XLOG( "잘못된 키 타입 %d", (int)type );
			return _T("xKEY_ERROR");
		}
	}
	LPCTSTR GetSubTypeString( void ) {
		XBaseKey::xSUBTYPE type = m_SubType;
		switch( type ) 
		{
		case XBaseKey::xSUB_MAIN:		return _T("xSUB_MAIN");
		case XBaseKey::xSUB_POS	:	return _T("xSUB_POS");
		case XBaseKey::xSUB_EFFECT:		return _T("xSUB_EFFECT");
		case XBaseKey::xSUB_ROT:		return _T("xSUB_ROT");
		case XBaseKey::xSUB_SCALE:		return _T("xSUB_SCALE");
		default:
			XLOG( "잘못된 서브키 타입 %d", (int)type );
			return _T("xSUB_ERROR");
		}
	}
private:
	ID m_idKey;			// 고유 아이디
	XBaseKey::xTYPE m_Type;
	XBaseKey::xSUBTYPE m_SubType;
	float m_fFrame;
	XBaseLayer::xTYPE m_LayerType;
	int m_nLayer;						// 속해있는 레이어 번호
	BOOL m_bLua;					// 루아코드를 갖는 키인가(실제 코드는 가지지 않음)
	void Init( void ) {
		m_idKey = 0;		// 게임에선 반드시 파일에서 로딩한 아이디만 있다
		m_Type = XBaseKey::xKEY_NONE; 
		m_SubType = XBaseKey::xSUB_MAIN;
		m_fFrame = 0; 
		m_LayerType = XBaseLayer::xNONE_LAYER;
		m_nLayer = 0;
		m_bLua = FALSE;
	}
protected:		// draw관련
public:
	XBaseKey() { Init(); }
	XBaseKey( XBaseKey::xTYPE type ) { Init(); m_Type = type; m_SubType = XBaseKey::xSUB_MAIN; }
	XBaseKey( XBaseKey::xTYPE type, XBaseKey::xSUBTYPE subType ) { Init(); m_Type = type; m_SubType = subType; }
/*	XBaseKey( XBaseKey::xTYPE type, XBaseKey::xSUBTYPE subType, float fFrame, XBaseLayer *pLayer ) { 
		Init(); 
		m_Type = type; 
		m_SubType = subType;
		m_fFrame = fFrame; 
		if( pLayer )
		{
			m_LayerType = pLayer->GetType();
			m_nLayer = pLayer->GetnLayer();
		}
	}
	XBaseKey( XBaseKey::xTYPE type, XBaseKey::xSUBTYPE subType, float fFrame, XBaseLayer::xTYPE layertype, int nLayer ) { 
		Init(); 
		m_Type = type; 
		m_SubType = subType;
		m_fFrame = fFrame; 
		m_LayerType = layertype;
		m_nLayer = nLayer;
	}
	XBaseKey( float fFrame, XBaseLayer::xTYPE layertype, int nLayer ) { 
		Init(); 
		m_fFrame = fFrame; 
		m_LayerType = layertype;
		m_nLayer = nLayer;
	} */
	XBaseKey( XBaseKey& keyBase ) {		// 카피 생성자
		Init();
		m_idKey = keyBase.GetidKey();
		m_Type = keyBase.GetType();
		m_SubType = keyBase.GetSubType();
		m_fFrame = keyBase.GetfFrame(); 
		m_LayerType = keyBase.GetLayerType();
		m_nLayer = keyBase.GetnLayer();
		m_bLua = keyBase.GetbLua();		// 게임에선 루아코드까지 카피할필요는 없음. 애초에 읽지도 않음.
	}
	virtual ~XBaseKey() {}

	GET_ACCESSOR( ID, idKey );
	GET_SET_ACCESSOR( float, fFrame );
	GET_SET_ACCESSOR( XBaseKey::xTYPE, Type );
	GET_SET_ACCESSOR( XBaseKey::xSUBTYPE, SubType );
	GET_ACCESSOR( XBaseLayer::xTYPE, LayerType );
	GET_ACCESSOR( int, nLayer );
	GET_ACCESSOR( BOOL, bLua );
	XBaseLayer* GetpLayer( XSprObj *pSprObj );
	
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) {}
	virtual void OnFinishLoad( XAniAction *pAction ) {}
	
};

////////////////////////////////////////////////////
class XKeyPos : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_POS;
private:
	float m_x, m_y;
	XBaseLayer::xtInterpolation m_Interpolation;		// 다음키까지 보간으로 이동해야함
	SPR::xtLINE m_PathType;	// 선형패스냐, 스플라인패스냐 이런거
	XKeyPos *m_pInterpolationEndKey;	// 보간 끝키. 여기에 값이 있으면 this키는 자동으로 보간시작키가 된다
	XKeyPos *m_pNextPathKey;	// 패스로 연결되어 있는 다음키
	XKeyPos *m_pPrevPathKey;	// 패스로 연결되어 있는 이전키
	int m_idxInterpolationEndKey;
	int m_idxNextPathKey;
	int m_idxPrevPathKey;
//	XKeyPos *m_pNextKey;	// 이게 과연 필요한지는 고민해봐야할 문제다. 액션로딩할때 넥스트키 할당하는게 부하가 얼마나 걸릴지가 관건
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_POS );
		m_x = m_y = 0; 
		m_Interpolation = XBaseLayer::xNONE;
//		m_pNextKey = NULL;
		m_PathType = SPR::xLINE_NONE;
		m_pInterpolationEndKey = m_pNextPathKey = m_pPrevPathKey = NULL;
		m_idxInterpolationEndKey = m_idxNextPathKey = m_idxPrevPathKey = -1;
	}
protected:
/*	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_POS ) {
			XLOG( "pKey의 서브타입이 xPOS가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyPos*, XBaseKey*>(pKey);
	} */
public:
	XKeyPos() { Init(); }
	XKeyPos( XBaseKey& keyBase, XKeyPos& key ) : XBaseKey( keyBase ) {
		Init();
		m_x = key.Getx();
		m_y = key.Gety();
		m_Interpolation = key.GetInterpolation();
		m_PathType = key.GetPathType();
		m_idxInterpolationEndKey = key.GetidxInterpolationEndKey();
		m_idxNextPathKey = key.GetidxNextPathKey();
		m_idxPrevPathKey = key.GetidxPrevPathKey();
	}
	virtual ~XKeyPos() {}
	// get/set
	GET_ACCESSOR( float, x );
	GET_ACCESSOR( float, y );
	XE::VEC2 GetPos( void ) { return XE::VEC2( m_x, m_y ); }
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	GET_ACCESSOR( SPR::xtLINE, PathType );
	GET_ACCESSOR( int, idxInterpolationEndKey );
	GET_ACCESSOR( int, idxNextPathKey );
	GET_ACCESSOR( int, idxPrevPathKey );
	GET_ACCESSOR( XKeyPos*, pInterpolationEndKey );
	GET_ACCESSOR( XKeyPos*, pNextPathKey );
	GET_ACCESSOR( XKeyPos*, pPrevPathKey );
	void Set( XBaseLayer::xtInterpolation interpolation ) { m_Interpolation = interpolation; }
//	XKeyPos *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void OnFinishLoad( XAniAction *pAction );
//	virtual void SetNextKey( XBaseKey* pKey ) { SetpNextKey( pKey ); }
};
/////////////////////////////////////////////////////
class XKeyEffect : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_EFFECT;
private:
	XBaseLayer::xtInterpolation m_Interpolation;	// 다음키까지 알파보간을 할건지 말건지
	DWORD m_dwFlag;			// 상/하, 좌/우 플립같은 토글성 이펙트는 이곳에 들어간다.
	xDM_TYPE m_DrawMode;			// screen, dodge, darken 찍기등 선택형 속성은 이곳에 들어간다.
	float m_fOpacity;				// 알파블랜드, 투명도 0.0 ~ 1.0
	XKeyEffect *m_pNextKey;	
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_EFFECT );
		m_Interpolation = XBaseLayer::xNONE;
		m_dwFlag = 0;
		m_DrawMode = xDM_NONE;
		m_fOpacity = 1.0f;
		m_pNextKey = NULL;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_EFFECT ) {
			XLOG( "pKey의 Image타입이 EFFECT가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyEffect*, XBaseKey*>(pKey);
	}
public:
	XKeyEffect() { Init(); }
//	XKeyEffect( XBaseKey::xTYPE type, float fFrame, XBaseLayer::xTYPE layerType, int nLayer ) : XBaseKey( type, s_subType, fFrame, layerType, nLayer ) { Init(); }
	XKeyEffect( XBaseKey& keyBase, XKeyEffect& key ) : XBaseKey( keyBase ) {
		Init();
		m_Interpolation = key.GetInterpolation();
		m_dwFlag = key.GetdwFlag();
		m_DrawMode = key.GetDrawMode();
		m_fOpacity = key.GetfOpacity();
	}
	virtual ~XKeyEffect() {}
	// get/set
	BOOL GetFlipHoriz( void ) { return (m_dwFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE; }
	BOOL GetFlipVert( void ) { return (m_dwFlag & EFF_FLIP_VERT) ? TRUE : FALSE; }
	GET_SET_ACCESSOR( float, fOpacity );
	GET_SET_ACCESSOR( DWORD, dwFlag );
	GET_SET_ACCESSOR( xDM_TYPE, DrawMode );
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	void Set( XBaseLayer::xtInterpolation interpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity ) { 
		m_Interpolation = interpolation;
		m_dwFlag = dwDrawFlag;
		m_DrawMode = drawMode;
		m_fOpacity = fOpacity;
	}
	XKeyEffect *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) { SetpNextKey( pKey ); }
};
//////////////////////////////////////////////////////
class XKeyRot : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_ROT;
private:
	float m_fAngleZ;					//  누적 절대각도. +720, -720도 모두 될수 있다.
	XBaseLayer::xtInterpolation m_Interpolation;		// 다음키까지 보간
	XKeyRot *m_pNextKey;
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_ROT );
		m_fAngleZ = 0;
		m_Interpolation = XBaseLayer::xNONE;
		m_pNextKey = NULL;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_ROT ) {
			XLOG( "pKey의 Image타입이 ROTATE가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyRot*, XBaseKey*>(pKey);
	}
public:
	XKeyRot() { Init(); }
/*	XKeyRot( float az ) {	
		Init(); 
		m_fAngleZ = az;
	} */
	XKeyRot( XBaseKey& keyBase, XKeyRot& key ) : XBaseKey( keyBase ) {
		Init();
		m_fAngleZ = key.GetfAngleZ();
		m_Interpolation = key.GetInterpolation();
	}
	virtual ~XKeyRot() {}
	// get/set
	GET_SET_ACCESSOR( float, fAngleZ );
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	void AddAngleZ( float angle ) { 	m_fAngleZ += angle; 	}
	void Set( XBaseLayer::xtInterpolation interpolation ) { m_Interpolation = interpolation; }
	XKeyRot *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) { SetpNextKey( pKey ); }
};
////////////////////////////////////////////////////
class XKeyScale : public XBaseKey
{
public:
	static const XBaseKey::xSUBTYPE s_subType = XBaseKey::xSUB_SCALE;
private:
	float m_fScaleX, m_fScaleY;
	XBaseLayer::xtInterpolation m_Interpolation;
	XKeyScale *m_pNextKey;
	void Init() {
		SetSubType( XBaseKey::xSUB_SCALE );
		m_fScaleX = m_fScaleY = 1.0f;
		m_Interpolation = XBaseLayer::xNONE;
		m_pNextKey = NULL;
	}
protected:
	void SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey == NULL ) {
			m_pNextKey = NULL;
			return;
		}
		if( pKey->GetSubType() != XBaseKey::xSUB_SCALE ) {
			XLOG( "pKey의 서브타입이 SCALE가 아니다 %d %3.2f", (int)pKey->GetSubType(), pKey->GetfFrame() );
			m_pNextKey = NULL;
			return;
		}
#endif
		m_pNextKey = SafeCast<XKeyScale*, XBaseKey*>(pKey);
	}
public:
	XKeyScale() { Init(); }
/*	XKeyScale( float sx, float sy ) {
		Init(); 
		m_fScaleX = sx;
		m_fScaleY = sy;
	} */
	XKeyScale( XBaseKey& keyBase, XKeyScale& key ) : XBaseKey( keyBase ) {
		Init();
		m_fScaleX = key.GetfScaleX();
		m_fScaleY = key.GetfScaleY();
		m_Interpolation = key.GetInterpolation();
	}
	virtual ~XKeyScale() {}
	// get/set
	GET_SET_ACCESSOR( float, fScaleX );
	GET_SET_ACCESSOR( float, fScaleY );
	GET_ACCESSOR( XBaseLayer::xtInterpolation, Interpolation );
	void AddScale( float sx, float sy ) { m_fScaleX += sx; m_fScaleY += sy; }
	void MulScale( float xMul, float yMul ) { m_fScaleX *= xMul; m_fScaleY *= yMul; }		// 인수는 1.0=100%인 배수가 들어온다
//	void Set( XBaseLayer::xtInterpolation interpolation ) { m_Interpolation = interpolation; }
	XKeyScale *GetpNextKey( void ) { return m_pNextKey; }
	// virtual 
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) { SetpNextKey( pKey ); }
};
////////////////////////////////////////////////////
class XKeyImageLayer : public XBaseKey
{
public: 
private:
	void Init() {
		SetType( XBaseKey::xKEY_IMAGE );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
#ifdef _XDEBUG
		if( pKey->GetType() != XBaseKey::xKEY_IMAGE ) {
			XLOG( "pKey의 타입이 IMAGE가 아니다 %d %3.2f", (int)pKey->GetType(), pKey->GetfFrame() );
			return NULL;
		}
#endif
		return pKey;
	}
public: 
	XKeyImageLayer() { Init(); }
	XKeyImageLayer( XBaseKey::xSUBTYPE imgtype ) { Init(); SetSubType( imgtype ); }
	XKeyImageLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
	virtual ~XKeyImageLayer() {}
	
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void SetNextKey( XBaseKey* pKey ) { XBREAK(1); }
};
////////////////////////////////////////////////////
class XKeyImage : public XKeyImageLayer
{
	int m_nSpr;		// m_pSprite의 인덱스 load할때만 씀
	XSprite *m_pSprite;
	void Init() {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_nSpr = 0;
		m_pSprite = NULL;
	}
public:
	XKeyImage() : XKeyImageLayer(XBaseKey::xSUB_MAIN) { Init(); }
/*	XKeyImage( float fFrame, XBaseLayer::xTYPE type, int nLayer, XSprite *pSpr ) : XBaseKey( fFrame, type, nLayer ) {
		Init();
		m_pSprite = pSpr;
	} */
	XKeyImage( XBaseKey& keyBase, XKeyImage& key ) : XKeyImageLayer( keyBase ) {
		Init();
		m_nSpr = key.GetnSpr();
//		m_pSprite = key.GetpSprite();
	}
	virtual ~XKeyImage() {}
	GET_ACCESSOR( int, nSpr );
	GET_SET_ACCESSOR( XSprite*, pSprite );
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyImageLayer::SetpNextKey( pKey ); }
};
////////////////////////////////////////////////////

////////////////////////////////////////////////////
class XKeyObjLayer : public XBaseKey
{
public: 
private:
	void Init( void ) {
		SetType( XBaseKey::xKEY_CREATEOBJ );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
		if( XBREAKF( pKey->GetType() != XBaseKey::xKEY_CREATEOBJ, "pKey의 타입이 CREATEOBJ가 아니다 %d %3.2f", (int)pKey->GetType(), pKey->GetfFrame() ) )
			return NULL;
		return pKey;
	}
public:
	XKeyObjLayer() { Init(); }
	XKeyObjLayer( XBaseKey::xSUBTYPE type ){ Init(); SetSubType( type ); } 
	XKeyObjLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
//	XKeyObjLayer( XBaseKey::xSUBTYPE type, float fFrame, XBaseLayer *pLayer ) 	: XBaseKey( XBaseKey::xKEY_CREATEOBJ, type, fFrame, pLayer ) {	}
//	XKeyObjLayer( XBaseKey::xSUBTYPE objtype, float fFrame, XBaseLayer::xTYPE type, int nLayer ) 	: XBaseKey( XBaseKey::xKEY_CREATEOBJ, objtype, fFrame, type, nLayer ) {	}
	virtual ~XKeyObjLayer() {}
	
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void SetNextKey( XBaseKey* pKey ) {XBREAK(1);}
};
////////////////////////////////////////////////////
class XKeyCreateObj : public XKeyObjLayer
{	
	DWORD m_dwID;					// 4바이트짜리 고유번호
	TCHAR m_szSprName[128];		// 스프라이트 파일 이름
	int m_nAction;						// 스프라이트를 읽은후 SetAction()을 할 번호
	xRPT_TYPE m_PlayType;
	void Init( void ) {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_dwID = 0;
		memset( m_szSprName, 0, sizeof(m_szSprName) );
		m_nAction = 0;
		m_PlayType = xRPT_LOOP;
	}
	void Destroy( void ) ;
public:
	XKeyCreateObj() { Init(); }
//	XKeyCreateObj( DWORD id, LPCTSTR szSprObj, XBaseLayer::xTYPE type, int nLayer, int nAction, xRPT_TYPE playType, float fFrame, float x, float y );
	XKeyCreateObj( XBaseKey& keyBase, XKeyCreateObj& key ) : XKeyObjLayer( keyBase ) {
		Init();
		m_dwID = key.GetdwID();
		_tcscpy_s( m_szSprName, key.GetszSprName() );
		m_nAction = key.GetnAction();
		m_PlayType = key.GetPlayType();
	}
	virtual ~XKeyCreateObj() { Destroy(); }
	
	GET_ACCESSOR( LPCTSTR, szSprName );
	GET_ACCESSOR( int, nAction );
	GET_ACCESSOR( DWORD, dwID );
	GET_SET_ACCESSOR( xRPT_TYPE, PlayType );
	void Set( xRPT_TYPE playType ) {
		m_PlayType = playType;
	}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyObjLayer::SetpNextKey( pKey ); }
};
////////////////////////////////////////////////////
/*class XKeyObjPos : public XKeyObjLayer, public XKeyPos
{
	void Init( void ) {	}
public:
	XKeyObjPos() { 		Init();	}
//	XKeyObjPos( float fFrame, XBaseLayer *pLayer, float x, float y ) : XKeyObjLayer( XBaseKey::xSUB_POS, fFrame, pLayer ) {		Init();		m_x = x;	m_y = y;	}
	XKeyObjPos( float fFrame, XBaseLayer::xTYPE type, int nLayer, float x, float y ) : XKeyPos( x, y ), XBaseKey( fFrame, type, nLayer ) {		Init();	}
	virtual ~XKeyObjPos() {}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyObjLayer::Execute( pSprObj, fOverSec ); XKeyPos::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyPos::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyPos::SetpNextKey( XKeyObjLayer::SetpNextKey( pKey ) ); }
}; // class XKeyObjPos
////////////////////////////////////////////////////
class XKeyObjEffect : public XKeyObjLayer, public XKeyEffect
{
	void Init( void ) {	}
public:
	XKeyObjEffect() { 		Init();	}
	XKeyObjEffect( float fFrame, XBaseLayer::xTYPE type, int nLayer ) : XBaseKey( fFrame, type, nLayer ) {		Init();	}	// XKeyEffect의 파라메터는 별도(Set())로 불러준다
	virtual ~XKeyObjEffect() {}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyObjLayer::Execute( pSprObj, fOverSec ); XKeyEffect::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyEffect::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyEffect::SetpNextKey( XKeyObjLayer::SetpNextKey( pKey ) ); }
}; // class XKeyObjEffect
////////////////////////////////////////////////////
class XKeyObjRot : public XKeyObjLayer, public XKeyRot
{
	void Init( void ) {	}
public:
	XKeyObjRot() { 		Init();	}
	XKeyObjRot( float fFrame, XBaseLayer::xTYPE type, int nLayer, float az ) : XKeyRot( az ), XBaseKey( fFrame, type, nLayer ) {		Init();	}
	virtual ~XKeyObjRot() {}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyObjLayer::Execute( pSprObj, fOverSec ); XKeyRot::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyRot::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyRot::SetpNextKey( XKeyObjLayer::SetpNextKey( pKey ) ); }
}; // class XKeyObjRot
////////////////////////////////////////////////////
class XKeyObjScale : public XKeyObjLayer, public XKeyScale
{
	void Init( void ) {	}
public:
	XKeyObjScale() { 		Init();	}
	XKeyObjScale( float fFrame, XBaseLayer::xTYPE type, int nLayer, float sx, float sy ) : XKeyScale( sx, sy ), XBaseKey( fFrame, type, nLayer ) {		Init();	}
	virtual ~XKeyObjScale() {}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyObjLayer::Execute( pSprObj, fOverSec ); XKeyScale::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyScale::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyScale::SetpNextKey( XKeyObjLayer::SetpNextKey( pKey ) ); }
}; // class XKeyObjScale
*/
////////////////////////////////////////////////////
namespace KE
{
	enum xTYPE
	{
		NONE = 0,
		CREATE_OBJ,			// 오브젝트 생성 이벤트
		HIT,					// 타격 이벤트
		ETC = 999			// 기타 다용도 이벤트
	};
	struct KEY_PARAM {
		union {
			struct {
				BYTE b1;
				BYTE b2;
				BYTE b3;
				BYTE b4;
			};
			struct {
				WORD w1;
				WORD w2;
			};
			DWORD dwParam;
			float fParam;
		};
	};
}
class XKeyEvent : public XBaseKey
{
	KE::xTYPE m_Event;		// 
	float m_lx, m_ly;				// 이벤트 발생 좌표. 센터를 기준으로한 로컬좌표
	KE::KEY_PARAM	m_Param[16];		// 4 * 16 byte만큼을 파라메터로 쓸수 있다
	void _Init() {
		m_lx = m_ly = 0;
		m_Event = KE::NONE;
		memset( m_Param, 0, sizeof(m_Param) );
	}
	void _Destroy() {}
public:
	XKeyEvent() : XBaseKey( XBaseKey::xKEY_EVENT ) { _Init(); }
	XKeyEvent( XBaseKey& keyBase, XKeyEvent& key ) : XBaseKey( keyBase ) {
		_Init();
		m_Event = key.GetEvent();
		m_lx = key.Getlx();
		m_ly = key.Getly();
		memcpy_s( (void*)m_Param, sizeof(m_Param), (void*)key.GetParam(), sizeof(m_Param) );
//		m_Param = key.GetParam();
	}
	virtual ~XKeyEvent() { _Destroy(); }
	
	GET_ACCESSOR( float, lx );
	GET_ACCESSOR( float, ly );
	GET_ACCESSOR( KE::xTYPE, Event );
	GET_ACCESSOR( const KE::KEY_PARAM*, Param );
	void SetPos( float lx, float ly ) { m_lx = lx; m_ly = ly; }
	void SetAngle( float angle ) { m_Param[1].fParam = angle; }
	float GetAngle( void ) { return m_Param[1].fParam; }
//	void CopyParam( XKeyEvent *pSrc ) { memcpy( m_Param, pSrc->GetParam(), sizeof(m_Param) ); }
	
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
}; // class XKeyEvent

////////////////////////////////////////////////////////////////////////////////////////////
// class XKeyDummyLayer 
class XKeyDummyLayer : public XBaseKey
{
	static const XBaseKey::xTYPE ThisType = XBaseKey::xKEY_DUMMY;
	static const XBaseLayer::xTYPE ThisLayerType = XBaseLayer::xDUMMY_LAYER;
public: 
private:
	void Init( void ) {
		SetType( ThisType );
	}
protected:
	XBaseKey* SetpNextKey( XBaseKey *pKey ) { 
		if( XBREAKF( pKey->GetType() != ThisType, "pKey의 타입이 %s가 아니다-> %s %3.2f", pKey->GetTypeString( ThisType ), pKey->GetTypeString(), pKey->GetfFrame() ) )
			return NULL;
		return pKey;
	}
public:
	XKeyDummyLayer() { Init(); }
	XKeyDummyLayer( XBaseKey::xSUBTYPE type ){ Init(); SetSubType( type ); } 
	XKeyDummyLayer( XBaseKey& keyBase ) : XBaseKey( keyBase ) { Init(); }
	virtual ~XKeyDummyLayer() {}

	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) {
		XBaseKey::Execute( pSprObj );
		XBREAK( GetLayerType() != ThisLayerType );
	}
//	virtual void SetNextKey( XBaseKey* pKey ) {XBREAK(1);}
};

class XKeyDummy : public XKeyDummyLayer
{
private:
	BOOL m_bActive;
	ID m_id;
	void Init() {
		SetSubType( XBaseKey::xSUB_MAIN );
		m_bActive = TRUE;
		m_id = 0;
	}
public:
	XKeyDummy() { Init(); 	}
/*	XKeyDummy( float fFrame, XBaseLayer::xTYPE type, int nLayer, BOOL bActive ) : XBaseKey( fFrame, type, nLayer ) { 
		Init(); 
		m_bActive = bActive;
	} */
	XKeyDummy( XBaseKey& keyBase, XKeyDummy& key ) : XKeyDummyLayer( keyBase ) {
		Init();
		m_bActive = key.GetbActive();
		m_id = key.Getid();
	}
	virtual ~XKeyDummy() { }

	GET_SET_ACCESSOR( BOOL, bActive );
	GET_ACCESSOR( ID, id );
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) {
		XKeyDummyLayer::Execute( pSprObj, fOverSec );
		XLayerDummy *pLayer = SafeCast<XLayerDummy*, XBaseLayer*>( GetpLayer( pSprObj ) );
		pLayer->SetbActive( m_bActive );		// 더미레이어의 더미를 켜거나 끈다.
		pLayer->Setid( m_id );
	}
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
};
// class XKeyDummyPos 
/*class XKeyDummyPos : public XKeyDummyLayer, public XKeyPos
{
	void Init( void ) {	}
public:
	XKeyDummyPos( ) {	Init();	}
	XKeyDummyPos( float fFrame, XBaseLayer::xTYPE type, int nLayer, float x, float y ) : XBaseKey( fFrame, type, nLayer ), XKeyPos( x, y ) {	Init();	}
	virtual ~XKeyDummyPos() {}
	// virtual 
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyDummyLayer::Execute( pSprObj, fOverSec ); XKeyPos::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyPos::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyPos::SetpNextKey( XKeyDummyLayer::SetpNextKey( pKey ) ); }
};
// class XKeyDummyRot 
class XKeyDummyRot : public XKeyDummyLayer, public XKeyRot
{
	void Init( void ) {	}
public:
	XKeyDummyRot( ) { Init(); }
//	XKeyDummyRot( float fFrame, XBaseLayer *pLayer, float az ) : XBaseKey( fFrame, pLayer ), XKeyRot( az ), XKeyDummyLayer( pAction ) {Init();	}
	XKeyDummyRot( float fFrame, XBaseLayer::xTYPE type, int nLayer, float az ) : XBaseKey( fFrame, type, nLayer ), XKeyRot( az ) {	Init();	}
	virtual ~XKeyDummyRot() {}
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyDummyLayer::Execute( pSprObj, fOverSec ); XKeyRot::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyRot::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyRot::SetpNextKey( XKeyDummyLayer::SetpNextKey( pKey ) ); }
}; 
// class XKeyDummyScale
class XKeyDummyScale : public XKeyDummyLayer, public XKeyScale
{
	void Init() {	}
public:
	XKeyDummyScale( ) {	Init();	}
//	XKeyDummyScale( float fFrame, XBaseLayer *pLayer, float sx, float sy ) : XBaseKey( pAction, XBaseKey::xSUB_SCALE, fFrame, pLayer ), XKeyScale( sx, sy ), XKeyDummyLayer( pAction ) {		Init();	}
	XKeyDummyScale( float fFrame, XBaseLayer::xTYPE type, int nLayer, float sx, float sy ) : XBaseKey( fFrame, type, nLayer ), XKeyScale( sx, sy ) {		Init();	}
	virtual ~XKeyDummyScale() {}
	// get/set
	// virtual
	virtual void Execute( XSprObj *pSprObj, float fOverSec=0 ) { XKeyDummyLayer::Execute( pSprObj, fOverSec ); XKeyScale::Execute( pSprObj, fOverSec ); }
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) { XKeyScale::Load( pSprDat, pRes ); }
	virtual void SetNextKey( XBaseKey* pKey ) { XKeyScale::SetpNextKey( XKeyDummyLayer::SetpNextKey( pKey ) ); }
};
*/
////////////////////////////////////////////////////
class XKeyPlaySound : public XBaseKey
{
public:
	XKeyPlaySound() : XBaseKey( XBaseKey::xKEY_SOUND ) {}
	XKeyPlaySound( XBaseKey& keyBase, XKeyPlaySound& key ) : XBaseKey( keyBase ) {
	}
	virtual ~XKeyPlaySound() {}
	virtual void Load( XSprDat *pSprDat, XResFile *pRes ) {}
};

class XKeySound : public XBaseKey
{
	ID m_idSound;
	float m_fVolume;
	void Init() {
		m_idSound = 0;
		m_fVolume = 0;
	}
	void Destroy() {}
public:
	XKeySound() { Init(); }
/*	XKeySound( float fFrame, XBaseLayer::xTYPE type, int nLayer, ID idSound ) : XBaseKey( XBaseKey::xKEY_SOUND, XBaseKey::xSUB_MAIN, fFrame, type, nLayer  )  { 
		Init();
		m_idSound = idSound;
	} */
	XKeySound( XBaseKey& keyBase, XKeySound& key ) : XBaseKey( keyBase ) {
		Init();
		m_idSound = key.GetidSound();
		m_fVolume = key.GetfVolume();
	}
	virtual ~XKeySound() { Destroy(); }
	void Execute( XSprObj *pSprObj, float fOverSec=0 );
	GET_ACCESSOR( ID, idSound );
	GET_ACCESSOR( float, fVolume );
	virtual void Load( XSprDat *pSprDat, XResFile *pRes );
};


#endif // __KEY_H__
