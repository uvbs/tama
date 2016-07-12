// WndLayer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "QLE2.h"
#include "MainFrm.h"
#include "XGraphicsD3D.h"

IMPLEMENT_DYNAMIC(XD3DWnd, CDockablePane)

BEGIN_MESSAGE_MAP(XD3DWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int XD3DWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	return 0;
}

void XD3DWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
	{
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}

void XD3DWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
	if( GetChain() )
	{
		LPDIRECT3DDEVICE9 pd3dDevice = GRAPHICS_D3D->GetDevice();
		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			CRect rect;
			GetClientRect( &rect );
			XE::VEC2 vRectSize = XE::VEC2( (float)rect.right - rect.left, (float)rect.bottom - rect.top );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			pd3dDevice->SetRenderTarget( 0, pBackBuffer );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(pd3dDevice->BeginScene()))
			{
				Draw();
				pd3dDevice->EndScene();
			}
			GetChain()->Present( NULL, NULL, GetSafeHwnd(), NULL, 0 );
			pBackBuffer->Release();
		} // pBackBuffer
	}
}
