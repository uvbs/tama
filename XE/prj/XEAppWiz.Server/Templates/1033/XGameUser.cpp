#include "stdafx.h"
#include "XGameUser.h"
#include "XClientConnection.h"
#include "XDBASvrConnection.h"
#include "XMain.h"
#include "XPacketCG.h"
#include "XPacketGDB.h"
#include "XGameUserMng.h"
#include "Network/XPacket.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XGameUser>* XMemPool<XGameUser>::s_pPool = NULL;

#define MAKE_PCONNECT( A )		XClientConnection *A = SafeCast<XClientConnection*, XEWinConnectionInServer*>( GetpConnect() )


XGameUser::XGameUser( XEUserMng *pUserMng, XEWinConnectionInServer *pConnect ) 
	: XEUser( pUserMng, pConnect )
{ 
	Init(); 
	m_timer1Min.Set( 60.f );
}

void XGameUser::Destroy()
{
}

void XGameUser::OnDestroy()
{
	XDBASvrConnection::sGet()->SendUpdateAccountLogout( 0, GetidAccount(), 0 );
//	AddLog( XGAME::xULog_Account_Logout, _T( "Logout" ) );
}

// 유저의 정보를 저장한다.
BOOL XGameUser::Save( void )
{	
	return TRUE;
}

inline ID XGameUser::GetidAccount( void ) 
{
	return m_pAccount->GetidAccount();
}
inline LPCTSTR XGameUser::GetstrName( void ) 
{
	return m_pAccount->GetstrName().c_str();
}
inline int XGameUser::GetLevel( void ) 
{
	return m_pAccount->GetLevel();
}

void XGameUser::AddLog( int logtype, const _tstring& strLog )
{
// 	if( int( XGC->m_LogLevel ) > logtype ) {
// 		if( DBA_SVR ) {
// 			DBA_SVR->SendUserLog( GetidAccount(), GetstrName(), logtype, strLog );
// 		}
// 	}
}

void XGameUser::SendDuplicateConnect()
{
	MAKE_PCONNECT( pConnect );	
	pConnect->SendAccountDuplicateLogout();
// 	XPacket ar( (ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT );
//   pConnect->Send( ar );		// 클라이언트로 데이타를 보냄
}

void XGameUser::Send( XPacket& ar )
{
	MAKE_PCONNECT( pConnect );	
  pConnect->Send( ar );		// 클라이언트로 데이타를 보냄
}

void XGameUser::SuccessLogin( XSAccount *pAccount, BOOL bReconnect )
{	
	m_pAccount = pAccount;
	// 클라로 계정정보 보내줌
	if( bReconnect == FALSE )		// 재접상황이면 계정정보는 보내지 않음
	{
		/*CString string1, string2;
		string1.Format( _T("SuccessLogin "));
		string2.Format( _T("%s"), szIP );
		string1 += string2;
		DBLog( USER_CONNECT, (TCHAR*)((LPCTSTR)string1));*/
	}
	else
	{
		/*CString string1, string2;
		string1.Format( _T("SuccessLogin "));
		string2.Format( _T("%s"), szIP );
		string1 += string2;
		DBLog( USER_RECONNECT, (TCHAR*)((LPCTSTR)string1));*/
	}
}
/**
 @brief 로그인에 성공하고 클라로 결과를 보내기전에 해야할일들을 모은다.
*/
void XGameUser::SuccessLoginBeforeSend( XSAccount *pAccount, BOOL bReconnect )
{
	// 어카운트 포인터를 최초 받음.
	m_pAccount = pAccount;
	//
	pAccount->SetpDelegateLevel( this );
}

/**
 @brief 로그인에 성공하고 클라로 결과를 보낸 후에 해야할일을 모은다
*/
void XGameUser::SuccessLoginAfterSend()
{
	if( XBREAK( m_pAccount == nullptr ) )
		return;
}
//Save
/**
 @brief 매 프레임 실행되는 프로세스
*/
void XGameUser::Process( float dt ) 
{
	// 1분타이머 
	if( m_timer1Min.IsOver() ) {
		// 1분마다 처리해야할일이 있다면 이곳에 코딩

		m_timer1Min.Reset();
	}
}

int XGameUser::RecvSample( XPacket& p )
{
	int i0;
	p >> i0;
	// 클라로 되돌려 보냄.
	XPacket ar( (ID)xCL2GS_SAMPLE );
	ar << 1;
	Send( ar );
	return 1;
}
