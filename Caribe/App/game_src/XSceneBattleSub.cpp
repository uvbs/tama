#include "StdAfx.h"
#include "XBattleField.h"
#include "XWndBattleField.h"
#include "XLegionObj.h"
#include "XSquadObj.h"
#include "XLegion.h"
#include "XPropLegion.h"
#include "XPropLegionH.h"
#include "XSceneBattle.h"
#include "XSceneBattleSub.h"

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

XE_NAMESPACE_START( XGAME )
	
bool xSceneBattleParam::IsValid() const {
	if( XBREAK( m_spLegion[0] == nullptr || m_spLegion[1] == nullptr ) )
		return false;
	if( XBREAK( !m_typeBattle || !m_Level || m_strName.empty() ) )
		return false;
#ifndef _XSINGLE
	if( XBREAK( !m_typeSpot ) )
		return false;
	if( XBREAK( !m_idSpot ) )
		return false;
	if( m_typeSpot == xSPOT_CASTLE || m_typeSpot == xSPOT_JEWEL || m_typeSpot == xSPOT_MANDRAKE )
		if( XBREAK( m_idEnemy == 0 ) )
			return false;
	if( m_typeSpot == xSPOT_JEWEL )
		if( XBREAK( m_Defense == 0 ) )
			return false;
	if( m_Defense > 0 )
		if( XBREAK( m_typeSpot != xSPOT_JEWEL ) )
			return false;
	if( m_typeSpot == xSPOT_CAMPAIGN || m_typeSpot == xSPOT_COMMON )
		if( XBREAK( m_idxStage < 0 ) )
			return false;
#endif // not _XSINGLE
	return true;
}

//////////////////////////////////////////////////////////////////////////
void xsCamp::Release() {
	m_spAcc.reset();
	m_spLegionObj->Release();

}

#ifdef _XSINGLE
void xsCamp::CreateLegion( const std::string& idsLegion, XGAME::xtSide bitSide ) {
	XBREAK( bitSide == 0 );
	XBREAK( idsLegion.empty() );
	m_idsLegion = idsLegion;
	m_bitSide = bitSide;
	auto pPropLegion = XPropLegion::sGet()->GetpProp( idsLegion );
	if( XASSERT( pPropLegion ) ) {
		m_spLegion = XLegion::sCreateLegionForNPC2( *pPropLegion, 50, false );
	}
}
// void xsCamp::ReCreateLegion( XWndBattleField* pWndWorld) 
// {
// 	CreateLegion( m_idsLegion, m_bitSide );
// 	CreateLegionObj();
// 	CreateSquadObj( pWndWorld, m_bitOption );
// }
#endif // _XSINGLE
// 
void xsCamp::CreateLegionObj()
{
	XBREAK( m_spLegion == nullptr );
	XBREAK( m_bitSide == 0 );
//	m_spLegionObj = XSPLegionObj( new XLegionObj( m_spLegion, m_bitSide ) );
	m_spLegionObj = std::make_shared<XLegionObj>( m_spLegion, m_bitSide );
}

/**
 @brief m_spLegionObj의 분대객체들을 생성한다.
*/
void xsCamp::CreateSquadObjs( XWndBattleField* pWndWorld, XGAME::xtBattle typeBattle )
{
#ifdef _XSINGLE
	XBREAK( m_idsLegion.empty() );
#endif // _XSINGLE
	XBREAK( m_spLegionObj == nullptr );
	XBREAK( pWndWorld == nullptr );
	auto pBattleField = pWndWorld->GetspBattleField();
	XBREAK( pBattleField == nullptr );
	m_bitOption = typeBattle;
	const XE::VEC3 vCenterWorld( pBattleField->GetvwSize().w * 0.5f, 231.f, 0 );
	BIT bitOption = XGAME::xLO_DEFAULT;
	if( typeBattle == xBT_GUILD_RAID && m_bitSide != xSIDE_PLAYER ) {
		bitOption |= xLO_NO_CREATE_DEAD;
	}
	const auto vwBasePos = (m_bitSide == xSIDE_PLAYER)? 
														vCenterWorld - XE::VEC3( 240, 0 ) :
														vCenterWorld + XE::VEC3( 240, 0 );
	// m_pLegion을 바탕으로 내부에서 분대 객체를 만듬. 분대객체안에서는 다시 유닛 객체를 만듬.
	m_spLegionObj->CreateSquadObjList( pWndWorld, vwBasePos, (xtLegionOption)bitOption );
#ifdef _XSINGLE
	auto pPropLegion = XPropLegion::sGet()->GetpProp( m_idsLegion );
	if( pPropLegion ) {
		for( auto& squad : pPropLegion->arySquads ) {
			auto spSquadObj = m_spLegionObj->GetspSquadObjByIdx( squad.idxPos );
			if( spSquadObj ) {
				spSquadObj->m_bShowHpInfo = squad.m_bShow;
			}
		}
	}
#endif // _XSINGLE
}

XE_NAMESPACE_END;

