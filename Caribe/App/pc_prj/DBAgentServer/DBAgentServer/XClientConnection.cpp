#include "stdafx.h"
#include "XClientConnection.h"
#include "XCGPacket.h"
#include "XGameUser.h"
#include "XMain.h"
//#include "XDBMng.h"
#include "XGame.h"
#include "XGameUserMng.h"
#include "XSocketForClient.h"

//#include "XSAccount.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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
	: XEWinConnectionInServer( MAIN->GetpSocketForClient(), socket, szIP, 0xffff )		
{
	Init();
	XTRACE( "create client connection: idConnect=0x%08x", m_idConnect );
}

// 클라이언트로부터 패킷을 받음.
BOOL XClientConnection::ProcessPacket( DWORD idPacket, XPacket& p )
{	
	XGameUser *pUser = SafeCast<XGameUser*, XEUser*>( GetpUser() );
	
	TRACE( "recv packet: %d", idPacket );

	switch( idPacket )
	{
	//case xPK_SAMPLE:		pUser->RecvSample( p );	break;		// 
	default:
		// 알수없는 패킷이 날아왔다.
		XBREAKF( 1, "unknown packet 0x%8x", idPacket );
		{
			DoDisconnect();
			return FALSE;
		}
		break;
	}
	TRACE( "recv packet: end" );
	return TRUE;	
}

void XClientConnection::OnError( xtError error, DWORD param1, DWORD param2 )
{
	if( GetpUser() )
	{
		XGameUser *pUser = SafeCast<XGameUser*, XEUser*>( GetpUser() );
		if( error == XENetworkConnection::xERR_PACKET_SIZE_TO_LARGE )
		{
			CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.",  GetszIP(), param1 );
		}
	} else
	{
		CONSOLE( "IP address=%s: 너무 큰 패킷(%d byte)이 날아와 접속을 끊었습니다.", GetszIP(), param1 );
	}
}


