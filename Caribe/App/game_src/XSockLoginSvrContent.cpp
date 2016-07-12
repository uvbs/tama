#include "stdafx.h"
#include "XSockLoginSvrContent.h"
#include "XSystem.h"

#define BTRACE		XTRACE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/**
 @brief facebook으로 로그인을 시도한다.
*/
bool XSockLoginSvr::SendLoginFromFacebook( XWnd *pCallback, const std::string& strcFbUserid, const std::string& strDeviceId )
{
	_XCHECK_CONNECT( 0 );
	XBREAK( strcFbUserid.empty() );
	XPacket ar((ID)xCL2LS_ACCOUNT_LOGIN_FROM_FACEBOOK);
	ar << VER_CLPK;
	ar << VER_CGPK;
	ar << VER_CPPK;
// 	if( ACCOUNT && XE::IsHave(ACCOUNT->GetSessionKey()) )
// 		ar << ACCOUNT->GetSessionKey();
	if( XGame::sIsHaveSessionKey() )
		ar << XGame::sGetSessionKey();
	else
		ar << _T("");
	ar << strcFbUserid;
//	_tstring tstrDeviceId = U82SZ( CppToJava("gcm_regid").c_str() );
	_tstring tstrDeviceId = U82SZ( strDeviceId.c_str() );
	ar << tstrDeviceId;
#ifdef _VER_ANDROID
	ar << "google";
#elif _VER_IOS
	ar << "apple";
#else
	ar << "google";
#endif // _VER_ANDROID

	ID idKey =
	AddResponse( xCL2LS_ACCOUNT_REQ_LOGIN_INFO, &XSockLoginSvr::RecvLoginSuccess, pCallback);
	AddResponse(idKey, xCL2LS_NO_ACCOUNT, &XSockLoginSvr::RecvNoAccount, pCallback);
	AddResponse(idKey, xCL2LS_ACCOUNT_RECONNECT_TRY, &XSockLoginSvr::RecvReconnectTry, pCallback);
	AddResponse( idKey, xCL2LS_DIFF_PACKET_VER, &XSockLoginSvr::RecvDiffPacketVer, pCallback );
	AddResponse(idKey, xCL2LS_CLOSED_SERVER, &XSockLoginSvr::RecvClosedServer, pCallback);
	Send( ar ); 
	return TRUE;
} 

/**
 @brief 현재 공격받고 있어서 로그인이 잠시 막힘
*/
void XSockLoginSvr::RecvLoginLockForBattle( XPacket& p )
{
	DWORD secStart, secTotal, secPass;
	p >> secPass;
	p >> secTotal;
	secStart = XTimer2::sGetTime() - secPass;
	XSceneTitle::s_secLoginLockStart = secStart;
	XSceneTitle::s_secLoginLockTotal = secTotal;
	if( SCENE_TITLE ) {
		SCENE_TITLE->OnRecvLoginLockForBattle( secStart, secTotal );
	} else {
		auto pScene = GAME->GetCurrScene();
		if( XASSERT(pScene) ) {
			pScene->DoExit( XGAME::xSC_TITLE );
		}
	}
}

/**
 @brief 로그인락 상태가 해제되었다.
*/
void XSockLoginSvr::RecvLoginLockFree( XPacket& p )
{
	if( SCENE_TITLE )
		SCENE_TITLE->OnRecvLoginLockFree();
	else
		GAME->OnExitApp( nullptr, 0, 0 );
}
BOOL XSockLoginSvr::SendCrashDump( ID idAcc, bool *pOutRet )
{
	_XCHECK_CONNECT( 0 );
	XPacket ar( (ID)xCL2LS_ERROR_LOG );
	int sizeDump = XGAME::SerializeCrashDump( ar, idAcc );
	if( pOutRet )
		*pOutRet = (sizeDump > 0);
	Send( ar );
	if( SCENE_TITLE )
		SCENE_TITLE->OnSendCrashDump( sizeDump );
	XSYSTEM::DeleteFile( SZ2C( XE::MakeDocFullPath( _T( "" ), _T( "dump.bin" ) ) ) );
	return TRUE;
}

