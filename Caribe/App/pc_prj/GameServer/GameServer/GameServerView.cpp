
// GameServerView.cpp : CGameServerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "GameServer.h"
#endif

#include "GameServerDoc.h"
#include "GameServerView.h"
#include "XGame.h"
#include "XMain.h"
#include "XDBASvrConnection.h"
#include "XLoginConnection.h"
#include "XClientConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGameServerView

IMPLEMENT_DYNCREATE(CGameServerView, CFormView)

BEGIN_MESSAGE_MAP(CGameServerView, CFormView)
END_MESSAGE_MAP()

// CGameServerView ����/�Ҹ�

CGameServerView::CGameServerView()
	: CFormView(CGameServerView::IDD)
	, XEServerView( this )
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CGameServerView::~CGameServerView()
{
}

void CGameServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGameServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CGameServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// ���� ����. ���� ��������Ʈ ����
	m_delegateMain.Create();

}


// CGameServerView ����

#ifdef _DEBUG
void CGameServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGameServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGameServerDoc* CGameServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGameServerDoc)));
	return (CGameServerDoc*)m_pDocument;
}
#endif //_DEBUG


void CGameServerView::OnDrawBefore( _tstring& str )
{
	float sec = XGame::sGet()->GettimerMain().GetsecPassTime();

	str += XE::Format(_T("Time:%d"), (int)ROUND_UP(sec) );

}

// CGameServerView �޽��� ó����
void CGameServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN ) {
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		// Ŭ���̾�Ʈ ���� �����ϴ� ���ϼ���
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr ) {
			str += XE::Format( _T("[%s:%d] ������ ��: %d/%d, %d/%d �ִ�ť:%d\n")
												, pSocketSvr->GetszName()
												, pSocketSvr->GetPort()
												, pSocketSvr->GetnumConnected(), pSocketSvr->GetmaxConnected()
												, pSocketSvr->GetnumLogined(), pSocketSvr->GetmaxLogined()
												, pSocketSvr->GetsizeMaxQ() );
			pSocketSvr->ClearSizeMaxQ();
		} END_LOOP;
		// DBA�� ����� Ŀ�ؼ�
		str += XE::Format( _T( "[XDBAConnection:%d] ��Ŷťũ��:%d bytes\n" ),
								// 			XDBASvrConnection::sGet()->->GetszName(),
											XDBASvrConnection::sGet()->GetPort(),
											XDBASvrConnection::sGet()->GetsizeMaxQ() );
		XDBASvrConnection::sGet()->ClearSizeMaxQ();
		// �α伭���� ����� Ŀ�ؼ�
		str += XE::Format( _T( "[XLoginConnection:%d] ��Ŷťũ��:%d bytes\n" ),
								// 			XDBASvrConnection::sGet()->->GetszName(),
												LOGIN_SVR->GetPort(),
												LOGIN_SVR->GetsizeMaxQ() );
		LOGIN_SVR->ClearSizeMaxQ();
		//
		str += XFORMAT( "FromLogin:%d\n", MAIN->m_fpsFromLogin.GetFps() );
		str += XFORMAT( "ToLogin:%d\n", MAIN->m_fpsToLogin.GetFps() );
		str += XFORMAT( "FromDBASave:%d\n", MAIN->m_fpsFromDBASave.GetFps() );
		str += XFORMAT( "FromDBADupl:%d\n", MAIN->m_fpsFromDBADupl.GetFps() );
		str += XFORMAT( "FromDBALoad:%d\n", MAIN->m_fpsFromDBALoad.GetFps() );
		str += XFORMAT( "ToDBA(L,S,O):%d/%d/%d\n", MAIN->m_fpsToDBA.GetFps(0), MAIN->m_fpsToDBA.GetFps(1), MAIN->m_fpsToDBA.GetFps(2) );
		str += XFORMAT( "FromClient:%d\n", MAIN->m_fpsFromClient.GetFps() );
		str += XFORMAT( "ToClientOk:%d\n", MAIN->m_fpsToClientOk.GetFps() );
		str += XFORMAT( "ToClientRetry:%d\n", MAIN->m_fpsToClientRetry.GetFps() );
		float avgLoad = (float)(MAIN->m_dLoad / MAIN->m_cntLoad);
		str += XFORMAT( "avgLoad:%.1f\n", avgLoad );
		str += XFORMAT("clientConnect used pool:%d/%d\n", XClientConnection::s_pPool->GetnNum(), XClientConnection::s_pPool->GetnMax() );
		str += XFORMAT("XSAcc used pool:%d/%d\n", XSAccount::s_pPool->GetnNum(), XSAccount::s_pPool->GetnMax() );
		str += XFORMAT("GameUser used pool:%d/%d\n", XGameUser::s_pPool->GetnNum(), XGameUser::s_pPool->GetnMax() );

		pWnd->SetWindowText( str.c_str() );
	}
/*
	CWnd *pWnd = GetDlgItem( IDC_ERR_TEXT );
	if( pWnd && MAIN )
	{
		if( MAIN->GetstrPublicKey().empty() )
			pWnd->SetWindowText( _T("���� ���� ����Ű�� �������� �ʾҽ��ϴ�.") );
		else
			if( MAIN->GetstrPublicKey().length() < 300 )
			{
				LPCTSTR szText = XE::Format( _T("����Ű�� ���˿� ������ �ֽ��ϴ�.\nkey=%s"), 
					C2SZ(MAIN->GetstrPublicKey().c_str()) );
				pWnd->SetWindowText( szText );
			}
	}
*/
}