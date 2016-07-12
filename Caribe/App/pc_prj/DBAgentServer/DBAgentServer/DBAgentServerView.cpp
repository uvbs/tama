
// DBAgentServerView.cpp : CDBAgentServerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "DBAgentServer.h"
#endif

#include "DBAgentServerDoc.h"
#include "DBAgentServerView.h"
#include "xmain.h"
#include "XDBUAccount.h"
#include "XFramework/XDBMng2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDBAgentServerView

IMPLEMENT_DYNCREATE(CDBAgentServerView, CFormView)

BEGIN_MESSAGE_MAP(CDBAgentServerView, CFormView)
END_MESSAGE_MAP()

// CDBAgentServerView ����/�Ҹ�

CDBAgentServerView::CDBAgentServerView()
	: CFormView(CDBAgentServerView::IDD)
	, XEServerView( this )
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CDBAgentServerView::~CDBAgentServerView()
{
}

void CDBAgentServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CDBAgentServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CDBAgentServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// ���� ����. ���� ��������Ʈ ����
	m_delegateMain.Create();

}


// CDBAgentServerView ����

#ifdef _DEBUG
void CDBAgentServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDBAgentServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDBAgentServerDoc* CDBAgentServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBAgentServerDoc)));
	return (CDBAgentServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CDBAgentServerView �޽��� ó����
void CDBAgentServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN ) {
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
		{
			str += XE::Format( _T("[%s:%d] ������ ��: %d/%d �ִ�ť:%d\n"), 
													pSocketSvr->GetszName(),
													pSocketSvr->GetPort(),
													pSocketSvr->GetnumConnected(), pSocketSvr->GetnumLogined(),
													pSocketSvr->GetsizeMaxQ() );
			pSocketSvr->ClearSizeMaxQ();
			pSocketSvr->DrawConnections( &str );
		} END_LOOP;
		str += XFORMAT( "FromGameSvr:%d\n", MAIN->m_fpsFromGameSvr.GetFps() );
		str += XFORMAT( "ToGameSvr:%d\n", MAIN->m_fpsToGameSvr.GetFps() );
		str += XFORMAT("reqQ:%d\n", XDBMng2<XGameSvrConnection>::sGet()->GetsizeReqQ() );
		str += XFORMAT("RstQ:%d\n", XDBMng2<XGameSvrConnection>::sGet()->GetsizeResultQ() );
		str += XFORMAT("DBUAcc used pool:%d/%d\n", XDBUAccount::s_pPool->GetnNum(), XDBUAccount::s_pPool->GetnMax() );
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