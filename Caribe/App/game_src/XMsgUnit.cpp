#include "stdafx.h"
#include "XMsgUnit.h"
#include "XBaseUnit.h"
#include "XBaseUnitH.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace XSKILL;

XE_NAMESPACE_START( xnUnit )
//
void XMsgQ::Release() 
{
	for( auto spMsg : m_qMsg1 )
		spMsg->Release();
	m_qMsg1.clear();
}

void XMsgQ::Process( XBaseUnit* pOwner )
{
	while( !m_qMsg1.empty() ) {
		auto& spMsg = m_qMsg1.GetFirst();
		spMsg->Process( pOwner );
		spMsg->Release();
#ifdef _XLEAK_DETECT
		sDelMsg( spMsg->getid() );
#endif // _XLEAK_DETECT
		m_qMsg1.pop_front();
	}
}

#ifdef _XLEAK_DETECT
XList4<XSPMsgBase> XMsgQ::s_qMsgAll;			// memory leak 감지용

void XMsgQ::sDelMsg( ID snMsg )
{
	s_qMsgAll.DelByID( snMsg );
}
#endif // _XLEAK_DETECT
void XMsgQ::sCheckLeak()
{
#ifdef _XLEAK_DETECT
	for( auto itor = s_qMsgAll.begin(); itor != s_qMsgAll.end(); ) {
		auto& spMsg = (*itor);
		const auto cnt = spMsg.use_count();
		if( cnt == 1 ) {
			s_qMsgAll.erase( itor++ );
		} else {
			++itor;
		}
	}
	XBREAK( !s_qMsgAll.empty() );
	s_qMsgAll.clear();
#endif // _XLEAK_DETECT
}

//////////////////////////////////////////////////////////////////////////
int XMsgBase::s_numObj = 0;


void XMsgDmg::Process( XBaseUnit* pOwner )
{
	auto pAtkObj = m_spAtkObj.get();		// 치트로 데미지를 줄수 있으므로 공격자가 널일수있음.
	XBREAK( m_spTarget == nullptr );
	if( m_spTarget->IsDead() || m_spTarget->IsDestroy() )
		return;
	XSPUnit spUnitAtker;
	if( pAtkObj && pAtkObj->GetType() == xOT_UNIT ) {
		spUnitAtker = std::static_pointer_cast<XBaseUnit>( m_spAtkObj );
	}
	const bool bCritical = ( m_bitAttrHit & xBHT_CRITICAL ) != 0;
	const bool bBySkill = ( m_bitAttrHit & xBHT_BY_SKILL ) != 0;	// 스킬이나 특성에의한 타격이다.
	const bool bMiss = ( m_bitAttrHit & xBHT_HIT ) == 0;
	const bool bPoison = ( m_bitAttrHit & xBHT_POISON ) != 0;
	// 공격자에게 "타격시"이벤트를 발생시킨다.
	if( spUnitAtker && !bBySkill ) {		// 일단 스킬데미지는 "타격시"이벤트를 발생시키지 않도록.
		spUnitAtker->OnAttackToDefender( m_spTarget.get(), std::abs(m_Dmg), bCritical, m_ratioPenet, m_typeDmg );
// 		spUnitAtker->OnEventJunctureCommon( xJC_ATTACK );
// 		if( m_typeDmg == xDMG_MELEE )
// 			spUnitAtker->OnEventJunctureCommon( xJC_CLOSE_ATTACK );	// 근접공격시
// 		else
// 		if( m_typeDmg == xDMG_RANGE )
// 			spUnitAtker->OnEventJunctureCommon( xJC_RANGE_ATTACK_ARRIVE );	// 원거리타격시
	}

	m_spTarget->DoDamage( m_spAtkObj
															 , m_Dmg
															 , m_ratioPenet
															 , m_typeDmg
															 , m_bitAttrHit
															 , m_attrDmg );
}

//////////////////////////////////////////////////////////////////////////
XMsgDmgFeedback::XMsgDmgFeedback( const xDmg& dmgInfo )
	: XMsgBase( xUM_DMG_FEEDBACK )
{
	m_dmgInfo = dmgInfo;
}

void XMsgDmgFeedback::Process( XBaseUnit* pOwner )
{
	// 현재는 공격자가 유닛타입일때만 지원함.
	auto spAtker = m_dmgInfo.m_spUnitAtker;
	if( spAtker ) {
		spAtker->OnDamagedToTarget( m_dmgInfo );
	}
}

//////////////////////////////////////////////////////////////////////////
XMsgKillTarget::XMsgKillTarget( const xDmg& dmgInfo )
	: XMsgBase( xUM_KILL_TARGET )
{
	m_dmgInfo = dmgInfo;
}

void XMsgKillTarget::Process( XBaseUnit* pOwner )
{
	// 현재는 공격자가 유닛타입일때만 지원함.
	auto spAtker = m_dmgInfo.m_spUnitAtker;
	if( spAtker ) {
		// 공격자에게 "적사살시" 이벤트를 날림.
		spAtker->OnEventJunctureCommon( xJC_KILL_ENEMY, 0, m_dmgInfo.m_spTarget.get() );
	}
}
//
//////////////////////////////////////////////////////////////////////////
XMsgAddAdjParam::XMsgAddAdjParam( int adjParam, xtValType valType, float adj )
	: XMsgBase( xUM_ADD_ADJPARAM )
	, m_AdjParam( (xtParameter)adjParam )
	, m_valType( valType )
	, m_valAdj( adj )
{
}

void XMsgAddAdjParam::Process( XBaseUnit* pOwner )
{
	// pOwner에게 보정파라메터 적용
	pOwner->AddAdjParam( m_AdjParam, m_valType, m_valAdj );
}

//////////////////////////////////////////////////////////////////////////
XMsgSetState::XMsgSetState( XGAME::xtState idxState, bool bFlag )
	: XMsgBase( xUM_SET_STATE )
	, m_idxState( idxState )
	, m_bFlag( bFlag )
{
}

void XMsgSetState::Process( XBaseUnit* pOwner )
{
	// pOwner에게 보정파라메터 적용
	pOwner->SetState( m_idxState, m_bFlag );
}


XE_NAMESPACE_END; // xnUnit


