#include "stdafx.h"
#include "XSpotCastle.h"
#include "XArchive.h"
#include "XAccount.h"
#ifdef _CLIENT
	#include "XGame.h"
	#include "XGameWnd.h"
	#include "XPropUser.h"
  #include "XSceneWorld.h"
  #include "XTemp.h"
#endif
#ifdef _GAME_SERVER
	#include "XGameUser.h"
	#include "XPropUser.h"
#endif
#include "XLegion.h"
#include "XPropHero.h"
#include "XWorld.h"
#include "XPropItem.h"
//#include "XCampObj.h"
#include "XStageObj.h"
#include "XGuild.h"
#include "server/XGuildMgr.h"
#include "XFramework/XFacebook.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//using namespace xCampaign;
using namespace XGAME;

xSec XSpotCastle::s_secResetHeroSoul = 0;
//////////////////////////////////////////////////////////////////////////
XSpotCastle::XSpotCastle( XWorld *pWorld, XPropWorld::xCASTLE* pProp, XDelegateSpot *pDelegate/*=nullptr*/ ) 
	: XSpot( pProp, XGAME::xSPOT_CASTLE, pWorld, pDelegate )
	, m_aryLocalStorage( pProp->m_aryProduce.size() )
{ 
	XBREAK( pProp->m_aryProduce.size() == 0 );
	Init(); 
	SetMaxLocalStorage();
}

void XSpotCastle::OnCreateNewOnServer( XSPAcc spAcc )
{
	XSpot::OnCreateNewOnServer( spAcc );
	// 최초엔 ? 상태로 만든다.
	SetPlayerOwner( 0, nullptr, TRUE );
		
}

void XSpotCastle::SetMaxLocalStorage()
{
	if( GetpProp() ) {
		int size = GetpProp()->m_aryProduce.size();
		if( m_aryLocalStorage.size() != size ) {
			m_aryLocalStorage.resize( size );
		}
		const float min = 60.f * 8.f ;// const float min = 30.f ;
		auto& aryProduce = GetpProp()->m_aryProduce;
		for( int i = 0; i < size; ++i ) {
			m_aryLocalStorage[i].m_Type = aryProduce[i].type;
			m_aryLocalStorage[i].maxSize = aryProduce[i].num * min;
		}
	}
// 	m_LocalStorage.maxSize = GetProducePerMin() * 60;
}
void XSpotCastle::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << m_idOwner;
	ar << GetstrName();
	ar << m_strcFbUserId;
	m_timerCalc.Serialize( ar );
	m_timerOwn.Serialize( ar );
	SerializeLocalStorage( ar );
	ar << m_powerUpperLimit;
	ar << m_idHeroDrop;
	if( ar.IsForDB() )
		ar << _tstring();
	else
		ar << m_strHello;
}
BOOL XSpotCastle::DeSerialize( XArchive& ar, DWORD ver ) 
{
	XSpot::DeSerialize( ar, ver );
	// null이 나올수도 있으나 그냥 리턴하지말고 남은 시리얼라이즈를 모두 끝내야함.
	// 스팟이 필요에의해 삭제될수 있기때문.
	ar >> m_idOwner;
	_tstring strName;
	ar >> strName;
	SetstrName( strName.c_str() );
	if( ver >= 27 ) {
		_tstring strTemp;
		ar >> strTemp;	m_strcFbUserId = SZ2C( strTemp );
	}
	int _verTimer = 1;
	if( ver < 22 )
		_verTimer = 0;
	m_timerCalc.DeSerialize( ar, _verTimer );
#ifdef _CLIENT
	// 클라에선 주기적 타이머로 쓴다.
	m_timerCalc.Set( XGAME::SEC_REGEN );
#endif
	m_timerOwn.DeSerialize( ar, _verTimer );
	DeSerializeLocalStorage( ar, ver );
	if( ver >= 22 ) {
		ar >> m_powerUpperLimit;
		if( m_powerUpperLimit < 1000 )
			m_powerUpperLimit = 0x80000000;
		if( m_powerUpperLimit == 0x7fffffff )
			m_powerUpperLimit = 0x80000000;
		if( ver >= 26 ) {
			ar >> m_idHeroDrop;
		}
	}
	if( ver >= 28 )
		ar >> m_strHello;
#ifdef _GAME_SERVER
	if( !IsActive() && GettimerSpawn().IsOff() ) {
#ifdef _DEV
		CONSOLE("유저가 없는데 타이머가 꺼져있어 다시 켬.");
#endif // _DEV
		ResetSpawn();
	}
#endif // _GAME_SERVER
	return TRUE;
}

void XSpotCastle::SerializeLocalStorage( XArchive& ar )
{
	ar << m_aryLocalStorage;
}

void XSpotCastle::DeSerializeLocalStorage( XArchive& ar, int verWorld )
{
	ar >> m_aryLocalStorage;
	SetMaxLocalStorage();	// 임시
	// 검증
	for( auto& res : m_aryLocalStorage ) {
		XBREAK( res.maxSize == 0 );
	}
// 	if( GetpProp() && m_LocalStorage.maxSize == 0 )
// 		SetMaxLocalStorage();
}
// 
/**
 스팟을 유저(플레이어 자신포함)의 소유로 만든다.
 idAcc가 0이면 NPC가 점령중
*/
void XSpotCastle::SetPlayerOwner( ID idAcc, LPCTSTR szName, BOOL isRespawn ) 
{
	// 이미 소유하고 있던 자원지면 다시 처리하지 않음.
	if( m_idOwner == idAcc && idAcc != 0 )
		return;
	m_idOwner = idAcc;
	SetPower( 0 );
	if( XE::IsHave(szName) )
		SetstrName( szName );
	if( idAcc )
		SetpLegion( nullptr );
	m_strHello.clear();
	m_strcFbUserId.clear();
#ifdef _CLIENT
	// 리젠 타이머를 세팅함.
	m_timerCalc.Set( XGAME::SEC_REGEN );
#else
	if( m_idOwner ) {
		// 정산타이머를 작동시킴
		GetTimerCalc().Start();
		// 소유타이머를 작동시킴
		GetTimerOwn().Start();
		if( isRespawn ) {
			ResetSpawn();
		} else
			GettimerSpawnMutable().Off();
	} else
		GettimerSpawnMutable().Off();
#endif
}

void XSpotCastle::ResetSpawn()
{
	int numSpot = GetNumSpots();
	XBREAK( numSpot < 0 || numSpot > 500 );
	const float base = (float)XGAME::SEC_REGEN_CASTLE;
	float sec = base + (numSpot * numSpot);
	sec *= 1.5f;
//	sec += xRandomF( sec * 0.1f );
// 	const float base = (float)XGAME::SEC_REGEN_CASTLE;
// 	float ns = ( numSpot*0.333f ) * ( numSpot*0.333f );
// 	float sec = 50 + ns * base;
// 	sec += xRandomF( sec * 0.1f );
	GettimerSpawnMutable().Set( sec );
}

/**
 @brief 스팟데이타를 초기화상태로 복구 시킨다.
*/
void XSpotCastle::Initialize( XSPAcc spAcc )
{
	SetPlayerOwner( spAcc->GetidAccount(), spAcc->GetstrName(), TRUE );
}

void XSpotCastle::CalculateResourceByPassTime( ID idAccCaller, xSec secPassTime, xSec secAdjustOffline ) 
{
	float min = ( secPassTime - secAdjustOffline ) / 60.f;
	if( min < 0 )
		min = 0;
	for( auto& res : GetpProp()->m_aryProduce ) {
		float producePerSec = res.num / 60.f;		// 초당생산량
		float amount = producePerSec * secPassTime;
		// 뺏긴상태의 성이라면 10%만 적용.
		if( idAccCaller != m_idOwner )
			amount *= 0.1f;		// 
		DoStackToLocal( amount, true );
	}
}

/**
 @brief 정산타이머에 의한 자원정산.
*/
void XSpotCastle::CalculateResource( ID idAcc, xSec secAdjustOffline ) 
{
	CalculateResourceByPassTime( idAcc, (xSec)m_timerCalc.GetsecPassTime(), secAdjustOffline );
	// 정산 타이머는 다시 리셋
	ResetTimerCalc();
}
/**
 지역창고에 있던 자원을 중앙창고로 옮기고 지역창고는 클리어 시킨다.
*/
void XSpotCastle::MoveResourceLocalToMain( XSPAcc spAccount, 
				XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry/*=nullptr*/ )
{
	XGAME::xRES_NUM resNum;
	for( auto& res : m_aryLocalStorage ) {
		spAccount->AddResource( res.m_Type, (int)res.numCurr );
		resNum.type = res.m_Type;
		resNum.num = res.numCurr;
		pOutAry->Add( resNum );
		// 옮기고 난후는 클리어
		res.numCurr = 0;
	}
// 	XGAME::xtResource type = GetProduceResource();
// 	spAccount->AddResource( type, (int)m_LocalStorage.numCurr );
// 	// 얼마나 옮겼는지를 알아야 하므로 어레이에 정보를 담음.
// 	resNum.type = type;
// 	resNum.num = m_LocalStorage.numCurr;
// 	pOutAry->Add( resNum );
//	m_LocalStorage.Clear();
}

void XSpotCastle::Process( float dt ) 
{
	XSpot::Process( dt );
	// 소유 타이머의 델리게이트
#ifdef _CLIENT
// 	if( !m_strcFbUserId.empty() && m_imgProfile.m_pImg == nullptr ) {
// 		// fbid가 있는데 프로필이미지가 아직 없으면 1초마다 한번씩 요청한다.
// 		if( !XFacebook::sGet()->IsRequestingProfileImage( m_strcFbUserId ) ) {
// 			if( m_timerSec.IsOff() || m_timerSec.IsOver() ) {
// 				if( m_timerSec.IsOff() )
// 					m_timerSec.Set( 1.f );
// 				m_timerSec.Off();
// 				asdfasdf
// 			}
// 		}
// 	}
	if( m_timerCalc.IsOver() ) {
		// 서버에선 들어올일 없음.
		if( GetpDelegate() )
			GetpDelegate()->DelegateOnCalcTimer( this, m_timerCalc );
		m_timerCalc.Reset();
	}
#endif
	//
	m_timerCalc.Update( dt );
	m_timerOwn.Update( dt );

}

/**
 spAccount가 DeSerialize된후 최초 호출
*/
void XSpotCastle::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	XSpot::OnAfterDeSerialize( pWorld, pDelegator, idAcc, secLastCalc );
#ifdef _CLIENT
	// 서버에서의 스팟 총 소유시간 % 리젠시간 = 시작해야할 리젠시간
	float secStartRegen = fmodf( m_timerCalc.GetsecPassTime(), (float)XGAME::SEC_REGEN );
	m_timerCalc.Set( XGAME::SEC_REGEN );
	m_timerCalc.AddTime( secStartRegen );
#endif
#ifdef _SERVER
	bool bOpenedSpot = IsOpenedSpot( pWorld );
	if( secLastCalc && bOpenedSpot ) {
		// 마지막 저장한후로부터의 시간을 얻는다.
		xSec secPassTime = (XTimer2::sGetTime() - secLastCalc);
		// 만약 접속끊기전에 내 소유였다면 그동안에 뺏기지 않았는지 확인한다.
		if( GettimerSpawn().IsOn() ) {
			// 스팟의 스폰타이머에 흘러간 시간을 더한다.
			if( GettimerSpawn().IsAddOver( (double)secPassTime ) ) {
				// 스폰(점령) 남은 시간
				xSec secRemainSpawn = (xSec)GettimerSpawn().GetsecRemainTime();
				// 뺏길만한 (오프라인)시간이었다.
				// 점령후~온라인 사이의 시간을 생산시간등에서 보정해줘야 하므로 따로 뽑는다.
				xSec secAfterCapture = secPassTime - secRemainSpawn;
				// 로그아웃시 정산을 했으므로 정산타이머는 0, 정산후~점령까지 시간동안의 정산을 추가로 해줘야 하므로 정산시간 생성.
				/*
				DB초기화 한번하면 이거 다시 살려서 검사해볼것. 
				이론적으로 새코드상으로는 정산타이머의 값이 남아있지 않아야 한다.
				*/
				XBREAK( GetTimerCalc().GetsecPassTime() > 0 );	
				CalculateResourceByPassTime( idAcc, secRemainSpawn, 0 );
				// 스폰(점령)시간 이후는 매칭이 안되면 NPC라도 무조건 매칭이 된다는 조건으로 정산을 미리 한다.
				// 뺏긴 이후의 시간이므로 idAcc를 0으로 줘서 뺏긴자원의 생산량을 적용하도록 한다
				CalculateResourceByPassTime( 0, secAfterCapture, 0 );
				if( GetpDelegate() )
					GetpDelegate()->DelegateOnSpawnTimer( this, secAfterCapture );
				GettimerSpawnMutable().Off();
			}
		} else {
			// 오프라인으로 있었던 시간동안을 정산한다.
			/*
			DB초기화 한번하면 이거 다시 살려서 검사해볼것. 
			이론적으로 새코드상으로는 정산타이머의 값이 남아있지 않아야 한다.
			*/
			XBREAK( GetTimerCalc().GetsecPassTime() > 0 );
			CalculateResourceByPassTime( idAcc, secPassTime, 0 );
		}
	}
#endif // SERVER
}


void XSpotCastle::OnSpawn( XSPAcc spAcc )
{
	XSpot::OnSpawn( spAcc );
}

#if defined(_CLIENT) || defined(_GAME_SERVER)
static void sSetAryRes( xResourceAry *pOutAry, xtResource type
											, float gold, const std::vector<float>& aryMultiply )
{
	auto amount = gold * aryMultiply[ type ];
	amount = amount + (amount * xRandomF(-0.3f, 0.3f));		// 자원보유량을 자원스럽게 보이기 위해 랜덤을 넣음.
	if( amount < 1.f )
		amount = 1.f;
	(*pOutAry)[ type ] = int( amount );
}
/**
 @brief 상대의 전투력과 레벨을 바탕으로 그 상대를 루팅할때 표준 자원량을 계산한다.
*/
void XSpotCastle::GetResLootStandard( xResourceAry *pOutAry, int powerEnemy, int lvEnemy )
{
	auto pProp = XPropUser::sGet()->GetTable( lvEnemy );
	XBREAK( pProp == nullptr );
	// 각 자원의 환율의 역수.
	std::vector<float> aryMultiply( XGAME::xRES_MAX );
	for( int i = xRES_WOOD; i < xRES_MAX; ++i ) {
		auto type = (xtResource)i;
		aryMultiply[ type ] = 1.f / XGC->m_resourceRate[ type ];
	}
	const float rateWood = xRandomF( 0.2f, 0.8f );		// 자연스럽게 하기위해 목재/철의 비율을 달리함.
	const float rateIron = 1.f - rateWood;
	if( lvEnemy >= XGAME::LEVEL_MANDRAKE ) {
		// 만드레이크까지 나옴
		// 금액을 5등분해서 계산함
		float goldWood = pProp->goldLoot * (0.2f * rateWood);
		float goldIron = pProp->goldLoot * (0.2f * rateWood);
		float goldJewel = pProp->goldLoot * 0.2f;
		float goldSulfur = pProp->goldLoot * 0.2f;
		float goldMandrake = pProp->goldLoot * 0.2f;
		sSetAryRes( pOutAry, xRES_WOOD, goldWood, aryMultiply );
		sSetAryRes( pOutAry, xRES_IRON, goldIron, aryMultiply );
		sSetAryRes( pOutAry, xRES_JEWEL, goldJewel, aryMultiply );
		sSetAryRes( pOutAry, xRES_SULFUR, goldSulfur, aryMultiply );
		sSetAryRes( pOutAry, xRES_MANDRAKE, goldMandrake, aryMultiply );
	} else
	if( lvEnemy >= XGAME::LEVEL_SULFUR ) {
		// 유황까지 나옴
		// 금액을 4등분해서 계산함
		float goldWood = pProp->goldLoot * (0.25f * rateWood);
		float goldIron = pProp->goldLoot * (0.25f * rateWood);
		float goldJewel = pProp->goldLoot * 0.25f;
		float goldSulfur = pProp->goldLoot * 0.25f;
		sSetAryRes( pOutAry, xRES_WOOD, goldWood, aryMultiply );	
		sSetAryRes( pOutAry, xRES_IRON, goldIron, aryMultiply );	
		sSetAryRes( pOutAry, xRES_JEWEL, goldJewel, aryMultiply );	
		sSetAryRes( pOutAry, xRES_SULFUR, goldSulfur, aryMultiply );
	} else
	if( lvEnemy >= XGAME::LEVEL_JEWEL ) {
		// 보석까지 나옴
		float goldWood = pProp->goldLoot * (0.333f * rateWood);	// 전체금액에서 1/3는 목재
		float goldIron = pProp->goldLoot * (0.333f * rateIron);	// 전체금액에서 1/3는 목재
		float goldJewel = pProp->goldLoot * 0.333f;				// 전체금액에서 1/3는 보석
		sSetAryRes( pOutAry, xRES_WOOD, goldWood, aryMultiply );	
		sSetAryRes( pOutAry, xRES_IRON, goldIron, aryMultiply );	
		sSetAryRes( pOutAry, xRES_JEWEL, goldJewel, aryMultiply );	
	} else {
		// 나무/철만 반반으로 쪼개면 됨
		sSetAryRes( pOutAry, xRES_WOOD, pProp->goldLoot * rateWood, aryMultiply );
		sSetAryRes( pOutAry, xRES_IRON, pProp->goldLoot * rateIron, aryMultiply );	
	}
}

/**
 @brief 매칭후 이 스팟에서 드랍될 자원양을 계산한다.
 구버전
*/
// void XSpotCastle::SetDropRes( XSPAcc spAcc, int power, int level )
// {
// 	XBREAK( power == 0 );
// 	XBREAK( level == 0 );
// 	ClearLootRes();
// 	xResourceAry aryLocals( XGAME::xRES_MAX );		// 지역창고
// 	xResourceAry aryMain( XGAME::xRES_MAX );		// 중앙창고
// 	// 상대의 레벨과 전투력으로 그 레벨의 표준 루팅양을 계산함.
// 	GetResLootStandard( &aryLocals, power, level );
// 	// 대박확률 적용
// 	// 10%초대박 20%대박 50%보통 20%거지
// 	const std::vector<float> aryChance = { 0.1f, 0.2f, 0.5f, 0.2f };
// //	const std::vector<float> aryChance = { 0.02f, 0.1f, 0.78f, 0.1f };
// 	const std::vector<float> aryMultiply = { 3.f, 2.f, 1.f, 0.5f };
// 	int idx = XE::GetDiceChance( aryChance );
// 	XBREAK( idx < 0 || idx >= (int)aryMultiply.size() );
// 	float mulLuckyOrig = aryMultiply[ idx ];
// 	for( auto& amount : aryLocals ) {
// 		float mulLucky = mulLuckyOrig + (mulLuckyOrig * xRandomF(-0.5f, 0.5f));
// 		amount = (int)(amount * mulLucky);	// 대박찬스를 보정
// 		amount = int(amount + (amount * xRandomF( -0.5f, 0.5f )));	// 자연스럽게 보이기 위해 각 자원에 +-30%보정한다.
// 	}
// 	// 상대와의 색깔차이에 따라 루팅율을 달리해서 적용한다.
// 	int grade = XGAME::GetHardLevel( power, spAcc->GetPowerExcludeEmpty() );
// 	float rateLootLocal = 0.4f;
// 	float rateLootMain = 0.2f;
// 	switch( grade ) {
// 	case 2:	{
// 		rateLootLocal = 0.8f;
// 		rateLootMain = 0.8f;
// 		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
// 			auto type = (XGAME::xtResource)i;
// 			AddLootRes( type, (int)(aryLocals[type] * rateLootLocal) );
// 		}
// 		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
// 			auto type = ( XGAME::xtResource )i;
// 			AddLootRes( type, (int)( aryMain[ type ] * rateLootMain ) );
// 		}
// 	} break;
// 	case 1: {
// 		rateLootLocal = 0.6f;
// 		rateLootMain = 0.4f;
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		int r = xRandom(3);
// 		auto resType1 = XGAME::xRES_NONE;
// 		auto resType2 = XGAME::xRES_NONE;
// 		if( r == 0 ) {
// 			resType1 = XGAME::xRES_JEWEL;
// 			resType2 = XGAME::xRES_SULFUR;
// 		} else
// 		if( r == 1 ) {
// 			resType1 = XGAME::xRES_JEWEL;
// 			resType2 = XGAME::xRES_MANDRAKE;
// 		} else {
// 			resType1 = XGAME::xRES_SULFUR;
// 			resType2 = XGAME::xRES_MANDRAKE;
// 		}
// 		AddLootRes( resType1, (int)( aryLocals[ resType1 ] * rateLootLocal ) );
// 		AddLootRes( resType2, (int)( aryLocals[ resType2 ] * rateLootLocal ) );
// 		AddLootRes( resType1, (int)( aryMain[ resType1 ] * rateLootMain ) );
// 		AddLootRes( resType2, (int)( aryMain[ resType2 ] * rateLootMain ) );
// 	} break;
// 	case 0: {
// 		rateLootLocal = 0.4f;
// 		rateLootMain = 0.2f;
// 		auto type = (XGAME::xtResource)( XGAME::xRES_JEWEL + xRandom(3) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 		AddLootRes( type, (int)( aryLocals[ type ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		AddLootRes( type, (int)( aryMain[ type ] * rateLootMain ) );
// 	} break;
// 	case -1: {
// 		rateLootLocal = 0.2f;
// 		rateLootMain = 0.1f; 
// 		if( xRandom(2) == 0 ) {
// 			AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 			AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		} else {
// 			AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 			AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		}
// 	} break;
// 	case -2: 
// 		rateLootLocal = 0.1f;
// 		rateLootMain = 0.05f; 
// 		if( xRandom(2) == 0 ) {
// 			if( xRandom( 2 ) == 0 ) {
// 				AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 				AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 			} else {
// 				AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 				AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 			}
// 		}
// 		break;
// 	default:
// 		XBREAK(1);
// 		break;
// 	}
// }
/**
 @brief 매칭후 이 스팟에서 드랍될 자원양을 계산한다.
 색등급에 따라 나오는 자원양의 차등을 없앰.
*/
void XSpotCastle::SetDropRes2( XSPAcc spAcc, int power, int level )
{
	XBREAK( power == 0 );
	XBREAK( level == 0 );
	ClearLootRes();
	xResourceAry aryStorage( XGAME::xRES_MAX );		// 상대가 가진 가상의 표준자원양
	// 상대의 레벨과 전투력으로 그 레벨의 표준 루팅양을 계산함.
	GetResLootStandard( &aryStorage, power, level );
	// 상대와의 색깔차이에 의해 노랑이상은 
	int grade = XGAME::GetHardLevel( power, spAcc->GetPowerExcludeEmpty() );
	float rateLootAdjByColor = 1.f;
	switch( grade ) {
	case 2:	rateLootAdjByColor = 5.f;	break;
	case 1: rateLootAdjByColor = 2.f;	break;
	case 0: rateLootAdjByColor = 1.5f; break;
	case -2: rateLootAdjByColor = 0.5f; break;
	}
	// 상대와의 색등급 차이에 따라 루팅율을 조정한다.
	xResourceAry aryLoot = aryStorage;			// 루팅할수 있는 양
	if( rateLootAdjByColor != 1.f ) {
		for( auto& amount : aryLoot ) {
			amount = (int)(amount * rateLootAdjByColor);
		}
	}
	// 운에의한 확률을 적용해 최종 루팅양을 산정한다.
	AdjustLootByLucky( &aryLoot );
	// 상대와의 색등급 차이에 따라 루팅율을 조정한다.
// 	int grade = XGAME::GetHardLevel( power, spAcc->GetPowerExcludeEmpty() );
// 	float rateLoot = 1.f;
// 	if( grade <= -2 )		// 회색의 경우만 차등을 둔다.
// 		rateLoot = 0.5f;
	// 루팅할수 있는 최종 양을 스팟에 입력한다.
	for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
		auto type = ( XGAME::xtResource )i;
		const int amountLoot = (int)(aryLoot[ type ] /** rateLoot*/);
		if( amountLoot > 0 )
			AddLootRes( type, amountLoot );
	}
}

/**
 @brief 표준 루팅양에서 운에의한 확률을 적용해서 최종 루팅양을 계산한다.
*/
void XSpotCastle::AdjustLootByLucky( xResourceAry *pAryMutable )
{
	// 대박확률 적용
	// 10%초대박 20%대박 50%보통 20%거지
//	const std::vector<float> aryChance = { 0.1f, 0.2f, 0.5f, 0.2f };
	const std::vector<float> aryChance = {0.02f, 0.1f, 0.78f, 0.1f};
	const std::vector<float> aryMultiply = { 3.f, 2.f, 1.f, 0.75f };		// 대박 배수.
	int idx = XE::GetDiceChance( aryChance );
	XBREAK( idx < 0 || idx >= (int)aryMultiply.size() );
	const float mulLuckyOrig = aryMultiply[ idx ];
	for( auto& refAmount : *pAryMutable ) {
		// 이미 포준루팅양 뽑을때 내부에서 랜덤가감을 하므로 여기서 추가로 할필요는 없다.
		refAmount = (int)( refAmount * mulLuckyOrig );	// 대박율 자원양에 곱함.
		// 약간의 확률을 가감해서 일률적이지 않게 한다.
// 		const float mulLucky = mulLuckyOrig + xRandomF(-0.25f, 0.25f);	
// 		refAmount = (int)(refAmount * mulLucky);	// 대박율 자원양에 곱함.
// 		refAmount = int(refAmount + (refAmount * xRandomF( -0.3f, 0.3f )));	// 자연스럽게 보이기 위해 각 자원에 +-30%보정한다.
	}
}

#endif // #if defined(_CLIENT) || defined(_GAME_SERVER)
// void XSpotCastle::SetDropRes( XSPAcc spAcc, 
// 							xResourceAry& aryLocals, 
// 							xResourceAry& aryMain, int power )
// {
// 	XBREAK( power == 0 );
// 	ClearLootRes();
// 	// 상대와의 색깔차이에 따라 루팅율을 달리해서 적용한다.
// 	int grade = XGAME::GetHardLevel( power, spAcc->GetPower() );
// 	float rateLootLocal = 0.4f;
// 	float rateLootMain = 0.2f;
// 	switch( grade ) {
// 	case 2:	{
// 		rateLootLocal = 0.8f;
// 		rateLootMain = 0.8f;
// 		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
// 			auto type = (XGAME::xtResource)i;
// 			AddLootRes( type, (int)(aryLocals[type] * rateLootLocal) );
// 		}
// 		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
// 			auto type = ( XGAME::xtResource )i;
// 			AddLootRes( type, (int)( aryMain[ type ] * rateLootMain ) );
// 		}
// 	} break;
// 	case 1: {
// 		rateLootLocal = 0.6f;
// 		rateLootMain = 0.4f;
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		int r = xRandom(3);
// 		auto resType1 = XGAME::xRES_NONE;
// 		auto resType2 = XGAME::xRES_NONE;
// 		if( r == 0 ) {
// 			resType1 = XGAME::xRES_JEWEL;
// 			resType2 = XGAME::xRES_SULFUR;
// 		} else
// 		if( r == 1 ) {
// 			resType1 = XGAME::xRES_JEWEL;
// 			resType2 = XGAME::xRES_MANDRAKE;
// 		} else {
// 			resType1 = XGAME::xRES_SULFUR;
// 			resType2 = XGAME::xRES_MANDRAKE;
// 		}
// 		AddLootRes( resType1, (int)( aryLocals[ resType1 ] * rateLootLocal ) );
// 		AddLootRes( resType2, (int)( aryLocals[ resType2 ] * rateLootLocal ) );
// 		AddLootRes( resType1, (int)( aryMain[ resType1 ] * rateLootMain ) );
// 		AddLootRes( resType2, (int)( aryMain[ resType2 ] * rateLootMain ) );
// 	} break;
// 	case 0: {
// 		rateLootLocal = 0.4f;
// 		rateLootMain = 0.2f;
// 		auto type = (XGAME::xtResource)( XGAME::xRES_JEWEL + xRandom(3) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 		AddLootRes( type, (int)( aryLocals[ type ] * rateLootLocal ) );
// 		AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		AddLootRes( type, (int)( aryMain[ type ] * rateLootMain ) );
// 	} break;
// 	case -1: {
// 		rateLootLocal = 0.2f;
// 		rateLootMain = 0.1f; 
// 		if( xRandom(2) == 0 ) {
// 			AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 			AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 		} else {
// 			AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 			AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 		}
// 	} break;
// 	case -2: 
// 		rateLootLocal = 0.1f;
// 		rateLootMain = 0.05f; 
// 		if( xRandom(0) == 0 ) {
// 			if( xRandom( 2 ) == 0 ) {
// 				AddLootRes( XGAME::xRES_WOOD, (int)( aryLocals[ XGAME::xRES_WOOD ] * rateLootLocal ) );
// 				AddLootRes( XGAME::xRES_WOOD, (int)( aryMain[ XGAME::xRES_WOOD ] * rateLootMain ) );
// 			} else {
// 				AddLootRes( XGAME::xRES_IRON, (int)( aryLocals[ XGAME::xRES_IRON ] * rateLootLocal ) );
// 				AddLootRes( XGAME::xRES_IRON, (int)( aryMain[ XGAME::xRES_IRON ] * rateLootMain ) );
// 			}
// 		}
// 		break;
// 	default:
// 		XBREAK(1);
// 		break;
// 	}
// }

void XSpotCastle::ClearSpot()
{
	XSpot::ClearSpot();
	m_idOwner = 0;
	m_timerOwn.Off();
	m_timerCalc.Off();
	ClearLocalStorage();
	m_strHello.clear();
	m_strcFbUserId.clear();
//	m_LocalStorage.Clear();
	m_numLose = 0;
}

void XSpotCastle::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, bool bRetreat )
{
//	ClearSpot();		 // 여기서 이거 하는게 맞는데 지역창고까지 클리어 시켜버려서 지금은 쓰면 안됨.
	if( bWin ) {
		// 자원지를 플레이어의 소유로 만듬.
		SetPlayerOwner( spAccWin->GetidAccount(), spAccWin->GetstrName(), TRUE );
		SetLevel( spAccWin->GetLevel() );
	} else {
		if( m_numLose > 3 ) {
			// 3패배를 하면 스팟을 사라지게 한다.
			ClearSpot();
			ResetSpot();
		}
	}
}
bool XSpotCastle::IsEnemy() const
{
	auto spOwner = GetspOwner().lock();
	if( spOwner )
		return spOwner->GetidAccount() != m_idOwner;
	return true;
}

#ifdef _CLIENT
/**
 @brief sec초마다 자원이 생성되는 프로세스. 
 클라이언트 전용
 @param pRoot 생산메시지를 어느 윈도우에 띄울지.
 @param secCycle 리젠 주기
*/
float XSpotCastle::RegenResource( float secCycle, ID idPlayer, std::vector<XGAME::xRES_NUM>* pOut )
{
//	XBREAK( pRoot == nullptr );
	_tstring str;
	for( auto& res : GetpProp()->m_aryProduce ) {
		float income = (res.num / 60.f) * secCycle;
		// 내 성이아닐땐 10%만 얻도록
		if( m_idOwner != idPlayer )
			income *= 0.1f;
		DoStackToLocal( income, true );
		if( pOut )
			pOut->push_back( xRES_NUM(res.type, income) );
// 		_tstring strRes = XGAME::GetStrResource( res.type );
// #ifdef _DEBUG
// 		if( income ) {
// 			str += XFORMAT("%s:%.2f\n", strRes.c_str(), income );
// #else
// 		if( (int)income ) {
// 			str += XFORMAT( "%s:%.0f\n", strRes.c_str(), income );
// #endif // _DEBUG
//		}
	}
// 	auto pWndMsg = new XWndProduceMsg( GetidSpot(), str.c_str(), 20.f, XCOLOR_GREEN );
// 	pRoot->Add( pWndMsg );
	return 1.f;
}

float XSpotCastle::RegenResource2( float secCycle, ID idPlayer, XMsgPusher *pMsgPusher )
{
	XBREAK( pMsgPusher == nullptr );
	_tstring str;
	for( auto& res : GetpProp()->m_aryProduce ) {
		float income = (res.num / 60.f) * secCycle;
		// 내 성이아닐땐 10%만 얻도록
		if( m_idOwner != idPlayer )
			income *= 0.1f;
		DoStackToLocal( income, true );
		const _tstring strRes = XGAME::GetStrResource( res.type );
#ifdef _DEBUG
		if( income ) {
			str = XFORMAT("%s:%.2f\n", strRes.c_str(), income );
#else
		if( (int)income ) {
			str = XFORMAT( "%s:%.0f\n", strRes.c_str(), income );
#endif // _DEBUG
		}
//  		auto pWndMsg = new XWndProduceMsg( GetidSpot(), str.c_str(), 20.f, XCOLOR_GREEN );
// 		pMsgPusher->Add( pWndMsg );
	}
	return 1.f;
}

#endif // _CLIENT

#ifdef _CHEAT
// 치트모드에서만 사용. 소탕등의 기능을 쓸때 일단 군단데이타는 모두 있어야 해서 
// 임시방편으로 더미군단데이터를 만들때 사용.
void XSpotCastle::CreateLegion( XSPAcc spAccount )
{
	SetLevel( spAccount->GetLevel() );
	XGAME::xLegionParam legionInfo;
	legionInfo.m_numVisible = XLegion::xMAX_NOT_FOG;
	auto pLegion = XLegion::sCreateLegionForNPC( GetLevel(), 
																							0,
																							legionInfo );
	XBREAK( GetspLegion() != nullptr );
	SetpLegion( pLegion );
	UpdatePower( GetspLegion() );
}
#endif // _CHEAT
/**
 @brief 드랍될 아이템을 예약한다.
*/
void XSpotCastle::DoDropRegisterItem( int level )
{
	// 모든성은 초록이상이므로 모두 드랍되도록 한다.
	// 초록이었다가 회색으로 변하는 스팟도 있지만 그것은 유저의 노력으로 된것이므로 인정한다.
	xSec secPass = XTimer2::sGetTime() - s_secResetHeroSoul;
	if( m_idHeroDrop == 0 || secPass > xHOUR(3) ) {
		// 갓챠로 뽑을수 있는 영웅리스트를 얻는다.
		auto pPropHero = PROP_HERO->GetpPropRandomByGetType( XGAME::xGET_GATHA );
		if( XBREAK( pPropHero == nullptr ) )
			return;
		// 그 영웅의 영혼석 아이템을 꺼낸다.
		auto pPropItem = PROP_ITEM->GetpPropByidHero( pPropHero->idProp );
		if( XBREAK( pPropItem == nullptr ) )
			return;
		// 드랍될 영웅을 다시 설정한다.
		m_idHeroDrop = pPropItem->idProp;
		// 리셋타이머를 다시 리셋함.
		s_secResetHeroSoul = XTimer2::sGetTime();
	}
	// 영혼석 드랍을 등록한다.
	XGAME::xDropItem drop;
	drop.idDropItem = m_idHeroDrop;
	drop.num = 1;
	drop.chance = XGC->m_rateDropSoulByCastle;
	AddDropItem( drop );
	XBREAK( drop.chance <= 0 || drop.chance > 1.f );
	XSpot::DoDropRegisterItem( level );
}

/**
 @brief 프로필이미지 요청한것이 도착했다.
*/
#ifdef _CLIENT
/**
 @brief 성스팟의 프로필 이미지 사진을 요청한다.
*/
void XSpotCastle::DoRequestProfileImage()
{
	XBREAK( m_strcFbUserId.empty() );
	XTRACE( "XSpotCastle::DoRequestProfileImage: fbid=%s", C2SZ( m_strcFbUserId ) );
	XFacebook::sGet()->sSetpDelegate( GAME );
	XFacebook::sGet()->GetRequestProfileImage( m_strcFbUserId );
}

void XSpotCastle::cbOnRecvProfileImage( const XE::xImage& imgInfo )
{
	//  일단 이미지만 받아두고 월드에서 그릴때 사진액자를 만든다.
	SAFE_RELEASE2( IMAGE_MNG, m_psfcProfile );
	m_psfcProfile = IMAGE_MNG->CreateSurface( GetstrcFbUserId(), imgInfo );
}
#endif // _CLIENT

