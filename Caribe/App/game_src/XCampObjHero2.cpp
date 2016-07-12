#include "stdafx.h"
#ifdef _XCAMP_HERO2
#include "XCampObjHero2.h"
#include "XStageObjHero2.h"
#include "XPropHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
XE_NAMESPACE_START( xCampaign )

/**
 @brief idHero를 바탕으로 고유스테이지 아이디를 만든다.
*/
// ID XCampObjHero2::sGetidStageByidHero( ID idHero, int idxStage, int cycle )
// {
// 	XASSERT( cycle > 0 );
// 	return (10000 * cycle) + idHero;
// }
// int XCampObjHero2::sGetCycleByidStage( ID idStage, ID idHero )
// {
// 	XASSERT( idStage >= 10000 );
// 	return ((idStage - idHero) / 10000);
// }

////////////////////////////////////////////////////////////////
XCampObjHero2::XCampObjHero2( XPropCamp::xPropHero *pProp )
	: XCampObj( xCT_HERO2, pProp, 0 )
{
	Init();
}

XSPStageObj XCampObjHero2::CreateStageObj( XSPPropStage spPropStage, int idxFloor )
{
	auto spStageObj = XSPStageObj( new XStageObjHero2( spPropStage ) );
	auto spStageHeroObj = std::static_pointer_cast<XStageObjHero2>( spStageObj );
	if( spStageHeroObj ) {
		auto spPropStageHero = std::static_pointer_cast<XPropCamp::xStageHero>( spPropStage );
		if( spPropStageHero )
			spStageHeroObj->SetidHero( spPropStageHero->m_idHero );
	}
	return spStageObj;
}

int XCampObjHero2::Serialize( XArchive& ar )
{
	XCampObj::Serialize( ar );
	ar << (short)m_idxFloorByOpen;
	ar << (short)0;
	return 1;
}

int XCampObjHero2::DeSerialize( XArchive& ar, int verCamp )
{
	XCampObj::DeSerialize( ar, verCamp );
	short s0;
	ar >> s0;		m_idxFloorByOpen = s0;
	// 층 객체가 부족하면 더 만듬.
	if( GetaryFloor().Size() <= m_idxFloorByOpen ) {
		const int idxOld = GetaryFloor().Size();
		GetaryFloor().resize( m_idxFloorByOpen + 1 );
		for( int f = idxOld; f <= m_idxFloorByOpen; ++f ) {
			CreateFloor( f );
		}
	}
	// validate검사.
	for( auto& aryStages : GetaryFloor() ) {
		for( auto spStageObj : aryStages ) {
			auto spStageObjHero = std::static_pointer_cast<XStageObjHero2>( spStageObj );
			if( spStageObjHero ) {
				if( spStageObjHero->GetidHero() == 0 ) {
					const ID idHero = spStageObjHero->GetspProp()->m_idHero;
					spStageObjHero->SetidHero( idHero );
				}
			}
		}
	}
	return 1;
}

/**
 @brief 24시간이 지나 캠페인이 리셋되었다.
 각 스테이지에 numClear값 리셋
*/

void XCampObjHero2::ResetCampAllFloor( int lvAcc )
{
	SetcntTry( 0 );
	SetidxLastPlay( -1 );
	for( auto& aryStages : GetaryFloor() ) {
		for( auto spStage : aryStages ) {
			spStage->InitStage();
		}
	}
}

#ifdef _GAME_SERVER
void XCampObjHero2::Update( XSPAcc spAcc )
{
	XCampObj::Update( spAcc );
// 	if( !m_bUpdated ) {
// 		UpdateStages();
// 	}
}
#endif // _GAME_SERVER


#ifdef _GAME_SERVER
void XCampObjHero2::UpdateStages()
{

//	XCampObj::updatest
}
#endif // _GAME_SERVER

/**
 @brief idHero의 리스트상의 인덱스를 얻음.
*/
int XCampObjHero2::GetidxByAryHeroes( const XVector<XPropHero::xPROP*>& ary, 
																			ID idHero )
{
	int idx = 0;
	for( auto pPropHero : ary ) {
		if( pPropHero->idProp == idHero )
			return idx;
		++idx;
	}
	return -1;
}

int XCampObjHero2::GetlvLegion( XPropCamp::xStage* pPropStage, int lvBase, int idxFloor )
{
	auto pPropCamp = GetpProp();
	const float lvStart = 12.f;
	const float lvEnd = 55.f;
	const float lvDist = ( lvEnd - lvStart ) / (float)pPropCamp->m_maxFloor;	// 층당 레벨 간격
//	const float lvLegion = lvStart + (pPropStage->idxStage * lvDist);
	const float lvLegionStartByFloor = lvStart + (idxFloor * lvDist);
	const float lvLegionEndByFloor = lvStart + ( (idxFloor + 1) * lvDist );
//	const float lvArea = lvLegionEndByFloor - lvLegionStartByFloor;
	const int maxStage = pPropCamp->GetnumStages();
	const float lvLegion = lvLegionStartByFloor + ((float)pPropStage->idxStage / maxStage) * lvDist;
	return (int)lvLegion;
}

int XCampObjHero2::GetlvLimit( int idxStage, int idxFloor  )
{
	return 0;		// 일단 제한은 없는걸로 시작.
}

/**
 @brief 부대를 생성할때 어떤 영웅을 생성할것인지를 커스텀하게 결정한다.
*/
ID XCampObjHero2::GetidHeroByCreateSquad( int idxSquad, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage )
{
	auto pPropStageHero = SafeCast<const XPropCamp::xStageHero*>( pPropStage );
	if( pPropStageHero ) {
		return pPropStageHero->m_idHero;
	}
	return XCampObj::GetidHeroByCreateSquad( idxSquad, pSquadProp, pPropStage );
}

/**
 @brief 부대를 생성할때 어떤 유닛을 생성할것인지를 커스텀하게 결정한다.
*/
XGAME::xtUnit XCampObjHero2::GetUnitWhenCreateSquad( int idxSquad
																				, XGAME::xtAttack typeAtk
																				, const XGAME::xSquad* pSquadProp
																				, const XPropCamp::xStage* pPropStage
																				, int idxFloor )
{
// 	if( XBREAK( pPropHero == nullptr ) )
	if( !typeAtk )
		return xUNIT_NONE;
	// 영웅의 공격타입을 얻는다.
	const auto sizeUnit = (xtSize)((idxFloor % (xSIZE_MAX - 1)) + 1);
	if( XBREAK( XGAME::IsInvalidSizeUnit( sizeUnit ) ) )
		return xUNIT_NONE;
	return XGAME::GetUnitBySizeAndAtkType( sizeUnit, typeAtk );
}

bool XCampObjHero2::ClearStage( int idxStage, int idxFloor )
{
	XLOCK_OBJ;
	bool bClear = false;
	// 오픈된마지막층(클리어가 아직 안된 마지막층)을 클리어할때만 아래 처리함.
	if( idxFloor == m_idxFloorByOpen ) {
		if( idxStage == GetidxLastUnlock() ) {
			// 최초클리어한 스테이지는 clear수를 안올림.
			const auto idxLastUnlock = GetidxLastUnlock();
			SetidxLastUnlock( idxLastUnlock + 1 );
			// 마지막 스테이지를 클리어한것이었으면 다음 층 잠금해제
			if( IsEndStage() ) {
				if( m_idxFloorByOpen < GetpProp()->m_maxFloor ) {
					++m_idxFloorByOpen;
					CreateFloor( m_idxFloorByOpen );
//					const auto sizeFloor = GetaryFloor().size();
//					GetaryFloor().resize( sizeFloor + 1 );		// 층하나 추가.
				}
				SetidxLastUnlock( 0 );
				bClear = true;
			}
		} else {
			auto spStageObj = GetspStage( idxStage, idxFloor );
			if( spStageObj ) {
				spStageObj->AddnumClear();
				if( XBREAK( spStageObj->GetnumClear() < 0 ) )
					spStageObj->ClearnumClear();
				spStageObj->DestroyLegion();
			}
		}
	} else {
		// 이미 클리어한층은 클리어횟수만 증가시킨다.
		auto spStageObj = GetspStage( idxStage, idxFloor );
		if( spStageObj ) {
			spStageObj->AddnumClear();
			if( XBREAK( spStageObj->GetnumClear() < 0 ) )
				spStageObj->ClearnumClear();
			spStageObj->DestroyLegion();
		}
	}
	SetidxLastPlay( -1 );		// ??
	return bClear;
}

XGAME::xtError XCampObjHero2::IsAbleTry( const XSPStageObj spStageObj, int idxFloor )
{
	XLOCK_OBJ;
	if( idxFloor < m_idxFloorByOpen ) {
		// 이미 클리어한 층
		// 캠페인 전체 횟수제한이 걸려있다면 횟수를 검사한다.
		auto pProp = GetpProp();
		if( pProp->numTry > 0 ) {
			// 이미 횟수를 다 썼다면 도전못함.
			if( GetcntTry() >= pProp->numTry )
				return XGAME::xE_NOT_ENOUGH_NUM_TRY;
		}
		if( !spStageObj->IsAbleTry() )
			return XGAME::xE_NOT_ENOUGH_NUM_TRY;
		// 이미 다 클리어했으면 더이상 도전 못함.
		if( spStageObj->IsAllNumClear() )
			return XGAME::xE_NOT_ENOUGH_NUM_CLEAR;
		return XGAME::xE_OK;
	} else {
		return XCampObj::IsAbleTry( spStageObj, idxFloor );
	}
}

XSPStageObjHero2 XCampObjHero2::GetspStageObjHero( int idxStage, int idxFloor )
{
	return std::static_pointer_cast<XStageObjHero2>( GetspStage( idxStage, idxFloor ) );
}

XE_NAMESPACE_END; // xCampaign

#endif // _XCAMP_HERO2
