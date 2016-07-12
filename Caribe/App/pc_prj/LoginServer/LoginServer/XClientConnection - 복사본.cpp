#include "stdafx.h"
#include "XClientConnection.h"
#include "XPacketCL.h"
#include "XPacketLG.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XDBMng2.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketForClient.h"
#include "XGameSvrSocket.h"
#include "XGameSvrConnection.h"
#include "XDBUAccount.h"
//#include "server/XSAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define DBMNG	XDBMng2<XClientConnection>::sGet()

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
void XClientConnection::OnInvalidVerify( void )
{
#pragma message("check this")
	CONSOLE( "invalid client: %s", GetszIP() );	
	// 클라에 에러메시지창 보내고
//	SendMsgBox( _T("invalid client") );	
	// 즉각 소켓 닫아버림.
	DoDisconnect(); 

}


XClientConnection::XClientConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( MAIN->GetpSocketForClient(), socket, szIP, 0xffff )		
{
	Init();
	XTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
}

// 클라이언트로부터 패킷을 받음.
BOOL XClientConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{	
	XGameUser *pUser = SafeCast<XGameUser*, XEUser*>( GetpUser() );
	XTRACE( "recv packet: %d - \n", idPacket );
	if (MAIN->GetbServerOpen() == false || MAIN->GetpSocketForGameSvr()->GetlistGameSvr().size() == 0) {
		XPacket ar((ID)xCL2LS_CLOSED_SERVER);
		Send(ar);
		return TRUE;
	}
	switch( idPacket ) {
	case xCL2LS_ACCOUNT_SESSION_UUID:		RecvRequestSESSIONID(p, idKey);		break;	// 세션키와함께 UUID로 접속
	case xCL2LS_ACCOUNT_SESSION_ID:			RecvRequestSESSIONUUID(p, idKey);	break;	// 세션키와함께 IDName 으로 접속
	case xCL2LS_ACCOUNT_LOGIN_FROM_UUID:	RecvRequestLoginUUID(p, idKey);		break;	// 세션키 없이 UUID 접속
	case xCL2LS_ACCOUNT_LOGIN_FROM_ID:		RecvRequesLogintID(p, idKey);		break;	// 세션키 없이 IDName 으로 접속
	case xCL2LS_ERROR_LOG:							RecvCallStack(p, idKey); break;
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%8x", idPacket ); {
			DoDisconnect();
			return FALSE;
		}
		break;
	}
	XTRACE( "recv packet: end\n" );
	return TRUE;	
}

void XClientConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
	if( GetpUser() )
	{
		XGameUser *pUser = SafeCast<XGameUser*, XEUser*>( GetpUser() );
		if( error == XENetworkConnection::xERR_PACKET_SIZE_TO_LARGE )
		{
			CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.",  GetszIP(), param1 );
		}
	} else
	{
		CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.", GetszIP(), param1 );
	}
}

void XClientConnection::RecvRequestNewID( XPacket& p, ID idKey )
{
//	TCHAR szUUID[ 256 ];	
	_tstring strUUID;
	_tstring strDeviceID;
	_tstring strNick;
	_tstring strAppstore;
		
//	p.ReadString( szUUID );
	p >> strUUID;
#pragma message("NickName 만드는게 되면그때 이 주석 부분을 살펴 본다.")
	//TCHAR szNickName[ 256 ];
	//	p.ReadString( szNickName ); //NickName 만드는게 되면그때 이 주석 부분을 살펴 본다.
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if( verCLPK != VER_CLPK ||  verCGPK != VER_CGPK || verCPPK != VER_CPPK )
	{
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD) VER_CLPK;
		ar << (DWORD) VER_CGPK;
		ar << (DWORD) VER_CPPK;
		Send( ar );		
		return;
	}
	
	p >> strDeviceID;	
	p >> strNick;

	XDBUAccount *pAccountBuff = new XDBUAccount;

	DBMNG->DoAsyncCall( this, 
						&XClientConnection::cbCreateNewAccount,
						pAccountBuff,
						strUUID,						
						strNick,
						strDeviceID,
						strAppstore,
						idKey );

}

void XClientConnection::cbCreateNewAccount( XDatabase *pDB,
									XDBUAccount *pAccOut, 
									_tstring& strUUID,
									_tstring strNickName,
									_tstring strDeviceID,
									_tstring strAppstore,
									ID idKey )
{
	BOOL bFound = pDB->AccountNickNameDuplicateCheck(strNickName);
	if (bFound)
	{
		bFound = pDB->CreateNewAccount(pAccOut, strNickName.c_str(), strUUID.c_str());
		if (bFound)
		{
			bFound = pDB->RegistAccountDeviceID(pAccOut->GetidAccount(), strDeviceID, strAppstore);
		}		
	}	
	DBMNG->RegisterSendCallback( this, 
								&XClientConnection::OnResultCreateNewAccount,
								idKey,
								bFound,								
								pAccOut,
								strNickName);
}

void XClientConnection::OnResultCreateNewAccount( ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strNickName )
{
	//	BTRACE( "%s", __TFUNC__ );
	do {
		if( bFound ) {						
			if( pAccount->IsCorrectAccountInfo() == FALSE )	{		// 정상계정인지 체크
				// 로그인서버에선 새계정으로 만들어졌는데 게임서버의 어떤에러로 인해
				// 클라로 "계정없음" 신호 보내줌. 
				SendNoAccount( XGAME::xCP_LOGIN_UUID );
			} else {
				//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
				int sereridx = pAccount->GetidAccount() % 1000;
				auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
				if( pGameSvrConnection ) {
					pGameSvrConnection->SendNewAccount( GetidConnect(), pAccount->GetidAccount(), strNickName );
// 					XPacket ar((ID)xLS2GS_NEW_ACCOUNT);		// 새계정 생성시 디폴트 데이타를 만들기 위해 이렇게 했는데 맞는지 몰겠음.					
// 					ar << GetidConnect();
// 					ar << (ID) pAccount->GetidAccount();
// 					ar << strNickName;
// 					pGameSvrConnection->Send( ar );
					_tstring strLog = _T("Account_New_Create");
					DBMNG->DoAsyncCall(	this,
										&XClientConnection::cbSendUserLog,
										pAccount->GetidAccount(), 
										(int)XGAME::xULog_Account_Create,
										strNickName,
										strLog);
				}
			}
		} else {
			// not found
			SendAccountNicknameDuplicate( bFound, strNickName );
// 			XPacket ar((ID)xCL2LS_ACCOUNT_NICKNAME_DUPLICATE);
// 			ar << (ID)bFound;
// 			ar << strNickName;
// 			Send(ar);
		}
	} while(0);

	SAFE_RELEASE_REF( pAccount );
}

// void XClientConnection::OnResultCreateNewAccount( ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strNickName )
// {
// 	//	BTRACE( "%s", __TFUNC__ );
// 	do
// 	{
// 		if( bFound )
// 		{						
// 			if( pAccount->IsCorrectAccountInfo() == FALSE )			// 정상계정인지 체크
// 			{
// 				// 로그인서버에선 새계정으로 만들어졌는데 게임서버의 어떤에러로 인해
// 				// 클라로 "계정없음" 신호 보내줌. 
// 				SendNoAccount( XGAME::xCP_LOGIN_UUID );
// 			} 			
// 			else
// 			{
// 				//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
// 				int sereridx = pAccount->GetidAccount() % 1000;
// 				XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
// 				//XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetFreeConnection( );
// 				if( pGameSvrConnection )
// 				{
// 					XPacket ar((ID)xLS2GS_NEW_ACCOUNT);		// 새계정 생성시 디폴트 데이타를 만들기 위해 이렇게 했는데 맞는지 몰겠음.					
// 					ar << GetidConnect();
// 					ar << (ID) pAccount->GetidAccount();
// 					ar << strNickName;
// 					pGameSvrConnection->Send( ar );
// 
// 					_tstring strLog = _T("Account_New_Create");
// 					DBMNG->DoAsyncCall(	this,
// 										&XClientConnection::cbSendUserLog,
// 										pAccount->GetidAccount(), 
// 										(int)XGAME::xULog_Account_Create,
// 										strNickName,
// 										strLog);
// 
// 				}
// 			}
// 		}
// 		else
// 		{
// 			XPacket ar((ID)xCL2LS_ACCOUNT_NICKNAME_DUPLICATE);
// 			ar << (ID)bFound;
// 			ar << strNickName;
// 			Send(ar);
// 		}
// 	} while(0);
// 
// 	SAFE_RELEASE_REF( pAccount );
// }

void XClientConnection::cbFindAccountByUUID(	XDatabase *pDB,
												XDBUAccount *pAccOut,
												_tstring& strUUID,
												_tstring strDeviceID,
												_tstring strAppstore,
												_tstring& strNickName,												
												ID idKey )
{
	XTRACE("%s:uuid=%s,nick=%s", __TFUNC__, strUUID.c_str(), strNickName.c_str() );
	BOOL bFound = pDB->FindAccountByUUID( pAccOut, strUUID.c_str() );

	DBMNG->RegisterSendCallback(	this, 
									&XClientConnection::OnResultFindAccountByUUID,
									idKey,
									bFound,
									pAccOut,
									strUUID,
									strNickName,
									strDeviceID, 
									strAppstore);
}
void XClientConnection::OnResultFindAccountByUUIDSessionKey(ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strUUID, _tstring strSessionKey)
{
  XBREAK( pAccount->GetidAccount() == 0 );
  SetidAccount( pAccount->GetidAccount() );

}

void XClientConnection::OnResultFindAccountByUUID( ID idKey,
													BOOL bFound, 
													XDBUAccount* pAccount,
													_tstring& strUUID,
													_tstring& strNickName,
													_tstring strDeviceID,
													_tstring strAppstore)
{
	XTRACE( "%s:found=%d,uuid=%s,nick=%s", __TFUNC__, bFound, strUUID.c_str(), strNickName.c_str() );
	do {
		if (bFound) {
      XBREAK( pAccount->GetidAccount() == 0 );
      SetidAccount( pAccount->GetidAccount() );
			if (pAccount->IsCorrectAccountInfo() == FALSE) {			// 정상계정인지 체크
				// 로그인서버에선 새계정으로 만들어졌는데 게임서버의 어떤에러로 인해
				// 클라로 "계정없음" 신호 보내줌. 
				SendNoAccount( XGAME::xCP_LOGIN_UUID );
			} else if (pAccount->Getlastconnectsvrid() > 0) {
				WORD ConnectGameServerID = (WORD)pAccount->Getlastconnectsvrid();
				// 게임서버로 계정조회 요청.
				auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(ConnectGameServerID);
				if (pGameSvrConnection) {
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), pAccount->GetidAccount() );
// 					XPacket ar((ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT);
// 					ar << GetidConnect();
// 					ar << (ID)pAccount->GetidAccount();
// 					pGameSvrConnection->Send(ar);			
				}
			} else if( pAccount->Getbattletargetid() > 0 ) {
				DWORD secStart = pAccount->Getsecbattletime();
				auto secPass = XTimer2::sGetTime() - secStart;  // 지나간 시간(초)
				if (secPass < XGC->m_secMaxBattleSession) {
					XPacket ar((ID)xCL2LS_LOGIN_LOCK_FOR_BATTLE);
					ar << secPass;
					ar << (int)XGC->m_secMaxBattleSession;   // 3분
					Send(ar);
					return;
				} else {
					if (MAIN->GetbServerOpen() == false) {
						XPacket ar((ID)xCL2LS_CLOSED_SERVER);
						Send(ar);
					} else {
						//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
						int sereridx = pAccount->GetidAccount() % 1000;
						XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
						if (pGameSvrConnection) {						
							_tstring strUserName = pAccount->GetstrName();
							if (strUserName.length() == 0) {
								SendNoAccount(XGAME::xCP_LOGIN_UUID);
							} else {
								ID idacc = pAccount->GetidAccount();								
								_tstring strConnectip = GetszIP();
								bool bSessionKeyLogin = false;
								pGameSvrConnection->SendLoginAccountInfo(	bSessionKeyLogin,
																			GetidConnect(),
																			pAccount->GetidAccount());
								DBMNG->DoAsyncCall(	this,
													&XClientConnection::cbUserLoginUpdate,
													idacc,
													sereridx,
													strConnectip,
													strUserName);
							}
						} else {
							//접속 해야 할 게임 서버를 찾지 못했다.. 이런 경우는 서버가 없다고 알려야 함.( 서버를 띄우던가.....)
							if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0) {
								XPacket ar((ID)xCL2LS_CLOSED_SERVER);
								Send(ar);
							} else {
								XPacket ar((ID)xCL2LS_CLOSED_SERVER);
								Send(ar);
							}
						}
					}
				}
			} else {
				if (MAIN->GetbServerOpen() == false) {
					SendClosedServer();
// 					XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 					Send(ar);
				} else {
					//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
					int sereridx = pAccount->GetidAccount() % 1000;
					auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
					if (pGameSvrConnection) {
						_tstring strLog = _T("Login: ");
						strLog += GetszIP();
						_tstring strUserName = pAccount->GetstrName();
						if (strUserName.length() == 0) {
							SendNoAccount(XGAME::xCP_LOGIN_UUID);
						} else {
							bool bSessionKeyLogin = false;
							pGameSvrConnection->SendLoginAccountInfo(bSessionKeyLogin,
																											GetidConnect(),
																											pAccount->GetidAccount());
						}
						DBMNG->DoAsyncCall(this,
															&XClientConnection::cbSendUserLog,
															pAccount->GetidAccount(),
															(int)XGAME::xULog_Account_Login,
															strUserName,
															strLog);
					} else {
						//접속 해야 할 게임 서버를 찾지 못했다.. 이런 경우는 서버가 없다고 알려야 함.( 서버를 띄우던가.....)
						if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0) {
							SendClosedServer();
// 							XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 							Send(ar);
						} else {
							SendClosedServer();
// 							XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 							Send(ar);
						}
					}
				}				
			}
		} else {
			// not found
			if (strNickName.empty() ) {
				SendNoAccount( XGAME::xCP_LOGIN_UUID );
			} else {				
				if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0 || MAIN->GetbServerOpen() == false) {
					SendClosedServer();
// 					XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 					Send(ar);
				} else {
					XDBUAccount *pAccountBuff = new XDBUAccount;
					DBMNG->DoAsyncCall(this,
						&XClientConnection::cbCreateNewAccount,
						pAccountBuff,
						strUUID,
						strNickName,
						strDeviceID,
						strAppstore,
						idKey);
				}				
			}
		}
	} while (0);

	SAFE_RELEASE_REF(pAccount);
}

// void XClientConnection::OnResultFindAccountByUUID( ID idKey,
// 													BOOL bFound, 
// 													XDBUAccount* pAccount,
// 													_tstring& strUUID,
// 													_tstring& strNickName,
// 													_tstring strDeviceID,
// 													_tstring strAppstore)
// {
// 	XTRACE( "%s:found=%d,uuid=%s,nick=%s", __TFUNC__, bFound, strUUID.c_str(), strNickName.c_str() );
// 	do
// 	{
// 		if (bFound)
// 		{
//       XBREAK( pAccount->GetidAccount() == 0 );
//       SetidAccount( pAccount->GetidAccount() );
// 			if (pAccount->IsCorrectAccountInfo() == FALSE)			// 정상계정인지 체크
// 			{
// 				// 로그인서버에선 새계정으로 만들어졌는데 게임서버의 어떤에러로 인해
// 				// 클라로 "계정없음" 신호 보내줌. 
// 				SendNoAccount( XGAME::xCP_LOGIN_UUID );
// 			}
// 			else if (pAccount->Getlastconnectsvrid() > 0)
// 			{
// 				WORD ConnectGameServerID = (WORD)pAccount->Getlastconnectsvrid();
// 				// 게임서버로 계정조회 요청.
// 				XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(ConnectGameServerID);
// 				if (pGameSvrConnection)
// 				{
// 					XPacket ar((ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT);
// 					ar << GetidConnect();
// 					ar << (ID)pAccount->GetidAccount();
// 					pGameSvrConnection->Send(ar);			
// 				}
// 			}
// 			else if (pAccount->Getbattletargetid() > 0) {
// 				DWORD secStart = pAccount->Getsecbattletime();
// 				auto secPass = XTimer2::sGetTime() - secStart;  // 지나간 시간(초)
// 				if (secPass < XGC->m_secMaxBattleSession) {
// 					XPacket ar((ID)xCL2LS_LOGIN_LOCK_FOR_BATTLE);
// 					ar << secPass;
// 					ar << (int)XGC->m_secMaxBattleSession;   // 3분
// 					Send(ar);
// 					return;
// 				} else {
// 					if (MAIN->GetbServerOpen() == false) {
// 						XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 						Send(ar);
// 					} else {
// 						//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
// 						int sereridx = pAccount->GetidAccount() % 1000;
// 						XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
// 						if (pGameSvrConnection) {						
// 							_tstring strUserName = pAccount->GetstrName();
// 							if (strUserName.length() == 0) {
// 								SendNoAccount(XGAME::xCP_LOGIN_UUID);
// 							} else {
// 								ID idacc = pAccount->GetidAccount();								
// 								_tstring strConnectip = GetszIP();
// 								bool bSessionKeyLogin = false;
// 								pGameSvrConnection->SendLoginAccountInfo(	bSessionKeyLogin,
// 																			GetidConnect(),
// 																			pAccount->GetidAccount());
// 								DBMNG->DoAsyncCall(	this,
// 													&XClientConnection::cbUserLoginUpdate,
// 													idacc,
// 													sereridx,
// 													strConnectip,
// 													strUserName);
// 							}
// 						} else {
// 							//접속 해야 할 게임 서버를 찾지 못했다.. 이런 경우는 서버가 없다고 알려야 함.( 서버를 띄우던가.....)
// 							if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0) {
// 								XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 								Send(ar);
// 							} else {
// 								XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 								Send(ar);
// 							}
// 						}
// 					}
// 				}
// 			} else {
// 				if (MAIN->GetbServerOpen() == false) {
// 					XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 					Send(ar);
// 				} else {
// 					//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
// 					int sereridx = pAccount->GetidAccount() % 1000;
// 					XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
// 					if (pGameSvrConnection) {
// 						_tstring strLog = _T("Login: ");
// 						strLog += GetszIP();
// 						_tstring strUserName = pAccount->GetstrName();
// 						if (strUserName.length() == 0)
// 						{
// 							SendNoAccount(XGAME::xCP_LOGIN_UUID);
// 						}
// 						else
// 						{
// 							bool bSessionKeyLogin = false;
// 							pGameSvrConnection->SendLoginAccountInfo(bSessionKeyLogin,
// 								GetidConnect(),
// 								pAccount->GetidAccount());
// 						}
// 						DBMNG->DoAsyncCall(this,
// 							&XClientConnection::cbSendUserLog,
// 							pAccount->GetidAccount(),
// 							(int)XGAME::xULog_Account_Login,
// 							strUserName,
// 							strLog);
// 					}
// 					else
// 					{
// 						//접속 해야 할 게임 서버를 찾지 못했다.. 이런 경우는 서버가 없다고 알려야 함.( 서버를 띄우던가.....)
// 
// 						if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0)
// 						{
// 							XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 							Send(ar);
// 						}
// 						else
// 						{
// 							XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 							Send(ar);
// 						}
// 					}
// 				}				
// 			}
// 		}
// 		else
// 		{
// 			if (strNickName.empty() ) {
// 				SendNoAccount( XGAME::xCP_LOGIN_UUID );
// 			}
// 			else
// 			{				
// 				if (MAIN->GetpSocketForGameSvr()->GetlistGameSvr().Size() == 0 || MAIN->GetbServerOpen() == false)
// 				{
// 					XPacket ar((ID)xCL2LS_CLOSED_SERVER);
// 					Send(ar);
// 				}
// 				else
// 				{
// 					XDBUAccount *pAccountBuff = new XDBUAccount;
// 
// 					DBMNG->DoAsyncCall(this,
// 						&XClientConnection::cbCreateNewAccount,
// 						pAccountBuff,
// 						strUUID,
// 						strNickName,
// 						strDeviceID,
// 						strAppstore,
// 						idKey);
// 				}				
// 			}
// 			
// 		}
// 	} while (0);
// 
// 	SAFE_RELEASE_REF(pAccount);
// }

void XClientConnection::cbFindAccountByID(XDatabase *pDB, XDBUAccount *pAccOut, _tstring strID, _tstring strPW, _tstring strDeviceID, _tstring strAppstore, ID idKey)
{
	BOOL bFound = pDB->FindAccountIDName(pAccOut, strID);

	if (bFound)
	{
		pDB->RegistAccountDeviceID(pAccOut->GetidAccount(), strDeviceID, strAppstore);
	}
//	if (bFound)
	{
// 		if (strPW != pAccOut->GetstrPassword())
// 		{
// 			bFound = FALSE;
// 		}
		DBMNG->RegisterSendCallback(this,
			&XClientConnection::OnResultFindAccountByID,
			idKey,
			bFound,
			pAccOut,
			strID,
			strPW);
		
	}

	
}
void XClientConnection::OnResultFindAccountByIDSessionKey(ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strID, _tstring strPW, _tstring strSessionKey)
{
  XBREAK( pAccount->GetidAccount() == 0 );
  SetidAccount( pAccount->GetidAccount() );

}

void XClientConnection::OnResultFindAccountByID( ID idKey,	BOOL bFound, XDBUAccount* pAccount, _tstring strID, _tstring strPW )
{
	do {
		if( bFound ) {						
      XBREAK( pAccount->GetidAccount() == 0 );
      SetidAccount( pAccount->GetidAccount() );
			// 패스워드 틀림
			if( pAccount->GetstrPassword() != strPW ) {
				XPacket ar( (ID)xCL2LS_WRONG_PASSWORD );
				Send( ar );
			} else
			if( pAccount->IsCorrectAccountInfo() == FALSE ) {			// 정상계정인지 체크
				// 로그인서버에선 새계정으로 만들어졌는데 게임서버의 어떤에러로 인해
				// 클라로 "계정없음" 신호 보내줌. 
				SendNoAccount( XGAME::xCP_LOGIN_IDPW );
			} else 
			if (pAccount->Getlastconnectsvrid() > 0) {
				WORD ConnectGameServerID = (WORD)pAccount->Getlastconnectsvrid();
				// 게임서버로 계정조회 요청.
				auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(ConnectGameServerID);
				if (pGameSvrConnection) {
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), pAccount->GetidAccount() );
// 					XPacket ar((ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT);
// 					ar << GetidConnect();
// 					ar << (ID)pAccount->GetidAccount();
// 					pGameSvrConnection->Send(ar);
				}	
			} else {
				//int sereridx = pAccount->GetidAccount() % 1000;
				//XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
				////정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
				////XGameSvrConnection *pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetFreeConnection();
				//if (pGameSvrConnection)
				//{
        //pGameSvrConnection->SendLoginAccountInfo( GetidConnect(), pAccount->GetidAccount() );
				//}

				int sereridx = pAccount->GetidAccount() % 1000;
				auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
				if (pGameSvrConnection) {				
					_tstring strLog = _T("Login: ");
					strLog += GetszIP();
					_tstring strUserName = pAccount->GetstrName();
					if (strUserName.length() == 0) {
						SendNoAccount(XGAME::xCP_LOGIN_IDPW);
					} else {
            bool bSessionKeyLogin = false;
            pGameSvrConnection->SendLoginAccountInfo( bSessionKeyLogin, 
                                                      GetidConnect(), 
                                                      pAccount->GetidAccount() );
					}
					DBMNG->DoAsyncCall(this,
						&XClientConnection::cbSendUserLog,
						pAccount->GetidAccount(),
						(int)XGAME::xULog_Account_Login,
						strUserName,
						strLog);
				}
			}
		} else {
			// 계정 없음.
			SendNoAccount( XGAME::xCP_LOGIN_IDPW );
		}
	} while (0);
	SAFE_RELEASE_REF(pAccount);
}

/**
 @brief 계정 없음.
*/
void XClientConnection::SendNoAccount( XGAME::xtConnectParam param )
{
	XPacket ar( (ID)xCL2LS_NO_ACCOUNT );
	ar << (DWORD)param;
	Send( ar );
}


void XClientConnection::RecvRequesLogintID( XPacket& p, ID idKey )
{
	_tstring strID;	
	_tstring strPW;		//암호화 해야 하지 않을까?
	_tstring strSessionKey;
	_tstring strDeviceID;
	_tstring strAppstore;
	p >> strID;
	p >> strPW;
	
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if (verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK)
	{
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD)VER_CLPK;
		ar << (DWORD)VER_CGPK;
		ar << (DWORD)VER_CPPK;
		Send(ar);
		return;
	}
//#error("-------------------------------접속락 검사")
	p >> strSessionKey;
	p >> strDeviceID;
	p >> strAppstore;
	auto pAccountBuff = new XDBUAccount;
	DBMNG->DoAsyncCall(	this,
						&XClientConnection::cbFindAccountByID,
						pAccountBuff,
						strID,
						strPW,
						strDeviceID,
						strAppstore,
						idKey);
}

void XClientConnection::RecvRequestLoginUUID( XPacket& p, ID idKey )
{
	_tstring strUUID;
	_tstring strNickName;
	_tstring strSessionKey;
	_tstring strDeviceID;
	_tstring strAppstore;
	DWORD verCLPK, verCGPK, verCPPK;

	p >> strUUID;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if (verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK) {
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD)VER_CLPK;
		ar << (DWORD)VER_CGPK;
		ar << (DWORD)VER_CPPK;
		Send(ar);
		return;	
	}
	p >> strSessionKey;
	p >> strNickName;
	p >> strDeviceID;
	p >> strAppstore;

	auto pAccountBuff = new XDBUAccount;
	DBMNG->DoAsyncCall(this,
		&XClientConnection::cbFindAccountByUUID,
		pAccountBuff,
		strUUID,
		strDeviceID,
		strAppstore,
		strNickName,		
		idKey);
}

void XClientConnection::RecvRequestSESSIONID( XPacket& p, ID idKey )
{
	_tstring strID;
	_tstring strPW;
	_tstring strSSID;
	//	p.ReadString( szUUID );	// 경고: 이제 TCHAR[]버퍼로 읽지 말것.
	p >> strID;
	p >> strPW;
	p >> strSSID;
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if (verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK) {
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD)VER_CLPK;
		ar << (DWORD)VER_CGPK;
		ar << (DWORD)VER_CPPK;
		Send(ar);
		return;
	}
	XDBUAccount *pAccountBuff = new XDBUAccount;
	DBMNG->DoAsyncCall(this,
		&XClientConnection::cbFindAccountByIDSession,
		pAccountBuff,
		strID,
		strPW,
		strSSID,
		idKey);
}

void XClientConnection::RecvRequestSESSIONUUID(XPacket& p, ID idKey)
{
	_tstring strUUID;
	_tstring strSSID;
	//	p.ReadString( szUUID );	// 경고: 이제 TCHAR[]버퍼로 읽지 말것.
	p >> strUUID;
	p >> strSSID;
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if (verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK)
	{
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD)VER_CLPK;
		ar << (DWORD)VER_CGPK;
		ar << (DWORD)VER_CPPK;
		Send(ar);
		return;
	}
	XDBUAccount *pAccountBuff = new XDBUAccount;
	DBMNG->DoAsyncCall(this,
		&XClientConnection::cbFindAccountByUUIDSessionKey,
		pAccountBuff,
		strUUID,
		strSSID,
		idKey);
}

void XClientConnection::cbFindAccountByIDSession(XDatabase *pDB, XDBUAccount *pAccOut, _tstring strID, _tstring strPW, _tstring strSessionKey, ID idKey)
{
	BOOL bFound = pDB->FindAccountSessionID(pAccOut, strID, strSessionKey);

	DBMNG->RegisterSendCallback(this,
								&XClientConnection::OnResultFindAccountByIDSessionKey,
								idKey,
								bFound,
								pAccOut,
								strID,
								strPW,
								strSessionKey);
}
void XClientConnection::cbFindAccountByUUIDSessionKey(XDatabase *pDB, XDBUAccount *pAccOut, _tstring strUUID, _tstring strSessionKey, ID idKey)
{
	BOOL bFound = pDB->FindAccountSessionUUID(pAccOut, strUUID, strSessionKey);

	DBMNG->RegisterSendCallback(this,
								&XClientConnection::OnResultFindAccountByUUIDSessionKey,
								idKey,
								bFound,
								pAccOut,
								strUUID, 
								strSessionKey);
}

void XClientConnection::cbSendUserLog(XDatabase *pDB, ID idaccount, int LogType, _tstring strNickName, _tstring strLog)
{
	pDB->AddUserLog(idaccount, LogType, strNickName, strLog );
}
void XClientConnection::cbUserLoginUpdate(XDatabase *pDB, ID idacc, int svridx, _tstring strConnectip, _tstring strUserName)
{
	BOOL bFound = pDB->SaveAccountLoginSave(idacc, svridx, strConnectip);
	if (bFound)
	{

		DBMNG->RegisterSendCallback(this,
									&XClientConnection::OnResultUserLoginSave,
									idacc,
									svridx,
									strUserName,
									strConnectip);
	}
}
void XClientConnection::OnResultUserLoginSave(ID idacc, int svridx, _tstring strConnectip, _tstring strUserName)
{
	_tstring strLog = _T("[Login] IP:");
	strLog += strConnectip;
	strLog = _T(" ServerIdx:");
	strLog += svridx;
	DBMNG->DoAsyncCall(	this,
						&XClientConnection::cbSendUserLog,
						idacc,
						(int)XGAME::xULog_Account_Login,
						strUserName,
						strLog);
	
}

/**
 @brief 접속대기중인 유저의 로그인언락 해제를 알린다.
*/
void XClientConnection::SendUnlockLoginForBattle()
{
  XPacket ar( (ID)xCL2LS_LOGIN_LOCK_FREE );
  Send( ar );
}

// 유저로부터 크래쉬 덤프를 받았다.
void XClientConnection::RecvCallStack( XPacket& p, ID idKey )
{
	ID idAccDump = XGAME::DeSerializeCrashDump( p, GetidAccount() );
	if( idAccDump ) {
#ifdef _DEV
		CONSOLE( "recv crash log:idAcc=%d", idAccDump );
#endif // _DEV
	}
}

void XClientConnection::SendAccountNicknameDuplicate( BOOL bFound, const _tstring& strNickName )
{
	XPacket ar( (ID)xCL2LS_ACCOUNT_NICKNAME_DUPLICATE );
	ar << (ID)bFound;
	ar << strNickName;
	Send( ar );
}

void XClientConnection::SendClosedServer()
{
	XPacket ar( (ID)xCL2LS_CLOSED_SERVER );
	Send( ar );
}

void XClientConnection::SendWrongPassword()
{
	XPacket ar( (ID)xCL2LS_WRONG_PASSWORD );
	Send( ar );
}

void XClientConnection::SendAccountReconnectTry()
{
	XPacket ar( (ID)xCL2LS_ACCOUNT_RECONNECT_TRY );
	Send( ar );
}

void XClientConnection::SendAccountReqLoginInfo( ID idAcc, const char *cIP, WORD port )
{
	if( XE::IsEmpty( cIP ) ) {
		// 이런일이 생기면 클라에 메시지 보내주자.
		return;
	}
	XPacket ar( (ID)xCL2LS_ACCOUNT_REQ_LOGIN_INFO );
	ar << idAcc;
	ar << cIP;
	ar << port;
	ar << (WORD)0;
	Send( ar );
}

