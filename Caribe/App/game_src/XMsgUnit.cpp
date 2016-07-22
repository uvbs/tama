xFL_AI
#include "stdafx.h"
#include "XMsgUnit.h"
#include "XBaseUnit.h"

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
void XMsgQ::Process()
{
	while( !m_qMsg1->empty() ) {
		auto& spMsg = m_qMsg1->GetFirst();
		spMsg->Process();
		m_qMsg1->pop_front();
	}
}

//////////////////////////////////////////////////////////////////////////
void XMsgDmg::Process()
{
	auto pAtkObj = m_spAtkObj.get();		// 치트로 데미지를 줄수 있으므로 공격자가 널일수있음.
	XBREAK( m_spTarget == nullptr );
	if( m_spTarget->IsDead() || m_spTarget->IsDestroy() )
		return;
	XSPUnit spUnitAtker;
	if( m_spAtkObj->GetType() == xOT_UNIT ) {
		spUnitAtker = std::static_pointer_cast<XBaseUnit>( m_spAtkObj );
	}
	const bool bCritical = ( m_bitAttrHit & xBHT_CRITICAL ) != 0;
	const bool bBySkill = ( m_bitAttrHit & xBHT_BY_SKILL ) != 0;	// 스킬이나 특성에의한 타격이다.
	const bool bMiss = ( m_bitAttrHit & xBHT_HIT ) == 0;
	const bool bPoison = ( m_bitAttrHit & xBHT_POISON ) != 0;
	// 공격자에게 "타격시"이벤트를 발생시킨다.
	if( spUnitAtker && !bBySkill ) {		// 일단 스킬데미지는 "타격시"이벤트를 발생시키지 않도록.
		spUnitAtker->OnAttackToDefender( spUnitAtker.get(), std::abs(m_Dmg), bCritical, m_ratioPenet, m_typeDmg );
	}

	m_spTarget->DoDamage( pAtkObj
											, m_Dmg
											, m_ratioPenet
											, m_typeDmg
											, m_bitAttrHit
											, m_attrDmg );
}

//////////////////////////////////////////////////////////////////////////
XMsgDmgFeedback::XMsgDmgFeedback( const xDmg& dmgInfo )
{
	m_dmgInfo = dmgInfo;
}

void XMsgDmgFeedback::Process()
{
	// 현재는 공격자가 유닛타입일때만 지원함.
	auto spAtker = m_dmgInfo.m_spUnitAtker;
	if( spAtker ) {
		spAtker->OnDamagedToTarget( m_dmgInfo );
	}
}

//////////////////////////////////////////////////////////////////////////
XMsgKillTarget::XMsgKillTarget( const xDmg& dmgInfo )
{
	m_dmgInfo = dmgInfo;
}

void XMsgKillTarget::Process()
{
	// 현재는 공격자가 유닛타입일때만 지원함.
	auto spAtker = m_dmgInfo.m_spUnitAtker;
	if( spAtker ) {
		// 공격자에게 "적사살시" 이벤트를 날림.
		spAtker->OnEventJunctureCommon( xJC_KILL_ENEMY, 0, m_dmgInfo.m_spTarget.get() );
	}
}
//
XE_NAMESPACE_END; // xnUnit


