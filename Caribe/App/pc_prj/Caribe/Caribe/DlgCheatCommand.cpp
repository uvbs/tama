// DlgCheatCommand.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgCheatCommand.h"
#include "afxdialogex.h"


// CDlgCheatCommand ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgCheatCommand, CDialogEx)

CDlgCheatCommand::CDlgCheatCommand(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCheatCommand::IDD, pParent)
{
// 	auto pCtrl = GetDlgItem( IDC_TEXT_CMD );
// 	if( pCtrl ) {
// 		pCtrl->SetWindowText( _T( "prop_legion" ) );
// 	}
}

CDlgCheatCommand::~CDlgCheatCommand()
{
}

void CDlgCheatCommand::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text( pDX, IDC_EDIT1, m_strCmd );
	DDX_Text( pDX, IDC_TEXT_CMD, m_strDesc );
}


BEGIN_MESSAGE_MAP(CDlgCheatCommand, CDialogEx)
END_MESSAGE_MAP()


// CDlgCheatCommand �޽��� ó�����Դϴ�.
