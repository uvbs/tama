// DlgNewBgObj.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgNewBgObj.h"
#include "afxdialogex.h"


// CDlgNewBgObj ��ȭ �����Դϴ�.

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

// CDlgNewBgObj �޽��� ó�����Դϴ�.
BOOL CDlgNewBgObj::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_strSpr = m_Prop.m_strSpr.c_str();
	m_strSndFile = m_Prop.m_strSnd.c_str();
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	for( int i = 1; i < XGAME::xBOT_MAX; ++i ) {
		_tstring strEnum = C2SZ( xnBgObj::GetstrEnum( (XGAME::xtBgObj)i ) );
		m_comboType.AddString( strEnum.c_str() );
	}
// 	m_comboType.AddString( _T("�� ��¦��") );
// 	m_comboType.AddString( _T("����") );
// 	m_comboType.AddString( _T("������") );
// 	m_comboType.AddString( _T("��ϲ���") );
// 	m_comboType.AddString( _T("�������") );
// 	m_comboType.AddString( _T( "�������" ) );
// 	m_comboType.AddString( _T( "�������" ) );
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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