﻿//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

#include "d3dsettings.h"
#include "d3denumeration.h"
extern DWORD FULLSCREEN_WIDTH ;
extern DWORD FULLSCREEN_HEIGHT;

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESETFAILED         0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d




//-----------------------------------------------------------------------------
// Name: class CD3DApplication
// Desc: A base class for creating sample D3D9 applications. To create a simple
//       Direct3D application, simply derive this class into a class (such as
//       class CMyD3DApplication) and override the following functions, as 
//       needed:
//          OneTimeSceneInit()    - To initialize app data (alloc mem, etc.)
//          InitDeviceObjects()   - To initialize the 3D scene objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//          DeleteDeviceObjects() - To cleanup the 3D scene objects
//          FinalCleanup()        - To cleanup app data (for exitting the app)
//          MsgProc()             - To handle Windows messages
//-----------------------------------------------------------------------------
class CD3DApplication
{
public:
    CD3DSettings      m_d3dSettings;
protected:
    CD3DEnumeration   m_d3dEnumeration;

    // Internal variables for the state of the app
    bool              m_bWindowed;
    bool              m_bActive;
    bool              m_bDeviceLost;
    bool              m_bMinimized;
    bool              m_bMaximized;
    bool              m_bIgnoreSizeChange;
    bool              m_bDeviceObjectsInited;
    bool              m_bDeviceObjectsRestored;

    // Internal variables used for timing
    bool              m_bFrameMoving;
    bool              m_bSingleStep;

	int               m_nLanguage;

    // Internal error handling function
    HRESULT DisplayErrorMsg( HRESULT hr, DWORD dwType );

    // Internal functions to manage and render the 3D scene
    static bool ConfirmDeviceHelper( D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType, 
		D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
    void    BuildPresentParamsFromSettings();
    bool    FindBestWindowedMode( bool bRequireHAL, bool bRequireREF );
    bool    FindBestFullscreenMode( bool bRequireHAL, bool bRequireREF );
    HRESULT ChooseInitialD3DSettings();
    HRESULT Initialize3DEnvironment();
    HRESULT HandlePossibleSizeChange();
    HRESULT Reset3DEnvironment();
    HRESULT ToggleFullscreen();
    HRESULT ForceWindowed();
    HRESULT UserSelectNewDevice();
    void    Cleanup3DEnvironment();
    HRESULT Render3DEnvironment();
    virtual HRESULT AdjustWindowForChange();
    virtual void UpdateStats();

protected:
    // Main objects used for creating and rendering the 3D scene
    HWND              m_hWnd;              // The main app window
    HWND              m_hWndFocus;         // The D3D focus window (usually same as m_hWnd)
    HMENU             m_hMenu;             // App menu bar (stored here when fullscreen)
    DWORD             m_dwCreateFlags;     // Indicate sw or hw vertex processing
    DWORD             m_dwWindowStyle;     // Saved window style for mode switches
    RECT              m_rcWindowBounds;    // Saved window bounds for mode switches
    RECT              m_rcWindowClient;    // Saved client area size for mode switches

    // Variables for timing
    FLOAT             m_fTime;             // Current time in seconds
	
    // Overridable variables for the app
    TCHAR*            m_strWindowTitle;    // Title for the app's window
    DWORD             m_dwCreationWidth;   // Width used to create window
    DWORD             m_dwCreationHeight;  // Height used to create window
    bool              m_bShowCursorWhenFullscreen; // Whether to show cursor when fullscreen
    bool              m_bClipCursorWhenFullscreen; // Whether to limit cursor pos when fullscreen

    // Overridable functions for the 3D scene created by the app
    virtual HRESULT OneTimeSceneInit()                         { return S_OK; }
    virtual HRESULT InitDeviceObjects()                        { return S_OK; }
    virtual HRESULT RestoreDeviceObjects()                     { return S_OK; }
    virtual HRESULT FrameMove()                                { return S_OK; }
    virtual HRESULT Render()                                   { return S_OK; }
    virtual HRESULT InvalidateDeviceObjects()                  { return S_OK; }
    virtual HRESULT DeleteDeviceObjects()                      { return S_OK; }
    virtual HRESULT FinalCleanup()                             { return S_OK; }

public:
    LPDIRECT3D9       m_pD3D;              // The main D3D object
    FLOAT             m_fElapsedTime;      // Time elapsed since last frame
	void SetApp();
	DWORD	GetCreationWidth() { return m_dwCreationWidth; }
	DWORD	GetCreationHeight() { return m_dwCreationHeight; }
	bool	IsWindow() { return m_bWindowed; }
	
    virtual HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT) { return S_OK; }

    D3DPRESENT_PARAMETERS m_d3dpp;         // Parameters for CreateDevice/Reset
    LPDIRECT3DDEVICE9 m_pd3dDevice;        // The D3D rendering device
    D3DCAPS9          m_d3dCaps;           // Caps for the device
	LPDIRECT3DSWAPCHAIN9 m_pd3dChain;
//    D3DSURFACE_DESC   m_d3dsdBackBuffer;   // Surface desc of the backbuffer
	HINSTANCE         m_hInstance;
    bool              m_bStartFullscreen;  // Whether to start up the app in fullscreen mode
	HCURSOR           m_hCursor;
    TCHAR             m_strDeviceStats[90];// String to hold D3D device stats
    TCHAR             m_strFrameStats[90]; // String to hold frame stats
    FLOAT             m_fFPS;              // Instanteous frame rate
	CRect GetDeviceRect() { return CRect( 0, 0, m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight ); }
	void  SetDeviceCursor( HCURSOR hCursor );
	void ClipCursor();
//    DWORD             m_dwAdapter;

    // Functions to create, run, pause, and clean up the application
    virtual HRESULT Create( HINSTANCE hInstance );
    virtual INT     Run();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void    Pause( bool bPause );
    virtual         ~CD3DApplication()                         { }

    // Internal constructor
    CD3DApplication();

#ifndef	__COLA
//		HWND GetSafeHwnd() { return m_hWnd; } 
		LPDIRECT3DDEVICE9 GetDevice() { return m_pd3dDevice; }
#endif

};



#endif



