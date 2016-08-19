// ConsoleView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "ConsoleView.h"

// CConsoleView
static CConsoleView *s_pConsoleView = nullptr;
CConsoleView *GetConsoleView() { return s_pConsoleView; }

IMPLEMENT_DYNCREATE(CConsoleView, CFormView)

CConsoleView::CConsoleView()
	: CFormView(CConsoleView::IDD)
	, m_strConsole(_T(""))
{
	s_pConsoleView = this;
}

CConsoleView::~CConsoleView()
{
	s_pConsoleView = nullptr;
}
CConsoleView *CConsoleView::CreateOne( CWnd *pParent )
{
	CConsoleView *pView = new CConsoleView;
	CCreateContext *pContext = nullptr;

#if 0
	if( !pView->CreateEx(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0,0,205,157),
		pParent, -1, pContext ) )
#else
	if (!pView->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 500), pParent, 0, pContext))
#endif
	//if( !pFormView->CreateEx( 0, AfxRegisterWndClass(0, 0, 0, 0), nullptr,
	//	WS_CHILD | WS_VISIBLE, CRect( 0, 0, 205, 157), pParent, -1, pContext) )
	{
		AfxMessageBox( _T("Failed in creating CThumbView") );
	}

	pView->OnInitialUpdate();

	return pView;
}

void CConsoleView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strConsole);
}

BEGIN_MESSAGE_MAP(CConsoleView, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CConsoleView 진단입니다.

#ifdef _DEBUG
void CConsoleView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CConsoleView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CConsoleView 메시지 처리기입니다.
void CConsoleView::Message( const char *cFormat, ... )
{
	if( ::IsWindow(GetSafeHwnd()) == FALSE )		return;
	char buff[4096];
	va_list	vl;

    va_start(vl, cFormat);
    vsprintf_s(buff, 256, cFormat, vl);
    va_end(vl);

	CString s;
	UpdateData(TRUE);		// 컨트롤에서 변수
	s.Format( _T("%s\r\n"), Convert_char_To_TCHAR(buff) );
	m_strConsole += s;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// 변수에서 컨트롤
	::OutputDebugString( m_strConsole );		// TRACE로 한글출력이 안되서 이걸로 바꿈

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
}

void CConsoleView::MessageString( LPCTSTR sz )
{
	UpdateData(TRUE);		// 컨트롤에서 변수
	m_strConsole += sz;
	m_strConsole += _T("\r\n");
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// 변수에서 컨트롤
	::OutputDebugString( m_strConsole );		// TRACE로 한글출력이 안되서 이걸로 바꿈

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
}

void	CConsoleView::Message( LPCTSTR sz, ... )
{
	TCHAR buff[4096];
	va_list	vl;

    va_start(vl, sz);
    _vstprintf_s(buff, sz, vl);
    va_end(vl);

	MessageString( buff );

}

void CConsoleView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	RECT rect;
	GetClientRect( &rect );
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->MoveWindow( &rect ); 	
	{
		CSize size;
		size.cx = 0;
		size.cy = 0;
		SetScrollSizes( MM_TEXT, size );
	}
}
void CConsoleView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CWnd *pEdit = GetDlgItem( IDC_EDIT1 );
	if( pEdit )
		pEdit->MoveWindow( 0, 0, cx, cy );
}

