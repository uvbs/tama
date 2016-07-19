#include "stdafx.h"
#include "XSpotDaily.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
#include "XGame.h"
#endif
#include "XLegion.h"
#include "XWorld.h"
#include "XSystem.h"
#include "XGlobalConst.h"
// 
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
const int XSpotDaily::c_maxFloor = 9;			// 최대 단계수
const int XSpotDaily::c_lvLegionStart = 9;		// 시작 군단레벨

void XSpotDaily::OnCreateNewOnServer( XSPAcc spAcc )
{
// 	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	m_dowToday = dowToday;
// 	int hour, min, sec;
// 	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
	// 오늘0시 기준으로 현재시간은 총 몇초인가를 계산한다.
//	int secTotal = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
// 	// 오늘 요일의 이벤트 스팟을 활성화 시킨다.
// 	spAcc->GetpWorld()->SetActiveDailySpotToRandom( dowToday, secTotal, spAcc );
}

void XSpotDaily::Serialize( XArchive& ar ) 
{
	XBREAK( m_idxFloor > 0x7f );
	XSpot::Serialize( ar );
	ar << (BYTE)GetpProp()->idSpot;
	ar << (char)m_idxFloor; // m_Type;
	ar << (BYTE)m_numEnter;
	ar << (BYTE)m_dowToday;

	ar << (char)m_cntWeekByFloor;
	XASSERT( m_aryDay.Size() == 7 );
	for( auto& day : m_aryDay ) {
		ar << (char)day.m_numStar;
	}
//	m_timerCreate.Serialize( ar );
	m_timerCreate2.Serialize( ar );
}
BOOL XSpotDaily::DeSerialize( XArchive& ar, DWORD verWorld ) 
{
	XSpot::DeSerialize( ar, verWorld );
	ID idProp;
	BYTE b0;
	char c0;
	ar >> b0;	idProp = b0;
	if( verWorld >= 30 ) {
		ar >> c0;	m_idxFloor = c0; //m_Type = (XGAME::xtDailySpot)b0;
	} else {
		ar >> c0;	m_idxFloor = 0;	// 이전버전은 읽고 버림
	}
	ar >> b0;	m_numEnter = b0;
	ar >> b0;	m_dowToday = (XE::xtDOW)b0;
	if( verWorld >= 30 ) {
		ar >> c0;	m_cntWeekByFloor = c0;
		XASSERT( m_aryDay.Size() == 7 );
		for( auto& day : m_aryDay ) {
			ar >> c0;		day.m_numStar = c0;
		}
	}
	if( verWorld >= 31 ) {
		m_timerCreate2.DeSerialize( ar );
	} else {
		CTimer timerDummy;
		timerDummy.DeSerialize( ar );
	}
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
//	m_Type = XSpotDaily::sGetDowToType( dow );
	m_numEnter = 0;
	SetstrName( XTEXT(2228) );	// 요일 전장

// 	m_timerCreate.Set( (float)( 24 * 60 * 60 ) );		// 24시간을 세팅
// 	m_timerCreate.SetPassSec( (float)secPass );	// 흘러간 시간을 더함.
	m_timerCreate2.Set( 24 * 60 * 60 );		// 24시간을 기다리도록 세팅
	m_timerCreate2.AddSec( secPass );
	DestroyLegion();
//	ResetLevel( spAccount );
//	Update( spAccount );
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

int XSpotDaily::GetRemainEnter() const 
{
	return _XGC->m_numEnterDaily - m_numEnter;
}
void XSpotDaily::AddnumEnter() 
{
	if( m_numEnter < _XGC->m_numEnterDaily )
		++m_numEnter;
}

/**
 @brief 이 스팟이 공격이 가능한 상태인지 검사한다.
 pOut을 넘겨주면 구체적인 에러코드를 담아준다.
*/
bool XSpotDaily::IsAttackable( XSPAcc spAcc, xtError *pOut )
{
	xtError errCode = xERR_OK;
	do {
		if( m_timerCreate2.IsOver() ) {
			errCode = xERR_TIMEOUT;
			break;
		}
		if( m_numEnter >= _XGC->m_numEnterDaily ) {
			errCode = xERR_OVER_ENTER_NUM;
			break;
		}
	} while (0);
	if( pOut )
		*pOut = errCode;
	return errCode == xERR_OK;
}

void XSpotDaily::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	XSpot::OnAfterBattle( spAccWin, idAccLose, bWin, numStar, bRetreat );
	GetbitFlag().bUpdated = 0;		// 전투를 한번하고 나오면 다시 클리어 된다.
	if( bWin ) {	// 이길때만 깎이는걸로 정책전환.
		AddnumEnter();
//		const auto day = XSYSTEM::GetDayOfWeek();
		const auto day = m_dowToday;		// 전투전엔 일요일이었는데 전투후 월욜로 바뀔수도 있어서.
		XBREAK( m_aryDay.size() == 0 );
		if( numStar > m_aryDay[day].m_numStar ) {
			m_aryDay[day].m_numStar = numStar;
			bool bClear = true;
			for( auto& day : m_aryDay ) {
				if( day.m_numStar != 3 ) {
					bClear = false;
					break;
				}
			}
			// 모든 요일을 클리어 함.
			if( bClear ) {
				++m_idxFloor;
				if( m_idxFloor >= c_maxFloor ) {
					m_idxFloor = c_maxFloor - 1;
					// 더이상 단계가 없음.
				} else {
					m_cntWeekByFloor = 0;
					for( auto& day : m_aryDay ) {
						day.Clear();
					}
					GetbitFlag().bUpdated = 1;
					m_cntTouch = 2;
					DestroyLegion();
					Update( spAccWin );
				}
			}
		}
	}
}

XGAME::xtUnit XSpotDaily::GetUnitByDow( XE::xtDOW dow ) const
{
	const auto size = GetSizeUnitByFloor();
	XGAME::xtAttack atkType = XGAME::xAT_NONE;
	switch( dow ) {
	case XE::xDOW_MONDAY:
	case XE::xDOW_THURSDAY:
		atkType = XGAME::xAT_TANKER;
		break;
	case XE::xDOW_TUESDAY:
	case XE::xDOW_FRIDAY:
		atkType = XGAME::xAT_RANGE;
		break;
	case XE::xDOW_WEDNESDAY:
	case XE::xDOW_SATURDAY:
		atkType = XGAME::xAT_SPEED;
		break;
	case XE::xDOW_SUNDAY: {
		const int maxAtkType = XGAME::xAT_MAX - 1;
		atkType = ( XGAME::xtAttack )( ( m_cntWeekByFloor % maxAtkType ) + 1 );
	} break;
	default:
		break;
	}
	const auto unit = XGAME::GetUnitBySizeAndAtkType( size, atkType );
	return unit;
}

/**
 @brief 현재 단계의 dow요일의 군단레벨 계산.
 lv9 ~ lv50
 총 9단계
 각 단계는 +6렙씩 증가
 각 요일은 월~일까지 0~6의 상대레벨
*/
int XSpotDaily::GetlvLegionDow( XE::xtDOW dow ) const
{
 	const int lvStart = c_lvLegionStart;
 	const int lvMax = XGAME::MAX_ACC_LEVEL;
 	const int maxFloor = XSpotDaily::c_maxFloor;			// 총 단계수
 	const int lvArea = lvMax - lvStart;		// 총 군단레벨 범위
 	const float lvAddPerFloor = ROUND_FLOAT(lvArea / (float)maxFloor, 0);		// 층당 증가 레벨(반올림)
 	const int lvStartCurr = (int)(lvStart + m_idxFloor * lvAddPerFloor);
	const float lvPerDay = ( lvAddPerFloor / 7.f);
	return (int)(lvStartCurr + XE::GetidxDow( dow ) * lvPerDay);
}

/**
 @brief 현재 단계의 시작레벨을 구한다.
*/
int XSpotDaily::GetlvLegionCurrFloor() const 
{
	const int lvStart = c_lvLegionStart;
	const int lvMax = XGAME::MAX_ACC_LEVEL;
	const int maxFloor = XSpotDaily::c_maxFloor;			// 총 단계수
	const int lvArea = lvMax - lvStart;		// 총 군단레벨 범위
	const float lvAddPerFloor = ROUND_FLOAT( lvArea / (float)maxFloor, 0 );		// 층당 증가 레벨(반올림)
	const int lvStartCurr = (int)( lvStart + m_idxFloor * lvAddPerFloor );
	return lvStartCurr;
}

void XSpotDaily::_SetdowToday( XSPAcc spAcc, XE::xtDOW dowToday ) 
{
#if _DEV_LEVEL <= DLV_DEV_PERSONAL
	m_dowToday = dowToday;
	DestroyLegion();
	CreateLegion( spAcc );
#endif
}

/**
 스팟에 Daily군대를 생성시킨다.
*/
void XSpotDaily::CreateLegion( XSPAcc spAccount )
{
	if( GetstrName().empty() )
		ResetName( spAccount );
//	int lvLegion = GetLevel();
	const int lvLegion = GetlvLegionDow( GetdowToday() );
	SetLevel( lvLegion );
	// 렙이 낮아 소형유닛만 보유하고 있을때 적이 중형급만 나온다면 질수밖에 없다.
	// 그래서 일단 크기대신에 병과별로 나오게 한다.
	XGAME::xLegionParam infoLegion;
	infoLegion.unit = GetUnitByToday();
	auto pLegion = XLegion::sCreateLegionForNPC( lvLegion,
																							0,
																							infoLegion );
#if _DEV_LEVEL <= DLV_LOCAL
	XBREAK( GetspLegion() != nullptr );
#endif
	SetpLegion( pLegion );
	// 요일 보상(reward를 vector로 받아야 할거 같다.
	ClearLootRes();
	const int lvFloor = GetlvLegionCurrFloor();
	XVector<XGAME::xReward> aryRes;		// 오늘의 보상 리스트
	sGetRewardDaily( GetpProp(), m_dowToday, lvFloor, &aryRes );
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

bool XSpotDaily::sGetRewardDailyToday( XPropWorld::xDaily* pProp
																		, int lvLegion
																		, XVector<XGAME::xReward>* pOutAry )
{
	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
	return sGetRewardDaily( pProp, dowToday, lvLegion, pOutAry );
}

bool XSpotDaily::sGetRewardDaily( XPropWorld::xDaily* pProp
																, XE::xtDOW dow
																, int lvLegion
																, XVector<XGAME::xReward>* pOutAry )
{
	int idxDow = ( dow == XE::xDOW_SUNDAY ) ? 6 : dow - 1;
	auto& ary = pProp->m_aryDays[idxDow];	// 해당요일의 보상리스트
	// 계산
	for( auto& reward : ary ) {
		// 보상양을 가공한다.
		auto lvBase = lvLegion - XSpotDaily::c_lvLegionStart;
		if( lvBase < 0 )
			lvBase = 0;
		const auto v1 = pProp->m_v1;
		const auto v2 = pProp->m_v2;
		const auto v3 = pProp->m_v3;
		int numRes = (int)( v1 + ( lvBase * v2 ) * ( reward.num * v3 ) );
		XGAME::xReward rew = reward;
		rew.num = numRes;
		pOutAry->Add( rew );
	}
	return true;
}

/**
 @brief 스팟 터치시 호출
*/
bool XSpotDaily::Update( XSPAcc spAcc )
{
	auto dowToday = XSYSTEM::GetDayOfWeek();
	if( dowToday != m_dowToday ) {
		// 요일이 바뀜
		m_dowToday = dowToday;
		if( dowToday == XE::xDOW_MONDAY ) {
			// 한주가 지남.
			++m_cntWeekByFloor;
			DestroyLegion();		// 군단을 삭제해서 스팟 터치하면 다시 만들도록 한다.
//			CreateLegion( spAcc );		// 군단을 다시 업데이트.
		}
	}
	int hour, min, sec;
	XSYSTEM::GetHourMinSec( &hour, &min, &sec );
	const int secPass = ( hour * 60 * 60 ) + ( min * 60 ) + sec;
	// 타이머가 없거나 24시간이 오버됐으면 스팟을 다시 리셋함.
	if( m_timerCreate2.IsOff() || ( m_timerCreate2.IsOver() ) ) {
		SetSpot( dowToday, secPass, spAcc );
	}
	return true;
}

void XSpotDaily::OnTouch( XSPAcc spAcc )
{
	if( --m_cntTouch <= 0 )
		GetbitFlag().bUpdated = 0;		// 알림 확인
}


