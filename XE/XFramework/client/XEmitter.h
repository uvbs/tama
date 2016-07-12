#pragma once
/********************************************************************
	@date:	2015/06/06 20:44
	@file: 	C:\xuzhu_work\Project\iPhone_may\XE\XFramework\client\XEmitter.h
	@author:	xuzhu
	
	@brief:	파티클 Emitter
*********************************************************************/
#include "XParticle.h"
#include "XPropParticle.h"

//class XBaseParticleMng;
class XParticleMng;
/****************************************************************
* @brief 파티클 방출 객체
* @author xuzhu
* @date	2015/06/06 20:44
* 
*****************************************************************/
class XEmitter
{
	_tstring m_strIdentifier;
	ID m_idKey = 0;
	XPropParticle::xEmitter *_m_pProp = nullptr;
	XE::VEC3 m_vPos;	// 방출 위치(부모에 상대적,위치함수로 변경가능)
	XE::VEC3 m_vDelta;	// 방출방향과 힘 벡터.
	CTimer m_timerCycle;	// 방출 주기
//	int m_numEmit = 0;		// 한번에 뿜어내는 입자 개수.
	XSprObj *m_psoSurfaces = nullptr;	// 파티클텍스쳐의 모음(sprObj사용하지 않아도 되게 바껴야 함)
	XSurface *m_psfcParticle = nullptr;
	_tstring m_strSpr;
	int m_idxSpr = 0;
	XE::VEC3 m_vGravity;
	CTimer m_timerLife;		// 이미터 존재시간
	bool m_bDestroy = false;
	std::vector<xParticle::XCompFunc*> m_aryComponents;
	void Init() {
		m_aryComponents.resize( xParticle::xIC_MAX );
	}
	void Destroy();
public:
	XEmitter( const XE::VEC2& vPos, XPropParticle::xEmitter* pPropEmitter, int idxArySpr );
	virtual ~XEmitter() { Destroy(); }
	//
//	GET_SET_ACCESSOR( int, numEmit );
	GET_SET_ACCESSOR_CONST( bool, bDestroy );
	void FrameMove( XParticleMng *pMng, float dt );
	void AddComponent( xParticle::xtIdxComp idxComp, xParticle::XCompFunc *pComp ) {
		m_aryComponents[ idxComp ] = pComp;
	}
	float GetValueByComp( xParticle::xtIdxComp idxComp ) {
		auto pComp = m_aryComponents[ idxComp ];
		if( pComp )
			return pComp->GetValue();
		return 1.f;
	}
}; // class XEmitter
