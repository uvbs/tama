#include "stdafx.h"
#include "XUnitCyclops.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XLegionObj.h"
#include "XEObjMngWithType.h"
#include "XSquadObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XUnitCyclops::XUnitCyclops( XSquadObj *pSquadObj,
							ID idProp,
							BIT bitSide, 
							const XE::VEC3& vPos,
							float multipleAbility )
	: XUnitCommon( pSquadObj, idProp, bitSide, vPos, multipleAbility )
{
	Init();
}
/**
 @brief 
*/
void XUnitCyclops::ShootRangeAttack( UnitPtr& _spTarget,
									const XE::VEC3& vwSrc,
									const XE::VEC3& _vwDst,
									float damage,
									bool bCritical,
									const std::string& strType,
									const _tstring& strSpr )
{
	sShootLaser( GetThisUnit(), _spTarget, vwSrc, _vwDst, damage, bCritical, strSpr );
}

void XUnitCyclops::sShootLaser( UnitPtr spShooter,
																			UnitPtr& _spTarget,
																			const XE::VEC3& vwSrc,
																			const XE::VEC3& _vwDst,
																			float damage,
																			bool bCritical,
																			const _tstring& _strSpr )
{
	if( _spTarget == nullptr )
		return;
	_tstring strSpr = _strSpr;
	if( strSpr.empty() )
		strSpr = _T("laser.spr");
	bool bDual = false;
	// 사이클롭스 이중열선
	auto pBuff = spShooter->FindBuffSkill( _T( "dual_laser" ) );
	if( pBuff ) {
		// 확률적으로 레이저 두개가 나간다.
		float prob = pBuff->GetInvokeRatioByLevel();
		if( prob > 0 && XE::IsTakeChance( prob ) )
			bDual = true;

	}
	int lvInferno = 0;
//	LPCTSTR szSpr = _T("laser.spr");
	pBuff = spShooter->FindBuffSkill( _T("inferno"));
	if( pBuff ) {
		strSpr = _T("laser_inferno.spr");
		lvInferno = pBuff->GetLevel();
	}
	int numBeam = 1;
	if( bDual )
		numBeam = 2;

	UnitPtr spTarget = _spTarget;
	for( int i = 0; i < numBeam; ++i )	{
		if( i == 1 )	{
			// 현재상대하고 있는 타겟부대에서 새 타겟을 요청함.
			spTarget = spShooter->GetpSquadObj()->GetAttackTargetForUnit( spShooter );
			if( spTarget == nullptr )
				break;
		}
		// 레이저 생성
		// 레이저는 타겟의 바닥쪽을 향해 발사된다.
		XE::VEC3 vwDst = spTarget->GetvwPos();
		//
		auto pLaser = new XObjLaser( strSpr.c_str(),
																vwSrc,
																vwDst );
		if( lvInferno > 0 )
			pLaser->SetScaleObj( 1.f, 1.f * lvInferno );
		spShooter->GetpWndWorld()->AddObj( WorldObjPtr( pLaser ) );
		// 범위공격 대상 선정
		XArrayLinearN<XBaseUnit*, 512> ary;
		BIT bitSide = ~(spShooter->GetCamp());
		float radius = 3.f;
		{
			auto pBuff = spShooter->FindBuffSkill(_T("spread"));
			if( pBuff )
				radius += radius * pBuff->GetAbilMinbyLevel();
		}
		XEObjMngWithType::sGet()->GetListUnitRadius( &ary, 
													spTarget.get(),
													spTarget->GetvwPos().ToVec2(),
													xMETER_TO_PIXEL(radius), 
													bitSide, 
													10, 
													TRUE );
		//
		// 각각의 대상에게 데미지.
		XARRAYLINEARN_LOOP_AUTO( ary, pUnit ) {
			if( XASSERT( pUnit ) ) {
				BIT bitHit = XGAME::xBHT_HIT;
				BOOL bCritical = spShooter->IsCritical( pUnit->GetThisUnit() );
				if( bCritical ) {
					damage *= spShooter->GetCriticalPower();
					bitHit |= XGAME::xBHT_CRITICAL;
				}
				if( damage == 0 )
					bitHit &= ~XGAME::xBHT_HIT;
				float ratioPenet = spShooter->GetPenetrationRatio();
				pUnit->DoDamage( spShooter.get(), damage, ratioPenet, XSKILL::xDMG_RANGE, bitHit, XGAME::xDA_FIRE );
			}
		} END_LOOP;
		auto pBuff = spShooter->FindBuffSkill(_T("flame_fit"));
		if( pBuff )	{
			auto pEffect = pBuff->GetEffectIndex(0);
			if( pEffect )		{
				float abil = pBuff->GetAbilMinbyLevel();
				XBREAK( abil == 0 );
				float radius = pBuff->GetInvokeSizeByLevel();
				XBREAK( radius == 0 );
				float dmg = damage * abil;
				float sec = pEffect->secDurationInvoke;
	//  			sec = 99999.f;
				XBREAK( sec == 0 );
				auto pFlame = new XObjFlame( spShooter,
											spTarget->GetvwPos(),
											dmg,
											xMETER_TO_PIXEL(radius),
											sec,
											XECompCamp::sGetOtherSideFilter(spShooter->GetCamp().GetbitCamp()),
											_T("eff_flame.spr"), 2 );
				spShooter->AddObj( pFlame );
			}
		}
	}

}

