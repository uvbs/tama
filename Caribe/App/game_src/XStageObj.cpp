#include "stdafx.h"
#include "XLegionH.h"
#include "XStageObj.h"
#include "XLegion.h"
#include "XCampObj.h"
#include "XAccount.h"
#include "XGlobalConst.h"
#include "XPropLegionH.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XE_NAMESPACE_START( xCampaign )
//////////////////////////////////////////////////////////////////////////
int xTryer::Serialize( XArchive& ar )
{
	ar << strName;
	ar << idAcc;
	XBREAK( level > 0xff );
	ar << (BYTE)level;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << power;
	return 1;
}
int xTryer::DeSerialize( XArchive& ar, int ver )
{
	XBREAK( ver == 0 );
	ar >> strName;
	ar >> idAcc;
	BYTE b0;
	ar >> b0;	level = b0;
	ar >> b0 >> b0 >> b0;
	ar >> power;
	return 1;
}
//////////////////////////////////////////////////////////////////////////
int XStageObj::sSerialize( XArchive& ar, StageObjPtr spStageObj )
{
	ar << VER_CAMP_SERIALIZE;
	return spStageObj->Serialize( ar );
}
int XStageObj::sDeSerialize( XArchive& ar, CampObjPtr spCamp, StageObjPtr spStageObj )
{
	int ver;
	ar >> ver;
	if( ver == 0 )
		return 0;
	int idxStage = spStageObj->GetidxStage();
	return spStageObj->DeSerialize( spCamp, idxStage, ar, ver );
}

////////////////////////////////////////////////////////////////
XStageObj::XStageObj( StagePtr& spPropStage )
{
//	XLOCK_OBJ;
	Init();
	// spPropStage은 널이올수 있음.
	_m_spPropStage = spPropStage;
}

void XStageObj::Destroy()
{
}

int XStageObj::Serialize( XArchive& ar )
{
	XLOCK_OBJ;
	XBREAK( GetspPropStage()->idxStage > 0xff );
	XBREAK( m_numClear > 0xff );
	ar << GetspPropStage()->idProp;
	ar << (char)0; //GetspPropStage()->idxStage;
	ar << (char)m_numClear;
	XBREAK( m_LevelLegion > 0x7f );
	ar << (char)m_LevelLegion;
	ar << (char)m_aryDrops.size();
	ar << (char)m_numStar;
	ar << xboolToByte( m_bRecvReward );
	ar << (char)0;
	ar << (char)0;
	ar << m_listTryer;
	// DB저장용은 군단정보를 저장하지 않는다.
	if( ar.IsForDB() ) {
		ar << 0;
	} else {
		if( m_spLegion != nullptr ) {
			m_spLegion->SerializeFull( ar );
			ar << m_Power;
		} else
			ar << 0;
	}
	ar << m_aryDrops;
	MAKE_CHECKSUM( ar );
	return 1;
}
int XStageObj::DeSerialize( const CampObjPtr spCampObj,
							int idxStage, XArchive& ar, int verCamp ) 
{
	XLOCK_OBJ;
	char c0;
	int sizeDrops;
	auto pPropCamp = spCampObj->GetpProp();
	if( verCamp >= 11 ) {
		ID idProp;
		ar >> idProp;
		// 만약 idProp스테이지가 사라졌다면 null이 된다.
		_m_spPropStage = pPropCamp->GetspStageByidStage( idProp );
		XBREAK( _m_spPropStage == nullptr );
	} else {
		_m_spPropStage = pPropCamp->GetspStageByidxStage( idxStage );
		XBREAK( _m_spPropStage == nullptr );
	}

	ar >> c0;	// idxStageSaved = c0;
	ar >> c0;	m_numClear = c0;
	ar >> c0;	m_LevelLegion = c0;
	ar >> c0;	sizeDrops = c0;
	ar >> c0;	m_numStar = c0;
	ar >> c0;	m_bRecvReward = xbyteToBool( (BYTE)c0 );
	ar >> c0 >> c0;
	ar.SetverArchiveInstant( verCamp );
	ar >> m_listTryer;
	m_Power = 0;
	m_spLegion = LegionPtr(XLegion::sCreateDeserializeFull( ar ));
	if( m_spLegion )
		ar >> m_Power;
	m_aryDrops.clear();
	ar >> m_aryDrops;
	if( _m_spPropStage == nullptr ) {
		m_spLegion.reset();
		m_Power = 0;
	}
	if( m_spLegion && m_Power == 0 )
		m_Power = XLegion::sGetMilitaryPower( m_spLegion );
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

bool XStageObj::CreateLegion( CampObjPtr spCampObj, int lvBase, int idxFloor )
{
	XLOCK_OBJ;
	m_Power = 0;	// 군단을 새로 만들면 반드시 갱신하도록.
#if defined(_XSINGLE) || !defined(_CLIENT)
	m_spLegion = CreateLegion2( spCampObj, lvBase, &m_LevelLegion, idxFloor );
#endif
	if( m_spLegion ) {
		m_numSquad = m_spLegion->GetNumSquadrons();
		m_Power = XLegion::sGetMilitaryPower( m_spLegion );
	}
	return m_spLegion != nullptr;
}

/**
 @brief 스테이지의 군단을 생성한다. 
 @param pOutLvLegion 생성된 군단의 레벨을 담는다.
*/
#if defined(_XSINGLE) || !defined(_CLIENT)
LegionPtr XStageObj::CreateLegion2( CampObjPtr spCampObj, int lvBase, int *pOutLvLegion, int idxFloor ) const
{
	// XLegion::sCreateLegionForNPC2로 통합하는게 맞지만 지금은 너무 많이 꼬여있어서 당장통합하기가 어려움. 일단 구현해놓고 작은부분부터 통합하는게 나을듯.
	XLegion *pLegion = new XLegion;
	auto spLegion = LegionPtr( pLegion );
	//
	do {
		auto pPropStage = _m_spPropStage.get();
		if( XBREAK( pPropStage == nullptr ) ) {
			break;
		}
		pLegion->SetgradeLegion( pPropStage->m_spxLegion->gradeLegion );
		if( pPropStage->m_spxLegion->gradeLegion == XGAME::xGL_ELITE ) {
			pLegion->SetRateAtk( RATE_ATK_DEFAULT_ELITE );
			pLegion->SetRateHp( RATE_HP_DEFAULT_ELITE );
		} else
		if( pPropStage->m_spxLegion->gradeLegion == XGAME::xGL_RAID ) {
			pLegion->SetRateAtk( RATE_ATK_DEFAULT_RAID );
			pLegion->SetRateHp( RATE_HP_DEFAULT_RAID );
		}
		int numSquad = pPropStage->m_spxLegion->numSquad;
		int lvLegion = spCampObj->GetlvLegion( pPropStage, lvBase, idxFloor );
//		m_LevelLegion = lvLegion;		// 외부파라메터로 받도록 수정됨.
	//	XBREAK( spPropStage->levelLegion == 0 );
		if( numSquad == 0 ) {
			const auto& tblLegion = XGC->GetLegionTable( lvLegion );
			numSquad = tblLegion.m_numSquad;
		}
		// 
		// 메뉴얼지정된 squad부터 생성하고 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
		//////////////////////////////////////////////////////////////////////////
		const int idx[XGAME::MAX_SQUAD] = {2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14};
		// 메뉴얼 지정된 부대부터 생성
		int i = 0;
		for( auto squadMutable : pPropStage->m_spxLegion->arySquads ) {
			XGAME::xSquad sqParam;
			sqParam.idxPos = ( squadMutable.idxPos >= 0 ) ? squadMutable.idxPos : idx[i];
			if( m_paramLegion.unit ) {
				squadMutable.unit = m_paramLegion.unit;
				squadMutable.idHero = 0;
			}
			SetSquadParam( spCampObj, sqParam.idxPos, lvLegion, idxFloor, squadMutable, &sqParam );
			XBREAK( sqParam.idHero == 0 );
			auto pPropHero = PROP_HERO->GetpProp( sqParam.idHero );
// 			if( squad.idHero == 0 )
// 				squad.idHero = spCampObj->GetidHeroByCreateSquad( sqParam.idxPos, &squad, pPropStage );
// 			auto pPropHero = XLegion::sGetpPropHeroByInfo( squad, lvLegion );
// 			sqParam.unit = XLegion::sGetUnitByInfo( squad, pPropHero, lvLegion );
// 			sqParam.lvHero = XLegion::sGetLvHeroByInfo( squad, lvLegion );
// 			sqParam.lvSkill = XLegion::sGetLvSkillByInfo( squad, lvLegion );
// 			sqParam.lvSquad = XLegion::sGetLvSquadByInfo( squad, lvLegion );
// 			sqParam.grade = XLegion::sGetGradeHeroByInfo( squad, lvLegion );
// 			sqParam.mulAtk = squad.mulAtk;
// 			sqParam.mulHp = squad.mulHp;

			auto pSquad = XLegion::sCreateSquadronForNPC2( lvLegion
																									, pPropHero
																									, sqParam );
			if( XASSERT( pSquad ) ) {
				//pLegion->AddSquadron( sqParam.idxPos, pSquad, true );
				pLegion->AddSquadron( sqParam.idxPos, pSquad, true );
				// 리더 영웅 지정
				XLegion::sSetLeaderByInfo( pPropStage->m_spxLegion.get(), spLegion, pSquad );
			}
			++i;
		}  // for legion.arySquads
		// 생성해야할 부대수가 부족하다면 부족한 squad수만큼은 남은 슬롯들중에서 랜덤으로 채워넣는다.
		if( pLegion->GetNumSquadrons() < numSquad ) {
			// 빈 슬롯들의 인덱스를 추려낸다.
			XList4<int> listSlotNum;
			if( pPropStage->m_spxLegion->arySquads.size() != 0 ) {
				for( int i = 0; i < pLegion->GetMaxSquadSlot(); ++i ) {
					if( pLegion->GetpSquadronByidxPos( i ) == nullptr )
						listSlotNum.Add( i );
				}
			}
			// 모자르는 부대수만큼 추가생성
			const int numSquadExt = numSquad - pLegion->GetNumSquadrons();
			const auto& squad = pPropStage->m_spxLegion->squadDefault;
			XGAME::xSquad sqParam;
			//
			for( int i = 0; i < numSquadExt; ++i ) {
				int idxSlot;
				if( pPropStage->m_spxLegion->arySquads.size() == 0 ) {
					// xml로 메뉴얼 지정이 없었을땐 슬롯 순서대로 부대를 생성.
					idxSlot = idx[i];
				} else {
					auto itor = listSlotNum.GetItorFromRandom();
					if( XASSERT( itor != listSlotNum.end() ) ) {
						idxSlot = ( *itor );
						listSlotNum.erase( itor );	// 한번 사용한 인덱스슬롯 번호는 다시 사용하지 않게.
					}
				}
				SetSquadParam( spCampObj, sqParam.idxPos, lvLegion, idxFloor, squad, &sqParam );
				XBREAK( sqParam.idHero == 0 );
				auto pPropHero = PROP_HERO->GetpProp( sqParam.idHero );
				auto pSquad = XLegion::sCreateSquadronForNPC2( lvLegion
																										, pPropHero
																										, sqParam );
				if( XASSERT( pSquad ) ) {
//					pLegion->AddSquadron( idxSlot, pSquad, true );
					pLegion->AddSquadron( idxSlot, pSquad, true );
					// 리더 영웅 지정
					XLegion::sSetLeaderByInfo( pPropStage->m_spxLegion.get(), spLegion, pSquad );
				}
			}
		} // if( pLegion->GetNumSquadrons() < numSquad ) {
		// 리더가 지정되지 않았을경우 자동으로 리더를 정함.
		if( pLegion->GetpLeader() == nullptr )
			pLegion->SetAutoLeader();
		if( pPropStage->m_spxLegion->arySquads.size() == 0 ) {
			pLegion->AdjustLegion();
		}

		if( pOutLvLegion )
			*pOutLvLegion = lvLegion;
	} while( 0 );
	return spLegion;
}

void XStageObj::SetSquadParam( XSPCampObj spCampObj
														, int idxPos
														, int lvLegion
														, int idxFloor
														, const XGAME::xSquad& propSquad
														, XGAME::xSquad* pOut ) const
{
//	auto propSquad = _propSquad;		// mutable copy본
	auto pPropStage = _m_spPropStage.get();
	auto unit = propSquad.unit;
	ID idHero = propSquad.idHero;
	if( idHero == 0 )
		idHero = spCampObj->GetidHeroByCreateSquad( idxPos, &propSquad, pPropStage );
	auto pPropHero = XLegion::sGetpPropHeroByInfo( idHero, propSquad.atkType, unit, lvLegion );
	XBREAK( pPropHero == nullptr );
	pOut->idHero = pPropHero->idProp;
	if( !unit )
		unit = spCampObj->GetUnitWhenCreateSquad( idxPos, pPropHero->typeAtk, &propSquad, pPropStage, idxFloor );
	pOut->unit = XLegion::sGetUnitByInfo( unit, propSquad.sizeUnit, pPropHero->typeAtk, lvLegion );
	pOut->lvHero = XLegion::sGetLvHeroByInfo( propSquad.lvHero, lvLegion );
	pOut->lvSkill = XLegion::sGetLvSkillByInfo( propSquad.lvSkill, lvLegion );
	pOut->lvSquad = XLegion::sGetLvSquadByInfo( propSquad.lvSquad, lvLegion );
	pOut->grade = XLegion::sGetGradeHeroByInfo( propSquad.grade, lvLegion );
	pOut->mulAtk = propSquad.mulAtk;
	pOut->mulHp = propSquad.mulHp;
}

#endif // #if defined(_XSINGLE) || !defined(_CLIENT)


void XStageObj::DestroyLegion()
{
	XLOCK_OBJ;
	m_spLegion.reset();
	m_Power = 0;
}

void XStageObj::AddItems( ID idItem, int num )
{
	XGAME::xDropItem item( idItem, num );
	m_aryDrops.push_back( item );
}
void XStageObj::AddItems( LPCTSTR szIds, int num )
{
	auto pPropItem = PROP_ITEM->GetpProp( szIds );
	if( XASSERT( pPropItem ) )
		AddItems( pPropItem->idProp, num );
}
void XStageObj::AddItems( XPropItem::xPROP* pProp, int num )
{
	AddItems( pProp->idProp, num );
}
void XStageObj::AddItems( ID idItem, int num, float dropChance )
{
	XGAME::xDropItem item( idItem, num, dropChance );
	m_aryDrops.push_back( item );
}

/**
 @brief 프로퍼티에 있는 드랍품목을 스테이지동적객체에 카피한다.
*/
bool XStageObj::SetDropItemsFromProp( const CampObjPtr spCampObj )
{
	bool bUpdate = false;
	if( XASSERT(GetspPropStage()) ) {
		if( m_aryDrops.size() == 0 ) {
			bUpdate = true;
			for( auto& reward : GetspPropStage()->aryReward ) {
				XBREAK( reward.rateDrop == 0 );
				AddItems( reward.GetidItem(), reward.num, reward.rateDrop );
			}
		}
	}
	return bUpdate;
}

void XStageObj::Update( CampObjPtr spCampObj, int idxFloor )
{
	// 전투력만 얻어둔다. 이미 부대가 있으면 있는부대의 전투력으로계산한다.
	if( m_Power == 0 ) { // 스테이지군단은 생성된이후에 파라메터가 안변함을 보장해야함.
		if( m_spLegion ) {
			m_Power = XLegion::sGetMilitaryPower( m_spLegion );
		} else {
#if defined(_XSINGLE) || !defined(_CLIENT)
			auto spLegion = CreateLegion2( spCampObj, 0, &m_LevelLegion, idxFloor );
// 			auto spLegion = CreateLegion2( spCampObj, 0, nullptr );
			int power = XLegion::sGetMilitaryPower( spLegion );
			m_Power = power;
#endif
		}
	}

}

bool XStageObj::IsAbleTry() const 
{
	int maxTry = GetspPropStage()->maxTry;
	if( maxTry == 0 )
		return true;	// 이제 maxtry 0은 무한대로 도전
	return maxTry > 0 && m_numTry < maxTry;
}

// 완전히 클리어했는가.
bool XStageObj::IsAllNumClear() const
{
	auto spPropStage = GetspPropStage();
	const int maxWin = spPropStage->maxWin;
	if( maxWin == 0 )
		return false;	// 승리횟수에 제한이 없음(ap가 허용하는한 계속 도전가능) 
	// 승리횟수에 제한이 있을때.
	return maxWin > 0 && m_numClear >= maxWin;
}


XE_NAMESPACE_END;   // namespace xCampiagn

