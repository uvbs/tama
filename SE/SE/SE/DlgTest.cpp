// DlgTest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgTest.h"
#include "MainFrm.h"
#include "XGraphics.h"
// CDlgTest

IMPLEMENT_DYNCREATE(CDlgTest, CFormView)

CDlgTest::CDlgTest()
	: CFormView(CDlgTest::IDD)
{

}

CDlgTest::~CDlgTest()
{
}

void CDlgTest::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgTest, CFormView)
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CDlgTest 진단입니다.

#ifdef _DEBUG
void CDlgTest::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgTest::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgTest 메시지 처리기입니다.

void CDlgTest::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	GetParentFrame()->RecalcLayout();		// 이거 살린채로 스프릿윈도우에 넣으면 무한루프돔
//	SetScrollSizes( MM_TEXT, CSize( 1000, 1000 ) );

//	ResizeParentToFit(FALSE);

//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

// static
CDlgTest *CDlgTest::CreateOne( CWnd *pParent )
{
	CDlgTest *pFormView = new CDlgTest;
	//CDlgTest *pFormView = NULL;
	//CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CDlgTest);
	//pFormView = (CDlgTest *)pRuntimeClass->CreateObject();

//	CDockableFormViewAppDoc *pDoc = CDockableFormViewAppDoc::CreateOne();
//	pFormView->m_pDocument = pDoc;

	CCreateContext *pContext = NULL;

#if 0
	if( !pFormView->CreateEx(0, NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,205,157),
		pParent, -1, pContext ) )
#else
	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 500), pParent, 0, pContext))
#endif
	//if( !pFormView->CreateEx( 0, AfxRegisterWndClass(0, 0, 0, 0), NULL,
	//	WS_CHILD | WS_VISIBLE, CRect( 0, 0, 205, 157), pParent, -1, pContext) )
	{
		AfxMessageBox( _T("Failed in creating CDlgTest") );
	}

	pFormView->OnInitialUpdate();

	return pFormView;
}

void CDlgTest::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			D3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(D3DDEVICE->BeginScene()))
			{
				GRAPHICS->DrawRect( 0, 0, 50.f, 50.f, XCOLOR_RED );
				D3DDEVICE->EndScene();
			}
			GetChain()->Present( NULL, NULL, GetSafeHwnd(), NULL, 0 );
			pBackBuffer->Release();
		}
	}
}

void CDlgTest::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
	{
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}


int CDlgTest::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// TODO: Add your message handler code here and/or call default

	int nResult = 0;

	CFrameWnd* pParentFrame = GetParentFrame();

	if( pParentFrame == pDesktopWnd )
	{
		// When this is docked
		nResult = CFormView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	else
	{
		// When this is not docked

		BOOL isMiniFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMiniFrameWnd ) );
		BOOL isPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CPaneFrameWnd ) );
		BOOL isMultiPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMultiPaneFrameWnd ) );

		// pDesktopWnd is the frame window for CDockablePane

		nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );

		//nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
		//if( nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT )
		//	return nResult;

		//if (pDesktopWnd != NULL)
		//{
		//	// either re-activate the current view, or set this view to be active
		//	//CView* pView = pDesktopWnd->GetActiveView();
		//	//HWND hWndFocus = ::GetFocus();
		//	//if (pView == this &&
		//	//	m_hWnd != hWndFocus && !::IsChild(m_hWnd, hWndFocus))
		//	//{
		//	//	// re-activate this view
		//	//	OnActivateView(TRUE, this, this);
		//	//}
		//	//else
		//	//{
		//	//	// activate this view
		//	//	pDesktopWnd->SetActiveView(this);
		//	//}
		//}
	}

	return nResult;

//	return __super::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
