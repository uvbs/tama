// WndTimeline.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "WndTimeline.h"
#include "FrameView.h"
#include "XAniAction.h"

// CWndTimeline
static CWndTimeline *s_pWndTimeline = NULL;
CWndTimeline *GetWndTimeline() { return s_pWndTimeline; }
CWndTimeline* CWndTimeline::s_pInstance = nullptr;

IMPLEMENT_DYNAMIC(CWndTimeline, CDockablePane)

CWndTimeline::CWndTimeline()
{
	s_pWndTimeline = this;
	s_pInstance = this;
	m_pView = NULL;
}

CWndTimeline::~CWndTimeline()
{
}


BEGIN_MESSAGE_MAP(CWndTimeline, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWndTimeline 메시지 처리기입니다.
int CWndTimeline::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_dlgBar.Create( this, IDD_FRAMEVIEW_BAR, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_FRAMEVIEW_BAR );
	m_dlgBar.OnInitialUpdate();

	m_pView = XMyD3DView::CreateOne( this, new CFrameView );

	return 0;
}

void CWndTimeline::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_dlgBar.SetWindowPos( NULL, 0, 0, cx, 40, SWP_NOZORDER );
	if( m_pView )
		m_pView->SetWindowPos( NULL, 0, 40, cx, cy, SWP_NOZORDER );
}

///////////////////////////////////////////////////////////////////////////////////
XDlgBarTimeline* XDlgBarTimeline::s_pInstance = nullptr;
static XDlgBarTimeline *s_pDlgBarTimeline = NULL;
XDlgBarTimeline *GetDlgBarTimeline() { return s_pDlgBarTimeline; }

XDlgBarTimeline::XDlgBarTimeline()
{
	s_pDlgBarTimeline = this;
	s_pInstance = this;
	m_nKeyEditMode = 0;
}

BEGIN_MESSAGE_MAP(XDlgBarTimeline, CDialogBar)
	ON_CBN_SELCHANGE(IDC_COMBO1, &XDlgBarTimeline::OnCbnSelchangeCombo1)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RADIO_KEY_MOVE, &XDlgBarTimeline::OnBnClickedRadioKeyMove)
	ON_BN_CLICKED(IDC_RADIO_KEY_SCALE, &XDlgBarTimeline::OnBnClickedRadioKeyScale)
	ON_BN_CLICKED(IDC_BUTT_ADD_LAYER, &CFrameView::OnBnClickedButtAddLayer)
	ON_BN_CLICKED(IDC_BUTT_CREATEOBJ, &CFrameView::OnBnClickedButtCreateobj)
END_MESSAGE_MAP()

void XDlgBarTimeline::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_KEY_MOVE, m_nKeyEditMode);
	DDX_Control(pDX, IDC_COMBO1, m_comboPlayMode);
}

int XDlgBarTimeline::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return 0;
}

void XDlgBarTimeline::OnInitialUpdate()
{
	CDialogBar::OnInitialUpdate();
	UPDATE_D2C();		// 초기에 한번 해줘야 연결이 된다.
	UpdateSlider();
	m_comboPlayMode.AddString( _T("반복") );
	m_comboPlayMode.AddString( _T("한번만") );
	m_comboPlayMode.AddString( _T("왕복(미구현)") );
	m_comboPlayMode.SetCurSel( 0 );
	
//	UPDATE_D2C();
}

void XDlgBarTimeline::Update()
{
	UpdateSlider();
	auto spAction = SPROBJ->GetspAction();
	if( spAction )
		m_comboPlayMode.SetCurSel( (int)spAction->GetPlayMode() );
}

void XDlgBarTimeline::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	spAction->SetPlayMode( (xRPT_TYPE)m_comboPlayMode.GetCurSel() );
	SPROBJ->SetPlayType( spAction->GetPlayMode() );

}

void XDlgBarTimeline::UpdateSlider()
{
	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem( IDC_SLIDER_SPEED );
	if( pSlider )
	{
		pSlider->SetRange( 10, 100 );
		pSlider->SetPos( (int)(SPROBJ->GetSpeedCurrentAction() * 100) );
		pSlider->SetTicFreq( 5 );
		TCHAR szBuff[16];
		_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
		GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
	}
}

void XDlgBarTimeline::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.&
	if( nSBCode == SB_THUMBTRACK )
	{
		if( pScrollBar == GetDlgItem( IDC_SLIDER_SPEED ) )
		{
			CSliderCtrl *pSliderCtrl = (CSliderCtrl *)pScrollBar;
			SPROBJ->SetSpeedCurrentAction( pSliderCtrl->GetPos() / 100.0f );
			TCHAR szBuff[16];
			_stprintf_s( szBuff, _T("%1.2f"), SPROBJ->GetSpeedCurrentAction() );
			GetDlgItem( IDC_TEXT_SPEED )->SetWindowText( szBuff );
			Invalidate( FALSE );
		}
	}

	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}

void XDlgBarTimeline::OnBnClickedRadioKeyMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nKeyEditMode = KEYEDIT_MOVE;
}

void XDlgBarTimeline::OnBnClickedRadioKeyScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nKeyEditMode = KEYEDIT_SCALE;
}
