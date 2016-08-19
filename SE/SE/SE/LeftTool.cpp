// LeftTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "LeftTool.h"
#include "Tool.h"
#include "FrameView.h"

// CLeftTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLeftTool, CDialog)

CLeftTool::CLeftTool(CWnd* pParent /*=NULL*/)
	: CDialog(CLeftTool::IDD, pParent)
{

}

CLeftTool::~CLeftTool()
{
}

void CLeftTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLeftTool, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CLeftTool 메시지 처리기입니다.
void CLeftTool::UpdateSlider()
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem( IDC_SLIDER_SPEED );
	pSlider->SetRange( 10, 100 );
	pSlider->SetPos( (int)(SPROBJ->GetSpeedCurrentAction() * 100) );
	pSlider->SetTicFreq( 5 );
	TCHAR szBuff[16];
	_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
	GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
}

void CLeftTool::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nSBCode == SB_THUMBTRACK )
	{
		if( pScrollBar == GetDlgItem( IDC_SLIDER_SPEED ) )
		{
			CSliderCtrl *pSliderCtrl = (CSliderCtrl *)pScrollBar;
			SPROBJ->SetSpeedCurrentAction( pSliderCtrl->GetPos() / 100.0f );
			TCHAR szBuff[16];
			_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
			GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
			GetFrameView()->Update();
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLeftTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	UpdateSlider();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
