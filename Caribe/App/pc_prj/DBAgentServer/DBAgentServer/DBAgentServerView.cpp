
// DBAgentServerView.cpp : CDBAgentServerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CDBAgentServerView 생성/소멸

CDBAgentServerView::CDBAgentServerView()
	: CFormView(CDBAgentServerView::IDD)
	, XEServerView( this )
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CDBAgentServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// 최초 시작. 메인 델리게이트 생성
	m_delegateMain.Create();

}


// CDBAgentServerView 진단

#ifdef _DEBUG
void CDBAgentServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDBAgentServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDBAgentServerDoc* CDBAgentServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBAgentServerDoc)));
	return (CDBAgentServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CDBAgentServerView 메시지 처리기
void CDBAgentServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN ) {
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
		{
			str += XE::Format( _T("[%s:%d] 접속자 수: %d/%d 최대큐:%d\n"), 
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
			pWnd->SetWindowText( _T("구글 마켓 공개키가 설정되지 않았습니다.") );
		else
			if( MAIN->GetstrPublicKey().length() < 300 )
			{
				LPCTSTR szText = XE::Format( _T("공개키의 포맷에 문제가 있습니다.\nkey=%s"), 
					C2SZ(MAIN->GetstrPublicKey().c_str()) );
				pWnd->SetWindowText( szText );
			}
	}
*/
}