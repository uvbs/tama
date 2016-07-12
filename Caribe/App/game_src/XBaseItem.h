#pragma once
#include "XPropItem.h"
#include "XArchive.h"

#define VER_ITEM_SERIALIZE		2
#if VER_ITEM_SERIALIZE > 0xff
#error "버전값은 255이상을 넘을 수 없습니다."
#endif

////////////////////////////////////////////////////////////////
class XBaseItem
{
public:
	static XBaseItem* sCreateItem( LPCTSTR szIdentifier );
	static XBaseItem* sCreateItem( const _tstring& strIdentifier ) {
		return sCreateItem( strIdentifier.c_str() );
	}
	static XBaseItem* sCreateItem( ID idProp );
  static XBaseItem* sCreateItem( ID idProp, ID snItem );
	static XBaseItem* sCreateItem( XPropItem::xPROP *pProp, ID snItem = 0 );
	static void sSerialize( XArchive& ar, XBaseItem *pItem );
	static XBaseItem* sCreateDeSerialize( XArchive& ar, int ver );
  static XBaseItem* sCreateDeSerialize( XArchive& ar, ID idProp, ID snItem, int ver );
  static int sDeSerializeHeaderAndType( XArchive& ar, ID *pOutidProp, ID *pOutsnItem, XGAME::xtItem *pOutType, int ver );
	static int sDeSerializeHeader( XArchive& ar, ID *pOutidProp, ID *pOutsnItem, int ver );
	static int sDeSerializeNoHeader( XArchive& ar, XBaseItem *pItem, ID idProp, ID snItem, int ver );
private:
private:
  ID m_idProp = 0;
	XPropItem::xPROP *_m_pProp = nullptr;
  ID m_keyProp = 0;
	ID m_snItem;			// 아이템 시리얼 번호
	BOOL  m_bEquip;			// 아이템을 히어로가 사용중이면 TRUE
	int m_Num;				// 겹칠수 있는 아이템의 경우 개수
	bool m_bNew;			// 새로 얻은 아이템?
	union uBook
	{
		DWORD m_Param[ 4 ];		// 커스텀 파라메터
		struct xBook {
			XINT64 exp;
			DWORD reserved;
			WORD w0;
			BYTE b0;
			BYTE b1;
		};
		xBook m_Book;
	};
	uBook m_uBook;
//	xParam m_Param;
	void Init() {
		m_snItem = 0;
		m_Num = 1;
		m_bEquip = FALSE;
		m_bNew = false;
		XCLEAR_ARRAY(m_uBook.m_Param);
	}
	void Destroy();
public:
	XBaseItem() { Init(); }
	XBaseItem( ID idItem, ID snNum );
	XBaseItem( LPCTSTR szIdentifier, ID snNum );
	XBaseItem( XPropItem::xPROP *pProp, ID snItem = 0 );
	virtual ~XBaseItem() { Destroy(); }
	//
	bool operator == ( const XBaseItem *pComp ) {
		return m_snItem == pComp->GetsnItem();
	}
  XPropItem::xPROP* GetpProp() {
    if( m_idProp && m_keyProp != PROP_ITEM->GetidKey() ) {
      _m_pProp = PROP_ITEM->GetpProp( m_idProp );
      XBREAK( _m_pProp == nullptr );
      m_keyProp = PROP_ITEM->GetidKey();
    }
    return _m_pProp;
  }
  void SetpProp( ID idProp ) {
    _m_pProp = PROP_ITEM->GetpProp( idProp );
    if( XBREAK( _m_pProp == nullptr ) )
      return;
    m_idProp = idProp;
    m_keyProp = PROP_ITEM->GetidKey();
  }
//	GET_ACCESSOR( XPropItem::xPROP*, pProp );
	GET_ACCESSOR_CONST( ID, snItem );
	GET_SET_ACCESSOR( int, Num );
	GET_ACCESSOR( BOOL, bEquip );
	GET_SET_ACCESSOR(bool, bNew);
	ID getid() {
		return m_snItem;
	}
	XGAME::xtItem GetType( void ) {
		return GetpProp()->type;
	}
	ID GetidProp( void ) {
		return GetpProp()->idProp;
	}
	LPCTSTR GetszIdentifier( void ) {
		return GetpProp()->strIdentifier.c_str();
	}
	_tstring GetstrIdentifier() {
		return GetpProp()->strIdentifier;
	}
	LPCTSTR GetszName( void ) {
		if( GetpProp()->idName == 0xffffffff )
			return _T( "" );
		return XTEXT(GetpProp()->idName);
	}
	LPCTSTR GetszDesc( void ) {
		if( GetpProp()->idDesc == 0xffffffff )
			return _T("");
		return XTEXT(GetpProp()->idDesc);
	}
	LPCTSTR GetszIcon( void ) {
		return GetpProp()->strIcon.c_str();
	}
	void SetBookExp( XINT64 exp ) {
		m_uBook.m_Book.exp = exp;
	}
	XINT64 GetBookExp( void ) {
		return m_uBook.m_Book.exp;
	}
// 	XGAME::xtClan GetBookClan( void ) {
// 		return (XGAME::xtClan)GetpProp()->subType1;
// 	}
	bool IsBook() {
		return GetpProp()->type == XGAME::xIT_BOOK;
	}
	bool IsEquip() {
		return GetpProp()->type == XGAME::xIT_EQUIP;
	}
	bool IsSoul() {
		return GetpProp()->type == XGAME::xIT_SOUL;
	}
	ID GetidHero();
	LPCTSTR GetidsHero();
	LPCTSTR GetResPathIcon( void );
	void SetbEquip(BOOL bEquip)
	{
		//이미 m_bEquip가 TRUE인(다른 히어로가 사용중인) 장비를 사용해선 안된다
		XBREAK(m_bEquip && bEquip);
		m_bEquip = bEquip;
	}
private:
	// ar << 
	virtual int Serialize( XArchive& ar );
	// ar >>
	virtual int DeSerialize( XArchive& ar, int ver );
public:
	// 
	virtual void MakeFakeOption( void ) {}
	bool IsBetterThan( XBaseItem* pItemCompare );
	float GetValAdjParam( XGAME::xtParameter adjParam );
};

