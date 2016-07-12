#include "stdafx.h"
#include "XGame.h"
#include "XMain.h"
#include "server/XSAccount.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGame *XGame::s_pGame = nullptr;
XGame* XGame::sCreateInstance( void ) {
	XBREAK( s_pGame != nullptr );
	s_pGame = new XGame;
	return s_pGame;
}

XGame::XGame()
{
	Init();
//	m_listAccountFromLogin.Create( MAIN->GetmaxConnect() );
	m_timerMain.Start();
	m_timerSec.Set( 1.f );
}

void XGame::Destroy()
{
// 	for( auto itor : m_mapAccFromLogin ) {
// 		XSPSAcc pAcc = (itor).second;
// 		SAFE_RELEASE_REF( pAcc );
// 	}
	m_mapAccFromLogin.clear();
// 	XLIST2_LOOP( m_listAccountFromLogin, XSPSAcc, pAccount )
// 	{
// 		// 이제 XAccount는 레퍼런스 객체이므로 SAFE_RELEASE_REF로 해제해야함.
// 		SAFE_RELEASE_REF( pAccount );
// 	} END_LOOP;
}

void XGame::Create( void )
{
	// 클라/서버 공통 데이타 로딩
	XGameCommon::Create();
	XGameCommon::CreateCommon();
  XBREAK( XGlobalConst::sGet()->IsError() );    // 스레드내부에서 로딩하지 않도록 미리 읽음.
}


void XGame::FrameMove( float dt )
{
	if( m_timerSec.IsOver() ) {
		m_timerSec.Reset();
		// 주기적으로 검사해서 너무 오래된 계정은 날림.
		UpdateLoginAcc();
	}
	m_timerMain.Update( dt );
}

/**
 @brief 주기적으로 검사해서 너무 오래된 계정은 날림.
*/
void XGame::UpdateLoginAcc()
{
	for( auto itor = m_mapAccFromLogin.begin(); itor != m_mapAccFromLogin.end(); ) {
		auto& acc = itor->second;
		XBREAK( acc.m_timerLife.IsOff() );
		if( acc.m_timerLife.IsOver() ) {
			m_mapAccFromLogin.erase( itor++ );
		} else {
			++itor;
		}
	}
}

XGame::xAccLogin* XGame::FindLoginAccount( ID idAccount )
{
	auto itor = m_mapAccFromLogin.find( idAccount );
	if( itor != m_mapAccFromLogin.end() ) {
		return &(itor->second);
// 		xAccLogin& acc = itor->second;
// 		return acc.m_spAcc;
	}
	return nullptr;
}
/**
 @brief ID(이멜)로 찾는버전
*/
XGame::xAccLogin* XGame::FindLoginAccount( const _tstring& strID )
{
	auto itor = m_mapAccFromLoginBystrID.find( strID );
	if( itor != m_mapAccFromLoginBystrID.end() ) {
		return &(itor->second);
// 		auto& acc = itor->second;
// 		return acc.m_spAcc;
	}
	return nullptr;
}

/**
 @brief 맵에서 뺌. 삭제는 아님.
*/
void XGame::DelLoginAccount( ID idAccount )
{
	m_mapAccFromLogin.erase( idAccount );
}

/**
 @brief 
 @param pNew null이 올수도 있음.
*/
bool XGame::AddLoginAccount( ID idAcc, XSPSAcc pNew )
{
	XBREAK( idAcc == 0 );
	bool bOk = true;
//	const ID idAcc = pNew->GetidAccount();
	// 기존에 이미 같은 idAcc계정이 있으면 삭제하고 다시 추가한다.
	auto itor = m_mapAccFromLogin.find( idAcc );
	if( itor != m_mapAccFromLogin.end() ) {
		m_mapAccFromLogin.erase( itor );
		bOk = false;
	}
	// 맵에 추가.
	xAccLogin acc;
	acc.m_timerLife.Set( 60.f * 60.f );
	acc.m_cntTry = 0;
	acc.m_spAcc =  pNew;
	m_mapAccFromLogin[ idAcc ] = acc;
	return bOk;
}