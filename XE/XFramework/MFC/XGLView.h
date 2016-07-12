#pragma once

#include "EGL/egl.h"
#include "EGL/eglplatform.h"
#include "GLES2/gl2.h"
#pragma comment( lib, "../../../../../XE/Third_Party_lib/OGLES2/WindowsX86/Lib/libEGL.lib" )
#pragma comment( lib, "../../../../../XE/Third_Party_lib/OGLES2/WindowsX86/Lib/libGLESv2.lib" )


////////////////////////////////////////////////////////////////
class XGLView 
{
	enum xtTextureFilter {
		TF_NEAREST,
		TF_BILINEAR,
		TF_TRILINEAR,
	};
	HWND    m_hWnd;
	HDC		m_hdc;
	CDC*	m_pDC;
	EGLContext		m_eglContext;
	EGLDisplay		m_eglDisplay;
	EGLSurface		m_eglSurface;	
	EGLConfig		m_eglConfig;
	//
	GLuint m_texFilter;// = 0; /* Which Filter To Use */
	GLuint m_colorRenderbuffer;
	GLuint m_framebuffer;
	GLuint m_depthRenderbuffer;

	void Init() {
		m_hWnd = NULL;
		m_hdc = NULL;
		m_pDC = NULL;
		m_texFilter = TF_NEAREST;
		m_colorRenderbuffer = 0;
		m_framebuffer = 0;
		m_depthRenderbuffer = 0;
	}
	void Destroy();
public:
	XGLView();
	virtual ~XGLView() { Destroy(); }
	//
	bool Create( CView *pView );
	EGLConfig SelectEGLConfiguration(const BOOL bDepth, const BOOL bPBuffer, const int nFSAA);
	void Draw();
	void Draw( CDC* pTargetDC );
	void ResizeWindow(int width, int height);
};

