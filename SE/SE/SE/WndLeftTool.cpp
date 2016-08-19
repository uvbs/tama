// WndLeftTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndLeftTool.h"

static CWndLeftTool *s_pWndLeftTool = NULL;
CWndLeftTool *GetWndLeftTool() { return s_pWndLeftTool; }

// CWndLeftTool

IMPLEMENT_DYNAMIC(CWndLeftTool, CDockablePane)

CWndLeftTool::CWndLeftTool()
{
	s_pWndLeftTool = this;
	m_pviewThumb = NULL;
}

CWndLeftTool::~CWndLeftTool()
{
}


BEGIN_MESSAGE_MAP(CWndLeftTool, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndLeftTool 메시지 처리기입니다.



int CWndLeftTool::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
//	m_dlgLeftTool.Create( IDD_LEFTTOOL, this );
//	m_dlgLeftTool.ShowWindow( SW_SHOW );
//	m_dlgTest.Create( IDD_FORMVIEW, this );
//	m_dlgTest.ShowWindow( SW_SHOW );
//	m_pdlgTest->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1234, NULL );
	m_pdlgTest = CDlgTest::CreateOne(this);
//	m_pviewThumb = CThumbView::CreateOne( this );

	return 0;
}

void CWndLeftTool::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_pviewThumb )
		m_pviewThumb->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
	if( m_pdlgTest )
		m_pdlgTest->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}
