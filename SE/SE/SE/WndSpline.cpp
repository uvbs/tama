// WndSpline.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndSpline.h"

static CWndSpline *s_pWndSpline = NULL;
CWndSpline *GetWndSpline() { return s_pWndSpline; }


// CWndSpline

IMPLEMENT_DYNAMIC(CWndSpline, CDockablePane)

CWndSpline::CWndSpline()
{
	s_pWndSpline = this;
	m_pviewSpline = NULL;
}

CWndSpline::~CWndSpline()
{
}


BEGIN_MESSAGE_MAP(CWndSpline, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndSpline 메시지 처리기입니다.



int CWndSpline::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pviewSpline = XViewSpline::CreateOne( this );

	return 0;
}

void CWndSpline::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_pviewSpline )
		m_pviewSpline->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}
