
// [!output PROJECT_NAME]View.cpp : [!output PROJECT_NAME]View Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "[!output PROJECT_NAME].h"
#endif

#include "[!output PROJECT_NAME]Doc.h"
#include "[!output PROJECT_NAME]View.h"
#include "MainFrm.h"
#include "client/XClientMain.h"
#include "client/XAppMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

[!output PROJECT_NAME]View* s_pView = NULL;
[!output PROJECT_NAME]View* GetView() { return s_pView; }
// [!output PROJECT_NAME]View

IMPLEMENT_DYNCREATE([!output PROJECT_NAME]View, CScrollView)

BEGIN_MESSAGE_MAP([!output PROJECT_NAME]View, CScrollView)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// [!output PROJECT_NAME]View ����/�Ҹ�

[!output PROJECT_NAME]View::[!output PROJECT_NAME]View()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	s_pView = this;

}

[!output PROJECT_NAME]View::~[!output PROJECT_NAME]View()
{
}

BOOL [!output PROJECT_NAME]View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow(cs);
}



void [!output PROJECT_NAME]View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CRect rect;
	GetClientRect( &rect );
	//	MAP->SetViewSize( rect.Width(), rect.Height() );
	DragAcceptFiles( TRUE );
	XD3DView::CreateView( GetSafeHwnd(), MAINFRAME->GetDevice(), MAINFRAME->m_d3dpp.BackBufferFormat );
	CSize size(PHY_WIDTH, PHY_HEIGHT);
	//	CSize size(0, 0);
	SetScrollSizes( MM_TEXT, size );	
	GetClientRect( &rect );
	SetWindowPos( NULL, 0, 0, PHY_WIDTH, PHY_HEIGHT, SWP_NOZORDER | SWP_NOMOVE );
	GetClientRect( &rect );
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//	pMainFrm->RecalcLayout();
	GetClientRect( &rect );
	GetParentFrame()->RecalcLayout();
	SetScaleToFitSize(rect.Size());
	CFrameWnd *pFrm = GetParentFrame();
	ResizeParentToFit(FALSE);
	ResizeParentToFit();

	appDelegate.Create( XE::DEVICE_WINDOWS, PHY_WIDTH, PHY_HEIGHT );
}

// [!output PROJECT_NAME]View �׸���
void [!output PROJECT_NAME]View::Draw( void )
{
	XD3DView::Draw( D3DDEVICE, XApp::sGetMain(), this );
	if( XApp::sGetMain()->GetbDestroy() )
	{
		ASSERT(AfxGetMainWnd() != NULL );
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}
}

void [!output PROJECT_NAME]View::OnDraw(CDC* /*pDC*/)
{
	[!output PROJECT_NAME]Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	Draw();
}
// [!output PROJECT_NAME]View ����

#ifdef _DEBUG
void [!output PROJECT_NAME]View::AssertValid() const
{
	CScrollView::AssertValid();
}

void [!output PROJECT_NAME]View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

[!output PROJECT_NAME]Doc* [!output PROJECT_NAME]View::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS([!output PROJECT_NAME]Doc)));
	return ([!output PROJECT_NAME]Doc*)m_pDocument;
}
#endif //_DEBUG


// [!output PROJECT_NAME]View �޽��� ó����
void [!output PROJECT_NAME]View::OnSize(UINT nType, int cx, int cy)	
{
	CView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( D3DDEVICE )
	{
		GRAPHICS->SetPhyScreenSize( cx, cy );
		//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}

void [!output PROJECT_NAME]View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_MOVE, 1, (float)point.x, (float)point.y );
	__super::OnMouseMove(nFlags, point);
}

void [!output PROJECT_NAME]View::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_DOWN, 1, (float)point.x, (float)point.y );

	__super::OnLButtonDown(nFlags, point);
}

void [!output PROJECT_NAME]View::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XApp::sGetMain() )
		XApp::sGetMain()->OnTouchEvent( XClientMain::xTE_UP, 1, (float)point.x, (float)point.y );
	__super::OnLButtonUp(nFlags, point);
}


BOOL [!output PROJECT_NAME]View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( zDelta < 0 )	// �Ʒ��� ����(���)
	{
	} else
	if( zDelta > 0 )
	{
	}
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}


void [!output PROJECT_NAME]View::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XAPP )
		XAPP->OnTouchEvent( XClientMain::xTE_RDOWN, 1, (float)point.x, (float)point.y );

	__super::OnLButtonDown(nFlags, point);
}
void [!output PROJECT_NAME]View::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	if( XAPP )
		XAPP->OnTouchEvent( XClientMain::xTE_RUP, 1, (float)point.x, (float)point.y );


	__super::OnLButtonUp(nFlags, point);
}
