// DlgDrawProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgDrawProperty.h"


// CDlgDrawProperty 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDrawProperty, CDialog)

CDlgDrawProperty::CDlgDrawProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDrawProperty::IDD, pParent)
	, m_bFlipHoriz(FALSE)
	, m_bFlipVert(FALSE)
	, m_DrawMode(xDM_NORMAL)
	, m_fOpacity(1.0f)
	, m_nOpacitySlider(0)
	, m_bInterpolation(TRUE)
{

}

CDlgDrawProperty::~CDlgDrawProperty()
{
}  

void CDlgDrawProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Check( pDX, IDC_FLIP_HORIZ, m_bFlipHoriz );
	DDX_Check( pDX, IDC_FLIP_VERT, m_bFlipVert );
	DDX_Control( pDX, IDC_COMBO_DRAWMODE, m_ctrlDrawMode );
	DDX_Slider( pDX, IDC_SLIDER_OPACITY, m_nOpacitySlider );
	DDX_Check( pDX, IDC_INTERPOLATION, m_bInterpolation );
	DDX_Text( pDX, IDC_EDIT_ROTATEZ2, m_vRangeAlpha.v1 );
	DDX_Text( pDX, IDC_EDIT_ROTATEZ3, m_vRangeAlpha.v2 );
}


BEGIN_MESSAGE_MAP(CDlgDrawProperty, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_EDIT_INTERPOLATION, &CDlgDrawProperty::OnBnClickedEditInterpolation)
END_MESSAGE_MAP()


// CDlgDrawProperty 메시지 처리기입니다.

BOOL CDlgDrawProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
//	m_ctrlDrawMode.ResetContent();
	m_ctrlDrawMode.AddString( _T("그리지 않음") );
	m_ctrlDrawMode.AddString( _T("normal(multiply)") );
	m_ctrlDrawMode.AddString( _T("add(linear dodge)") );
	m_ctrlDrawMode.AddString( _T("multiply") );
	m_ctrlDrawMode.AddString( _T("subtract") );
	m_ctrlDrawMode.AddString( _T("lighten") );
	m_ctrlDrawMode.AddString( _T("darken") );
	m_ctrlDrawMode.AddString( _T("screen") );
	m_ctrlDrawMode.AddString( _T( "gray" ) );

	m_ctrlDrawMode.SetCurSel( (int)m_DrawMode );
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem( IDC_SLIDER_OPACITY );
	pSlider->SetRange( 0, 100 );
	m_nOpacitySlider = (int)(m_fOpacity * 100.0f);
	pSlider->SetPos( m_nOpacitySlider );
	pSlider->SetTicFreq( 5 );
	pSlider->SetPageSize( 10 );
	pSlider->SetLineSize( 5 );
	CString str;
	str.Format( _T("%3d%%"), m_nOpacitySlider );
	GetDlgItem( IDC_TEXT_OPACITY )->SetWindowText( str );
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDrawProperty::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData( TRUE );		// 컨트롤에서 변수로
	m_DrawMode = (xDM_TYPE)m_ctrlDrawMode.GetCurSel();
	m_fOpacity = m_nOpacitySlider / 100.0f;
	CDialog::OnOK();
}

void CDlgDrawProperty::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
// 	if( nSBCode == SB_THUMBTRACK )	{
		if( pScrollBar == GetDlgItem( IDC_SLIDER_OPACITY ) )	{
			UPDATE_C2D();
			CString str;
			str.Format( _T("%3d%%"), m_nOpacitySlider );
			GetDlgItem( IDC_TEXT_OPACITY )->SetWindowText( str );
			Invalidate( FALSE );
		}
// 	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgDrawProperty::OnBnClickedEditInterpolation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XDlgEditInterpolation dlg;
	dlg.m_Interpolation = m_Interpolation;
	if( dlg.DoModal() == IDOK )
	{
		m_Interpolation = dlg.m_Interpolation;
		if( m_Interpolation == xSpr::xNONE )		// 보간안함
		{
			m_bInterpolation = FALSE;
			UPDATE_D2C();
		}
	}
}
