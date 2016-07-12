#include "stdafx.h"
#include "XCampObjHero.h"
#include "XStageObjHero.h"
#include "XPropHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( xCampaign )

/**
 @brief idHero를 바탕으로 고유스테이지 아이디를 만든다.
*/
ID XCampObjHero::sGetidStageByidHero( ID idHero, int idxStage, int cycle )
{
	XASSERT( cycle > 0 );
	return (10000 * cycle) + idHero;
//	return (100000 * idxStage) + idHero;
}
int XCampObjHero::sGetCycleByidStage( ID idStage, ID idHero )
{
	XASSERT( idStage >= 10000 );
	return ((idStage - idHero) / 10000);
}

////////////////////////////////////////////////////////////////
XCampObjHero::XCampObjHero( XPropCamp::xProp *pProp )
	: XCampObj( xCT_HERO, pProp, 0 )
{
	Init();
}

XSPStageObj XCampObjHero::CreateStageObj( XSPPropStage spPropStage, int idxFloor )
{
	auto spStageObj = XSPStageObj( new XStageObjHero( spPropStage ) );
	return spStageObj;
}

int XCampObjHero::DeSerialize( XArchive& ar, int verCamp )
{
	XCampObj::DeSerialize( ar, verCamp );
#ifdef _GAME_SERVER
	// 로딩후 스테이지를 순회하며 군단레벨을 다시 계산한다.
	int maxStage = GetaryStages().size();
	for( int i = 0; i < maxStage; ++i ) {
		auto spStageObj = GetspStage( i );
		if( XASSERT(spStageObj) ) {
			auto spPropStage = GetspStage( i )->GetspPropStage();
			if( XASSERT(spPropStage) ) {
				int lvLegion = GetlvLegionByidxHero( i, maxStage );
				XStageObjHero::sSetLevelLegion( spPropStage, lvLegion );
				spStageObj->SetLevelLegion( lvLegion );
				// 스테이지 클릭할때 드랍템 세팅하도록 바뀜
// 				for( auto& reward : spPropStage->aryReward )
// 					spStageObj->AddItems( reward.GetidItem(), reward.num );
			}
		}
	}
#endif // _GAME_SERVER
	return 1;
}

#ifdef _GAME_SERVER
void XCampObjHero::Update( XSPAcc spAcc )
{
	XCampObj::Update( spAcc );
	if( !m_bUpdated ) {
		UpdateStages();
	}
}
#endif // _GAME_SERVER

void XCampObjHero::ResetCamp( int lvAcc, int idxFloor  )
{
	auto idxLastPlay = GetidxLastPlay();
	auto idxLastUnlock = GetidxLastUnlock();
	XCampObj::ResetCamp( lvAcc, idxFloor );
	SetidxLastPlay( idxLastPlay );
	SetidxLastUnlock( idxLastUnlock );
}

bool CompareProp( XPropHero::xPROP *pProp1, XPropHero::xPROP *pProp2 )
{
	return ( pProp1->priority < pProp2->priority );
}

#ifdef _GAME_SERVER
void XCampObjHero::UpdateStages()
{
	// 최신버전 영웅리스트를 뽑는다.
	XVector<XPropHero::xPROP*> aryProps;
	PROP_HERO->GetpPropByGetToAry( &aryProps, XGAME::xGET_GATHA );
	PROP_HERO->GetpPropByGetToAry( &aryProps, XGAME::xGET_QUEST );
	std::sort( aryProps.begin(), aryProps.end(), CompareProp );
//	MakeHeroesList( &aryProps );
	const int maxStage = aryProps.size() * 3;
	//////////////////////////////////////////////////////////////////////////
	// 삭제된 스테이지 처리
	// 현재 스테이지리스트 백업
	std::vector<XSPStageObj> aryOld = GetaryStages();
	ClearStageAll();	// 현재스테이지리스트 삭제
	GetaryStages().resize( maxStage );	// 리스트크기 다시 잡음.
	for( auto spStageObj : aryOld ) {
		auto pStageObjHero = SafeCast<XStageObjHero*>( spStageObj.get() );
		if( XASSERT(pStageObjHero) ) {
			// 스테이지에 배정된 영웅의 원본리스트상에서의 인덱스를 얻음.
			int cycle = sGetCycleByidStage( spStageObj->GetidProp(), pStageObjHero->GetidHero() );
			XASSERT(cycle > 0);	// 첫번째 사이클은 1부터 시작.
			int idxHero = GetidxByAryHeroes( aryProps, pStageObjHero->GetidHero() );
//			int idxNew = GetidxByAryHeroes( aryProps, pStageObjHero->GetidHero() );
			if( idxHero >= 0 ) {
				int idxNew = ( (cycle-1) * aryProps.size() ) + idxHero;
				// 새인덱스에 스테이지를 담는다.
				SetspStageObj( idxNew, spStageObj );
			}
			// idxNew가 -1이면 삭제된스테이지이므로 리스트에 담지 않는다.
		}
	}
	// 추가된 스테이지 채워넣기
//	int idxStage = 0;
	for( int i = 0; i < maxStage; ++i ) {
		int idxHero = i % aryProps.size();
		int cycle = (i / aryProps.size()) + 1;
		auto pPropHero = aryProps[ idxHero ];
		// 비어있는 스테이지엔 새로 만들어 넣어줌.
		if( GetspStage( i ) == nullptr ) {
			ID idStage = sGetidStageByidHero( pPropHero->idProp, i, cycle );
			auto spPropStage 
				= XStageObjHero::sCreatePropStage( GetpProp(),
													idStage,
													i, 
													pPropHero->idProp );
			int lvLegion = GetlvLegionByidxHero( i, maxStage );
			XStageObjHero::sSetLevelLegion( spPropStage, lvLegion );
			if( XASSERT(spPropStage) ) {
				auto pStageObj = new XStageObjHero( spPropStage );
				pStageObj->SetidHero( pPropHero->idProp );
				pStageObj->SetLevelLegion( lvLegion );
				// 스테이지 클릭할때 드랍템 세팅하도록 바뀜
// 				for( auto& reward : spPropStage->aryReward )
// 					pStageObj->AddItems( reward.GetidItem(), reward.num );
				auto spStageObj = XSPStageObj( pStageObj );
// 				auto spStageObj = CreateStageObj( pPropStage );
				SetspStageObj( i, spStageObj );
			}
		}
	}
	m_bUpdated = true;

}
#endif // _GAME_SERVER
/**
 @brief 가챠로 뽑을수 있는 영웅리스트를 받아서 3배로늘여 순환되는 배열을 만든다.
*/
// void XCampObjHero::MakeHeroesList( XVector<XPropHero::xPROP*> *pOutAry )
// {
// 	XVector<XPropHero::xPROP*> aryProps;
// 	PROP_HERO->GetpPropByGetToAry( &aryProps, XGAME::xGET_GATHA );
// 	// 영웅리스트 소트
// 	// aryProps.sort();
// 	const int maxStage = aryProps.size() * 3;
// 	for( int i = 0; i < maxStage; ++i ) {
// 		auto pProp = aryProps[ i % aryProps.size() ];
// 		pOutAry->push_back( pProp );
// 	}
// 	
// }
/**
 @brief 해당스테이지의 군단레벨을 계산한다.
*/
int XCampObjHero::GetlvLegionByidxHero( int idxStage, int maxStage ) const
{
	const float lvStart = 12.f;	// 1스테이지의 레벨
	const float lvLastStage = 55.f;	// 마지막스테이지의 레벨
	return (int)(lvStart + (((lvLastStage - lvStart) / maxStage) * idxStage));
}
/**
 @brief idHero의 리스트상의 인덱스를 얻음.
*/
int XCampObjHero::GetidxByAryHeroes( const XVector<XPropHero::xPROP*>& ary, 
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

void XCampObjHero::CreateLegionIfEmpty( int lvBase, int idxFloor )
{
	// 영웅의 전장은 미리 부대를 만들어 두지 않는다.
}

XE_NAMESPACE_END; // xCampaign

