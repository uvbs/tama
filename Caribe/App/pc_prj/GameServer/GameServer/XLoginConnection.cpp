#include "stdafx.h"
#include "XLoginConnection.h"
#include "server/XSAccount.h"
#include "XMain.h"
#include "XFramework/server/XWinConnection.h"
#include "XPacketLG.h"
#include "XPacketGDB.h"
#include "XSocketSvrForClient.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XGame.h"
#include "XDBASvrConnection.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
//#define BTRACE(F,...)		(0)

#ifdef _DUMMY_GENERATOR
	#define SIZE_QUEUE		(1024 * 1024 * 50)
#else
	#define SIZE_QUEUE		(1024 * 1024 * 10)
#endif // _DUMMY_GENERATOR
//
XLoginConnection::XLoginConnection()
	: XESocketClientInServer( SIZE_QUEUE )		// 싱글톤으로 바뀔예정으므로 생성자 파라메터를 없앰.
{
	Init();
}

void XLoginConnection::OnLogined( void )
{
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	XConsole( _T( "로그인서버 연결됨." ) );
#endif
}

void XLoginConnection::OnDisconnect( void )
{
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	XConsole(_T("로그인서버 연결 끊김."));
#endif
//	XBREAK(1);
}


BOOL XLoginConnection::Send( XPacket& ar )
{
#ifdef _XPACKET_BUFFER
	// connect()되고 로긴서버로부터 첫번째 확인패킷이 와야 TRUE가 된다.
	// 연결이 끊어지면 FALSE가 된다.
	if( GetbReadyServer() == FALSE )
	{
		// 로긴서버와의 접속이 끊겼으면 연결될때까지 버퍼에 잠시 쌓아둔다.
		XPacketMem *pPacketMem = new XPacketMem( ar );
		m_listPacketToLogin.Add( pPacketMem );
		return FALSE;
	}
#endif // _XPACKET_BUFFER
	//
	return XESocketClientInServer::Send( ar );
}

BOOL XLoginConnection::GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes )
{
	int size = m_listPacketToLogin.size();
	if( size )
	{
		int bytes = 0;
		XLIST_LOOP( m_listPacketToLogin, XPacketMem*, pPacketMem )
		{
			bytes += pPacketMem->GetSize();
		} END_LOOP;
		*pOutNum = size;
		*pOutBytes = bytes;
		return TRUE;
	}
	*pOutNum = 0;
	*pOutBytes = 0;
	return FALSE;
}

void XLoginConnection::FlushSendPacket( void )
{
	XBREAK( IsDisconnected() == TRUE );
	int size = m_listPacketToLogin.size();
	if( size == 0 )
		return;
	CONSOLE("로긴서버가 끊어져 못보낸 패킷들을 보내기 시도......%d개", size );
	int bytes = 0;
	XLIST_LOOP( m_listPacketToLogin, XPacketMem*, pPacketMem )
	{
		XPacket ar( pPacketMem );
		bytes += pPacketMem->GetSize();
		XESocketClientInServer::Send( ar );
		SAFE_DELETE( pPacketMem );
	} END_LOOP;
	m_listPacketToLogin.Clear();
	CONSOLE("패킷 방출 완료......%d bytes", bytes );
}


//패킷 처리
BOOL XLoginConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	int packetid =  (int)idPacket;
	XTRACE("G:XLoginConnection:recv_packet=%d", packetid );
	switch( packetid )
	{
	case xLS2GS_NEW_ACCOUNT:				RecvNewAccount( p );	break;
	case xLS2GS_NEW_DUMMY_ACCOUNT:			RecvNewDummyAccount( p );	break;
	case xLS2GS_ACCOUNT_IS_ALREADY_CONNECT:	RecvIsAleadyConnectAccount ( p );	break;
	case xLS2GS_LOGIN_ACCOUNT_INFO:			RecvLoginAccountInfoLoad ( p );		break;
	case xLS2GS_EXIT_GAMESVR:	RecvExitGameSvr( p ); break;
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		break;
	}

	return TRUE;
}

void XLoginConnection::RecvSVRConnectSuccess( XPacket& p )
{
//	XPacket ar;
//	ar << (ID)xLS2GS_PK_CONNECT_SUCCESS;
//	ar << (ID)XEnv::sGet()->GetGSvrID();
//	ar << (ID)MAIN->GetmaxConnect();
//	Send( ar );
}


void XLoginConnection::SendAccountIsAlreadyConnect( ID idConnectNew, ID idAcc )
{
	MAIN->m_fpsToLogin.Process();
	XPacket ar( (ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT );
	ar << idConnectNew;
//	ar << idConnectAlready;	// 사용하는곳이 없어서 삭제.
	ar << idAcc;
	Send( ar );
}
/**
 @brief 로그인서버로부터 이미 접속되어있는 계정인지 확인요청을 받음
 이곳은 게임서버.
*/
void XLoginConnection::RecvIsAleadyConnectAccount ( XPacket& p )
{
	MAIN->m_fpsFromLogin.Process();
	ID	idAccount = 0;
	ID newidConnect = 0;
// 	ID alreadyuseridConnect = 0;

	p >> newidConnect;		// 로그인서버의 클라이언트 커넥션 아이디
	p >> idAccount;

	if( XGAME_USERMNG ) {		
		XSPGameUser spUser = XGAME_USERMNG->GetspUser( idAccount );
		if( spUser ) {
			// 이미 idAcc계정이 접속되어있음.
			// 클라가 앱을 종료하도록 보냄.
			spUser->SendDuplicateConnect();
			// 클라가 안끊거나 못끊는상황일수도 있으므로 서버에서도 자체적으로 끊음.
			if( spUser->GetspConnect() )
				spUser->GetspConnect()->DoAsyncDestroy(1);		// 바로 끊으니까 클라에서 못받아서 1초뒤 비동기로 끊게 함.
// 			ID alreadyuseridConnect = spUser->GetidConnect();
// 			if (alreadyuseridConnect != newidConnect) {				// ??
// 				// 클라가 앱을 종료하도록 보냄.
// 				spUser->SendDuplicateConnect();
// 			} else {
// 				XBREAK(1);		// 이런경우가 있음?
// 				// 정상로그아웃 시킴.
// 				DBA_SVR->SendUpdateAccountLogout( newidConnect, alreadyuseridConnect, idAccount );
// 			} 
			_tstring strLog = _T("Duplicate Logout( ");
			strLog += spUser->GetspConnect()->GetszIP();
			strLog += _T(" )");
			spUser->AddLog(XGAME::xULog_Account_Duplicate_connect, strLog);
			// 다시 로그인서버로 돌려보냄.(클라에게 재접을 요청함)
			BTRACE( "G0:%s: idAcc=%d", __TFUNC__, idAccount );
			SendAccountIsAlreadyConnect( newidConnect/*, alreadyuseridConnect*/, idAccount );
		} else {
			// 실제로 user가 없으므로 정상로그아웃으로 처리.
			//접속한것으로 알고 있는데 실제 유저가 없으므로 이런 경우는 직접 LastServerid = 0 으로 바꿔 줘야 겠지?
			BTRACE( "G1:%s: idAcc=%d", __TFUNC__, idAccount );
			XDBASvrConnection::sGet()->SendUpdateAccountLogout( newidConnect, idAccount );
		}
	}
}
/**
 @brief Login에게 접속 해도 된다고 알려 준다.
*/
void XLoginConnection::SendLoginAccountInfo( ID idAcc, ID idConnect, const _tstring& strUUID )
{
	MAIN->m_fpsToLogin.Process();
	XPacket ar( (ID)xLS2GS_LOGIN_ACCOUNT_INFO );
	ar << idAcc;
	ar << idConnect;
	ar << strUUID;
	Send( ar );
}

/**
 @brief DB에서 idAccount의 계정정보를 읽어오라는 요청
*/
void XLoginConnection::RecvLoginAccountInfoLoad ( XPacket& p )
{
	MAIN->m_fpsFromLogin.Process();
	ID	idAccount = 0;
	ID idConnect = 0;
	BYTE b0;
	p >> idConnect;
	p >> idAccount;	
	p >> b0;  bool bSessionKeyLogin = (b0 != 0);
	p >> b0 >> b0 >> b0;
	XTRACE( "%s:idAcc=%d", __TFUNC__, idAccount );
	
	if( XGAME_USERMNG ) {
		auto spUser = XGAME_USERMNG->GetspUser( idAccount );
		if( spUser ) {
			XBREAK(1);		// <<< 여기 들어오는 경우가 있음?
			spUser->SendDuplicateConnect( );
			if( spUser->GetspConnect() )
				spUser->GetspConnect()->DoAsyncDestroy( 1 );		// 바로 끊으니까 클라에서 못받아서 1초뒤 비동기로 끊게 함.
			// 클라에게 재접을 요청함.
			SendAccountIsAlreadyConnect( idConnect, idAccount );
		} else {
			XDBASvrConnection::sGet()->SendLoginAccountInfoLoad( bSessionKeyLogin, idConnect, idAccount );
		}
	}
}


/**
 새계정이 생성됨
*/
void XLoginConnection::RecvNewAccount( XPacket& p )
{
	_tstring strNickName, strUUID, strFbUserId;
	ID idConnect;
	ID idAccount;
	p >> idConnect;
	p >> idAccount;
	p >> strUUID;
	p >> strNickName;
	p >> strFbUserId;
	XBREAK( strUUID.empty() );
	XBREAK( strNickName.empty() );
	XBREAK( idAccount == 0 );
	XBREAK( idConnect == 0 );
	BTRACE( "G0:%s: nick=%s", __TFUNC__, strNickName.c_str() );
// 	auto spAcc = std::make_shared<XSAccount>(idAccount, strUUID);
	auto spAcc = XSPSAcc( new XSAccount( idAccount, strUUID ) );
	// 디폴트 데이타를 넣음.
	spAcc->CreateDefaultAccount();
	spAcc->SetstrName( strNickName.c_str() );
	if( !strFbUserId.empty() )
		spAcc->SetstrFbUserId( strFbUserId );
	// 대기열에 넣고
	auto pGame = GetGame();
	if( spAcc && pGame->AddLoginAccount( spAcc->GetidAccount(), spAcc ) == false )	{
// 		CONSOLE( "이미 도착한 계정정보가 와서 삭제함.: idAcc=%d", spAcc->GetidAccount() );
		// 이미 로그인서버에서 온 데이타이므로 삭제.
// 		SAFE_RELEASE_REF( spAcc );
		XBREAK(1);
		return;	// 일단 리턴
#pragma message("여기 처리 꼼꼼하게 다시 할것")
	}
	// 디폴트 데이타 곧바로 세이브
#ifdef _XBOT
	XConsole( _T( "send to db by create newAcc:%s" ), strNickName.c_str() );
#endif // _XBOT
	DBA_SVR->SendSaveAccountInfo( spAcc, TRUE, idConnect );	// 세이브한 결과 요청을 한다.
	// XDBASvrConnection::RecvSaveAccountInfo로 응답받는다.
}

/**
 @brief 새 더미 계정을 만든다.
*/
void XLoginConnection::RecvNewDummyAccount( XPacket& p )
{
	ID idAccount; //, idConnect;
	int cnt;
	_tstring strNick;
	p >> idAccount;
	p >> strNick;
	p >> cnt;
// 	auto spAcc = std::make_shared<XSAccount>(idAccount);
	auto spAcc = XSPSAcc( new XSAccount( idAccount ) );
	// 디폴트 데이타를 넣음.
	spAcc->CreateDummyAccount();
	spAcc->SetstrName( strNick );
//	CONSOLE("send to DBSvr save account:id=%d", spAcc->GetidAccount() );
	// 디폴트 데이타 곧바로 세이브
	if( DBA_SVR) {
		DBA_SVR->SendSaveAccountInfo( spAcc, TRUE, 0, xGDBPK_SAVE_DUMMY_ACCOUNT, cnt  );	// 세이브한 결과 요청을 한다.
	}
//	SAFE_RELEASE_REF( spAcc );
}

/**
 @brief idDefender의 로그인락을 풀어준다.
*/
void XLoginConnection::SendUnlockLoginForBattle( ID idDefender )
{
	XPacket ar( (ID)xLS2GS_LOGIN_UNLOCK_FOR_BATTLE );
	ar << idDefender;
	Send( ar );
}
//// 게임서버쪽의 로드가 모두 끝나서 유저받을 준비가 됐다는걸 알림
void XLoginConnection::SendDidFinishLoad( void )
{
	int n = 0;
	XPacket ar((ID)xLS2GS_DID_FINISH_LOAD);
//	WORD port = MAIN->GetpSocketForClient()->GetPort();
	XBREAK(XEnv::sGet()->GetGSvrID() > 0xffff );
	WORD port = MAIN->GetpSocketForClient()->GetPort();
	const std::string strIPExternal = XEnv::sGet()->GetstrIPExternal();
	XBREAK( strIPExternal.empty() );
	ar << port;
	ar << (WORD)XEnv::sGet()->GetGSvrID();
	ar << strIPExternal;
	Send( ar );
//	
}

void XLoginConnection::SendSaveDummyComplete( ID idAcc )
{
#ifdef _DEV_SERVER
	XPacket ar( (ID)xLS2GS_SAVE_DUMMY_COMPLETE );
	ar << idAcc;
	Send( ar );
#endif // _DEV_SERVER
}

/**
 @brief 로긴서버에서 이 게임서버의 현재 상태를 보냄.
 .게임서버에 붙은 클라이언트 수(동접자수)
*/
void XLoginConnection::SendGameSvrInfo( int numClients )
{
	MAIN->m_fpsToLogin.Process();
	XPacket ar( (ID)xLS2GS_GAMESVR_INFO );
	ar << numClients;
	Send( ar );
}

/**
 @brief 이 ,게임서버를 종료시킴.
*/
void XLoginConnection::RecvExitGameSvr( XPacket& p )
{
	XMain::sGet()->SetbDestroy( TRUE );
}

void XLoginConnection::SendReload( const std::string& strTags )
{
	XPacket ar( (ID)xLS2GS_RELOAD );
	ar << strTags;
	Send( ar );
}



