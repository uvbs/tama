#pragma once
#include "XKeyBase.h"

class XKeyEffect : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	//static xSpr::xtKey sGetType() {
	//	return xSpr::xKT_MOVE;
	//}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_EFFECT;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_MOVE;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_EFFECT;
	}
//	static const xSpr::xtKeySub s_subType = xSpr::xKTS_EFFECT;
	// 외부호출 방지용
	XKeyEffect( const XKeyEffect& src ) {
		*this = src;
	}
private:
	xSpr::xtInterpolation m_Interpolation;	// 다음키까지 알파보간을 할건지 말건지
	DWORD m_dwFlag;			// 상/하, 좌/우 플립같은 토글성 이펙트는 이곳에 들어간다.
	xDM_TYPE m_DrawMode;			// screen, dodge, darken 찍기등 선택형 속성은 이곳에 들어간다.
	float m_fOpacityOrig;				// 알파블랜드, 투명도 0.0 ~ 1.0
	float m_fOpacityRandomed;
	XE::VEC2 m_vRange;
//	XSPLayerMove m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType( xSpr::xKTS_EFFECT );
		m_Interpolation = xSpr::xLINEAR;
		m_dwFlag = 0;
		m_DrawMode = xDM_NONE;
		m_fOpacityOrig = 1.f;
		m_fOpacityRandomed = 9999.f;
	}
protected:
	XKeyEffect() : XBaseKey() { Init(); }
public:
	XKeyEffect( XSPAction spAction, ID idLayer, ID idLocalInLayer )
		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); }
	XKeyEffect( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer );
	virtual ~XKeyEffect() {}

// 	XKeyEffect( const XKeyEffect& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: /*XKeyEffect( src ),*/ XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	// get/set
	BOOL GetFlipHoriz() const { return ( m_dwFlag & EFF_FLIP_HORIZ ) ? TRUE : FALSE; }
	BOOL GetFlipVert() const { return ( m_dwFlag & EFF_FLIP_VERT ) ? TRUE : FALSE; }
	GET_SET_ACCESSOR_CONST( float, fOpacityOrig );
	GET_SET_ACCESSOR_CONST( DWORD, dwFlag );
	GET_SET_ACCESSOR_CONST( xDM_TYPE, DrawMode );
	GET_ACCESSOR_CONST( xSpr::xtInterpolation, Interpolation );
	float GetOpacityRandomed() {
		if( m_fOpacityRandomed == 9999.f )
			m_fOpacityRandomed = AssignRandom();
		return m_fOpacityRandomed;
	}
	float AssignRandom() const {
		return m_fOpacityOrig + xRandomF( m_vRange.v1, m_vRange.v2 );
	}
	//GET_ACCESSOR( xSPLayerMove, spLayer );
	void Set( xSpr::xtInterpolation interpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity ) {
		m_Interpolation = interpolation;
		m_dwFlag = dwDrawFlag;
		m_DrawMode = drawMode;
		m_fOpacityOrig = fOpacity;
	}
	// virtual 
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec = 0 ) override;
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual BOOL EditDialog();
	virtual void ValueAssign( XBaseKey *pSrc ) {
		XKeyEffect *pKey = dynamic_cast<XKeyEffect *>( pSrc );
		XBREAK( pKey == nullptr );
		*this = *pKey;
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	void InitRandom() override {
		m_fOpacityRandomed = 9999.f;		// 랜덤적용되는 변수는 초기화를 시킴
	}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // XKeyEffect