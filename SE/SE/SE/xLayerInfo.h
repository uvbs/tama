#pragma once
#include "Sprdef.h"

/**
 @brief 레이어 정적 정보
*/
struct xLayerInfo {
	ID m_idLayer = 0;
	xSpr::xtLayer m_bitType = xSpr::xLT_NONE;		//레이어타입
	int m_nLayer = 0;						// 레이어번호
	float m_fLastFrame = 0;					// 툴에서만 사용
	XE::VEC2 m_vAdjAxis;	// 좌표축 LAYER_MOVE타입일경우 그 레이어의 GetvAdjustAxis()를 저장.
	ID m_idLocalByNewKey = 0;			// 키가 생성되면 이값이 ++되며 레이어내에서 고유한 로컬아이디를 만들어 낸다. 레이어가 카피되더라도 이값은 유지된다.
	xLayerInfo() {}
	xLayerInfo( ID idLayer, xSpr::xtLayer type, int nLayer, float fLastFrame, const XE::VEC2& vAdjAxis ) 
		: m_idLayer(idLayer), m_bitType(type), m_nLayer(nLayer), m_fLastFrame(fLastFrame), m_vAdjAxis(vAdjAxis) {}
// 	xLayerInfo( XSprDat *pSprDat, ID id=0 );
// 	BOOL IsSame( SPBaseLayer spLayer ) {
// 		return ( type == spLayer->GetType() && nLayer == spLayer->GetnLayer() );
// 	}
	ID getid() const {
		return m_idLayer;
	}
	ID GenerateidKeyLocal() {
		return ++m_idLocalByNewKey;
	}
	inline bool IsSame( xSpr::xtLayer bitType, int nLayer ) const {
		return (bitType & m_bitType) && (nLayer == m_nLayer);
	}
	bool Save( XResFile& res ) const;
	bool Load( XResFile& res, int verSprDat );
};



