#include "stdafx.h"
#include "XSpotCampaign.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
#include "XGame.h"
#endif
#ifdef _GAME_SERVER
	#include "XGameUser.h"
#endif
#include "XLegion.h"
#include "XPropHero.h"
#include "XWorld.h"
#include "XPropItem.h"
#include "XCampObj.h"
#include "XStageObj.h"
#include "XGuild.h"
#include "server/XGuildMgr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
XSpotCampaign::XSpotCampaign( XWorld *pWorld, XPropWorld::xCampaign* pProp, XPropCamp::xProp *pPropCamp )
	: XSpot( pProp, XGAME::xSPOT_CAMPAIGN, pWorld )
{
	Init();
	if( pProp ) {
		SetstrName( XTEXT( pProp->idName ) );
		m_spCampObj = xCampaign::XCampObj::sCreateCampObj( pPropCamp, 0 );
	}
}

void XSpotCampaign::OnCreateNewOnServer( XSPAcc spAcc )
{
}

/** //////////////////////////////////////////////////////////////////
 @brief 
*/
void XSpotCampaign::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	XBREAK( GetpProp()->idSpot > 0xffff );
	ar << (WORD)GetpProp()->idSpot;
	ar << (WORD)0;
	XASSERT(m_spCampObj != nullptr);
	XCampObj::sSerialize( m_spCampObj, ar );
}

/** //////////////////////////////////////////////////////////////////
 @brief 
*/
BOOL XSpotCampaign::DeSerialize( XArchive& ar, DWORD ver ) 
{
	XSpot::DeSerialize( ar, ver );
	ID idProp;
	WORD w0;
	ar >> w0;	idProp = w0;
	ar >> w0;
//	m_spCampaignObj = xCampaign::XCampObj::sCreateDeserialize( ar );
	if( m_spCampObj == nullptr ) {
		m_spCampObj = xCampaign::XCampObj::sCreateDeserialize( ar );
	} else {
		XCampObj::sDeserializeUpdate( m_spCampObj, ar );
	}
	return TRUE;
}

/**
 @brief 스테이지 하나를 클리어하면 스테이지에 드랍아이템 판정을 한다.
*/
int XSpotCampaign::DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum/* = 1.f*/ ) const
{
	XBREAK( m_spCampObj->GetspStageLastPlay() == nullptr );
	auto spStage = m_spCampObj->GetspStageLastPlay();
	if( spStage ) {
		lvSpot = spStage->GetLevelLegion();
	}
	XBREAK( lvSpot == 0 );
	// 징표등 기본 드랍.
	XSpot::DoDropItem( spAcc, pOutAry, lvSpot, multiplyDropNum );

	return pOutAry->size();
}

/**
 스팟에 있던 Campaign군대를 없앤다. 군대가 없으면 스팟은 비어있는것으로 친다.
*/
void XSpotCampaign::ClearLegion( void )
{
	SetLevel( 0 );
	DestroyLegion();
}

/**
 @brief pStageObj의 군단정보를 만든다.
*/
void XSpotCampaign::CreateLegion( xCampaign::XStageObj *pStageObj )
{
	auto spLegion = pStageObj->CreateLegion( m_spCampObj, 0, 0 );
	SetspLegion( pStageObj->GetspLegion() );	// 스팟의 군단정보에도 넣는다.
	//
	UpdatePower( GetspLegion() );
}

bool XSpotCampaign::Update( XSPAcc spAcc )
{
	// 각 스테이지의 군단이 아직 생성안되었다면 생성시킴.
	if( XBREAK( m_spCampObj == nullptr ) )
		return true;
#ifdef _GAME_SERVER
	m_spCampObj->Update( spAcc );
#endif // _GAME_SERVER
	return true;
}

void XSpotCampaign::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
	if( m_spCampObj )
		m_spCampObj->OnFinishBattle();
}

void XSpotCampaign::SerializeForBattle( XArchive* pOut, const XParamObj2& param )
{
	XSpot::SerializeForBattle( pOut, param );
	///< 
	const int idxStage = param.GetInt( "idxStage" );
	const int idxFloor = param.GetInt( "idxFloor" );
	*pOut << idxStage;
	*pOut << idxFloor;
	// 요청한 스테이지의 정보를 팩킹함.
	const auto spStage = m_spCampObj->GetspStage( idxStage, idxFloor );
	XStageObj::sSerialize( *pOut, spStage );
}

void XSpotCampaign::DeSerializeForBattle( XArchive& arLegion, XArchive& arAdd, XSPAcc spAcc )
{
	XSpot::DeSerializeForBattle( arLegion, arAdd, spAcc );
	int idxStage, idxFloor;
	arAdd >> idxStage >> idxFloor;
}

void XSpotCampaign::ResetLevel( XSPAcc spAcc )
{
	if( GetLevel() == 0 ) {
		if( m_spCampObj ) {
			auto spStageObj = m_spCampObj->GetspStageObjCurrPlaying();
			if( spStageObj ) {
				int lvLegion = spStageObj->GetLevelLegion();
				XBREAK( lvLegion <= 0 );
				SetLevel( lvLegion );
			}
		}
	}
}

void XSpotCampaign::ResetName( XSPAcc spAcc )
{
	if( m_spCampObj ) {
		if( GetstrName().empty() )
			if( m_spCampObj->GetpProp() )
				SetstrName( XTEXT( m_spCampObj->GetpProp()->idName ) );
	}
}

void XSpotCampaign::OnBeforeBattle( XSPAcc spAcc )
{
	// 이름이 지정안되어있으면 이름을 지정함.
	if( GetstrName().empty() )
		ResetName( spAcc );
	// 레벨이 지정안되어있으면 레벨을 지정함.
	if( GetLevel() <= 0 )
		ResetLevel( spAcc );
}

void XSpotCampaign::ClearSpot()
{
	if( m_spCampObj != nullptr )
		m_spCampObj->DestroyLegionbyLastPlayStage();
	XSpot::ClearSpot();
}

