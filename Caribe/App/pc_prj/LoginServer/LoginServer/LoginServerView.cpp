
// LoginServerView.cpp : CLoginServerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "LoginServer.h"
#endif

#include "LoginServerDoc.h"
#include "LoginServerView.h"
#include "XMain.h"
#include "XFramework/server/XNetworkConnection.h"
#include "XClientConnection.h"
#include "XDBUAccount.h"
#include "XFramework/XDBMng2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLoginServerView

IMPLEMENT_DYNCREATE(CLoginServerView, CFormView)

BEGIN_MESSAGE_MAP(CLoginServerView, CFormView)
END_MESSAGE_MAP()

// CLoginServerView ����/�Ҹ�

CLoginServerView::CLoginServerView()
	: CFormView(CLoginServerView::IDD)
	, XEServerView( this )
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CLoginServerView::~CLoginServerView()
{
}

// void CLoginServerView::OnClose()
// {
// 	MAIN->OnDestroy();
// }

void CLoginServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CLoginServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CLoginServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// ���� ����. ���� ��������Ʈ ����
	m_delegateMain.Create();

}


// CLoginServerView ����

#ifdef _DEBUG
void CLoginServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLoginServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CLoginServerDoc* CLoginServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLoginServerDoc)));
	return (CLoginServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CLoginServerView �޽��� ó����
void CLoginServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN )
	{
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
		{
			str += XE::Format( _T("[%s:%d] ������ ��: %d/%d,%d/%d �ִ�ť:%d\n")
													, pSocketSvr->GetszName()
													, pSocketSvr->GetPort()
													, pSocketSvr->GetnumConnected(), pSocketSvr->GetmaxConnected()
													, pSocketSvr->GetnumLogined(), pSocketSvr->GetmaxLogined()
													, pSocketSvr->GetsizeMaxQ() );
			pSocketSvr->DrawConnections( &str );
			pSocketSvr->ClearSizeMaxQ();
		} END_LOOP;
		str += XFORMAT("ToGameSvr:%d\n", MAIN->m_fpsToGameSvr.GetFps() );
		str += XFORMAT("FromGameSvr:%d\n", MAIN->m_fpsFromGameSvr.GetFps() );
		str += XFORMAT("ToClient:%d\n", MAIN->m_fpsToClient.GetFps() );
		str += XFORMAT( "FromClient:%d\n", MAIN->m_fpsFromClient.GetFps() );
		if( XDBMng2<XClientConnection>::sGet()->GetbBusy() )
			str += XFORMAT( "reqQ:%d(busy)\n", XDBMng2<XClientConnection>::sGet()->GetsizeReqQ() );
		else
			str += XFORMAT("reqQ:%d\n", XDBMng2<XClientConnection>::sGet()->GetsizeReqQ() );
		str += XFORMAT("RstQ:%d\n", XDBMng2<XClientConnection>::sGet()->GetsizeResultQ() );
		str += XFORMAT("client used pool:%d/%d\n", XClientConnection::s_pPool->GetnNum(), XClientConnection::s_pPool->GetnMax() );
		str += XFORMAT("DBUAcc used pool:%d/%d\n", XDBUAccount::s_pPool->GetnNum(), XDBUAccount::s_pPool->GetnMax() );
/*
		LPCTSTR szText = XE::Format( _T("fps:%d ������ ��: %d �ִ�ť:%.1f%%"), 
									MAIN->m_FPS, 
									m_numConnect, 
									XNetworkConnection::sGetmaxQueueRatio());
*/
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

