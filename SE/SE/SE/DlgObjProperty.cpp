// DlgObjProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgObjProperty.h"
#include "XDlgLuaEditor.h"
#include "SprObj.h"

// CDlgObjProperty 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgObjProperty, CDialog)

CDlgObjProperty::CDlgObjProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgObjProperty::IDD, pParent)
	, m_id(0)
{

}

CDlgObjProperty::~CDlgObjProperty()
{
}

void CDlgObjProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_id);
	DDX_Control(pDX, IDC_COMBO_PLAYTYPE, m_ctrlPlayType);
}


BEGIN_MESSAGE_MAP(CDlgObjProperty, CDialog)
	ON_BN_CLICKED(IDC_LUA_DIALOG, &CDlgObjProperty::OnBnClickedLuaDialog)
END_MESSAGE_MAP()


// CDlgObjProperty 메시지 처리기입니다.

BOOL CDlgObjProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlPlayType.AddString( _T("반복") );
	m_ctrlPlayType.AddString( _T("한번만 플레이") );
//	m_ctrlPlayType.AddString( _T("왕복") );
//	m_ctrlPlayType.AddString( _T("왕복재생을 반복") );
//	m_ctrlPlayType.AddString( _T("거꾸로") );
//	m_ctrlPlayType.AddString( _T("한번만 플레이 후 대기") );

	m_ctrlPlayType.SetCurSel( (int)m_PlayType );
	UPDATE_D2C();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgObjProperty::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UPDATE_C2D();
	m_PlayType = (xRPT_TYPE)m_ctrlPlayType.GetCurSel();
	CDialog::OnOK();
}

void CDlgObjProperty::OnBnClickedLuaDialog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XDlgLuaEditor dlg;

	dlg.m_strLua = m_strLua;
	dlg.m_strTestLua = XSprDat::s_cGlobalLua;
	if( dlg.DoModal() == IDOK )
	{
		m_strLua = dlg.m_strLua;					// 키할당 루아코드
	}
}
