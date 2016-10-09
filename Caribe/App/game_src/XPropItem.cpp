#include "stdafx.h"
#include "XPropItem.h"
#include "etc/Token.h"
#include "XFramework/XTextTable.h"
#include "XFramework/XConstant.h"
#include "XPropHero.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

#define ID_START_SOUL	60000
/*
PROP_ITEM = new XPropItem;
if( PROP_ITEM->Load( _T("propItem.txt") ) == FALSE )
{
	// 에러처리
}
*/
XPropItem *PROP_ITEM = nullptr;

XPropItem::XPropItem() 
{ 
	Init(); 
	//
}

void XPropItem::Destroy()
{
	DestroyProps();
}

void XPropItem::DestroyProps() 
{
	for( auto pProp : m_aryItem ) {
		SAFE_DELETE( pProp );
	}
	m_aryItem.clear();
	m_mapData.clear();
	m_mapID.clear();
}

BOOL XPropItem::ReadProp( CToken& token, DWORD dwParam )
{
	ID idItem = token.GetNumber();
	if( GetpProp( idItem ) )
	{
		XERROR( "%s: duplication Item ID. %d", GetstrFilename(), idItem );
		return FALSE;
	}
	if( idItem == TOKEN_EOF )
		return FALSE;	// false를 리턴해야 루프를 빠져나옴
	xPROP *pItem = new xPROP;		
	pItem->idProp = idItem;													// 아이템 ID
	//
	XBREAK( pItem->idProp > 0xffff );		// 워드크기 이상 금지
	pItem->strIdentifier = GetTokenIdentifier( token );						// 아이템 식별자
	pItem->idName = GetTokenID( token );
	pItem->idDesc = GetTokenID( token );
	pItem->type			= (XGAME::xtItem) TokenGetNumber( token );			// 아이템 종류
	pItem->subType1		= (int)TokenGetNumber( token );
	pItem->useType		= (XGAME::xtItemUse) TokenGetNumber( token );				// 아이템 사용방식
	pItem->grade		= (XGAME::xtGrade) TokenGetNumber( token );					// 아이템 등급
	pItem->maxStack		= token.GetNumber();
	pItem->parts		= (XGAME::xtParts) TokenGetNumber( token );
	pItem->m_strPayItem = GetTokenString( token );
	pItem->SetgoldCost( token.GetNumber() );									// 금화 가격
	pItem->cashCost		= token.GetNumber();									// 캐쉬 가격
	for( int i = 0; i < MAX_ADJPARAM; ++i )	{
		xAdj adj;
		adj.adjParam = (XGAME::xtParameter) TokenGetNumber( token );
		adj.param = token.GetNumberF();
//		pItem->aryAdjParam.Add( adj );
		pItem->aryAdjParam[i] = adj;
	}
	pItem->strActive = GetTokenString( token );			// 아이템 스킬
	pItem->strIdHero = GetTokenString(token);
	pItem->strIcon		= GetTokenString( token );								// 아이템 아이콘
	if( pItem->type == XGAME::xIT_SOUL ) {
		XBREAK( PROP_HERO == nullptr );
		XBREAKF( pItem->strIdHero.empty(), "영혼석아이템에 영웅식별자 누락:%s", pItem->strIdentifier.c_str() );
		auto pPropHero = PROP_HERO->GetpProp( pItem->strIdHero );
		if( pPropHero ) {
			pItem->strIcon = pPropHero->strFace;
		} else {
			XBREAKF( pPropHero == nullptr, "idItem=%d:%s라는 영웅은 없음", pItem->idProp, pItem->strIdHero.c_str() );
		}
	} else {
		if( pItem->strIcon.empty() == false )
			pItem->strIcon += _T( ".png" );
	}

	// 추가
	Add( pItem->strIdentifier, pItem );	
	
	return TRUE;
}


void XPropItem::Add( const _tstring& strIdentifer, xPROP *pProp )
{
	m_mapData[ strIdentifer ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
	m_aryItem.Add( pProp );
}

void XPropItem::Add( xPROP *pProp )
{
	XBREAK( pProp == nullptr );
	XBREAK( pProp->strIdentifier.empty() );
	XBREAK( pProp->idProp == 0 );
	m_mapData[ pProp->strIdentifier ] = pProp;		// map에다 넣음.
	m_mapID[ pProp->idProp ] = pProp;		// ID로 검색용 맵에도 넣음.
	m_aryItem.Add( pProp );
}

void XPropItem::OnDidFinishReadProp( CToken& token )
{
	// 60000번 부터 영혼석이 차지한다. PropHero를 읽어서 모든 영웅의 영혼석을 프로그램적으로 생성한다.
	const ID idSoulStart = ID_START_SOUL;
	auto& aryHeros = PROP_HERO->GetaryProp();
//	XARRAYLINEAR_LOOP_AUTO( aryHeros, pPropHero ) {
	for( auto pPropHero : aryHeros ) {
		_tstring ids = XFORMAT( "hero_%s", pPropHero->strIdentifier.c_str() );
		auto pPropItem = new xPROP;
		pPropItem->strIdentifier = ids;
		pPropItem->idProp = idSoulStart + pPropHero->idProp;
		pPropItem->type = XGAME::xIT_SOUL;
		pPropItem->grade = XGAME::xGD_COMMON;
		pPropItem->SetgoldCost( 10 );
		pPropItem->maxStack = 9999;
		pPropItem->strIdHero = pPropHero->strIdentifier;
		pPropItem->strIcon = pPropHero->strFace;
		Add( ids, pPropItem );
	}
	// 임시리스트에 받아뒀던 아이템 정보를 정적배열로 옮김
	int size = GetSize();
}
/**
 @brief 영웅아이디 idHero로 해당영웅의 영혼석 프로퍼티를 얻는다.
*/
const XPropItem::xPROP* XPropItem::GetpPropByidHero( ID idHero )
{
	const ID idSoul = sGetidHeroToidItemSoul( idHero ); // ID_START_SOUL + idHero;
	return GetpProp( idSoul );
}
/**
 @brief 아이템을 이름으로 검색한다.
*/
XPropItem::xPROP* XPropItem::GetpPropFromName( LPCTSTR szName )
{
	XBREAK( szName == nullptr );
	XBREAK( XE::IsEmpty( szName ) );
	for( auto pProp : m_aryItem ) {
		if( XE::IsSame( XTEXT( pProp->idName ), szName ) )
			return pProp;
	}
	return nullptr;
}

/**
 @brief grade아이템중에서 typeItem타입의 아이템 리스트를 얻는다.
*/
int XPropItem::GetListWithGrade( XGAME::xtGrade grade, XGAME::xtItem typeItem, XList4<xPROP*> *pOutList )
{
	for( auto pProp : m_aryItem ) {
		if( ( pProp->grade == grade || grade == XGAME::xGD_NONE )
			&& ( pProp->type == typeItem || typeItem == XGAME::xIT_NONE ) )
			pOutList->Add( pProp );
	}
	return pOutList->size();
}

/**
 @brief grade급 아이템을 랜덤으로 num개 얻어 리스트에 넣는다.
*/
int XPropItem::GetpPropRandom( XGAME::xtGrade grade, XGAME::xtItem typeItem, int num, XList4<xPROP*> *pOutList )
{
	int cnt = 0;
	XList4<xPROP*> listItems;
	// grade급 아이템을 모두 얻는다.
	GetListWithGrade( grade, typeItem, &listItems );
	for( int i = 0; i < num; ++i )
	{
		// 리스트내에서 한 아이템의 이터레이터를 꺼낸다.
		auto itor = listItems.GetItorFromRandom();
		if( itor != listItems.end() )
		{
			pOutList->Add( (*itor) );
			listItems.erase(itor++);	// 꺼낸건 삭제한다.
		}
	}
	return pOutList->size();
}

/**
 @brief 현재 레벨에 따른 등급별 장착템 가격계산.
*/
DWORD XPropItem::xPROP::GetBuyCost( int level ) const
{
// 	if( IsEquipable() ) {
// 		int lvSector = -1;
// 		// 현재 유저계정레벨에 맞춰 드랍테이블을 선택한다.
// 		int lv = level;
// 		for( int i = 0; i < XGC->m_aryDropRatePerLevel.GetMax(); ++i ) {
// 			if( lv <= XGC->m_aryDropRatePerLevel[ i ].lvEnd ) {
// 				lvSector = i;
// 				break;
// 			}
// 		}
// 		if( lvSector >= 0 ) {
// 			// 실제 이 확률로 전투를 해서 돈을 벌때를 기반하여 계산.
// 			const auto& table = XGC->m_aryDropRatePerLevel[ lvSector ];
// 			double ratio = table.aryDropRate[ grade ] / (double)XGC->m_frEquipDropMax;
// 			if( ratio == 0 )
// 				ratio = 0.00001;
// 			double valuable = 1.f / ratio;		// 가치도
// 			const DWORD goldPerCombat = 50000;	// 1회전투시 평균 금화 획득양
// 			XINT64 price = (XINT64)( valuable * goldPerCombat );
// 			if( price > 0x7fffffff )
// 				price = 0x7fffffff;
// 			price = ( (XINT64)( price / 1000 ) ) * 1000;	// 1000이하 절삭
// // 			if( price > 100000 )
// // 				price = 100000;
// 			return (DWORD)price;
// 		}
// 		XBREAK(1);
// 	} else {
		return m_numCost;
// 	}
// 	return 0;
}

DWORD XPropItem::xPROP::GetSellCost( int level ) const
{
	int cost = (int)(GetBuyCost( level ) * XGC->m_rateShopSell);
	cost = std::min( cost, 100000 );
	cost = std::max( cost, 0 );
	return cost;
}

void XPropItem::xPROP::Serialize( XArchive& ar ) const {
	XBREAK( subType1 > 0xff );
	ar << idProp << strIdentifier << idName << idDesc;
	ar << m_numCost;
	ar << cashCost;
	ar << (BYTE)type;
	ar << (BYTE)useType;
	ar << (BYTE)grade;
	ar << (BYTE)parts;

	ar << m_strPayItem;
	ar << (short)maxStack;
	ar << (char)subType1;
	ar << (BYTE)0;

	ar << strActive << strIdHero << strIcon;
	ar << (int)aryAdjParam.size();		// 최대사이즈로 넣는다.
	for( auto& adj : aryAdjParam ) {
		XBREAK( adj.adjParam > 0x7f );
		ar << (char)adj.adjParam;
		ar << (BYTE)0;
		ar << (WORD)0;
		ar << adj.param;
	}

}
void XPropItem::xPROP::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	char c0;
	short s0;
	ar >> idProp >> strIdentifier >> idName >> idDesc;
	ar >> m_numCost;
	ar >> cashCost;
	ar >> b0;		type = (xtItem)b0;
	ar >> b0;		useType = (xtItemUse)b0;
	ar >> b0;		grade = (xtGrade)b0;
	ar >> b0;		parts = (xtParts)b0;

	ar >> m_strPayItem;
	ar >> s0;		maxStack = s0;
	ar >> c0;		subType1 = c0;
	ar >> b0;

	ar >> strActive >> strIdHero >> strIcon;
	aryAdjParam.clear();
	int max;
	ar >> max;
	for( int i = 0; i < max; ++i ) {
		char c0;
		xAdj adj;
		ar >> c0;		adj.adjParam = (xtParameter)c0;
		ar >> c0;
		ar >> s0;
		ar >> adj.param;
//		ar >> s0;		adj.param = (s0 / 100.f);
		aryAdjParam.Add( adj );
	}
}

void XPropItem::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_ITEM;
	ar << (int)m_aryItem.size();
	for( auto pProp : m_aryItem ) {
		ar << (*pProp);
	}
}
void XPropItem::DeSerialize( XArchive& ar, int )
{
	DestroyProps();	
	int ver;
	int num;
	ar >> ver;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pProp = new xPROP();
		ar >> (*pProp);
		Add( pProp );
	}
}

ID XPropItem::sGetidHeroToidItemSoul( ID idPropHero )
{
	return ID_START_SOUL + idPropHero;
}