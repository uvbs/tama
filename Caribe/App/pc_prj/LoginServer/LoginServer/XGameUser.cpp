#include "stdafx.h"
#include "XGameUser.h"
#include "XClientConnection.h"
#include "XMain.h"
#include "XPacketCG.h"
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

#define MAKE_PCONNECT( A ) \
	XBREAK( GetspConnect() == nullptr ); \
	auto A = SafeCast<XClientConnection*>( GetspConnect().get() );


XGameUser::XGameUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect ) 
	: XEUser( pUserMng, spConnect )
{ 
	Init(); 
}

void XGameUser::Destroy()
{
//	Save( );
}

// 로그인서버로 계정정보를 보낸다
BOOL XGameUser::Save( void )
{	
//	DBMNG->RequestSaveAccountAll ( GetidUser(), GetpAccount (), NULL,  GetpConnect()->GetszIP());
	return TRUE;
}

// void XGameUser::SendDuplicateConnect()
// {
// 	MAKE_PCONNECT( pConnect );	
// 	XPacket ar((ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT);
//     pConnect->Send( ar );		// 클라이언트로 데이타를 보냄
// }

void XGameUser::Send( XPacket& ar )
{
	MAKE_PCONNECT( pConnect );	
  pConnect->__SendForXE( ar );		// 클라이언트로 데이타를 보냄
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

//Save
void XGameUser::OnDestroy()
{	
}

void XGameUser::Process( float dt ) 
{
	// 스태미너가 갱신되었으면 클라이언트에 업데이트값을 보내준다.
/*
	if( GetpAccount()->UpdateStamina() )
		if( GetpConnect() )
		{
			XClientConnection *pConnect = static_cast<XClientConnection*>( GetpConnect() );
			pConnect->SendUpdateStamina( GetpAccount() );
		}
*/
}

