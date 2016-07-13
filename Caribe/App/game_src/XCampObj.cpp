/********************************************************************
	@date:	2015/05/18 17:29
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XCampObj.cpp
	@author:	xuzhu
	
	@brief:	캠페인 일반화
	@note 캠페인정보의 멀티스레드 동기화 방식은 길드 메모리의 내용을 원본으로 한다.
	이제 "stage"블럭을 사용했다면 무조건 고유아이디를 부여해야한다.
	만약 "stage"블럭없이 num_stage같은걸로만 생성되었다면 1번부터 차례대로 고유번호가
	부여된다. 이의경우 DB에 저장된 후 스테이지정보가 바꼈을때 이전에 클리어했던
	상황과 달라질수도 있음을 명심해야한다.
*********************************************************************/
#include "stdafx.h"
#include "XCampObj.h"
#include "XStageObj.h"
#include "XLegion.h"
#include "XAccount.h"
#include "XGlobalConst.h"
#include "XSystem.h"
#include "XCampObjHero.h"
#include "XCampObjHero2.h"
#include "XPropLegionH.h"
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( xCampaign )

#ifdef _CLIENT
int XCampObj::s_idxStage = -1;	// 클라에서 파라메터를 넘기기가 마땅찮아서 궁여지책으로 이렇게....;;;
int XCampObj::s_idxFloor = 0;	// 클라에서 파라메터를 넘기기가 마땅찮아서 궁여지책으로 이렇게....;;;
#endif // _CLIENT

XSPCampObj sCreateCampObj( xtType typeCamp, XPropCamp::xProp* pPropCamp, int grade )
{
	XBREAK( pPropCamp == nullptr );
	switch( typeCamp ) {
	case xCT_NORMAL: {
		//return XSPCampObj( new XCampObj( xCT_NORMAL, pPropCamp, grade ) );
		auto spCampObj = XSPCampObj( new XCampObj( xCT_NORMAL, pPropCamp, grade ) );
		spCampObj->OnCreate();
		return spCampObj;
	} break;
	case xCT_HERO: {
//		return XSPCampObj( new XCampObjHero( pPropCamp ) );
		auto spCampObj = XSPCampObjHero( new XCampObjHero( pPropCamp ) );	// 기존데이터 호환을 위해 남겨둬야함.
		spCampObj->OnCreate();
		return spCampObj;
	} break;
	case xCT_HERO2: {
		auto spCampObj = XSPCampObjHero2( new XCampObjHero2( SafeCast<XPropCamp::xPropHero*>( pPropCamp ) ) );
		spCampObj->OnCreate();
		return spCampObj;
	} break;
	default:
		XBREAK( 1 );
		break;
	}
	return nullptr;
}

/**
 @brief XCampObj전용 creator로 일단 남겨둠.
*/
XSPCampObj XCampObj::sCreateCampObj( XPropCamp::xProp* pPropCamp, int grade )
{
	XBREAK( pPropCamp == nullptr );
//	return XSPCampObj( new XCampObj( xCT_NORMAL, pPropCamp, grade ) );
	auto spCampObj = XSPCampObj( new XCampObj( xCT_NORMAL, pPropCamp, grade ) );
	spCampObj->OnCreate();
	return spCampObj;
}

int XCampObj::sSerialize( XSPCampObj& spCampObj,  XArchive& ar )
{
	if( spCampObj == nullptr ) {
		ar << 0;
		return 1;
	}
	ar << (BYTE)VER_CAMP_SERIALIZE;
	ar << (BYTE)spCampObj->GetType();
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << spCampObj->GetidProp();
	XArchive arCamp;	// 캠페인이 프로퍼티에서 없어질경우를 대비에 이렇게 함. 한꺼번에 건너뛰려고.
	arCamp.SetbForDB( ar.IsForDB() );
	spCampObj->Serialize( arCamp );
	ar << arCamp;
	return 1;
}

XSPCampObj XCampObj::sCreateDeserialize( XArchive& ar )
{
	BYTE b0;
	ar >> b0;	int ver = b0;
	ar >> b0;	auto type = (xtType)b0;
	ar >> b0 >> b0;
	if( ver <= 0 )
		return nullptr;
	ID idProp;
	ar >> idProp;
	XArchive arCamp;
	ar >> arCamp;
	auto pProp = XPropCamp::sGet()->GetpProp( idProp );
	if( pProp == nullptr )	// 캠페인이 사라졌으면 그냥 리턴
		return nullptr;
	if( ver < 19 && (type == xCT_HERO || type == xCT_HERO2) ) {
		// 19버전 미만 영웅캠페인은 캠페인데이터를 다 날림.
		return nullptr;
	} else {
		auto spCampObj = xCampaign::sCreateCampObj( type, pProp, 0 );
		spCampObj->DeSerialize( arCamp, ver );
		return spCampObj;
	}
}


/**
 @brief 군단정보만 묶는다.
*/
int XCampObj::sSerializeLegion( XSPCampObj& spCampObj, XArchive& ar )
{
	if( spCampObj == nullptr ) {
		ar << 0;
		return 1;
	}
	ar << VER_CAMP_SERIALIZE;
	return spCampObj->SerializeLegion( ar );
}

int XCampObj::sDeSerializeLegion( XSPCampObj& spCampObj, XArchive& ar )
{
	int verCamp;
	ar >> verCamp;
	if( verCamp == 0 )
		return 0;
	return spCampObj->DeSerializeLegion( ar, verCamp );
}


int XCampObj::sDeserializeUpdate( XSPCampObj spCampObj, XArchive& ar ) 
{
// 	int verCamp;
// 	arCamp >> verCamp;
// 	if( verCamp == 0 )
// 		return 0;
	BYTE b0;
	ar >> b0;	int verCamp = b0;
	ar >> b0;	auto type = (xtType)b0;
	ar >> b0 >> b0;
	if( verCamp <= 0 )
		return 0;
	ID idProp;
	ar >> idProp;
	XBREAK( spCampObj->GetidProp() != idProp );
	XArchive arCamp;
	ar >> arCamp;
	spCampObj->DeSerialize( arCamp, verCamp );
	return 1;
}

////////////////////////////////////////////////////////////////
XCampObj::XCampObj( xtType typeCamp, XPropCamp::xProp *pProp, int grade )
	: m_Type( typeCamp )
	, m_aryStages(1)
{
//	XLOCK_OBJ;	// 생성자에선 필요없지 않나?
	Init();
	m_pProp = pProp;
	m_Grade = grade;
	XBREAK( grade < 0 );

}

void XCampObj::Destroy()
{
	XLOCK_OBJ;
}

void XCampObj::OnCreate()
{
	auto pProp = m_pProp;
	if( XASSERT( pProp ) ) {	// 프로퍼티가 없는경우 ar >> arCamp식으로 통째로 건너뛰기때문에 이제 이게 널일경우는 없는걸로 함.
		m_keyProp = XPropCamp::sGet()->GetidKey();
		// 스테이지 인스턴스 생성.
		int idxFloor = 0;
		for( auto spStage : pProp->aryStages ) {
			auto spSObj = CreateStageObj( spStage, idxFloor );	// virtual
			m_aryStages[0].push_back( spSObj );
		}
		m_bAutoReset = pProp->bAutoReset;
		m_secResetCycle = pProp->secResetCycle;
	}
}

XSPStageObj XCampObj::CreateStageObj( XSPPropStage spPropStage, int idxFloor )
{
	auto spStageObj = XSPStageObj( new XStageObj( spPropStage ) );
	return spStageObj;
}


void XCampObj::CreateFloor( int idxFloor )
{
	if( GetaryFloor().Size() <= idxFloor ) {
		GetaryFloor().resize( idxFloor + 1 );
	}
	GetaryStages( idxFloor ).clear();
	// 스테이지 인스턴스 생성.
	for( auto spStage : m_pProp->aryStages ) {
		auto spSObj = CreateStageObj( spStage, idxFloor );
		GetaryStages( idxFloor ).push_back( spSObj );
	}
	// 부대를 다시 만듬.
//	CreateLegionAll( lvBase, idxFloor );
}

void XCampObj::AddStage( XSPStageObj spStageObj, int idxFloor )
{
	GetaryStages( idxFloor ).push_back( spStageObj );
}

/**
 @brief numStage수만큼 수동으로 스테이지객체를 생성
*/
void XCampObj::CreateStageObjManual( int numStage, int idxFloor )
{

}


int XCampObj::Serialize( XArchive& ar )
{
	XLOCK_OBJ;
	int sizeOld = ar.size();
	// strIdentifier는 용량때문에 생략하고 idProp으로 대체
// 	ar << VER_CAMP_SERIALIZE;
	if( XBREAK( m_pProp == nullptr ) )
		return 0;
// 	ar << m_pProp->idProp;
	ar << m_snCampaign;
	XBREAK( m_aryStages[0].size() > 0x7f );
	XBREAK( m_idxLastUnlock > 0x7f );
	XBREAK( m_idxLastPlay > 0x7f );
	ar << (char)m_aryStages[0].size();		// 모든층은 스테이지 수가 똑같다고 가정.
	ar << (char)m_idxLastUnlock;
	ar << (char)m_idxLastPlay;
	ar << (char)m_cntTry;
	ar << m_idAccTrying;
	ar << m_strTryer;
	ar << m_timerEnter;
	ar << m_timerOpen;
	XBREAK( m_Grade > 0x7f );
	ar << (char)m_Grade;
//	ar << (char)m_numStars;
	ar << (char)0;
	ar << (char)0;
	const int numFloor = m_aryStages.size();
	ar << (char)numFloor;
	for( auto& aryStages : m_aryStages ) {
		for( auto& spStageObj : aryStages ) {
			int size2 = ar.size();
			XArchive arStage;
			arStage.SetbForDB( ar.IsForDB() );
			spStageObj->Serialize( arStage );
			ar << arStage;			// 만약 스테이지가 사라지거나 해서 통째로 건너뛰려고 이렇게 함.
			int size22 = ar.size() - size2;
	//		TRACE("%d", size22 );
		}
	}
	int size = ar.size() - sizeOld;
	XBREAK( ar.IsForDB() && size >= 8000 );
	return 1;
}

/**
 @brief 
 @note 만약 캠페인이 프로퍼티에서 사라진경우 그 캠페인의 아카이브를 통째로 건너뛰어야하므로
 캠페인마다 arCamp로 한번더 묶어서 ar << arCamp 식으로 저장하자.
*/
int XCampObj::DeSerialize( XArchive& ar, int verCamp )
{
	XLOCK_OBJ;
	XBREAK( verCamp <= 0 );	// 위 설명참조
	XBREAK( m_pProp == nullptr );	// 캠페인이 사라졌으면 상위에서 ar >> arCamp로 통째로 건너뛰기때문에 이런경우는 없어야 함.
// 	if( verCamp < 0 )
// 		m_pProp = nullptr;		// 캠페인이 프로퍼티에서 사라진경우 이렇게 사용함.
	ar >> m_snCampaign;
	int size;
	char c0;
	ar >> c0;	size = c0;
	ar >> c0;	m_idxLastUnlock = c0;
	ar >> c0; m_idxLastPlay = c0;
	ar >> c0; m_cntTry = c0;
	ar >> m_idAccTrying;
	ar >> m_strTryer;
	ar >> m_timerEnter;
	ar >> m_timerOpen;
	ar >> c0;	m_Grade = c0;
	ar >> c0;
	ar >> c0;
	int numFloor = 1;
	if( verCamp >= 20 ) {
		ar >> c0; numFloor = c0;
	} else {
		ar >> c0; 
	}
	if( m_aryStages.Size() != numFloor ) {
		m_aryStages.resize( numFloor );
	}
	for( int f = 0; f < numFloor; ++f ) {
		if( m_aryStages[f].size() != size )
			m_aryStages[f].resize( size );
		for( int i = 0; i < size; ++i ) {
			auto spStageObj = GetspStage( i, f );
			// 이미 메모리가 할당되어 있으면 다시 할당하지 않음.
			if( spStageObj == nullptr ) {
				spStageObj = XSPStageObj( CreateStageObj( XSPPropStage() ) );	// virtual
				if( m_pProp ) {
					m_aryStages[f][i] = spStageObj;
				}
			}
			XArchive arStageObj;
			ar >> arStageObj;
			spStageObj->DeSerialize( GetThis(), i, arStageObj, verCamp );
		}
	}
	return 1;
}

/**
 @brief 각스테이지의 군단정보만 시리얼라이즈 시킨다.
*/
int XCampObj::SerializeLegion( XArchive& ar, int idxFloor )
{
	XLOCK_OBJ;
	ar << (char)GetaryStages( idxFloor ).size();
	ar << (char)idxFloor;
	ar << (char)0;
	ar << (char)0;
	for( auto spStageObj : m_aryStages[ idxFloor ] ) {
		auto spLegion = spStageObj->GetspLegion();
		if( spLegion != nullptr )
			spLegion->SerializeFull( ar );
		else
			ar << 0;
	}
	return 1;
}

/**
 @brief ar에서 군단정보만 꺼내 업데이트 한다.
*/
int XCampObj::DeSerializeLegion( XArchive& ar, int verCamp )
{
	XLOCK_OBJ;
	if( XBREAK( verCamp <= 0 ) )
		return 0;
	int size;
	int idxFloor = 0;
	if( verCamp >= 20 ) {
		char c0;
		ar >> c0;		size = c0;
		ar >> c0;		idxFloor = c0;
		ar >> c0 >> c0;
	} else {
		ar >> size;
	}
	XBREAK( size > 30 );		// 이상값 방지
	XBREAK( size != m_aryStages[ idxFloor ].size() );
	for( int i = 0; i < size; ++i ) {
		auto spLegion = LegionPtr( XLegion::sCreateDeserializeFull( ar ) );
		auto spStageObj = GetspStage( i, idxFloor );
		spStageObj->DestroyLegion();
		spStageObj->SetspLegion( spLegion );
	}
	return 1;
}

/**
 @brief idxStage를 깼다.
*/
bool XCampObj::ClearStage( int idxStage, int idxFloor ) 
{
	XLOCK_OBJ;
	auto spStageObj = GetspStage( idxStage, idxFloor );
	if( spStageObj ) {
		spStageObj->AddnumClear();
		if( XBREAK( spStageObj->GetnumClear() < 0 ) )
			spStageObj->ClearnumClear();
		spStageObj->DestroyLegion();
	}
//	if( m_idxPlaying == idxStage )
	XBREAK( idxStage != m_idxPlaying );
	m_idxPlaying = -1;
	if( idxStage == m_idxLastUnlock )
		++m_idxLastUnlock;
	return IsEndStage( idxFloor );

}

LegionPtr XCampObj::GetspLegionLastUnlockStage( int idxFloor )  
{
	XLOCK_OBJ;
	if( GetidxLastUnlock() >= (int)m_aryStages[ idxFloor ].size() )
		return LegionPtr();
	return GetaryStages( idxFloor )[ m_idxLastUnlock ]->GetspLegion();
}

LegionPtr XCampObj::GetspLegionLastPlayStage( int idxFloor ) 
{
	XLOCK_OBJ;
	if( m_idxLastPlay < 0 )
		return nullptr;
	return GetaryStages( idxFloor)[ m_idxLastPlay ]->GetspLegion();
}

void XCampObj::DestroyLegion( int idxStage, int idxFloor )
{
	XLOCK_OBJ;
	auto spStage = GetaryStages( idxFloor )[ idxStage ];
	if( spStage != nullptr )
		spStage->DestroyLegion();
}

/**
 @brief 모든 스테이지에 군단을 생성한다.
*/
void XCampObj::CreateLegionAll( int lvBase, int idxFloor )
{
	XLOCK_OBJ;
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		spStageObj->CreateLegion( GetThis(), lvBase, idxFloor );
	}
}

/**
 @brief 모든 스테이지의 외부파라메터에 같은값을 넣는다.
*/
void XCampObj::SetParamLegionAllStage( XGAME::xLegionParam& paramLegion, int idxFloor )
{
	XLOCK_OBJ;
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		// 모든스테이지를 같은 파라메터로 채운다.
		spStageObj->SetparamLegion( paramLegion );
	}
}

/**
 @brief 군대가 안만들어져있으면 만듬.
 메달캠페인의 경우 정상적으로 생성되었다면 없을수는 없다.
 DB초기화후엔 필요없는 함수.
*/
void XCampObj::CreateLegionIfEmpty( int lvBase, int idxFloor )
{
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		if( spStageObj->GetspLegion() == nullptr )
			spStageObj->CreateLegion( GetThis(), lvBase, idxFloor );
	}
}


/**
 @brief spStageObj가 현재 도전가능한 스테이지 인가.
*/
XGAME::xtError XCampObj::IsAbleTry( const XSPStageObj spStageObj, int idxFloor )
{
	XLOCK_OBJ;
	if( spStageObj->GetidxStage() > m_idxLastUnlock )
		return XGAME::xE_LOCK;
	// 캠페인 전체 횟수제한이 걸려있다면 횟수를 검사한다.
	auto pProp = GetpProp();
	if( pProp->numTry > 0 ) {
		// 이미 횟수를 다 썼다면 도전못함.
		if( m_cntTry >= pProp->numTry )
			return XGAME::xE_NOT_ENOUGH_NUM_TRY;
	}
	if( !spStageObj->IsAbleTry() )
		return XGAME::xE_NOT_ENOUGH_NUM_TRY;
	// 이미 다 클리어했으면 더이상 도전 못함.
	if( spStageObj->IsAllNumClear() )
		return XGAME::xE_NOT_ENOUGH_NUM_CLEAR;
	return XGAME::xE_OK;
}

/**
 @brief 전투 남은시간을 얻는다.
*/
xSec XCampObj::GetsecRemainTry() 
{
	XLOCK_OBJ;
	if( m_idAccTrying == 0 )
		return 0;
	if( m_timerEnter.IsOff() )
		return 0;
	auto secPass = m_timerEnter.GetsecPass();
	if( XGC->m_secMaxBattleSession <= secPass )
		return 0;
	return XGC->m_secMaxBattleSession - secPass;
}

/**
 @brief   캠페인 닫히기(재오픈)까지 남은 시간
*/
xSec XCampObj::GetsecRemainClose()
{
	XLOCK_OBJ;
	if( m_timerOpen.IsOff() )
		return 0;		// closed
	auto secPass = m_timerOpen.GetsecPass();
	xSec secLimit = xHOUR_TO_SEC(12);
	if( secPass >= secLimit )
		return 0;
	return secLimit - secPass;
}           

/**
 @brief 캠페인 리셋까지 남은 시간.
*/
xSec XCampObj::GetsecRemainReset()
{
	XLOCK_OBJ;
	if( m_timerOpen.IsOff() )
		return 0;		// closed
	auto secPass = m_timerOpen.GetsecPass();
	xSec secLimit = GetpProp()->secResetCycle;
	if( XBREAK(secLimit == 0) )
		return 0;
	if( secPass >= secLimit )
		return 0;
	return secLimit - secPass;
}

/**
 @brief 작전 시작
*/
void XCampObj::DoOpenCampaign( int lvBase, int idxFloor ) 
{
	XLOCK_OBJ;
	// 이미 오픈되어있고 시간도 남았다면 다시 작동시키지 않음.
	if( m_timerOpen.IsOn() && GetsecRemainClose() > 0 )
		return;
	InitCamp( lvBase, idxFloor );
	for( auto spStageObj : m_aryStages[ idxFloor ] ) {
		spStageObj->InitStage();
	}
	
	m_timerOpen.DoStart();

}

void XCampObj::InitCamp( int lvBase, int idxFloor ) 
{
	m_idxLastPlay = -1;
	m_idxLastUnlock = 0;
	m_idAccTrying = 0;
	m_strTryer.clear();
	m_timerEnter.Off();
	m_timerOpen.Off();
	m_cntTry = 0;
	m_bSendReward = false;
	m_idxPlaying = -1;
	m_aryStages[ idxFloor ].clear();
	CreateFloor( idxFloor );
	// 부대를 다시 만듬.
	CreateLegionAll( lvBase, idxFloor );
}

// 스팟을 터치하면 캠페인 업데이트를 한다.
// 	.타이머를 확인해서 꺼져있으면 켜주고, 현재시간 h/m/s를 구해서 타이머에 더해준다.
// 	.타이머가 켜져있으면 24시간이 지났는지 확인한다.
// 	.시간이 지났으면 캠페인을 초기화 한다.
/**
 @brief 캠페인을 업데이트 한다.
 // 길드레이드는 이것을 사용하지 않음.(자동화 시킬것)
*/
#ifdef _GAME_SERVER
// 업데이트는 게임서버에서 모두 다 하고 다른서버로 넘김.
void XCampObj::Update( XSPAcc spAcc )
{
	XBREAK( spAcc == nullptr );
	int lvBase = spAcc->GetLevel();
	if( m_timerOpen.IsOff() ) {
		if( m_bAutoReset && m_secResetCycle ) {
			// 타이머가 꺼져있으면 새로 켜준다.
			m_timerOpen.DoStart();
			// 오늘중 현재 지나간 초를 계산해 타이머에 더한다. 이것은 0시를 기준으로 자동리셋되게 하기 위함이다.
			int secToday = XSYSTEM::GetsecToday();
			m_timerOpen.AddSec( secToday );
		}
	}
	if( m_secResetCycle ) {
		XBREAK( m_timerOpen.IsOff() );
		if( m_timerOpen.IsOver( m_secResetCycle ) ) {
			ResetCampAllFloor( spAcc->GetLevel() );
// 			InitCamp( spAcc->GetLevel() );
			// 타이머가 꺼져있으면 새로 켜준다.
			m_timerOpen.DoStart();
			// 오늘중 현재 지나간 초를 계산해 타이머에 더한다. 이것은 0시를 기준으로 자동리셋되게 하기 위함이다.
			int secToday = XSYSTEM::GetsecToday();
			m_timerOpen.AddSec( secToday );
		}
	}
	// 일반적인 상황은 아니지만 부대가 혹시 없을때는 다시 생성시킴.
	for( int f = 0; f < m_aryStages.Size(); ++f ) {
		if( m_aryStages[f].size() == 0 ) {
			CreateFloor( f );
			// 부대를 다시 만듬.
			CreateLegionAll( lvBase, f );
		}
		CreateLegionIfEmpty( lvBase, f );
		for( auto spStageObj : m_aryStages[f] )
			spStageObj->Update( GetThis(), f );
	}
}
#endif // _GAME_SERVER

void XCampObj::ResetCamp( int lvAcc, int idxFloor )
{
	InitCamp( lvAcc, idxFloor );
}

void XCampObj::ResetCampAllFloor( int lvAcc )
{
	int idxFloor = 0;
	for( auto& aryStage : m_aryStages ) {
		ResetCamp( lvAcc, idxFloor );
		++idxFloor;
	}
}

bool XCampObj::FindTryer( ID idAcc, int idxFloor )
{
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		if( spStageObj->FindTryer( idAcc ) )
			return true;
	}
	return false;
}

/**
 @brief 총10스테이지면 1~9까지의 스테이지당 보상
*/
float XCampObj::GetRewardPerStage( int lvAcc, int idxFloor )
{
	int maxStages = GetNumStages( idxFloor );
	auto point = (m_pProp->reward * 0.5f) / (maxStages - 1);
	auto pointByLevel = (lvAcc / 5.f) * point;
	return pointByLevel;
}

/**
 @brief 마지막스테이지의 보상포인트
*/
int XCampObj::GetRewardPointLastStage( int lvAcc, int idxFloor )
{
	auto point = (int)(m_pProp->reward * 0.5f);
	auto pointByLevel = ( lvAcc / 5.f ) * point;
	return (int)pointByLevel;
}

/**
 @brief 계정레벨별 클리어한만큼의 총 보상포인트.
*/
int XCampObj::GetRewardPointByClear( int lvAcc, int idxFloor )
{
	XBREAK( lvAcc == 0 );
	XBREAK( lvAcc > XGAME::GetLevelMaxAccount() );
	int reward = 0;
	int maxStages = GetNumStages( idxFloor );
	int numClearStages = m_idxLastUnlock;
	float rate = 0.f;
	if( numClearStages == maxStages ) {
		// all clear
		reward = m_pProp->reward;		// 보상 100%를 다준다.
	} else 
	if( numClearStages < maxStages ) {
		// 일단 50%를 쪼개고 그 반을 다시 최대스테이지-1로 나눈다.
		// 10스테이지라면 1~9스테이지의 스테이지당 점수
		float stagePerPoint = GetRewardPerStage( lvAcc, idxFloor );
		reward = (int)stagePerPoint * numClearStages;
	} else {
		XBREAK( numClearStages > maxStages );
	}
	return reward;
}
/**
 @brief 모든 스테이지의 보상을 다 받았는가
*/
bool XCampObj::IsRecvAllReward( int idxFloor )
{
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		// 하나라도 보상안받은게 있으면 false
		if( !spStageObj->GetbRecvReward() )
			return false;
	}
	return true;
}
/**
 @brief idxStage로 진입한다.
*/
bool XCampObj::DoEnterStage( int idxStage, int idxFloor )
{
	auto spStageObj = GetspStage( idxStage, idxFloor );
	if( XASSERT(spStageObj) )
		if( IsAbleTry( spStageObj, idxFloor ) != XGAME::xE_OK )
			return false;
// 		if( !spStageObj->IsAbleTry( m_idxLastUnlock ) )
// 			return false;
	m_idxPlaying = idxStage;
//	++m_cntTry;		// 진입 시도 횟수 증가.
	return true;
}

void XCampObj::_DoClear( int idxFloor )
{
#ifdef _CHEAT
	m_idxLastUnlock = GetNumStages() - 1;
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		spStageObj->_ClearTryer();
	}
#endif // _CHEAT
}
int XCampObj::GetnumStar( int idxFloor )
{
	int nums = 0;
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		nums += spStageObj->GetnumStar();
	}
	return nums;
}

XSPPropStage XCampObj::GetspStageProp( int idxStage, int idxFloor ) 
{
	XLOCK_OBJ;
	return GetaryStages( idxFloor )[ idxStage ]->GetspPropStage();
}

XSPStageObj XCampObj::GetspStageObjWithidProp( ID idPropStage, int idxFloor )
{
	for( auto spStageObj : GetaryStages( idxFloor ) ) {
		if( spStageObj 
			&& spStageObj->GetidProp() == idPropStage )
			return spStageObj;
	}
	return nullptr;
}

int XCampObj::GetlvLegion( XPropCamp::xStage* pPropStage, int lvBase, int idxFloor )
{
	int lvLegion = pPropStage->m_spxLegion->lvLegion;
	if( lvLegion == 0 ) {
		// 프롭스테이지에 군단레벨이 지정되어있지않다면 기준레벨을 중심으로 adjLevelLgion으로 보정해서 쓴다.
		if( XBREAK( pPropStage->m_spxLegion->adjLvLegion == 0x7f ) ) {	// 군단레벨이 지정되어있지않은데 보정레벨도 없으면 에러
			return lvBase;
		}
		XBREAK( lvBase == 0 );
		lvLegion = lvBase + pPropStage->m_spxLegion->adjLvLegion;
	}
	return lvLegion;
}

int XCampObj::GetlvLimit( int idxStage, int idxFloor )
{
	auto spPropStage = GetspStageProp( idxStage, idxFloor );
	return ((spPropStage)? spPropStage->levelLimit : 0);
}

/**
 @brief 캠페인 군단생성시 idxSquad부대의 영웅이 될 아이디를 얻는다.
 확장성을 고려해서 virtual로 뺌
*/
ID XCampObj::GetidHeroByCreateSquad( int idxSquad, const XGAME::xSquad* pSquadProp, const XPropCamp::xStage* pPropStage )
{
	return pSquadProp->idHero;
}

XGAME::xtUnit XCampObj::GetUnitWhenCreateSquad( int idxSquad
																							, XGAME::xtAttack typeAtk
																							, const XGAME::xSquad* pSquadProp
																							, const XPropCamp::xStage* pPropStage
																							, int idxFloor )
{
	return pSquadProp->unit;
}

XE_NAMESPACE_END;   // namespace xCampiagn







