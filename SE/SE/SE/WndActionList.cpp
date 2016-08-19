// WndActionList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndActionList.h"


// CWndActionList

IMPLEMENT_DYNAMIC(CWndActionList, CDockablePane)

CWndActionList::CWndActionList()
{
	m_pView = NULL;
}

CWndActionList::~CWndActionList()
{
}


BEGIN_MESSAGE_MAP(CWndActionList, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndActionList 메시지 처리기입니다.
int CWndActionList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pView = CActionListView::CreateOne( this );

	return 0;
}

void CWndActionList::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_pView )
		m_pView->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}


