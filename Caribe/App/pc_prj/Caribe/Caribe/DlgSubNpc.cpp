// DlgSubNpc.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubNpc.h"
#include "afxdialogex.h"


// CDlgSubNpc ��ȭ �����Դϴ�.

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


// CDlgSubNpc �޽��� ó�����Դϴ�.


BOOL CDlgSubNpc::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	UPDATE_D2C();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
