// DlgGmMsg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "resource.h"
#include "DlgGmMsg.h"
#include "afxdialogex.h"


// DlgGmMsg ��ȭ �����Դϴ�.

CString DlgGmMsg::s_strMsg;		// ���� �޽���

IMPLEMENT_DYNAMIC(DlgGmMsg, CDialogEx)

DlgGmMsg::DlgGmMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgGmMsg::IDD, pParent)
	, m_strMsg( _T( "" ) )
{
	CString str = _T("test");
	CONSOLE("%s", str );
}

DlgGmMsg::~DlgGmMsg()
{
}

void DlgGmMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strMsg );
	DDV_MaxChars(pDX, m_strMsg, 255);
}


BEGIN_MESSAGE_MAP(DlgGmMsg, CDialogEx)
END_MESSAGE_MAP()


// DlgGmMsg �޽��� ó�����Դϴ�.


void DlgGmMsg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	s_strMsg = m_strMsg;

	CDialogEx::OnOK();
}
