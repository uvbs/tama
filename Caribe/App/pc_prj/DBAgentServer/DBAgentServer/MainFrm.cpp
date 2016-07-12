
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "DBAgentServer.h"
#include "DlgGmMsg.h"
#include "XGameSvrConnection.h"
#include "XSocketForGameSvr.h"
#include "XMain.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND( IDM_GM_MSG, &CMainFrame::OnGmMsg )
	ON_COMMAND( IDM_RELOAD_INI, &CMainFrame::OnReloadIni )
	ON_COMMAND( IDM_SEND_MAIL_ALL, &CMainFrame::OnSendMailAll )
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
	: XEServerMainFrame( _T("DBAgentServer") )
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

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


void CMainFrame::OnGmMsg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	DlgGmMsg dlg;
	dlg.m_strMsg = DlgGmMsg::s_strMsg;
	if( dlg.DoModal() == IDOK )
	{
		// 이곳에 코딩합니다.
		_tstring strMsg = dlg.m_strMsg;
		CONSOLE("%s", strMsg.c_str() );
		XSocketForGameSvr::sGet()
			->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::SendNotify, strMsg );
	}
}

void CMainFrame::OnReloadIni()
{
	MAIN->ReloadIni();
}

void CMainFrame::OnSendMailAll()
{
// 	XPostInfo post( XE::GenerateID() );
// 	post.SetstrSenderName( _T( "tester" ) );
// 	post.SetstrRecvName( _T("player") );
// 	post.SetstrTitle( _T( "This is title" ) );
// 	post.SetstrMessage( _T( "This is message." ) );
// 	post.AddPostGold( 100 );
// 	post.AddPostItem( 1, 1 );		// longsword
// 	XSocketForGameSvr::sGet()
// 		->CallFuncToConnect<XGameSvrConnection>( &XGameSvrConnection::, strMsg );
}

CString CMainFrame::GetstrTitle() const
{
	auto str = XEServerMainFrame::GetstrTitle();
	str += _T("-");
	str += XGAME::GetstrDEV_LEVEL();
	return str;
}

