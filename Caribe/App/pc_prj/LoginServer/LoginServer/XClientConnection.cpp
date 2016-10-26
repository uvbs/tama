#include "stdafx.h"
#include "XClientConnection.h"
#include "XPacketCL.h"
#include "XPacketLG.h"
#include "XGameUser.h"
#include "XMain.h"
#include "XFramework/XDBMng2.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketSvrForClient.h"
#include "XGameSvrSocket.h"
#include "XGameSvrConnection.h"
#include "XDBUAccount.h"
#include "XUserDB.h"
#include "XDatabase.h"
#include "XGlobalConst.h"
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

int XClientConnection::s_numUsed = 0;		//생성된 객체수

//////////////////////////////////////////////////////////////////////////
XClientConnection::XClientConnection( SOCKET socket, LPCTSTR szIP )
	: XEWinConnectionInServer( _T("clientConn")
														, MAIN->GetpSocketForClient()
														, socket
														, szIP
														, 0x10000 )
{
	Init();
// 	m_spLock = std::make_shared<XLock>();
	BTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
	::InterlockedIncrement( (LONG*)( &s_numUsed ) );
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
	case xCL2LS_ACCOUNT_SESSION_UUID:		RecvRequestSESSIONUUID(p, idKey);		break;	// 세션키와함께 UUID로 접속
	case xCL2LS_ACCOUNT_SESSION_ID:			RecvRequestSESSIONID(p, idKey);	break;	// 세션키와함께 IDName 으로 접속
	case xCL2LS_ACCOUNT_LOGIN_FROM_UUID:	RecvRequestLoginUUID(p, idKey);		break;	// 세션키 없이 UUID 접속
 	case xCL2LS_ACCOUNT_LOGIN_FROM_ID:		RecvRequesLoginID(p, idKey);		break;	// 세션키 없이 IDName 으로 접속
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

void XClientConnection::OnResultFindAccountByUUIDSessionKey(ID idKey, BOOL bFound, XSPDBUAcc spAcc, _tstring strUUID, _tstring strSessionKey)
{
  XBREAK( spAcc->GetidAccount() == 0 );
  SetidAccount( spAcc->GetidAccount() );
}

void XClientConnection::OnResultFindAccountByIDSessionKey(ID idKey, BOOL bFound, XSPDBUAcc spAcc, _tstring strID, _tstring strPW, _tstring strSessionKey)
{
  XBREAK( spAcc->GetidAccount() == 0 );
  SetidAccount( spAcc->GetidAccount() );
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
void XClientConnection::RecvRequesLoginID( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
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
	BTRACE( "%s 2 ", __TFUNC__ );
// 	DBMNG->DoAsyncCall( GetThis(),
// 						&XClientConnection::cbFindAccountByID,
// 						strID,
// 						strPW,
// 						strDeviceID,
// 						strAppstore,
// 						idKey);
	// 새버전
	DBMNG->DoAsyncCall( GetThis()
										, &XClientConnection::cbFindAccount
										, XGAME::xCP_LOGIN_IDPW
										, strID
										, strPW
										, strDeviceID
										, strAppstore
										, idKey);
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
	DBMNG->DoAsyncCall( GetThis()
										, &XClientConnection::cbFindAccount
										, XGAME::xCP_LOGIN_FACEBOOK
										, strtFbUserId
										, _tstring()
										, strDeviceID
										, strAppstore
										, idKey);
}

// 새 계정생성 요청
void XClientConnection::RecvCreateNewAccount( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
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
	XVERIFY_BREAK( strNickName.length() < 2 || strNickName.length() > 16 );
	p >> strDeviceID;
	p >> strAppstore;
	BYTE b0;
	p >> b0;	auto typeLogin = (XGAME::xtConnectParam)b0;
	XVERIFY_BREAK( IsValidConnectParam(typeLogin) == false );
	p >> b0 >> b0 >> b0;
#ifdef _XBOT
	const _tstring strUUID = strNickName;		// 봇모드에서는 닉이 uuid
#else
	const _tstring strUUID = C2SZ( XE::GetUUID() );
#endif // _XBOT
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
#ifdef _XBOT
		XBREAK(1);
#endif // _XBOT
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
 @brief 공통 로그인 루틴
 @param strID uuid모드면 uuid로 사용 facebook이면 fbUserId로 사용
*/
void XClientConnection::cbFindAccount( XDatabase *pDB
																			, XGAME::xtConnectParam typeLogin
																			, const _tstring& strID
																			, const _tstring& strPW
																			, const _tstring& strDeviceID
																			, const _tstring& strAppstore
																			, ID idKey )
{
	BTRACE( "%s:id=%s, pw=%s", __TFUNC__, strID.c_str(), strPW.c_str() );
	//
	XArchive arAcc;
	ID idAcc;
	if( typeLogin == XGAME::xCP_LOGIN_UUID ) {
		idAcc = pDB->GetpDB()->FindAccountByUUID( strID.c_str(), &arAcc );
	} else
	if( typeLogin == XGAME::xCP_LOGIN_IDPW ) {
		idAcc = pDB->GetpDB()->FindAccountByID( strID, &arAcc );
	} else
	if( typeLogin == XGAME::xCP_LOGIN_FACEBOOK ) {
		idAcc = pDB->GetpDB()->FindAccountByFacebook( strID, &arAcc );
	} else {
		XBREAKF(1, "알수없는 로그인 타입:%d", typeLogin );
		return;
	}
	if( idAcc )	{
		pDB->RegistAccountDeviceID( idAcc, strDeviceID, strAppstore);
	} else {
		BTRACE("L:%s:id=%s", __TFUNC__, strID.c_str() );
	}
	//
	DBMNG->RegisterSendCallback( GetThis(),
									&XClientConnection::OnResultFindAccount,
									idKey,
									arAcc,
									typeLogin,
									strID,
									strPW );
}

/**
 @brief 로그인의 결과를 메인스레드에서 보낸다.
 @param strID uuid모드에선 uuid, id/pw모드에선 id가 담긴다.
*/
void XClientConnection::OnResultFindAccount( ID idKey
																						, XArchive& arAcc
																						, const XGAME::xtConnectParam typeLogin
																						, const _tstring& strID
																						, const _tstring& strPW )
{
	do {
		ID idAcc;
		arAcc >> idAcc;
		// 게임서버 열린거 없으면 작업 취소
		auto pSocketForGameSvr = MAIN->GetpSocketForGameSvr();
		if( XBREAK( !pSocketForGameSvr ||
								(pSocketForGameSvr && pSocketForGameSvr->GetlistGameSvr().size() == 0) ) ) {
			SendClosedServer();
			break;
		}
		if( MAIN->GetbServerOpen() == false )	{	// 아직 사용하지 않음.
			SendClosedServer();
			break;
		}
		if( idAcc == 0 ) {
			// ID로 계정 못찾음.
			BTRACE( "L11:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
			SendNoAccount( typeLogin );
		} else {
			ID idBattleTarget;
			WORD widLastConnectSvr, w0;
			int verAcc, lvGM, secBattleTime;
			_tstring strNickName, strPWInDB;
			arAcc >> w0;	verAcc = w0;
			arAcc >> w0;	widLastConnectSvr = w0;
			arAcc >> lvGM >> strNickName;
			if( typeLogin == XGAME::xCP_LOGIN_UUID || typeLogin == XGAME::xCP_LOGIN_FACEBOOK ) {
				arAcc >> idBattleTarget >> secBattleTime;
				BTRACE( "%s:idAcc=%d,uuid=%s,nick=%s\n", __TFUNC__, idAcc, strID.c_str(), strNickName.c_str() );
			} else
			if( typeLogin == XGAME::xCP_LOGIN_IDPW ) {
#pragma message("여기도 idBattleTarget값 얻어와야함.")
				arAcc >> strPWInDB;
				BTRACE( "%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
				// 비번 검사
				if( strPWInDB != strPW ) {
					SendWrongPassword();
					break;
				}
			}
			if( XBREAK(strNickName.empty()) ) {		// 이제 이런경우 생기면 안됨.
				SendNoAccount( typeLogin );
				break;
			}
      SetidAccount( idAcc );
			//////////////////////////////////////////////////////////////////////////
			if (widLastConnectSvr > 0) {
				// 이미 누군가가 접속해 있(는거 같)음. 혹은 정상로그아웃을 못함.
				// 게임서버로 계정조회 요청.
				BTRACE( "L8:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
				auto pGameSvrConnection
					= pSocketForGameSvr->GetGameSvrConnection((ID)widLastConnectSvr);
				if (pGameSvrConnection) {
					BTRACE( "L9:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
					// 접속중인지 요청.
					pGameSvrConnection->SendAccountIsAlreadyConnect( GetidConnect(), idAcc );
				} else {
					// 해당 번호의 게임서버가 없으므로 그냥 비어있는 서버에 정상 접속시킴
					auto pGameSvrConn = pSocketForGameSvr->GetFreeConnection();
					if( pGameSvrConn ) {
						//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
						const bool bSessionKeyLogin = false;
						BTRACE( "L10:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
						//
						SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
					} else {
						// 열려있는 서버가 한개도 없을때는
						SendClosedServer();
					}
				} // if (pGameSvrConnection) {
				break;
			} else { // if (widLastConnectSvr > 0) {
				if( typeLogin == XGAME::xCP_LOGIN_UUID ) {
					//////////////////////////////////////////////////////////////////////////
					if( idBattleTarget > 0 ) {
						const DWORD secStart = secBattleTime;
						auto secPass = XTimer2::sGetTime() - secStart;  // 지나간 시간(초)
						if (secPass < XGC->m_secMaxBattleSession) {
							SendLoginLockForBattle( secPass );
							break;
						}
					}// if( idBattleTarget > 0 ) {
				}
			} // if (widLastConnectSvr > 0) {
			//////////////////////////////////////////////////////////////////////////
			//정상 로긴이 가능한 유저 이므로 접속 할꺼라고 게임 서버에 보내준다.
			const bool bSessionKeyLogin = false;
			BTRACE( "L10:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
			//
			SendToGameSvrLoginAccInfo( idAcc, strNickName, typeLogin, bSessionKeyLogin );
		// bFound
		} // idAcc
	} while (0);
	BTRACE( "L_end:%s: idAcc=%d, id=%s", __TFUNC__, idAcc, strID.c_str() );
}

//////////////////////////////////////////////////////////////////////////]
/**
 @brief uuid로 로그인을 시도한다.

*/
void XClientConnection::RecvRequestLoginUUID( XPacket& p, ID idKey )
{
	MAIN->m_fpsFromClient.Process();
	_tstring strUUID;
	_tstring strSessionKey;
	_tstring strDeviceID;
	_tstring strAppstore;
	DWORD verCLPK, verCGPK, verCPPK, dev_level;
	p >> strUUID;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	if( IsInvalidPacketVer( verCLPK, verCGPK, verCPPK ) ) {
		SendInvalidPacketVer();
		return;
	}
	p >> dev_level;
	XVERIFY_BREAK( dev_level != _DEV_LEVEL );
	p >> strSessionKey;
	p >> strDeviceID;
	p >> strAppstore;

	// 새 버전
	DBMNG->DoAsyncCall( GetThis()
										, &XClientConnection::cbFindAccount
										, XGAME::xCP_LOGIN_UUID
										, strUUID
										, _tstring()
										, strDeviceID
										, strAppstore
										, idKey );
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
// 	auto spAccBuff = std::make_shared<XDBUAccount>();
	auto spAccBuff = XSPDBUAcc( new XDBUAccount );
	DBMNG->DoAsyncCall( GetThis(),
		&XClientConnection::cbFindAccountByIDSession,
		spAccBuff,
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
	_tstring strDeviceID;
	_tstring strAppstore;
	p >> strUUID;
	DWORD verCLPK, verCGPK, verCPPK, dev_level;
	p >> verCLPK;
	p >> verCGPK;
	p >> verCPPK;
	// 여기에 다른 패킷 넣지말것.
	if (verCLPK != VER_CLPK || verCGPK != VER_CGPK || verCPPK != VER_CPPK) {
		XPacket ar((ID)xCL2LS_DIFF_PACKET_VER);		// 서버<->클라간 프로토콜이 다름. 클라보고 어플새로 받으라고 함.
		ar << (DWORD)VER_CLPK;
		ar << (DWORD)VER_CGPK;
		ar << (DWORD)VER_CPPK;
		Send(ar);
		return;
	}
	p >> dev_level;
	XVERIFY_BREAK( dev_level != _DEV_LEVEL );
	p >> strSSID;
	p >> strDeviceID;
	p >> strAppstore;
	auto spAccBuff = XSPDBUAcc( new XDBUAccount );
	DBMNG->DoAsyncCall( GetThis(),
		&XClientConnection::cbFindAccountByUUIDSessionKey,
		spAccBuff,
		strUUID,
		strSSID,
		idKey);
}

void XClientConnection::cbFindAccountByIDSession(XDatabase *pDB, XSPDBUAcc spAccOut, _tstring strID, _tstring strPW, _tstring strSessionKey, ID idKey)
{
	BOOL bFound = pDB->FindAccountSessionID(spAccOut, strID, strSessionKey);

	DBMNG->RegisterSendCallback( GetThis(),
								&XClientConnection::OnResultFindAccountByIDSessionKey,
								idKey,
								bFound,
								spAccOut,
								strID,
								strPW,
								strSessionKey);
}
void XClientConnection::cbFindAccountByUUIDSessionKey(XDatabase *pDB, XSPDBUAcc spAccOut, _tstring strUUID, _tstring strSessionKey, ID idKey)
{
	BOOL bFound = pDB->FindAccountSessionUUID(spAccOut, strUUID, strSessionKey);

	DBMNG->RegisterSendCallback( GetThis(),
								&XClientConnection::OnResultFindAccountByUUIDSessionKey,
								idKey,
								bFound,
								spAccOut,
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
//	MAIN->m_fpsToClient.Process();
	XPacket ar( (ID)xCL2LS_ACCOUNT_RECONNECT_TRY );
	ar << 1.f;
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