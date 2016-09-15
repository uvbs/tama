#include "stdafx.h"
#include "glesView.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XGLESView::Destroy() 
{
	TRACE("XGLESView::Destroy()\n");
	Release();
}

void XGLESView::Release()
{
	auto 
	bOk = eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	bOk = eglDestroySurface( m_eglDisplay, m_eglSurface );
	bOk = eglDestroyContext( m_eglDisplay, m_eglContext );
	bOk = eglTerminate( m_eglDisplay );
	//eglDestroySurface(m_eglDisplay, m_eglSurface);

	::ReleaseDC( m_hWnd, m_hdc );
	m_hWnd = NULL;
	m_hdc = NULL;

	delete m_pDC;
	m_pDC = NULL;
}

// void XGLESView::OnDestroy()
// {
// 	__super::OnDestroy();
// 	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
// }

int XGLESView::OnCreate( LPCREATESTRUCT lpcs )
{
	if( S_OK != CreateView( this ) )
		return -1;
	GRAPHICS_GL->RestoreDevice();
	return 0;
}

void XGLESView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

 	auto sizePhy = GRAPHICS_GL->GetPhyScreenSize();
// 	appDelegate.Create( XE::DEVICE_WINDOWS, (int)sizePhy.w, (int)sizePhy.h );
	CRect rect;
	GetClientRect( &rect );
	CSize size( (int)sizePhy.w, (int)sizePhy.h );
// 	CSize size(PHY_WIDTH, PHY_HEIGHT);
	//	CSize size(0, 0);
//	SetScrollSizes( MM_TEXT, size );	
	GetClientRect( &rect );
	SetWindowPos( NULL, 0, 0, (int)sizePhy.w, (int)sizePhy.h, SWP_NOZORDER | SWP_NOMOVE );
	GetClientRect( &rect );
	auto pMainFrm = AfxGetMainWnd();
	//	pMainFrm->RecalcLayout();
	GetClientRect( &rect );
	GetParentFrame()->RecalcLayout();
//	SetScaleToFitSize(rect.Size());
	CFrameWnd *pFrm = GetParentFrame();
//	ResizeParentToFit(FALSE);
//	ResizeParentToFit();
}

void XGLESView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize( nType, cx, cy );
	if( cx == 0 || cy == 0 )
		return;
	if( cx != EGL_WIDTH || cy != EGL_HEIGHT ) {
		if( GRAPHICS && XApp::sGetMain() )
			GRAPHICS->SetPhyScreenSize( cx, cy );
	}
}

void XGLESView::Draw()
{
	Draw( XApp::sGetMain() );
	if( XApp::sGetMain() && XApp::sGetMain()->GetbDestroy() ) {
		ASSERT( AfxGetMainWnd() != NULL );
		AfxGetMainWnd()->SendMessage( WM_CLOSE );
	}
}

void XGLESView::Draw( /*CDC* pTargetDC,*/ XClientMain *pMain )
{
	XPROF_OBJ_AUTO();
	if( pMain ) {
		XPROF_OBJ("frameMove/draw");
		pMain->FrameMove();				// XGraphics::ScreenToSurface()땜에 밖으로 나온듯
		pMain->Draw();
	}
	// Swap buffers
//	if( !pTargetDC->IsPrinting() ) {
	{
		XPROF_OBJ("swap buff");
		eglSwapBuffers( m_eglDisplay, m_eglSurface );
	}
//	}
}

void XGLESView::OnDraw( CDC* /*pDC*/ )
{
	
	// 	WinGLDoc* pDoc = GetDocument();
// 	ASSERT_VALID( pDoc );
// 	if( !pDoc )
// 		return;
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	Draw();
}

#include <iostream>
HRESULT XGLESView::CreateView( CView *pView )
{
	m_pDC = new CClientDC(pView);
	m_hdc = m_pDC->GetSafeHdc();
	m_hWnd = pView->GetSafeHwnd();
//	m_hdc = ::GetDC(m_hWnd);
	//if (!SetDCPixelFormat(m_hdc, 
	//	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE))
	//	return false;
	// Now create our render context.
	EGLint    major = 0;
	EGLint    minor = 0;
	m_eglDisplay = eglGetDisplay( m_hdc );
	if( m_eglDisplay == EGL_NO_DISPLAY ) {
		AfxMessageBox( _T( "eglGetDisplay() Fail..." ), MB_OK, -1 );
		return -1;
	}
	if (!eglInitialize(m_eglDisplay, &major, &minor)) {
		AfxMessageBox(_T("eglInitialize() Fail..."), MB_OK, -1);
		return -1;
	}
	eglBindAPI( EGL_OPENGL_ES_API );
	// 1 : 2 sampling AA, 2 : 4 sampling AA
	m_eglConfig = SelectEGLConfiguration(true, false, 0);
	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_hWnd, NULL);
	if (!m_eglSurface || m_eglSurface == EGL_NO_SURFACE) {
		AfxMessageBox(_T("eglCreateWindowSurface() Fail..."), MB_OK, -1);
		return -1;
	}
	EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
//	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, NULL);
	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, &contextAttribs[0]);
	if (!m_eglContext || m_eglContext == EGL_NO_CONTEXT) {
		AfxMessageBox(_T("eglCreateContext() Fail..."), MB_OK, -1);
		return -1;
	}
	if( eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext ) != TRUE ) {
		ASSERT( 0 );
	}
//	m_SurfaceMode = CDC_SURFACE_MODE;
	return S_OK;
}

EGLConfig XGLESView::SelectEGLConfiguration( const BOOL bDepth, const BOOL bPBuffer, const int nFSAA )
{
	EGLConfig	eglConfig = 0;
	EGLint		numConfigs = 0;
	EGLint		index = -1;

//	EGLint configAttribs[ 32 ] = {0, };
	// typical high-quality attrib list
	EGLint configAttribs[] = {
		// 32 bit color
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		// at least 24 bit depth
		EGL_DEPTH_SIZE, 24,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		// want opengl-es 2.x conformant CONTEXT
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
// 	configAttribs[ ++index ] = EGL_RENDERABLE_TYPE;
// 	configAttribs[ ++index ] = EGL_OPENGL_ES2_BIT;
// 	configAttribs[ ++index ] = EGL_DEPTH_SIZE;
// 	configAttribs[ ++index ] = 0;
// 
// 	switch( nFSAA )
// 	{
// 	case 1:
// 	{
// 		configAttribs[ ++index ] = EGL_SAMPLE_BUFFERS;
// 		configAttribs[ ++index ] = 1;
// 		configAttribs[ ++index ] = EGL_SAMPLES;
// 		configAttribs[ ++index ] = 2;
// 	} break;
// 	case 2:
// 	{
// 		configAttribs[ ++index ] = EGL_SAMPLE_BUFFERS;
// 		configAttribs[ ++index ] = 1;
// 		configAttribs[ ++index ] = EGL_SAMPLES;
// 		configAttribs[ ++index ] = 4;
// 	} break;
// 	default:
// 	{
// 		configAttribs[ ++index ] = EGL_SAMPLE_BUFFERS;
// 		configAttribs[ ++index ] = 0;
// 	} break;
// 	}

//	configAttribs[ ++index ] = EGL_NONE;

	if( EGL_FALSE == eglChooseConfig( m_eglDisplay, &configAttribs[ 0 ], &eglConfig, 1, &numConfigs ) )
	{
		return 0;
	}

	return eglConfig;
}

