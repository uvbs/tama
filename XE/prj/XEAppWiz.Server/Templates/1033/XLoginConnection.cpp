#include "stdafx.h"
#include "XLoginConnection.h"
#include "XDBASvrConnection.h"
#include "server/XSAccount.h"
#include "XMain.h"
#include "server/XWinConnection.h"
#include "XPacketLG.h"
#include "XPacketGDB.h"
#include "XSocketForClient.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XGame.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XLoginConnection* XLoginConnection::s_pSingleton = nullptr;

void XLoginConnection::sCreateSingleton( const std::string& strIP
	, WORD port
	, XNetworkDelegate *pDelegate )
{
	CONSOLE( "try connecting to login server: %s:%d", C2SZ( strIP.c_str() ), port );
	s_pSingleton = new XLoginConnection( strIP.c_str(), port, pDelegate );
}

void XLoginConnection::sDestroySingleton()
{
	SAFE_DELETE( s_pSingleton );
}

//////////////////////////////////////////////////////////////////////////
#define SIZE_QUEUE		(0xffff*4)
XLoginConnection::XLoginConnection( const std::string& strIP, unsigned short port, XNetworkDelegate *pDelegate, DWORD param )
	: XEWinSocketClient( strIP.c_str(), port, pDelegate, SIZE_QUEUE, param ) 
{
}

void XLoginConnection::OnConnect( void )
{
}

void XLoginConnection::OnDisconnect( void )
{
//	XBREAK(1);
}


BOOL XLoginConnection::Send( XPacket& ar )
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

BOOL XLoginConnection::GetSizeSendPacketBuffer( int *pOutNum, int *pOutBytes )
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

void XLoginConnection::FlushSendPacket( void )
{
	XBREAK( IsDisconnected() == TRUE );
	int size = m_listPacketToLogin.size();
	if( size == 0 )
		return;
	CONSOLE("로긴서버가 끊어져 못보낸 패킷들을 보내기 시도......%d개", size );
	int bytes = 0;
	XLIST_LOOP( m_listPacketToLogin, XPacketMem*, pPacketMem ) {
		XPacket ar( pPacketMem );
		bytes += pPacketMem->GetSize();
		XEWinSocketClient::Send( ar );
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
	switch( packetid ) {
	case xLS2GS_NEW_ACCOUNT:				RecvNewAccount( p );	break;
	case xLS2GS_ACCOUNT_IS_ALREADY_CONNECT:	RecvIsAleadyConnectAccount ( p );	break;
	case xLS2GS_LOGIN_ACCOUNT_INFO:			RecvLoginAccountInfoLoad ( p );		break;
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		break;
	}

	return TRUE;
}

// void XLoginConnection::RecvSVRConnectSuccess( XPacket& p )
// {
// }

void XLoginConnection::RecvIsAleadyConnectAccount ( XPacket& p )
{
	ID	idAccount = 0;
	ID newidConnect = 0;
	ID alreadyuseridConnect = 0;

	p >> newidConnect;
	p >> idAccount;
	if( XGAME_USERMNG ) {		
		XGameUser* pUser = XGAME_USERMNG->GetUser( idAccount );
		if( pUser ) {
			alreadyuseridConnect = pUser->GetidConnect();
			if (alreadyuseridConnect != newidConnect) {					
				pUser->SendDuplicateConnect();
			} else {
				XDBASvrConnection::sGet()->SendUpdateAccountLogout( newidConnect, alreadyuseridConnect, idAccount );
			} 
			_tstring strLog = _T("Duplicate Logout( ");
			strLog += pUser->GetpConnect()->GetszIP();
			strLog += _T(" )");
			pUser->AddLog(XGAME::xULog_Account_Duplicate_connect, strLog);
			SendAccountIsAlreadyConnect( newidConnect, alreadyuseridConnect, idAccount );
		} else {//접속한것으로 알고 있는데 실제 유저가 없으므로 이런 경우는 직접 LastServerid = 0 으로 바꿔 줘야 겠지?
			XDBASvrConnection::sGet()->SendUpdateAccountLogout( newidConnect, alreadyuseridConnect, idAccount );
		}
	}
}

void XLoginConnection::SendAccountIsAlreadyConnect( ID idConnectNew, ID idConnectAlready, ID idAcc )
{
	XPacket ar( (ID)xLS2GS_ACCOUNT_IS_ALREADY_CONNECT );
	ar << idConnectNew;
	ar << idConnectAlready;
	ar << idAcc;
	Send( ar );
}

/**
 @brief Login에게 접속 해도 된다고 알려 준다.
*/
void XLoginConnection::SendLoginAccountInfo( ID idAcc, ID idConnect )
{
	XPacket ar( (ID)xLS2GS_LOGIN_ACCOUNT_INFO );
	ar << idAcc;
	ar << idConnect;
	Send( ar );
}
/**
 @brief DB에서 idAccount의 계정정보를 읽어오라는 요청
*/
void XLoginConnection::RecvLoginAccountInfoLoad ( XPacket& p )
{
	ID	idAccount = 0;
	ID idConnect = 0;
  BYTE b0;
	p >> idConnect;
	p >> idAccount;	
  p >> b0;  bool bSessionKeyLogin = (b0 != 0);
  p >> b0 >> b0 >> b0;
	XTRACE( "%s:idAcc=%d", __TFUNC__, idAccount );
	if( XGAME_USERMNG )	{
		auto pUser = XGAME_USERMNG->GetUser( idAccount );
		if( pUser ) {			
			//pUser->SendDuplicateConnect( );
			pUser->DoDisconnect();
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
	_tstring strNickName;
	ID idConnect;
	ID idAccount;
	p >> idConnect;
	p >> idAccount;
	p >> strNickName;
	auto pAccount = new XSAccount(idAccount);
	// 디폴트 데이타를 넣음.
	pAccount->CreateDefaultAccount();
	pAccount->SetstrName(strNickName);
	// 대기열에 넣고
	if( pAccount && XGame::sGet()->AddLoginAccount( pAccount ) == FALSE ) {
		CONSOLE( "이미 도착한 계정정보가 와서 삭제함.: idAcc=%d", pAccount->GetidAccount() );
		// 이미 로그인서버에서 온 데이타이므로 삭제.
		SAFE_RELEASE_REF( pAccount );
		XBREAK(1);
		return;	// 일단 리턴
#pragma message("여기 처리 꼼꼼하게 다시 할것")
	}
	// 디폴트 데이타 곧바로 세이브
	//Login에 접속 해도 된다고 알려 준다.
	XDBASvrConnection::sGet()->SendSaveAccountInfo( pAccount, TRUE, idConnect );	// 세이브한 결과 요청을 한다.
}

/**
 @brief 게임서버쪽의 로드가 모두 끝나서 유저받을 준비가 됐다는걸 알림
*/
void XLoginConnection::SendDidFinishLoad( void )
{
	int n = 0;
	XPacket ar((ID)xLS2GS_DID_FINISH_LOAD);
	WORD port = MAIN->GetpSocketForClient()->GetPort();
	ar << port;
	ar << MAIN->GetGSvrID();
	ar << MAIN->GetstrIPExternal();
	Send( ar );
//	
}
//
//// 게임서버에서 로긴서버로 계정등록정보를 보냄
//void XLoginConnection::SendRegisterAccount( XAhgoUser *pUser, LPCTSTR szName, LPCTSTR szID, LPCTSTR szPassword1, LPCTSTR szPassword2 )
//{
////	XPacket ar;
////	ar << (ID)xLPK_REGISTER_ACCOUNT;
//////	ar << pUser->GetidAccount();
////	ar << pUser->GetidUser();
////	ar << szName;
////	ar << szID;
////	ar << szPassword1;
////	ar << szPassword2;
////	Send( ar );
//}
//
//void XLoginConnection::SendUpdateFreeMoneyStoneDate(ID idaccount, int datestamp)
//{
//	//XPacket ar;
//	//ar << (ID)xLPK_ACCOUNT_FREEMONEYSTONE_TIMESTAMP;
//	//ar << idaccount;
//	//ar << datestamp;
//	//Send( ar );
//}
//void XLoginConnection::SendUpdateFreeGem(ID idaccount, int datestamp)
//{
//	/*XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_FREEGEM_TIMESTAMP;
//	ar << idaccount;
//	ar << datestamp;
//	Send( ar );*/
//}
//void XLoginConnection::SendUpdateFirstRefill(ID idaccount, int datestamp)
//{
//	//XPacket ar;
//	//ar << (ID)xLPK_ACCOUNT_FIRSTREFILL;
//	//ar << idaccount;
//	//ar << datestamp;
//	//Send( ar );
//}
//
//// 로그인서버로부터 계정접속 정보가 옴
//void XLoginConnection::RecvLoginAccountInfo( XPacket& p )
//{
////	XSAccount *pAccount = new XSAccount;
////	pAccount->Create();
////	if( pAccount->RestorePacket( p ) == FALSE )
////	{
////		CONSOLE( "invalid account from loginsvr: id:%s, idAcc=0x%08x", pAccount->GetszID(), pAccount->GetidAccount() );
////		SAFE_DELETE( pAccount );
////		/*
////		이렇게 불량계정 처리된 계정은 어떻게 처리해야 하는가.
////		. 2가지 처리
////		1. 새계정 생성 실패
////			- 새계정을 생성하다 실패한경우로 인식이 되면 다시 접속할때 새계정생성 화면으로 다시 유도한다.
////		2. 그냥 데이타가 깨져있는등 불량계정일때. 
////			. 로긴서버로 idAcc를 보내서 다시한번 검증하게 한다음 문제있는 계정이면 일단 블럭처리
////			. 계정을 삭제해야 하나 말아야 하나.
////			. 관리자가 최대한 빨리 알수있도록 조치.
////		*/
////		return;
////	}
////	if( XBREAK( pAccount->GetmaxCardInven() == 0 || pAccount->GetmaxItemInven() == 0 ) )
////	{
////		SAFE_DELETE( pAccount );
////	}
////// 로긴서버로 로그인신호 무한대로 보내면 뻗을수 있다. 여기에 대한 처리할것.
////#pragma message("check this")
////	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
////	if( pAccount && AHGO->AddLoginAccount( pAccount ) == FALSE )
////	{
////#ifdef _DEV
////		CONSOLE( "이미 도착한 계정정보가 와서 삭제함.: idAcc=%d", pAccount->GetidAccount() );
////#endif
////		// 이미 로그인서버에서 온 데이타이므로 삭제.
////		SAFE_DELETE( pAccount );
////	}
////	
////#ifdef _xCONTENTS_BILLING
////	SendCashBalance( pAccount );
////#endif
//}
// 
//// 로그인서버로 계정정보를 보내 세이브 하게 한다.
//void XLoginConnection::SendSaveAccountInfo( XSAccount *pAccount, XEWinConnectionInServer *pConnect )
//{
//	//if( pAccount == NULL )		// 새계정만들기전에 접속끊으면 널일수 있다.
//	//	return;
//	//XPacket ar;
//	//ar << (ID)xLPK_SAVE_ACCOUNT_INFO;
//	//ar << pAccount->GetidAccount();
//	//pAccount->MakePacket( ar );  //1
//	//if( pConnect )
//	//	ar << pConnect->GetszIP();
//	//else 
//	//	ar << _T("new");
//	//Send( ar );
//}
//
//#ifdef _xCONTENTS_USERLOG
//void XLoginConnection::SendUserLog( XSAccount *pAccount, int Logtype, LPCTSTR szLog )
//{
//	if( pAccount == NULL )		// 유저에 관련된 Log 이므로 User 정보가 있어야 한다.
//		return;
//	XPacket ar;
//	ar << (ID)xLPK_USERLOG_ADD;
//	ar << (ID)pAccount->GetidAccount();
//	ar << (int)Logtype;
//	ar << pAccount->GetName();
//	ar << szLog;
//	Send( ar );
//}
//#endif //_xCONTENTS_USERLOG
//
//#ifdef _xCONTENTS_SERVER_EVENT
////void XLoginConnection::SendUserLog( XSAccount *pAccount, int Logtype, LPCTSTR szLog )
////{
////	if( pAccount == NULL )		// 유저에 관련된 Log 이므로 User 정보가 있어야 한다.
////		return;
////	XPacket ar;
////	ar << (ID)xLPK_SERVER_EVENT;
////	Send( ar );
////}
//#endif //_xCONTENTS_SERVER_EVENT
//
//
//
//// 로그인서버로부터 페이스북 계정으로 새계정이 생성되었음을 받았다.
//#ifdef _xCONTENTS_FACEBOOK
//void XLoginConnection::RecvNewAccountForFacebook( XPacket& p )
//{
//#ifdef _XINDONESIA
//	XBREAK(1);	// 인도네시아버전에서는 이쪽으로 들어올일이 없다.
//#endif
//	DWORD idAccount;
//	p >> idAccount;
//	int typeSummon;
//	p >> typeSummon;		// 유저가 선택한 소환수 타입 130320 추가
//	
//	TCHAR szFBUserID[80];
//	TCHAR szFBUserName[80];	
//	TCHAR szNickName[80];	
//	p.ReadString(  szFBUserID );
//	p.ReadString(  szFBUserName );	
//	p.ReadString(  szNickName );	
//
//
//	XSAccount *pAccount = new XSAccount( idAccount );
//	pAccount->Create();
//	// 빈계정이 돌아다니는 시간을 최소한으로 줄이기 위해 여기서 디폴트 계정을 만들고
//	// 곧바로 로긴서버로 세이브 한다.
//	pAccount->NewAccount( typeSummon );
//#ifdef _XINDONESIA
////	pAccount->SetSessionID( szBuff1 );
////	pAccount->SetSessionToken( szBuff2); 
//#endif
//	pAccount->SetFaceBookUserID( szFBUserID);
//	pAccount->SetFaceBookUserName( szFBUserName);
//	pAccount->SetName( szNickName );
//	SendSaveAccountInfo( pAccount, NULL );
//	// pAccount는 디폴트 값으로 채워져 있어야 한다.
//	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
//	AHGO->AddLoginAccount( pAccount );
//#ifdef _xCONTENTS_BILLING
//	SendCashBalance( pAccount );
//#endif
//}
//void XLoginConnection::RecvRegistAccountResult( XPacket& p )
//{
//	ID idaccount =0;
//	p >> idaccount;
//	int i0;
//	XPH::xtLytoResist result;
//	p >> i0;	result = (XPH::xtLytoResist) i0;;
//	TCHAR FBUserID[255];
//	TCHAR FBUserName[255];
//	TCHAR szNickname[255];
//	p.ReadString( FBUserID);
//	p.ReadString(FBUserName );
//	p.ReadString( szNickname );
//		
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idaccount );
//
//	if( pUser )
//	{
//		pUser->SendFBRegistAccountResult( idaccount, result, FBUserID, FBUserName, szNickname);
//	}
//	//	// 등록성공후
////	{
////		MAKE_PCONNECT( pConnect );
////		XPacket ar;
////		ar << (ID)xCL2GS_REGIST_FACEBOOK;
////		ar << 2;	// 1:success 0:fail 2:exist
////		ar << szuserid;
////		ar << szusername;
////		pConnect->Send( ar );
////	}
//}
//
//void XLoginConnection::SendRegistFBAccount( ID idaccount, LPCTSTR UserName, LPCTSTR FBuserid, LPCTSTR FBusername)
//{
//	XPacket ar;
//	ar << (ID)xLPK_REGIST_FB_ACCOUNT;
//	ar << (ID)idaccount;
//	ar.WriteString( UserName );
//	ar.WriteString( FBuserid);
//	ar.WriteString( FBusername);
//	Send( ar );
//}
//
//void XLoginConnection::RecvLoginFBAccount( XPacket& p )
//{
//	ID idAcc;
//	TCHAR szFbUserId[256];
//	TCHAR szFbUserName[256];
//	TCHAR szUserName[256];
////	TCHAR szSessionMemberID[256];
////	TCHAR szSessionToken[256];
//
//	XPH::xtLytoResult result;
//	int i0;
//
//	p >> idAcc;
//	p >> i0;	result = (XPH::xtLytoResult) i0;
//	p.ReadString( szFbUserId );
//	p.ReadString( szFbUserName );
//	p.ReadString( szUserName );
////	p.ReadString( szSessionMemberID );
////	p.ReadString( szSessionToken );
//
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idAcc );
//
//	if( pUser )
//	{
////#ifdef _XINDONESIA
////		if( pUser->GetpAccount() )
////		{
////			pUser->GetpAccount()->SetSessionID(szSessionMemberID);
////			pUser->GetpAccount()->SetSessionToken( szSessionToken);
////		}		
////#endif
//		pUser->SendLoginFacebookOnUUID( idAcc, result, szFbUserId, szFbUserName, szUserName );
//	}
//}
//
//// UUID로 로그인상태에서 페북로그인 시도
//void XLoginConnection::SendLoginFBAccount( ID idAccount, LPCTSTR szUserName, LPCTSTR szFBuserid, LPCTSTR szFBusername)
//{
//	XPacket ar;
//	ar << (ID)xLPK_LOGIN_FB_ACCOUNT;
//	ar << (ID)idAccount;
//	ar << szUserName;
//	ar << szFBuserid;
//	ar << szFBusername;
//	Send( ar );
//}
//
//#endif  //_xCONTENTS_FACEBOOK
//
//// 로그인서버로부터 UUID로 새계정이 생성되었음을 받았다.
//void XLoginConnection::RecvNewAccountForUUID( XPacket& p )
//{
////	DWORD idAccount;
////	p >> idAccount;
////	int typeSummon;
////	p >> typeSummon;		// 유저가 선택한 소환수 타입 130320 추가
////
////	XSAccount *pAccount = new XSAccount( idAccount );
////	pAccount->Create();
////	// 빈계정이 돌아다니는 시간을 최소한으로 줄이기 위해 여기서 디폴트 계정을 만들고
////	// 곧바로 로긴서버로 세이브 한다.
////	pAccount->NewAccount( typeSummon );
//////#ifdef _XINDONESIA
////	TCHAR NickName[255];
////	p.ReadString( NickName );	
////	if( XE::IsHave(NickName) )
////		pAccount->SetName( NickName );
//////#endif	
////	SendSaveAccountInfo( pAccount, NULL );
////	// pAccount는 디폴트 값으로 채워져 있어야 한다.
////	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
////	AHGO->AddLoginAccount( pAccount );
////#ifdef _xCONTENTS_BILLING
////	SendCashBalance( pAccount );
////#endif
//}
//

//
//#ifdef _xCONTENTS_SERVER_EVENT
//void XLoginConnection::SendRequestServerEvent()
//{
//	XPacket ar;
//	ar << (ID)xLPK_SERVER_EVENT;
//	Send( ar );
//}
//
//
//void XLoginConnection::RecvServerEvent(XPacket& p)
//{
//	int i = 0;
//	//EventMgr 같은거 만들어서 정보를 채우고 비교하고 적용하고 하는거 만들자.
//	XServerEventMng*	pSvrEvt = AHGO->GetpSvrEvt();
//	if( pSvrEvt )
//	{
//		pSvrEvt->RestorePacket( p );		
//		pSvrEvt->CheckSvrEvtRunning();
////		if( newevt )
////		{			
//			XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	
//			XPacket ar;
//			ar << (ID)xCL2GS_SVR_NOW_EVT;
//			pSvrEvt->MakePacket( ar );
//			pUserMng->SendAllUser( ar );
////		}
//	}
//}
//#endif 
//
//
//#ifdef _GOOGLE_VERIFY
///**
//로그인서버로 구글 결제정보를 보내 중복및 유효성 검증을 하도록 한다. 
//jsonReceiptFromClient: 클라로부터 받은 영수증(검증안된...)
//szSignature: 구글로부터 받은 검증되고 암호화된 영수증
//*/
//
//
//void XLoginConnection::SendGoogleVerify( ID idAccount, 
//										LPCTSTR jsonReceiptFromClient, 
//										LPCTSTR szSignature,
//										int numBuyGem )
//{
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_BUY_GEM_VERIFY;
//	ar << (ID)idAccount;	
//	ar << numBuyGem;		// 산 개수
//	ar << jsonReceiptFromClient;
//	ar << szSignature;
//	Send( ar );
//}
//#ifdef _XTHAILAND
//void XLoginConnection::SendGoogleVerifyWithPayletter(	ID			idaccount,
//																	LPCTSTR	lngUserNo,		// UserNo
//																	LPCTSTR		szUserID,			// 이용자 아이디
//																	LPCTSTR		szUserName,		// 이용자 이름
//																	LPCTSTR		szPayAmt,			// 결제 금액												
//																	LPCTSTR		szTID,				// PG사 결제번호
//																	LPCTSTR		szTDate,			// PG사 결제 시간
//																	LPCTSTR		szPGCode,		// 결제수단코드
//																	LPCTSTR		szUserIP,			// 이용자 IP주소
//																	LPCTSTR		szPackName,
//																	LPCTSTR		szToken,
//																	float Price,
//																	int num, 
//																	LPCTSTR		szJson,
//																	LPCTSTR		szSignature)
//{
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_VERIFY_FOR_INI3;
//	ar << (ID)idaccount;
//	ar << Price;
//	ar << num;
//	ar.WriteString( lngUserNo);
//	ar.WriteString( szUserID);
//	ar.WriteString( szUserName);
//	ar.WriteString( szPayAmt);
//	ar.WriteString( szTID);
//	ar.WriteString( szTDate);
//	ar.WriteString( szPGCode);
//	ar.WriteString( szUserIP);
//	ar.WriteString( szPackName);
//	ar.WriteString( szToken);
//	ar.WriteString( szJson );
//	ar.WriteString( szSignature );
//	Send( ar );
//}
//#endif
//#endif	// _GOOGLE_VERIFY
//
//void XLoginConnection::SendAppleVerify( ID idAcc, LPCTSTR szSku, LPCTSTR szReceipt, LPCTSTR szPurchaseInfo )
//{
//	//XPacket ar;
//	//ar << (ID)xLPK_APPLE_VERIFY;
//	//ar << (ID)idAcc;	
//	//ar << szSku;
//	//ar << szReceipt;
//	//ar << szPurchaseInfo;
//	//Send( ar );
//}
//
//#ifdef _xCONNECT_DUPLICATE
//void XLoginConnection::RecvAlreadyConnectUser ( XPacket& p)
//{
//	ID	idAccount;
//	ID idConnect;	
//	p >> idConnect;
//	p >> idAccount;
//
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_ALREADY_CONNECT;
//	ar << (ID)idAccount;
//	ar << (ID)idConnect;
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idAccount );
//	if( pUser )
//	{		
//		ar << (ID) 1; //Already Connect		
//		pUser->SendDisconnect( );
//	}
//	else
//	{
//		ar << (ID) 0; //ReConnect 요청.
//	} 
//	Send( ar );
//}
//#endif
//
//#ifdef _xCONTENTS_BILLING
//void XLoginConnection::SendCashBalance( XSAccount *pAccount )
//{
//	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
//		return;
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_CASH_BALANCE;	
//	ar << (ID)pAccount->GetidAccount();
//#ifdef _XINDONESIA
//	ar.WriteString( pAccount->GetName());
//#else
//	ar.WriteString( pAccount->GetFBUserID( ));
//#endif
//	//pAccount->MakePacket( ar );
//	Send( ar );
//}
//
//void XLoginConnection::RecvCashBalance( XPacket& p)
//{
//	
//	ID	idAccount;
//	int CashBalance;
//	p >> idAccount;
//	p >> CashBalance;
//
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idAccount );
//
//	if( pUser )
//	{
//		pUser->SetCashBalance( CashBalance );
//		pUser->SendCashBalance();
//	} else
//	{
//		// 못찾았다면 클라가 게임서버에 붙기전에 게임서버가 정보를 받은것이다.
//		// 로그인 대기열 계정에서 찾아본다.
//		XSAccount *pAcc = AHGO->FindLoginAccount( idAccount );
//		if( XASSERT(pAcc != NULL) )
//		{
//			pAcc->SetCashPoint( CashBalance );
//		}
//	}
//}
//
//void XLoginConnection::SendCashBuyItem( XSAccount *pAccount, LPCTSTR szSku, int num, int price, LPCTSTR szUserIP)
//{
//	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
//		return;
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_CASH_BUY_ITEM;
//	ar << (ID)pAccount->GetidAccount();
//	ar.WriteString( pAccount->GetName());
//	ar.WriteString( szSku );
//	ar << ( ID)num; // Gem 숫자
//	ar << ( ID)price; // Gem 총 사용 가격	
//	ar.WriteString( szUserIP );
//	ar.WriteString( pAccount->GetFBUserID());
//	//pAccount->MakePacket( ar );
//	Send( ar );
//}
//void XLoginConnection::RecvCashBuyItem( XPacket& p)
//{
//	//return;
//	ID	idAccount;
//	int AddGemNum;	
//	int CashBalance;
//	int Result;
//	int Type;
//	p >> idAccount;
//	p >> Result;
//	p >> Type;
//	p >> AddGemNum;	
//	p >> CashBalance;	
//
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idAccount );
//
//	if( pUser )
//	{
//		pUser->GetpAccount()->AddGem( AddGemNum );
//		//pUser->SendBuyItem( Result, AddGemNum );
//		pUser->SetCashBalance( CashBalance );
//		pUser->SendBuyItem( Result , AddGemNum, (BOOL)Type );
//		pUser->SendCashBalance();
//	}
//}
//
//void XLoginConnection::SendCashTopup( XSAccount *pAccount, LPCTSTR szCashCode, LPCTSTR szUserIP)
//{
//	if( pAccount == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
//		return;
//	XPacket ar;
//	ar << (ID)xLPK_ACCOUNT_CASH_RECHARGE;
//	ar << (ID)pAccount->GetidAccount();
//	ar.WriteString( pAccount->GetName());
//	ar.WriteString( szCashCode );
//	ar.WriteString( szUserIP);
//	//pAccount->MakePacket( ar );
//	Send( ar );
//}
//
//void XLoginConnection::RecvCashTopup( XPacket& p)
//{
//	ID	idAccount;		
//	int Result;
//	int CashBalance;
//	int bonusvalue;
//	int totalvalue;
////	int ReChargeCash;
//	p >> idAccount;
//	p >> Result;		
//	p >> CashBalance;
//	p >> bonusvalue;
//	p >> totalvalue;
//
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *pUser = pUserMng->GetUser( idAccount );
//
//	if( pUser )
//	{
//		pUser->SendCashReCharge(Result, totalvalue );
//		pUser->SetCashBalance( CashBalance );
//		pUser->SendCashBalance();
//	}
//}
//
//#endif //_xCONTENTS_BILLING
