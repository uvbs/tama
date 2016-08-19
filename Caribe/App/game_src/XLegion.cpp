#include "stdafx.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XArchive.h"
#include "XAccount.h"
#include "XPropUnit.h"
#include "XPropSquad.h"
#include "XExpTableHero.h"
#include "XBaseItem.h"
#include "constGame.h"
#include "XGlobalConst.h"
#include "XPropLegionH.h"
#include "XHero.h"
#ifndef _CLIENT
#include "XMain.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
/**
 @brief lvHero가 가질수 있는 최대부대레벨
*/
int XLegion::sGetSquadMaxLevelByHeroLevel( int lvHero )
{
	return PROP_SQUAD->GetLevelSquadMaxByHero( lvHero );
// 	return (int)(((float)lvHero / EXP_TABLE_HERO->GetMaxLevel()) 
// 		* PROP_SQUAD->GetMaxLevel());
}

int XLegion::sGetAvailableUnitByLevel( int level, XVector<XGAME::xtUnit> *pOutAry )
{
	pOutAry->Add( XGAME::xUNIT_SPEARMAN );
	pOutAry->Add( XGAME::xUNIT_ARCHER );
	pOutAry->Add( XGAME::xUNIT_PALADIN );
	int numAvailable = XAccount::sGetNumUnitByLevel( level ) - 3;
	XList4<XGAME::xtUnit> listAll;
	for( int i = 4; i < XGAME::xUNIT_MAX; ++i )
		listAll.Add( ( XGAME::xtUnit )i );
	for( int i = 0; i < numAvailable; ++i )	{
		auto pUnit = listAll.GetpFromRandom();
		if( pUnit ) {
			pOutAry->Add( *pUnit );
			listAll.Del( *pUnit );	// 중복된것을 넣지 않기 위해.
		}
	}
	return pOutAry->size();
}
/**
 @brief level대에 선택가능한 유닛
*/
int XLegion::sGetAvailableUnitByLevel( int level, 
						XArrayLinearN<XGAME::xtUnit,XGAME::xUNIT_MAX> *pOutAry )
{
	XVector<XGAME::xtUnit> ary;
	sGetAvailableUnitByLevel( level, &ary );
	for( auto elem : ary )
		pOutAry->Add( elem );
	return pOutAry->size();
}

float sGetDefaultRateHp( xtGradeLegion gradeLegion )
{
	if( gradeLegion == xGL_ELITE )
		return RATE_HP_DEFAULT_ELITE;
	else
	if( gradeLegion == xGL_RAID )
		return  RATE_HP_DEFAULT_RAID;
	return 1.f;
}
float sGetDefaultRateAtk( xtGradeLegion gradeLegion )
{
	if( gradeLegion == xGL_ELITE )
		return RATE_ATK_DEFAULT_ELITE;
	else
	if( gradeLegion == xGL_RAID )
		return  RATE_ATK_DEFAULT_RAID;
	return 1.f;
}
/**
 @brief legion정보에 맞춰 군단을 생성한다.
 @param lvExtern 자동생성을 위해 필요한 외부레벨. 스팟레벨이나 계정레벨이 됨.
*/
#if defined(_XSINGLE) || !defined(_CLIENT)
XSPLegion XLegion::sCreateLegionForNPC2( XGAME::xLegion& legion, int lvExtern, bool bAdjustLegion )
{
	auto pLegion = new XLegion;
	auto spLegion = XSPLegion( pLegion );
	//
	do {
		XBREAK( !legion.gradeLegion );
		pLegion->SetgradeLegion( legion.gradeLegion );
		if( legion.mulHp ) {
			pLegion->SetRateHp( legion.mulHp );
		} else {
			pLegion->SetRateHp( sGetDefaultRateHp( legion.gradeLegion ) );
		}
		if( legion.mulAtk ) {
			pLegion->SetRateAtk( legion.mulAtk );
		} else {
			pLegion->SetRateAtk( sGetDefaultRateAtk( legion.gradeLegion ) );
		}
		// 군단레벨 지정
		int lvLegion = legion.lvLegion;
		if( lvLegion ) {								
			if( legion.adjLvLegion != 0x7f )	
				lvLegion += legion.adjLvLegion;		// 군단레벨 보정
		} else {
			// 군단레벨이 지정안되어있으면 외부레벨이라도 있어야 함.
			XBREAK( lvExtern == 0 );	// 
			lvLegion = lvExtern;		// 군단레벨이 지정안되어있으면 외부레벨을 군단레벨로 씀.
		}
		XBREAK( lvLegion == 0 );
		// 군단레벨을 기준으로 부대수를 정한다.
		int numSquad = legion.numSquad;
		if( numSquad == 0 ) {
			const auto& tblLegion = XGC->GetLegionTable( lvLegion );
			numSquad = tblLegion.m_numSquad;
		} else {
			int a = 0;
		}
		XBREAK( numSquad == 0 );
		const int idx[ XGAME::MAX_SQUAD ] = {2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14};
		// 메뉴얼지정된 squad부터 생성하고 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
		// 메뉴얼 지정된 부대부터 생성
// 		for( int i = 0; i < (int)legion.arySquads.size(); ++i ) {
// 			const auto& squad = legion.arySquads[ i ];
		int i = 0;
		for( auto& squad : legion.arySquads ) {
			XGAME::xSquad sqParam = squad;
			sqParam.idxPos = ( squad.idxPos >= 0 ) ? squad.idxPos : idx[ i ];
			auto pPropHero = sGetpPropHeroByInfo( squad, lvLegion );
			sqParam.unit = sGetUnitByInfo( squad, pPropHero, lvLegion );
			sqParam.lvHero = sGetLvHeroByInfo( squad, lvLegion );
			sqParam.lvSkill = sGetLvSkillByInfo( squad, lvLegion );
			sqParam.lvSquad = sGetLvSquadByInfo( squad, lvLegion );
			sqParam.grade = sGetGradeHeroByInfo( squad, lvLegion );
			sqParam.mulAtk = squad.mulAtk;
			sqParam.mulHp = squad.mulHp;

			auto pSquad = sCreateSquadronForNPC2( lvLegion
																					, pPropHero
																					, sqParam );
			if( XASSERT( pSquad ) ) {
				pLegion->SetSquadron( sqParam.idxPos, pSquad, true );
				// 리더 영웅 지정
				sSetLeaderByInfo( &legion, spLegion, pSquad );
			} 
			++i;
		}  // for legion.arySquads
		// 생성해야할 부대수가 부족하다면 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
		if( pLegion->GetNumSquadrons() < numSquad ) {
			// 빈 슬롯들의 인덱스를 추려낸다.
			XList4<int> listSlotNum;
			if( legion.arySquads.size() != 0 ) {
				for( int i = 0; i < pLegion->GetMaxSquadSlot(); ++i ) {
					if( pLegion->GetpSquadronByIdx( i ) == nullptr )
						listSlotNum.Add( i );
				}
			}
			const int numSquadExt = numSquad - pLegion->GetNumSquadrons();
			auto& squad = legion.squadDefault;
			XGAME::xSquad sqParam = squad;
			for( int i = 0; i < numSquadExt; ++i ) {
				int idxSlot;
				if( legion.arySquads.size() == 0 ) {
					// xml로 메뉴얼 지정이 없었을땐 슬롯 순서대로 부대를 생성.
					idxSlot = idx[ i ];
				} else {
					auto itor = listSlotNum.GetItorFromRandom();
					if( XASSERT(itor != listSlotNum.end()) ) {
						idxSlot = ( *itor );
						listSlotNum.erase( itor );	// 한번 사용한 인덱스슬롯 번호는 다시 사용하지 않게.
					}
				}
				auto pPropHero = sGetpPropHeroByInfo( squad, lvLegion );
				sqParam.unit = sGetUnitByInfo( squad, pPropHero, lvLegion );
				sqParam.lvHero = sGetLvHeroByInfo( squad, lvLegion );
				sqParam.lvSkill = sGetLvSkillByInfo( squad, lvLegion );
				sqParam.lvSquad = sGetLvSquadByInfo( squad, lvLegion );
				sqParam.grade = sGetGradeHeroByInfo( squad, lvLegion );
				sqParam.mulAtk = squad.mulAtk;
				sqParam.mulHp = squad.mulHp;
#ifdef _XSINGLE
				sqParam.m_numAbil = squad.m_numAbil;
#endif // _XSINGLE
				auto pSquad = sCreateSquadronForNPC2( lvLegion
																						, pPropHero
																						, sqParam );
				if( XASSERT( pSquad ) ) {
					pLegion->SetSquadron( idxSlot, pSquad, true );
					// 리더 영웅 지정
					sSetLeaderByInfo( &legion, spLegion, pSquad );
				}
			} 
		} // if( pLegion->GetNumSquadrons() < numSquad ) {
		// 리더가 지정되지 않았을경우 자동으로 리더를 정함.
		if( pLegion->GetpLeader() == nullptr )
			pLegion->SetAutoLeader();
	} while(0);
#ifndef _XSINGLE
	if( bAdjustLegion && legion.arySquads.size() == 0 )
		pLegion->AdjustLegion();
#endif // not _XSINGLE
	return spLegion;
} // 
#endif // defined(_XSINGLE) || defined(!_CLIENT)

#if defined(_XSINGLE) || !defined(_CLIENT)

/**
@brief 군단생성의 일반화 버전. PC/NPC구분없이 쓸수 있다.
@param legion 군단프로퍼티
@param lvExtern 외부 레벨 파라메터. legion에 군단레벨이 지정되어있지 않을때 참조할 군단레벨
@param bReArrangeSquad 군단생성후 부대특성에 따라 위치를 재배치할것인지.
@param 생성한 영웅들의 리스트를 받는다.
*/
// XSPLegion XLegion::sCreatespLegion( const XGAME::xLegion& legion
// 																, int lvExtern
// 																, bool bReArrangeSquad
// 																, XVector<XHero*>* pOutAry )
// {
// 	auto spLegion = std::make_shared<XLegion>();
// 	auto pLegion = spLegion.get();
// 		//
// 	do {
// 		XBREAK( !legion.gradeLegion );
// 		pLegion->SetgradeLegion( legion.gradeLegion );
// 		if( legion.mulHp ) {
// 			pLegion->SetRateHp( legion.mulHp );
// 		} else {
// 			pLegion->SetRateHp( sGetDefaultRateHp( legion.gradeLegion ) );
// 		}
// 		if( legion.mulAtk ) {
// 			pLegion->SetRateAtk( legion.mulAtk );
// 		} else {
// 			pLegion->SetRateAtk( sGetDefaultRateAtk( legion.gradeLegion ) );
// 		}
// 		// 군단레벨 지정
// 		int lvLegion = legion.lvLegion;
// 		if( lvLegion ) {
// 			if( legion.adjLvLegion != 0x7f )
// 				lvLegion += legion.adjLvLegion;		// 군단레벨 보정
// 		} else {
// 			// 군단레벨이 지정안되어있으면 외부레벨이라도 있어야 함.
// 			XBREAK( lvExtern == 0 );	// 
// 			lvLegion = lvExtern;		// 군단레벨이 지정안되어있으면 외부레벨을 군단레벨로 씀.
// 		}
// 		XBREAK( lvLegion == 0 );
// 		// 군단레벨을 기준으로 부대수를 정한다.
// 		int numSquad = legion.numSquad;
// 		if( numSquad == 0 ) {
// 			const auto& tblLegion = XGC->GetLegionTable( lvLegion );
// 			numSquad = tblLegion.m_numSquad;
// 		} else {
// 			int a = 0;
// 		}
// 		XBREAK( numSquad == 0 );
// 		const int idx[XGAME::MAX_SQUAD] = { 2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14 };
// 		// 메뉴얼지정된 squad부터 생성하고 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
// 		// 메뉴얼 지정된 부대부터 생성
// 		// 		for( int i = 0; i < (int)legion.arySquads.size(); ++i ) {
// 		// 			const auto& squad = legion.arySquads[ i ];
// 		int i = 0;
// 		for( auto& squad : legion.arySquads ) {
// 			XGAME::xSquad sqParam = squad;
// 			sqParam.idxPos = (squad.idxPos >= 0) ? squad.idxPos : idx[i];
// 			auto pPropHero = sGetpPropHeroByInfo( squad, lvLegion );
// 			sqParam.unit = sGetUnitByInfo( squad, pPropHero, lvLegion );
// 			sqParam.lvHero = sGetLvHeroByInfo( squad, lvLegion );
// 			sqParam.lvSkill = sGetLvSkillByInfo( squad, lvLegion );
// 			sqParam.lvSquad = sGetLvSquadByInfo( squad, lvLegion );
// 			sqParam.grade = sGetGradeHeroByInfo( squad, lvLegion );
// 			sqParam.mulAtk = squad.mulAtk;
// 			sqParam.mulHp = squad.mulHp;
// 		
// 			auto pSquad = sCreateSquadron( lvLegion
// 																		, pPropHero
// 																		, sqParam );
// 			if( XASSERT( pSquad ) ) {
// 				pLegion->SetSquadron( sqParam.idxPos, pSquad, true );
// 				// 리더 영웅 지정
// 				sSetLeaderByInfo( &legion, spLegion, pSquad );
// 			}
// 			++i;
// 		}  // for legion.arySquads
// 			 // 생성해야할 부대수가 부족하다면 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
// 		if( pLegion->GetNumSquadrons() < numSquad ) {
// 			// 빈 슬롯들의 인덱스를 추려낸다.
// 			XList4<int> listSlotNum;
// 			if( legion.arySquads.size() != 0 ) {
// 				for( int i = 0; i < pLegion->GetMaxSquadSlot(); ++i ) {
// 					if( pLegion->GetpSquadronByIdx( i ) == nullptr )
// 						listSlotNum.Add( i );
// 				}
// 			}
// 			const int numSquadExt = numSquad - pLegion->GetNumSquadrons();
// 			auto& squad = legion.squadDefault;
// 			XGAME::xSquad sqParam = squad;
// 			for( int i = 0; i < numSquadExt; ++i ) {
// 				int idxSlot;
// 				if( legion.arySquads.size() == 0 ) {
// 					// xml로 메뉴얼 지정이 없었을땐 슬롯 순서대로 부대를 생성.
// 					idxSlot = idx[i];
// 				} else {
// 					auto itor = listSlotNum.GetItorFromRandom();
// 					if( XASSERT( itor != listSlotNum.end() ) ) {
// 						idxSlot = (*itor);
// 						listSlotNum.erase( itor );	// 한번 사용한 인덱스슬롯 번호는 다시 사용하지 않게.
// 					}
// 				}
// 				auto pPropHero = sGetpPropHeroByInfo( squad, lvLegion );
// 				sqParam.unit = sGetUnitByInfo( squad, pPropHero, lvLegion );
// 				sqParam.lvHero = sGetLvHeroByInfo( squad, lvLegion );
// 				sqParam.lvSkill = sGetLvSkillByInfo( squad, lvLegion );
// 				sqParam.lvSquad = sGetLvSquadByInfo( squad, lvLegion );
// 				sqParam.grade = sGetGradeHeroByInfo( squad, lvLegion );
// 				sqParam.mulAtk = squad.mulAtk;
// 				sqParam.mulHp = squad.mulHp;
// 				auto pSquad = sCreateSquadronForNPC2( lvLegion
// 																							, pPropHero
// 																							, sqParam );
// 				if( XASSERT( pSquad ) ) {
// 					pLegion->SetSquadron( idxSlot, pSquad, true );
// 					// 리더 영웅 지정
// 					sSetLeaderByInfo( &legion, spLegion, pSquad );
// 				}
// 			}
// 		} // if( pLegion->GetNumSquadrons() < numSquad ) {
// 			// 리더가 지정되지 않았을경우 자동으로 리더를 정함.
// 		if( pLegion->GetpLeader() == nullptr )
// 			pLegion->SetAutoLeader();
// 	} while( 0 );
// #ifndef _XSINGLE
// 	if( bReArrangeSquad && legion.arySquads.size() == 0 )
// 		pLegion->AdjustLegion();
// #endif // not _XSINGLE
// 	return spLegion;
// } 
#endif // #if defined(_XSINGLE) || !defined(_CLIENT)

/**
@brief 신버전 부대생성기.
파라메터는 반드시 유효한 값으로 들어와야한다.
*/
// #if defined(_XSINGLE) || !defined(_CLIENT)
// XSquadron* XLegion::sCreateSquadron( const int lvLegion
// 																	, XPropHero::xPROP *pPropHero
// 																	, const XGAME::xSquad& sqParam )
// {
// 	XASSERT( lvLegion > 0 );
// 	XASSERT( !IsInvalidUnit( sqParam.unit ) );
// 	XASSERT( pPropHero );
// 	XASSERT( sqParam.lvSkill > 0 && sqParam.lvSkill < XGAME::MAX_SKILL_LEVEL );
// 	XASSERT( sqParam.lvSquad > 0 && sqParam.lvSquad <= XGAME::MAX_SQUAD_LEVEL );
// 	XASSERT( !IsInvalidGrade( sqParam.grade ) );
// 	//
// 	
// 	pc/npc 구분없이 일반화 시키려면 
// 		1.XSquadron을 shared_ptr로 한다.
// 		2.
// 	싱글/서버 공통
// 		.pc군단을 propLegion으로 생성할때는 NPC버전으로 생성한다음, 생성된 영웅들을 모두 acc로 옮긴후 각 XSquadron::m_bCreateHero는 false로 해야한다.
// 
// 	auto pSquad = new XSquadron( pPropHero, sqParam.lvHero, sqParam.unit, sqParam.lvSquad, false );
// 	auto pHero = pSquad->GetpHero();
// 	pHero->SetGrade( sqParam.grade );
// 	pSquad->SetmulAtk( sqParam.mulAtk );
// 	pSquad->SetmulHp( sqParam.mulHp );
// 	for( auto& abil : sqParam.m_listAbil ) {
// 		pHero->SetAbilPoint( abil.m_idsAbil, abil.point );
// 	}
// 	return pSquad;
// }
// #endif // // #if defined(_XSINGLE) || !defined(_CLIENT)

/**
 @brief 분대 생성 모듈.
 @param idHero 0이 아니라면 idHero를 부대장으로 한다.
 @param levelHero 0이 아니라면 levelHero를 부대장 레벨로 한다.
 @param levelSkill 0이 아니라면 부대장의 스킬레벨로 한다.
 @param levelSuqad 0이 아니라면 부대레벨로 한다. 
 @note 레벨류는 랜덤요소를 넣지 말것. adjDiff로 조절함.
*/
#if defined(_XSINGLE) || !defined(_CLIENT)
XSquadron* XLegion::sCreateSquadronForNPC( int levelLegion,
											int adjDiff,
											XGAME::xtUnit unit,
											ID idHero, 
											int _levelHero, 
											int levelSkill, 
											int _levelSquad,
											XGAME::xtGradeLegion gradeLegion, 
											const XGAME::xLegionParam *pLegionInfo )
{
	const auto& tblLegion = XGC->GetLegionTable( levelLegion );
	int levelHero = _levelHero;
	// 영웅레벨 자동지정옵션이면 군단레벨을 기준으로 맞춘다.
	if( levelHero == 0 ) {
		levelHero = tblLegion.m_lvHero;
	}
	/*
	부대전투력조절에 관한 연구
	adjDiff로 세부조절하려고 했으나 군단내 모든영웅이 같은 레벨로 맞춰지기때문에 세부조절이 힘들다.
	영웅별로 하지말고 군단 전체 영웅레벨합산치를 던져서 각 영웅들이 나눠가져야 한다. 예를들어 합산레벨이10이고 영웅이 4명이면 2,3,2,3으로 맞춰져야 한다.
	*/
	if( levelHero <= 0 )
		levelHero = 1;
	if( levelHero > XGAME::MAX_HERO_LEVEL )
		levelHero = XGAME::MAX_HERO_LEVEL;
	int levelSquad  = _levelSquad;
	// 부대레벨 자동지정옵션이면 영웅레벨에 비례해서 레벨을 맞춘다.
	if( levelSquad == 0 ) {
		levelSquad = tblLegion.m_lvSquad;
	}
	if( levelSquad <= 0 )
		levelSquad = 1;
	if( levelSquad > PROP_SQUAD->GetMaxLevel() )
		levelSquad = PROP_SQUAD->GetMaxLevel();
	XPropHero::xPROP *pPropHero = nullptr;
	// 확률테이블에 의거해 영웅의 등급을 정함.
//	auto grade = XLegion::sGetRandomGradeHeroByTable( levelHero, gradeLegion );
	const auto grade = tblLegion.m_gradeHero;
	// 외부파라메터는 모든것에 우선한다.
	if( pLegionInfo && pLegionInfo->unit )
		unit = pLegionInfo->unit;
	XBREAK( unit == XGAME::xUNIT_NONE );
	if( idHero ) {
		pPropHero = PROP_HERO->GetpProp( idHero );
		XBREAK( pPropHero 
				&& pPropHero->typeAtk != XGAME::GetAtkType( unit ) );	// 지정한 영웅과 유닛의 타입이 서로 다름.
	} else {
		pPropHero 
			= PROP_HERO->GetPropRandomByAtkType( XGAME::GetAtkType( unit )
																				, levelLegion );
	}
// 	if( pLegionInfo && pLegionInfo->unit )	이걸 propHero결정보다 밑에서하면 외부에서 unit을 직접 지정했을때 버그 난다.
// 		unit = pLegionInfo->unit;
// 	XBREAK( unit == XGAME::xUNIT_NONE );
	auto pSquad = new XSquadron( pPropHero, levelHero, unit, levelSquad );
	pSquad->GetpHero()->SetGrade( grade );
	return pSquad;
}
/**
 @brief 신버전 부대생성기.
 파라메터는 반드시 유효한 값으로 들어와야한다.
*/
XSquadron* XLegion::sCreateSquadronForNPC2( const int lvLegion
																					, XPropHero::xPROP *pPropHero
																					, const XGAME::xSquad& sqParam )
{
	XASSERT( lvLegion > 0 );
	XASSERT( !IsInvalidUnit(sqParam.unit) );
	XASSERT( pPropHero );
//	XASSERT( sqParam.lvHero > 0 && sqParam.lvHero <= XGAME::MAX_HERO_LEVEL );
	XASSERT( sqParam.lvSkill > 0 && sqParam.lvSkill < XGAME::MAX_SKILL_LEVEL );
	XASSERT( sqParam.lvSquad > 0 && sqParam.lvSquad <= XGAME::MAX_SQUAD_LEVEL );
	XASSERT( !IsInvalidGrade(sqParam.grade) );
	auto pSquad = new XSquadron( pPropHero, sqParam.lvHero, sqParam.unit, sqParam.lvSquad );
	auto pHero = pSquad->GetpHero();
	if( sqParam.lvSkill > 0 ) {
		pHero->SetlvActive( sqParam.lvSkill );
		pHero->SetlvPassive( sqParam.lvSkill );
	}
	pHero->SetGrade( sqParam.grade );
	pSquad->SetmulAtk( sqParam.mulAtk );
	pSquad->SetmulHp( sqParam.mulHp );
#if defined(_CLIENT) || defined(_GAME_SERVER)
	for( auto& abil : sqParam.m_listAbil ) {
		pHero->SetAbilPoint( abil.m_idsAbil, abil.point );
	}
#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)

#ifdef _XSINGLE
	if( sqParam.m_listAbil.empty() && sqParam.m_numAbil > 0 ) {
		// 매뉴얼 지정한 특성이 없다면 랜덤으로 특성을 부여한다.
		auto listAbil = XPropTech::sGet()->GetaryUnitsAbil( sqParam.unit );
		XList4<XPropTech::xNodeAbil*> listCopy = listAbil;
		if( listAbil.size() > 0 ) {
			for( int i = 0; i < sqParam.m_numAbil; ++i ) {
				if( listCopy.empty() )
					break;
				auto pAbil = listCopy.PopFromRandom();
				if( pAbil ) {
					pHero->SetAbilPoint( pAbil->strSkill, xRandom( 1, 5 ) );
				}
			}
		}
	}
#endif // _XSINGLE
	return pSquad;
}
/**
 @brief 
*/
XSquadron* XLegion::sCreateSquadronForNPC2( const int lvLegion
																					, const XGAME::xSquad& sqParam )
{
	XBREAK( sqParam.idHero == 0 );
	auto pPropHero = PROP_HERO->GetpProp( sqParam.idHero );
	if( XASSERT(pPropHero) ) {
		return sCreateSquadronForNPC2( lvLegion, pPropHero, sqParam );
	}
	return nullptr;
}

#endif // defined(_XSINGLE) || !defined(_CLIENT)
/**
 @brief 유저레벨 level에 맞는 NPC부대를 생성시킨다.
 NPC전용으로만 써야 한다.
 @param level 유저레벨
 @param adjDiff -10~+10범위 난이도 보정치
 @note 레벨류는 랜덤요소를 넣지 말것. adjDiff로 정교하게 조절할것이므로
*/
XLegion* XLegion::sCreateLegionForNPC( int levelUser, int adjDiff,
									XGAME::xLegionParam& legionInfo )
{
	XBREAK( adjDiff < -10 || adjDiff > 10 );
	const auto& tblLegion = XGC->GetLegionTable( levelUser );
	int numSquadron = tblLegion.m_numSquad;
#ifdef _XSINGLE
	numSquadron = 15;
#endif
	XLegion *pLegion = new XLegion;
	pLegion->SetgradeLegion( legionInfo.x_gradeLegion );
	int maxHero = PROP_HERO->GetSize();
	int idx[ XGAME::MAX_SQUAD ] = {2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14};
	XBREAK( numSquadron > XNUM_ARRAY(idx) );
	XHero *pFirst = nullptr;
	// 이 부대가 랜덤선택가능한 유닛목록을 미리 뽑는다.
	XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> aryUnits;
	XLegion::sGetAvailableUnitByLevel( levelUser, &aryUnits );
	//
	for( int i = 0; i < numSquadron; ++i ) {
		int idxSquad = idx[i];
#ifdef _CLIENT
	#ifdef _XSINGLE
		int lvHero = 1;
		auto selectedUnit = XGAME::GetUnitRandomByFilter( legionInfo.x_filterUnit );
		int lvSquadLevel = 1;
//		XGAME::xtUnit unit = XLegion::sCreateUnitType( spawnType );
		// 첫번째 부대는 지정된 값으로
		if( idxSquad == 2 ) {
			selectedUnit = XGC->m_unitRight;
			if( selectedUnit == XGAME::xUNIT_NONE )
				selectedUnit = (XGAME::xtUnit)(1 + xRandom(XGAME::xUNIT_MAX-1));
			lvSquadLevel = XGC->m_lvSquadRight;
			if( lvSquadLevel <= 0 )
				lvSquadLevel = 1;
			lvHero = XGC->m_lvHero1;
			if( lvHero <= 0 )
				lvHero = 1;
			if( lvHero > XGAME::MAX_HERO_LEVEL )
				lvHero = XGAME::MAX_HERO_LEVEL;
		}
		// 선택된 유닛에 맞춰 영웅을 랜덤으로 뽑는다.
		ID idHero = XLegion::sGetHeroByUnit( selectedUnit );
		auto pPropHero = PROP_HERO->GetpProp( idHero );
		if( i == 0 ) {
			// 첫번째 부대의 경우 영웅이름이 지정되어있으면 해당영웅을 생성시키고 그에맞춰 유닛도 바꾼다.
			if( XGC->m_strHero2 != _T("random") && !XGC->m_strHero2.empty() ) {
				// 영웅이름이 지정되어있으면 영웅에 맞춰서 유닛을 바꾼다.
				pPropHero = PROP_HERO->GetpProp( XGC->m_strHero2 );
				// small~big사이의 typeAtk유닛을 랜덤으로 얻는다.
				selectedUnit = XGC->m_unitRight;
				if( pPropHero->typeAtk != XGAME::GetAtkType( selectedUnit ) ) {
					CONSOLE( "영웅(%s)의 공격타입(%d)과 지정한 유닛(%d)의 타입이 맞지 않아 자동변환됨.", pPropHero->GetstrName().c_str(), pPropHero->typeAtk, XGAME::GetAtkType( selectedUnit ) );
					// 영웅에 맞춰서 유닛(소형)을 뽑음.
					selectedUnit = XGAME::GetRandomUnit( pPropHero->typeAtk, XGAME::xSIZE_BIG );
				}
			}
			XBREAK( pPropHero == nullptr );
			idHero = pPropHero->idProp;
		}
		auto pSquad = new XSquadron( pPropHero, lvHero, selectedUnit, lvSquadLevel );
		pLegion->SetSquadron( idxSquad, pSquad, TRUE );
		if( XASSERT( pSquad ) )
			if( pFirst == nullptr )
				pLegion->SetpLeader( pFirst );
	#else
		XBREAK(1);	// 싱글모드가 아니면 클라에서 사용금지.
	#endif

#else
		// 필터링을 한다.
		auto selectedUnit = XGAME::xUNIT_NONE;
		if( legionInfo.x_filterUnit ) {
			// 유닛필터가 있다면 한번더 거른다.
			std::vector<XGAME::xtUnit> ary;
			XARRAYLINEARN_LOOP_AUTO( aryUnits, unit ) {
				if( XGAME::IsUnitFilter( unit, legionInfo.x_filterUnit ) )
					ary.push_back( unit );
			} END_LOOP;
			if( ary.size() ) 
				selectedUnit = ary[ xRandom( ary.size() ) ];
//			selectedUnit = XGAME::GetUnitRandomByFilter( legionInfo.x_filterUnit );
		}
		else
			selectedUnit = aryUnits.GetFromRandom();
		if( XASSERT(selectedUnit) ) {
			auto pSquad = sCreateSquadronForNPC( levelUser, 
																						adjDiff, 
																						selectedUnit, 
																						0, 
																						0, 
																						0, 
																						0, 
																						legionInfo.x_gradeLegion, 
																						&legionInfo );
			pLegion->SetSquadron( idx[i], pSquad, TRUE );
			if( XASSERT( pSquad ) )
				if( pFirst == nullptr )
					pLegion->SetpLeader( pFirst );
		}
#endif
	}
	// 원거리 부대등은 뒤로 보내는등의 조정을 한다.
#ifndef _XSINGLE
	// 1:1테스트에서 스왑됨.
	pLegion->AdjustLegion();
#endif // not single
	pLegion->MakeFogs( legionInfo.m_numVisible );
	return pLegion;
}
/**
 @brief squad정보에 의해 영웅 프로퍼티를 선택한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야한다.
*/
XPropHero::xPROP* XLegion::sGetpPropHeroByInfo( ID idHero
																							, XGAME::xtAttack atkType
																							, XGAME::xtUnit unit
																							, int lvLegion )
{
	XPropHero::xPROP *pPropHero = nullptr;
	if( idHero == 0 ) {
		// 영웅이 따로 지정되어있지 않다면 병과를 기준으로 랜덤생성한다.(atkType이 0이면 내부에서 모든타입을 대상으로 찾음)
		if( atkType ) {
			pPropHero = PROP_HERO->GetPropRandomByAtkType( atkType, lvLegion );
		} else {
			if( unit == 0 )
				pPropHero = PROP_HERO->GetPropRandomByAtkType( atkType, lvLegion );	// 병과까지 랜덤으로 영웅을 뽑음.
			else
				pPropHero = PROP_HERO->GetPropRandomByAtkType( XGAME::GetAtkType(unit), lvLegion ); // 지정된 유닛의 병과에 맞춰서 영웅을 뽑음.
		}
	} else {
		pPropHero = PROP_HERO->GetpProp( idHero );
	}
	XBREAK( pPropHero == nullptr );
	return pPropHero;
}

/**
 @brief squad정보에 의해 유닛을 선택한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야한다.
*/
XGAME::xtUnit XLegion::sGetUnitByInfo( XGAME::xtUnit unit
																		, XGAME::xtSize sizeUnit
																		, XGAME::xtAttack typeAtk
																		, int lvLegion )
{
	XBREAK( !typeAtk );		//pPropHero를 안받는 대신 이건 꼭 있어야 함.
	if( unit == 0 ) {		// 유닛이 따로 지정되어있지 않으면 랜덤으로 선택.
		if( lvLegion > 0 ) {
			// 군단레벨을 참조하도록 되어있으면 그 레벨에서 가질수 있는 유닛목록에서 뽑는다.
			XVector<XGAME::xtUnit> aryUnits;
			XLegion::sGetAvailableUnitByLevel( lvLegion, &aryUnits );
			// 영웅의 타입과 맞는 유닛만 추려낸다.
			XList4<XGAME::xtUnit> listUnits;
			for( auto unitAble : aryUnits ) {
				if( typeAtk == XGAME::GetAtkType( unitAble ) )
					listUnits.Add( unitAble );
			}
			XBREAK( listUnits.size() == 0 );
			unit = *(listUnits.GetpFromRandom());	// 그중 랜덤으로 선택한다.
		} else {
			// 영웅의 공격타입을 기준으로 랜덤유닛 선택
			auto 
			filter = XGAME::AddUnitFilter( typeAtk );
			// size_unit을 기준으로 랜덤유닛 선택
			if( sizeUnit )
				filter = XGAME::AddUnitFilter( sizeUnit, filter );
			unit = XGAME::GetUnitRandomByFilter( filter );
		}
	} else {
		XBREAK( typeAtk != XGAME::GetAtkType( unit ) );
		if( sizeUnit ) {
			// 사이즈가 명시되어있을때 명시된 유닛과 사이즈가 맞지않으면 에러
			XBREAK( XGAME::GetSizeUnit( unit ) != sizeUnit );
		}
	}
	XBREAK( XGAME::IsInvalidUnit(unit) );
	return unit;
}
/**
 @brief squad정보에 의해 영웅 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvHeroByInfo( int lvHero, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	if( lvHero == 0 ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		lvHero = tblLegion.m_lvHero;
	}
	XBREAK( lvHero == 0 );
	return lvHero;
}
/**
 @brief squad정보에 의해 영웅 스킬 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvSkillByInfo( int lvSkill, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	if( lvSkill == 0 ) {
		lvSkill = 1;
	}
	XBREAK( lvSkill == 0 );
	return lvSkill;
}

/**
 @brief squad정보에 의해 영웅 부대 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvSquadByInfo( int lvSquad, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	if( lvSquad == 0 ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		lvSquad = tblLegion.m_lvSquad;
	}
	XBREAK( lvSquad == 0 );
	return lvSquad;
}

/**
 @brief squad정보에 의해 영웅 부대 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
XGAME::xtGrade XLegion::sGetGradeHeroByInfo( XGAME::xtGrade gradeHero, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	if( !gradeHero ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		gradeHero = tblLegion.m_gradeHero;
	}
	XBREAK( IsInvalidGrade(gradeHero) );
	return gradeHero;
}

// 앞으로 squad를 직접 받는 시리즈는 쓰지 말것.
XPropHero::xPROP* XLegion::sGetpPropHeroByInfo( const XGAME::xSquad& squad, int lvLegion )
{
	XPropHero::xPROP *pPropHero = nullptr;
	if( squad.idHero == 0 ) {
		// 영웅이 따로 지정되어있지 않다면 병과를 기준으로 랜덤생성한다.(atkType이 0이면 내부에서 모든타입을 대상으로 찾음)
		if( squad.atkType ) {
			pPropHero = PROP_HERO->GetPropRandomByAtkType( squad.atkType, lvLegion );
		} else {
			if( squad.unit == 0 )
				pPropHero = PROP_HERO->GetPropRandomByAtkType( squad.atkType, lvLegion );	// 병과까지 랜덤으로 영웅을 뽑음.
			else
				pPropHero = PROP_HERO->GetPropRandomByAtkType( XGAME::GetAtkType(squad.unit), lvLegion ); // 지정된 유닛의 병과에 맞춰서 영웅을 뽑음.
		}
	} else {
		pPropHero = PROP_HERO->GetpProp( squad.idHero );
	}
	XBREAK( pPropHero == nullptr );
	return pPropHero;
}

/**
 @brief squad정보에 의해 유닛을 선택한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야한다.
*/
XGAME::xtUnit XLegion::sGetUnitByInfo( const XGAME::xSquad& squad, XPropHero::xPROP *pPropHero, int lvLegion )
{
	XBREAK( pPropHero == nullptr );
	auto unit = squad.unit;
	if( unit == 0 ) {		// 유닛이 따로 지정되어있지 않으면 랜덤으로 선택.
		if( lvLegion > 0 ) {
			// 군단레벨을 참조하도록 되어있으면 그 레벨에서 가질수 있는 유닛목록에서 뽑는다.
			XVector<XGAME::xtUnit> aryUnits;
			XLegion::sGetAvailableUnitByLevel( lvLegion, &aryUnits );
			// 영웅의 타입과 맞는 유닛만 추려낸다.
			XList4<XGAME::xtUnit> listUnits;
			for( auto unitAble : aryUnits ) {
				if( pPropHero->typeAtk == XGAME::GetAtkType( unitAble ) )
					listUnits.Add( unitAble );
			}
			XBREAK( listUnits.size() == 0 );
			unit = *(listUnits.GetpFromRandom());	// 그중 랜덤으로 선택한다.
		} else {
			// 영웅의 공격타입을 기준으로 랜덤유닛 선택
			auto 
			filter = XGAME::AddUnitFilter( pPropHero->typeAtk );
			// size_unit을 기준으로 랜덤유닛 선택
			if( squad.sizeUnit )
				filter = XGAME::AddUnitFilter( squad.sizeUnit, filter );
			unit = XGAME::GetUnitRandomByFilter( filter );
		}
	} else {
		XBREAK( pPropHero->typeAtk != XGAME::GetAtkType( unit ) );
// 				if( pPropHeroOrig ) {
// 					// 영웅이 명시되어 있을때 영웅의 공격타입과 유닛의 공격타입이 맞지 않으면 에러
// 					XBREAK( XGAME::GetAtkType( unit ) != pPropHeroOrig->typeAtk );
// 				}
		if( squad.sizeUnit ) {
			// 사이즈가 명시되어있을때 명시된 유닛과 사이즈가 맞지않으면 에러
			XBREAK( XGAME::GetSizeUnit( unit ) != squad.sizeUnit );
		}
	}
	XBREAK( XGAME::IsInvalidUnit(unit) );
	return unit;
}
/**
 @brief squad정보에 의해 영웅 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvHeroByInfo( const XGAME::xSquad& squad, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	int lvHero = squad.lvHero;
	if( lvHero == 0 ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		lvHero = tblLegion.m_lvHero;
	}
	XBREAK( lvHero == 0 );
	return lvHero;
}
/**
 @brief squad정보에 의해 영웅 스킬 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvSkillByInfo( const XGAME::xSquad& squad, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	int lvSkill = squad.lvSkill;
	if( lvSkill == 0 ) {
		lvSkill = 1;
	}
	XBREAK( lvSkill == 0 );
	return lvSkill;
}

/**
 @brief squad정보에 의해 영웅 부대 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
int XLegion::sGetLvSquadByInfo( const XGAME::xSquad& squad, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	int lvSquad = squad.lvSquad;
	if( lvSquad == 0 ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		lvSquad = tblLegion.m_lvSquad;
	}
	XBREAK( lvSquad == 0 );
	return lvSquad;
}

/**
 @brief squad정보에 의해 영웅 부대 레벨을 결정한다.
 @return 리턴값은 반드시 유효한 값을 돌려줘야 한다.
*/
XGAME::xtGrade XLegion::sGetGradeHeroByInfo( const XGAME::xSquad& squad, int lvLegion )
{
	XBREAK( lvLegion == 0 );
	auto gradeHero = squad.grade;
	if( !gradeHero ) {
		const auto& tblLegion = XGC->GetLegionTable( lvLegion );
		gradeHero = tblLegion.m_gradeHero;
	}
	XBREAK( IsInvalidGrade(gradeHero) );
	return gradeHero;
}

// void XLegion::sSetLeaderByInfo( const XGAME::xLegion& legion, XSPLegion spLegion, XSquadron *pSquad )
void XLegion::sSetLeaderByInfo( const XGAME::xLegion* pxLegion, XSPLegion spLegion, XSquadron *pSquad )
{
	if( pSquad == nullptr ) 
		return;
	// 리더 영웅 지정
	auto pHero = pSquad->GetpHero();
	if( XASSERT( pHero ) ) {
		if( pxLegion->idBoss ) {
			if( pHero->GetidProp() == pxLegion->idBoss )
				spLegion->SetpLeader( pHero );
		}
	} // pHero
}

/**
 @brief 리더가 지정되지 않았을경우 슬롯순서에 맞춰 첫번째 부대가 리더로 지정된다.
*/
void XLegion::SetAutoLeader()
{
	XBREAK( m_pLeader != nullptr );
	const int idx[ XGAME::MAX_SQUAD ] = {2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14};
	for( int i = 0; i < m_arySquadrons.GetMax(); ++i ) {
		auto pSquad = m_arySquadrons[ idx[i] ];
		if( pSquad ) {
			m_pLeader = pSquad->GetpHero();
			XBREAK( m_pLeader == nullptr );
			return;
		}
	}
}

void XLegion::AdjustLegion()
{
	int idx = 0;
	XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
		if( pSquad ) {
			// 원거리 유닛이 앞줄에 있으면.
			if( pSquad->GetAtkType() == XGAME::xAT_RANGE && idx < 5 ) {
				// 자리를 서로 바꿔준다.
				std::swap( m_arySquadrons[idx + 5], m_arySquadrons[idx] );
			} else
			// 스피드유닛이 1,2째줄에 있으면 3번째 줄로 빼준다.
			if( pSquad->GetAtkType() == XGAME::xAT_SPEED && idx < 10 ) {
				if( idx < 5 )
					std::swap( m_arySquadrons[ idx + 10 ], m_arySquadrons[ idx ] );
				else if( idx < 10 )
					std::swap( m_arySquadrons[ idx + 5 ], m_arySquadrons[ idx ] );
			} else
			// 근접유닛이 뒷열에 있으면 앞열로 옮긴다.
			if( pSquad->GetAtkType() == XGAME::xAT_TANKER && idx >= 5 ) {
				std::swap( m_arySquadrons[ idx % 5 ], m_arySquadrons[ idx ] );
			}

		}
		++idx;
	} END_LOOP;
}

XGAME::xtGrade XLegion::sGetRandomGradeHeroByTable( int levelUser, XGAME::xtGradeLegion gradeLegion )
{
	XBREAK(1);	// 여기 들어옴?
	XGAME::xtGrade grade = XGAME::xGD_COMMON;
	if( gradeLegion == XGAME::xGL_ELITE ||
		gradeLegion == XGAME::xGL_RAID ) {
		// 정예는 무조건 4성 이상.
		grade = XGAME::xGD_EPIC;
	} else if( levelUser <= 5 )	{
		// 1등급만
		grade = XGAME::xGD_COMMON;
	} else if( levelUser <= 10 ) {
		// 1,2등급
// 		XArrayLinearN<float, 2> aryChance;
// 		aryChance.Add( 0.5f );
// 		aryChance.Add( 0.5f );
// 		int idxSel = XE::GetDiceChance( aryChance );
// 		grade = ( XGAME::xtGrade )( XGAME::xGD_COMMON + idxSel );
		grade = XGAME::xGD_VETERAN;
	} else if( levelUser <= 20 )	{
		// 1,2,3등급
// 		XArrayLinearN<float, 3> aryChance;
// 		aryChance.Add( 0.3f );
// 		aryChance.Add( 0.4f );
// 		aryChance.Add( 0.3f );
// 		int idxSel = XE::GetDiceChance( aryChance );
// 		grade = ( XGAME::xtGrade )( XGAME::xGD_COMMON + idxSel );
		grade = XGAME::xGD_RARE;
	} else
		grade = XGAME::xGD_RARE;
// 	else if( levelUser <= 20 ) {
// 		// 2,3등급
// // 		XArrayLinearN<float, 2> aryChance;
// // 		aryChance.Add( 0.5f );
// // 		aryChance.Add( 0.5f );
// // 		int idxSel = XE::GetDiceChance( aryChance );
// // 		grade = ( XGAME::xtGrade )( XGAME::xGD_VETERAN + idxSel );
// 	} else	{
// 		// 3등급만.
// 		grade = XGAME::xGD_RARE;
// 	}
	return grade;
}

XGAME::xtUnit XLegion::sCreateUnitType( int spawnType )
{
	XGAME::xtUnit unit = XGAME::xUNIT_NONE;
	if( spawnType == 0 )
		unit = ( XGAME::xtUnit )( 1 + random( XGAME::xUNIT_MAX - 1 ) );
	else
	if( spawnType == 1 )
		unit = ( XGAME::xtUnit )( XGAME::xUNIT_SPEARMAN + random( 3 ) );
	else
	if( spawnType == 2 )
		unit = ( XGAME::xtUnit )( XGAME::xUNIT_MINOTAUR + random( 3 ) );
	else
	if( spawnType == 3 )
		unit = ( XGAME::xtUnit )( XGAME::xUNIT_GOLEM + random( 3 ) );
	else
	// 소형~중형
	if( spawnType == 4 )
		unit = ( XGAME::xtUnit )( XGAME::xUNIT_SPEARMAN + random( 6 ) );	
	else
	{
		XBREAK( 1 );
	}
	return unit;
}
/**
 @brief 유닛 레벨(lvUnit)에 따른 각 사이즈별 유닛수를 돌려준다.
*/
int XLegion::sGetNumUnitByLevel( XGAME::xtUnit unit, int lvUnit )
{
	int numUnit = 1;
	switch( XGAME::GetSizeUnit(unit) )
	{
	case XGAME::xSIZE_SMALL:	numUnit = 5 + lvUnit;	break;
	case XGAME::xSIZE_MIDDLE:	numUnit = lvUnit / 5;	break;
	}
	if( numUnit < 1 )
		numUnit = 1;
	return numUnit;
}
//////////////////////////////////////////////////////////////////////////
XLegion* XLegion::sCreateDeserializeFull( XArchive& ar )
{
	int verLegion;
	BYTE b0;
	ar >> b0;	verLegion = b0;
	if( verLegion == 0 )
	{
		ar >> b0 >> b0 >> b0;
		return nullptr;
	}
	XLegion *pLegion = new XLegion;
	BOOL bRet = pLegion->DeSerializeFull( ar, verLegion );
	if( bRet == FALSE )
		SAFE_DELETE( pLegion );
	return pLegion;
}

/**
 @brief 부대의 생존여부만 아카이빙되어있는 
*/
// XLegion* XLegion::sCreateDeserializeForGuildRaid( XArchive& ar )
// {
// 
// 	auto pLegion = new XLegion;
// 	int numSquads = pLegion->DeserializeForGuildRaid( ar );
// 	if( numSquads == 0 ) {
// 		SAFE_DELETE( pLegion );
// 	} else {
// 		return XSPLegion( pLegion );
// 	}
// }
//////////////////////////////////////////////////////////////////////////
XLegion::XLegion() 
{ 
	Init(); 
}

void XLegion::Destroy()
{
	XARRAYN_DESTROY( m_arySquadrons );
}

void XLegion::Serialize( XArchive& ar )
{
	int size = m_arySquadrons.GetMax();
	ar << (BYTE)size;
	XBREAK( m_gradeLegion > 0xff );
	ar << (BYTE)m_gradeLegion;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << m_snLegion;
	ar << m_RateAtk;
	ar << m_RateHp;
	ar << m_aryResourceHero;
	for( int i = 0; i < size; ++i ) {
		if( m_arySquadrons[i] ) {
			ar << 1;
			m_arySquadrons[i]->Serialize(ar);
		} else {
			ar << 0;
		}
	}
	if( m_pLeader )
		ar << m_pLeader->GetsnHero();
	else
		ar << 0;		// 부대가 없다면 영웅이 없을수도 있음.
	SerializeFogs( ar );
	MAKE_CHECKSUM( ar );
}

BOOL XLegion::DeSerialize( XArchive& ar, XSPAcc spAcc, int verLegion ) 
{
	BYTE b0;
	int size;
	ar >> b0;	size = b0;
	ar >> b0;	m_gradeLegion = (XGAME::xtGradeLegion)b0;
	ar >> b0 >> b0;
	ar >> m_snLegion;
	if( verLegion >= 7 ) {
		ar >> m_RateAtk;
		ar >> m_RateHp;
	}
	if( verLegion >= 5 )
		ar >> m_aryResourceHero;
	int fill = 0;
	XHero *pFirst = nullptr;
	for( int i = 0; i < size; ++i )  {
		SAFE_DELETE( m_arySquadrons[i] );	// 이미 데이타가 있으면 삭제하고 넣는다.
		ar >> fill;
		if( fill == 1 ) {
			XSquadron *pSquad = new XSquadron;
			if( pSquad->DeSerialize( ar, spAcc, verLegion ) == FALSE )
				return FALSE;
			m_arySquadrons[ i ] = pSquad;
			if( pFirst == nullptr )
				pFirst = pSquad->GetpHero();
		} else {
			XBREAK( fill != 0 );
		}
	}
	{
		ID snLeader;
		ar >> snLeader;
		if( snLeader ) {
			XHero *pHero = spAcc->GetHero( snLeader );
			if( XBREAK( pHero == nullptr ) )
				return FALSE;
			SetpLeader( pHero );
		} else
			SetpLeader( nullptr );
	}
	if( verLegion >= 6 ) {
		DeSerializeFogs( ar, verLegion );
	}
	RESTORE_VERIFY_CHECKSUM( ar );
	return TRUE;
}

int XLegion::SerializeFogs( XArchive& ar )
{
	ar << (int)m_listFogs.size();
	for( auto pSq : m_listFogs ) {
		if( XASSERT( pSq ) )
			if( XASSERT( pSq->GetpHero() ) )
				ar << pSq->GetpHero()->GetsnHero();
	}
	return 1;
}
int XLegion::DeSerializeFogs( XArchive& ar, int verLegion )
{
	XList4<ID> listFogs;
	int sizeFogs;
	ar >> sizeFogs;
	for( int i = 0; i < sizeFogs; ++i ) {
		ID snHero;
		ar >> snHero;
		listFogs.Add( snHero );
	}
	m_listFogs.clear();
	for( auto snHero : listFogs ) {
		auto pSq = GetSquadronByHeroSN( snHero );
		if( XASSERT( pSq ) ) {
			m_listFogs.Add( pSq );
		}
	}
	return 1;
}

int XLegion::sSerializeFull( XArchive& ar, XSPLegion spLegion )
{
	if( spLegion == nullptr ) {
		ar << 0;
		return 1;
	}
	spLegion->SerializeFull( ar );
	return 1;
}

/**
 타 유저의 군대를 받을때 쓴다. 영웅의 모든정보를 담는다.
*/
void XLegion::SerializeFull( XArchive& ar )
{
	ar << (BYTE)VER_LEGION_SERIALIZE;
	int size = m_arySquadrons.GetMax();
	ar << (BYTE)size;
	ar << (BYTE)66;	// full serialize 표시
	XBREAK( m_gradeLegion > 0xff );
	ar << (BYTE)m_gradeLegion;
	ar << m_snLegion;
	ar << m_RateAtk;
	ar << m_RateHp;
	ar << m_aryResourceHero;
	for( int i = 0; i < size; ++i ) {
		if( m_arySquadrons[i] ) {
			ar << 1;
			m_arySquadrons[ i ]->SerializeFull( ar );
		} else
			ar << 0;
	}
	if( m_pLeader )
		ar << m_pLeader->GetsnHero();
	else
		ar << 0;		// 부대가 없다면 영웅이 없을수도 있음.
	SerializeFogs( ar );
	MAKE_CHECKSUM( ar );
}

/**
 타 유저 부대정보 가져오는 용도
*/
BOOL XLegion::DeSerializeFull( XArchive& ar, int verLegion ) 
{

	BYTE b0;
	int size, fill;
	ar >> b0;	size = b0;
	ar >> b0;
	XBREAK( b0 != 66 );
	ar >> b0;	m_gradeLegion = (XGAME::xtGradeLegion)b0;
	ar >> m_snLegion;
	if( verLegion >= 7 ) {
		ar >> m_RateAtk;
		ar >> m_RateHp;
	}
	if( verLegion >= 5 )
		ar >> m_aryResourceHero;
	XHero *pFirst = nullptr;
	//
	for( int i = 0; i < size; ++i ) {
		SAFE_DELETE( m_arySquadrons[ i ] );	// 이미 데이타가 있으면 삭제하고 넣는다.
		ar >> fill;
		if( fill == 1 ) {
			XSquadron *pSquad = new XSquadron;
			if( pSquad->DeSerializeFull( ar, verLegion ) == FALSE )
				return FALSE;
			m_arySquadrons[ i ] = pSquad;
			if( pFirst == nullptr )
				pFirst = pSquad->GetpHero();
		} else {
			if( XBREAK( fill != 0 ) )
				return FALSE;
		}
	}
	{
		ID snLeader;
		ar >> snLeader;
		if( snLeader ) {
			auto pSquad = GetSquadronByHeroSN( snLeader );
			if( XBREAK( pSquad == nullptr ) )
				return FALSE;
			XBREAK( pSquad->GetpHero() == nullptr );
			SetpLeader( pSquad->GetpHero() );
		}
		else
			SetpLeader( nullptr );
	}
	if( verLegion >= 6 )
		DeSerializeFogs( ar, verLegion );
	RESTORE_VERIFY_CHECKSUM( ar );
	return TRUE;
}

/**
this가 풀버전 군단객체일때 링크버전 군단객체를 spAcc를 참조하여 생성해서 돌려준다.
영웅정보가 풀로 저장되어 있으므로 snHero만 취하고 나머지 영웅정보는 버린다.
*/
XLegion* XLegion::CreateLegionForLink( XSPAcc spAcc )
{
	// 빈 군단 객체를 만듬
	XLegion *pNewLegion = new XLegion( m_snLegion );
	// 중대 어레이를 루프
	XARRAYN_LOOP_IDX( m_arySquadrons, XSquadron*, i, pSq )
	{
		if( pSq )
		{
			// 중대영웅의 snHero를 가져와서 계정의 영웅리스트에서 영웅 포인트를 찾음.
			ID snHero = pSq->GetpHero()->GetsnHero();
			XHero *pHero = spAcc->GetHero( snHero );
			if( XASSERT( pHero ) )
			{
				// 그 영웅포인트로 중대객체를 다시 만듬.
				//			XSquadron sq( pHero );
				XSquadron *pSq = new XSquadron( pHero );
				pNewLegion->SetSquadron( i, pSq, FALSE );
				if (m_pLeader && pHero->GetsnHero() == m_pLeader->GetsnHero())
					pNewLegion->SetpLeader(pHero);
			}
		}

	} END_LOOP;
	return pNewLegion;
}


// 부대 수를 얻는다.
int XLegion::GetNumSquadrons( void ) 
{
	int size = m_arySquadrons.GetMax();
	int num = 0;
	for( int i = 0; i < size; ++i ) {
//		if( m_arySquadrons[i]->GetidPropHero() )
		if( m_arySquadrons[ i ] )
			++num;
	}
	return num;
}

ID XLegion::sGetHeroByUnit( XGAME::xtUnit unit )
{
	ID idHeros[ 9 ] = {101, 101, 101, 1000, 1002, 1004, 2000, 2002, 2004};
	int idx = 0;
	XPropUnit::xPROP *pPropUnit = PROP_UNIT->GetpProp( unit );
	XBREAK( pPropUnit == NULL );
	if( pPropUnit->IsTanker() ) {
		idx = 0 + random( 3 );
		auto pProp = PROP_HERO->GetpProp( idHeros[ idx ] );
		if( XASSERT( pProp ) ) {
			XASSERT( pProp->IsTanker() );
		}
	} else
	if( pPropUnit->IsRange() ) {
		idx = 3 + random( 3 );
		auto pProp = PROP_HERO->GetpProp( idHeros[idx] );
		if( XASSERT(pProp) ) {
			XASSERT( pProp->IsRange() );
		}
	} else
	if( pPropUnit->IsSpeed() ) {
		idx = 6 + random( 3 );
		auto pProp = PROP_HERO->GetpProp( idHeros[ idx ] );
		if( XASSERT( pProp ) ) {
			XASSERT( pProp->IsSpeed() );
		}
	} else {
		XBREAK(1);
	}

	return idHeros[idx];
}

/**
 @brief 영웅 sn번호로 부대를 찾는다.
*/
XSquadron* XLegion::GetSquadronByHeroSN( ID snHero ) const
{
	int size = m_arySquadrons.GetMax();
	for( int i = 0; i < size; ++i ) {
		const auto pSquad = m_arySquadrons.At(i);
		if( pSquad ) {
			auto pHero = pSquad->GetpHero();
			if( pHero->GetsnHero() == snHero )
				return m_arySquadrons.At(i);
		}
	}
	return nullptr;
}

/**
 @brief 영웅sn번호로 슬롯 인덱스를 얻는다.
*/
int XLegion::GetSquadronIdxByHeroSN( ID snHero )
{
	int size = m_arySquadrons.GetMax();
	for( int i = 0; i < size; ++i )
		if( m_arySquadrons[ i ] && m_arySquadrons[ i ]->GetpHero()->GetsnHero() == snHero )
			return i;
	return -1;
}

XHero* XLegion::GetpHeroBySN( ID snHero ) const
{
	auto pSquad = GetSquadronByHeroSN( snHero );
	if( pSquad )
		return pSquad->GetpHero();
	return nullptr;
}

XHero* XLegion::GetpHeroByIdxSquad( int idxSquad )
{
	if( idxSquad < 0 )
		return nullptr;
	auto pSquad = GetSquadron( idxSquad );
	if( pSquad )
		return pSquad->GetpHero();
	return nullptr;
}
/**
 @brief 리더의ㅏ 부대 인덱스를 얻는다.
*/
int XLegion::GetIdxSquadByLeader()
{
	if( m_pLeader )
		return GetSquadronIdxByHeroSN(m_pLeader->GetsnHero() );
	return -1;
}
/**
 @brief pHeroSrc를 가진 부대와 pHeroDst를 가진 부대의 슬롯을 서로 바꾼다
*/
BOOL XLegion::SwapSlotSquad( XHero *pHeroSrc, XHero *pHeroDst )
{
	XBREAK( pHeroSrc == nullptr );
	XBREAK( pHeroDst == nullptr );
	int idxSrc = GetSquadronIdxByHeroSN( pHeroSrc->GetsnHero() );
	if( idxSrc < 0 )
		return FALSE;
	int idxDst = GetSquadronIdxByHeroSN( pHeroDst->GetsnHero() );
	if( idxDst < 0 )
		return FALSE;
	std::swap( m_arySquadrons[idxSrc], m_arySquadrons[idxDst] );
	return TRUE;
}

BOOL XLegion::SwapSlotSquad( int idxSrc, int idxDst )
{
	if( idxSrc < 0 || idxSrc >= m_arySquadrons.GetMax() )
		return FALSE;
	if( idxDst < 0 || idxDst >= m_arySquadrons.GetMax() )
		return FALSE;
	std::swap( m_arySquadrons[ idxSrc ], m_arySquadrons[ idxDst ] );
	return TRUE;
}

BOOL XLegion::RemoveSquad( ID snHero )
{
	int idx = GetSquadronIdxByHeroSN( snHero );
	if( idx < 0 )
		return FALSE;
	if (m_arySquadrons[idx] != nullptr && m_arySquadrons[idx]->GetpHero() == m_pLeader)
		m_pLeader = nullptr;
	SAFE_DELETE( m_arySquadrons[ idx ] );
	return TRUE;
}
BOOL XLegion::RemoveSquad( XHero *pHero )
{
	return RemoveSquad( pHero->GetsnHero() );
}

BOOL XLegion::RemoveSquad( int idx )
{
	if (m_arySquadrons[idx] != nullptr && m_arySquadrons[idx]->GetpHero() == m_pLeader)
		m_pLeader = nullptr;
	SAFE_DELETE( m_arySquadrons[ idx ] );
	return TRUE;
}

/**
 @brief 군단내 영웅들의 리스트를 돌려준다.
*/
int XLegion::GetHerosToAry( XArrayLinearN<XHero*, XGAME::MAX_SQUAD>& aryOut )
{
	XARRAYN_LOOP( m_arySquadrons, XSquadron*, pSq ) {
		if( pSq ) {
			XBREAK( pSq->GetpHero() == NULL );
			aryOut.Add( pSq->GetpHero() );
		}
	} END_LOOP;
	return aryOut.size();
}
int XLegion::GetHerosToAry( XVector<XHero*> *pOutAry )
{
	XARRAYN_LOOP( m_arySquadrons, XSquadron*, pSq ) {
		if( pSq ) {
			XBREAK( pSq->GetpHero() == NULL );
			pOutAry->push_back( pSq->GetpHero() );
		}
	} END_LOOP;
	return pOutAry->size();
}

/**
 @brief 방금 레벨업한 영웅들 리스트를 받아준다.
 만약 pAryOut이 NULL이면 렙업한 영웅들 수만 돌려준다.
 이 함수를 수행하고 나면 XHero::m_Level::m_bLevelUp 은 자동으로 false가 된다.
*/
int XLegion::GetLevelUpHerosToAry( XArrayLinearN<XHero*, XGAME::MAX_SQUAD> *pAryOut )
{
	int num = 0;
	XARRAYN_LOOP( m_arySquadrons, XSquadron*, pSq ) {
		if( pSq ) {
			XBREAK( pSq->GetpHero() == NULL );
			BOOL bLvUp = pSq->GetpHero()->GetbLevelUpAndClear( XGAME::xTR_LEVEL_UP );
			if( bLvUp ) {
				++num;
				if( pAryOut )
					pAryOut->Add( pSq->GetpHero() );
			}
		}
	} END_LOOP;
	return num;
}

/**
 @brief 더미용 함수
 지나치게 높은 레벨의 더미유저가 나오는걸 방지
 유저의 경우에도 버그등으로 인해 최대치보다 높은레벨을 가지고 있는 유저가 있다면 레벨을 보정해준다.
*/
void XLegion::DummyDataRechange( int levelAcc, XAccount *pAcc )
{
#if defined(_DEV) && defined(_GAME_SERVER)
	// 이 부대가 랜덤선택가능한 유닛목록을 미리 뽑는다.
//  sGetAvailableUnitByLevel는 랜덤이므로 더미 만들당시 리스트와 지금 리스트가 다를수 있다. 그러므로 언락된 유닛리스트로 비교해야 한다.
// 	XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> aryUnits;
// 	XLegion::sGetAvailableUnitByLevel( levelAcc, &aryUnits );
// 	int size = m_arySquadrons.GetMax();
// 	for( int i = 0; i < size; ++i ) {
// 		XSquadron *pSq = m_arySquadrons[i];
// 		if( pSq ) {
// 			auto pHero = pSq->GetpHero();
// 			if( XASSERT(pHero) ) {
// 				BOOL bFind = aryUnits.Find( pHero->GetUnit() );
// 				if( !bFind ) {
// 					// 가능한 유닛목록에 없는 유닛을 가졌을경우 군단데이타 다시 만듬.
// 					XBREAK(1); // DB클리어했기때문에 이런건 없어야함.
// // 					m_arySquadrons[i] = nullptr;
// // 					XSquadron *pSquad = nullptr;
// // 					auto unit = aryUnits.GetFromRandom();
// // 					XGAME::xtGrade grade = pHero->GetGradeMax();
// // 					bool bLeader = false;
// // 					if( GetpLeader() )
// // 						if( GetpLeader()->GetsnHero() == pHero->GetsnHero() )
// // 							bLeader = true;
// // 					pSquad = pAcc->CreateSquadronByRandom( this, 
// // 															i, 
// // 															levelAcc, 
// // 															nullptr, 
// // 															grade, 
// // 															unit );
// // 					SetpLeader( pSquad->GetpHero() );
// // 					// 구 데이타 삭제
// // 					SAFE_DELETE( pSq );
// 				}
// 			}
// 		}
// 	}
// 
// 	XARRAYN_LOOP( m_arySquadrons, XSquadron*, pSq ) {
// 		if( pSq ) {
// 			XBREAK( pSq->GetpHero() == NULL );
// 			auto pHero = pSq->GetpHero();
// 			// 영웅의 레벨이 군주레벨보다 높으면 군주레벨로 보정한다.
// 			if( pHero->GetLevel() > levelAcc ) {
// 				XBREAK( 1 ); // DB클리어했기때문에 이런건 없어야함.
// // 				XBREAK( levelAcc <= 0 );
// // 				pHero->SetLevel( levelAcc );
// 			}
// 			int levelSquadMax = sGetSquadMaxLevelByHeroLevel( pHero->GetLevel() );
// 			XBREAK( levelSquadMax > PROP_SQUAD->GetMaxLevel() );
// 			// 영웅의 부대레벨이 최대치보다 크다면 보정함.
// 			if( pHero->GetlevelSquad() > levelSquadMax ) {
// 				XBREAK( 1 ); // DB클리어했기때문에 이런건 없어야함.
// // 				auto pPropSquad = PROP_SQUAD->GetTable( levelSquadMax );
// // 				int maxUnit = pPropSquad.GetMaxUnit( pHero->GetUnit() );
// // 				pHero->SetlevelSquad( levelSquadMax );
// // 				pHero->SetnumUnit( maxUnit );
// // 				pHero->SetlevelSquad( levelSquadMax );
// 			}
// 		}
// 	} END_LOOP;
#endif // _DEV
}

/**
 @brief spLegion의 군사력을 얻는다.
*/
int XLegion::sGetMilitaryPower( XSPLegion spLegion )
{
	XBREAK( spLegion == nullptr );
	float score = 0;
	XArrayLinearN<XHero*, XGAME::MAX_SQUAD> aryHeroes;
	spLegion->GetHerosToAry( aryHeroes );
	int scoreMax = 0;
	// 메인 군단에 속한 영웅들의 전투력 합산
	XARRAYLINEARN_LOOP_AUTO( aryHeroes, pHero ) {
		// 해당 영웅이 현재 선택중인 유닛의 특성포인트만 적용한다.
// 		int numAbilPoint = 0;
// 		if( pAcc )
// 			numAbilPoint = pAcc->GetNumSetAbilPoint( pHero->GetUnit() );
// 		if( XBREAK( numAbilPoint > 25 ) )
// 			numAbilPoint = 25;
		auto scoreHero = sGetMilitaryPower( pHero );
		if( scoreHero > scoreMax )
			scoreMax = scoreHero;     // 가장 전투력이 쎈 영웅의 값을 받아둔다.
		score += scoreHero;
	} END_LOOP;
	// 만약 부대를 다채우지 않았다면 가장쎈 영웅의 전투력으로 메운다.
// 	if( pAcc ) {
// 		// 이거는 여기들어오면 안될것같다. 내 계정 저장할때만 쓰이는거라...
// 		int numRemain = XAccount::sGetMaxSquadByLevel( pAcc->GetLevel() ) - spLegion->GetNumSquadrons();
// 		if( numRemain > 0 ) {
// 			score += ( scoreMax * numRemain );
// 		}
// 	}
	return (int)score;
}

// 유닛전투력 기본상수
const float x_scoreUnit = 100.f;
// 영웅 레벨별 표준 스탯증가치
const float x_baseAddRateHero[ XGAME::xGD_MAX ] = {0, 0.01f, 0.028f, 0.056f, 0.099f, 0.165f};
// 1렙영웅의 표준 기본 스탯
const float x_baseStatHero[ XGAME::xGD_MAX ] = {0, 1.5f, 1.63f, 1.75f, 1.88f, 2.0f};
// 무기의 등급별 표준 증폭치
const float x_baseWeapon[ XGAME::xGD_MAX ] = { 0, 0.01f, 0.0266f, 0.0707f, 0.188f, 0.5f };
// 머리,가슴,다리방어구의 등급별 표준 증폭치
const float x_baseArmor[ XGAME::xGD_MAX ] = {0, 0.01f, 0.024f, 0.0577f, 0.1387f, 0.3333f};
// 장신구의 등급별 표준 증폭치
const float x_baseAcce[ XGAME::xGD_MAX ] = {0, 0.01f, 0.0234f, 0.0548f, 0.1282f, 0.3f };
// 스킬레벨별 표준 능력치
const float x_baseSkill[ XGAME::MAX_SKILL_LEVEL ] = {0, 0.1f, 0.12f, 0.14f, 0.17f, 0.19f, 0.21f, 0.23f, 0.26f, 0.28f, 0.30f };
// 특성개수별 표준 증가량
//const float x_baseAbilPoint[ 25 + 1 ] = {0, 0.012f, 0.025f, 0.038f, 0.050f, 0.063f, 0.075f, 0.088f, 0.1f, 0.113f, 0.125f, 0.138f, 0.150f, 0.163f, 0.175f, 0.188f, 0.2f, 0.213f, 0.225f, 0.238f, 0.250f, 0.263f, 0.275f, 0.288f, 0.3f };
const float x_baseAbil = 0.012f;

int XLegion::sGetMilitaryPower( XHero *pHero )
{
	auto grade = pHero->GetGrade();	
	if( XBREAK(XGAME::IsInvalidGrade( grade ) ) )
		return 0;
	// 영웅등급/레벨별 배수
	float mulByHero = x_baseStatHero[ grade ] 
		+ ( pHero->GetLevel() * x_baseAddRateHero[ grade ] );
	// 장착아이템 등급별 배수
	float mulByItem = 0.f;
	for( int i = 1; i < XGAME::xPARTS_MAX; ++i ) {
		auto parts = (XGAME::xtParts) i;
		auto pItem = pHero->GetEquipItem( parts );
		if( pItem ) {
			switch( parts )
			{
			case XGAME::xPARTS_HEAD:
			case XGAME::xPARTS_CHEST:
			case XGAME::xPARTS_FOOT:
				mulByItem += x_baseArmor[ pItem->GetpProp()->grade ];
				break;
			case XGAME::xPARTS_HAND:
				mulByItem += x_baseWeapon[ pItem->GetpProp()->grade ];
				break;
			case XGAME::xPARTS_ACCESSORY:
				mulByItem += x_baseAcce[ pItem->GetpProp()->grade ];
				break;
			default:
				XBREAK(1);
				break;
			}
		}
	}
	// 영웅의 등급/레벨별 증폭치에 아이템으로 증폭한다음 그것을 유닛상수와 다시 곱한다.
	float score = x_scoreUnit * ( 1.f + ((mulByHero - 1.f) * (1.f + mulByItem)));
	// 스킬레벨에 따른 증폭치
	float mulSkillActive = x_baseSkill[ pHero->GetlvActive() ];
	float mulSkillPassive = x_baseSkill[ pHero->GetlvPassive() ];
	// 찍은특성포인트 개수에 따른 증폭치
	int numAbilPoint = 0;
//   if( pAcc ) {
		// 해당 영웅이 현재 선택중인 유닛의 특성포인트만 적용한다.
//    numAbilPoint = pAcc->GetNumSetAbilPoint( pHero->GetUnit() );
//		numAbilPoint = pHero->GetNumSetAbilPoint( pHero->GetUnit() );
	// pHero가 가진 특성포인트 전체 합으로 계산.
		numAbilPoint = pHero->GetNumSetAbilPoint();
		const int maxPoint = XGAME::MAX_ABIL_UNIT * 5 * 3;	// 5(abil maxpoint) 3(atkType)
		if( numAbilPoint > maxPoint ) {
			CONSOLE("경고:영웅의 특성수가 %d개 이상임.", maxPoint );
		}
//     if( numAbilPoint > 25 )
//       numAbilPoint = 25;
//   }
	float mulAbil = numAbilPoint * x_baseAbil;  // 특성포인트1개당 표준 증가치
	// 
	score *= 1.f + (mulSkillActive + mulSkillPassive + mulAbil);
	const int baseNumUnits = 5;   // 기본 병사수
	score *= ( baseNumUnits + (pHero->GetlevelSquad() - 1) );
	return (int)score;
}

/**
 @brief lvAcc레벨의 최대 전투력
*/
int XLegion::sGetMilitaryPowerMax( int lvAcc )
{
//  float score = 0;
	// lvAcc레벨에서 가능한 최고 부대수
	// 모든 부대의 영웅이 최고레벨, 최고등급
	// 모든 영웅의 부대레벨이 최고레벨
	// 모든 영웅의 스킬레벨이 최고레벨
	// 영웅이 모두 4성템을 갖고 있음.

	int lvHeroMax = lvAcc;    // 최대가능한 영웅레벨
	XGAME::xtGrade gradeHero = XGAME::xGD_EPIC;   // 최대 가능한 영웅 등급
	XGAME::xtGrade gradeItem = XGAME::xGD_EPIC;   // 최대 가능한 아이템 등급
	int maxLevelSquad = PROP_SQUAD->GetLevelSquadMaxByHero( lvHeroMax );
	int maxLevelActive = XPropUpgrade::sGet()->GetLevelSkillMaxByHero( lvHeroMax );
	int maxLevelPassive = maxLevelActive;
	// 영웅등급/레벨별 배수
	float mulByHero = x_baseStatHero[ gradeHero ] + ( lvHeroMax * x_baseAddRateHero[ gradeHero ] );
	// 장착아이템 등급별 배수
	float mulByItem = 0.f;
	for( int i = 1; i < XGAME::xPARTS_MAX; ++i ) {
		auto parts = ( XGAME::xtParts ) i;
		switch( parts ) {
		case XGAME::xPARTS_HEAD:
		case XGAME::xPARTS_CHEST:
		case XGAME::xPARTS_FOOT:
			mulByItem += x_baseArmor[ gradeItem ];
			break;
		case XGAME::xPARTS_HAND:
			mulByItem += x_baseWeapon[ gradeItem ];
			break;
		case XGAME::xPARTS_ACCESSORY:
			mulByItem += x_baseAcce[ gradeItem ];
			break;
		default:
			XBREAK( 1 );
			break;
		}
	}
	// 영웅의 등급/레벨별 증폭치에 아이템으로 증폭한다음 그것을 유닛상수와 다시 곱한다.
	float scoreHero = x_scoreUnit * ( 1.f + ( ( mulByHero - 1.f ) * ( 1.f + mulByItem ) ) );
	// 스킬레벨에 따른 증폭치
	float mulSkillActive = x_baseSkill[ maxLevelActive ];
	float mulSkillPassive = x_baseSkill[ maxLevelPassive ];
	// 찍은특성포인트 개수에 따른 증폭치
	int maxAbilPoint = XAccount::sGetMaxTechPoint( lvAcc );
	float mulAbil = maxAbilPoint * x_baseAbil;  // 특성포인트1개당 표준 증가치
	// 
	scoreHero *= 1.f + ( mulSkillActive + mulSkillPassive + mulAbil );
	const int baseNumUnits = 5;   // 기본 병사수
	scoreHero *= ( baseNumUnits + ( maxLevelSquad - 1 ) );
	int numSquad = XAccount::sGetMaxSquadByLevelForPlayer( lvAcc );
	return (int)(scoreHero * numSquad);
}

/**
 @brief 클라이언트로부터 온 길드레이드용 군단정보를 풀어서 그에 맞게 
 군단내용을 조절한다.
*/
int XLegion::DeserializeForGuildRaid( XArchive& ar )
{
	int size;
	ar >> size;
	for( int i = 0; i < size; ++i ) {
		ID snHero;
		int live;
		ar >> snHero;
		ar >> live;
		if( live ) {
			XBREAK( live != 11 );
		}
		auto pSquad = GetSquadronByHeroSN( snHero );
		if( XASSERT( pSquad ) ) {
			pSquad->GetpHero()->SetbLive( live != 0 );	// 죽은영웅으로 표시
		}
	}
	return size;
}
BOOL XLegion::SetSquadron( int idx, XSquadron *pSq, BOOL bCreateHero ) 
{
	if( idx < 0 || idx >= m_arySquadrons.GetMax() )
		return FALSE;
//	XBREAK( m_arySquadrons[ idx ] != nullptr );
	if( m_arySquadrons[ idx ] ) {
		SAFE_DELETE( m_arySquadrons[ idx ] );
	}
	m_arySquadrons[ idx ] = pSq;
	m_arySquadrons[ idx ]->SetbCreateHero( bCreateHero );
	return TRUE;
}
void XLegion::ClearResourceSquad()
{
	m_aryResourceHero.clear();
// 	XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
// 		if( pSquad ) {
// 			pSquad->SetbResourceSquad( false );	// 일단 초기화
// 		}
// 	} END_LOOP;
}
/**
 @brief 랜덤으로 numSquad수만큼의 부대를 자원부대로 설정한다.
*/
void XLegion::SetResourceSquad( int numSquad )
{

	int num = numSquad;
	m_aryResourceHero.clear();
	XList4<XSquadron*> listSquads;
	// 부대를 임시 리스트에 담는다.
	XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
		if( pSquad ) {
//			pSquad->SetbResourceSquad( false );	// 일단 초기화
			listSquads.Add( pSquad );
		}
	} END_LOOP;
	// 뽑아야할 수만큼 루프를 돈다.
	while( num-- ) {
		// 랜덤으로 한부대를 뽑는다.
		auto itor = listSquads.GetItorFromRandom();
		if( itor != listSquads.end() ) {
			// 그 부대를 자원부대로 설정한다.
			XSquadron *pSuqad = (*itor);
			if( XASSERT(pSuqad) ) {
				//pSuqad->SetbResourceSquad( true );
				m_aryResourceHero.push_back( pSuqad->GetpHero()->GetsnHero() );
			}
			// 그 부대를 임시리스트에서 삭제한다.
			listSquads.erase( itor );
		}
		// 반복
	}
	
}

/**
 @brief 자원부대중에 전멸한 부대의 수를 얻는다.
*/
int XLegion::GetNumDeadSquadByResourceSquad()
{
	int num = 0;
	XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
		if( pSquad 
			&& IsResourceSquad( pSquad->GetidPropHero() )
			&& !pSquad->GetpHero()->GetbLive() )
			++num;
	} END_LOOP;
	return num;
}
/**
 @brief 자원부대가 이미 세팅되어있는지
*/
bool XLegion::IsSettedResourceSquad()
{
	return m_aryResourceHero.size() > 0;
}

bool XLegion::IsResourceSquad( ID snHeroSrc )
{
	for( auto snHero : m_aryResourceHero )
		if( snHero == snHeroSrc )
			return true;
	return false;
}
/**
 @brief 현재부대에게 안개를 씌운다.
 랜덤으로 3부대만 남기고 안개를 씌운다.
 3부대이하는 안개가 없다. 4부대부터 안개가 1개씩 생긴다.
 @param numVisible 보여야하는(안개덮히지않은)부대의 수
*/
void XLegion::MakeFogs( int numVisible )
{
	int numFog = GetMaxFogs( numVisible );
	m_listFogs.clear();
	XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
		if( pSquad )
			m_listFogs.Add( pSquad );
	} END_LOOP;
	// 안개개수외의 부대는 안개리스트에서 제외한다.
	m_listFogs.DelByRandom( GetNumSquadrons() -  numFog );
}
/**
 @brief 최대 안개부대 수를 정한다.
*/
int XLegion::GetMaxFogs( int numVisible )
{
	int num = GetNumSquadrons() - numVisible;
	if( num < 0 )
		num = 0;
	return num;
}
/**
 @brief snHero부대가 현재 안개로 덮혀있는가.
*/
bool XLegion::IsFog( ID snHero )
{
	for( auto pSquad : m_listFogs ) {
		if( XASSERT(pSquad) )
			if( XASSERT(pSquad->GetpHero()) )
				if( pSquad->GetpHero()->GetsnHero() == snHero )
					return true;
	}
	return false;
}

int XLegion::GetNumFogs()
{
	return m_listFogs.size();
}

bool XLegion::DelFogSquad( ID snHero )
{
	m_listFogs.DelIf( [snHero]( XSquadron *pSquad )->bool {
		return pSquad->GetpHero()->GetsnHero() == snHero;
	});
	return true;
}
/**
 @brief 정찰된(unfog)부대의 영웅 sn번호를 벡터에 담아준다.
*/
int XLegion::GetUnFogList( std::vector<ID> *pOutAry)
{
	for( auto pSquad : m_listFogs ) {
		if( pSquad ) {
			XBREAK( pSquad->GetpHero() == nullptr );
			pOutAry->push_back( pSquad->GetpHero()->GetsnHero() );
		}
	}
	return pOutAry->size();
}
/**
 @brief 외부어레이로 정찰된 부대정보를 갱신한다.
*/
void XLegion::SetUnFogList( const std::vector<ID>& aryUnFogHeroSN )
{
	m_listFogs.clear();
	for( auto snHero : aryUnFogHeroSN ) {
		auto pSquad = GetSquadronByHeroSN( snHero );
		if( pSquad )
			m_listFogs.Add( pSquad );
	}
}
/**
 @brief 군단내에서 pHeroFrom영웅을 pHeroTo로 바꾼다.
*/
bool XLegion::ChangeHeroInSquad( XHero *pHeroFrom, XHero *pHeroTo )
{
	if( XBREAK( pHeroFrom == nullptr ) )
		return false;
	if( XBREAK( pHeroTo == nullptr ) )
		return false;
	auto pSquad = GetSquadronByHeroSN( pHeroFrom->GetsnHero() );
	if( pSquad ) {
		// 기존영웅이 리더였으면 리더 바꿔줌.
		if( m_pLeader && m_pLeader->GetsnHero() == pHeroFrom->GetsnHero() )
			m_pLeader = pHeroTo;
		// 자원부대에 기존 영웅이 있었으면 교체해줌.
		for( ID& snHero : m_aryResourceHero ) {
			if( snHero == pHeroTo->GetsnHero() )
				snHero = pHeroTo->GetsnHero();
		}
		pSquad->ChangeHero( pHeroTo );
		return true;
	}
	return false;
}

/**
 @brief snSquadHero가 영웅으로 있는 부대의 유닛 개별 hp를 얻는다. 
 @param bHero 그 부대의 영웅유닛의 hp를 얻는다.
*/
float XLegion::GethpMaxEach( ID snSquadHero, bool bHero )
{
	auto pHero = GetpHeroBySN( snSquadHero );
	if( XBREAK( pHero == nullptr) )
		return 0;
	auto pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
	if( XBREAK(pPropUnit == nullptr) )
		return 0;
	const float mulByLvSquad = PROP_SQUAD->GetAvgMulByLvSquad( pPropUnit->size, pHero->GetlevelSquad() );
	float hpBase = pPropUnit->hpMax * mulByLvSquad;
	const auto gradeLegion = GetgradeLegion();
	// 기본hp에 추가값이 있으면 곱한다. 엘리트나 레이드군단이라던가 보석광산으로 인해 곱해지는값이 변할수 있다.
	float rateHp = GetRateHp();
	if( gradeLegion != XGAME::xGL_NORMAL && rateHp == 1.f ) {
		// 정예나 레이드인데 배수가 지정되지 않으면 디폴트 배수를 사용함.
		if( gradeLegion == XGAME::xGL_ELITE )
			rateHp = RATE_HP_DEFAULT_ELITE;
		else if( gradeLegion == XGAME::xGL_RAID )
			rateHp = RATE_HP_DEFAULT_RAID;
	}
	if( XBREAK( rateHp < 1.f ) )
		rateHp = 1.f;
	auto pSq = GetSquadronByHeroSN( pHero->GetsnHero() );
	const float rateHpSq = pSq->GetmulHp();
	XBREAK( rateHpSq == 0 );
	hpBase *= rateHp * rateHpSq;
	if( bHero ) {
		switch( pPropUnit->size ) {
		case XGAME::xSIZE_SMALL:	hpBase *= 5.f; break;
		case XGAME::xSIZE_MIDDLE:	hpBase *= 1.f;	break;
		case XGAME::xSIZE_BIG: 
			hpBase = hpBase / mulByLvSquad;	
			hpBase *= 0.25f;
			break; // 대형은 1이하값으로 보정되므로 역수를 나눠서 보정되기전 오리지날값(부대16렙값)으로 사용한다.
		default:
			XBREAK(1);
			break;
		}
	}
	const float hpMax = hpBase * pHero->GetHpMaxRatio();
	return hpMax;
}

// bool XLegion::IsValid() const
// {
// 	for( int i = 0; i < m_arySquadrons.GetMax(); ++i ) {
// 		const auto pSquad = m_arySquadrons[ i ];
// 		if( pSquad ) {
// 			if( pSquad->IsNpc() )
// 				return false;
// 		}
// 	}
// 	return true;
// }

bool XLegion::IsNpc() const
{
	for( int i = 0; i < m_arySquadrons.GetMax(); ++i ) {
		const auto pSquad = m_arySquadrons.At(i);
		if( pSquad ) {
			if( !pSquad->IsNpc() )
				return false;
		}
	}
	return true;
}
