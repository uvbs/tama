﻿#include "stdafx.h"
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

XSPLegion XSpotPrivateRaid::GetspLegionPlayer()
{
	if( _m_spLegionPlayer == nullptr ) {
		XBREAK( m_legionDatPlayer.IsInvalid() );
		auto spAcc = (GetspOwner().expired())? nullptr : GetspOwner().lock();
		XBREAK( spAcc == nullptr );
		_m_spLegionPlayer = XLegion::sCreateLegionWithDat( m_legionDatPlayer, spAcc );
	}
	return _m_spLegionPlayer;
}



void XSpotPrivateRaid::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
//	XLegion::sSerialize( m_spLegionPlayer, &ar );
	ar << m_legionDatPlayer;
	// 본 부대외에 추가 영웅들 리스트를 풀버전으로 보낸다.
	ar << m_aryEnter[1].size();
	for( auto pHero : m_aryEnter[1] ) {
		XHero::sSerialize( ar, pHero );
	}
}

BOOL XSpotPrivateRaid::DeSerialize( XArchive& ar, DWORD ver )
{
	XSpot::DeSerialize( ar, ver );
	//
//	XSPAcc spAcc;
//	XLegion::sDeSerializeUpdate( m_spLegionPlayer, spAcc, ar );
	_m_spLegionPlayer = nullptr;
	ar >> m_legionDatPlayer;			// 일단 데이터로만 읽어두고 Deserial이 끝나면 객체를 만든다.
	XLIST4_DESTROY( m_aryEnter[1] );
	int numEnter;
	ar >> numEnter;
	for( int k = 0; k < numEnter; ++k ) {
			auto pHero = XHero::sCreateDeSerialize2( ar, nullptr );
 			m_aryEnter[1].push_back( pHero );
//			}
	}
	return TRUE;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 플레이어의 전체 출전리스트를 받아 앞부분은 군단으로 생성하고 나머지는 대기열에 넣는다.
*/
void XSpotPrivateRaid::UpdatePlayerEnterList( const XList4<ID>& _listHero, XSPAccConst spAcc )
{
	XList4<ID> listHero = _listHero;
	// 플레이어 군단객체 생성(외부에서의 군단포인터 참조문제로 한번 생성하면 바꾸지 않음)
	auto spLegionPlayer = _m_spLegionPlayer;
	if( spLegionPlayer == nullptr ) {
		spLegionPlayer = std::make_shared<XLegion>();
	}
	// 군단내 기존데이타 삭제
	spLegionPlayer->DestroySquadronAll();
	// 전체 출전리스트를 건네고 필요한 인원만 부대를 생성하고 생성한 부대는 리스트에서 뺀다.
	ProcCreateSquadron( spLegionPlayer, &listHero, spAcc );
	// 나머지 인원은 대기열 리스트에 넣는다.
	m_aryEnter[0].clear();
	for( auto snHero : listHero ) {
		auto pHero = spAcc->GetpcHeroBySN( snHero );
		if( XASSERT( pHero ) ) {
			m_aryEnter[0].push_back( const_cast<XHero*>( pHero ) );
		}
	}
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 영웅리스트를 바탕으로 플레이어측 군단객체를 생성한다.
*/
void XSpotPrivateRaid::ProcCreateSquadron( XSPLegion spLegion, XList4<ID>* pOutlistHero, XSPAccConst spAcc ) const
{
	// 플레이어의 현재 레벨에서 가질수 있는 최대 부대수
	const int maxHero = XAccount::sGetMaxSquadByLevelForPlayer( spAcc->GetLevel() );
	// 영웅리스트를 바탕으로 군단객체를 생성한다. 영웅은 반드시 보유중인 영웅이어야 한다.
	auto& listHero = (*pOutlistHero);
	int idx = 0;
	for( auto itor = listHero.begin(); itor != listHero.end(); ) {
		const ID snHero = ( *itor );
		auto pHero = spAcc->GetpcHeroBySN( snHero );
		if( XASSERT( pHero ) ) {
			spLegion->CreateAddSquadron( idx, pHero, false );
			// 생성한건 리스트에서 뺀다.
			listHero.erase( itor++ );
			if( ++idx >= maxHero )
				break;
		}
	}
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
		// 일반 NPC군단 생성알고리즘으로 레벨에 맞는 부대를 생성함
		auto spLegion = XLegion::sCreateLegionForNPC2( propLegion, lvSpot, true );
		SetspLegion( spLegion );
		// 생성후 모자라는 수는 랜덤으로 생성.
		const auto& tblLegion = XGC->GetLegionTable( lvSpot );
		const int lvSquad = tblLegion.m_lvSquad;
		const int remain = c_maxSquad - spLegion->GetNumSquadrons();
//		for( int i = 0; i < c_maxSquad; ++i ) {
		for( int i = 0; i < remain; ++i ) {		// 일단 테스트를 위해서 이렇게
			const xtGet bit = (xtGet)( xGET_GATHA | xGET_QUEST | xGET_GUILD_RAID | xGET_MEDAL_SPOT );
			auto pPropHero = XHero::sGet()->GetpPropRandomByGetType( bit );
			const auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, (xtSize)xRandom( 1, 3 ) );
			auto pHero = XHero::sCreateHero( pPropHero, lvSquad, unit, nullptr );
			m_aryEnter[1].push_back( pHero );
		}
	}
#endif // #if defined(_XSINGLE) || !defined(_CLIENT)
}

void XSpotPrivateRaid::SerializeForBattle( XArchive* pOut, const XParamObj2& param )
{
	XSpot::SerializeForBattle( pOut, param );
	// 플레이어측 군단과 출전리스트를 sn형태로 팩킹
	XLegion::sSerialize( GetspLegionPlayer(), pOut );
	//
	*pOut << m_aryEnter[0].size();
	for( auto pHero : m_aryEnter[0] ) {
		*pOut << pHero->GetsnHero();
	}
	// 적측 군단과 출전리스트 팩킹
	XBREAK( GetspLegion() == nullptr );
	XLegion::sSerialize( GetspLegion(), pOut );
	// 적측 영웅정보는 풀버전으로 받는다.
	*pOut << m_aryEnter[1].size();
	for( auto pHero : m_aryEnter[1] ) {
		XHero::sSerialize( pOut, pHero );
	}
	MAKE_CHECKSUM(*pOut);
}

void XSpotPrivateRaid::DeSerializeForBattle( XArchive& ar, XArchive& arAdd, XSPAcc spAcc )
{
	XSpot::DeSerializeForBattle( ar, arAdd, spAcc );
	//
	m_aryEnter[0].clear();
	XLIST4_DESTROY( m_aryEnter[1] );
	XLegion::sDeSerializeUpdate( GetspLegionPlayer(), spAcc, ar );
	int size;
	ar >> size;
	for( int i = 0; i < size; ++i ) {
		ID snHero;
		ar >> snHero;
		auto pHero = spAcc->GetpcHeroBySN( snHero );
		if( XASSERT(pHero) ) {
			m_aryEnter[0].push_back( const_cast<XHero*>( pHero ) );
		}
	}
	// 적측
	XLegion::sDeSerializeUpdate( GetspLegion(), spAcc, ar );
	ar >> size;
	for( int i = 0; i < size; ++i ) {
		auto pHero = XHero::sCreateDeSerialize2( ar, nullptr );
		if( XASSERT( pHero ) ) {
			m_aryEnter[1].push_back( pHero );
		}
	}
	

}

