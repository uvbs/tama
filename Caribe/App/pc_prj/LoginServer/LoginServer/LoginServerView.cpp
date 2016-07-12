
// LoginServerView.cpp : CLoginServerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CLoginServerView 생성/소멸

CLoginServerView::CLoginServerView()
	: CFormView(CLoginServerView::IDD)
	, XEServerView( this )
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CLoginServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// 최초 시작. 메인 델리게이트 생성
	m_delegateMain.Create();

}


// CLoginServerView 진단

#ifdef _DEBUG
void CLoginServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLoginServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CLoginServerDoc* CLoginServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLoginServerDoc)));
	return (CLoginServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CLoginServerView 메시지 처리기
void CLoginServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN )
	{
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
		{
			str += XE::Format( _T("[%s:%d] 접속자 수: %d/%d,%d/%d 최대큐:%d\n")
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
		LPCTSTR szText = XE::Format( _T("fps:%d 접속자 수: %d 최대큐:%.1f%%"), 
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

