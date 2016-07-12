#include "stdafx.h"
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
//#include "Mainfrm.h"
#include "d3dView.h"
#include "etc/XGraphics.h"
#include "XFramework/client/XClientMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


void XD3DView::Release()
{
	SafeRelease( &m_pd3dChain );
}

HRESULT XD3DView::CreateView( HWND hWnd, LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat )
{
	SafeRelease( &m_pd3dChain );
//	CMainFrame *pMainFrm = g_pMainFrm;
    
	m_hWnd = hWnd;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	// Use the current display mode.
//	d3dpp.BackBufferFormat = g_pMainFrm->m_d3dpp.BackBufferFormat;
	d3dpp.BackBufferFormat = backBufferFormat;
	// m_hWnd contains child window handle
	d3dpp.hDeviceWindow=m_hWnd;
	// m_pSwapChain is IDirect3DSwapChain *
	HRESULT hr = pd3dDevice->CreateAdditionalSwapChain( &d3dpp, &m_pd3dChain );
	// Store render target surface desc
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
//	XBREAK( m_pd3dChain == NULL );
	if( m_pd3dChain )
	{
		m_pd3dChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
		pBackBuffer->Release();
	}
	return S_OK;
}

void XD3DView::Draw( LPDIRECT3DDEVICE9 pd3dDevice, XClientMain *pMain, CWnd *pWnd )
{
	XPROF_OBJ_AUTO();
	{
		XPROF_OBJ("XClientMain::FrameMove");
		pMain->FrameMove();				// XGraphics::ScreenToSurface()땜에 밖으로 나온듯
	}
	//
	{
	XPROF_OBJ( "XD3DView::Draw" )
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			CRect rect;
			pWnd->GetClientRect( &rect );
//			XE::VEC2 vRectSize = XE::VEC2( (float)rect.right - rect.left, (float)rect.bottom - rect.top );
			XE::SetProjection( GRAPHICS->GetLogicalScreenSize().w, GRAPHICS->GetLogicalScreenSize().h );
			pd3dDevice->SetRenderTarget( 0, pBackBuffer );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, XCOLOR_BLACK, 1.0f, 0 );
			if(SUCCEEDED(pd3dDevice->BeginScene()))
			{
				OnDrawD3DView( pMain );
				pd3dDevice->EndScene();
			}
			// flip
			GetChain()->Present( NULL, NULL, pWnd->GetSafeHwnd(), NULL, 0 );
			pBackBuffer->Release();

		} // pBackBuffer
	}
	}
}

void XD3DView::OnDrawD3DView( XClientMain *pMain )
{
	pMain->Draw();
}

#endif // _VER_DX
