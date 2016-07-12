#include "stdafx.h"
#ifdef _XCAMP_HERO2
#include "XStageObjHero2.h"
#include "XStruct.h"
#include "XLegion.h"
#include "XCampObjHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define LV_LIMIT_UNDER	5

XE_NAMESPACE_START( xCampaign )

// void XStageObjHero2::sSetLevelLegion( StagePtr spPropStage, int lvLegion )
// {
// 	spPropStage->legion.lvLegion = lvLegion;
// 	spPropStage->levelLimit = lvLegion - LV_LIMIT_UNDER;
// 	if( spPropStage->levelLimit > XGAME::GetLevelMaxAccount() )
// 		spPropStage->levelLimit = XGAME::GetLevelMaxAccount();
// }
//////////////////////////////////////////////////////////////////////////
XStageObjHero2::XStageObjHero2( StagePtr spPropStage )
: XStageObj( spPropStage )
{
	Init();
}

/**
 @brief 영웅캠페인은 스테이지가 많으므로 이걸 전부 DB에 저장할필요는 없다.
*/
int XStageObjHero2::Serialize( XArchive& ar )
{
	XLOCK_OBJ;
	ar << (char)GetnumClear();
	ar << (char)0;
	ar << (short)0;
	if( ar.IsForDB() ) {
		// DB저장시엔 모든정보를 저장할필요는 없다.
		ar << (int)0;
	} else {
		ar << m_idHero;
		ar << GetPower();
		ar << (char)GetLevelLegion();
		ar << (char)0;
		ar << (short)0;
	}
	return 1;
}
/**
 @brief 
 영웅의전장 스테이지는 프로퍼티가 미리 세팅되어 있지않고 가챠영웅리스트에 맞춰서
 다이나믹하게 설정되어야 하므로 Deserialize에서 실시간으로 프로퍼티를 생성한다.
*/
int XStageObjHero2::DeSerialize( const CampObjPtr spCampObj,
								int idxStage, XArchive& ar, int verCamp )
{
	XLOCK_OBJ;

	auto pPropCamp = spCampObj->GetpProp();
	auto spPropStage = pPropCamp->GetspStageByidxStage( idxStage );
	XBREAK( spPropStage == nullptr );
	SetspPropStage( spPropStage );
	char c0;
	DWORD dw0;
	ar >> c0;		SetnumClear( c0 );
	ar >> c0;		
	ar >> c0 >> c0;
	ar >> dw0;
	if( dw0 > 0 ) {
		m_idHero = dw0;
		int i0;
		ar >> i0;		SetPower( i0 );
		ar >> c0;		SetLevelLegion( c0 );
		ar >> c0 >> c0 >> c0;
	}
	SetDropItemsFromProp( spCampObj );
	
	return 1;
}

// int XStageObjHero2::AddnumClear() override 
// {
// 	XLOCK_OBJ;
// // 	if( GetnumStar() > 0 )	// 별이 없으면(최초클리어) numClear수를 올리지 않음.
// 		return XStageObj::AddnumClear();
// 	return 0;
// }


/**
 @brief 프로퍼티에 있는 드랍품목을 스테이지동적객체에 카피한다.
*/
bool XStageObjHero2::SetDropItemsFromProp( const CampObjPtr spCampObj )
{
	bool bUpdate = false;
	if( XASSERT(GetspPropStage()) ) {
		float dropChance = 1.f;
		// 최초 언락된 스테이지에선 아이템 드랍안함.
		int idxStage = GetidxStage();
		if( spCampObj->GetidxLastUnlock() == idxStage )
			dropChance = 0.f;
		ClearItems();
		bUpdate = true;
		for( auto& reward : GetspPropStage()->aryReward )
			AddItems( reward.GetidItem(), reward.num, dropChance );
	}
	return bUpdate;
}

XE_NAMESPACE_END; // xCampaign
#endif // _XCAMP_HERO2
