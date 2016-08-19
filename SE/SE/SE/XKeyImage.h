#pragma once
#include "XKeyBase.h"

class XSprite;
//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XKeyImage : public XBaseKey
{
friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
//friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
//	int m_nSpr = 0;		// m_pSprite의 인덱스 load할때만 씀
	XSprite *m_pSprite = nullptr;
//	xSPLayerImage m_spLayer;		// 이 키가 속해있는 레이어

	void Init() {
		SetSubType( xSpr::xKTS_MAIN );
	}
 	XKeyImage( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
 		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); }
	// 외부 호출 방지용
	XKeyImage( const XKeyImage& src ) {
		*this = src;
	}
public:
	XKeyImage( XSPAction spAction, float fFrame, xSPLayerImage spLayer, int idxSpr );
	virtual ~XKeyImage() {}
//	GET_SET_ACCESSOR( int, nSpr );
// 	XKeyImage( const XKeyImage& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: /*XKeyImage( src ), */XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	GET_SET_ACCESSOR( XSprite*, pSprite );
	int GetnSpr();
	//GET_ACCESSOR( xSPLayerImage, spLayer );
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 ) override;
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
 	virtual BOOL IsUseSprite( XSprite *pSpr ) { return (m_pSprite == pSpr); }
// 	virtual BOOL IsUseSprite( int idxSpr ) { 
// 		return ( m_nSpr == idxSpr );
// 	}
	virtual BOOL EditDialog() { return FALSE; }
	virtual void ValueAssign( XBaseKey *pSrc ) {		
		*this = *(dynamic_cast<XKeyImage *>(pSrc));
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	virtual void DrawToolTip( XSprDat *pSprDat, float x, float y ) override;
	void InitRandom() override {}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
}; // key image
