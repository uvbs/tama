#include "stdafx.h"
#ifdef _VER_DX
#include "XE_MFCFramework.h"
#include "_DirectX/XGraphicsD3DTool.h"
#include "etc/path.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

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
	WNDCLASS wndClass = {CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
											NULL, NULL, (HBRUSH)GetStockObject( WHITE_BRUSH ), NULL,
											_T( "Fullscreen Window" )};
	RegisterClass( &wndClass );
	m_hwndRenderFullScreen = CreateWindow( _T( "Fullscreen Window" ), NULL,
																				WS_POPUP, CW_USEDEFAULT,
																				CW_USEDEFAULT, 100, 100,
																				hwndSafe, 0L, NULL, 0L );
#ifdef _VER_DX
	CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
	CD3DApplication::m_hWnd = NULL;
	HRESULT hr = CD3DApplication::Create( hInstance );

#ifdef _XLIBPNG
	XBREAK( GRAPHICS_D3D == NULL );
#else
	XBREAK( GRAPHICS_D3DTOOL == NULL );
#endif
	GRAPHICS_D3D->Initialize( GetDevice() );
	XSurfaceD3D::SetDevice( GetDevice() );
//	hr = GRAPHICS_D3DTOOL->CreateFont(18);
	DWORD dwTextureCaps = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.TextureCaps;
	DWORD dwMaxTextureWidth = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.MaxTextureWidth;
	dwMaxTextureWidth = 2048;
	XSurface::SetMaxSurfaceWidth( dwMaxTextureWidth );
	XSurfaceD3D::Initialize( dwTextureCaps );
	//
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
			XERROR( "Failed to load effect file\r\nerror:%s", (LPCTSTR)strErr );
		} else
		{
			switch( hr )
			{
			case D3DERR_INVALIDCALL:
				XERROR( "Failed to load effect file-0x%0X\r\nD3DERR_INVALIDCALL", hr );
			case D3DXERR_INVALIDDATA:
				XERROR( "Failed to load effect file-0x%0X\r\nD3DXERR_INVALIDDATA", hr );
			case E_OUTOFMEMORY:
				XERROR( "Failed to load effect file-0x%0X\r\nE_OUTOFMEMORY", hr );
			default:
				XERROR( "Failed to load effect file-0x%0X\r\n%s", hr, DXGetErrorString( hr ) );
			}
		}
    } 
	XE::Init( m_pd3dDevice, 0, 0, pEffect );
#else
	XE::SetProjection( 0, 0 );
#endif // _VER_DX

	return hr;
}
#endif // dx