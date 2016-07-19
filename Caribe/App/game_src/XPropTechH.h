#pragma once
#if defined(_CLIENT) || defined(_GAME_SERVER)
#include "XFramework/XEBaseProp.h"
#include "XXMLDoc.h"

const int VER_PROP_TECH = 1;

XE_NAMESPACE_START( XGAME )
struct xRES_NUM;
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
	inline void DrawIcon( const XE::VEC2& vAdj = XE::VEC2() ) {
		psfcIcon->Draw( vPos + vAdj );
	}
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
		return (tier - 1) * 5;
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
XE_NAMESPACE_END;

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

