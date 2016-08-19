#include "stdafx.h"
//#include "LE2.h"
#include "XMyD3DView.h"
#include "MainFrm.h"
#include "XGraphicsD3D.h"

BEGIN_MESSAGE_MAP(XMyD3DView, CView)
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

XMyD3DView *XMyD3DView::CreateOne( CWnd *pParent, XMyD3DView *pView )
{
//	XDlgLayer *pView = new XDlgLayer;
	//CThumbView *pFormView = nullptr;
	//CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CThumbView);
	//pFormView = (CThumbView *)pRuntimeClass->CreateObject();

//	CDockableFormViewAppDoc *pDoc = CDockableFormViewAppDoc::CreateOne();
//	pFormView->m_pDocument = pDoc;

	CCreateContext *pContext = nullptr;

#if 0
	if( !pView->CreateEx(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0,0,205,157),
		pParent, -1, pContext ) )
#else
	if (!pView->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 500), pParent, 0, pContext))
#endif
	//if( !pFormView->CreateEx( 0, AfxRegisterWndClass(0, 0, 0, 0), nullptr,
	//	WS_CHILD | WS_VISIBLE, CRect( 0, 0, 205, 157), pParent, -1, pContext) )
	{
		AfxMessageBox( _T("Failed in creating CThumbView") );
	}

	pView->OnInitialUpdate();

	return pView;
}

int XMyD3DView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nResult = 0;

	CFrameWnd* pParentFrame = GetParentFrame();

	if( pParentFrame == pDesktopWnd )
	{
		// When this is docked
		nResult = CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
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

		//if (pDesktopWnd != nullptr)
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

#ifdef _DEBUG
void XMyD3DView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void XMyD3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

void XMyD3DView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

void XMyD3DView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( D3DDEVICE )
	{
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}

void XMyD3DView::OnDraw(CDC* pDC)
{
//	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� __super::OnPaint()��(��) ȣ������ ���ʽÿ�.
	if( GetChain() )
	{
		LPDIRECT3DDEVICE9 pd3dDevice = GRAPHICS_D3D->GetDevice();
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			CRect rect;
			GetClientRect( &rect );
			XE::VEC2 vRectSize = XE::VEC2( (float)rect.right - rect.left, (float)rect.bottom - rect.top );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			pd3dDevice->SetRenderTarget( 0, pBackBuffer );
			pd3dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
//			D3DRECT rectClear={0, 29, rect.right, rect.bottom};
//			pd3dDevice->Clear( 1, &rectClear, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(pd3dDevice->BeginScene()))
			{
				Draw();
				pd3dDevice->EndScene();
			}
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		} // pBackBuffer
	}

}
