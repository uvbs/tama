// DlgScaleProp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgScaleProp.h"


// CDlgScaleProp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgScaleProp, CDialog)

CDlgScaleProp::CDlgScaleProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleProp::IDD, pParent)
	, m_fScaleX(1.0)
	, m_fScaleY(1.0)
// 	, m_rangeXY1(0)
// 	, m_rangeXY2(0)
// 	, m_rangeX1(0)
// 	, m_rangeX2(0)
// 	, m_rangeY1(0)
// 	, m_rangeY2(0)
	, m_bInterpolation(FALSE)
{

}

CDlgScaleProp::~CDlgScaleProp()
{
}

void CDlgScaleProp::DoDataExchange(CDataExchange* pDX)
{
// 	CONSOLE( "1(%.1f, %.1f), (%.1f, %.1f), (%.1f, %.1f) "
// 					 , m_rangeX1, m_rangeX2
// 					 , m_rangeY1, m_rangeY2
// 					 , m_rangeXY1, m_rangeXY2 );
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_SCALEX, m_fScaleX );
	DDX_Text( pDX, IDC_EDIT_SCALEY, m_fScaleY );
	DDX_Check( pDX, IDC_INTERPOLATION, m_bInterpolation );
	DDX_Text( pDX, IDC_EDIT_POSX4, m_rangeXY.v1 );
	DDX_Text( pDX, IDC_EDIT_POSX5, m_rangeXY.v2 );
	DDX_Text( pDX, IDC_EDIT_POSX2, m_rangeX.v1 );
	DDX_Text( pDX, IDC_EDIT_POSX3, m_rangeX.v2 );
	DDX_Text( pDX, IDC_EDIT_POSY2, m_rangeY.v1 );
	DDX_Text( pDX, IDC_EDIT_POSY3, m_rangeY.v2 );
//		CONSOLE( "2(%.1f, %.1f), (%.1f, %.1f), (%.1f, %.1f) "
// 					 , m_rangeX1, m_rangeX2
// 					 , m_rangeY1, m_rangeY2
// 					 , m_rangeXY1, m_rangeXY2 );
}

// DDX_Text( pDX, IDC_EDIT_POSX4, m_rangeXY1 );
// DDX_Text( pDX, IDC_EDIT_POSX5, m_rangeXY2 );
// DDX_Text( pDX, IDC_EDIT_POSX2, m_rangeX1 );
// DDX_Text( pDX, IDC_EDIT_POSX3, m_rangeX2 );
// DDX_Text( pDX, IDC_EDIT_POSY2, m_rangeY1 );
// DDX_Text( pDX, IDC_EDIT_POSY3, m_rangeY2 );
// DDX_Text( pDX, IDC_EDIT_POSX4, m_rangeXY.v1 );
// DDX_Text( pDX, IDC_EDIT_POSX5, m_rangeXY.v2 );
// DDX_Text( pDX, IDC_EDIT_POSX2, m_rangeX.v1 );
// DDX_Text( pDX, IDC_EDIT_POSX3, m_rangeX.v2 );
// DDX_Text( pDX, IDC_EDIT_POSY2, m_rangeY.v1 );
// DDX_Text( pDX, IDC_EDIT_POSY3, m_rangeY.v2 );


BEGIN_MESSAGE_MAP(CDlgScaleProp, CDialog)
	ON_BN_CLICKED(IDC_EDIT_INTERPOLATION, &CDlgScaleProp::OnBnClickedEditInterpolation)
END_MESSAGE_MAP()


// CDlgScaleProp 메시지 처리기입니다.


void CDlgScaleProp::OnBnClickedEditInterpolation()
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
