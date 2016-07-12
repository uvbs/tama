#include "StdAfx.h"
#if 0
#include "XGSvrMng.h"
#include "XFramework/server/XWinConnection.h"
//#include "XServerMain.h"
//#include "XWinSocketSvr.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGSvr::XGSvr( XGSvrMng *pSvrMng, XEWinConnectionInServer *pConnect ) 
{ 
	Init(); 
	XBREAK( pConnect->IsDisconnected() == TRUE );		// 소켓이 끊긴상태로 와선 안된다.
	m_pConnect = pConnect;
	m_pSvrMng = pSvrMng;
	
}

void XGSvr::DoDisconnect( void )
{
	m_pConnect->DoDisconnect();
}



//////////////////////////////////////////////////////////////////////
XGSvrMng::XGSvrMng( XEWinSocketSvr *pParent, int maxSvr)
{
	Init();
	m_pParentSocketSvr = pParent;
	XBREAK( m_pParentSocketSvr == NULL );
	m_listGSvr.Create( maxSvr );
}

// 계정아이디로 유저를 찾는다. 
XGSvr* XGSvrMng::GetSvrFromSvrID( ID SvrID )
{
	std::map<ID, XGSvr*>::iterator itor;
	itor = m_mapSvr.find( SvrID );
	XGSvr *pSvr = NULL;
	if( itor != m_mapSvr.end() )
	{
		pSvr = (*itor).second;
		XBREAK( pSvr == NULL );		// 이런경우는 없어야 한다.
		return pSvr;
	}
	return NULL;
}

/**
 매니저에서 유저를 빼고 각종 핸들러를 불러준다.
 윈소켓서버에서 유저가 삭제되기 직전에 불려진다.
*/
void XGSvrMng::DelGSvr( XGSvr *pSvr )
{
	// 죽기전 마지막 핸들러를 보내준다.	
	pSvr->OnDestroy();
	// 하위 유저매니저에게도 알린다.
	OnDestroyGSvr( pSvr );

	m_mapSvr.erase( pSvr->GetServerID() );
	// 리스트에서도 빼버림
	int idxResult = m_listGSvr.Del( pSvr );		
	XBREAK( idxResult == -1 );			// 삭제실패. 비상상황.
}
//
BOOL XGSvrMng::Add( XGSvr *pSvr) 
{ 
	BOOL bRet = TRUE;
	// XUserMng를 스레드 내에서 사용하는 곳이 있나? 왜 이렇게 했지?
	do
	{
	//	m_pParentSocketSvr->EnterCS();
		XBREAK( pSvr == NULL );
		XBREAK( pSvr->GetpConnect() == NULL );
		XBREAK( pSvr->GetServerID() == 0 );
		// 같은 아이디가 이미 있으면 안됨
		// 일단 봇테스트엔 같은 uuid, idAcc를 쓸거라서...
		if( XBREAK( GetSvrFromSvrID( pSvr->GetServerID() ) != NULL ) )	
		{
			CONSOLE( "GameSvr 중복 접속 SvrID =%d", pSvr->GetServerID());
		} 
		m_mapSvr[ pSvr->GetServerID() ] = pSvr;	// 계정번호기반 검색용  맵에 추가
		m_listGSvr.Add( pSvr );
	//	m_listAddReserve.Add( pUser );
	//	m_pParentSocketSvr->LeaveCS();
	} while(0);
	return bRet;
}

//
void XGSvrMng::Process( float dt ) 
{
//	m_pParentSocketSvr->EnterCS();
	if( m_listGSvr.size() <= 0 )
		return;
	//
	XLIST2_LOOP( m_listGSvr, XGSvr*, pSvr)
	{
		pSvr->Process( dt );
		//게이트 웨이 서버 역할을 위해 필요함.
	} END_LOOP;
//	m_pParentSocketSvr->LeaveCS();
}
// 삭제예약된 객체들을 한번에 삭제시킨다.
void XGSvrMng::Flush( void ) 
{
}

// XWinSocketSvr의 스레드이외의 스레드에서 부르지 말것.
void XGSvrMng::Save( void )
{
}
/////////////////////////////////////////////////////////////////
#endif // 0