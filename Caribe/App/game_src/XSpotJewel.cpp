#include "stdafx.h"
#include "XSpotJewel.h"
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
XSpotJewel::XSpotJewel( XWorld *pWorld, XPropWorld::xJEWEL *pProp )
	: XSpot( pProp, XGAME::xSPOT_JEWEL, pWorld ) 
{
	Init();
	if( pProp ) {
		SetMaxLocal( pProp );
	}
}
void XSpotJewel::Destroy()
{
}
void XSpotJewel::OnCreateNewOnServer( XSPAcc spAcc )
{
	if( GetpProp()->idx == 0 )
	{
		// 말뚝광산이므로 첨부터 내꺼임
		SetPlayerOwner( spAcc->GetidAccount(), spAcc->GetstrName() );
	}
}

void XSpotJewel::SetMaxLocal( XPropWorld::xJEWEL *pProp )
{
	XBREAK( pProp == nullptr );
	m_lsLocal.maxSize = pProp->GetProduce() * 60.f * 8.f;
}
void XSpotJewel::Serialize( XArchive& ar ) 
{
	XSpot::Serialize( ar );
	ar << (BYTE)GetpProp()->idSpot;
	ar << (BYTE)m_levelMine;
	ar << (WORD)m_Defense;
	ar << m_idOwner;
	ar << m_idMatchEnemy;
	ar << GetstrName();
	ar << m_strcFbUserId;
	if( ar.IsForDB() )
		ar << _tstring();
	else
		ar << m_strHello;
	ar << (BYTE)m_idxLegion;
	ar << (BYTE)0; //(BYTE)xboolToByte( m_bSimul );
	ar << (BYTE)m_lvMatchEnemyForSimul;
	ar << (BYTE)0;
	ar << (DWORD)m_secLastEvent;
	m_timerCalc.Serialize( ar );
	m_timerOwn.Serialize( ar );
	ar << m_lsLocal.numCurr;
	ar << m_lsLocal.maxSize;
//#ifdef _DEV
	ar << m_strNameByMatchEnemyForSimul;
}

BOOL XSpotJewel::DeSerialize( XArchive& ar, DWORD ver )
{
#ifndef _DUMMY_GENERATOR
#endif // not _DUMMY_GENERATOR
	XSpot::DeSerialize( ar, ver );
//	ID idProp;
	BYTE b0;
	WORD w0;
	ar >> b0;	//idProp = b0;
	ar >> b0;	m_levelMine = b0;
	ar >> w0;	m_Defense = w0;
	// null이 나올수도 있으나 그냥 리턴하지말고 남은 시리얼라이즈를 모두 끝내야함.
	// 스팟이 필요에의해 삭제될수 있기때문.
	ar >> m_idOwner;
	ar >> m_idMatchEnemy;
	_tstring strName;
	ar >> strName;
	SetstrName( strName.c_str() );
	if( ver >= 28 ) {
		_tstring strTemp;
		ar >> strTemp;	m_strcFbUserId = SZ2C( strTemp );
	}
	if( ver >= 28 )
		ar >> m_strHello;
	ar >> b0;	m_idxLegion = b0;
	ar >> b0;   //m_bSimul = (b0!=0);
  ar >> b0;	m_lvMatchEnemyForSimul = b0;
	ar >> b0;
	DWORD dw0;
	ar >> dw0;	m_secLastEvent = (double)dw0;
	int _verTimer = 1;
	if( ver < 22)
		_verTimer = 0;
	m_timerCalc.DeSerialize( ar, _verTimer );
#ifdef _CLIENT
	// 클라에선 주기적 타이머로 쓴다.
	m_timerCalc.Set( XGAME::SEC_REGEN );
#endif
	m_timerOwn.DeSerialize( ar, _verTimer );
	ar >> m_lsLocal.numCurr;
	ar >> m_lsLocal.maxSize;
	// 8시간분량크기에서 1시간분량 크기로 줄임.
	if( GetpProp() )
		SetMaxLocal( GetpProp() );
	if( ver >= 25 ) {
		ar >> m_strNameByMatchEnemyForSimul;
	}
	return TRUE;
}

void XSpotJewel::SerializeForBattle( XArchive* pOut, const XParamObj2& param )
{
	XSpot::SerializeForBattle( pOut, param );
	*pOut << m_levelMine;
	*pOut << m_Defense;
}

void XSpotJewel::DeSerializeForBattle( XArchive& arLegion, XArchive& arAdd, XSPAcc spAcc )
{
	XSpot::DeSerializeForBattle( arLegion, arAdd, spAcc );
	arAdd >> m_levelMine;
	arAdd >> m_Defense;
}


// 스팟을 유저(플레이어 자신포함)의 소유로 만든다.
void XSpotJewel::SetPlayerOwner( ID idAcc, LPCTSTR szName ) 
{
	// 이미 소유하고 있던 자원지면 다시 처리하지 않음.
	if( m_idOwner == idAcc )
		return;
	m_idOwner = idAcc;
  SetPower( 0 );
	SetstrName( szName );
	if( GetLevel() == 0 )
		SetLevel(1);
	if( m_levelMine == 0 )
		m_levelMine = 1;
	if( m_Defense == 0 )
		ResetDefense( GetLevel() );
//	m_lootJewel = 0;
	m_strHello.clear();
	m_strcFbUserId.clear();
#ifdef _CLIENT
	// 리소스 리젠 타이머를 세팅함.
	m_timerCalc.Set( XGAME::SEC_REGEN );
#else
	// 소유타이머를 작동시킴
	m_timerCalc.Start();
	m_timerOwn.Start();
#endif
}
#ifdef _DEV
/**
 @brief 전투승리로 매치상대와 주인이 바뀜.
 @note 플레이어의 상대를 주인으로 바꾸는것이므로 정상적인 플레이에선 사용할일이 없는 함수.
*/
void XSpotJewel::ChangeOwnerByWinForDummy()
{
	XBREAK( GetLevel() == 0 );
	XBREAK( m_idMatchEnemy == 0 );
 	XBREAK( m_strNameByMatchEnemyForSimul.empty() );
 	XBREAK( m_lvMatchEnemyForSimul == 0 );
	m_idOwner = m_idMatchEnemy;			// 새주인
	SetstrName( m_strNameByMatchEnemyForSimul.c_str() );	// 새주인 이름
	SetLevel( m_lvMatchEnemyForSimul );			// 새 스팟 레벨
	if( m_levelMine < 50 )
		++m_levelMine;
	ResetDefense( GetLevel() );
	// 소유타이머를 리셋시킨다.
	m_timerCalc.Start();
	// 정산타이머를 리셋시킨다.
	m_timerOwn.Start();
}
#endif // _DEV

void XSpotJewel::CalculateResource( ID idAcc, xSec secAdjustOffline )
{
	CalculateResourceByPassTime( idAcc, (xSec)m_timerCalc.GetsecPassTime(), secAdjustOffline );
	// 정산 타이머는 다시 리셋
	m_timerCalc.Reset();
}
void XSpotJewel::CalculateResourceByPassTime( ID idAcc, xSec secPassTime, xSec secAdjustOffline ) 
{
	if( m_idOwner == idAcc )		// 이거 해야 하나?
	{
		float min = (secPassTime - secAdjustOffline) / 60.f;
		float amount = GetpProp()->GetProduce() * min;
		DoStackToLocal( amount, true );
	}
}

/**
 지역창고에 있던 자원을 중앙창고로 옮기고 지역창고는 클리어 시킨다.
*/
void XSpotJewel::MoveResourceLocalToMain( XSPAcc spAccount, 
				XArrayLinearN<XGAME::xRES_NUM, XGAME::xRES_MAX> *pOutAry/*=nullptr*/ )
{
	XGAME::xRES_NUM resNum;
	spAccount->AddResource( XGAME::xRES_JEWEL, (int)m_lsLocal.numCurr );
	resNum.type = XGAME::xRES_JEWEL;
	resNum.num = m_lsLocal.numCurr;
	pOutAry->Add( resNum );
	m_lsLocal.Clear();
}


void XSpotJewel::Process( float dt ) 
{
	XSpot::Process( dt );
	// 소유 타이머의 델리게이트
#ifdef _CLIENT
	// 내소유고 타이머가 안켜져있으면 타이머를 켠다.
	if( ACCOUNT->GetidAccount() == m_idOwner &&
		m_timerCalc.IsOff() )
		m_timerCalc.Set( XGAME::SEC_REGEN );
	if( ACCOUNT->GetidAccount() != m_idOwner &&
		m_timerCalc.IsOn() )
		m_timerCalc.Off();
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
 보석광산의 정산은 만약 누군가가 뺏었다면 뺏은 즉시 정산을 시켜저장 해버리므로
 원래 광산주인이 접속을 할때 따로 정산할 필요가 없다.(이미 뺏긴상태이므로)
 idAcc: 스팟이 속한 계정주인
*/
void XSpotJewel::OnAfterDeSerialize( XWorld *pWorld, XDelegateSpot *pDelegator, ID idAcc, xSec secLastCalc )
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
		xSec secPassTime = (XTimer2::sGetTime() - secLastCalc);
		// 오프라인 동안에 뺏겼을 경우 그 당시 이미 정산되어 DB에 저장되어 있어야 한다.
		if( m_idOwner == idAcc )
		{
			// 광산이 계정주인의 것일때만 정산한다.
//			m_timerCalc.On();	// 간혹 정산타이머가 꺼진채 저장된 경우를 위해
			XBREAK( m_timerCalc.GetsecPassTime() > 0 );	
// 			m_timerCalc.Reset();
// 			m_timerCalc.AddTime( secPassTime );
			CalculateResourceByPassTime( idAcc, secPassTime, 0 );
		}
	}
#endif // SERVER
}


void XSpotJewel::ClearSpot()
{
	XSpot::ClearSpot();
	m_idOwner = 0;
	m_timerOwn.Off();
	m_timerCalc.Off();
	m_Defense = 0;
	m_idxLegion = 0;
	m_secLastEvent = 0;
	m_lsLocal.Clear();
	m_strHello.clear();
	m_strcFbUserId.clear();
}

void XSpotJewel::SetMatch( const XGAME::xJewelMatchEnemy& infoMatch ) 
{
	XBREAK( GetidOwner() != infoMatch.m_idAcc );
	SetLevel( infoMatch.m_lvAcc );
	SetstrName( infoMatch.m_strName );
	int lvMine = infoMatch.m_LvMine;
	if( lvMine <= 0 )
		lvMine = 1;
	m_levelMine = lvMine;
	if( infoMatch.m_Defense <= 0 )
		ResetDefense( infoMatch.m_lvAcc );
	//		SetlootJewel( infoMatch. );
	SetPower( infoMatch.m_Power );
	SetScore( infoMatch.m_Ladder );
	if( !infoMatch.m_strFbUserId.empty() )
		m_strcFbUserId = SZ2C(infoMatch.m_strFbUserId);
	if( !infoMatch.m_strHello.empty() )
		m_strHello = infoMatch.m_strHello;
	if( infoMatch.m_idxLegion >= 0 )
		m_idxLegion = infoMatch.m_idxLegion;
	if( infoMatch.m_lootJewel )
		SetlootJewel( infoMatch.m_lootJewel );
}

void XSpotJewel::OnAfterBattle( XSPAcc spAccWin, ID idAccLose, bool bWin, int numStar, bool bRetreat )
{
	if( bWin ) {
		ClearSpot();
		// 거점레벨 올린다. 방어도 리셋한다.
		SetPlayerOwner( spAccWin->GetidAccount(), spAccWin->GetstrName() );
		SetLevel( spAccWin->GetLevel() );
		AddLevelMine( 1 );
		// 이겼으므로 방어도 다시 리셋
		ResetDefense( spAccWin->GetLevel() );
	} else {
		if( !bRetreat ) {
			// 공략에 실패했으므로 방어도만 다운시킨다.
			DoDefenseDamage();
		}
	}
}
void XSpotJewel::OnSpawn( XSPAcc spAcc )
{
	XSpot::OnSpawn( spAcc );
//	DoDropRegisterEquip( spAcc->GetLevel() );
}

bool XSpotJewel::IsEnemy() const
{
	auto spOwner = GetspOwner().lock();
	if( spOwner )
		return spOwner->GetidAccount() != m_idOwner;
	return true;
}
/**
 @brief 방어도를 하락시킨다.
*/
void XSpotJewel::DoDefenseDamage()
{
	int sub = 1;
	AddDefense( -sub );
}

/**
 @brief 광산의 업데이트를 위해 주요정보들을 아카이브에 담는다.
*/
void XSpotJewel::GetJewelInfo( XGAME::xJewelInfo* pOut )
{
	pOut->m_idxJewel = GetpProp()->idx;
	pOut->m_idOwner = m_idOwner;
//	pOut->m_idMatchEnemy = 
	pOut->m_strName = GetstrName();
	pOut->m_LevelMine = m_levelMine;
	pOut->m_Defense = m_Defense;
}

