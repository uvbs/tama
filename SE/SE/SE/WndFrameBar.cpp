// WndFrameBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndFrameBar.h"


// CWndFrameBar

IMPLEMENT_DYNAMIC(CWndFrameBar, CDockablePane)

//CWndFrameBar* CWndFrameBar::s_pInstance = nullptr;

CWndFrameBar::CWndFrameBar()
{
//	s_pInstance = this;
}

CWndFrameBar::~CWndFrameBar()
{
}


BEGIN_MESSAGE_MAP(CWndFrameBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndFrameBar 메시지 처리기입니다.
int CWndFrameBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_dlgBar.Create( this, IDD_FRAMEVIEW_BAR, CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_FRAMEVIEW_BAR );
//	m_dlgBar.EnableDocking(CBRS_ALIGN_ANY);	
//	m_dlgBar.MoveWindow( 0, 0, 600, 40 );
//	DockControlBar(&m_dlgBar);
//	m_dlgBar.ShowWindow( SW_SHOW );
//	ShowControlBar(&m_dlgBar, TRUE, TRUE);

	return 0;
}

void CWndFrameBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_dlgBar.SetWindowPos( NULL, 0, 0, cx, 40, SWP_NOZORDER );
}


