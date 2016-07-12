#include "stdafx.h"
#include "XClientConnection.h"
#include "XPacketCL.h"
#include "XPacketLG.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XDBMng2.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketSvrForClient.h"
#include "XGameSvrSocket.h"
#include "XGameSvrConnection.h"
#include "XDBUAccount.h"
#include "XUserDB.h"
#include "XDatabase.h"
//#include "server/XSAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
#define BTRACE(F,...)		(0)

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
	: XEWinConnectionInServer( _T("clientConn"), MAIN->GetpSocketForClient(), socket, szIP, 0xffff )		
{
	Init();
// 	m_spLock = std::make_shared<XLock>();
	BTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
}

// 클라이언트로부터 패킷을 받음.
BOOL XClientConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{	
	auto pUser = SafeCast<XGameUser*>( GetspUser().get() );
	BTRACE( "recv packet: %d - \n", idPacket );
	if (MAIN->GetbServerOpen() == false || MAIN->GetpSocketForGameSvr()->GetlistGameSvr().size() == 0) {
		XPacket ar((ID)xCL2LS_CLOSED_SERVER);
		Send(ar);
		return TRUE;
	}
	switch( idPacket ) {
	case xCL2LS_ACCOUNT_NEW:						RecvCreateNewAccount( p, idKey );		break;	// 새 계정생성 요청
	case xCL2LS_ACCOUNT_SESSION_UUID:		RecvRequestSESSIONID(p, idKey);		break;	// 세션키와함께 UUID로 접속
	case xCL2LS_ACCOUNT_SESSION_ID:			RecvRequestSESSIONUUID(p, idKey);	break;	// 세션키와함께 IDName 으로 접속
	case xCL2LS_ACCOUNT_LOGIN_FROM_UUID:	RecvRequestLoginUUID(p, idKey);		break;	// 세션키 없이 UUID 접속
 	case xCL2LS_ACCOUNT_LOGIN_FROM_ID:		RecvRequesLogintID(p, idKey);		break;	// 세션키 없이 IDName 으로 접속
	case xCL2LS_ACCOUNT_LOGIN_FROM_FACEBOOK:		RecvRequesLoginByFacebook( p, idKey );		break;	// facebook으로 접속
	case xCL2LS_ERROR_LOG:							RecvCallStack(p, idKey); break;
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%8x", idPacket ); {
			DoDisconnect();
			return FALSE;
		}
		break;
	}
	BTRACE( "recv packet: end\n" );
	return TRUE;	
}

void XClientConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
#pragma message("여기서 에러를 바로 처리하지말고 에러메시지만 쌓아놨다가 메인스레드에서 일괄 처리하자.")
	if( GetspUser() )	{
		auto pUser = SafeCast<XGameUser*>( GetspUser().get() );
		if( error == XENetworkConnection::xERR_PACKET_SIZE_TO_LARGE )		{
			CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.",  GetszIP(), param1 );
		}
	} else	{
		CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.", GetszIP(), param1 );
	}
}


void XClientConnection::OnResultFindAccountByUUIDSessionKey(ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strUUID, _tstring strSessionKey)
{
  XBREAK( pAccount->GetidAccount() == 0 );
  SetidAccount( pAccount->GetidAccount() );

}

void XClientConnection::OnResultFindAccountByIDSessionKey(ID idKey, BOOL bFound, XDBUAccount* pAccount, _tstring strID, _tstring strPW, _tstring strSessionKey)
{
  XBREAK( pAccount->GetidAccount() == 0 );
  SetidAccount( pAccount->GetidAccount() );

}


/**
 @brief 계정 없음.
 클라는 이를 받아 새로 닉네임을 입력받고 새계정생성을 요청한다.
*/
void XClientConnection::SendNoAccount( XGAME::xtConnectParam param /*= XGAME::xCP_NONE*/ )
{
	XBREAK( param == XGAME::xCP_NONE );
	XPacket ar( (ID)xCL2LS_NO_ACCOUNT );
	ar << (DWORD)param;
	Send( ar );
}

/**
 @brief 사용자가 ID/PW로 로그인을 시도함.
*/
void XClientConnection::RecvRequesLogintID( XPacket& p, ID idKey )
{
	_tstring strID;	
	_tstring strPW;		//암호화 해야 하지 않을까?
	_tstring strSessionKey;
	_tstring strDeviceID;
	_tstring strAppstore;
	p >> strID;
	p >> strPW;
	BTRACE( "%s: id=%s ", __TFUNC__, strID.c_str() );
	
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if( IsInvalidPacketVer( verCLPK, verCGPK, verCPPK ) ) {
		SendInvalidPacketVer();
		return;
	}
	p >> strSessionKey;
	p >> strDeviceID;
	p >> strAppstore;
// 	auto pAccountBuff = new XDBUAccount;
	BTRACE( "%s 2 ", __TFUNC__ );
	DBMNG->DoAsyncCall( GetThis(),
						&XClientConnection::cbFindAccountByID,
// 						pAccountBuff,
						strID,
						strPW,
						strDeviceID,
						strAppstore,
						idKey);
}

/**
 @brief ID로 계정을 검색
*/
void XClientConnection::cbFindAccountByID(XDatabase *pDB
// 																				, XDBUAccount *pAccOut
																				, const _tstring& strID
																				, const _tstring& strPW
																				, const _tstring& strDeviceID
																				, const _tstring& strAppstore
																				, ID idKey)
{
	BTRACE( "%s 3 ", __TFUNC__ );
	XArchive arAcc;
	const ID idAcc = pDB->GetpDB()->FindAccountByID( strID, &arAcc );
	if( idAcc )	{
		pDB->RegistAccountDeviceID( idAcc, strDeviceID, strAppstore);
	} else {
		BTRACE("L:%s:id=%s", __TFUNC__, strID.c_str() );
	}
	BTRACE( "%s 4 ", __TFUNC__ );
	DBMNG->RegisterSendCallback( GetThis(),
		&XClientConnection::OnResultFindAccountByID,
		idKey,
		idAcc,
		arAcc,
		strID,
		strPW);
}

void XClientConnection::OnResultFindAccountByID( ID idKey
																								,	const ID idAcc
																								, XArchive& arAcc
																								, const _tstring& strID
																								, const _tstring& strPW )
{
	BTRACE( "L5:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
	const XGAME::xtConnectParam typeLogin = XGAME::xCP_LOGIN_IDPW;
	do {
		if( idAcc ) {
			BTRACE( "L6:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
			ID idAccAr; //, idBattleTarget;
			WORD widLastConnectSvr, w0;
			int verAcc, lvGM; //, secBattleTime;
			_tstring strNickName, strPWInDB;
			arAcc >> idAccAr;
	//		arAcc >> strUUID;
			arAcc >> w0;	verAcc = w0;
			arAcc >> w0;	widLastConnectSvr = w0;
			arAcc >> lvGM >> strNickName >> strPWInDB;
	// 		arAcc >> idBattleTarget >> secBattleTime;
	//		BTRACE( "%s:idAcc=%d,uuid=%s,nick=%s\n", __TFUNC__, idAcc, trUUID.c_str(), strNickName.c_str() );
//			BTRACE( "%s:idAcc=%d,nick=%s\n", __TFUNC__, idAcc, strNickName.c_str() );
			//
			BTRACE( "L7:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
			auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
			if( XBREAK( !pSocketForGameSvr ||
				(pSocketForGameSvr && pSocketForGameSvr->GetlistGameSvr().size() == 0) ) ) {
				SendClosedServer();
				break;
			}
// 			static int s_cntProcess = 0;		// 실행횟수
// 			static DWORD s_dwStart = GetTickCount();
// 			++s_cntProcess;
// 			DWORD dwEnd = GetTickCount();
// 			if( (dwEnd - s_dwStart) > 1000 ) {
// 				// 초당 처리 건수
// 				int cntExec = s_cntProcess / ((dwEnd - s_dwStart) / 1000.f);
// 				s_cntProcess = 0;
// 				s_dwStart = GetTickCount();
// 			}
			SetidAccount( idAcc );
			// 패스워드 틀림
			if( strPWInDB != strPW ) {
				SendWrongPassword();
				break;
			} else
			if (widLastConnectSvr > 0) {
				const ID idLastConnectSvr = widLastConnectSvr;
				// 누군가가 idLastConnectSvr에 이미 접속되어있다.
				// 해당번호의 게임서버로 계정조회 요청.
				BTRACE( "L8:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
				auto pGameSvrConnection 
					= pSocketForGameSvr->GetGameSvrConnection(idLastConnectSvr);
				if (pGameSvrConnection) {
					BTRACE( "L9:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), idAcc );
				} else {
					// 해당 번호의 게임서버가 없으므로 정상 접속시킴
					auto pGameSvrConn = pSocketForGameSvr->GetFreeConnection();
					if( pGameSvrConn ) {
						//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
						const bool bSessionKeyLogin = false;
						BTRACE( "L10:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
						SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
					} else {
						// 열려있는 서버가 한개도 없을때는
						SendClosedServer();
					}
				}
				break;
			} else {
				//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
				const bool bSessionKeyLogin = false;
				BTRACE( "L10:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
				SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
			}
		} else {
			// ID로 계정 못찾음.
			BTRACE( "L11:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
			SendNoAccount( typeLogin );
		}
	} while (0);
	BTRACE( "L_end:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
}

/**
 @brief 사용자가 facebook userid로 로그인요청.
*/
void XClientConnection::RecvRequesLoginByFacebook( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if( IsInvalidPacketVer( verCLPK, verCGPK, verCPPK ) ) {
		SendInvalidPacketVer();
		return;
	}
	_tstring strtFbUserId;
	_tstring strDeviceID;
	_tstring strAppstore;
	_tstring strSessionKey;
	p >> strSessionKey;
	p >> strtFbUserId;
	p >> strDeviceID;
	p >> strAppstore;
	XVERIFY_BREAK( strtFbUserId.empty() );
	DBMNG->DoAsyncCall( GetThis(),
						&XClientConnection::cbFindAccountByFacebook,
						strtFbUserId,
						strDeviceID,
						strAppstore,
						idKey);
}

/**
 @brief ID로 계정을 검색
*/
void XClientConnection::cbFindAccountByFacebook(XDatabase *pDB
																				, const _tstring& strtFbUserId
																				, const _tstring& strDeviceID
																				, const _tstring& strAppstore
																				, ID idKey)
{
	XArchive arAcc;
	const ID idAcc = pDB->GetpDB()->FindAccountByFacebook( strtFbUserId, &arAcc );
	if( idAcc )	{
		pDB->RegistAccountDeviceID( idAcc, strDeviceID, strAppstore);
	}
	DBMNG->RegisterSendCallback( GetThis(),
											&XClientConnection::OnResultFindAccountByFacebook,
											idKey,
											idAcc,
											arAcc,
											strtFbUserId);
}

/**
 @brief 
*/
void XClientConnection::OnResultFindAccountByFacebook( ID idKey
																								,	const ID idAcc
																								, XArchive& arAcc
																								, const _tstring& strFbUserId )
{
	const auto typeLogin = XGAME::xCP_LOGIN_FACEBOOK;
	do {
//		BTRACE( "%s:idAcc=%d,uuid=%s,nick=%s\n", __TFUNC__, idAcc, strUUID.c_str(), strNickName.c_str() );
		//
		auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
		if( XBREAK( !pSocketForGameSvr ||
			(pSocketForGameSvr && pSocketForGameSvr->GetlistGameSvr().size() == 0) ) ) {
			SendClosedServer();
			break;
		}
		if( idAcc ) {
			ID idAcc, idBattleTarget;
			WORD widLastConnectSvr, w0;
			int verAcc, lvGM, secBattleTime;
			_tstring strNickName;
			arAcc >> idAcc;
			arAcc >> w0;	verAcc = w0;
			arAcc >> w0;	widLastConnectSvr = w0;
			arAcc >> lvGM >> strNickName;
			arAcc >> idBattleTarget >> secBattleTime;
			BTRACE( "%s:idAcc=%d,nick=%s\n", __TFUNC__, idAcc, strNickName.c_str() );
      SetidAccount( idAcc );
			if (widLastConnectSvr > 0) {
				// 게임서버로 계정조회 요청.
				auto pGameSvrConnection 
					= pSocketForGameSvr->GetGameSvrConnection(widLastConnectSvr);
				if (pGameSvrConnection) {
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), idAcc );
				} else {
					SendClosedServer();
				}
				break;
			} else {
				//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
				const bool bSessionKeyLogin = false;
				SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
			}
		// idAcc
		} else {
			// ID로 계정 못찾음.(닉넴 입력 요구)
			SendNoAccount( typeLogin );
		}
	} while (0);
}

// 새 계정생성 요청
void XClientConnection::RecvCreateNewAccount( XPacket& p, ID idKey )
{
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
	_tstring strNickName;
	_tstring strDeviceID;
	_tstring strAppstore;
	p >> strNickName;
	p >> strDeviceID;
	p >> strAppstore;
	BYTE b0;
	p >> b0;	auto typeLogin = (XGAME::xtConnectParam)b0;
	p >> b0 >> b0 >> b0;
	const _tstring strUUID = C2SZ( XE::GetUUID() );
	_tstring strFbUserId;
	if( typeLogin == XGAME::xCP_LOGIN_NEW_ACC_BY_FACEBOOK ) {
		p >> strFbUserId;
	}
	//
	DBMNG->DoAsyncCall( GetThis(),
										&XClientConnection::cbCreateNewAccount2,
										strUUID,
										strNickName,
										strDeviceID,
										strAppstore,
										strFbUserId,
										typeLogin,
										idKey);
}

void XClientConnection::cbCreateNewAccount2( XDatabase *pDB,
																						const _tstring& strUUID,
																						const _tstring& strNickName,
																						const _tstring& strDeviceID,
																						const _tstring& strAppstore,
																						const _tstring& strFbUserId,
																						const XGAME::xtConnectParam typeLogin,
																						ID idKey )
{
	BTRACE( "L_start:%s: nick=%s", __TFUNC__, strNickName.c_str() );
	XBREAK( strNickName.empty() );
	XBREAK( strUUID.empty() );
	XBREAK( strAppstore.empty() );
	// 닉 중복검사
	ID idAcc = 0;
	const bool bDuplicate = !pDB->AccountNickNameDuplicateCheck( strNickName );
	if ( !bDuplicate )	{
		BTRACE( "L0:%s: nick=%s", __TFUNC__, strNickName.c_str() );
		idAcc = pDB->GetpDB()->CreateNewAccount( strNickName, strUUID );
		if( XASSERT(idAcc) )	{
			BTRACE( "L1:%s: nick=%s", __TFUNC__, strNickName.c_str() );
			pDB->RegistAccountDeviceID( idAcc, strDeviceID, strAppstore);
			// 페북 정보가 같이 왔다면 그것도 저장.
			if( !strFbUserId.empty() )
				pDB->GetpDB()->RegistFBAccount( idAcc, strFbUserId, _T("") );
		}	else {
			BTRACE( "L2:%s: nick=%s", __TFUNC__, strNickName.c_str() );
		}
	}	 else {
#if _DEV_LEVEL <= DLV_LOCAL
		XBREAK(1);
#endif
	}
	XBREAK( bDuplicate && idAcc != 0 );
	// 메인스레드에서 결과를 보내도록 등록.
	DBMNG->RegisterSendCallback( GetThis(), 
								&XClientConnection::OnResultCreateNewAccount,
								idKey,
								idAcc,
								strUUID,
								strNickName,
								strFbUserId,
								typeLogin,
								bDuplicate );
}

/**
 @brief 계정생성 요청 결과를 보냄
 이곳은 메인스레드임.
*/
void XClientConnection::OnResultCreateNewAccount( ID idKey
																								, ID idAcc
																								, const _tstring& strUUID
																								, const _tstring& strNickName
																								, const _tstring& strFbUserId
																								, const XGAME::xtConnectParam typeLogin
																								, bool bDuplicate )
{
	BTRACE( "L0:%s: nick=%s", __TFUNC__, strNickName.c_str() );
	do {
		if( !bDuplicate ) {
			//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
#ifdef _XBOT
			XConsole( _T("try create newAcc:%s"), strNickName.c_str() );
//			CONSOLE( "try create newAcc:%s", strNickName.c_str() );
#endif // _XBOT
// 			int sereridx = idAcc % 1000;
// 			auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
			auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
			auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetFreeConnection();
			if( pGameSvrConnection ) {
				BTRACE( "L1:%s: nick=%s", __TFUNC__, strNickName.c_str() );
				pGameSvrConnection->SendNewAccount( GetidConnect()
																					, idAcc
																					, strUUID
																					, strNickName
																					, strFbUserId );
				_tstring strLog = _T("Account_New_Create");
				if( !strFbUserId.empty() )
					strLog += XFORMAT(":Facebook(%s)", strFbUserId.c_str() );
				//
				DBMNG->DoAsyncCall( GetThis(),
									&XClientConnection::cbSendUserLog,
									idAcc, 
									(int)XGAME::xULog_Account_Create,
									strNickName,
									strLog);
			}
		// !bDuplicate
		} else {
			// 닉네임 중복
			BTRACE( "L3:%s: nick=%s", __TFUNC__, strNickName.c_str() );
			SendAccountNicknameDuplicate( strNickName, typeLogin );
		}
	} while(0);
}

/**
 @brief uuid로 로그인을 시도한다.

*/
void XClientConnection::RecvRequestLoginUUID( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
	_tstring strUUID;
//	_tstring strNickName;
	_tstring strSessionKey;
	_tstring strDeviceID;
	_tstring strAppstore;
	DWORD verCLPK, verCGPK, verCPPK;
	p >> strUUID;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if( IsInvalidPacketVer( verCLPK, verCGPK, verCPPK ) ) {
		SendInvalidPacketVer();
		return;	
	}
	p >> strSessionKey;
//	p >> strNickName;
	p >> strDeviceID;
	p >> strAppstore;

// 	auto pAccountBuff = new XDBUAccount;
	DBMNG->DoAsyncCall( GetThis(),
		&XClientConnection::cbFindAccountByUUID,
// 		pAccountBuff,
		strUUID,
		strDeviceID,
		strAppstore,
// 		strNickName,		
		idKey);
}

/**
 @brief 
*/
void XClientConnection::cbFindAccountByUUID( XDatabase *pDB,
// 																						XDBUAccount *pAccOut,
																						const _tstring& strUUID,
																						const _tstring& strDeviceID,
																						const _tstring& strAppstore,
																						ID idKey )
{
	BTRACE( "%s:uuid=%s", __TFUNC__, strUUID.c_str() );
	//
	XArchive arAcc;
	const ID idAcc = pDB->GetpDB()->FindAccountByUUID( strUUID.c_str(), &arAcc );
	//
	DBMNG->RegisterSendCallback( GetThis(), 
									&XClientConnection::OnResultFindAccountByUUID,
									idKey,
									idAcc,
									arAcc,
									strUUID,
									strDeviceID, 
									strAppstore);
}

/**
 @brief uuid로그인의 결과를 메인스레드에서 보낸다.
*/
void XClientConnection::OnResultFindAccountByUUID( ID idKey
																									, ID idAcc
																									, XArchive& arAcc
																									, const _tstring& strUUID
																									, const _tstring& strDeviceID
																									, const _tstring& strAppstore )
{
	const XGAME::xtConnectParam typeLogin = XGAME::xCP_LOGIN_UUID;
	do {
		ID idAcc;
		arAcc >> idAcc;

// 		ID idBattleTarget;
// 		WORD widLastConnectSvr, w0;
// 		int verAcc, lvGM, secBattleTime;
// 		_tstring strNickName;
// 		arAcc >> w0;	verAcc = w0;
// 		arAcc >> w0;	widLastConnectSvr = w0;
// 		arAcc >> lvGM >> strNickName >> idBattleTarget >> secBattleTime;
// 		BTRACE( "%s:idAcc=%d,uuid=%s,nick=%s\n", __TFUNC__, idAcc, strUUID.c_str(), strNickName.c_str() );
		//
		auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
		if( XBREAK( !pSocketForGameSvr ||
								(pSocketForGameSvr 
									&& pSocketForGameSvr->GetlistGameSvr().size() == 0) ) ) {
			SendClosedServer();
			break;
		}
		if( MAIN->GetbServerOpen() == false )	{	// 아직 사용하지 않음.
			SendClosedServer();
			break;
		}
		if ( idAcc ) {
			ID idBattleTarget;
			WORD widLastConnectSvr, w0;
			int verAcc, lvGM, secBattleTime;
			_tstring strNickName;
			arAcc >> w0;	verAcc = w0;
			arAcc >> w0;	widLastConnectSvr = w0;
			arAcc >> lvGM >> strNickName >> idBattleTarget >> secBattleTime;
			BTRACE( "%s:idAcc=%d,uuid=%s,nick=%s\n", __TFUNC__, idAcc, strUUID.c_str(), strNickName.c_str() );
			if( XBREAK(strNickName.empty()) ) {		// 이제 이런경우 생기면 안됨.
				SendNoAccount( typeLogin );
				break;
			}
      SetidAccount( idAcc );
			//////////////////////////////////////////////////////////////////////////
			if (widLastConnectSvr > 0) {
				// 이미 누군가가 접속해 있(는거 같)음. 혹은 정상로그아웃을 못함.
				const ID idLastConnectSvr = widLastConnectSvr;
				// 게임서버로 계정조회 요청.
				auto pGameSvrConnection 
					= pSocketForGameSvr->GetGameSvrConnection(idLastConnectSvr);
				if (pGameSvrConnection) {
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), idAcc );
				} else {
					SendClosedServer();
				}
				break;
			} else 
			//////////////////////////////////////////////////////////////////////////
			if( idBattleTarget > 0 ) {
				const DWORD secStart = secBattleTime;
				auto secPass = XTimer2::sGetTime() - secStart;  // 지나간 시간(초)
				if (secPass < XGC->m_secMaxBattleSession) {
					SendLoginLockForBattle( secPass );
					break;
				}
			}// if( idBattleTarget > 0 ) {
			//////////////////////////////////////////////////////////////////////////
			//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
			const bool bSessionKeyLogin = false;
			SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
		// bFound
		} else {
		// not found
			// no account를 보내서 닉네임 새로 받고 새계정을 요청하도록 한다.
			SendNoAccount( typeLogin );
		}
	} while (0);
}

/**
 @brief 정상로그인이 가능한 유저이므로 곧 접속할거라고 게임서버에 알린다.
 @param param uuid인지 id/pw인지 어느걸로 접속시도했었는지 
*/
bool XClientConnection::SendToGameSvrLoginAccInfo( ID idAcc
																									, const _tstring& strNick
																									, XGAME::xtConnectParam param
																									, bool bSessionKeyLogin )
{
	//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
// 	int sereridx = idAcc % 1000;
// 	auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetGameSvrConnection(sereridx);
	auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
	auto pGameSvrConnection = MAIN->GetpSocketForGameSvr()->GetFreeConnection();
	if (pGameSvrConnection) {
		if ( XBREAK(strNick.empty()) ) {
			BTRACE( "%s:idAcc=%d", __TFUNC__, idAcc );
			SendNoAccount( param );
		} else {
			const _tstring strConnectip = GetszIP();
			pGameSvrConnection->SendLoginAccountInfo(	false,		// session key login
																								GetidConnect(),
																								idAcc );
			// 마지막으로 접속한 게임서버 아이디를 기록한다.
			DBMNG->DoAsyncCall( GetThis(),
								&XClientConnection::cbUserLoginUpdate,
								idAcc,
								pGameSvrConnection->GetidSvr(),
								strConnectip,
								strNick );
			// log
			const _tstring strLog = _tstring(_T("Login: ")) + GetszIP();
			DBMNG->DoAsyncCall( GetThis(),
												&XClientConnection::cbSendUserLog,
												idAcc,
												(int)XGAME::xULog_Account_Login,
												strNick,
												strLog);
		}
	// if (pGameSvrConnection) {
	} else {
		//접속 해야 할 게임 서버를 찾지 못했다.. 이런 경우는 서버가 없다고 알려야 함.( 서버를 띄우던가.....)
		auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
		XBREAK( pSocketForGameSvr == nullptr );
		if ( XASSERT(pSocketForGameSvr && pSocketForGameSvr->GetlistGameSvr().size() == 0) ) {
			SendClosedServer();
			return false;
		}
	}
	return true;
}

void XClientConnection::cbUserLoginUpdate(XDatabase *pDB
																				, ID idacc
																				, ID idSvr
																				, const _tstring& strConnectip
																				, const _tstring& strUserName)
{
	// 마지막 접속한 게임서버 아이디를 기록한다.
	BOOL bFound = pDB->GetpDB()->UpdateAccountLogin(idacc, idSvr, strConnectip);
	if (bFound)
	{

		DBMNG->RegisterSendCallback( GetThis(),
									&XClientConnection::OnResultUserLoginSave,
									idacc,
									idSvr,
									strUserName,
									strConnectip);
	}
}

void XClientConnection::RecvRequestSESSIONID( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
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
	DBMNG->DoAsyncCall( GetThis(),
		&XClientConnection::cbFindAccountByIDSession,
		pAccountBuff,
		strID,
		strPW,
		strSSID,
		idKey);
}

void XClientConnection::RecvRequestSESSIONUUID(XPacket& p, ID idKey)
{
	MAIN->m_fpsFromClient.Process();
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
	DBMNG->DoAsyncCall( GetThis(),
		&XClientConnection::cbFindAccountByUUIDSessionKey,
		pAccountBuff,
		strUUID,
		strSSID,
		idKey);
}

void XClientConnection::cbFindAccountByIDSession(XDatabase *pDB, XDBUAccount *pAccOut, _tstring strID, _tstring strPW, _tstring strSessionKey, ID idKey)
{
	BOOL bFound = pDB->FindAccountSessionID(pAccOut, strID, strSessionKey);

	DBMNG->RegisterSendCallback( GetThis(),
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

	DBMNG->RegisterSendCallback( GetThis(),
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


void XClientConnection::OnResultUserLoginSave(ID idacc, int svridx, _tstring strConnectip, _tstring strUserName)
{
	_tstring strLog = _T("[Login] IP:");
	strLog += strConnectip;
	strLog = _T(" ServerIdx:");
	strLog += svridx;
	DBMNG->DoAsyncCall( GetThis(),
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

void XClientConnection::SendAccountNicknameDuplicate( 
																				const _tstring& strNickName
																			, const XGAME::xtConnectParam typeLogin )
{
	MAIN->m_fpsToClient.Process();
	XPacket ar( (ID)xCL2LS_ACCOUNT_NICKNAME_DUPLICATE );
	ar << (DWORD)typeLogin;
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
	MAIN->m_fpsToClient.Process();
	XPacket ar( (ID)xCL2LS_WRONG_PASSWORD );
	Send( ar );
}

void XClientConnection::SendAccountReconnectTry()
{
	MAIN->m_fpsToClient.Process();
	XPacket ar( (ID)xCL2LS_ACCOUNT_RECONNECT_TRY );
	Send( ar );
}

void XClientConnection::SendAccountReqLoginInfo( ID idAcc, const char *cIP, WORD port, const _tstring& strUUID )
{
	MAIN->m_fpsToClient.Process();
	BTRACE( "%s:idAcc=%d, cIp=%s, port=%d uuid=%s", __TFUNC__, idAcc, cIP, port, strUUID.c_str() );
	if( XE::IsEmpty( cIP ) ) {
#pragma message("이런일이 생기면 클라에 메시지 보내주자")
		return;
	}
	XBREAK( XE::IsEmpty(cIP) );
	XPacket ar( (ID)xCL2LS_ACCOUNT_REQ_LOGIN_INFO );
	ar << idAcc;
	ar << cIP;
	ar << port;
	ar << (WORD)0;
	ar << strUUID;
	Send( ar );
}

/**
 @brief 공격받고 있어서 로그인이 잠시 잠긴상태임
*/
void XClientConnection::SendLoginLockForBattle( int secPass )
{
	XPacket ar( (ID)xCL2LS_LOGIN_LOCK_FOR_BATTLE );
	ar << secPass;
	ar << (int)XGC->m_secMaxBattleSession;	// 3분
	Send( ar );
}

bool XClientConnection::IsInvalidPacketVer( DWORD verCLPK, DWORD verCGPK, DWORD verCPPK )
{
	return( verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK );
}

void XClientConnection::SendInvalidPacketVer()
{
	XPacket ar( (ID)xCL2LS_DIFF_PACKET_VER );		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
	ar << (DWORD)VER_CLPK;
	ar << (DWORD)VER_CGPK;
	ar << (DWORD)VER_CPPK;
	Send( ar );
}
