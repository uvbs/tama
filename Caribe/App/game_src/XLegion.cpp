#include "stdafx.h"
#include "XLegionH.h"
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

//////////////////////////////////////////////////////////////////////////
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 
*/
void xnLegion::xSquadDat::Serialize( XArchive& ar ) const {
	ar << (char)m_idxPos;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	ar << m_snHero;
}
void xnLegion::xSquadDat::DeSerialize( XArchive& ar, DWORD ver ) {
	char c0;
	ar >> c0;		m_idxPos = c0;
	ar >> c0 >> c0 >> c0;
	ar >> m_snHero;
}
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 
*/
void xnLegion::xLegionDat::Serialize( XArchive& ar ) const {
	ar << m_listSquad;
	ar << m_snLeader;
}
void xnLegion::xLegionDat::DeSerialize( XArchive& ar, DWORD ver ) {
	ar >> m_listSquad;
	ar >> m_snLeader;
}


/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 군단데이터로 군단객체를 생성한다.
*/
// XSPLegion XLegion::sCreateLegionWithDat( const xnLegion::xLegionDat& dat, 
// 																				 XSPAccConst spAcc ) 
// {
// 	auto spLegion = std::make_shared<XLegion>();
// 	// 부대들의 정보
// 	for( const auto& datSq : dat.m_listSquad ) {
// 		auto pHero = spAcc->GetpcHeroBySN( datSq.m_snHero );
// 		if( pHero ) {		///< 영웅을 삭제하거나 했으면 없을수도 있다.
// 			auto pSq = spLegion->CreateAddSquadron( datSq.m_idxPos, pHero, false );
// 		}
// 	}
// 	auto pLeader = spAcc->GetpcHeroBySN( dat.m_snLeader );
// 	spLegion->SetpLeader( std::const_pointer_cast<XHero>( pLeader ) );
// 	return spLegion;
// }

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
				pLegion->AddSquadron( sqParam.idxPos, pSquad, true );
//				pLegion->AddSquadron( sqParam.idxPos, pSquad, true );
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
					if( pLegion->GetpSquadronByidxPos( i ) == nullptr )
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
					pLegion->AddSquadron( idxSlot, pSquad, true );
//					pLegion->AddSquadron( idxSlot, pSquad, true );
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



/**
 @brief 분대 생성 모듈.
 @param idHero 0이 아니라면 idHero를 부대장으로 한다.
 @param levelHero 0이 아니라면 levelHero를 부대장 레벨로 한다.
 @param levelSkill 0이 아니라면 부대장의 스킬레벨로 한다.
 @param levelSuqad 0이 아니라면 부대레벨로 한다. 
 @note 레벨류는 랜덤요소를 넣지 말것. adjDiff로 조절함.
*/
#if defined(_XSINGLE) || !defined(_CLIENT)
XSPSquadron XLegion::sCreateSquadronForNPC( int levelLegion,
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
	auto pSquad = std::make_shared<XSquadron>( pPropHero, levelHero, unit, levelSquad );
	pSquad->GetpHero()->SetGrade( grade );
	return pSquad;
}
/**
 @brief 신버전 부대생성기.
 파라메터는 반드시 유효한 값으로 들어와야한다.
*/
XSPSquadron XLegion::sCreateSquadronForNPC2( const int lvLegion
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
	auto pSquad = std::make_shared<XSquadron>( pPropHero, sqParam.lvHero, sqParam.unit, sqParam.lvSquad );
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
XSPSquadron XLegion::sCreateSquadronForNPC2( const int lvLegion
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
	XSPHero pFirst = nullptr;
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
		auto pSquad = std::make_shared<XSquadron>( pPropHero, lvHero, selectedUnit, lvSquadLevel );
		pLegion->AddSquadron( idxSquad, pSquad, true );
//		pLegion->AddSquadron( idxSquad, pSquad, TRUE );
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
			pLegion->AddSquadron( idx[i], pSquad, TRUE );
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

void XLegion::sSetLeaderByInfo( const XGAME::xLegion* pxLegion, XSPLegion spLegion, XSPSquadron pSquad )
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
	if( !m_listSquadrons.empty() ) {
		auto pSquad = m_listSquadrons.GetFromRandom();
		XBREAK( pSquad->GetpHero() == nullptr );
		m_pLeader = pSquad->GetpHero();
	}
}
/** //////////////////////////////////////////////////////////////////
 @brief 전략적으로 부대내 위치를 바꿔줌.
*/
void XLegion::AdjustLegion()
{
	int idx = 0;
	for( auto pSq : m_listSquadrons ) {
		// 원거리 유닛이 앞줄에 있으면.
		if( pSq->GetAtkType() == XGAME::xAT_RANGE && idx < 5 ) {
			// 자리를 서로 바꿔준다.
			SwapSlotSquad( idx+5, idx );
//			std::swap( m_arySquadrons[idx + 5], m_arySquadrons[idx] );
		} else
		// 스피드유닛이 1,2째줄에 있으면 3번째 줄로 빼준다.
		if( pSq->GetAtkType() == XGAME::xAT_SPEED && idx < 10 ) {
			if( idx < 5 )
				SwapSlotSquad( idx + 10, idx );
			//std::swap( m_arySquadrons[idx + 10], m_arySquadrons[idx] );
			else if( idx < 10 )
				SwapSlotSquad( idx + 5, idx );
//				std::swap( m_arySquadrons[ idx + 5 ], m_arySquadrons[ idx ] );
		} else
		// 근접유닛이 뒷열에 있으면 앞열로 옮긴다.
		if( pSq->GetAtkType() == XGAME::xAT_TANKER && idx >= 5 ) {
			SwapSlotSquad( idx % 5, idx );
//			std::swap( m_arySquadrons[ idx % 5 ], m_arySquadrons[ idx ] );
		}
		++idx;
	}
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
		grade = XGAME::xGD_VETERAN;
	} else if( levelUser <= 20 )	{
		// 1,2,3등급
		grade = XGAME::xGD_RARE;
	} else
		grade = XGAME::xGD_RARE;
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
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 
*/
void XLegion::sSerialize( XSPLegionConst spcLegion, XArchive* pOut ) {
	XArchive& ar = *pOut;
	if( spcLegion )
		ar << VER_LEGION_SERIALIZE;
	else
		ar << 0;
	if( spcLegion )
		spcLegion->Serialize( ar );
}
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 
*/
void XLegion::sDeserialize( XSPLegion spLegionTarget, XSPAccConst spAcc, XArchive& ar ) {
	DWORD ver;
	ar >> ver;
	XBREAK( ver < 0 );
	if( ver == 0 )
		return;
	spLegionTarget->DeSerialize( ar, spAcc, ver );
}

/** //////////////////////////////////////////////////////////////////
 @brief 아카이브에 있는 군단정보를 pOut에 갱신한다.
 군단객체를 통째로 삭제하고 생성해서 하지 않기 위해 만듬.
*/
void XLegion::sDeSerializeUpdate( XSPLegion spOut, XSPAcc spAcc, XArchive& ar ) {
	int ver;
	ar >> ver;
	XBREAK( ver < 0 );
	if( ver == 0 )
		return;
	spOut->Destroy();
	spOut->DeSerialize( ar, spAcc, ver );
}

//////////////////////////////////////////////////////////////////////////
XLegion::XLegion() 
{ 
	Init(); 
}

void XLegion::Destroy()
{
	DestroySquadronAll();
}
// 객체 파괴될때 list내용을 파괴시키지 않기 위한 궁여지책. squadron을 shared_ptr로 바꿔야 한다.
void XLegion::Clear()
{
	m_listSquadrons.clear();
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 부대객체를 모두 날림
*/
void XLegion::DestroySquadronAll()
{
	m_listFogs.clear();
	m_aryResourceHero.clear();
//	XLIST4_DESTROY( m_listSquadrons );
	m_listSquadrons.clear();
	m_pLeader = nullptr;
}

void XLegion::Serialize( XArchive& ar ) const
{
	const int size = m_listSquadrons.size();
	ar << (BYTE)size;
	XBREAK( m_gradeLegion > 0xff );
	ar << (BYTE)m_gradeLegion;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << m_snLegion;
	ar << m_RateAtk;
	ar << m_RateHp;
	ar << m_aryResourceHero;
//	ar << m_listSquadrons;
	for( auto pSquad : m_listSquadrons ) {
		pSquad->Serialize( ar );
	}
	if( m_pLeader )
		ar << m_pLeader->GetsnHero();
	else
		ar << 0;		// 부대가 없다면 영웅이 없을수도 있음.
	SerializeFogs( ar );
	MAKE_CHECKSUM( ar );
}

BOOL XLegion::DeSerialize( XArchive& ar, XSPAccConst spAcc, int verLegion ) 
{
	BYTE b0;
	ar >> b0;	const int size = b0;
	ar >> b0;	m_gradeLegion = (XGAME::xtGradeLegion)b0;
	ar >> b0 >> b0;
	ar >> m_snLegion;
	ar >> m_RateAtk;
	ar >> m_RateHp;
	ar >> m_aryResourceHero;
	//XLIST4_DESTROY( m_listSquadrons );
	m_listSquadrons.clear();
	for( int i = 0; i < size; ++i ) {
		auto pSquad = std::make_shared<XSquadron>();
		if( !pSquad->DeSerialize( ar, spAcc, verLegion ) )
			return FALSE;
		m_listSquadrons.push_back( pSquad );
	}
	{
		ID snLeader;
		ar >> snLeader;
		if( snLeader ) {
			auto pHero = spAcc->GetpcHeroBySN( snLeader );
			if( XBREAK( pHero == nullptr ) )
				return FALSE;
			SetpLeader( std::const_pointer_cast<XHero>( pHero ) );
		} else
			SetpLeader( nullptr );
	}
	DeSerializeFogs( ar, verLegion );
	RESTORE_VERIFY_CHECKSUM( ar );
	return TRUE;
}

int XLegion::SerializeFogs( XArchive& ar ) const
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
	const int size = m_listSquadrons.size(); 
	ar << (BYTE)size;
	ar << (BYTE)66;	// full serialize 표시
	XBREAK( m_gradeLegion > 0xff );
	ar << (BYTE)m_gradeLegion;
	ar << m_snLegion;
	ar << m_RateAtk;
	ar << m_RateHp;
	ar << m_aryResourceHero;
	for( auto pSq : m_listSquadrons ) {
		pSq->SerializeFull( ar );
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
	ar >> b0;	const int size = b0;
	ar >> b0;
	XBREAK( b0 != 66 );
	ar >> b0;	m_gradeLegion = (XGAME::xtGradeLegion)b0;
	ar >> m_snLegion;
	ar >> m_RateAtk;
	ar >> m_RateHp;
	ar >> m_aryResourceHero;
	m_listSquadrons.clear();
	for( int i = 0; i < size; ++i ) {
		auto pSq = std::make_shared<XSquadron>();
		if( !pSq->DeSerializeFull( ar, verLegion ) )
			return FALSE;
		m_listSquadrons.push_back( pSq );
	}
	do {
		ID snLeader;
		ar >> snLeader;
  	if( snLeader ) {
			auto pSquad = GetSquadronByHeroSN( snLeader );
			if( XBREAK( pSquad == nullptr ) ) {
				SetpLeader( nullptr );
				break;
			}
			XBREAK( pSquad->GetpHero() == nullptr );
			SetpLeader( pSquad->GetpHero() );
		} else {
			SetpLeader( nullptr );
		}
	} while(0);
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
	for( auto pSqSrc : m_listSquadrons ) {
		const ID snHero = pSqSrc->GetpHero()->GetsnHero();
		auto pHero = spAcc->GetHero( snHero );
		if( XASSERT( pHero ) ) {
			auto pSqNew = std::make_shared<XSquadron>( pHero );
			pNewLegion->AddSquadron( pSqSrc->GetidxPos(), pSqNew, false );
			if( m_pLeader && pHero->GetsnHero() == m_pLeader->GetsnHero() )
				pNewLegion->SetpLeader( pHero );
		}
	}
	return pNewLegion;
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
XSPSquadron XLegion::GetSquadronByHeroSN( ID snHero ) const
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetsnHero() == snHero )
			return pSq;
	}
	return nullptr;
}

/**
 @brief 영웅sn번호로 슬롯 인덱스를 얻는다.
 @param 검색에 성공하면 -1 ~를 리턴한다. 실패하면 -99를 리턴한다.
*/
int XLegion::_GetIdxSquadByHeroSN( ID snHero ) const
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetsnHero() == snHero )
			return pSq->GetidxPos();
	}
	return -99;
}

XSPHero XLegion::GetpHeroBySN( ID snHero ) const
{
	auto pSquad = GetSquadronByHeroSN( snHero );
	if( pSquad )
		return pSquad->GetpHero();
	return nullptr;
}

XSPHero XLegion::GetpHeroByIdxPos( int idxPos )
{
	XBREAK( idxPos < 0 );		// 이런경우가 있다면 가급적 이 함수를 사용하지 않는쪽으로 수정할것.
	if( idxPos < 0 )
		return nullptr;
	auto pSquad = GetpmSquadronByidxPos( idxPos );
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
		return _GetIdxSquadByHeroSN(m_pLeader->GetsnHero() );
	return -1;
}
/**
 @brief pHeroSrc를 가진 부대와 pHeroDst를 가진 부대의 슬롯을 서로 바꾼다
*/
void XLegion::SwapSlotSquad( XSPHero pHeroSrc, XSPHero pHeroDst )
{
	XBREAK( pHeroSrc == nullptr );
	XBREAK( pHeroDst == nullptr );
	auto pSq1 = GetSquadronByHeroSN( pHeroSrc->GetsnHero() );
	auto pSq2 = GetSquadronByHeroSN( pHeroDst->GetsnHero() );
	auto idxPos = pSq1->GetidxPos();
	pSq1->SetidxPos( pSq2->GetidxPos() );
	pSq2->SetidxPos( idxPos );
}

void XLegion::SwapSlotSquad( int idxSrc, int idxDst )
{
	auto pSq1 = GetpmSquadronByidxPos( idxSrc );
	auto pSq2 = GetpmSquadronByidxPos( idxDst );
	if ( pSq1 )
		pSq1->SetidxPos( idxDst );
	if( pSq2 )
		pSq2->SetidxPos( idxSrc );
// 	if( idxSrc < 0 || idxSrc >= m_arySquadrons.GetMax() )
// 		return FALSE;
// 	if( idxDst < 0 || idxDst >= m_arySquadrons.GetMax() )
// 		return FALSE;
// 	std::swap( m_arySquadrons[ idxSrc ], m_arySquadrons[ idxDst ] );
}

void XLegion::DestroySquadBysnHero( ID snHero )
{
	// snHero로 부대를 꺼낸다.
	auto pSq = GetSquadronByHeroSN( snHero );
	// 그 부대 sn으로 파괴한다.
	DestroySquadron( pSq->GetsnSquad() );
// 	int idx = _GetIdxSquadByHeroSN( snHero );
// 	if( idx < 0 )
// 		return;
// 	for( auto itor = m_listSquadrons.begin(); itor != m_listSquadrons.end(); ) {
// 		auto pSq = (*itor);
// 		const ID idLeader = (m_pLeader)? m_pLeader->GetsnHero() : 0;
// 		if( pSq->GetpHero()->GetsnHero() == idLeader ) {
// 			m_pLeader = 0;
// 			SAFE_DELETE( pSq );
// 			m_listSquadrons.erase( itor++ );
// 			return TRUE;
// 		} else {
// 			++itor;
// 		}
// 	}
// 	return FALSE;
}
// BOOL XLegion::DestroySquad( XSPHero pHero )
// {
// 	return DestroySquadBysnHero( pHero->GetsnHero() );
// }

void XLegion::DestroySquadByIdxPos( int idx )
{
	auto pSq = GetpSquadronByidxPos( idx );
	if( pSq ) {
		DestroySquadron( pSq->GetsnSquad() );
	}
// 	if (m_arySquadrons[idx] != nullptr && m_arySquadrons[idx]->GetpHero() == m_pLeader)
// 		m_pLeader = nullptr;
// 	SAFE_DELETE( m_arySquadrons[ idx ] );
// 	return TRUE;
}

/** //////////////////////////////////////////////////////////////////
@brief idSquad아이디를 가진 부대를 삭제한다.
*/
void XLegion::DestroySquadron( ID snSquad )
{
	for( auto itor = m_listSquadrons.begin(); itor != m_listSquadrons.end(); ) {
		auto pSquad = (*itor);
		if( pSquad->GetsnSquad() == snSquad ) {
			if( m_pLeader == pSquad->GetpHero() )
				m_pLeader = nullptr;
//			SAFE_DELETE( pSquad );
			m_listSquadrons.erase( itor++ );
		} else {
			++itor;
		}
	}
}

int XLegion::GetHerosToAry( XVector<XSPHero> *pOutAry )
{
	for( auto pSq : m_listSquadrons ) {
		pOutAry->push_back( pSq->GetpHero() );
	}
	return pOutAry->size();
}

int XLegion::GetHeroesToList( XList4<XSPHero>* pOutList ) 
{
	pOutList->clear();
	for( auto pSq : m_listSquadrons ) {
		pOutList->push_back( pSq->GetpHero() );
	}
	return pOutList->size();
}
/**
 @brief spLegion의 군사력을 얻는다.
*/
int XLegion::sGetMilitaryPower( XSPLegion spLegion )
{
	XBREAK( spLegion == nullptr );
	float score = 0;
	XVector<XSPHero> aryHeroes;
	spLegion->GetHerosToAry( &aryHeroes );
	int scoreMax = 0;
	// 메인 군단에 속한 영웅들의 전투력 합산
	//XARRAYLINEARN_LOOP_AUTO( aryHeroes, pHero ) {
	for( auto pHero : aryHeroes ) {
		// 해당 영웅이 현재 선택중인 유닛의 특성포인트만 적용한다.
		auto scoreHero = sGetMilitaryPower( pHero );
		if( scoreHero > scoreMax )
			scoreMax = scoreHero;     // 가장 전투력이 쎈 영웅의 값을 받아둔다.
		score += scoreHero;
	}
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

int XLegion::sGetMilitaryPower( XSPHero pHero )
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
//		auto pItem = pHero->GetsnEquipItem( parts );
		auto pProp = pHero->GetpPropEquipItem( parts );
		if( pProp ) {
			switch( parts )
			{
			case XGAME::xPARTS_HEAD:
			case XGAME::xPARTS_CHEST:
			case XGAME::xPARTS_FOOT:
				mulByItem += x_baseArmor[ pProp->grade ];
				break;
			case XGAME::xPARTS_HAND:
				mulByItem += x_baseWeapon[ pProp->grade ];
				break;
			case XGAME::xPARTS_ACCESSORY:
				mulByItem += x_baseAcce[ pProp->grade ];
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
/** //////////////////////////////////////////////////////////////////
 @brief 부대를 추가한다
 @param pSq 부대의 포인터. idxPos가 반드시 지정되어야 한다.
*/
void XLegion::AddSquadron( int idxPos, XSPSquadron pSq, bool bCreateHero ) 
{
	// -1이라도 명시적으로 지정되어야 한다.
	XBREAK( idxPos == -99 );
	// 기존것을 지운다.
	DestroySquadron( pSq->GetsnSquad() );
	//
	pSq->SetbCreateHero( bCreateHero );
	pSq->SetidxPos( idxPos );
	m_listSquadrons.push_back( pSq );

}

void XLegion::ClearResourceSquad()
{
	m_aryResourceHero.clear();
}
/**
 @brief 랜덤으로 numSquad수만큼의 부대를 자원부대로 설정한다.
*/
void XLegion::SetResourceSquad( int numSquad )
{

	int num = numSquad;
	m_aryResourceHero.clear();
	// 부대를 임시 리스트에 담는다.
	XList4<XSPSquadron> listSquads = m_listSquadrons;
	// 뽑아야할 수만큼 루프를 돈다.
	while( num-- ) {
		// 랜덤으로 한부대를 뽑는다.
		auto itor = listSquads.GetItorFromRandom();
		if( itor != listSquads.end() ) {
			// 그 부대를 자원부대로 설정한다.
			XSPSquadron pSuqad = (*itor);
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
	//XARRAYN_LOOP_AUTO( m_arySquadrons, pSquad ) {
	for( auto pSquad : m_listSquadrons ) {
		if( pSquad 
			&& IsResourceSquad( pSquad->GetidPropHero() )
			&& !pSquad->GetpHero()->GetbLive() )
			++num;
	}
	return num;
}
/**
 @brief 자원부대가 이미 세팅되어있는지
*/
bool XLegion::IsSettedResourceSquad()
{
	return m_aryResourceHero.size() > 0;
}

bool XLegion::IsResourceSquad( ID snHeroSrc ) const
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
	for( auto pSquad : m_listSquadrons ) {
		if( pSquad )
			m_listFogs.Add( pSquad );
	}
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
bool XLegion::IsFog( ID snHero ) const
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
	m_listFogs.DelIf( [snHero]( XSPSquadron pSquad )->bool {
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
bool XLegion::ChangeHeroInSquad( XSPHero pHeroFrom, XSPHero pHeroTo )
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
float XLegion::GethpMaxEach( ID snSquadHero, bool bHero ) const
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

bool XLegion::IsNpc() const
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->IsPc() )
			return false;
	}
// 	for( int i = 0; i < m_arySquadrons.GetMax(); ++i ) {
// 		const auto pSquad = m_arySquadrons.at(i);
// 		if( pSquad ) {
// 			if( !pSquad->IsNpc() )
// 				return false;
// 		}
// 	}
	return true;
}

XSPSquadron XLegion::GetpmSquadronByidxPos( int idxPos )
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetidxPos() == idxPos )
			return pSq;
	}
	return nullptr;
}
const XSPSquadron XLegion::GetpSquadronByidxPos( int idxPos ) const
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetidxPos() == idxPos )
			return pSq;
	}
	return nullptr;
}

const XSPSquadron XLegion::GetpSquadronBySN( ID snSquad ) const
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetsnSquad() == snSquad )
			return pSq;
	}
	return nullptr;
}

XSPSquadron XLegion::GetpmSquadronBySN( ID snSquad )
{
	for( auto pSq : m_listSquadrons ) {
		if( pSq->GetsnSquad() == snSquad )
			return pSq;
	}
	return nullptr;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 주어진 영웅으로 부대객체를 만들어 군단에 포함시킨다.
*/
XSPSquadron XLegion::CreateAddSquadron( int idxSquad, XSPHeroConst pHero, bool bCreateHero )
{
	auto pSq = std::make_shared<XSquadron>( std::const_pointer_cast<XHero>( pHero ) );
	AddSquadron( idxSquad, pSq, bCreateHero );
	return pSq;

}

