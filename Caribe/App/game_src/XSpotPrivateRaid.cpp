#include "stdafx.h"
#include "XGlobalConst.h"
#include "XXMLDoc.h"
#include "XPropLegionH.h"
#include "XPropLegion.h"
#include "XHero.h"
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
#include "XSystem.h"
#include "XSpotPrivateRaid.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

const int XSpotPrivateRaid::c_maxSquad = 30;

////////////////////////////////////////////////////////////////
XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld )
	: XSpot( pWorld, xSPOT_PRIVATE_RAID )
	, m_aryEnter(2)
	, m_arySelected(2)
{
	Init();
}

XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp )
	: XSpot( pProp, xSPOT_PRIVATE_RAID, pWorld )
	, m_aryEnter( 2 )
	, m_arySelected( 2 )
{
	Init();
}

void XSpotPrivateRaid::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
}

BOOL XSpotPrivateRaid::DeSerialize( XArchive& ar, DWORD ver )
{
	XSpot::DeSerialize( ar, ver );
	return TRUE;
}

void XSpotPrivateRaid::AddEnterHero( XHero* pHero, int idxSide )
{
	if( !pHero )
		return;
	if( IsExistEnterHero( pHero, idxSide ) )
		return;
	m_aryEnter[idxSide].push_back( pHero );

}
bool XSpotPrivateRaid::IsExistEnterHero( XHero* pHero, int idxSide )
{
	if( !pHero )
		return false;
	return m_aryEnter[idxSide].FindByID( pHero->GetsnHero() ) != nullptr;
}

void XSpotPrivateRaid::DelEnterHero( XHero* pHero, int idxSide )
{
	if( !pHero )
		return;
	m_aryEnter[idxSide].DelByID( pHero->GetsnHero() );
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 출전영웅들 간에 교환을 한다.
*/
void XSpotPrivateRaid::ChangeEnterHero( XHero* pHero1, XHero* pHero2, int idxSide )
{
	// 둘중에 하나라도 널이면 안됨
	if( !IsExistEnterHero(pHero1, idxSide) )
		return;
	if( !IsExistEnterHero(pHero2, idxSide) )
		return;
	// 첫번째 원소를 빼고 다음 이터레이터(A)를 받는다.
	// 두번째 원소를 빼고 다음 이터레이터를(B) 받는다.
	// 첫번째 원소를 B에 인서트 한다. b가 end면 맨뒤에 넣는다.
	// 두번째 원소를 A에 인서트 한다. a가 end면 맨뒤에 넣는다.
	auto itor1 = m_aryEnter[idxSide].GetItorByID( pHero1->GetsnHero() );
	auto itor2 = m_aryEnter[idxSide].GetItorByID( pHero2->GetsnHero() );
	(*itor1) = pHero2;
	(*itor2) = pHero1;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 리스트에 없는 pHeroNew영웅을 pExistHero가 있던 위치에 삽업하고 기존 영웅은 삭제
*/
void XSpotPrivateRaid::ReplaceEnterHero( XHero* pHeroNew, XHero* pExistHero, int idxSide )
{
	if( !IsExistEnterHero( pExistHero, idxSide ) )
		return;
	if( !IsExistEnterHero( pHeroNew, idxSide ) )
		return;

	auto itorExist = m_aryEnter[idxSide].GetItorByID( pExistHero->GetsnHero() );
	(*itorExist) = pHeroNew;
}

int XSpotPrivateRaid::GetidxEnterHero( XHero* pHero, int idxSide )
{
	return m_aryEnter[idxSide].GetIndex( pHero );
}

XHero* XSpotPrivateRaid::GetSelectEnterHero( int idxSide )
{
	return m_arySelected[ idxSide ];
}
void XSpotPrivateRaid::SetSelectEnterHero( XHero* pHero, int idxSide )
{
	m_arySelected[ idxSide ] = pHero;
}

bool XSpotPrivateRaid::IsSelectedHero( XHero* pHero, int idxSide )
{
	auto pSelected = m_arySelected[ idxSide ];
	if( pSelected && pSelected->GetsnHero() == pHero->GetsnHero() )
		return true;
	return false;
}

bool XSpotPrivateRaid::Update( XSPAcc spAcc )
{
#if defined(_XSINGLE) || !defined(_CLIENT)
	auto& listEnemy = m_aryEnter[ 1 ];
	if( listEnemy.empty() ) {
		XBREAK( GetLevel() == 0 );
		CreateEnemyEnterHeroes( GetLevel() );
	}
#endif // #if defined(_XSINGLE) || !defined(_CLIENT)

	return true;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 적부대를 생성한다.
*/
void XSpotPrivateRaid::CreateEnemyEnterHeroes( int lvSpot )
{
#if defined(_XSINGLE) || !defined(_CLIENT)
	XGAME::xLegion propLegion;
	auto pPropLegion = XPropLegion::sGet()->GetpProp( GetpProp()->m_idsLegion );
	if( XASSERT(pPropLegion) ) {
		propLegion = *pPropLegion;		// 카피본
		auto spLegion = XLegion::sCreateLegionForNPC2( propLegion, lvSpot, true );
		SetspLegion( spLegion );
		const int remain = c_maxSquad - spLegion->GetNumSquadrons();
//		for( int i = 0; i < c_maxSquad; ++i ) {
		for( int i = 0; i < remain; ++i ) {		// 일단 테스트를 위해서 이렇게
			const xtGet bit = (xtGet)( xGET_GATHA | xGET_QUEST | xGET_GUILD_RAID | xGET_MEDAL_SPOT );
			auto pPropHero = XHero::sGet()->GetpPropRandomByGetType( bit );
			const auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, (xtSize)xRandom( 1, 3 ) );
			auto pHero = XHero::sCreateHero( pPropHero, 1, unit );
			m_aryEnter[1].push_back( pHero );
		}


		일단 테스트를 위해 이런방식으로 전투를 시작해보고
		리스트로 군단을 만드는건 그다음에 하자





// 		auto spLegion = std::make_shared<XLegion>();
// 		const auto& tblLegion = XGC->GetLegionTable( lvSpot );
// 		const int numSquad = tblLegion.m_numSquad;
// 		for( auto pHero, int i = 0 : m_aryEnter[1], ++i ) {
// 
// 			spLegion->AddSquadron( i, )
// 		}
	}
#endif // #if defined(_XSINGLE) || !defined(_CLIENT)

// 	for( int i = 0; i < 30; ++i ) {
// 
// 		auto spLegion = XLegion::sCreateLegionForNPC2()
// 
// 		const xtGet bit = (xtGet)( xGET_GATHA | xGET_QUEST | xGET_GUILD_RAID | xGET_MEDAL_SPOT );
// 		auto pPropHero = XHero::sGet()->GetpPropRandomByGetType( bit );
// 		const auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, (xtSize)xRandom( 1, 3 ) );
// 		XHero::sCreateHero( pPropHero, 1, unit );
// 	}

}