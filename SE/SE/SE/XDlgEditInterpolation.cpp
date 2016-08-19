// XDlgEditInterpolation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "XDlgEditInterpolation.h"
#include "MainFrm.h"
#include "XViewGraph.h"
// XDlgEditInterpolation 대화 상자입니다.

IMPLEMENT_DYNAMIC(XDlgEditInterpolation, CDialog)

XDlgEditInterpolation::XDlgEditInterpolation(CWnd* pParent /*=nullptr*/)
	: CDialog(XDlgEditInterpolation::IDD, pParent)
	, m_Interpolation(xSpr::xLINEAR)
	, m_nSelectFunc(0)
{

}

XDlgEditInterpolation::~XDlgEditInterpolation()
{
}

void XDlgEditInterpolation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_BOGAN_NONE, m_nSelectFunc);
}


BEGIN_MESSAGE_MAP(XDlgEditInterpolation, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BOGAN_NONE, &XDlgEditInterpolation::OnBnClickedBoganNone)
	ON_BN_CLICKED(IDC_BOGAN_LINEAR, &XDlgEditInterpolation::OnBnClickedBoganLinear)
	ON_BN_CLICKED(IDC_BOGAN_ACCEL, &XDlgEditInterpolation::OnBnClickedBoganAccel)
	ON_BN_CLICKED(IDC_BOGAN_DEACCEL, &XDlgEditInterpolation::OnBnClickedBoganDeaccel)
	ON_BN_CLICKED(IDC_BOGAN_SMOOTH, &XDlgEditInterpolation::OnBnClickedBoganSmooth)
	ON_BN_CLICKED(IDC_BOGAN_SPLINE, &XDlgEditInterpolation::OnBnClickedBoganSpline)
END_MESSAGE_MAP()


// XDlgEditInterpolation 메시지 처리기입니다.

void XDlgEditInterpolation::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CDialog::OnOK();
	m_Interpolation = (xSpr::xtInterpolation)m_nSelectFunc;
}
#include "TestView.h"
BOOL XDlgEditInterpolation::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_nSelectFunc = (int)m_Interpolation;
	UPDATE_D2C();
/*	CCreateContext context;
	context.m_pNewViewClass = RUNTIME_CLASS( CAdjustView );
	CAdjustView *pView = (CAdjustView *)((CFrameWnd *)this)->CreateView( &context );
	pView->ShowWindow( SW_NORMAL );
	pView->MoveWindow( 50, 150, 200, 200 );  */
	CRect rect;
	CWnd *pWnd = GetDlgItem( IDC_GRAPH );
	pWnd->GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.left += 5;
	rect.right -= 5;
	rect.top += 15;
	rect.bottom -= 5;
	CRuntimeClass  *pObject;
	pObject = RUNTIME_CLASS( XViewGraph );
	XViewGraph* pView = (XViewGraph*) pObject->CreateObject();

	if (!pView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,
		rect, this, AFX_IDW_PANE_FIRST, nullptr))
	{
	   TRACE0("Failed to create view window\n");
	   return -1;
	} 
	pView->SetpDlg( this );
//	pView->MoveWindow( 20, 150, 200, 200 );  
//	pView->SetParent( this );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void XDlgEditInterpolation::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}

void XDlgEditInterpolation::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void XDlgEditInterpolation::OnBnClickedBoganNone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}

void XDlgEditInterpolation::OnBnClickedBoganLinear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}

void XDlgEditInterpolation::OnBnClickedBoganAccel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}

void XDlgEditInterpolation::OnBnClickedBoganDeaccel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}

void XDlgEditInterpolation::OnBnClickedBoganSmooth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}

void XDlgEditInterpolation::OnBnClickedBoganSpline()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UPDATE_C2D();
	Invalidate(0);
}
