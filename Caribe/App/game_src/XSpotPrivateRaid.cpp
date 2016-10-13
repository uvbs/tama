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
using namespace xnLegion;

// const int XSpotPrivateRaid::c_maxSquad = 30;
// const int XSpotPrivateRaid::c_maxWins = 2;

////////////////////////////////////////////////////////////////
XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld )
	: XSpot( pWorld, xSPOT_PRIVATE_RAID )
	, m_arySelected(2)
{
	Init();
}

XSpotPrivateRaid::XSpotPrivateRaid( XWorld* pWorld, XPropWorld::xBASESPOT* pProp )
	: XSpot( pProp, xSPOT_PRIVATE_RAID, pWorld )
	, m_arySelected( 2 )
{
	Init();
}


void XSpotPrivateRaid::Serialize( XArchive& ar )
{
	XBREAK( m_listEnterEnemy.empty() && GetspLegion() );
	// 적군단
	if( ar.IsForDB() ) {
		// db저장시에는 적군단 정보를 저장하지 않는다. 출전리스트가 있으므로.
		DestroyLegion();	
	}
	//
	XSpot::Serialize( ar );
	//
	// 플레이어 군단
	ar << m_legionDatPlayer;
	// 플레이어측 전체 출전영웅
	ar << m_listEnterPlayer;
	if( !ar.IsForDB() ) {
		// 랜덤목록이므로 DB에 저장하지 않는다.
		// 적부대추가 출전인원 
		// 본 부대외에 추가 영웅들 리스트를 풀버전으로 보낸다.
		SerializeEnterEnemy( ar );
	} else {
		ar << 0;
	}
	ar << (char)m_numWins;
	ar << (char)0;
	ar << (char)0;
	ar << (char)0;
	ar << m_secTimer;

}

void XSpotPrivateRaid::SerializeEnterEnemy( XArchive& ar ) const
{
	ar << m_listEnterEnemy.size();
	for( auto pHero : m_listEnterEnemy ) {
		ar << pHero->GetsnHero();
		XHero::sSerialize( ar, pHero );
	}
}


BOOL XSpotPrivateRaid::DeSerialize( XArchive& ar, DWORD ver )
{
	XSpot::DeSerialize( ar, ver );
	//
	ar >> m_legionDatPlayer;			// 
	ar >> m_listEnterPlayer;
	DeSerializeEnterEnemy( ar, ver );
	char c0;
	ar >> c0;		m_numWins = c0;
	ar >> c0 >> c0 >> c0;
	ar >> m_secTimer;
	return TRUE;
}

void XSpotPrivateRaid::DeSerializeEnterEnemy( XArchive& ar, int verWorld )
{
	const bool bEmpty = m_listEnterEnemy.empty();
	int numEnter;
	ar >> numEnter;
	if( numEnter ) {
		for( int k = 0; k < numEnter; ++k ) {
			ID snHero;
			ar >> snHero;
			if( bEmpty ) {
				auto pHero = XHero::sCreateDeSerialize2( ar, nullptr );
				m_listEnterEnemy.push_back( pHero );
			} else {
				auto ppHero = m_listEnterEnemy.GetpByIndex( k );
				if( ppHero && ( *ppHero )->GetsnHero() == snHero ) {
					auto pHero = *ppHero;
					XHero::sDeSerializeUpdate( ar, pHero, nullptr );
				} else {
					auto pHero = XHero::sCreateDeSerialize2( ar, nullptr );
					m_listEnterEnemy.push_back( pHero );
				}
			}
		}
	} else {
		//XLIST4_DESTROY( m_listEnterEnemy );
		m_listEnterEnemy.clear();
	}
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 플레이어의 전체 출전리스트를 받아 앞부분은 군단으로 생성하고 나머지는 대기열에 넣는다.
*/
void XSpotPrivateRaid::UpdatePlayerEnterList( const XList4<ID>& listHero, int lvAcc )
{
	// 리스트가 올라올때마다 군단과 리스트를 모두 갱신한다.
	m_listEnterPlayer.clear();
	m_legionDatPlayer.Clear();
	m_listEnterPlayer = listHero;		// 군단소속영웅 포함해서 모두 리스트에 넣는다.
	// 전체 출전리스트를 건네고 필요한 인원만 부대에 포함시킨다.
	ProcCreateSquadron( &m_legionDatPlayer, listHero, lvAcc );
	// 나머지 인원은 대기열 리스트에 넣는다.
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 영웅리스트를 바탕으로 플레이어측 군단객체를 생성한다.
*/
void XSpotPrivateRaid::ProcCreateSquadron( xLegionDat* pOutDat,
																					 const XList4<ID>& listHero, 
																					 int lvAcc ) const
{
	// 플레이어의 현재 레벨에서 가질수 있는 최대 부대수
	const int maxHero = XAccount::sGetMaxSquadByLevelForPlayer( lvAcc );
	// 영웅리스트를 바탕으로 군단객체를 생성한다. 영웅은 반드시 보유중인 영웅이어야 한다.
	int idx = 0;
	for( const auto snHero : listHero ) {
		pOutDat->AddSquad( idx, snHero );
		if( ++idx >= maxHero )
			break;
	}
}


#ifdef _CLIENT
void XSpotPrivateRaid::AddEnterHero( XSPHero pHero, int idxSide )
{
	if( !pHero )
		return;
	if( IsExistEnterHero( pHero, idxSide ) )
		return;
	if( idxSide == 0 ) {
		m_listEnterPlayer.push_back( pHero->GetsnHero() );
	} else {
		m_listEnterEnemy.push_back( pHero );
	}
//	m_aryEnter[idxSide].push_back( pHero );

}
bool XSpotPrivateRaid::IsExistEnterHero( XSPHero pHero, int idxSide )
{
	if( !pHero )
		return false;
	if( idxSide == 0 ) {
		return m_listEnterPlayer.IsExist( pHero->GetsnHero() );
	}
	return m_listEnterEnemy.FindByID( pHero->GetsnHero() ) != nullptr;
	//return m_aryEnter[idxSide].FindByID( pHero->GetsnHero() ) != nullptr;
}

void XSpotPrivateRaid::DelEnterHero( XSPHero pHero, int idxSide )
{
	if( !pHero )
		return;
	if( idxSide == 0 ) {
		m_listEnterPlayer.Del( pHero->GetsnHero() );
	} else {
		m_listEnterEnemy.DelByID( pHero->GetsnHero() );
	}
	//m_aryEnter[idxSide].DelByID( pHero->GetsnHero() );
}
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 출전영웅들 간에 교환을 한다.
*/
void XSpotPrivateRaid::ChangeEnterHero( XSPHero pHero1, XSPHero pHero2, int idxSide )
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
	if( idxSide == 0 ) {
		auto itor1 = m_listEnterPlayer.GetItor( pHero1->GetsnHero() );
		auto itor2 = m_listEnterPlayer.GetItor( pHero2->GetsnHero() );
		( *itor1 ) = pHero2->GetsnHero();
		( *itor2 ) = pHero1->GetsnHero();
	} else {
		auto itor1 = m_listEnterEnemy.GetItorByID( pHero1->GetsnHero() );
		auto itor2 = m_listEnterEnemy.GetItorByID( pHero2->GetsnHero() );
		(*itor1) = pHero2;
		(*itor2) = pHero1;
	}
}
	
/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 리스트에 없는 pHeroNew영웅을 pExistHero가 있던 위치에 삽업하고 기존 영웅은 삭제
*/
void XSpotPrivateRaid::ReplaceEnterHero( XSPHero pHeroNew, XSPHero pExistHero, int idxSide )
{
	if( !IsExistEnterHero( pExistHero, idxSide ) )
		return;
	if( !IsExistEnterHero( pHeroNew, idxSide ) )
		return;
	if( idxSide == 0 ) {
		auto itorExist = m_listEnterPlayer.GetItor( pExistHero->GetsnHero() );
		( *itorExist ) = pHeroNew->GetsnHero();
	} else {
		auto itorExist = m_listEnterEnemy.GetItorByID( pExistHero->GetsnHero() );
		( *itorExist ) = pHeroNew;
	}
}

#endif // _CLIENT


int XSpotPrivateRaid::GetidxEnterHero( XSPHero pHero, int idxSide )
{
	if( idxSide == 0 ) {
		return m_listEnterPlayer.GetIndex( pHero->GetsnHero() );
	}
	return m_listEnterEnemy.GetIndexByID( pHero );
}

XSPHero XSpotPrivateRaid::GetSelectEnterHero( int idxSide )
{
	return m_arySelected[ idxSide ];
}

/** //////////////////////////////////////////////////////////////////
 @brief 도전횟수를 다시 초기화 한다.
*/
void XSpotPrivateRaid::ResetTry()
{
	m_numWins = 0;
	m_secTimer = XTimer2::sGetTime();
}

void XSpotPrivateRaid::SetSelectEnterHero( XSPHero pHero, int idxSide )
{
	m_arySelected[idxSide] = pHero;
}

bool XSpotPrivateRaid::IsSelectedHero( XSPHero pHero, int idxSide )
{
	auto pSelected = m_arySelected[ idxSide ];
	if( pSelected && pSelected->GetsnHero() == pHero->GetsnHero() )
		return true;
	return false;
}

void XSpotPrivateRaid::OnAfterBattle( XSPAcc spAccWin
																			, ID idAccLose
																			, bool bWin
																			, int numStar
																			, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
	if( bWin ) {
		m_listEnterEnemy.clear();
		const int maxWins = GetPropParam().GetInt( "num_win" );
		if( m_numWins < maxWins )
			++m_numWins;

	}
}


// int XSpotPrivateRaid::DoDropItem( XSPAcc spAcc, 
// 																	XArrayLinearN<ItemBox, 256> *pOutAry, 
// 																	int lvSpot, 
// 																	float multiplyDropNum ) const
// {
// }

bool XSpotPrivateRaid::Update( XSPAcc spAcc )
{
#if defined(_XSINGLE) || !defined(_CLIENT)
	if( m_listEnterEnemy.empty() || GetspLegion() == nullptr) {
		SetLevel( GetpProp()->level );
		CreateEnemyEnterHeroes( GetLevel() );
		//
		auto pProp = PROP_ITEM->GetpProp( GetPropParam().GetStrt("drop") );
		if( XASSERT( pProp ) ) {
			int num = xRandom( GetPropParam().GetInt( "min" ), GetPropParam().GetInt( "max" ) );
			xDropItem drop( pProp->idProp, num, 1.f );;
			AddDropItem( drop );
		}
	}
	// 타이머 세팅 안되어 있으면 타이머 켜고 도전회수 채움
	if( m_secTimer == 0 ) {
		ResetTry();
	} else {
		// 타이머가 켜져있고 한시간이 지났으면 도전회수 다시 채우고 타이머 리셋
		const xSec secReset = GetPropParam().GetInt("sec_reset");
		if( XTimer2::sGetTime() - m_secTimer > secReset ) {
			ResetTry();
		}
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
		m_listEnterEnemy.clear();
		// 군단소속 영웅들을 리스트에 넣는다.
		spLegion->GetHeroesToList( &m_listEnterEnemy );
		// 생성후 모자라는 수는 랜덤으로 생성.
		const auto& tblLegion = XGC->GetLegionTable( lvSpot );
		const int lvSquad = tblLegion.m_lvSquad;
		const int maxSquad = GetpBaseProp()->m_Param.GetInt("max_squad");
		const int remain = maxSquad - spLegion->GetNumSquadrons();
//		for( int i = 0; i < c_maxSquad; ++i ) {
		for( int i = 0; i < remain; ++i ) {		// 일단 테스트를 위해서 이렇게
			const xtGet bit = (xtGet)( xGET_GATHA | xGET_QUEST | xGET_GUILD_RAID | xGET_MEDAL_SPOT );
			auto pPropHero = XHero::sGet()->GetpPropRandomByGetType( bit );
			const auto unit = XGAME::GetRandomUnit( pPropHero->typeAtk, (xtSize)xRandom( 1, 3 ) );
			auto pHero = XHero::sCreateHero( pPropHero, lvSquad, unit, nullptr );
			m_listEnterEnemy.push_back( pHero );
		}
	}
#endif // #if defined(_XSINGLE) || !defined(_CLIENT)
}

void XSpotPrivateRaid::SerializeForBattle( XArchive* pOut, const XParamObj2& param )
{
	XSpot::SerializeForBattle( pOut, param );
	// 플레이어측 군단과 출전리스트를 sn형태로 팩킹
//	XLegion::sSerialize( m_spLegionPlayer, pOut );
	// 플레이어측 군단정보
	*pOut << m_legionDatPlayer;
	// 플레이어측 추가 출전정보
	*pOut << m_listEnterPlayer;
// 	*pOut << m_listEnterPlayer.size();
// 	for( auto snHero : m_listEnterPlayer ) {
// 		*pOut << snHero;
// 	}
	// 적측 군단과 출전리스트 팩킹
	XBREAK( GetspLegion() == nullptr );
	XLegion::sSerialize( GetspLegion(), pOut );
	// 적측 영웅정보는 풀버전으로 받는다.
	SerializeEnterEnemy( *pOut );
// 	*pOut << m_listEnterEnemy.size();
// 	for( auto pHero : m_listEnterEnemy ) {
// 		XHero::sSerialize( pOut, pHero );
// 	}
	MAKE_CHECKSUM(*pOut);
}

void XSpotPrivateRaid::DeSerializeForBattle( XArchive& ar, XArchive& arAdd, XSPAcc spAcc )
{
	XSpot::DeSerializeForBattle( ar, arAdd, spAcc );
	//
//	XLegion::sDeSerializeUpdate( m_spLegionPlayer, spAcc, ar );
	// 플레이어측 군단정보
	ar >> m_legionDatPlayer;
	// 플레이어측 추가 출전 정보
	ar >> m_listEnterPlayer;
// 	m_listEnterPlayer.clear();
//	XLIST4_DESTROY( m_listEnterEnemy );
// 	ar >> size;
// 	for( int i = 0; i < size; ++i ) {
// 		ID snHero;
// 		ar >> snHero;
// 		auto pHero = spAcc->GetpcHeroBySN( snHero );
// 		if( XASSERT(pHero) ) {
// 			m_aryEnter[0].push_back( std::const_pointer_cast<XHero>( pHero ) );
// 		}
// 	}
	// 적측
	XLegion::sDeSerializeUpdate( GetspLegion(), spAcc, ar );
	DeSerializeEnterEnemy( ar, 0 );
// 	int size;
// 	ar >> size;
// 	for( int i = 0; i < size; ++i ) {
// 		auto pHero = XHero::sCreateDeSerialize2( ar, nullptr );
// 		if( XASSERT( pHero ) ) {
// 			m_listEnterEnemy.push_back( pHero );
// 		}
// 	}
	RESTORE_VERIFY_CHECKSUM_NO_RETURN(ar);
}

XList4<XSPHero> XSpotPrivateRaid::GetlistEnter( int idxSide )
{
	if( idxSide == 0 ) {
		if( XASSERT(!GetspOwner().expired()) ) {
			XList4<XSPHero> listEnter;
			for( auto snHero : m_listEnterPlayer ) {
				auto pHero = GetspOwner().lock()->GetpHeroBySN( snHero );
				listEnter.push_back( pHero );
			}
			return listEnter;
		}
	}
	return m_listEnterEnemy;
}

