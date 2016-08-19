// DlgPosProp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgPosProp.h"
#include "XDlgEditInterpolation.h"

// CDlgPosProp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPosProp, CDialog)

CDlgPosProp::CDlgPosProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPosProp::IDD, pParent)
	, m_x(0)
	, m_y(0)
	, m_bInterpolation(FALSE)
	, m_minX( 0 )
	, m_maxX( 0 )
	, m_minY( 0 )
	, m_maxY( 0 )
	, m_radius( 0 )
{

}

CDlgPosProp::~CDlgPosProp()
{
}

void CDlgPosProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_POSX, m_x );
	DDX_Text( pDX, IDC_EDIT_POSY, m_y );
	DDX_Check( pDX, IDC_INTERPOLATION, m_bInterpolation );
	DDX_Text( pDX, IDC_EDIT_POSX2, m_minX );
	DDX_Text( pDX, IDC_EDIT_POSX3, m_maxX );
	DDX_Text( pDX, IDC_EDIT_POSY2, m_minY );
	DDX_Text( pDX, IDC_EDIT_POSY3, m_maxY );
	DDX_Text( pDX, IDC_EDIT_POSY4, m_radius );
}


BEGIN_MESSAGE_MAP(CDlgPosProp, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgPosProp::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgPosProp 메시지 처리기입니다.

void CDlgPosProp::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CDialog::OnOK();
}

// 보간함수 선택 버튼
void CDlgPosProp::OnBnClickedButton1()
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
