/**
 @file	XSockGameSvr.cpp
 @brief  
 @note Send류 함수에 있는 _XCHECK_CONNECT는 연결이 끊겨있을때 에러팝업창을 출력하고 동시에 FALSE를 리턴한다. 파라메터는 팝업창에 뜰 텍스트의 아이디이다. 0은 디폴트 메시지이다.
 @author xuzhu
 @date 2014-09-15
 @todo nothing
*/
#include "stdafx.h"
#include "XSockGameSvrBot.h"
#include "../../../game_src/XPacketCG.h"
//#include "client/XTimeoutMng.h"
#include "XAccount.h"
#include "XMain.h"

#define BTRACE		XTRACE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

#define SIZE_QUEUE		0x10000
#define _XCHECK_CONNECT( ID_TEXT ) {}

//////////////////////////////////////////////////////////////////////////
XSockGameSvrBot::XSockGameSvrBot( XBotObj* pOwner, XNetworkDelegate *pDelegate)
: XWinSocketBot( pOwner, pDelegate, SIZE_QUEUE ) 
{
	Init();
	//
	AddResponse( xCL2GS_INVALID_VERIFY,
					&XSockGameSvrBot::RecvInvalidVerify, nullptr );
}

BOOL XSockGameSvrBot::CallRecvCallback( ID idPacket, XPacket& p )
{
	std::map<ID, xCALLBACK>::iterator itor;
	itor = m_mapCallback.find( idPacket );
	if( itor == m_mapCallback.end() )	{
//		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		XTRACE("unknown idPacket:%d", idPacket);		// 다른 게임 패킷은 모두 씹는다.
		return FALSE;
	}
	xCALLBACK c = itor->second;
//	XBREAK( c.pCallback == nullptr );
  if( c.pCallback != nullptr )
	  (this->*c.pCallback)( p, c );
	return TRUE;
}

void XSockGameSvrBot::cbOnDisconnectByExternal()
{
	XBREAK(1);
}



BOOL XSockGameSvrBot::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	BTRACE( "packet:%d", idPacket );
	xtPacket packet = (xtPacket) idPacket;
	/*
		원래는 SendRequest()함수를 쓰면 아래 Arrive로 자동으로 타임아웃 처리를 했는데
		기존 Pvp코드에서 자동으로 Arrive를 하면 안되는 코드들이 있어 호환성을 위해 삭제함
		SendRequest()를 사용하는 함수는 수동으로 TIMEOUT_MNG->Arrive() 를 해줘야 함.
	*/
//	XTimeoutMng::sGet()->ArriveWithKey( (DWORD)packet );	// 요청했던 응답이 돌아와서 타임아웃 해제시킴
	if( CallRecvCallback( idPacket, p ) == FALSE )
		return FALSE;

	return TRUE;
}


BOOL XSockGameSvrBot::SendNormal( XPacket& ar, ID idTextDisconnect )
{
	_XCHECK_CONNECT(idTextDisconnect);
	//
	return Send( ar );
}

/**
 패킷헤더만 보내는 다용도 send
 bTimeout: 타임아웃을 적용할것인지
*/
BOOL XSockGameSvrBot::SendPacketHeaderOnly( xtPacket idPacket )
{
	//
	XPacket ar((ID)idPacket);
	// 응답없는 일반 send
	return SendNormal( ar );
}

BOOL XSockGameSvrBot::SendReqLogin( ID idAccount )
{
	_XCHECK_CONNECT( 0 );
	XBREAK( idAccount == 0 );
	xtPacket idPacket = xCL2GS_NONE;
	idPacket = xCL2GS_ACCOUNT_REQ_LOGIN;
	XLOGB( "send without session key" );
	XPacket ar((ID)idPacket);
	ar << VER_CGPK;
	ar << idAccount;
	ar << _T("");
	ID idKey =
		AddResponse(xCL2GS_ACCOUNT_SUCCESS_LOGIN, &XSockGameSvrBot::RecvSuccessLogin, nullptr);
	AddResponse( idKey, xCL2GS_ACCOUNT_RECONNECT_TO_GAMESVR, &XSockGameSvrBot::RecvReconnectTryToGameSvr, nullptr );
	Send( ar ); 

	return TRUE;
} 

// 로그인 성공하여 계정정보를 받음.
void XSockGameSvrBot::RecvSuccessLogin( XPacket& p, const xCALLBACK& c )
{
// 	XConnector::sGet()->DispatchFSMEvent( xConnector::xFE_LOGINED_GAMESVR );
	// 재접시에도 ap때문에 데이타 받아야함.
	XLOGB( "Accont size: %d bytes", p.size() );
	BYTE b0;
	p >> b0;	// XGC->m_apPerMin = b0;
	p >> b0;	m_CryptoObj.SetkeyPrivate( b0 );
	p >> b0 >> b0;
	DWORD dw0;
	for( int i = 0; i < 16; ++i )
		p >> dw0;
	auto pAccount = GetpBotObj()->GetpAcc();
	_tstring _strPublicKey;
	if (pAccount == nullptr)	{
		pAccount = new XAccount;
		GetpBotObj()->SetpAcc( pAccount );
		XArchive arAcc(0x10000);
		p >> arAcc;
		// 암호화 키테이블
		m_CryptoObj.DeSerializeKeyTable( p );
	} else {
		// pAccount에다 직접 Deserialize해도 문제없도록 해야하는데 시간이걸리니 일단 땜빵.
		//XBREAK(1);

	}
	// 로그인 성공하자 마자 ID/PW를 등록.
// 	if( GetpBotObj()->GetstrID().empty() ) {
// 		const _tstring strID = GetpBotObj()->GetstrtNickGenerate();
// 		SendReqRegistName( strID, _T( "1234" ) );
// 	}
	GetpBotObj()->OnLoginSuccessByGameSvr();
	XMain::sGet()->m_fpsLoginG.Process();
}

// 같은 계정으로 다른곳에서 접속을 해서 this는 끊어야 한다.
void XSockGameSvrBot::RecvDuplicateLogout(XPacket& ar, const xCALLBACK& c)
{
	XLOGB("XSockGameSvr::RecvDuplicateLogout");
	XBREAK(1);
}

/**
 @brief 접속을 시도했으나 이미 로그인되어있어 THIS가 다시 로그인을 시도해야함.7
*/
void XSockGameSvrBot::RecvReconnectTry( XPacket& ar )
{
	XLOGB("XSockGameSvr::RecvReconnectTry: 서버에 이미 접속되어있어 재접요청을 받음.");
	XBREAK( 1 );
// 	XConnector::sGet()->SetidNextFSM( xConnector::xFID_DELAY_AFTER_CONNECTING_LOGINSVR );
}

void XSockGameSvrBot::RecvInvalidVerify( XPacket& ar, const xCALLBACK& c )
{
	XLOGB( "%s:", __TFUNC__ );
	XBREAK( 1 );
}

BOOL XSockGameSvrBot::SendReqRegistName( const _tstring& strID, const _tstring& strPassword )
{
	XBREAK( strID.empty() );
	XBREAK( strPassword.empty() );
	XTRACE( "%s:id=%s", __TFUNC__, strID.c_str() );
	XPacket ar( (ID)xCL2GS_ACCOUNT_NAME_REGIST );
	ID idKey =
		AddResponse( ar.GetidPacket(),
									&XSockGameSvrBot::RecvRegistID, nullptr );
	ar << strID << strPassword;
	Send( ar );

	return 1;
}
void XSockGameSvrBot::RecvRegistID( XPacket& p, const xCALLBACK& c )
{
	int result;
//	TCHAR szName[ MAX_PATH ], szPassword[ MAX_PATH ];
	_tstring strID, strPW;
	p >> result;
	p >> strID;
	p >> strPW;
	XTRACE( "%s:id=%s bOk=%d", __TFUNC__, strID.c_str(), result );
	XBREAK( strID.empty() );
//	GetpBotObj()->SetstrID( strID );
	strID.clear();
	strPW.clear();
}

/**
 @brief 계정정보가 로그인서버로부터 아직 도착하지 않아 잠시후에 다시 로그인을 시도해야함.
*/
void XSockGameSvrBot::RecvReconnectTryToGameSvr( XPacket& p, const xCALLBACK& c )
{
	WORD w0;
	BYTE b0;
	p >> w0;	float sec = (float)w0 / 1000.f;
	p >> b0;	bool bToLogin = xbyteToBool(b0);
	XParamObj paramObj;
	paramObj.Set( "sec", sec );
	if( bToLogin ) {
		GetpBotObj()->DispatchFSMEvent( xConnector::xFE_RECONNECT_TRY_LOGINSVR, paramObj );
	} else {
		GetpBotObj()->DispatchFSMEvent( xConnector::xFE_RECONNECT_TRY_GAMESVR, paramObj );
	}
}



// 아이디로 로그인한후 실패하면
// 계정생성하고 계정등록
// 
// 봇 100개 한꺼번에 돌리기
// 
// 
// 
