#pragma once
#include "XKeyBase.h"

////////////////////////////////////////////////////////////////////////////////////////////
class XKeySound : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	//static xSpr::xtKey sGetType() {
	//	return xSpr::xKT_MOVE;
	//}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_POS;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_MOVE;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_POS;
	}
	// 외부호출 방지용
	XKeySound( const XKeySound& src ) {
		*this = src;
	}
private:
	ID m_idSound;
	float m_fVolume;
//	XSPLayerSound m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType( xSpr::xKTS_MAIN );
		m_idSound = 0;
		m_fVolume = 0;
	}
	void Destroy() {}
protected:
	XKeySound() : XBaseKey() { Init(); }
	XKeySound( XSPAction spAction, ID idLayer, ID idLocalInLayer )
		: XBaseKey( spAction, idLayer, idLocalInLayer ) {
		Init();
	}
public:
	XKeySound( XSPAction spAction, float fFrame, XSPLayerSound spLayer, ID idSound );
	virtual ~XKeySound() { Destroy(); }
// 	XKeySound( const XKeySound& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec = 0 ) override;
	GET_ACCESSOR( ID, idSound );
	GET_ACCESSOR( float, fVolume );
	//	//GET_ACCESSOR( xSPLayerSound, spLayer );
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual void ValueAssign( XBaseKey *pSrc ) {
		*this = *( (XKeySound *)pSrc );
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	virtual BOOL EditDialog();
	void InitRandom() override {}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
};
