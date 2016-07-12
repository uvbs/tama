
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "GameServer.h"

#include "MainFrm.h"
#include "XMain.h"
#include "XSocketSvrForClient.h"
#include "XGameUserMng.h"
#include "XLegion.h"
#include "XPropLegion.h"
#include "XResObj.h"
#include "server/XSAccount.h"
#include "XLoginConnection.h"
#include "XDBASvrConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND( IDM_SAVE, &CMainFrame::OnSave )
	ON_COMMAND( IDM_CREATE_POWER_TABLE, &CMainFrame::OnCreatePowerTable )
	ON_COMMAND( IDM_RELOAD, &CMainFrame::OnReload )
	ON_COMMAND( IDM_RELOAD_INI, &CMainFrame::OnReloadIni )
	ON_COMMAND( IDM_RELOAD_TAGS, &CMainFrame::OnReloadTags )
	ON_COMMAND( IDM_RELOAD_IDACC, &CMainFrame::OnReloadidAcc )
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
	: XEServerMainFrame( _T("GameServer") )
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


void CMainFrame::OnSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( MAIN->GetpSocketForClient() )
		MAIN->GetpSocketForClient()->GetpUserMng()->Save();
}


void CMainFrame::OnCreatePowerTable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	{
		XResFile res;
		if( res.OpenForWrite( _T( "npc_power.txt" ) ) ) {
			TCHAR bom = 0xFEFF;
			res.Write( &bom, 2 );
			for( int lv = 1; lv <= 50; ++lv ) {
				XGAME::xLegionParam info;
				auto pLegion = XLegion::sCreateLegionForNPC( lv, 0, info );
				auto spLegion = LegionPtr( pLegion );
				int power = XLegion::sGetMilitaryPower( spLegion );
				auto szStr = XFORMAT( "%d\t%d\r\n", lv, power );
				res.Write( (void*)szStr, sizeof( TCHAR ), _tcslen( szStr ) );
			}
		}
	}
	CONSOLE("npc군단 전투력 테이블 생성 완료......................win32/npc_power.txt");
	//////////////////////////////////////////////////////////////////////////
	{
		XResFile res;
		if( res.OpenForWrite( _T( "user_max_power.txt" ) ) ) {
			TCHAR bom = 0xFEFF;
			res.Write( &bom, 2 );
			for( int lv = 1; lv <= 50; ++lv ) {
				int power = XLegion::sGetMilitaryPowerMax( lv );
				auto szStr = XFORMAT( "%d    %d        %d\r\n", lv, 
											XAccount::sGetMaxTechPoint(lv), 
											power );
				res.Write( (void*)szStr, sizeof( TCHAR ), _tcslen( szStr ) );
			}
		}
	}
	CONSOLE( "유저 최대 전투력 테이블 생성 완료......................win32/user_max_power.txt" );
}

void CMainFrame::OnReload()
{
	XPropLegion::sGet()->DestroyAll();
	if( !XPropLegion::sGet()->Load( _T( "propLegion.xml" ) ) )
		CONSOLE("reload failed.....propLegion.xml");
}

void CMainFrame::OnReloadIni()
{
	MAIN->ReloadIni();
}

CString CMainFrame::GetstrTitle() const
{
	auto str = XEServerMainFrame::GetstrTitle();
	str += _T( "-" );
	str += XGAME::GetstrDEV_LEVEL();
	return str;
}

void CMainFrame::OnReloadTags()
{
	if( CONSOLE_MAIN )
		CONSOLE_MAIN->LoadTags();
	XEServerMain::sGet()->ShowLogFilter( "tags" );
	if( LOGIN_SVR )
		LOGIN_SVR->SendReload( "tags" );
	if( DBA_SVR )
		DBA_SVR->SendReload( "tags" );
}

void CMainFrame::OnReloadidAcc()
{
	if( CONSOLE_MAIN )
		CONSOLE_MAIN->LoadidAcc();
	XEServerMain::sGet()->ShowLogFilter( "idacc" );
	if( LOGIN_SVR )
		LOGIN_SVR->SendReload( "idacc" );
	if( DBA_SVR )
		DBA_SVR->SendReload( "idacc" );
}
