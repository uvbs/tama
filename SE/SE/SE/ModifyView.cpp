// ModifyView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "MainFrm.h"
#include "ModifyView.h"
#include "ConsoleView.h"


// CModifyView
static CModifyView *s_pModifyView = nullptr;
CModifyView *GetModifyView() { return s_pModifyView; }

IMPLEMENT_DYNCREATE(CModifyView, CFormView)

CModifyView::CModifyView()
	: CFormView(CModifyView::IDD)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fRotZ(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
{
	s_pModifyView = this;
}

CModifyView::~CModifyView()
{
}

void CModifyView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POSX, m_fPosX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_fPosY);
	DDX_Text(pDX, IDC_EDIT_ROTATEZ, m_fRotZ);
	DDX_Text(pDX, IDC_EDIT_SCALEX, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT_SCALEY, m_fScaleY);
}

BEGIN_MESSAGE_MAP(CModifyView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_POSX, &CModifyView::OnEnChangeEditPosx)
	ON_WM_KEYDOWN()
	ON_EN_SETFOCUS(IDC_EDIT_POSX, &CModifyView::OnEnSetfocusEditPosx)
	ON_EN_KILLFOCUS(IDC_EDIT_POSX, &CModifyView::OnEnKillfocusEditPosx)
END_MESSAGE_MAP()


// CModifyView 진단입니다.

#ifdef _DEBUG
void CModifyView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CModifyView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CModifyView 메시지 처리기입니다.

void CModifyView::OnEnChangeEditPosx()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UPDATE_C2D();
}

BOOL CModifyView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
			UPDATE_C2D();
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CModifyView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CModifyView::OnEnSetfocusEditPosx()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_pMainFrm->m_bEditing = TRUE;
//	GetConsoleView()->Message( _T("SetFocus") );
}

void CModifyView::OnEnKillfocusEditPosx()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_pMainFrm->m_bEditing = FALSE;
//	GetConsoleView()->Message( _T("KillFocus") );
}
