// DlgBattleOption.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgBattleOption.h"
#include "afxdialogex.h"
#include "client/XAppMain.h"


// CDlgBattleOption ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgBattleOption, CDialogEx)

CDlgBattleOption::CDlgBattleOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBattleOption::IDD, pParent)
{

}

CDlgBattleOption::~CDlgBattleOption()
{
}

void CDlgBattleOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check( pDX, IDC_CHECK_FACE, XAPP->m_bShowFace );
	DDX_Check( pDX, IDC_CHECK_WAIT_AFTER_WIN, XAPP->m_bWaitAfterWin );
}


BEGIN_MESSAGE_MAP(CDlgBattleOption, CDialogEx)
END_MESSAGE_MAP()


// CDlgBattleOption �޽��� ó�����Դϴ�.
