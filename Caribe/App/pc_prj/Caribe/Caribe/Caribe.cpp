
// Caribe.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
// 

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Caribe.h"
#include "MainFrm.h"

#include "CaribeDoc.h"
#include "CaribeView.h"
//#include "_DirectX/XGraphicsD3DTool.h"
#include "OpenGL2/XGraphicsOpenGL.h"
#include "client/XAppMain.h"
#include "XFramework/client/XEContent.h"
#include "CrashDlg.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


// CaribeApp

BEGIN_MESSAGE_MAP(CaribeApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CaribeApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CaribeApp 생성

CaribeApp::CaribeApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Caribe.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CaribeApp 개체입니다.

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
	while ( nullptr != szStr )
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


CaribeApp theApp;

// CaribeApp 초기화

BOOL CaribeApp::InitInstance()
{
#ifndef DEBUG
	BOOL bRet = SetCrashHandlerFilter ( &TheCrashHandlerFunction ) ;
	if( bRet != TRUE )
		XLOG( "경고:에러검출 시스템이 활성화되지 않았습니다. return code:%d", (int)bRet );
	else
		XLOG( "에러검출 시스템이 활성화되었습니다" );
#endif
	// 게임의 논리적 해상도를 지정
	XE::SetLogicalGameSize( (float)XRESO_WIDTH, (float)XRESO_HEIGHT );
	XE::VEC2 sizeLogReso = 
		XClientMain::sCalcLogicalResolution( XE::VEC2(PHY_WIDTH, PHY_HEIGHT), 
												XGAME_SIZE );

#if defined(_VER_OPENGL)
	auto pGraphicsGL = new XGraphicsOpenGL( (int)sizeLogReso.w, (int)sizeLogReso.h, xPIXELFORMAT_RGB565 );
	pGraphicsGL->SetPhyScreenSize( PHY_WIDTH, PHY_HEIGHT );
	GRAPHICS = pGraphicsGL;
#elif defined(_VER_DX)// _VER_OPENGL
	#ifdef _XLIBPNG
		GRAPHICS_D3D = new XGraphicsD3D( (int)sizeLogReso.w, (int)sizeLogReso.h, xPIXELFORMAT_RGB565 );		// DirectX초기화때문에 MainFrame이 생성되기 전에 생성되어야 한다
		GRAPHICS = GRAPHICS_D3D;
	#else
		GRAPHICS_D3DTOOL = new XGraphicsD3DTool( (int)sizeLogReso.w, (int)sizeLogReso.h, xPIXELFORMAT_RGB565 );		// DirectX초기화때문에 MainFrame이 생성되기 전에 생성되어야 한다
		GRAPHICS = GRAPHICS_D3DTOOL;
	#endif
#endif // DX
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CaribeDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CaribeView));
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
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	CMainFrame *pMainfrm = (CMainFrame*)m_pMainWnd;
	pMainfrm->SetTitle();
	return TRUE;
}

int CaribeApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);
	TRACE("CaribeApp:ExitInstance\n");
	return CWinApp::ExitInstance();
}

// CaribeApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CaribeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CaribeApp 메시지 처리기

BOOL CaribeApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( GetView() ) {
		GetView()->Draw();
// 		CWinApp::OnIdle(lCount);
// 		return TRUE;
	} else {
		exit(1);
	}
// 	else
// 		return CWinApp::OnIdle( lCount );
	auto bOk = CWinApp::OnIdle(lCount);
	return TRUE;

// 	return CWinApp::OnIdle(lCount);
}


