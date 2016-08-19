// WndThumbView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndThumbView.h"

static CWndThumbView *s_pWndThumbView = NULL;
CWndThumbView *GetWndThumbView() { return s_pWndThumbView; }

// CWndThumbView

IMPLEMENT_DYNAMIC(CWndThumbView, CDockablePane)

CWndThumbView::CWndThumbView()
{
	s_pWndThumbView = this;
	m_pviewThumb = NULL;
}

CWndThumbView::~CWndThumbView()
{
}


BEGIN_MESSAGE_MAP(CWndThumbView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndThumbView 메시지 처리기입니다.

int CWndThumbView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pviewThumb = CThumbView::CreateOne( this );

	return 0;
}

void CWndThumbView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_pviewThumb )
		m_pviewThumb->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}
