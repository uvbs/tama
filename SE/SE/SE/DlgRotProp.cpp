// DlgRotProp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgRotProp.h"


// CDlgRotProp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRotProp, CDialog)

CDlgRotProp::CDlgRotProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRotProp::IDD, pParent)
	, m_fRotZ(0)
	, m_bInterpolation(FALSE)
{

}

CDlgRotProp::~CDlgRotProp()
{
}

void CDlgRotProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_ROTATEZ, m_fRotZ );
	DDX_Check( pDX, IDC_INTERPOLATION, m_bInterpolation );
	DDX_Text( pDX, IDC_EDIT_ROTATEZ2, m_vRange.x );
	DDX_Text( pDX, IDC_EDIT_ROTATEZ3, m_vRange.y );
}


BEGIN_MESSAGE_MAP(CDlgRotProp, CDialog)
	ON_BN_CLICKED(IDC_EDIT_INTERPOLATION, &CDlgRotProp::OnBnClickedEditInterpolation)
END_MESSAGE_MAP()


// CDlgRotProp 메시지 처리기입니다.


void CDlgRotProp::OnBnClickedEditInterpolation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XDlgEditInterpolation dlg;
	dlg.m_Interpolation = m_Interpolation;
	if( dlg.DoModal() == IDOK )	{
		m_Interpolation = dlg.m_Interpolation;
		if( m_Interpolation == xSpr::xNONE )		// 보간안함
		{
			m_bInterpolation = FALSE;
			UPDATE_D2C();
		}
	}
}
