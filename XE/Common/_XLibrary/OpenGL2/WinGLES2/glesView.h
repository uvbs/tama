#pragma once
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include "XE.h"
#include "etc/global.h"
class XClientMain;
class XGLESView : public CView
{
	HWND m_hWnd = nullptr;              // The main app window
	HDC		m_hdc = nullptr;
	CDC*	m_pDC = nullptr;
	XE::VEC2 m_vScaleView;
	EGLContext		m_eglContext = nullptr;
	EGLDisplay		m_eglDisplay = nullptr;
	EGLSurface		m_eglSurface = nullptr;
	EGLConfig		m_eglConfig = nullptr;
	void Init() {
		m_hWnd = NULL;
		m_vScaleView.Set( 1.0f );
	}
	void Destroy();
private:
	HGLRC		m_hRenderContext;
	int			m_SurfaceMode;
	EGLConfig SelectEGLConfiguration( const BOOL bDepth, const BOOL bPBuffer, const int nFSAA );
public:
	XGLESView() { Init(); }
	virtual ~XGLESView() { Destroy(); }

	GET_ACCESSOR( const XE::VEC2&, vScaleView );
	void SetScaleView( float scale ) { m_vScaleView.Set( scale ); Update(); }

	HRESULT CreateView( CView *pView );
	void Release();
	void Draw( /*CDC* pTargetDC,*/ XClientMain *pMain );

	// virtual
	virtual void Update( void ) {}
	void Draw();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnSize( UINT nType, int cx, int cy );
//	afx_msg void OnDestroy();
	virtual void OnDraw( CDC* /*pDC*/ );
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
};
