#pragma once

#include "XXMLDoc.h"
#include "XStruct.h"

const int VER_PROP_WORLD = 1;

class CaribeView;
////////////////////////////////////////////////////////////////
class XPropWorld : public XXMLDoc
{
	enum { xST_BUILDING = 0xffffffff };
	static ID s_idGlobal;
	static float s_ratioProduce;		// 생산양 조절용 비율.
	static float s_ratioCastleProduce;	// 성 생산량 조절 비율
	static float s_ratioRegen;		// 리젠시간 조절용 비율.
public:
// 	static float sGetMultiplyProduce() {
// 		return m_ratioproduce;
// 	} 
	struct xBASESPOT {
		_tstring strIdentifier;
		_tstring strWhen;			// 특정퀘스트를 받으면 등장. null이면 구름까질때 등장.
		_tstring strWhenDel;
#ifdef _DEV
		_tstring strCode;
#endif
		ID idWhen = 0;
		ID idWhenDel = 0;
		XGAME::xtSpot type;
		ID idSpot = 0;					// 스팟 아이디
		XE::VEC2 vWorld;			// 월드좌표의 좌표
		ID idArea = 0;					// 속해있는 지역
		ID idCode = 0;					///< 스팟코드
		ID idName = 0;			///< 스팟의 이름(필요할때만)
		int level = 0;			// 스팟레벨의 수동지정
		struct bitfield {
			 bool bOpened : 1;		// 첨부터 구름이 오픈되어있는 스팟.
			 // reserved
			 bool b1 : 1;
			 bool b2 : 1;
			 bool b3 : 1;			
		} m_bitAttr;
		_tstring strSpr;		// 스팟이미지로 쓸 spr이 따로 있다면 지정한다. 없으면 디폴트로 npc스팟을 쓴다.
		ID idAct = 1;				// 스팟이미지 액션아이디
		xBASESPOT( XGAME::xtSpot _type ) : type(_type) {
			*((BYTE*)(&m_bitAttr)) = 0;
		}
		inline XGAME::xtSpot GetType() {
			return type;
		}
		bool IsInvalidSpot() const {
			return idArea == 0;
		}
		bool IsOpened() const {
			return m_bitAttr.bOpened != 0;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	// 유저 스팟
	struct xCASTLE : public xBASESPOT {
	private:
#if _DEV_LEVEL <= DLV_DEV_CREW
		std::vector<XGAME::xRES_NUM> m_aryProduceOrig;	// 변조되지 않은 오리지날값(save용)
#endif // 
	public:
		// 이 성에서 생산할수 있는 자원들(로딩시 생산율변동에 의해 변조될수 있다)
		std::vector<XGAME::xRES_NUM> m_aryProduce;		// 분당생산량
//		XGAME::xtDropCastle typeDrop = XGAME::xDC_NONE;
		xCASTLE() : xBASESPOT( XGAME::xSPOT_CASTLE ) {}
		void SetAryProduce( std::vector<XGAME::xRES_NUM>& arySrc ) {
			m_aryProduce = arySrc;
#if _DEV_LEVEL <= DLV_DEV_CREW
			m_aryProduceOrig = arySrc;
#endif // WIN32
			for( auto& res : m_aryProduce ) {
				res.num = res.num * s_ratioCastleProduce;
			}
		}
#if _DEV_LEVEL <= DLV_DEV_CREW
		std::vector<XGAME::xRES_NUM>& _GetAryProduceOrig() {
			return m_aryProduceOrig;
		}
#endif // WIN32
		XGAME::xRES_NUM* GetProduce( XGAME::xtResource type ) {
			for( auto& res : m_aryProduce ) {
				if( res.type == type )
					return &res;
			}
			return nullptr;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class CaribeView;
	};
	// 보석광산 스팟
	struct xJEWEL : public xBASESPOT {
		int idx;
	private:
		float produce;				// 생산량(분)
	public:
		xJEWEL() : xBASESPOT( XGAME::xSPOT_JEWEL ){
			idx = 0;
			produce = 0;
		}
		float GetProduce() {
			return produce * s_ratioProduce;
		}
		void SetProduce( float _produce )  {
			produce = _produce;
		}
		float _GetProduce() {
			return produce;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class CaribeView;
	};
	struct xSULFUR : public xBASESPOT {
	private:
		float produce;				// 생산량
		float secRegenNpc;			// npc리스폰 시간
	public:
		xSULFUR() : xBASESPOT( XGAME::xSPOT_SULFUR ) {
			produce = 0;
			secRegenNpc = 0;
		}
// 		inline XGAME::xtSpot GetType() {
// 			return XGAME::xSPOT_SULFUR;
// 		}
		float GetProduce() {
			return produce * s_ratioProduce;
		}
		void SetProduce( float _produce )  {
			produce = _produce;
		}
		float _GetProduce() {
			return produce;
		}
		float _GetsecRegen() {
			return secRegenNpc;
		}
		float GetsecRegen() {
			return secRegenNpc * s_ratioRegen;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class CaribeView;
		friend class XPropWorld;
	};
	struct xMANDRAKE : public xBASESPOT {
	private:
		float produce;				// 생산량
	public:
		int idx;
		xMANDRAKE() : xBASESPOT( XGAME::xSPOT_MANDRAKE ) {
			produce = 0;
			idx = 0;
		}
		float GetProduce() {
			return produce * s_ratioProduce;
		}
		void SetProduce( float _produce )  {
			produce = _produce;
		}
		float _GetProduce() {
			return produce;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class CaribeView;
	};
	// NPC스팟
	struct xNPC : public xBASESPOT {
	private:
		float secRegen = 0;
	public:
// 		XGAME::xtUnit unit = XGAME::xUNIT_NONE;		// 특정한 유닛으로 채우고 싶다면.
// 		XGAME::xtGradeLegion gradeLegion = XGAME::xGL_NONE;	///< 정예인가
//		XGAME::xLegion legion;			// 군단생성정보
		std::string m_idsLegion;			// xLegion의 프로퍼티 식별자.
//		XGAME::xtClan clan = XGAME::xCL_NONE;
		xNPC() : xBASESPOT( XGAME::xSPOT_NPC ) {}
		float _GetsecRegen() {
			return secRegen;
		}
		float GetsecRegen() {
			return secRegen * s_ratioRegen;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class CaribeView;
		friend class XPropWorld;
	};
	// Daily스팟
	struct xDaily : public xBASESPOT {
		float dummy = 0;
		float m_v1 = 1.f;
		float m_v2 = 1.f;
		float m_v3 = 1.f;
		XVector<XVector<XGAME::xReward>> m_aryDays;		// 일주일간의 보상
		xDaily() : xBASESPOT( XGAME::xSPOT_DAILY ), m_aryDays(7) {
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	// Special스팟
// 	struct xSpecial : public xBASESPOT {
// 		float dummy;
// 		xSpecial() : xBASESPOT( XGAME::xSPOT_SPECIAL ) {
// 			dummy = 0;
// 		}
// 	};
	// 켐페인스팟
	struct xCampaign : public xBASESPOT {
// 		_tstring strWhen;			// 특정퀘스트를 받으면 등장. null이면 구름까질때 등장.
 		_tstring strCamp;			///< 캠페인 식별자.
		ID idCamp = 0;
		xCampaign() : xBASESPOT( XGAME::xSPOT_CAMPAIGN ) {}
// 		inline XGAME::xtSpot GetType() {
// 			return type;
// 		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	// 방문스팟
	struct xVisit : public xBASESPOT {
		ID idDialog = 0;				///< 방문시 대화(?)텍스트
		std::string idsSeq;				///< 방문시 컷씬
		xVisit() : xBASESPOT( XGAME::xSPOT_VISIT ) {}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	struct xCash : public xBASESPOT {
		float secRegen = 0;		// 리젠시간(초)
		float produceMin = 0;		// 캐쉬 리젠 개수
		float produceMax = 0;		// 캐쉬 리젠 개수
		xCash() : xBASESPOT( XGAME::xSPOT_CASH ) {}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
  struct xGuildRaid : public xBASESPOT {
    int dummy;
    xGuildRaid() : xBASESPOT( XGAME::xSPOT_GUILD_RAID ) {}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
  };
  struct xPrivateRaid : public xBASESPOT {
    int dummy;
    xPrivateRaid() : xBASESPOT( XGAME::xSPOT_PRIVATE_RAID ) {}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
  };
  struct xCommon : public xBASESPOT {
    _tstring strType;
    _tstring strParam;
    int nParam[4];
    xCommon() : xBASESPOT( XGAME::xSPOT_COMMON ) {
      XCLEAR_ARRAY( nParam );
    }
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
  };
private:
	_tstring m_strIdentifier;	// 맵 식별자
	_tstring m_strImg;			// 맵 파일명
	// xml에 있는만큼만 할당하기때문에 널이 들어가있는 일은 없다.
	XList4<xBASESPOT*> _m_listSpots;		// 나중에 맵방식으로 바꿀것.
	std::unordered_map<ID, xBASESPOT*> m_mapSpots;	// 모든 스팟의 맵구조
	std::unordered_map<_tstring, xBASESPOT*> m_mapSpotsIds;
	std::unordered_map<_tstring, XList4<xBASESPOT*>> m_mapSpotsByWhen;		// 키는 퀘스트 식별자. 해당퀘를 얻으면 해당스팟이 생성된다.
	std::unordered_map<_tstring, XList4<xBASESPOT*>> m_mapSpotsByWhenDel;		// 키는 퀘스트 식별자. 해당퀘가 파괴디ㅗ면 해당스팟도 파괴된다.
	std::unordered_map<ID, XList4<xBASESPOT*>> m_mapSpotsByCode;		// 키:스팟코드, 밸류:스팟
	int m_numSpot[ XGAME::xSPOT_MAX ];

	void Init() {
		XCLEAR_ARRAY( m_numSpot );
	}
	void Destroy();
	//
public:
	XPropWorld() {
		Init();
	}
	XPropWorld( LPCTSTR szXml );
	virtual ~XPropWorld() { Destroy(); }
	//
	GET_TSTRING_ACCESSOR( strIdentifier );
	GET_TSTRING_ACCESSOR( strImg );
// 	XArrayLinear<xBASESPOT*>& GetArySpots( void ) {
// 		return m_arySpots;
// 	}
	template<typename T, int N>
	int GetSpotsPropToAry( XArrayLinearN<T,N> *pOut, 
						XGAME::xtSpot typeSpot = XGAME::xSPOT_NONE ) {
		for( auto pProp : _m_listSpots ) {
			if( pProp->type == typeSpot || typeSpot == XGAME::xSPOT_NONE )
				pOut->Add( pProp );
		}
		return pOut->size();
	}
//	XArrayLinear<xNPC*>& GetAryNpcs( void ) {
//		return m_aryNpcs;
//	}

	bool OnDidFinishLoad() override;
	BOOL LoadSpots( XEXmlNode& node );
	xBASESPOT* LoadCastle( XEXmlNode& node, XEXmlNode& childNode );
	xJEWEL* LoadJewel( XEXmlNode& node, XEXmlNode& childNode );
	xSULFUR* LoadSulfur( XEXmlNode& node, XEXmlNode& childNode );
	xMANDRAKE* LoadMandrake( XEXmlNode& node, XEXmlNode& childNode );
	xNPC* LoadNpc( XEXmlNode& node, XEXmlNode& childNode );
	xDaily* LoadDaily( XEXmlNode& node, XEXmlNode& childNode );

	xBASESPOT* CreateSpot( XGAME::xtSpot type );
//	void UpdateSpot( xBASESPOT *pBaseSpot );
	//
	xCASTLE* GetpPropCastle( ID idProp ) {
		return GetpProp<xCASTLE>( XGAME::xSPOT_CASTLE, idProp );
	}
	xJEWEL* GetpPropJewel( ID idProp ) {
		return GetpProp<xJEWEL>( XGAME::xSPOT_JEWEL, idProp );
	}
	xJEWEL* GetpPropJewel( LPCTSTR szIdentifier );
	xJEWEL* GetpPropJewelByIdx( int idx );
	xSULFUR* GetpPropSulfur( ID idProp ) {
		return GetpProp<xSULFUR>( XGAME::xSPOT_SULFUR, idProp );
	}
	xMANDRAKE* GetpPropMandrake( ID idProp ) {
		return GetpProp<xMANDRAKE>( XGAME::xSPOT_MANDRAKE, idProp );
	}
	xMANDRAKE* GetpPropMandrakeByIdx( int idx );
	xNPC* GetpPropNpc( ID idProp ) {
		return GetpProp<xNPC>( XGAME::xSPOT_NPC, idProp );
	}
	xNPC* GetpPropNpc( LPCTSTR szIdentifier );
	xDaily* GetpPropDaily( ID idProp ) {
		return GetpProp<xDaily>( XGAME::xSPOT_DAILY, idProp );
	}
// 	xSpecial* GetpPropSpecial( ID idProp ) {
// 		return GetpProp<xSpecial>( XGAME::xSPOT_SPECIAL, idProp );
// 	}
	xCampaign* GetpPropCampaign( ID idProp ) {
		return GetpProp<xCampaign>( XGAME::xSPOT_CAMPAIGN, idProp );
	}
	xVisit* GetpPropVisit( ID idProp ) {
		return GetpProp<xVisit>( XGAME::xSPOT_VISIT, idProp );
	}
	/**
	 유황 스팟을 모두 읽어 주어진 어레이에 담아준다.
	*/
	template<int N>
	int GetPropSulfurToAry( XArrayLinearN<xSULFUR*,N>& aryOut ) {
		for( auto pProp : _m_listSpots ) {
			if( pProp->type == XGAME::xSPOT_SULFUR ) {
				xSULFUR *pSulfur = static_cast<xSULFUR*>( pProp );
				XBREAK( pSulfur == nullptr );
				aryOut.Add( pSulfur );
			}
		}
		return aryOut.size();
	}
	/**
		typeSpot타입의 스팟을 모두 읽어 주어진 어레이에 담아준다.	 
		typeSpot으로 NONE을 주면 모든 스팟을 읽는다.
	*/
	template<typename T, int N>
	int GetPropToAry( XGAME::xtSpot typeSpot, XArrayLinearN<T, N>& aryOut ) {
		for( auto pProp : _m_listSpots ) {
			if( pProp->type == typeSpot || typeSpot == XGAME::xSPOT_NONE ) {
				T pSpot = static_cast<T>( pProp );
				XBREAK( pSpot == nullptr );
				aryOut.Add( pSpot );
			}
		}
		return aryOut.size();
	}

	/**
	 typeSpot타입의 스팟을 vCenter를 기준으로 반경 radius안에서 모두 찾아 pOutAry에 넣어 돌려준다.
	*/
	template<int N>
	int GetSpotInSight( XArrayLinearN<xBASESPOT*,N> *pOutAry, 
								XGAME::xtSpot typeSpot,
								const XE::VEC2& vCenter,
								float radius ) {
		for( auto pProp : _m_listSpots ) {
			if( pProp->type == typeSpot ) {
				float distSq = (pProp->vWorld - vCenter).Lengthsq();
				if( distSq <= radius * radius )
					pOutAry->Add( pProp );
			}
		}
		return pOutAry->size();
	};
	template<typename T>
	T* GetpProp( XGAME::xtSpot typeSpot, ID idProp ) {
		auto pProp = GetpProp( idProp );
		if( pProp ) {
			XBREAK( pProp->type != typeSpot );
			return static_cast<T*>( pProp );
		}
// 		for( auto pProp : _m_listSpots ) {
// 			if( pProp->idSpot == idProp ) {
// 				if( pProp->type == typeSpot ) {
// 					return static_cast<T*>( pProp );
// 				}
// 				else {
// 					XBREAKF( 1, "invalid spot type:id=%d, type=%d", idProp, (int)pProp->type );
// 					break;
// 				}
// 			}
// 		}
		return nullptr;
	}
	xBASESPOT* GetpProp( ID idProp );
	xBASESPOT* GetpProp( const _tstring& strIdentifier );
	inline xBASESPOT* GetpProp( LPCTSTR szIdentifier ) {
//		_tstring ids = szIdentifier;
		return GetpProp( _tstring(szIdentifier) );
	}

	/**
	 vLT-vRB 사각형 영역안에 있는 typeSpot스팟을 모두 찾아내어 어레이에 담아준다.
	*/
	template<typename T, int N>
	int GetpPropFromArea( XGAME::xtSpot typeSpot, 
									const XE::VEC2& vLT, 
									const XE::VEC2& vRB,
									XArrayLinearN<T*,N> *pOutAry ) {
		XE::VEC2 vSize = vRB - vLT;
		for( auto pProp : _m_listSpots ) {
			if( pProp->type == typeSpot ) {
				if( XE::IsArea( vLT, vSize, pProp->vWorld ) )
					pOutAry->Add( static_cast<T*>( pProp ) );
			}
		}
		return pOutAry->size();
	}
	void SetAreaToSpots();
	void GetBaseSpotAttr( xBASESPOT *pSpot, XEXmlNode& childNode );
	XList4<xBASESPOT*>* GetpPropWhen( const _tstring& strIdentifier ) {
		if( strIdentifier.empty() )
			return nullptr;
		auto itor = m_mapSpotsByWhen.find( strIdentifier );
		if( itor == m_mapSpotsByWhen.end() )
			return nullptr;
		return &(itor->second);
	}
	XList4<xBASESPOT*>* GetpPropWhen( LPCTSTR szIdentifier ) {
		_tstring ids = szIdentifier;
		return GetpPropWhen( ids );
	}
	void AddWhenSpot( const _tstring& strWhenKey, xBASESPOT *pSpot );
	void AddWhenDelSpot( const _tstring& strWhenDelKey, xBASESPOT *pSpot );
	XList4<xBASESPOT*>* GetpPropWhenDel( const _tstring& strIdentifier ) {
		auto itor = m_mapSpotsByWhenDel.find( strIdentifier );
		if( itor == m_mapSpotsByWhenDel.end() )
			return nullptr;
		return &(itor->second);
	}
	XList4<xBASESPOT*>* GetpPropWhenDel( LPCTSTR szIdentifier ) {
		_tstring ids = szIdentifier;
		return GetpPropWhenDel( ids );
	}
	/// 스팟코드로 스팟리스트를 찾아내는 코드.
	XList4<xBASESPOT*>* GetpPropByCode( ID idCode ) {
		auto itor = m_mapSpotsByCode.find( idCode );
		if( itor == m_mapSpotsByCode.end() )
			return nullptr;
		return &( itor->second );
	}
	bool Save( LPCTSTR szXml );
	bool SaveDaily( xDaily* pProp, XEXmlNode& nodeDaily );
	void AddBaseAttribute( xBASESPOT *pProp, XEXmlNode& node );
	LPCTSTR MakeIds( ID idSpot );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
#ifdef _xIN_TOOL
	void DestroySpot( ID idSpot );
	xBASESPOT* DelSpot( ID idSpot );
	void CopyProp( xBASESPOT *pBasePropDst, xBASESPOT *pBasePropSrc );
	void AddSpot( xBASESPOT *pSpot );
#endif // _xIN_TOOL
	template<typename T>
	void CopyPropTemplate( xBASESPOT *pBasePropDst, xBASESPOT *pBasePropSrc ) {
		auto pDst = static_cast<T>( pBasePropDst );
		auto pSrc = static_cast<T>( pBasePropSrc );
		*pDst = *pSrc;
	}
private:	
#ifndef _xIN_TOOL
	void AddSpot( xBASESPOT *pSpot );
#endif // not _xIN_TOOL
#ifdef _xIN_TOOL
	void DestroySpotInMap( std::unordered_map<_tstring, XList4<xBASESPOT*>>& mapSpots, ID idSpot );
	void DestroySpotInMap( std::unordered_map<ID, XList4<xBASESPOT*>>& mapSpots, ID idSpot );
#endif // _xIN_TOOL

	static xBASESPOT* sCreateSpot( XGAME::xtSpot type );
}; // XPropWorld

extern XPropWorld *PROP_WORLD;
