/********************************************************************
	@date:	2014/12/10 19:01
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XPropTech.h
	@author:	xuzhu
	
	@brief:	테크트리 프로퍼티
*********************************************************************/
#pragma once
#if defined(_CLIENT) || defined(_GAME_SERVER)
#include "XFramework/XEBaseProp.h"
#include "XXMLDoc.h"

const int VER_PROP_TECH = 1;

namespace XGAME {
	struct xRES_NUM;
}
class XPropTech;
class XPropTech : public XXMLDoc, public XEBaseProp
{
public:
	enum { ADJ_X = 85,
		MAX_TIER = 5,		// 최대 단계수
		MAX_POINT = 5		// 특성당 최대 포인트수
	};
	static std::shared_ptr<XPropTech> sGet();
  static void sDestroy() {
    s_spSingleton.reset();
  }
	static ID sGenerateID() {
		return s_idGlobal++;
	}
private:
	static std::shared_ptr<XPropTech> s_spSingleton;
	static ID s_idGlobal;
public:
	struct xtResearch {
		int sec = 0;		// 필요연구 시간.
		XArrayLinearN<XGAME::xtResource, 2> aryResourceNeed;	///< 필요 자원2가지
		XArrayLinearN<int, 2> aryNumNeed;		///< 필요한 자원 양.
		XVector<XGAME::xRES_NUM> GetaryNeedRes() const;
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	// 특성하나의 노드
	struct xNodeAbil;
	struct xNodeAbil {
	private:
		XList4<ID> listParentID;		///< 부모노드의 아이디 리스트
		XList4<xNodeAbil*> listParent;	///< 부모노드 리스트(부모는 복수가 될수 있다. 또한 루트가 아니어도 부모가 없을수도 있다.
		XList4<ID> listChildID;		///< 자식노드의 아이디 리스트(ID붙은것들은 로딩할때외엔 쓰지 않음.
	public:
		ID idNode;					///< 노드 아이디
//		std::string strIdentifier;		///< 식별 아이디 문자열
		_tstring strSkill;			///< 스킬 식별자(아이콘은 스킬프로퍼티것을 쓴다)
		_tstring strIcon;			///< 스킬프로퍼티가 있다면 처음 읽을때 거기서 참조하고 직접 지정하면 지정한 값으로 세팅되게
		XGAME::xtUnit unit = XGAME::xUNIT_NONE;	///< 어떤 유닛의 특성인가.
		ID idName;					///< 이름
		ID idDesc;					///< 설명
//		int lvOpenable;					///< 오픈가능 레벨
		int point;					///< 현재 찍은 포인트(이것은 인스턴스로 사용시에만 사용된다.)
		int maxPoint;				///< 최대 포인트
		int tier = 0;				///< 레벨단계
		XE::VEC2 vPos;				///< 노드 출력 위치
		XList4<xNodeAbil*> listChild;	///< 차일드 노드 리스트
#ifdef _CLIENT
		XSurface *psfcIcon;				///< 아이콘 서피스
		void DrawIcon( const XE::VEC2& vAdj = XE::VEC2());
#endif
		xNodeAbil() {
			idNode = 0; //sGenerateID();
//			lvOpenable = 1;
			point = 0;
			maxPoint = MAX_POINT;
			idName = 0;
			idDesc = 0;
#ifdef _CLIENT
			psfcIcon = nullptr;
#endif // _CLIENT
		}
		ID getid() {
			return idNode;
		}
		const XList4<xNodeAbil*>& GetListParents() {
			return listParent;
		}
		XList4<xNodeAbil*>& GetListParentsMutable() {
			return listParent;
		}
		XGAME::xtSize GetSizeUnit() {
			return XGAME::GetSizeUnit( unit );
		}
		int GetLvOpanable() {
			return (tier-1) * 5;
		}
		void AddChild( xNodeAbil *pNode ) {
			listChild.Add( pNode );
			pNode->listParent.Add( this );
		}
		// 이 노드가 최상단 노드(더미)인가
		bool IsRoot() {
			return idNode == 0;
		}
		// 이노드가 _tier노드인가
		bool IsTier( int _tier ) {
			return _tier == tier;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class XPropTech;
	}; // struct xNodeAbil
private:
	// 이거 왜 맵으로 안했징?
	XList4<xNodeAbil*> m_listNode;				///< 전체 프로퍼티 배열
	XVector<XList4<xNodeAbil*>> m_aryUnitsAbil;		///< 유닛별로 따로 모아놓은거.
	XVector<xNodeAbil*> m_aryRoots;					// 각 유닛들 특성의 루트들.(트리구조. 최상단루트는 더미)
	std::vector<int> m_aryGoldUnlock;	// 잠금해제된 개수별 금화비용
	std::vector<xtResearch> m_aryCost;	///< 특성연구 비용(시간/자원)
	void Init() {
		m_aryRoots[ 0 ] = nullptr;
	}
	void Destroy();
public:
	XPropTech();
	virtual ~XPropTech() { Destroy(); }

	int GetGoldUnlockWithNum( int numUnlock ) {
		// 모든 테이블을 다 만들수가 없어서 테이블 범위를 벗어나서 요청이 들어오면 가장 비싼 값으로 리턴함.
		if( numUnlock < (int)m_aryGoldUnlock.size() )
			return m_aryGoldUnlock[ numUnlock ];
		else
			return m_aryGoldUnlock[ m_aryGoldUnlock.size() - 1 ];
		return -1;
	}
	// 포인트개수에 따른 훈련비용(시간/자원)
	xtResearch& GetResearchWithNum( int numPoint ) {
		if( XASSERT(numPoint > 0) )
			if( XASSERT( numPoint < (int)m_aryCost.size() ) )
				return m_aryCost[ numPoint ];
		return m_aryCost[ 0 ];
	}
	xNodeAbil* GetpNode( ID idAbil );
	// 특성의 스킬이름으로 검색
	xNodeAbil* GetpNode( LPCTSTR szSkill );
	xNodeAbil* GetpNode( const _tstring& strSkill ) { 
		XBREAK( strSkill.empty() == TRUE ); 
		return GetpNode( strSkill.c_str() ); 
	}
	xNodeAbil* GetpNode( XGAME::xtUnit unit, ID idAbil );
	xNodeAbil* GetpNodeByName( XGAME::xtUnit unit, LPCTSTR szName );
	xNodeAbil* GetpNodeBySkill( XGAME::xtUnit unit, LPCTSTR idsSkill );
	xNodeAbil* GetpNodeBySkill( XGAME::xtUnit unit, const _tstring& strSkill ) {
		return GetpNodeBySkill( unit, strSkill.c_str() );
	}
//	xNodeAbil* GetNodeAbilByAry( XArrayLinearN<xNodeAbil*, 256>& ary, const std::string& strAbilIds );
	xNodeAbil* GetNodeAbilByAry( XArrayLinearN<xNodeAbil*, 256>& ary, ID idNode );
	xNodeAbil* GetNodeAbilByAry( const XVector<xNodeAbil*>& ary, ID idNode );
	XPropTech::xNodeAbil* GetNodeAbilByAry( const XList4<xNodeAbil*>& ary, ID idNode );
	/**
	 @brief 모든 유닛의 특성노드 리스트를 만든다.
	*/
	template<int N>
	int GetNodesToAry( XArrayLinearN<xNodeAbil*, N> *pOut ) {
		for( auto pNode : m_listNode ) {
			pOut->Add( pNode );
		}
		return pOut->size();
	}
	template<int N>
	int GetNodesToAry( XGAME::xtUnit unit, XArrayLinearN<xNodeAbil*, N> *pOut ) {
		int max = m_aryUnitsAbil[unit].size();
		for( auto pNode : m_aryUnitsAbil[unit] ) {
			pOut->Add( pNode );
		}
		return pOut->size();
	}
	XList4<xNodeAbil*>& GetaryUnitsAbil( XGAME::xtUnit unit ) {
		return m_aryUnitsAbil[unit];
	}
	void Add( XGAME::xtUnit unit, xNodeAbil *pNewNode );
	
	virtual int GetSize( void ) { 
		return m_listNode.size();
	}

	LPCTSTR GetszIcon( XGAME::xtUnit unit, ID idNode ) {
		xNodeAbil *pNode = GetpNode( unit, idNode );
		if( pNode )
			return pNode->strIcon.c_str();
		return _T("");
	}
	BOOL Load( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
	xNodeAbil* LoadUnit( XEXmlNode& nodeRoot, XGAME::xtUnit unit );
	xNodeAbil* GetNodeByPos( XGAME::xtUnit unit, const XE::VEC2& vPos );
	BOOL Save( LPCTSTR szXml );
	void DelNode( ID idNode );
	int GetNodesByTierToAry( XGAME::xtUnit unit, int tier, XArrayLinearN<xNodeAbil*, 128> *pOut );
	/// 최상단 더미노드를 리턴한다.
	xNodeAbil* GetpNodeRoot( XGAME::xtUnit unit ) {
		return m_aryRoots[ unit ];
	}
	void DelOutLinkByNode( XGAME::xtUnit unit, ID idNode );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
private:
	BOOL ReadProp( CToken& token, DWORD dwParam ) override;					///< txt 파싱
	void SetSec( int tier, int point, int sec );
//	void SetGold( int tier, int point, DWORD gold );
	void SetResource( int tier, int point, XGAME::xtResource resType, int num );
	void OnDidBeforeReadProp( CToken& token );
	void OnDidFinishReadProp( CToken& token ) override;
	void DoLinkParentChild( XGAME::xtUnit unit );
	void ClearChild( xNodeAbil* pParent, xNodeAbil* pChild );
	void ClearChild( xNodeAbil* pParent, ID idChild );
	void ClearChild( ID idParent, xNodeAbil* pChild );
	void ClearChild( ID idParent, ID idChild );
public:
};

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

