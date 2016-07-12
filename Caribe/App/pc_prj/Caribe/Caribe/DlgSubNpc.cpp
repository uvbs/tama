// DlgSubNpc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgSubNpc.h"
#include "afxdialogex.h"


// CDlgSubNpc 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSubNpc, CDialogEx)

CDlgSubNpc::CDlgSubNpc(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubNpc::IDD, pParent)
: m_secRegen( 0 )
, m_strClan( _T( "" ) )
//, m_Elite( 0 )
{
}

CDlgSubNpc::~CDlgSubNpc()
{
}

void CDlgSubNpc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_SEC_REGEN, m_secRegen );
	DDX_Text( pDX, IDC_SEC_REGEN2, m_strClan );
//	DDX_Text( pDX, IDC_ELITE, m_Elite );
}


BEGIN_MESSAGE_MAP(CDlgSubNpc, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubNpc 메시지 처리기입니다.


BOOL CDlgSubNpc::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	UPDATE_D2C();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
