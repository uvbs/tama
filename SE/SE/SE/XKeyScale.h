#pragma once
#include "XKeyBase.h"

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XKeyScale : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	//static xSpr::xtKey sGetType() {
	//	return xSpr::xKT_MOVE;
	//}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_SCALE;
	}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_SCALE;
	}
//	static const xSpr::xtKeySub s_subType =xSpr::xKTS_SCALE;
	// 외부호출 방지용
	XKeyScale( const XKeyScale& src ) {
		*this = src;
	}
private:
	XE::VEC2 m_vScaleOrig;		// 기준값
	XE::VEC2 m_vScaleRandomed;		// 랜덤적용된 값
	XE::VEC2 m_vRangeXY, m_vRangeX, m_vRangeY;
	xSpr::xtInterpolation m_Interpolation;
	void Init() {
		SetSubType(xSpr::xKTS_SCALE );
		m_vScaleOrig.Set( 1.f );
		m_vScaleRandomed.Init();
		m_Interpolation = xSpr::xLINEAR;
	}
protected:
 	XKeyScale() : XBaseKey() { Init(); }
public:
	XKeyScale( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); }
	XKeyScale( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float sx, float sy );
	virtual ~XKeyScale() {}
	XBaseKey* CreateCopy() override;
	// get/set
// 	GET_SET_ACCESSOR( float, fScaleX );
// 	GET_SET_ACCESSOR( float, fScaleY );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScaleOrig );
//	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScaleRandomed );
	XE::VEC2 GetvScaleRandomed();
	GET_SET_ACCESSOR_CONST( xSpr::xtInterpolation, Interpolation );
	//GET_ACCESSOR( xSPLayerMove, spLayer );
	void AddScale( float sx, float sy ) { 
		m_vScaleOrig += XE::VEC2(sx, sy); 
	}
	void MulScale( float xMul, float yMul ) { 
		m_vScaleOrig *= XE::VEC2(xMul, yMul);
	}		// 인수는 1.0=100%인 배수가 들어온다
	void Set( xSpr::xtInterpolation interpolation ) { 
		m_Interpolation = interpolation; 
	}
	XE::VEC2 XKeyScale::AssignRandom() const;
	// virtual 
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 ) override;
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual BOOL EditDialog();
	virtual void ValueAssign( XBaseKey *pSrc ) {		
		XKeyScale *pKey = dynamic_cast<XKeyScale *>(pSrc);
		XBREAK( pKey == nullptr );
		*this = *pKey;
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	void InitRandom() override {
		m_vScaleRandomed.Init();		// 랜덤적용되는 변수는 초기화를 시킴
	}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // KeyScale
