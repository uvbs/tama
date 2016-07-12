/*
 *  SprObj.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once

#include "Key.h"
#include "Layer.h"
#include "SprDat.h"
#include "etc/xMath.h"
#include "etc/xGraphics.h"
//#include "XBaseObj.h"
#ifdef _SPR_USE_LUA
#undef  _SPR_USE_LUA
#endif
#ifdef _SPR_USE_LUA
#include "XLua.h"
#endif
#ifdef _VER_OPENGL
//#include "Mathematics.h"
#endif

class XSprObj;
struct LAYER_INFO;

XE_NAMESPACE_START( xSpr )
// KeyEvent 델리게이트시 넘겨주는 파라메터
struct xEvent {
	_tstring m_strSpr;
	ID m_idAct = 0;
	xRPT_TYPE m_PlayMode = xRPT_LOOP;
	XSprObj* m_pSprObj = nullptr;
	XKeyEvent* m_pKey = nullptr;
	ID m_idEvent = 0;
	XE::VEC2 m_vLocal;
	float m_dAng = 0;
	float m_fOverSec = 0;
	float m_Scale = 1.f;
	bool m_bTraceParent = false;
	float m_secLifeTime = 0;
};
//
XE_NAMESPACE_END; // xSpr


class XDelegateSprObj
{
private:
	void _Init() {}
	void _Destroy() { }
public:
	XDelegateSprObj() { _Init();}
	virtual ~XDelegateSprObj() { _Destroy(); }	
	// get/set
	// virtual
//	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XDelegateSprObj* pParent=NULL ) { return new XSprObj( szSpr, pParent ); }
//	virtual void OnEventCreateObj( float lx, float ly, WORD id, float fAngle, float fOverSec ) {}
	virtual void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, ID idEvent, float fAngle, float fOverSec ) {}
	virtual void OnProcessDummy( ID id, const XE::VEC2& vLocalT, float ang, const XE::VEC2& vScale, const CHANNEL_EFFECT& cnEffect, float dt, float fFrmCurr ) {}
	virtual void OnEventCreateSfx( XSprObj *pSprObj, XBaseKey *pKey, float lx, float ly, float scale, LPCTSTR szSpr, ID idAct, xRPT_TYPE typeLoop, float secLifeTime, BOOL bTraceParent, float fAngle, float fOverSec ) {}
	virtual void OnEventCreateSfx( const xSpr::xEvent& event ) {}
	virtual void OnEventEtc( const xSpr::xEvent& event ) {}
	virtual void OnFinishPlayAction( XSprObj *pSprObj, ID idAct, ID idDelegate ) {}
	// 델리게이트에서 XLayerImage::SetDrawInfoToSpr()로 pSpr을 세팅해주고 pSpr을 드로잉 해주시오
	virtual BOOL OnDelegateDrawImageLayer( XSprObj *pSprObj, XSprite *pSprSrc, XLayerImage *pImageLayer, XEFFECT_PARAM *pEffectParam, float x, float y, const MATRIX &mParent ) { return FALSE; }
	// 오브젝트 생성키가 실행되기전 불려지는 델리게이트
	virtual BOOL OnDelegateCreateObj( XSprObj *pSprObj, XSprite *pSprSrc, XLayerObject *pImageLayer, XEFFECT_PARAM *pEffectParam, float x, float y, const MATRIX &mParent ) { return FALSE; }
};

// 이 액션에서 로드될 XSprDat들은 미리 모두 로드된다
/*typedef struct {
	CHAR szFilename[128];
	XSprObj *pSprObj;
	DWORD dwID;
} USE_SPROBJ;*/

class XLua;
//////////////////////////////////////////////////////////////////////////
// XSprObj에서 쓰는 액션정보
class XObjAct 
{
	XSprObj *m_pSprObj;
	int m_nNumLayers;
	XBaseLayer **m_ppLayers;		// 레이어 리스트
	XAniAction *m_pAction;			// 이 노드가 가리키는 SprDat에서의 액션 포인터. read only
	void Init() {
		m_pAction = NULL;
		m_nNumLayers = 0;
		m_ppLayers = NULL;
		m_pSprObj = NULL;
	}
	void Destroy();
	void DestroyLayer();
public:
	XObjAct( XSprObj *pSprObj ) { Init(); m_pSprObj = pSprObj; }
	XObjAct( XSprObj *pSprObj, XAniAction *pAction );
	~XObjAct() { Destroy(); }
	// get/set
	GET_SET_ACCESSOR_CONST( XAniAction*, pAction );
	GET_ACCESSOR( int, nNumLayers );
	
	// layerㅂ
// 	template<typename T>
// 	T* GetpLayerByidLocalInLayerT( ID idLocalInLayer ) {
// 		XBaseLayer **ppLayers = m_ppLayers;
// 		int num = m_nNumLayers;
// 		while( num-- ) {
// 			XBaseLayer *pLayer = *ppLayers++;
// 			if( pLayer == nullptr )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
// 			if( pLayer->GetidLayer() == idLocalInLayer )
// 				return SafeCast<T*>( pLayer );
// 		}
// 		return nullptr;
// 	}
	XBaseLayer* GetpLayerByidLocalInLayer( ID idLocalInLayer ) const;
	XBaseLayer *CreateLayer( int idx, XBaseLayer::xTYPE type = XBaseLayer::xNONE_LAYER, int nLayer = -1, float fAdjAxisX=0, float fAdjAxisY=0 );
	XBaseLayer *CreateLayer( int idx, LAYER_INFO* pLayerInfo );
	XBaseLayer *GetLayer( XBaseLayer::xTYPE type, int nLayer );
	void ClearLayer();
	DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX& m, BYTE *pa, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL );
	XBaseLayer *GetLayerInPixel( float lx, float ly );
	// etc
	void FrameMove( float dt, float fFrmCurr );
#ifdef _VER_OPENGL
	void Draw( float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam );
#else
	void Draw( float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam );
#endif
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar );

} ;
//////////////////////////////////////////////////////////////////////////
class XBaseObj;
class XDelegateSprObj;
class XSprObj
{
public:
	static float s_LuaDt;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다
private:
#ifdef _XSPR_LAZY_LOAD
	// Lazy로딩할때 필요한 정보
	struct LAZY_INFO {
		ID idAct;			// SetAction되었으나 SprDat가 없어서 세팅되지 못하고 예약상태로 올라감.
		xRPT_TYPE playType;
		_tstring strFilename;	// 읽으려고 한 spr파일명
		CTimer timerLazyLoad;
		BOOL bKeepSrc;
		XE::VEC3 vHSL;
		LAZY_INFO() {
			idAct = 0;
			playType = xRPT_DEFAULT;
			bKeepSrc = FALSE;
		}
	};
	LAZY_INFO m_LazyInfo;
#endif
	struct xUseSprObj {
		ID m_idActParent = 0;
		ID m_idLayerParent = 0;
		XSprObj* m_pSprObj = nullptr;
	};
	DWORD m_dwID;												// 이 오브젝트만의고유한 아이디
	XSprDat *m_pSprDat;
	XSprObj *m_pParentSprObj;		// this가 자식이면 부모포인터를 가리킨다
	XDelegateSprObj *m_pDelegate;		// 델리게이트
	int m_nNumSprObjs;
//	XSprObj **m_ppSprObjs;					// 사전에 로드되는 XSprObj들의 리스트
	XVector<xUseSprObj> m_aryUseSprObj;
	float m_fFrameCurrent;						// 현재 프레임번호
	BOOL m_bPause;
//	XBaseKey *m_pParentKey;						// 이 오브젝트를 생성하게한 키가 있다면 저장. 없으면 널
	int m_nKeyCurr;							// 현재 가리키고 있는 노드의 index
	XObjAct *_m_pObjActCurr;				// 현재 선택된 액션. 이거 직접 쓰지말것. 이거 널로 되어있으면 경고띄워주기 위함
	int m_nNumObjActs;
	XObjAct **m_ppObjActs;			// sprdat의 액션수만큼 똑같이 생성시켜 각각 레이어를 가지고 있는다
	// transform
	float	m_fAdjustAxisX, m_fAdjustAxisY;	// 회전축보정
	float m_fRotX, m_fRotY, m_fRotZ;
	float m_fScaleX, m_fScaleY;
	float	m_fAlpha;					// 외부지정 옵션
	float	m_ColorR, m_ColorG, m_ColorB;		// 컬러
	xDM_TYPE m_DrawMode;		// 외부지정 옵션
	DWORD m_dwDrawFlag;		// EFF_****
	xRPT_TYPE m_PlayType;			// 루핑등등
	BOOL m_bFinish;					// 애니메이션이 끝났는가
	float m_multiplySpeed;				// 애니메이션 스피드 배속
  bool m_bCallHandler = false;    // 이벤트키의 콜백이 실행중.
#ifdef _SPR_USE_LUA
	XLua *m_pLua;					// SprObj마다 붙어있는 루아쓰레드
#endif 
	void Init() {
		m_dwID = XE::GenerateID();
		m_pSprDat = NULL;
		m_pDelegate = NULL;
		m_pParentSprObj = NULL;
		m_nNumSprObjs = 0;
//		m_ppSprObjs = NULL;
		m_fFrameCurrent = 0;
		m_bPause = FALSE;
		_m_pObjActCurr = NULL;
//		m_pParentKey = NULL;
		m_nKeyCurr = 0;
		m_ppObjActs = NULL;
		m_nNumObjActs = 0;
		m_PlayType = xRPT_LOOP;
		m_bFinish = FALSE;
		m_multiplySpeed = 1.0f;
		// transform
		SetScale( 1.0f, 1.0f );
//		m_fScaleX = m_fScaleY = 1.0f;		// 위에서 SetScale()로 하므로 지움
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		m_fAlpha = 1.0f;
		m_ColorR = m_ColorG = m_ColorB = 1.0f;
		m_DrawMode = xDM_ERROR;
		m_dwDrawFlag = 0;
#ifdef _SPR_USE_LUA
		m_pLua = NULL;
#endif
	}
	void Destroy();
	void SetpObjActCurr( XObjAct *pObjAct ) { _m_pObjActCurr = pObjAct; }
	SET_ACCESSOR( DWORD, dwID );
	XSprDat* GetpSprDat() {
//		XBREAK( m_pSprDat == NULL );		// 비동기 로딩써야하므로 브레이크 걸리면 안됨.
		return m_pSprDat;
	}
public:
	XSprObj() { Init(); }		// 툴에서 필요해서 살림. new XSprObj;로만 생성해서 sprObj->Load()로 읽어서 에러검출 할수 있도록.
	XSprObj( DWORD dwID );
	XSprObj( LPCTSTR szFilename, XDelegateSprObj *pDelegate = nullptr );
	XSprObj( LPCTSTR szFilename, const XE::VEC3& vHSL, XDelegateSprObj *pDelegate = nullptr );
  // for lua
	XSprObj( BOOL bKeepSrc, const char *cFilename );
#ifdef WIN32
	XSprObj( BOOL bKeepSrc, LPCTSTR szFilename );
#endif // WIN32
	XSprObj( const _tstring& strSpr, XDelegateSprObj *pDelegate = nullptr ) 
		: XSprObj( strSpr.c_str(), pDelegate ) {}

	virtual ~XSprObj() { Destroy(); }
	
	void Reset() {	// new로 생성이 안되고 캐시에서 다시 가져오게 됐을경우에도 기본적인 사항은 리셋시켜함
		ResetAction();
		SetpObjActCurr( NULL );
	}
	// get/set
//	GET_ACCESSOR( XSprDat*, pSprDat );
	GET_ACCESSOR( DWORD, dwID );
	ID GetidSprObj() { return m_dwID; }
//	GET_ACCESSOR( XObjAct*, pObjActCurr );
  GET_SET_ACCESSOR( bool, bCallHandler );
	XObjAct* GetpObjActCurr() const {
#ifdef _XSPR_LAZY_LOAD
		if( m_pSprDat == NULL )
			NULL;
#else
		XBREAK( _m_pObjActCurr == NULL );
#endif
		return _m_pObjActCurr;
	}
	XObjAct* GetpObjAct( ID idAct ) const;
	BOOL IsError() {
		return m_pSprDat == NULL;
	}
//	GET_SET_ACCESSOR( XBaseKey*, pParentKey );
	GET_ACCESSOR_CONST( float, fFrameCurrent );
	GET_SET_ACCESSOR_CONST( BOOL, bPause );
	void SetbPause( bool bPause ) {
		m_bPause = (bPause)? TRUE : FALSE;
	}
	BOOL IsPlaying() const { return !m_bPause; }
	GET_ACCESSOR_CONST( xRPT_TYPE, PlayType );
#ifdef _SPR_USE_LUA
	GET_ACCESSOR( XLua*, pLua );
#endif
//	GET_ACCESSOR( BOOL, bFinish );
	void SetRotateX( float angle ) { m_fRotX = angle; }
	void SetRotateY( float angle ) { m_fRotY = angle; }
	void SetRotateZ( float angle ) { m_fRotZ = angle; }
	void SetRotate( float angle ) { SetRotateZ( angle ); }
	GET_ACCESSOR_CONST( float, fRotX );
	GET_ACCESSOR_CONST( float, fRotY );
	GET_ACCESSOR_CONST( float, fRotZ );
	float GetRotate() const { return GetfRotZ(); }
	void SetScale( float scalexy ) { SetScale( scalexy, scalexy ); }
	void SetScale( const XE::VEC2& vScale ) { SetScale( vScale.x, vScale.y ); }
	void SetScale( float sx, float sy ) { m_fScaleX = sx; m_fScaleY = sy; 	}
	float GetScaleX() const { return m_fScaleX; }
	float GetScaleY() const { return m_fScaleY; }
	XE::VEC2 GetScale() const { return XE::VEC2( m_fScaleX, m_fScaleY ); }
	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	void SetAdjustAxis( const XE::VEC2& vAdjAxis ) { 
		m_fAdjustAxisX = vAdjAxis.x; m_fAdjustAxisY = vAdjAxis.y; 
	}
	GET_SET_ACCESSOR_CONST( xDM_TYPE, DrawMode );
	GET_SET_ACCESSOR_CONST( float, fAlpha );
	GET_SET_ACCESSOR_CONST( float, multiplySpeed );
	void SetColor( float r, float g, float b ) {
		m_ColorR = r;
		m_ColorG = g;
		m_ColorB = b;
	}
	void SetColor( XCOLOR col ) {
		m_ColorR = XCOLOR_RGB_R(col) / 255.f;
		m_ColorG = XCOLOR_RGB_G(col) / 255.f;
		m_ColorB = XCOLOR_RGB_B(col) / 255.f;
	}
	GET_ACCESSOR_CONST( float, ColorR );
	GET_ACCESSOR_CONST( float, ColorG );
	GET_ACCESSOR_CONST( float, ColorB );
//	LPCTSTR GetszFilename() { return m_pSprDat->GetszFilename(); }
	LPCTSTR GetSprFilename() {
		return (GetpSprDat())? GetpSprDat()->GetszFilename() : _T("");
	}
	inline LPCTSTR GetszFilename() {
		return GetSprFilename();
	}
	void SetFlipHoriz( BOOL bFlag ) {
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_HORIZ;
			SetRotateY(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_HORIZ;  
			SetRotateY(0);
		}
	}
		
	void SetFlipVert( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_VERT;
			SetRotateX(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_VERT;  
			SetRotateX(0);
		}
	}
	BOOL IsFlipHoriz() {
		return (m_dwDrawFlag & EFF_FLIP_HORIZ);
	}
	GET_SET_ACCESSOR( XDelegateSprObj*, pDelegate );
	GET_SET_ACCESSOR( XSprObj*, pParentSprObj );
	void AddRotate( float ax, float ay, float az ) { m_fRotX += ax; m_fRotY += ay; m_fRotZ += az; }
	float GetSpeedCurrentAction() {
#ifdef _XDEBUG
		if( !GetAction() )
			XLOG( "sprobj id=%d %s GetAction()=NULL", m_dwID, m_pSprDat->GetszFilename() );
#endif
		return GetAction()->GetfSpeed(); 
	}
	// 이거 완전땜빵. 이런함수를 만들면안되고 각 액션마다 바운딩박스를 만들어야 한다.
/*	XSprite* GetSprite() {
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
			return pLayer->GetpSpriteCurr();
		return NULL;
	} */
	// 땜빵계열 함수들. 차차 없애나가자.
/*	float GetWidth() { 
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
			return pLayer->GetpSpriteCurr()->GetWidth() * m_fScaleX * pLayer->GetcnScale().fScaleX;
		return 0;
	}
	float GetHeight() { 
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
				return pLayer->GetpSpriteCurr()->GetHeight() * m_fScaleY * pLayer->GetcnScale().fScaleY;
		return 0;
	} */
	float GetWidth() {
		XBREAK( GetpObjActCurr() == NULL );
		XBREAK( GetpObjActCurr()->GetpAction() == NULL );
		XE::VEC2 vLT = GetpObjActCurr()->GetpAction()->GetBoundBoxLT();
		XE::VEC2 vRB = GetpObjActCurr()->GetpAction()->GetBoundBoxRB();
		return (vRB.x - vLT.x) * GetScaleX();
	}
	float GetHeight() {
		XBREAK( GetpObjActCurr() == NULL );
		XBREAK( GetpObjActCurr()->GetpAction() == NULL );
		XE::VEC2 vLT = GetpObjActCurr()->GetpAction()->GetBoundBoxLT();
		XE::VEC2 vRB = GetpObjActCurr()->GetpAction()->GetBoundBoxRB();
		return (vRB.y - vLT.y) * GetScaleY();
	}
	XE::VEC2 GetSize() { return XE::VEC2( GetWidth(), GetHeight() ); }
// 	XE::VEC2 GetSize( ID idAct ) { 
// 		XBREAK(1);
// 		// 이거 사용하지 말것. 제대로 동작 안함. GetSize(void)를 사용할것.
// 		XBREAK( GetpObjAct( idAct ) == NULL );
// 		XBaseLayer *pBaseLayer = GetpObjAct( idAct )->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 );
// 		if( pBaseLayer )
// 		{
// 			XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( pBaseLayer );
// 			XBREAK( pLayer == NULL );
// 			if( pLayer->GetpSpriteCurr() )
// 			{
// 				XBREAK( pLayer->GetpSpriteCurr() == NULL );
// 				return XE::VEC2( pLayer->GetpSpriteCurr()->GetWidth() * m_fScaleX * pLayer->GetcnScale().fScaleX,
// 									pLayer->GetpSpriteCurr()->GetHeight() * m_fScaleY * pLayer->GetcnScale().fScaleY );
// 			}
// 		}
// 		return XE::VEC2();
// 	}
	XE::VEC2 GetAdjust() {
		XE::VEC2 v;
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = (XLayerImage *)GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 );
		if( pLayer )
			if( pLayer->GetpSpriteCurr() ) {
//				return pLayer->GetpSpriteCurr()->GetAdjust() * m_fScaleY * pLayer->GetcnScale().fScaleY;
				return pLayer->GetpSpriteCurr()->GetAdjust() * GetScale() * pLayer->GetcnScale().m_vScale;
			}
		return v;
	}
	
	BOOL IsFinish() const {				// 애니메이션이 끝났는가?
		return m_bFinish;
	}
	void DoFinish() {
		m_bFinish = TRUE;
	}

	// frame
	void Transform( XE::VEC2 *pvOutPos ) { Transform( &pvOutPos->x, &pvOutPos->y ); }
	void Transform( float *lx, float *ly ) {
		Vec3 v = Vec3(*lx, *ly, 0);
		MATRIX m, mRotX, mRotY, mRotZ, mScale;
		MatrixRotationX( mRotX, D2R(m_fRotX) );
		MatrixRotationY( mRotY, D2R(m_fRotY) );
		MatrixRotationZ( mRotZ, D2R(m_fRotZ) );
		MatrixScaling( mScale, GetScaleX(), GetScaleY(), 1.0f );
		MatrixIdentity( m );
		MatrixMultiply( m, m, mScale );
		MatrixMultiply( m, m, mRotX );
		MatrixMultiply( m, m, mRotY );
		MatrixMultiply( m, m, mRotZ );
		Vec4 v4d;
		MatrixVec4Multiply( v4d, v, m );
		*lx = v4d.x;
		*ly = v4d.y;
	}
	void Transform( float *fAngle ) {
		// 좌우플립됐을경우는....
		if( m_fRotY == 180.0f ) {
			float a = *fAngle;
			if( a > 180.0f )		// 일단 각도계를 -180~+180기준으로 바꾼다음
				a -= 360.0f;
			else if( a < -180.0f )
				a += 360.0f;
			*fAngle = -a;		// 부호를 바꿔준다
		}
		// 다른 각도변환은 일단은 하지 않는다.
	}
	void FrameMove( float dt );
	void JumpKeyPos( XAniAction *pAction, float fJumpFrame );	// fFrameCurrent위치로 바로 점프한다. 그사이의 키는 실행하지 않는다
	void JumpToRandomFrame();
	// draw
//	void Draw( int x, int y ) { Draw( (float)x, (float)y ); }
#ifdef _VER_OPENGL
	void Draw( float x, float y ) {
        MATRIX m;
        MatrixIdentity( m );
        Draw( x, y, m );
    }
	void Draw( float x, float y, const MATRIX &m );
#endif
#ifdef _VER_DX
	inline void Draw( float x, float y ) { 
		D3DXMATRIX m; 
		D3DXMatrixIdentity( &m ); 
		Draw( x, y, m ); 
	}
	void Draw( float x, float y, const D3DXMATRIX &m );
#endif
	inline void Draw( const XE::VEC2& vPos ) { 
		Draw( vPos.x, vPos.y ); 
	}
	inline void Draw( const XE::VEC2& vPos, const MATRIX &m ) {
		Draw( vPos.x, vPos.y, m );
	}
	// action
	BOOL IsHaveAction( ID idAct ) {
#ifdef _XSPR_LAZY_LOAD
		if( m_pSprDat == NULL )
			return FALSE;
		return m_pSprDat->IsHaveAction( idAct );
#else
		XBREAK( m_pSprDat == NULL );
#endif
	}
	inline void ResetAction()	{	// 현재설정된 액션그대로 초기화만 시킨다
		m_fFrameCurrent = 0;
		SetKeyCurrStart();
		m_bFinish = FALSE;
	}
	void SetAction( DWORD id, xRPT_TYPE playType = xRPT_LOOP, BOOL bExecFrameMove=TRUE );
	XAniAction *GetAction() const { 
#ifdef _XSPR_LAZY_LOAD
		if( m_pSprDat == NULL )
			return NULL;
#else
		XBREAKF( !GetpObjActCurr(), "sprobj id=%d %s m_pObjActCurr=NULL", m_dwID, m_pSprDat->GetszFilename() );
#endif
		if( XBREAK( GetpObjActCurr() == nullptr ) )
			return nullptr;
		return GetpObjActCurr()->GetpAction(); 
	}
	XAniAction *GetAction( ID idAct );
	ID GetActionID() {		// GetAction()->GetID()이렇게 쓰지말고 이걸쓸것
		XAniAction *pAction = GetAction();
		return (pAction)? pAction->GetID() : 0;
	}
	XObjAct *AddObjAct( int idx, XAniAction *pAction );
	// 애니메이션 최대 플레이 시간을 초단위로 얻는다.
	inline float GetPlayTime() {
		XAniAction *pAction = GetAction();
		if( pAction )
			return pAction->GetPlayTime();
		return 0.0f;
	}
	// key
	void SetKeyCurrStart() {	m_nKeyCurr = 0; }			// 현재 가리키는 키를 맨 처음으로 돌린다.
	
	// layer
	XBaseLayer* GetpLayerByidLocalInLayer( XObjAct* pActObj, ID idLocalInLayer );
	template<typename T>
	T* GetpLayerByidLocalInLayerT( XObjAct* pActObj, ID idLocalInLayer ) {
		if( XASSERT(pActObj) )
			return SafeCast<T*>( pActObj->GetpLayerByidLocalInLayer( idLocalInLayer ) );
		return nullptr;
	}

	XBaseLayer *GetLayer( XBaseLayer::xTYPE type, int nLayer ) { 
		XBaseLayer *pLayer = GetpObjActCurr()->GetLayer( type, nLayer ); 
#ifdef _XDEBUG
		if( pLayer == NULL ) {
			XERROR( "레이어를 찾을수 없다. %d %d", (int)type, nLayer );
			return NULL;
		}
#endif
		return pLayer;
	} // lx, ly는 this가 화면에 출력된 좌표를 0,0으로 하는 로컬좌표
	XLayerMove* GetpLayerMove( XBaseLayer::xTYPE type, int nLayer ) { 
		XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
		if( pLayer && pLayer->IsTypeLayerMove() == FALSE ) {
			XERROR( "LayerMove타입이 아니다" );
			return NULL;
		}
#endif
		return (XLayerMove *)pLayer;
	}
	XLayerImage* GetpLayerImage( XBaseLayer::xTYPE type, int nLayer ) { 
		XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
		if( pLayer->IsTypeLayerImage() == FALSE ) {
			XERROR( "LayerImage타입이 아니다" );
			return NULL;
		}
#endif
		return (XLayerImage *)pLayer;
	}
	XLayerObject* GetpLayerObj( XBaseLayer::xTYPE type, int nLayer ) { 
		XBaseLayer *pLayer = GetLayer( type, nLayer );
#ifdef _XDEBUG
		if( pLayer->IsTypeLayerObj() == FALSE ) {
			XERROR( "LayerObj타입이 아니다" );
			return NULL;
		}
#endif
		return (XLayerObject *)pLayer;
	}
	// etc
	DWORD GetPixel( float cx, float cy, float mx, float my, BYTE *pa=NULL, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL ) { 
		MATRIX m; 
		MatrixIdentity( m ); 
		return GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
	}
	DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa=NULL, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL );
	MATRIX* GetMatrix( MATRIX *pOut, float lx, float ly );
	BOOL IsDrawOutPartly( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyLeft( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyRight( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyBottom( const XE::VEC2& vPos );
	BOOL IsDrawOutPartlyTop( const XE::VEC2& vPos );
	void SetPlayTime( float secPlay );
	XSprite* GetSprite( int idx ) {
		if( XBREAK( m_pSprDat == NULL ) )
			return NULL;
		return m_pSprDat->GetSprite( idx );
	}

	// file
	BOOL Load( LPCTSTR szFilename, const XE::VEC3& vHSL, BOOL bKeepSrc, bool bAsyncLoad );
	// preload sprobj
	XSprObj* AddSprObj( LPCTSTR szSprObj, ID idAct, xRPT_TYPE playMode, ID idBase, ID idLocal, ID idActParent, ID idLayerParent );
	// virtual 
#ifdef _SPR_USE_LUA
	virtual XLua* CreateScript();
	static void RegisterLua( XLua *pLua );
#endif
	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XDelegateSprObj *pDelegate=NULL );
	// lua
	void LuaSetAction( DWORD idAct, xRPT_TYPE playType ) { SetAction( idAct, playType );	}
	void LuaDraw( float x, float y ) { FrameMove( s_LuaDt ); Draw( x, y ); }
	void LuaSetRotate( float angle ) { SetRotate( angle ); }
	void LuaSetScale( float scale ) { SetScale( scale ); }
	void LuaSetScaleXY( float sx, float sy ) { SetScale( sx, sy ); }
	XSprObj* LuaAddSprObj( ID idBase, ID idLocal, const char *cSprObj, ID idAct, xRPT_TYPE playMode ) {
		XBREAKF( idAct == 0, "LuaAddSprObj: idAct가 지정되지 않았다" );
		return AddSprObj( Convert_char_To_TCHAR( cSprObj ), idAct, playMode, idBase, idLocal, 0, 0 );
	}
	BOOL RecreateLua() ;
//	friend class XKeyCreateObj;
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar );
};

