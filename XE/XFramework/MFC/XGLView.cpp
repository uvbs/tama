#include "stdafx.h"
#include "XGLView.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// libEGL.lib;libGLESv2.lib;opengl32.lib;glu32.lib;glut32.lib;glaux.lib;%(AdditionalDependencies)

XGLView::XGLView()
{
	Init();
}

void XGLView::Destroy()
{
	glDeleteFramebuffers(1, &m_framebuffer);
	m_framebuffer = 0;
	glDeleteRenderbuffers(1, &m_colorRenderbuffer);
	m_colorRenderbuffer = 0;

	if (m_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &m_depthRenderbuffer);
		m_depthRenderbuffer = 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// egl destroy
	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(m_eglDisplay);
	//eglDestroySurface(m_eglDisplay, m_eglSurface);
	ReleaseDC(m_hWnd, m_hdc);
	m_hWnd = NULL;
	m_hdc = NULL;

	delete m_pDC;
	m_pDC = NULL;
}
bool XGLView::Create( CView *pView )
{
	m_pDC = new CClientDC(pView);
	m_hdc = m_pDC->GetSafeHdc();
	m_hWnd = pView->GetSafeHwnd();
	//m_hdc = ::GetDC(m_hWnd);

	//if (!SetDCPixelFormat(m_hdc, 
	//	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE))
	//	return false;

	EGLint    major = 0;
	EGLint    minor = 0;

	m_eglDisplay = eglGetDisplay(m_hdc);
	if (m_eglDisplay == EGL_NO_DISPLAY) {
		AfxMessageBox(_T("failed eglGetDisplay()"), MB_OK);
		return false;
	}

	if (!eglInitialize(m_eglDisplay, &major, &minor)) {
		AfxMessageBox(_T("failed eglInitialize()"), MB_OK);
		return false;
	}

	eglBindAPI(EGL_OPENGL_ES_API);
	//const EGLint pi32ConfigAttribs[] =
	//{
	//	EGL_LEVEL,				0,
	//	EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
	//	EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
	//	EGL_NATIVE_RENDERABLE,	EGL_FALSE,
	//	EGL_DEPTH_SIZE,			EGL_DONT_CARE,
	//	EGL_NONE
	//};

	///*
	//	Step 5 - Find a config that matches all requirements.
	//	eglChooseConfig provides a list of all available configurations
	//	that meet or exceed the requirements given as the second
	//	argument. In most cases we just want the first config that meets
	//	all criteria, so we can limit the number of configs returned to 1.
	//*/
	//int iConfigs;
	//if (!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &iConfigs) || (iConfigs != 1))
	//{
	//}
	// 1 : 2 sampling AA, 2 : 4 sampling AA
	m_eglConfig = SelectEGLConfiguration(false, false, 0);

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_hWnd, NULL);
	if (!m_eglSurface || m_eglSurface == EGL_NO_SURFACE) {
		AfxMessageBox(_T("failed eglCreateWindowSurface()"), MB_OK);
		return false;
	}

	EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

	//	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, NULL);
	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, &contextAttribs[0]);
	if (!m_eglContext || m_eglContext == EGL_NO_CONTEXT) {
		AfxMessageBox(_T("failed eglCreateContext()"), MB_OK);
		return false;
	}
	ASSERT( eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == TRUE );
	//
	m_texFilter = TF_NEAREST; /* Which Filter To Use */

	glGenRenderbuffers(1, &m_colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

	// Create the depth buffer.(일단 안쓰므로 주석처리)
//	glGenRenderbuffers(1, &m_depthRenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	// Create the framebuffer object.
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferRenderbuffer(	GL_FRAMEBUFFER,
								GL_COLOR_ATTACHMENT0,
								GL_RENDERBUFFER,
								m_colorRenderbuffer);
	glFramebufferRenderbuffer(	GL_FRAMEBUFFER, 
								GL_DEPTH_ATTACHMENT, 
								GL_RENDERBUFFER, 
								m_depthRenderbuffer);

	// Bind the color buffer for rendering.
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	// Get width and height info.
	//glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	//glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
	//std::cout << "Initialize " << width << ":" << height << "\r\n";

	//ResizeWindow(1280, 640);
	return true;
}

EGLConfig XGLView::SelectEGLConfiguration(const BOOL bDepth, const BOOL bPBuffer, const int nFSAA)
{
	EGLConfig	eglConfig =  0;
	EGLint		numConfigs =  0;
	EGLint		index = -1;

	EGLint configAttribs[32] = {0, };

	configAttribs[++index] = EGL_RENDERABLE_TYPE;
	configAttribs[++index] = EGL_OPENGL_ES2_BIT;
	configAttribs[++index] = EGL_DEPTH_SIZE;
	configAttribs[++index] = 24;

	switch(nFSAA)
	{
	case 1:
		{
			configAttribs[++index] = EGL_SAMPLE_BUFFERS;
			configAttribs[++index] = 1;
			configAttribs[++index] = EGL_SAMPLES;
			configAttribs[++index] = 2;
		} break;
	case 2:
		{
			configAttribs[++index] = EGL_SAMPLE_BUFFERS;
			configAttribs[++index] = 1;
			configAttribs[++index] = EGL_SAMPLES;
			configAttribs[++index] = 4;
		} break;
	default:
		{
			configAttribs[++index] = EGL_SAMPLE_BUFFERS;
			configAttribs[++index] = 0;
		} break;
	}

	configAttribs[++index] = EGL_NONE;

	if( EGL_FALSE == eglChooseConfig(m_eglDisplay, &configAttribs[0], &eglConfig, 1, &numConfigs) )
	{
		return 0;
	}

	return eglConfig;
}

void XGLView::ResizeWindow(int width, int height)
{
	//GRAPHICS->SetPhyScreenSize( width, height );
	// Create the depth buffer.
	if( m_depthRenderbuffer )
	{
		glGenRenderbuffers(1, &m_depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	}
#if 0
	/* (height/width) ratio */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if (height == 0) {
		height = 1;
	}

	// window 크기가 변경될 경우, width vs height 의 크기를 비교하여, 큰쪽을 기준으로 비율을 정하자.
	if (width > height)
		ratio = (GLfloat)width / (GLfloat)height;
	else
		ratio = (GLfloat)height / (GLfloat)width;

	/* Setup our viewport. */
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	screenSize = ivec2(width, height);

	/* Set the projection transform */
	//float h = 4.0f * screenSize.y / screenSize.x;
	//mat4 projectionMatrix = mat4::Ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	//mat4 projectionMatrix = mat4::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
	//mat4 projectionMatrix = mat4::Frustum(-2, 2, -h/2, h/2, 5, 100);
	mat4 projectionMatrix = mat4::Perspective(45.0f, ratio, 1.0f, 100.0f);

	glUseProgram(m_simple.Program);
	glUniformMatrix4fv(m_simple.Uniforms.Projection, 1, 0, projectionMatrix.Pointer());

	glUseProgram(m_cubebox.Program);
	glUniformMatrix4fv(m_cubebox.Uniforms.Projection, 1, 0, projectionMatrix.Pointer());
#endif
}
void XGLView::Draw( CDC* pTargetDC )
{
	// Swap buffers
	if (!pTargetDC->IsPrinting()) {
		eglSwapBuffers(m_eglDisplay, m_eglSurface);
	}
}
void XGLView::Draw()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}
