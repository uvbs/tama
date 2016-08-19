// WndAdjustView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndAdjustView.h"

static CWndAdjustView *s_pWndAdjustView = NULL;
CWndAdjustView *GetWndAdjustView() { return s_pWndAdjustView; }

// CWndAdjustView

IMPLEMENT_DYNAMIC(CWndAdjustView, CDockablePane)

CWndAdjustView::CWndAdjustView()
{
	s_pWndAdjustView = this;
	m_pviewAdjust = NULL;
}

CWndAdjustView::~CWndAdjustView()
{
}


BEGIN_MESSAGE_MAP(CWndAdjustView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndAdjustView 메시지 처리기입니다.
int CWndAdjustView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pviewAdjust = CAdjustView::CreateOne( this );

	return 0;
}

void CWndAdjustView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_pviewAdjust )
		m_pviewAdjust->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}
