
// [!output PROJECT_NAME]View.cpp : C[!output PROJECT_NAME]View Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "[!output PROJECT_NAME].h"
#endif

#include "[!output PROJECT_NAME]Doc.h"
#include "[!output PROJECT_NAME]View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C[!output PROJECT_NAME]View

IMPLEMENT_DYNCREATE(C[!output PROJECT_NAME]View, CFormView)

BEGIN_MESSAGE_MAP(C[!output PROJECT_NAME]View, CFormView)
END_MESSAGE_MAP()

// C[!output PROJECT_NAME]View ����/�Ҹ�

C[!output PROJECT_NAME]View::C[!output PROJECT_NAME]View()
	: CFormView(C[!output PROJECT_NAME]View::IDD)
	, XEServerView( this )
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

C[!output PROJECT_NAME]View::~C[!output PROJECT_NAME]View()
{
}

void C[!output PROJECT_NAME]View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL C[!output PROJECT_NAME]View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void C[!output PROJECT_NAME]View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// ���� ����. ���� ��������Ʈ ����
	m_delegateMain.Create();

}


// C[!output PROJECT_NAME]View ����

#ifdef _DEBUG
void C[!output PROJECT_NAME]View::AssertValid() const
{
	CFormView::AssertValid();
}

void C[!output PROJECT_NAME]View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

C[!output PROJECT_NAME]Doc* C[!output PROJECT_NAME]View::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C[!output PROJECT_NAME]Doc)));
	return (C[!output PROJECT_NAME]Doc*)m_pDocument;
}
#endif //_DEBUG


// C[!output PROJECT_NAME]View �޽��� ó����
void C[!output PROJECT_NAME]View::OnDraw(CDC* pDC)
{
	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
/*
	CWnd *pWnd = GetDlgItem( IDC_ERR_TEXT );
	if( pWnd && MAIN )
	{
		if( MAIN->GetstrPublicKey().empty() )
			pWnd->SetWindowText( _T("���� ���� ����Ű�� �������� �ʾҽ��ϴ�.") );
		else
			if( MAIN->GetstrPublicKey().length() < 300 )
			{
				LPCTSTR szText = XE::Format( _T("����Ű�� ���˿� ������ �ֽ��ϴ�.\nkey=%s"), 
					C2SZ(MAIN->GetstrPublicKey().c_str()) );
				pWnd->SetWindowText( szText );
			}
	}
*/
}