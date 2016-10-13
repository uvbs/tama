#include "stdafx.h"
#include "XBaseItem.h"
#include "XArchive.h"
#include "XPropHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// static
/**
 @brief identifier에 의해 아이템 객체를 생성한다.
*/
XBaseItem* XBaseItem::sCreateItem( LPCTSTR szIdentifier )
{
	auto pProp = PROP_ITEM->GetpProp( szIdentifier );
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	return XBaseItem::sCreateItem( pProp );
}
XBaseItem* XBaseItem::sCreateItem( ID idProp )
{
	auto pProp = PROP_ITEM->GetpProp( idProp );
	if( XBREAK( pProp == nullptr ) )
		return nullptr;
	return XBaseItem::sCreateItem( pProp );
}
XBaseItem* XBaseItem::sCreateItem( ID idProp, ID snItem )
{
  auto pProp = PROP_ITEM->GetpProp( idProp );
  if( XBREAK( pProp == nullptr ) )
    return nullptr;
  return XBaseItem::sCreateItem( pProp, snItem );
}
XBaseItem* XBaseItem::sCreateItem( const XPropItem::xPROP *pProp, ID snItem )
{
	XBaseItem *pItem = new XBaseItem( pProp, snItem );
	return pItem;
}


/**
 @brief 아카이브로부터 아이템 객체를 생성한다.
*/
XBaseItem* XBaseItem::sCreateDeSerialize( XArchive& ar, int ver )
{
	int type;
	XBaseItem *pBaseItem = nullptr;
	ar >> type;
	switch( type )
	{
	case 0:
	default:
		pBaseItem = new XBaseItem;
		pBaseItem->DeSerialize( ar, ver );
		break;
	}
	XBREAK( pBaseItem == nullptr );
	return pBaseItem;
}

/**
 @brief idProp과 snItem을 외부에서 받는 버전
*/
XBaseItem* XBaseItem::sCreateDeSerialize( XArchive& ar, ID idProp, ID snItem, int ver )
{
  int type;
  XBaseItem *pBaseItem = nullptr;
  ar >> type;
  switch( type )
  {
  case 0:
  default:
    pBaseItem = new XBaseItem;
    pBaseItem->DeSerialize( ar, ver );
    break;
  }
  XBREAK( pBaseItem == nullptr );
  return pBaseItem;
}

void XBaseItem::sSerialize( XArchive& ar, XBaseItem *pItem )
{
	if( XBREAK( pItem == nullptr ) )
		return;
	ar << (DWORD)pItem->GetType();
	pItem->Serialize( ar );
}

int XBaseItem::sDeSerializeHeader( XArchive& ar, ID *pOutidProp, ID *pOutsnItem, int ver )
{
	ar >> (*pOutidProp);
	ar >> (*pOutsnItem);
	return 1;
}
int XBaseItem::sDeSerializeHeaderAndType( XArchive& ar, ID *pOutidProp, ID *pOutsnItem, XGAME::xtItem *pOutType, int ver )
{
  int i0;
  ar >> i0;   *pOutType = (XGAME::xtItem) i0;
  ar >> ( *pOutidProp );
  ar >> ( *pOutsnItem );
  return 1;
}
/**
 @brief XBaseItem객체 의존적이지 않은 Deserialize
 이미 존재하는 아이템에 대해서도 Update가 가능하다.
*/
int XBaseItem::sDeSerializeNoHeader( XArchive& ar, XBaseItem *pItem, ID idProp, ID snItem, int ver )
{
  if(XBREAK( pItem == nullptr ) )
    return 0;
//	BOOL bEquip;
  XBREAK( pItem->m_snItem && pItem->m_snItem != snItem );
	pItem->m_snItem = snItem;
  pItem->SetpProp( idProp );
	if( XBREAK( pItem->GetpProp() == nullptr ) )
    return 0;
	ar >> pItem->m_Num;
	int numAry;
	ar >> numAry;
	for( int i = 0; i < numAry; ++i ) {
		ar >> pItem->m_uBook.m_Param[ i ];
	}
	int i0;
	ar >> i0;
	//ar >> bEquip; pItem->m_bEquip = (BOOL)bEquip;
	return 1;
}
////////////////////////////////////////////////////////////////
XBaseItem::XBaseItem( ID idItem, ID snNum )
{
	Init();	
	m_idProp = idItem;
	_m_pProp = const_cast<XPropItem::xPROP*>( PROP_ITEM->GetpProp( idItem ) );
	XBREAK( _m_pProp == nullptr );
	if( _m_pProp == nullptr )
		return;
//	m_keyProp = PROP_ITEM->GetidKey();
	m_snItem = snNum;
}

XBaseItem::XBaseItem( LPCTSTR szIdentifier, ID snNum )
{
	Init();
	//
	_m_pProp = const_cast<XPropItem::xPROP*>( PROP_ITEM->GetpProp( szIdentifier ) );
	XBREAK( _m_pProp == nullptr );
	if( _m_pProp == nullptr )
		return;
	m_idProp = _m_pProp->idProp;
	//m_keyProp = PROP_ITEM->GetidKey();
	m_snItem = snNum;
}

XBaseItem::XBaseItem( const XPropItem::xPROP *pProp, ID snItem )
{
	Init();
	XBREAK( pProp == nullptr );
	_m_pProp = const_cast<XPropItem::xPROP*>( pProp );
	m_idProp = _m_pProp->idProp;
	//m_keyProp = PROP_ITEM->GetidKey();
	if( snItem == 0 )
		m_snItem = XE::GenerateID();
}


void XBaseItem::Destroy()
{
}


const XPropItem::xPROP* XBaseItem::GetpProp() const 
{
// 	if( m_idProp && m_keyProp != PROP_ITEM->GetidKey() ) {
// 		_m_pProp = const_cast<XPropItem::xPROP*>( PROP_ITEM->GetpProp( m_idProp ) );
// 		XBREAK( _m_pProp == nullptr );
// 		m_keyProp = PROP_ITEM->GetidKey();
// 	}
	return _m_pProp;
}
void XBaseItem::SetpProp( ID idProp ) 
{
	_m_pProp = const_cast<XPropItem::xPROP*>( PROP_ITEM->GetpProp( idProp ) );
	if( XBREAK( _m_pProp == nullptr ) )
		return;
	m_idProp = idProp;
//	m_keyProp = PROP_ITEM->GetidKey();
}

// ar << 
int XBaseItem::Serialize( XArchive& ar )
{
	ar << GetpProp()->idProp;
	ar << m_snItem;
	ar << m_Num;
	int numAry = XNUM_ARRAY(m_uBook.m_Param);
	ar << numAry;
	XBREAK( GetpProp()->type == XGAME::xIT_BOOK && GetBookExp() == 0 );
	for( int i = 0; i < numAry; ++i )	{
		ar << m_uBook.m_Param[i];
	}
	ar << (DWORD)0;
	return 1;
}

// ar >>
int XBaseItem::DeSerialize( XArchive& ar, int ver )
{
	ID idProp, snItem;
	sDeSerializeHeader( ar, &idProp, &snItem, ver );
	sDeSerializeNoHeader( ar, this, idProp, snItem, ver );
	return 1;
}

/**
 @brief 아이콘의 리소스 패스를 얻는다.,.
*/
LPCTSTR XBaseItem::GetResPathIcon( void ) 
{
	return XE::MakePath( DIR_IMG, GetpProp()->strIcon );
}

/**
 @brief this가 pItemCompare보다 좋냐
 pItemEquip를 이미 슬롯에 장착되어 있는 템으로 하는게 좋다. 두템이 옵션이 달라 우열을 가릴수 없을땐 this가 좋은걸로 한다.
 비교대상과 능력이 같을경우 this가 좋은것으로 한다.
*/
bool XBaseItem::IsBetterThan( const XBaseItem* pItemEquip )
{
	// 등급이 높으면 일단 좋음.
	if( GetpProp()->grade > pItemEquip->GetpProp()->grade )
		return true;
	// 이 템이 가진 옵션을 비교
//	XARRAYLINEARN_LOOP_AUTO( GetpProp()->aryAdjParam, &adj )
	for( const auto& adj : GetpProp()->aryAdjParam ) {
		if( adj.adjParam ) {
			auto val = adj.param;	// this템의 능력치값
			// this의 능력치가 비교템의 해당 보정치값보다 크다.
			auto valEquip = pItemEquip->GetValAdjParam( adj.adjParam );
			// 비교템이 아예 해당옵션을 가지고 있지 않으면 this가 좋은거
			if( valEquip == 0 )
				return true;
			if( val > valEquip )
				return true;
		}
	}
	return false;
}

/**
 @brief 이템의 adjParam옵션의 능력치값을 얻는다.
 @return 해당 옵션을 가지고 있지않으면 0을 리턴한다.
*/
float XBaseItem::GetValAdjParam( XGAME::xtParameter adjParam ) const 
{
// 	XARRAYLINEARN_LOOP_AUTO( GetpProp()->aryAdjParam, &adj )
	for( const auto& adj : GetpProp()->aryAdjParam ) {
		// this가 가진 옵션중 하나가 비교할옵션과 같으면 그 능력치값을 리턴
		if( adj.adjParam == adjParam )
			return adj.param;
	}
	// 해당 보정파라메터의 능력치가 없으면 0
	return 0;
}

/**
@brief 어떤 영웅의 영혼석인지 영웅의 idProp을 얻는다.
@return 영혼석이 아니거나 영웅이 존재하지 않는다면 0을 리턴한다.
*/
ID XBaseItem::GetidHero()
{
	if (XBREAK(GetpProp()->type != XGAME::xIT_SOUL))
		return 0;
	auto pProp = PROP_HERO->GetpProp(GetpProp()->strIdHero);
	if (pProp)
		return pProp->idProp;
	return 0;
}
/**
@brief 영혼석 대상 영웅의 strIdentifier 얻는다.
*/
LPCTSTR XBaseItem::GetidsHero()
{
	if (XBREAK(GetpProp()->type != XGAME::xIT_SOUL))
		return nullptr;
	return GetpProp()->strIdHero.c_str();
}