#pragma once
#include "d3dapp.h"
#include "XE.h"
#include "global.h"
class XD3DView
{
    HWND              m_hWnd;              // The main app window
	LPDIRECT3DSWAPCHAIN9 m_pd3dChain;
	XE::VEC2 m_vScaleView;
//	DWORD             m_dwWindowStyle;     // Saved window style for mode switches
//	RECT              m_rcWindowBounds;    // Saved window bounds for mode switches
//	RECT              m_rcWindowClient;    // Saved client area size for mode switches
	void Init() {
		m_pd3dChain = NULL;
		m_hWnd = NULL;
		m_vScaleView.Set( 1.0f );
	}
	void Destroy() {}
public:
    D3DSURFACE_DESC   m_d3dsdBackBuffer;   // Surface desc of the backbuffer
	XD3DView() { Init(); }
	virtual ~XD3DView() { Destroy(); }

	GET_ACCESSOR( const XE::VEC2&, vScaleView );
	void SetScaleView( float scale ) { m_vScaleView.Set( scale ); Update(); }

	LPDIRECT3DSWAPCHAIN9 GetChain() { 
//		if( m_pd3dChain == NULL )
//			Create();
		return m_pd3dChain; 
	}
	HRESULT CreateView( HWND hWnd, LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat );
	void Release();

	// virtual
	virtual void Update() {}
};
