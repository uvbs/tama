#include "stdafx.h"
#include "XSockLoginSvr.h"
#include "XPacketCL.h"
#include "XAccount.h"
#include "XConnector.h"
#include "XLoginInfo.h"
#include "XSockGameSvr.h"
#include "XSystem.h"

//#include "XFramework/android/JniHelper.h"


#define BTRACE		XTRACE

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

#define SIZE_QUEUE		(1024 * 1024 * 10)

// std::shared_ptr<XSockLoginSvr> XSockLoginSvr::s_spInstance;
// ////////////////////////////////////////////////////////////////
// std::shared_ptr<XSockLoginSvr>& XSockLoginSvr::sGet() {
// 	if( s_spInstance == nullptr )
// 		s_spInstance = std::shared_ptr<XSockLoginSvr>( new XSockLoginSvr );
// 	return s_spInstance;
// }
////////////////////////////////////////////////////////////////

#ifdef _VER_IOS
 #ifdef _XBSD_SOCKET
XSockLoginSvr::XSockLoginSvr( XNetworkDelegate *pDelegate )
: XBSDSocketClient( pDelegate, SIZE_QUEUE, 0 )
 #else // bsd socket
XSockLoginSvr::XSockLoginSvr( XNetworkDelegate *pDelegate )
: XCFSocketClient( pDelegate, SIZE_QUEUE, 0 )
 #endif // not bsd socket
#endif // ios
#ifdef WIN32
  #ifdef _XBSD_SOCKET
XSockLoginSvr::XSockLoginSvr( XNetworkDelegate *pDelegate )
: XBSDSocketClient( pDelegate, SIZE_QUEUE, 0 )
  #else
XSockLoginSvr::XSockLoginSvr( XNetworkDelegate *pDelegate )
: XEWinSocketClient( pDelegate, SIZE_QUEUE, 0 ) 
  #endif
#endif
#ifdef _VER_ANDROID
XSockLoginSvr::XSockLoginSvr( XNetworkDelegate *pDelegate )
: XBSDSocketClient( pDelegate, SIZE_QUEUE, 0 )
#endif
{
	Init();
	AddResponse( xCL2LS_DIFF_PACKET_VER,
		                          &XSockLoginSvr::RecvDiffPacketVer, nullptr );
	AddResponse( xCL2LS_LOGIN_LOCK_FREE,
		                          &XSockLoginSvr::RecvLoginLockFree, nullptr );
}

BOOL XSockLoginSvr::CallRecvCallback( ID idPacket, XPacket& p )
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
void XSockLoginSvr::cbOnDisconnectByExternal()
{
	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_DISCONNECTED_LOGINSVR );
}


BOOL XSockLoginSvr::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	BTRACE( "packet:%d", idPacket );
	xtCLPacket packet = (xtCLPacket) idPacket;
	XTimeoutMng::sGet()->ArriveWithKey( (DWORD)packet );	// 요청했던 응답이 돌아와서 타임아웃 해제시킴
	if( CallRecvCallback( idPacket, p ) == FALSE )
		return FALSE;
	return TRUE;
}

BOOL XSockLoginSvr::SendNormal( XPacket& ar, ID idTextDisconnect )
{
	_XCHECK_CONNECT(idTextDisconnect);
	return Send( ar );
}


/**
 패킷헤더만 보내는 다용도 send
 bTimeout: 타임아웃을 적용할것인지
*/
BOOL XSockLoginSvr::SendPacketHeaderOnly( xtCLPacket idPacket )
{
	//
	XPacket ar((ID)idPacket);
	// 응답없는 일반 send
	return SendNormal( ar );
}

// void XSockLoginSvr::SendRequestPacketHeaderOnly( XWnd *pCallback, xtCLPacket idPacket )
// {
// /*
// 	XPacket ar;
// 	ar << idPacket;
// 	Send( ar );
// 	XBREAK( pCallback == nullptr );
// 	TIMEOUT_MNG->Add( pCallback->GetID(), idPacket, 10.f );
// 	CONSOLE("send packet header only:%d", idPacket );
// */
// }

/**
*/
void XSockLoginSvr::RecvInvalidClient( XPacket& p )
{
}
/**
 @brief 새 계정생성을 요청
*/
BOOL XSockLoginSvr::SendReqCreateAccount( XWnd *pCallback, const _tstring& strNickname
																												, const std::string& strFbUserId
																												, const std::string& strDeviceId )
{
	_XCHECK_CONNECT( 0 );
	XPacket ar( (ID)xCL2LS_ACCOUNT_NEW );
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
	ar << strNickname;
//	std::string strDeviceId = CppToJava("gcm_regid");
	_tstring tstrDeviceId = U82SZ( strDeviceId.c_str() );
	// 노트2
	//deviceid2 = _T("APA91bGeBE2XYOkTZeL-gHrAC0ST6-9M73DLxSUkHe3CNYCzQEf4UVnVSLceQwv9MS56cdJKp_Rp8K5RrZ96BcQfM3mbTfketN7lOQUPnh65fdbdSMwn1IU19Oi59B-9JonipNti9_GNeVMTqC96qMxJ0DbwKTX7jg");
	// 화웨이 미디어패드
	//deviceid2 = _T("APA91bFbNbbyNOS3DG4AM1m0qW_inMzNkC - M4QwklnGeboQm7TIDeehFAWJFEwvUIVYZfPkRKUAWuhD0gCjtI6J62ctVhOoJWgtYECvHLRG5xDmUdTdZ6sFTnDlEcEx3vwnFVzRFNpEasjOrmfQTE3b_P1ep1inFCQ");
	ar << tstrDeviceId;
#ifdef _VER_ANDROID
	ar << "google";
#elif _VER_IOS
	ar << "apple";
#else
	ar << "google";
#endif // _VER_ANDROID
	if( strFbUserId.empty() ) {
		ar << (BYTE)XGAME::xCP_LOGIN_NEW_ACC;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
	}
	else {
		ar << (BYTE)XGAME::xCP_LOGIN_NEW_ACC_BY_FACEBOOK;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << strFbUserId;
	}
	//여기다 추가한건 아래 SendLoginFromIDPW에도 추가해야함
	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvr::RecvLoginSuccess, pCallback);
	AddResponse(idKey, xCL2LS_ACCOUNT_NICKNAME_DUPLICATE, &XSockLoginSvr::RecvDuplicateNickName, pCallback);
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvr::RecvNoAccount, pCallback);
	AddResponse( idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvr::RecvDiffPacketVer, pCallback );
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvr::RecvClosedServer, pCallback);
	Send( ar ); 
	return TRUE;
} 

/**
 @brief uuid로 로그인을 시도한다.
*/
BOOL XSockLoginSvr::SendLoginFromUUID( XWnd *pCallback, const char *cUUID, const std::string& strDeviceId )
{
	_XCHECK_CONNECT( 0 );
	XBREAK( XE::IsEmpty(cUUID) );
	xtCLPacket idPacket = xCL2LS_NONE;
//	if( ACCOUNT && XE::IsHave(ACCOUNT->GetSessionKey()) ) {
	if( XGame::sIsHaveSessionKey() ) {
//		idPacket = xCL2LS_ACCOUNT_SESSION_UUID;
		idPacket = xCL2LS_ACCOUNT_LOGIN_FROM_UUID;		// 세션키 로그인이 현재 안되서 이걸로 함.
		CONSOLE( "send session uuid");
	} else {
		idPacket = xCL2LS_ACCOUNT_LOGIN_FROM_UUID;
		CONSOLE( "send uuid only:%s", C2SZ(cUUID) );
	}
	XPacket ar((ID)idPacket);
	ar << cUUID;
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
	ar << (DWORD)_DEV_LEVEL;
//	SerializeCrashDump( ar );
//	if( ACCOUNT && XE::IsHave(ACCOUNT->GetSessionKey()) )
	if( XGame::sIsHaveSessionKey() )
//		ar << ACCOUNT->GetSessionKey();
		ar << XGame::sGetSessionKey();
	else
		ar << _T("");
//	std::string strDeviceId = CppToJava("gcm_regid");
	_tstring tstrDeviceId = U82SZ( strDeviceId.c_str() );
	XTRACE("gcm regid=%s", strDeviceId.c_str() );
	// 노트2
	//deviceid2 = _T("APA91bGeBE2XYOkTZeL-gHrAC0ST6-9M73DLxSUkHe3CNYCzQEf4UVnVSLceQwv9MS56cdJKp_Rp8K5RrZ96BcQfM3mbTfketN7lOQUPnh65fdbdSMwn1IU19Oi59B-9JonipNti9_GNeVMTqC96qMxJ0DbwKTX7jg");
	// 화웨이 미디어패드
	//deviceid2 = _T("APA91bFbNbbyNOS3DG4AM1m0qW_inMzNkC - M4QwklnGeboQm7TIDeehFAWJFEwvUIVYZfPkRKUAWuhD0gCjtI6J62ctVhOoJWgtYECvHLRG5xDmUdTdZ6sFTnDlEcEx3vwnFVzRFNpEasjOrmfQTE3b_P1ep1inFCQ");
	ar << tstrDeviceId;
#ifdef _VER_ANDROID
	ar << "google";
#elif _VER_IOS
	ar << "apple";
#else
	ar << "google";
#endif // _VER_ANDROID

	//여기다 추가한건 아래 SendLoginFromIDPW에도 추가해야함
	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvr::RecvLoginSuccess, pCallback);
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvr::RecvNoAccount, pCallback);
	AddResponse(idKey, xCL2LS_ACCOUNT_RECONNECT_TRY, &XSockLoginSvr::RecvReconnectTry, pCallback);
	AddResponse( idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvr::RecvDiffPacketVer, pCallback );
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvr::RecvClosedServer, pCallback);
	AddResponse( idKey, xCL2LS_LOGIN_LOCK_FOR_BATTLE, &XSockLoginSvr::RecvLoginLockForBattle, pCallback );
	Send( ar ); 
	return TRUE;
} 

/**
 @brief 로그인 성공하여 계정정보를 받음.
*/
void XSockLoginSvr::RecvLoginSuccess(XPacket& p)
{
	_tstring strIP, strUUID;
	WORD port, w1;
	ID idAccount;
	p >> idAccount;
	p >> strIP;
	p >> port;
	p >> w1;
	p >> strUUID;
	CONSOLE("XSockLoginSvr::RecvLoginFromUUID: idAcc=%d, ip=%s:%d uuid=%s", (int)idAccount, strIP.c_str(), (int)port, strUUID.c_str() );
//	GAME->OnConnectedLoginSvr( idAccount, szIP, port );
	const std::string cstrUUID = SZ2C( strUUID );
	const auto& strLoginInfoUUID = LOGIN_INFO.GetstrUUID();
	// loginInfo에 uuid가 있는데 서버에서 받은것과 다를순 없다.
	if( strLoginInfoUUID.empty() ) {	
		// 최초 계정생성후 로그인 상황.
		LOGIN_INFO.SetUUIDLogin( cstrUUID.c_str() );
		LOGIN_INFO.Save();
	} else
	if( XBREAK(strLoginInfoUUID != cstrUUID) ) {
		// 만약에 이런일이 생겼다면 새로 받은 uuid로 다시 기록함.
		LOGIN_INFO.SetUUIDLogin( cstrUUID.c_str() );
		LOGIN_INFO.Save();
	}
	bool bOk;
	SendCrashDump( idAccount, &bOk );
	if( bOk )
		XSYSTEM::xSleep( 1 );	// 곧바로 커넥션을 끊어버리니 서버에서 받지를 못한다.
	XParamObj paramObj;
	paramObj.Set( "id_acc", idAccount );
	paramObj.Set( "ip", strIP );
	paramObj.Set( "port", port );
	const std::string strcIP = SZ2C(strIP);
	GAMESVR_SOCKET->SetIpAndPort( strcIP, port );
	XConnector::sGet()->SetParam( xConnector::xFID_LOGINING_GAMESVR, paramObj );
	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_LOGINED_LOGINSVR, paramObj );

//	XConnector::sGet()->OnLoginedLoginSvr( idAccount, strIP, port );
	if( SCENE_TITLE )
		SCENE_TITLE->OnConnectedLoginSvr( );
}

/**
 @brief id/pw로 로그인시도
*/
BOOL XSockLoginSvr::SendLoginFromIDPW(XWnd *pCallback
																		, const char *cID
																		, const char *cPW
																		, const std::string& strDeviceId )
{
	_XCHECK_CONNECT(0);
	xtCLPacket idPacket = xCL2LS_NONE;
//	if (ACCOUNT && XE::IsHave(ACCOUNT->GetSessionKey())) {
	if( XGame::sIsHaveSessionKey() ) {
		idPacket = xCL2LS_ACCOUNT_SESSION_ID;
		CONSOLE("send session uuid");
	} else {
		idPacket = xCL2LS_ACCOUNT_LOGIN_FROM_ID;
		CONSOLE("send uuid only");
	}
	XPacket ar((ID)idPacket);
	ar << cID;
	ar << cPW;
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
//	if (ACCOUNT && XE::IsHave(ACCOUNT->GetSessionKey()))
	if( XGame::sIsHaveSessionKey() )
//		ar << ACCOUNT->GetSessionKey();
		ar << XGame::sGetSessionKey();
	else
		ar << _T("");
//	std::string strDeviceId = CppToJava( "gcm_regid" );
	_tstring tstrDeviceId = U82SZ( strDeviceId.c_str() );
	// 노트2
	//deviceid2 = _T("APA91bGeBE2XYOkTZeL-gHrAC0ST6-9M73DLxSUkHe3CNYCzQEf4UVnVSLceQwv9MS56cdJKp_Rp8K5RrZ96BcQfM3mbTfketN7lOQUPnh65fdbdSMwn1IU19Oi59B-9JonipNti9_GNeVMTqC96qMxJ0DbwKTX7jg");
	// 화웨이 미디어패드
	//deviceid2 = _T("APA91bFbNbbyNOS3DG4AM1m0qW_inMzNkC - M4QwklnGeboQm7TIDeehFAWJFEwvUIVYZfPkRKUAWuhD0gCjtI6J62ctVhOoJWgtYECvHLRG5xDmUdTdZ6sFTnDlEcEx3vwnFVzRFNpEasjOrmfQTE3b_P1ep1inFCQ");
	ar << tstrDeviceId;
#ifdef _VER_ANDROID
	ar << "google";
#elif _VER_IOS
	ar << "apple";
#else
	ar << "google";
#endif // _VER_ANDROID

	ID idKey =
//		AddResponse(xCL2LS_ACCOUNT_NOT_EXIST, &XSockLoginSvr::RecvNoAccount, pCallback);
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvr::RecvLoginSuccess, pCallback );
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvr::RecvNoAccount, pCallback);
	AddResponse(idKey, xCL2LS_ACCOUNT_RECONNECT_TRY, &XSockLoginSvr::RecvReconnectTry, pCallback);
	AddResponse(idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvr::RecvDiffPacketVer, pCallback);
	AddResponse(idKey, xCL2LS_WRONG_PASSWORD, &XSockLoginSvr::RecvWrongPassword, pCallback);
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvr::RecvClosedServer, pCallback);
  AddResponse( idKey, xCL2LS_LOGIN_LOCK_FOR_BATTLE, &XSockLoginSvr::RecvLoginLockForBattle, pCallback );

	Send(ar);

	return TRUE;
}


void XSockLoginSvr::RecvDiffPacketVer( XPacket& p )
{
	DWORD verCLPK, verCGPK, verCPPK;
	p >> verCLPK >> verCGPK >> verCPPK;
	auto pAlert = XWND_ALERT( "client updated! goto AppStore!.\ncurrent ver:%d.%d.%d\nnew ver:%d.%d.%d", 
				VER_CLPK, VER_CGPK, VER_CPPK, verCLPK, verCGPK, verCPPK );
	pAlert->SetEvent( XWM_CLICKED, GAME, &XGame::OnGotoAppStore );
	XConnector::sGet()->StopFSM();
}

// 같은 계정으로 다른곳에서 접속을 해서 this는 끊어야 한다.
void XSockLoginSvr::RecvDuplicateLogout( XPacket& ar )
{
	// 로그인서버라도 처리해야하지 않을까?
/*
	XWND_ALERT_PARAM_RET( pAlert, 
		nullptr, 
		XWnd::xOK, 
		XCOLOR_WHITE, 
		"%s", XTEXT(2037) );
	pAlert->SetEvent( XWM_OK, GAME, &XT3::OnExitApp );
*/
}

// 접속을 시도했으나 이미 로그인되어있어 THIS가 다시 로그인을 시도해야함.
void XSockLoginSvr::RecvReconnectTry( XPacket& ar )
{
/*
	// 세션이 있을때 xCL2LS_ACCOUNT_SESSION_DEVICE_ID
	// 세션이 없을때 xCL2LS_ACCOUNT_LOGIN_FROM_DEVICE_ID
	float secDelay;
	ar >> secDelay;
	if( ACCOUNT )
		ACCOUNT->SetSessionkey( _T("") );
	XBREAK( secDelay == 0.f );
	//	GAME->GetpScheduler()->RegisterScheduler( 1, secDelay );
	GAME->DoReconnectForExistUser( secDelay );
	CONSOLE("RecvReconnectTry. clear sessionkey");
*/
	CONSOLE( "%s", __TFUNC__ );
	float sec;
	ar >> sec;
	XParamObj paramObj;
	paramObj.Set( "sec", sec );
	XConnector::sGet()->DispatchFSMEvent(
		xConnector::xFE_RECONNECT_TRY_LOGINSVR, paramObj );
// 	XConnector::sGet()->SetidNextFSM( xConnector::xFID_DELAY_AFTER_CONNECTING_LOGINSVR );
}

void XSockLoginSvr::RecvDuplicateNickName(XPacket& p)
{
// 	ID idResult;
	_tstring strNickName;
// 	p >> idResult;
	DWORD dw0;
	p >> dw0;		auto typeLogin = (XGAME::xtConnectParam)dw0;
	p >> strNickName;
	XConnector::sGet()->StopFSM();
	if( SCENE_TITLE )
		SCENE_TITLE->RecvDuplicateNickName( typeLogin, strNickName );
// 	if (idResult == 0 && SCENE_TITLE)
// 		SCENE_TITLE->RecvDuplicateNickName();
}

void XSockLoginSvr::RecvNoAccount(XPacket& p)
{
	DWORD dw0;
	XGAME::xtConnectParam param;
	p >> dw0;	param = (XGAME::xtConnectParam)dw0;
	DoDisconnect();
	XConnector::sGet()->StopFSM();
	LOGIN_INFO.ClearLoginInfo();
	if( SCENE_TITLE )
		SCENE_TITLE->RecvNoAccount( param );
}

void XSockLoginSvr::RecvWrongPassword(XPacket &p)
{
	XConnector::sGet()->StopFSM();
	DoDisconnect();
	if( SCENE_TITLE )
		SCENE_TITLE->RecvWrongPassword( nullptr, 0, 0 );
}

void XSockLoginSvr::RecvClosedServer(XPacket &p)
{
	XConnector::sGet()->StopFSM();
	if (SCENE_TITLE)
		SCENE_TITLE->RecvClosedSvr();
}

