#pragma once

#include <map>
#include <string>

#include "../Resource/defineGame.h"
#include "XFramework/XEBaseProp.h"
#include "XArchive.h"

//#include "Skill/xSkill.h"

const int VER_PROP_ITEM = 1;

// 아이템 프롭
class XPropItem : public XEBaseProp
{
public:
	enum { MAX_ADJPARAM = 4 };
	struct xAdj {
		XGAME::xtParameter adjParam = XGAME::xNONE_PARAM;	// 보정파라메터
		float param = 0;					// 보정값
	};
	static ID sGetidHeroToidItemSoul( ID idPropHero );
	//////////////////////////////////////////////////////////////////////////
	struct xPROP {
	private:
		DWORD goldCost;					///< 금화가격. 장착템은 이 가격을 사용하지 않음.
	public:
		ID idProp;						///< 아이템 아이디
		_tstring strIdentifier;			///< 아이템 식별자
		ID idName = 0;
		ID idDesc = 0;
		XGAME::xtItem type;				///< 아이템 종류
		int subType1;					///< 종류(부1)
		XGAME::xtItemUse useType;		///< 아이템 사용방식
		XGAME::xtGrade grade;			///< 아이템 등급
		int maxStack;					///< 몇개나 겹칠수 있는 아이템인가. 0이거나 음수거나 1은 겹쳐지지 않음
		XGAME::xtParts parts;			///< 장착부위
		DWORD cashCost;					///< 캐쉬가격
		_tstring strActive;				///< 아이템 스킬
//		int numOverlap;					
		_tstring strIdHero;				///< 영혼석일경우 어떤 영웅의 영혼석인지
		_tstring strIcon;				///< 아이콘 png파일명
		XVector<xAdj> aryAdjParam;	///< 보정파라메터
		
		xPROP() 
		: aryAdjParam( MAX_ADJPARAM ) {
			idProp = 0;
			type = XGAME::xIT_NONE;
			subType1 = 0;
			useType = XGAME::xIU_NONE;
			grade = XGAME::xGD_NONE;
			maxStack = 0;
			parts = XGAME::xPARTS_NONE;
			goldCost = 0;
			cashCost = 0;
		}
		void SetgoldCost( DWORD cost ) {
			goldCost = cost;
		}
//		BOOL IsOverlapable() const { return numOverlap > 1; }		/// 겹치기가 가능한 아이템인가
		bool IsEquipable() const {
			XBREAK( type == XGAME::xIT_EQUIP && parts == XGAME::xPARTS_NONE );
			return  type == XGAME::xIT_EQUIP && parts != XGAME::xPARTS_NONE;
		}
		DWORD GetBuyCost( int level ) const;
		DWORD GetSellCost( int level ) const;
		// 전리품이냐
		bool IsBooty() const {
			return type == XGAME::xIT_SCALP || type == XGAME::xIT_MEDAL || type == XGAME::xIT_SCROLL;
		}
		bool IsSoul() const {
			return type == XGAME::xIT_SOUL;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	}; // xProp
private:
	std::unordered_map<_tstring, xPROP*> m_mapData;			// 아이템 프로퍼티 리스트
	std::unordered_map<ID, xPROP*> m_mapID;					// 프로퍼티 아이디로 구성된 맵
//	std::unordered_map<ID, xPROP*>::iterator m_itorID;
	XVector<xPROP*> m_aryItem;		// 모든 아이템의 리스트
//	XList4<xPROP*> m_listTemp;		// 임시용 리스트

	void Init() {
	}
	void Destroy();

public:
	XPropItem();
	virtual ~XPropItem() { Destroy(); }
	//
	//
// 	void GetNextClear() {
// 		m_itorID = m_mapID.begin();
// 	}

//	GET_ACCESSOR( XArrayLinear<xPROP*>&, aryItem );

	xPROP* GetpProp( LPCTSTR szIdentifier ) { 
		TCHAR szBuff[ 256 ];
		_tcscpy_s( szBuff, szIdentifier );
		_tcslwr_s( szBuff );
		_tstring str = szBuff;
        return GetpProp( str ); 
    }
	// strIdentifier는 모두 소문자여야 함.
	xPROP* GetpProp( const _tstring& strIdentifier ) {
		XBREAK( strIdentifier.empty() == TRUE );
		std::unordered_map<_tstring, xPROP*>::iterator itor;
		itor = m_mapData.find( strIdentifier );
		if( itor == m_mapData.end() )
			return nullptr;							// 못찾았으면 널 리턴
		xPROP*pMob = (*itor).second;
		XBREAK( pMob == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
		return pMob;
	}
	xPROP* GetpProp( ID idType ) {
		XBREAK( idType == 0 );
		std::unordered_map<ID, xPROP*>::iterator itor;
		itor = m_mapID.find( idType );
		if( itor == m_mapID.end() ) {
			return nullptr;							// 못찾았으면 에러 리턴
		}
		xPROP *pMob = (*itor).second;
		XBREAK( pMob == nullptr );			// 위에서 find로 검사했기때문에 nullptr나와선 안됨.
		return pMob; 
	}
	xPROP* GetpPropByidHero( ID idHero );
	/// idProp아이템의 아이콘파일의 패스를 얻는다. res패스이므로 img/icon_test.png 이런식이 된다.
	LPCTSTR GetIconResPath( ID idProp ) {
		auto pProp = GetpProp( idProp );
		if( pProp == nullptr )
			return nullptr;
		return XE::MakePath( DIR_IMG, pProp->strIcon.c_str() );
	}
	xPROP* GetpPropFromName( LPCTSTR szName );
	void Add( const _tstring& strIdentifier, xPROP *pProp );
	void Add( xPROP *pProp );
	xPROP* GetpPropRandom() {
		return *(m_aryItem.GetpFromRandom());
	}
	int GetListWithGrade( XGAME::xtGrade grade, XGAME::xtItem typeItem, XList4<xPROP*> *pOutList );
	// grade아이템을 랜덤으로 num개 얻어낸다.
	int GetpPropRandom( XGAME::xtGrade grade, XGAME::xtItem typeItem, int num, XList4<xPROP*> *pOutList );
	BOOL ReadProp( CToken& token, DWORD dwParam=0 ) override;
	int GetSize() override {
		return m_mapData.size();
	}
	void OnDidFinishReadProp( CToken& token ) override;
	void DestroyProps();
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	//	xPROP* GetClanBookProp( XGAME::xtClan clan );
};

typedef std::pair<XPropItem::xPROP*, int> ItemBox;
namespace XGAME {
  inline void GetItemBox( const ItemBox& box, XPropItem::xPROP** ppOut, int *pOutNum ) {
    *ppOut = std::get<0>( box );
    *pOutNum = std::get<1>( box );
  }
}; // XGAME

extern XPropItem *PROP_ITEM;