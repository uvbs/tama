#pragma once
#include "XLayerBase.h"

class XKeyPos;
/*
   !!!!!!!!!!!!!!!! 아래 구조체에 클래스형이나 stl타입같은거 쓰지말것 !!!!!!!!!!!!!!
*/
struct CHANNEL_POS {
	XE::VEC2 m_vPos;			// 현재좌표(중심점으로부터의 x,y좌표)
//	SPR::xtLINE m_typePath;	// 패스의 라인종류(라인/스플라인...)
	xSpr::xtInterpolation m_interpolation;	// 패스의 보간방법
// 	XKeyPos *pStartKey;		// 패스의 시작키
// 	XKeyPos *pEndKey;		// 패스의 끝키
 	XE::VEC2 m_vStart;		// 시작좌표(랜덤요소때문에 키에서 직접 참조하지 않고 사본을 받음)
 	XE::VEC2 m_vEnd;			// 끝좌표
	float m_fStartKeyFrame = 0;
	float m_fEndKeyFrame = 0;
	//
	CHANNEL_POS() { Clear(); }
	const XE::VEC2& GetvPos() const {
		return m_vPos;
	}
	void Clear() {
		m_vPos.Set( 0 );
		m_vStart.Set( 0 );
		m_vEnd.Set( 0 );
//		m_typePath = SPR::xLINE_NONE;
		m_interpolation = xSpr::xNONE;
//		pStartKey = pEndKey = nullptr;
// 		m_fStartKeyFrame = m_fEndKeyFrame = -1.f;
	}
	void FrameMove( float dt, float fFrmCurr, XSprObj* pParentSprObj );
};

struct CHANNEL_ROT {
	float fAngle;					// 현재각도(degree)-일단은 z축회전만 지원하자
	xSpr::xtInterpolation interpolation;	
	float fAngleDest;				// 도착각도(누적)
	float fAngleSrc;				// 시작각도(누적버전)
	float fNextKeyFrame;	
	float fStartKeyFrame;	
	//
	CHANNEL_ROT() { Clear(); }
	float GetdAng() const {
		return fAngle;
	}
	void Clear() {
		fAngle = 0;
		interpolation = xSpr::xNONE;
		fAngleDest = fAngleSrc = 0;
		fNextKeyFrame = fStartKeyFrame = 0;
	}
	void FrameMove( float dt, float fFrmCurr, XSprObj* pParentSprObj );
};

struct CHANNEL_SCALE {
	XE::VEC2 vScale;					// 현재 스케일
	xSpr::xtInterpolation interpolation;
	XE::VEC2 vScaleDest;				// 목표 스케일
	XE::VEC2 vScaleSrc;				// 시작 스케일
	float fNextKeyFrame;
	float fStartKeyFrame;
	//
	CHANNEL_SCALE() { Clear(); }
	const XE::VEC2& GetvScale() const {
		return vScale;
	}
	void Clear() {
		vScale.Set(1.f);
		interpolation = xSpr::xNONE;
		vScaleDest.Set(1.f);
		vScaleSrc.Set(1.f);
		fNextKeyFrame = fStartKeyFrame = 0;
	}
	void FrameMove( float dt, float fFrmCurr, XSprObj* pParentSprObj );
};

struct CHANNEL_EFFECT {
	float fAlpha = 1.f;					// 현재 알파값
	DWORD dwDrawFlag = 0;				// EFF_
	xDM_TYPE DrawMode = xDM_NORMAL;				// draw mode
	xSpr::xtInterpolation interpolation = xSpr::xNONE;	// 다음키까지 보간을 한다.
	float fAlphaDest = 1.f;				// 보간하며 변환될 목표알파값
	float fAlphaSrc = 1.f;				//     "               시작알파값
	float fNextKeyFrame = 0;	// 보간하며 변환될 다음키의 프레임
	float fStartKeyFrame = 0;	//         "            시작키의 프레임
	//
	CHANNEL_EFFECT() { Clear(); }
	BOOL IsFlipHoriz() const { return dwDrawFlag & EFF_FLIP_HORIZ; }
	BOOL IsFlipVert() const { return dwDrawFlag & EFF_FLIP_VERT; }
	void Clear() {
		fAlpha = 1.f;
		dwDrawFlag = 0;
		DrawMode = xDM_NORMAL;
		interpolation = xSpr::xNONE;
		fAlphaDest = fAlphaSrc = 1.f;
		fNextKeyFrame = fStartKeyFrame = 0;
	}
	void FrameMove( float dt, float fFrmCurr, XSprObj* pParentSprObj );
};


//////////////////////////////////////////////////////////////////////////
class XLayerMove : public XBaseLayer
{
public:
	static bool sCalcInterpolation( float *pfOut, float fTimeLerp, xSpr::xtInterpolation inter );
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_MOVE;
	}
private:
	//	float m_fAdjustAxisX, m_fAdjustAxisY;		// 회전축 보정
	CHANNEL_POS	m_cnPos;
	CHANNEL_ROT	m_cnRot;
	CHANNEL_EFFECT m_cnEffect;
	CHANNEL_SCALE m_cnScale;
	D3DXMATRIX m_mWorld;
	void Init() {
// 		memset( &m_cnPos, 0, sizeof( CHANNEL_POS ) );
// 		memset( &m_cnEffect, 0, sizeof( CHANNEL_EFFECT ) );
// 		memset( &m_cnRot, 0, sizeof( CHANNEL_ROT ) );
// 		memset( &m_cnScale, 0, sizeof( CHANNEL_SCALE ) );
// 		m_cnScale.fScaleX = m_cnScale.fScaleY = 1.0f;
// 		m_cnEffect.fAlpha = 1.0f;
// 		m_cnEffect.DrawMode = xDM_NORMAL;
		//		m_fAdjustAxisX = m_fAdjustAxisY = 0;
	}
protected:
	XE::VEC2 m_vAdjustAxis;
public:
	// 	void Assign( SPLayerMove spDstLayer ) {
	// 		Assign( spDstLayer );
	// 		spDstLayer->m_fAdjustAxisX = m_fAdjustAxisX;
	// 		spDstLayer->m_fAdjustAxisY = m_fAdjustAxisY;
	// 		spDstLayer->m_cnPos = m_cnPos;
	// 		spDstLayer->m_cnRot = m_cnRot;
	// 		spDstLayer->m_cnScale = m_cnScale;
	// 		spDstLayer->m_cnEffect = m_cnEffect;
	// 	}
	XLayerMove() { Init(); }
	XLayerMove( xSpr::xtLayer type, LPCTSTR szLabel, BOOL bAbleOpen ) : XBaseLayer( type, szLabel, bAbleOpen ) { Init(); }
	virtual ~XLayerMove() {}

	const D3DXMATRIX& GetMatrix() const {
		return m_mWorld;
	}
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vAdjustAxis );
	inline void SetAdjustAxis( const XE::VEC2& vAdj ) {
		m_vAdjustAxis = vAdj;
	}
	inline void SetAdjustAxis( float adjx, float adjy ) {
		SetAdjustAxis( XE::VEC2( adjx, adjy ) );
	}
	inline void AddAdjustAxis( const XE::VEC2& vAdd ) {
		m_vAdjustAxis += vAdd;
	}
	inline void AddAdjustAxis( float dx, float dy ) {
		AddAdjustAxis( XE::VEC2( dx, dy ) );
	}
	float GetfAdjustAxisX() const {
		return m_vAdjustAxis.x;
	}
	float GetfAdjustAxisY() const {
		return m_vAdjustAxis.y;
	}
	// 	GET_ACCESSOR( float, fAdjustAxisX );
	// 	GET_ACCESSOR( float, fAdjustAxisY );
	// 	XE::VEC2 GetAdjustAxis() { 
	// 		return XE::VEC2( m_fAdjustAxisX, m_fAdjustAxisY ); 
	// 	}
	float Getx() const { return m_cnPos.m_vPos.x; }
	float Gety() const { return m_cnPos.m_vPos.y; }
	const XE::VEC2& GetPos() const {
		return m_cnPos.m_vPos;
	}
	void Setxy( float x, float y ) {
		m_cnPos.m_vPos.Set( x, y );
	}
	void SetPos( const XE::VEC2& vPos ) {
		m_cnPos.m_vPos = vPos;
	}
	void SetPos( float x, float y ) {
		m_cnPos.m_vPos.Set( x, y );
	}
	void SetAngleZ( float az ) { m_cnRot.fAngle = az; }
	float GetScaleX() const { return m_cnScale.vScale.x; }
	float GetScaleY() const { return m_cnScale.vScale.y; }
	inline XE::VEC2 GetvScale() const {
		return m_cnScale.GetvScale();
	}
	void SetScaleX( float sx ) { m_cnScale.vScale.x = sx; }
	void SetScaleY( float sy ) { m_cnScale.vScale.y = sy; }
	void SetScale( float sx, float sy ) { 
		m_cnScale.vScale.Set( sx, sy );
	}
	inline void SetScale( const XE::VEC2& vScale ) { 
		m_cnScale.vScale = vScale; 
	}
	GET_SET_ACCESSOR_CONST( const CHANNEL_POS&, cnPos );
	GET_SET_ACCESSOR_CONST( const CHANNEL_EFFECT&, cnEffect );
	GET_SET_ACCESSOR_CONST( const CHANNEL_ROT&, cnRot );
	GET_SET_ACCESSOR_CONST( const CHANNEL_SCALE&, cnScale );
	const XE::VEC2& GetvPosBycnPos() const {
		return m_cnPos.m_vPos;
	}
	const float GetdRotBycnRot() const {
		return m_cnRot.fAngle;
	}
	const XE::VEC2& GetvScaleBycnScale() const {
		return m_cnScale.vScale;
	}
	CHANNEL_POS& GetcnPosMutable() {
		return m_cnPos;
	}
	CHANNEL_ROT& GetcnRotMutable() {
		return m_cnRot;
	}
	CHANNEL_SCALE& GetcnScaleMutable() {
		return m_cnScale;
	}
	CHANNEL_EFFECT& GetcnEffectMutable() {
		return m_cnEffect;
	}
	void SetcnEffect( xSpr::xtInterpolation inter, float dest, float src, DWORD dwDrawFlag, xDM_TYPE drawMode, float fStartKeyFrame, float fNextKeyFrame );
// 	void SetcnPos( XKeyPos *pStartKey, XKeyPos *pEndKey, SPR::xtLINE pathType, xSpr::xtInterpolation inter );
// 	void SetcnRot( xSpr::xtInterpolation inter, float dest, float src, float fStartKeyFrame, float fNextKeyFrame );
// 	void SetcnScale( xSpr::xtInterpolation inter, float osx, float osy, float ssx, float ssy, float fStartKeyFrame, float fNextKeyFrame );
// 	inline void SetcnPos( const CHANNEL_POS& cn ) {
// 		m_cnPos = cn;
// 	}
// 	inline void SetcnRot( const CHANNEL_ROT& cn ) {
// 		m_cnRot = cn;
// 	}
// 	inline void SetcnScale( const CHANNEL_SCALE& cn ) {
// 		m_cnScale = cn;
// 	}
	void SetFlipHoriz( BOOL bFlag ) { ( bFlag ) ? m_cnEffect.dwDrawFlag |= EFF_FLIP_HORIZ : m_cnEffect.dwDrawFlag &= ~EFF_FLIP_HORIZ; }
	void SetFlipVert( BOOL bFlag ) { ( bFlag ) ? m_cnEffect.dwDrawFlag |= EFF_FLIP_VERT : m_cnEffect.dwDrawFlag &= ~EFF_FLIP_VERT; }
	void Transform( XE::VEC2 *pvOutPos ) { Transform( &pvOutPos->x, &pvOutPos->y ); }
	void Transform( float *pOutLx, float *pOutLy );
	virtual void Clear();
	virtual void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj );
	virtual void MoveFrame( float fFrmCurr ) {}
	virtual void Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj ) {};
	//	virtual SPBaseLayer CopyDeep() = 0;
	virtual CString GetLabel();
	virtual XBaseKey* DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType ) override;
	virtual void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj ) {}
private:
	bool IsExistSamePosKey( XBaseKey *pExist ) const;
	XSPLayerMove GetThis() {
		return std::static_pointer_cast<XLayerMove>( XBaseLayer::GetThis() );
	}
};


