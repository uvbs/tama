#include "stdafx.h"
#include "XSpotMandrake.h"
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
using namespace xSpot;
////////////////////////////////////////////////////////////////
XSpotMandrake::XSpotMandrake( XWorld *pWorld, XPropWorld::xMANDRAKE *pProp )
	: XSpot( pProp, XGAME::xSPOT_MANDRAKE, pWorld ) 
{
	Init();
//		m_lsLocal.maxSize = (int)( ( ( (float)GetpProp()->GetProduce() / XGAME::SEC_REGEN ) * 60 * 60 ) * 8 );
  if( pProp )
		SetMaxLocal( pProp );
}

void XSpotMandrake::Destroy()
{
}

void XSpotMandrake::OnCreateNewOnServer( XSPAcc spAcc )
{
		// 최초 ? 상태로 만듬.
	ClearSpot();
	Initialize( spAcc );
// 	SetPlayerOwner( 0,
// 									spAcc->GetstrName(),
// 									spAcc->GetidAccount(),
// 									spAcc->GetLevel(),
// 									0,
// 									0,
// 									0 );
}

/**
 @brief 스팟을 최초상태로 초기화 시킨다.
*/
void XSpotMandrake::Initialize( XSPAcc spAcc )
{
//	SetPlayerOwner( 0, _T(""), 0, 0, 0, 0, 0 );
	SetPlayerOwner( 0, _T("") );
}

void XSpotMandrake::SetMaxLocal( XPropWorld::xMANDRAKE *pProp )
{
	//XBREAK( pProp == nullptr );
	if( !pProp )
		return;
	m_lsLocal.maxSize = pProp->GetProduce() * 60.f * 8.f;
}

void XSpotMandrake::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << (BYTE)GetpProp()->idSpot;
	ar << (BYTE)m_idxLegion;
	ar << (BYTE)m_Win;
	ar << (BYTE)0;
	ar << m_idOwner;
	ar << m_idMatchEnemy;
	ar << GetstrName();
	ar << m_strcFbUserId;
	if( ar.IsForDB() )
		ar << _tstring();
	else
		ar << m_strHello;
	ar << 0; //m_loot;
	ar << m_Reward;
	ar << m_secLastEvent;
	m_timerCalc.Serialize( ar );
	m_timerOwn.Serialize( ar );
	m_lsLocal.Serialize( ar );
}
BOOL XSpotMandrake::DeSerialize( XArchive& ar, DWORD ver ) 
{
#ifndef _DUMMY_GENERATOR
//	// CONSOLE( "deserialize mandrake" );
#endif // not _DUMMY_GENERATOR
	XSpot::DeSerialize( ar, ver );
	ID idProp;
	BYTE b0;
	DWORD dw0;
	ar >> b0;	idProp = b0;
	ar >> b0;	m_idxLegion = b0;
	ar >> b0;	m_Win = b0;
	ar >> b0;
	// null이 나올수도 있으나 그냥 리턴하지말고 남은 시리얼라이즈를 모두 끝내야함.
	// 스팟이 필요에의해 삭제될수 있기때문.
	ar >> m_idOwner;
	ar >> m_idMatchEnemy;
	_tstring strName;
	ar >> strName;
	SetstrName( strName.c_str() );
	if( ver >= 28 ) {
		_tstring strTemp;
		ar >> strTemp; m_strcFbUserId = SZ2C(strTemp);
	}
	if( ver >= 28 ) {
		ar >> m_strHello;
	}
	ar >> dw0;		//	ar >> m_loot;
	ar >> m_Reward;
	ar >> m_secLastEvent;
	int _verTimer = 1;
	if( ver < 22 )
		_verTimer = 0;
	m_timerCalc.DeSerialize( ar, _verTimer );
#ifdef _CLIENT
	// 클라에선 주기적 타이머로 쓴다.
	m_timerCalc.Set( XGAME::SEC_REGEN );
#endif
	m_timerOwn.DeSerialize( ar, _verTimer );
	m_lsLocal.DeSerialize( ar, _verTimer );
//	XBREAK( GetpProp() == nullptr );
	if( GetpProp() )
		SetMaxLocal( GetpProp() );		// 프로퍼티가 변해서 최대크기가 바꼈을수 있으니 새로 세팅해야함.
	XBREAK(m_lsLocal.maxSize == 0);
	if( m_idOwner == 0 )
		SetstrName(_T(""));
	return TRUE;
}

/**
 스팟을 유저(플레이어 자신포함)의 소유로 만든다.
 idAcc가 0이고 idEnemy가 자기자신이면 ? 상태가 된다.
*/
void XSpotMandrake::SetPlayerOwner( ID idAcc, LPCTSTR szName )
{
	// 이미 소유하고 있던 자원지면 다시 처리하지 않음.
	if( m_idOwner == idAcc )
		return;
	m_idOwner = idAcc;
	m_idMatchEnemy = 0;
  SetPower( 0 );
	SetstrName( szName );
	SetLevel( 0 );
	m_secLastEvent = 0;
	m_idxLegion = 0;
	m_Win = 0;
// 	m_Reward = 1000;
	m_Reward = 0;
	SetScore( 0 );
	m_strHello.clear();
	m_strcFbUserId.clear();
#ifdef _CLIENT
	// 리소스 리젠 타이머를 세팅함.
	if( idAcc )
		m_timerCalc.Set( XGAME::SEC_REGEN );
#else
	// 소유타이머를 작동시킴
	if( idAcc )	{
		m_timerCalc.Start();
		m_timerOwn.Start();
	}
#endif
}
// void XSpotMandrake::SetPlayerOwner( ID idAcc, LPCTSTR szName, ID idEnemy, int level, int win, int reward, int score )
// {
// 	// 이미 소유하고 있던 자원지면 다시 처리하지 않음.
// 	if( m_idOwner == idAcc )
// 		return;
// 	m_idOwner = idAcc;
// 	m_idMatchEnemy = idEnemy;
//   SetPower( 0 );
// 	SetstrName( szName );
// 	SetLevel( level );
// // 	m_loot = 0;
// 	m_secLastEvent = 0;
// 	m_idxLegion = 0;
// 	m_Win = win;
// 	if( reward < 1000 )
// 		reward = 1000;		// 디폴트 값.
// 	m_Reward = reward;
// 	SetScore( score );
// #ifdef _CLIENT
// 	// 리소스 리젠 타이머를 세팅함.
// 	if( idAcc )
// 		m_timerCalc.Set( XGAME::SEC_REGEN );
// #else
// 	// 소유타이머를 작동시킴
// 	if( idAcc )
// 	{
// 		m_timerCalc.Start();
// 		m_timerOwn.Start();
// 	}
// #endif
// }

// int XSpotMandrake::DoDropItem( XSPAcc spAcc, XArrayLinearN<ItemBox, 256> *pOutAry, float multiplyDropNum = 1.f )
// {
// 	auto pPropItem = sDoDropEquip( spAcc->GetLevel() );
// 	if( pPropItem )
// 		SetidDropItem( pPropItem->idProp );
// 	//
// 	XSpot::DoDropItem( spAcc, pOutAry, multiplyDropNum );
// }


/**
 현재시간까지의 자원을 정산한다.
*/
void XSpotMandrake::CalculateResource( ID idAcc, xSec secAdjustOffline )
{
	CalculateResourceByPassTime( idAcc, (xSec)m_timerCalc.GetsecPassTime(), secAdjustOffline );
	// 정산 타이머는 다시 리셋
	m_timerCalc.Reset();
}
void XSpotMandrake::CalculateResourceByPassTime( ID idAcc, xSec secPassTime, xSec secAdjustOffline )
{
	if( m_idOwner == idAcc )
	{
		float min = ( secPassTime - secAdjustOffline ) / 60.f;
		float amount = GetpProp()->GetProduce() * min;
		DoStackToLocal( amount, true );
	}
}


void XSpotMandrake::MoveResourceLocalToMain( XSPAcc spAccount,
		XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry/* = nullptr*/ )
{
	XGAME::xRES_NUM resNum;
	spAccount->AddResource( XGAME::xRES_MANDRAKE, (int)m_lsLocal.numCurr );
	resNum.type = XGAME::xRES_MANDRAKE;
	resNum.num = m_lsLocal.numCurr;
	pOutAry->Add( resNum );
	m_lsLocal.Clear();
}

void XSpotMandrake::Process( float dt )
{
	XSpot::Process( dt );
	// 소유 타이머의 델리게이트
#ifdef _CLIENT
	if( m_timerCalc.IsOver() )	{
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

void XSpotMandrake::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
{
	XSpot::OnAfterDeSerialize( pWorld, pDelegator, idAcc, secLastCalc );
#ifdef _CLIENT
	// 서버에서의 스팟 총 소유시간 % 리젠시간 = 시작해야할 리젠시간
	float secStartRegen = fmodf( m_timerCalc.GetsecPassTime(), (float)XGAME::SEC_REGEN );
	m_timerCalc.Set( XGAME::SEC_REGEN );
	m_timerCalc.AddTime( secStartRegen );
#endif
#ifdef _SERVER
	const bool bOpenedSpot = IsOpenedSpot( pWorld );
	if( secLastCalc && bOpenedSpot ) {
		// 마지막 저장한후로부터의 시간을 얻는다.
		xSec secPassTime = ( XTimer2::sGetTime() - secLastCalc );
		// 오프라인 동안에 뺏겼을 경우 그 당시 이미 정산되어 DB에 저장되어 있어야 한다.
		if( m_idOwner == idAcc )
		{
			// 광산이 계정주인의 것일때만 정산한다.
// 			m_timerCalc.On();	// 간혹 정산타이머가 꺼진채 저장된 경우를 위해
// 			m_timerCalc.Reset();
// 			m_timerCalc.AddTime( secPassTime );
			CalculateResourceByPassTime( idAcc, secPassTime, 0 );
		}
	}
#endif // SERVER
}

void XSpotMandrake::OnSpawn( XSPAcc spAcc )
{
	XSpot::OnSpawn( spAcc );
//	DoDropRegisterEquip( spAcc->GetLevel() );
}

bool XSpotMandrake::IsEnemy() const
{
	auto spOwner = GetspOwner().lock();
	if( spOwner )
		return spOwner->GetidAccount() != m_idOwner;
	return true;
}
/**
 @brief 스팟의 현재상태를 얻는다.
 @param idAccPlayer 이스팟을 가진 플레이어 계정의 아이디
*/
xSpot::xtStateMandrake XSpotMandrake::GetState( ID idAccPlayer )
{
	XBREAK( idAccPlayer == 0 );
	if( m_idOwner == 0 )
		return xSM_NO_MATCH;
	else
	if( m_idOwner != idAccPlayer )
		return xSM_CHALLENGE;
	else
	if( m_idOwner == idAccPlayer )
		return xSM_DEFENSE;
	else {
		XBREAK(1);
	}
	return xSM_NONE;
}


/**
 @brief 지역이 오픈됨 이벤트 받음.
*/
void XSpotMandrake::OnOpenedArea( XPropCloud::xCloud* pPropArea )
{
	// 이미 스팟이 생성되어 있었다고 하더라도 지역이 오픈되면 ?상태로 시작해야 한다.
	ClearSpot();
}

void XSpotMandrake::OnAfterBattle( XSPAcc spAccWin
																, ID idAccLose
																, bool bWin
																, bool bRetreat )
{
	int reward = m_Reward;
	ClearSpot();		// spLegion등을 클리어함.
	ResetSpot();
	if( bWin ) {
		// 클리어 후 스팟의 소유를 내껄로 바꾼다.
		SetPlayerOwner( spAccWin->GetidAccount(), spAccWin->GetstrName() );
		SetLevel( spAccWin->GetLevel() );
		XBREAK( reward <= 0 );
//		reward /= 2;	// 그대로 받는걸로 바뀜.
		if( XBREAK(reward < 1000) )
			reward = 1000;
		SetReward( reward );	// 상대방이 쌓아둔 보상의 절반은 내가 먹는다.
		SetScore( spAccWin->GetLadder() );
	} else {
		if( !bRetreat ) {
		}
		// 우리가 상대를 뺏으러 갔지만 졌음.
		// 스팟을 ?상태로 만듬
		SetPlayerOwner( 0, _T("") );
	}
}

