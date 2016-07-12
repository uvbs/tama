#include "StdAfx.h"
#include "XGameSvrSocket.h"
#include "XFramework/XDBMng2.h"
#include "XGameSvrConnection.h"
#include "XClientConnection.h"
#include "XSocketSvrForClient.h"
#include "XPacketLG.h"
#include "XPacketCL.h"
#include "XMain.h"
#include "server/XSAccount.h"
#include "MainFrm.h"
#include "XGameUserMng.h"

//#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
#define BTRACE(F,...)		(0)

using namespace std;

// #define DBMNG	XDBMng2<XGameSvrConnection>::sGet()
#ifdef _DUMMY_GENERATOR
	#define SIZE_QUEUE		(1024 * 1024 * 50)
#else
	#define SIZE_QUEUE		(1024 * 1024 * 10)
#endif // _DUMMY_GENERATOR

XGameSvrConnection::XGameSvrConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( _T("conn_sockG")
													, MAIN->GetpSocketForGameSvr()
													, socket
													, szIP
													, SIZE_QUEUE )		
{
	Init();
}

void XGameSvrConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
}

BOOL XGameSvrConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	switch( idPacket )
	{
//	case xLPK_REGISTER_ACCOUNT:				RecvRegisterAccount( p );					break;
//	case xLPK_SAVE_ACCOUNT_INFO:			RecvSaveAccountInfo( p );					break;
	case xLS2GS_DID_FINISH_LOAD:			RecvDidFinishLoad( p );						break;
	case xLS2GS_LOGIN_ACCOUNT_INFO:			RecvAccountReadyForConnect( p );			break;
	case xLS2GS_SAVE_DUMMY_COMPLETE:		RecvSaveDummyComplete( p );					break;
	case xLS2GS_ACCOUNT_IS_ALREADY_CONNECT: RecvDuplicateConnectProcess(p);				break;
  case xLS2GS_LOGIN_UNLOCK_FOR_BATTLE:  RecvUnlockLoginForBattle( p );  break;
	case xLS2GS_GAMESVR_INFO:						RecvGameSvrInfo( p ); break;
	case xLS2GS_RELOAD:									RecvReload( p ); break;
	
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%x", idPacket );
		//불량 패킷 숫자를 확인 하고 Disconnect 하는 부분
		//Server <->Server 에서는 사용 하지 않음.
		//		if( ++m_cntDoubt > 5 )
		{
//			MAIN->_DoDisconnectUser( pUser );
			return FALSE;
		}
		break;
	}
	return TRUE;
}

/**
 @brief 게임서버로부터 게임서버의 현재 상태를 받음.
*/
void XGameSvrConnection::RecvGameSvrInfo( XPacket& p )
{
	MAIN->m_fpsFromGameSvr.Process();
	int numClient;
	p >> numClient;
	// this게임서버의 동접자 갱신.
	m_numConnectedClient = numClient;
}

void XGameSvrConnection::RecvDidFinishLoad( XPacket& p )
{
	/*TCHAR szIPExternal[32];*/
	_tstring strIPExternal;
	WORD port, w1;
	p >> port;	m_GameSvrPort = port;
	p >> w1;	SetGSvrID( w1 );
	p >> strIPExternal;
	//p.ReadString( szIPExternal );
	const std::string strcIpExternal = SZ2C( strIPExternal );
	m_strcIPExternal = strcIpExternal;
	const auto widSvr = GetGSvrID();
	auto pSvr = MAIN->GetpSocketForGameSvrConst();
	if( pSvr->IsExist( widSvr ) ) {
		XBREAKF(1, "중복된 게임서버 아이디:idGSvr=%d", widSvr );
		// 게임서버 종료시킴.
		SendExitGameSvr();
		return;
	}
	// 게임서버의 로그인 완료
	// 게임서버소켓에 게임서버커넥션을 추가함.
	MAIN->GetpSocketForGameSvr()->OnLoginedGameSvr( GetThis(), widSvr, strcIpExternal, m_GameSvrPort );
	CONSOLE("GameServer(%d) logined: 외부주소:%s:%d", widSvr, C2SZ(strcIpExternal), port );
	// 게임서버로부터 로그인준비가 끝났음을 통보받았으므로 게임클라이언트들에게 소켓을 개방함.
	MAIN->CreateSocketForClient();
}

void XGameSvrConnection::SendAccountIsAlreadyConnect( ID idConnect, ID idAcc )
{
	MAIN->m_fpsToGameSvr.Process();
	XPacket ar( (ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT );
	ar << idConnect;
	ar << idAcc;
	Send( ar );
}

/**
 @brief 게임서버를 종료시키게 함.
*/
void XGameSvrConnection::SendExitGameSvr()
{
	XPacket ar( (ID)xLS2GS_EXIT_GAMESVR );
	Send( ar );
}

/**
 @brief idConnect로 로그인서버에 접속해있는 클라이언트 연결객체를 얻음.
*/
// XSPXClientConnection XGameSvrConnection::GetspClientConnect( ID idConnect )
// {
// 	auto pSocketSvrForClient = MAIN->GetpSocketForClient();
// 	if( XBREAK( pSocketSvrForClient) )
// 		return nullptr;
// 	auto spConn = pSocketSvrForClient->GetspConn( idConnect );
// 	if( XASSERT(spConn) ) {
// 		return std::static_pointer_cast<XClientConnection>( spConn)
// 	}
// 	return nullptr;
// }

/**
 @brief 게임서버로부터 "계정이 중복접속되어있어 끊었으니 이따 다시 재접해달라"고 요청을 보냄.
 이곳은 로그인서버
*/
void XGameSvrConnection::RecvDuplicateConnectProcess(XPacket& p)
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idaccount = 0;
	ID newidConnect = 0;
//	ID alreadyuseridConnect = 0;
		
	p >> newidConnect;
//	p >> alreadyuseridConnect;	사용하는곳이 없어서 삭제.
	p >> idaccount;

	auto pSocketSvrForClient = MAIN->GetpSocketForClient();
	auto spConnect = pSocketSvrForClient->GetspConnect( newidConnect );
	if( !spConnect )
		return;
	XAUTO_LOCK3( spConnect );
	auto spClientConnect = std::static_pointer_cast<XClientConnection>( spConnect );
	if( XBREAK( spClientConnect == nullptr ) )
		return;
	///< 클라에게 재접 요청
	spClientConnect->SendAccountReconnectTry();
// 	auto pWinConnection = pSocketSvrForClient->FindConnection(newidConnect);
// 	if( !pWinConnection )
// 		return;
// 	auto pClientConnect = SafeCast<XClientConnection*>( pWinConnection );
// 	if ( XBREAK(pClientConnect == nullptr ) )
// 		return;
// 	///< 클라에게 재접 요청
// 	pClientConnect->SendAccountReconnectTry();
}

/**
 @brief 게임서버로부터 idAccount에게 이제 접속해도 된다고 알려줌.
 게임서버의 XLoginConnection::SendLoginAccountInfo()로부터 호출됨.
*/
void XGameSvrConnection::RecvAccountReadyForConnect( XPacket& p )
{
	MAIN->m_fpsFromGameSvr.Process();
	ID idAccount =0;
	ID idConnect = 0;
	_tstring strUUID;
	p >> idAccount;
	p >> idConnect;
	p >> strUUID;
	XBREAK( strUUID.empty() );
	BTRACE( "L:%s:idAcc=%d, uuid=%s", __TFUNC__, idAccount, strUUID.c_str() );

// 	auto pWinConnect = MAIN->GetpSocketForClient()->FindConnection( idConnect );
// 	if( !pWinConnect ) {
// 		XTRACE( "L:%s:idAcc=%d, not found connect=0x%08x", __TFUNC__, idAccount, idConnect );
// 		return;
// 	}
// 	auto pClientConnect = SafeCast<XClientConnection*>( pWinConnect );
	// 다시 건네줘야할 클라이언트 객체가 아직도 있는지 확인.
	auto pSocketSvrForClient = MAIN->GetpSocketForClient();
	auto spConnect = pSocketSvrForClient->GetspConnect( idConnect );
	if( !spConnect )
		return;
	// 현재 커넥션으로 온 게임서버의 정보를 줘야 함.
	const std::string strcIP = GetstrcIPExternal();
	const WORD port = GetGameSvrPort();
	XBREAK( strcIP.empty() );
	XBREAK( port == 0 );
	// 가장 한가한 게임서버를 찾음.
// 	XSPGameSvrConnect spConnectionForGameSvr = MAIN->GetpSocketForGameSvr()->GetFreeConnection();
// 	if( spConnectionForGameSvr ) {
// 		XAUTO_LOCK3( spConnectionForGameSvr );
// 		strcIP = spConnectionForGameSvr->GetstrcIPExternal();
// 		port = spConnectionForGameSvr->GetGameSvrPort();
// 	}
	if( !strcIP.empty() ) {
		// 가장 한가한 게임서버쪽으로 접속하라고 ip등의 정보를 보냄.
		XAUTO_LOCK3( spConnect );
		auto spClientConnect = std::static_pointer_cast<XClientConnection>( spConnect );
		if( XBREAK( spClientConnect == nullptr ) )
			return;
		if( !spClientConnect->IsDisconnected() ) {
			//
			spClientConnect->SendAccountReqLoginInfo( idAccount, strcIP.c_str(), port, strUUID );
			BTRACE( "L:%s:idAcc=%d", __TFUNC__, idAccount );
		}
	} else {
		XBREAKF(1, "이런상황 생기면 안됨");
	}
}

// 더미 저장이 완료되었다.
void XGameSvrConnection::RecvSaveDummyComplete( XPacket& p )
{
	ID idAccount;
	p >> idAccount;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
	MAIN->OnArriveSaveDummy();
}

/**
 @brief idAttacker가 idDefender에 대한 공격이 끝남.
*/
void XGameSvrConnection::RecvUnlockLoginForBattle( XPacket& p )
{
  ID idDefender;
  p >> idDefender;

  auto _spConnect = MAIN->GetpSocketForClient()->GetspConnByIdAcc( idDefender );
  if( _spConnect ) {
		XAUTO_LOCK3( _spConnect );
    auto spConnect = std::static_pointer_cast<XClientConnection>( _spConnect );
    spConnect->SendUnlockLoginForBattle();
  }
//   auto pUserMng = MAIN->GetpSocketForClient()->GetpUserMng();
//   if( XASSERT( pUserMng ) ) {
//     // 현재 그 유저가 로그인대기중이라면 알림
//     auto pUser = pUserMng->GetUserFromAccountID( idDefender );
//     if( pUser ) {
//       auto pConnect = static_cast<XClientConnection*>( pUser->GetpConnect() );
//       if( XASSERT( pConnect ) )
//         pConnect->SendUnlockLoginForBattle();
//     }
//   }
}

/**
 @brief idAcc의 계정정보를 DB에서 읽어오라.
 @param bSessionKeyLogin 세션키로그인(재접속)으로 들어온것이면 true
*/
void XGameSvrConnection::SendLoginAccountInfo( bool bSessionKeyLogin, ID idConnect, ID idAcc )
{
	MAIN->m_fpsToGameSvr.Process();
  XPacket ar( (ID)xLS2GS_LOGIN_ACCOUNT_INFO );
  ar << idConnect;
  ar << idAcc;
  ar << (BYTE)xboolToByte(bSessionKeyLogin);
  ar << (BYTE)0;
  ar << (BYTE)0;
  ar << (BYTE)0;
  Send( ar );
	// 게임서버의 XLoginConnection::RecvLoginAccountInfoLoad()에서 받는다.
}

void XGameSvrConnection::SendNewAccount( ID idConnect
																			, ID idAcc
																			, const _tstring& strUUID
																			, const _tstring& strNickName
																			, const _tstring& strFbUserId )
{
	XPacket ar( (ID)xLS2GS_NEW_ACCOUNT );		// 새계정 생성시 디폴트 데이타를 만들기 위해 이렇게 했는데 맞는지 몰겠음.					
	ar << idConnect;
	ar << idAcc;
	ar << strUUID;
	ar << strNickName;
	ar << strFbUserId;
	Send( ar );
}

#ifdef _DUMMY_GENERATOR
void XGameSvrConnection::SendNewDummyAccount( ID idAcc, const _tstring& strName, int cntCreate )
{
	XPacket ar( (ID)xLS2GS_NEW_DUMMY_ACCOUNT );
	ar << idAcc;
	ar << strName;
	ar << cntCreate;
	Send( ar );
}
#endif // _DUMMY_GENERATOR

void XGameSvrConnection::RecvReload( XPacket& p )
{
	std::string strTag;
	p >> strTag;
	if( strTag == "tags" ) {
		if( CONSOLE_MAIN )
			CONSOLE_MAIN->LoadTags();
		XEServerMain::sGet()->ShowLogFilter( "tags" );
	} else
	if( strTag == "idacc" ) {
		if( CONSOLE_MAIN )
			CONSOLE_MAIN->LoadidAcc();
		XEServerMain::sGet()->ShowLogFilter( "idacc" );
	} else {
		CONSOLE( "RecvReload: 알수없는 reload tag:%s", C2SZ( strTag.c_str() ) );
	}
}


