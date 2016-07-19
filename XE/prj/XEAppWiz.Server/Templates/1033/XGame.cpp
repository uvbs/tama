#include "stdafx.h"
#include "XGame.h"
#include "XMain.h"
#include "server/XSAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGame *XGame::s_pGame = nullptr;

XGame* XGame::sCreateInstance() {
	s_pGame = new XGame;
	return s_pGame;
}

XGame::XGame()
{
	m_listAccountFromLogin.Create( MAIN->GetmaxConnect() );
	m_timerMain.Start();
}

void XGame::Destroy()
{
	XLIST2_LOOP( m_listAccountFromLogin, XSAccount*, pAccount ) {
		// 이제 XAccount는 레퍼런스 객체이므로 SAFE_RELEASE_REF로 해제해야함.
		SAFE_RELEASE_REF( pAccount );
	} END_LOOP;
}

void XGame::Create()
{
	XGameCommon::Create();
	XGameCommon::CreateCommon();
	XBREAK( XGlobalConst::sGet()->IsError() );    // 스레드내부에서 로딩하지 않도록 미리 읽음.
}


void XGame::FrameMove( float dt )
{
	m_timerMain.Update( dt );
}

XSAccount* XGame::FindLoginAccount( ID idAccount )
{
	XLIST2_LOOP( m_listAccountFromLogin, XSAccount*, pAccount )
	{
		if( pAccount->GetidAccount() == idAccount )
			return pAccount;
	} END_LOOP;
	return NULL;
}

XSAccount* XGame::FindLoginAccount( LPCTSTR szID )
{
	XLIST2_LOOP( m_listAccountFromLogin, XSAccount*, pAccount ) {
		if( _tcsicmp( pAccount->GetstrID(), szID ) == 0 )
			return pAccount;
	} END_LOOP;
	return NULL;
}

void XGame::DelLoginAccount( ID idAccount )
{
	// idAccount를 가진 계정을 찾는다.
	XSAccount *pExist = nullptr;
	XLIST2_LOOP( m_listAccountFromLogin, XSAccount*, pAccount ) {
		if( idAccount == pAccount->GetidAccount() ) {
			pExist = pAccount;
			break;
		}
	} END_LOOP;
	// 있으면 삭제한다.
	if( pExist )  {
		m_listAccountFromLogin.Del( pExist );
		SAFE_RELEASE_REF( pExist );
	}
}

BOOL XGame::AddLoginAccount( XSAccount *pNew )
{
	// 기존에 이미 같은 idAcc계정이 있으면 삭제하고 다시 추가한다.
	DelLoginAccount( pNew->GetidAccount() );
	//
	m_listAccountFromLogin.Add( pNew );
	return TRUE;
}