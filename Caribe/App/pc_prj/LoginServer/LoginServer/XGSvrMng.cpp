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
	XBREAK( pConnect->IsDisconnected() == TRUE );		// ������ ������·� �ͼ� �ȵȴ�.
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

// �������̵�� ������ ã�´�. 
XGSvr* XGSvrMng::GetSvrFromSvrID( ID SvrID )
{
	std::map<ID, XGSvr*>::iterator itor;
	itor = m_mapSvr.find( SvrID );
	XGSvr *pSvr = NULL;
	if( itor != m_mapSvr.end() )
	{
		pSvr = (*itor).second;
		XBREAK( pSvr == NULL );		// �̷����� ����� �Ѵ�.
		return pSvr;
	}
	return NULL;
}

/**
 �Ŵ������� ������ ���� ���� �ڵ鷯�� �ҷ��ش�.
 �����ϼ������� ������ �����Ǳ� ������ �ҷ�����.
*/
void XGSvrMng::DelGSvr( XGSvr *pSvr )
{
	// �ױ��� ������ �ڵ鷯�� �����ش�.	
	pSvr->OnDestroy();
	// ���� �����Ŵ������Ե� �˸���.
	OnDestroyGSvr( pSvr );

	m_mapSvr.erase( pSvr->GetServerID() );
	// ����Ʈ������ ������
	int idxResult = m_listGSvr.Del( pSvr );		
	XBREAK( idxResult == -1 );			// ��������. ����Ȳ.
}
//
BOOL XGSvrMng::Add( XGSvr *pSvr) 
{ 
	BOOL bRet = TRUE;
	// XUserMng�� ������ ������ ����ϴ� ���� �ֳ�? �� �̷��� ����?
	do
	{
	//	m_pParentSocketSvr->EnterCS();
		XBREAK( pSvr == NULL );
		XBREAK( pSvr->GetpConnect() == NULL );
		XBREAK( pSvr->GetServerID() == 0 );
		// ���� ���̵� �̹� ������ �ȵ�
		// �ϴ� ���׽�Ʈ�� ���� uuid, idAcc�� ���Ŷ�...
		if( XBREAK( GetSvrFromSvrID( pSvr->GetServerID() ) != NULL ) )	
		{
			CONSOLE( "GameSvr �ߺ� ���� SvrID =%d", pSvr->GetServerID());
		} 
		m_mapSvr[ pSvr->GetServerID() ] = pSvr;	// ������ȣ��� �˻���  �ʿ� �߰�
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
		//����Ʈ ���� ���� ������ ���� �ʿ���.
	} END_LOOP;
//	m_pParentSocketSvr->LeaveCS();
}
// ��������� ��ü���� �ѹ��� ������Ų��.
void XGSvrMng::Flush( void ) 
{
}

// XWinSocketSvr�� �������̿��� �����忡�� �θ��� ����.
void XGSvrMng::Save( void )
{
}
/////////////////////////////////////////////////////////////////
#endif // 0