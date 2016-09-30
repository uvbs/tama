#include "stdafx.h"
#include "XSpotEtc.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
#include "XGame.h"
#endif
#ifdef _GAME_SERVER
	#include "XGameUser.h"
#endif
#include "XLegion.h"
#include "XPropHero.h"
#include "XWorld.h"
#include "XPropItem.h"
#include "XCampObjHero.h"
#include "XStageObjHero.h"
#include "XGuild.h"
#include "server/XGuildMgr.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
void XSpotVisit::OnCreateNewOnServer( XSPAcc spAcc )
{
}
void XSpotVisit::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << (DWORD)0;
}
BOOL XSpotVisit::DeSerialize( XArchive& ar, DWORD verWorld ) 
{
	DWORD dw0;
	XSpot::DeSerialize( ar, verWorld );
	ar >> dw0;
	return TRUE;
}

////////////////////////////////////////////////////////////////
void XSpotCash::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
	XBREAK( m_numCash > 255 );
	ar << (BYTE)m_numCash;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
}

BOOL XSpotCash::DeSerialize( XArchive& ar, DWORD verWorld )
{
#ifndef _DUMMY_GENERATOR
	// CONSOLE( "deserialize CashSpot" );
#endif // not _DUMMY_GENERATOR
	XSpot::DeSerialize( ar, verWorld );
	BYTE b0;
	ar >> b0;	m_numCash = b0;
	ar >> b0;
	ar >> b0;
	ar >> b0;
	return TRUE;
}
void XSpotCash::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	XSpot::OnAfterDeSerialize( pWorld, pDelegator, idAcc, secLastCalc );
#ifdef _SERVER
	if( secLastCalc )	{
		// 마지막 저장한후로부터 현재까지 지나간 시간을 얻는다.
		xSec secPassTime = ( XTimer2::sGetTime() - secLastCalc );
		GettimerSpawnMutable().AddTime( secPassTime );
	}
#endif // SERVER
}

//////////////////////////////////////////////////////////////////////////
void XSpotCommon::OnCreateNewOnServer( XSPAcc spAcc )
{

}

void XSpotCommon::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
	XBREAK( GetpProp()->idSpot > 0xffff );
	ar << (WORD)GetpProp()->idSpot;
	XBREAK( m_DayOfCamp > 0xffff );
	ar << (WORD)m_DayOfCamp;
	if( IsGuildRaid() ) {
		// 길드레이드 스팟의 경우 DB저장시에는 캠페인정보를 저장하지 않음. 캠페인정보는 길드쪽에서 따로 저장함.
		if( ar.IsForDB() )
			ar << 0;
		else
			XCampObj::sSerialize( m_spCampObj, ar );
	} else
		XCampObj::sSerialize( m_spCampObj, ar );
}
BOOL XSpotCommon::DeSerialize( XArchive& ar, DWORD verWorld )
{
	XSpot::DeSerialize( ar, verWorld );
	ID idProp;
	WORD w0;
	ar >> w0;	idProp = w0;
	ar >> w0;	m_DayOfCamp = w0;
	m_spCampObj = XCampObj::sCreateDeserialize( ar );
#ifdef _XCAMP_HERO2
	if( m_spCampObj && m_spCampObj->GetType() == xCT_HERO ) {
		m_spCampObj.reset();
	}
#endif // _XCAMP_HERO2
	return TRUE;
}

XPropCamp::xProp* XSpotCommon::GetpPropCamp()
{
	return XPropCamp::sGet()->GetpProp( GetpProp()->strParam );
}

CampObjPtr XSpotCommon::GetspCampObj( XGuild *pGuild )
{
#ifdef _GAME_SERVER
	if( IsGuildRaid() ) {
		if( m_spCampObj != nullptr )
			return m_spCampObj;
		auto pProp = GetpPropCamp();
		if( XASSERT( pProp ) ) {
			if( XASSERT(pGuild) ) {
				m_spCampObj = pGuild->FindspRaidCampaign( pProp->idProp );
				return m_spCampObj;
			}
		}
	} else
	if( IsMedalCamp() ) {
		int day = XTimer2::sGetTime() / 60 / 60 / 24;
		if( day != m_DayOfCamp ) { 
			m_spCampObj.reset();
			// 날짜가 바꼈으면 캠페인을 새로 생성함.
			// 스팟마다 달라지는부분: 드랍품목, 드랍개수. 도전횟수, 
// 			int grade = GetpProp()->nParam[ 0 ];
// 			XBREAK( grade >= 4 );
			auto pPropCamp = GetpPropCamp();
			XBREAK( pPropCamp == nullptr );
			m_spCampObj = xCampaign::XCampObj::sCreateCampObj( pPropCamp, 0 );
			auto spOwner = GetspOwner().lock();
			XBREAK( spOwner == nullptr );
			// 군대생성시 참조할 외부파라메터를 모든 스테이지에 적용한다.
			XGAME::xLegionParam paramLegion;
			paramLegion.unit = (XGAME::xtUnit)((day % (XGAME::xUNIT_MAX-1)) + 1);
			m_spCampObj->SetParamLegionAllStage( paramLegion );
			m_spCampObj->CreateLegionAll( spOwner->GetLevel() );
			// 드랍아이템 품목을 작성한다.
			UpdateDropItems();
			m_DayOfCamp = day;
		}
		// 날짜가 안바꼈으면 현재 캠페인을 리턴함.
		return m_spCampObj;
	} else
	if( IsHeroCamp() ) {
		// m_spCampObj가 세이브되었다가 로드된것일수도 있으므로
		// 단순히 이미 있는지를 검사하면 안되고 스테이지바뀔게 없는지 업데이트과정을 한번 거쳐야 한다.
		auto pProp = GetpPropCamp();
		if( m_spCampObj == nullptr ) {
			if( XASSERT( pProp ) ) {
#ifdef _XCAMP_HERO2
				m_spCampObj = xCampaign::sCreateCampObj( xCampaign::xCT_HERO2, pProp, 0 );
#else
				m_spCampObj = xCampaign::sCreateCampObj( xCampaign::xCT_HERO, pProp, 0 );
#endif // _XCAMP_HERO2
			}
		}
		// 스테이지순서가 바꼈을수 있으므로 업데이트를 한번한다.
		XBREAK( m_spCampObj == nullptr );
// 		auto spCampObjHero = SafeCast<XCampObjHero*>( m_spCampObj.get() );
// 		if( XASSERT(spCampObjHero) )
			// 업데이트가 이미 되어있다면 다시 하지 않음.
// 			if( !spCampObjHero->GetbUpdated() )
// 				spCampObjHero->UpdateStages();
		return m_spCampObj;
	}
	return nullptr;
#else
	return m_spCampObj;
#endif // _GAME_SERVER
}
/**
 @brief 각 스테이지가 드랍할수 있는 품목을 세팅한다. 이미 리스트가 있다면 넣지 않는다.
*/
void XSpotCommon::UpdateDropItems()
{
	if( m_spCampObj == nullptr )
		return;
	if( IsMedalCamp() ) {
		// 생성할때 만드니까 여기서 필요없긴한데 그냥 넣음.
// 		int grade = GetpProp()->nParam[ 0 ];
// 		XBREAK( grade >= 4 );
		int numStages = m_spCampObj->GetNumStages();
		for( int i = 0; i < numStages; ++i ) {
			auto spStageObj = m_spCampObj->GetspStage( i );
			// 드랍아이템목록이 없을때만 넣는다.
			if( spStageObj && spStageObj->GetaryDrops().size() == 0 ) {
				// 각 스테이지마다 등급에 맞는 3병과 메달을 넣는다.
				int num = i + 1;
				auto 
				ids = XGAME::GetIdsMedal( XGAME::xAT_TANKER, 0 );
				spStageObj->AddItems( ids, num );
			}
		}
	}
}

bool XSpotCommon::Update( XSPAcc spAcc )
{
	UpdateDropItems();
	return false;
}

#ifdef _SERVER
CampObjPtr XSpotCommon::GetspCampObj( ID idGuild )
{
#ifdef _CLIENT
	XBREAK(1);	// 클라에선 아직 사용안됨. GAME->m_pGuild를 가져오도록 바꿔야 함.
#endif
	if( m_spCampObj != nullptr )
		return m_spCampObj;
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if( XASSERT(pGuild) )
		return GetspCampObj( pGuild );
	return CampObjPtr();
}
#endif // _SERVER

/**
 @brief 스테이지 하나를 클리어하면 스테이지에 드랍아이템 판정을 한다.
*/
int XSpotCommon::DoDropItem( XSPAcc spAcc, 
							XArrayLinearN<ItemBox, 256> *pOutAry, 
							int lvSpot, 
							float multiplyDropNum/* = 1.f*/ ) const
{
	// 징표등 기본 드랍.
	XSpot::DoDropItem( spAcc, pOutAry, lvSpot, multiplyDropNum );
	if( IsMedalCamp() ) {
		if( XASSERT( m_spCampObj ) ) {
// 			auto atkType = (XGAME::xtAttack)(1 + xRandom(XGAME::xAT_MAX-1) );
// 			auto idsMedal = XGAME::GetIdsMedal( atkType, grade );
			int idxStage = m_spCampObj->GetidxPlaying();
			auto spStageObj = m_spCampObj->GetspStage( idxStage );
			if( XASSERT(spStageObj) ) {
				//
				for( auto& drop : spStageObj->GetaryDrops() ) {
					auto pPropItem = PROP_ITEM->GetpProp( drop.idDropItem );
					if( XASSERT(pPropItem) ) {
						ItemBox itemBox;
						std::get<0>( itemBox ) = pPropItem;
						std::get<1>( itemBox ) = drop.num;
						pOutAry->Add( itemBox );
					}
				}
			}
		}
	}
	return pOutAry->size();
}

void XSpotCommon::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
	if( m_spCampObj )
		m_spCampObj->OnFinishBattle();
}

void XSpotCommon::ResetLevel( XSPAcc spAcc )
{
	if( GetLevel() == 0 ) {
		if( m_spCampObj ) {
			auto spStageObj = m_spCampObj->GetspStageObjCurrPlaying();
			if( spStageObj ) {
				int lvLegion = spStageObj->GetLevelLegion();
				XBREAK( lvLegion <= 0 );
				SetLevel( lvLegion );
			}
		}
	}
}

void XSpotCommon::ResetName( XSPAcc spAcc )
{
	if( m_spCampObj ) {
		if( GetstrName().empty() )
			if( m_spCampObj->GetpProp() )
				SetstrName( XTEXT(m_spCampObj->GetpProp()->idName) );
	}
}

void XSpotCommon::OnBeforeBattle( XSPAcc spAcc )
{
	// 이름이 지정안되어있으면 이름을 지정함.
	if( GetstrName().empty() )
		ResetName( spAcc );
	// 레벨이 지정안되어있으면 레벨을 지정함.
	if( GetLevel() <= 0 )
		ResetLevel( spAcc );
}

bool XSpotCommon::IsGuildRaid() const
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return (pProp)? (pProp->strType == _T( "guild_raid" )) : false;
}
bool XSpotCommon::IsMedalCamp() const 
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return ( pProp ) ? ( pProp->strType == _T( "fixed_camp" ) ) : false;
}
// 영웅의전장인가.
bool XSpotCommon::IsHeroCamp() const 
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return ( pProp ) ? ( pProp->strType == _T( "hero_camp" ) ) : false;
}

// bool XSpotCommon::IsPrivateRaid() const
// {
// 	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
// 	return ( pProp ) ? ( pProp->strType == _T( "private.raid" ) ) : false;
// }
