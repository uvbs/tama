// DlgSelectConnect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSelectConnect.h"
#include "afxdialogex.h"


// CDlgSelectConnect ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSelectConnect, CDialogEx)

CDlgSelectConnect::CDlgSelectConnect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSelectConnect::IDD, pParent)
{

}

CDlgSelectConnect::~CDlgSelectConnect()
{
}

void CDlgSelectConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSelectConnect, CDialogEx)
  ON_BN_CLICKED( IDC_BUTTON1, &CDlgSelectConnect::OnBnClickedButton1 )
  ON_BN_CLICKED( IDC_BUTTON2, &CDlgSelectConnect::OnBnClickedButton2 )
  ON_BN_CLICKED( IDC_BUTTON3, &CDlgSelectConnect::OnBnClickedButton3 )
END_MESSAGE_MAP()


// CDlgSelectConnect �޽��� ó�����Դϴ�.


void CDlgSelectConnect::OnBnClickedButton1()
{
  // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
  m_idxSelect = 0;
  CDialogEx::OnOK();
}


void CDlgSelectConnect::OnBnClickedButton2()
{
  // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
  m_idxSelect = 1;
  CDialogEx::OnOK();
}


void CDlgSelectConnect::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_idxSelect = 2;
	CDialogEx::OnOK();
}
