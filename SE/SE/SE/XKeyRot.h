#pragma once
#include "XKeyBase.h"

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XKeyRot : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	//static xSpr::xtKey sGetType() {
	//	return xSpr::xKT_MOVE;
	//}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_ROT;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_MOVE;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_ROT;
	}
// 	static const xSpr::xtKeySub s_subType =xSpr::xKTS_ROT;
	// 외부호출 방지용
	XKeyRot( const XKeyRot& src ) {
		*this = src;
	}
private:
	float m_fAngleZ;					//  누적 절대각도. +720, -720도 모두 될수 있다.
	XE::VEC2 m_vRange;				// 랜덤 범위
	xSpr::xtInterpolation m_Interpolation;		// 다음키까지 보간
	float m_dAngle = 9999.f;			// 랜덤적용된 최종 값
//	XSPLayerMove m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType(xSpr::xKTS_ROT );
		m_fAngleZ = 0;
		m_Interpolation = xSpr::xLINEAR;
	}
protected:
 	XKeyRot() : XBaseKey() { Init(); }
public:
	XKeyRot( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); }
	XKeyRot( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float az );
	virtual ~XKeyRot() {}
// 	XKeyRot( const XKeyRot& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	// get/set
//	GET_SET_ACCESSOR_CONST( float, fAngleZ );
	SET_ACCESSOR( float, fAngleZ );
	float GetdAngleOrig() const {
		return m_fAngleZ;
	}
	float GetdAngle() {
		if( m_dAngle == 9999.f ) 
			m_dAngle = AssignRandom();
		return m_dAngle;
	}
	float AssignRandom() const;
	GET_SET_ACCESSOR_CONST( xSpr::xtInterpolation, Interpolation );
	//GET_ACCESSOR( xSPLayerMove, spLayer );
	void AddAngleZ( float angle ) { 
		m_fAngleZ += angle; 
	}
	void Set( xSpr::xtInterpolation interpolation ) { m_Interpolation = interpolation; }
	// virtual 
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 ) override;
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual BOOL EditDialog();
	virtual void ValueAssign( XBaseKey *pSrc ) {		
		XKeyRot *pKey = dynamic_cast<XKeyRot *>(pSrc);
		XBREAK( pKey == nullptr );
		*this = *pKey;
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	void InitRandom() override {
		m_dAngle = 9999.f;		// 랜덤적용되는 변수는 초기화를 시킴
	}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // KeyRot