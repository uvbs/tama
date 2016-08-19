// XDlgConsole.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "game.h"
#include "XDlgConsole.h"
#include "MainFrm.h"

// XDlgConsole ��ȭ �����Դϴ�.
static XDlgConsole *s_pDlgConsole = NULL;
XDlgConsole *GetDlgConsole( void ) { return s_pDlgConsole; }

IMPLEMENT_DYNAMIC(XDlgConsole, CDialog)

XDlgConsole::XDlgConsole(CWnd* pParent /*=NULL*/)
	: CDialog(XDlgConsole::IDD, pParent)
	, m_strConsole(_T(""))
{
	s_pDlgConsole = this;
}

XDlgConsole::~XDlgConsole()
{
}

void XDlgConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strConsole);
}


BEGIN_MESSAGE_MAP(XDlgConsole, CDialog)
END_MESSAGE_MAP()


// XDlgConsole �޽��� ó�����Դϴ�.

BOOL XDlgConsole::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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

	UpdateData(TRUE);		// ��Ʈ�ѿ��� ����
	m_strConsole += s;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// �������� ��Ʈ��
	::OutputDebugString( m_strConsole );		// TRACE�� �ѱ������ �ȵǼ� �̰ɷ� �ٲ�

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
	*/
}

void XDlgConsole::MessageString( LPCTSTR szString )
{
	if( ::IsWindow(GetSafeHwnd()) == FALSE )		return;
	UpdateData(TRUE);		// ��Ʈ�ѿ��� ����
	CString strString = szString;
	strString += _T("\r\n");
	m_strConsole += strString;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// �������� ��Ʈ��
	
//	_tcscat_s( buff, _T("\n") );
	::OutputDebugString( strString );		// TRACE�� �ѱ������ �ȵǼ� �̰ɷ� �ٲ�

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
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
