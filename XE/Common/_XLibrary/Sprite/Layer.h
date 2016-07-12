/*
 *  Layer.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#ifdef _SERVER
#error("서버에서 이 파일이 include되어선 안됨")
#endif 
#include "etc/XSurfaceDef.h"
#include "etc/xMath.h"
#include "Sprdef.h"


struct XEFFECT_PARAM
{
	xDM_TYPE drawMode;
	float fAlpha;
	XEFFECT_PARAM() {
		drawMode = xDM_NORMAL;
		fAlpha = 1.0f;
	}
};

class XSprite;
class XSprObj;
///////////////////////////////////////////////////////////////////////////////
class XBaseLayer
{
public:
// 	enum xSpr::xtLayer { xSpr::xLT_NONE = 0
// 						, xSpr::xLT_IMAGE
// 						, xSpr::xLT_OBJ
// 						, xSpr::xLT_SOUND
// 						, xSpr::xLT_EVENT
// 						, xSpr::xLT_DUMMY
// 						, xMAX_LAYER };
	enum xtInterpolation { xNONE=0, xLINEAR, xACCEL, xDEACCEL, xSMOOTH, xSPLINE };
	static xSpr::xtLayer sConvertNewToOld( WORD w0 );
	static LPCTSTR GetTypeStr( xSpr::xtLayer type = xSpr::xLT_NONE ) { 
		switch( type ) 
		{
		case xSpr::xLT_IMAGE:		return _T("xSpr::xLT_IMAGE");
		case xSpr::xLT_OBJ:			return _T("xSpr::xLT_OBJ");
		case xSpr::xLT_SOUND:		return _T("xSpr::xLT_SOUND");
		case xSpr::xLT_EVENT:		return _T("xSpr::xLT_EVENT");
		case xSpr::xLT_DUMMY:	return _T("xSpr::xLT_DUMMY");
		default:
			XLOG( "잘못된 레이어 타입 %d", (int)type );
			return _T("xERROR_LAYER");
		}
	}
	LPCTSTR GetTypeString( xSpr::xtLayer type = xSpr::xLT_NONE ) { 
		if( type == xSpr::xLT_NONE )
			type = GetType();
		return XBaseLayer::GetTypeStr( type );
	}
private:
	ID m_idLayer;
	xSpr::xtLayer m_Type;							// 
	int m_nLayer;								// 레이어번호. 번호는 type마다 따로매겨진다
	void Init( void ) {
		m_idLayer = 0;
		m_Type = xSpr::xLT_NONE;
		m_nLayer = 0;
	}
public:
	XBaseLayer() { Init(); }
	XBaseLayer( xSpr::xtLayer type ) {
		Init();
		m_Type = type;
	}
	virtual ~XBaseLayer() {}
	GET_SET_ACCESSOR_CONST( ID, idLayer );
	GET_SET_ACCESSOR( xSpr::xtLayer, Type );
	GET_SET_ACCESSOR( int, nLayer );
	virtual void Clear( void ) { }
	BOOL IsTypeLayerMove( void ) { return ( m_Type == xSpr::xLT_IMAGE || m_Type == xSpr::xLT_OBJ || m_Type == xSpr::xLT_DUMMY ) ? TRUE : FALSE;	}
	BOOL IsTypeLayerImage( void ) { return ( m_Type == xSpr::xLT_IMAGE ) ? TRUE : FALSE;	}
	BOOL IsTypeLayerObj( void ) { return ( m_Type == xSpr::xLT_OBJ ) ? TRUE : FALSE;	}
	BOOL IsTypeLayerDummy( void ) { return ( m_Type == xSpr::xLT_DUMMY ) ? TRUE : FALSE;	}
	virtual void FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr ) {}
#ifdef _VER_OPENGL
	virtual void Draw( XSprObj *pSprObj, float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam ) {}
#else
	virtual void Draw( XSprObj *pSprObj, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam ) {}
#endif
	virtual BOOL IsSameLayer( XBaseLayer *pLayer ) {
		if( m_Type == pLayer->GetType() && m_nLayer == pLayer->GetnLayer() )		// 레이어타입과 레이어번호가 같은가
			return TRUE;
		return FALSE;
	}
	virtual DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) { return 0; }
	virtual int Serialize( XArchive& ar, XSprObj *pSprObj ) { return 1;}
	virtual int DeSerialize( XArchive& ar, XSprObj *pSprObj ) { return 1; }
		
}; // class XBaseLayer

///////////////////////////////////////////////////////////////////////////////
class XKeyPos;
struct CHANNEL_POS {
	XE::VEC2 vPos;			// 중심점으로부터의 x,y좌표
//	SPR::xtLINE typePath;	// 패스의 라인종류(라인/스플라인...)
	XBaseLayer::xtInterpolation interpolation = XBaseLayer::xNONE;	// 패스의 보간방법
	XKeyPos *pStartKey = nullptr;		// 패스의 시작키
	XKeyPos *pEndKey = nullptr;		// 패스의 끝키
	XE::VEC2 m_vStart;
	XE::VEC2 m_vEnd;
	float m_frameStartKey = 0.f;
	float m_frameEndKey = 0.f;
	void Clear() {
		vPos.Set( 0 );
		interpolation = XBaseLayer::xNONE;
		pStartKey = pEndKey = nullptr;
		m_vStart = m_vEnd = XE::VEC2(0);
		m_frameStartKey = 0.f;
		m_frameEndKey = 0.f;
	}
	void FrameMove( float dt, float frmCurr );
};

struct CHANNEL_ROT {
	float fAngle = 0.f;					// 현재각도-일단은 z축회전만 지원하자
	XBaseLayer::xtInterpolation interpolation = XBaseLayer::xNONE;	
	float fAngleDest = 0.f;				// 도착각도(누적)
	float fAngleSrc = 0.f;				// 시작각도(누적버전)
	float fNextKeyFrame = 0.f;	
	float fStartKeyFrame = 0.f;	
	void Clear() {
		interpolation = XBaseLayer::xNONE;
		fAngle = fAngleDest = fAngleSrc = 0.f;
		fNextKeyFrame = fStartKeyFrame = 0.f;
	}
	void FrameMove( float dt, float frmCurr );
};

struct CHANNEL_SCALE {
// 	float fScaleX, fScaleY;					// 현재 스케일
	XE::VEC2 m_vScale;
	XBaseLayer::xtInterpolation interpolation = XBaseLayer::xNONE;	
// 	float fScaleDestX, fScaleDestY;				// 목표 스케일
// 	float fScaleSrcX, fScaleSrcY;				// 시작 스케일
	XE::VEC2 m_vScaleSrc;
	XE::VEC2 m_vScaleDest;
	float fNextKeyFrame = 0.f;	
	float fStartKeyFrame = 0.f;	
	CHANNEL_SCALE() 
		: m_vScale(XE::VEC2(1.f)), m_vScaleSrc(XE::VEC2(1.f)), m_vScaleDest(XE::VEC2(1.f)) { }
	void Clear() {
		m_vScale = m_vScaleSrc = m_vScaleDest = XE::VEC2(1.f);
		fNextKeyFrame = fStartKeyFrame = 0.f;
	}
	void FrameMove( float dt, float frmCurr );
};

struct CHANNEL_EFFECT {
	float fAlpha = 1.f;					// 현재 알파값
	DWORD dwDrawFlag = 0;				// EFF_
	xDM_TYPE DrawMode = xDM_NORMAL;				// draw mode
	XBaseLayer::xtInterpolation interpolation = XBaseLayer::xNONE;	// 다음키까지 보간을 한다.
	float fAlphaDest = 1.f;				// 보간하며 변환될 목표알파값
	float fAlphaSrc = 1.f;				//     "               시작알파값
	float fNextKeyFrame = 0.f;	// 보간하며 변환될 다음키의 프레임
	float fStartKeyFrame = 0.f;	//         "            시작키의 프레임
	BOOL IsFlipHoriz() { return dwDrawFlag & EFF_FLIP_HORIZ; }
	BOOL IsFlipVert() { return dwDrawFlag & EFF_FLIP_VERT; }
	void Clear() {
		fAlpha = 1.f;
		dwDrawFlag = 0;
		DrawMode = xDM_NORMAL;
		interpolation = XBaseLayer::xNONE;
		fAlphaDest = fAlphaSrc = 1.f;
		fNextKeyFrame = fStartKeyFrame = 0.f;
	}
	void FrameMove( float dt, float frmCurr );
};


///////////////////////////////////////////////////////////////////////////////
class XLayerMove : public XBaseLayer
{
//	float m_fAdjustAxisX, m_fAdjustAxisY;		// 회전축 보정
	XE::VEC2 m_vAdjAxis;		// 회전축 보정
	CHANNEL_POS	m_cnPos;
	CHANNEL_ROT	m_cnRot;		
	CHANNEL_EFFECT m_cnEffect;
	CHANNEL_SCALE m_cnScale;
	void Init( void ) {
// 		memset( &m_cnPos, 0, sizeof(CHANNEL_POS) );
// 		memset( &m_cnEffect, 0, sizeof(CHANNEL_EFFECT) );
// 		memset( &m_cnRot, 0, sizeof(CHANNEL_ROT) );
// 		memset( &m_cnScale, 0, sizeof(CHANNEL_SCALE) );
// 		m_cnScale.fScaleX = m_cnScale.fScaleY = 1.0f;
// 		m_cnEffect.fAlpha = 1.0f;
// 		m_cnEffect.DrawMode = xDM_NORMAL;
// 		m_fAdjustAxisX = m_fAdjustAxisY = 0;
	}
public:
	XLayerMove() { Init(); }
	XLayerMove( xSpr::xtLayer type ) : XBaseLayer( type ) { Init(); }
	virtual ~XLayerMove() {}
	
//	GET_SET_ACCESSOR( float, x );
//	GET_SET_ACCESSOR( float, y );
//	void Setxy( float x, float y ) { m_x = x; m_y = y; }
// 	float Getx( void ) { return m_cnPos.vPos.x; }
// 	float Gety( void ) { return m_cnPos.vPos.y; }
	const XE::VEC2& GetPos() const { return m_cnPos.vPos; }
	void Setxy( float x, float y ) { m_cnPos.vPos.Set( x, y ); }
	void SetAngleZ( float az ) { m_cnRot.fAngle = az; }
//	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vAdjAxis );
	float GetScaleX() const { return m_cnScale.m_vScale.x; }
	float GetScaleY() const { return m_cnScale.m_vScale.y; }
	void SetScaleX( float sx ) { m_cnScale.m_vScale.x = sx; }
	void SetScaleY( float sy ) { m_cnScale.m_vScale.y = sy; }
	void SetScale( float sx, float sy ) { SetScaleX(sx); SetScaleY(sy); }
// 	GET_ACCESSOR_CONST( float, fAdjustAxisX );
// 	GET_ACCESSOR_CONST( float, fAdjustAxisY );
	GET_ACCESSOR( CHANNEL_POS&, cnPos );
	GET_ACCESSOR( CHANNEL_EFFECT&, cnEffect );
	GET_ACCESSOR( CHANNEL_ROT&, cnRot );
	GET_ACCESSOR( CHANNEL_SCALE&, cnScale );
// 	void SetcnEffect( xtInterpolation inter, float dest, float src, DWORD dwDrawFlag, xDM_TYPE drawMode, float fStartKeyFrame, float fNextKeyFrame ) { 
// 		m_cnEffect.interpolation = inter;
// 		m_cnEffect.fAlphaDest = dest;
// 		m_cnEffect.fAlphaSrc = src;
// 		m_cnEffect.dwDrawFlag = dwDrawFlag;
// 		m_cnEffect.DrawMode = drawMode;
// 		m_cnEffect.fNextKeyFrame = fNextKeyFrame;
// 		m_cnEffect.fStartKeyFrame = fStartKeyFrame;
// 	}
// 	void SetcnPos( XKeyPos *pStartKey, XKeyPos *pEndKey, SPR::xtLINE pathType, xtInterpolation inter ) { 
// 		m_cnPos.interpolation = inter;
// 		m_cnPos.pStartKey = pStartKey;
// 		m_cnPos.pEndKey = pEndKey;
// 		m_cnPos.typePath = pathType;
// 	}
// 	void SetcnRot( xtInterpolation inter, float dest, float src, float fStartKeyFrame, float fNextKeyFrame ) {
// 		m_cnRot.interpolation = inter;
// 		m_cnRot.fAngleDest = dest;
// 		m_cnRot.fAngleSrc = src;
// 		m_cnRot.fNextKeyFrame = fNextKeyFrame;
// 		m_cnRot.fStartKeyFrame = fStartKeyFrame;
// 	}
// 	void SetcnScale( xtInterpolation inter, float osx, float osy, float ssx, float ssy, float fStartKeyFrame, float fNextKeyFrame ) {
// 		m_cnScale.interpolation = inter;
// 		m_cnScale.fScaleDestX = osx;		m_cnScale.fScaleDestY = osy;
// 		m_cnScale.fScaleSrcX = ssx;		m_cnScale.fScaleSrcY = ssy;
// 		m_cnScale.fNextKeyFrame = fNextKeyFrame;
// 		m_cnScale.fStartKeyFrame = fStartKeyFrame;
// 	}
	
	void SetFlipHoriz( BOOL bFlag ) { ( bFlag ) ? m_cnEffect.dwDrawFlag |= EFF_FLIP_HORIZ : m_cnEffect.dwDrawFlag &= ~EFF_FLIP_HORIZ; }
	void SetFlipVert( BOOL bFlag ) { ( bFlag ) ? m_cnEffect.dwDrawFlag |= EFF_FLIP_VERT : m_cnEffect.dwDrawFlag &= ~EFF_FLIP_VERT; }
	void Transform( XE::VEC2 *pvOutPos ) { Transform( &pvOutPos->x, &pvOutPos->y ); }
	void Transform( float *pOutLx, float *pOutLy ) {
		MATRIX m, mRotX, mRotY, mRotZ, mScale;
		MatrixRotationX( mRotX, 0 );
		MatrixRotationY( mRotY, ( GetcnEffect().IsFlipHoriz() )? D2R(180.f) : 0 );
		MatrixRotationZ( mRotZ, D2R(m_cnRot.fAngle) );
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
	static float sCalcInterpolation( float fTimeLerp, XBaseLayer::xtInterpolation inter );

	// virtual
	virtual void Clear( void ) { 
		XBaseLayer::Clear();
		// 키에 의해서 변하는것들만 이곳에 넣는다. m_fAdjustX같은건 키에의해서 변하지 않으므로 넣지 않는다
		m_cnPos.Clear();
		m_cnRot.Clear();
		m_cnScale.Clear();
		m_cnEffect.Clear();
// 		memset( &m_cnPos, 0, sizeof(CHANNEL_POS) );
// 		memset( &m_cnEffect, 0, sizeof(CHANNEL_EFFECT) );
// 		memset( &m_cnRot, 0, sizeof(CHANNEL_ROT) );
// 		memset( &m_cnScale, 0, sizeof(CHANNEL_SCALE) );
// 		m_cnScale.fScaleX = m_cnScale.fScaleY = 1.0f;
// 		m_cnEffect.fAlpha = 1.0f;
// 		m_cnEffect.DrawMode = xDM_NORMAL;
	}
	virtual void FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr );
#ifdef _VER_OPENGL
	virtual void Draw( XSprObj *pSprObj, float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam ) {}
#else
	virtual void Draw( XSprObj *pSprObj, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam ) {}
#endif
	virtual DWORD GetPixel( float lx, float ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  ) { return 0; }
}; // class XLayerMove

///////////////////////////////////////////////////////////////////////////////
class XLayerImage : public XLayerMove
{
	XSprite *m_pSpriteCurr;
	void Init( void ) {
		m_pSpriteCurr = NULL;
	}
public:
	XLayerImage() : XLayerMove( xSpr::xLT_IMAGE ) { 
		Init(); 
	}
	virtual ~XLayerImage() {}
	GET_SET_ACCESSOR( XSprite*, pSpriteCurr );
	void SetDrawInfoToSpr( XSprObj *pSprObj, XSprite *pSpr, XEFFECT_PARAM *pEffectParam );
	virtual void Clear( void ) { XLayerMove::Clear(); Init(); }
//#ifdef _VER_OPENGL
	virtual void Draw( XSprObj *pSprObj, float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam );
//#else
//	virtual void Draw( XSprObj *pSprObj, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam );
//#endif
	virtual DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb );
	virtual int Serialize( XArchive& ar, XSprObj *pSprObj );
	virtual int DeSerialize( XArchive& ar, XSprObj *pSprObj );
};
///////////////////////////////////////////////////////////////////////////////
class XLayerObject : public XLayerMove
{
	XSprObj *m_pSprObjCurr;
	void Init( void ) {
		m_pSprObjCurr = NULL;
	}
public:
	XLayerObject() : XLayerMove( xSpr::xLT_OBJ ) { 
		Init(); 
	}
	virtual ~XLayerObject() { }
	virtual void Clear( void ) { XLayerMove::Clear(); Init(); }
	virtual void FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr );
#ifdef _VER_OPENGL
	virtual void Draw( XSprObj *pSprObj, float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam );
#else
	virtual void Draw( XSprObj *pSprObj, float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam );
#endif
//	virtual DWORD GetPixel( float lx, float ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  );
	GET_SET_ACCESSOR( XSprObj*, pSprObjCurr );
};
////////////////////////////////////////////////////
// 다용도 더미 레이어
class XLayerDummy : public XLayerMove
{
	BOOL m_bActive;
	ID m_id;		// 다용도 아이디
	void Init() {	
		m_bActive = FALSE;
		m_id = 0;
	}
	void Destroy() {}
public:
	XLayerDummy() : XLayerMove( xSpr::xLT_DUMMY ) { Init(); }
	virtual ~XLayerDummy() { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( BOOL, bActive );
	GET_SET_ACCESSOR( ID, id );
	//
	virtual void FrameMove( XSprObj *pSprObj, float dt, float fFrmCurr );
};
///////////////////////////////////////////////////////////////////////////////
class XLayerSound : public XBaseLayer
{
	void Init( void ) {
	}
	void Destroy( void ) {}
public:
	XLayerSound() : XBaseLayer( xSpr::xLT_SOUND ) { 
		Init(); 
	}
	virtual ~XLayerSound() {}
};

///////////////////////////////////////////////////////////////////
inline float xiLinearLerp( float timeLerp ) { return timeLerp; }
float xiSmoothStep( float timeLerp, float startLerp, float endLerp );
float xiHigherPowerDeAccel( float timeLerp, float A, float B );
float xiHigherPowerAccel( float timeLerp, float A, float B );
float xiCatmullrom( float t, float p0, float p1, float p2, float p3 );

