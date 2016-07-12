#pragma once
#ifdef _VER_DX
#include "dxutil.h"
#include "d3denumeration.h"
#include "d3dsettings.h"
#include "d3dutil.h"
#include "d3dapp.h"

class XE_MFCFramework : public CD3DApplication
{
    HWND       m_hwndRenderWindow;
    HWND       m_hwndRenderFullScreen;
public:
	XE_MFCFramework() {
		m_hwndRenderWindow = NULL;
		m_hwndRenderFullScreen = NULL;
	}
	virtual ~XE_MFCFramework() {}

	HRESULT Create( HWND hwndSafe, HINSTANCE hInstance );
	HRESULT AdjustWindowForChange();
};
// d3d
#else
class XE_MFCFramework
{
	HWND       m_hwndRenderWindow;
	HWND       m_hwndRenderFullScreen;
public:
	XE_MFCFramework() {
		m_hwndRenderWindow = NULL;
		m_hwndRenderFullScreen = NULL;
	}
	virtual ~XE_MFCFramework() {}

	HRESULT Create( HWND hwndSafe, HINSTANCE hInstance );
	HRESULT AdjustWindowForChange();
};

// not dx
#endif