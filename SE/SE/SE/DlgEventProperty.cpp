// DlgEventProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgEventProperty.h"
#include "XDlgLuaEditor.h"

// CDlgEventProperty 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEventProperty, CDialog)

CDlgEventProperty::CDlgEventProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEventProperty::IDD, pParent)
	, m_idObj(0)
	, m_strSpr(_T(""))
	, m_idAct(0)
	, m_secLifeTime(0)
	, m_bTraceParent(FALSE)
	, m_Scale( 0 )
{
	m_EventType = xSpr::xEKT_NONE;
	m_LoopType = 1;
}

CDlgEventProperty::~CDlgEventProperty()
{
}

void CDlgEventProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_idObj );
	DDX_Control( pDX, IDC_COMBO_EVENTTYPE, m_ctrlEventType );
	DDX_Text( pDX, IDC_EDIT_SPR, m_strSpr );
	DDX_Text( pDX, IDC_EDIT_ACTID, m_idAct );
	DDX_Control( pDX, IDC_COMBO_LOOP, m_ctrlLoopType );
	DDX_Text( pDX, IDC_EDIT_LIFE_TIME, m_secLifeTime );
	DDV_MinMaxFloat( pDX, m_secLifeTime, 0, 9999 );
	DDX_Check( pDX, IDC_CHECK_PARENT, m_bTraceParent );
	DDX_Text( pDX, IDC_EDIT_SCALE, m_Scale );
	DDX_Text( pDX, IDC_EDIT_STR, m_strParam[0] );
	DDX_Text( pDX, IDC_EDIT_STR2, m_strParam[1] );
	DDX_Text( pDX, IDC_EDIT_STR3, m_strParam[2] );
	DDX_Text( pDX, IDC_EDIT_STR4, m_strParam[3] );
}


BEGIN_MESSAGE_MAP(CDlgEventProperty, CDialog)
	ON_BN_CLICKED(IDC_LUA_DIALOG, &CDlgEventProperty::OnBnClickedLuaDialog)
END_MESSAGE_MAP()


// CDlgEventProperty 메시지 처리기입니다.

BOOL CDlgEventProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlEventType.AddString( _T("오브젝트 생성") );
	m_ctrlEventType.AddString( _T("타격") );
	m_ctrlEventType.AddString( _T("이펙트 생성") );
	m_ctrlEventType.AddString( _T("기타") );
	m_ctrlEventType.SetCurSel( (int)m_EventType - 1 );		// xSpr::xNONE이 있기때문에 -1했음

	m_ctrlLoopType.AddString( _T("한번만") );
	m_ctrlLoopType.AddString( _T("반복") );
	m_ctrlLoopType.AddString( _T("무한반복") );
	m_ctrlLoopType.SetCurSel( (int)m_LoopType - 1 );		// 1~
	
	//	UPDATE_D2C();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgEventProperty::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UPDATE_C2D();
	m_EventType = (xSpr::xtEventKey)(m_ctrlEventType.GetCurSel() + 1);
	m_LoopType = m_ctrlLoopType.GetCurSel() + 1;
	CDialog::OnOK();
}

void CDlgEventProperty::OnBnClickedLuaDialog()
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
