#include "stdafx.h"
#include "XSockGameSvr.h"
#include "XCGPacket.h"
#include "XGame.h"
#include "client/XTimeoutMng.h"
#include "XAccount.h"

#define BTRACE		XTRACE

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define XWND_ALERT(...)	{}

#define _XCHECK_CONNECT( ID_TEXT ) \
	if( IsDisconnected() ) { \
		int _idText = ID_TEXT; \
		if( _idText == 0 )	_idText = 6; \
		XWND_ALERT( "%s", XTEXT(_idText) ); \
		return FALSE; \
	}

#define SIZE_QUEUE		0xffff
#ifdef _VER_IOS
 #ifdef _XBSD_SOCKET
XSockGameSvr::XSockGameSvr( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
: XBSDSocketClient( cIP, port, pDelegate, SIZE_QUEUE, param )
 #else // bsd socket
XSockGameSvr::XSockGameSvr( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
: XCFSocketClient( cIP, port, pDelegate, SIZE_QUEUE, param )
 #endif // not bsd socket
#endif // ios
#ifdef WIN32
XSockGameSvr::XSockGameSvr( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
: XEWinSocketClient( cIP, port, pDelegate, SIZE_QUEUE, param ) 
#endif
#ifdef _VER_ANDROID
XSockGameSvr::XSockGameSvr( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
: XBSDSocketClient( cIP, port, pDelegate, SIZE_QUEUE, param )
#endif
{
	Init();
}

void XSockGameSvr::OnConnect( void )
{
}

void XSockGameSvr::OnDisconnect( void )
{
	CONSOLE("OnDisconnect");
}

BOOL XSockGameSvr::CallRecvCallback( ID idPacket, XPacket& p )
{
	std::map<ID, xCALLBACK>::iterator itor;
	itor = m_mapCallback.find( idPacket );
	if( itor == m_mapCallback.end() )
	{
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		return FALSE;
	}
	xCALLBACK c = itor->second;
	XBREAK( c.pCallback == NULL );
	(this->*c.pCallback)( p );
	return TRUE;
}


BOOL XSockGameSvr::ProcessPacket( DWORD idPacket, XPacket& p )
{
	BTRACE( "packet:%d", idPacket );
	xtPacket packet = (xtPacket) idPacket;

	/*
		원래는 SendRequest()함수를 쓰면 아래 Arrive로 자동으로 타임아웃 처리를 했는데
		기존 Pvp코드에서 자동으로 Arrive를 하면 안되는 코드들이 있어 호환성을 위해 삭제함
		SendRequest()를 사용하는 함수는 수동으로 TIMEOUT_MNG->Arrive() 를 해줘야 함.
	*/
	XTimeoutMng::sGet()->ArriveWithKey( (DWORD)packet );	// 요청했던 응답이 돌아와서 타임아웃 해제시킴
	if( CallRecvCallback( idPacket, p ) == FALSE )
		return FALSE;
/*
	switch( packet )
	{
	case xPK_SEND_ACCOUNT_ID:		RecvSendAccountID( p );		break;
	case xPK_ACCOUNT_DUPLICATE_LOGOUT:	RecvDuplicateLogout( p );	break;
	case xPK_ACCOUNT_RECONNECT_TRY:		RecvReconnectTry( p );	break;
	case xPK_ACCOUNT_RECONNECT_UUID:	RecvSendAccountID( packet, p );	break;
	// cheat
	default:
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		break;
	}
*/

	return TRUE;
}

/*
// 요청/응답류 send함수. 보낸 패킷아이디를 식별자로 타임아웃이 처리된다.
BOOL XSockGameSvr::SendRequest( XPacket& ar, DWORD idTextTimeout )
{
	_XCHECK_CONNECT( idTextTimeout );	// 커넥션이 끊어져있다면 알림창을 띄우게 하고 FALSE를 리턴한다.
	DWORD idPacket = ar.GetPacketHeader();
	Send( ar );
//	XTimeoutMng::sGet()->Add( pTimeoutCallback->GetID(), idPacket );
	return TRUE;
}
*/


BOOL XSockGameSvr::SendNormal( XPacket& ar, ID idTextDisconnect )
{
	_XCHECK_CONNECT(idTextDisconnect);
	//
	return Send( ar );
}


//========================================================
// 치트 
#ifdef _CHEAT
BOOL XSockGameSvr::SendReqCheatGetGold( XWnd *pTimeoutCallback, DWORD gold )
{
/*
	_XCHECK_CONNECT(0);
	//
	XPacket ar;
	ar << (ID) xPK_CHEAT_GET_GOLD;
	ar << 1;
	ar << gold;
	Send( ar );
	GAME->GetpAccount()->AddMoney( gold );
	if( SCENE_LOBBY )
		SCENE_LOBBY->UpdateLobbyUI();
*/
	//
	return TRUE;
}

BOOL XSockGameSvr::SendReqCheatGetCarrot( XWnd *pTimeoutCallback, DWORD carrot )
{
/*
	_XCHECK_CONNECT(0);
	//
	XPacket ar;
	ar << (ID) xPK_CHEAT_GET_GOLD;	// 같은 패킷을 씀.
	ar << 2;
	ar << carrot;
	Send( ar );
	GAME->GetpAccount()->AddCashCarrot( carrot );
	if( SCENE_LOBBY )
		SCENE_LOBBY->UpdateLobbyUI();
*/
	//
	return TRUE;
}

#endif // cheat
//////////////////////////////////////////////////////////////////////////

// 
void XSockGameSvr::RecvMsgBox( XPacket& p )
{
	TCHAR szBuff[ 1024 ];
	DWORD idText;
	p.ReadString( szBuff );
	p >> idText;
//	GAME->RecvMsgBox( szBuff, idText );
}

/**
 패킷헤더만 보내는 다용도 send
 bTimeout: 타임아웃을 적용할것인지
*/
BOOL XSockGameSvr::SendPacketHeaderOnly( xtPacket idPacket )
{
	//
	XPacket ar;
	ar << (ID) idPacket;
	// 응답없는 일반 send
	return SendNormal( ar );
}

void XSockGameSvr::SendRequestPacketHeaderOnly( XWnd *pCallback, xtPacket idPacket )
{
/*
	XPacket ar;
	ar << idPacket;
	Send( ar );
	XBREAK( pCallback == NULL );
	TIMEOUT_MNG->Add( pCallback->GetID(), idPacket, 10.f );
	CONSOLE("send packet header only:%d", idPacket );
*/
}

/**
*/
void XSockGameSvr::RecvInvalidClient( XPacket& p )
{
}

BOOL XSockGameSvr::SendLoginFromUUID( XWnd *pCallback, const char *cUUID, LPCTSTR szNickname )
{
	_XCHECK_CONNECT( 0 );
	XPacket ar;
	xtPacket idPacket = xPK_NONE;
	if( GAME->GetpAccount() && XE::IsHave(GAME->GetpAccount()->GetSessionKey()) )
	{
		idPacket = xPK_ACCOUNT_SESSION_UUID;
//		idPacket = MAKE_HEADER(xPK_ACCOUNT_LOGIN, xPK_ACCOUNT_SESSION_UUID);
		CONSOLE( "send session uuid" );
	}
	else
	{
		idPacket = xPK_ACCOUNT_LOGIN_FROM_UUID;
//		idPacket = MAKE_HEADER(xPK_ACCOUNT_LOGIN, xPK_ACCOUNT_LOGIN_FROM_UUID);
		CONSOLE( "send uuid only" );
	}
	ar << idPacket;
	ar << cUUID;
	if( GAME->GetpAccount() && XE::IsHave(GAME->GetpAccount()->GetSessionKey()) )
		ar << GAME->GetpAccount()->GetSessionKey();
	else
		ar << _T("");
	ar << szNickname;
	ID idKey =
	AddResponse( xPK_ACCOUNT_NOT_EXIST, &XSockGameSvr::RecvNoAccount, pCallback );
	AddResponse( idKey, xPK_ACCOUNT_NEW, &XSockGameSvr::RecvNewAccount, pCallback );
	AddResponse( idKey, xPK_SEND_ACCOUNT_ID, &XSockGameSvr::RecvSendAccountID, pCallback );
	Send( ar ); 
//	XTimeoutMng::sGet()->Add( pCallback->GetID(), idPacket );


	/*
	 응답 전략 구상
	 .카테고리 값으로 타임아웃 매니저에 등록. 타임아웃 객체엔 패킷3개가 들어있어야 한다.
	 .서버에서 처리한후 3가지 패킷중 하나를 보내면 타임아웃 매니저를 모두 뒤져 해당 패킷을 
	  가진 객체를 찾아 없앤다.
	 응답 전략 구상2
	  .패킷3가지를 타임아웃 객체에 등록할때 공통된 키값을 가지고 등록된다.
	  .서버에서 3가지중 한가지 패킷이 도착하면 그 패킷을 타임아웃 객체에서 찾아 키값을 얻는다.
	  .키값을 가진 타임아웃 객체들을 모두 삭제한다.
	  .패킷이 안올때 타임아웃은?
	 ID idc = 
	 AddResponse( xPK_ACCOUNT_SUCCESS_LOGIN, XD(&XSockGameSvr::RecvAccountID) );
	 AddResponse( idc, xPK_ACCOUNT_NO_ACCOUNT, XD(&XSockGameSvr::RecvNoAccount) );
	 AddResponse( idc, xPK_ACCOUNT_NEW_ACCOUNT, XD(&XSockGameSvr::RecvNewAccount) );
	*/
	
	return TRUE;
} 

// 로그인 성공하여 계정정보를 받음.
void XSockGameSvr::RecvSendAccountID( XPacket& p ) 
{
	TCHAR szBuff[256];
	p.ReadString( szBuff );
	GAME->ChangeFSMConnect( XGAME::xFS_SUCCESS_LOGIN );
//	if( SCENE_TITLE )
//		SCENE_TITLE->OnConnectedGameSvr();
/*
	TIMEOUT_MNG->Arrive( xPK_ACCOUNT_SESSION_DEVICE_ID );
	TIMEOUT_MNG->Arrive( xPK_ACCOUNT_LOGIN_FROM_DEVICE_ID );
	
	XUINT64 llCheckSum;
	TCHAR szSessionKey[1024];
	p.ReadString( szSessionKey );
	// 서버의 PropItem과 클라의것이 다르다. 보통 클라의 프로퍼티를 다른폴더(ex:Package폴더)에서 읽는경우 발생한다.

	CONSOLE("success login and recv account: sessionKey=%s", szSessionKey );
	if( GAME->GetpAccount() == NULL )
	{
		XAccount *pAccount = new XAccount;
		pAccount->DeSerialize( p );
		p >> llCheckSum;
#ifdef _DEBUG
		XBREAK( llCheckSum != PROP_ITEM->GetllCheckSum() );
#endif
		GAME->SetpAccount( pAccount );
	}
	GAME->GetpAccount()->SetSessionkey( szSessionKey );
	XBREAK( GAME->GetpAccount() == NULL );
	if( GAME->GetpAccount() )
		CONSOLE( "login account:idAcc=%d", GAME->GetpAccount()->GetidAccount() );
	if( SCENE_TITLE )
		SCENE_TITLE->OnAcceptLogin();
	else
	if( SCENE_PVE )
		SCENE_PVE->OnAcceptLogin();
	// 미완료된 결제 정보가 남아있다. 계정로그인직후 바로 그것을 처리한다.
	if( GAME->GetpIncompletePurchase() )
	{
		CONSOLE("found incompleted purchase");
		XMW::PURCHASE *p = GAME->GetpIncompletePurchase();
		if( SendBuyGemByIAP( GAME,  p->bGoogle,
			p->strJson.c_str(),
			p->strSku.c_str(),
			p->strToken.c_str(),
			p->strOrderId.c_str(),
			p->price,
			p->strTransactDate.c_str() ) == FALSE )
		{
			CONSOLE("send fail incompleted purchase");
			// 보내는데 실패하면 다시 끊었다 붙여본다.
			DoDisconnect();
		}
	}*/
}

// 같은 계정으로 다른곳에서 접속을 해서 this는 끊어야 한다.
void XSockGameSvr::RecvDuplicateLogout( XPacket& ar )
{
/*
	XWND_ALERT_PARAM_RET( pAlert, 
		NULL, 
		XWnd::xOK, 
		XCOLOR_WHITE, 
		"%s", XTEXT(2037) );
	pAlert->SetEvent( XWM_OK, GAME, &XT3::OnExitApp );
*/
}

// 접속을 시도했으나 이미 로그인되어있어 THIS가 다시 로그인을 시도해야함.
void XSockGameSvr::RecvReconnectTry( XPacket& ar )
{
/*
	// 세션이 있을때 xPK_ACCOUNT_SESSION_DEVICE_ID
	// 세션이 없을때 xPK_ACCOUNT_LOGIN_FROM_DEVICE_ID
	float secDelay;
	ar >> secDelay;
	if( GAME->GetpAccount() )
		GAME->GetpAccount()->SetSessionkey( _T("") );
	XBREAK( secDelay == 0.f );
	//	GAME->GetpScheduler()->RegisterScheduler( 1, secDelay );
	GAME->DoReconnectForExistUser( secDelay );
	CONSOLE("RecvReconnectTry. clear sessionkey");
*/
}

/**
 구글(혹은 애플스토어)에서 인앱상품 결제를 마치고 게임서버에 결제했다는것을 알림.
 영수증이 변조되었다면 실패가 떨어질수 있슴.
*/
BOOL XSockGameSvr::SendBuyCashItemByIAP( XWnd *pTimeoutCallback,	
									BOOL bGoogle,
									LPCTSTR szJson,
									LPCTSTR szIdentifier,
									LPCTSTR szToken,
									LPCTSTR szOrderId,
									float price, 
									LPCTSTR szTransactDate )
{
	_XCHECK_CONNECT(0);
	//
	XPacket ar;
	ar << (ID)xPK_LOBBY_BUY_CASHITEM_BY_IAP;
	ar << ((bGoogle)? _T("Google") : _T("Apple"));
	ar << szJson;
	ar << szIdentifier;
	int len = _tcslen(szToken);
	ar << szToken;
	ar << szOrderId;
	ar << 0.f;		// 사용안함
	ar << szTransactDate;
	ID idKey =
		AddResponse( xPK_LOBBY_BUY_CASHITEM_BY_IAP, 
					&XSockGameSvr::RecvBuyCashItemByIAP, pTimeoutCallback );
	Send( ar );
	return TRUE;
}

void XSockGameSvr::RecvBuyCashItemByIAP( XPacket& p ) 
{
}

