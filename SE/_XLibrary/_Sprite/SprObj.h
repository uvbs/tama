/*
 *  SprObj.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#ifndef __SPROBJ_H__
#define __SPROBJ_H__

#include "Key.h"
#include "Layer.h"
#include "SprDat.h"
#include "xMath.h"
#include "XGraphics.h"
//#include "XBaseObj.h"

// 이 액션에서 로드될 XSprDat들은 미리 모두 로드된다
/*typedef struct {
	CHAR szFilename[128];
	XSprObj *pSprObj;
	DWORD dwID;
} USE_SPROBJ;*/

class XLua;
// XSprObj에서 쓰는 액션정보
class XObjAct 
{
	XSprObj *m_pSprObj;
	int m_nNumLayers;
	XBaseLayer **m_ppLayers;		// 레이어 리스트
	XAniAction *m_pAction;			// 이 노드가 가리키는 SprDat에서의 액션 포인터. read only
	void Init( void ) {
		m_pAction = NULL;
		m_nNumLayers = 0;
		m_ppLayers = NULL;
		m_pSprObj = NULL;
	}
	void Destroy( void );
	void DestroyLayer( void );
public:
	XObjAct( XSprObj *pSprObj ) { Init(); m_pSprObj = pSprObj; }
	XObjAct( XSprObj *pSprObj, XAniAction *pAction ) { 
		m_pAction = pAction; 
		m_pSprObj = pSprObj; 
		m_nNumLayers = pAction->GetnNumLayerInfo();
		m_ppLayers = new XBaseLayer*[ m_nNumLayers ];
		memset( m_ppLayers, 0, sizeof(XBaseLayer*) * m_nNumLayers );
	}
	~XObjAct() { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( XAniAction*, pAction );
	GET_ACCESSOR( int, nNumLayers );
	
	// layerㅂ
	XBaseLayer *CreateLayer( int idx, XBaseLayer::xTYPE type = XBaseLayer::xNONE_LAYER, int nLayer = -1, float fAdjAxisX=0, float fAdjAxisY=0 );
	XBaseLayer *GetLayer( XBaseLayer::xTYPE type, int nLayer );
	void ClearLayer( void );
	DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX& m, BYTE *pa=NULL, BYTE *pr=NULL, BYTE *pg=NULL, BYTE *pb=NULL );
	XBaseLayer *GetLayerInPixel( float lx, float ly );
	// etc
	void FrameMove( float dt, float fFrmCurr );
#ifdef _VER_OPENGL
	void Draw( float x, float y, XEFFECT_PARAM *pEffectParam );
#else
	void Draw( float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam );
#endif
} ;
class XBaseObj;
class XSprObj
{
public:
	static float s_LuaDt;		// LuaDraw()에서 씀. 이번프레임에 결정된 dt값을 저장한다
private:
	DWORD m_dwID;												// 이 오브젝트만의고유한 아이디
	XSprDat *m_pSprDat;
	XSprObj *m_pParentSprObj;		// this가 자식이면 부모포인터를 가리킨다
	XBaseObj *m_pParent;		// 부모
	int m_nNumSprObjs;
	XSprObj **m_ppSprObjs;					// 사전에 로드되는 XSprObj들의 리스트
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
	XLua *m_pLua;					// SprObj마다 붙어있는 루아쓰레드
	void Init( void ) {
		m_dwID = 0;
		m_pSprDat = NULL;
		m_pParent = NULL;
		m_pParentSprObj = NULL;
		m_nNumSprObjs = 0;
		m_ppSprObjs = NULL;
		m_fFrameCurrent = 0;
		m_bPause = FALSE;
		_m_pObjActCurr = NULL;
//		m_pParentKey = NULL;
		m_nKeyCurr = 0;
		m_ppObjActs = NULL;
		m_nNumObjActs = 0;
		m_PlayType = xRPT_LOOP;
		m_bFinish = FALSE;
		// transform
		SetScale( 1.0f, 1.0f );
//		m_fScaleX = m_fScaleY = 1.0f;
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		m_fAlpha = 1.0f;
		m_ColorR = m_ColorG = m_ColorB = 1.0f;
		m_DrawMode = xDM_ERROR;
		m_dwDrawFlag = 0;
		m_pLua = NULL;
	}
	void Destroy( void );
	void SetpObjActCurr( XObjAct *pObjAct ) { _m_pObjActCurr = pObjAct; }
	SET_ACCESSOR( DWORD, dwID );
public:
//	XSprObj() { Init(); }
	XSprObj( DWORD dwID ) { 
		Init();
		m_dwID = dwID;
	}
#ifdef WIN32
	XSprObj( LPCTSTR szFilename, XBaseObj *pParent=NULL ) { Init(); Load( szFilename ); m_pParent = pParent; }
    // for lua
	XSprObj( BOOL bKeepSrc, const char *cFilename ) { Init(); Load( Convert_char_To_TCHAR(cFilename), bKeepSrc ); m_pParent = NULL; }
	XSprObj( BOOL bKeepSrc, LPCTSTR szFilename ) { Init(); Load( szFilename, bKeepSrc ); m_pParent = NULL; }
#else
	XSprObj( const char *szFilename, XBaseObj *pParent=NULL ) { Init(); Load( szFilename ); m_pParent = pParent; }
//	XSprObj( const char *szFilename ) { Init(); Load( szFilename ); m_pParent = NULL; }
    // for lua
	XSprObj( BOOL bKeepSrc, const char *cFilename ) { Init(); Load( cFilename, bKeepSrc ); m_pParent = NULL; }
#endif

	virtual ~XSprObj() { Destroy(); }
	
	void Reset( void ) {	// new로 생성이 안되고 캐시에서 다시 가져오게 됐을경우에도 기본적인 사항은 리셋시켜함
		ResetAction();
		SetpObjActCurr( NULL );
	}
	// get/set
//	GET_ACCESSOR( XSprDat*, pSprDat );
	XSprDat* GetpSprDat( void ) { XBREAK( m_pSprDat == NULL ); return m_pSprDat; }
	GET_ACCESSOR( DWORD, dwID );
	ID GetidSprObj( void ) { return m_dwID; }
//	GET_ACCESSOR( XObjAct*, pObjActCurr );
	XObjAct* GetpObjActCurr( void ) {
		XBREAK( _m_pObjActCurr == NULL );
		return _m_pObjActCurr;
	}
	XObjAct* GetpObjAct( ID idAct ) {
		if( XBREAK( idAct >= MAX_ID ) )
			return NULL;
		return m_ppObjActs[ idAct ];
	}
//	GET_SET_ACCESSOR( XBaseKey*, pParentKey );
	GET_ACCESSOR( float, fFrameCurrent );
	GET_SET_ACCESSOR( BOOL, bPause );
	BOOL IsPlaying( void ) { return !m_bPause; }
	GET_ACCESSOR( xRPT_TYPE, PlayType );
	GET_ACCESSOR( XLua*, pLua );
//	GET_ACCESSOR( BOOL, bFinish );
	void SetRotateX( float angle ) { m_fRotX = angle; }
	void SetRotateY( float angle ) { m_fRotY = angle; }
	void SetRotateZ( float angle ) { m_fRotZ = angle; }
	void SetRotate( float angle ) { SetRotateZ( angle ); }
	GET_ACCESSOR( float, fRotX );
	GET_ACCESSOR( float, fRotY );
	GET_ACCESSOR( float, fRotZ );
	float GetRotate( void ) { return GetfRotZ(); }
	void SetScale( float scalexy ) { SetScale( scalexy, scalexy ); }
	void SetScale( XE::VEC2 vScale ) { SetScale( vScale.x, vScale.y ); }
	void SetScale( float sx, float sy ) { m_fScaleX = sx; m_fScaleY = sy; 	}
	float GetScaleX( void ) { return m_fScaleX; }
	float GetScaleY( void ) { return m_fScaleY; }
	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	GET_SET_ACCESSOR( xDM_TYPE, DrawMode );
	GET_SET_ACCESSOR( float, fAlpha );
	void SetColor( float r, float g, float b ) {
		m_ColorR = r;
		m_ColorG = g;
		m_ColorB = b;
	}
	GET_ACCESSOR( float, ColorR );
	GET_ACCESSOR( float, ColorG );
	GET_ACCESSOR( float, ColorB );
	LPCTSTR GetszFilename( void ) { return m_pSprDat->GetszFilename(); }
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
	GET_SET_ACCESSOR( XBaseObj*, pParent );
	GET_SET_ACCESSOR( XSprObj*, pParentSprObj );
	void AddRotate( float ax, float ay, float az ) { m_fRotX += ax; m_fRotY += ay; m_fRotZ += az; }
	float GetSpeedCurrentAction( void ) {
#ifdef _XDEBUG
		if( !GetAction() )
			XLOG( "sprobj id=%d %s GetAction()=NULL", m_dwID, m_pSprDat->GetszFilename() );
#endif
		return GetAction()->GetfSpeed(); 
	}
	// 이거 완전땜빵. 이런함수를 만들면안되고 각 액션마다 바운딩박스를 만들어야 한다.
	XSprite* GetSprite( void ) {
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
			return pLayer->GetpSpriteCurr();
		return NULL;
	}
	float GetWidth( void ) { 
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
			return pLayer->GetpSpriteCurr()->GetWidth() * m_fScaleX * pLayer->GetcnScale().fScaleX;
		return 0;
	}
	float GetHeight( void ) { 
		XBREAK( GetpObjActCurr() == NULL );
		XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );
		if( pLayer && pLayer->GetpSpriteCurr() )
				return pLayer->GetpSpriteCurr()->GetHeight() * m_fScaleY * pLayer->GetcnScale().fScaleY;
		return 0;
	}
	XE::VEC2 GetSize( void ) { return XE::VEC2( GetWidth(), GetHeight() ); }
	XE::VEC2 GetSize( ID idAct ) { 
		XBREAK( GetpObjAct( idAct ) == NULL );
		XBaseLayer *pBaseLayer = GetpObjAct( idAct )->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 );
		if( pBaseLayer )
		{
			XLayerImage *pLayer = SafeCast<XLayerImage*, XBaseLayer*>( pBaseLayer );
			XBREAK( pLayer == NULL );
			if( pLayer->GetpSpriteCurr() )
			{
				XBREAK( pLayer->GetpSpriteCurr() == NULL );
				return XE::VEC2( pLayer->GetpSpriteCurr()->GetWidth() * m_fScaleX * pLayer->GetcnScale().fScaleX,
									pLayer->GetpSpriteCurr()->GetHeight() * m_fScaleY * pLayer->GetcnScale().fScaleY );
			}
		}
		return XE::VEC2();
	}
	XE::VEC2 GetAdjust( void ) {
		XE::VEC2 v;
		XLayerImage *pLayer = (XLayerImage *)GetpObjActCurr()->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 );
		if( pLayer )
			if( pLayer->GetpSpriteCurr() ) {
				return pLayer->GetpSpriteCurr()->GetAdjust() * m_fScaleY * pLayer->GetcnScale().fScaleY;
			}
		return v;
	}
	
	BOOL IsFinish( void ) {				// 애니메이션이 끝났는가?
		return m_bFinish;
	}
	LPCTSTR GetSprFilename( void ) { return (GetpSprDat())? GetpSprDat()->GetszFilename() : NULL; }

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
		if( m_fRotY == 180.0f )
		{
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
	// draw
	void Draw( int x, int y ) { Draw( (float)x, (float)y ); }
#ifdef _VER_OPENGL
	void Draw( float x, float y );
#endif
#ifdef WIN32
	void Draw( float x, float y ) { D3DXMATRIX m; D3DXMatrixIdentity( &m ); Draw( x, y, m ); }
	void Draw( float x, float y, const D3DXMATRIX &m );
#endif
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
	// action
	void ResetAction( void )	{	// 현재설정된 액션그대로 초기화만 시킨다
		m_fFrameCurrent = 0;
		SetKeyCurrStart();
		m_bFinish = FALSE;
	}
	void SetAction( DWORD id, xRPT_TYPE playType = xRPT_LOOP, BOOL bExecFrameMove=TRUE );
	XAniAction *GetAction( void ) { 
		XBREAKF( !GetpObjActCurr(), "sprobj id=%d %s m_pObjActCurr=NULL", m_dwID, m_pSprDat->GetszFilename() );
		return GetpObjActCurr()->GetpAction(); 
	}
	XAniAction *GetAction( ID idAct ) {
		if( XBREAK( idAct >= MAX_ID ) )		// id는 unsigned이므로 < 0은 검사할필요 없음
			return NULL;
		return m_ppObjActs[ idAct ]->GetpAction();
	}
	ID GetActionID( void ) {		// GetAction()->GetID()이렇게 쓰지말고 이걸쓸것
		XAniAction *pAction = GetAction();
		return (pAction)? pAction->GetID() : 0;
	}
	XObjAct *AddObjAct( int idx, XAniAction *pAction );
	// key
	void SetKeyCurrStart( void ) {	m_nKeyCurr = 0; }			// 현재 가리키는 키를 맨 처음으로 돌린다.
	
	// layer
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
		if( pLayer->IsTypeLayerMove() == FALSE ) {
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
	// file
	BOOL Load( LPCTSTR szFilename, BOOL bKeepSrc=FALSE );
	// preload sprobj
	XSprObj* AddSprObj( LPCTSTR szSprObj, ID idAct, xRPT_TYPE playMode, ID idBase, ID idLocal );
	// virtual 
//	virtual XLua* CreateScript( void ) { return new XLua; }
//	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XBaseObj *pParent=NULL ) { return new XSprObj( szSpr, pParent ); }
	static void RegisterLua( XLua *pLua );
	// lua
	void LuaSetAction( DWORD idAct, xRPT_TYPE playType ) { SetAction( idAct, playType );	}
	void LuaDraw( float x, float y ) { FrameMove( s_LuaDt ); Draw( x, y ); }
	void LuaSetRotate( float angle ) { SetRotate( angle ); }
	void LuaSetScale( float scale ) { SetScale( scale ); }
	void LuaSetScaleXY( float sx, float sy ) { SetScale( sx, sy ); }
	XSprObj* LuaAddSprObj( ID idBase, ID idLocal, const char *cSprObj, ID idAct, xRPT_TYPE playMode ) {
		XBREAKF( idAct == 0, "LuaAddSprObj: idAct가 지정되지 않았다" );
		return AddSprObj( Convert_char_To_TCHAR( cSprObj ), idAct, playMode, idBase, idLocal );
	}
//	friend class XKeyCreateObj;
};

#endif 