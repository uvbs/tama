#pragma once
#include "XKeyBase.h"

//////////////////////////////////////////////////////////////////////////
// 이 클래스도 일반화 시킬수 있을듯
/**
 @brief 
*/
class XKeyDummy : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	static xSpr::xtKey sGetType() {
		return xSpr::xKT_DUMMY;
	}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_MAIN;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_DUMMY;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_MAIN;
	}
//	static const xSpr::xtKeySub s_subType =xSpr::xKTS_MAIN;
	// 외부호출 방지용
	XKeyDummy( const XKeyDummy& src ) {
		*this = src;
	}
private:
	BOOL m_bActive;
	ID m_id;		// 다용도 아이디
//	XSPLayerDummy m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType( xSpr::xKTS_MAIN );
		m_bActive = TRUE;
		m_id = 0;
	}
protected:
	XKeyDummy() : XBaseKey() { Init(); }
 	XKeyDummy( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); 	}
public:
	XKeyDummy( XSPAction spAction, float fFrame, XSPLayerDummy spLayer, BOOL bActive );
	virtual ~XKeyDummy() { }
// 	XKeyDummy( const XKeyDummy& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: /*XKeyDummy( src ),*/ XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;

	GET_SET_ACCESSOR( BOOL, bActive );
	GET_SET_ACCESSOR( ID, id );
	//GET_ACCESSOR( xSPLayerDummy, spLayer );
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 ) override;
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual BOOL EditDialog(); // { m_bActive = !m_bActive; return TRUE; }
	virtual void ValueAssign( XBaseKey *pSrc ) {		
		*this = *(dynamic_cast<XKeyDummy *>(pSrc));
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	void InitRandom() override {}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // KeyDummy
