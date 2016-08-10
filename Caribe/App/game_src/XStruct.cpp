#include "stdafx.h"
#include "XStruct.h"
#include "XBaseItem.h"
#include "XHero.h"
#include "XXMLDoc.h"
#include "XProps.h"
#include "XAccount.h"
#include "XLegion.h"
#include "XSpot.h"
#include "XWorld.h"
#include "XPropLegionH.h"
#ifdef _CLIENT
#include "XGame.h"
#endif // _CLIENT

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XGAME )

/**
 @brief node블럭내의 szTag이름의 블럭들을 xRES_NUM타입으로 읽어서 pOutAry에 쌓는다.
 @param nodeRoot 검색할 루트노드
 @param szNodeName nodeRoot내에서 szNodeName이름의 노드를 찾는다.
 @param szTag 디버깅용 식별태그
 @param pOutAry szNodeName의 값을 읽어 ary에 쌓는다.
*/
int LoadResFromXMLToAry( XEXmlNode& nodeRoot, LPCTSTR szNodeName, LPCTSTR szTag, 
						std::vector<xRES_NUM> *pOutAry )
{
	auto childNode = nodeRoot.GetFirst();
	while( !childNode.IsEmpty() ) {
		if( childNode.GetstrName() == szNodeName ) {
			xRES_NUM resNum;
			resNum.LoadFromXML( childNode, szTag );
			if( resNum.IsValid() )
				pOutAry->push_back( resNum );
			else
				CONSOLE("경고:resource의 타입이 이상함.%s", szTag );
		}
		childNode = childNode.GetNext();
	}
	return pOutAry->size();
}
/**
 @brief XML 노드로부터 reward를 읽어들인다.
*/
bool xRES_NUM::LoadFromXML( XEXmlNode& node, LPCTSTR szTag )
{
	bool bRet = true;
	int idx = 0;
	XEXmlAttr attr = node.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		if( attr.GetcstrName() == "resource" 
			|| attr.GetcstrName() == "res" 
			|| attr.GetcstrName() == "type" ) {
			if( CONSTANT->IsFound() ) {
				type = (xtResource)sReadResourceConst( attr, szTag );
			} else
				bRet = false;
		} else
		if( attr.GetcstrName() == "num" ) {
			num = attr.GetFloat();
		} else {
			XBREAKF( 1, "%s:알수없는 속성이름. %s", szTag, attr.GetstrName().c_str() );
			bRet = false;
		}
		//
		attr = attr.GetNext();
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
int XGAME::xBattleLog::Serialize( XArchive& ar ) const {
	// 주의. 이것은 DB저장용으로도 사용됨
	// xDB클리어할때 ver정보 포함하도록 수정할것.
	ar << snLog;
	ar << idEnemy;
	ar << strName;
	ar << score;
	ar << addScore;
	ar << aryLoot;
	ar << powerBattle;
	ar << xboolToByte( bRevenge );		// byte
	ar << xboolToByte( bWin );
	XBREAK( m_lvAcc > 0xff );
	ar << (BYTE)m_lvAcc;
	ar << (BYTE)xboolToByte( m_bKill );
	ar << sec;
	return 1;
}
int XGAME::xBattleLog::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	ar >> snLog;
	ar >> idEnemy;
	ar >> strName;
	ar >> score;
	ar >> addScore;
	ar >> aryLoot;
	ar >> powerBattle;
	ar >> b0;	bRevenge = ( b0 != 0 );		// byte
	ar >> b0;	bWin = ( b0 != 0 );
	ar >> b0;	m_lvAcc = b0;
	ar >> b0; m_bKill = ( b0 != 0 );
	ar >> sec;
	return 1;
}
/**
 @brief 루팅자원 정보를 넣는데 이미 같은 자원종류가 있으면 거기다 더하고 없으면 새로 추가한다.
*/
int XGAME::xBattleLog::AddLootRes( XGAME::xtResource type, int add )
{
	XBREAK( XGAME::IsInvalidResource( type ) );
	for( auto& res : aryLoot ) {
		if( res.type == type ) {
			res.num += add;
			return (int)res.num;
		}
	}
	aryLoot.push_back( XGAME::xRES_NUM( type, add ) );
	return add;
}
//////////////////////////////////////////////////////////////////////////
// int XGAME::xBattleLog::xRes::Serialize( XArchive& ar ) {
// 	ar << (int)typeRes;
// 	ar << num;
// 	return 1;
// }
// int XGAME::xBattleLog::xRes::DeSerialize( XArchive& ar, int ver ) {
// 	int i0;
// 	ar >> i0;	typeRes = ( XGAME::xtResource )i0;
// 	ar >> num;
// 	return 1;
// }
//////////////////////////////////////////////////////////////////////////
bool xBattleStart::IsValid() const {
	if( XBREAK(m_spLegion[0] == nullptr || m_spLegion[1] == nullptr) )
		return false;
	if( XBREAK(!m_typeBattle || !m_Level || m_strName.empty()) )
		return false;
#ifndef _XSINGLE
	if( XBREAK(!m_typeSpot) )
		return false;
	if( XBREAK(!m_idSpot) )
		return false;
	if( m_typeSpot == xSPOT_CASTLE || m_typeSpot == xSPOT_JEWEL || m_typeSpot == xSPOT_MANDRAKE )
		if( XBREAK(m_idEnemy == 0) )
			return false;
	if( m_typeSpot == xSPOT_JEWEL )
		if( XBREAK(m_Defense == 0) )
			return false;
	if( m_Defense > 0 )
		if( XBREAK(m_typeSpot != xSPOT_JEWEL) )
			return false;
	if( m_typeSpot == xSPOT_CAMPAIGN || m_typeSpot == xSPOT_COMMON )
		if( XBREAK(m_idxStage < 0) )
			return false;
#endif // not _XSINGLE
	return true;
}
// void xBattleStart::Serialize( XArchive& ar ) const
// {
// 	XASSERT( ar.IsForDB() == false );
// 	ar << m_idEnemy;
// 	XBREAK( m_idSpot > 0xffff );
// 	ar << (WORD)m_idSpot;
// 	ar << (BYTE)m_typeSpot;
// 	XBREAK( m_Level > 0xff );
// 	ar << (BYTE)m_Level;
// 	ar << m_strName;
// 	XLegion::sSerializeFull( )
// }
// void DeSerialize( XArchive& ar, int )
// {
// 
// }
//////////////////////////////////////////////////////////////////////////
int xBattleResult::Serialize( XArchive& ar ) const {
	XBREAK( VER_ITEM_SERIALIZE > 0xff );
	XBREAK( ladder > 0xffff );
	XBREAK( idxStage > 0xff );
	XBREAK( m_numCashtem < 0 );		// 값이 입력이 안됨.
	ar << (WORD)idSpot;
	ar << (WORD)ladder;
	ar << logForAttacker;

	ar << (BYTE)typeSpot;
	ar << (BYTE)xboolToByte( bFake );
	XBREAK( m_mulByStar > 0xff );
	ar << (BYTE)m_mulByStar;
	XBREAK( m_totalStar > 0xff );
	ar << (BYTE)m_totalStar;

	ar << (BYTE)numStar;
	ar << (BYTE)idxStage;
	ar << (WORD)0;
	ar << m_powerAccIncludeEmpty;
	ar << m_powerAccExcludeEmpty;
	ar << m_numCashtem;
	do {
		{
			XArchive _arHeroes;
			_arHeroes << aryHeroes.size();
			XARRAYLINEARN_LOOP( aryHeroes, XHero*, pHero ) {
				_arHeroes << pHero->GetsnHero();
				pHero->SerializeUpgrade( _arHeroes );
			} END_LOOP;
			ar << _arHeroes;
			ar << m_aryLevelUpHeroes;
		}
		ar << aryAccRes;
		ar << arSpot;
		if( IsLose() )
			break;
		XBREAK( aryUpdated.size() > 0xff );
		XBREAK( aryDrops.size() > 0xff );
		ar << (BYTE)xboolToByte( bLevelupAcc );
		ar << (BYTE)aryDrops.size();
		ar << (BYTE)xboolToByte( bClearSpot );
		ar << (BYTE)xboolToByte( bVsPVP );
		{
			XArchive _arUpdated;
			_arUpdated << (char)aryUpdated.size();
			_arUpdated << (BYTE)VER_ITEM_SERIALIZE;
			_arUpdated << (BYTE)0;
			_arUpdated << (BYTE)0;
			XARRAYLINEARN_LOOP_AUTO( aryUpdated, pItem ) {
				XBaseItem::sSerialize( _arUpdated, pItem );
			} END_LOOP;
			ar << _arUpdated;
		}
		XARRAYLINEARN_LOOP_AUTO( aryDrops, const &itemBox ) {
			XPropItem::xPROP *pProp;
			int num;
			XGAME::GetItemBox( itemBox, &pProp, &num );
			ar << (WORD)pProp->idProp;
			XBREAK( num > 0xffff );
			ar << (short)num;
		} END_LOOP;
//		ar << arEachSpot;
	} while( 0 );
	return 1;
}
int xBattleResult::DeSerialize( XArchive& ar, int ) {
	BYTE b0;
	WORD w0;
	ar >> w0;	idSpot = w0;
	ar >> w0;	ladder = w0;
	ar >> logForAttacker;
	ar >> b0;	typeSpot = ( XGAME::xtSpot )b0;
	ar >> b0;	bFake = xbyteToBool( b0 );
//	ar >> w0;	//ptBrave = w0;
	ar >> b0;	m_mulByStar = b0;
	ar >> b0;	m_totalStar = b0;
	ar >> b0;	numStar = b0;
	ar >> b0;	idxStage = b0;
	ar >> b0 >> b0;
	ar >> m_powerAccIncludeEmpty;
	ar >> m_powerAccExcludeEmpty;
	ar >> m_numCashtem;
	do {
		ar >> arHeroes;
		ar >> m_aryLevelUpHeroes;
		ar >> aryAccRes;
		ar >> arSpot;
		int sizeDrops;
		if( IsLose() )
			break;
		ar >> b0;	bLevelupAcc = xbyteToBool( b0 );
		ar >> b0;	sizeDrops = b0;
		ar >> b0;	bClearSpot = xbyteToBool( b0 );
		ar >> b0;	bVsPVP = xbyteToBool( b0 );
		ar >> arUpdated;
		for( int i = 0; i < sizeDrops; ++i ) {
			short sw0;
			ar >> w0;  ID idProp = w0;
			ar >> sw0;  int num = sw0;
			auto pProp = PROP_ITEM->GetpProp( idProp );
			if( XASSERT( pProp ) ) {
				ItemBox itembox = std::make_pair( pProp, num );
				aryDrops.Add( itembox );
			}
		}
//		ar >> arEachSpot;
	} while( 0 );
	return 1;
}
/**
 @brief arHeroes의 영웅하나의 deserialize
*/
int xBattleResult::DeSerializeHeroes( XArchive& ar, XSPAcc spAcc )
{
	int size;
	ar >> size;
	ID snHero;
	for( int i = 0; i < size; ++i ) {
		ar >> snHero;
		XHero *pHero = spAcc->GetHero( snHero );
		if( XASSERT(pHero) )
			pHero->DeSerializeUpgrade( ar );
		else
			return 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
void xBattleFinish::Serialize( XArchive& ar )
{
	XBREAK( ar.IsForDB() );	// 저장엔 사용되지 않는다(아마도?)
	XBREAK( idSpot == 0 );
	if( !bCheatKill ) {	// 치트가 아닐때 세션키가 없으면 안됨.
		XBREAK( snSession == 0 );
	}
	ar << idSpot;
	ar << snSession;
//	ar << idEnemy;
	XBREAK( ebCode > 0xff );
	XBREAK( bitWinner > 0xff );
	XBREAK( idxStage > 0xff );
	ar << (BYTE)ebCode;
	ar << (BYTE)bitWinner;
	ar << (BYTE)idxStage;
	ar << (BYTE)bRunAwaySulfur;

	ar << (BYTE)bCheatKill;
	if( secPlay > 0xff )
		secPlay = 0xff;
	ar << (BYTE)secPlay; 
//	XBREAK( ptBrave > 0xffff );
//	ar << (WORD)ptBrave;
	ar << (WORD)0;
	ar << arLegion[ 0 ];
	ar << arLegion[ 1 ];
	ar << m_rateHpAlly;
	ar << m_rateHpEnemy;
}
void xBattleFinish::DeSerialize( XArchive& ar, int )
{
	BYTE b0;
	ar >> idSpot;
	ar >> snSession;
//	ar >> idEnemy;
	ar >> b0;	ebCode = (XGAME::xtExitBattle)b0;
	ar >> b0;	bitWinner = (XGAME::xtSide)b0;
	ar >> b0;	idxStage = b0;
	ar >> b0;	bRunAwaySulfur = xbyteToBool( b0 );

	ar >> b0;	bCheatKill = xbyteToBool( b0 );
	ar >> b0;	secPlay = b0;
	WORD w0;
	ar >> w0;	//ptBrave = w0;
	ar >> arLegion[0];
	ar >> arLegion[1];
	ar >> m_rateHpAlly;
	ar >> m_rateHpEnemy;
}

/**
 @brief 아이템 식별자 읽는 공통 모듈
*/
XPropItem::xPROP* sReadItemIdentifier( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return nullptr;
	XPropItem::xPROP *pProp = PROP_ITEM->GetpProp( strConst );
	if( pProp == nullptr ) {
		CONSOLE( "%s:%s라는 아이템은 없습니다.", szTag, strConst.c_str() );
	}
	return pProp;
}

ID sReadItemIdsToId( XEXmlNode& node, const char *cKey, LPCTSTR szNodeName )
{
	_tstring strIds = node.GetTString( cKey );
	if( !strIds.empty() ) {
		auto pProp = PROP_ITEM->GetpProp( strIds );
		if( XASSERT( pProp ) ) {
			return pProp->idProp;
		} else {
			CONSOLE( "%s:drop item=%s 라는 아이템은 없음.", szNodeName, strIds.c_str() );
		}
	}
	else {
		CONSOLE( "%s:drop item이 정해지지 않았다.", szNodeName );
	}
	return 0;
}

DWORD sReadResourceConst( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return 0;
	DWORD dwValue = CONSTANT->GetValue( strConst );
	if( CONSTANT->IsFound() == FALSE )
		CONSOLE( "%s:%s라는 자원은 없습니다.", szTag, strConst.c_str() );
	return dwValue;
}

DWORD sReadSpotConst( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return 0;
	DWORD dwValue = CONSTANT->GetValue( strConst );
	if( CONSTANT->IsFound() == FALSE )
		CONSOLE( "%s:%s라는 스팟종류는 없습니다.", szTag, strConst.c_str() );
	return dwValue;
}

DWORD sReadConst( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() || strConst == _T("0"))
		return 0;
	DWORD dwValue = CONSTANT->GetValue( strConst );
	if( CONSTANT->IsFound() == FALSE )
		CONSOLE( "%s:%s라는 상수는 없습니다.", szTag, strConst.c_str() );
	return dwValue;
}

XPropHero::xPROP* sReadHeroIdentifier( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return nullptr;
	if( strConst == _T("none") )
		return nullptr;
	auto pProp = PROP_HERO->GetpProp( strConst );
	if( pProp == nullptr )
		CONSOLE( "%s:%s라는 영웅은 없습니다.", szTag, strConst.c_str() );
	return pProp;
}

XPropCloud::xCloud* sReadAreaIdentifier( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return nullptr;
	auto pCloud = PROP_CLOUD->GetpProp( strConst );
	if( pCloud == nullptr )
		CONSOLE( "%s:%s라는 지역은 없습니다.", szTag, strConst.c_str() );
	return pCloud;
}

XPropWorld::xBASESPOT* sReadSpotIdentifier( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return nullptr;
	auto pSpot = PROP_WORLD->GetpProp( strConst );
	if( pSpot == nullptr )
		CONSOLE( "%s:%s라는 스팟은 없습니다.", szTag, strConst.c_str() );
	return pSpot;
}

XGAME::xtUnit sReadUnit( XEXmlAttr& attr, LPCTSTR szTag )
{
	_tstring strConst = attr.GetTString();
	if( strConst.empty() )
		return XGAME::xUNIT_NONE;
	DWORD dwValue = CONSTANT->GetValue( strConst );
	if( CONSTANT->IsFound() == FALSE )
		CONSOLE( "%s:%s라는 유닛은 없습니다.", szTag, strConst.c_str() );
	if( XGAME::IsInvalidUnit( (XGAME::xtUnit)dwValue) )
		CONSOLE( "%s:잘못된 유닛 define%s", szTag, strConst.c_str() );
	return (XGAME::xtUnit)dwValue;
}

#define CHECK_ONLY_ONE( NAME, CSTR) \
	if( idx++ > 0 ) { \
		XBREAK(1); \
		CONSOLE( "%s조건은 1개만 가능합니다.", NAME, _T(CSTR) ); \
		return false;\
	}
//////////////////////////////////////////////////////////////////////////

void xBattleStartInfo::SetSpotData( XSpot *pBaseSpot )
{
	m_typeSpot = pBaseSpot->GettypeSpot();
	m_Level = pBaseSpot->GetLevel();
	m_idSpot = pBaseSpot->GetidSpot();
	m_strName = pBaseSpot->GetstrName();
	m_Power = pBaseSpot->GetPower();
	m_Ladder = pBaseSpot->GetScore();
}

void xBattleStartInfo::Serialize( XArchive& ar, XSpot *pBaseSpot ) const {
	XBREAK( ar.IsForDB() );
	XBREAK( m_typeSpot > 0xff );
	XBREAK( m_Level > 0xff );
	XBREAK( m_idSpot > 0xffff );
	XBREAK( m_typeBattle > 0xff );
	XBREAK( m_idxStage > 0x7f || m_idxStage < -1 );
	XBREAK( m_AP > 0xffff );
	XBREAK( m_apMax > 0xffff );
	XBREAK( !m_bRecon && m_snSession == 0 );	// 전투는 상대가 npc든 플레이어든 세션아이디는 무조건 있어야 함.
	if( pBaseSpot && pBaseSpot->GetspLegion() == nullptr ) {
		XBREAK( !m_bRecon && m_spLegion == nullptr );		// 공격인데 스팟과 여기 양쪽에 군단정보가 없는경우는 없을듯.
	}
//	XBREAK( !m_bRecon && m_spLegion == nullptr );		// << 이런경우가 있음?(스팟공격시 m_spLegion,은 null이라도 pSpot에 군단정보가 들어있음)
	ar << (DWORD)0;		// reserved
	ar << (BYTE)m_typeSpot;
	ar << (BYTE)m_Level;
	ar << (WORD)m_idSpot;
	ar << m_snSession;
	ar << m_idEnemy;
	ar << m_strName;
	ar << (BYTE)m_typeBattle;
	ar << (char)m_idxStage;
	ar << (BYTE)xboolToByte( m_bRecon );
	ar << (char)xboolToByte(m_bInitSpot);
	XLegion::sSerializeFull( ar, m_spLegion );
	ar << (WORD)m_AP;
	ar << (WORD)m_apMax;
	ar << m_Power;
	ar << m_Ladder;
	XSpot::sSerialize( ar, pBaseSpot );
	MAKE_CHECKSUM( ar );
	ar << m_arParam;

}
//void xBattleStartInfo::DeSerialize( XArchive& ar, XSpot *pBaseSpot, int ) {
void xBattleStartInfo::DeSerialize( XArchive& ar, XWorld *pWorld, int ) {
	BYTE b0;
	char c0;
	WORD w0;
	DWORD dw0;
	ar >> dw0;
	ar >> b0;	m_typeSpot = (XGAME::xtSpot)b0;
	ar >> b0;	m_Level = b0;
	ar >> w0;	m_idSpot = w0;
	ar >> m_snSession;
	ar >> m_idEnemy;
	ar >> m_strName;
	ar >> b0;	m_typeBattle = (xtBattle)b0;
	ar >> c0;	m_idxStage = c0;
	ar >> b0;	m_bRecon = xbyteToBool(b0);
	ar >> c0;	m_bInitSpot = xbyteToBool(c0);
	m_spLegion = LegionPtr( XLegion::sCreateDeserializeFull( ar ) );
	ar >> w0;	m_AP = w0;
	ar >> w0;	m_apMax = w0;
	ar >> m_Power;
	ar >> m_Ladder;
	auto pBaseSpot = pWorld->GetSpot( m_idSpot );
	XBREAK( m_idSpot && pBaseSpot == nullptr );	// 아이디가 있는데 못찾는경우는 없어야함.
	XSpot::sDeSerialize( ar, pBaseSpot );
	if( pBaseSpot && pBaseSpot->GetspLegion() == nullptr ) {
		XBREAK( !m_bRecon && m_spLegion == nullptr );
	}
	RESTORE_VERIFY_CHECKSUM_NO_RETURN(ar);
	ar >> m_arParam;
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief XML 노드로부터 reward를 읽어들인다.
*/
xReward::xReward( XHero* pHero ) {
	XBREAK( pHero == nullptr );
	SetHero( pHero->GetidProp(), 0 );
}

int xReward::sLoadResFromXMLToAry( XEXmlNode& nodeRoot, LPCTSTR szNodeName, LPCTSTR szTag,
																	std::vector<xReward> *pOutAry )
{
	auto childNode = nodeRoot.GetFirst();
	while( !childNode.IsEmpty() ) {
		if( childNode.GetstrName() == szNodeName ) {
			xReward reward;
			reward.LoadFromXML( childNode, szTag );
			if( reward.IsValid() )
				pOutAry->push_back( reward );
			else {
				CONSOLE( "경고:reward type이 이상함.%s", szTag );
			}
		}
		childNode = childNode.GetNext();
	}
	return pOutAry->size();
}


bool xReward::LoadFromXML( XEXmlNode& nodeReward, LPCTSTR szTag )
{
	bool bRet = true;
	int idx = 0;
	XEXmlAttr attr = nodeReward.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		if( attr.GetcstrName() == "item" ) {
			CHECK_ONLY_ONE( szTag, "reward" );
			auto pProp = sReadItemIdentifier( attr, szTag );
			if( pProp ) {
				rewardType = xtReward::xRW_ITEM;
				idReward = pProp->idProp;
			} else
				bRet = false;
		} else
		if( attr.GetcstrName() == "resource" || attr.GetcstrName() == "res" ) {
			CHECK_ONLY_ONE( szTag, "reward" );
			DWORD val = sReadResourceConst( attr, szTag );
			if( CONSTANT->IsFound() ) {
				rewardType = xtReward::xRW_RESOURCE;
				idReward = val;
			} else
				bRet = false;
		} else
		if( attr.GetcstrName() == "cash" ) {
			CHECK_ONLY_ONE( szTag, "reward" );
			rewardType = xtReward::xRW_CASH;
			int n = attr.GetInt();
			if( n != 0 )
				num = n;
		} else
		if( attr.GetcstrName() == "hero" ) {
			CHECK_ONLY_ONE( szTag, "reward" );
			auto pProp = sReadHeroIdentifier( attr, szTag );
			if( pProp ) {
				rewardType = xtReward::xRW_HERO;
				idReward = pProp->idProp;
			} else
				bRet = false;
		} else
		if( attr.GetcstrName() == "num" ) {
			num = attr.GetInt();
		} else
		if( attr.GetcstrName() == "gold" ) {
			rewardType = xtReward::xRW_GOLD;
			int n = attr.GetInt();
			if( n )
				num = n;
		} else
		if( attr.GetcstrName() == "guild_coin" ) {
			rewardType = xtReward::xRW_GUILD_POINT;
			int n = attr.GetInt();
			if( n )
				num = n;
		} else
		if( attr.GetcstrName() == "rate" ) {
			rateDrop = attr.GetFloat();
		} else
		if( attr.GetcstrName() == "param" ) {
			dwParam = (DWORD)attr.GetInt();
		} else {
			XBREAKF( 1, "%s:알수없는 속성이름. %s", szTag, attr.GetstrName().c_str() );
			bRet = false;
		}
		//
		attr = attr.GetNext();
	}
	return bRet;
}

/**
 @brief nodeRoot노드에 cNodeName이름의 보상노드를 만들고 보상attr을 생성한다.
 @param cNodeName 보상노드의 이름. 지정하지 않으면 "reward"로 기본지정된다.
 @param reward노드가 생길 상위 노드
*/
bool xReward::SaveXML( XEXmlNode& nodeRoot, LPCTSTR szTag, const std::string& _strcNodeName ) const
{
	const std::string strcNodeName = (_strcNodeName.empty())? "reward" : _strcNodeName;
	auto reward = nodeRoot.AddNode( strcNodeName );
	switch( rewardType )
	{
	case XGAME::xtReward::xRW_ITEM: {
		auto pProp = PROP_ITEM->GetpProp( idReward );
		if( XASSERT(pProp) ) {
			reward.AddAttribute( "item", pProp->strIdentifier );
		}
	} break;
	case XGAME::xtReward::xRW_GOLD:
		reward.AddAttribute( "gold", num );
		break;
	case XGAME::xtReward::xRW_CASH:
		reward.AddAttribute( "cash", num );
		break;
	case XGAME::xtReward::xRW_RESOURCE: {
		const std::string strEnum = SZ2C( XGAME::GetEnumResource( (xtResource)idReward ) );
		if( !strEnum.empty() )
			reward.AddAttribute( "res", strEnum );
	} break;
	case XGAME::xtReward::xRW_HERO: {
		auto pProp = PROP_HERO->GetpProp( idReward );
		if( XASSERT(pProp) ) {
			reward.AddAttribute( "hero", pProp->strIdentifier );
		}
	} break;
	case XGAME::xtReward::xRW_GUILD_POINT:
		reward.AddAttribute( "guild_coin", num );
		break;
	default:
		XBREAK(1);
		return false;
	}
	if( num )
		reward.AddAttribute( "num", num );
	if( rateDrop != 1.f )
		reward.AddAttribute( "rate", rateDrop );
	if( dwParam )
		reward.AddAttribute( "param", dwParam );
	return true;
}


int xReward::Serialize( XArchive& ar ) const
{
	ar << (char)rewardType;
	ar << (char)0;
	ar << (short)0;

	ar << num;
	ar << idReward;
	ar << dwParam;
	ar << rateDrop;
	return 1;
}
int xReward::DeSerialize( XArchive& ar, int )
{
	char c0;
	short s0;
	ar >> c0;	rewardType = (xtReward)c0;
	ar >> c0;
	ar >> s0;	
	ar >> num;
	ar >> idReward;
	ar >> dwParam;
	ar >> rateDrop;
	return 1;
}
//////////////////////////////////////////////////////////////////////////
int xSquad::Serialize( XArchive& ar ) const {
	XBREAK( ar.IsForDB() );	// DB저장용으로는 사용되지 않는다.
//	XBREAK( XGAME::IsInvalidGrade( grade ) );
	ar << idHero;
	ar << (char)idxPos;
	ar << (BYTE)unit;
	ar << (char)lvSquad;
	ar << (char)lvSkill;
	//
	ar << (char)lvHero;
	ar << (char)adjLvHero;
	ar << (BYTE)grade;
	ar << (BYTE)sizeUnit;
	//
	ar << (char)atkType;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	//
	ar << mulAtk;
	ar << mulHp;
	return 1;
}
int xSquad::DeSerialize( XArchive& ar, int ver ) {
	BYTE b0;
	char c0;
	ar >> idHero;
	ar >> c0;	idxPos = c0;
	ar >> b0;	unit = ( XGAME::xtUnit )b0;
	ar >> c0;	lvSquad = c0;
	ar >> c0;	lvSkill = c0;
	//
	ar >> c0;	lvHero = c0;
	ar >> c0;	adjLvHero = c0;
	ar >> b0; grade = (XGAME::xtGrade)b0;
	ar >> b0;	sizeUnit = (xtSize)b0;
	//
	ar >> c0;	atkType = (xtAttack)c0;
	ar >> c0;
	ar >> c0;
	ar >> c0;
	//
	ar >> mulAtk;
	ar >> mulHp;
	return 1;
}
bool xSquad::LoadFromXML( XEXmlNode& nodeSquad, LPCTSTR szTag )
{
	bool bRet = true;
	int idx = 0;
	XEXmlAttr attr = nodeSquad.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
		if( attr.GetcstrName() == "idx_pos" || attr.GetcstrName() == "pos") { // 하위호환성 유지를 위해 pos도 넣음.
			idxPos = attr.GetInt();
			XBREAK( idxPos < -1 || idxPos >= XGAME::MAX_SQUAD );	// 디폴트값(-1)을 직접 넣을수 있도록 함.
		} else
		if( attr.GetcstrName() == "hero" ) {
			auto pProp = sReadHeroIdentifier( attr, szTag );
			if( pProp ) {
				idHero = pProp->idProp;
			}
		} else
		if( attr.GetcstrName() == "type_atk" ) {
			atkType = (XGAME::xtAttack)sReadConst( attr, szTag );
			XBREAK( atkType < 0 || atkType >= xAT_MAX );
		} else
		if( attr.GetcstrName() == "lv_hero" ) {
			lvHero = attr.GetInt();
		} else
		if( attr.GetcstrName() == "adj_level" ) {
			adjLvHero = attr.GetInt();
		} else
		if( attr.GetcstrName() == "grade" ) {
			grade = (XGAME::xtGrade)sReadConst( attr, szTag );
			XBREAK( grade < 0 || grade >= xGD_MAX );
		} else
		if( attr.GetcstrName() == "unit" ) {
			unit = sReadUnit( attr, szTag );
		} else
		if( attr.GetcstrName() == "size_unit" ) {
			sizeUnit = (xtSize)sReadConst( attr, szTag );
		} else
		if( attr.GetcstrName() == "lv_squad" ) {
			lvSquad = attr.GetInt();
		} else
// 		if( attr.GetcstrName() == "num_unit" ) {
// 			numUnit = attr.GetInt();
// 		} else
		if( attr.GetcstrName() == "lv_skill" ) {
			lvSkill = attr.GetInt();
		} else
		if( attr.GetcstrName() == "mul_atk" ) {
			mulAtk = attr.GetFloat();
			if( mulAtk <= 0.01f )		// 이하값은 디폴트값으로 침
				mulAtk = 1.f;
		} else
		if( attr.GetcstrName() == "mul_hp" ) {
			mulHp = attr.GetFloat();
			if( mulHp <= 0.01f )
				mulHp = 1.f;
		} else 
#ifdef _XSINGLE
		if( attr.GetcstrName() == "show" ) {
			m_bShow = attr.GetBool();
		} else
		if( attr.GetcstrName() == "num_abil" ) {
			m_numAbil = attr.GetInt();
		} else
#endif // _XSINGLE
#ifdef _DEBUG
		if( attr.GetcstrName() == "debug" ) {
			debug = attr.GetInt();
		} else 
#endif // _DEBUG
		{
			XBREAKF( 1, "%s:알수없는 속성이름. %s", szTag, attr.GetstrName().c_str() );
			bRet = false;
		}
		//
		attr = attr.GetNext();
	}
	auto nodeAbilRoot = nodeSquad.FindNode( "abil" );
	if( nodeAbilRoot.IsHave() ) {
		auto nodeAbil = nodeAbilRoot.GetFirst();
		while( nodeAbil.IsHave() ) {
			xAbil2 abil;
			abil.m_idsAbil = C2SZ( nodeAbil.GetcstrName() );
			abil.point = nodeAbil.GetInt( "point" );
			m_listAbil.Add( abil );
// 			auto pAbil = XPropTech::sGet()->GetpNode( idsAbil );
// 			if( pAbil ) {
// 			}
			nodeAbil = nodeAbil.GetNext();
		}
	}
	return bRet;
}
bool xSquad::SaveXML( XEXmlNode& nodeLegion, const std::string& strName )
{
	auto nodeSquad = nodeLegion.AddNode( strName );
	nodeSquad.AddAttribute( "idx_pos", idxPos );
	{
		auto pPropHero = PROP_HERO->GetpProp( idHero );
		if( pPropHero ) {
			nodeSquad.AddAttribute( "hero", pPropHero->strIdentifier );
		}
	}
	nodeSquad.AddAttribute( "type_atk", XGAME::GetstrEnumTypeAtk( atkType) );
	nodeSquad.AddAttribute( "lv_hero", lvHero );
	nodeSquad.AddAttribute( "grade", XGAME::GetstrEnumGrade(grade) );
	nodeSquad.AddAttribute( "unit", XGAME::GetstrEnumUnit(unit) );
	nodeSquad.AddAttribute( "size_unit", XGAME::GetstrEnumSizeUnit(sizeUnit) );
	nodeSquad.AddAttribute( "lv_squad", lvSquad );
	nodeSquad.AddAttribute( "lv_skill", lvSkill );
	nodeSquad.AddAttribute( "mul_atk", mulAtk );
	nodeSquad.AddAttribute( "mul_hp", mulHp );
	//
	if( m_listAbil.size() ) {
		auto nodeAbilRoot = nodeSquad.AddNode( "abil" );
		for( auto& abil : m_listAbil ) {
			auto nodeAbil = nodeAbilRoot.AddNode( SZ2C(abil.m_idsAbil) );
			nodeAbil.AddAttribute( "point", abil.point );
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
int xLegion::Serialize( XArchive& ar ) const {
	XBREAK( ar.IsForDB() );	// DB저장용으로는 사용되지 않는다.
	XBREAK( gradeLegion > 0xff );
	ar << strIds;
	ar << idBoss;
	ar << (BYTE)numSquad;
	ar << (BYTE)lvLegion;
	ar << (BYTE)gradeLegion;
	ar << (char)adjLvLegion;
	ar << squadDefault;
	ar << arySquads;
	return 1;
}
int xLegion::DeSerialize( XArchive& ar, int ver ) {
	BYTE b0;
	char c0;
	_tstring tstrIds;
	ar >> tstrIds;	strIds = SZ2C(tstrIds);
	ar >> idBoss;
	ar >> b0;	numSquad = b0;
	ar >> b0;	lvLegion = b0;
	ar >> b0;	gradeLegion = (XGAME::xtGradeLegion)b0;
	ar >> c0;	adjLvLegion;
	ar >> squadDefault;
	ar >> arySquads;
	return 1;
}
bool xLegion::LoadFromXML( XEXmlNode& nodeLegion, LPCTSTR szTag )
{
//	snLegion = XE::GenerateID();
	bool bRet = true;
	int idx = 0;
	XEXmlAttr attr = nodeLegion.GetFirstAttribute();
	while( !attr.IsEmpty() ) {
// 		if( attr.GetcstrName() == "sn" ) {
// 			std::string str = attr.GetString();
// 			snLegion = strtoul( str.c_str(), nullptr, 16 );
// 		} else
		if( attr.GetcstrName() == "name" ) {
			idName = attr.GetInt();
		} else
		if( attr.GetcstrName() == "level" ) {
			lvLegion = attr.GetInt();
		} else
		if( attr.GetcstrName() == "adj_level" ) {
			adjLvLegion = attr.GetInt();
		} else
		if( attr.GetcstrName() == "grade" ) {
			gradeLegion = (XGAME::xtGradeLegion)sReadConst( attr, szTag );
		} else
		if( attr.GetcstrName() == "lv_limit" ) {
			lvLimit = attr.GetInt();
		} else
		if( attr.GetcstrName() == "num_squad" ) {
			numSquad = attr.GetInt();
		} else
		if( attr.GetcstrName() == "boss" ) {
			auto pProp = sReadHeroIdentifier( attr, szTag );
			if( pProp ) {
				idBoss = pProp->idProp;
			}
		} else
		if( attr.GetcstrName() == "mul_atk" ) {
			mulAtk = attr.GetFloat();
			XBREAK( mulAtk <= 0 );
		} else
		if( attr.GetcstrName() == "mul_hp" ) {
			mulHp = attr.GetFloat();
			XBREAK( mulHp <= 0 );
		} else {
			XBREAKF( 1, "%s:알수없는 속성이름. %s", szTag, attr.GetstrName().c_str() );
			bRet = false;
		}
		//
		attr = attr.GetNext();
	} // attrs
	// squad default
	auto nodeSquadDefault = nodeLegion.FindNode( "squad_default" );
	if( nodeSquadDefault.IsHave() )
		squadDefault.LoadFromXML( nodeSquadDefault, szTag );
	// squad nodes
	auto nodeSquad = nodeLegion.GetFirst();
	while( !nodeSquad.IsEmpty() ) {
		if( nodeSquad.GetcstrName() == "squad" ) {
			xSquad squad = squadDefault;
			if( squad.LoadFromXML( nodeSquad, szTag ) ) {
				// 유닛은 지정안되어 있을경우. 디폴트정보에서 쓴다.
				if( !squad.unit && squadDefault.unit )
					squad.unit = squadDefault.unit;
				if( !squad.idHero )
					squad.idHero = squadDefault.idHero;
				arySquads.Add( squad );
			}
		}
		//
		nodeSquad = nodeSquad.GetNext();
	}
	// adj_level나 levelLegion 둘중 하나는 값이 있어야 한다.
	XBREAK( lvLegion == 0 && adjLvLegion == 0x7f );
	return bRet;
}

bool xLegion::SaveXML( XEXmlNode& nodeRoot )
{
// 	XBREAK(1);		// 현재 save하면 원래 식별자나 주석 다 날아가서 일단 안씀.
	XBREAK( m_idProp == 0 );
	auto nodeLegion = nodeRoot.AddNode( strIds );
// 	std::string str = XE::Format("0x%8x", idProp );
// 	nodeLegion.AddAttribute( "sn", str );
	nodeLegion.AddAttribute( "name", idName );
	nodeLegion.AddAttribute( "level", lvLegion );
	nodeLegion.AddAttribute( "grade", XGAME::GetstrEnumGradeLegion( gradeLegion ) );
	nodeLegion.AddAttribute( "lv_limit", lvLimit );
	nodeLegion.AddAttribute( "adj_level", adjLvLegion );
	nodeLegion.AddAttribute( "num_squad", numSquad );
	{
		auto pPropHero = PROP_HERO->GetpProp( idBoss );
		if( pPropHero ) {
			nodeLegion.AddAttribute( "boss", pPropHero->strIdentifier );
		}
	}
	nodeLegion.AddAttribute( "mul_atk", mulAtk );
	nodeLegion.AddAttribute( "mul_hp", mulHp );
	squadDefault.SaveXML( nodeLegion, "squad_default" );
	for( auto& squad : arySquads ) {
		squad.SaveXML( nodeLegion, "squad" );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _CLIENT
// void xResearch::Start() 
// {
// 	XBREAK( m_snHero == 0 );
// 	XBREAK( XGAME::IsInvalidUnit(unit) );
// 	XBREAK( m_idAbil == 0 );
// 	XBREAK( m_secTotal == 0 );
// 	// 클라기준 시간으로 바꿈.
// 	m_timer.DoStart( m_timer.GetsecStart() );
// }
#else
void xResearch::Start( ID snHero, XGAME::xtUnit unit, ID idAbil, xSec secResearchComplete ) 
{
	XBREAK( m_idAbil != 0 );		// 연구가끝나면 0으로 초기화 시켜야함.
	m_snHero = snHero;
	XBREAK( XGAME::IsInvalidUnit(unit) );
	m_unit = unit;
	m_idAbil = idAbil;
	m_timer.DoStart();
	m_secTotal = secResearchComplete;
}
#endif // not _CLIENT

int xResearch::Serialize( XArchive& ar ) const 
{
	ar << m_snHero;
	XBREAK( m_idAbil > 0xffff );
	ar << (BYTE)m_unit;
	ar << (BYTE)0;
	ar << (WORD)m_idAbil;
	ar << m_secTotal;
	m_timer.Serialize( ar );
	return 1;
}
int xResearch::DeSerialize( XArchive& ar, int )
{
	BYTE b0;
	WORD w0;
	ar >> m_snHero;
	ar >> b0;		m_unit = (XGAME::xtUnit)b0;
	ar >> b0;
	ar >> w0;		m_idAbil = w0;
	ar >> m_secTotal;
	// Deserialize내부에서 클라기준시간으로 바꿔서 타이머를 세팅함.
	m_timer.DeSerialize( ar, 0 );
	return 1;
}
//////////////////////////////////////////////////////////////////////////
void xEncounter::Serialize( XArchive& ar ) const
{
	XBREAK( m_idSpot > 0xffff );
	ar << (BYTE)VER;
	ar << (BYTE)0;
	ar << (WORD)m_idSpot;
	ar << m_numSulfur;
	ar << m_idDefender;
	ar << m_secEncount;
	ar << m_strDefender;
}
void xEncounter::DeSerialize( XArchive& ar, int )
{
	BYTE b0;
	WORD w0;
	ar >> b0;	int ver = b0;
	ar >> b0;
	ar >> w0; m_idSpot = w0;
	ar >> m_numSulfur;
	ar >> m_idDefender;
	ar >> m_secEncount;
	ar >> m_strDefender;
}

//////////////////////////////////////////////////////////////////////////
void xJewelInfo::Serialize( XArchive& ar ) const
{
	XBREAK( ar.IsForDB() );
	XBREAK( m_idxJewel > 0xff );
	XBREAK( m_LevelMine > 0xff );
	XBREAK( m_Defense > 0xffff );
	ar << (char)m_idxJewel;
	ar << (BYTE)m_LevelMine;
	ar << (WORD)m_Defense;
	ar << m_idOwner;
	ar << m_idMatchEnemy;
	ar << m_strName;
}
void xJewelInfo::DeSerialize( XArchive& ar, int )
{
	char c0;
	WORD w0;
	ar >> c0;	m_idxJewel = c0;
	ar >> c0;	m_LevelMine = c0;
	ar >> w0;	m_Defense = w0;
	ar >> m_idOwner;
	ar >> m_idMatchEnemy;
	ar >> m_strName;
}
//////////////////////////////////////////////////////////////////////////
void xJewelMatchEnemy::Serialize( XArchive& ar )
{
	if( m_idAcc ) {		// 상대를 못찾는 경우도 있으므로.
		XBREAK( m_strName.empty() );
		XBREAK( m_lvAcc == 0 );
		XBREAK( m_Power == 0 );
//		XBREAK( m_Ladder == 0 );
	}
//	XBREAK( m_idxMine < 1 || m_idxMine > 2 );
	XBREAK( m_lvAcc > 0xff );
	XBREAK( m_LvMine > 0xff );
	XBREAK( m_Defense > 0xffff );
	XBREAK( m_idxLegion > 0xff );
	ar << m_idAcc;
	ar << m_strName;
	ar << m_strFbUserId;
	ar << m_strHello;
	ar << (BYTE)m_lvAcc;
	ar << (BYTE)m_LvMine;
	ar << (WORD)m_Defense;
	ar << m_Power;
	ar << m_Ladder;
	ar << (char)m_idxLegion;
//	ar << (char)m_idxMine;
	ar << (char)xboolToByte(m_bDummy);
	ar << (char)0;
	ar << (char)0;
	ar << m_lootJewel;
}
void xJewelMatchEnemy::DeSerialize( XArchive& ar, int ver )
{
	char c0;
	BYTE b0;
	WORD w0;
	ar >> m_idAcc;
	ar >> m_strName;
	ar >> m_strFbUserId;
	ar >> m_strHello;
	ar >> b0;		m_lvAcc = b0;
	ar >> b0;		m_LvMine = b0;
	ar >> w0;		m_Defense = w0;
	ar >> m_Power;
	ar >> m_Ladder;
	ar >> c0;		m_idxLegion = c0;
	ar >> c0;		m_bDummy = (c0 != 0);
	ar >> c0 >> c0;
	ar >> m_lootJewel;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _CLIENT
XE::VEC2 xFocusMng::GetCurrFocus()
{
	if( m_Timer.IsOff() )
		return XE::VEC2( 0 );
	float lerpTime = m_Timer.GetSlerp();
	if( lerpTime > 1.f )
		lerpTime = 1.f;
	float lerp = XE::xiHigherPowerDeAccel( lerpTime, 1.f, 0 );
	if( lerpTime == 1.f ) {
		m_Timer.Off();
		// 일단은 그냥 이렇게. 나중에 코드 독립적으로 만든다.
		GAME->DispatchEventToSeq( xHelp::xHE_END_CAMERA_MOVING );
	}
	return vStart + ( vEnd - vStart ) * lerp;
}
// _sec==0이면 자동시간계산.
void xFocusMng::DoMove( const XE::VEC2& _vStart, const XE::VEC2& _vEnd, float _sec )
{
	vStart = _vStart;
	vEnd = _vEnd;
	float len = ( vEnd - vStart ).Length();
	if( _sec == 0 ) {
		float sec = len / 512.f;
		m_Timer.Set( sec );
	}
	else
		m_Timer.Set( _sec );
}
#endif // _CLIENT



XE_NAMESPACE_END;

