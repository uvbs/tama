#include "stdafx.h"
#include "XSockLoginSvrBot.h"
#include "VerPacket.h"
#include "XAccount.h"
#include "XSockGameSvrBot.h"
#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef WIN32
//#define _XBSD_SOCKET
#endif // WIN32
#define BTRACE		XTRACE

#define _XCHECK_CONNECT( ID_TEXT ) {}

#define SIZE_QUEUE		0x10000

XSockLoginSvrBot::XSockLoginSvrBot( XBotObj* pOwner, XNetworkDelegate *pDelegate )
: XWinSocketBot( pOwner, pDelegate, SIZE_QUEUE ) 
{
	Init();
	AddResponse( (ID)xCL2LS_DIFF_PACKET_VER,
		       &XSockLoginSvrBot::RecvDiffPacketVer );
}

BOOL XSockLoginSvrBot::CallRecvCallback( ID idPacket, XPacket& p )
{
	std::map<ID, xCALLBACK>::iterator itor;
	itor = m_mapCallback.find( idPacket );
	if( itor == m_mapCallback.end() ) {
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		return FALSE;
	}
	xCALLBACK c = itor->second;
	XBREAK( c.pCallback == nullptr );
	(this->*c.pCallback)( p );
	return TRUE;
}
/**
 @brief 외부요인에 의해 연결이 끊어짐.
*/
void XSockLoginSvrBot::cbOnDisconnectByExternal()
{
//	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_DISCONNECTED_LOGINSVR );
}


BOOL XSockLoginSvrBot::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	BTRACE( "packet:%d", idPacket );
	xtCLPacket packet = (xtCLPacket) idPacket;
//	XTimeoutMng::sGet()->ArriveWithKey( (DWORD)packet );	// 요청했던 응답이 돌아와서 타임아웃 해제시킴
	if( CallRecvCallback( idPacket, p ) == FALSE )
		return FALSE;
	return TRUE;
}

BOOL XSockLoginSvrBot::SendNormal( XPacket& ar, ID idTextDisconnect )
{
	_XCHECK_CONNECT(idTextDisconnect);
	return Send( ar );
}


/**
 패킷헤더만 보내는 다용도 send
 bTimeout: 타임아웃을 적용할것인지
*/
BOOL XSockLoginSvrBot::SendPacketHeaderOnly( xtCLPacket idPacket )
{
	//
	XPacket ar((ID)idPacket);
	// 응답없는 일반 send
	return SendNormal( ar );
}
/**
*/
void XSockLoginSvrBot::RecvInvalidClient( XPacket& p )
{
}
/**
 @brief 새 계정생성을 요청
*/
BOOL XSockLoginSvrBot::SendReqCreateAccount( const _tstring& strNickname )
{
	_XCHECK_CONNECT( 0 );
	XPacket ar( (ID)xCL2LS_ACCOUNT_NEW );
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
	ar << strNickname;
	ar << _T("deviceid");
	ar << "google";
	ar << (BYTE)XGAME::xCP_LOGIN_NEW_ACC;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	//여기다 추가한건 아래 SendLoginFromIDPW에도 추가해야함
	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvrBot::RecvLoginSuccess);
	AddResponse(idKey, xCL2LS_ACCOUNT_NICKNAME_DUPLICATE, &XSockLoginSvrBot::RecvDuplicateNickName);
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvrBot::RecvNoAccount);
	AddResponse( idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvrBot::RecvDiffPacketVer );
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvrBot::RecvClosedServer);
	Send( ar ); 
	return TRUE;
} 

/**
 @brief uuid로 로그인을 시도한다.
*/
BOOL XSockLoginSvrBot::SendLoginByUUID( const char *cUUID )
{
	_XCHECK_CONNECT( 0 );
	XBREAK( XE::IsEmpty(cUUID) );
	const xtCLPacket idPacket = xCL2LS_ACCOUNT_LOGIN_FROM_UUID;
	//
	XPacket ar((ID)idPacket);
	ar << cUUID;
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
	ar << _T("");		// session key
	ar << _T("deviceid");
	ar << "google";

	//여기다 추가한건 아래 SendLoginFromIDPW에도 추가해야함
	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvrBot::RecvLoginSuccess);
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvrBot::RecvNoAccount);
	AddResponse(idKey, xCL2LS_ACCOUNT_RECONNECT_TRY, &XSockLoginSvrBot::RecvReconnectTry);
	AddResponse( idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvrBot::RecvDiffPacketVer);
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvrBot::RecvClosedServer);
// 	AddResponse( idKey, xCL2LS_LOGIN_LOCK_FOR_BATTLE, &XSockLoginSvrBot::RecvLoginLockForBattle, nullptr );
	Send( ar ); 
	return TRUE;
} 

/**
 @brief id/pw로 로그인시도
*/
BOOL XSockLoginSvrBot::SendLoginByIDPW( const char *cID, const char *cPW )
{
	_XCHECK_CONNECT(0);
	xtCLPacket idPacket = xCL2LS_NONE;
	idPacket = xCL2LS_ACCOUNT_LOGIN_FROM_ID;
	XLOGB("send uuid only");
	XPacket ar((ID)idPacket);
	ar << cID;
	ar << cPW;
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
	ar << _T("");
	ar << _T("deviceid");
	ar << "google";

	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvrBot::RecvLoginSuccess );
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvrBot::RecvNoAccount);
	AddResponse(idKey, xCL2LS_ACCOUNT_RECONNECT_TRY, &XSockLoginSvrBot::RecvReconnectTry);
	AddResponse(idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvrBot::RecvDiffPacketVer);
	AddResponse(idKey, xCL2LS_WRONG_PASSWORD, &XSockLoginSvrBot::RecvWrongPassword);
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvrBot::RecvClosedServer);

	Send(ar);

	return TRUE;
}

/**
 @brief 로그인 성공하여 계정정보를 받음.
*/
void XSockLoginSvrBot::RecvLoginSuccess(XPacket& p)
{
	_tstring strIP, strUUID;
	WORD port, w1;
	ID idAccount;
	p >> idAccount;
	p >> strIP;
	p >> port;
	p >> w1;
	p >> strUUID;
	XBREAK( strIP.empty() );
	const std::string strcIP = SZ2C(strIP);
	GetpBotObj()->OnLoginSuccessByLoginSvr( idAccount, strcIP, port );
	XMain::sGet()->m_fpsLoginL.Process();
}

void XSockLoginSvrBot::RecvDiffPacketVer( XPacket& p )
{
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK >> verCGPK >> verCPPK;
	XBREAK(1);
}

// 같은 계정으로 다른곳에서 접속을 해서 this는 끊어야 한다.
void XSockLoginSvrBot::RecvDuplicateLogout( XPacket& ar )
{
	XBREAK( 1 );
}

// 접속을 시도했으나 이미 로그인되어있어 THIS가 다시 로그인을 시도해야함.
void XSockLoginSvrBot::RecvReconnectTry( XPacket& ar )
{
	float sec;
	ar >> sec;
	XParamObj paramObj;
	paramObj.Set( "sec", sec );
	GetpBotObj()->DispatchFSMEvent( xConnector::xFE_RECONNECT_TRY_LOGINSVR, paramObj );
	XMain::sGet()->m_fpsDupl.Process();
}

void XSockLoginSvrBot::RecvDuplicateNickName(XPacket& p)
{
	_tstring strNickName;
	DWORD dw0;
	p >> dw0;		auto typeLogin = (XGAME::xtConnectParam)dw0;
	p >> strNickName;
	GetpBotObj()->OnDuplicateNick();
}

void XSockLoginSvrBot::RecvNoAccount(XPacket& p)
{
	DWORD dw0;
	XGAME::xtConnectParam param;
	p >> dw0;	param = (XGAME::xtConnectParam)dw0;
	GetpBotObj()->OnNoAccount( param );
//	DoDisconnect();
//	XBREAK( 1 );
}

void XSockLoginSvrBot::RecvWrongPassword(XPacket &p)
{
	DoDisconnect();
	XBREAK( 1 );
}

void XSockLoginSvrBot::RecvClosedServer(XPacket &p)
{
	XParamObj paramObj;
	paramObj.Set( "sec", xRandomF( 1.f, 3.f ) );
	GetpBotObj()->DispatchFSMEvent( xConnector::xFE_RECONNECT_TRY_LOGINSVR, paramObj );
// 	XBREAK( 1 );
}

