#include "stdafx.h"
#include "XDBASvrConnection.h"
#include "XLoginConnection.h"
#include "server/XSAccount.h"
#include "XMain.h"
#include "server/XWinConnection.h"
#include "XPacketGDB.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XGame.h"
#include "XPacketLG.h"
#include "XPacketCG.h"
#include "XSocketForClient.h"
#include "XClientConnection.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XDBASvrConnection* XDBASvrConnection::s_pSingleton = nullptr;

void XDBASvrConnection::sCreateSingleton( const std::string& strIP
																				, WORD port
																				, XNetworkDelegate *pDelegate )
{
	CONSOLE( "try connecting to Db Agent server: %s:%d", C2SZ( strIP.c_str() ), port );
	s_pSingleton = new XDBASvrConnection( strIP.c_str(), port, pDelegate );
}

void XDBASvrConnection::sDestroySingleton()
{
	SAFE_DELETE( s_pSingleton );
}
//////////////////////////////////////////////////////////////////////////
#define SIZE_QUEUE		(0xffff*4)
XDBASvrConnection::XDBASvrConnection( const char *cIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
	: XEWinSocketClient( cIP, port, pDelegate, SIZE_QUEUE, param ) 
{
	Init();
}
/**
 @brief DBA서버와의 연결이 완료된 후에 호출된다.
*/
void XDBASvrConnection::OnConnect( void )
{
}

void XDBASvrConnection::OnDisconnect( void )
{
}

BOOL XDBASvrConnection::CallRecvCallback( ID idKey, XPacket& p ) 
{
	auto itor = m_mapCallback.find( idKey );
	if( itor == m_mapCallback.end() )   {
    // 여기 assert가 걸리면 보내는쪽에서 idKey값을 보내줬는지 확인할것.
		XBREAKF( 1, "not found idKey from DBAgentSvr 0x%8x", idKey );
		return FALSE;
	}
	ID idClientConnect = (itor->second).idClientConnect;
	XGameUser *pOwner = nullptr;
	auto pConnect = MAIN->GetpSocketForClient()->GetConnection( idClientConnect );
	if( pConnect )
		pOwner = SafeCast<XGameUser*>( pConnect->GetpUser() );
	if( pOwner )
		(itor->second).callbackFunc( pOwner, p );
	m_mapCallback.erase( itor );
	return TRUE;
}

//패킷 처리
BOOL XDBASvrConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	ID idClientConnection;
	p >> idClientConnection;	
	// idClientConnection은 이제 시스템이 관리하므로 따로 보낼필요는 없지만 
	// 자동콜백을 쓰려면 코드호환성을 위해서는 0이 아닌값을 넣어줘야 함. 
	if( idClientConnection == 0 ) {
		switch( idPacket ) {
		case xGDBPK_LOGIN_ACCOUNT_INFO_LOAD: RecvLoginAccountInfoLoad( p );	break;
		case xGDBPK_UPDATE_ACCOUNT_LOGOUT: RecvAccountDuplicateProcess(p); break;
		case xGDBPK_SAVE_ACCOUNT_INFO:	RecvSaveAccountInfo( p ); break;
		default:
			// 알수없는 패킷이 날아왔다.
			XBREAKF( 1, "unknown or invalid packet from DBAgentSvr 0x%8x", idPacket );
			break;
		}
	} else {
		CallRecvCallback( idKey, p );
	}
	return TRUE;
}
void XDBASvrConnection::SendUpdateAccountLogout( ID idConnectNew, ID idConnectAlready, ID idAcc )
{
	XPacket ar( (ID)xGDBPK_UPDATE_ACCOUNT_LOGOUT );
	ar << idConnectNew;
	ar << idConnectAlready;
	ar << idAcc;
	Send( ar );
}
// 
/**
 @brief 로그인서버로 계정정보를 보내 세이브 하게 한다.
 bReq가 FALSE이면 idConnect는 0이어도 된다.
 @param idReqPacket bReq를 TRUE로 해서 결과를 돌려받을때 사용하게 할 패킷아이디
*/
void XDBASvrConnection::SendSaveAccountInfo( XSAccount *pAccount,
																							BOOL bReq,
																							ID idConnectClient,	// 로긴서버로부터온 클라이언트 커넥션 아이디
																							ID idReqPacket,
																							int cnt )
{	
	XPacket ar((ID)xGDBPK_SAVE_ACCOUNT_INFO);
	ar << pAccount->GetidAccount();
	XTRACE( "%s", __TFUNC__ );
	ar.SetbForDB( true );
	pAccount->Serialize( ar );
	ar << ((bReq)? 11 : 0);	// 성공 확인 요청
	ar << idConnectClient;
	ar << idReqPacket;
	Send( ar );
}

/**
 세이브가 성공했다고 돌아옴
*/
void XDBASvrConnection::RecvSaveAccountInfo( XPacket& p )
{
	ID idAccount;
	ID idConnect;
	BOOL bFound;
	p >> bFound;
	p >> idAccount;
	p >> idConnect;
	//Login에 접속 해도 된다고 알려 준다.
	XLoginConnection::sGet()->SendLoginAccountInfo( idAccount, idConnect );
}
/**
 @brief 로그인서버로부터 계정등록 성공 메시지를 받음.
*/
// void XDBASvrConnection::RecvSuccessRegisterAccount( XPacket& p )
// {
// }
// 
// // 계정등록을 시도했으나 중복된 계정으로 등록이 실패함.
// void XDBASvrConnection::RecvFailedRegisterAccount( XPacket& p )
// {
// }

// 게임서버쪽의 로드가 모두 끝나서 유저받을 준비가 됐다는걸 알림
// void XDBASvrConnection::SendDidFinishLoad( void )
// {
// }

// 로그인서버로부터 계정접속 정보가 옴
// void XDBASvrConnection::RecvLoginAccountInfo( XPacket& p )
// {
// }
 
void XDBASvrConnection::SendUserLog( ID idAccount, _tstring strUserName, int Logtype, _tstring strLog )
{	
	if (idAccount == 0 )		// 유저에 관련된 Log 이므로 User 정보가 있어야 한다.
		return;
	XPacket ar((ID)xGDBPK_USERLOG_ADD);
	ar << idAccount;
	ar << (int)Logtype;
	ar << strUserName;
	ar << strLog;
	Send(ar);
}
// 로그인서버로부터 UUID로 새계정이 생성되었음을 받았다.
// void XDBASvrConnection::RecvNewAccountForUUID( XPacket& p )
// {
// }

BOOL XDBASvrConnection::Send( XPacket& ar )
{
#ifdef _XPACKET_BUFFER
	// connect()되고 로긴서버로부터 첫번째 확인패킷이 와야 TRUE가 된다.
	// 연결이 끊어지면 FALSE가 된다.
	if( GetbReadyServer() == FALSE ) {
		// 로긴서버와의 접속이 끊겼으면 연결될때까지 버퍼에 잠시 쌓아둔다.
		XPacketMem *pPacketMem = new XPacketMem( ar );
		m_listPacketToLogin.Add( pPacketMem );
		return FALSE;
	}
#endif // _XPACKET_BUFFER
	//
	return XEWinSocketClient::Send( ar );
}

BOOL XDBASvrConnection::GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes )
{
	int size = m_listPacketToLogin.size();
	if( size ) {
		int bytes = 0;
		XLIST_LOOP( m_listPacketToLogin, XPacketMem*, pPacketMem ) {
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

void XDBASvrConnection::FlushSendPacket( void )
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
		XEWinSocketClient::Send( ar );
		SAFE_DELETE( pPacketMem );
	} END_LOOP;
	m_listPacketToLogin.Clear();
	CONSOLE("패킷 방출 완료......%d bytes", bytes );
}
#ifdef _GOOGLE_VERIFY
/**
로그인서버로 구글 결제정보를 보내 중복및 유효성 검증을 하도록 한다. 
jsonReceiptFromClient: 클라로부터 받은 영수증(검증안된...)
szSignature: 구글로부터 받은 검증되고 암호화된 영수증
*/


void XDBASvrConnection::SendGoogleVerify( ID idAccount, 
										LPCTSTR jsonReceiptFromClient, 
										LPCTSTR szSignature,
										int numBuyGem )
{
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_BUY_GEM_VERIFY;
	ar << (ID)idAccount;	
	ar << numBuyGem;		// 산 개수
	ar << jsonReceiptFromClient;
	ar << szSignature;
	Send( ar );
}
#ifdef _XTHAILAND
void XDBASvrConnection::SendGoogleVerifyWithPayletter(	ID			idaccount,
																	LPCTSTR	lngUserNo,		// UserNo
																	LPCTSTR		szUserID,			// 이용자 아이디
																	LPCTSTR		szUserName,		// 이용자 이름
																	LPCTSTR		szPayAmt,			// 결제 금액												
																	LPCTSTR		szTID,				// PG사 결제번호
																	LPCTSTR		szTDate,			// PG사 결제 시간
																	LPCTSTR		szPGCode,		// 결제수단코드
																	LPCTSTR		szUserIP,			// 이용자 IP주소
																	LPCTSTR		szPackName,
																	LPCTSTR		szToken,
																	float Price,
																	int num, 
																	LPCTSTR		szJson,
																	LPCTSTR		szSignature)
{
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_VERIFY_FOR_INI3;
	ar << (ID)idaccount;
	ar << Price;
	ar << num;
	ar.WriteString( lngUserNo);
	ar.WriteString( szUserID);
	ar.WriteString( szUserName);
	ar.WriteString( szPayAmt);
	ar.WriteString( szTID);
	ar.WriteString( szTDate);
	ar.WriteString( szPGCode);
	ar.WriteString( szUserIP);
	ar.WriteString( szPackName);
	ar.WriteString( szToken);
	ar.WriteString( szJson );
	ar.WriteString( szSignature );
	Send( ar );
}
#endif
#endif	// _GOOGLE_VERIFY

void XDBASvrConnection::SendAppleVerify( ID idAcc, LPCTSTR szSku, LPCTSTR szReceipt, LPCTSTR szPurchaseInfo )
{
}

void XDBASvrConnection::RecvAccountDuplicateProcess(XPacket& p)
{
	ID idaccount = 0;
	ID newidConnect = 0;
	ID alreadyuseridConnect = 0;

	p >> newidConnect;
	p >> alreadyuseridConnect;
	p >> idaccount;

	auto pUser = XGAME_USERMNG->GetUser(idaccount);
	if (pUser) {
		pUser->SendDuplicateConnect();
	}
	XLoginConnection::sGet()->SendAccountIsAlreadyConnect( newidConnect, alreadyuseridConnect, idaccount );
}

#ifdef _xCONNECT_DUPLICATE
void XDBASvrConnection::RecvAlreadyConnectUser ( XPacket& p)
{
	ID	idAccount;
	ID idConnect;	
	p >> idConnect;
	p >> idAccount;

	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_ALREADY_CONNECT;
	ar << (ID)idAccount;
	ar << (ID)idConnect;
	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *pUser = pUserMng->GetUser( idAccount );
	if( pUser )
	{		
		ar << (ID) 1; //Already Connect		
		pUser->SendDisconnect( );
	}
	else
	{
		ar << (ID) 0; //ReConnect 요청.
	} 
	Send( ar );
}
#endif

#ifdef _xCONTENTS_BILLING
void XDBASvrConnection::SendCashBalance( XSAccount *pAccount )
{
	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_BALANCE;	
	ar << (ID)pAccount->GetidAccount();
#ifdef _XINDONESIA
	ar.WriteString( pAccount->GetName());
#else
	ar.WriteString( pAccount->GetFBUserID( ));
#endif
	//pAccount->MakePacket( ar );
	Send( ar );
}

void XDBASvrConnection::RecvCashBalance( XPacket& p)
{
	
	ID	idAccount;
	int CashBalance;
	p >> idAccount;
	p >> CashBalance;

	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *pUser = pUserMng->GetUser( idAccount );

	if( pUser )
	{
		pUser->SetCashBalance( CashBalance );
		pUser->SendCashBalance();
	} else
	{
		// 못찾았다면 클라가 게임서버에 붙기전에 게임서버가 정보를 받은것이다.
		// 로그인 대기열 계정에서 찾아본다.
		XSAccount *pAcc = AHGO->FindLoginAccount( idAccount );
		if( XASSERT(pAcc != NULL) )
		{
			pAcc->SetCashPoint( CashBalance );
		}
	}
}

void XDBASvrConnection::SendCashBuyItem( XSAccount *pAccount, LPCTSTR szSku, int num, int price, LPCTSTR szUserIP)
{
	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_BUY_ITEM;
	ar << (ID)pAccount->GetidAccount();
	ar.WriteString( pAccount->GetName());
	ar.WriteString( szSku );
	ar << ( ID)num; // Gem 숫자
	ar << ( ID)price; // Gem 총 사용 가격	
	ar.WriteString( szUserIP );
	ar.WriteString( pAccount->GetFBUserID());
	//pAccount->MakePacket( ar );
	Send( ar );
}
void XDBASvrConnection::RecvCashBuyItem( XPacket& p)
{
	//return;
	ID	idAccount;
	int AddGemNum;	
	int CashBalance;
	int Result;
	int Type;
	p >> idAccount;
	p >> Result;
	p >> Type;
	p >> AddGemNum;	
	p >> CashBalance;	

	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *pUser = pUserMng->GetUser( idAccount );

	if( pUser )
	{
		pUser->GetpAccount()->AddGem( AddGemNum );
		//pUser->SendBuyItem( Result, AddGemNum );
		pUser->SetCashBalance( CashBalance );
		pUser->SendBuyItem( Result , AddGemNum, (BOOL)Type );
		pUser->SendCashBalance();
	}
}

void XDBASvrConnection::SendCashTopup( XSAccount *pAccount, LPCTSTR szCashCode, LPCTSTR szUserIP)
{
	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_RECHARGE;
	ar << (ID)pAccount->GetidAccount();
	ar.WriteString( pAccount->GetName());
	ar.WriteString( szCashCode );
	ar.WriteString( szUserIP);
	//pAccount->MakePacket( ar );
	Send( ar );
}

void XDBASvrConnection::RecvCashTopup( XPacket& p)
{
	ID	idAccount;		
	int Result;
	int CashBalance;
	int bonusvalue;
	int totalvalue;
//	int ReChargeCash;
	p >> idAccount;
	p >> Result;		
	p >> CashBalance;
	p >> bonusvalue;
	p >> totalvalue;

	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *pUser = pUserMng->GetUser( idAccount );

	if( pUser )
	{
		pUser->SendCashReCharge(Result, totalvalue );
		pUser->SetCashBalance( CashBalance );
		pUser->SendCashBalance();
	}
}

#endif //_xCONTENTS_BILLING

void XDBASvrConnection::SendLoginAccountInfoLoad( bool bSessionKeyLogin, ID idConnect, ID idAcc )
{
  XPacket ar( (ID)xGDBPK_LOGIN_ACCOUNT_INFO_LOAD );
  ar << idAcc;
  ar << idConnect;
  ar << (BYTE)xboolToByte( bSessionKeyLogin );
  ar << (BYTE)0;
  ar << (BYTE)0;
  ar << (BYTE)0;
  Send( ar );
}

void XDBASvrConnection::RecvLoginAccountInfoLoad ( XPacket& p )
{
	ID idAccount = 0;
	ID idConnect = 0;
  char c0;
	p >> idAccount;
	p >> idConnect;
  p >> c0;  bool bSessionKeyLogin = (c0 != 0); 
  p >> c0;  xtDB retDB = (xtDB)c0;
  p >> c0 >> c0;
	XTRACE( "%s:idAcc=%d, retDB=%d", __TFUNC__, idAccount, retDB );

	auto pAccount = new XSAccount( idAccount );
	if( retDB == xDB_OK ) {
		XTRACE("%s", __TFUNC__);
		if( pAccount->DeSerialize( p ) == 0 ) {
			SAFE_RELEASE_REF( pAccount );
		}
	} else
	if( retDB == xDB_INVALID_ACCOUNT ) {
		pAccount->CreateDefaultAccount();
		// 디폴트 데이타 만들자 마자 곧바로 세이브
		SendSaveAccountInfo( pAccount, FALSE, 0 );
	} else
	if( XBREAK(retDB == xDB_NOT_FOUND) ) {
		// 계정을 찾지 못함.(이런경우는 없어야 함)
		return;
	}
	if( pAccount ) {
    if( XGame::sGet()->AddLoginAccount( pAccount ) == FALSE ) {
      CONSOLE( "이미 도착한 계정정보가 와서 삭제함.: idAcc=%d", pAccount->GetidAccount() );
      // 이미 로그인서버에서 온 데이타이므로 삭제.
      SAFE_RELEASE_REF( pAccount );
    }
	}
	//Login에 접속 해도 된다고 알려 준다.
	if( pAccount )
		XLoginConnection::sGet()->SendLoginAccountInfo( idAccount, idConnect );
}

ID XDBASvrConnection::SendAccountNickNameDuplicateCheck(ID idaccount, _tstring  szAccountNickName, ID idPacket)
{
	XPacket ar((ID)xGDBPK_ACCOUNT_NICKNAME_DUPLICATE);
	ar << idaccount;
	ar << szAccountNickName;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendAccountNameDuplicateCheck(ID idaccount, _tstring  szAccountName, ID idPacket)
{
	XPacket ar((ID)xGDBPK_ACCOUNT_NAME_DUPLICATE);
	ar << idaccount;
	ar << szAccountName;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendAccountNameRegistCheck(ID idaccount, _tstring  szAccountName, _tstring  szPassword, ID idPacket)
{
	XPacket ar((ID)xGDBPK_ACCOUNT_NAME_REGIST_CHECK);
	ar << idaccount;
	ar << szAccountName;
	ar << szPassword;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendAccountNameRegist(ID idaccount, _tstring  szAccountName, _tstring  szPassword, ID idPacket)
{
	XPacket ar((ID)xGDBPK_ACCOUNT_NAME_REGIST);
	ar << idaccount;
	ar << szAccountName;
	ar << szPassword;	
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendAccountNickNameChange(ID idaccount, _tstring  szAccountNickName, ID idPacket)
{
	XPacket ar((ID)xGDBPK_ACCOUNT_NICKNAME_CHANGE);
	ar << idaccount;
	ar << szAccountNickName;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendUpdateLogin(ID idaccount, ID GameServerID, _tstring lastconnectip)
{
	XPacket ar((ID)xGDBPK_SAVE_ACCOUNT_LOGIN);
	ar << idaccount;
	ar << GameServerID;
	ar << lastconnectip;	
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendGooglePayload(ID idaccount, int producttype, _tstring productID, ID idClientConnection, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_PAYLOAD);
	ar << idaccount;
	ar << producttype;
	ar << productID;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGoogleBuyCashVerify(ID idaccount, int producttype, _tstring strProductID, _tstring strReceipt, ID idClientConnection, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_GOOGLE);
	ar << idaccount;
	ar << producttype;
	ar << strProductID;
	ar << strReceipt;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendAppleBuyCashVerify(ID idaccount, _tstring strProductID, _tstring strReceipt, _tstring strPurchaseInfo, ID idClientConnection, ID idPacket) //szSku, LPCTSTR szReceipt, LPCTSTR szPurchaseInfo 
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_APPLE);
	ar << idaccount;
	ar << strProductID;
	ar << strReceipt;
	ar << strPurchaseInfo;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

