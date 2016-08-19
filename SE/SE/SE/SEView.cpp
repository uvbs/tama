
// SEView.cpp : CSEView 클래스의 구현
//

#include "stdafx.h"
#include "SE.h"

#include "Mainfrm.h"
#include "SEDoc.h"
#include "SEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSEView

IMPLEMENT_DYNCREATE(CSEView, CView)

BEGIN_MESSAGE_MAP(CSEView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSEView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CSEView 생성/소멸

CSEView::CSEView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CSEView::~CSEView()
{
}

BOOL CSEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.cx = 0;
	cs.cy = 0;

	return CView::PreCreateWindow(cs);
}

// CSEView 그리기

void CSEView::OnDraw(CDC* /*pDC*/)
{
	CSEDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CSEView 인쇄


void CSEView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CSEView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CSEView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CSEView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CSEView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSEView::OnContextMenu(CWnd* pWnd, CPoint point)
{
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CSEView 진단

#ifdef _DEBUG
void CSEView::AssertValid() const
{
	CView::AssertValid();
}

void CSEView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSEDoc* CSEView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSEDoc)));
	return (CSEDoc*)m_pDocument;
}
#endif //_DEBUG


// CSEView 메시지 처리기
int CSEView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT rect;
	GetClientRect( &rect );
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CSEView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	RECT rect;
	GetClientRect( &rect );
    // Register a class for a fullscreen window
//    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
//                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
//                          _T("Fullscreen Window") };
 //   RegisterClass( &wndClass );

	
//	CMainFrame *pMainFrm = g_pMainFrm; 
//	pMainFrm->Create( GetSafeHwnd() );		// d3d9 초기화
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_HIDEWINDOW );	// 이 뷰가 앞서만든 분할뷰들을 가려버리는 일이 생겨 없애버림

}

void CSEView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, 0, 0);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CSEView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

