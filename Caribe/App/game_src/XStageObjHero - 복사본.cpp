#include "stdafx.h"
#include "XStageObjHero.h"
#include "XStruct.h"
#include "XLegion.h"

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
	spPropStage->levelLegion = lvLegion;
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
 		ar << spPropStage->idProp;

//		spPropStage->Serialize( ar );
		ar << (BYTE)GetnumStar();
		ar << (BYTE)GetnumClear();
		ar << (BYTE)0;
		ar << (BYTE)0;
		if( ar.IsForDB() ) {
			ar << m_idHero;		// 0을 넣어야 맞지만 역직렬화 코드에서 0이면 assert되는게 있어서 그냥 넣음.
			ar << 0;	// aryDrop
			ar << 0;	// legion
			ar << 0;	// power
		} else {
			ar << m_idHero;
			ar << GetaryDrops();
			XLegion::sSerializeFull( ar, GetspLegion() );
			ar << GetPower();
// 			if( GetspLegion() != nullptr ) {
// 				GetspLegion()->SerializeFull( ar );
// 				ar << GetPower();
// 			} else {
// 				ar << 0;
// 				ar << 0;
// 			}
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
int XStageObjHero::DeSerialize( XPropCamp::xProp* pPropCamp, 
								int idxStage, XArchive& ar, int verCamp )
{
	XLOCK_OBJ;
	BYTE b0;
	DWORD dw0;
	auto pPropStage = new XPropCamp::xStage;
	pPropStage->DeSerialize( ar, verCamp );
	SetspPropStage( StagePtr( pPropStage ) );
	SetLevelLegion( pPropStage->levelLegion );
	ar >> b0;	SetnumStar( b0 );
	ar >> b0;	SetnumClear( b0 );
	ar >> b0;	
	ar >> b0;	
	ar >> m_idHero;
	XBREAK( m_idHero == 0 );
	ar >> GetaryDrops();
	// 군단데이터가 있으면 생성.
	SetspLegion( LegionPtr(XLegion::sCreateDeserializeFull( ar )) );
	if( verCamp >= 17 ) {
		ar >> dw0;	SetPower( dw0 );
	}
// 	if( GetspLegion() ) {
// 		ar >> dw0;	
// 		XBREAK( dw0 == 0 );
// 		SetPower( dw0 );
// 	} else {
// 		ar >> dw0;
// 		SetPower( dw0 );
// 	}
	RESTORE_VERIFY_CHECKSUM( ar );
	return 1;
}

StagePtr XStageObjHero::sCreatePropStage( XPropCamp::xProp *pPropCamp, 
										ID idProp,
										int idxStage, 
										ID idHeroDrop )
{
	auto pPropStage = new XPropCamp::xStage;
	pPropStage->idProp = idProp;
	XGAME::xReward reward;
	auto pPropSoul = PROP_ITEM->GetpPropByidHero( idHeroDrop );
	if( XASSERT(pPropSoul) )
		reward.SetItem( pPropSoul->idProp, 1 );
	pPropStage->aryReward.push_back( reward );
	pPropStage->idxStage = idxStage;
	pPropStage->maxWin = 2;
//	pPropStage->levelLegion = lvLegion;
//	pPropStage->levelLimit = lvLegion - 5;
	pPropStage->rateDrop = 0.9f;
	return StagePtr( pPropStage );
}
XE_NAMESPACE_END; // xCampaign
