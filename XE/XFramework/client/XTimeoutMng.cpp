#include "stdafx.h"
#include "XTimeoutMng.h"
#include "_Wnd2/XWnd.h"
#include "XApp.h"
#include "XClientMain.h"
#include "XEContent.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 장차 사라질 변수-이거 대신 sGet을 쓰기 바람.
//XTimeoutMng *TIMEOUT_MNG = NULL;

//////////////////////////////////////////////////////////////////////////
// static
XTimeoutMng* XTimeoutMng::s_pSingleton = NULL;
XAutoPtr<XTimeoutMng*> XTimeoutMng::s_autoDestroy;

XTimeoutMng* XTimeoutMng::sGet( void )
{
	if( s_pSingleton == NULL ) {
		s_pSingleton = new XTimeoutMng;
		s_autoDestroy.Setptr( s_pSingleton );
	}
	return s_pSingleton;
}

//////////////////////////////////////////////////////////////////////////
/**
 bExclusive:독점모드. TRUE가 되면 일정시간동안 응답이 없을때부터 화면 입력이 금지되며 어둡게 변한다.
*/
void XTimeoutMng::Add( ID idWnd, DWORD idPacket, float secTimeout, BOOL bExclusive )
{
	xREQUEST req;
	XBREAK( idWnd == 0 );
	req.idWnd = idWnd;
	req.idPacket = idPacket;
	req.idSendPacket = idPacket;
	req.timerTimout.Set( secTimeout );
	req.bBlocking = bExclusive;
	m_listTimeout.push_back( req );
	m_timerRequest.Set( 3.f );		// 3초후부터 메시지 출력.
}

void XTimeoutMng::Add( ID idWnd, DWORD idPacket, ID idSubPacket, BOOL bBlocking, float secTimeout )
{
	xREQUEST req;
	XBREAK( idWnd == 0 );
	req.idWnd = idWnd;
	req.idPacket = idPacket;
	req.idSendPacket = idPacket;
	req.idSubPacket = idSubPacket;
	req.bBlocking = bBlocking;
	req.timerTimout.Set( secTimeout );
	m_listTimeout.push_back( req );
	m_timerRequest.Set( 3.f );		// 3초후부터 메시지 출력.
}

/**
 하나의 공통된 키값으로 여러개의 응답패킷을 기다리는 경우 사용
 ex: 
 send( 접속요청 패킷A )
 AddByKey( uniqueKey, 계정없음패킷B );
 AddByKey( uniqueKey, 새계정패킷C );
 AddByKey( uniqueKey, 접속성공패킷C );

*/
void XTimeoutMng::AddByKey( ID idWnd, ID idKey, ID idPacket, BOOL bBlocking, float secTimeout )
{
	XBREAK( idWnd == 0 );
	XBREAK( idPacket > 0xffff );
	xREQUEST *pReq = FindByKey( idKey );
	if( pReq )
	{
		// 이미 같은키로 등록된게 있으면 어레이에 추가만 한다.
		XBREAK( pReq->idWnd != idWnd );
		XBREAK( pReq->bBlocking != bBlocking );
		pReq->aryPacketID.Add( idPacket );
	} else
	{
		xREQUEST req;
		req.idWnd = idWnd;
		req.idPacket = idKey;	// idPacket이 아니고 idKey라는것에 주의
		req.idSendPacket = idPacket;
		req.bBlocking = bBlocking;
		req.timerTimout.Set( secTimeout );
		req.aryPacketID.Add( idPacket );	// 패킷 어레이에 패킷아이디를 등록
		m_listTimeout.push_back( req );
		m_timerRequest.Set( 3.f );	// 3초후부터 메시지 출력.
	}
	
}

// 요청했던 패킷이 도착해서 타임아웃 리스트에서 지움
void XTimeoutMng::Arrive( DWORD idPacket, ID idSubPacket )
{
	m_timerRequest.Off();
	LIST_MANUAL_LOOP( m_listTimeout, xREQUEST, itor, req )
	{
		if( req.idPacket == idPacket && 
			(idSubPacket == 0 || req.idSubPacket == idSubPacket) )
		{
			m_listTimeout.erase( itor++ );
		} else
			++itor;
	} END_LOOP;
}


BOOL XTimeoutMng::IsHavePacketID( xREQUEST& req, ID idCompare )
{
	XARRAYLINEARN_LOOP( req.aryPacketID, ID, idPacket )
	{
		if( idPacket == idCompare )
			return TRUE;
	} END_LOOP;
	return FALSE;
}
/**
 .패킷 아이디를 받으면 우선 그 아이디로 검색을 해본다.
 .없으면 그 패킷아이디를 어레이에 가지고 있는 객체가 있는지 검사해본다.
 .있으면 찾아 없앤다.
*/
void XTimeoutMng::ArriveWithKey( DWORD idPacket )
{
	m_timerRequest.Off();
	BOOL bFound = FALSE;
	LIST_MANUAL_LOOP( m_listTimeout, xREQUEST, itor, req )
	{
		if( req.idPacket == idPacket )
		{
			m_listTimeout.erase( itor++ );
			bFound = TRUE;
		} else
			++itor;
	} END_LOOP;
	if( bFound )
		return;
	// idPacket으로 못찾았으면 idPacket을 어레이로 갖고 있는 객체가 있는지 찾는다.
	LIST_MANUAL_LOOP( m_listTimeout, xREQUEST, itor, req )
	{
		if( IsHavePacketID( req, idPacket ) )
		{
			m_listTimeout.erase( itor++ );
			bFound = TRUE;
		} else
			++itor;
	} END_LOOP;


}

void XTimeoutMng::Process( float dt )
{
	if( m_bClear )
	{
		m_listTimeout.clear();
		m_bClear = FALSE;
		m_timerRequest.Off();
	}
	LIST_MANUAL_LOOP( m_listTimeout, xREQUEST, itor, req )
	{
		if( req.timerTimout.IsOver() )
		{
//#ifndef _DEBUG
			// 타임아웃 시간이 지났다.
//			XWnd *pWnd = m_pWndTop->Find( req.idWnd );
			if( XE::GetMain()->GetpGame() )
			{
				XWnd *pWnd = XE::GetMain()->GetpGame()->Find( req.idWnd );
				if( pWnd )
				{
					// 콜백을 돌려줄 윈도우를 찾아보고 아직도 있으면 콜백을 날려줌.
					XTimeoutDelegate *pDelegate = dynamic_cast<XTimeoutDelegate*>( pWnd );
					if( pDelegate )
					{
//						pDelegate->OnTimeout( req.idSendPacket );
						pDelegate->OnTimeout2( req.idSendPacket, req.idSubPacket );
					}
				}
			}
//#endif // not debug
			m_listTimeout.erase( itor++ );
		} else
			++itor;
	} END_LOOP;
}

BOOL XTimeoutMng::Find( ID idPacket )
{
	LIST_LOOP( m_listTimeout, xREQUEST, itor, req )
	{
		if( req.idPacket == idPacket )
			return TRUE;
	} END_LOOP;
	return FALSE;
}

XTimeoutMng::xREQUEST* XTimeoutMng::FindByKey( ID idKey )
{
	LIST_LOOP( m_listTimeout, xREQUEST, itor, req )	{
		if( req.idPacket == idKey )
			return &req;
	} END_LOOP;
	return NULL;
}

BOOL XTimeoutMng::IsRequesting( void ) 
{
	if( m_listTimeout.size() == 0 )
		return FALSE;
	LIST_LOOP( m_listTimeout, xREQUEST, itor, req )	{
		// 블로킹 되는 타임아웃 객체가 하나라도 있으면 "응답중"으로 리턴
		if( req.bBlocking )
			return TRUE;
	} END_LOOP;
	return FALSE;
}

/**
 @brief 현재 응답을 기다리고 있는 패킷번호를 돌려준다.
*/
XTimeoutMng::xREQUEST* XTimeoutMng::GetRequesting()
{
	for( auto& req : m_listTimeout )	{
		if( req.bBlocking )		{
			return &req;
		}
	}
	return nullptr;
}

