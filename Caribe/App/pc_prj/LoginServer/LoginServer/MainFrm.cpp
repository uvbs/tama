
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "LoginServer.h"

#include "MainFrm.h"
#include "XFramework/XDBMng2.h"
#include "XDBUAccount.h"
#include "XDatabase.h"
#include "XMain.h"
// #include "XGameSvrConnection.h"
// #include "XGameSvrSocket.h"
// #include "XPacketLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND( ID_CREATE_DUMMY, &CMainFrame::OnCreateDummy )
	ON_COMMAND( IDM_RELOAD_INI, &CMainFrame::OnReloadIni )
// 	ON_COMMAND( IDM_CONTROL_PANEL, &CMainFrame::OnControlPanel )
// 	ON_UPDATE_COMMAND_UI( IDM_CONTROL_PANEL, &CMainFrame::OnUpdateControlPanel )
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
	: XEServerMainFrame( _T("LoginServer") )
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
//	m_cntSave = 0;
}

CMainFrame::~CMainFrame()
{
}

// void CMainFrame::OnClose()
// {
// 	MAIN->OnDestroy();
// }
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// wndConsole
	if (!m_wndConsole.Create(this, &m_dlgConsole, CString("Console"), IDD_CONSOLE, IDM_VIEW_CONSOLE, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM ))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndConsole.SetBarStyle( m_wndConsole.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndConsole.EnableDocking( CBRS_ALIGN_BOTTOM );
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndConsole);
	//	FloatControlBar( &m_wndConsole, CPoint(600,100) );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

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

/**
 @brief 더미 계정을 만든다.
*/
void CMainFrame::OnCreateDummy()
{
#if defined(_DUMMY_GENERATOR)
	if( MAIN->IsFinishCreateDummy() == FALSE )		// 아직 이전 세이브가 완료되지 않음.
		return;
	auto numCreate = MAIN->StartCreateDummy( 5000 );
	CONSOLE( "creae dummy account:%d........", numCreate );
#endif
}

void CMainFrame::OnReloadIni()
{
	MAIN->ReloadIni();
}

// void CMainFrame::OnControlPanel()
// {
// //	CDlgControlPanel dlg;
// }
// 
// void CMainFrame::OnUpdateControlPanel( CCmdUI* pCmdUI )
// {
// 
// }

CString CMainFrame::GetstrTitle() const
{
	auto str = XEServerMainFrame::GetstrTitle();
	str += _T( "-" );
	str += XGAME::GetstrDEV_LEVEL();
	return str;
}

