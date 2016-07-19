#include "stdafx.h"
#include "XClientConnection.h"
#include "XLoginConnection.h"
#include "XPacketCG.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketForClient.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XClientConnection>* XMemPool<XClientConnection>::s_pPool = NULL;

#define XVERIFY_BREAK( EXP )	{	\
	if( XBREAK( EXP ) )	{	\
	OnInvalidVerify(); \
	return;	\
	}	\
}

#define XVERIFY_BREAKF( EXP, F, ... )	{	\
	if( XBREAKF( EXP, F, ##__VA_ARGS__ ) )	{	\
	OnInvalidVerify(); \
	return;	\
	}	\
}

// 불량데이타 보내는 유저에 대한 처리
void XClientConnection::OnInvalidVerify()
{
#pragma message("check this")
	CONSOLE( "-------------------------------invalid client: %s", GetszIP() );	
	// 클라에 에러메시지창 보내고
	SendInvalidVerify();
	// 즉각 소켓 닫아버림.
	DoDisconnect(); 
}
/**
 @brief 
*/
XClientConnection::XClientConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( MAIN->GetpSocketForClient(), socket, szIP, 0xffff )		
{
	Init();
	XTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
}
/**
 @brief		클라이언트로부터 패킷을 받음.
*/
BOOL XClientConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{	
	TRACE( "recv packet: %d - \n", idPacket );
	int retv = 1;
	//
	if( idPacket == xCL2GS_ACCOUNT_REQ_LOGIN ) {
		// XGameUser가 아직 안만들어진 커넥션으로 들어오는 패킷 처리
		RecvLogin(p);
	} else {
		// XGameUser가 만들어진 이후에 커넥션으로 들어오는 패킷 처리
		auto pUser = SafeCast<XGameUser*>( GetpUser() );
		if (XBREAK(pUser == nullptr)) {
			CONSOLE("idPacket=%d, idConnect=0x%08x", idPacket, GetidConnect());
			OnInvalidVerify();
// 			DoDisconnect();
			return FALSE;
		}
		//
		switch( idPacket ) {
		case xCL2GS_SAMPLE:		retv = pUser->RecvSample( p );	break;		// 
		default: {
			// 알수없는 패킷이 날아왔다.
			XBREAKF( 1, "unknown packet 0x%8x", idPacket );
			DoDisconnect();
			return FALSE;
			} break;
		}
	}
	TRACE( "recv packet: end" );
	if( retv == 0 )
		OnInvalidVerify();
	return TRUE;	
}

void XClientConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
	if( GetpUser() ) {
		auto pUser = SafeCast<XGameUser*>( GetpUser() );
		if( error == XENetworkConnection::xERR_PACKET_SIZE_TO_LARGE ) {
			CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.",  GetszIP(), param1 );
		}
	} else {
		CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.", GetszIP(), param1 );
	}
}
/**
 @brief 게임서버로 최초 로그인
 로그인서버에서 인증이 끝난 후 게임서버로 최초 진입.
*/
void XClientConnection::RecvLogin( XPacket& p )
{
	ID idAccount;
	int verCGPK;
	TCHAR szSessionKey[256];
	p >> verCGPK;		// 클라의 프로토콜 버전(클라<->게임서버간)
	p >> idAccount;
	p.ReadString( szSessionKey );
	if( XLoginConnection::sGet()->IsDisconnected() )
		CONSOLE("로그인 서버의 연결이 끊어졌습니다.");
	// 로긴서버로부터 받은 계정중에 내 아이디가 있는지 찾는다.
	XSAccount *pAccount = XGame::sGet()->FindLoginAccount( idAccount );
	if( pAccount ) {
		pAccount->SetverCGPK( verCGPK );
		// 유저객체 만들고 로그인 성공시킴
		auto pGameUser = CreateUserObj( idAccount, pAccount, FALSE );
		XGame::sGet()->DelLoginAccount( idAccount );
		//
		int nTest = 0;
		XPacket ar( (ID)xCL2GS_ACCOUNT_SUCCESS_LOGIN );
		XTRACE("%s", __TFUNC__);
		// reserved
		ar << nTest;
		pAccount->Serialize( ar );
		Send( ar );
		///< 클라에 계정정보를 보낸후 처리할일들을 한다.
		pGameUser->SuccessLoginAfterSend();
	} else
	{
		// 로긴서버로부터 아직 계정정보가 도착안했다
		OnAccountInfoNotYetFromLoginSvr( idAccount );
	}
}
/**
 @brief 
*/
XGameUser* XClientConnection::CreateUserObj( ID idAccount, XSAccount *pAccount, BOOL bReconnect )
{
	XEUser *pUser = XEWinConnectionInServer::CreateAddUser( pAccount );
	if( pUser ) {
		auto pGUser = SafeCast<XGameUser*>( pUser );
		pGUser->SuccessLoginBeforeSend( pAccount, bReconnect );
		return pGUser;
	}
	return nullptr;
}

/**
 @brief 로긴서버로부터 아직 계정정보가 도착안했다
*/
void XClientConnection::OnAccountInfoNotYetFromLoginSvr( ID idAccount )
{
#ifdef _DEV_SERVER
	CONSOLE( "idAcc:%d 의 계정정보가 로긴서버로부터 아직 도착하지 않아 접속끊음.", idAccount );
	// 일단은 별다른 처리없이 즉각 끊어버림.
#endif
	SendLoginFailedGameSvr( 0 );
	DoDisconnect();
}
/**
 @brief 여러가지 이유로 게임서버에 접속실패함.
*/
void XClientConnection::SendLoginFailedGameSvr( DWORD idMsg )
{
	XPacket ar((ID)xCL2GS_LOGIN_FAILED_GAMESVR);
	ar << idMsg;
	Send( ar );
}

// 접속해제 요청이 들어온 커넥션을 끊음.
void XClientConnection::RecvDisconnectMe( XPacket& ar )
{
	DoDisconnect();
}

void XClientConnection::SendInvalidVerify()
{
	XPacket ar( (ID)xCL2GS_INVALID_VERIFY );
	Send( ar );

}

void XClientConnection::SendAccountDuplicateLogout()
{
	XPacket ar( (ID)xCL2GS_ACCOUNT_DUPLICATE_LOGOUT );
	Send( ar );		// 클라이언트로 데이타를 보냄
}