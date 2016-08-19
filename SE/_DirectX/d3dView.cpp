#include "stdafx.h"
//#include "Mainfrm.h"
#include "d3dView.h"
//#include "XGraphics.h"


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
