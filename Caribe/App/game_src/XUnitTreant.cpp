#include "stdafx.h"
#include "XUnitTreant.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XEObjMngWithType.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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
		pBuff = spShooter->FindBuffSkill( _T( "gaiant_rock" ) );
		if( pBuff ) {
			float ratio = pBuff->GetAbilMinbyLevel();
			pRock->SetSplash( 4.f, ratio );
			pRock->SetScaleObj( 2.f );
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
//	const _tstring strSpr = C2SZ(_strSpr);
	sShootRock( GetThisUnit(), spTarget, vwSrc, vwDst, damage, bCritical, strSpr );
// 	_tstring strSpr = _T("rock.spr");
// 	ID idAct = 1 + random(2);
// 	{
// 		auto pBuff = FindBuffSkill(_T("adaman_rock"));
// 		if( pBuff )
// 		{
// 			strSpr = _T("rock_adaman.spr");
// 			idAct = 1;
// 		}
// 	}
// 	// 바위생성
// 	XObjRock *pRock = new XObjRock( GetpWndWorld(), 
// 									GetThisUnit(),
// 									GetspTarget(),
// 									vwSrc, 
// 									vwDst,
// 									damage,
// 									bCritical,
// 									strSpr.c_str(), idAct );
// 	// 탄성특성
// 	{
// 		auto pBuff = FindBuffSkill(_T("elastic"));
// 		if( pBuff )
// 		{
// 			int cntBounce = 1;
// 			auto pBuff2 = FindBuffSkill(_T("strong_elastic"));
// 			if( pBuff2 )
// 				cntBounce = (int)pBuff2->GetAbilMinbyLevel();
// 			// 탄성으로 인해 튀는 바위의 데미지를 설정.
// 			float addDamageRatio = pBuff->GetAbilMinbyLevel();
// 			pRock->SetElastic( damage * addDamageRatio, cntBounce );
// 		}
// 		// 거대한바위 특성
// 		pBuff = FindBuffSkill(_T("gaiant_rock"));
// 		if( pBuff )
// 		{
// 			float ratio = pBuff->GetAbilMinbyLevel();
// 			pRock->SetSplash( 4.f, ratio );
// 			pRock->SetScaleObj( 2.f );
// 		}
// 	}
// 	pRock->SetpDelegate( this );
// 	GetpWndWorld()->AddObj( WorldObjPtr( pRock ) );
}

void XUnitTreant::OnArriveBullet( XObjBullet *pBullet,
								const UnitPtr& spAttacker,
								const UnitPtr& spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam )
{
	int numApply = 1;
	float meterRadius = 0;
	//
	XBaseUnit::OnArriveBullet( pBullet, spAttacker, spTarget, vwDst, damage, bCritical, sprArrive, idActArrive, dwParam );
	if( spTarget != nullptr )
	{
	} else
	{
		// 넌타겟팅으로 날아옴.(탄성바위)
		numApply = 1;		// 범위내 타겟 한명에게만 데미지.
		meterRadius = 3.f;
	}
	// 광역 속성인가.
	if( pBullet->GetmeterRadius() > 0 )
	{
		numApply = 5;
		meterRadius = pBullet->GetmeterRadius();
	}
	if( meterRadius > 0 )
	{
		// 범위공격 대상 선정
		XE::VEC2 vCenter;
		if( spTarget != nullptr )
			vCenter = spTarget->GetvwPos().ToVec2();
		else
			vCenter = vwDst.ToVec2();
		XArrayLinearN<XBaseUnit*, 512> ary;
		BIT bitSide = ~GetCamp();
		// 타겟이 있을땐 위에서 이미 데미지를 한번 먹였기때문에 스플래시에서 다시 먹일필요 없댜ㅏ.
		BOOL bIncludeCenter = (spTarget != nullptr)? FALSE : TRUE;
		XEObjMngWithType::sGet()->GetListUnitRadius( &ary, 
													(spTarget!=nullptr)? spTarget.get() : nullptr,
													vCenter,
													xMETER_TO_PIXEL(meterRadius), 
													bitSide, 
													numApply, 
													bIncludeCenter );
		//
		// 각각의 대상에게 데미지.
		XARRAYLINEARN_LOOP_AUTO( ary, pUnit ) {
			if( XASSERT( pUnit ) ) {
				float ratio = 1.f;
				// 중심타겟이 아닌경우는 스플래시 데미지율로.
				if( spTarget && pUnit->GetsnObj() != spTarget->GetsnObj() ) {
					ratio = pBullet->GetratioDamageSplash();
					XBREAK( ratio == 0.f );
				}
				float ratioPenet = GetPenetrationRatio();
				BIT bitHit = XGAME::xBHT_HIT;
				if( bCritical )
					bitHit |= XGAME::xBHT_CRITICAL;
				if( damage == 0 )
					bitHit &= ~XGAME::xBHT_HIT;
				pUnit->DoDamage( this, damage * ratio, ratioPenet, XSKILL::xDMG_RANGE, bitHit, XGAME::xDA_NONE );
			}
		} END_LOOP;
	}
}

