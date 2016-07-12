// DlgNewBgObj.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgNewBgObj.h"
#include "afxdialogex.h"


// CDlgNewBgObj 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNewBgObj, CDialogEx)

CDlgNewBgObj::CDlgNewBgObj(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNewBgObj::IDD, pParent)
// 	, m_posX( 0 )
// 	, m_posY( 0 )
// 	, m_type( 0 )
{

}

CDlgNewBgObj::~CDlgNewBgObj()
{
}

void CDlgNewBgObj::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text( pDX, IDC_EDIT1, m_Prop.m_idObj );
	DDX_Text( pDX, IDC_EDIT6, m_Prop.m_vwPos.x );
	DDX_Text( pDX, IDC_EDIT7, m_Prop.m_vwPos.y );
	DDX_Text( pDX, IDC_EDIT5, m_Prop.m_idAct );
	DDX_Text( pDX, IDC_EDIT2, m_strSpr );
	DDX_Text( pDX, IDC_EDIT8, m_strSndFile );
	DDX_Control( pDX, IDC_COMBO1, m_comboType );
}


BEGIN_MESSAGE_MAP(CDlgNewBgObj, CDialogEx)
END_MESSAGE_MAP()

// CDlgNewBgObj 메시지 처리기입니다.
BOOL CDlgNewBgObj::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_strSpr = m_Prop.m_strSpr.c_str();
	m_strSndFile = m_Prop.m_strSnd.c_str();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	for( int i = 1; i < XGAME::xBOT_MAX; ++i ) {
		_tstring strEnum = C2SZ( xnBgObj::GetstrEnum( (XGAME::xtBgObj)i ) );
		m_comboType.AddString( strEnum.c_str() );
	}
// 	m_comboType.AddString( _T("물 반짝이") );
// 	m_comboType.AddString( _T("폭포") );
// 	m_comboType.AddString( _T("수증기") );
// 	m_comboType.AddString( _T("용암끓기") );
// 	m_comboType.AddString( _T("용암증기") );
// 	m_comboType.AddString( _T( "용암증기" ) );
// 	m_comboType.AddString( _T( "용암증기" ) );
	XBREAK( !m_Prop.m_Type );
	m_comboType.SetCurSel( ((int)m_Prop.m_Type)-1 );
	UPDATE_D2C();
// 	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem( IDC_SLIDER_OPACITY );
// 	pSlider->SetRange( 0, 100 );
// 	m_nOpacitySlider = (int)(m_fOpacity * 100.0f);
// 	pSlider->SetPos( m_nOpacitySlider );
// 	pSlider->SetTicFreq( 1 );
// 	CString str;
// 	str.Format( _T("%3d%%"), m_nOpacitySlider );
// 	GetDlgItem( IDC_TEXT_OPACITY )->SetWindowText( str );
// 	
// 	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgNewBgObj::OnOK()
{
	UPDATE_C2D();
	m_Prop.m_Type = (XGAME::xtBgObj)(m_comboType.GetCurSel() + 1);
	m_Prop.m_strType = xnBgObj::GetstrEnum( m_Prop.m_Type );
	m_Prop.m_strSpr = m_strSpr;
	if( !m_strSndFile.IsEmpty() )
		m_Prop.m_strSnd = XE::GetFileTitle( (LPCTSTR)m_strSndFile );
	CDialogEx::OnOK();
}