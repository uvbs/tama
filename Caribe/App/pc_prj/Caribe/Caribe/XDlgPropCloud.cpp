// XDlgPropCloud.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "XDlgPropCloud.h"
#include "afxdialogex.h"


// XDlgPropCloud ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(XDlgPropCloud, CDialogEx)

XDlgPropCloud::XDlgPropCloud(CWnd* pParent /*=nullptr*/)
	: CDialogEx(XDlgPropCloud::IDD, pParent)
	, m_idCloud( 0 )
	, m_idxImg( 0 )
	, m_xPos( 0 )
	, m_yPos( 0 )
	, m_lvOpenable( 0 )
	, m_Cost( 0 )
	, m_xPosCost( 0 )
	, m_yPosCost( 0 )
	, m_idName( 0 )
	, m_idsPrecede( _T( "" ) )
	, m_idsItem( _T( "" ) )
	, m_idsArea( _T( "" ) )
{

}

XDlgPropCloud::~XDlgPropCloud()
{
}

void XDlgPropCloud::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_ID, m_idCloud );
	DDX_Text( pDX, IDC_EDIT_IDXIMG, m_idxImg );
	DDX_Text( pDX, IDC_EDIT_POS_X, m_xPos );
	DDX_Text( pDX, IDC_EDIT_POS_Y, m_yPos );
	DDX_Text( pDX, IDC_EDIT_LV_OPEN, m_lvOpenable );
	DDX_Text( pDX, IDC_EDIT_COST, m_Cost );
	DDX_Text( pDX, IDC_EDIT_POS_X_COST, m_xPosCost );
	DDX_Text( pDX, IDC_EDIT_POS_Y_COST, m_yPosCost );
	DDX_Text( pDX, IDC_EDIT2, m_idName );
	DDX_Text( pDX, IDC_EDIT9, m_idsPrecede );
	DDX_Text( pDX, IDC_EDIT10, m_idsItem );
	DDX_Text( pDX, IDC_EDIT_IDS, m_idsArea );
}


BEGIN_MESSAGE_MAP(XDlgPropCloud, CDialogEx)
END_MESSAGE_MAP()


// XDlgPropCloud �޽��� ó�����Դϴ�.


BOOL XDlgPropCloud::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


