// BotView.cpp : CBotView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Bot.h"
#endif

#include "BotDoc.h"
#include "BotView.h"
#include "XMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBotView
CBotView* CBotView::s_pInstance = nullptr;

IMPLEMENT_DYNCREATE(CBotView, CFormView)

BEGIN_MESSAGE_MAP(CBotView, CFormView)
END_MESSAGE_MAP()

// CBotView 생성/소멸

CBotView::CBotView()
	: CFormView(CBotView::IDD)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	s_pInstance = this;

}

CBotView::~CBotView()
{
}

BOOL CBotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CBotView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
// 	m_pMain = new XMain();
// 	m_pMain->Create();
}

// CBotView 그리기

void CBotView::OnDraw(CDC* /*pDC*/)
{
	CBotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	XMain::sGet()->Process();
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	if( m_timerSec.IsOff() )
		m_timerSec.Set(1.f);
	if( m_timerSec.IsOver() ) {
		CWnd* pWnd = GetDlgItem( IDC_TEXT1 );
		if( pWnd ) {
			_tstring str = XMain::sGet()->GetstrInfo( 0 );
			pWnd->SetWindowText( str.c_str() );
		}
		m_timerSec.Reset();
	}
}


// CBotView 진단

#ifdef _DEBUG
void CBotView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBotView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBotDoc* CBotView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBotDoc)));
	return (CBotDoc*)m_pDocument;
}
#endif //_DEBUG


// CBotView 메시지 처리기
