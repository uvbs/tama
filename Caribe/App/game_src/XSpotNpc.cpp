#include "stdafx.h"
#include "XSpotNpc.h"
#include "XArchive.h"
#include "XAccount.h"
#include "XPropLegionH.h"
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
#include "XPropLegion.h"
#include "XStruct.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//using namespace xCampaign;
using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
/**
 @brief OnCreate는 Deserialize로 생성된 스팟에서는 호출되지 않는다.
*/
void XSpotNpc::OnCreateNewOnServer( XSPAcc spAcc )
{
	if( GetpProp()->GetsecRegen() )
	{
		GettimerSpawnMutable().AddTime( GettimerSpawn().GetsecWaitTime() + 1.f );
		XBREAK( GettimerSpawn().IsOver() == FALSE );
	}
}

void XSpotNpc::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	XBREAK( GetpProp()->idSpot > 0xffff );
	ar << (WORD)GetpProp()->idSpot;
	ar << (BYTE)m_typeResource;
	ar << (BYTE)0;
	ar << m_numResource;
	ar << GetseedRandom();
}
BOOL XSpotNpc::DeSerialize( XArchive& ar, DWORD ver ) 
{
#ifndef _DUMMY_GENERATOR
//	// CONSOLE( "deserialize npc" );
#endif	
	XSpot::DeSerialize( ar, ver );
	ID idProp;
	BYTE b0;
	WORD w0;
	ar >> w0;	idProp = w0;
	ar >> b0;	m_typeResource = ( XGAME::xtResource )b0;
	if( XGAME::IsInvalidResource(m_typeResource) )
		m_typeResource = XGAME::xRES_WOOD;
// 	if( m_typeResource == XGAME::xRES_MAX ) {
// 		m_typeResource = XGAME::xRES_NONE;
// 	} else
// 	if( XGAME::IsInvalidResource(m_typeResource) ) {
// 		XBREAK( m_typeResource != XGAME::xRES_NONE );
// 	}
	ar >> b0;	;
	ar >> m_numResource;
	if( m_numResource <= 0 )
		m_numResource = 1;
	{
		DWORD dw0;
		ar >> dw0;	SetseedRandom( dw0 );
	}
	// 비액티브상태(스폰을 기다리는중)인데 타이머가 꺼져있다면 다시 켜준다.
#ifdef _GAME_SERVER
	if( GetpProp() ) {
		auto pProp = GetpProp();
		if( pProp->GetsecRegen() && GettimerSpawn().IsOff() && !IsActive() )
			ResetSpawn();
		if( GetstrName().empty() ) {
			// 이름이 지정되어있지 않으면 지정한다.
			if( pProp->idName == 0 ) {
				if( GetpAreaProp()->lvArea <= 1 )
					SetstrName( XTEXT( 2001 ) );	// 1레벨지역은 야만족
				else
					SetstrName( XTEXT( 2012 ) );	// 
			}
			else {
				SetstrName( XTEXT( pProp->idName ) );
			}
		}
	}
//	XBREAK( GettimerSpawn().IsOff() );
#endif // _GAME_SERVER
	return TRUE;
}

void XSpotNpc::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
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

bool XSpotNpc::Update( XSPAcc spAcc )
{
	int lvSpot = GetLevel();
	if( lvSpot == 0 ) {
		ResetLevel( spAcc );
		lvSpot = GetLevel();
		XBREAK( lvSpot == 0 );
	}
	if( IsInvalidResource(m_typeResource) ) {
		ResetResource( lvSpot, spAcc );
	}
	if( IsValidResource(m_typeResource) && GetnumLoots() == 0 )
		ResetResource( lvSpot, spAcc );
	return XSpot::Update( spAcc );
}

/**
 @brief 스팟을 다시 리젠 시킨다.
*/
void XSpotNpc::DoSpawnSpot()
{
	ResetSpawn();
	GettimerSpawnMutable().AddTime( 99999.f );		// 강제로 시간을 앞당긴다.
}

void XSpotNpc::ResetSpawn( void ) 
{
	if( XBREAK(GetpProp()->GetsecRegen() == 0) )
		return;
	XBREAK( GettimerSpawn().IsOn() == TRUE );	// 보통은 꺼져있어야 함.
	// 일정시간후에 리스폰 되도록 타이머를 맞춘다.
 	const float base = GetpProp()->GetsecRegen();
	float sec = 180.f + (float)xRandom(-30,30);
	if( IsEventSpot() )
		sec *= 0.5f;
	auto spOwner = GetspOwner().lock();
	if( spOwner ) {
		if( spOwner->GetLevel() < 2 ) {
			// 극초반 npc가 너무 늦게 나오는거같아 줄여봄.
			sec = 30;
		}
	}

#ifdef _DEV
//	sec /= 2.f;
#endif
	GettimerSpawnMutable().Set( sec );
}

/**
 스팟에 NPC군대를 생성시킨다.
*/
void XSpotNpc::CreateLegion( XSPAcc spAcc )
{
	XBREAK( GetseedRandom() == 0 );
	XBREAK( GetPower() == 0 );	// 이제 전투력이 미리 계산되어진다.
	XBREAK( GetLevel() == 0 );
	XBREAK( spAcc == nullptr );
	// 정찰(군대가생성됨)할때 드랍템도 결정됨.
	bool bInvalid = XGAME::IsInvalidResource( m_typeResource );
	XBREAK( bInvalid );		// 이제 ?자원 허용안함.
	if( bInvalid || ( m_numResource <= 1 ) )	{	// 구버전 데이타가 1로 저장하는경우가 있어 호환을 위해 남겨둠. 원래는 0이어야 맞음.
		ResetResource( GetLevel(), spAcc );
	}
	// 군대생성직전 스폰될당시의 랜덤시드값으로 바꾸고 군단을 생성한다.
	XSeed seed( GetseedRandom() );
// 	XLegion *pLegion = CreateLegion( GetpProp(), GetLevel() );
	XSPLegion spLegion = CreateLegion( GetpProp(), GetLevel() );
	XBREAK( GetspLegion() != nullptr );
	SetspLegion( spLegion );
	// 군대가 생성되었다면 자동적으로 스폰 타이머는 꺼진다.
	GettimerSpawnMutable().Off();
	int powerOld = GetPower();
	UpdatePower( spLegion );
	// 랜덤시드방식에 문제가 있어 일단 powerOld와 GetPower()는 다를수있게 허용함.
// 	XBREAK( powerOld != GetPower() );	// 검증
}

/**
 @brief 
*/
XSPLegion XSpotNpc::CreateLegion( XPropWorld::xNPC *pProp, int lvSpot )
{
	XBREAK( lvSpot == 0 );
#if defined(_XSINGLE) || !defined(_CLIENT)
	XGAME::xLegion legion("");
	auto pPropLegion = XPropLegion::sGet()->GetpProp( pProp->m_idsLegion );
	if( pPropLegion )
		legion = *pPropLegion;
	return XLegion::sCreateLegionForNPC2( legion, lvSpot, true );
#else
	return XSPLegion();
#endif
}


/**
 @brief 스팟 레벨을 다시 잡는다.
*/
void XSpotNpc::ResetLevel( XSPAcc spAcc )
{
	if( XBREAK( spAcc == nullptr ) )
		return;
	auto pProp = GetpProp();
	if( pProp == nullptr )
		return;
	int lvOld = GetLevel();
	// 레벨이 지정되어있는 스팟의 경우 프로퍼티레벨과 달라졌으면 다시 계산한다.
// 	if( !pProp->clan && pProp->level > 0 ) {
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

void XSpotNpc::ResetResource( int lvSpot, XSPAcc spAcc )
{
	// 적절한 자원 보상
	int numBase = 100;
	if( spAcc->IsLockAcademy() ) {
		// 훈련소가열리기전까진 목재와 철만 나오게 한다.
		m_typeResource = ( XGAME::xtResource )( xRandom( XGAME::xRES_WOOD, XGAME::xRES_IRON ) );
	} else {
		// 확률은 모든자원 똑같이 하되 자원시세에 따라 나오는양을 조절하자
		m_typeResource = (XGAME::xtResource)( xRandom( XGAME::xRES_WOOD, XGAME::xRES_MANDRAKE ) );
	}
	m_numResource = (int)( numBase + ( lvSpot * ( numBase*0.5f ) ) );
	m_numResource += (int)( m_numResource * xRandomF( 0.1f ) );
	m_numResource *= 2;
	ClearLootRes();
	XBREAK( XGAME::IsInvalidResource( m_typeResource ) );
	SetLootRes( m_typeResource, m_numResource );
}

void XSpotNpc::ResetName( XSPAcc spAcc )
{
	auto pProp = GetpProp();
	if( pProp->idName == 0 ) {
			SetstrName( XTEXT( 2012 ) );
// 		}
	}
	else {
		SetstrName( XTEXT( GetpProp()->idName ) );
	}
}
void XSpotNpc::ResetPower( int lvSpot )
{
	XBREAK( GetspLegion() != nullptr );
	DestroyLegion();	// 전투력이 새로 계산되는것이므로 군단을 살려둘이유가 없다.
	// 새시드값을 생성한후 백업받아두고 시드값을 새걸로 바꾼다.
	auto seedNew = ::xGenerateSeed();
	SetseedRandom( seedNew );
//	SetseedRandom( ::timeGetTime() );
	XSeed seed( seedNew );
	auto spLegion = CreateLegion( GetpProp(), lvSpot );
//	LegionPtr spLegion( pLegion );
	// 전투력을 산출한다.
	UpdatePower( spLegion );
}

void XSpotNpc::OnSpawn( XSPAcc spAcc )
{
	XSpot::OnSpawn( spAcc );
	//
	// 드랍리스트를 여기서정하는이유는 미리 정해지고 색등급이 내려가도 드랍품목은 그대로 살아있게 하기 위함
	// 드랍자원초기화
//	SetLootRes( XGAME::xRES_NONE, 0 );	// 루팅자원을 ?로 나타내게 하기 위해
	ClearLootRes();
	XBREAK( GetLevel() == 0 );
	ResetResource( GetLevel(), spAcc ); 
}
/**
 @brief 드랍확률이 있는 모든 아이템목록을 얻는다.
 드랍확률이 1%만 되도 목록에 포함되게 된다.
*/
int XSpotNpc::GetDropableList( std::vector<ID> *pOutAry )
{
	XSpot::GetDropableList( pOutAry );

	return pOutAry->size();
}

bool XSpotNpc::IsElite() 
{
	auto pPropSpot = GetpProp();
	if( pPropSpot->m_idsLegion.empty() )
		return false;
	auto pPropLegion = XPropLegion::sGet()->GetpProp( pPropSpot->m_idsLegion );
	if( pPropLegion )
		return pPropLegion->gradeLegion == XGAME::xGL_ELITE;
	return false;
// 	return GetpProp()->legion.gradeLegion == XGAME::xGL_ELITE;
}
