
// SE.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "SE.h"
#include "MainFrm.h"

#include "SEDoc.h"
#include "SEView.h"
#include "AnimationView.h"
#include "XViewSpline.h"
#include "OpenAL\\XSoundMngOpenAL.h"
#include "CrashDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSEApp

BEGIN_MESSAGE_MAP(CSEApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSEApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSEApp 생성

CSEApp::CSEApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}
CSEApp::~CSEApp()
{
	SAFE_DELETE( GRAPHICS_D3DTOOL );
	SAFE_DELETE( SOUNDMNG );
}


// 유일한 CSEApp 개체입니다.

CSEApp theApp;

#ifndef DEBUG
LONG __stdcall TheCrashHandlerFunction ( EXCEPTION_POINTERS * pExPtrs )
{
	FILE *fp;
	_tfopen_s( &fp, _T("error.txt"), _T("at") );
	xPutsTimeString( fp );
    pExPtrs = pExPtrs ;
    
    LPCTSTR szStr = GetFaultReason ( pExPtrs ) ;
	_fputts( _T("FaultReason\n\n"), fp );
    if( szStr )
		_fputts( szStr, fp );
	_fputts( _T("\n"), fp );

	szStr = GetRegisterString ( pExPtrs ) ;
	_fputts( _T("Register\n"), fp );
	if( szStr )
		_fputts( szStr, fp );
	_fputts( _T("\n"), fp );
        
    szStr = GetFirstStackTraceString ( GSTSO_MODULE     |
                                        GSTSO_SYMBOL    |
                                        GSTSO_SRCLINE    ,
                                       pExPtrs        ) ;
	if( szStr )
	    _fputts( szStr, fp );
	_fputts( _T("\n"), fp );
    while ( NULL != szStr )
    {
        szStr = GetNextStackTraceString ( GSTSO_MODULE     |
                                            GSTSO_SYMBOL   |
                                            GSTSO_SRCLINE   ,
                                          pExPtrs        ) ;
		if( szStr )
			_fputts( szStr, fp );
		_fputts( _T("\n"), fp );
    }
	
	fclose(fp);
//    LONG lReturnVal = EXCEPTION_EXECUTE_HANDLER ;

    CCrashDlg Dlg ;
    Dlg.SetExceptionPointers ( pExPtrs ) ;
    
    LONG lReturnVal = EXCEPTION_EXECUTE_HANDLER ;
    
    INT_PTR iRet = Dlg.DoModal ( ) ;
    
    if ( IDOK == iRet )
    {
        lReturnVal = EXCEPTION_EXECUTE_HANDLER ;
    }
    else
    {
        lReturnVal = EXCEPTION_CONTINUE_SEARCH ;
    }

    return ( lReturnVal ) ;
}
#endif // not debug

// CSEApp 초기화
#include "xuzhukey.h"
BOOL CSEApp::InitInstance()
{
#ifndef DEBUG
    BOOL bRet = SetCrashHandlerFilter ( &TheCrashHandlerFunction ) ;
	if( bRet != TRUE )
		XLOG( "경고:에러검출 시스템이 활성화되지 않았습니다. return code:%d", (int)bRet );
	else
		XLOG( "에러검출 시스템이 활성화되었습니다" );
#endif
	// 커맨드 라인 읽음
//	CONSOLE( "%s", m_lpCmdLine );
	if( XE::IsHave( m_lpCmdLine ) )
	{
		TCHAR szSpr[1024];
		_tcscpy_s( szSpr, m_lpCmdLine+1 );		// 처음 따옴표 뗌
		szSpr[ _tcslen( szSpr )-1 ] = 0;	// 끝에 따옴표 뗌
//		memcpy( szSpr, m_lpCmdLine+1, _tcslen( m_lpCmdLine ) - 2 );	// "test.spr"에서 따옴표 떼냄 
//		CONSOLE( "%s %d", szSpr, _tcslen( m_lpCmdLine ) );
		GetLongPathName( szSpr, szSpr, MAX_PATH );
		CONSOLE( "command line:%s", szSpr );
		TOOL->SetstrCmdLine( CString( szSpr ) );
	}
//	XE::SetWorkDir();
	GRAPHICS_D3DTOOL = new XGraphicsD3DTool( 100, 100, xPIXELFORMAT_RGBA8 );
	xSRand( timeGetTime() );
	
	SOUNDMNG = new XSoundMngOpenAL;

	/*if( freopen("c:/stderr.txt", "wt", stderr ) != NULL )
	{
		printf("stderr file open success");
	}*/

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	CleanState();
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSEDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CAnimationView));
//		RUNTIME_CLASS(CSEView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->SetTitle();
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	TOOL->LoadINI();
	// 툴 초기화
#ifdef NDEBUG
//	_InitApp( m_pMainWnd->GetSafeHwnd() );
#endif
	
	// 커맨드 라인으로 실행됬으면 spr 읽음
//	CONSOLE("hello");
//	TOOL->SetstrCmdLine( _T("C:\\Project\\Work\\iPhone\\MAC\\civ defense\\Resource\\spr\\eff_Lily.spr") );
	if( TOOL->GetstrCmdLine().IsEmpty() == FALSE )
	{
		CONSOLE("Load cmdline spr: %s", TOOL->GetstrCmdLine() );
		TOOL->LoadSpr( TOOL->GetstrCmdLine() );	
	}
	if( !TOOL->GetpathBg().empty() ) {
		TOOL->LoadBg( TOOL->GetpathBg().c_str() );
	}


	return TRUE;
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CSEApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSEApp 사용자 지정 로드/저장 메서드

void CSEApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
//	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CSEApp::LoadCustomState()
{
}

void CSEApp::SaveCustomState()
{
}

// CSEApp 메시지 처리기



BOOL CSEApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( GetAnimationView() )
		GetAnimationView()->Draw();
	if( GetViewSpline() )
		GetViewSpline()->Draw();
	CWinAppEx::OnIdle(lCount);
	return TRUE;
}

int CSEApp::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinAppEx::Run();
}
