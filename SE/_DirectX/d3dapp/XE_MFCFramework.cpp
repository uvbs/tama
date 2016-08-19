#include "stdafx.h"
#include "XE_MFCFramework.h"
#include "XGraphicsD3DTool.h"

HRESULT XE_MFCFramework::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        ::ShowWindow( CD3DApplication::m_hWnd, SW_HIDE );
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }
    return S_OK;
}

LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
        AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }

    if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }

    if( msg == WM_KEYUP && wParam == VK_ESCAPE )
    {
        // User wants to leave fullscreen mode
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

HRESULT XE_MFCFramework::Create( HWND hwndSafe, HINSTANCE hInstance )
{
    // Register a class for a fullscreen window
	XLOG( "Create" );
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, CW_USEDEFAULT,
                                           CW_USEDEFAULT, 100, 100,
										   hwndSafe, 0L, NULL, 0L );
    CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
    CD3DApplication::m_hWnd = NULL;
    HRESULT hr = CD3DApplication::Create( hInstance );

	XBREAK( GRAPHICS_D3DTOOL == NULL );
	GRAPHICS_D3DTOOL->Initialize( GetDevice() );
	XSurfaceD3D::SetDevice( GetDevice() );
//	hr = GRAPHICS_D3DTOOL->CreateFont(18);
	DWORD dwTextureCaps = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.TextureCaps;
	DWORD dwMaxTextureWidth = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.MaxTextureWidth;
	XSurface::SetMaxSurfaceWidth( dwMaxTextureWidth );
	XSurfaceD3D::Initialize( dwTextureCaps );

	LPD3DXBUFFER pCompileError;
	LPD3DXEFFECT pEffect;
//	hr = D3DXCreateEffectFromResource( m_pd3dDevice, NULL, MAKEINTRESOURCE(IDR_FX1), NULL, NULL 0, NULL, &pEffect, &pCompileError );
	hr = D3DXCreateEffectFromFile( m_pd3dDevice, _T("2deffect.fx"), NULL, NULL, 0, NULL, &pEffect, &pCompileError );
    if (FAILED(hr))
    {
		if( pCompileError )
		{
			LPCTSTR str = (LPCTSTR)pCompileError->GetBufferPointer();
			CString strErr = str;
			XERROR( "Failed to load effect file\r\n%s", (LPCTSTR)strErr );
		} else
		{
			XERROR( "Failed to load effect file\r\n" );
		}
    } 
	XE::Init( m_pd3dDevice, 0, 0, pEffect );

	return hr;
}
