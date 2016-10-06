#include "stdafx.h"
#include "XSpotSulfur.h"
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
//#include "XCampObj.h"
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

//using namespace xCampaign;
using namespace XGAME;

////////////////////////////////////////////////////////////////
XSpotSulfur::XSpotSulfur( XWorld *pWorld, XPropWorld::xSULFUR* pProp, XDelegateSpot *pDelegate )
	: XSpot( pProp, XGAME::xSPOT_SULFUR, pWorld, pDelegate ) 
{
	Init();
	if( pProp ) {
		m_lsLocal.maxSize = pProp->GetProduce() * 60.f * 8.f;
		XBREAK( m_lsLocal.maxSize <= 0 );
	}
}
void XSpotSulfur::Destroy()
{
}
void XSpotSulfur::OnCreateNewOnServer( XSPAcc spAcc )
{
	DoSpawnSpot();
}
void XSpotSulfur::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << GetpProp()->idSpot;
	if( ar.IsForDB() )
		ar << 0;
	else
		ar << m_idEncounterUser; //m_numSulfur;
	m_lsLocal.Serialize( ar );
	if( ar.IsForDB() ) {
		// DB저장시에는 로그확인했으면 로그는 저장안한다.
#pragma message("로그아웃할때  클리어하는건데 여기다 하면 안된다. 정섭은 주기적으로 세이브를 계속 하기 때문에.")
		if( m_bCheckEncounterLog ) {
			m_bCheckEncounterLog = false;
			m_aryEncounter.Clear();
		}
		ar << (DWORD)0;
	} else {
		ar << xboolToByte( m_bCheckEncounterLog );
		ar << (char)0;
		ar << (char)0;
		ar << (char)0;
	}
	ar << m_aryEncounter;
}
BOOL XSpotSulfur::DeSerialize( XArchive& ar, DWORD ver ) 
{
	XSpot::DeSerialize( ar, ver );
	ID idProp;
	ar >> idProp;
	// null이 나올수도 있으나 그냥 리턴하지말고 남은 시리얼라이즈를 모두 끝내야함.
	// 스팟이 필요에의해 삭제될수 있기때문.
	ar >> m_idEncounterUser;	//m_numSulfur;
	m_lsLocal.DeSerialize( ar );
	if( ver >= 24 ) {
		DWORD dw0;
		ar >> dw0;		// m_bCheckEncounterLog
	}
	if( ver >= 23 ) {
		ar >> m_aryEncounter;
	}
	return TRUE;
}

void XSpotSulfur::Process( float dt )
{
	XSpot::Process( dt );
}

void XSpotSulfur::ClearSpot() 
{
	XSpot::ClearSpot();
	//
//	m_bEncounter = false;
	m_idEncounterUser = 0;
//	m_lsLocal.Clear();		// 플레이어가 수거하기전까진 없어지지 않는다.
}

void XSpotSulfur::DoDropRegisterRes( XSPAcc spAcc )
{
	const int lvSpot = GetLevel();
	XBREAK( lvSpot == 0 );
	ClearLootRes();
//	int numSulfur = 10000 + xRandom( 15000 );
	int numSulfur = ((int)(lvSpot / 5) + 1) * 5000;
	numSulfur += xRandom( (int)(numSulfur * 0.25f) );
	SetLootRes( XGAME::xRES_SULFUR, numSulfur );
}

void XSpotSulfur::ResetName( XSPAcc spAcc )
{
	SetstrName( XTEXT( 2020 ) );
}
/**
 @brief 타이머나 어떤 이벤트에 의해서 스팟이 스폰되었다. 이것은 스폰생성과는 별개다.
*/
void XSpotSulfur::OnSpawn( XSPAcc spAcc )
{
	XSpot::OnSpawn( spAcc );
}

void XSpotSulfur::MoveResourceLocalToMain( XSPAcc spAccount, 
		XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry/* = nullptr*/ ) 
{
	XGAME::xRES_NUM resNum;
	spAccount->AddResource( XGAME::xRES_SULFUR, (int)m_lsLocal.numCurr );
	resNum.type = XGAME::xRES_SULFUR;
	resNum.num = m_lsLocal.numCurr;
	pOutAry->Add( resNum );
	m_lsLocal.Clear();
}

void XSpotSulfur::ResetSpawn( void ) 
{
	const float base = GetpProp()->GetsecRegen();
	if( XBREAK(base == 0) )
		return;
	XBREAK( GettimerSpawn().IsOn() == TRUE );	// 보통은 꺼져있어야 함.
	// 일정시간후에 리스폰 되도록 타이머를 맞춘다.
	int numSpot = GetNumSpots();
	XBREAK( numSpot < 0 || numSpot > 200 );
	float ns = numSpot / 3.5f;
	float sec = base + (( ns * ns ) * 2) * (base * 1.f);
	sec += xRandomF( sec * 0.1f );
	GettimerSpawnMutable().Set( sec );

}

void XSpotSulfur::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	XSpot::OnAfterDeSerialize( pWorld, pDelegator, idAcc, secLastCalc );
#ifdef _SERVER
	if( secLastCalc ) {
		// 마지막 저장한후로부터 현재까지 지나간 시간을 얻는다.
		xSec secPassTime = ( XTimer2::sGetTime() - secLastCalc );
		GettimerSpawnMutable().AddTime( secPassTime );
	}
#endif // SERVER
}
void XSpotSulfur::DoSpawnSpot() 
{
	ResetSpawn();
	GettimerSpawnMutable().AddTime( 99999.f );		// 강제로 시간을 앞당긴다.
}

/**
 스팟에 NPC군대를 생성시킨다.
*/
void XSpotSulfur::CreateLegion( XSPAcc spAcc )
{
	XBREAK( GetPower() == 0 );	// 이제 전투력이 미리 계산되어진다.
	XBREAK( GetLevel() == 0 );
	XBREAK( spAcc == nullptr );
	// 정찰(군대가생성됨)할때 드랍템도 결정됨.
	if( GetLootAmount( XGAME::xRES_SULFUR ) == 0 )	// 드랍자원이 아직 없을때만 다시 만든다.
		DoDropRegisterRes( spAcc );
	//
	// 군대생성직전 스폰될당시의 랜덤시드값으로 바꾸고 군단을 생성한다.
	XSeed seed( GetseedRandom() );
	XLegion *pLegion = CreateLegion( GetpProp(), GetLevel() );
	XBREAK( GetspLegion() != nullptr );
	SetpLegion( pLegion );
	// 군대가 생성되었다면 자동적으로 스폰 타이머는 꺼진다.
	GettimerSpawnMutable().Off();
	int powerOld = GetPower();
	UpdatePower( GetspLegion() );
	// 랜덤시드방식에 문제가 있어 일단 powerOld와 GetPower()는 다를수있게 허용함.
// 	XBREAK( powerOld != GetPower() );	// 검증
}

/**
 @brief 
*/
XLegion* XSpotSulfur::CreateLegion( XPropWorld::xBASESPOT *pProp, int lvSpot ) const
{
	XBREAK( lvSpot == 0 );
	XGAME::xLegionParam legionInfo;
	XLegion *pLegion = XLegion::sCreateLegionForNPC( lvSpot, 0, legionInfo );
	return pLegion;
}

/**
 @brief 스팟 레벨을 다시 잡는다.
*/
void XSpotSulfur::ResetLevel( XSPAcc spAcc )
{
	if( XBREAK( spAcc == nullptr ) )
		return;
	auto pProp = GetpProp();
	if( pProp == nullptr )
		return;
	int lvOld = GetLevel();
	// 레벨이 지정되어있는 스팟의 경우 프로퍼티레벨과 달라졌으면 다시 계산한다.
	if( pProp->level > 0 ) {
		if( pProp->level != GetLevel() )
			SetLevel( 0 );
	}
	// 레벨산정 알고리즘이 변경되었을수 있으니 레벨리셋명령을 받을때마다 적정레벨을 다시 계산해서
	int lv = CalcLevel( pProp, spAcc->GetLevel() );
	if( pProp->strWhen.empty() ) {
		// 일반 스팟
		SetLevel( lv );
	} else {
		// 퀘 스팟
		// 그 레벨이 기존레벨보다 작을때만 갱신한다.
		if( GetLevel() == 0 || lv < GetLevel() ) {	// DB에서 저장된걸 불러왔을수 있으므로 0체크 해야함.
			SetLevel( lv );
		}
	}
}

void XSpotSulfur::ResetPower( int lvSpot )
{
	XBREAK( GetspLegion() != nullptr );
	DestroyLegion();	// 전투력이 새로 계산되는것이므로 군단을 살려둘이유가 없다.
	// 새시드값을 생성한후 백업받아두고 시드값을 새걸로 바꾼다.
	const auto seedNew = xGenerateSeed();
	SetseedRandom( seedNew );
//	SetseedRandom( ::timeGetTime() );
	XSeed seed( seedNew );
	XLegion *pLegion = CreateLegion( GetpProp(), lvSpot );
	XSPLegion spLegion( pLegion );
	// 전투력을 산출한다.
	UpdatePower( spLegion );
}

void XSpotSulfur::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	if( idAccLose ) {
		// 유황스팟 인카운터전이었으면 승패에 관계없이 지워야함(npc는 이겼으므로)
		ClearSpot();
		ResetSpot();
	} else {
		// npc와 싸운상황
		if( bWin ) {
			// npc에게 이김
//			XSpot::ClearSpot();
			ClearSpot();		// 이제 인카운터가 일어났는지에 대한 판단을 result.bEncounter가 하므로 다 지워도 됨.
			ResetSpot();
		}	else {}
	}
}
