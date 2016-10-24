#include "StdAfx.h"
#ifdef _XSINGLE
#include "XSceneBattleSub.h"
#include "XSceneBattle.h"
#include "XSceneBattleSingle.h"
#include "XGame.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XPropLegion.h"
#include "XPropLegionH.h"
#include "XAccount.h"
#include "XHero.h"
#include "XScenePrivateRaid.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
//using namespace XSKILL;

//////////////////////////////////////////////////////////////////////////
/**
 @brief 군단객체 생성에 필요한 파라메터들을 생성
 for 싱글모드
*/
XSPAcc XSceneBattle::sCreateAcc()
{
	XSPAcc spAcc = XAccount::sGetPlayer();
	if( spAcc == nullptr ) {
		// 가상 계정 생성
		spAcc = std::make_shared<XAccount>( 1 );
		spAcc->SetpDelegateLevel( GAME );
		XAccount::sSetPlayer( spAcc );
		// 가상 파라메터 입력
		spAcc->CreateFakeAccount();
		// 플레이어측 군단생성
		auto pPropLegion = XPropLegion::sGet()->GetpProp( "single1_player" );
		if( pPropLegion ) {
			auto spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, 50, false );
			for( auto pSquad : spLegion->GetlistSquadrons() ) {
				if( pSquad && pSquad->GetpHero() ) {
					ACCOUNT->AddHero( pSquad->GetpHero() );
					pSquad->SetbCreateHero( FALSE );
				}
			}
			ACCOUNT->SetspLegion( 0, spLegion );
		}
	}
	return spAcc;
}

/** //////////////////////////////////////////////////////////////////
@brief 싱글모드용. 플레이어의 가상계정을 생성하고 전투씬에 넘겨줄 파라메터를 생성한다.
*/
std::shared_ptr<XGAME::xSceneBattleParam>
XSceneBattle::sSetBattleParam()
{
//	XGAME::xBattleStart bs;
	const int level = 50;
	// 플레이어의 가상계정 생성
	auto spAcc = sCreateAcc();
	// 전투씬 파라메터 준비
	XVector<XSPLegion> aryLegion;
	aryLegion.push_back( spAcc->GetCurrLegion() );
	{
		// 적 진영 군단 생성
		auto pPropLegion = XPropLegion::sGet()->GetpProp( "single1_enemy" );
		if( pPropLegion ) {
			auto spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, 
																										 level, false );
			aryLegion.push_back( spLegion );
		}
	}
	// 전투씬 파라메터
	auto spSceneParam 
		= std::make_shared<XGAME::xSceneBattleParam>( 0,
																									xSPOT_NPC,
																									1,
																									50,
																									_T( "babarian" ),
																									aryLegion,
																									XGAME::xBT_NORMAL,
																									0, -1, 0 );
	return spSceneParam;
}

/** //////////////////////////////////////////////////////////////////
 @brief 개인레이드용 파라메터 세팅
*/
std::shared_ptr<XGAME::xPrivateRaidParam>
XScenePrivateRaid::sSetPrivateRaidParam()
{
	//	XGAME::xBattleStart bs;
	const int level = 50;
	// 플레이어의 가상계정 생성
	auto spAcc = sCreateAcc();
	// 전투씬 파라메터 준비
	XVector<XSPLegion> aryLegion;
	aryLegion.push_back( spAcc->GetCurrLegion() );
	{
		// 적 진영 군단 생성
		auto pPropLegion = XPropLegion::sGet()->GetpProp( "single1_enemy" );
		if( pPropLegion ) {
			auto spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion,
																										 level, false );
			aryLegion.push_back( spLegion );
		}
	}
	// 전투씬 파라메터
	auto spSceneParam
		= std::make_shared<XGAME::xPrivateRaidParam>( 0,
																									xSPOT_NPC,
																									1,
																									50,
																									_T( "babarian" ),
																									aryLegion );
	for( int i = 0; i < 2; ++i ) {
		// 군단 영웅 외에 모자라는 영웅수만큼 새로 생성
		int remain = MAX_SQUAD_PVRAID - spAcc->GetCurrLegion()->GetNumSquadrons();
		for( int k = 0; k < remain; ++k ) {
			// 랜덤으로 하나 선택
//			auto pPropHero = XHero::sGet()->GetPropRandom();
			const xtGet bit = (xtGet)(xGET_GATHA | xGET_QUEST | xGET_GUILD_RAID | xGET_MEDAL_SPOT);
			auto pPropHero = XHero::sGet()->GetpPropRandomByGetType( bit );
			const auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, (xtSize)xRandom(1,3) );
			auto pHero = XHero::sCreateHero( pPropHero, 1, unit, spAcc );
			spSceneParam->m_aryEnter[i].push_back( pHero );
		}
	}
	return spSceneParam;
}

#endif // _XSINGLE
