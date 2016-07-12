
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Bot.h"

#include "MainFrm.h"
#include "XMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND( IDM_READY_OFFLINE, &CMainFrame::OnReadyOffline )
	ON_UPDATE_COMMAND_UI( IDM_READY_OFFLINE, &CMainFrame::OnUpdateReadyOffline )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// wndConsole
	if (!m_wndConsole.Create(this
												, &m_dlgConsole
												, CString("Console")
												, IDD_CONSOLE
												, IDM_VIEW_CONSOLE
												, WS_CHILD | WS_VISIBLE | CBRS_RIGHT )) {
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndConsole.SetBarStyle( m_wndConsole.GetBarStyle() 
														| CBRS_TOOLTIPS 
														| CBRS_FLYBY 
														| CBRS_SIZE_DYNAMIC );
	m_wndConsole.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar(&m_wndConsole );
	//FloatControlBar( &m_wndConsole, CPoint(100,600) );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	cs.cx = 800;
	cs.cy = 480;

	return TRUE;
}
// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����

void CMainFrame::OnReadyOffline()
{
	XMain::sGet()->TogglebReadyOffline();
}

void CMainFrame::OnUpdateReadyOffline( CCmdUI *pCmdUI )
{
	if( XMain::sGet()->GetbReadyOffline() )
		pCmdUI->SetCheck( 1 );
	else
		pCmdUI->SetCheck( 0 );
}