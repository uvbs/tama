#include "stdafx.h"
#include "XStageObjHero.h"
#include "XStruct.h"
#include "XLegion.h"
#include "XCampObjHero.h"
#include "XPropLegionH.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define LV_LIMIT_UNDER	5

XE_NAMESPACE_START( xCampaign )

void XStageObjHero::sSetLevelLegion( StagePtr spPropStage, int lvLegion )
{
	spPropStage->m_spxLegion->lvLegion = lvLegion;
	spPropStage->levelLimit = lvLegion - LV_LIMIT_UNDER;
	if( spPropStage->levelLimit > XGAME::GetLevelMaxAccount() )
		spPropStage->levelLimit = XGAME::GetLevelMaxAccount();
}
//////////////////////////////////////////////////////////////////////////
XStageObjHero::XStageObjHero( StagePtr spPropStage )
: XStageObj( spPropStage )
{
	Init();
}

int XStageObjHero::Serialize( XArchive& ar )
{
	XLOCK_OBJ;
	auto spPropStage = GetspPropStage();
	if( XASSERT(spPropStage) ) {
		XBREAK( spPropStage->idProp == 0 );
		ar << spPropStage->idProp;	// cycle과 idHero정보가 있음.
		XBREAK( m_idHero == 0 );
		ar << m_idHero;
		XBREAK( spPropStage->m_spxLegion->lvLegion == 0 );
		XBREAK( spPropStage->m_spxLegion->lvLegion > 0xff );
//		spPropStage->Serialize( ar );
		ar << (BYTE)spPropStage->m_spxLegion->lvLegion;
		ar << (BYTE)GetnumStar();
		ar << (BYTE)GetnumClear();
		ar << (BYTE)0;
		if( ar.IsForDB() ) {
//			ar << 0;	// aryDrop
			ar << 0;	// legion
			ar << 0;	// power
		} else {
//			ar << GetaryDrops();
			XLegion::sSerializeFull( ar, GetspLegion() );
			ar << GetPower();
		}
	}
	MAKE_CHECKSUM( ar );
	return 1;
}
/**
 @brief 
 영웅의전장 스테이지는 프로퍼티가 미리 세팅되어 있지않고 가챠영웅리스트에 맞춰서
 다이나믹하게 설정되어야 하므로 Deserialize에서 실시간으로 프로퍼티를 생성한다.
*/
int XStageObjHero::DeSerialize( const CampObjPtr spCampObj,
//								XPropCamp::xProp* pPropCamp, 
								int idxStage, XArchive& ar, int verCamp )
{
	XLOCK_OBJ;
	BYTE b0;
	DWORD dw0;
	ID idProp;
	auto pPropCamp = spCampObj->GetpProp();
// 	auto pPropStage = new XPropCamp::xStage;
// 	sSetDefaultProp( pPropStage );
	ar >> idProp;
	ar >> m_idHero;
	XBREAK( idProp == 0 );
	XBREAK( m_idHero == 0 );

	StagePtr spPropStage 
		= sCreatePropStage( pPropCamp, idProp, idxStage, m_idHero );
	SetspPropStage( spPropStage );
//	pPropStage->DeSerialize( ar, verCamp );
	ar >> b0; sSetLevelLegion( GetspPropStage(), b0 );
	SetLevelLegion( spPropStage->m_spxLegion->lvLegion );
	ar >> b0;	SetnumStar( b0 );
	ar >> b0;	SetnumClear( b0 );
	ar >> b0;	
// 	ar >> GetaryDrops();
	// 군단데이터가 있으면 생성.
	SetspLegion( XSPLegion(XLegion::sCreateDeserializeFull( ar )) );
	ar >> dw0;	SetPower( dw0 );
	// aryDrops복구해야함.
	SetDropItemsFromProp( spCampObj );
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

/*
저장해야할 스테이지 프롭
idProp
levelLegion
//aryReward
//idxStage
*/
StagePtr XStageObjHero::sCreatePropStage( XPropCamp::xProp *pPropCamp, 
										ID idProp,
										int idxStage, 
										ID idHeroDrop )
{
//	auto pPropStage = new XPropCamp::xStage;
	auto pPropStage = XPropCamp::sCreatePropStage( pPropCamp->m_Type );
	sSetDefaultProp( pPropStage );
	pPropStage->idProp = idProp;
	XGAME::xReward reward;
	auto pPropSoul = PROP_ITEM->GetpPropByidHero( idHeroDrop );
	if( XASSERT(pPropSoul) )
		reward.SetItem( pPropSoul->idProp, 1 );
	pPropStage->aryReward.push_back( reward );
	pPropStage->idxStage = idxStage;
//	pPropStage->maxWin = 2;
//	pPropStage->levelLegion = lvLegion;
//	pPropStage->levelLimit = lvLegion - 5;
//	pPropStage->rateDrop = 0.9f;
	return StagePtr( pPropStage );
}

void XStageObjHero::sSetDefaultProp( XSPPropStage spProp )
{
	XBREAK( spProp == nullptr );
	spProp->maxWin = 2;
	spProp->rateDrop = 0.9f;
}
/**
 @brief 프로퍼티에 있는 드랍품목을 스테이지동적객체에 카피한다.
*/
bool XStageObjHero::SetDropItemsFromProp( const CampObjPtr spCampObj )
{
	bool bUpdate = false;
	if( XASSERT(GetspPropStage()) ) {
		float dropChance = 1.f;
		// 최초 언락된 스테이지에선 아이템 드랍안함.
		int idxStage = GetidxStage();
		if( spCampObj->GetidxLastUnlock() == idxStage )
			dropChance = 0.f;
		// 스테이지에서 드랍할수 있는 아이템을 스팟에 등록한다.
		ClearItems();
		bUpdate = true;
		for( auto& reward : GetspPropStage()->aryReward )
			AddItems( reward.GetidItem(), reward.num, dropChance );
	}
	return bUpdate;
}

XE_NAMESPACE_END; // xCampaign
