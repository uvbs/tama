
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "[!output PROJECT_NAME].h"

#include "MainFrm.h"
#include "XGame.h"
#include "client/XAppMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
CMainFrame *g_pMainFrm = NULL;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_CHEAT_MODE, &CMainFrame::OnCheatMode)
	ON_UPDATE_COMMAND_UI(ID_CHEAT_MODE, &CMainFrame::OnUpdateCheatMode)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	g_pMainFrm = this;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::SetTitle( void )
{
	CString strTitle;
	strTitle = _T("XESample - ");
	strTitle += GAME_VER;
	SetWindowText( (LPCTSTR)strTitle ); 
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	XE_MFCFramework::Create( GetSafeHwnd(), AfxGetInstanceHandle() );		// d3d9 초기화

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// wndConsole
	if (!m_wndConsole.Create(this, &m_dlgConsole, CString("Console"), IDD_CONSOLE, IDM_VIEW_CONSOLE, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM ))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndConsole.SetBarStyle( m_wndConsole.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndConsole.EnableDocking( CBRS_ALIGN_BOTTOM );
	DockControlBar(&m_wndConsole);

	//	FloatControlBar( &m_wndConsole, CPoint(100,600) );

	SetTitle();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	int scrW = GetSystemMetrics( SM_CXSCREEN );
	int scrH = GetSystemMetrics( SM_CYSCREEN );

	// MainFrm의 초기위치를 지정
	float ratio = (float)PHY_HEIGHT / PHY_WIDTH;
	cs.cx = PHY_WIDTH;	// 창크기
	if( scrH <= 1050 )
		cs.cx = 500;
	else
	if( scrH <= PHY_HEIGHT )
		cs.cx = 600;
	cs.cy = (int)(cs.cx * ratio);// 430 * 2;
	cs.cx += 20;
	cs.cy += 216; 
	cs.x = scrW/2 - (cs.cx/2);
	//	cs.y = scrH/2 - (cs.cy/2);;
	cs.y = 0; 

	return TRUE;
}

// CMainFrame 진단

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


// CMainFrame 메시지 처리기
void CMainFrame::OnCheatMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XAPP->OnCheatMode();
}


void CMainFrame::OnUpdateCheatMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( XAPP->m_bDebugMode )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
