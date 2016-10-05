#include "stdafx.h"
#include "XSpotPrivateRaid.h"
#include "XSpots.h"
#include "XSpotDaily.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
#include "XGame.h"
#endif
#ifdef _GAME_SERVER
	#include "XGameUser.h"
#endif
#include "XPropUser.h"
#include "XLegion.h"
#include "XPropHero.h"
#include "XWorld.h"
#include "XPropItem.h"
//#include "XCampObj.h"
#include "XStageObj.h"
#include "XGuild.h"
#include "XGlobalConst.h"
#include "XSystem.h"
#ifdef _SERVER
#include "server/XGuildMgr.h"
#endif // _SERVER
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//using namespace xCampaign;
using namespace XGAME;
/**
 @brief Deserialize용
 @param typeSpot 지금은 구조상 외부에서 받아야하는데 Deserialize안에서 typeSpot을 읽도록 바꿔서 외부에서 받지 않아도 되도록 해야할듯.
*/
XSpot::XSpot( XWorld *pWorld, XGAME::xtSpot typeSpot )
	: m_aryLoots( XGAME::xRES_MAX )
{
	Init();
	m_pWorld = pWorld;
	m_typeSpot = typeSpot;
}
XSpot::XSpot( XPropWorld::xBASESPOT *pProp, XGAME::xtSpot typeSpot, XWorld *pWorld, XDelegateSpot *pDelegate )
	: m_aryLoots( XGAME::xRES_MAX ) 
{
	Init();
	XBREAK( pProp == nullptr );	// 이제 여기는 유효한 값만 와야함.
	m_pWorld = pWorld;
	m_snSpot = XE::GenerateID();
	m_pDelegate = pDelegate;
	_m_pBaseProp = pProp;
//	m_keyProp = PROP_WORLD->GetidKey();
	m_idProp = pProp->idSpot;
	m_typeSpot = pProp->GetType();
}

// XPropWorld::xBASESPOT* XSpot::GetpBaseProp()
// {
// //  XBREAK( _m_pBaseProp == nullptr );
// 	if( m_keyProp != PROP_WORLD->GetidKey() ) {
// //		XBREAK( m_idProp == 0 );
// 		SetpBaseProp( m_idProp );
// 	}
// 	return _m_pBaseProp;
// }
/**
 @brief 
 const를 씌울수가 없어 여러모로 불편해서 방식을 바꿈.
 프로퍼티를 재로딩한경우는 virtual ReloadProp()같은걸로 일괄적으로 교체하도록 함.
*/
XPropWorld::xBASESPOT* XSpot::GetpBaseProp() const
{
	return _m_pBaseProp;
}

void XSpot::SetpBaseProp( ID idProp )
{
	_m_pBaseProp = PROP_WORLD->GetpProp( idProp );
//	XBREAK( _m_pBaseProp == nullptr );
	SetpBaseProp( _m_pBaseProp ); // 스팟이 없어졌으면 null일수 있음.
}

void XSpot::SetpBaseProp( XPropWorld::xBASESPOT* pProp )
{
	_m_pBaseProp = pProp;
	if( pProp ) {
// 		m_keyProp = PROP_WORLD->GetidKey();
		m_idProp = pProp->idSpot;
	} else {
// 		m_keyProp = 0;
		m_idProp = 0;
	}
}

/**
 @brief PropWorld가 재로딩됨.
*/
void XSpot::ReloadProp()
{
	XBREAK( m_idProp == 0 );
	XBREAK( PROP_WORLD == nullptr );
	auto pProp = PROP_WORLD->GetpProp( m_idProp );
	SetpBaseProp( pProp );
}

/**
 @brief 스팟이 new로 생성되면 최초 한번 호출된다. spAcc가 필요해서 호출함.
 월드에 add된후에 호출된다.

*/
void XSpot::OnCreate( XSPAcc spAcc )
{
	XBREAK( spAcc == nullptr );
	m_spOwner = spAcc;		// 편의상 스팟 주인을 알수 있게함.
#ifdef _GAME_SERVER
	// 다른 서버나 클라에선 필요 없다.
	UpdateLevelArea( spAcc );
	int lvOld = GetLevel();
	ResetLevel( spAcc );
	// 레벨이 안달라졌더라도 군단생성 알고리즘이 바꼈을수 있으니 다시 전투력을 계산해야 한다
//	XBREAK( GetLevel() == 0 );
	ResetPower( GetLevel() );	
#endif
}

void XSpot::Restore()
{
	ReloadProp();
}

void XSpot::Serialize( XArchive& ar ) 
{
	ar << (BYTE)m_typeSpot;
	XBREAK( m_Level > 255 );
	ar << (BYTE)m_Level;
	XBREAK( m_Score > 0xffff );
	ar << (WORD)m_Score;
	ar << m_snSpot;
//	ar << m_strName;
	m_timerSpawn.Serialize( ar );
	ar << (BYTE)xboolToByte(m_bDestroy);
	ar << (BYTE)(*((BYTE*)(&m_bitFlag)));
	XBREAK( GetpBaseProp()->idSpot > 0xffff );
	ar << (WORD)GetpBaseProp()->idSpot;
//	XBREAK( m_typeSpot == XGAME::xSPOT_NPC && m_timerSpawn)
//	ar << m_idDropItem;
	XBREAK( m_aryDropItems.size() > 10 );
	ar << m_aryDropItems;
	ar << m_Power;
	int num = 0;
	for( auto loot : m_aryLoots )
		if( loot != 0 )
			++num;
	XBREAK( m_numLose > 0xff );
	XBREAK( m_Difficult <= -128 || m_Difficult >= 128 );
	ar << (BYTE)m_numLose;
	ar << (char)m_Difficult;
	ar << (BYTE)0;
	ar << (BYTE)num;
	XBREAK( m_aryLoots.size() > XGAME::xRES_MAX );
	int idx = 0;
	for( auto loot : m_aryLoots ) {
		if( loot != 0 ) {
			ar << idx;
			ar << loot;
		}
		++idx;
	}
	int size = 0;
	// 패킷 최적화 대상. 군단정보는 꼭 필요할때만 보내도록 최적화 해야함.
	if( m_spLegion != nullptr && ar.IsForDB() == false ) {
		int sizeOld = ar.size();
		m_spLegion->SerializeFull( ar );
		size = ar.size() - sizeOld;
		XTRACE("size legion=%d", size );
	} else
		ar << 0;
}
/**
 @brief 
 @param ver VER_WORLD_SERIALIZE
*/
BOOL XSpot::DeSerialize( XArchive& ar, DWORD verWorld ) 
{
	BYTE b0;
	WORD w0;
	char c0;
	// m_typeSpot은 외부에서 읽었음.
	ar >> b0;	m_Level = b0;
	ar >> w0;	m_Score = w0;
	ar >> m_snSpot;
	//	m_strName = szBuff;
	int _verTimer = 1;
	if( verWorld < 22 )
		_verTimer = 0;
	m_timerSpawn.DeSerialize( ar, _verTimer );
	ar >> b0; m_bDestroy = ( b0 != 0 );
	ar >> b0; *( (BYTE*)( &m_bitFlag ) ) = b0;
	ar >> w0; m_idProp = w0;
	SetpBaseProp( (ID)w0 );
	// 삭제된 스팟이면 프로퍼티가 없을수 있다.
	ar >> m_aryDropItems;
	for( auto itor = m_aryDropItems.begin(); itor != m_aryDropItems.end(); ) {
		xDropItem drop = (*itor);
		if( drop.idDropItem == 0 )
			itor = m_aryDropItems.erase( itor );
		else
			++itor;
	}
//	ar >> m_idDropItem;
	ar >> m_Power;			// 성스팟의 경우 군단정보가 없어도 파워를 저장하므로 읽음.
	ar >> b0;   m_numLose = b0;
	ar >> c0;	m_Difficult = c0;
	XBREAK( m_Difficult <= -128 || m_Difficult >= 128 );
	ar >> b0;	//m_numStar = b0;
	ar >> b0;	int numLoot = b0;
	for( auto& v : m_aryLoots )		// 클리어
		v = 0;
	for( int i = 0; i < numLoot; ++i ) {
		int idx, num;
		ar >> idx;
		ar >> num;
		if( num < 0 )
			num = 1;
		m_aryLoots[ idx ] = num;
	}
//	ar >> m_aryLoots;
	m_spLegion = LegionPtr( XLegion::sCreateDeserializeFull( ar ) );
	if( GetpBaseProp() == nullptr )
		m_spLegion.reset();
	// 군단정보가 있으면 파워정보를 다시 갱신한다.
	// 위에서읽은 Power값을 우선시 한다. 보석광산의 경우 상대 Acc객체가 없어서 정확히 계산안된다.
	// 군대가 있는데 파워가 없는경우가 있는지 확인.
	XBREAK( m_spLegion && m_Power == 0 );
	auto pBaseProp = GetpBaseProp();
	if( pBaseProp )  {
		if( pBaseProp->idName && m_strName.empty() )
			m_strName = XTEXT( pBaseProp->idName );
		if( pBaseProp->level && m_Level == 0 )
			m_Level = pBaseProp->level;
		if( m_strName.empty() )
			ResetName( nullptr );
	}	
	return TRUE;
}

void XSpot::sSerialize( XArchive& ar, XSpot *pSpot ) 

{
	ar << (BYTE)VER_WORLD_SERIALIZE;
	ar << (BYTE)((pSpot)? 1 : 0);
	ar << (BYTE)0;
	ar << (BYTE)0;
	if( pSpot )
		pSpot->Serialize( ar );
}

/**
 이미 pSpot이 존재할때 사용하는 버전
*/
int XSpot::sDeSerialize( XArchive& ar, XSpot *pSpot )
{
	BYTE b0;
	int ver;
	XGAME::xtSpot typeSpot;
	ar >> b0;	ver = b0;
	XBREAK( ver == 0 );
	ar >> b0;
	if( b0 == 0 )		// null스팟이 시리얼라이즈됨.
		return 1;
	ar >> b0 >> b0;
	ar >> b0;	typeSpot = ( XGAME::xtSpot )b0;
	return pSpot->DeSerialize( ar, ver );
}

int XSpot::sDeserializeUpdate( XWorld *pWorld, ID idSpot, XArchive& ar )
{
	XSpot *pBaseSpot = pWorld->GetSpot( idSpot );
	if( XASSERT(pBaseSpot) )
	{
		return XSpot::sDeSerialize( ar, pBaseSpot );
	}
	return 0;
}

XSpot* XSpot::sCreateDeSerialize( XArchive& ar, XWorld *pWorld )
{
	BOOL bRet = FALSE;
	BYTE b0;
	int verWorld;
	XGAME::xtSpot typeSpot;
	ar >> b0;	verWorld = b0;
	ar >> b0 >> b0 >> b0;
	ar >> b0;	typeSpot = (XGAME::xtSpot)b0;
	XSpot *pSpot = nullptr;
	switch( typeSpot )
	{
	case XGAME::xSPOT_CASTLE:
		pSpot = new XSpotCastle( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_JEWEL:
		pSpot = new XSpotJewel( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_SULFUR:
		pSpot = new XSpotSulfur( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_MANDRAKE:
		pSpot = new XSpotMandrake( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_NPC:
		pSpot = new XSpotNpc( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_DAILY:
		pSpot = new XSpotDaily( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
// 	case XGAME::xSPOT_SPECIAL:
// 		pSpot = new XSpotSpecial( pWorld, typeSpot  );
// 		bRet = pSpot->DeSerialize( ar, verWorld );
// 		break;
	case XGAME::xSPOT_CAMPAIGN:
		pSpot = new XSpotCampaign( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_VISIT:
		pSpot = new XSpotVisit( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	case XGAME::xSPOT_CASH:
		pSpot = new XSpotCash( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
// 	case XGAME::xSPOT_GUILD_RAID:
// 		pSpot = new XSpotGuildRaid( pWorld,  );
// 		bRet = pSpot->DeSerialize( ar, verWorld );
// 		break;
  case XGAME::xSPOT_PRIVATE_RAID:
    pSpot = new XSpotPrivateRaid( pWorld );
    bRet = pSpot->DeSerialize( ar, verWorld );
    break;
	case XGAME::xSPOT_COMMON:
		pSpot = new XSpotCommon( pWorld, typeSpot );
		bRet = pSpot->DeSerialize( ar, verWorld );
		break;
	default:
		XBREAKF(1, "unknown spot type:%d", typeSpot );
		return nullptr;
		break;
	}
//	if( bRet == FALSE )
	if( XBREAK( pSpot == nullptr ) )
		return nullptr;
	if( pSpot->GetpBaseProp() == nullptr )
	{
		CONSOLE("deserialize failed:type=%d", typeSpot );
		SAFE_DELETE(pSpot);
		 
		return nullptr;
	}
#ifdef _CLIENT
	pSpot->SetpDelegate( GAME );
#endif // _CLIENT
	return pSpot;
}

XSpot* XSpot::sCreate( XWorld *pWorld, XPropWorld::xBASESPOT *pBaseProp, XSPAcc spAcc, XDelegateSpot *pDelegate )
{
	if( XBREAK( pBaseProp == nullptr ) )
		return nullptr;
	// 이미 생성된건지 확인.
	XSpot *pSpot = pWorld->GetSpot( pBaseProp->idSpot );
	do {
		if( pSpot ) {
			XBREAK( pSpot->GetpBaseProp() == nullptr );   // 리스트에서 삭제해야함.
			break;
		}
		switch( pBaseProp->type ) {
		case XGAME::xSPOT_CASTLE: {
			auto pProp = static_cast<XPropWorld::xCASTLE*>( pBaseProp );
			pSpot = new XSpotCastle( pWorld, pProp, pDelegate );
		} break;
		case XGAME::xSPOT_JEWEL: {
			auto pProp = static_cast<XPropWorld::xJEWEL*>( pBaseProp );
			pSpot = new XSpotJewel( pWorld, pProp );
		} break;
		case XGAME::xSPOT_SULFUR: {
			auto pProp = static_cast<XPropWorld::xSULFUR*>( pBaseProp );
			pSpot = new XSpotSulfur( pWorld, pProp, pDelegate );
		} break;
		case XGAME::xSPOT_MANDRAKE: {
			auto pProp = static_cast<XPropWorld::xMANDRAKE*>( pBaseProp );
			pSpot = new XSpotMandrake( pWorld, pProp );
		} break;
		case XGAME::xSPOT_NPC: {
			auto pProp = static_cast<XPropWorld::xNPC*>( pBaseProp );
			pSpot = new XSpotNpc( pWorld, pProp, pDelegate );
		} break;
		case XGAME::xSPOT_DAILY: {
			auto pProp = static_cast<XPropWorld::xDaily*>( pBaseProp );
			pSpot = new XSpotDaily( pWorld, pProp, pDelegate );
		} break;
// 		case XGAME::xSPOT_SPECIAL: {
// 			auto pProp = static_cast<XPropWorld::xSpecial*>( pBaseProp );
// 			pSpot = new XSpotSpecial( pWorld, pProp, pDelegate );
// 		} break;
		case XGAME::xSPOT_CAMPAIGN: {
			auto pPropSpot = static_cast<XPropWorld::xCampaign*>( pBaseProp );
			auto pPropCamp = XPropCamp::sGet()->GetpProp( pPropSpot->idCamp );
			if( XASSERT( pPropCamp ) ) {
				pSpot = new XSpotCampaign( pWorld, pPropSpot, pPropCamp );
			}
		} break;
		case XGAME::xSPOT_VISIT: {
			auto pProp = static_cast<XPropWorld::xVisit*>( pBaseProp );
			pSpot = new XSpotVisit( pWorld, pProp );
		} break;
		case XGAME::xSPOT_CASH: {
			auto pProp = static_cast<XPropWorld::xCash*>( pBaseProp );
			pSpot = new XSpotCash( pWorld, pProp );
		} break;
		case XGAME::xSPOT_PRIVATE_RAID: {
			auto pProp = static_cast<XPropWorld::xPrivateRaid*>( pBaseProp );
			pSpot = new XSpotPrivateRaid( pWorld, pProp );
		} break;
		case XGAME::xSPOT_COMMON: {
			auto pProp = static_cast<XPropWorld::xCommon*>( pBaseProp );
			pSpot = new XSpotCommon( pWorld, pProp );
		} break;
		default:
			XBREAKF( 1, "unknown spot type:type=%d", pBaseProp->type );
			break;
		}
		if( pBaseProp && pSpot ) {
			if( pBaseProp->level > 0 )
				pSpot->SetLevel( pBaseProp->level );
			if( pBaseProp->idName )
				pSpot->SetstrName( XTEXT( pBaseProp->idName ) );
		}
	} while( 0 );
	if( XBREAK( pSpot == nullptr ) )
		return nullptr;
	if( pSpot->GetpBaseProp() == nullptr )
	{
		SAFE_DELETE( pSpot );
		return nullptr;
	}
	//	pSpot->OnCreate( spAcc );
	return pSpot;
}

void XSpot::SetpLegion( XLegion *pLegion ) 
{
	m_spLegion = LegionPtr( pLegion );
}

/**
 @brief 
*/
void XSpot::Process( float dt ) 
{
	// 스폰시간이 되면 델리게이트를 발생시킨다.
#ifdef _SERVER
	if( GettimerSpawn().IsOver() )
	{
		if( GetpDelegate() )
			GetpDelegate()->DelegateOnSpawnTimer( this, 0 );
		GettimerSpawnMutable().Off();
	}
#endif // _SERVER
	//
	m_timerSpawn.Update( dt );
	//
}

/**
 @brief 
*/
void XSpot::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	m_pDelegate = pDelegator;
}

/**
 @brief 지역스폰레벨을 업데이트 한다.
*/
void XSpot::UpdateLevelArea( XSPAcc spAcc )
{
// 	XBREAK( spAcc == nullptr );
// 	m_LevelArea = GetLevelByPlayer( spAcc->GetLevel() );
}


/**
 @brief 징표 드랍을 위해 주사위를 굴린다.
*/
ItemBox XSpot::sDoDropScalp( XSPAcc spAcc )
{
	ItemBox2 itemBox2 = sDoDropScalp2( spAcc );
	if( itemBox2.first != 0 ) {
		auto pProp = PROP_ITEM->GetpProp( itemBox2.first );
		if( XASSERT(pProp) )
			return std::make_pair( pProp, itemBox2.second );
	}
	return std::make_pair( nullptr, 0 );
}
ItemBox2 XSpot::sDoDropScalp2( XSPAcc spAcc )
{
	ID idItem = 0;
	int dice = random( 100 );
	if( dice < 20 )
		idItem = 10000;		// crow
	else if( dice < 40 )
		idItem = 10001;		// ironlord
	else if( dice < 60 )
		idItem = 10002;		// freedom
	else if( dice < 80 )
		idItem = 10003;		// ancient
	else
		idItem = 10004;		// flame
	if( XBREAK( idItem == 0 ) ) {
		auto nullItem = std::make_pair( 0, 0 );
		return nullItem;
	}
	return std::make_pair( idItem, 1 );
}

xDropItem XSpot::sDoDropScalpWithDropItem( XSPAcc spAcc )
{
	ItemBox2 itemBox2 = sDoDropScalp2( spAcc );
	if( itemBox2.first != 0 ) {
		return xDropItem( itemBox2.first, itemBox2.second, 1.f );
	}
	return xDropItem();
}

/**
 @brief 부대업글 전리품
*/
ItemBox XSpot::sDoDropMedal( XSPAcc spAcc, int lvSpot )
{
	ItemBox2 itemBox2 = sDoDropMedal2( spAcc, lvSpot );
	if( itemBox2.first != 0 ) {
		auto pProp = PROP_ITEM->GetpProp( itemBox2.first );
		if( XASSERT( pProp ) )
			return std::make_pair( pProp, itemBox2.second );
	}
	return std::make_pair( nullptr, 0 );
}
ItemBox2 XSpot::sDoDropMedal2( XSPAcc spAcc, int lvSpot )
{
	XBREAK( lvSpot == 0 );
	ID idHead[ 3 ];
	idHead[ 0 ] = XGAME::GetSquadLvupItem( XGAME::xAT_TANKER );
	idHead[ 1 ] = XGAME::GetSquadLvupItem( XGAME::xAT_RANGE );
	idHead[ 2 ] = XGAME::GetSquadLvupItem( XGAME::xAT_SPEED );
	std::vector<float> aryChance0 = {0.99f, 0.01f, 0.f, 0.f};
	std::vector<float> aryChance1 = {0.72f, 0.21f, 0.06f, 0.01f};
	ID idItem = 0;
	int idxSel = 0;
	if( lvSpot < 10 )
		idxSel = XE::GetDiceChance( aryChance0 );
	else
		idxSel = XE::GetDiceChance( aryChance1 );
	if( XBREAK( idxSel < 0 ) )
		idxSel = 0;
	idItem = idHead[ random(3) ] + idxSel;
	return std::make_pair( idItem, 1 );
}
xDropItem XSpot::sDoDropMedalWithDropItem( XSPAcc spAcc, int lvSpot )
{
	ItemBox2 itemBox2 = sDoDropMedal2( spAcc, lvSpot );
	if( itemBox2.first != 0 ) {
		return xDropItem( itemBox2.first, itemBox2.second, 1.f );
	}
	return xDropItem();
}
ItemBox XSpot::sDoDropScroll( XSPAcc spAcc, int lvSpot )
{
	ItemBox2 itemBox2 = sDoDropScroll2( spAcc, lvSpot );
	if( itemBox2.first != 0 ) {
		auto pProp = PROP_ITEM->GetpProp( itemBox2.first );
		if( XASSERT( pProp ) )
			return std::make_pair( pProp, itemBox2.second );
	}
	return std::make_pair( nullptr, 0 );
}
/**
 @brief 스킬 두루마리
*/
ItemBox2 XSpot::sDoDropScroll2( XSPAcc spAcc, int lvSpot )
{
	XBREAK( lvSpot == 0 );
	const ID idHead = 10010;	// scroll01
	std::vector<float> aryChance0 = {0.99f, 0.01f, 0.f, 0.f};
	std::vector<float> aryChance1 = {0.72f, 0.21f, 0.06f, 0.01f};
	ID idItem = 0;
	int idxSel = 0;
	if( lvSpot < 10 )
		idxSel = XE::GetDiceChance( aryChance0 );
	else
		idxSel = XE::GetDiceChance( aryChance1 );
	if( XBREAK( idxSel < 0 ) )
		idxSel = 0;
	idItem = idHead + idxSel;
	return std::make_pair( idItem, 1 );
}
xDropItem XSpot::sDoDropScrollWithDropItem( XSPAcc spAcc, int lvSpot )
{
	ItemBox2 itemBox2 = sDoDropScroll2( spAcc, lvSpot );
	if( itemBox2.first != 0 ) {
		return xDropItem( itemBox2.first, itemBox2.second, 1.f );
	}
	return xDropItem();
}

XPropItem::xPROP* XSpot::sDoDropEquip( int level )
{
	// 장비템이 떨어져야 하는가.
	float rateLastChance = XGC->m_rateDropLastChance;
	float multiply = 1.f / rateLastChance;	// 뻥튀기 확률
	float rate = XGC->m_rateEquipDrop * multiply;	// 장비드랍률의 3배의 확률로 드랍리스트에 올라감.
	bool bSuccess = XE::IsTakeChance( rate );
	if( !bSuccess )
		return nullptr;
	int lvSec = -1;
	// 현재 유저계정레벨에 맞춰 드랍테이블을 선택한다.
	int lv = level;
	for( int i = 0; i< XGC->m_aryDropRatePerLevel.GetMax(); ++i ) {
		if( lv >= XGC->m_aryDropRatePerLevel[i].lvStart &&
			lv <= XGC->m_aryDropRatePerLevel[i].lvEnd ) {
			lvSec = i;
			break;
		}
	}
	if( lvSec >= 0 ) {
		int idxGrade = XE::GetDiceChance( XGC->m_aryDropRatePerLevel[lvSec].aryDropRate, 
									XGC->m_frEquipDropMax );
		if( XASSERT(idxGrade > 0 && idxGrade < XGAME::xGD_MAX) ) {
			XList4<XPropItem::xPROP*> listItems;
			PROP_ITEM->GetpPropRandom( (XGAME::xtGrade)idxGrade, 
										XGAME::xIT_EQUIP, 
										1,			// 1개만 떨군다.
										&listItems );
			if( listItems.size() > 0 ) {
				auto ppProp = listItems.GetpFirst();
				if( XASSERT(ppProp) )
					return *ppProp;
			}
		}
	}
	return nullptr;
}
/**
 @brief 스팟 승리후 드랍처리의 기본형
*/
int XSpot::DoDropItem( XSPAcc spAcc, 
					XArrayLinearN<ItemBox,256> *pOutAry, 
					int lvSpot,
					float multiplyDropNum ) const
{
	// 드랍아이템목록이 있다면 확률검사 해서 드랍시킨다.
	DoDropList( spAcc, pOutAry, multiplyDropNum );
	return pOutAry->size();
}

/**
 @brief 드랍리스트가 있다면 드랍처리를 한다.
*/
int XSpot::DoDropList( XSPAcc spAcc,
						XArrayLinearN<ItemBox, 256> *pOutAry,
						float multiplyDropNum ) const
{
	int num = 0;
	for( auto& drop : m_aryDropItems ) {
		bool bDrop = false;
		if( drop.chance == 1.f )
			bDrop = true;
		else if( drop.chance < 1.f ) {
			bDrop = XE::IsTakeChance( drop.chance );
		}
		if( bDrop ) {
			auto pPropItem = PROP_ITEM->GetpProp( drop.idDropItem );
			if( XASSERT( pPropItem ) ) {
				ItemBox item( pPropItem, drop.num );
				pOutAry->Add( item );
				++num;
			}
		}
	}
	return num;
}
/**
 @brief 드랍확률이 있는 모든 아이템목록을 얻는다.
 드랍확률이 1%만 되도 목록에 포함되게 된다.
*/
int XSpot::GetDropableList( std::vector<ID> *pOutAry )
{
	for( auto& dropItem : m_aryDropItems ) {
		pOutAry->push_back( dropItem.idDropItem );
	}
	return pOutAry->size();
}

int XSpot::GetNumSpots() const
{
	if( m_pWorld == nullptr )
		return 0;
	int num = m_pWorld->GetNumSpots( m_typeSpot );
	XBREAK( num > 500 );
	return m_pWorld->GetNumSpots( m_typeSpot );
}

/**
 @brief 이 스팟의 레벨 범위
*/
bool XSpot::GetLevelRangeByArea( int *pOutLvMin, int *pOutLvMax, int lvAcc ) const
{
	auto pArea = GetpAreaProp();
	if( XBREAK(pArea == nullptr) )
		return false;
	int lvArea = GetLevelSpawn( lvAcc );
	if( lvArea < 1 )
		lvArea = 1;
	*pOutLvMin = lvArea;
	*pOutLvMax = lvArea/* + xRandom( 2 )*/;
	return true;
}

/**
 @brief 지역레벨을 플레이어 레벨에 의해서 계산한다.
*/
int XSpot::GetLevelSpawn( int lvAcc ) const 
{
	XBREAK( lvAcc == 0 );
	int lvArea = GetpAreaProp()->lvArea;
	if( lvArea < lvAcc ) {
		// 스팟레벨이 현재 계정레벨보다 작고 그 범위가 -5이내라면 모두 노랑색이 매치되도록 한다.
		if( lvArea > lvAcc - 5 ) {
			if( lvAcc <= 5 )
				lvArea = lvAcc - 1;	// 초저렙에선 동렙이라도 주황색일수 있으니 한단계 더 낮춘다.
			else
				lvArea = lvAcc;	// 동렙으로 설정하여 노랑색이 매치되도록 한다.
		} else {
			auto spOwner = GetspOwner().lock();
			lvArea = XGC->GetNpcLevelByColor( spOwner->GetPowerExcludeEmpty(), -1 );
//			lvArea = lvAcc - 1;
		}
	}
	XBREAK( lvArea <= 0 || lvArea > XGAME::GetLevelMaxAccount() );
	return lvArea;
}

void XSpot::CreateLegion( XSPAcc spAccount )
{
	// this와 싸우기 적당한 수준의 NPC군단를 생성한다.
	ResetLevel( spAccount );
	XBREAK( GetLevel() == 0 );
	// 지역 난이도수준에 맞는 스팟 레벨을 산출한다.
	// 지역 오픈레벨을 기준으로 한다.
	auto pArea = GetpAreaProp();
	if( XASSERT(pArea) ) {
		if( m_Level == 0 ) {
			XBREAK( pArea->lvArea == 0 || pArea->lvArea > XGAME::GetLevelMaxAccount() );
			int level = pArea->lvArea + xRandom( 3 );
			SetLevel( level );
		}
		//
		XGAME::xLegionParam legionInfo;
		legionInfo.m_numVisible = XLegion::xMAX_NOT_FOG;
		auto pLegion = XLegion::sCreateLegionForNPC( //spAccount->GetLevel(),
													GetLevel(), 
													m_Difficult,
													legionInfo );
		XBREAK( GetspLegion() != nullptr );
		SetpLegion( pLegion );
		XBREAK( IsPC() == true );
		UpdatePower( GetspLegion() );
	}
}


bool XSpot::_sGetRewardDailyToday( XPropWorld::xDaily* pProp, int lvAcc, XVector<XGAME::xReward>* pOutAry )
{
	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
	return _sGetRewardDaily( pProp, dowToday, lvAcc, pOutAry );
}

bool XSpot::_sGetRewardDaily( XPropWorld::xDaily* pProp, XE::xtDOW dow, int lvAcc, XVector<XGAME::xReward>* pOutAry )
{
	int idxDow = (dow == XE::xDOW_SUNDAY)? 6 : dow - 1;
	auto& ary = pProp->m_aryDays[ idxDow ];	// 해당요일의 보상리스트
	// 계산
	for( auto& reward : ary ) {
		// 보상양을 가공한다.
		auto lvBase = lvAcc - 7;
		if( lvBase < 0 )
			lvBase = 0;
		const auto v1 = pProp->m_v1;
		const auto v2 = pProp->m_v2;
		const auto v3 = pProp->m_v3;
		int numRes = (int)(v1 + (lvBase * v2) * (reward.num * v3));
		XGAME::xReward rew = reward;
		rew.num = numRes;
		pOutAry->Add( rew );
	}
	return true;
}
/**
 @brief 스팟이 속한 지역의 프로퍼티를 꺼낸다.
*/
XPropCloud::xCloud* XSpot::GetpAreaProp() const
{
	return PROP_CLOUD->GetpAreaHaveSpot( GetidSpot() );
}

int XSpot::GetLevelSpawn() const
{
	return GetpAreaProp()->lvArea;
}

void XSpot::ClearSpot() 
{
	m_Score = 0;
	m_Power = 0;
	m_numLose = 0;
	m_Level = 0;
	m_Difficult = 0;
	DestroyLegion();
	*( (BYTE*)( &m_bitFlag ) ) = 0;
	m_strName.clear();
	m_timerSpawn.Off();
	m_aryDropItems.clear();
	m_seedRandom = 0;
	ClearLootRes();
}

/**
 @brief 스팟이 스폰이 되면 호출되는 핸들러. 타이머에 의한 스폰이거나 캐쉬기능에 의한 리스폰이라거나 스팟이 다시 화면상에 나타나면 스폰이다.
*/
void XSpot::OnSpawn( XSPAcc spAcc ) 
{
	/*
	OnSpawn의 추상화.
	스팟이 화면에 나타나면 호출된다.
	최초 스폰이므로 각종 초기화
	스팟레벨의 지정
	전리품드랍 예약
	장비템드랍 예약
	자원드랍 예약
	*/
	XBREAK( spAcc == nullptr );
	m_timerSpawn.Off();
//	?? = GetLevelSpawn( spAcc->GetLevel() );
	m_aryDropItems.clear();
//	ClearLootRes();
	// 스팟레벨을 결정한다. 스폰당시 계정레벨을 기반으로 한다.
	ResetLevel( spAcc );
	// 스팟 전투력 계산
	ResetPower( GetLevel() );
// 	// 전리품 드랍을 예약한다.
// 	DoDropRegisterBooty( spAcc, 1.f, GetPower(), GetLevel() );
// 	// 장비템 드랍
	DoDropRegisterEquip( spAcc->GetLevel() );
	// 기타 아이템 드랍예약
	DoDropRegisterItem( spAcc->GetLevel() );
	// 자원드랍 예약
	DoDropRegisterRes( spAcc );
	// 레벨등에 의하여 스팟 이름이 달라져야 한다면 따로 처리
	ResetName( spAcc );
}

void XSpot::AddDropItem( ID idDrop, int num, float chance )
{
	AddDropItem( xDropItem( idDrop, num, chance ) );
}

void XSpot::AddDropItem( const _tstring& strIds, int num, float chance )
{
	auto pProp = PROP_ITEM->GetpProp( strIds );
	if( XASSERT(pProp) ) {
		AddDropItem( xDropItem( pProp->idProp, num, chance ) );
	}
}

void XSpot::AddDropItem( xDropItem& dropItem )
{
	ID idDrop = dropItem.idDropItem;
	XBREAK( idDrop == 0 );
	XBREAK( dropItem.num == 0 );
	for( auto& drop : m_aryDropItems ) {
		if( drop.idDropItem == idDrop )
			return;		// 이미 있는 품목은 다시 넣지 않음.
	}
	XBREAK( dropItem.chance <= 0 );		// 확률이 0이하면 경고
	XBREAK( dropItem.chance > 1.f );		// 확률이 1.0이상이면 경고.
	m_aryDropItems.push_back( dropItem );
}

void XSpot::AddDropItem( xDropItem&& dropItem )
{
	AddDropItem( dropItem );
}

/**
 @brief 장비템 하나의 드랍을 예약함.
*/
void XSpot::DoDropRegisterEquip( int level )
{
	// 3배확률로 드랍리스트에 올라가게 하고 실제 드랍시에는 1/3확률을 적용.
	auto pPropItem = sDoDropEquip( level );
	if( pPropItem ) {
		float rate = XGC->m_rateDropLastChance;
		AddDropItem( pPropItem->idProp, 1, rate );
	}
}

void XSpot::DoDropRegisterItem( int level ) 
{
	// 소탕권
	if( XGC->m_rateKillDrop > 0 )
		AddDropItem( _T("item_kill"), 1, XGC->m_rateKillDrop );
}

/**
 @brief 드랍아이템 리스트를 어레이에 담아준다.
*/
int XSpot::GetDropItems( std::vector<xDropItem> *pOutAry )
{
	for( auto& drop : m_aryDropItems ) {
		pOutAry->push_back( drop );
	}
	return pOutAry->size();
}

void XSpot::SetspLegion( LegionPtr spLegion )
{
	m_spLegion = spLegion;
	UpdatePower( nullptr );
}

void XSpot::SetDropItems( const std::vector<xDropItem>& aryDropItem )
{
	m_aryDropItems = aryDropItem;
}
/**
 @brief 스팟에 주둔중인 적부대의 전투력을 계산한다.
 @param spAccEnemy적이 pc이며 계정정보(특성트리포함)가 있다면 그것을 포함해서 계산한다. null이면 특성치에 대한 전투력은 계산되지 않는다.
 @param spLegion 외부파라메터. 이것이 지정되어있다면 이 부대정보로 계산한다.
*/
void XSpot::UpdatePower( LegionPtr spLegion )
{
	// 외부파라메터가 있을때는 그군단의 전투력으로 세팅하고 없으면 내군단의 전투력으로 세팅한다.
	if( spLegion )
		m_Power = XLegion::sGetMilitaryPower( spLegion );
	else
	if( m_spLegion )
		m_Power = XLegion::sGetMilitaryPower( m_spLegion );
}


#if defined(_CLIENT) || defined(_GAME_SERVER)

int XSpot::GetNeedAP( XSPAcc spAcc )
{
	return XPropUser::sGet()->GetTable( spAcc->GetLevel() )->apPerBattle;
}

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

/**
 @brief 스팟의 레벨을 결정한다.
 이 함수는 npc류가 등장하는 스팟에만 적용된다.
*/
int XSpot::CalcLevel( XPropWorld::xBASESPOT *pProp, int lvAcc ) const 
{
	XASSERT( pProp->type == XGAME::xSPOT_NPC || pProp->type == XGAME::xSPOT_SULFUR );
	int lv = 0;
	if( pProp->level ) {
		// 프로퍼티에 직접 레벨이 지정된 경우
		lv = pProp->level;
	} else
	if( !pProp->strWhen.empty() ) {
		// 퀘로 발생하는 스팟인 경우.
		lv = lvAcc;
		if( lvAcc > 1 && lvAcc <= 5 )
			--lv;
	} else {
		int lvMin, lvMax;
		// 스팟이 속한 지역의 레벨대를 구함.
		GetLevelRangeByArea( &lvMin, &lvMax, lvAcc );
		lv = xRandom( lvMin, lvMax );
		if( lv <= 0 )
			lv = 1;
	}
	XBREAK( lv == 0 );
	return lv;
}

void XSpot::OnAfterBattle( XSPAcc spAccWin
												, ID idAccLose
												, bool bWin
												, int numStar
												, bool bRetreat )
{
	if( bWin ) {
		// 대부분의 스팟의 일반적인 처리
		ClearSpot();		
		ResetSpot();
	} else {

	}
}

/**
 @brief 스팟이 쓸수 있는 상태인지 검사
 스팟이 맵에 있더라도 지역이 아직 안열렸으면 활성화되지 않는다.
*/
bool XSpot::IsOpenedSpot( XWorld *pWorld )
{
	// 슈발 이거 여기다 놓을게 아니라 XWorld안으로 들어가는게 맞음.
	auto pPropSpot = GetpBaseProp();
	XBREAK( pPropSpot == nullptr );
	const ID idArea = pPropSpot->idArea;
	XBREAK( idArea == 0 );
	if( XASSERT(pWorld) ) {
		if( pWorld->IsOpenedArea( idArea ) ) {
			return true;
		}
	}
	return false;
}

bool XSpot::IsOurBuilding() const 
{
	const auto ids = GetpBaseProp()->strIdentifier;
	return ( ids == _T("spot.labor") || ids == _T("spot.tavern") || ids == _T("spot.market") 
		|| ids == _T("spot.cathedral") || ids == _T("spot.academy") 
		|| ids == _T("spot.barrack") || ids == _T("spot.embassy") );
	
}

void XSpot::SetLootRes( const XVector<XGAME::xReward>& aryReward )
{
	for( auto& reward : aryReward ) {
		if( XASSERT(reward.rewardType == xtReward::xRW_RESOURCE) ) {
			const auto typeRes = reward.GetResourceType();
			SetLootRes( typeRes, reward.num );
		}
	}
}

/**
@brief 루팅할수 있는 자원들 리스트를 얻는다.
*/
void XSpot::GetLootInfo( XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pAry ) const
{
	int idx = 0;
	for( auto resNum : m_aryLoots ) {
		if( resNum > 0 ) {
			auto resType = (XGAME::xtResource)idx;
			pAry->Add( XGAME::xRES_NUM( resType, resNum ) );
		}
		++idx;
	}
}

/**
 @brief 루팅할수 있는 자원들 리스트를 얻는다.
*/
void XSpot::GetLootInfo( XVector<XGAME::xRES_NUM>* pOutAry ) const
{
	int idx = 0;
	for( auto resNum : m_aryLoots ) {
		if( resNum > 0 ) {
			auto resType = (XGAME::xtResource)idx;
			pOutAry->Add( XGAME::xRES_NUM( resType, resNum ) );
		}
		++idx;
	}
}

/** //////////////////////////////////////////////////////////////////
 @brief 전투를 위한 스팟갱신정보를 아카이브에 담는다.
 이어지는 전투에 필요한 정보만 최소한으로 받을것.
*/
void XSpot::SerializeForBattle( XArchive* pOut, const XParamObj2& param )
{
	// 스팟의 군단정보는 모두 npc이므로(플레이어거는 XAcc에 있으므로) full버전으로 팩킹함.
	XLegion::sSerializeFull( *pOut, m_spLegion );
}
/** //////////////////////////////////////////////////////////////////
 @brief 전투를 위한 정보를 갱신한다.
 @param arAdd 스팟별 추가정보
*/
void XSpot::DeSerializeForBattle( XArchive& arLegion, 
																	XArchive& arAdd, 
																	XSPAcc spAcc )
{
	XLegion::sDeSerializeUpdate( &m_spLegion, spAcc, arLegion );
}
