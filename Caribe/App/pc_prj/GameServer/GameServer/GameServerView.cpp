
// GameServerView.cpp : CGameServerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CGameServerView 생성/소멸

CGameServerView::CGameServerView()
	: CFormView(CGameServerView::IDD)
	, XEServerView( this )
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CGameServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// 최초 시작. 메인 델리게이트 생성
	m_delegateMain.Create();

}


// CGameServerView 진단

#ifdef _DEBUG
void CGameServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGameServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGameServerDoc* CGameServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
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

// CGameServerView 메시지 처리기
void CGameServerView::OnDraw(CDC* pDC)
{
//	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
	CWnd *pWnd = XEServerView::sGetView()->GetDlgItem( IDC_TEXT_NUM_CONNECT );
	if( pWnd && MAIN ) {
		_tstring str = XE::Format(_T("fps::%d\n"), MAIN->m_FPS );
		// 클라이언트 들이 접속하는 소켓서버
		XARRAYLINEAR_LOOP( MAIN->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr ) {
			str += XE::Format( _T("[%s:%d] 접속자 수: %d/%d, %d/%d 최대큐:%d\n")
												, pSocketSvr->GetszName()
												, pSocketSvr->GetPort()
												, pSocketSvr->GetnumConnected(), pSocketSvr->GetmaxConnected()
												, pSocketSvr->GetnumLogined(), pSocketSvr->GetmaxLogined()
												, pSocketSvr->GetsizeMaxQ() );
			pSocketSvr->ClearSizeMaxQ();
		} END_LOOP;
		// DBA에 연결된 커넥션
		str += XE::Format( _T( "[XDBAConnection:%d] 패킷큐크기:%d bytes\n" ),
								// 			XDBASvrConnection::sGet()->->GetszName(),
											XDBASvrConnection::sGet()->GetPort(),
											XDBASvrConnection::sGet()->GetsizeMaxQ() );
		XDBASvrConnection::sGet()->ClearSizeMaxQ();
		// 로긴서버에 연결된 커넥션
		str += XE::Format( _T( "[XLoginConnection:%d] 패킷큐크기:%d bytes\n" ),
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