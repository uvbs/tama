// BotView.cpp : CBotView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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

// CBotView ����/�Ҹ�

CBotView::CBotView()
	: CFormView(CBotView::IDD)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	s_pInstance = this;

}

CBotView::~CBotView()
{
}

BOOL CBotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CBotView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
// 	m_pMain = new XMain();
// 	m_pMain->Create();
}

// CBotView �׸���

void CBotView::OnDraw(CDC* /*pDC*/)
{
	CBotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	XMain::sGet()->Process();
	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
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


// CBotView ����

#ifdef _DEBUG
void CBotView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBotView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBotDoc* CBotView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBotDoc)));
	return (CBotDoc*)m_pDocument;
}
#endif //_DEBUG


// CBotView �޽��� ó����
