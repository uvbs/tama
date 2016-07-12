// DlgCheatCommand.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgCheatCommand.h"
#include "afxdialogex.h"


// CDlgCheatCommand 대화 상자입니다.

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


// CDlgCheatCommand 메시지 처리기입니다.
