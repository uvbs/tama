#include "stdafx.h"
#include "XUnitArcher.h"
#include "XObjEtc.h"
#include "XWndBattleField.h"
#include "XBattleField.h"
#include "XLegionObj.h"
#include "XSoundMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XSKILL;
#ifdef _XMEM_POOL
template<> XPool<XUnitArcher>* XMemPool<XUnitArcher>::s_pPool = NULL;
#endif
////////////////////////////////////////////////////////////////
XUnitArcher::XUnitArcher( XSPSquad spSquadObj,
						ID idProp,
						BIT bitSide, 
						const XE::VEC3& vPos,
						float multipleAbility )
: XUnitCommon( spSquadObj, idProp, bitSide, vPos, multipleAbility )
{
	Init();

}

void XUnitArcher::Destroy()
{
}

/**
 @brief 발사체를 생성하고 날린다.
*/
// void XUnitArcher::sShootArrow( UnitPtr spTarget,
// 															 const XE::VEC3& vwSrc,
// 															 const XE::VEC3& vwDst,
// 															 float damage,
// 															 bool bCritical )
// {
// 	_tstring strSpr = _T( "arrow.spr" );
// 	bool bFrozenArrow = false;
// 	// 냉기폭발화살이 발동되었는지 확인.
// 	XARRAYLINEARN_LOOP_AUTO( m_aryInvokeSkillByAttack, &eff ) {
// 		if( eff.pDat->GetstrIdentifier() == _T( "chill_blast_arrow" ) ) {
// 			bFrozenArrow = true;
// 			break;
// 		}
// 	} END_LOOP;
// 	if( bFrozenArrow ) {
// 		strSpr = _T( "arrow2.spr" );		// 냉기화살
// 	}
// 	// 메인 화살
// 	auto pArrow = new XObjArrow( GetpWndWorld(),
// 															 GetThisUnit(),
// 															 spTarget,
// 															 vwSrc,
// 															 vwDst,
// 															 damage,
// 															 bCritical,
// 															 strSpr.c_str(), 1, 11.f );
// 	//										strSpr.c_str(), 1, 15.f );
// 	if( XASSERT( pArrow ) ) {
// 		XARRAYLINEARN_LOOP_AUTO( m_aryInvokeSkillByAttack, &eff ) {
// 			_tstring strInvokeSkill;
// 			if( bFrozenArrow ) {
// 				// 빙결화살 특성이 있으면 빙결화살의 발동스킬을 화살에 넣음.
// 				auto pBuff = FindBuffSkill( _T( "freeze_arrow" ) );
// 				if( pBuff )
// 					strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
// 				else
// 					strInvokeSkill = eff.pEffect->strInvokeSkill;
// 				pArrow->AddInvokeSkill( strInvokeSkill );
// 			}
// 			{
// 				auto pBuff = FindBuffSkill( _T( "instant_death_arrow" ) );
// 				if( pBuff )
// 					strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
// 				else
// 					strInvokeSkill = eff.pEffect->strInvokeSkill;
// 				pArrow->AddInvokeSkill( strInvokeSkill );
// 			}
// 		} END_LOOP;
// 		pArrow->SetpDelegate( this );
// 		GetpWndWorld()->AddObj( WorldObjPtr( pArrow ) );
// 	}
// 	// 추가화살
// 	// 애기살 특성이 있는가.
// 	XBuffObj *pBuff = FindBuffSkill( _T( "pyeonjeon" ) );
// 	if( pBuff ) {
// 		float ratioInvoke = pBuff->GetInvokeRatioByLevel();
// 		// 확률에 걸렸는가.
// 		if( XE::IsTakeChance( ratioInvoke ) ) {
// 			// 애기살은 무조건 크리.
// 			float damageLocal = damage * GetCriticalPower();
// 			auto pSubArrow = new XObjArrow( GetpWndWorld(),
// 																			GetThisUnit(),
// 																			spTarget,
// 																			vwSrc,
// 																			vwDst,
// 																			damageLocal,
// 																			true,
// 																			_T( "arrow.spr" ), 2, 11.f );
// 			if( XASSERT( pSubArrow ) ) {
// 				pSubArrow->SetpDelegate( this );
// 				GetpWndWorld()->AddObj( WorldObjPtr( pSubArrow ) );
// 			}
// 		}
// 	}
// 	if( IsRange() ) {
// 		// 조준사격버프가 있을때
// 		pBuff = FindBuffSkill( _T( "aim_shoot" ) );
// 		if( pBuff ) {
// 			float ratioInvoke = pBuff->GetAbilMinbyLevel();
// 			if( XE::IsTakeChance( ratioInvoke ) ) {
// 				float damageLocal = damage * GetCriticalPower();
// 				auto pSubArrow = new XObjArrow( GetpWndWorld(),
// 																				GetThisUnit(),
// 																				spTarget,
// 																				vwSrc,
// 																				vwDst,
// 																				damageLocal,
// 																				true,
// 																				_T( "arrow_critical.spr" ), 1, 13.f );
// 				// 																				_T("arrow.spr"), 2, 13.f );
// 				if( XASSERT( pSubArrow ) ) {
// 					pSubArrow->SetpDelegate( this );
// 					GetpWndWorld()->AddObj( WorldObjPtr( pSubArrow ) );
// 				}
// 			}
// 		}
// 	}
// }

/**
 @brief 타점에서 호출되며 원거리 발사체를 만들어 날린다.
*/
void XUnitArcher::ShootRangeAttack( UnitPtr& spTarget,
																		const XE::VEC3& vwSrc,
																		const XE::VEC3& vwDst,
																		float damage,
																		bool bCritical,
																		const std::string& strType,
																		const _tstring& strSpr )
{
	SOUNDMNG->OpenPlaySoundBySec( 31, xRandomF(0.5f, 1.5f) );
	ShootArrow( spTarget, vwSrc, vwDst, damage, bCritical );
}

// XSkillUser*
// XUnitArcher::CreateAndAddToWorldShootObj( XSkillDat *pDat,
// 																				int level,
// 																				XSkillReceiver *pBaseTarget,
// 																				XSkillUser *pCaster,
// 																				const XE::VEC2& vPos )
// {
// 	auto pTargetUnit = dynamic_cast<XBaseUnit*>(pBaseTarget);
// 	if( !pTargetUnit && vPos.IsZero() )
// 		return this;
// 	//
// 	//	TransformByObj( &m_vlActionEvent );
// 	const XE::VEC3 vwSrc = GetvwPos() + XBaseUnit::GetvlActionEvent();
// 	const auto vwDst = pTargetUnit->GetvCenterWorld();
// 	//
// 	_tstring strSpr = _T( "arrow.spr" );
// 	//
// 	if( pDat->GetIds() == _T("chill_explosion") ) {
// 		strSpr = pDat->GetstrShootObj(); // _T( "arrow2.spr" );		// 냉기화살
// 		// 메인 화살
// 		auto pArrow = new XObjArrow( GetpWndWorld(),
// 																 GetThisUnit(),
// 																 pTargetUnit->GetThisUnit(),
// 																 vwSrc,
// 																 vwDst,
// 																 0,		// damage
// 																 false,	// critical
// 																 strSpr.c_str(), 1, 11.f );
// 		if( XASSERT( pArrow ) ) {
// 			auto strInvokeSkill = eff.pEffect->strInvokeSkill;	// 디폴트 발동스킬
// 			// 냉기폭발화살을 대체하는 발동스킬을 줄 스킬이 있다면 교체해서 받음.
// 			//strInvokeSkill = 대체발동스킬ByBuffs( pDat );
// 			{
// 				// 빙결화살 특성이 있으면 빙결화살의 발동스킬을 화살에 넣음.																													
// 				auto pBuff = FindBuffSkill( _T( "freeze_arrow" ) );
// 				if( pBuff ) {
// 					strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
// 				}
// 			}{
// 				// 즉사화살 특성이 있으면 즉사화살의 발동스킬을 화살에 넣음.																													
// 				auto pBuff = FindBuffSkill( _T( "instant_death_arrow" ) );
// 				if( pBuff ) {
// 					strInvokeSkill = pBuff->GetEffectIndex( 0 )->strInvokeSkill;
// 				}
// 			}
// 			pArrow->AddInvokeSkill( pDat-> );
// 			pArrow->SetpDelegate( this );
// 			GetpWndWorld()->AddObj( WorldObjPtr( pArrow ) );
// 		}
// 	} else
// 	if( pDat->GetIds() == _T( "something" ) ) {
// 
// 	} else {
// 		// 표준 스킬발사체 객체생성
// 		return XBaseUnit::CreateAndAddToWorldShootObj( pDat, 
// 																									 level, 
// 																									 pBaseTarget, 
// 																									 pCaster, 
// 																									 vPos );
// 	}
// 	
// 	return this;
// }
