// XDlgConsole.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "game.h"
#include "XDlgConsole.h"
#include "MainFrm.h"

// XDlgConsole 대화 상자입니다.
static XDlgConsole *s_pDlgConsole = NULL;
XDlgConsole *GetDlgConsole( void ) { return s_pDlgConsole; }

IMPLEMENT_DYNAMIC(XDlgConsole, CDialog)

XDlgConsole::XDlgConsole(CWnd* pParent /*=NULL*/)
	: CDialog(XDlgConsole::IDD, pParent)
	, m_strConsole(_T(""))
{
	s_pDlgConsole = this;
	InitializeCriticalSection( &m_cs );
}

XDlgConsole::~XDlgConsole()
{
	DeleteCriticalSection( &m_cs );
}

void XDlgConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strConsole);
}


BEGIN_MESSAGE_MAP(XDlgConsole, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// XDlgConsole 메시지 처리기입니다.

BOOL XDlgConsole::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	GetClientRect( &rect );
	if( AfxGetMainWnd() )
	{
//	if( g_pMainFrm ) {
//		CRect rectMain;
//		g_pMainFrm->GetClientRect( &rectMain );
//		MoveWindow( rectMain.left, rectMain.bottom, rectMain.Width(), rect.Height() );
		int scrW = GetSystemMetrics( SM_CXSCREEN );
		int scrH = GetSystemMetrics( SM_CYSCREEN );
		MoveWindow( 0, scrH-rect.Height(), rect.Width(), rect.Height() );
	}
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->MoveWindow( &rect ); 	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void XDlgConsole::Message( const char *cFormat, ... )
{
	if( ::IsWindow(GetSafeHwnd()) == FALSE )		return;
	char buff[1024];
	va_list	vl;

    va_start(vl, cFormat);
    vsprintf_s(buff, 256, cFormat, vl);
    va_end(vl);
	MessageString( Convert_char_To_TCHAR( buff ) );

/*	CString s;
	s.Format( _T("%s\r\n"), Convert_char_To_TCHAR(buff) );

	UpdateData(TRUE);		// 컨트롤에서 변수
	m_strConsole += s;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// 변수에서 컨트롤
	::OutputDebugString( m_strConsole );		// TRACE로 한글출력이 안되서 이걸로 바꿈

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
	*/
}

void XDlgConsole::MessageString( LPCTSTR szString )
{
	EnterCriticalSection( &m_cs );
	if( ::IsWindow(GetSafeHwnd()) == FALSE )		return;
	UpdateData(TRUE);		// 컨트롤에서 변수
	CString strString = szString;
	strString += _T("\r\n");
	m_strConsole += strString;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// 변수에서 컨트롤
	
//	_tcscat_s( buff, _T("\n") );
//	::OutputDebugString( strString );		// TRACE로 한글출력이 안되서 이걸로 바꿈

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
	LeaveCriticalSection( &m_cs );
}

void	XDlgConsole::Message( LPCTSTR sz, ... )
{
	TCHAR buff[1024];
	va_list	vl;
    va_start(vl, sz);
    _vstprintf_s(buff, 512, sz, vl);
    va_end(vl);

	MessageString( buff );

//	CString s;
//	s.Format( _T("%s\r\n"), buff );

}


void XDlgConsole::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CWnd *pEdit = GetDlgItem( IDC_EDIT1 );
	if( pEdit )
	{
		pEdit->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE );
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
