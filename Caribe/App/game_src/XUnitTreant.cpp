#include "stdafx.h"
#include "XUnitTreant.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XEObjMngWithType.h"
#include "XMsgUnit.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XSKILL;
using namespace XGAME;

////////////////////////////////////////////////////////////////
XUnitTreant::XUnitTreant( XSquadObj *pSquadObj, 
													ID idProp,
													BIT bitSide, const 
													XE::VEC3& vPos,
													float multipleAbility )
: XUnitCommon( pSquadObj, idProp, bitSide, vPos, multipleAbility )
{
	Init();

}

void XUnitTreant::Destroy()
{
}

void XUnitTreant::sShootRock( UnitPtr spShooter,
															UnitPtr& spTarget,
															const XE::VEC3& vwSrc,
															const XE::VEC3& vwDst,
															float damage,
															bool bCritical,
															const _tstring& _strSpr )
{
	_tstring strSpr = _strSpr;
	if( strSpr.empty() )
		strSpr = _T( "rock.spr" );
	ID idAct = 1 + random( 2 );
	{
		auto pBuff = spShooter->FindBuffSkill( _T( "adaman_rock" ) );
		if( pBuff ) {
			strSpr = _T( "rock_adaman.spr" );
			idAct = 1;
		}
	}
	// 바위생성
	auto pRock = new XObjRock( spShooter->GetpWndWorld(),
														spShooter,
														spShooter->GetspTarget(),
														vwSrc,
														vwDst,
														damage,
														bCritical,
														strSpr.c_str(), idAct );
	// 탄성특성
	{
		auto pBuff = spShooter->FindBuffSkill( _T( "elastic" ) );
		if( pBuff ) {
			int cntBounce = 1;
			auto pBuff2 = spShooter->FindBuffSkill( _T( "strong_elastic" ) );
			if( pBuff2 )
				cntBounce = (int)pBuff2->GetAbilMinbyLevel();
			// 탄성으로 인해 튀는 바위의 데미지를 설정.
			float addDamageRatio = pBuff->GetAbilMinbyLevel();
			pRock->SetElastic( damage * addDamageRatio, cntBounce );
		}
		// 거대한바위 특성
		pBuff = spShooter->FindBuffSkill( _T( "giant_rock" ) );
		if( pBuff ) {
			const int lvSkill = pBuff->GetLevel();
			pRock->SetSplash( 3.f + lvSkill, 0.5f );
			pRock->SetScaleObj( 2.f + lvSkill );
			// 			float ratio = pBuff->GetAbilMinbyLevel();
// 			pRock->SetSplash( 4.f, ratio );
// 			pRock->SetScaleObj( 2.f );
		}
	}
	pRock->SetpDelegate( spShooter.get() );
	spShooter->GetpWndWorld()->AddObj( WorldObjPtr( pRock ) );
}

void XUnitTreant::ShootRangeAttack( UnitPtr& spTarget,
									const XE::VEC3& vwSrc,
									const XE::VEC3& vwDst,
									float damage,
									bool bCritical,
									const std::string& strType,
									const _tstring& strSpr )
{
	sShootRock( GetThisUnit(), spTarget, vwSrc, vwDst, damage, bCritical, strSpr );
}

// 2/3/4/5/6
void XUnitTreant::OnArriveBullet( XObjBullet *pBullet,
								const UnitPtr& spAttacker,
								const UnitPtr& spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam )
{
	int maxCost = -1;
	float meterRadius = pBullet->GetmeterRadius();
	const BIT bitSide = ~GetCamp();
	float ratio = 1.f;
	XSPUnit spCenter = spTarget;
	bool bIncludeCenter = false;
	XE::VEC2 vCenter;
	XVector<XSPUnit> ary;
	{
		// 거대한바위: 광역속성
		auto pBuff = FindBuffSkill( _T( "giant_rock" ) );
		if( pBuff ) {
			maxCost = (int)pBuff->GetAbilMinbyLevel();
		}
	}
	if( spTarget != nullptr ) {
		// 기본적으로 대상하나에게는 데미지가 들어감.
		XBaseUnit::OnArriveBullet( pBullet, spAttacker, spTarget, vwDst, damage, bCritical, sprArrive, idActArrive, dwParam );
		// 주변에 스플래시데미지
		if( maxCost > 0 ) {
			// 범위공격 대상 선정
			vCenter = spTarget->GetvwPos().ToVec2();
			// 중심타겟은 위에서 이미 데미지를 한번 먹였기때문에 스플래시에서 다시 먹일필요 없댜ㅏ.
			bIncludeCenter = false;
			// 스플래시이므로 데미지율을 적용.
			ratio = pBullet->GetratioDamageSplash();
			XBREAK( ratio == 0 );
		} // maxCost > 0
	} else {
		// 넌타겟팅으로 날아옴.(탄성바위)
		meterRadius = 3.f;		// 이것도 발동반경으로.
		vCenter = vwDst.ToVec2();
		if( maxCost > 0 )
			ratio = pBullet->GetratioDamageSplash();
		// 중심타겟이 없으므로 true적용(의미없음)
		bIncludeCenter = true;
		if( maxCost <0 )
			maxCost = 0;			// 광역속성이 없으면 타겟하나에만 적용되도록.
	}
	// 광역으로 데미지를 먹여야하면 타겟을 얻음.
	if( maxCost >= 0 ) {
		XEObjMngWithType::sGet()->GetListUnitRadius2( &ary,
																									spCenter.get(),
																									vCenter,
																									xMETER_TO_PIXEL( meterRadius ),
																									bitSide,
																									maxCost,		// 0이면 코스트에 관계없이 타겟 하나만 얻음.
																									bIncludeCenter );
	}
	// 각각의 대상에게 데미지.
	for( auto spUnit : ary ) {
		if( XASSERT( spUnit ) ) {
			DoDamageToTarget( spUnit, damage * ratio, XSKILL::xDMG_RANGE, bCritical, XGAME::xDA_NONE );
		}
	} // for

}

void XUnitTreant::DoDamageToTarget( XSPUnit spTarget
																	, float damage
																	, XSKILL::xtDamage typeDmg
																	, bool bCritical
																	, XGAME::xtDamageAttr typeDmgAttr )
{
	if( XASSERT( spTarget ) ) {
		float ratioPenet = GetPenetrationRatio();
		BIT bitHit = XGAME::xBHT_HIT;
		if( bCritical )
			bitHit |= XGAME::xBHT_CRITICAL;
		if( damage == 0 )
			bitHit &= ~XGAME::xBHT_HIT;
//		spTarget->DoDamage( this, damage, ratioPenet, typeDmg, bitHit, typeDmgAttr );
		auto pMsg = std::make_shared<xnUnit::XMsgDmg>( GetThis()
																								, spTarget
																								, damage
																								, ratioPenet
																								, typeDmg
																								, bitHit
																								, typeDmgAttr );
		spTarget->PushMsg( pMsg );
	}
}

void XUnitTreant::FrameMove( float dt )
{
	XUnitCommon::FrameMove( dt );
	//
	if( m_timerSec.IsOff() )
		m_timerSec.Set( 1.f );
	if( m_timerSec.IsOver() ) {
		auto pBuff = FindBuffSkill( _T( "invoke_photosynthesis" ) );
		if( pBuff ) {
			pBuff->AddAbilMin( 0.01f );
			if( m_cntPerSec < 200 )
				++m_cntPerSec;
			float scale = 1.f + (m_cntPerSec * 0.01f);
			const float scaleMax = 1.5f;
			if( scale > scaleMax )
				scale = scaleMax;
			SetScaleObj( GetScaleUnitOrg() * scale );
		}
		m_timerSec.Reset();
	}
// 
// 	if( pSkillDat->GetstrIdentifier() == _T( "photosynthesis" ) ) {
// 		static int si = 0;
// 		if( GetUnitType() == xUNIT_TREANT ) {
// 			if( m_cntPerSec < 50 )
// 				++m_cntPerSec;
// 			XBREAK( pBuffObj == nullptr );
// 			float scale = 1.f + (m_cntPerSec * 0.01f);
// 			const float scaleMax = 1.5f;
// 			if( scale > scaleMax )
// 				scale = scaleMax;
// 			SetScaleObj( GetScaleUnitOrg() * scale );
// 		}
// 	}

}
