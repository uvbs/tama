
// Bot.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Bot.h"
#include "MainFrm.h"
#include "XMain.h"

#include "BotDoc.h"
#include "BotView.h"
// #include "BugslayerUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBotApp

BEGIN_MESSAGE_MAP(CBotApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CBotApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CBotApp ����
// #ifndef DEBUG
// LONG __stdcall TheCrashHandlerFunction( EXCEPTION_POINTERS * pExPtrs )
// {
// 	FILE *fp;
// 	_tfopen_s( &fp, _T( "error.txt" ), _T( "at" ) );
// 	xPutsTimeString( fp );
// 	pExPtrs = pExPtrs;
// 
// 	LPCTSTR szStr = GetFaultReason( pExPtrs );
// 	_fputts( _T( "FaultReason\n\n" ), fp );
// 	if( szStr )
// 		_fputts( szStr, fp );
// 	_fputts( _T( "\n" ), fp );
// 
// 	szStr = GetRegisterString( pExPtrs );
// 	_fputts( _T( "Register\n" ), fp );
// 	if( szStr )
// 		_fputts( szStr, fp );
// 	_fputts( _T( "\n" ), fp );
// 
// 	szStr = GetFirstStackTraceString( GSTSO_MODULE |
// 		GSTSO_SYMBOL |
// 		GSTSO_SRCLINE,
// 		pExPtrs );
// 	if( szStr )
// 		_fputts( szStr, fp );
// 	_fputts( _T( "\n" ), fp );
// 	while( nullptr != szStr ) {
// 		szStr = GetNextStackTraceString( GSTSO_MODULE |
// 			GSTSO_SYMBOL |
// 			GSTSO_SRCLINE,
// 			pExPtrs );
// 		if( szStr )
// 			_fputts( szStr, fp );
// 		_fputts( _T( "\n" ), fp );
// 	}
// 
// 	fclose( fp );
// 	//    LONG lReturnVal = EXCEPTION_EXECUTE_HANDLER ;
// 
// // 	CCrashDlg Dlg;
// // 	Dlg.SetExceptionPointers( pExPtrs );
// 
// 	LONG lReturnVal = EXCEPTION_EXECUTE_HANDLER;
// 
// // 	INT_PTR iRet = Dlg.DoModal();
// // 
// // 	if( IDOK == iRet ) {
// // 		lReturnVal = EXCEPTION_EXECUTE_HANDLER;
// // 	} else {
// // 		lReturnVal = EXCEPTION_CONTINUE_SEARCH;
// // 	}
// 
// 	return ( lReturnVal );
// }
// #endif // not debug

CBotApp::CBotApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Bot.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CBotApp ��ü�Դϴ�.

CBotApp theApp;


// CBotApp �ʱ�ȭ

BOOL CBotApp::InitInstance()
{
// #ifndef DEBUG
// 	BOOL bRet = SetCrashHandlerFilter( &TheCrashHandlerFunction );
// 	if( bRet != TRUE )
// 		XLOG( "���:�������� �ý����� Ȱ��ȭ���� �ʾҽ��ϴ�. return code:%d", (int)bRet );
// 	else
// 		XLOG( "�������� �ý����� Ȱ��ȭ�Ǿ����ϴ�" );
// #endif
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBotDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CBotView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//
	XMain::sGet()->Create();

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CBotApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CBotApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CBotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CBotApp �޽��� ó����
BOOL CBotApp::OnIdle(LONG lCount)
{
	if( CBotView::sGet() )
		CBotView::sGet()->OnDraw( nullptr );
	CWinApp::OnIdle(lCount);
	return TRUE;
}


