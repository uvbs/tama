#include "stdafx.h"
#include "XSpotEtc.h"
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
#include "XCampObjHero.h"
#include "XStageObjHero.h"
#include "XGuild.h"
#include "server/XGuildMgr.h"
#include "XSystem.h"

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
void XSpotDaily::OnCreateNewOnServer( XSPAcc spAcc )
{
	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
	m_dowToday = dowToday;
	int hour, min, sec;
	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
	// 오늘0시 기준으로 현재시간은 총 몇초인가를 계산한다.
	int secTotal = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
	// 오늘 요일의 이벤트 스팟을 활성화 시킨다.
	spAcc->GetpWorld()->SetActiveDailySpotToRandom( dowToday, secTotal, spAcc );
}

void XSpotDaily::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << (BYTE)GetpProp()->idSpot;
	ar << (BYTE)m_Type;
	ar << (BYTE)m_numEnter;
	ar << (BYTE)m_dowToday;
	m_timerCreate.Serialize( ar );
// 	m_timerEnter.Serialize( ar );	// 삭제되었으므로 dummy로 읽고 없앰.
}
BOOL XSpotDaily::DeSerialize( XArchive& ar, DWORD ver ) 
{
#ifndef _DUMMY_GENERATOR
//	// CONSOLE( "deserialize daily" );
#endif // not _DUMMY_GENERATOR
	XSpot::DeSerialize( ar, ver );
	ID idProp;
	BYTE b0;
	ar >> b0;	idProp = b0;
	ar >> b0;	m_Type = (XGAME::xtDailySpot)b0;
	ar >> b0;	m_numEnter = b0;
	ar >> b0;	m_dowToday = (XE::xtDOW)b0;
	m_timerCreate.DeSerialize( ar );
	return TRUE;
}

void XSpotDaily::ResetLevel( XSPAcc spAcc )
{
	SetLevel( spAcc->GetLevel() + 1 );
}

void XSpotDaily::ResetName( XSPAcc spAcc )
{
	if( GetstrName().empty() )
		SetstrName( XTEXT( 2228 ) );	// 요일 전장
}

void XSpotDaily::SetSpot( XE::xtDOW dow, int secPass, XSPAcc spAccount ) 
{
	m_dowToday = dow;
	m_Type = XSpotDaily::sGetDowToType( dow );
	m_numEnter = 0;
	SetstrName( XTEXT(2228) );	// 요일 전장

	m_timerCreate.Set( (float)( 24 * 60 * 60 ) );		// 24시간을 세팅
	m_timerCreate.SetPassSec( (float)secPass );	// 흘러간 시간을 더함.
	ResetLevel( spAccount );
}

void XSpotDaily::ResetPower( int lvSpot )
{
	SetPower( 0 );
}
/**
 @brief 요일스팟 전투에 입장한다.
 성공하면 xERR_OK를 반환하고 실패할경우는 그외 에러코드를 반환한다.
*/
XSpotDaily::xtError XSpotDaily::DoEnter() 
{
	xtError errCode = xERR_OK;
	if( IsAttackable( nullptr, &errCode ) ) {
		// 입장가능.
		//++m_numEnter;
	}
	return errCode;

}

/**
 @brief 이 스팟이 공격이 가능한 상태인지 검사한다.
 pOut을 넘겨주면 구체적인 에러코드를 담아준다.
*/
bool XSpotDaily::IsAttackable( XSPAcc spAcc, xtError *pOut )
{
	xtError errCode = xERR_OK;
	do {
		if( m_timerCreate.IsOver() ) {
			errCode = xERR_TIMEOUT;
			break;
		}
		if( m_numEnter >= xNUM_ENTER ) {
			errCode = xERR_OVER_ENTER_NUM;
			break;
		}
	} while (0);
	if( pOut )
		*pOut = errCode;
	return errCode == xERR_OK;
}

void XSpotDaily::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, bRetreat );
	if( bWin ) {	// 이길때만 깎이는걸로 정책전환.
		AddnumEnter();
	}
}

/**
 스팟에 Daily군대를 생성시킨다.
*/
void XSpotDaily::CreateLegion( XSPAcc spAccount )
{
	/*
	.오늘의 일(day)수를 구한다.
	.% 3을 해서 중,소,대를 가린다.
	.정해진 크기의 유닛으로 군대를 만든다.
	.군단난이도는 노랑색 수준.
	*/
// 	XSpot::CreateLegion( spAccount );
	if( GetLevel() == 0 )
		ResetLevel( spAccount );
	if( GetstrName().empty() )
		ResetName( spAccount );
	int lvLegion = GetLevel();
	// 렙이 낮아 소형유닛만 보유하고 있을때 적이 중형급만 나온다면 질수밖에 없다.
	// 그래서 일단 크기대신에 병과별로 나오게 한다.
	auto typeAtk = GetAtkType();
	XGAME::xLegionParam infoLegion;
// 	infoLegion.SetUnitFilter( typeAtk );	// 해당병과의 유닛만 나오도록
//	infoLegion.unit = XGAME::GetUnitRandomByFilter( XGAME::xUF_ALL );
	infoLegion.unit = (xtUnit)((m_Day % (xUNIT_MAX-1)) + 1);	// 매일 유닛이 바뀌게
	auto pLegion = XLegion::sCreateLegionForNPC( //GetspOwner()->GetLevel(), 
												lvLegion,
												0,
												infoLegion );
	XBREAK( GetspLegion() != nullptr );
	SetpLegion( pLegion );
	// 요일 보상(reward를 vector로 받아야 할거 같다.
	ClearLootRes();
	XVector<XGAME::xReward> aryRes;		// 오늘의 보상 리스트
	XSpot::sGetRewardDaily( GetpProp(), m_dowToday, spAccount->GetLevel(), &aryRes );
	// 드랍자원을 세팅(현재는 자원류 보상만 지원됨)
	SetLootRes( aryRes );
	UpdatePower( GetspLegion() );
}

void XSpotDaily::Process( float dt )
{

	XSpot::Process( dt );
}


/**
 @brief 오늘 요일에 따라 다른 클랜의 징표가 떨어진다.
*/
int XSpotDaily::DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum ) const
{
	return pOutAry->size();
}

bool XSpotDaily::Update( XSPAcc spAcc )
{
	m_Day = XTimer2::sGetTime() / 60 / 60 / 24;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// void XSpotSpecial::OnCreateNewOnServer( XSPAcc spAcc )
// {
// 	if( XBREAK( spAcc == nullptr ) )
// 		return;
// 	// 스페셜 스팟 타이머가 아직 세팅안되어 있다면 세팅한다.
// //	if( spAcc->GetymdSpecialSpotStart().IsActive() == FALSE )
// 	if( spAcc->IsActiveSpecialSpotTimer() == false )
// 		spAcc->SetSpecialSpotTimer();
// 	// 스페셜 스팟이 지금 활성화 되어야 하는지 확인한다.
// 	spAcc->DoCheckSpecialSpotActivateNow();
// }
// void XSpotSpecial::Serialize( XArchive& ar ) 
// {
// 	XSpot::Serialize( ar );
// 	ar << (BYTE)GetpProp()->idSpot;
// 	ar << (BYTE)m_Type;
// 	ar << (BYTE)m_numEnter;
// 	ar << (BYTE)m_numEnterTicket;
// //	m_timerCreate.Serialize( ar );
// 	ar << m_secCreate;
// 	ar << m_secReleased;
// 	m_timerRecharge.Serialize( ar );
// 	ar << m_idxRound;
// 	// 활성화된 스팟인데 군대가 없으면 경고.
// //	XBREAK( m_Type != XGAME::xSS_NONE && GetspLegion() == nullptr );
// // 	if( GetspLegion() != nullptr && ar.IsForDB() == false )		// 비활성화된 스팟은 군대가 없음.
// // 		GetspLegion()->SerializeFull( ar );
// // 	else
// // 		ar << 0;
// }
// BOOL XSpotSpecial::DeSerialize( XArchive& ar, DWORD ver ) 
// {
// #ifndef _DUMMY_GENERATOR
// //	// CONSOLE( "deserialize special" );
// #endif // not _DUMMY_GENERATOR
// 	XSpot::DeSerialize( ar, ver );
// 	ID idProp;
// 	BYTE b0;
// 	ar >> b0;	idProp = b0;
// 	ar >> b0;	m_Type = (XGAME::xtSpecialSpot)b0;
// 	ar >> b0;	m_numEnter = b0;
// 	ar >> b0;	m_numEnterTicket = b0;
// 	ar >> m_secCreate;
// 	ar >> m_secReleased;
// 	m_timerRecharge.DeSerialize( ar );
// 	ar >> m_idxRound;
// 	return TRUE;
// }
// 
// /**
//  스팟에 Special군대를 생성시킨다.
// */
// void XSpotSpecial::CreateLegion( XSPAcc spAcc )
// {
// 	if( GetLevel() == 0 )
// 		SetLevel( spAcc->GetLevel() );
// 	// spAccount와 싸우기 적당한 수준의 군단을 생성시킨다.
// //	SetstrName( _T( "barbarian" ) );
// 	int lv = GetLevel() + m_idxRound;
// 	XGAME::xLegionParam legionInfo;
// 	legionInfo.x_gradeLegion = XGAME::xGL_ELITE;
// 	XLegion *pLegion = XLegion::sCreateLegionForNPC( //GetspOwner()->GetLevel(), 
// 													lv, 0, legionInfo );
// 	XBREAK( GetspLegion() != nullptr );
// 	SetpLegion( pLegion );
// 	UpdatePower( GetspLegion() );
// // 	int power = XLegion::sGetMilitaryPower( GetspLegion(), nullptr );
// // 	SetPower( power );
// 	ClearDropItems();
// 	DoDropRegisterEquip( spAcc->GetLevel() );
// }
// 
// void XSpotSpecial::Process( float dt )
// {
// 	// 스팟은 활성화 되어있는데 타이머가 꺼져있으면 안됨.
// //	XBREAK( m_Type != XGAME::xSS_NONE && m_timerCreate.IsOff() );
// 	XBREAK( m_Type != XGAME::xSS_NONE && m_secCreate == 0 );
// 	if( m_timerSec.IsOff() )
// 		m_timerSec.Set( 1.f );
// 	// 1초마다 한번씩 검사.
// 	if( m_timerSec.IsOver() )
// 	{
// //		if( m_timerCreate.IsOver() && m_Type != XGAME::xSS_NONE )
// 		if( m_secCreate )
// 		{
// 			xSec secCurr = XTimer2::sGetTime();
// 			XBREAK( secCurr - m_secCreate > 24 * 60 * 60 * 8 );	// 지나치게 오래된 시간값.
// 			if( secCurr - m_secCreate > 24 * 60 * 60 )
// 			{
// 				// 24시간 다되면 스팟 사라짐.
// 				if( GetpDelegate() )
// 					GetpDelegate()->DelegateReleaseSpotBefore( this );
// 				ReleaseSpot();
// 				m_secCreate = 0;
// 				m_secReleased = secCurr;
// 				if( GetpDelegate() )
// 					GetpDelegate()->DelegateReleaseSpotAfter( this );
// //				m_timerCreate.Off();
// 			}
// 		}
// 		// 입장횟수 충전타이머
// 		if( m_timerRecharge.IsOver() )
// 		{
// 			// 20분이 지나면 입장횟수를 1증가시킴
// 			if( ++m_numEnterTicket > xNUM_ENTER )
// 				m_numEnterTicket = xNUM_ENTER;
// 			// 스폰 델리게이트를 이용한다.
// 			if( GetpDelegate() )
// 				GetpDelegate()->DelegateOnSpawnTimer( this, 0 );
// 			m_timerRecharge.Reset();
// 		}
// 	}
// 	XSpot::Process( dt );
// }
// 
// bool XSpotSpecial::Update( XSPAcc spAcc )
// {
// 	xSec secCurr = XTimer2::sGetTime();
// 	// 자정으로부터 지나간 시간을 세팅한다.
// 	int hour, min, sec;
// 	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
// 	int secPass = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
// 	if( m_Type != XGAME::xSS_NONE )	{
// 		if( m_secCreate == 0 ||
// 			(m_secCreate > 0 && secCurr - m_secCreate > 24 * 60 * 60 * 8) )
// 		{
// 			m_secCreate = secCurr;
// 			m_secCreate -= secPass;
// 			m_secReleased = 0;
// 		}
// 	} else	{
// 		m_secCreate = 0;
// 		if( m_secReleased == 0 )
// 		{
// 			m_secReleased = secCurr;
// 			m_secReleased -= secPass;
// 		}
// 	}
// 	return true;
// }
//////////////////////////////////////////////////////////////////////////
void XSpotVisit::OnCreateNewOnServer( XSPAcc spAcc )
{
}
void XSpotVisit::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << (DWORD)0;
}
BOOL XSpotVisit::DeSerialize( XArchive& ar, DWORD verWorld ) 
{
	DWORD dw0;
	XSpot::DeSerialize( ar, verWorld );
	ar >> dw0;
	return TRUE;
}

////////////////////////////////////////////////////////////////
void XSpotCash::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
	XBREAK( m_numCash > 255 );
	ar << (BYTE)m_numCash;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
}

BOOL XSpotCash::DeSerialize( XArchive& ar, DWORD verWorld )
{
#ifndef _DUMMY_GENERATOR
	// CONSOLE( "deserialize CashSpot" );
#endif // not _DUMMY_GENERATOR
	XSpot::DeSerialize( ar, verWorld );
	BYTE b0;
	ar >> b0;	m_numCash = b0;
	ar >> b0;
	ar >> b0;
	ar >> b0;
	return TRUE;
}
void XSpotCash::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	XSpot::OnAfterDeSerialize( pWorld, pDelegator, idAcc, secLastCalc );
#ifdef _SERVER
	if( secLastCalc )	{
		// 마지막 저장한후로부터 현재까지 지나간 시간을 얻는다.
		xSec secPassTime = ( XTimer2::sGetTime() - secLastCalc );
		GettimerSpawnMutable().AddTime( secPassTime );
	}
#endif // SERVER
}

//////////////////////////////////////////////////////////////////////////
void XSpotCommon::OnCreateNewOnServer( XSPAcc spAcc )
{

}

void XSpotCommon::Serialize( XArchive& ar )
{
	XSpot::Serialize( ar );
	XBREAK( GetpProp()->idSpot > 0xffff );
	ar << (WORD)GetpProp()->idSpot;
	XBREAK( m_DayOfCamp > 0xffff );
	ar << (WORD)m_DayOfCamp;
	if( IsGuildRaid() ) {
		// 길드레이드 스팟의 경우 DB저장시에는 캠페인정보를 저장하지 않음. 캠페인정보는 길드쪽에서 따로 저장함.
		if( ar.IsForDB() )
			ar << 0;
		else
			XCampObj::sSerialize( m_spCampObj, ar );
	} else
		XCampObj::sSerialize( m_spCampObj, ar );
}
BOOL XSpotCommon::DeSerialize( XArchive& ar, DWORD verWorld )
{
	XSpot::DeSerialize( ar, verWorld );
	ID idProp;
	WORD w0;
	ar >> w0;	idProp = w0;
	ar >> w0;	m_DayOfCamp = w0;
	m_spCampObj = XCampObj::sCreateDeserialize( ar );
#ifdef _XCAMP_HERO2
	if( m_spCampObj && m_spCampObj->GetType() == xCT_HERO ) {
		m_spCampObj.reset();
	}
#endif // _XCAMP_HERO2
	return TRUE;
}

XPropCamp::xProp* XSpotCommon::GetpPropCamp()
{
	return XPropCamp::sGet()->GetpProp( GetpProp()->strParam );
}

CampObjPtr XSpotCommon::GetspCampObj( XGuild *pGuild )
{
#ifdef _GAME_SERVER
	if( IsGuildRaid() ) {
		if( m_spCampObj != nullptr )
			return m_spCampObj;
		auto pProp = GetpPropCamp();
		if( XASSERT( pProp ) ) {
			if( XASSERT(pGuild) ) {
				m_spCampObj = pGuild->FindspRaidCampaign( pProp->idProp );
				return m_spCampObj;
			}
		}
	} else
	if( IsMedalCamp() ) {
		int day = XTimer2::sGetTime() / 60 / 60 / 24;
		if( day != m_DayOfCamp ) { 
			m_spCampObj.reset();
			// 날짜가 바꼈으면 캠페인을 새로 생성함.
			// 스팟마다 달라지는부분: 드랍품목, 드랍개수. 도전횟수, 
// 			int grade = GetpProp()->nParam[ 0 ];
// 			XBREAK( grade >= 4 );
			auto pPropCamp = GetpPropCamp();
			XBREAK( pPropCamp == nullptr );
			m_spCampObj = xCampaign::XCampObj::sCreateCampObj( pPropCamp, 0 );
			auto spOwner = GetspOwner().lock();
			XBREAK( spOwner == nullptr );
			// 군대생성시 참조할 외부파라메터를 모든 스테이지에 적용한다.
			XGAME::xLegionParam paramLegion;
			paramLegion.unit = (XGAME::xtUnit)((day % (XGAME::xUNIT_MAX-1)) + 1);
			m_spCampObj->SetParamLegionAllStage( paramLegion );
			m_spCampObj->CreateLegionAll( spOwner->GetLevel() );
			// 드랍아이템 품목을 작성한다.
			UpdateDropItems();
			m_DayOfCamp = day;
		}
		// 날짜가 안바꼈으면 현재 캠페인을 리턴함.
		return m_spCampObj;
	} else
	if( IsHeroCamp() ) {
		// m_spCampObj가 세이브되었다가 로드된것일수도 있으므로
		// 단순히 이미 있는지를 검사하면 안되고 스테이지바뀔게 없는지 업데이트과정을 한번 거쳐야 한다.
		auto pProp = GetpPropCamp();
		if( m_spCampObj == nullptr ) {
			if( XASSERT( pProp ) ) {
#ifdef _XCAMP_HERO2
				m_spCampObj = xCampaign::sCreateCampObj( xCampaign::xCT_HERO2, pProp, 0 );
#else
				m_spCampObj = xCampaign::sCreateCampObj( xCampaign::xCT_HERO, pProp, 0 );
#endif // _XCAMP_HERO2
			}
		}
		// 스테이지순서가 바꼈을수 있으므로 업데이트를 한번한다.
		XBREAK( m_spCampObj == nullptr );
// 		auto spCampObjHero = SafeCast<XCampObjHero*>( m_spCampObj.get() );
// 		if( XASSERT(spCampObjHero) )
			// 업데이트가 이미 되어있다면 다시 하지 않음.
// 			if( !spCampObjHero->GetbUpdated() )
// 				spCampObjHero->UpdateStages();
		return m_spCampObj;
	}
	return nullptr;
#else
	return m_spCampObj;
#endif // _GAME_SERVER
}
/**
 @brief 각 스테이지가 드랍할수 있는 품목을 세팅한다. 이미 리스트가 있다면 넣지 않는다.
*/
void XSpotCommon::UpdateDropItems()
{
	if( m_spCampObj == nullptr )
		return;
	if( IsMedalCamp() ) {
		// 생성할때 만드니까 여기서 필요없긴한데 그냥 넣음.
// 		int grade = GetpProp()->nParam[ 0 ];
// 		XBREAK( grade >= 4 );
		int numStages = m_spCampObj->GetNumStages();
		for( int i = 0; i < numStages; ++i ) {
			auto spStageObj = m_spCampObj->GetspStage( i );
			// 드랍아이템목록이 없을때만 넣는다.
			if( spStageObj && spStageObj->GetaryDrops().size() == 0 ) {
				// 각 스테이지마다 등급에 맞는 3병과 메달을 넣는다.
				int num = i + 1;
				auto 
				ids = XGAME::GetIdsMedal( XGAME::xAT_TANKER, 0 );
				spStageObj->AddItems( ids, num );
			}
		}
	}
}

bool XSpotCommon::Update( XSPAcc spAcc )
{
	UpdateDropItems();
	return false;
}

#ifdef _SERVER
CampObjPtr XSpotCommon::GetspCampObj( ID idGuild )
{
#ifdef _CLIENT
	XBREAK(1);	// 클라에선 아직 사용안됨. GAME->m_pGuild를 가져오도록 바꿔야 함.
#endif
	if( m_spCampObj != nullptr )
		return m_spCampObj;
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if( XASSERT(pGuild) )
		return GetspCampObj( pGuild );
	return CampObjPtr();
}
#endif // _SERVER

/**
 @brief 스테이지 하나를 클리어하면 스테이지에 드랍아이템 판정을 한다.
*/
int XSpotCommon::DoDropItem( XSPAcc spAcc, 
							XArrayLinearN<ItemBox, 256> *pOutAry, 
							int lvSpot, 
							float multiplyDropNum/* = 1.f*/ ) const
{
	// 징표등 기본 드랍.
	XSpot::DoDropItem( spAcc, pOutAry, lvSpot, multiplyDropNum );
	if( IsMedalCamp() ) {
		if( XASSERT( m_spCampObj ) ) {
// 			auto atkType = (XGAME::xtAttack)(1 + xRandom(XGAME::xAT_MAX-1) );
// 			auto idsMedal = XGAME::GetIdsMedal( atkType, grade );
			int idxStage = m_spCampObj->GetidxPlaying();
			auto spStageObj = m_spCampObj->GetspStage( idxStage );
			if( XASSERT(spStageObj) ) {
				//
				for( auto& drop : spStageObj->GetaryDrops() ) {
					auto pPropItem = PROP_ITEM->GetpProp( drop.idDropItem );
					if( XASSERT(pPropItem) ) {
						ItemBox itemBox;
						std::get<0>( itemBox ) = pPropItem;
						std::get<1>( itemBox ) = drop.num;
						pOutAry->Add( itemBox );
					}
				}
			}
		}
	}
 	return pOutAry->size();
}

void XSpotCommon::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, bRetreat );
	if( m_spCampObj )
		m_spCampObj->OnFinishBattle();
}

void XSpotCommon::ResetLevel( XSPAcc spAcc )
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

void XSpotCommon::ResetName( XSPAcc spAcc )
{
	if( m_spCampObj ) {
		if( GetstrName().empty() )
			if( m_spCampObj->GetpProp() )
				SetstrName( XTEXT(m_spCampObj->GetpProp()->idName) );
	}
}

void XSpotCommon::OnBeforeBattle( XSPAcc spAcc )
{
	// 이름이 지정안되어있으면 이름을 지정함.
	if( GetstrName().empty() )
		ResetName( spAcc );
	// 레벨이 지정안되어있으면 레벨을 지정함.
	if( GetLevel() <= 0 )
		ResetLevel( spAcc );
}

bool XSpotCommon::IsGuildRaid() const
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return (pProp)? (pProp->strType == _T( "guild_raid" )) : false;
}
bool XSpotCommon::IsMedalCamp() const 
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return ( pProp ) ? ( pProp->strType == _T( "fixed_camp" ) ) : false;
}
// 영웅의전장인가.
bool XSpotCommon::IsHeroCamp() const 
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return ( pProp ) ? ( pProp->strType == _T( "hero_camp" ) ) : false;
}
