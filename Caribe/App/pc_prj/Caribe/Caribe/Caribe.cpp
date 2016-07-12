
// Caribe.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CaribeApp ����

CaribeApp::CaribeApp()
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
	SetAppID(_T("Caribe.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CaribeApp ��ü�Դϴ�.

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

// CaribeApp �ʱ�ȭ

BOOL CaribeApp::InitInstance()
{
#ifndef DEBUG
	BOOL bRet = SetCrashHandlerFilter ( &TheCrashHandlerFunction ) ;
	if( bRet != TRUE )
		XLOG( "���:�������� �ý����� Ȱ��ȭ���� �ʾҽ��ϴ�. return code:%d", (int)bRet );
	else
		XLOG( "�������� �ý����� Ȱ��ȭ�Ǿ����ϴ�" );
#endif
	// ������ ���� �ػ󵵸� ����
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
		GRAPHICS_D3D = new XGraphicsD3D( (int)sizeLogReso.w, (int)sizeLogReso.h, xPIXELFORMAT_RGB565 );		// DirectX�ʱ�ȭ������ MainFrame�� �����Ǳ� ���� �����Ǿ�� �Ѵ�
		GRAPHICS = GRAPHICS_D3D;
	#else
		GRAPHICS_D3DTOOL = new XGraphicsD3DTool( (int)sizeLogReso.w, (int)sizeLogReso.h, xPIXELFORMAT_RGB565 );		// DirectX�ʱ�ȭ������ MainFrame�� �����Ǳ� ���� �����Ǿ�� �Ѵ�
		GRAPHICS = GRAPHICS_D3DTOOL;
	#endif
#endif // DX
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


	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CaribeDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CaribeView));
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

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  SDI ���� ���α׷������� ProcessShellCommand �Ŀ� �̷��� ȣ���� �߻��ؾ� �մϴ�.
	CMainFrame *pMainfrm = (CMainFrame*)m_pMainWnd;
	pMainfrm->SetTitle();
	return TRUE;
}

int CaribeApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);
	TRACE("CaribeApp:ExitInstance\n");
	return CWinApp::ExitInstance();
}

// CaribeApp �޽��� ó����


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
void CaribeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CaribeApp �޽��� ó����

BOOL CaribeApp::OnIdle(LONG lCount)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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


