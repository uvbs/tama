/********************************************************************
	@date:	2015/06/11 15:33
	@file: 	C:\xuzhu_work\Project\iPhone_may\XE\XFramework\client\XPropParticle.h
	@author:	xuzhu
	
	@brief:	파티클 스크립트 로더
*********************************************************************/
#pragma once
#include "XXMLDoc.h"
#include "etc/Debug.h"
#include <map>
#include <memory>
#include <vector>

namespace XE {
enum xtBlendFunc;
}
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/11 15:38
*****************************************************************/
namespace xParticle {
	enum xtFunc {
		xFN_NONE = 0,
		xFN_FIXED,
		xFN_RANDOM,
		xFN_LINEAR,
		xFN_SIN,
		xFN_COS,
	};
	// 컴포넌트 인덱스.
	enum xtIdxComp {
		xIC_NONE = 0,
		xIC_SPEED = 0,
		xIC_ANGLE,
		xIC_DELTA_X,
		xIC_DELTA_Y,
		xIC_SCALE,
		xIC_ALPHA,
		xIC_LIFE,
		xIC_INTERVAL,
		xIC_GRAVITY,
		xIC_NUM_EMIT,
		xIC_MAX,
	};
	inline const char* GetIdsByIdxComp( xtIdxComp idxComp ) {
		switch( idxComp )
		{
		case xParticle::xIC_SPEED:	return "speed";
		case xParticle::xIC_ANGLE:	return "angle";
		case xParticle::xIC_DELTA_X:	return "delta_x";
		case xParticle::xIC_DELTA_Y:	return "delta_y";
		case xParticle::xIC_SCALE:	return "scale";
		case xParticle::xIC_ALPHA:	return "alpha";
		case xParticle::xIC_LIFE:	return "life";
		case xParticle::xIC_INTERVAL:	return "interval";
		case xParticle::xIC_GRAVITY: return "gravity";
		case xParticle::xIC_NUM_EMIT: return "num_emit";
		default:
			XBREAK(1);
			break;
		}
		return "";
	}
	struct xFunc {
		xtFunc funcType = xFN_NONE;
		XE::VEC2 range;
		float secCycle = 1.f;
	};
};
class XPropParticle : public XXMLDoc
{
	static ID s_idGlobal;
	static ID sGenerateID() {
		return ++s_idGlobal;
	}
public:
	struct xEmitter {
		ID idProp = 0;
		XE::VEC3 vwPos;			/// 방출 좌표
		XE::xtBlendFunc blendFunc;
		std::vector<xParticle::xFunc> aryFunc;
		// 입자파라메터
		_tstring strSpr;		/// 사용 spr파일명
//		int idxSpr = -1;			/// spr파일의 스프라이트인덱스
		XVector<int> m_aryIdxSpr;		// 스프라이트 인덱스 리스트.
		XE::VEC3 vwDelta;		/// 이동값
		int numEmit = 1;		/// 1회 방출량
		float secLife = -1;	/// 이미터 존재시간. >0:해당초동안 0:1회 -1:무한대
		// 파티클 개별
		struct Particle {
			std::vector<xParticle::xFunc> aryFunc;
			Particle() : aryFunc( xParticle::xIC_MAX ) {
//				aryFunc.resize( xParticle::xIC_MAX );
				aryFunc[ xParticle::xIC_SPEED ].range.Set( 1.f );
				aryFunc[ xParticle::xIC_ALPHA ].range.Set( 1.f );
				aryFunc[ xParticle::xIC_SCALE ].range.Set( 1.f );
			}
		} particle;
		xEmitter();
	};
	struct xSfx {
		_tstring strIdentifier;
		std::vector<xEmitter*> aryEmitter;
		~xSfx();
	};
public:
	static std::shared_ptr<XPropParticle>& sGet();
private:
	static std::shared_ptr<XPropParticle> s_spInstance;
private:
	std::map<std::string, xSfx*> m_mapSfx;
	std::map<std::string, xEmitter*> m_mapSf;
	bool m_bError = false;
	void Init() {}
	void Destroy();
public:
	XPropParticle();
	virtual ~XPropParticle() { Destroy(); }
	//
	bool Load( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
	void LoadObj( XEXmlNode& node );
	xEmitter* LoadEmitter( XEXmlNode& node );
	XE::xtBlendFunc GetBlendFunc( XEXmlNode& node, const char *cKey );
	void LoadParticle( XEXmlNode& node, xEmitter* pOut );
	int LoadFunc( XEXmlNode& nodeRoot, const char *cKey, xParticle::xFunc *pOut );
	xSfx* GetpProp( const std::string& strIdentifier ) {
		auto itor = m_mapSfx.find( strIdentifier );
		if( itor == m_mapSfx.end() )
			return nullptr;
		xSfx *pProp = itor->second;
		XBREAK( pProp == nullptr );
		return pProp;
	}
	xSfx* GetpProp( const char *cIdentifier ) {
		std::string ids = cIdentifier;
		return GetpProp( ids );
	}
	
	
}; // class XPropParticle

