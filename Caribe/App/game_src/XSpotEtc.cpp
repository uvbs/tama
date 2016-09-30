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
// const int XSpotDaily::c_maxFloor = 9;			// 최대 단계수
// const int XSpotDaily::c_lvLegionStart = 9;		// 시작 군단레벨
// 
// void XSpotDaily::OnCreateNewOnServer( XSPAcc spAcc )
// {
// 	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	m_dowToday = dowToday;
// 	int hour, min, sec;
// 	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
// 	// 오늘0시 기준으로 현재시간은 총 몇초인가를 계산한다.
// 	int secTotal = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
// 	// 오늘 요일의 이벤트 스팟을 활성화 시킨다.
// 	spAcc->GetpWorld()->SetActiveDailySpotToRandom( dowToday, secTotal, spAcc );
// }
// 
// void XSpotDaily::Serialize( XArchive& ar ) 
// {
// 	XBREAK( m_idxFloor > 0x7f );
// 	XSpot::Serialize( ar );
// 	ar << (BYTE)GetpProp()->idSpot;
// 	ar << (char)m_idxFloor; // m_Type;
// 	ar << (BYTE)m_numEnter;
// 	ar << (BYTE)m_dowToday;
// 
// 	ar << (char)m_cntWeekByFloor;
// 	XASSERT( m_aryDay.Size() == 7 );
// 	for( auto& day : m_aryDay ) {
// 		ar << (char)day.m_numStar;
// 	}
// 	m_timerCreate.Serialize( ar );
// }
// BOOL XSpotDaily::DeSerialize( XArchive& ar, DWORD verWorld ) 
// {
// 	XSpot::DeSerialize( ar, verWorld );
// 	ID idProp;
// 	BYTE b0;
// 	char c0;
// 	ar >> b0;	idProp = b0;
// 	if( verWorld >= 30 ) {
// 		ar >> c0;	m_idxFloor = c0; //m_Type = (XGAME::xtDailySpot)b0;
// 	} else {
// 		ar >> c0;	m_idxFloor = 0;	// 이전버전은 읽고 버림
// 	}
// 	ar >> b0;	m_numEnter = b0;
// 	ar >> b0;	m_dowToday = (XE::xtDOW)b0;
// 	if( verWorld >= 30 ) {
// 		ar >> c0;	m_cntWeekByFloor = c0;
// 		XASSERT( m_aryDay.Size() == 7 );
// 		for( auto& day : m_aryDay ) {
// 			ar >> c0;		day.m_numStar = c0;
// 		}
// 	}
// 	m_timerCreate.DeSerialize( ar );
// 	return TRUE;
// }
// 
// void XSpotDaily::ResetLevel( XSPAcc spAcc )
// {
// 	SetLevel( spAcc->GetLevel() + 1 );
// }
// 
// void XSpotDaily::ResetName( XSPAcc spAcc )
// {
// 	if( GetstrName().empty() )
// 		SetstrName( XTEXT( 2228 ) );	// 요일 전장
// }
// 
// void XSpotDaily::SetSpot( XE::xtDOW dow, int secPass, XSPAcc spAccount ) 
// {
// 	m_dowToday = dow;
// //	m_Type = XSpotDaily::sGetDowToType( dow );
// 	m_numEnter = 0;
// 	SetstrName( XTEXT(2228) );	// 요일 전장
// 
// 	m_timerCreate.Set( (float)( 24 * 60 * 60 ) );		// 24시간을 세팅
// 	m_timerCreate.SetPassSec( (float)secPass );	// 흘러간 시간을 더함.
// 	ResetLevel( spAccount );
// 	Update( spAccount );
// }
// 
// void XSpotDaily::ResetPower( int lvSpot )
// {
// 	SetPower( 0 );
// }
// /**
//  @brief 요일스팟 전투에 입장한다.
//  성공하면 xERR_OK를 반환하고 실패할경우는 그외 에러코드를 반환한다.
// */
// XSpotDaily::xtError XSpotDaily::DoEnter() 
// {
// 	xtError errCode = xERR_OK;
// 	if( IsAttackable( nullptr, &errCode ) ) {
// 		// 입장가능.
// 		//++m_numEnter;
// 	}
// 	return errCode;
// 
// }
// 
// /**
//  @brief 이 스팟이 공격이 가능한 상태인지 검사한다.
//  pOut을 넘겨주면 구체적인 에러코드를 담아준다.
// */
// bool XSpotDaily::IsAttackable( XSPAcc spAcc, xtError *pOut )
// {
// 	xtError errCode = xERR_OK;
// 	do {
// 		if( m_timerCreate.IsOver() ) {
// 			errCode = xERR_TIMEOUT;
// 			break;
// 		}
// 		if( m_numEnter >= xNUM_ENTER ) {
// 			errCode = xERR_OVER_ENTER_NUM;
// 			break;
// 		}
// 	} while (0);
// 	if( pOut )
// 		*pOut = errCode;
// 	return errCode == xERR_OK;
// }
// 
// void XSpotDaily::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
// {
// 	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
// 	GetbitFlag().bUpdated = 0;		// 전투를 한번하고 나오면 다시 클리어 된다.
// 	if( bWin ) {	// 이길때만 깎이는걸로 정책전환.
// 		AddnumEnter();
// //		const auto day = XSYSTEM::GetDayOfWeek();
// 		const auto day = m_dowToday;		// 전투전엔 일요일이었는데 전투후 월욜로 바뀔수도 있어서.
// 		XBREAK( m_aryDay.size() == 0 );
// 		if( numStar > m_aryDay[day].m_numStar ) {
// 			m_aryDay[day].m_numStar = numStar;
// 			bool bClear = true;
// 			for( auto& day : m_aryDay ) {
// 				if( day.m_numStar != 3 ) {
// 					bClear = false;
// 					break;
// 				}
// 			}
// 			// 모든 요일을 클리어 함.
// 			if( bClear ) {
// 				++m_idxFloor;
// 				if( m_idxFloor >= c_maxFloor ) {
// 					m_idxFloor = c_maxFloor - 1;
// 					// 더이상 단계가 없음.
// 				} else {
// 					m_cntWeekByFloor = 0;
// 					for( auto& day : m_aryDay ) {
// 						day.Clear();
// 					}
// 					GetbitFlag().bUpdated = 1;
// 					m_cntTouch = 2;
// 					DestroyLegion();
// 					Update( spAccWin );
// 				}
// 			}
// 		}
// 	}
// }
// 
// XGAME::xtUnit XSpotDaily::GetUnitByDow( XE::xtDOW dow ) const
// {
// 	const auto size = GetSizeUnitByFloor();
// 	XGAME::xtAttack atkType = XGAME::xAT_NONE;
// 	switch( dow ) {
// 	case XE::xDOW_MONDAY:
// 	case XE::xDOW_THURSDAY:
// 		atkType = XGAME::xAT_TANKER;
// 		break;
// 	case XE::xDOW_TUESDAY:
// 	case XE::xDOW_FRIDAY:
// 		atkType = XGAME::xAT_RANGE;
// 		break;
// 	case XE::xDOW_WEDNESDAY:
// 	case XE::xDOW_SATURDAY:
// 		atkType = XGAME::xAT_SPEED;
// 		break;
// 	case XE::xDOW_SUNDAY: {
// 		const int maxAtkType = XGAME::xAT_MAX - 1;
// 		atkType = ( XGAME::xtAttack )( ( m_cntWeekByFloor % maxAtkType ) + 1 );
// 	} break;
// 	default:
// 		break;
// 	}
// 	const auto unit = XGAME::GetUnitBySizeAndAtkType( size, atkType );
// 	return unit;
// }
// 
// /**
//  @brief 현재 단계의 dow요일의 군단레벨 계산.
//  lv9 ~ lv50
//  총 9단계
//  각 단계는 +6렙씩 증가
//  각 요일은 월~일까지 0~6의 상대레벨
// */
// int XSpotDaily::GetlvLegionDow( XE::xtDOW dow ) const
// {
//  	const int lvStart = c_lvLegionStart;
//  	const int lvMax = XGAME::MAX_ACC_LEVEL;
//  	const int maxFloor = XSpotDaily::c_maxFloor;			// 총 단계수
//  	const int lvArea = lvMax - lvStart;		// 총 군단레벨 범위
//  	const float lvAddPerFloor = ROUND_FLOAT(lvArea / (float)maxFloor, 0);		// 층당 증가 레벨(반올림)
//  	const int lvStartCurr = (int)(lvStart + m_idxFloor * lvAddPerFloor);
// 	const float lvPerDay = ( lvAddPerFloor / 7.f);
// 	return (int)(lvStartCurr + XE::GetidxDow( dow ) * lvPerDay);
// }
// 
// /**
//  @brief 현재 단계의 시작레벨을 구한다.
// */
// int XSpotDaily::GetlvLegionCurrFloor() const 
// {
// 	const int lvStart = c_lvLegionStart;
// 	const int lvMax = XGAME::MAX_ACC_LEVEL;
// 	const int maxFloor = XSpotDaily::c_maxFloor;			// 총 단계수
// 	const int lvArea = lvMax - lvStart;		// 총 군단레벨 범위
// 	const float lvAddPerFloor = ROUND_FLOAT( lvArea / (float)maxFloor, 0 );		// 층당 증가 레벨(반올림)
// 	const int lvStartCurr = (int)( lvStart + m_idxFloor * lvAddPerFloor );
// 	return lvStartCurr;
// }
// /**
//  스팟에 Daily군대를 생성시킨다.
// */
// void XSpotDaily::CreateLegion( XSPAcc spAccount )
// {
// 	/*
// 	.오늘의 일(day)수를 구한다.
// 	.% 3을 해서 중,소,대를 가린다.
// 	.정해진 크기의 유닛으로 군대를 만든다.
// 	.군단난이도는 노랑색 수준.
// 	*/
// // 	XSpot::CreateLegion( spAccount );
// // 	if( GetLevel() == 0 )
// // 		ResetLevel( spAccount );
// 	if( GetstrName().empty() )
// 		ResetName( spAccount );
// //	int lvLegion = GetLevel();
// 	const int lvLegion = GetlvLegionDow( GetdowToday() );
// 	SetLevel( lvLegion );
// 	// 렙이 낮아 소형유닛만 보유하고 있을때 적이 중형급만 나온다면 질수밖에 없다.
// 	// 그래서 일단 크기대신에 병과별로 나오게 한다.
// //	auto typeAtk = GetAtkType();
// 	XGAME::xLegionParam infoLegion;
// //	infoLegion.unit = (xtUnit)((m_Day % (xUNIT_MAX-1)) + 1);	// 매일 유닛이 바뀌게
// 	infoLegion.unit = GetUnitByToday();
// 	auto pLegion = XLegion::sCreateLegionForNPC( lvLegion,
// 																							0,
// 																							infoLegion );
// 	XBREAK( GetspLegion() != nullptr );
// 	SetpLegion( pLegion );
// 	// 요일 보상(reward를 vector로 받아야 할거 같다.
// 	ClearLootRes();
// 	XVector<XGAME::xReward> aryRes;		// 오늘의 보상 리스트
// 	sGetRewardDaily( GetpProp(), m_dowToday, lvLegion, &aryRes );
// 	// 드랍자원을 세팅(현재는 자원류 보상만 지원됨)
// 	SetLootRes( aryRes );
// 	UpdatePower( GetspLegion() );
// }
// 
// void XSpotDaily::Process( float dt )
// {
// 
// 	XSpot::Process( dt );
// }
// 
// /**
//  @brief 오늘 요일에 따라 다른 클랜의 징표가 떨어진다.
// */
// int XSpotDaily::DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, int lvSpot, float multiplyDropNum ) const
// {
// 	return pOutAry->size();
// }
// 
// bool XSpotDaily::sGetRewardDailyToday( XPropWorld::xDaily* pProp
// 																		, int lvLegion
// 																		, XVector<XGAME::xReward>* pOutAry )
// {
// 	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	return sGetRewardDaily( pProp, dowToday, lvLegion, pOutAry );
// }
// 
// bool XSpotDaily::sGetRewardDaily( XPropWorld::xDaily* pProp
// 																, XE::xtDOW dow
// 																, int lvLegion
// 																, XVector<XGAME::xReward>* pOutAry )
// {
// 	int idxDow = ( dow == XE::xDOW_SUNDAY ) ? 6 : dow - 1;
// 	auto& ary = pProp->m_aryDays[idxDow];	// 해당요일의 보상리스트
// 	// 계산
// 	for( auto& reward : ary ) {
// 		// 보상양을 가공한다.
// 		auto lvBase = lvLegion - XSpotDaily::c_lvLegionStart;
// 		if( lvBase < 0 )
// 			lvBase = 0;
// 		const auto v1 = pProp->m_v1;
// 		const auto v2 = pProp->m_v2;
// 		const auto v3 = pProp->m_v3;
// 		int numRes = (int)( v1 + ( lvBase * v2 ) * ( reward.num * v3 ) );
// 		XGAME::xReward rew = reward;
// 		rew.num = numRes;
// 		pOutAry->Add( rew );
// 	}
// 	return true;
// }
// 
// /**
//  @brief 스팟 터치시 호출
// */
// bool XSpotDaily::Update( XSPAcc spAcc )
// {
// 	auto dowToday = XSYSTEM::GetDayOfWeek();
// 	if( dowToday != m_dowToday ) {
// 		// 요일이 바뀜
// 		if( dowToday == XE::xDOW_MONDAY ) {
// 			// 한주가 지남.
// 			++m_cntWeekByFloor;
// 			CreateLegion( spAcc );		// 군단을 다시 업데이트.
// 		}
// 		m_dowToday = dowToday;
// 	}
// 	//m_Day = XTimer2::sGetTime() / 60 / 60 / 24;
// 	return true;
// }
// 
// void XSpotDaily::OnTouch( XSPAcc spAcc )
// {
// 	if( --m_cntTouch == 0 )
// 		GetbitFlag().bUpdated = 0;		// 알림 확인
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

void XSpotCommon::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
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

bool XSpotCommon::IsPrivateRaid() const
{
	auto pProp = static_cast<XPropWorld::xCommon*>( PROP_WORLD->GetpProp( GetidSpot() ) );
	return ( pProp ) ? ( pProp->strType == _T( "private.raid" ) ) : false;
}
