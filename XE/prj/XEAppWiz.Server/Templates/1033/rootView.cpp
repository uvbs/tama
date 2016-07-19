
// [!output PROJECT_NAME]View.cpp : C[!output PROJECT_NAME]View 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// C[!output PROJECT_NAME]View 생성/소멸

C[!output PROJECT_NAME]View::C[!output PROJECT_NAME]View()
	: CFormView(C[!output PROJECT_NAME]View::IDD)
	, XEServerView( this )
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void C[!output PROJECT_NAME]View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// 최초 시작. 메인 델리게이트 생성
	m_delegateMain.Create();

}


// C[!output PROJECT_NAME]View 진단

#ifdef _DEBUG
void C[!output PROJECT_NAME]View::AssertValid() const
{
	CFormView::AssertValid();
}

void C[!output PROJECT_NAME]View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

C[!output PROJECT_NAME]Doc* C[!output PROJECT_NAME]View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C[!output PROJECT_NAME]Doc)));
	return (C[!output PROJECT_NAME]Doc*)m_pDocument;
}
#endif //_DEBUG


// C[!output PROJECT_NAME]View 메시지 처리기
void C[!output PROJECT_NAME]View::OnDraw(CDC* pDC)
{
	XEServerView::OnDraw( IDC_TEXT_NUM_CONNECT ) ;
/*
	CWnd *pWnd = GetDlgItem( IDC_ERR_TEXT );
	if( pWnd && MAIN )
	{
		if( MAIN->GetstrPublicKey().empty() )
			pWnd->SetWindowText( _T("구글 마켓 공개키가 설정되지 않았습니다.") );
		else
			if( MAIN->GetstrPublicKey().length() < 300 )
			{
				LPCTSTR szText = XE::Format( _T("공개키의 포맷에 문제가 있습니다.\nkey=%s"), 
					C2SZ(MAIN->GetstrPublicKey().c_str()) );
				pWnd->SetWindowText( szText );
			}
	}
*/
}