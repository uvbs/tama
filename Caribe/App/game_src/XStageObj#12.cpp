#include "stdafx.h"
#include "XStageObj.h"
#include "XLegion.h"
#include "XCampObj.h"
#include "XAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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
	// DB저장용은 군단정보를 저장하지 않는다.(일단 개발의 편의성을 위해 모든스테이지의 군단정보를 다 저장하는걸로, 최적화시 용량을 줄인다.)
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
// 	for( auto& elem : m_aryDrops ) {
// 		ar << elem.first;
// 		ar << elem.second;
// 	}
	MAKE_CHECKSUM( ar );
	return 1;
}
int XStageObj::DeSerialize( const CampObjPtr spCampObj,
//							XPropCamp::xProp* pPropCamp, 
							int idxStage, XArchive& ar, int verCamp ) 
{
	XLOCK_OBJ;
	char c0;
//	int idxStageSaved;
	int sizeDrops;
	auto pPropCamp = spCampObj->GetpProp();
	if( verCamp >= 11 ) {
		ID idProp;
		ar >> idProp;
		// 만약 idProp스테이지가 사라졌다면 null이 된다.
		_m_spPropStage = pPropCamp->GetspStageByidStage( idProp );
		XBREAK( _m_spPropStage == nullptr );
	} else {
 		_m_spPropStage = pPropCamp->GetpStageByidxStage( idxStage );
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
// 	if( pPropCamp ) {
///<	이거 필요없지 않나? 있든없든 일단 로딩하고 상위에서 날리든지 결정하면 될문제.
// 		_m_spPropStage = pPropCamp->GetpStage( idxStageSaved );
// 		// 만약 스테이지가 없어졌을때 죽지않기 위해 일단 Deserial을 모두 하고 삭제시킨다.
// 	}
	if( _m_spPropStage == nullptr ) {
		m_spLegion.reset();
		m_Power = 0;
	}
	if( m_spLegion && m_Power == 0 )
		m_Power = XLegion::sGetMilitaryPower( m_spLegion );
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

bool XStageObj::CreateLegion( CampObjPtr spCampObj, int lvBase )
{
	XLOCK_OBJ;
	m_Power = 0;	// 군단을 새로 만들면 반드시 갱신하도록.
	m_spLegion = CreateLegion2( spCampObj, lvBase, &m_LevelLegion );
	m_numSquad = m_spLegion->GetNumSquadrons();
	if( m_spLegion )
		m_Power = XLegion::sGetMilitaryPower( m_spLegion );
	return m_spLegion != nullptr;
}
/**
 @brief 스테이지의 군단을 생성한다. 
 @param pOutLvLegion 생성된 군단의 레벨을 담는다.
*/
LegionPtr XStageObj::CreateLegion2( CampObjPtr spCampObj, int lvBase, int *pOutLvLegion ) const
{
	XLegion *pLegion = new XLegion;
	auto spLegion = LegionPtr( pLegion );
	//
	do {
		auto pPropStage = _m_spPropStage.get();
		if( XBREAK( pPropStage == nullptr ) ) {
			break;
		}
		pLegion->SetgradeLegion( pPropStage->legion.gradeLegion );
		if( pPropStage->legion.gradeLegion == XGAME::xGL_ELITE ) {
			pLegion->SetRateAtk( RATE_ATK_DEFAULT_ELITE );
			pLegion->SetRateHp( RATE_HP_DEFAULT_ELITE );
		} else
		if( pPropStage->legion.gradeLegion == XGAME::xGL_RAID ) {
			pLegion->SetRateAtk( RATE_ATK_DEFAULT_RAID );
			pLegion->SetRateHp( RATE_HP_DEFAULT_RAID );
		}
//		m_numSquad = pPropStage->numSquad;
		int numSquad = pPropStage->legion.numSquad;
		int lvLegion = pPropStage->legion.lvLegion;
		if( lvLegion == 0 ) {
			// 프롭스테이지에 군단레벨이 지정되어있지않다면 기준레벨을 중심으로 adjLevelLgion으로 보정해서 쓴다.
			if( XBREAK( pPropStage->legion.adjLvLegion == 0x7f ) ) {	// 군단레벨이 지정되어있지않은데 보정레벨도 없으면 에러
				break;
			}
			XBREAK( lvBase == 0 );
			lvLegion = lvBase + pPropStage->legion.adjLvLegion;
		}
//		m_LevelLegion = lvLegion;
	//	XBREAK( spPropStage->levelLegion == 0 );
		if( numSquad == 0 )
			numSquad = XLegion::sGetNumSquadByUserLevel( lvLegion );
		// 이 부대가 랜덤선택가능한 유닛목록을 미리 뽑는다.
		XArrayLinearN<XGAME::xtUnit, XGAME::xUNIT_MAX> aryUnits;
		XLegion::sGetAvailableUnitByLevel( lvLegion, &aryUnits );
		XGAME::xtUnit unitDefault = XGAME::xUNIT_NONE;
		if( pPropStage->legion.squadDefault.unit )
			unitDefault = pPropStage->legion.squadDefault.unit;	// 디폴트 유닛
		//
		for( int i = 0; i < numSquad; ++i ) {
// 			auto pSquadProp = pPropStage->legion.GetManualSquad( i );
			// i슬롯 부대가 메뉴얼 지정이라면 메뉴얼 부대 프로퍼티를 꺼냄
			auto pSquadProp = pPropStage->legion.GetPropSquadWhenManual( i );
			// 메뉴얼 지정된 부대가 없으면 디폴트 부대로 한다.
			if( pSquadProp == nullptr )
				pSquadProp = &pPropStage->legion.squadDefault;
			XBREAK( pSquadProp == nullptr );
			XSquadron *pSquad = nullptr;
			// i번째 분대가 수동으로 지정되어야 하는가?
	//		auto unit = unitDefault;
			auto unit = pSquadProp->unit;
			// 외부파라메터로 유닛이 명시되어있다면 그것이 우선된다.
			if( m_paramLegion.unit ) {
				unit = m_paramLegion.unit;
			} else
			// 특정유닛으로 지정되어 있지 않다면 랜덤으로 뽑음.
			if( !unit )	// 유닛이 따로 지정되어있지 않다면 랜덤으로.
				unit = aryUnits.GetFromRandom();
#if defined(_XSINGLE) || !defined(_CLIENT)
			pSquad = XLegion::sCreateSquadronForNPC( lvLegion,
													0,
													unit,
													pSquadProp->idHero,
													pSquadProp->lvHero,
													pSquadProp->lvSkill,
													pSquadProp->lvSquad,
													pPropStage->legion.gradeLegion,
													&m_paramLegion );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
			if( XASSERT( pSquad ) )
				pLegion->SetSquadron( i, pSquad, TRUE );
		}
		if( pOutLvLegion )
			*pOutLvLegion = lvLegion;
	} while( 0 );
	return spLegion;
}

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
		// 최초 언락된 스테이지에선 아이템 드랍안함.
		int idxStage = GetidxStage();
		// 스테이지에서 드랍할수 있는 아이템을 스팟에 등록한다.
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

void XStageObj::Update( CampObjPtr spCampObj )
{
	// 전투력만 얻어둔다. 이미 부대가 있으면 있는부대의 전투력으로계산한다.
	if( m_Power == 0 ) { // 스테이지군단은 생성된이후에 파라메터가 안변함을 보장해야함.
		if( m_spLegion ) {
			m_Power = XLegion::sGetMilitaryPower( m_spLegion );
		} else {
			auto spLegion = CreateLegion2( spCampObj, 0, nullptr );
			int power = XLegion::sGetMilitaryPower( spLegion );
			m_Power = power;
		}
	}

}

// bool XStageObj::IsClear()
// {
// 	XLOCK_OBJ;
// 	int maxTry = GetspPropStage()->maxTry;
// 	if( maxTry > 0 ) {
// 		// 최대 클리어횟수가 있을때는 그것을 다 채우면 클리어
// 		return maxTry > 0 && m_numClear >= maxTry;
// 	}
// 	if( GetspPropStage()->nu)
// 	if( maxTry == 0 )
// 		return false;	// 이제 maxtry 0은 무한대로 도전
// }


XE_NAMESPACE_END;   // namespace xCampiagn

