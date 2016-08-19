// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "SE.h"
//#define TOOL_VER	"SE temp.01"
#include "verStr.h"
#include "MainFrm.h"
#include "Tool.h"
//#include "Graphic2d.h"
#include "ActionListView.h"
#include "AdjustView.h"
#include "AnimationView.h"
#include "ConsoleView.h"
#include "ModifyView.h"
#include "FrameView.h"
#include "ThumbView.h"
#include "UndoMng.h"
#include "XDlgConvert.h"
#include "SEFont.h"
#include "WndTimeline.h"
#include "XAniAction.h"
#include "XActObj.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//..\..\..\_XLibrary\Sprite\SprMng;.\;..\..\..\PC\_DirectX;..\..\..\PC\_Tool;..\..\..\PC\_DirectX\DXUT;..\..\..\_XLibrary\etc;..\..\..\_XLibrary\etc\D3D;..\..\..\_XLibrary\etc\_GraphicsSub\_D3D;..\..\..\_XLibrary;..\..\..\_XLibrary\Sound\Windows

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_PLAY, &CMainFrame::OnPlay)
	ON_COMMAND(ID_PLAY_STOP, &CMainFrame::OnPlayStop)
	ON_UPDATE_COMMAND_UI(ID_PLAY, &CMainFrame::OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_PLAY_STOP, &CMainFrame::OnUpdatePlayStop)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_TOOL_STOP, &CMainFrame::OnToolStop)
	ON_COMMAND(ID_SPR_OPEN, &CMainFrame::OnSprOpen)
	ON_COMMAND(ID_SPR_SAVE, &CMainFrame::OnSprSave)
	ON_COMMAND(ID_SPR_SAVEAS, &CMainFrame::OnSprSaveas)
	ON_UPDATE_COMMAND_UI(ID_SPR_SAVE, &CMainFrame::OnUpdateSprSave)
	ON_COMMAND(ID_SPR_NEW, &CMainFrame::OnSprNew)
	ON_COMMAND(ID_MOVE, &CMainFrame::OnMoveMode)
	ON_UPDATE_COMMAND_UI(ID_MOVE, &CMainFrame::OnUpdateMoveMode)
	ON_COMMAND(ID_ROTATE, &CMainFrame::OnRotateMode)
	ON_UPDATE_COMMAND_UI(ID_ROTATE, &CMainFrame::OnUpdateRotateMode)
	ON_COMMAND(ID_SCALE, &CMainFrame::OnScaleMode)
	ON_UPDATE_COMMAND_UI(ID_SCALE, &CMainFrame::OnUpdateScaleMode)
	ON_WM_KEYUP()
	ON_COMMAND(ID_PREV_FRAME, &CMainFrame::OnPrevFrame)
	ON_COMMAND(ID_NEXT_FRAME, &CMainFrame::OnNextFrame)
	ON_UPDATE_COMMAND_UI(ID_PREV_FRAME, &CMainFrame::OnUpdatePrevFrame)
	ON_UPDATE_COMMAND_UI(ID_NEXT_FRAME, &CMainFrame::OnUpdateNextFrame)
	ON_COMMAND(ID_LOCK_AXISX, &CMainFrame::OnLockAxisx)
	ON_COMMAND(ID_LOCK_AXISY, &CMainFrame::OnLockAxisy)
	ON_COMMAND(ID_LOCK_AXISXY, &CMainFrame::OnLockAxisxy)
	ON_UPDATE_COMMAND_UI(ID_LOCK_AXISX, &CMainFrame::OnUpdateLockAxisx)
	ON_UPDATE_COMMAND_UI(ID_LOCK_AXISY, &CMainFrame::OnUpdateLockAxisy)
	ON_UPDATE_COMMAND_UI(ID_LOCK_AXISXY, &CMainFrame::OnUpdateLockAxisxy)
	ON_COMMAND(ID_OPEN_CONT_IMAGE, &CMainFrame::OnOpenContImage)
	ON_COMMAND(ID_EDIT_UNDO, &CMainFrame::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMainFrame::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMainFrame::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMainFrame::OnUpdateEditRedo)
	ON_COMMAND(ID_OPEN_ALIGNED_IMAGE, &CMainFrame::OnOpenAlignedImage)
	ON_UPDATE_COMMAND_UI(ID_OPEN_ALIGNED_IMAGE, &CMainFrame::OnUpdateOpenAlignedImage)
	ON_COMMAND(ID_SCALE_VIEW1, &CMainFrame::OnScaleView1)
	ON_COMMAND(ID_SCALE_VIEW2, &CMainFrame::OnScaleView2)
	ON_COMMAND(ID_SCALE_VIEW3, &CMainFrame::OnScaleView3)
	ON_COMMAND(ID_SCALE_VIEW4, &CMainFrame::OnScaleView4)
	ON_COMMAND(ID_FRAMESKIP, &CMainFrame::OnFrameskip)
	ON_UPDATE_COMMAND_UI(ID_FRAMESKIP, &CMainFrame::OnUpdateFrameskip)
	ON_COMMAND(ID_CONVERT, &CMainFrame::OnConvert)
	ON_BN_CLICKED(IDC_BUTT_ADD_LAYER, &CMainFrame::OnBnClickedButtAddLayer)
	ON_BN_CLICKED(IDC_BUTT_CREATEOBJ, &CMainFrame::OnBnClickedButtCreateobj)
	ON_COMMAND(ID_ADD_LAYER_IMAGE, &CMainFrame::OnAddLayerImage)
	ON_COMMAND(ID_ADD_LAYER_OBJECT, &CMainFrame::OnAddLayerObject)
	ON_COMMAND(ID_COMP_CONVERTER, &CMainFrame::OnCompConverter)
	ON_COMMAND( IDM_LOAD_BG, &CMainFrame::OnLoadBg )
	ON_COMMAND( IDM_MODE_BG_MOVE, &CMainFrame::OnModeMoveByBg )
	ON_UPDATE_COMMAND_UI( IDM_MODE_BG_MOVE, &CMainFrame::OnUpdateModeMoveByBg )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸
CMainFrame *g_pMainFrm = NULL;

CMainFrame::CMainFrame()
{
	g_pMainFrm = this;
	m_hwndRenderWindow = NULL;
	m_hwndRenderFullScreen = NULL;
	m_bEditing = FALSE;
	m_bSplitInit = FALSE;
	SAFE_DELETE( SE::g_pFDBasic );
	SAFE_DELETE( SE::g_pFont );
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	Create( GetSafeHwnd() );		// d3d9 초기화

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다. 
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

//	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
//		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Sprite thumbnail view
	if (!m_wndThumbView.Create(_T("Sprites Thumbnail View"), this, CRect(0, 0, 400, 500), TRUE, ID_WND_THUMBVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndThumbView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndThumbView);
//	m_wndThumbView.FloatPane( CRect( 100, 100, 500, 500 ) );	// 주의: 파라메터는 l,t,r,b다 x,y,w,h가 아님

	// Adjust View
	if (!m_wndAdjustView.Create(_T("Sprite Adjust View"), this, CRect(0, 0, 400, 285), TRUE, ID_WND_ADJUSTVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndAdjustView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndAdjustView);

//	m_wndAdjustView.FloatPane( CRect( 100, 500, 500, 900 ) );

//	m_wndThumbView.DockPane(&m_wndAdjustView, &CRect(0, 0, 100, 100), DM_STANDARD);
	// AdjustView를 ThumbView아래 위치시킨다
	m_wndAdjustView.DockToWindow( &m_wndThumbView, CBRS_BOTTOM );

	// Spline Test View
/*	if (!m_wndSpline.Create(_T("Spline Test view"), this, CRect(0, 0, 600, 600), TRUE, ID_WND_SPLINE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndSpline.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndSpline);
	m_wndSpline.FloatPane( CRect( 100, 300, 800, 1000 ) );
	*/
//	m_wndSpline.DockToWindow( &m_wndAdjustView, CBRS_BOTTOM );

	// 툴바윈도우를 생성(테스트)
/*	if (!m_wndLeftTool.Create(_T("툴바"), this, CRect(0, 0, 300, 285), TRUE, ID_WND_LEFTTOOL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndLeftTool.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndLeftTool);
	BOOL bfloat = m_wndLeftTool.CanFloat();
	m_wndLeftTool.FloatPane( CRect( 100, 100, 500, 1000 ) );
*/
	// Frame dialog bar
/*	if (!m_wndFrameBar.Create(_T("dialog bar"), this, CRect(0, 0, 400, 285), TRUE, ID_WND_DIALOGBAR, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndFrameBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFrameBar); 
//	m_wndFrameBar.FloatPane( CRect( 100, 500, 500, 900 ) );
*/
	// Console view
	if (!m_wndConsole.Create(_T("Console view"), this, CRect(0, 0, 400, 100), TRUE, ID_WND_CONSOLE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndConsole.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndConsole); 
	// Timeline view
	if (!m_wndTimeline.Create(_T("Timeline view"), this, CRect(0, 0, 400, 400), TRUE, ID_WND_TIMELINE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndTimeline.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndTimeline); 
	// ActionList view
	if (!m_wndActionList.Create(_T("ActionList view"), this, CRect(0, 0, 125, 200), TRUE, ID_WND_ACTIONLIST, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI ))
	{
		TRACE0("툴바 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndActionList.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndActionList); 
	m_wndActionList.DockToWindow( &m_wndTimeline, CBRS_LEFT );
	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

//	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	DWORD dwTextureCaps = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.TextureCaps;
	GetConsoleView()->Message( _T("Caps.Pow2=%d"), (dwTextureCaps & D3DPTEXTURECAPS_POW2)? 1 : 0 );
	return 0;
}

LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
//        g_pView->OnToggleFullScreen();
        theApp.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }

    if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }

    if( msg == WM_KEYUP && wParam == VK_ESCAPE )
    {
        // User wants to leave fullscreen mode
//        g_pView->OnToggleFullScreen();
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


HRESULT CMainFrame::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        ::ShowWindow( CD3DApplication::m_hWnd, SW_HIDE );
//        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }
    return S_OK;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	int scrW = GetSystemMetrics( SM_CXSCREEN );
	int scrH = GetSystemMetrics( SM_CYSCREEN );

	// MainFrm의 초기위치를 지정
	cs.cx = 1680;
	cs.cy = 1024;
	cs.x = scrW/2 - (cs.cx/2);
	cs.y = scrH/2 - (cs.cy/2);;

	return TRUE;
//	return __super::PreCreateWindow(cs);
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

#if 1 
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	RECT rect;
	GetClientRect( &rect );
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
#ifdef _MODIFY_VIEW
	// 세로4칸 가로1칸으로 나눔
	m_wndSplitter.CreateStatic(this, 4, 1);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CAnimationView), CSize(10,10), pContext);
	// 값 에디팅 뷰를 결합
	m_wndSplitter.CreateView(2, 0, RUNTIME_CLASS(CModifyView), CSize(10,10), pContext);
	// 맨 아래부분에 콘솔뷰를 결합.
	m_wndSplitter.CreateView(3, 0, RUNTIME_CLASS(CConsoleView), CSize(10,0), pContext);
	// 나눈 세라인의 세로크기를 지정
	m_wndSplitter.SetRowInfo( 0, (int)((rect.bottom - rect.top)*0.4), 10 );	
	m_wndSplitter.SetRowInfo( 1, (int)((rect.bottom - rect.top)*0.4), 10 );
	m_wndSplitter.SetRowInfo( 2, (int)((rect.bottom - rect.top)*0.05), 10 );	// 0.05. 기능을 만들기전까지 숨겨두자
	m_wndSplitter.SetRowInfo( 3, (int)((rect.bottom - rect.top)*0.15), 10 );
#else
	// 세로3칸 가로1칸으로 나눔
/*	m_wndSplitter.CreateStatic(this, 3, 1);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CAnimationView), CSize(10,10), pContext);
	// 맨 아래부분에 콘솔뷰를 결합.
	m_wndSplitter.CreateView(2, 0, RUNTIME_CLASS(CConsoleView), CSize(10,0), pContext);
	// 나눈 세라인의 세로크기를 지정
	m_wndSplitter.SetRowInfo( 0, (int)((rect.bottom - rect.top)*0.45), 10 );	
	m_wndSplitter.SetRowInfo( 1, (int)((rect.bottom - rect.top)*0.4), 10 );
	m_wndSplitter.SetRowInfo( 2, (int)((rect.bottom - rect.top)*0.15), 10 );	
	*/
#endif

	// 중간부분을 둘로 나눔
/*	m_wndSplitter3.CreateStatic(
		&m_wndSplitter,
		1, 2,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter.IdFromRowCol( 1, 0 ) );
	m_wndSplitter3.CreateView(0, 0, RUNTIME_CLASS(CActionListView), CSize(100, 200), pContext);
	m_wndSplitter3.CreateView(0, 1, RUNTIME_CLASS(CFrameView), CSize(400, 200), pContext);
	m_wndSplitter3.SetColumnInfo( 0, 100, 10 );
	m_wndSplitter3.SetColumnInfo( 1, (int)((rect.right - rect.left-100)*1.0f), 10 );
*/	
	m_bSplitInit = TRUE;

	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 나눈 세라인의 세로크기를 지정
	RECT rect;
	GetClientRect( &rect );
	if( m_bSplitInit && nType != SIZE_MINIMIZED /*&& cx > 0 && cy > 0*/ )
	{
#ifdef _MODIFY_VIEW
		m_wndSplitter.SetRowInfo( 0, (int)((rect.bottom - rect.top)*0.4), 0 );	
		m_wndSplitter.SetRowInfo( 1, (int)((rect.bottom - rect.top)*0.4), 0 );
		m_wndSplitter.SetRowInfo( 2, (int)((rect.bottom - rect.top)*0.05), 0 );
		m_wndSplitter.SetRowInfo( 3, (int)((rect.bottom - rect.top)*0.15), 0 );
#else
/*		m_wndSplitter.SetRowInfo( 0, (int)((rect.bottom - rect.top)*0.45), 0 );	
		m_wndSplitter.SetRowInfo( 1, (int)((rect.bottom - rect.top)*0.4), 0 );
		m_wndSplitter.SetRowInfo( 2, (int)((rect.bottom - rect.top)*0.15), 0 );
	*/
#endif

		// 중간
/*		RECT rect3;
		m_wndSplitter3.GetClientRect( &rect3 );
		m_wndSplitter3.SetColumnInfo( 0, 100, 0 );
		m_wndSplitter3.SetColumnInfo( 1, (int)((rect.right - rect.left-100)*1.0f), 10 );
		m_wndSplitter.RecalcLayout();
		m_wndSplitter3.RecalcLayout();
		*/
	}
}
#endif // 0

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_bCtrl = false;
	m_bShift = false;
	m_bAlt = false;
	if( nFlags & MK_CONTROL )
		m_bCtrl = true;
	if( nFlags & MK_ALT )
		m_bAlt = true;
	if( nFlags & MK_SHIFT )
		m_bShift = true;

#ifdef _DEBUG
//	CONSOLE("keydown: ctrl=%d, alt=%d, shift=%d", (int)m_bCtrl, (int)m_bAlt, (int)m_bShift );
#endif // _DEBUG

	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) 
	{
		// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	} // if spAction
	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nFlags & MK_CONTROL )
		m_bCtrl = false;
	if( nFlags & MK_ALT )
		m_bAlt = false;
	if( nFlags & MK_SHIFT )
		m_bShift = false;
	if( nChar == VK_LEFT ) {
		GetFrameView()->SetKeyCursor( XE::POINT(-1,0) );
	} else
	if( nChar == VK_RIGHT ) {
		GetFrameView()->SetKeyCursor( XE::POINT( 1, 0 ) );
	}
#ifdef _DEBUG
	CONSOLE( "keyup: ctrl=%d, alt=%d, shift=%d", (int)m_bCtrl, (int)m_bAlt, (int)m_bShift );
#endif // _DEBUG
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::DoPlayStopToggle( bool bToStart )
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction )		return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SPROBJ->IsPause() ) {		
		// 재실행 될때는 미리 플래그를 바꿔서 랜덤이 적용되며 시작되도록 한다.
		SPROBJ->SetPause( !SPROBJ->GetPause() );
		// 현재 멈춰 있는 상태
		// 0프레임부터 현재 프레임까지를 모두 실행시키고 플레이가 시작,
		SPROBJ->MoveFrame( SPROBJ->GetFrmCurr() );
	} else {
		// 현재 플레이 중인 상태
		switch( GetDlgBarTimeline()->GetPlayMode() ) {
		case 0:	{
//			if( spAction->GetRepeatMark() > 0 )	// 도돌이표가 있는상태면
			if( bToStart )	
					SPROBJ->MoveFrame( 0 );			// 첨으로 돌아가서 멈춤
		} break;
		case 1:	{
			// 현재 위치에서 멈춤
			if( bToStart )
				if( SPROBJ->IsEndFrame() )	// 1회플레이 모드일땐 현재 마지막 위치에 가있으면 첨으로 되돌리고 다시 플레이 하도록 한다
					SPROBJ->MoveFrame( 0 );
		} break;
		default:	
			break;
		}
		// 멈출땐 플레그를 나중에 토글시켜서 현재까지 실행된 랜덤 상태를 보존하게 한다.
		SPROBJ->SetPause( !SPROBJ->GetPause() );
	}
// 	if( SPROBJ->IsPause() ) {
// 		GetFrameView()->MoveFrame( 0 );
// 	}
}

void CMainFrame::OnPlay()
{
	DoPlayStopToggle( false );
}

void CMainFrame::OnUpdatePlay(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SPROBJ->GetPause() == TRUE )
		pCmdUI->SetCheck( 0 );
	else
		pCmdUI->SetCheck( 1 );
}

// 정지시 처음으로 돌아가지 않음.
void CMainFrame::OnPlayStop()
{
	CONSOLE("OnPlayStop");
	DoPlayStopToggle( true );
}

void CMainFrame::OnUpdatePlayStop( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( SPROBJ->GetPause() == TRUE )
		pCmdUI->SetCheck( 0 );
	else
		pCmdUI->SetCheck( 1 );
}


void CMainFrame::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnToolStop()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	BOOL bSkip = FALSE;
	UINT nFlags = 0;
	if( pMsg->wParam == VK_SHIFT ) 
		nFlags |= MK_SHIFT;
	else if( pMsg->wParam == VK_CONTROL ) 
		nFlags |= MK_CONTROL;
	else if( pMsg->wParam == VK_MENU )
		nFlags |= MK_ALT;

	if( pMsg->message == WM_KEYDOWN )
	{
//		if( m_bEditing && (pMsg->wParam == '1' || pMsg->wParam == '2') )
//			bSkip = TRUE;
		OnKeyDown( pMsg->wParam, 1, nFlags );
		GetFrameView()->OnKeyDown( pMsg->wParam, 1, nFlags );
		if( GetModifyView() )
			GetModifyView()->OnKeyDown( pMsg->wParam, 1, nFlags );
//		GetAdjustView()->OnKeyDown( pMsg->wParam, 1, nFlags );
	} else
	if( pMsg->message == WM_KEYUP )
	{
		OnKeyUp( pMsg->wParam, 1, nFlags );
		GetFrameView()->OnKeyUp( pMsg->wParam, 1, nFlags ); 
//		GetAdjustView()->OnKeyUp( pMsg->wParam, 1, nFlags );	// 여기다 이거넣으면 다른뷰에서 화살표 해도 전달되기땜에 안됨
	} else
	if( pMsg->message == WM_SYSKEYDOWN )
	{
		GetFrameView()->OnKeyDown( pMsg->wParam, 1, nFlags );
		if( GetModifyView() )
			GetModifyView()->OnKeyDown( pMsg->wParam, 1, nFlags );
	} else
	if( pMsg->message == WM_SYSKEYUP )
	{
		GetFrameView()->OnKeyUp( pMsg->wParam, 1, nFlags ); 
	} else
	if( pMsg->message == WM_RBUTTONUP )
	{
		GetActionListView()->OnRButtonUp( nFlags, CPoint( LOWORD(pMsg->lParam ), HIWORD(pMsg->lParam ) ) );
	}

	BOOL bRet = 0;
	if( m_bEditing == FALSE )		// EditBox를 사용중이지 않을때만 Accelerator를 실행하게 한다
		 bRet = CFrameWndEx::PreTranslateMessage(pMsg);
	
	return bRet;
}

void CMainFrame::OnSprOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog		dlg( true, _T("spr"), NULL, OFN_FILEMUSTEXIST,
					 	_T("spr Files(*.spr)|*.spr|AllFiles(*.*)|*.*||"), NULL );

	if ( dlg.DoModal() == IDOK )
	{
		TOOL->LoadSpr( dlg.GetPathName() );
//		SPROBJ->CalcBoundBox();
	}
}

void CMainFrame::OnSprSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( TOOL->GetstrFilename().IsEmpty() )
	{
		OnSprSaveas();
		return;
	}
//	SPRDAT->ReindexingSprite();							// 스프라이트들의 인덱스를 새로 매긴다
	// 저장
	CString strFilename = TOOL->GetstrFilename();
	// 저장하기전에 이전파일을 백업폴더로 카피한다.
	CString strSrc = TOOL->GetstrPath() + strFilename;
//	CString strDst = TOOL->GetstrWorkPath() + _T("backup\\");
	CString strDst = XE::GetCwd();	// SE.exe 실행파일 폴더를 얻어냄
	strDst += _T("backup\\");
	::CreateDirectory( strDst, nullptr );
	strDst += XE::GetFileTitle( strFilename );
	SYSTEMTIME time;
	::GetLocalTime( &time );
	CString strTime;
	strTime.Format(_T("%02d%02d%02d%02d%02d"), time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
	strDst += strTime + _T( ".spr" );;
	::CopyFile( strSrc, strDst, FALSE );
	if( SPROBJ->Save( (LPCTSTR)strFilename ) )
		GetConsoleView()->Message( _T("%s 저장완료"), (LPCTSTR)strFilename );
	else
	{
		if( SPROBJ->GetpSprDat()->GetError() == XBaseRes::ERR_PERMISSION_DENIED ) 
		{
			XALERT( "Check Out이 되어있지 않습니다" ) ;
			CONSOLE( "%s is Permission denied", strFilename );
		}
		else if( SPROBJ->GetpSprDat()->GetError() == XBaseRes::ERR_READONLY )
			XALERT( "파일이 ReadOnly상태여서 저장에 실패했습니다." ) ;
		else
			XALERT( "저장에 실패했습니다." ) ;
	}
	GetFrameView()->Update();
}

void CMainFrame::OnUpdateSprSave(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	if( TOOL->GetstrFilename().IsEmpty() )		// 저장한 파일명이 없으면 메뉴를 나타내지 않는다.
	if( SPRDAT->GetNumSprite() == 0 )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );

}

void CMainFrame::OnSprSaveas()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog		dlg( false, _T("spr"), NULL, OFN_FILEMUSTEXIST,
								_T("sprite Files(*.spr)|*.spr|AllFiles(*.*)|*.*||"), NULL );
	if ( dlg.DoModal() == IDOK )
	{
//		SPRDAT->ReindexingSprite();							// 스프라이트들의 인덱스를 새로 매긴다
		CString strFilename = dlg.GetFileName();
		CString strPath = dlg.GetFolderPath() + _T("\\");
		TOOL->SetstrPath( strPath );
		SPROBJ->Save( (LPCTSTR)strFilename );
		GetConsoleView()->Message( _T("%s 저장완료"), (LPCTSTR)strFilename );
		TOOL->SetstrFilename( strFilename );				// 저장한 파일명을 기억해뒀다가 그냥 save할때 쓴다
		SetTitle();
	}
}

void CMainFrame::SetTitle()
{
	CString strTitle;
	if( TOOL->GetstrFilename().IsEmpty() )
		strTitle = _T("제목 없음");
	else
		strTitle = TOOL->GetstrFilename();
	strTitle += _T(" - SE ");
	strTitle += TOOL_VER + _T("-beta");
	SetWindowText( (LPCTSTR)strTitle );
}
void CMainFrame::OnSprNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->Destroy();			// 로드하기전에 툴내에 sprobj를 파괴
	TOOL->Init();					// 툴 정보 초기화
	TOOL->Create();				// sprobj 생성
	TOOL->SetstrFilename( CString() );
	GetThumbView()->Update();
	GetAdjustView()->Update();
	GetFrameView()->Update();
	GetActionListView()->Update();
	SetTitle();

}

void CMainFrame::OnMoveMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetnEditMode( EM_MOVE );
}

void CMainFrame::OnUpdateMoveMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetnEditMode() == EM_MOVE )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnRotateMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetnEditMode( EM_ROTATE );
}

void CMainFrame::OnUpdateRotateMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetnEditMode() == EM_ROTATE )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnScaleMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetnEditMode( EM_SCALE );
}

void CMainFrame::OnUpdateScaleMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetnEditMode() == EM_SCALE )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

HRESULT CMainFrame::Create( HWND hwndRenderWindow )
{
    // Register a class for a fullscreen window
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, CW_USEDEFAULT,
                                           CW_USEDEFAULT, 100, 100,
										   CWnd::GetSafeHwnd(), 0L, NULL, 0L );
//    CD3DApplication::m_hWnd = m_hwndRenderWindow = hwndRenderWindow;
    CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
    HRESULT hr = CD3DApplication::Create( AfxGetInstanceHandle() );

//	g_Grp2D.SetDevice( GetDevice() );
	GRAPHICS_D3DTOOL->Initialize( GetDevice() );
//	hr = GRAPHICS_D3DTOOL->CreateFont(15);
//	SE::g_pFont = new XFontDX( _T("맑은 고딕"), 15.f );
	SE::g_pFDBasic = new XFontDatDX( _T("맑은 고딕"), 13.f );
	SE::g_pFont = SE::g_pFDBasic->CreateFontObj();
	D3DXMATRIXA16 mView;
	D3DXMatrixIdentity( &mView );
	GetDevice()->SetTransform( D3DTS_VIEW, &mView );
	// Turn off culling
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// Turn off D3D lighting
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// Turn on the zbuffer
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD dwTextureCaps = CD3DApplication::m_d3dSettings.PDeviceInfo()->Caps.TextureCaps;
	XSurfaceD3D::Initialize( dwTextureCaps );

	LPD3DXBUFFER pCompileError;
	LPD3DXEFFECT pEffect;
	hr = D3DXCreateEffectFromResource( m_pd3dDevice, NULL, MAKEINTRESOURCE(IDR_FX1), NULL, NULL, 0, NULL, &pEffect, &pCompileError );
    if (FAILED(hr))
    {
		if( pCompileError )
		{
			LPCTSTR str = (LPCTSTR)pCompileError->GetBufferPointer();
			CString strErr = str;
			XERROR( "Failed to load effect file\r\n%s", (LPCTSTR)strErr );
		} else
		{
			XERROR( "Failed to load effect file\r\n" );
		}
    } 
	XE::Init( m_pd3dDevice, 0, 0, pEffect );

//	m_pSurface = new XSurfaceD3D( GetDevice(), _T("test2.jpg") );

	return hr;
}
HRESULT CMainFrame::InitDeviceObjects()
{ return S_OK; }
HRESULT CMainFrame::DeleteDeviceObjects()                      
{ return S_OK; }

HRESULT CMainFrame::RestoreDeviceObjects()
{ 
	return S_OK; 
}
HRESULT CMainFrame::OneTimeSceneInit()
{
	HRESULT hr = S_OK;

    return hr;
}


void CMainFrame::OnPrevFrame()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SPROBJ->IsPause() )
	{
		float fFrame = SPROBJ->GetFrmCurr() - spAction->GetSpeed();		// 한프레임 앞으로
		if( fFrame < 0 )
			fFrame = spAction->GetfMaxFrame() - spAction->GetSpeed();
		fFrame = ROUND_FLOAT( fFrame, spAction->GetSpeed() );		// speed단위로 반올림
//		SPROBJ->MoveFrame( fFrame );
		GetFrameView()->MoveFrame( fFrame );
		GetAnimationView()->Update();
		GetFrameView()->Update();
	}
}

void CMainFrame::OnNextFrame()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SPROBJ->IsPause() )
	{
		float fFrame = SPROBJ->GetFrmCurr() + spAction->GetSpeed();		// 한프레임 뒤로
		if( fFrame >= spAction->GetfMaxFrame() )
			fFrame = 0;
		fFrame = ROUND_FLOAT( fFrame, spAction->GetSpeed() );		// speed단위로 반올림
// 		SPROBJ->MoveFrame( fFrame );
		GetFrameView()->MoveFrame( fFrame );
		GetAnimationView()->Update();
		GetFrameView()->Update();
		CWndTimeline::sGet()->Invalidate( FALSE );
	}
}

void CMainFrame::OnUpdatePrevFrame(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) 
	{
		if( SPROBJ->IsPause() )
			pCmdUI->Enable( 1 );
		else
			pCmdUI->Enable( 0 );
	} else
		pCmdUI->Enable( 0 );
}

void CMainFrame::OnUpdateNextFrame(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) 
	{
		if( SPROBJ->IsPause() )
			pCmdUI->Enable( 1 );
		else
			pCmdUI->Enable( 0 );
	} else
		pCmdUI->Enable( 0 );
}

void CMainFrame::OnLockAxisx()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetAxisLock( AL_X );
}

void CMainFrame::OnLockAxisy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetAxisLock( AL_Y );
}

void CMainFrame::OnLockAxisxy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetAxisLock( AL_XY );
}

void CMainFrame::OnUpdateLockAxisx(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetAxisLock() == AL_X )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnUpdateLockAxisy(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetAxisLock() == AL_Y )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnUpdateLockAxisxy(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if( TOOL->GetAxisLock() == AL_XY )
		pCmdUI->SetCheck( 1 );
	else 
		pCmdUI->SetCheck( 0 );
}

void CMainFrame::OnOpenContImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	CFileDialog		dlg( true, _T("png"), NULL, OFN_FILEMUSTEXIST,
//					 	_T("PNG Files(*.png)|*.png|AllFiles(*.*)|*.*||"), NULL );
	CFileDialog		dlg( true, NULL, NULL, OFN_FILEMUSTEXIST,
					 	_T("image Files(*.png, *.tif) |*.png;*.tif|AllFiles(*.*)|*.*||"), NULL );	// ;하고 뒤에 띄우면 안되는군

	if ( dlg.DoModal() == IDOK )
	{
		CString strFullPath = dlg.GetPathName();
		TOOL->CutContinuosImage( strFullPath );
		if( 1 )
			GetConsoleView()->Message( _T("%s 변환성공."), strFullPath );
		else
			GetConsoleView()->Message( _T("%s 변환실패."), strFullPath );
		GetThumbView()->Update();
		GetAdjustView()->Update();
	}
}

void CMainFrame::OnEditUndo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( UNDOMNG->Undo() )
	{
		TOOL->GetSelectKey().Clear();		// 일단 키셀렉트 시킨건 언두가 안되니 셀렉트키는 모두 지워버리자
		TOOL->GetSelToolTipKey().Clear();
		TOOL->SetspSelLayer( NULL );
		SPROBJ->MoveFrame();
		ALL_VIEW_UPDATE();
	}
}

void CMainFrame::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	UNDO_PACK *pUndoPack = UNDOMNG->UndoTop();
	if( pUndoPack ) 
	{
		if( !pUndoPack->stackUndo.empty() ) 
		{
			XBaseUndo *pUndo = pUndoPack->stackUndo.top();
			pCmdUI->SetText( CString(_T("실행 취소(Undo) - ")) + pUndo->XBaseUndo::GetText() );
		}
		else
			pCmdUI->Enable(0);
	} else {
		pCmdUI->SetText( CString(_T("실행 취소(Undo)")) );
		pCmdUI->Enable(0);
	}
}

void CMainFrame::OnEditRedo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( UNDOMNG->Redo() )
	{
		TOOL->GetSelectKey().Clear();		// 일단 키셀렉트 시킨건 언두가 안되니 셀렉트키는 모두 지워버리자
		TOOL->GetSelToolTipKey().Clear();
		SPROBJ->MoveFrame();
		ALL_VIEW_UPDATE();
	}
}

void CMainFrame::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	UNDO_PACK *pRedoPack = UNDOMNG->RedoTop();
	if( pRedoPack ) 
	{
		if( !pRedoPack->stackUndo.empty() ) 
		{
			XBaseUndo *pRedo = pRedoPack->stackUndo.top();
			pCmdUI->SetText( CString(_T("다시 실행(Redo) - ")) + pRedo->XBaseUndo::GetText() );
		}
		else
			pCmdUI->Enable(0);
	} else {
		pCmdUI->SetText( CString(_T("다시 실행(Redo)")) );
		pCmdUI->Enable(0);
	}
}

void CMainFrame::OnOpenAlignedImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog		dlg( true, _T("png"), NULL, OFN_FILEMUSTEXIST,
					 	_T("image Files(*.png, *.tif)|*.png;*.tif|AllFiles(*.*)|*.*||"), NULL );

	if ( dlg.DoModal() == IDOK )
	{
		CString strFullPath = dlg.GetPathName();
		if( TOOL->CutAlignedImage( strFullPath ) == FALSE )
			GetConsoleView()->Message( _T("%s 읽기실패."), strFullPath );
		GetThumbView()->Update();
		GetAdjustView()->Update();
	}
}

void CMainFrame::OnUpdateOpenAlignedImage(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifdef _DEBUG
	pCmdUI->Enable( 1 );
#else
	pCmdUI->Enable( 0 );
#endif
}

void CMainFrame::OnScaleView1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetAdjustView()->SetScaleView( 1.0f );
}

void CMainFrame::OnScaleView2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetAdjustView()->SetScaleView( 2.0f );
}

void CMainFrame::OnScaleView3()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetAdjustView()->SetScaleView( 3.0f );
}

void CMainFrame::OnScaleView4()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetAdjustView()->SetScaleView( 4.0f );
}

void CMainFrame::OnFrameskip()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetbFrameSkip( ! TOOL->GetbFrameSkip() );
}

void CMainFrame::OnUpdateFrameskip(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
		pCmdUI->SetCheck( TOOL->GetbFrameSkip() );
}

void CMainFrame::OnConvert()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XDlgConvert dlg;
	if ( dlg.DoModal() == IDOK )
	{
	}
}

void CMainFrame::OnBnClickedButtAddLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 메인프레임에 빈메시지 처리기라도 있어야 버튼이 ENABLE되더라.
}


void CMainFrame::OnBnClickedButtCreateobj()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 메인프레임에 빈메시지 처리기라도 있어야 버튼이 ENABLE되더라.
}

void CMainFrame::OnAddLayerImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	TOOL->AddLayerImage();
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( spActObj ) {
		auto spLayer = spActObj->AddLayerAuto<XLayerImage>();
		TOOL->SetspSelLayer( spLayer );
		GetFrameView()->UpdateOfsYBySelectedLayer();
	}
// 	XSPAction spAction = SPROBJ->GetspAction();
// 	if( spAction == NULL )	return;
// 	auto spLayer = spAction->AddImgLayer();
// 	TOOL->SetspSelLayer( spLayer );		// 새로생긴 레이어를 자동으로 선택해준다
// 	GetFrameView()->Update();
}

void CMainFrame::OnAddLayerObject()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	TOOL->AddLayerObject();
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( spActObj ) {
		auto spLayer = spActObj->AddLayerAuto<XLayerObject>();
		TOOL->SetspSelLayer( spLayer );
		GetFrameView()->UpdateOfsYBySelectedLayer();
	}
// 	XSPAction spAction = SPROBJ->GetspAction();
// 	if( spAction == NULL )	return;
// 	auto spLayer = spAction->AddObjLayer();
// 	TOOL->SetspSelLayer( spLayer );		// 새로생긴 레이어를 자동으로 선택해준다
// 	GetFrameView()->Update();
}


void CMainFrame::OnCompConverter()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnLoadBg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog		dlg( true, _T("png"), NULL, OFN_FILEMUSTEXIST,
					 	_T("png Files(*.png)|*.png|AllFiles(*.*)|*.*||"), NULL );

	if ( dlg.DoModal() == IDOK )
	{
		CString strFullPath = dlg.GetPathName();
		TOOL->LoadBg( (LPCTSTR)strFullPath );
	}
}

void CMainFrame::OnModeMoveByBg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetAnimationView()->TogglebModeBackBg();
}

void CMainFrame::OnUpdateModeMoveByBg( CCmdUI *pCmdUI )
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( GetAnimationView()->GetbModeBackBg() );
}

