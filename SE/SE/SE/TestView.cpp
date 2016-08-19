// TestView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "TestView.h"
#include "MainFrm.h"

// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

CTestView::CTestView()
{

}

CTestView::~CTestView()
{
}

BEGIN_MESSAGE_MAP(CTestView, CView)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestView 그리기입니다.

void CTestView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			RECT rect;
			GetClientRect( &rect );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			D3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(D3DDEVICE->BeginScene()))
			{
				D3DDEVICE->EndScene();
			}
			GetChain()->Present( NULL, NULL, GetSafeHwnd(), NULL, 0 );
			pBackBuffer->Release();
		}
	}
}


// CTestView 진단입니다.

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTestView 메시지 처리기입니다.

int CTestView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return MA_ACTIVATE;
//	return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CTestView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

void CTestView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
	{
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}
