#include "stdafx.h"
#include "XDBASvrConnection.h"
#include "XLoginConnection.h"
#include "server/XSAccount.h"
#include "XMain.h"
#include "XFramework/server/XWinConnection.h"
#include "XPacketGDB.h"
#include "XGameUserMng.h"
#include "XGameUser.h"
#include "XGame.h"
#include "XPacketLG.h"
#include "XLegion.h"
#include "XPacketCG.h"
#include "XSocketSvrForClient.h"
#include "XRanking.h"
#include "server/XGuildMgr.h"
#include "XClientConnection.h"
#include "XSpots.h"
#include "XGlobalConst.h"
#include "XCampObj.h"

using namespace XGAME;
#define BTRACE(F,...)		XTRACE(F,__VA_ARGS__)
//#define BTRACE(F,...)		(0)

extern XRanking* RANKING_DB;
//extern XGuildMgr* GUILD_MGR;


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _DUMMY_GENERATOR
	#define SIZE_QUEUE		(1024 * 1024 * 50)
#else
	#define SIZE_QUEUE		(1024 * 1024 * 10)
#endif // _DUMMY_GENERATOR

std::shared_ptr<XDBASvrConnection> XDBASvrConnection::s_spInstance;
// XLockCS XDBASvrConnection::s_Lock;
////////////////////////////////////////////////////////////////
std::shared_ptr<XDBASvrConnection>& XDBASvrConnection::sGet() {	/*s_Lock.Lock(__TFUNC__);	*/if( s_spInstance == nullptr ) 		s_spInstance = std::make_shared<XDBASvrConnection>();	/*s_Lock.Unlock();*/	return s_spInstance;}
void XDBASvrConnection::sDestroyInstance() {
	s_spInstance.reset();
}
//////////////////////////////////////////////////////////////////////////
XDBASvrConnection::XDBASvrConnection() 
	: XESocketClientInServer( SIZE_QUEUE )
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

bool XDBASvrConnection::OnCreate( const std::string& strcIP
																, WORD port
																, XNetworkDelegate *pDelegate
																, DWORD param /*= 0*/ )
{
	return __super::OnCreate( strcIP, port, pDelegate, param );
}

// void XDBASvrConnection::CreateRanking()
// {
// 	//m_pRanking = new XRanking;
// }

void XDBASvrConnection::OnLogined()
{
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	XConsole( _T( "DBA서버로 연결되었습니다." ) );
#endif
	SendRequestRankInfo();

	XPacket ar1((ID)xGDBPK_GUILD_INFO);	//서버 실행 했으면 길드 정보 알려 달라고 DB 에 요청 한다.
	Send(ar1);
}

void XDBASvrConnection::OnDisconnect()
{
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
	XConsole( _T( "DBA서버로의 연결이 끊겼습니다." ) );
#endif
//	XBREAK(1);
}

BOOL XDBASvrConnection::CallRecvCallback( ID idKey, XPacket& p ) 
{
	std::map<ID,xCALLBACK>::iterator itor;
	itor = m_mapCallback.find( idKey );
	if( itor == m_mapCallback.end() ) 
	{
		// 보내는쪽에서 idKey값을 보내줬는지 확인할것.
		XBREAKF( 1, "not found idKey from DBAgentSvr 0x%8x", idKey );
		return FALSE;
	}
	ID idClientConnect = (itor->second).idClientConnect;
	XGameUser *pOwner = nullptr;
	auto pServerForClient = MAIN->GetpSocketForClient();
	if( XASSERT(pServerForClient) ) {
		auto spConnect = pServerForClient->GetspConnect( idClientConnect );
		if( spConnect ) {
			XAUTO_LOCK3( spConnect );
			auto spUser = spConnect->GetspUser();
			if( spUser ) {
				pOwner = SafeCast<XGameUser*>( spUser.get() );
			}
		}
		if( pOwner ) {
			( itor->second ).callbackFunc( pOwner, p );
		}
		m_mapCallback.erase( itor );
	}
// 	XGameUser *pOwner = NULL;
// 	auto spConnect = MAIN->GetpSocketForClient()->GetspConnection( idClientConnect );
// 	if( spConnect )
// 		pOwner = SafeCast<XGameUser*>( spConnect->GetspUser().get() );
// 	if( pOwner )
// 		(itor->second).callbackFunc( pOwner, p );
// 	m_mapCallback.erase( itor );
	return TRUE;
}

//패킷 처리
BOOL XDBASvrConnection::ProcessPacket( DWORD idPacket, ID idKey, XPacket& p )
{
	XAUTO_LOCK;
	ID idClientConnection;
	p >> idClientConnection;	
	// idClientConnection은 이제 시스템이 관리하므로 따로 보낼필요는 없지만 
	// 자동콜백을 쓰려면 코드호환성을 위해서는 0이 아닌값을 넣어줘야 함. 
	if( idClientConnection == 0 )
	{
		switch( idPacket )
		{
		case xGDBPK_LOGIN_ACCOUNT_INFO_LOAD: RecvLoginAccountInfoLoad( p );	break;
		case xGDBPK_UPDATE_ACCOUNT_LOGOUT: RecvAccountDuplicateProcess(p); break;
		case xGDBPK_SAVE_ACCOUNT_INFO:	RecvSaveAccountInfo( p ); break;
		case xGDBPK_SAVE_DUMMY_ACCOUNT: RecvSaveDummyAccount( p ); break;
		case xGDBPK_LOGIN_UNLOCK_FOR_BATTLE:  RecvUnlockLoginForBattle( p );  break;
		case xGDBPK_POST_INFO:			RecvPostInfo(p); break;
		case xGDBPK_RANK_INFO:			RecvRankInfo(p); break;
		case xGDBPK_GUILD_INFO:			RecvGuildInfoAll(p); break;
		case xGDBPK_GAME_NOTIFY:		RecvNotify(p); break;
//		case xGDBPK_GUILD_UPDATE:		RecvGuildInfoUpdate(p); break;
		case xGDBPK_GUILD_UPDATE_USER:	RecvGuildUserEvent( p );	break;
		case xGDBPK_GUILD_EVENT:	RecvGuildEvent( p );	break;
		case xGDBPK_GUILD_UPDATE_CAMPAIGN:		RecvGuildUpdateCampaign( p ); break;
//		case xGDBPK_GUILD_JOIN_ACCEPT:	RecvGuildJoinAccept(p); break;
		case xGDBPK_GUILD_JOIN_RESULT:	RecvGuildAcceptResult(p); break;		
//		case xGDBPK_GUILD_KICK:			RecvGuildKickResult(p); break;		
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
// 
/**
 @brief 로그인서버로 계정정보를 보내 세이브 하게 한다.
 bReq가 FALSE이면 idConnect는 0이어도 된다.
 @param idReqPacket bReq를 TRUE로 해서 결과를 돌려받을때 사용하게 할 패킷아이디
*/
void XDBASvrConnection::SendSaveAccountInfo( XSPSAcc spAcc,
											BOOL bReq,
											ID idConnectClient,	// 로긴서버로부터온 클라이언트 커넥션 아이디
											ID idReqPacket,
											int cnt )
{	
	MAIN->m_fpsToDBA.Process(1);
	XPacket ar((ID)xGDBPK_SAVE_ACCOUNT_INFO, true, 0x10000);
	ar << spAcc->GetidAccount();
	BTRACE( "G0:%s:idAcc=%d nick=%s", __TFUNC__, spAcc->GetidAccount(), spAcc->GetstrName() );
	ar.SetbForDB( true );
	spAcc->Serialize( ar );
	ar << ((bReq)? 11 : 0);	// 성공 확인 요청
	ar << idConnectClient;
	ar << ((idReqPacket)? idReqPacket : ar.GetidPacket());
	ar << (int)( ( 0x80000000 ) | cnt );
	Send( ar );
	// RecvSaveAccountInfo, RecvSaveDummyAccount에서 사용
}

/**
 세이브가 성공했다고 돌아옴
 DBSvr의 XGameSvrConnection::cbSaveAccount()로 부터 호출받음.
*/
void XDBASvrConnection::RecvSaveAccountInfo( XPacket& p )
{
	MAIN->m_fpsFromDBASave.Process();
	ID idAccount;
	ID idConnect;
	BOOL bFound;
	_tstring strUUID;
	p >> bFound;
	p >> idAccount;
	p >> idConnect;
	p >> strUUID;
	BTRACE( "%s: idAcc=%d ", __TFUNC__, idAccount );
	//Login에 접속 해도 된다고 알려 준다.
	LOGIN_SVR->SendLoginAccountInfo( idAccount, idConnect, strUUID );
}

/**
 @brief 더미 계정 저장이 완료되었다.
*/
void XDBASvrConnection::RecvSaveDummyAccount( XPacket& p )
{
	ID idAccount;
	ID idConnect;
	BOOL bFound;
	_tstring strUUID;
	p >> bFound;
	p >> idAccount;
	p >> idConnect;
	p >> strUUID;
	//Login에 저장이 완료되었음을 알린다.
	LOGIN_SVR->SendSaveDummyComplete( idAccount );
}



//로그인서버로부터 계정등록 성공 메시지를 받음.
void XDBASvrConnection::RecvSuccessRegisterAccount( XPacket& p )
{
	//DWORD idAccount;
	//TCHAR szName[ 64 ];
	//TCHAR szID[ 64 ];
	//TCHAR szPassword[ 64 ];
	//p >> idAccount;
	//p.ReadString( szName );
	//p.ReadString( szID );
	//p.ReadString( szPassword );
	//// 계정아이디로 유저를 찾음.
	//XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	//XAhgoUser *spUser = pUserMng->GetUser( idAccount );
	//if( spUser )
	//{
	//	spUser->GetspAcc()->SetName( szName );
	//	spUser->GetspAcc()->SetszID( szID );
	//	// 클라로 답장,
	//	spUser->SendSuccessRegisterAccount( szPassword );
	//}
}

// 계정등록을 시도했으나 중복된 계정으로 등록이 실패함.
void XDBASvrConnection::RecvFailedRegisterAccount( XPacket& p )
{
//	DWORD idAccount;
//	TCHAR szName[ 64 ];
//	TCHAR szID[ 64 ];
//	TCHAR szPassword[ 64 ];
//	p >> idAccount;
//	p.ReadString( szName );
//	p.ReadString( szID );
//	p.ReadString( szPassword );
//	// 계정아이디로 유저를 찾음.
//	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
//	XAhgoUser *spUser = pUserMng->GetUser( idAccount );
//	if( spUser )
//	{
////		spUser->GetspAcc()->SetName( szName );
////		spUser->GetspAcc()->SetszID( szID );
//		// 클라로 답장,
//		spUser->SendFailedRegisterAccount( szName, szID );
//	}
}

// 게임서버쪽의 로드가 모두 끝나서 유저받을 준비가 됐다는걸 알림
void XDBASvrConnection::SendDidFinishLoad()
{
	/*XPacket ar;
	ar << (ID)xLPK_DID_FINISH_LOAD;
	WORD port = MAIN->GetpSocketForClient()->GetPort();
	ar << port;
	ar << (WORD) 0;
	ar << MAIN->GetcIPExternal();
	Send( ar );*/
	
}

// 로그인서버로부터 계정접속 정보가 옴
void XDBASvrConnection::RecvLoginAccountInfo( XPacket& p )
{
//	XSPSAcc spAcc = new XSAccount;
//	spAcc->Create();
//	if( spAcc->RestorePacket( p ) == FALSE )
//	{
//		CONSOLE( "invalid account from loginsvr: id:%s, idAcc=0x%08x", spAcc->GetszID(), spAcc->GetidAccount() );
//		SAFE_RELEASE_REF( spAcc );
//		/*
//		이렇게 불량계정 처리된 계정은 어떻게 처리해야 하는가.
//		. 2가지 처리
//		1. 새계정 생성 실패
//			- 새계정을 생성하다 실패한경우로 인식이 되면 다시 접속할때 새계정생성 화면으로 다시 유도한다.
//		2. 그냥 데이타가 깨져있는등 불량계정일때. 
//			. 로긴서버로 idAcc를 보내서 다시한번 검증하게 한다음 문제있는 계정이면 일단 블럭처리
//			. 계정을 삭제해야 하나 말아야 하나.
//			. 관리자가 최대한 빨리 알수있도록 조치.
//		*/
//		return;
//	}
//	if( XBREAK( spAcc->GetmaxCardInven() == 0 || spAcc->GetmaxItemInven() == 0 ) )
//	{
//		SAFE_RELEASE_REF( spAcc );
//	}
//// 로긴서버로 로그인신호 무한대로 보내면 뻗을수 있다. 여기에 대한 처리할것.
//#pragma message("check this")
//	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
//	if( spAcc && AHGO->AddLoginAccount( spAcc ) == FALSE )
//	{
//#ifdef _DEV
//		CONSOLE( "이미 도착한 계정정보가 와서 삭제함.: idAcc=%d", spAcc->GetidAccount() );
//#endif
//		// 이미 로그인서버에서 온 데이타이므로 삭제.
//		SAFE_RELEASE_REF( spAcc );
//	}
//	
//#ifdef _xCONTENTS_BILLING
//	SendCashBalance( spAcc );
//#endif
}
 
void XDBASvrConnection::SendPushMsgRegist(ID idacc, int type1, ID type2, int recvtime, _tstring strBuff)
{
	if (idacc == 0) return;
	XPacket ar((ID)xGDBPK_GAME_PUSH_REGIST);
	ar << idacc;
	ar << type1;
	ar << type2;
	ar << recvtime;
	ar << strBuff;
	Send(ar);
}
void XDBASvrConnection::SendPushMsgUnRegist(ID idacc, int type1, ID type2)
{
	if (idacc == 0) return;
	XPacket ar((ID)xGDBPK_GAME_PUSH_UNREGIST);
	ar << idacc;
	ar << type1;
	ar << type2;
	Send(ar);
}

/**
 @brief GCM regid를 DB에 저장.
*/
ID XDBASvrConnection::SendReqGCMRegistId( ID idAcc, const _tstring& strRegistId, const _tstring& strPlatform )
{
	XPacket ar( (ID)xGDBPK_GAME_PUSH_GCM_REGIST_ID );
	ar << idAcc;
	ar << strRegistId;
	ar << strPlatform;
	Send( ar );
	return ar.GetidKey();
}



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

#ifdef _xCONTENTS_SERVER_EVENT
//void XDBASvrConnection::SendUserLog( XSPSAcc spAcc, int Logtype, LPCTSTR szLog )
//{
//	if( spAcc == NULL )		// 유저에 관련된 Log 이므로 User 정보가 있어야 한다.
//		return;
//	XPacket ar;
//	ar << (ID)xLPK_SERVER_EVENT;
//	Send( ar );
//}
#endif //_xCONTENTS_SERVER_EVENT


// 로그인서버로부터 페이스북 계정으로 새계정이 생성되었음을 받았다.
#ifdef _xCONTENTS_FACEBOOK
void XDBASvrConnection::RecvNewAccountForFacebook( XPacket& p )
{
#ifdef _XINDONESIA
	XBREAK(1);	// 인도네시아버전에서는 이쪽으로 들어올일이 없다.
#endif
	DWORD idAccount;
	p >> idAccount;
	int typeSummon;
	p >> typeSummon;		// 유저가 선택한 소환수 타입 130320 추가
	
	TCHAR szFBUserID[80];
	TCHAR szFBUserName[80];	
	TCHAR szNickName[80];	
	p.ReadString(  szFBUserID );
	p.ReadString(  szFBUserName );	
	p.ReadString(  szNickName );	


	XSPSAcc spAcc = std::make_shared<XSAccount>( idAccount );
	spAcc->Create();
	// 빈계정이 돌아다니는 시간을 최소한으로 줄이기 위해 여기서 디폴트 계정을 만들고
	// 곧바로 로긴서버로 세이브 한다.
	spAcc->NewAccount( typeSummon );
#ifdef _XINDONESIA
//	spAcc->SetSessionID( szBuff1 );
//	spAcc->SetSessionToken( szBuff2); 
#endif
	spAcc->SetFaceBookUserID( szFBUserID);
	spAcc->SetFaceBookUserName( szFBUserName);
	spAcc->SetName( szNickName );
	SendSaveAccountInfo( spAcc, NULL );
	// spAcc는 디폴트 값으로 채워져 있어야 한다.
	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
	AHGO->AddLoginAccount( spAcc );
#ifdef _xCONTENTS_BILLING
	SendCashBalance( spAcc );
#endif
}
void XDBASvrConnection::RecvRegistAccountResult( XPacket& p )
{
	ID idaccount =0;
	p >> idaccount;
	int i0;
	XPH::xtLytoResist result;
	p >> i0;	result = (XPH::xtLytoResist) i0;;
	TCHAR FBUserID[255];
	TCHAR FBUserName[255];
	TCHAR szNickname[255];
	p.ReadString( FBUserID);
	p.ReadString(FBUserName );
	p.ReadString( szNickname );
		
	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *spUser = pUserMng->GetUser( idaccount );

	if( spUser )
	{
		spUser->SendFBRegistAccountResult( idaccount, result, FBUserID, FBUserName, szNickname);
	}
	//	// 등록성공후
//	{
//		MAKE_PCONNECT( pConnect );
//		XPacket ar;
//		ar << (ID)xCL2GS_REGIST_FACEBOOK;
//		ar << 2;	// 1:success 0:fail 2:exist
//		ar << szuserid;
//		ar << szusername;
//		pConnect->Send( ar );
//	}
}

void XDBASvrConnection::SendRegistFBAccount( ID idaccount, LPCTSTR UserName, LPCTSTR FBuserid, LPCTSTR FBusername)
{
	XPacket ar;
	ar << (ID)xLPK_REGIST_FB_ACCOUNT;
	ar << (ID)idaccount;
	ar.WriteString( UserName );
	ar.WriteString( FBuserid);
	ar.WriteString( FBusername);
	Send( ar );
}

void XDBASvrConnection::RecvLoginFBAccount( XPacket& p )
{
	ID idAcc;
	TCHAR szFbUserId[256];
	TCHAR szFbUserName[256];
	TCHAR szUserName[256];
//	TCHAR szSessionMemberID[256];
//	TCHAR szSessionToken[256];

	XPH::xtLytoResult result;
	int i0;

	p >> idAcc;
	p >> i0;	result = (XPH::xtLytoResult) i0;
	p.ReadString( szFbUserId );
	p.ReadString( szFbUserName );
	p.ReadString( szUserName );
//	p.ReadString( szSessionMemberID );
//	p.ReadString( szSessionToken );

	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *spUser = pUserMng->GetUser( idAcc );

	if( spUser )
	{
//#ifdef _XINDONESIA
//		if( spUser->GetspAcc() )
//		{
//			spUser->GetspAcc()->SetSessionID(szSessionMemberID);
//			spUser->GetspAcc()->SetSessionToken( szSessionToken);
//		}		
//#endif
		spUser->SendLoginFacebookOnUUID( idAcc, result, szFbUserId, szFbUserName, szUserName );
	}
}

// UUID로 로그인상태에서 페북로그인 시도
void XDBASvrConnection::SendLoginFBAccount( ID idAccount, LPCTSTR szUserName, LPCTSTR szFBuserid, LPCTSTR szFBusername)
{
	XPacket ar;
	ar << (ID)xLPK_LOGIN_FB_ACCOUNT;
	ar << (ID)idAccount;
	ar << szUserName;
	ar << szFBuserid;
	ar << szFBusername;
	Send( ar );
}

#endif  //_xCONTENTS_FACEBOOK

// 로그인서버로부터 UUID로 새계정이 생성되었음을 받았다.
void XDBASvrConnection::RecvNewAccountForUUID( XPacket& p )
{
//	DWORD idAccount;
//	p >> idAccount;
//	int typeSummon;
//	p >> typeSummon;		// 유저가 선택한 소환수 타입 130320 추가
//
//	XSPSAcc spAcc = std::make_shared<XSAccount>( idAccount );
//	spAcc->Create();
//	// 빈계정이 돌아다니는 시간을 최소한으로 줄이기 위해 여기서 디폴트 계정을 만들고
//	// 곧바로 로긴서버로 세이브 한다.
//	spAcc->NewAccount( typeSummon );
////#ifdef _XINDONESIA
//	TCHAR NickName[255];
//	p.ReadString( NickName );	
//	if( XE::IsHave(NickName) )
//		spAcc->SetName( NickName );
////#endif	
//	SendSaveAccountInfo( spAcc, NULL );
//	// spAcc는 디폴트 값으로 채워져 있어야 한다.
//	// 로긴서버에서 받은 계정정보를 대기열에 넣는다.
//	AHGO->AddLoginAccount( spAcc );
//#ifdef _xCONTENTS_BILLING
//	SendCashBalance( spAcc );
//#endif
}

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
	return XESocketClientInServer::Send( ar );
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

void XDBASvrConnection::FlushSendPacket()
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
		XESocketClientInServer::Send( ar );
		SAFE_DELETE( pPacketMem );
	} END_LOOP;
	m_listPacketToLogin.Clear();
	CONSOLE("패킷 방출 완료......%d bytes", bytes );
}

#ifdef _xCONTENTS_SERVER_EVENT
void XDBASvrConnection::SendRequestServerEvent()
{
	XPacket ar;
	ar << (ID)xLPK_SERVER_EVENT;
	Send( ar );
}


void XDBASvrConnection::RecvServerEvent(XPacket& p)
{
	int i = 0;
	//EventMgr 같은거 만들어서 정보를 채우고 비교하고 적용하고 하는거 만들자.
	XServerEventMng*	pSvrEvt = AHGO->GetpSvrEvt();
	if( pSvrEvt )
	{
		pSvrEvt->RestorePacket( p );		
		pSvrEvt->CheckSvrEvtRunning();
//		if( newevt )
//		{			
			XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	
			XPacket ar;
			ar << (ID)xCL2GS_SVR_NOW_EVT;
			pSvrEvt->MakePacket( ar );
			pUserMng->SendAllUser( ar );
//		}
	}
}
#endif 


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
	//XPacket ar;
	//ar << (ID)xLPK_APPLE_VERIFY;
	//ar << (ID)idAcc;	
	//ar << szSku;
	//ar << szReceipt;
	//ar << szPurchaseInfo;
	//Send( ar );
}

/**
 @brief idAcc계정의 DB에 로그아웃정보를 쓴다.
 @param idConnectNew 로그인서버의 커넥션 아이디. 이것이 0이 아니면 XDBASvrConnection::RecvAccountDuplicateProcess()으로 응답을 받고 0이면 응답을 받지 않는다.
*/
ID XDBASvrConnection::SendUpdateAccountLogout( ID idConnectNew
//																							, ID idConnectAlready
																							, ID idAcc )
{
	MAIN->m_fpsToDBA.Process(2);
	XPacket ar( (ID)xGDBPK_UPDATE_ACCOUNT_LOGOUT );
	ar << (ID)idConnectNew;
//	ar << (ID)idConnectAlready;
	ar << (ID)idAcc;
	Send( ar );
	return ar.GetidKey();
}

/**
 @brief 
 XGameSvrConnection::OnResultAccountLogOut()로부터 받음.
*/
void XDBASvrConnection::RecvAccountDuplicateProcess(XPacket& p)
{
	MAIN->m_fpsFromDBADupl.Process();
	ID idaccount = 0;
	ID newidConnect = 0;
//	ID alreadyuseridConnect = 0;

	p >> newidConnect;
//	p >> alreadyuseridConnect;
	p >> idaccount;

	XSPGameUser spUser = XGAME_USERMNG->GetspUser(idaccount);
	if (spUser) {
		// 여전히 접속되어 있다면 한번더 보내고 강제 접속해제시킴.
		spUser->SendDuplicateConnect();
		spUser->DoDestroy();
	}
	// 새 접속유저에게 재접요청을 보냄.
	LOGIN_SVR->SendAccountIsAlreadyConnect( newidConnect, idaccount );
//	LOGIN_SVR->SendAccountIsAlreadyConnect( newidConnect, alreadyuseridConnect, idaccount );
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
	XAhgoUser *spUser = pUserMng->GetUser( idAccount );
	if( spUser )
	{		
		ar << (ID) 1; //Already Connect		
		spUser->SendDisconnect( );
	}
	else
	{
		ar << (ID) 0; //ReConnect 요청.
	} 
	Send( ar );
}
#endif

#ifdef _xCONTENTS_BILLING
void XDBASvrConnection::SendCashBalance( XSPSAcc spAcc )
{
	if( spAcc == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_BALANCE;	
	ar << (ID)spAcc->GetidAccount();
#ifdef _XINDONESIA
	ar.WriteString( spAcc->GetName());
#else
	ar.WriteString( spAcc->GetFBUserID( ));
#endif
	//spAcc->MakePacket( ar );
	Send( ar );
}

void XDBASvrConnection::RecvCashBalance( XPacket& p)
{
	
	ID	idAccount;
	int CashBalance;
	p >> idAccount;
	p >> CashBalance;

	XAUserMng *pUserMng = MAIN->GetpUserMngAhgo();
	XAhgoUser *spUser = pUserMng->GetUser( idAccount );

	if( spUser )
	{
		spUser->SetCashBalance( CashBalance );
		spUser->SendCashBalance();
	} else
	{
		// 못찾았다면 클라가 게임서버에 붙기전에 게임서버가 정보를 받은것이다.
		// 로그인 대기열 계정에서 찾아본다.
		XSPSAcc pAcc = AHGO->FindLoginAccount( idAccount );
		if( XASSERT(pAcc != NULL) )
		{
			pAcc->SetCashPoint( CashBalance );
		}
	}
}

void XDBASvrConnection::SendCashBuyItem( XSPSAcc spAcc, LPCTSTR szSku, int num, int price, LPCTSTR szUserIP)
{
	if( spAcc == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_BUY_ITEM;
	ar << (ID)spAcc->GetidAccount();
	ar.WriteString( spAcc->GetName());
	ar.WriteString( szSku );
	ar << ( ID)num; // Gem 숫자
	ar << ( ID)price; // Gem 총 사용 가격	
	ar.WriteString( szUserIP );
	ar.WriteString( spAcc->GetFBUserID());
	//spAcc->MakePacket( ar );
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
	XAhgoUser *spUser = pUserMng->GetUser( idAccount );

	if( spUser )
	{
		spUser->GetspAcc()->AddGem( AddGemNum );
		//spUser->SendBuyItem( Result, AddGemNum );
		spUser->SetCashBalance( CashBalance );
		spUser->SendBuyItem( Result , AddGemNum, (BOOL)Type );
		spUser->SendCashBalance();
	}
}

void XDBASvrConnection::SendCashTopup( XSPSAcc spAcc, LPCTSTR szCashCode, LPCTSTR szUserIP)
{
	if( spAcc == NULL )		// 빌링에서는 당연히 ccount 가 있어야 한다.
		return;
	XPacket ar;
	ar << (ID)xLPK_ACCOUNT_CASH_RECHARGE;
	ar << (ID)spAcc->GetidAccount();
	ar.WriteString( spAcc->GetName());
	ar.WriteString( szCashCode );
	ar.WriteString( szUserIP);
	//spAcc->MakePacket( ar );
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
	XAhgoUser *spUser = pUserMng->GetUser( idAccount );

	if( spUser )
	{
		spUser->SendCashReCharge(Result, totalvalue );
		spUser->SetCashBalance( CashBalance );
		spUser->SendCashBalance();
	}
}

#endif //_xCONTENTS_BILLING

void XDBASvrConnection::SendLoginAccountInfoLoad( bool bSessionKeyLogin, ID idConnect, ID idAcc )
{
	MAIN->m_fpsToDBA.Process(0);
	XPacket ar( (ID)xGDBPK_LOGIN_ACCOUNT_INFO_LOAD );
	ar << idAcc;
	ar << idConnect;
	ar << (BYTE)xboolToByte( bSessionKeyLogin );
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	Send( ar );
	auto itor = MAIN->m_mapResponse.find( idAcc );
	if( itor != MAIN->m_mapResponse.end() ) {
		auto& resTime = itor->second;
		resTime.m_timer.Set( 0, TRUE );
	} else {
		XMain::xResponseTime resTime;
		resTime.m_idAcc = idAcc;
		resTime.m_timer.Set( 0, TRUE );
		MAIN->m_mapResponse[ idAcc ] = resTime;
	}
}

/**
 @brief DB서버로부터 idAcc의 full 계정정보를 받음.
*/
void XDBASvrConnection::RecvLoginAccountInfoLoad ( XPacket& p )
{
	MAIN->m_fpsFromDBALoad.Process();
	ID idAccount = 0;
	ID idConnect = 0;
	_tstring strUUID;
	char c0;
	p >> idAccount;
	p >> idConnect;
	p >> c0;  bool bSessionKeyLogin = (c0 != 0); 
	p >> c0;  xtDB retDB = (xtDB)c0;
	p >> c0 >> c0;
	XTRACE( "%s:idAcc=%d, retDB=%d", __TFUNC__, idAccount, retDB );
	{
		auto itor = MAIN->m_mapResponse.find( idAccount );
		if( itor != MAIN->m_mapResponse.end() ) {
			auto& resTime = itor->second;
			auto secPass = resTime.m_timer.GetPassSec();
			MAIN->m_dLoad += secPass;
			++MAIN->m_cntLoad;
		}
	}
//	auto spAcc = std::make_shared<XSAccount>( idAccount );
	auto spAcc = XSPSAcc( new XSAccount( idAccount ));
	if( retDB == xDB_OK ) {
		XTRACE("%s", __TFUNC__);
		if( spAcc->DeSerialize( p ) == 0 )	{
			spAcc.reset();
//			SAFE_RELEASE_REF( spAcc );
		}
	} else
	if( retDB == xDB_INVALID_ACCOUNT ) {
		spAcc->CreateDefaultAccount();
		// 디폴트 데이타 만들자 마자 곧바로 세이브
		SendSaveAccountInfo( spAcc, FALSE, 0 );
	} else
	if( XBREAK(retDB == xDB_NOT_FOUND) )	{
		// 계정을 찾지 못함.(이런경우는 없어야 함)
		return;
	}
	if( retDB == xDB_OK && spAcc )	{
//     if( bSessionKeyLogin == false )
//       spAcc->ClearBattleSession();   // 세션키로그인한게 아니면 전투세션 날림.
		auto pGame = GetGame();
		bool bOkAddLoginAccount = pGame->AddLoginAccount( idAccount, spAcc );
		/* 클라이언트가 로그인을 시도하다가 게임서버에 접속하지 않고 끊어졌을때, 
		게임서버엔 그 계정이 대기열에 올라가고 클라가 다시 접속했을때 한번더 오므로 이런현상이 생김 */
// 		XBREAKF( !bOkAddLoginAccount , "이미 도착한 계정정보가 있음: idAcc=%d", spAcc->GetidAccount() );
//     if( ! )    {
// 			// 이미 도착한정보가 있다면 새로 받은걸로 바꿔야 되는거 아닐까.
// 			XBREAK(1);
//       XTRACE( "이미 도착한 계정정보가 있음: idAcc=%d", spAcc->GetidAccount() );
//       // 이미 로그인서버에서 온 데이타이므로 삭제.
// //      SAFE_RELEASE_REF( spAcc );
//     }
		//Login에 접속 해도 된다고 알려 준다.
		const _tstring& strUUID = spAcc->GetstrUUID();
		XBREAK( strUUID.empty() );
		LOGIN_SVR->SendLoginAccountInfo( idAccount, idConnect, strUUID );
	}
}

/**
 idAccount의 부대정보를 요청해서 idClientConnect에게 전투정보를 보낸다. 
*/
ID XDBASvrConnection::SendGetUserLegionByidAccount( ID idSpot, 
													ID idAccount, 
													ID idClientConnection,
													ID idPacket )
{
	XPacket ar( (ID)xGDBPK_GET_MAIN_LEGION_BY_IDACC );
	ar << idSpot;
	ar << idAccount;
	ar << idClientConnection;
	ar << idPacket;
	Send( ar );	
	return ar.GetidKey();
}

/**
 min~max점수 사이의 유저중 한놈을 랜덤으로 골라 부대데이타를 받아온다.
*/

ID XDBASvrConnection::SendGetUserLegionByScore( int scoreMin, 
												int scoreMax, 
												int idxLegion,
												ID idFinder,
												XArchive& arRequest,
												ID idPacket )
{
	XPacket ar( (ID)xGDBPK_GET_USER_LEGION_BY_POWER );
	ar << idFinder;
	ar << scoreMin;
	ar << scoreMax;		
	ar << idxLegion;
	ar << idPacket;
	ar << arRequest;
	Send( ar );	
	return ar.GetidKey();
}

/**
 idOwner유저의 목재와철 수량을 가져와서 idConnect유저에게 idPacket의 일을 한다.
*/
void XDBASvrConnection::SendReqGetWoodIron( ID idSpot, ID idOwner, 
											ID idConnect, ID idPacket )
{
	XPacket ar( (ID)xGDBPK_GET_WOOD_IRON );
	ar << idSpot;
	ar << idOwner;
	ar << idConnect;		
	ar << idPacket;
	Send( ar );	
}

/**
 idAccount유저의 목재/철 보유량에 addWood, addIron만큼을 더한다.
*/
void XDBASvrConnection::SendAddWoodIron( ID idAccount, int addWood, int addIron )
{
	XPacket ar( (ID)xGDBPK_GET_WOOD_IRON );
	ar << idAccount;
	ar << addWood;		
	ar << addIron;
	Send( ar );	
}

void XDBASvrConnection::SendCashInfoUpdate(ID idAccount, int remaincash)
{
	XPacket ar((ID)xGDBPK_UPDATE_CASH);
	ar << idAccount;
	ar << remaincash;	
	Send(ar);
}

/**
 min~max점수 사이의 유저중 한놈을 랜덤으로 받아온다
*/
ID XDBASvrConnection::SendReqGetUserByPower( ID idFinder,
												ID idSpot, 
												int scoreMin, 
												int scoreMax, 
												xSec secAdjustOffline,
												ID idClientConnection,
												ID idPacket )
{
	XPacket ar( (ID)xGDBPK_GET_USER_BY_POWER );
	ar << idFinder;
	ar << idSpot;
	ar << scoreMin;
	ar << scoreMax;		
	ar << (DWORD) secAdjustOffline;
	ar << idClientConnection;
	ar << idPacket;
	Send( ar );	
	return ar.GetidKey();
}

/**
 보석광산 매치를 요청한다.
 idxMine: 보석광산 인덱스 번호. 1번 2개가 있다.
 scoreMin ~ scoreMax: 이 점수대의 유저를 대상으로 검색한다.
 idExclude: 이 계정아이디는 제외하고 검색을 요청한다.
*/
ID XDBASvrConnection::SendReqMatchJewelMine( int idxMine, 
																						ID idFinder,
																						int powerMin, 
																						int powerMax,
																						ID idExclude,
																						bool bSimul,
																						bool bSimulWin,
																						ID idClientConnect )
{
	XArchive arParam;
	arParam << idFinder;
	arParam << (char)idxMine;
	arParam << (char)xboolToByte(bSimul);
	arParam << (char)xboolToByte(bSimulWin);
	arParam << (char)0;
	arParam << powerMin << powerMax;
	//
	XPacket ar( (ID)xGDBPK_REQ_JEWEL_MINE );
	ar << idClientConnect;
	ar << arParam;
	Send( ar );	
	return ar.GetidKey();
}

/**
 보석광산의 군대정보를 idAcc로 얻어온다.
*/
ID XDBASvrConnection::SendGetJewelLegionByidAcc( int idxMine,
												ID idSpot, 
												ID idAccount, 
												ID idClientConnection,
												ID idPacket )
{
	XPacket ar( (ID)xGDBPK_GET_JEWEL_LEGION_BY_IDACC );
	ar << idxMine;
	ar << idAccount;
	ar << idClientConnection;
	ar << idPacket;
	Send( ar );	
	return ar.GetidKey();
}

/**
 idAccount가 갖고 있던  idxMine광산의 소유정보를 삭제시킨다.
*/
void XDBASvrConnection::SendChangeJewelOwn( ID idDBAcc, const XGAME::xJewelInfo& info )
{
	XBREAK( idDBAcc == 0 );
	XPacket ar( (ID)xGDBPK_CHANGE_JEWEL_OWN );
	ar << idDBAcc;
	XArchive arJewel;
	info.Serialize( arJewel );
	ar << arJewel;
	Send( ar );	
}

//void XDBASvrConnection::SendAddDefenseByJewel( int idxMine, ID idAcc, int def )
void XDBASvrConnection::SendAddDefenseByJewel( XSpotJewel *pSpot )
{
	XPacket ar( (ID)xGDBPK_ADD_DEFENSE_BY_JEWEL );
	XArchive arJewel;
	arJewel << pSpot->GetpProp()->idx;
	arJewel << pSpot->GetidOwner();	// oldOwner
	arJewel << pSpot->GetidOwner();	// newOwner
	arJewel << pSpot->GetstrName();	// newOwner name
	arJewel << pSpot->GetlevelMine();
	arJewel << pSpot->GetDefense();
	arJewel << (DWORD)XGame::sGet()->GettimerMain().GetsecPassTime();	// XTimer2::sGet으로 바꿔야 할듯.
	ar << arJewel;
	Send( ar );	
}

/**
 @brief idDBAcc유저의 DB에 pSpot의 광산정보를 업데이트 시킨다.
 아직은 침공시뮬에만 쓰인다.
*/
#ifdef _DEV
void XDBASvrConnection::SendUpdateJewelMineInfo( ID idDBAcc, ID idSender
																							, XSpotJewel *pSpot )
{
	XGAME::xJewelInfo info;
	pSpot->GetJewelInfo( &info );
	info.m_idMatchEnemy = idSender;		// 저쪽상대의 상대는 나.(매치상대이름과 레벨은 보낼필요없음.
	XArchive arInfo;
	info.Serialize( arInfo );
	//
	XPacket ar( (ID)xGDBPK_UPDATE_JEWEL_INFO );
	ar << idDBAcc;
	ar << arInfo;
	Send( ar );
}
#endif // _DEV


/**
 @brief pvp전투에 승리하면 상대DB에 결과를 보내저장시킨다.
 @param powerGrade 패배했을당시 방어자의 색깔등급
*/
ID XDBASvrConnection::SendReqDefeatInfo( ID idDefender, 
										ID idAttacker, 
										XGAME::xtSpot type,
										int powerGrade )
{
	XPacket ar( (ID)xGDBPK_DEFEAT_INFO );
	ar << idDefender;
	ar << idAttacker;
	ar << (BYTE)type;
	ar << (char)powerGrade;
	ar << (BYTE)0;
	ar << (BYTE)0;
	Send( ar );
	return ar.GetidKey();
}

/**
 @brief pSpot의 idOwner계정의 로그인락을 요청.
*/
ID XDBASvrConnection::SendReqLoginLockForBattle( ID idAttacker, 
																									ID idDefender, 
																									ID idSpot, 
																									ID snSpot, 
																									XGAME::xtSpot type, 
																									DWORD param1, DWORD param2 )
{
	DWORD secTotal = XGC->m_secMaxBattleSession;      //
	XPacket ar( (ID)xGDBPK_LOGIN_LOCK_FOR_BATTLE );
	ar << idAttacker;
	ar << idDefender;
	// 주고받기 좋게 파라메터들은 묶어서 보낸다.
	XArchive arPack;
	arPack << idSpot;
	arPack << snSpot;
	arPack << (DWORD)type;
	arPack << secTotal;
	arPack << param1 <<  param2;
	ar << arPack;
	Send( ar );
	return ar.GetidKey();
}

/**
 @brief idDefender의 로그인락을 풀어준다.
*/
void XDBASvrConnection::SendUnlockLoginForBattle( ID idDefender, ID idAttacker )
{
	XPacket ar( (ID)xGDBPK_LOGIN_UNLOCK_FOR_BATTLE );
	ar << idDefender;
	ar << idAttacker;
	Send( ar );
}

/**
 @brief 언락 완료
*/
void XDBASvrConnection::RecvUnlockLoginForBattle( XPacket& p )
{
	ID idDefender;
	ID idAttacker;
	p >> idDefender >> idAttacker;
	// 로그인서버로 토스하여 접속대기중인 유저에게 알림.
	LOGIN_SVR->SendUnlockLoginForBattle( idDefender );
}

/**
 @brief 길드레이드 캠페인정보 요청
*/
ID XDBASvrConnection::SendReqCampaignByGuildRaid( ID idAcc, ID idGuild, ID idCamp, ID idSpot
																								, xCampaign::CampObjPtr spCampObj )
{
	XPacket ar( (ID)xGDBPK_GUILD_REQ_CAMPAIGN_RAID );
	ar << idAcc << idGuild << idCamp << idSpot;
	if( spCampObj != nullptr ) {
		ar << 11;
		XArchive arCamp;
		xCampaign::XCampObj::sSerialize( spCampObj, arCamp );
// 		spCampObj->Serialize( arCamp );
		ar << arCamp;
	} else {
		ar << 0;
	}
	Send( ar );
	return ar.GetidKey();
}

/**
 @brief 길드레이드 스테이지에 진입요청
*/
ID XDBASvrConnection::SendReqEnterGuildRaid( ID idAcc, ID idGuild, ID idCamp, 
											ID idSpot, LPCTSTR szName, int level, int power )
{
	XPacket ar( (ID)xGDBPK_GUILD_REQ_ENTER_RAID );
	ar << idAcc << idGuild << idCamp << idSpot;
	XArchive arParam;
	arParam << szName;
	arParam << level << power;
	ar << arParam;
	Send( ar );
	return ar.GetidKey();
}

/**
 @brief 길드레이드 스테이지에서 빠져나옴
 @param arLegionSimple 부대생사여부 정보만 있는 아카이브
*/
void XDBASvrConnection::SendReqLeaveGuildRaid( ID idAcc, ID idGuild, 
												int lvAcc, ID idCamp, 
												ID idSpot, XGAME::xtWin typeWin,
												const XArchive& arLegionSimple )
{
	XPacket ar( (ID)xGDBPK_GUILD_REQ_LEAVE_RAID );
	ar << idAcc << idGuild << idCamp << idSpot;
	XBREAK( typeWin > 0xff );
	ar << (BYTE)typeWin;
	XBREAK( lvAcc > 0xff );
	ar << (BYTE)lvAcc;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << arLegionSimple;
	Send( ar );
}

/**
 @brief 길드 캠페인정보 업데이트
*/
void XDBASvrConnection::RecvGuildUpdateCampaign( XPacket& p )
{
	ID idGuild, idCamp;
	XArchive arCamp;
	p >> idGuild >> idCamp;
	p >> arCamp;
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if(	XASSERT(pGuild) ) {
		pGuild->DeSerializeUpdateGuildRaidCamp( arCamp, idCamp );
	}
}

/**
 @brief 길드레이드 오픈
*/
ID XDBASvrConnection::SendReqGuildRaidOpen( ID idAcc, ID idGuild, ID idCamp, ID idSpot,
											XArchive& arCampLegion )
{
	XPacket ar( (ID)xGDBPK_GUILD_OPEN_RAID );
	ar << idAcc << idGuild << idCamp << idSpot;
	ar << arCampLegion;
	Send( ar );
	return ar.GetidKey();
}

// xuzhu end

void XDBASvrConnection::SendShopList(XSPSAcc spAcc)
{
	if (XBREAK(spAcc == NULL)) return;
		
	XPacket ar((ID)xGDBPK_SHOP_LISTINFO_UPDATE);
	ar << spAcc->GetidAccount();
	spAcc->SerializeShopList( ar );	
	Send(ar);
}

/**
 @brief 계정의 우편함을 요청
*/
ID XDBASvrConnection::SendPostInfoRequest(XSPSAcc spAcc )
{
	if( XBREAK( spAcc == nullptr ) ) {
		// 계정을 찾지 못함.(이런경우는 없어야 함)
		return 0;
	}
	XPacket ar((ID)xGDBPK_POST_INFO);
	ar << spAcc->GetidAccount();
	Send(ar);
	return ar.GetidKey();
}

/**
 @brief 편지 한통(snPost)의 정보를 업데이트
*/
// ID XDBASvrConnection::SendPOSTInfoOneUpdate(XSPSAcc spAcc, ID Postsn, ID cbIdPacket)
// {
// 	XPostInfo* pPostInfo = spAcc->GetPostInfo(Postsn);
// 	if (pPostInfo == nullptr)
// 	{
// 		return 0;
// 	}
// 	XArchive arOut;
// 
// 	XPacket ar((ID)xGDBPK_POST_INFO_UPDATE);
// 	ar << spAcc->GetidAccount();
// 	ar << Postsn;
// 	pPostInfo->Serialize(ar);
// 	ar << cbIdPacket;
// 	Send(ar);
// 	return ar.GetidKey();	
// }
// void XDBASvrConnection::SendPOSTInfoUpdate(XSPSAcc spAcc)
// {
// 	if (XBREAK(spAcc == NULL))
// 	{
// 		// 계정을 찾지 못함.(이런경우는 없어야 함)
// 		return;
// 	}
// 	XArchive arOut;
// 	XPacket ar((ID)xGDBPK_POST_INFO_UPDATE);
// 	ar << spAcc->GetidAccount();
// 	spAcc->SerializePostUpdate(arOut);
// 	ar << arOut;
// 	Send(ar);	
// }

/**
 @brief 유저가 요청한게 아닌 우편데이타에 대해서 계정에 동기화
*/
void XDBASvrConnection::RecvPostInfo( XPacket& p )
{
	ID idAcc;
	int numPost;
	XArchive arPosts;
	p >> idAcc;
	p >> numPost;
	p >> arPosts;
	XBREAK( idAcc == 0 );
	auto spUser = XGAME_USERMNG->GetspUser( idAcc );
	if( spUser ) {
		auto spAcc = spUser->GetspAcc();
		if( spAcc ) {
			spAcc->DeSerializePostInfo( arPosts, VER_POST );
			// 유저에게 전송
			spUser->SendSyncPosts();
		}
	}
}

// void XDBASvrConnection::RecvPostInfo(XPacket& p)
// {
// 	int count = 0;
// 	ID idaccount = 0;
// 	ID PackSize = 0;
// 	p >> PackSize;
// 	p >> idaccount;
// 	XBREAK(idaccount == 0);
// 
// 	auto spUser = XGAME_USERMNG->GetspUser(idaccount);
// 	if( spUser && spUser->GetspAcc() && spUser->GetspAcc()->GetPostInfo() ) {
// 		p >> count;
// 		if( count > 0 ) {
// 			for( int i = 0; i < count; i++ ) {
// 				XPostInfo* pPost = new XPostInfo;
// 				asdfasdf 여기수정
// 				ID index = 0;
// 				int status = 0;
// 				int itemcount = 0;
// 				_tstring strBuff;
// 
// 				p >> (ID)index;
// 				if( index == 0 ) {
// 					index = spUser->GetspAcc()->GenerateSN();
// 					//pPost->SetIndex(index);
// 				}
// 				pPost->SetIndex( index );
// 				//p >> strBuff;	pPost->SetstrRecvName(strBuff.c_str());
// 				p >> strBuff;	pPost->SetstrTitle( strBuff.c_str() );
// 				p >> strBuff;	pPost->SetstrMessage( strBuff.c_str() );
// 				p >> strBuff;	pPost->SetstrSenderName( strBuff.c_str() );
// 
// 				p >> itemcount;
// 				for( int n = 0; n < itemcount; n++ ) {
// 					XPostItem* pItemData = new XPostItem;
// 					pItemData->DeSerialize( p );
// 					pPost->AddlistPostItems( pItemData );
// 				}
// 				spUser->GetspAcc()->GetPostInfo()->Add( pPost );
// 			}
// 		}
// 		spUser->SendPostInfo();
// 	}
// }

void XDBASvrConnection::SendPOSTInfoAddidaccount(ID Receiveidaccount, XPostInfo* pData)
{
	if (pData)
	{
		XPacket ar((ID)xGDBPK_POST_ADD);
		pData->Serialize(ar);
		Send(ar);
	}
	//SAFE_DELETE(pData);
}

void XDBASvrConnection::SendPOSTInfoAddusername(_tstring strRecvName, XPostInfo* pData)
{
	if (pData)
	{
		XPacket ar((ID)xGDBPK_POST_ADD);
		pData->Serialize(ar);
		Send(ar);
	}
	//SAFE_DELETE(pData);
}

// ID XDBASvrConnection::SendPOSTInfoAdd(ID idaccount
// 																		, ID postsn
// 																		, ID posttype
// 																		, XArchive& p
// 																		, ID cbidpacket)
// {
// 	XPacket ar((ID)xGDBPK_POST_ADD);
// 	ar << idaccount;
// 	ar << postsn;
// 	ar << posttype;
// 	int nCount = 0;
// 	int rewardtableid = 0;
// 
// 
// 	if ((XGAME::xtPostType)posttype == XGAME::xtPostType::xPOSTTYPE_NORMAL)
// 	{
// 		_tstring strSenderName;
// 		_tstring strRecvName;
// 		_tstring strTitle;
// 		_tstring strMessage;
// 
// 		p >> strSenderName;
// 		p >> strRecvName;
// 		p >> strTitle;
// 		p >> strMessage;
// 		p >> nCount;
// 
// 		ar << strSenderName;
// 		ar << strRecvName;
// 		ar << strTitle;
// 		ar << strMessage;
// 		ar << nCount;
// 
// 		for (int n = 0; n < nCount; n++)
// 		{
// 			XPostItem Item;
// 			Item.DeSerialize(p);
// 			Item.Serialize(ar);
// 		}		
// 	}
// 	/*else if ((XGAME::xtPostType)posttype == XGAME::xtPostType::xPOSTTYPE_VECTOR)
// 	{
// 		p >> nCount;
// 
// 		ar << nCount;
// 
// 		for (int n = 0; n < nCount; n++)
// 		{
// 			XPostItem Item;
// 			Item.DeSerialize(p);
// 			Item.Serialize(ar);
// 		}
// 
// 	}
// 	else if ((XGAME::xtPostType)posttype == XGAME::xtPostType::xPOSTTYPE_TABLE)
// 	{
// 		p >> rewardtableid;		
// 		ar << rewardtableid;
// 	}	*/
// 	ar << cbidpacket;
// 	Send(ar);
// 	return ar.GetidKey();
// }

ID XDBASvrConnection::SendPOSTInfoAdd(ID idAcc
																		, XGAME::xtPostType postType
																		, const XPostInfo& postInfo )
{
	XPacket ar((ID)xGDBPK_POST_ADD);
	ar << idAcc;
	ar << (BYTE)postType;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << postInfo;
	Send(ar);
	return ar.GetidKey();
}

/**
 @brief 메일에 첨부되어있는 아이템만 삭제시킨다.
*/
ID  XDBASvrConnection::SendRemoveItemsOnlyAtPost(ID idAcc, ID snPost)
{
	XPacket ar((ID)xGDBPK_POST_ITEMS_REMOVE);
	ar << idAcc;
	ar << snPost;
	Send(ar);
	return ar.GetidKey();
}
ID  XDBASvrConnection::SendPostDestroy(ID idaccount, ID snPost/*, ID idPacket*/)
{
	XPacket ar((ID)xGDBPK_POST_DESTROY);
	ar << idaccount;
	ar << snPost;	
//	ar << idPacket;	
	Send(ar);
	return ar.GetidKey();
}
void XDBASvrConnection::RecvRankInfo(XPacket& p)
{
	XAUTO_LOCK2( RANKING_DB );
	RANKING_DB->DeSerialize(p);
}

void XDBASvrConnection::RecvGuildInfoAll(XPacket& p)
{
	//if (GUILD_MGR)
	{
		XGuildMgr::sGet()->DeSerializeGuildAll(p);
	}
}
// void XDBASvrConnection::RecvGuildKickResult(XPacket& p)
// {
// 	ID idaccount;
// 	ID result;
// //	ID cbidpacket;
// 	
// 	p >> idaccount;
// 	p >> result;	
// 	XSPGameUser spUser = XGAME_USERMNG->GetspUser( idaccount );
// 	if( spUser ) {
// 		spUser->GetspAcc()->SetGuildIndex( 0 );
// 		spUser->GetspAcc()->SetGuildgrade( XGAME::xtGuildGrade::xGGL_NONE );
// 
// 		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_KICK_RESULT );
// 		ar << result;
// 		spUser->Send( ar );
// 	}
// }

/**
 @brief 길드에서 가입을 승인하거나 거절함.
*/
void XDBASvrConnection::RecvGuildAcceptResult(XPacket& p)
{	
	DWORD dw0;
	ID idAccConfirm;	
	ID idAccReqer;
	ID idGuild;
// 	ID guildacceptflag;
// 	ID result;

	p >> dw0;	auto flagAccept = (xtGuildAcceptFlag)dw0;
	p >> dw0;	auto errCode = (xtGuildError)dw0;
	p >> idAccConfirm;
	p >> idAccReqer;
	p >> idGuild;

	auto spUser = XGAME_USERMNG->GetspUser( idAccReqer );
	if( spUser ) {
		auto spAcc = spUser->GetspAcc();
		// 해당유저가 접속중이면 가입신청결과를 갱신
		if( flagAccept == xGAF_ACCEPT ) {
			if( spAcc->GetGuildIndex() == 0 ) {
				spAcc->SetGuildIndex( idGuild );
				spAcc->SetGuildgrade( XGAME::xtGuildGrade::xGGL_LEVEL1 );
				spAcc->ClearlistGuildJoinReq();
			}
		} else
		if( flagAccept == xGAF_REFUSE ) {
		}
		XPacket ar( (ID)xCL2GS_LOBBY_GUILD_JOIN_RESULT );
		ar << (DWORD)flagAccept;
		ar << (DWORD)errCode;
		ar << idAccConfirm;
		ar << idAccReqer;
		ar << idGuild;
		if( flagAccept == xGAF_REFUSE ) {
			spAcc->SerializeJoinReqGuild( ar );
		}
		spUser->Send( ar );
	}
}

// void XDBASvrConnection::RecvGuildInfoUpdate(XPacket& p)
// {	
// //	XBREAK(GUILD_MGR == NULL);
// 
// 	bool bBroadcast = true;
// 	ID idGuild;
// 	ID eventUpdate;
// 	_tstring context;
// 	XPacket ar((ID)xCL2GS_LOBBY_GUILD_UPDATE);
// 	p >> idGuild;
// 	p >> eventUpdate;
// 	ar << idGuild;
// 	ar << eventUpdate;
// 
// 	switch ((XGAME::xtGuildUpdate) eventUpdate)
// 	{
// 	// 새 길드가 생성됨
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_CREATE:	{
// 		XArchive arGuild;
// 		p >> arGuild;
// 		XGuild* pData = new XGuild;
// 		pData->DeSerialize( arGuild );
// 		XGuildMgr::sGet()->AddGuild( pData );
// 	} break;
// 	case xGU_UPDATE_MEMBER_LIST: {
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
// 		if( pGuild ) {
// 			XAUTO_LOCK4( pGuild );
// 			ID idAccMaster;
// 			p >> idAccMaster;
// 			if( idAccMaster ) {
// 				_tstring strMasterName;
// 				p >> strMasterName;
// 				pGuild->SetstrMasterName( strMasterName );
// 			}
// 			XArchive arParam;
// 			p >> arParam;
// 			pGuild->DeSerializeGuildMemberlist( arParam );
// 		} else {
// 			bBroadcast = false;
// 		}
// 	} break;
// 	// guildindex길드의 멤버가 업데이트 됨?
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER: {		UPDATE_MEMBER_INFO로 변경되어야 함.
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
// 		if (pGuild) {
// 			ID idActMember = 0;
// 			int gradeActMember = 0;
// 			ID idtgtMember = 0;
// 			int gradetgtMember = 0;
// 			ID MasterIdAcc = 0;
// 			_tstring strMemberName;
// 			//
// 			p >> idActMember;
// 			p >> gradeActMember;
// 			p >> idtgtMember;
// 			p >> gradetgtMember;
// 			p >> MasterIdAcc;
// 			p >> strMemberName;
// 			//
// 			if (MasterIdAcc > 0) {
// 				pGuild->SetGuildMasteridaccount(MasterIdAcc);
// 				pGuild->SetstrMasterName(strMemberName.c_str());
// 			}
// 			XSPGameUser spactUser = XGAME_USERMNG->GetspUser(idActMember);
// 			if (spactUser)	{
// 				spactUser->GetspAcc()->SetGuildgrade((XGAME::xtGuildGrade)gradeActMember);
// 			}
// 			if (gradetgtMember == XGAME::xGGL_NONE) {
// 				//추방
// 				pGuild->RemoveGuildMember(idtgtMember);				
// 			} else {
// 				XSPGameUser sptgtUser = XGAME_USERMNG->GetspUser(idtgtMember);
// 				if (sptgtUser) {
// 					sptgtUser->GetspAcc()->SetGuildgrade((XGAME::xtGuildGrade)gradetgtMember);
// 				}
// 			}			
// 			pGuild->DeSerializeGuildMemberlist(p);
// 			ar << (ID)XGAME::xtGuildUpdate::xGU_UPDATE_MEMBER;
// 			ar << idActMember;
// 			ar << gradeActMember;
// 			ar << idtgtMember;
// 			ar << gradetgtMember;
// 			ar << MasterIdAcc;
// 			ar << strMemberName;
// 
// 			pGuild->SerializeGuildMemberlist(ar);
// 		} else {
// 			bBroadcast = false;
// 		}
// 	}	break;
// 	// 가입허락으로 인한 업데이트의 갱신
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_ACCEPT: {
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
// 		if (pGuild) {
// 			XArchive arMember, arReqList;
// 			p >> arMember >> arReqList;
// 			ar << arMember << arReqList;
// // 			pGuild->DeSerializeGuildMemberlist(p);
// // 			pGuild->DeSerializeGuildReqMemberlist(p);
// // //			ar << (ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_ACCEPT;
// // 			pGuild->SerializeGuildMemberlist(ar);
// // 			pGuild->SerializeGuildReqMemberlist(ar);
// 		} else {
// 			bBroadcast = false;
// 		}
// 	} break;
// 	// 길드멤버의 업데이트
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ: {
// // 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(guildindex);
// // 		if (pGuild) {
// 			XArchive arReqerList;
// 			p >> arReqerList;
// 			ar << arReqerList;
// 			받는 클라측의 코드도 수정해야함.
// // 			pGuild->DeSerializeGuildReqMemberlist(p);
// //			ar << (ID)XGAME::xtGuildUpdate::xGU_UPDATE_JOIN_REQ;
// // 			pGuild->SerializeGuildReqMemberlist(ar);
// // 		} else {
// // 			bBroadcast = false;
// // 		}
// 	} break;
// 	// 길드 설명을 업데이트
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_CONTEXT: {
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
// 		if (pGuild) {
// 			_tstring strContext;
// 			p >> strContext;
// 			pGuild->SetstrGuildContext(strContext.c_str());
// //			ar << (ID)XGAME::xtGuildUpdate::xGU_UPDATE_CONTEXT;
// 			ar << pGuild->GetstrGuildContext();
// 		}	else {
// 			bBroadcast = false;
// 		}
// 	} break;
// 	// 길드 옵션을 업데이트
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_OPTION: {
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
// 		if (pGuild) {
// 			pGuild->DeSerializeGuildOption(p);
// //			ar << (ID)XGAME::xtGuildUpdate::xGU_UPDATE_OPTION;
// 			pGuild->SerializeGuildOption(ar);
// 		} else {
// 			bBroadcast = false;
// 		}
// 	} break;
// 	// 길드 삭제를 업데이트
// 	case XGAME::xtGuildUpdate::xGU_UPDATE_DELETE: {
// 		XGuild* pGuild = XGuildMgr::sGet()->FindGuild(idGuild);
// 		if (pGuild) {
// 			XGuildMgr::sGet()->RemoveGuild(idGuild);
// 		} else {
// 			bBroadcast = false;
// 		}
// 	}	break;
// 	// idAcc유저의 길드가입신청 목록의 갱신
// 	case xGU_UPDATE_DEL_JOIN_REQ: {
// 		ID idAcc;
// 		p >> idAcc;
// 		auto spUser = XGAME_USERMNG->GetspUser( idAcc );
// 		if( spUser ) {
// 			// XAUTO_LOCK4( spUser );		// 안해도 되나?
// 			// 해당유저가 있을경우 가입신청 목록을 갱신
// 			if( spUser->GetspAcc()  )
// 				spUser->GetspAcc()->DelGuildJoinReq( idGuild );
// 		}
// 	} break;
// 	}
// 	// 게임서버에 물려있는 모든 클라이언트에게 갱신.
// 	모든 클라이언트들에게 방송할필요는 없다. 해당 길드원에게만 방송하게 바꾸자.
// 	if (MAIN && bBroadcast) {
// 		MAIN->GetpSocketForClient()->CallFuncToConnect<XClientConnection>(&XClientConnection::SendGuildInfo
// 																																		, idGuild
// 																																		, eventUpdate
// 																																		, ar);
// 	}
// }

/**
 @brief 길드이벤트를 받아서 접속중인 길드원 전체에게 방송한다
*/
void XDBASvrConnection::RecvGuildEvent( XPacket& p )
{
	WORD w0;
	BYTE b0;
	p >> w0;		ID idGuild = w0;
	p >> b0;		auto event = (xtGuildEvent)b0;			// 어떠한 종류의 이벤트가 일어났는가.
	p >> b0;		
	bool bBroadcast = true;
	//
	XArchive arParam;
	p >> arParam;
	//
	auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	if( !pGuild && event != xGEV_CREATE )
		return;
	//
	switch( event ) {
// 	case xGEV_GUILD_INFO: {
// // 		XAccount::sDeSerializeGuildInfo( arParam, spAcc );
// 	} break;
	case xGEV_CREATE: {
		pGuild = new XGuild();
		pGuild->DeSerialize( arParam );
		XGuildMgr::sGet()->AddGuild( pGuild );
	} break;
	case xGEV_DESTROY: {
		XGuildMgr::sGet()->RemoveGuild( idGuild );
		bBroadcast = false;
	} break;
	case xGEV_JOIN_ACCEPT:
	case xGEV_JOIN_REJECT:
	case xGEV_CHANGE_MEMBER_INFO:
	case xGEV_KICK:
	case xGEV_OUT:
	case xGEV_UPDATE: {
		XGuild::sUpdateByEvent( pGuild, event, arParam );
	} break;
	default:
		XBREAK( 1 );
		break;
	}
	if( bBroadcast ) {
		for( auto pMember : pGuild->GetListMember() ) {
			auto spUser = XGAME_USERMNG->GetspUser( pMember->m_idAcc );
			if( spUser ) {
				// 접속해있는 길드원모두에게 알린다.
				spUser->SendGuildEvent( event, arParam );
			}
		}
	}
}

/**
@brief idAcc유저가 접속해있다면 해당 유저에게 길드이벤트를 방송
*/
void XDBASvrConnection::RecvGuildUserEvent( XPacket& p )
{
	ID idAcc;
	WORD w0;
	BYTE b0;
	p >> idAcc;
	p >> b0;		auto event = (xtGuildEvent)b0;			// 어떠한 종류의 이벤트가 일어났는가.
	p >> b0;
	p >> w0;		ID idGuild = w0;		// 어떤 길드에서 일어난 이벤트인가.
	auto spUser = XGAME_USERMNG->GetspUser( idAcc );
	if( !spUser )
		return;		// 없으면 아무것도 안함.
	auto spAcc = spUser->GetspAcc();
	if( !spAcc )
		return;
	// 여기선 가급적 길드정보에 대한 조작(동기화)은 하지말자.
	const auto pGuild = XGuildMgr::sGet()->FindGuild( idGuild );
	//
	XArchive arParam;
	p >> arParam;
	// 여기선 개인동기화만 하는걸로.
	XAccount::sUpdateByGuildEvent( spAcc, event, pGuild, arParam );
	// 해당유저의 클라에게 전송
	spUser->SendGuildEvent( event, arParam );
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

ID XDBASvrConnection::SendGuildJoinReq(XSPSAcc spAcc, ID guildid, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_JOIN_REQ);
	ar << spAcc->GetidAccount();
	ar << spAcc->GetstrName();
	ar << guildid;
	ar << spAcc->GetLevel();
	ar << idPacket;	
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildJoinAccept(ID idaccount, ID reqidaccount, int flag, ID guildid, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_JOIN_ACCEPT);
	ar << idaccount;
	ar << reqidaccount;
	ar << guildid;
	ar << flag;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

// void XDBASvrConnection::RecvGuildJoinAccept( XPacket& p )
// {
// 	JoinAccept에 대한 응답은 GameUser쪽에서 바로 받고
// 	가입승인에 대한 해당 유저에게 통보는 JoinResult에서 하고 있으니 이건 필요 없을것 같은데...
// 	ID idaccount;
// 	ID guildindex;
// 	ID guildacceptflag;
// 	ID idPacket;
// 
// 	p >> idaccount;
// 	p >> guildindex;
// 	p >> guildacceptflag;
// 	p >> idPacket;
// 
// 
// 	XSPGameUser spUser = XGAME_USERMNG->GetspUser( idaccount );
// 	if( spUser ) {
// 		spUser->GetspAcc()->DeSerializeJoinReqGuild( p );
// 
// 		XPacket ar( (ID)idPacket );
// 		ar << idaccount;
// 		ar << guildindex;
// 		ar << guildacceptflag;
// 		spUser->GetspAcc()->SerializeJoinReqGuild( ar );
// 		spUser->Send( ar );
// 	}
// }

ID XDBASvrConnection::SendGuildOut(ID idAcc, ID idGuild, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_OUT);
	ar << idAcc;
	ar << idGuild;	
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildKick(ID idAccKicker, ID idAccTarget, ID idGuild, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_KICK);
	ar << idAccKicker;	
	ar << idAccTarget;
	ar << idGuild;	
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildCreate(XSPSAcc spAcc
																			 , const _tstring& strGuildName
																			 , const _tstring& strGuildContext
																			 , ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_CREATE);
	ar << spAcc->GetidAccount();
	ar << spAcc->GetstrName();
	ar << strGuildName;
	ar << strGuildContext;
	ar << idPacket;	
	spAcc->SerializeJoinReqGuild(ar);
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildUpdateMember(XSPSAcc spAcc, ID idAccTarget, xtGuildGrade grade )
{
	XPacket ar((ID)xGDBPK_GUILD_UPDATE_MEMBER);
	ar << spAcc->GetGuildIndex();
	ar << spAcc->GetidAccount();
	ar << idAccTarget;
	ar << (DWORD)grade;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildUpdateContext(XSPSAcc spAcc, _tstring strguildcontext, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_UPDATE_CONTEXT);
	ar << spAcc->GetGuildIndex();
	ar << spAcc->GetidAccount();
	ar << strguildcontext;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
ID XDBASvrConnection::SendGuildUpdateOption(XSPSAcc spAcc, ID bAutoAccept, ID bBlockReq, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GUILD_UPDATE_OPTION);
	ar << spAcc->GetGuildIndex();
	ar << spAcc->GetidAccount();
	ar << bAutoAccept;
	ar << bBlockReq;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
void XDBASvrConnection::GuildUpdateContext(XPacket& p)
{

}
void XDBASvrConnection::GuildUpdateOption(XPacket& p)
{

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

ID XDBASvrConnection::SendShopGemCall(ID idaccount, int remaingem, ID idClientConnection, ID idPacket)
{
	XPacket ar((ID)xGDBPK_SHOP_GEM_CALL);
	ar << idaccount;
	ar << remaingem;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendUpdateLogin(ID idaccount, ID GameServerID, const _tstring& lastconnectip )
{
	XPacket ar((ID)xGDBPK_SAVE_ACCOUNT_LOGIN);
	ar << idaccount;
	ar << GameServerID;
	ar << lastconnectip;	
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendTradeGemCall(ID idaccount, int remaingem, ID idClientConnection, ID idPacket)
{
	XPacket ar((ID)xGDBPK_TRADE_GEM_CALL);
	ar << idaccount;
	ar << remaingem;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}

void XDBASvrConnection::RecvNotify(XPacket& p)
{
	_tstring strMessage;
	p >> strMessage;

	if (MAIN)
	{
		MAIN->GetpSocketForClient()->CallFuncToConnect<XClientConnection>(&XClientConnection::SendNotify, strMessage);
	}
//		MAIN->GetpSocketForGameSvr()->CallFuncToConnect<XGameSvrConnection>(&XGameSvrConnection::SendNotify, strMsg);

}

/**
 @brief DB서버로 방어자에게 방어정보를 전달한다.
*/
ID XDBASvrConnection::SendDefenseInfo( ID idAccount, XGAME::xBattleLog& log )
{
	XPacket ar( (ID)xGDBPK_DEFENSE_INFO_TO_DEFENDER );
	ar << idAccount;
	ar << VER_BATTLE_LOG;
	ar << log;
	Send( ar );
	return ar.GetidKey();
}


void XDBASvrConnection::SendRequestRankInfo()
{
	XPacket ar((ID)xGDBPK_RANK_INFO);	//서버 실행 했으면 랭킹 정보 알려 달라고 DB 에 요청 한다.
//	ar << (ID)XEnv::sGet()->GetGSvrID(); //
	Send(ar);
}
ID XDBASvrConnection::SendGooglePayload(ID idaccount
																			, XGAME::xtPlatform platform
																			, XGAME::xtCashType typeProduct
																			, const _tstring& idsProduct
																			, ID idClientConnection
																			, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_PAYLOAD);
	ar << idaccount;
	ar << (BYTE)platform;
	ar << (BYTE)typeProduct;
	ar << (BYTE)0;
	ar << (BYTE)0;
	ar << idsProduct;
	ar << idClientConnection;
	ar << idPacket;
	Send(ar);
	return ar.GetidKey();
}
/**
 @brief DBA에 인앱구매정보를 보내 검증한다.
*/
ID XDBASvrConnection::SendGoogleBuyCashVerify(ID idaccount
																						, const XGAME::xInApp& inapp
																						, ID idClientConnection
																						, ID idPacket)
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_GOOGLE);
	ar << idaccount;
	ar << inapp;
	ar << XEnv::sGet()->GetstrPublicKey();
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
/**
 @brief 거래에 쓰인 페이로드를 삭제하고 거래를 마무리 함.
*/
ID XDBASvrConnection::SendDeletePayload(ID idAcc
																			, const _tstring& idsProduct
																			, const _tstring& strPayload )
{
	XPacket ar((ID)xGDBPK_GAME_INAPP_DELETE_PAYLOAD);
	ar << idAcc;
	ar << idsProduct;
	ar << strPayload;
//	ar << idClientConnection;
	Send(ar);
	return ar.GetidKey();
}

ID XDBASvrConnection::SendReqGetMandrakeLegionByIdAcc( int idxMandrake
																										, ID snSpot
																										, ID idOwner
																										, ID idPacketWith )
{
	XPacket ar( (ID)xGDBPK_GET_MANDRAKE_LEGION_BY_IDACC );
	ar << idxMandrake;
	ar << snSpot;	// 오류검사용
	ar << idOwner;
	ar << idPacketWith;
	Send( ar );
	return ar.GetidKey();
}
/**
 @brief 유황기습자에게 기습정보를 보냄.
*/
ID XDBASvrConnection::SendEncounterWin( ID idPlayer
																			, ID idEncounterUser
																			, LPCTSTR szEncounter
																			, int numSulfur
																			, ID idSpot )
{
	XBREAK( idPlayer == 0 );
	XBREAK( idEncounterUser == 0 );
	XBREAK( numSulfur == 0 );
	XBREAK( idSpot == 0 );
	XBREAK( XE::IsEmpty(szEncounter) );
	//
	XPacket ar( (ID)xGDBPK_ENCOUNTER_WIN );
	ar << idPlayer;
	ar << idEncounterUser;		// encounter
	ar << numSulfur;
	ar << idSpot;
	ar << szEncounter;
	Send( ar );
	return ar.GetidKey();
}

/**
 DBA서버로 만드레이크 스팟의 소유권을 수정하도록 보낸다.
*/
void XDBASvrConnection::SendChangeMandrakeOwn( int idxMandrake, ID idAcc, ID idNewOwner, ID idEnemy, LPCTSTR szName, int win, int reward, int idxLegion, int addOffWin )
{
	XPacket ar( (ID)xGDBPK_CHANGE_MANDRAKE_OWN );
	ar << idxMandrake;
	ar << idAcc;		// DB에서 갱신될 대상.
	ar << idNewOwner;			// idNewOwner
	ar << idEnemy;			// idEnemy
	ar << szName;		// 새주인 이름
	ar << win;			// 승수
	ar << reward;			// 보상. 상대는 패했으므로 절반의 보상만 가져간다.
	ar << idxLegion;
	ar << addOffWin;
	Send( ar );
}

ID XDBASvrConnection::SendReqMatchMandrake( ID idAcc, int idxMandrake, int powerMin, int powerMax, ID snSpot )
{
	XPacket ar( (ID)xGDBPK_REQ_MATCH_MANDRAKE );
	ar << idAcc;
	ar << idxMandrake;
	ar << powerMin;
	ar << powerMax;
	ar << snSpot;	// 검증용
	Send( ar );
	return ar.GetidKey();
}

ID XDBASvrConnection::SendReqRegisterFacebook( ID idAcc
																						, const _tstring& strFbUserId
																						, const _tstring& strFbUsername )
{
	XPacket ar( (ID)xGDBPK_REGISTER_FACEBOOK );
	ar << idAcc;
	ar << strFbUserId;
	ar << strFbUsername;
	Send( ar );
	return ar.GetidKey();
}

ID XDBASvrConnection::SendReqHelloMsgByidAcc( ID idAcc, ID idSpot )
{
	XPacket ar( (ID)xGDBPK_REQ_HELLO_MSG_BY_IDACC );
	ar << idAcc;
	ar << idSpot;
	Send( ar );
	return ar.GetidKey();
}

void XDBASvrConnection::SendReload( const std::string& tag )
{
	XPacket ar((ID)xGDBPK_RELOAD);
	ar << tag;
	Send(ar);
}
